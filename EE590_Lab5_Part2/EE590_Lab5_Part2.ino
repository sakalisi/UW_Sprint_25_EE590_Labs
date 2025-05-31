/**
 * @file EE590_Lab5_Part2.ino
 * @brief FreeRTOS-based Light Monitoring and Anomaly Detection System using ESP32
 *
 * @mainpage EE590 Lab5 Part 2
 *
 * @section overview Overview
 * This sketch reads light intensity using a photoresistor, smooths the readings using a
 * sliding window average, detects anomalies, displays real-time values on an I2C LCD,
 * and concurrently calculates prime numbers using FreeRTOS tasks pinned to ESP32 cores.
 *
 * @section author Author
 * Created by Sai Jayanth Kalisi, 2025
 *
 */

//========= LIBRARIES =========
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//========= PIN DEFINITIONS =========
#define LED 1       ///< Output LED pin for anomaly alert
#define LEDR 18     ///< Input photoresistor pin meant to read analog inputs
#define SDA_PIN 20  ///< I2C Data Pin
#define SCL_PIN 21  ///< I2C Clock Pin

//========= LCD SETUP =========
/**
 * @brief 16x2 I2C LCD at address 0x27
 */
LiquidCrystal_I2C lcd(0x27, 16, 2);

//========= TASK HANDLES =========
TaskHandle_t TaskLEDR_Handle = NULL;
TaskHandle_t TaskLCD_Handle = NULL;
TaskHandle_t TaskANOMALY_Handle = NULL;
TaskHandle_t TaskPRIME_Handle = NULL;

//========= GLOBAL VARIABLES =========
static SemaphoreHandle_t semaphore;  ///< Semaphore for data synchronization

int newLightLevel = 0;       ///< Most recent light level that was read by LEDR
int sma = 0;                 ///< current rolling SMA value
const int WINDOW_SIZE = 5;   ///< Window size over which to calculate sliding mean
int window[WINDOW_SIZE];     ///< Window of Window Size over which to calculate sliding mean
int currIndex = 0;           ///< Current Index of Window to replace older value
int currSum = 0;             ///< Current rolling sum
bool updatedNewVal = false;  ///< whether or not a new value has been added to the window

//========= SETUP =========
/**
 * @brief Arduino setup function
 * @details 1. Initialize pins, serial, LCD, etc
 *          2. Create binary semaphore for synchronizing light level data.
 *          3. Create Tasks
 *          - Create the `Light Detector Task` and assign it to Core 0.
 *          - Create `LCD Task` and assign it to Core 0.
 *          - Create `Anomaly Alarm Task` and assign it to Core 1.
 *          - Create `Prime Calculation Task` and assign it to Core 1.
 *          4. Scheduler attempted before realizing that FreeRTOS automatically establishes a round robin system
 * Initializes peripherals, LCD, semaphore, and starts FreeRTOS tasks
 */
void setup() {
  Serial.begin(115200);
  pinMode(LEDR, INPUT);
  pinMode(LED, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);

  semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(semaphore);

  xTaskCreatePinnedToCore(LightDetectorTask, "LEDRread", 4096, NULL, 1, &TaskLEDR_Handle, 0);
  xTaskCreatePinnedToCore(LCDTask, "UpdateLCD", 4096, NULL, 1, &TaskLCD_Handle, 0);
  xTaskCreatePinnedToCore(AnomalyAlarmTask, "DetectAnomaly", 4096, NULL, 1, &TaskANOMALY_Handle, 1);
  xTaskCreatePinnedToCore(PrimeCalculationTask, "FindPrime", 4096, NULL, 1, &TaskPRIME_Handle, 1);
  // xTaskCreatePinnedToCore(schedulerTask, "scheduleAll", 4096, NULL, 2, &TaskPRIME_Handle, 1); // Commented out scheduler
}


//========= LOOP =========
/**
 * @brief Empty main loop since tasks handle all operations
 */
void loop() {}

// void schedulerTask(void *arg){
//   while (1) {
//     void *ptr = NULL;
//     LightDetectorTask(ptr);
//     LCDTask(ptr);
//     AnomalyAlarmTask(ptr);
//     PrimeCalculationTask(ptr);
//     vTaskDelay(pdMS_TO_TICKS(1));
//   }
// }

//========= TASKS =========

/**
 * @brief Reads light sensor data, updates sliding window average (SMA), and sets flags.
 * @details This is meant to run on Core 0
 *           1. Initialize Variables -> initialized as globals already
 *           2. Loop Continuously
 *            - Read light level from the photoresistor.
 *            - Take semaphore
 *            - Calculate the simple moving average and update variables.
 *            - Give semaphore to signal data is ready.
 *            - Delay
 * @param arg Unused task parameter
 */
void LightDetectorTask(void *arg) {
  while (1) {
    int newRead = analogRead(LEDR);

    xSemaphoreTake(semaphore, portMAX_DELAY);
    currSum -= window[currIndex];
    window[currIndex] = newRead;
    currSum += newRead;
    currIndex = (currIndex + 1) % WINDOW_SIZE;

    newLightLevel = newRead;
    sma = currSum / WINDOW_SIZE;  //This means that the first few are most likely anomalies
    updatedNewVal = true;

    xSemaphoreGive(semaphore);
    vTaskDelay(pdMS_TO_TICKS(500));  //creating a 0.5 second delay between each new read;
  }
}

/**
 * @brief Displays light sensor value and moving average (SMA) on the LCD
 * @details This is meant to run on Core 0
 *          1. Initialize Variables
 *          2. Loop Continuously
 *            - Wait for semaphore.
 *            - If data has changed, update the LCD with the new light level and SMA. -> Why should it matter if its different?
 *            - Give back the semaphore.
 *            - Delay
 * @param arg Unused task parameter
 */
void LCDTask(void *arg) {
  int prevVal = -1;
  int prevSMA = -1;
  while (1) {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    if (updatedNewVal && prevVal != newLightLevel && prevSMA != sma) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LEDR READ: ");
      lcd.print(newLightLevel);
      lcd.setCursor(0, 1);
      lcd.print("SMA: ");
      lcd.print(sma);

      prevVal = newLightLevel;
      prevSMA = sma;
      updatedNewVal = false;
    }
    xSemaphoreGive(semaphore);
    vTaskDelay(pdMS_TO_TICKS(250));
  }
}

/**
 * @brief Monitors SMA value and triggers LED alert if value exceeds threshold
 * @details This is meant to run on Core 1
 *          1. Loop Continuously
 *             - Wait for semaphore.
 *             - Get SMA value
 *             - Give back the semaphore.
 *             - Check if SMA indicates a light anomaly (outside thresholds).
 *             - If anomaly detected, flash a LED signal.
 *             - Delay
 * @param arg Unused task parameter
 */
void AnomalyAlarmTask(void *arg) {
  while (1) {
    //Should be better, since I am not using sma elsewhere in task
    xSemaphoreTake(semaphore, portMAX_DELAY);
    int currentSMA = sma;
    xSemaphoreGive(semaphore);

    if (currentSMA > 3800 || currentSMA < 300) {
      for (int i = 0; i < 4; i++) {
        digitalWrite(LED, HIGH);
        vTaskDelay(pdMS_TO_TICKS(200));
        digitalWrite(LED, LOW);
        vTaskDelay(pdMS_TO_TICKS(200));
      }
      vTaskDelay(1000);
    } else {
      vTaskDelay(pdMS_TO_TICKS(200));
    }
  }
}

/**
 * @brief Checks whether a number is prime
 * @details This is a helper functionf for Prime Calculation Task
 * @param n Integer to check
 * @return true if prime, false otherwise
 */
bool isPrime(int n) {
  if (n <= 1) {
    return false;
  }
  for (int i = 2; i * i < n; i++) {
    if (n % i == 0) return false;
  }
  return true;
}

/**
 * @brief Calculates prime numbers from 0 to 5000 in background
 * @details This is meant to run on Core 1
 *            1. Loop from 2 to 5000
 *             - Check if the current number is prime.
 *             - If prime, print the number to the serial monitor.
 * @param arg Unused task parameter
 */

void PrimeCalculationTask(void *arg) {
  for (int i = 0; i < 5000; i++) {
    if (isPrime(i)) {
      Serial.print("Prime found: ");
      Serial.println(i);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  vTaskSuspend(NULL);
}
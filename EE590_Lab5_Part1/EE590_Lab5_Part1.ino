/**
 * @file EE590_Lab5_Part1.ino
 * @brief FreeRTOS-based SRTF Task Scheduling System on ESP32
 *
 * @mainpage EE590 Lab5 - Part 1
 *
 * @section overview Overview
 * This sketch demonstrates Shortest Remaining Time First (SRTF) scheduling using FreeRTOS tasks on an ESP32.
 * The program performs three tasks:
 * - Blinks an LED for a total of 500 ms.
 * - Displays an incrementing counter on an LCD. in 2 seconds
 * - Prints alphabet characters to the serial monitor in 13 seconds.
 * Tasks are managed by a scheduler that resumes them based on the shortest remaining execution time.
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
#define LED 1              ///< Output LED pin
#define LED_TIME 100       ///< LED blink duration in milliseconds
#define SDA_PIN 20         ///< I2C data pin
#define SCL_PIN 21         ///< I2C clock pin
#define LCD_TIME 100       ///< LCD update interval in milliseconds
#define PRINT_TIME 500     ///< Serial print interval in milliseconds


//========= LCD SETUP =========
/**
 * @brief 16x2 I2C LCD at address 0x27
 */
LiquidCrystal_I2C lcd(0x27, 16, 2);

//========= TASK EXECUTION TIMES =========
const TickType_t ledTaskExecutionTime = pdMS_TO_TICKS(500);         ///< LED total execution time 500 ms
const TickType_t counterTaskExecutionTime = pdMS_TO_TICKS(2000);    ///< LCD count total time 2 s
const TickType_t alphabetTaskExecutionTime = pdMS_TO_TICKS(13000);  ///< Alphabet print total time 13 s

//========= REMAINING TIMES =========
volatile TickType_t remainingLedTime = ledTaskExecutionTime;
volatile TickType_t remainingCounterTime = counterTaskExecutionTime;
volatile TickType_t remainingAlphabetTime = alphabetTaskExecutionTime;

//========= STATE FLAGS =========
volatile bool LEDdone = false;     ///< Flag to track completion of LED task
volatile bool COUNTdone = false;   ///< Flag to track completion of counter task
volatile bool ALPHAdone = false;   ///< Flag to track completion of alphabet task

//========= Global VALUES =========
volatile int count = 0;            ///< Current LCD count value
volatile char glyph = 'A' - 1;     ///< Current alphabet character

//========= TASK HANDLES =========
TaskHandle_t TaskLED_Handle = NULL;
TaskHandle_t TaskCOUNT_Handle = NULL;
TaskHandle_t TaskALPHA_Handle = NULL;
TaskHandle_t TaskSchedule_Handle = NULL;


//========= TASK DEFINITIONS =========

/**
 * @brief Blinks an LED until the allotted execution time expires
 * @param arg Unused task parameter
 */
void ledTask(void *arg) {
  pinMode(LED, OUTPUT);
  while (1) {
    // check if task needs any further changes in this time slice
    if (!LEDdone && remainingLedTime >= pdMS_TO_TICKS(LED_TIME * 2)) {
      // if so, do task in current time slice by toggling LED on and off
      digitalWrite(LED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(LED_TIME));
      digitalWrite(LED, LOW);
      vTaskDelay(pdMS_TO_TICKS(LED_TIME));
      remainingLedTime -= pdMS_TO_TICKS(LED_TIME * 2);
    } else {
      // if not, consider task as complete
      Serial.println("LED Complete");
      LEDdone = true;
    }
    vTaskSuspend(NULL);
  }
}

/**
 * @brief Displays a counter on the LCD every 100 ms until it reaches 20 or time expires
 * @param arg Unused task parameter
 */
void counterTask(void *arg) {
  while (1) {
    // check if task needs any further changes in this time slice
    if (!COUNTdone && count < 20 && remainingCounterTime >= pdMS_TO_TICKS(LCD_TIME)) {
      //if so, do task during time slice by incrementing count and printing to LCD
      count++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Count: ");
      lcd.print(count);
      
      // Serial.print("Time: " + String(millis()) + " for ");
      // Serial.println(count);
      vTaskDelay(pdMS_TO_TICKS(LCD_TIME));
      remainingCounterTime -= pdMS_TO_TICKS(LCD_TIME);
    } else {
      // if not consider task complete
      Serial.println("Count Complete");
      COUNTdone = true;
    }
    vTaskSuspend(NULL);
  }
}

/**
 * @brief Prints the alphabet to the serial monitor every 500 ms until Z or time expires
 * @param arg Unused task parameter
 */
void alphabetTask(void *arg) {
  while (1) {
    //check if task needs any further changes in this time slice. 
    if (!ALPHAdone && glyph < 'Z' && remainingAlphabetTime >= pdMS_TO_TICKS(PRINT_TIME)) {
      //if so, do next time slice by printing next char
      glyph++;
      // Serial.print("Time: " + String(millis()) + " for ");
      Serial.print(glyph);
      vTaskDelay(pdMS_TO_TICKS(PRINT_TIME));
      remainingAlphabetTime -= pdMS_TO_TICKS(PRINT_TIME);
    } else {
      // if not, consider task as done
      Serial.println();
      Serial.println("Alphabet Done");
      ALPHAdone = true;
    }
    vTaskSuspend(NULL);
  }
}

/**
 * @brief Implements Shortest Remaining Time First (SRTF) scheduling
 *        and resets all tasks when all are finished
 * @param arg Unused task parameter
 */
void scheduleTasks(void *arg) {
  while (1) {
    // Reset if all are done
    if (LEDdone && COUNTdone && ALPHAdone) {
      Serial.println("Resetting tasks...");
      count = 0;
      glyph = 'A' - 1;
      remainingLedTime = ledTaskExecutionTime;
      remainingCounterTime = counterTaskExecutionTime;
      remainingAlphabetTime = alphabetTaskExecutionTime;
      LEDdone = COUNTdone = ALPHAdone = false;
      
      vTaskSuspend(TaskLED_Handle);
      vTaskSuspend(TaskCOUNT_Handle);
      vTaskSuspend(TaskALPHA_Handle);
    }

    // SRTF Scheduling
    if (!LEDdone && (remainingLedTime <= remainingCounterTime || COUNTdone) && (remainingLedTime <= remainingAlphabetTime || ALPHAdone) && eTaskGetState(TaskLED_Handle) == eSuspended) {
      vTaskResume(TaskLED_Handle);
    } else if (!COUNTdone && (remainingCounterTime <= remainingLedTime || LEDdone) && (remainingCounterTime <= remainingAlphabetTime || ALPHAdone) && eTaskGetState(TaskCOUNT_Handle) == eSuspended) {
      vTaskResume(TaskCOUNT_Handle);
    } else if (!ALPHAdone && (remainingAlphabetTime <= remainingLedTime || LEDdone) && (remainingAlphabetTime <= remainingCounterTime || COUNTdone) && eTaskGetState(TaskALPHA_Handle) == eSuspended) {
      vTaskResume(TaskALPHA_Handle);
    }

    vTaskDelay(pdMS_TO_TICKS(1));  //frequency with which to check
  }
}

//========= SETUP =========
/**
 * @brief Arduino setup function
 * @details Initializes peripherals and creates tasks pinned to Core 0
 */
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Create tasks pinned to core 0
  xTaskCreatePinnedToCore(ledTask, "BlinkingLED", 2048, NULL, 1, &TaskLED_Handle, 0);
  xTaskCreatePinnedToCore(counterTask, "CountingLCD", 2048, NULL, 1, &TaskCOUNT_Handle, 0);
  xTaskCreatePinnedToCore(alphabetTask, "AlphabetPrintSerial", 4096, NULL, 1, &TaskALPHA_Handle, 0);
  xTaskCreatePinnedToCore(scheduleTasks, "SchedulerSRTF", 2048, NULL, 2, &TaskSchedule_Handle, 0); // Higher priority
}


//========= LOOP =========
/**
 * @brief Empty main loop as tasks handle all operations
 */
void loop() {
  // not used
}

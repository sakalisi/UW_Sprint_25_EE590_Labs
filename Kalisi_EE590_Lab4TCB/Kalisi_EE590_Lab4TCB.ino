/**
 * @file EE_590_Lab4TCB.ino
 *
 * @mainpage EE 590 Lab 4 TCB
 *
 * @section description Description
 * This sketch holds the implementation corresponding to the Lab document for task 2.
 * Lab 4 arduino ESP 32 File.
 *
 * @section circuit Circuit
 * - LCD connected to SDA an SCL at pin 20 and 21.
 * - green LED connected at pin 1
 * - orange/yellow LED connected at pin 2
 *
 * @section libraries Libraries
 * - arduino-esp32 (https://github.com/espressif/arduino-esp32)
 * - Wire
 * - LiquidCrystal_I2C
 *
 * @section notes Notes
 * - Comments are Doxygen compatible.
 *
 * @section date DATE
 * 05/25/25 
 *
 * @section author Author
 * Last Modified by Sai Jayanth Kalisi on 05/25/2025.
 */

#include "driver/gpio.h" ///< Required for GPIO use
#include "soc/io_mux_reg.h" ///< Required for quick GPIO use
#include "soc/gpio_reg.h" ///< Required for GPIO
#include "soc/gpio_periph.h" ///< Required for GPIO
#include "soc/timer_group_reg.h" ///< Required for Timing
#include "Wire.h" ///< Required for I2C communication
#include <LiquidCrystal_I2C.h> ///< Required for Quick LCD usage

// ========== CONSTS and DEFINEs =========== //
const int LED1 = 1; ///< Green LED pin
const int LED2 = 2; ///< Yellow/Orange LED pin

#define TIMER_DIVIDER_VAL 80 ///< Timer partition
#define N_MAX_TASKS       4 ///< Max number of tasks to consider. Should theoretically be 5, with last as NULL, but ignoring that for now

// =============== ENUMS =============== //

typedef enum taskstate{
  STATE_INACTIVE = 0,
  STATE_RUNNING,
  STATE_READY,
  STATE_WAITING, // not really used, kept from ICTE
} taskstate; ///< All possible states for Tasks to be 

// =============== STRUCTS =============== //
struct LEDControl {
  int pin;                          ///< Current LED Pin. Useful for multiple LED setup
  bool isBlinking;                  ///< Current LED action. Not really used, could be removed. Remanent from ICTE
  bool state;                       ///< Current LED state. should be HIGH or LOW
  unsigned long previousMicros;     ///< time since last blink in Microseconds
  unsigned long timeSinceBeginTask; ///< time since task started. Not really used, could be removed. Tried to do time based, but count based looks better
  unsigned long blinkInterval;      ///< how long between swaps in Microseconds
  int blinkCount;                   ///< how many blinks done. Max wshould be 16. Value can be coded in as another attribute
  bool isDone;                      ///< whether task is done
}; ///< All possible variables needed for controlling the LED

struct LCDControl {
  int currentCount;             ///< How many counted so far
  unsigned long previousMillis; ///< How long since updated in Milliseconds (used millis instead of micros when testing never really removed)
  unsigned long interval;       ///< Usual time between updates
  bool isDone;                  ///< if task is done
}; ///< All possible variables needed for controlling the LCD

struct LEDFreqControl {
  int pin;                  ///< Current LED Pin. Useful for multiple LED setup
  int freq;                 ///< freq of LED, calculates pulse width based on how often the pulse can take place
  int resolution;           ///< how detailed the pulse can be
  int previousMicros;       ///< How long since last time the intensity changed
  int variousFreqsChecked;  ///< How many frequencies we checked
  int changeTime;           ///< Minimum time between each intensity change
  bool isDone;              ///< If task is done
}; ///< All possible variables needed to control LED intensity

struct PrintControl {
  int previousMicros;   ///< Time since last print
  int changeTime;       ///< minimum time between each print
  int currentCount;     ///< Current number of chars printed
  const char* toPrint;  ///< vals to rotate through 
  bool isDone;          ///< if print is complete
}; ///< Required variables for controlling Print of all alphabets. Might att len of "toPrint" variable to be more robust.

typedef struct TCBstruct {
  void (*ftpr)(void *p);  ///< Function pointer
  void *arg_ptr;          ///< Arguements for the function
  taskstate state;        ///< current state of task
  int pid;                ///< Task ID
  int priority;           ///< Task Priority. It is set by scheduler.
  unsigned int delay;     ///< Delay after task. Not really used. Holdover from ICTE
} TCBStruct; ///< All possible variables needed to develop a thread control block

// =============== GLOBAL VARIABLES =============== //

LCDControl lcdControl = {1, 0, 500, false};                                       ///< LCD Control task pre-initialization
LiquidCrystal_I2C lcd(0x27, 16, 2);                                               ///< LCD pre-initialization
LEDControl led1 = {LED1, true, LOW, 0, 0, 62500, 0, false};                       ///< LED1 task pre-initialization
LEDFreqControl ledcControl = {LED2, 100, 11, 0, 0, 1000000, false};               ///< ledc-Control task pre-initialization
PrintControl printTask = {0, 1000000, 0, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", false};    ///< print task pre-initialization

TCBStruct TaskList[N_MAX_TASKS]; ///< list of all tasks 

int baseTaskIndex = 0;                ///< task priority modifier
volatile int currentRunningTask = -1; ///< current Task index

// =============== TASK FUNCTIONS =============== //

/**
 * @name Task Functions
 * @{
 */

/**
 * Name: taskA
 * @brief Handles setup and calling of blinking LED
 * @param p arguments to any function if needed
 * @details This function is a holdover from the ICTE. Modified it. Did not change much of the underlying logic.
 *          Calls LED blinking function.
 *          if task is done, and its the current task, sets the current running task to nothing, sets current state to inactive
 *          and prints out that the task is completed.
 */
void taskA(void *p) {
  handleLEDBlinking(led1);
  if (led1.isDone && TaskList[0].state != STATE_INACTIVE) {
    TaskList[0].state = STATE_INACTIVE;
    currentRunningTask = -1;

    Serial.print("Task Blink LED, priority ");
    Serial.print(TaskList[0].priority);
    Serial.println(" Completed");
  }
}

/**
 * Name: taskB
 * @brief Handles counting and display of current count on LCD
 * @param p arguments to any function if needed
 * @details Modified version of TaskA logic, but for LCD Control
 *          if task is done, returns
 *          if time threshold passes, count is printed and updated, threshold is reset. 
 *          if count surpasses threshold, task is considered done, and task status is updated, printed as complete, with priority
 */
void taskB(void *p) {
  unsigned long currentMillis = millis();

  if (lcdControl.isDone) {
    TaskList[1].state = STATE_INACTIVE; // might not be needed due to threshold logic. Keeping it here for now.
    return;
  }

  if (currentMillis - lcdControl.previousMillis >= lcdControl.interval) {
    lcdControl.previousMillis = currentMillis;
    lcd.setCursor(7, 0);
    lcd.print("   ");
    delayMicroseconds(20000);
    lcd.setCursor(7, 0);
    lcd.print(lcdControl.currentCount);
    delayMicroseconds(20000);

    lcdControl.currentCount++;

    //threshold to meet to be considered complete
    if (lcdControl.currentCount > 10) {
      lcdControl.isDone = true;
      TaskList[1].state = STATE_INACTIVE;
      currentRunningTask = -1;
      Serial.print("Task LCD Count, priority ");
      Serial.print(TaskList[1].priority);
      Serial.println(" Completed");
    }
  }
}

/**
 * Name: taskC
 * @brief Handles increasing intensity of LED
 * @param p arguments to any function if needed
 * @details Modified version of TaskA logic, but LED intensity control
 *          if task is done, returns
 *          LED is constantly outputted at a given frequency
 *          if time threshold passes, intensity is and updated, time threshold is reset. 
 *          if count surpasses number of intensities to check, task is considered done, and task status is updated, printed as complete, with priority
 */
void taskC(void *p) {
  unsigned long currentMicros = micros();
  if (ledcControl.isDone) {
    TaskList[2].state = STATE_INACTIVE; // might not be needed due to threshold logic. Keeping it here for now.
    return;
  }
  
  ledcWrite(ledcControl.pin, ledcControl.variousFreqsChecked * 50);

  if(currentMicros - ledcControl.previousMicros > ledcControl.changeTime) {
    ledcControl.previousMicros = currentMicros;
    ledcControl.variousFreqsChecked++;

    if(ledcControl.variousFreqsChecked > 10) {
      ledcControl.variousFreqsChecked = 0;
      ledcControl.isDone = true;
      TaskList[2].state = STATE_INACTIVE;
      currentRunningTask = -1;
      Serial.print("Task LED Intensity, priority ");
      Serial.print(TaskList[2].priority);
      Serial.println(" Completed");
    }
  }
}

/**
 * Name: taskD
 * @brief Handles Printing of alphabet
 * @param p arguments to any function if needed
 * @details Modified version of TaskA logic, but for printing alphabet
 *          if task is done, returns
 *          if time threshold passes, value is printed and updated, time threshold is reset. 
 *          if count surpasses number of letters to check, task is considered done, and task status is updated, printed as complete, with priority
 */
void taskD(void *p){
  unsigned long currentMicros = micros();
  if (printTask.isDone) {
    TaskList[3].state = STATE_INACTIVE; // might not be needed due to threshold logic. Keeping it here for now.
    return;
  }

  if(currentMicros - printTask.previousMicros > printTask.changeTime) {
    Serial.print(printTask.toPrint[printTask.currentCount]);
    printTask.currentCount++;
    printTask.previousMicros = currentMicros;

    if(printTask.currentCount > 25) {
      Serial.println();
      ledcControl.isDone = true;
      TaskList[3].state = STATE_INACTIVE;
      currentRunningTask = -1;
      Serial.print("Alphabet Print, priority ");
      Serial.print(TaskList[3].priority);
      Serial.println(" Completed");
    }
  }
}

/**
 * @}
 */

// =============== LED Handler =============== //

/**
 * Name: handleLEDBlinking
 * @brief blinks an LED a given number of times at a given frequency
 * @param led struct containing all information needed to run task. See struct for details
 * @details This function is a holdover from the ICTE. Modified it. Did not change much of the underlying logic.
 *          if task is done, returns
 *          if time threshold is met, swaps LED state
 *          if blink count reaches count, considers task done
 */
void handleLEDBlinking(LEDControl& led) {
  unsigned long currentMicros = micros();

  if (led.isDone) return;

  if ((currentMicros - led.previousMicros) >= led.blinkInterval) {
    led.previousMicros = currentMicros;
    led.state = !led.state;
    digitalWrite(led.pin, led.state);
    led.blinkCount++;

    if (led.blinkCount >= 16) { // 8 blinks = 16 transitions
      led.isDone = true;
    }
  }
}

// =============== SCHEDULER =============== //
/**
 * Name: scheduler
 * @brief handles running tasks in given order
 * @details if no task is running:
 *           - loops through all tasks and runs the first one that is ready, given the current offset it needs to follow
 *           - If state is ready runs only that task and nothing else for the cycle
 *          If a task is running:
 *           - Calls just that task
 *          If all tasks have run, calls resetTasks() function and updates starting index
 */
void scheduler() {
  if (currentRunningTask == -1) {
    for (int i = 0; i < N_MAX_TASKS; i++) {
      int idx = (baseTaskIndex + i) % N_MAX_TASKS;
      if (TaskList[idx].state == STATE_READY) {
        Serial.print("Started task at ");
        Serial.println(idx);
        TaskList[idx].state = STATE_RUNNING;
        currentRunningTask = idx;
        TaskList[idx].ftpr(TaskList[idx].arg_ptr);
        TaskList[idx].priority = i + 1;
        break;  // Only one task runs at a time
      }
    }
  } else {
    // Serial.print("Continuing Task at ");
    // Serial.println(currentRunningTask);
    TaskList[currentRunningTask].ftpr(TaskList[currentRunningTask].arg_ptr);
  }

  // Check if all are inactive, then rotate
  bool allDone = true;
  for (int i = 0; i < N_MAX_TASKS; i++) {
    if (TaskList[i].state != STATE_INACTIVE) {
      allDone = false;
      break;
    }
  }

  if (allDone) {
    baseTaskIndex = (baseTaskIndex + 1) % N_MAX_TASKS;
    resetTasks();
  }
}

/**
 * Name: resetTasks
 * @brief resets all tasks to a working state
 * @details goes through all structs and sets isDone to false, and ensures all reach their starting values
 *          Sets the state of all tasks to STATE_READY, and dictates that no task is currently running
 */
void resetTasks() {
  //resetting blink
  led1.blinkCount = 0;
  led1.previousMicros = 0;
  led1.state = LOW;
  digitalWrite(led1.pin, led1.state);
  led1.isDone = false;

  //resetting LCD
  lcdControl.currentCount = 1;
  lcdControl.previousMillis = 0;
  lcdControl.isDone = false;

  //resetting LEDC
  ledcControl.isDone = false;
  ledcControl.variousFreqsChecked = 0;

  //resetting printing
  printTask.currentCount = 0;
  printTask.previousMicros = 0;
  printTask.isDone = false;

  for (int i = 0; i < N_MAX_TASKS; i++) {
    TaskList[i].state = STATE_READY;
  }
  currentRunningTask = -1;
}

// =============== SETUP =============== //

/**
 * Name: setup
 * @brief sets up all pins and initializes tasks to the task list. 
 * @details Starts up serial, LED pins, I2C pins, LCD pins and Timer.
 *          Initializes array of tasks and their primary values
 */
void setup() {
  Serial.begin(115200);

  pinMode(led1.pin, OUTPUT);
  digitalWrite(led1.pin, LOW);

  Wire.begin(20, 21);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Count: ");

  ledcAttach(ledcControl.pin, ledcControl.freq, ledcControl.resolution);

  uint32_t timer_config = (TIMER_DIVIDER_VAL << 13) | (1 << 31) | (1 << 30);
  *((volatile uint32_t *) TIMG_T0CONFIG_REG(0)) = timer_config;
  *((volatile uint32_t *) TIMG_T0UPDATE_REG(0)) = 1;

  TaskList[0] = {taskA, NULL, STATE_READY, 1, 0, 0};
  TaskList[1] = {taskB, NULL, STATE_READY, 2, 0, 0};
  TaskList[2] = {taskC, NULL, STATE_READY, 3, 0, 0};
  TaskList[3] = {taskD, NULL, STATE_READY, 4, 0, 0};
}

// =============== MAIN LOOP =============== //
/**
 * Name: loop
 * @brief loop equivalent to while(1), runs scheduler every cycle and updates timer
 */
void loop() {
  *((volatile uint32_t *) TIMG_T0UPDATE_REG(0)) = 1; 
  scheduler(); 
  delayMicroseconds(15000);
}
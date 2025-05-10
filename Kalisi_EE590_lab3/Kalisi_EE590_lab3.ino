/**
 * @file EE_590_Lab_3.ino
 *
 * @mainpage EE 590 Lab 3
 *
 * @section description Description
 * This sketch holds the implementation corresponding to the Lab document for task 2-6
 * Lab 3 arduino ESP 32 File.
 *
 * @section circuit Circuit
 * - Green LED connected to pin 1.
 * - LEDR connected to pin 10.
 *
 * @section libraries Libraries
 * - arduino-esp32 (https://github.com/espressif/arduino-esp32)
 *   - Interacts with on-board IMU.
 *
 * @section notes Notes
 * - Comments are Doxygen compatible.
 *
 * @section todo TODO
 * - Don't use Doxygen style formatting inside the body of a function.
 *
 * @section date DATE
 * 05/08/25 
 *
 * @section author Author
 * - Created by Gokul Nathan (TA).
 * - Last Modified by Sai Jayanth Kalisi on 05/08/2025.
 */


// =========== Libraries ===========
#include "590Lab3.h"
#include "Special590functions.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"
#include "soc/timer_group_reg.h"


// =========== Defines ===========
#define INITIAL_BUFFER_SIZE 5 ///< Buffer size initial for testing Task 3.
#define FIB_SIZE 10 ///< N elements of the Fibonacci Series to find. 
#define FACT_VAL 10 ///< Value for which the Factorial needs to be found.

#define TIME_DIV 80 ///< dividing the 80Mhz to 1Mhz.
#define TIME_EN (1<<31) ///< enabling timer.
#define TIME_INCREMENT_MODE (1<<30) ///< choosing to increment timer instead of decrement.
#define COUNT 1000000 ///< num cycles to count to, which is equivalent to 1 second.

#define LED 1 ///< LED output.


// =========== GLOBAL VARIABLES ===========
// uint32_t LEDR_timer; ///< Timer which represents how long it has been since LEDR has been read.
uint32_t LED_timer; ///< Timer which represents how long it has been since LED has changed/been outputted to.
// uint32_t AVG_timer; ///< Timer which represents how long it has been since the last average was taken.
uint32_t BACKGROUND_timer; ///< Timer which represents how long it has been since the background tasks were called.
DynamicArray processedData; ///< processData holds a growing dynamic array that stores a list of average brightnesses
CircularBuffer cb; ///< cb is a Circular buffer, meant to be at size 5, holds LEDR brightness values
CircularBuffer cb_t5; ///< cb_t5 is a circular buffer used to test Task 5. 

// ==== HELPER and TEST TASK FUNCTIONS ====

/**
 * @name Lab Test Functions
 * @{
 */

/**
 * Name: task2
 * @brief A function to run required Fibonacci and Factorial Test Cases. 
 * Also details how much memory each task takes up.
 */
void task2(){
  // Task 2: Fibonacci and Factorial test cases. Use as needed
  printString("Task 2: Fibonacci and Factorial\n");
  
  ///// Handling Fibonacci
  unsigned long long *fibSequence;
  fibonacci(FIB_SIZE, &fibSequence);
  printString("Fibonacci: ");

  for (int i = 0; i < FIB_SIZE; i++) {
    printInt(fibSequence[i]);
    if (i < FIB_SIZE-1) printString(", ");
  }
  free(fibSequence);
  printString("\n");

  printString("The size of the Fibonacci is: ");
  printInt(sizeof(unsigned long long) * FIB_SIZE);
  printString("\n");

  ///// Handling Factorial
  unsigned long long factorialResult;
  factorial(FACT_VAL, &factorialResult);

  printString("Factorial of ");
  printInt(FACT_VAL);
  printString(": ");
  printInt(factorialResult);
  printString("\n");

  printString("Mem use of Factorial is: ");
  printInt(sizeof(unsigned long long) * 2);
  printString("\n");
  printString("Task 2 Completed.\n");
  printString("\n");

}

/**
 * Name: task3
 * @brief A function to run required Dynamic Array Test Cases. 
 * Also details how much memory the task takes up.
 */
void task3(){
  // Task 4: Dynamic Array sample test cases. Use as needed
  printString("Task 3: Dynamic Array\n");
  DynamicArray arr;
  initArray(&arr, 2);
  addElement(&arr, 21);
  addElement(&arr, 26);
  addElement(&arr, 31);
  addElement(&arr, 19);
  addElement(&arr, 24);
  addElement(&arr, 29);
  addElement(&arr, 1);
  addElement(&arr, 65);
  addElement(&arr, 36);
  addElement(&arr, 41);
  printArray(&arr);

  printString("CurrentSize: ");
  printInt((&arr)->size);
  printString("/");
  printInt((&arr)->capacity);
  printString("\n");
  printString("Memory Report:\n");
  printString("\t- Total allocated: ");
  printInt(sizeof(int) * (&arr)->capacity);
  printString(" Bytes\n");
  printString("\t- Used: ");
  printInt(sizeof(int) * (&arr)->size);
  printString(" Bytes\n");
  printString("\t- FreeSpace for more elements: ");
  printInt(sizeof(int) * ((&arr)->capacity - (&arr)->size));
  printString(" Bytes\n");

  freeArray(&arr);
  printString("Freed\n");
  printString("Task 3 Completed.\n");
  printString("\n");
}

/**
 * Name: task5
 * @brief A function to run required Reverse String and Circular Buffer Test Cases. 
 */
void task5() {
  // Task 5: Reverse String and Circular Buffer
    printString("Task 5: Reverse String and Circular Buffer\n");
    char str[] = "reversed";
    reverseString(str);
    printString("Reversed String: ");
    printString(str);
    printString("\n");

    initBuffer(&cb_t5, BUFFER_SIZE);
    writeBuffer(&cb_t5, 10);
    writeBuffer(&cb_t5, 20);
    writeBuffer(&cb_t5, 30);
    int read = readBuffer(&cb_t5);
    pushBuffer(&cb_t5, 40);
    pushBuffer(&cb_t5, 50);
    int pop = popBuffer(&cb_t5);
    pop = popBuffer(&cb_t5);
    writeBuffer(&cb_t5, 60);
    pop = popBuffer(&cb_t5);
    pop = popBuffer(&cb_t5);
    pop = popBuffer(&cb_t5);
    pop = popBuffer(&cb_t5);
    pop = popBuffer(&cb_t5);
    freeBuffer(&cb_t5);

    printString("Completed Task 5\n");
    printString("\n");
}

/**
 * Name: task4
 * @brief A function to run required Pointer Operations Test Cases, and prints out truth table. 
 */
void task4() {
  printString("Task 4: Pointer Operations\n");

  //base array
  int array[] = { 3, 7, 11, 15 };
  printString("Array1: ");
  print_static_array(array, 4);

  // Handling Copy, and array modify
  int array2[4];
  mem_copy(array2, array, sizeof(array2));
  printString("Array 2 copied 4 elems from array 1: ");
  print_static_array(array2, 4);
  
  printString("Array tripled: ");
  array_modify(array, 4, mult_3);
  print_static_array(array, 4);

  printString("Array2 + 1: ");
  array_modify(array2, 4, add_1);
  print_static_array(array2, 4);

  //Truth Table
  const char* a = "10000";
  const char* b = "1100";
  print_truth_table(a, b);

  Serial.println("Task 4 completed.\n");
}

/**
 * @}
 */


/**
 * @name Testing Helper Functions
 * @{
 */

/**
 * Name: double_value
 * @brief Doubles input integer and returns a new integer.
 * @param x is any integer. 
 * @return 2x.
 */
int double_value (int x) {
  return 2 * x;
}

/**
 * Name: mult_3
 * @brief Triples input integer and returns a new integer.
 * @param x is any integer.
 * @return 3x 
 */
int mult_3 (int x) {
  return 3 * x;
}

/**
 * Name: subtract_ten
 * @brief Subtracts 10 from the integer provided and returns a new integer.
 * @param x any integer.
 * @return x - 10
 */
int subtract_ten (int x) {
  return x - 10;
}

/**
 * Name: add_1
 * @brief adds 1 to the integer provided and returns a new integer.
 * @param x is any integer.
 * @return x + 1
 */
int add_1 (int x) {
  return x + 1;
}

/**
 * Name: testPointerOperations
 * @brief Testing for Task 4, checks all functions involved. 
 * Tests all functions, not just ones looked at in the demo.
 */
void testPointerOperations() {
  printString("Testing Task 4: Pointer Operations\n");

  //base array
  int array[] = { 3, 7, 11, 15 };
  printString("Array1: ");
  print_static_array(array, 4);

  // Distance calcs
  int distance = pointer_distance(&array[0], &array[3], array, array + 5);
  printString("Pointer Distance (array[1] to array[4]): ");
  printInt(distance);  // Expected: 3
  printString("\n");

  distance = pointer_distance(&array[3], &array[0], array, array + 5);
  printString("Pointer Distance (array[4] to array[1]): ");
  printInt(distance);  // Expected: -3
  printString("\n");

  distance = pointer_distance(&array[0], &array[5], array, array + 5);
  printString("Pointer Distance (array[0] to out-of-bounds): ");
  if(distance == INT_MAX){
    printString("INT_MAX (out_of_bounds)");
  } else {
    printInt(distance);
  }   // Expected: INT_MAX
  printString("\n");

  // Handling Copy, and array modify
  int array2[4];
  mem_copy(array2, array, sizeof(array2));
  printString("Array 2 copied 4 elems from array 1: ");
  print_static_array(array2, 4);
  
  printString("Array tripled: ");
  array_modify(array, 4, mult_3);
  print_static_array(array, 4);

  printString("Array2 + 1: ");
  array_modify(array2, 4, add_1);
  print_static_array(array2, 4);

  //str_to_int
  printString("Testing str_to_int\n");
  printInt(str_to_int("4745"));
  printString("\n");
  printInt(str_to_int("456545"));
  printString("\n");
  printInt(str_to_int("-42323"));
  printString("\n");

  //Truth Table
  const char* a = "10000";
  const char* b = "1100";

  print_truth_table(a, b);

  Serial.println("Task 4 completed.\n");
}

/**
 * Name: testReverse
 * @brief Testing reverse function for task 5.
 */
void testReverse(){
  printString("Testing Task 5: Reversing a String.\n");
  
  char myString[] = "embedded systems";
  printString("Original String is: ");
  printString(myString);
  printString("\n");
  reverseString(myString);
  printString("Reversed String is: ");
  printString(myString);
  printString("\n");
  printString("\n");
}

/**
 * Name: testCircularBuffer
 * @brief Testing circular buffer functions for task 5.
 * Tests all functions, not just those looked at in the demo.
 */
void testCircularBuffer() {
  printString("Testing Task 5: Circular Buffer Operations.\n");

  // Initialize buffer
  initBuffer(&cb_t5, INITIAL_BUFFER_SIZE);
  printString("Initialized Circular Buffer with size 5.\n");

  // Write elements into the buffer
  printString("Writing elements [1, 2, 3, 4, 5]:\n");
  for (int i = 1; i <= 5; i++) {
    writeBuffer(&cb_t5, i);
  }
  printString("Expected buffer contents: [1, 2, 3, 4, 5].\n");
  printString("Actual buffer contents: ");
  for (int i = 0; i < cb_t5.count; i++) {
    printInt(cb_t5.buffer[(cb_t5.tail + i) % cb_t5.max_size]);
    printString(" ");
  }
  printString("\n");

  // Read the oldest element without modifying the buffer
  int readValue = readBuffer(&cb_t5);
  printString("\n");
  printString("Read the oldest element (before pop): ");
  printInt(readValue);  // Expected: 1
  printString("\n");

  // Pop the oldest element
  int poppedValue = popBuffer(&cb_t5);

  // Read the oldest element again without modifying the buffer
  readValue = readBuffer(&cb_t5);
  printString("Read the oldest element (after pop): ");
  printInt(readValue);  // Expected: 2
  printString("\n");

  // Resize the buffer
  printString("\n");
  printString("Resizing buffer from size 5 to 10...\n");
  resizeBuffer(&cb_t5, 10);

  // Fill buffer with values counting down from 99
  printString("\n");
  printString("Filling buffer with values counting down from 99:\n");
  for (int i = 99; i >= 90; i--) {
    writeBuffer(&cb_t5, i);
  }
  printString("Expected buffer contents: [2, 3, 4, 5, 99, 98, 97, 96, 95, 94, ]\n");
  printString("Actual buffer contents: [");
  for (int i = 0; i < cb_t5.count; i++) {
    printInt(cb_t5.buffer[(cb_t5.tail + i) % cb_t5.max_size]);
    printString(", ");
  }
  printString("]");

  // Pop and print all values
  printString("\n");
  printString("Popping values:\n");
  int break_count = 0;
  while (!isEmpty(&cb_t5)) {
    int value = popBuffer(&cb_t5);
    break_count += 1;
    if (break_count > 30) break;
  }

  printString("Expected buffer contents: []\n");
  printString("Actual buffer contents: [");
  for (int i = 0; i < cb_t5.count; i++) {
    printInt(cb_t5.buffer[(cb_t5.tail + i) % cb_t5.max_size]);
    printString(" ");
  }
  printString("]");


  // Free buffer memory
  freeBuffer(&cb_t5);
  printString("\n");
  printString("Circular Buffer test completed.\n");
}

/**
 * @}
 */

// ================== MAIN FUNCTIONS =======================

/**
 * @name Main Function Components
 * @{
 */

/**
 * Name: start_timer
 * @brief Setting up timer
 * @details Sets up a timer, expected 10Mhz timer.
 */
void start_timer() {
  uint32_t timer_config = TIME_EN | TIME_INCREMENT_MODE | (TIME_DIV << 13);

  //Writing config into timer register
  *(volatile uint32_t *) TIMG_T0CONFIG_REG(0) = timer_config;  
}

/**
 * Name: setup
 * @brief sets up all pins, timers and arrays to be used. 
 * @details Serial Monitor is begun at 9600 baud
 *      Tasks 2-5 are run. During testing, testPointerOperations, testReverse, and testCircularBuffer are run as well.
 *      Pin LED is enabled as GPIO, marked as an output and instantiated to 0. 
 *      Timers are configured. All timers are initialized.
 *      LEDC is attached 10 100Hz and 11 precision.
 */
void setup() {
  Serial.begin(9600);

  printString("Starting Arduino Tasks -> Test case D\n");
  task2();
  task3();
  task4();
  // testPointerOperations();
  // testReverse();
  // testCircularBuffer();
  task5();

  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED], PIN_FUNC_GPIO); // DEFINING LED_PIN as a GPIO PIN
  *(volatile uint32_t *) GPIO_ENABLE_REG |= (1 << LED); // Enabling pin
  *(volatile uint32_t *) GPIO_OUT_REG &= ~(1 << LED); // Switching off in the beginning

  start_timer();
  // AVG_timer = *(volatile uint32_t *) TIMG_T0LO_REG(0); // start val for Averaging LEDR readings timer set up
  LED_timer = *(volatile uint32_t *) TIMG_T0LO_REG(0); // start val for LED output modulation timer set up
  BACKGROUND_timer = *(volatile uint32_t *) TIMG_T0LO_REG(0); // start val for background tasks timer set up
  // LEDR_timer = *(volatile uint32_t *) TIMG_T0LO_REG(0); // start val for appending to circular buffer timer set up

  // Initialize arrays
  initBuffer(&cb, 5);
  initArray(&processedData, 10);

  //initialize LED as LEDC
  ledcAttach(LED, 100, 11); 
}

/**
 * Name: loop
 * @brief loop to be run repeatedly. Equivalent to running everything in main whith a while(1).
 * @details Updates timers and reads analog input from LEDR pin.
 *      checks if timers of 500ms, 2.5s, 1s, and 10s are triggered. expected outcomes are detailed further
 */
void loop() {
  // Task 6
  // Observed Behavior: Simulates environmental sensing via an LDR, processes the readings, updates LED state, and stores averaged results.
  // Commented Purpose: Demonstrates integration of string manipulation, dynamic memory, and circular buffers in a real-time sensing and control task.
  // Edge Case Handling: Handles buffer wrap-around, buffer full condition, and dynamic array resizing automatically.
  // Error Handling: Assumes sensor and LED GPIO are properly initialized elsewhere as direct register access is used.
  *(volatile uint32_t *) TIMG_T0UPDATE_REG(0) = 1; // Latch timer value
  uint32_t curr_time = *(volatile uint32_t *) TIMG_T0LO_REG(0); //read timer val

  simulateSensorData(&cb, &processedData);

  //    c. Check if 1 Second has passed since last LED update.
  //       - If yes:
  //          • Use most recent average as a brightness level for the LED.
  //          • Apply this brightness level to the LED using PWM register logic.
  //          • Reset the 1000 ms timer.
  if(curr_time - LED_timer > COUNT) {
    ledcWrite(LED, (&processedData)->data[(&processedData)->size - 1]);
    LED_timer = curr_time;
  }

  //    d. Check if 10 seconds have passed since the last background tasks:
  //       - If yes:
  //          • Do all tasks from 2 to 5 again.
  //          • Reset the 10000 ms timer.
  if(curr_time - BACKGROUND_timer > 10 * COUNT) {
    BACKGROUND_timer = curr_time;
    task2();
    task3();
    task4();
    task5();
  }

  // 3. Do not use any blocking function calls (no delays).
  // 4. Do not use pinMode() or digitalWrite(); use direct register access instead.
  // 5. Ensure that your code does not interfere with other running tasks in the .ino file.
}
/**
 * @}
 */

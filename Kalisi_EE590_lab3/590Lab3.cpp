/**
 * @file 590Lab3.cpp
 *
 * @mainpage Base functions for Lab 3
 *
 * @section description Description
 * This cpp file holds all functionality and implementations for lab tasks
 *
 * @section notes Notes
 * - Comments are Doxygen compatible.
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
#include "Arduino.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soc/timer_group_reg.h"

#define LEDR 10

/**
 * @name Task 2
 * @{
 */

/**
 * Name: fibonacci
 * @brief Demo Task 2.1: Fibonacci Sequence
 * @details Observed Behavior: Generates a Fibonacci sequence of length N and stores it in a dynamic array.
 *      Commented Purpose: Illustrates recursive or iterative numerical computation.
 *      Edge Case Handling: Exit early if N <= 0.
 *      Error Handling: Assumes memory is properly allocated for sequence.
 * @param N length of the fibonacci series.
 * @param sequence address of the pointer provided, which holds the fibonacci series. 
 */
void fibonacci(int N, unsigned long long **sequence) {
  // to do
  // task 2 step 1: Print the fibonacdi sequence upto digit n
  if(N < 0) return;

  *sequence = (unsigned long long*) malloc(N * sizeof(unsigned long long));

  if(*sequence == NULL) return;

  if(N > 0) (*sequence)[0] = 0;
  if(N > 1) (*sequence)[1] = 1;

  for (int i = 2; i < N; i++) {
    (*sequence)[i] = (*sequence)[i-1] + (*sequence)[i-2];
  }
}

/**
 * Name: factorial
 * @brief Demo Task 2.2: Factorial Calculation
 * @details Observed Behavior: Computes the factorial of a given non-negative integer.
 * Commented Purpose: Basic recursion/iteration exercise for mathematical operations.
 * Edge Case Handling: For n = 0, result is 1.
 * Error Handling: Returns -1 for negative input.
 * @param n number for which a factorial is taken
 * @param result address of the unsigned long long which holds the fibonacci result 
 * @retval -1 if n < 0
 */
int factorial(int n, unsigned long long *result) {
  // to do
  // task 2 step 1: Print the fibonacdi sequence upto digit n
  if (n < 0) return -1;
  if (n == 0 || n == 1) {
    *result = 1;
    return 1;
  }
  
  *result = n * factorial(n - 1, result);
  return *result;
}

/**
 * @}
 */

/**
 * @name Task 3
 * @{
 */

/**
 * Name: initArray
 * @brief Demo Task 3.1: Initialize Dynamic Array
 * @details Observed Behavior: Initializes a dynamic integer array with specified capacity.
 * Commented Purpose: Teaches dynamic memory allocation.
 * Edge Case Handling: Ensures zero-size start is not allowed.
 * Error Handling: Returns on malloc failure.
 * @param arr Dynamic Array to be initialized
 * @param initialCapacity Capacity for which the array memory needs to be initialized.
 */
void initArray(DynamicArray *arr, int initialCapacity) {
  // Allocate memory for initialCapacity integers.
  // Set the array’s size to 0.
  // Set the array’s capacity to the value of initialCapacity.
  // If memory allocation fails, print an error message to the Serial Monitor and exit the function.
  arr->size = 0;
  arr->capacity = initialCapacity;
  arr->data = (int *) malloc(sizeof(int) * initialCapacity);

  if(arr->data == NULL) {
    printString("Initialization of Array failed.\n");
  }
}

/**
 * Name: addElement
 * @brief Demo Task 3.2: Add Element to Dynamic Array
 * @details Observed Behavior: Adds an element to the array; resizes if needed.
 * Commented Purpose: Demonstrates array growth logic.
 * Edge Case Handling: Handles capacity overflow.
 * Error Handling: Memory errors should be caught and reported.
 * @param arr Dynamic Array to which an additional element needs to be added
 * @param element Element which needs to be added
 */
void addElement(DynamicArray *arr, int element) {
  // Before inserting, check if the array has reached its capacity.
  // If so, reallocate memory to double the current capacity and update the internal capacity value.
  // After ensuring space is available, insert the new value at the current size index.
  // Increment the size field.
  if(arr->size > arr->capacity - 1) {
    arr->data = (int *) realloc(arr->data, arr->capacity * 2 * sizeof(int));
    if(arr->data == NULL) {
      printString("Increasing Capacity failed.\n");
    } else {
      arr->capacity = arr->capacity * 2;
    }
  }

  arr->data[arr->size] = element;
  arr->size++;
  printString("addElement: ");
  printInt(element);
  printString("\n");
}

/**
 * Name: printArray
 * @brief Demo Task 3.3: Print Dynamic Array
 * @details Observed Behavior: Prints each element of the array to the serial monitor.
 * Commented Purpose: Provides output visibility of internal data.
 * Edge Case Handling: Prints only "[]" if empty.
 * Error Handling: Assumes array is initialized.
 * @param arr Dynamic Array to which an additional element needs to be printed
 */
void printArray(DynamicArray *arr) {
  // Loop through the array and print each integer.
  // Separate values using commas or tabs.
  // Print a newline character after the last value for formatting.
  printString("[");
  for (int i = 0; i < arr->size - 1; i++){
    printInt(arr->data[i]);
    printString(", ");
  }
  printInt(arr->data[arr->size-1]);
  printString("]\n");
}

/**
 * Name: freeArray
 * @brief COMPLETED SAMPLE FUNCTION Free Dynamic Array
 * @details  Observed Behavior: Frees memory and resets array fields.
 * Commented Purpose: Demonstrates cleanup of dynamic structures.
 * Edge Case Handling: Can be safely called multiple times.
 * @param arr Dynamic Array to which an additional element needs to be printed
 */
void freeArray(DynamicArray *arr) {
  free(arr->data);
  arr->data = NULL;
  arr->size = 0;
  arr->capacity = 0;
}

/**
 * @}
 */

/**
 * @name Task 4
 * @{
 */

/**
 * Name: pointer_distance
 * @brief Demo Task 4.1: Pointer Distance
 * @details  Calculates the number of elements between two pointers within the same array.
 * Commented Purpose: Demonstrates cleanup of dynamic structures.
 * Edge Case Handling: Can be safely called multiple times.
 * @param ptr1 first pointer to compare.
 * @param ptr2 second pointers to compare.
 * @param array_start beginning of the array.
 * @param array_end ending bounds of the array.
 * @retval INT_MAX if out of bounds.
 * @return integer distance.
 */
int pointer_distance(int *ptr1, int *ptr2, int *array_start, int *array_end) {
  // complete the function
  if(array_start <= ptr1 && array_end > ptr1 && array_end > ptr2 && array_start <= ptr2) {
    return (ptr2 - ptr1);
  }
  return INT_MAX;
}

/**
 * Name: mem_copy
 * @brief Demo Task 4.2: Memory Copy
 * @details  Copies `n` bytes from `src` to `dest`.
 * @param dest pointer to destination
 * @param src pointer to source
 * @param n number of bytes to copy
 */
void mem_copy(void *dest, const void *src, size_t n) {
  // complete the function
  if (n <= 0){
    return;
  }

  char *d = (char *)dest;
  const char *s = (const char *)src;
  
  for (size_t i = 0; i < n; i++){
    d[i] = s[i];
  }
}

/**
 * Name: array_modify
 * @brief Demo Task 4.3: Array Modify
 * @details Modifies each element of the array using a function pointer.
 * @param arr pointer to static (non dynamic) array to modify
 * @param len length of array
 * @param mod_func function pointer which points to the function to apply to each element
 * @retval -1 if invalid inputs
 * @retval 0 if successfully modified array
 */
int array_modify(int *arr, size_t len, int (*mod_func)(int)) {
  // complete the function
  if(arr == NULL || mod_func == NULL) {
    return -1;
  }

  for(int i = 0; i < len; i++){
    arr[i] = mod_func(arr[i]);
  }
  return 0;
}

/**
 * Name: print_static_array
 * @brief Helper function 4.2-4.3: print_static_array
 * @details Prints "len" values from the array pointed to by 'arr'
 * @param arr pointer to array to print
 * @param len length of array
 */
void print_static_array(int *arr, size_t len){
  if(len < 0) return;

  printString("[");
  for (int i = 0; i < len - 1; i++){
    printInt(arr[i]);
    printString(", ");
  }

  if(len > 0) printInt(arr[len-1]);
  printString("]\n");
}

/**
 * Name: str_to_int
 * @brief Demo Task 4.4: String Array
 * @details Converts a string representing a positive unsigned integer into its numeric equivalent
 *      String representing a positive unsigned integer into its numeric equivalent
 * @param str pointer to string
 * @retval -1 if error
 * @return integer equivalent of the string
 */
int str_to_int(const char *str) {
  if(str == NULL) return -1;
  bool isNeg = false;

  if(str[0] == '-') isNeg = true;

  int currNum = 0;
  for (int i = 0; i < strlen(str); i++){
    if(isNeg && i > 0) {
      if(str[i] < 48 || str[i] > 58) {
        return -1;
      }
      currNum = currNum * 10 + (str[i] - 48);
    }
    else if(!isNeg && i >=0) {
      if(str[i] < 48 || str[i] > 58) {
        return -1;
      }
      currNum = currNum * 10 + (str[i] - 48);
    }
  }
  return isNeg ? -1 * currNum : currNum;
}

/**
 * Name: print_truth_table
 * @brief Demo Task 4.5: Print Truth Table
 * @details Observed Behavior: Computes and displays a truth table from two binary strings.
 *      Commented Purpose: Explains bitwise operations and string parsing.
 *      Edge Case Handling: Limits output to 16 bits max.
 *      Function to print the truth table for bitwise operations
 * @param a pointer to string which contains only numbers
 * @param b pointer to string which contains only numbers
 */
void print_truth_table(const char *a, const char *b) {
  int binary_a = str_to_int(a);
  int binary_b = str_to_int(b);

  // Print table header using Special590functions
  printString("\nTruth Table for Bitwise Operations\n");
  printString("------------------------------------------\n");
  printString("Index\tA\tB\tA AND B\tA OR B\tA XOR B\tNOT A\tNOT B\n");
  printString("------------------------------------------\n");

  // Loop through each bit position (up to 16 bits)
  for (int i = 0; i <= 15; i++) {
    if((binary_a >> i) > 0 ||  (binary_b >> i) > 0) {
      int bit_A = (binary_a >> i) & 1;
      int bit_B = (binary_b >> i) & 1;
      int bit_and = bit_A & bit_B;
      int bit_xor = bit_A ^ bit_B;
      int bit_or = bit_A | bit_B;
      int bit_not_A = (~binary_a >> i) & 1;
      int bit_not_B = (~binary_b >> i) & 1;

      printInt(i);
      printString("\t");
      printInt(bit_A);
      printString("\t");
      printInt(bit_B);
      printString("\t");
      printInt(bit_and);
      printString("\t");
      printInt(bit_or);
      printString("\t");
      printInt(bit_xor);
      printString("\t");
      printInt(bit_not_A);
      printString("\t");
      printInt(bit_not_B);
      printString("\n");
    }
  }
  printString("------------------------------------------\n");
}

/**
 * @}
 */

/**
 * @name Task 5
 * @{
 */

/**
 * Name: reverseString
 * @brief Demo Task 5.1: String Manipulation
 * @details Reverses a null-terminated string provided in place. 
 * @param str pointer to string which will be reversed.
 */
void reverseString(char *str) {
  //To Do:

  // Set a pointer or index to the beginning of the string.
  // Set another pointer/index to the end of the string (just before the null terminator).
  // Swap characters at the two ends.
  // Move both pointers toward the center.
  // Repeat until the pointers meet or cross.
  char * ptr_beg;
  char * ptr_end;
  char temp;

  ptr_beg = str;
  ptr_end = str + strlen(str) - 1;

  while (ptr_beg < ptr_end) {
    temp = *ptr_beg;
    *ptr_beg = *ptr_end;
    *ptr_end = temp;

    ptr_beg++;
    ptr_end--;
  }
}

/**
 * Name: initBuffer
 * @brief Demo Task 5.2: Circular Buffer
 * @details COMPLETED SAMPLE FUNCTION. Mallocs buffer, sets up head, tail, count and max size
 * @param cb pointer to circular buffer to be initialized.
 * @param size size of which to initialize the buffer
 */
void initBuffer(CircularBuffer *cb, size_t size) {
  cb->buffer = (int *)malloc(size * sizeof(int));
  cb->head = 0;
  cb->tail = 0;
  cb->count = 0;
  cb->max_size = size;
}

/**
 * Name: freeBuffer
 * @brief Demo Task 5.2: Circular Buffer
 * @details COMPLETED SAMPLE FUNCTION. frees buffer, sets pointer to NULL, sets all integers to 0.
 * @param cb pointer to circular buffer to be freed.
 */
void freeBuffer(CircularBuffer *cb) {
  free(cb->buffer);
  cb->buffer = NULL;
  cb->head = 0;
  cb->tail = 0;
  cb->count = 0;
  cb->max_size = 0;
}

/**
 * Name: isFull
 * @brief Demo Task 5.2: Circular Buffer
 * @details Checks if buffer is full.
 * @param cb pointer to circular buffer.
 * @retval true if buffer count is same as max size.
 * @retval false if buffer count is not the same as max size.
 */
bool isFull(CircularBuffer *cb) {
  // Return true if size equals capacity.
  // Return false otherwise.
  return cb->count == cb->max_size;
}

/**
 * Name: isEmpty
 * @brief Demo Task 5.2: Circular Buffer
 * @details Checks if buffer is empty.
 * @param cb pointer to circular buffer.
 * @retval true if buffer count is 0.
 * @retval false if buffer count is not 0.
 */
bool isEmpty(CircularBuffer *cb) {
  // Return true if size is zero.
  // Return false otherwise.
  return cb->count == 0;
}

/**
 * Name: writeBuffer
 * @brief Demo Task 5.2: Circular Buffer
 * @details Writes a value to buffer if not full. Else throws an error.
 * @param cb pointer to circular buffer to be written to.
 * @param value value to be written.
 */
void writeBuffer(CircularBuffer *cb, int value) {
  //If the buffer is full, do not write. Throw an error.
  // Otherwise:
  // Write value at head.
  // Advance head (wrap if needed).
  // Increment size of the buffer.
  if(!isFull(cb)){
    cb->buffer[cb->head] = value;
    if(cb->head == cb->max_size - 1) {
      cb->head = 0;
    } else {
      cb->head++;
    }
    cb->count++;

    printString("Write: ");
    printInt(value);
    printString("\n");
  } else {
    printString("Buffer is Full. Unable to write more.\n");
  }
}

/**
 * Name: readBuffer
 * @brief Demo Task 5.2: Circular Buffer
 * @details Returns value at tail.
 * @param cb pointer to circular buffer.
 * @retval -1 if empty.
 * @retval value at tail. 
 */
int readBuffer(CircularBuffer *cb) {
  // If buffer is empty, return -1.
  // Otherwise:
  // Read value at tail.
  // Return value.
  if(isEmpty(cb)) return -1;
  printString("Read -> ");
  printInt(cb->buffer[cb->tail]);
  printString("\n");
  return cb->buffer[cb->tail];
}

/**
 * Name: popBuffer
 * @brief Demo Task 5.2: Circular Buffer
 * @details Returns value at tail. increments tail, wraps tail around if past max size.
 * @param cb pointer to circular buffer.
 * @retval -1 if empty.
 * @retval value at tail. 
 */
int popBuffer(CircularBuffer *cb) {
  // If buffer is empty, return -1.
  // Read value at tail.
  // Advance tail, reduce size.
  // Return value.
  if(isEmpty(cb)) {
    printString("Pop ");
    printInt(-1);
    printString("\n");
    return -1;
  }
  
  int value = cb->buffer[cb->tail];
  if(cb->tail == cb->max_size - 1) {
    cb->tail = 0;
  } else {
    cb->tail++;
  }
  
  cb->count--;

  //Documentation
  printString("Pop ");
  printInt(value);
  printString(". Tail at: ");
  printInt(cb->tail);
  printString("\n");

  //return
  return value;
}

/**
 * Name: pushBuffer
 * @brief Demo Task 5.2: Circular Buffer
 * @details adds value at head. Increments head. If tail is ahead of head, increment tail as well.
 * @param cb pointer to circular buffer.
 * @param value value to be pushed to the buffer. 
 */
void pushBuffer(CircularBuffer *cb, int value) {
  // If buffer is full:
  // Overwrite at tail and advance tail.
  // Write at head.
  // Advance head.
  // If not full, increment size.
  printString("Push ");
  printInt(value);
  printString("\n");

  if(!isFull(cb)){
    cb->count++;
  } else {
    printString("Overwrote Tail. Was full. Initial at ");
    printInt(cb->tail);
    printString(". Tail is now at ");
    if(cb->tail == cb->max_size) {
      cb->tail = 0;
    } else {
      cb->tail++;
    }
    printInt(cb->tail);
    printString("\n");
  }
  
  printString("Head rewritten at ");
  printInt(cb->head);
  printString(". New Head at ");
  cb->buffer[cb->head] = value;
  if(cb->head == cb->max_size - 1) {
    cb->head = 0;
  } else {
    cb->head++;
  }  
  printInt(cb->head);
  printString("\n");
}

/**
 * Name: resizeBuffer
 * @brief Demo Task 5.2: Circular Buffer
 * @details resizes buffer, keeping FIFO order
 * @param cb pointer to circular buffer.
 * @param new_size Size that is larger than the original size of the cb. 
 */
void resizeBuffer(CircularBuffer *cb, size_t new_size) {
  // Allocate a new array with the new capacity.
  // Copy the old values (in correct FIFO order) into the new buffer.
  // Free the old buffer.
  // Update buffer pointers and capacity.
  printString("Attempting resize from ");
  printInt(cb->max_size);
  printString(" to ");
  printInt(new_size);
  printString("\n");

  if(new_size < cb->max_size) {
    printString("New size cannot be smaller than the old size. Keeping things the same.\n");
    return;
  }
  
  int* buffer2 = (int *)malloc(new_size * sizeof(int));
  for (int i = 0; i < cb->count; i++){
    buffer2[i] = cb->buffer[(cb->tail + i) % cb->max_size];
  }

  free(cb->buffer);
  cb->buffer = buffer2;
  cb->head = cb->count;
  cb->tail = 0;
  cb->max_size = new_size;
}

/**
 * @}
 */

/**
 * Name: simulateSensorData
 * @brief Demo Task 6: Buffer Operations (NOT USED)
 * @details Observed Behavior: Reads LDR sensor values at 2 Hz and computes averaged brightness at 1 Hz.
 *      Commented Purpose: Integrates sensor input, buffer management, and real-time processing.
 *      Edge Case Handling: Ensures LED brightness updates only on valid data.
 *      Error Handling: Assumes circular buffer and dynamic array are initialized.
 * @param cb pointer to circular buffer.
 * @param processedData pointer to Dynamic Array. 
 */
void simulateSensorData(CircularBuffer *cb, DynamicArray *processedData) {
  //This function prototype is predefined. You are allowed to modify the function as needed.
  //Note that the functionality of this task is implemented in the loop function. 

  static uint32_t AVG_timer = 0; //basic timer
  static uint32_t LEDR_timer = 0; //basic timer
  uint32_t curr_time = *(volatile uint32_t *) TIMG_T0LO_REG(0);

  // 2. Inside the main loop:
  //    a. Check if 500 milliseconds have passed since the last sample:
  //       - If yes:
  //          • Read the current value from the LDR sensor.
  //          • Store this value into the circular buffer.
  //          • Reset the 500 ms timer.
  if(curr_time - LEDR_timer > 1000000/2) {
    writeBuffer(cb, analogRead(LEDR));
    LEDR_timer = curr_time;
  }

  //    b. Check if 2.5 seconds have passed since the last average:
  //       - If yes:
  //          • Compute the average of the values currently in the circular buffer.
  //          • Store this average in the dynamic array.
  //          • Print the contents of the dynamic array to the serial monitor.
  //          • Clear the Circular buffer 
  //          • Reset the 2500 ms timer.
  if(curr_time - AVG_timer > 1000000 * 5 / 2) {
    int i = 0; 
    while(!isEmpty(cb)) {
      i += popBuffer(cb);
    }
    i /= 5;
    
    // printString("Current avg circular buffer: ");
    // printInt(i);
    // printString("\n");

    addElement(processedData, i);
    printArray(processedData);
    AVG_timer = curr_time;
  }
}

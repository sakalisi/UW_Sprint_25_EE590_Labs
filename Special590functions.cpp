/**
 * @file Special590functions
 *
 * @mainpage print functions for Lab 3
 *
 * @section description Description
 * This cpp file holds all print functions made
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
#include "Special590functions.h"
#include <Arduino.h>
#include <cmath>

// =========== DEFINES ===========

#define BUFFER_SIZE 128          // Define buffer size for all strings

// =========== GLOBALS ===========
char stringBuffer[BUFFER_SIZE];  // Declare a global buffer for strings


// =========== FUNCTIONS ===========

/**
 * Name: printChar
 * @brief Task 1 step 1: Print Char pointed to by c.
 * @details prints character to the serial pointed to by c.
 * @param c pointer pointing to a character to be printed.
 */
void printChar(const char *c) {
  Serial.print(*c);
}

/**
 * Name: printString
 * @brief Task 1 step 2: Print str.
 * @details prints all characters until a new line is detected.
 * @param str pointer pointing to a list of chars to be printed.
 */
void printString(const char *str) {
  //task 1 step 2
  // This is a fully completed function for you to study.

  // Iterate through the memory locations of the input string
  char *dest = stringBuffer;  // Pointer to the destination buffer
  const char *src = str;      // Pointer to the source string

  // Copy characters from src to dest until null terminator or buffer limit
  while (*src != '\0' && (dest - stringBuffer) < BUFFER_SIZE - 1) {
    *dest = *src;     // Copy character
    printChar(dest);  // Print the copied character
    if (*dest == '\n') {
      break;  // Stop at newline character
    }
    src++;   // Move source pointer to next character
    dest++;  // Move destination pointer to next position
  }

  // Null terminate the buffer
  *dest = '\0';

  // Reset the buffer (clear all data)
  for (int i = 0; i < BUFFER_SIZE; i++) {
    stringBuffer[i] = '\0';
  }
}

/**
 * Name: printFloat
 * @brief Task 1 step 3: Print str.
 * @details prints all characters in a float until teh 2nd decimal place.
 * @param f float to be printed.
 */
void printFloat(float f) {
  // task 1 step 3: Print the given float value 'f'
  // 'f' can be upto 8 digits long
  // 'f' should always be printed out to 2 decimal places
  int n;
  char buffer [8];
  n = sprintf(buffer, "%0.2f", f);
  printString(buffer);
}

/**
 * Name: printInt
 * @brief Helper function I created to print ints faster.
 * @details Prints all ints digit by digit.
 * @param num number to print. Assumes that all nums are valid numbers.
 */
void printInt(int num) {
    // Handle negative numbers
    if (num < 0) {
        printChar("-");
        num = -num;
    }

    // Convert integer to string using recursion
    if (num / 10 != 0) {
        printInt(num / 10);  // print leading digits
    }

    char digit = (num % 10) + '0';
    printChar(&digit);         // print current digit
}

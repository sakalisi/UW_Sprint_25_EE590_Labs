/**
 * @file EE_590_Lab4LCD.ino
 *
 * @mainpage EE 590 Lab 4 LCD
 *
 * @section description Description
 * This sketch holds the implementation corresponding to the Lab document for task 1.
 * Lab 4 arduino ESP 32 File.
 *
 * @section circuit Circuit
 * - LCD connected to SDA an SCL at pin 20 and 21.
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

// ========== Libraries =========== //
#include <Wire.h> ///< Required for I2C communication

// ========== DEFINEs =========== //
#define I2C_ADDR      0x27 ///< I2C address of the LCD (apparently can be from 0x20 to 0x27)
#define LCD_BACKLIGHT 0x08 ///< LCD backlight control bit (enabling on, iirc)
#define ENABLE        0x04 ///< Enable datalatching bit (will be used for latching via pulse enable)
#define RS            0x01 ///< Register select bit  (0 = command, 1 = data)

// ========== CONSTS and GLOBALs =========== //

int cursorCol = 0; ///< Current Column Position of LED Cursor
int cursorRow = 0; ///< Current Row Position of LED Cursor

// ========== HELPER FUNCTIONs =========== //

/**
 * @name I2C LCD Transmission
 * @{
 */

/**
 * Name: i2cSend
 * @brief Sends a byte to the LCD via I2C with the backlight bit set if enabled.
 * @param data The byte to send to the LCD, part nibble, part information
 */
void i2cSend(uint8_t data) {
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(data | LCD_BACKLIGHT);
  Wire.endTransmission();
}

/**
 * Name: pulseEnable
 * @brief Pulses the enable pin to latch data into the LCD.
 * @param data The byte to latch with ENABLE bit toggled.
 */
void pulseEnable(uint8_t data) {
  i2cSend(data | ENABLE);
  delayMicroseconds(1); 
  i2cSend(data & ~ENABLE);
  delayMicroseconds(1);
}

/**
 * Name: sendNibble
 * @brief Sends a 4-bit nibble to the LCD.
 * @param nibble The upper or lower 4 bits of the data/command.
 * @param mode command mode or data mode, 0 for command, RS for data.
 */
void sendNibble(uint8_t nibble, uint8_t mode) {
  uint8_t data = (nibble << 4) | mode;
  i2cSend(data);
  pulseEnable(data);
}

/**
 * Name: sendByte
 * @brief Sends a full 8-bit byte to the LCD by splitting it into two nibbles.
 * @param value value to send.
 * @param mode command mode or data mode, 0 for command, RS for data.
 */
void sendByte(uint8_t value, uint8_t mode) {
  sendNibble(value >> 4, mode);
  delayMicroseconds(50); // Hope I am able to use delays here to pause, between sending nibbles.
  sendNibble(value & 0x0F, mode);
  delayMicroseconds(50); // delays again
}

/**
 * Name: lcdCommand
 * @brief Sends a command byte to the LCD via the sendByte Function.
 * Is different from the lcd Write char because of RS
 * @param cmd The command byte.
 */
void lcdCommand(uint8_t cmd) {
  sendByte(cmd, 0);
}

/**
 * Name: lcdWriteChar
 * @brief Writes a single character to the LCD at the current cursor location.
 * Is different from the lcd Command because of RS
 * @param ch The character to display.
 */
void lcdWriteChar(char ch) {
  sendByte(ch, RS);
}

/**
 * Name: lcdSetCursor
 * @brief Sets the cursor to the specified column and row. Used for wrapping and initial setup
 * @param col column of cursor location. (min 0, max 15)
 * @param row row of cursor location. (min 0, max 1)
 */
void lcdSetCursor(int col, int row) {
  static const uint8_t row_offsets[] = {0x00, 0x40};
  lcdCommand(0x80 | (col + row_offsets[row]));
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
 * Name: setup
 * @brief setup all pins - initializes the LCD and Serial monitor.
 */
void setup() {
  Wire.begin(20, 21);
  delay(50); // delay used to ensure wire is initialized as needed. Online says is useful for this particular LCD

  // found online that this triple-write is specified as required. Not sure as to why though. Maybe takes some time to load? 
  // I initially tried with just one, (the first one). didnt work
  // Tried just 4 bit, didnt work. Interesting
  sendNibble(0x03, 0); delay(5); //8-bit mode
  sendNibble(0x03, 0); delayMicroseconds(150); //reinforces 8-bit
  sendNibble(0x03, 0); delayMicroseconds(150); //absolutely ensures 8-bit
  sendNibble(0x02, 0); // 4-bit mode

  lcdCommand(0x28); // Function set
  lcdCommand(0x0C); // Display ON
  lcdCommand(0x06); // Entry mode
  lcdCommand(0x01); // should clear
  delay(2); 
  cursorCol = 0;
  cursorRow = 0;
  lcdSetCursor(cursorCol, cursorRow);
  Serial.begin(9600);
}

/**
 * Name: loop
 * @brief  loop to be run repeatedly. Equivalent to running everything in main whith a while(1).
 * @details Reads characters from Serial and displays them on the LCD.
 *        Handles newline and wrapping between two rows.
 */
void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      cursorCol = 0;
      cursorRow = (cursorRow + 1) % 2;
      lcdSetCursor(cursorCol, cursorRow);
    } else if (c == '\r' or c == '\t') {
      // ignore
    } else {
      lcdWriteChar(c);
      cursorCol++;
      if (cursorCol >= 16) {
        cursorCol = 0;
        cursorRow = (cursorRow + 1) % 2;
        lcdSetCursor(cursorCol, cursorRow);
      }
    }
  }
}

/**
 * @}
 */
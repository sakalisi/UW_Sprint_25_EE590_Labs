/**
 * @file EE_590_Lab4_BLE.ino
 *
 * @mainpage EE 590 Lab 4 BLE
 *
 * @section description Description
 * This sketch holds the implementation corresponding to the Lab document for task 3.
 * Lab 4 arduino ESP 32 File.
 *
 * @section circuit Circuit
 * - LCD connected to SDA an SCL at pin 20 and 21.
 * - Button connected to pin 13
 *
 * @section libraries Libraries
 * - arduino-esp32 (https://github.com/espressif/arduino-esp32)
 * - Arduino BLE
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
#include <BLE2902.h>            ///< Required for BLE handling which Device, Utils and Server file to use, since i have a few files called such, requires to be first
#include <BLEDevice.h>          ///< Required for BLE Device handling
#include <BLEUtils.h>           ///< Required for BLE
#include <BLEServer.h>          ///< Required for BLE Server Handling
#include "Wire.h"               ///< Required for I2C
#include <LiquidCrystal_I2C.h>  ///< Required for quick LCD handling

// ========== CONSTS and DEFINEs =========== //
// Generate random Service and Characteristic UUIDs: https://www.uuidgenerator.net/

#define SERVICE_UUID        "abac6fe4-7fce-47a5-bc1b-85f92ecd786b" ///< Service UUID
#define CHARACTERISTIC_UUID "d7b62563-544f-4e33-9251-f29577256ead" ///< Characteristic UUID
#define BUTTON_PIN 13                                              ///< Push Button Pin

// ========== GLOBALs =========== //
LiquidCrystal_I2C lcd(0x27, 16, 2); ///< LCD Pre-initialization

volatile bool newMessageReceived = false; //< Checks if new message has been received from the BLE
hw_timer_t * timer = NULL; ///< timer pre-initialized to null. Will be updated in setup
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; ///< Assuming we need a mutex for timer stuff. Asked Gokul. Said should be fine

volatile unsigned long counter = 0;     ///< current count
volatile bool updateDisplay = true;     ///< whether to update the display, flag
volatile bool buttonInterrupt = false;  ///< whether an interrupt took place with regard to buttons, flag
volatile bool bleInterrupt = false;     ///< whether an interrupt took place with regard to ble, flag

// ========== ISRs =========== //
// ==============> TODO: Write your timer ISR here.

/**
 * Name: onTimer
 * @brief ISR to handle counter update on timer interrupt call
 * @details enters mutex, ensures counter cannot be updated by anything else. updates it and then exits.
 */
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  if (updateDisplay) counter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

// ==============> TODO: Create an ISR function to handle button press here.
/**
 * Name: onButtonPress
 * @brief ISR to handle button press flag setting on button interrupt call
 */
void IRAM_ATTR onButtonPress() {
  buttonInterrupt = true;
}

// ========== CLASSs =========== //
/**
 * Name: onWrite
 * @brief Callback from BLE if written, will set flag to true. Used as ISR
 */
class MyCallbacks: public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *pCharacteristic) {
     // =========> TODO: This callback function will be invoked when signal is
     // 		     received over BLE. Implement the necessary functionality that
     //		     will trigger the message to the LCD.
     bleInterrupt = true;
   }
};

/**
 * Name: setup
 * @brief sets up bluetooth, LCD, timer and button
 * @details sets up bluetooth with given name, as expected in the Lab 4 documentation 
 */
void setup() {
  BLEDevice::init("SJK_ESP32");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

   // =========> TODO: Initialize LCD display
  Wire.begin(20, 21);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);

   // =========> TODO: create a timer, attach an interrupt, set an alarm which will
   //                  update the counter every second.
  timer = timerBegin(100000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 100000, true, 0);

    // ========> TODO: Set button pin as input and attach an interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);
}

/**
 * Name: loop
 * @brief Equivalent to while (1) in main. Checks three things each time, if it needs to update, if it has been interrupted via BLE, or via Button Press
 */
void loop() {
 // =========> TODO: Print out an incrementing counter to the LCD.
 //                  If a signal has been received over BLE, print out “New
 //                  Message!” on the LCD.
 //                  If the button has been pressed, print out "Button Pressed"
 //                  on the LCD.
 static unsigned long lastDisplayed = -1;

  // Display counter if changed
  if (updateDisplay && counter != lastDisplayed) {
    lastDisplayed = counter;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Count: ");
    lcd.setCursor(7, 0);
    lcd.print(counter);
  }

  // Handle button press
  if (buttonInterrupt) {
    updateDisplay = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button Pressed");
    delay(2000);
    buttonInterrupt = false;
    updateDisplay = true;
    lastDisplayed = -1; // Force update
  }

  // Handle BLE message
  if (bleInterrupt) {
    updateDisplay = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("New Message!");
    delay(2000);
    bleInterrupt = false;
    updateDisplay = true;
    lastDisplayed = -1; // Force update
  }

  delayMicroseconds(1000);  // Small delay to reduce CPU load
}

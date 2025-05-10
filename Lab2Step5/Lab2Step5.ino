// FileName: Lab2Step5.ino
// Author: Sai Jayanth Kalisi
// Date: 04/26/2025
// Description: This ino file is created for dimming an external LED 
    // based on inputs/light exposure to the photoresistor

// =============== INCLUDES ===============

#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"
#include "soc/timer_group_reg.h"

// ================ MACROS ================

#define LED_PIN 1 //output led
#define LEDR_PIN 10 //input photoresistor

// ======= Function IMPLEMENTATIONS =======

// Name: setup
// Description: setup pins and serial for debugging purposes
void setup() {
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED_PIN], PIN_FUNC_GPIO); // select pin as GPIO
  *(volatile uint32_t *) GPIO_ENABLE_REG |= (1 << LED_PIN);  // enable pin
  *(volatile uint32_t *) GPIO_OUT_REG &= ~(1 << LED_PIN);    // set initial state for led as off

  // attach an LED to be controlled at a resolution of 11 and freq of 100Hz
  ledcAttach(LED_PIN, 100, 11); 

  Serial.begin(115200);
}

// Name: loop
// Description: loop is equivalent to while(1), for(;;) or loop based gotos in main
// Expected behavior: every cycle, photoresistor is read via analog read, 
    // and the duty cycle of the LED is modified based on the read value
    // read value is then printed to serial.
void loop() {
  // put your main code here, to run repeatedly:
  ledcWrite(LED_PIN, analogRead(LEDR_PIN));
  Serial.println(analogRead(LEDR_PIN));
}

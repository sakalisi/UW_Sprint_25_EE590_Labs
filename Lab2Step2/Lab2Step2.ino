// FileName: Lab2Step2.ino
// Author: Sai Jayanth Kalisi
// Date: 04/26/2025
// Description: This ino file is created for blinking an external LED 
    // using registers

// =============== INCLUDES ===============

#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"

// ================ MACROS ================

#define LED_PIN 1
//Above marks LED pin used

// ======= Function IMPLEMENTATIONS =======

// Name: setup
// Description: setup pins and serial for debugging purposes
void setup() {
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED_PIN], PIN_FUNC_GPIO); // select pin as GPIO
  *(volatile uint32_t *) GPIO_ENABLE_REG |= (1 << LED_PIN);  // enable pin
  
  Serial.begin(115200);
}

// Name: loop
// Description: loop is equivalent to while(1), for(;;) or loop based gotos in main
// Expected behavior: Switches LED on and off every 1 second
void loop() {
  //on and wait
  *(volatile uint32_t *) GPIO_OUT_REG |= (1 << LED_PIN);
  delay(1000);

  //write outputted to serial monitor
  // Serial.write("Outputted \n");
  
  //switch off and wait
  *(volatile uint32_t *) GPIO_OUT_REG &= (0 << LED_PIN);
  delay(1000);
}

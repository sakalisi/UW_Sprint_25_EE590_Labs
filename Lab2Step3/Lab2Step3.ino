// FileName: Lab2Step3.ino
// Author: Sai Jayanth Kalisi
// Date: 04/26/2025
// Description: This ino file is created to compare 
    // for blinking an external LED 
    // using registers and arduino defined function/macros

// =============== INCLUDES ===============

#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"

// ================ MACROS ================

#define LED_PIN 1
//Above marks LED pin used

// =========== GLOBAL VARIABLES ===========

unsigned long timeTotal; 
//time Total variable used to keep track of additive time

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
// Expected Behavior: every loop, 1000 iterations of on and off are done for DMA and 
    // arduino built in macros for digital write. Total time for all iterations of each
    // type is printed. A delay for 1 second is implemented at the end of this print.
void loop() {
    //resetting timeTotal every loop
    timeTotal = 0;
  
    //looping 1000 times for direct register access
    for(int i = 0; i < 1000; i++){
        unsigned long time1 = micros(); //start time 
        *(volatile uint32_t *) GPIO_OUT_REG |= (1 << LED_PIN); //on
        *(volatile uint32_t *) GPIO_OUT_REG &= (0 << LED_PIN); //off
        timeTotal += micros() - time1; //adding to total time
    }

    Serial.print("Register Time: ");
    Serial.println(timeTotal); //printing total time taken for register
    timeTotal = 0;

    for(int i = 0; i < 1000; i++){
        unsigned long time2 = micros(); //start time
        digitalWrite(LED_PIN, HIGH); //on
        digitalWrite(LED_PIN, LOW); //off
        timeTotal += micros() - time2; //adding to total time
    }
    Serial.print("Macro Time: ");
    Serial.println(timeTotal); //printing toral time taken for macro
    
    delay(1000);
}

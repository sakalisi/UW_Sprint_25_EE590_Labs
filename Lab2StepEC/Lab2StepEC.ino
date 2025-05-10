// FileName: Lab2StepEC.ino
// Author: Sai Jayanth Kalisi
// Date: 04/26/2025
// Description: This ino file is created for modifying an external LED 
    // output frequency based on inputs/light exposure to the photoresistor

// =============== INCLUDES ===============

#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"
#include "soc/timer_group_reg.h"

// ================ MACROS ================

#define LED_PIN 1 //output LED
#define LEDR_PIN 10 //photoresistor input
#define TIME_FREQ 80000000 //timer frequency

// =========== GLOBAL VARIABLES ===========
uint32_t timer_val;
bool currentState = true; //not required if using xor
int freqMod; //modifier is changed depending on photoresistor light exposure

// ======= Function IMPLEMENTATIONS =======

// Name: start_timer
// Description: Setting up timer
// Expected Behavior: a 80 MHz timer
void start_timer() {
  *(volatile uint32_t *) TIMG_T0CONFIG_REG(0) = 0xE0002000;  
}

// Name: setup
// Description: setup pins and serial for debugging purposes. Timer is also set_up
void setup() {
  // put your setup code here, to run once:
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED_PIN], PIN_FUNC_GPIO); // DEFINING LED_PIN as a GPIO PIN
  *(volatile uint32_t *) GPIO_ENABLE_REG |= (1 << LED_PIN); // Enabling pin
  *(volatile uint32_t *) GPIO_OUT_REG &= ~(1 << LED_PIN); // Switching off in the beginning

  //talk about how to configure pins for input
  start_timer();
  timer_val = *(volatile uint32_t *) TIMG_T0LO_REG(0); //start val for timer set up

  Serial.begin(115200);
}

// Name: loop
// Description: loop is equivalent to while(1), for(;;) or loop based gotos in main
// Expected behavior: every cycle, photoresistor is read via analog read, 
    // and the LED frequency is modified based on the read value. based on thresholds 
    // met (200, 500, 900) of analog read values, the led period is (off, 0.5hz, 1 hz and 2 hz)
    // read value range requivalent is printed to serial
void loop() {
  *(volatile uint32_t *) TIMG_T0UPDATE_REG(0) = 1; // Latch timer value
  uint32_t timer_val2 = *(volatile uint32_t *) TIMG_T0LO_REG(0); //read timer val
  int currIntensity = analogRead(LEDR_PIN); //photo resistor input val
  
  // off condition
  if(currIntensity < 200) {
    *(volatile uint32_t *) GPIO_OUT_REG &= ~(1 << LED_PIN);
    Serial.println("OFF");
  } else {
    if (currIntensity < 500) { //low frequency condition
      freqMod = 1;
      Serial.println("LOW");
    } else if (currIntensity < 900) { //mid frequency condition
      freqMod = 2;
      Serial.println("MID");
    } else { //high frequency condition
      freqMod = 4;
      Serial.println("HIGH");
    }

    //based on condition ser, time is checked. If time check passes period requirements
    //light output is swapped from off to on and viceversa
    if(timer_val2 - timer_val > TIME_FREQ/freqMod) {
      if(currentState){
        *(volatile uint32_t *) GPIO_OUT_REG |= (1 << LED_PIN); //on
      }
      else {
        *(volatile uint32_t *) GPIO_OUT_REG &= ~(1 << LED_PIN); //off
      }
      currentState = !currentState; //swap state
      timer_val = timer_val2;
    }
  } 
}

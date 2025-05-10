// FileName: Lab2Step4.ino
// Author: Sai Jayanth Kalisi
// Date: 04/26/2025
// Description: This ino file is created for blinking an external LED 
    // using registers and register timers

// =============== INCLUDES ===============

#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"
#include "soc/timer_group_reg.h"

// ================ MACROS ================

#define LED_PIN 1 //output pin to LED
#define TIME_DIV 80 //dividing the 80Mhz to 1Mhz
#define TIME_EN (1<<31) //enabling
#define TIME_INCREMENT_MODE (1<<30) //choosing to increment
#define COUNT 1000000 //num cycles to count to

// ======= Function IMPLEMENTATIONS =======

// Name: start_timer
// Description: Setting up timer
// Expected Behavior: a 1 MHz timer
void start_timer() {
  //uint which holds enable timer and increment mode, as well as divider
  uint32_t timer_config = TIME_EN | TIME_INCREMENT_MODE | (TIME_DIV << 13);

  //Writing config into timer register
  *(volatile uint32_t *) TIMG_T0CONFIG_REG(0) = timer_config;  
}

// Name: setup
// Description: setup pins and serial for debugging purposes. Timer is also set_up
void setup() {
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED_PIN], PIN_FUNC_GPIO); // DEFINING LED_PIN as a GPIO PIN
  *(volatile uint32_t *) GPIO_ENABLE_REG |= (1 << LED_PIN); // Enabling pin
  *(volatile uint32_t *) GPIO_OUT_REG &= ~(1 << LED_PIN); // Switching off in the beginning

  start_timer();
  Serial.begin(115200);
}

// Name: loop
// Description: loop is equivalent to while(1), for(;;) or loop based gotos in main
// Expected behavior: Switches LED on and off every 1 second
void loop() {
  *((volatile uint32_t *) TIMG_T0UPDATE_REG(0)) = 1; // Latch timer value

  static uint32_t timer_val = 0; //basic timer
  uint32_t timer_val2 = *(volatile uint32_t *) TIMG_T0LO_REG(0); //update lo reg of timer 0

  if(timer_val2 - timer_val >= COUNT) { // checks if 1 second has passed
    //swap bit in use, if on makes off
    *(volatile uint32_t *) GPIO_OUT_REG ^= (1 << LED_PIN);
    // write outputted to serial monitor

    // Serial.println("Outputted \n");
    timer_val = timer_val2; //reset timer after condition has been met
  }
}

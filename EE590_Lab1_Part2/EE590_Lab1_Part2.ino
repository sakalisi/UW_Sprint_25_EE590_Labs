#define LED_PIN1 1
#define LED_PIN2 14
//Above lists multiple LED Pins used

/*
  Created by Sai Jayanth Kalisi

  Multiple blinking LEDs -> EE 590 Lab 1
*/

//setting up LEDs and Serial for debugging purposes
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  Serial.begin(115200);
}

//timers
int i = 0;
int j = 0;

//loop is equivalent to while(1), for(;;), or loop based gotos in main
void loop() {
  // put your main code here, to run repeatedly:

  //increment timers
  i = (i + 1) % 200;
  j = (j + 1) % 300;
  
  //handling LED1
  if(i == 100) {
    digitalWrite(LED_PIN1, HIGH);
  } else if (i == 0) {
    digitalWrite(LED_PIN1, LOW);
  }

  //handling LED2
  if(j == 150) {
    digitalWrite(LED_PIN2, HIGH);
  } else if (j == 0) {
    digitalWrite(LED_PIN2, LOW);
  }

  //handling actual wait for timer increment
  delay(1);
}

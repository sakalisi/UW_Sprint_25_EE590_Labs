#define LED_PIN1 1
#define LED_PIN2 19
#define BUTTON_PIN1 13
#define BUTTON_PIN2 12
//Above define the pins in use for the extra credit problem

/*
Created by Sai Jayanth Kalisi

Extra credit component for the Lab 1 EE 590
*/

/*
Setup for GPIO 
Buttons are marked as pull ups as recommended by the lab documentation
Serial is at 115200 baud as per standard
*/
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN1, OUTPUT);
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  Serial.begin(115200);
}

//Keeping track of state and frequency of each blink
bool blink1 = true;
bool blink2 = true;
int blink1Freq = 1000;
int blink2Freq = 500;

//Time keeping
int i = 0;
int j = 0;

//letting the compiler know that this prevButton value can change repeatedly
// not sure if is necessary, but putting it down just in case
volatile int prevButton = HIGH;

/*
Handling states -> state is true if blinking, false if not;
*/
bool StateMachineBlinks(uint8_t button, bool state) {
  // Ensuring the swap happens only once per button press
  if(digitalRead(button) == LOW && prevButton == HIGH){
    prevButton = LOW;
    return !state;
  }
  prevButton = HIGH;
  return state;
}

/* 
Handling actual blinking of given LED 
based on input state, time and frequency
*/
void handleLED(uint8_t LED, bool state, int time, int freq) {
  if(!state) {
    digitalWrite(LED, LOW);
  }
  else if (i % freq > freq/2) {
    digitalWrite(LED, HIGH);
  }
  else if (i % freq <= freq/2) {
    digitalWrite(LED, LOW);
  }
}

/* 
Loop occurs repeatedly, alternate to while(1), for(;;) or looping gotos in main
*/
void loop() {
  // put your main code here, to run repeatedly:

  //Handling blinking for both LEDs based on both buttons
  blink1 = StateMachineBlinks(BUTTON_PIN1, blink1);
  blink2 = StateMachineBlinks(BUTTON_PIN2, blink2);

  //printing blink state for posterity
  // Serial.println(blink1);
  // Serial.println(blink2);
  // Serial.println();

  //updating timer
  i = (i + 1) % blink1Freq;
  j = (j + 1) % blink2Freq;

  // LED control
  handleLED(LED_PIN1, blink1, i, blink1Freq);
  handleLED(LED_PIN2, blink2, j, blink2Freq);

  //delaying for each clock tick. 
  delay(1);
}

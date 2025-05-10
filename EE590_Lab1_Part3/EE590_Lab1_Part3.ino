#define LED_PIN1 1
#define BUTTON_PIN 13
//Above defining GPIO for input and output

/*
  Created By Sai Jayanth Kalisi

  Button controlled LED -> EE 590 Lab 1
*/

//setupGPIO pins based pin numbers mentioned above
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN1, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

/*
Loop is an equivalent to while(1), for(;;) or looped based gotos
*/
void loop() {
  // put your main code here, to run repeatedly:

  //Tried alternative. 
  // digitalWrite(LED_PIN1, !digitalRead(BUTTON_PIN));
  
  //if button pressed, put LED on, else off
  if(digitalRead(BUTTON_PIN) == HIGH){
    digitalWrite(LED_PIN1, LOW);
  }
  else {
    digitalWrite(LED_PIN1, HIGH);
  }

  delay(1);

}

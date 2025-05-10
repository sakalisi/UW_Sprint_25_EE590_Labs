#define LED_PIN 1
//Above marks LED pin used

/*
  Created By Sai Jayanth Kalisi

  Blinking LED -> Lab 1 EE 590
*/

//setup pins and serial for debugging purposes
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

//loop is equivalent to while(1), for(;;) or loop based gotos in main
void loop() {
  // put your main code here, to run repeatedly:

  //on
  digitalWrite(LED_PIN, HIGH);
  //wait 200 ms
  delay(200);
  //write outputted to serial monitor
  Serial.write("Outputted \n");
  //switch off
  digitalWrite(LED_PIN, LOW);
  //wait 200 ms
  delay(200);
}

int ledPins[] = {2,3,4,5,6,7,8,9};

void setup() {
  for (int index = 0; index < 8; index++){
    pinMode(ledPins[index],OUTPUT);
  }
}

// the loop function runs over and over again forever
void loop() {
  for (int ledIndex = 0; ledIndex < 8; ledIndex++){
    blink(ledIndex,250,250);
   

  }
   // blink(7,500,500);
}

void blink(int ledNumber, int onDelay, int offDelay){
  digitalWrite(ledPins[ledNumber], HIGH);
  delay(onDelay);
  digitalWrite(ledPins[ledNumber], LOW);
  delay(offDelay);
}

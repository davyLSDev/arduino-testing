void setup() {
  Serial.begin(9600);
}

void loop() {
  for (int index = 1; index < 25; index++){
    Serial.println(index);
    delay(250);
  }
}

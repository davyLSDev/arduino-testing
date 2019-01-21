// Drive 6 7-segment common anode displays using 
// a 74138 to select the digit
// a 7447 to take an input number to drive the segments
// a pnp transistor to provide enough current to light the LEDs
//
// 

int digitSelect[] = {4,3,2}; // Arduino pins 4,3,2 drive 74138 pins A,B,C                 
int digitNumberToBits[6][3] = { { 0,0,0 },
                                { 0,0,1 },
                                { 0,1,0 },
                                { 0,1,1 },
                                { 1,0,0 }, 
                                { 1,0,1 } };
int digitValue[] = {8,7,6,5}; // Arduino pins 8,7,6,5 drive 7447 pins D,C,B,A
int digitValueToBits[10][4] = { { 0,0,0,0 },
                                { 0,0,0,1 },
                                { 0,0,1,0 },
                                { 0,0,1,1 },
                                { 0,1,0,0 }, 
                                { 0,1,0,1 },
                                { 0,1,1,0 },
                                { 0,1,1,1 },
                                { 1,0,0,0 },
                                { 1,0,0,1 } };                               

void setup() {
  Serial.begin(9600);
  for (int digitSelectPin = 0; digitSelectPin < 3; digitSelectPin++){
    pinMode(digitSelect[digitSelectPin],OUTPUT);
    }
  for (int digitValuePin = 0; digitValuePin < 4; digitValuePin++){
    pinMode(digitValue[digitValuePin],OUTPUT);
    }  
}

void loop() {
  //Serial.println("loop going");
//  selectDigit(0);
//  troubleshoot();
//  writeDigit(0); // works
//  writeDigit(1); // works
//  writeDigit(2); // works
//  writeDigit(3); // works
//  writeDigit(4); // works
//  writeDigit(5); // works
//  writeDigit(6); // works
//  writeDigit(7); // works
//  writeDigit(8); // works
//  writeDigit(9); // works

  for (int digit = 0; digit < 6; digit++){
    selectDigit(digit);
    for (int number = 0; number < 10; number++){
      writeDigit(number);
      delay(200);
    }
  }  
  

}

void writeDigit(int digit){
  for (int valueBit = 0; valueBit < 4; valueBit++){
//    Serial.println(digit);
//    delay(300);
    digitalWrite(digitValue[valueBit],digitValueToBits[digit][valueBit]);
  }
}

void selectDigit(int digitNumber){
  for (int numberBit = 0; numberBit < 3; numberBit++){
    digitalWrite(digitSelect[numberBit],digitNumberToBits[digitNumber][numberBit]);
  }
}

void troubleshoot(){
/* digitValue[0] => Arduino pin 8
   digitValue[1] => Arduino pin 7
   digitValue[2] => Arduino pin 6
   digitValue[3] => Arduino pin 5 */

  digitalWrite(digitValue[3],LOW);
  delay(200);
  digitalWrite(digitValue[3],HIGH);
  delay(200);
}

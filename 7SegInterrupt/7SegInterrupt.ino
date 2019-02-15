/* References  
 *  1. https://www.hobbytronics.co.uk/arduino-timer-interrupts
 *  Example Timer1 Interrupt
 *  Flash LED every second
 *   
 *  2. https://www.instructables.com/id/Arduino-Timer-Interrupts/
 *  
 *  Calculation:
 *  CMR = [ 16,000,000Hz/ (prescaler * frequency) ] - 1
 *  where   CMR (compare match register)
 *          frequency (desired interrupt frequency) 
 *  Let's assume 30hz is fine for LED project, and select a 1024 prescalar, therefore
 *  CMR = 16,000,000/(1024 * 30) = 520.83, let's call it 520, 
 *  
 *  Now, check to see which timer could be used by: 256 < CMR < 65,536
 *  we see 256 < 520 < 65,536, therefore I must use timer1 for this interrupt.
*/

int dTest = 0;

// From the seven segment code ...
// Drive 6 7-segment common anode displays using 
// a 74138 to select the digit
// a 7447 to take an input number to drive the segments
// a pnp transistor to provide enough current to light the LEDs

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

long displayNumber;

void setup()
{

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 3;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts();             // enable all interrupts
  
// From the seven segment code ...
  Serial.begin(9600);
  for (int digitSelectPin = 0; digitSelectPin < 3; digitSelectPin++){
    pinMode(digitSelect[digitSelectPin],OUTPUT);
    }
  for (int digitValuePin = 0; digitValuePin < 4; digitValuePin++){
    pinMode(digitValue[digitValuePin],OUTPUT);
    }  
}

ISR(TIMER1_COMPA_vect)        // interrupt service routine 
{
    long number = displayNumber;
    int digitNumber = 0;
  do {
    int digit = (int) number % 10;
      selectDigit(digitNumber++);
      writeDigit(digit);
    number /= 10;
  } while (number > 0);
}

void loop()
{
  displayNumber=111111;
//  displayNumber = displayNumber++;
  Serial.println(displayNumber);
//   testAllDigits ();
}

// Functions for the seven segment code here ...

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

void writeNumber(int number){
//void writeNumber(int number){
//  int delayTime = 5;
  int digitNumber = 0;
  do {
    int digit = number % 10;
      selectDigit(digitNumber++);
      writeDigit(digit);
    number /= 10;
  } while (number > 0);
}

// Some troubleshooting code

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

void testAllDigits (){
   int timeDelay = 4;

   dTest = 0;
   while ( dTest < 6 ){
     selectDigit(dTest);
     writeDigit(dTest);
//     Serial.println(dTest);
     delay(timeDelay);
     dTest++;
   }
}

/*
 * Hello World!
 *
 * This is the Hello World! for Arduino. 
 * It shows how to send data to the computer
 */


void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);           // set up Serial library at 9600 bps
  
    // prints hello with ending line break 
}

void loop()                       // run over and over again
{
  Serial.println("Hello world!");
  delay(250);
  Serial.println("Goodbye world!");
  delay(500);
  Serial.println("This");
  delay(250);
  Serial.println("Seems to work!");
  delay(250);
}

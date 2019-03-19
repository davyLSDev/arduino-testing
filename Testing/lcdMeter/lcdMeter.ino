/* Sketch for creating an lcd panel meter
 * 
 */

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Software SPI (slower updates, more flexible pin options):
// Arduino pin 3 - Serial clock out (SCLK)
// Arduino pin 4 - Serial data out (DN)
// Arduino pin 5 - Data/Command select (D/C)
// Arduino pin 6 - LCD reset (RST)
// Arduino pin 7 - LCD chip enable (SCE)
Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 7, 6);

// Needed function
float mInv(float divider){
  float inverse = 1.0/divider;
  return inverse;
}

const int solarPanelInput = A0;
const int isoPot = A1;
const int timePot = A2;
const int aperaturePot = A3;
int isoValueTable[] = {-375,-75,15,3,6,12, \
   25,50,100,200,400,800,1600,3200};   
const int timeValueTable[] = {-4000,-2000,-1000, \
  -500,-250,-125,-60,-30,-15,-8,-4,-2, \
  1,2,4,8,15,30,60,120,240,3600,7200,14400,28800};
const int aperatureValueTable[] = { 1,2,3,4,6,8, \
  11,22,32,45,64,90,128,181,256,362,512,724,1024};
int solarPanelValue = 0;
float isoValue = 0;
float timeValue = 0;
float aperatureValue = 0;
int lcdContrast = 50;
int lastSolarPanelValue = -1;

String text = "LUX: ";
String text2 = "ISO: ";

//int potValue = 0;

void setup()                    
{
  Serial.begin(9600);
  
  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(lcdContrast);
  display.setTextSize(1);
  display.setRotation(2);
  display.clearDisplay();
}

void loop()                       
{
  display.display();
//  test();
  drawMeter("f4.5", "1/1000", 3200 );
  for (int i = 0; i < 101; i++){
    updateMeter (i);
    delay(300);
  }
  
}

int getSolarPanelReading(){
// Tweak these next values for light reading  
  const int minLight = 0;
  const int maxLight = 360;
  int lightReading = analogRead(solarPanelInput);
  int lightRange = map (lightReading, minLight, maxLight, 0, 7);
  return lightRange; 
}

int getPotValue(int potIdentity, int potMinValue, int potMaxValue, int potValueTable[]){
  int value = analogRead(potIdentity);
//  int arraySize = sizeof(potValueTable) - 1;
  int potRange = map (value, potMinValue, potMaxValue, 0, 13);
  int valueFromTable = potValueTable[potRange];
  return valueFromTable;
}

void updateInformation (int lightIntensity, int iso, int shutterTime, int aperature ){
  for (int blankLines = 0; blankLines < 18; blankLines++){
    Serial.println("");
    Serial.println("Light intensity is:");
    Serial.println(lightIntensity);
    Serial.println("iso is");
    Serial.println(iso);
    Serial.println("shutter speed is:");
    Serial.println(shutterTime);
    Serial.println("aperature is:");
    Serial.println(aperature);
  }
  delay(2000);
}

void stuffNnonsense(){
  //  display.display();
//  display.clearDisplay();
//  display.drawLine(1, 48, 48, 48, BLACK);
//  display.display();
//  delay(400);
//  
//  display.clearDisplay();
//  display.drawLine(1, 1, 48, 48, BLACK);
//  display.display();
//  delay(400);
//  
//  display.clearDisplay();
//  display.drawLine(84, 1, 48, 48, BLACK);
//  display.display();
//  delay(400);
//
//  display.clearDisplay();
//  display.drawLine(48, 84, 48, 48, BLACK);
//  display.display();
//  delay(400);
  
//  for (int testValue = 1 ; testValue < 100; testValue++){
//    updateMeter(testValue);
}

void test(){
  for (int x = 1; x < 85; x++){
    display.writePixel(x, 1, BLACK);
    display.display();
  }

  for (int y = 1; y < 24; y++){
    display.clearDisplay();
    delay (300);
    display.drawLine(1, y*2, 84, y*2, BLACK);
    display.display();
  }
}

void drawMeter(String fstop, String shutter, int iso){
  display.clearDisplay();
  
// lables
  display.setCursor(0, 0);
  display.println(fstop);
  display.setCursor(48, 0);
  display.println(shutter);
  display.setCursor(0, 40);
  display.println("ISO");
  display.setCursor(60, 40);
  display.println(String (iso, DEC));
  display.setCursor(28, 40);
  display.println("-");
  display.setCursor(52, 40);
  display.println("+");

// define the meter bottom
  display.drawFastHLine(0, 38, 34, BLACK);
  display.drawFastVLine(34, 38, 8, BLACK);
  display.drawFastHLine(50, 38, 34, BLACK);
  display.drawFastVLine(50, 38, 8, BLACK);
  
// the centre of the needle  
  display.fillCircle( 43, 37, 2, BLACK);
  display.display();
}

void updateMeter (int meterValue){
  int xInit = 43;
  int yInit = 37;
  int radius = 35;

  int x;
  int y;

  int angle = 1.8*meterValue;
  if (meterValue >90){
    x = abs(sin(angle))*radius;
  } else {
    x = abs(sin(angle))*radius;
  }
  
  y = abs(cos(angle))*radius;

  display.drawLine(xInit, yInit, x, y, BLACK);
  
//  switch (meterValue) {
//    case 1:
//      display.drawLine(xInit, yInit, 0, 37, BLACK);
//      break;
//    case 2:
//      display.drawLine(xInit, yInit, 43, 9, BLACK);
//      break;
//    case 3:
//      display.drawLine(xInit, yInit, 84, 37, BLACK);
//      break;
//    default:
//      display.drawLine(xInit, yInit, 43, 9, BLACK);;
//      break;
//  }
  display.display();
}

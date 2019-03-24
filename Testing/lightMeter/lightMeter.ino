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
  display.clearDisplay();
  display.setTextSize(1);
  display.setRotation(2);
//  display.display();
}

void loop()                       
{
  for (int testValue = 1 ; testValue < 85; testValue++){
    updateMeter(testValue);
    delay(400);
  }
    
/*  display.clearDisplay();
  display.drawLine(0, 10, 48, 48, BLACK);
  display.display();
  delay(2000);
  display.drawLine(0, 10, 0, 0, BLACK);
  
  display.display();*/
  
  solarPanelValue = getSolarPanelReading();
  isoValue = getPotValue(isoPot,0,1023,isoValueTable);
//  timeValue = getPotValue(timePot);
//  aperatureValue = getPotValue(aperaturePot);
//  updateInformation (solarPanelValue, isoValue, 100, 11); 
//  String updateMessage = text + solarPanelValue;
//  Serial.println(updateMessage);
//  updateMessage = text2 + isoValue;
//  Serial.println(updateMessage);

  
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  if (lastSolarPanelValue != solarPanelValue){
    display.setCursor(5,0);
    display.setTextColor(WHITE);
    display.println(lastSolarPanelValue);
    display.setTextColor(BLACK);
    display.println(solarPanelValue);
    lastSolarPanelValue = solarPanelValue;  
  }
  else {
  display.setCursor(0,0);
  display.println(text + solarPanelValue);
  lastSolarPanelValue = solarPanelValue;
  }
  display.setCursor(40,0);
  display.println(text2);
  delay(2000);
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

void updateMeter (int meterValue){
  display.clearDisplay();
  display.drawLine(meterValue/4, (meterValue/2), 48, 48, BLACK);
  display.display();
}

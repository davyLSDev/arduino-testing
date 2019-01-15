// Needed function
float mInv(float divider){
  float inverse = 1.0/divider;
  return inverse;
}

const int solarPanelInput = A0;
const int isoPot = A1;
const int timePot = A2;
const int aperaturePot = A3;
const int isoValueTable[] = {-375,-75,15,3,6,12, \
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

//int potValue = 0;

int ledPins[] = {2,3,4,5,6,7,8,9};
//String text = "Scaled light intensity => ";
//String text2 = "Pot value => ";
//String updateMessage = "";

void setup()                    
{
  Serial.begin(9600);
  for (int index = 0; index < 8; index++){
    pinMode(ledPins[index],OUTPUT);
  }         
}

void loop()                       
{
  blankAllLeds();
  solarPanelValue = getSolarPanelReading();
  isoValue = getPotValue(isoPot,0,1023,isoValueTable);
//  timeValue = getPotValue(timePot);
//  aperatureValue = getPotValue(aperaturePot);
  ledMeterOutput(solarPanelValue);
  updateInformation (solarPanelValue, isoValue, 100, 11); 
/*  String updateMessage = text + solarPanelValue;
  Serial.println(updateMessage);
  updateMessage = text2 + potValue;
  Serial.println(updateMessage);*/
}

void ledMeterOutput(int meterInput){
  switch (meterInput){
    case 0:
      digitalWrite(ledPins[0], HIGH);
      break;
    case 1:
      digitalWrite(ledPins[1], HIGH);
      break;
    case 2:
      digitalWrite(ledPins[2], HIGH);
      break;
    case 3:
      digitalWrite(ledPins[3], HIGH);
      digitalWrite(ledPins[4], HIGH);
      break;
    case 4:
      digitalWrite(ledPins[4], HIGH);
      break;
    case 5:
      digitalWrite(ledPins[5], HIGH);
      break;
    case 6:
      digitalWrite(ledPins[6], HIGH);
      break;
    case 7:
      digitalWrite(ledPins[7], HIGH);
      break;
    case 8:
      digitalWrite(ledPins[8], HIGH);
      break;
  }
   
}

void blankAllLeds(){
  for (int ledNumber = 0; ledNumber < 8; ledNumber++){
    //delay(500);
    digitalWrite(ledPins[ledNumber], LOW);
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

//int getPotValue(int potNumber){
//  int potValue = analogRead(potNumber);
//  return potValue; 
//}

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

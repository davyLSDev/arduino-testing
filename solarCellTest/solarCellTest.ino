const int solarPanelInput = A0;
int solarPanelValue = 0;
int ledPins[] = {2,3,4,5,6,7,8,9};
String text = "Scaled light intensity => ";
String updateMessage = "";

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
  String updateMessage = text + solarPanelValue;
  Serial.println(updateMessage);
//  delay(250);
  ledMeterOutput(solarPanelValue);
  delay(100);
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

/* Sketch for creating an lcd solar panel meter
 * 
 * improvements
 * 1. put scale marks on
 * 2. use pot and two pushbutton switches to adjust iso, fstop, shutter speed,
 *    (lcd backlight brightness?)
 *    add UI to the LCD panel to let user know which value is being changed
 * 3. Calibrate the meter
 *    add a trim pot for calibration?
 * 4. update the UI (sketch in Dia first maybe?)
 * 5. make provision for times > 1 sec with more precision
 * 6. make provision for wider aperatures, and other intermediate values
 * 7. change lcd contrast (I didn't notice a great amount of diff in values though)
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

/* Arduino pin A1 A1 -> pot
  * Arduino pin  8  -> LH switch (up)
  * Arduino pin  9  -> RH switch (down)
  * Arduino pin  10 -> pwm pin for lcd brightness  
 */

const int upSwitch = 8;
const int downSwitch = 9;
const int lcdBackpanelLight = 10; 
const int solarPanelInput = A0;
const int changeVariablesPot = A1;

const float isoTable[] = {3.75,7.5,15,3,6,12, \
   25,50,100,200,400,800,1600,3200};   
const String shutterSpeedTable[] = {"1/1000", \
  "1/500","1/250","1/125","1/60","1/30","1/15","1/8","1/4","1/2", \
  "00:01","00:02","00:04","00:08","00:15","00:30","00:60","02:00","04:00","08:00","16:00","32:00","1:04 H"};
const float aperatureTable[] = { 1.4,2,2.8,4,5.6,8, \
  11,22,32,45,64,90,128,181,256,362,512,724,1024};

int solarPanel = 0;
int lcdContrast = 50;

byte brightness;
float iso = 0;
float shutterSpeed = 0;
float aperature = 0;

void setup()                    
{

  pinMode (upSwitch, INPUT);
  pinMode (downSwitch, INPUT);
  pinMode (lcdBackpanelLight, OUTPUT);
  Serial.begin(9600);
  
  display.begin(); // initialize display
  display.setContrast(lcdContrast);
  display.setTextSize(1);
  display.setRotation(2);
  display.clearDisplay();
}

void loop()                       
{
/* variable register is used thus
 *  0 -> don't change any of iso, fstop, or shutter speed
 *  1 -> change iso
 *  2 -> change shutter speed
 *  3 -> change fstop
 *  4 -> change the lcd backlight brightness
 *  
 *  The idea is that 
 */
  int variableRegister; // = 4; // this will later change depending upon the up down switches
  variableRegister = getVariableChoice();

  if (variableRegister !=0){
    updateVariables (variableRegister);
  }
  
//  Serial.println(String(isoTable[int(iso)], DEC));
  Serial.println(shutterSpeedTable[int(shutterSpeed)]);
  analogWrite(lcdBackpanelLight, brightness);
 
  solarPanel = getSolarPanelReading();
  drawMeter("f4.5", "1/1000", int(isoTable[int(iso)]) );
  updateMeter (solarPanel);
  display.clearDisplay();
  delay(100);
}

void drawMeter(String fstop, String shutter, int iso){
  byte upperLeftCorner = 0x1;
  byte upperRightCorner = 0x2;
//  byte lowerRightCorner = 0x4;
//  byte lowerLeftCorner = 0x8;
  byte scaleCorners = upperLeftCorner | upperRightCorner;

  int numberOfScaleMarks = 5;
//  int scaleMarksAlignment = 1; // 0 -> top, 1 -> centre, 2 -> bottom
  int scaleLineLength = 4;
  int scaleBaseRadius = 6;
  int scaleRadius = 25;
  int needleBaseFillWidth = 2;
  int bracketLength = 34;
  int bracketHeight = 8;

  String minusSign = "-";
  String plusSign = "+";
   
  struct coordinate {
    int x;
    int y;
  };

  struct coordinate fstopCoordinate = {0, 0};
  struct coordinate isoLableCoordinate = {0, 40};
  struct coordinate isoValueCoordinate = {60, 40};
  struct coordinate shutterSpeedCoordinate = {48, 0};
  struct coordinate leftBracketCoordinate = {0, 38};
  struct coordinate rightBracketCoordinate = {50, 38};
  struct coordinate needleBaseCoordinate = {43, 37};
  struct coordinate minusSignCoordinate = {28, 40};
  struct coordinate plusSignCoordinate = {52, 40};
  
  display.clearDisplay();
  
// lables
  display.setCursor(fstopCoordinate.x, fstopCoordinate.y);
  display.println(fstop);
  display.setCursor(shutterSpeedCoordinate.x, shutterSpeedCoordinate.y);
  display.println(shutter);
  display.setCursor(isoLableCoordinate.x, isoLableCoordinate.y);
  display.println("ISO");
  display.setCursor(isoValueCoordinate.x, isoValueCoordinate.y);
  display.println(String (iso));
  display.setCursor(minusSignCoordinate.x, minusSignCoordinate.y);
  display.println(minusSign);
  display.setCursor(plusSignCoordinate.x, plusSignCoordinate.y);
  display.println(plusSign);

// draw the meter bottom brackets
  display.drawFastHLine(leftBracketCoordinate.x, leftBracketCoordinate.y, bracketLength, BLACK);
  display.drawFastVLine(leftBracketCoordinate.x+bracketLength, leftBracketCoordinate.y, bracketHeight, BLACK);
  display.drawFastHLine(rightBracketCoordinate.x, rightBracketCoordinate.y, bracketLength, BLACK);
  display.drawFastVLine(rightBracketCoordinate.x, rightBracketCoordinate.y, bracketHeight, BLACK);
  
// draw the centre of the needle  
  display.fillCircle(needleBaseCoordinate.x, needleBaseCoordinate.y, needleBaseFillWidth, BLACK);
  display.display();

// draw the meter's scale using circle segments
//  display.drawCircle(needleBaseCoordinate.x, needleBaseCoordinate.y, scaleRadius, BLACK);
  display.drawCircleHelper(needleBaseCoordinate.x, needleBaseCoordinate.y, scaleBaseRadius, scaleCorners, BLACK);
  display.drawCircleHelper(needleBaseCoordinate.x, needleBaseCoordinate.y, scaleRadius, scaleCorners, BLACK);

// draw the scale marks

  scaleMarks (needleBaseCoordinate.x, needleBaseCoordinate.y, numberOfScaleMarks, scaleRadius, scaleLineLength);
}

void updateMeter (int meterValue){
  int xInit = 43;
  int yInit = 37;
  int radius = 31;

  int xTip;
  int yTip;

  float Pi = 3.1415926;
  float angle = meterValue*Pi/100.0; // trig functions are in radians!

  xTip = xInit - int(radius*cos(angle));
  yTip = yInit - int(radius*abs(sin(angle)));

  display.drawLine(xInit, yInit, xTip, yTip, BLACK);
  display.display();
}
  
  void scaleMarks (int xCoordinate, int yCoordinate, int numberOfMarks, int radius, int markLength){
   
}

int getSolarPanelReading(){
// Tweak these next values for light reading  
  const int minLight = 0;
  const int maxLight = 360;
  int lightReading = analogRead(solarPanelInput);
  int lightRange = map (lightReading, minLight, maxLight, 0, 100);
  return lightRange; 
}

int getVariableChoice(){
/*  0 -> don't change any of iso, fstop, or shutter speed
 *  1 -> change iso
 *  2 -> change shutter speed
 *  3 -> change fstop
 *  4 -> change the lcd backlight brightness
 */
  return 1; // change iso
//  return 4; // lcd brightness ... works!
}

void updateVariables (int updateVariableChoice){
  const int minBrightness = 0;
  const int maxBrightness = 255;
  const int minIsoIndex = 0;
  const int maxIsoIndex = 13;
  const int minShutterSpeedIndex = 0;
  const int maxShutterSpeedIndex = 22;
  const int minAperatureIndex = 0;
  const int maxAperatureIndex = 18;
  const int minPotValue = 0;
  const int maxPotValue = 1023;

  const int minValue [] { minPotValue, minIsoIndex, minShutterSpeedIndex, minAperatureIndex, minBrightness };
  const int maxValue [] { maxPotValue, maxIsoIndex, maxShutterSpeedIndex, maxAperatureIndex, maxBrightness };

  int i = updateVariableChoice;
  int variable = map(analogRead(changeVariablesPot), minValue[0], maxValue[0], minValue[i], maxValue[i]);
  
  switch (i) {
    case 1:
      iso = variable;
      break;
    case 2:
      shutterSpeed = variable;
      break;
    case 3:
      aperature = variable;
      break;
    case 4:
      brightness = variable;
      break;
    case 5:
      break;                       
  }
}

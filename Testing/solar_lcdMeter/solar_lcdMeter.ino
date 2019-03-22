/* Sketch for creating an lcd solar panel meter
 * 
 * improvements
 * 1. put scale marks on
 * 2. use pot and two pushbutton switches to adjust iso, fstop, shutter speed,
 *    (lcd backlight brightness?)
 * 3. Calibrate the meter
 *    add a trim pot for calibration?
 * 4. update the UI (sketch in Dia first maybe?)
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
const int changeValuesPot = A1;

const int isoValueTable[] = {-375,-75,15,3,6,12, \
   25,50,100,200,400,800,1600,3200};   
const int timeValueTable[] = {-4000,-2000,-1000, \
  -500,-250,-125,-60,-30,-15,-8,-4,-2, \
  1,2,4,8,15,30,60,120,240,3600,7200,14400,28800};
const int aperatureValueTable[] = { 1,2,3,4,6,8, \
  11,22,32,45,64,90,128,181,256,362,512,724,1024};

int solarPanelValue = 0;
int lcdContrast = 50;

float isoValue = 0;
float timeValue = 0;
float aperatureValue = 0;

void setup()                    
{

  pinMode (upSwitch, INPUT);
  pinMode (downSwitch, INPUT);
  pinMode (lcdBackpanelLight, OUTPUT);
//  Serial.begin(9600);
  
  display.begin(); // initialize display
  display.setContrast(lcdContrast);
  display.setTextSize(1);
  display.setRotation(2);
  display.clearDisplay();
}

void loop()                       
{
  byte brightness = 255;
  analogWrite(lcdBackpanelLight, brightness);
  
  display.display();
  solarPanelValue = getSolarPanelReading();
  drawMeter("f4.5", "1/1000", 3200 );
  updateMeter (solarPanelValue);
  display.clearDisplay();
  delay(50);
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
  display.println(String (iso, DEC));
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

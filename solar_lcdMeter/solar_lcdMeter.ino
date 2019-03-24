/* Sketch for creating an lcd solar panel meter
 * 
 * Improvements
 * 
 * 1. put scale marks on
 *    DONE: place scale marks
 *    DONE: Don't calculate every display loop, too heavy, calculate once and keep in a vector
 * 2. use pot and two pushbutton switches to adjust 
 *    DONE: iso, fstop, shutter speed,
 *    DONE: (lcd backlight brightness?)
 *    DONE: Add UI to the LCD panel to let user know which value is being changed.
 *    Don't change value by only reading the pot value as this leads to values
 *      changing values when the up down buttons are pressed because the pot
 *      position is at the last position for the last value that was changed.
 * 3. Calibrate the meter
 *    add a trim pot for calibration?
 * 4. DONE: update the UI (sketch in Dia first maybe?)
 * 5. make provision for times > 1 sec with more precision
 * 6. make provision for wider apertures, and other intermediate values
 * 7. make provision to change lcd contrast (I didn't notice a great amount of diff in values though)
 * 8. Get rid of global variables that are unneeded
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
//Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 7, 6);
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 11, 5, 7, 6);
// harware SPI not working => Adafruit_PCD8544 display = Adafruit_PCD8544(5, 7 ,6);

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
const float apertureTable[] = { 1.4,2,2.8,4,5.6,8, \
  11,22,32,45,64,90,128,181,256,362,512,724,1024};
const String changeLable[] = {"", "ISO", "SPD", " F#", " LT"};

int solarPanel = 0;
int lcdContrast = 50;
int lastUpSwitchState = 1;
int lastDownSwitchState = 1;
int lastVariableChoice = 0;
int pauseTime = 200;

byte brightness;

float isoIDX = 0;
float shutterSpeedIDX = 0;
float apertureIDX = 0;
unsigned long debounceTimeValue = 0;

const int numberOfScaleMarks = 5; // 3 gives lots less flicker
struct coordinate {
  int x;
  int y;
};

struct coordinate needleBaseCoordinate = {32, 47};
int needleRadius = 38; //31
int scaleRadius = 32; //25
int markLineLength = 4;
struct coordinate markBottom[numberOfScaleMarks];
struct coordinate markTop[numberOfScaleMarks];

/********************
 * setup
 ********************/
void setup()                    
{
//  SPI.setClockDivider(SPI_CLOCK_DIV16); // I had to add this in to get harware SPI to work, except it doesn't
  pinMode (upSwitch, INPUT);
  pinMode (downSwitch, INPUT);
  pinMode (lcdBackpanelLight, OUTPUT);
  Serial.begin(9600);
  GetScaleMarkCoordinates (needleBaseCoordinate.x, needleBaseCoordinate.y, scaleRadius, numberOfScaleMarks, markLineLength);
  
  display.begin(); // initialize display
  display.setContrast(lcdContrast);
  display.setTextSize(1);
  display.setRotation(2);
  display.clearDisplay();
}

/********************
 * loop
 ********************/
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
  int variableChoice;
  variableChoice = getVariableChoice(debounceTimeValue, lastVariableChoice);
  lastVariableChoice = variableChoice;
  if (variableChoice !=0){
    updateVariables (variableChoice);
  }
  analogWrite(lcdBackpanelLight, brightness);
  solarPanel = getSolarPanelReading();
  drawMeter(("f"+String(apertureTable[int(apertureIDX)])), shutterSpeedTable[int(shutterSpeedIDX)], int(isoTable[int(isoIDX)]), variableChoice);
  updateMeter (solarPanel);
// DEBUG TEMP to see if this can stop the blinking  
  display.clearDisplay();
  delay(pauseTime);
}

/*********************************************************************************
 * FUNCTIONS HERE 
 *********************************************************************************/
void drawMeter(String fstop, String shutter, int iso, int changeVariable){
  byte upperLeftCorner = 0x1;
  byte upperRightCorner = 0x2;
//  byte lowerRightCorner = 0x4;
//  byte lowerLeftCorner = 0x8;
  byte scaleCorners = upperLeftCorner | upperRightCorner;
  int scaleBaseRadius = 6;
  int needleBaseFillWidth = 2;
  int bracketLength = 34;
  int bracketHeight = 8;
  String minusSign = "-";
  String plusSign = "+";
  
  struct coordinate isoValueCoordinate = {0, 0};
  struct coordinate changeLableCoordinate = {66, 40};
  struct coordinate fstopCoordinate = {54, 10};
  struct coordinate shutterSpeedCoordinate = {48, 0};
/*  struct coordinate leftBracketCoordinate = {0, 38};
  struct coordinate rightBracketCoordinate = {50, 38};*/
  struct coordinate minusSignCoordinate = {16, 40};
  struct coordinate plusSignCoordinate = {42, 40};
  
  display.clearDisplay();
  
// lables
  display.setCursor(fstopCoordinate.x, fstopCoordinate.y);
  display.println(fstop.substring(0, 5));
  display.setCursor(shutterSpeedCoordinate.x, shutterSpeedCoordinate.y);
  display.println(shutter);

  if (changeVariable !=0){
    display.setCursor(changeLableCoordinate.x, changeLableCoordinate.y);
    display.println(changeLable[changeVariable]);
  }
  
  
  display.setCursor(isoValueCoordinate.x, isoValueCoordinate.y);
  display.println(String (iso));
  display.setCursor(minusSignCoordinate.x, minusSignCoordinate.y);
  display.println(minusSign);
  display.setCursor(plusSignCoordinate.x, plusSignCoordinate.y);
  display.println(plusSign);

/* DON'T BOTHER TO draw the meter bottom brackets
  display.drawFastHLine(leftBracketCoordinate.x, leftBracketCoordinate.y, bracketLength, BLACK);
  display.drawFastVLine(leftBracketCoordinate.x+bracketLength, leftBracketCoordinate.y, bracketHeight, BLACK);
  display.drawFastHLine(rightBracketCoordinate.x, rightBracketCoordinate.y, bracketLength, BLACK);
  display.drawFastVLine(rightBracketCoordinate.x, rightBracketCoordinate.y, bracketHeight, BLACK); */
  
// draw the centre of the needle  
  display.fillCircle(needleBaseCoordinate.x, needleBaseCoordinate.y, needleBaseFillWidth, BLACK);
  display.display();

// draw the meter's scale using circle segments
//  display.drawCircle(needleBaseCoordinate.x, needleBaseCoordinate.y, scaleRadius, BLACK);
  display.drawCircleHelper(needleBaseCoordinate.x, needleBaseCoordinate.y, scaleBaseRadius, scaleCorners, BLACK);
  display.drawCircleHelper(needleBaseCoordinate.x, needleBaseCoordinate.y, scaleRadius, scaleCorners, BLACK);
// draw the scale marks  
  scaleMarks (needleBaseCoordinate.x, needleBaseCoordinate.y, numberOfScaleMarks, scaleRadius, markLineLength);
}

/*********************
 * update the meter 
 *********************/ 
void updateMeter (int meterValue){

  int xTip;
  int yTip;

  float Pi = 3.1415926;
  float angle = meterValue*Pi/100.0; // trig functions are in radians!

  xTip = needleBaseCoordinate.x - int(needleRadius*cos(angle));
  yTip = needleBaseCoordinate.y - int(needleRadius*abs(sin(angle)));

  display.drawLine(needleBaseCoordinate.x, needleBaseCoordinate.y, xTip, yTip, BLACK);
  display.display();
}

/*********************
 * calculate the scale mark coordinates
 *********************/
  void GetScaleMarkCoordinates (int xCoordinate, int yCoordinate, int radius, int numberOfMarks, int markLength){
    int xTop;
    int yTop;
    int xBottom;
    int yBottom;
    int outsideRadius = radius + markLength;
    int markNumber=1;

    float Pi = 3.1415926;
    int angleForEachMark = 180/(1+numberOfMarks);
    int angleToPlaceMark = 90;

    while (markNumber <= numberOfMarks) {
//      Serial.println("number of marks is "+String(numberOfMarks, DEC));
      angleToPlaceMark = markNumber*angleForEachMark;
//      Serial.println("angle to place this mark is: "+String(angleToPlaceMark, DEC));
      float angle = angleToPlaceMark*Pi/180.0; // trig functions are in radians!
      markTop[markNumber-1].x = xCoordinate - int(outsideRadius*cos(angle));
      markTop[markNumber-1].y = yCoordinate - int(outsideRadius*abs(sin(angle)));
      markBottom[markNumber-1].x = xCoordinate - int(radius*cos(angle));
      markBottom[markNumber-1].y = yCoordinate - int(radius*abs(sin(angle)));
//      display.drawLine(xBottom, yBottom, xTop, yTop, BLACK);
//      display.display();  // thought to place this after this while loop
      markNumber++;
    } 
  }

/*********************
 * draw the scale marks 
 *********************/ 
  void scaleMarks (int xCoordinate, int yCoordinate, int numberOfMarks, int radius, int markLength){

    int markNumber=1;
    while (markNumber <= numberOfMarks) {
      display.drawLine(markBottom[markNumber-1].x, markBottom[markNumber-1].y, markTop[markNumber-1].x, markTop[markNumber-1].y, BLACK);
      display.display();  // thought to place this after this while loop
      markNumber++;
    }
}

/*********************
 * fetch the solar panel voltage 
 *********************/
int getSolarPanelReading(){
// Tweak these next values for light reading  
  const int minLight = 0;
  const int maxLight = 360;
  int lightReading = analogRead(solarPanelInput);
  int lightRange = map (lightReading, minLight, maxLight, 0, 100);
  return lightRange; 
}

/*********************
 * fetch the variable choice
 *********************/
int getVariableChoice(unsigned long lastTime, int lastChoice){
/*  0 -> don't change any of iso, fstop, or shutter speed
 *  1 -> change iso
 *  2 -> change shutter speed
 *  3 -> change fstop
 *  4 -> change the lcd backlight brightness
 */
//  return 1; // change iso ... works, but improve by displaying decimal values like 3.75, 7.5
//  return 2; // change shutter speed ... works just fine
//  return 3; // change fstop ... works!
//  return 4; // lcd brightness ... works!

  unsigned long timeNow = millis();
  unsigned long debounceDelay = 50;

  int upSwitchState = digitalRead(upSwitch);
  int downSwitchState = digitalRead(downSwitch);
  int choice = 0;
//  Serial.println("up is "+String(upSwitchState));
//  Serial.println("down is "+String(downSwitchState));

  if ( (timeNow - lastTime ) >= debounceDelay){
    lastTime = timeNow;
    if ( upSwitchState != lastUpSwitchState ) {
      lastUpSwitchState = upSwitchState;
      if ( upSwitchState == 0 ) {
        lastChoice++;
      }
    }
    
    if ( downSwitchState != lastDownSwitchState ) {
      if ( downSwitchState == 0 ) {
        lastChoice--;
      }
    }

    if ( lastChoice > 4 ) lastChoice = 0; // wraparound
    if ( lastChoice < 0 ) lastChoice = 4;
  }
  
  return lastChoice;
}

/*********************
 * update the variables
 *********************/
void updateVariables (int updateVariableChoice){
  const int minBrightness = 0;
  const int maxBrightness = 255;
  const int minIsoIndex = 0;
  const int maxIsoIndex = 13;
  const int minShutterSpeedIndex = 0;
  const int maxShutterSpeedIndex = 22;
  const int minApertureIndex = 0;
  const int maxApertureIndex = 18;
  const int minPotValue = 0;
  const int maxPotValue = 1023;

  const int minValue [] { minPotValue, minIsoIndex, minShutterSpeedIndex, minApertureIndex, minBrightness };
  const int maxValue [] { maxPotValue, maxIsoIndex, maxShutterSpeedIndex, maxApertureIndex, maxBrightness };

  int i = updateVariableChoice;
  int variable = map(analogRead(changeVariablesPot), minValue[0], maxValue[0], minValue[i], maxValue[i]);
  
  switch (i) {
    case 1:
      isoIDX = variable;
      break;
    case 2:
      shutterSpeedIDX = variable;
      break;
    case 3:
      apertureIDX = variable;
      break;
    case 4:
      brightness = variable;
      break;                       
  }
}

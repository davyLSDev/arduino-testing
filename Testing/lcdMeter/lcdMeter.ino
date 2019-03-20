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

int lcdContrast = 50;

void setup()                    
{
  Serial.begin(9600);
  
  display.begin(); // initialize display
  
  display.setContrast(lcdContrast);
  display.setTextSize(1);
  display.setRotation(2);
  display.clearDisplay();
}

void loop()                       
{
  display.display();
//  test();
//  drawMeter("f4.5", "1/1000", 3200 );
  for (int i = 0; i < 101; i++){
    drawMeter("f4.5", "1/1000", 3200 );
    updateMeter (i);
    display.clearDisplay();
    delay(300);
  }
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

  int numberOfScaleMarks = 5;
  int scaleMarksAlignment = 1; // 0 -> top, 1 -> centre, 2 -> bottom
  int scaleLineHeight = 4;
  int scaleRadius = 20;
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
  display.fillCircle( needleBaseCoordinate.x, needleBaseCoordinate.y, needleBaseFillWidth, BLACK);
  display.display();
}

void updateMeter (int meterValue){
  int xInit = 43;
  int yInit = 37;
  int radius = 29;

  int xTip;
  int yTip;

  int angle = 1.8*meterValue;
//  if (meterValue >90){
//    xTip = abs(sin(angle))*radius;
//  } else {
//    x = abs(sin(angle))*radius;
//  }
//  
//  y = abs(cos(angle))*radius;

  xTip = xInit + radius*cos(angle);
  yTip = yInit - radius*abs(sin(angle));

  display.drawLine(xInit, yInit, xTip, yTip, BLACK);
  display.display();
}

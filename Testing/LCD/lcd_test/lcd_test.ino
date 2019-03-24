
#include <PCD8544.h>


const int lcdPin_CLK = 3;
const int lcdPin_DIN = 4;
const int lcdPin_DC = 5;
const int lcdPin_RESET = 6;
const int lcdPin_SCE = 7;

PCD8544 lcd(lcdPin_CLK, lcdPin_DIN, lcdPin_DC, lcdPin_RESET);

void setup() {
//  lcd.init();
  lcd.setCursor(0,0);
  lcd.print("Hello, World!");
}

void loop() {
  lcd.setCursor(0,1);
  lcd.print(millis());
}

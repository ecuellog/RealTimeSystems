#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  lcd.begin(16, 2);
}

int val = 0;
void loop() {
  lcd.clear();
  lcd.print("light: ");
  lcd.print(val++);
  lcd.setCursor(0, 1);
  lcd.print("x: ");
  lcd.print(val++);
  lcd.print(" y: ");
  lcd.print(val++);

  delay(250);
}


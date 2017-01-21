#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  // Initialize pin 53 for logic analyzer timing.
  pinMode(53, OUTPUT);
  lcd.begin(16, 2);
}

void loop() {
  lcd.print("123");
  delay(1000);
  lcd.clear();
}


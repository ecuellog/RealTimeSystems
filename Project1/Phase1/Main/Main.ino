/*
 * Project 1 Phase 1
 * Authors: Konrad Schultz, Edguardo Cuello
 */
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int laserPin = 53;
int lightSensorPin = 6;

int currentLight = 0;
int ambientLight = 0;

void setup() {
  pinMode(laserPin, OUTPUT);
  ambientLight = analogRead(lightSensorPin);
  currentLight = ambientLight;
}

void loop() {
  checkLight();
  refreshLcd();
  delay(100);
}

void checkLight() {
    // light goes up by 100
    currentLight = analogRead(lightSensorPin);
}

void refreshLcd() {
  lcd.clear();
  int lightDelta = currentLight - ambientLight;
  if (lightDelta > 100) {
    lcd.print("ON : ");
  } else {
    lcd.print("OFF: ");
  }
  lcd.print(lightDelta);
  lcd.setCursor(0, 1);
  lcd.print("x: ");
  lcd.print("jsX");
  lcd.print(" y: ");
  lcd.print("jsY");
}

void shoot() {
  digitalWrite (laserPin, HIGH);
}
void unshoot() {
  digitalWrite(laserPin, LOW);
}


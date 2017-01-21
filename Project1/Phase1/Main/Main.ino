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
  currentLight = ambientLight
}

void loop() {

}

void checkLight() {
    // light goes up by 100
    currentLight = analogRead(lightSensorPin);
    if (currentLight - ambientLight > 100) {
      // TODO: action when light is shined on us
    }
}

void refreshLcd() {
  lcd.clear();
  lcd.print("light: ");
  lcd.print(currentLight);
  lcd.setCursor(0, 1);
  lcd.print("x: ");
  lcd.print("jsX");
  lcd.print(" y: ");
  lcd.print("jsY");
}

// TODO: rewrite to use timer/interrupt
void shoot() {
  digitalWrite (laserPin, HIGH);
  delay (1000);
  digitalWrite (laserPin, LOW);
}


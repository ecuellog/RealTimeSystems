/* 
 *  Code modified from arduino website.
 */

int lightSensorPin = 6;

int ambientLight = 0;
void setup() {
    Serial.begin(9600);
    ambientLight = analogRead(lightSensorPin);
}

void loop() {
    int light = analogRead(lightSensorPin);

    // light goes up by 100
    if (light - ambientLight > 100) {
      Serial.println("SHINY");
    }
}

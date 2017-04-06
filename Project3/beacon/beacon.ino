#define LIGHTPIN 6
#define LEDPIN 13

int ambientLight = 0;
void setup() {
    Serial.begin(9600);
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, LOW);
    ambientLight = analogRead(LIGHTPIN);
}

void loop() {
    int light = analogRead(LIGHTPIN);

    // light goes down by 100
    if (ambientLight - light > 100) {
      digitalWrite(LEDPIN, HIGH);
    } else {
      digitalWrite(LEDPIN, LOW);
    }
}

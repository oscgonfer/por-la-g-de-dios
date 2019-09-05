#include <Arduino.h> 
#define interval 60

int RELAY_1 = 12;
int RELAY_2 = 13;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
}
void loop()
{
  // digitalWrite(RELAY_1, LOW);
  // delay(interval);
  // digitalWrite(RELAY_1, HIGH);
  // delay(interval);
  digitalWrite(RELAY_2, HIGH);
  delay(interval);
  digitalWrite(RELAY_2, LOW);
  delay(interval);
}

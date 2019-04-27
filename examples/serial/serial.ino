#include "SDHT.h"

SDHT dht;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (dht.broadcast(DHT22, 2)) layout();
  delay(2000);
}

void layout() {
  Serial.print("   Humdity => ");
  Serial.println(String(dht.humidity, 1));
  Serial.print("   Celsius => ");
  Serial.println(String(dht.celsius, 2));
}

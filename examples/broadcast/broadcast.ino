#include "SDHT.h"

SDHT dht;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (dht.read(DHT22, 2)) layout();
  delay(2000);
}

void layout() {
  Serial.print("   Celsius => ");
  Serial.println(String(double(dht.celsius) / 10, 1));
  Serial.print("   Humdity => ");
  Serial.println(String(double(dht.humidity) / 10, 1));
}

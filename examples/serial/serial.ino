#include "SDHT.h"

SDHT dht;
int8_t notice;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((notice  = dht.broadcast(2, DHT22)) > 0) layout(); else Serial.println(notice);
  delay(4000);
}

void layout() {
  Serial.print("   Notice => ");
  Serial.println(String(notice));
  Serial.print("   Humdity => ");
  Serial.println(String(dht.humidity, 1));
  Serial.println("   Temperature => ");
  Serial.print("   Celsius => ");
  Serial.print(String(dht.temperature.celsius, 2));
  Serial.print("   Fahrenheit => ");
  Serial.println(String(dht.temperature.fahrenheit, 2));
  Serial.println("   Heat index => ");
  Serial.print("   Celsius => ");
  Serial.print(String(dht.heat.celsius, 2));
  Serial.print("   Fahrenheit => ");
  Serial.println(String(dht.heat.fahrenheit, 2));
}

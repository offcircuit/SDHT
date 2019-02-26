#include "SDHT.h"

SDHT dht;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  dht.broadcast(2, DHT22);
  layout();
  delay(4000);
}

void layout() {
  Serial.print("   Notice => ");
  Serial.print(dht.notice);
  Serial.print("   Humdity => ");
  Serial.println(String(dht.humidity, 1));
  Serial.println("   Temperature => ");
  Serial.print("   Celsius => ");
  Serial.print(String(dht.temperature.celsius, 2));
  Serial.print("   Fahrenheit => ");
  Serial.print(String(dht.temperature.fahrenheit, 2));
  Serial.print("   Kelvin => ");
  Serial.println(String(dht.temperature.kelvin, 2));
  Serial.println("   Heat index => ");
  Serial.print("   Celsius => ");
  Serial.print(String(dht.heat.celsius, 2));
  Serial.print("   Fahrenheit => ");
  Serial.print(String(dht.heat.fahrenheit, 2));
  Serial.print("   Kelvin => ");
  Serial.println(String(dht.heat.kelvin, 2));

}

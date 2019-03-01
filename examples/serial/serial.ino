#include "SDHT.h"

SDHT dht;
int8_t notice;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((notice  = dht.broadcast(DHT22, 2)) == SDHT_OK) layout(); else Serial.println(notice);
  delay(2000);

}

void layout() {
  Serial.print("   Notice => ");
  Serial.println(String(notice));
  Serial.print("   Humdity => ");
  Serial.println(String(dht.humidity, 1));
  Serial.println("   Temperature => ");
  Serial.print("   Celsius => ");
  Serial.print(String(dht.celsius, 2));
  Serial.print("   Fahrenheit => ");
  Serial.println(String(dht.fahrenheit(dht.celsius), 2));
  Serial.println("   Heat index => ");
  Serial.print("   Celsius => ");
  Serial.print(String(dht.heatIndex(dht.humidity, dht.celsius), 2));
  Serial.print("   Fahrenheit => ");
  Serial.println(String(dht.fahrenheit(dht.heatIndex(dht.humidity, dht.celsius)), 2));
}

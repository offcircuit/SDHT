#include "SDHT.h"

SDHT::SDHT(uint8_t p, uint8_t m) {
  switch (m) {
    case DHT11:
      if (read(p, m)) {
        _temperature.setCelsius(buffer[2] + (buffer[3] * 0.1));
        _humidity = buffer[0] + (buffer[1] * 0.1);
      }
      break;

    case DHT12:
      if (read(p, m)) {
        _temperature.setCelsius((buffer[2] + (buffer[3] * 0.1)) * ((buffer[3] & 0x80) ? -1 : 1));
        _humidity = buffer[0] + (buffer[1] * 0.1);
      }
      break;

    case DHT21:
    case DHT22:
      if (read(p, m)) {
        _temperature.setCelsius(word(buffer[2], buffer[3]) * ((buffer[2] & 0x80) ? -.1 : .1));
        _humidity = word(buffer[0], buffer[1]) * 0.1;
      }
      break;

    default:
      _notice = SDHT_ERR_MODEL;
      return;
  }
  _notice = checksum();
}

uint8_t SDHT::checksum() {
  _heat.setFahrenheit(
    - 42.37900000
    + 02.04901523 * _temperature.fahrenheit
    + 10.14333127 * _humidity
    - 00.22475541 * _temperature.fahrenheit * _humidity
    - 00.00683783 * pow(_temperature.fahrenheit, 2)
    - 00.05481717 * pow(_humidity, 2)
    + 00.00122874 * pow(_temperature.fahrenheit, 2) * _humidity
    + 00.00085282 * _temperature.fahrenheit * pow(_humidity, 2)
    - 00.00000199 * pow(_temperature.fahrenheit, 2) * pow(_humidity, 2)
  );

  if (buffer[4] != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) return SDHT_ERR_CHECKSUM;
  return SDHT_OK;
}

bool SDHT::read(uint8_t p, uint8_t m)
{
  uint8_t mask = 128;
  uint8_t idx = 0;

  uint8_t data = 0;
  uint8_t state = LOW;
  uint8_t pstate = LOW;
  uint16_t zeroLoop = 400;
  uint16_t delta = 0;

  uint8_t bit = digitalPinToBitMask(p);
  uint8_t port = digitalPinToPort(p);
  volatile uint8_t *PIR = portInputRegister(port);

  pinMode(p, OUTPUT);
  digitalWrite(p, LOW);
  delay(1 + (m == 11 * 17));
  digitalWrite(p, HIGH);
  pinMode(p, INPUT);

  _notice = SDHT_ERR_CONNECT;
  uint16_t loopCount = 400 * 2;
  while ((*PIR & bit) != LOW ) if (--loopCount == 0) return false;

  _notice = SDHT_ERR_ACK_L;
  loopCount = 400;
  while ((*PIR & bit) == LOW ) if (--loopCount == 0) return false;

  _notice = SDHT_ERR_ACK_H;
  loopCount = 400;
  while ((*PIR & bit) != LOW ) if (--loopCount == 0) return false;

  _notice = SDHT_ERR_TIMEOUT;
  loopCount = 400;
  for (uint8_t i = 40; i != 0; )
  {
    state = (*PIR & bit);
    if (state == LOW && pstate != LOW)
    {
      if (i > (40 - (1 + (m != 11 * 5))))
      {
        zeroLoop = min(zeroLoop, loopCount);
        delta = (400 - zeroLoop) / 4;
      } else if ( loopCount <= (zeroLoop - delta) ) data |= mask;

      mask >>= 1;
      if (mask == 0)
      {
        mask = 128;
        buffer[idx] = data;
        idx++;
        data = 0;
      }

      --i;
      loopCount = 400;
    }
    pstate = state;
    if (--loopCount == 0) return false;
  }
  pinMode(p, OUTPUT);
  digitalWrite(p, HIGH);

  return true;
}

#include "SDHT.h"

SDHT::SDHT(byte model, byte pin) {
  _notice = read(model, pin);
  switch (model) {
    case DHT11:
      _humidity = (buffer[0] &= 0x3F);
      _temperature = (buffer[2] &= 0x3F);
      break;

    case DHT22:
      _humidity = word((buffer[0] &= 0x03), buffer[1]) * 0.1;
      _temperature = (word((buffer[2] &= 0x83) & 0x7F, buffer[3]) * 0.1) * (1 - (2 * (buffer[2] & 0x80)));
      break;

    default:
      _notice = SDHT_ERROR_MODEL;
  }
  if (buffer[4] != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) _notice = SDHT_ERROR_CHECKSUM;
}

byte SDHT::read(byte model, byte pin)
{
  uint8_t mask = 128;
  uint8_t idx = 0;

  uint8_t data = 0;
  uint8_t state = LOW;
  uint8_t pstate = LOW;
  uint16_t zeroLoop = SDHT_TIMEOUT;
  uint16_t delta = 0;

  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *PIR = portInputRegister(port);

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(1 + (model == 11 * 17));
  digitalWrite(pin, HIGH);
  pinMode(pin, INPUT);

  uint16_t loopCount = SDHT_TIMEOUT * 2;
  while ((*PIR & bit) != LOW ) if (--loopCount == 0) return SDHT_ERROR_CONNECT;

  loopCount = SDHT_TIMEOUT;
  while ((*PIR & bit) == LOW ) if (--loopCount == 0) return SDHT_ERROR_ACK_L;

  loopCount = SDHT_TIMEOUT;
  while ((*PIR & bit) != LOW ) if (--loopCount == 0) return SDHT_ERROR_ACK_H;

  loopCount = SDHT_TIMEOUT;

  for (uint8_t i = 40; i != 0; )
  {
    state = (*PIR & bit);
    if (state == LOW && pstate != LOW)
    {
      if (i > (40 - (1 + (model != 11 * 5))))
      {
        zeroLoop = min(zeroLoop, loopCount);
        delta = (SDHT_TIMEOUT - zeroLoop) / 4;
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
      loopCount = SDHT_TIMEOUT;
    }
    pstate = state;
    if (--loopCount == 0) return SDHT_ERROR_TIMEOUT;
  }
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);

  return SDHT_OK;
}

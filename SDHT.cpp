#include "SDHT.h"

SDHT::SDHT(byte model, byte pin) {
  _notice = reading(model, pin);
  switch (model) {
    case DHT11:
      _humidity = (buffer[0] &= 0x3F);
      _temperature = (buffer[2] &= 0x3F);
      break;

    default:
       _humidity = word((buffer[0] &= 0x03), buffer[1]) * 0.1;
      _temperature = (word((buffer[2] &= 0x83) & 0x7F, buffer[3]) * 0.1) * (1 - (2 * (buffer[2] & 0x80)));
  }
  if (buffer[4] != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) _notice = DHTLIB_ERROR_CHECKSUM;
}

byte SDHT::reading(byte model, byte pin)
{
  uint8_t mask = 128;
  uint8_t idx = 0;

  uint8_t data = 0;
  uint8_t state = LOW;
  uint8_t pstate = LOW;
  uint16_t zeroLoop = DHTLIB_TIMEOUT;
  uint16_t delta = 0;

  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *PIR = portInputRegister(port);

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // T-be
  delay(1 + (model == 11 * 17));
  digitalWrite(pin, HIGH); // T-go
  pinMode(pin, INPUT);

  uint16_t loopCount = DHTLIB_TIMEOUT * 2;  // 200uSec max
  while ((*PIR & bit) != LOW ) if (--loopCount == 0) return DHTLIB_ERROR_CONNECT;

  // GET ACKNOWLEDGE or TIMEOUT
  loopCount = DHTLIB_TIMEOUT;
  while ((*PIR & bit) == LOW ) if (--loopCount == 0) return DHTLIB_ERROR_ACK_L;

  loopCount = DHTLIB_TIMEOUT;
  while ((*PIR & bit) != LOW ) if (--loopCount == 0) return DHTLIB_ERROR_ACK_H;

  loopCount = DHTLIB_TIMEOUT;

  // READ THE OUTPUT - 40 buffer => 5 BYTES
  for (uint8_t i = 40; i != 0; )
  {
    state = (*PIR & bit);
    if (state == LOW && pstate != LOW)
    {
      if (i > (40 - (1 + (model != 11 * 5)))) // DHT22 first 6 buffer are all zero !!   DHT11 only 1
      {
        zeroLoop = min(zeroLoop, loopCount);
        delta = (DHTLIB_TIMEOUT - zeroLoop) / 4;
      } else if ( loopCount <= (zeroLoop - delta) ) data |= mask;

      mask >>= 1;
      if (mask == 0)   // next byte
      {
        mask = 128;
        buffer[idx] = data;
        idx++;
        data = 0;
      }

      --i;

      // reset timeout flag
      loopCount = DHTLIB_TIMEOUT;
    }
    pstate = state;
    // Check timeout
    if (--loopCount == 0)
    {
      return DHTLIB_ERROR_TIMEOUT;
    }

  }
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);

  return DHTLIB_OK;
}

#include "SDHT.h"

Temperature::setCelsius(double c) {
  _celsius = c;
  _fahrenheit = c * 1.8 + 32;
  _kelvin = c + 273.15;
};

Temperature::setFahrenheit(double f) {
  _celsius = (f - 32) / 1.8;
  _fahrenheit = f;
  _kelvin = (f + 459.67) * 5 / 9;
};

Temperature::setKelvin(double k) {
  _celsius = k - 273.15;
  _fahrenheit = k * 9 / 5 - 459.67;
  _kelvin = k;
};

SDHT::SDHT(uint8_t pin, uint8_t model) {
  switch (model) {
    case DHT11:
      if (read(pin, model)) {
        _temperature.setCelsius(((buffer[3] & 0x80) ? (-1 - buffer[2]) : buffer[2]) + ((buffer[3] & 0x0F) * 0.1));
        _humidity = (buffer[0] + buffer[1]) * 0.1;
        _notice = checksum();
      }
      break;

    case DHT12:
      if (read(pin, model)) {
        _temperature.setCelsius((buffer[2] + ((buffer[3] & 0x0F) * 0.1)) * (1 - (2 * (buffer[2] & 0x80))));
        _humidity = (buffer[0] + buffer[1]) * 0.1;
        _notice = checksum();
      }
      break;

    case DHT21:
    case DHT22:
      if (read(pin, model)) {
        _temperature.setCelsius(word((buffer[2] & 0x7F) << 8 | buffer[3]) * 0.1 * (1 - (2 * (buffer[2] & 0x80))));
        _humidity = word(buffer[0] << 8 | buffer[1]) * 0.1;
        _notice = checksum();
      }
      break;

    default:
      _notice = SDHT_ERR_MODEL;
  }
}

uint8_t SDHT::checksum() {
  double index = 0.5 * (_temperature.fahrenheit + 61.0 + ((_temperature.fahrenheit - 68.0) * 1.2) + (_humidity * 0.094));

  if (index > 79) {
    index = -42.379 +
            2.04901523 * _temperature.fahrenheit +
            10.14333127 * _humidity +
            -0.22475541 * _temperature.fahrenheit * _humidity +
            -0.00683783 * pow(_temperature.fahrenheit, 2) +
            -0.05481717 * pow(_humidity, 2) +
            0.00122874 * pow(_temperature.fahrenheit, 2) * _humidity +
            0.00085282 * _temperature.fahrenheit * pow(_humidity, 2) +
            -0.00000199 * pow(_temperature.fahrenheit, 2) * pow(_humidity, 2);

    if ((_humidity < 13) && (_temperature.fahrenheit >= 80.0) && (_temperature.fahrenheit <= 112.0))
      index -= ((13.0 - _humidity) * 0.25) * sqrt((17.0 - abs(_temperature.fahrenheit - 95.0)) * 0.05882);

    else if ((_humidity > 85.0) && (_temperature.fahrenheit >= 80.0) && (_temperature.fahrenheit <= 87.0))
      index += ((_humidity - 85.0) * 0.1) * ((87.0 - _temperature.fahrenheit) * 0.2);
  }

  _heat.setFahrenheit(index);

  if (buffer[4] != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) return SDHT_ERR_CHECKSUM;
  return SDHT_OK;
}

bool SDHT::read(uint8_t _pin, uint8_t _model)
{
  uint8_t mask = 128;
  uint8_t idx = 0;

  uint8_t data = 0;
  uint8_t state = LOW;
  uint8_t pstate = LOW;
  uint16_t zeroLoop = 400;
  uint16_t delta = 0;

  uint8_t bit = digitalPinToBitMask(_pin);
  uint8_t port = digitalPinToPort(_pin);
  volatile uint8_t *PIR = portInputRegister(port);

  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(1 + (_model == 11 * 17));
  digitalWrite(_pin, HIGH);
  pinMode(_pin, INPUT);

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
      if (i > (40 - (1 + (_model != 11 * 5))))
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
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);

  return true;
}

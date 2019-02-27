#include "SDHT.h"

uint8_t SDHT::broadcast(uint8_t pin, uint8_t model) {
  int8_t notice;

  if (model > DHT22) notice = SDHT_ERROR_MODEL;
  else if ((_port = digitalPinToPort(pin)) == NOT_A_PIN) notice = SDHT_ERROR_PIN;
  else if (!(notice = read(pin, (model < DHT21) ? 20 : 1))) {
    switch (model) {
      case DHT11:
        _humidity = _data[0] + (_data[1] * .1);
        _temperature.setCelsius(_data[2] + (_data[3] * .1));
        break;

      case DHT12:
        _humidity = _data[0] + (_data[1] * .1);
        _temperature.setCelsius((_data[2] + ((_data[3] & 0x7F) * .1)) * ((_data[3] & 0x80) ? -1 : 1));
        break;

      case DHT21:
      case DHT22:
        _humidity = word(_data[0], _data[1]) * .1;
        _temperature.setCelsius(word((_data[2] & 0x7F), _data[3]) * ((_data[2] & 0x80) ? -.1 : .1));
        break;
    }
#ifndef SDHT_NO_HEAT

#ifndef SDHT_NO_FAHRENHEIT
    double fahrenheit = _temperature.fahrenheit;
#else
    double fahrenheit = _temperature.celsius * 1.8 + 32;
#endif

    double heatIndex = (0.5 * (fahrenheit + 61.0 + ((fahrenheit - 68.0) * 1.2) + (_humidity * 0.094)));

    if (heatIndex > 79) {
      heatIndex =
        - 42.37900000
        + 02.04901523 * fahrenheit
        + 10.14333127 * _humidity
        - 00.22475541 * fahrenheit * _humidity
        - 00.00683783 * pow(fahrenheit, 2)
        - 00.05481717 * pow(_humidity, 2)
        + 00.00122874 * pow(fahrenheit, 2) * _humidity
        + 00.00085282 * fahrenheit * pow(_humidity, 2)
        - 00.00000199 * pow(fahrenheit, 2) * pow(_humidity, 2)
        ;

      if ((_humidity < 13.0) && (fahrenheit >= 80.0) && (fahrenheit <= 112.0))
        heatIndex -= ((13.0 - _humidity) * 0.25) * sqrt((17.0 - abs(fahrenheit - 95.0)) * 0.05882);
      else if ((_humidity > 85.0) && (fahrenheit >= 80.0) && (fahrenheit <= 87.0))
        heatIndex += ((_humidity - 85.0) * 0.1) * ((87.0 - fahrenheit) * 0.2);
    }
    _heat.setFahrenheit(heatIndex);

#endif
    notice = (_data[4] == uint8_t(_data[0] + _data[1] + _data[2] + _data[3])) ? SDHT_OK : SDHT_WRONG_PARITY;
  }
  interrupts();
  return notice;
}

bool SDHT::pulse(uint8_t bitmask) {
  _signal = 0;
  while ((*portInputRegister(_port) & _bitmask) == bitmask) if (SDHT_CYCLES < _signal++) return false;
  return (_signal != 0);
}

uint8_t SDHT::read(uint8_t pin, uint8_t msDelay)
{
  uint16_t buffer;

  _bitmask = digitalPinToBitMask(pin);
  _data[0] = _data[1] = _data[2] = _data[3] = _data[4] = 0;

#ifdef ESP8266
  yield();
#endif

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(msDelay);
  digitalWrite(pin, HIGH);
  pinMode(pin, INPUT);

  noInterrupts();

  if (!pulse(_bitmask)) return SDHT_ERROR_CONNECT;
  if (!pulse(0)) return SDHT_ERROR_REQUEST;
  if (!pulse(_bitmask)) return SDHT_ERROR_RESPONSE;

  for (int i = 0; i < 40; i++) {
    if (!pulse(0)) return SDHT_ERROR_WAIT - (i * 2);
    buffer = _signal;
    if (!pulse(_bitmask) < 0) return SDHT_ERROR_VALUE - (i * 2 + 1);
    _data[i / 8] += _data[i / 8] + (_signal > buffer);
  }
  return false;
}

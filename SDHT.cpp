#include "SDHT.h"

int8_t SDHT::broadcast(uint8_t pin, uint8_t model) {
  if (model > DHT22) return notice(SDHT_ERROR_MODEL);
  else if ((_port = digitalPinToPort(pin)) == NOT_A_PIN) return notice(SDHT_ERROR_PIN);
  else {
    uint16_t buffer, signal;
    uint8_t data[5] = {0, 0, 0, 0, 0};

    _bitmask = digitalPinToBitMask(pin);

#ifdef ESP8266
    yield();
#endif

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay((model < DHT21) ? 20 : 1);
    digitalWrite(pin, HIGH);
    pinMode(pin, INPUT);

    noInterrupts();

    if (!pulse(_bitmask)) return notice(SDHT_ERROR_CONNECT);
    if (!pulse(0)) return notice(SDHT_ERROR_REQUEST);
    if (!pulse(_bitmask)) return notice(SDHT_ERROR_RESPONSE);

    for (int i = 0; i < 40; i++) {
      if (!(buffer = pulse(0))) return notice(SDHT_ERROR_WAIT - (i * 2));
      if (!(signal = pulse(_bitmask))) return notice(SDHT_ERROR_VALUE - (i * 2 + 1));
      data[i / 8] += data[i / 8] + (signal > buffer);
    }

    if (data[4] != uint8_t(data[0] + data[1] + data[2] + data[3])) return notice(SDHT_ERROR_PARITY);

    switch (model) {
      case DHT11:
        _humidity = data[0] + (data[1] * .1);
        _temperature.setCelsius(data[2] + (data[3] * .1));
        break;

      case DHT12:
        _humidity = data[0] + (data[1] * .1);
        _temperature.setCelsius((data[2] + ((data[3] & 0x7F) * .1)) * ((data[3] & 0x80) ? -1 : 1));
        break;

      case DHT21:
      case DHT22:
        _humidity = word(data[0], data[1]) * .1;
        _temperature.setCelsius(word((data[2] & 0x7F), data[3]) * ((data[2] & 0x80) ? -.1 : .1));
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
  }
  return notice(SDHT_OK);
}

int8_t SDHT::notice(int8_t id) {
  interrupts();
  return id;
}

uint16_t SDHT::pulse(uint8_t bitmask) {
  int16_t signal = 0;
  while ((*portInputRegister(_port) & _bitmask) == bitmask) if (SDHT_CYCLES < signal++) return 0;
  return signal;
}

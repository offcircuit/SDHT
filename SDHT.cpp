#include "SDHT.h"

int32_t SDHT_REG_STATE;

int8_t SDHT::broadcast(uint8_t model, uint8_t pin) {
  if (model > DHT22) return SDHT_NOTICE_ERROR_MODEL;
  else if ((_port = digitalPinToPort(pin)) == NOT_A_PIN) return SDHT_NOTICE_ERROR_PIN;
  else {
    uint16_t buffer, signal;
    uint8_t data[5] = {0, 0, 0, 0, 0};
    
#ifdef ESP8266
    volatile uint32_t *mode, *output;
    yield();
    SDHT_REG_STATE = xt_rsil(15);
#else
    volatile uint8_t *mode, *output;
    SDHT_REG_STATE = SREG;
    SREG &= 0x7F;
#endif

    _bitmask = digitalPinToBitMask(pin);
    mode = portModeRegister(_port);
    output = portOutputRegister(_port);
    
    *mode |= _bitmask;
    *output &= ~_bitmask;
    wait((((model < DHT21) ? 20 : 1) * 1000) << (1 + (F_CPU >= 16000000L)));
    *output |= _bitmask;
    *mode &= ~_bitmask;

    if (!pulse(_bitmask)) return SDHT_NOTICE_ERROR_CONNECT;
    if (!pulse(0)) return SDHT_NOTICE_ERROR_REQUEST;
    if (!pulse(_bitmask)) return SDHT_NOTICE_ERROR_RESPONSE;

    for (uint8_t i = 0; i < 40; i++) {
      if (!(buffer = pulse(0))) return SDHT_NOTICE_ERROR_LOW(i);
      if (!(signal = pulse(_bitmask))) return SDHT_NOTICE_ERROR_HIGH(i);
      data[i / 8] += data[i / 8] + (signal > buffer);
    }

    if (data[4] != uint8_t(data[0] + data[1] + data[2] + data[3])) return SDHT_NOTICE_ERROR_PARITY;

    switch (model) {
      case DHT11:
        _humidity = data[0] + (data[1] * .1);
        _celsius = data[2] + (data[3] * .1);
        break;

      case DHT12:
        _humidity = data[0] + (data[1] * .1);
        _celsius = (data[2] + ((data[3] & 0x7F) * .1)) * ((data[3] & 0x80) ? -1 : 1);
        break;

      case DHT21:
      case DHT22:
        _humidity = word(data[0], data[1]) * .1;
        _celsius = word((data[2] & 0x7F), data[3]) * ((data[2] & 0x80) ? -.1 : .1);
        break;
    }
  }
  return SDHT_NOTICE_OK;
}

double SDHT::fahrenheit(double celsius) {
  return celsius * 1.8 + 32;
}

double SDHT::heatIndex(double humidity, double celsius) {

  celsius = fahrenheit(celsius);
  double heatIndex = (0.5 * (celsius + 61.0 + ((celsius - 68.0) * 1.2) + (humidity * 0.094)));

  if (heatIndex > 79) {
    heatIndex =
      - 42.37900000
      + 02.04901523 * celsius
      + 10.14333127 * humidity
      - 00.22475541 * celsius * humidity
      - 00.00683783 * pow(celsius, 2)
      - 00.05481717 * pow(humidity, 2)
      + 00.00122874 * pow(celsius, 2) * humidity
      + 00.00085282 * celsius * pow(humidity, 2)
      - 00.00000199 * pow(celsius, 2) * pow(humidity, 2)
      ;

    if ((humidity < 13.0) && (celsius >= 80.0) && (celsius <= 112.0))
      heatIndex -= ((13.0 - humidity) * 0.25) * sqrt((17.0 - abs(celsius - 95.0)) * 0.05882);
    else if ((humidity > 85.0) && (celsius >= 80.0) && (celsius <= 87.0))
      heatIndex += ((humidity - 85.0) * 0.1) * ((87.0 - celsius) * 0.2);
  }
  return (heatIndex - 32) / 1.8;
}

uint16_t SDHT::pulse(uint8_t bitmask) {
  uint16_t signal = 0;
  while ((*portInputRegister(_port) & _bitmask) == bitmask) if (SDHT_CYCLES < signal++) return 0;
  return signal;
}

void SDHT::wait(uint16_t useconds) {
  interrupts();
  delayMicroseconds(useconds);
  noInterrupts();
}

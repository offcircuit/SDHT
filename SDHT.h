#ifndef SDHT_H
#define SDHT_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#define SDHT_NO_HEAT //UNCOMMENT TO DISABLE HEAT INDEX
//#define SDHT_NO_KELVIN //UNCOMMENT TO DISABLE KELVIN
//#define SDHT_NO_FAHRENHEIT //UNCOMMENT TO DISABLE FAHRENHEIT

#define DHT11 0
#define DHT12 1
#define DHT21 2
#define DHT22 3

#define SDHT_CYCLES microsecondsToClockCycles(1000)

#define SDHT_NULL 0
#define SDHT_OK 1
#define SDHT_LOADED 2
#define SDHT_WRONG_PARITY 3

#define SDHT_ERROR_MODEL -1
#define SDHT_ERROR_PIN -2
#define SDHT_ERROR_CONNECT -3
#define SDHT_ERROR_REQUEST -4
#define SDHT_ERROR_RESPONSE -5
#define SDHT_ERROR_WAIT -6
#define SDHT_ERROR_VALUE -7

class SDHT
{
  private:
    class Temperature {
      public:
        double celsius = 0;

#ifndef SDHT_NO_FAHRENHEIT
        double fahrenheit = 32;
#endif

#ifndef SDHT_NO_KELVIN
        double kelvin = 273.15;
#endif

        setCelsius(double c) {
          celsius = c;

#ifndef SDHT_NO_FAHRENHEIT
          fahrenheit = c * 1.8 + 32;
#endif

#ifndef SDHT_NO_KELVIN
          kelvin = c + 273.15;
#endif
        }

        setFahrenheit(double f) {
          celsius = (f - 32) / 1.8;

#ifndef SDHT_NO_FAHRENHEIT
          fahrenheit = f;
#endif

#ifndef SDHT_NO_KELVIN
          kelvin = (f + 459.67) * 5 / 9;
#endif
        }

        setKelvin(double k) {
          celsius = k - 273.15;

#ifndef SDHT_NO_FAHRENHEIT
          fahrenheit = k * 9 / 5 - 459.67;
#endif

#ifndef SDHT_NO_KELVIN
          kelvin = k;
#endif
        }
    };

    double _humidity = 0;
    uint8_t data[5], _notice = SDHT_NULL, _bit;
    uint16_t _signal;

#ifndef SDHT_NO_HEAT
    Temperature _heat;
#endif

    Temperature _temperature;

    uint8_t read(uint8_t pin, uint8_t msDelay);
    bool pulse(uint8_t port, uint8_t bitmask);

  public:
    const double &humidity = _humidity;
    const uint8_t &notice = _notice;

#ifndef SDHT_NO_HEAT
    const Temperature &heat = _heat;
#endif

    const Temperature &temperature = _temperature;

    explicit SDHT() {};
    uint8_t broadcast(uint8_t pin, uint8_t model);
};

#endif

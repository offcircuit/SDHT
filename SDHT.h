#ifndef SDHT_H
#define SDHT_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DHT11 0
#define DHT12 1
#define DHT21 2
#define DHT22 3

#define SDHT_CYCLES microsecondsToClockCycles(1000)

#define SDHT_NULL 0
#define SDHT_OK 1
#define SDHT_LOADED 2
#define SDHT_WRONG_PARITY 3
#define SDHT_ERROR_MODEL 4
#define SDHT_ERROR_PIN 5
#define SDHT_ERROR_CONNECT 6
#define SDHT_ERROR_REQUEST 7
#define SDHT_ERROR_RESPONSE 8
#define SDHT_ERROR_WAIT 9
#define SDHT_ERROR_VALUE 10

class SDHT
{
  private:
    class Temperature {
      public:
        const double &celsius = _celsius, &fahrenheit = _fahrenheit, &kelvin = _kelvin;

        setCelsius(double c) {
          _celsius = c;
          _fahrenheit = c * 1.8 + 32;
          _kelvin = c + 273.15;
        }

        setFahrenheit(double f) {
          _celsius = (f - 32) / 1.8;
          _fahrenheit = f;
          _kelvin = (f + 459.67) * 5 / 9;
        }

        setKelvin(double k) {
          _celsius = k - 273.15;
          _fahrenheit = k * 9 / 5 - 459.67;
          _kelvin = k;
        }

      private:
        double _celsius = 0, _fahrenheit = 32, _kelvin = 273.15;
    };

    double _humidity = 0;
    uint8_t data[5], _notice = SDHT_NULL, _pin, _port, _bit;
    uint16_t _signal;

    Temperature _heat, _temperature;

    uint8_t read(uint16_t msDelay);
    bool pulse(uint8_t b);

  public:
    const double &humidity = _humidity;
    const uint8_t &notice = _notice;
    Temperature &heat = _heat, &temperature = _temperature;

    explicit SDHT() {};
    uint8_t broadcast(uint8_t pin, uint8_t model);
};

#endif

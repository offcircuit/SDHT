#ifndef SDHT_H
#define SDHT_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DHT11 11
#define DHT12 12
#define DHT21 21
#define DHT22 22

#define SDHT_OK 1
#define SDHT_ERR_CHECKSUM 2
#define SDHT_ERR_TIMEOUT 3
#define SDHT_ERR_CONNECT 4
#define SDHT_ERR_ACK_L 5
#define SDHT_ERR_ACK_H 6
#define SDHT_ERR_MODEL 7


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
    uint8_t buffer[5] = {0, 0, 0, 0, 0};
    uint8_t _notice = SDHT_OK;
    Temperature _heat, _temperature;

    uint8_t checksum();
    bool read(uint8_t p, uint8_t m);

  public:
    const double &humidity = _humidity;
    const uint8_t &notice = _notice;
    Temperature &heat = _heat, &temperature = _temperature;;

    SDHT(uint8_t p, uint8_t m);
};

#endif

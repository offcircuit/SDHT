#ifndef TEMPERATURE_H
#define TEMPERATURE_H

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

class Temperature {
  public:
    const double &celsius = _celsius, &fahrenheit = _fahrenheit, &kelvin = _kelvin;

    setCelsius(double c);
    setFahrenheit(double f);
    setKelvin(double k);

  private:
    double _celsius = 0, _fahrenheit = 32, _kelvin = 273.15;
};

class SDHT
{
  public:
    const double &humidity = _humidity;
    const uint8_t &notice = _notice;
    Temperature &heat = _heat, &temperature = _temperature;;

    SDHT(uint8_t pin, uint8_t model);

  private:
    uint8_t buffer[5] = {0, 0, 0, 0, 0};
    double _humidity = 0;
    uint8_t _notice = SDHT_OK;
    Temperature _heat, _temperature;

    uint8_t checksum();
    bool read(uint8_t _pin, uint8_t _model);
};

#endif
#endif

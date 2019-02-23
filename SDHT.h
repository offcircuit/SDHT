#ifndef SDHT_H
#define SDHT_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DHT22 22
#define DHT11 11

#define DHTLIB_OK                   0
#define DHTLIB_ERROR_CHECKSUM       -1
#define DHTLIB_ERROR_TIMEOUT        -2
#define DHTLIB_ERROR_CONNECT        -3
#define DHTLIB_ERROR_ACK_L          -4
#define DHTLIB_ERROR_ACK_H          -5

#define DHTLIB_DHT11_WAKEUP         18
#define DHTLIB_DHT_WAKEUP           1

#define DHTLIB_DHT11_LEADING_ZEROS  1
#define DHTLIB_DHT_LEADING_ZEROS    6

// max timeout is 100 usec.
// For a 16 Mhz proc 100 usec is 1600 clock cycles
// loops using DHTLIB_TIMEOUT use at least 4 clock cycli
// so 100 us takes max 400 loops
// so by dividing F_CPU by 40000 we "fail" as fast as possible
#define DHTLIB_TIMEOUT 400 // (F_CPU/40000)

class SDHT
{
  public:
    const double &humidity = _humidity;
    const double &temperature = _temperature;
    const byte &notice = _notice;

    SDHT(byte model, byte pin);
    
  private:
    uint8_t buffer[5];
    int _notice = 0;
    double _humidity = 0;
    double _temperature = 0;
    byte reading(byte _model, byte _pin);
};

#endif

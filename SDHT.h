#ifndef SDHT_H
#define SDHT_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DHT22 22
#define DHT11 11

#define SDHT_OK 0
#define SDHT_ERROR_CHECKSUM -1
#define SDHT_ERROR_TIMEOUT -2
#define SDHT_ERROR_CONNECT -3
#define SDHT_ERROR_ACK_L -4
#define SDHT_ERROR_ACK_H -5
#define SDHT_ERROR_MODEL -6

#define SDHT_TIMEOUT 400

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
    byte read(byte model, byte pin);
};

#endif

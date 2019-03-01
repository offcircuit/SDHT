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

#define SDHT_CYCLES microsecondsToClockCycles(200)

#ifdef ESP8266
#define SDHT_NOTICE(i) ({xt_wsr_ps(SDHT_REG_STATE); i;})
#else
#define SDHT_NOTICE(i) ({SREG = SDHT_REG_STATE; i;})
#endif

#define SDHT_OK 1
#define SDHT_ERROR_PARITY 0
#define SDHT_ERROR_MODEL -1
#define SDHT_ERROR_PIN -2
#define SDHT_ERROR_CONNECT -3
#define SDHT_ERROR_REQUEST -4
#define SDHT_ERROR_RESPONSE -5
#define SDHT_ERROR_LOW -6
#define SDHT_ERROR_HIGH -7

#define SDHT_NOTICE_OK SDHT_NOTICE(SDHT_OK)
#define SDHT_NOTICE_ERROR_PARITY SDHT_NOTICE(SDHT_ERROR_PARITY)
#define SDHT_NOTICE_ERROR_MODEL SDHT_ERROR_MODEL
#define SDHT_NOTICE_ERROR_PIN SDHT_ERROR_PIN
#define SDHT_NOTICE_ERROR_CONNECT SDHT_NOTICE(SDHT_ERROR_CONNECT)
#define SDHT_NOTICE_ERROR_REQUEST SDHT_NOTICE(SDHT_ERROR_REQUEST)
#define SDHT_NOTICE_ERROR_RESPONSE SDHT_NOTICE(SDHT_ERROR_RESPONSE)
#define SDHT_NOTICE_ERROR_LOW(i) SDHT_NOTICE(SDHT_ERROR_LOW - (i * 2))
#define SDHT_NOTICE_ERROR_HIGH(i) SDHT_NOTICE(SDHT_ERROR_HIGH - (i * 2))

class SDHT
{
  public:
    const double &humidity = _humidity, &celsius = _celsius;

    explicit SDHT() {};
    
    int8_t broadcast(uint8_t model, uint8_t pin);
    double fahrenheit(double celsius);
    double heatIndex(double humidity, double celsius);
    
  private:
    double _humidity = 0, _celsius = 0;
    uint8_t _bitmask, _port;

    uint16_t pulse(uint8_t bitmask);
    void wait(uint16_t useconds);
};

#endif

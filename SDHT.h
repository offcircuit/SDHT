#ifndef SDHT_H
#define SDHT_H

#define DHT11 0
#define DHT12 1
#define DHT21 2
#define DHT22 3

#define SDHT_CYCLES microsecondsToClockCycles(200)

class SDHT
{
  private:
    double _humidity = 0, _celsius = 0;

  public:
    const double &humidity = _humidity, &celsius = _celsius;

    explicit SDHT() {};

    bool broadcast(uint8_t model, uint8_t pin) {
      uint8_t data[5] = {0, 0, 0, 0, 0};

#ifdef ESP8266
      yield();
#endif

      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      pulseIn(pin, HIGH, (model < DHT21) ? 20000 : 1000);
      digitalWrite(pin, HIGH);
      pinMode(pin, INPUT);

      if (!pulseIn(pin, HIGH, 300)) return false;
      for (uint8_t i = 0; i < 40; i++) data[i / 8] += data[i / 8] + (pulse(pin, LOW) > pulse(pin, HIGH));

      if (data[4] != uint8_t(data[0] + data[1] + data[2] + data[3])) return false;

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
          _humidity = (uint16_t(data[0] << 8) | data[1]) * .1;
          _celsius = (uint16_t((data[2] & 0x7F)) | data[3]) * ((data[2] & 0x80) ? -.1 : .1);
          break;

        default: return false;
      }
      return true;
    }

    uint16_t pulse(uint8_t pin, int state) {
      uint16_t cycle = SDHT_CYCLES;
      while ((digitalRead(pin) == state) && cycle--);
      return cycle;
    }
};

#endif

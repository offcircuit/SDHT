#ifndef LCDIC2_H
#define LCDIC2_H

#include "Wire.h"

#define LCDIC2_MODE         0b100
#define LCDIC2_DISPLAY      0b1000
#define LCDIC2_MOVE         0b10000
#define LCDIC2_FUNCTION     0b100000
#define LCDIC2_CGRAM        0b1000000
#define LCDIC2_DDRAM        0b10000000

#define LCDIC2_BITS_4       0b0
#define LCDIC2_BITS_8       0b10000

#define LCDIC2_LINES_1      0b0
#define LCDIC2_LINES_2      0b1000

#define LCDIC2_DOTS_8       0b0
#define LCDIC2_DOTS_10      0b100

#define LCDIC2_CURSOR       0b0000
#define LCDIC2_SHIFT        0b1000

#define LCDIC2_LEFT         0b000
#define LCDIC2_RIGHT        0b100

#define LCDIC2_DEC          0b0
#define LCDIC2_INC          0b1

class LCDIC2 {
  private:
    bool _blink = false, _cursor = true, _display = true, _gain = LCDIC2_INC, _shift = false;
    uint8_t _address, _height, _width;
    bool flag();
    void wait(uint16_t us);
    bool write(uint8_t data, uint8_t rs = 0);
    bool writeCommand(uint8_t registry, uint16_t us = 0);
    void writeData(uint8_t data, uint8_t rs = 0);
    void writeHigh(uint8_t data, uint8_t rs = 0);
    void writeLow(uint8_t data, uint8_t rs = 0);
    bool writeMessage(uint8_t registry, uint8_t data);

  public:
    LCDIC2(uint8_t address, uint8_t width, uint8_t height);
    bool begin();
    bool backlight(bool state);
    bool blink(bool state);
    bool clear();
    bool cursor(bool state);
    bool cursor(uint8_t x, uint8_t y);
    bool cursorLeft();
    bool cursorRight();
    bool display(bool state);
    bool glyph(uint8_t character);
    bool glyph(uint8_t id, uint8_t map[]);
    bool home();
    bool leftToRight();
    bool moveLeft();
    bool moveRight();
    size_t print(String data);
    bool rightToLeft();
    bool shift(bool state);
};

#endif

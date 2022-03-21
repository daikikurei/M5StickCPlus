// https://wpdbox.blogspot.com/2015/12/blog-post.html
// https://gitefu.github.io/2019/12/05/%E5%88%9D%E3%82%81%E3%81%A6%E3%81%AE%E8%87%AA%E4%BD%9Carduino%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9%E3%83%AA/

#ifndef tof_hat_h
#define tof_hat_h

#include "Arduino.h"

class tof_hat
{
public:
  tof_hat();
  uint16_t measure_distance();

private:
  byte gbuf[16];
  uint16_t bswap(byte b[]);
  uint16_t makeuint16(int lsb, int msb);
  uint16_t VL53L0X_decode_vcsel_period(short vcsel_period_reg);
  void write_byte_data(byte data);
  void write_byte_data_at(byte reg, byte data);
  void write_word_data_at(byte reg, uint16_t data);
  byte read_byte_data();
  byte read_byte_data_at(byte reg);
  uint16_t read_word_data_at(byte reg);
  void read_block_data_at(byte reg, int sz);
};

#endif
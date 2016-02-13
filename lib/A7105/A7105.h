#ifndef __A7105_H__
#define __A7105_H__

#include <Arduino.h>
#include <Streaming.h>
#include <SPI.h>

#include "constants.h"

#define u8 uint8_t

class A7105 {
public:
  A7105(u8 _cs_pin);

  void begin();
  
  void write_register(u8 reg, u8 data);
  u8 read_register(u8 reg);

  void dump_registers();

  void reset();

private:
  inline void t_start() {digitalWrite(cs_pin, LOW);}
  inline void t_end() {digitalWrite(cs_pin, HIGH);}

  void init_registers();

  u8 cs_pin;
};

#endif

#ifndef __A7105_H__
#define __A7105_H__

#include <Arduino.h>
#include <Streaming.h>
#include <SPI.h>

#include "constants.h"

#define u8 uint8_t

inline void panic(char* msg) {
  Serial << "PANIC (" << msg << ")!" << endl <<" HANGING." << endl;
  while (true) {}
}

class A7105 {
public:
  A7105(u8 _cs_pin);

  void begin();
  void reset();

  void dump_registers();

private:
  inline void t_start() {digitalWrite(cs_pin, LOW);}
  inline void t_end() {digitalWrite(cs_pin, HIGH);}

  void init_registers();

  void write_register(u8 reg, u8 data);
  u8 read_register(u8 reg);

  u8 cs_pin;
};

#endif

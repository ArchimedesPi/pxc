#ifndef __A7105_H__
#define __A7105_H__

#include <Arduino.h>
#include <Streaming.h>
#include <SPI.h>

#include "constants.h"

#define u8 uint8_t
#define u32 uint32_t

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

  void set_channel(u8 channel);
  void set_tx_power(u8 powerlevel);
  void write_id(u32 id);
  void write_data(u8 *buffer, unsigned int len);
  void read_data(u8 *buffer, unsigned int len);

  void write_register(u8 reg, u8 data);
  u8 read_register(u8 reg);
  void strobe(u8 state);


private:
  inline void t_start() {digitalWrite(cs_pin, LOW);}
  inline void t_end() {digitalWrite(cs_pin, HIGH);}

  void init_registers();
  void calibrate_if();
  void calibrate_vco(u8 channel);

  u8 cs_pin;
};

#endif

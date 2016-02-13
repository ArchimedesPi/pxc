#include "A7105.h"


A7105::A7105(u8 _cs_pin) : cs_pin(_cs_pin) {}

void A7105::begin() {
  pinMode(cs_pin, OUTPUT);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  reset(); // soft-reset the chip

  write_register(A7105_REG_GIO1S, A7105_ENABLE_4WIRE); // enable 4-wire spi

  init_registers();
}

void A7105::init_registers() {
  write_register(A7105_REG_MODE_CONTROL, 0x63); // various radio options
  write_register(A7105_REG_FIFO_1, 0x0f);
  write_register(A7105_REG_CLOCK, 0x05);

  Serial << "reg(clock) = " << read_register(A7105_REG_CLOCK) << endl;
  if (read_register(A7105_REG_CLOCK) == 0x05) {
    Serial << "chip and wiring are sane." << endl;
  } else {
    panic("chip is not returning sane values. check your wiring.");
  }

  write_register(A7105_REG_DATA_RATE, 0x04);
  write_register(A7105_REG_TX_II, 0x2b);
  write_register(A7105_REG_RX, 0x62);
  write_register(A7105_REG_RX_GAIN_I, 0x80);
  write_register(A7105_REG_RX_GAIN_IV, 0x0A);
  write_register(A7105_REG_CODE_I, 0x07);
  write_register(A7105_REG_CODE_II, 0x17);
  write_register(A7105_REG_RX_DEM_TEST, 0x47);
}

// we use deviation's specs here, instead of the datasheet. this is a bit wtfy but it works.
void A7105::calibrate_if() {
  Serial << "Calibrating IF bank: ";

  write_register(A7105_REG_CALIBRATION, 0b001);

  int tries = 0;
  do {
    Serial << "*";
    if (tries >= 3)
      panic("IF calibration timed out!");

    delay(1);
    tries += 1;
  } while (!!read_register(A7105_REG_CALIBRATION));

  if (read_register(A7105_REG_IF_CALIBRATION_I) & 0b1000 != 0) {
    Serial << _HEX(read_register(A7105_REG_IF_CALIBRATION_I)) << endl;
    panic("IF calibration failed");
  }

  Serial << " [DONE]" << endl;
}

void A7105::calibrate_vco(u8 channel) {
  Serial << "Calibrating VCO channel " << _HEX(channel) << " ";

  set_channel(channel);

  write_register(A7105_REG_CALIBRATION, 0b010);

  int tries = 0;
  do {
    Serial << "*";
    if (tries >= 3)
      panic("VCO calibration timed out!");

    delay(1);
    tries += 1;
  } while ((read_register(A7105_REG_CALIBRATION) & 0b010) != 0);

  if ((read_register(A7105_REG_VCO_CALIBRATION_I) & 0b1000) != 0)
    panic("VCO calibration failed");

  Serial << " [DONE]";
}

void A7105::set_channel(u8 channel) {
  Serial << "set_channel(" << _HEX(channel) << ")" << endl;
  write_register(A7105_REG_PLL_I, channel);
}

// -- debugging
void A7105::dump_registers() {
  for (int reg=0x00; reg<=0xff; reg++) {
    Serial << "reg(" << _HEX(reg) << ") = " << read_register(reg) << endl;
  }
}



// -- low-level device communication
void A7105::reset() {
  write_register(A7105_REG_MODE, 0x00); // soft reset
}

void A7105::write_register(u8 reg, u8 data) {
  t_start();
  SPI.transfer(reg);
  SPI.transfer(data);
  t_end();
}

u8 A7105::read_register(u8 reg) {
  u8 data;

  t_start();
  SPI.transfer(0x40 | reg);
  data = SPI.transfer(0);
  t_end();

  return data;
}

void A7105::strobe(u8 state) {
  Serial << "strobe(" << _HEX(state) << ")" << endl;

  t_start();
  SPI.transfer(state);
  t_end();
}

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

  strobe(A7105_STATE_STANDBY);

  delay(10);

  calibrate_if();
  delay(1);
  calibrate_vco(0x00);
  delay(1);
  calibrate_vco(0xa0);

  strobe(A7105_STATE_STANDBY);

  set_tx_power(A7105_TXPOWER_30mW);
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
  Serial << "Calibrating IF bank: " << endl;

  write_register(A7105_REG_CALIBRATION, 0b001);

  read_register(A7105_REG_CALIBRATION);

  unsigned long ms = millis();
  while (millis() - ms < 500) {
    if (!read_register(A7105_REG_CALIBRATION)) break;
  }
  if (millis() - ms >= 500) {
    panic("IF calibration timed out!");
  }

  u8 calib_result = read_register(A7105_REG_IF_CALIBRATION_I);
  read_register(0x24);
  Serial << "IF calib result: 0x" << _HEX(calib_result) << "/0b" << _BIN(calib_result) << endl;

  if (calib_result & (1 << 4)) {
    panic("IF calibration failed");
  }

  Serial << " [DONE]" << endl;
}

void A7105::calibrate_vco(u8 channel) {
  Serial << "Calibrating VCO channel 0x" << _HEX(channel) << endl;

  set_channel(channel);

  write_register(A7105_REG_CALIBRATION, 0b010);

  unsigned long ms = millis();
  while(millis() - ms < 500) {
    if (!read_register(A7105_REG_CALIBRATION)) break;
  }
  if (millis() - ms >= 500) {
    panic("VCO calibration timed out!");
  }

  u8 calib_result = read_register(A7105_REG_VCO_CALIBRATION_I);
  if (calib_result & (1 << 3)) {
    panic("VCO calibration failed");
  }

  Serial << " [DONE]" << endl;
}

void A7105::set_channel(u8 channel) {
  Serial << "set_channel(" << _HEX(channel) << ")" << endl;
  write_register(A7105_REG_PLL_I, channel);
}

void A7105::set_tx_power(u8 power) {
  Serial << "Setting TX power to level " << power << " (internal enum id). ";
  u8 pac, tbg;
  switch(power) {
    case 0: pac = 0; tbg = 0; break;
    case 1: pac = 0; tbg = 1; break;
    case 2: pac = 0; tbg = 2; break;
    case 3: pac = 0; tbg = 4; break;
    case 4: pac = 1; tbg = 5; break;
    case 5: pac = 2; tbg = 7; break;
    case 6: pac = 3; tbg = 7; break;
    case 7: pac = 3; tbg = 7; break;
    default: pac = 0; tbg = 0; break;
  };
  Serial << "TX power registers: pac = 0x" << _HEX(pac) << ", tbg = 0x" << _HEX(tbg) << endl;
  write_register(A7105_REG_TX_TEST, (pac << 3) | tbg);
}

void A7105::write_id(u32 id) {
  t_start();
  SPI.transfer(A7105_REG_ID);
  SPI.transfer((id >> 24) & 0xFF);
  SPI.transfer((id >> 16) & 0xFF);
  SPI.transfer((id >> 8) & 0xFF);
  SPI.transfer((id >> 0) & 0xFF);
  t_end();
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
  t_start();
  SPI.transfer(state);
  t_end();
}

void A7105::write_data(u8 *buffer, unsigned int len) {
  t_start();
  SPI.transfer(A7105_STATE_RESET_WRITE_POINTER);
  SPI.transfer(A7105_FIFO_START);

  for (unsigned int i = 0; i < len; i++) {
    SPI.transfer(buffer[i]);
  }
  t_end();
}

void A7105::read_data(u8 *buffer, unsigned int len) {
  strobe(A7105_STATE_RESET_READ_POINTER);

  for (unsigned int i; i < len; i++) {
    buffer[i] = read_register(A7105_FIFO_START);
  }
}

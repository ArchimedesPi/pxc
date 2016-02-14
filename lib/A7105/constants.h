#ifndef __A7105_ADDRESSES_H__
#define __A7105_ADDRESSES_H__

enum A7105_Register {
  // reset, etc
  A7105_REG_MODE = 0x00,
  // used to set transmitter options
  A7105_REG_MODE_CONTROL = 0x01,
  // used to select calibration mode
  A7105_REG_CALIBRATION = 0x02,
  // used to set the FIFO end pointer (FEP)
  A7105_REG_FIFO_1 = 0x03,
  // used to set transmitter ID
  A7105_REG_ID = 0x06,
  // enables 4-wire SPI
  A7105_REG_GIO1S = 0x0b,
  // clock settings
  A7105_REG_CLOCK = 0x0d,
  // controls data rate division
  A7105_REG_DATA_RATE = 0x0e,
  // channel number select
  A7105_REG_PLL_I = 0x0f,
  // controls frequency deviation
  A7105_REG_TX_II = 0x15,
  // controls receiver settings
  A7105_REG_RX = 0x18,
  // more receiver settings
  A7105_REG_RX_GAIN_I = 0x19,
  // reserved constants
  A7105_REG_RX_GAIN_IV = 0x1C,
  // encoding settings
  A7105_REG_CODE_I = 0x1F,
  // more encoding settings
  A7105_REG_CODE_II = 0x20,
  // contains flag for checking IF calibration
  A7105_REG_IF_CALIBRATION_I = 0x22,
  // contains flag for checking VCO calibration
  A7105_REG_VCO_CALIBRATION_I = 0x25,
  // TX power settings
  A7105_REG_TX_TEST = 0x28,
  // RX demodulator settings
  A7105_REG_RX_DEM_TEST = 0x29,
  // value written to GIO1S to enable 4-wire SPI
  A7105_ENABLE_4WIRE = 0x19,
};

// -- states
enum A7105_State {
  A7105_STATE_SLEEP                = 0x80,
  A7105_STATE_IDLE                 = 0x90,
  A7105_STATE_STANDBY              = 0xA0,
  A7105_STATE_PLL                  = 0xB0,
  A7105_STATE_RX                   = 0xC0,
  A7105_STATE_TX                   = 0xD0,
  A7105_STATE_RESET_WRITE_POINTER  = 0xE0,
  A7105_STATE_RESET_READ_POINTER   = 0xF0,
};


#endif

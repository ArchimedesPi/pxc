#ifndef __A7105_ADDRESSES_H__
#define __A7105_ADDRESSES_H__

// reset, etc
#define A7105_REG_MODE 0x00
// used to set transmitter options
#define A7105_REG_MODE_CONTROL 0x01
// used to select calibration mode
#define A7105_REG_CALIBRATION 0x02
// used to set the FIFO end pointer (FEP)
#define A7105_REG_FIFO_1 0x03
// used to set transmitter ID
#define A7105_REG_ID 0x06
// enables 4-wire SPI
#define A7105_REG_GIO1S 0x0b
// clock settings
#define A7105_REG_CLOCK 0x0d
// controls data rate division
#define A7105_REG_DATA_RATE 0x0e
// channel number select
#define A7105_REG_PLL_I 0x0f
// controls frequency deviation
#define A7105_REG_TX_II 0x15
// controls receiver settings
#define A7105_REG_RX 0x18
// more receiver settings
#define A7105_REG_RX_GAIN_I 0x19
// reserved constants
#define A7105_REG_RX_GAIN_IV 0x1C
// encoding settings
#define A7105_REG_CODE_I 0x1F
// more encoding settings
#define A7105_REG_CODE_II 0x20
// contains flag for checking IF calibration
#define A7105_REG_IF_CALIBRATION_I 0x22
// contains flag for checking VCO calibration
#define A7105_REG_VCO_CALIBRATION_I 0x25
// TX power settings
#define A7105_REG_TX_TEST 0x28
// RX demodulator settings
#define A7105_REG_RX_DEM_TEST 0x29

#define A7105_ENABLE_4WIRE 0x19 // value written to GIO1S to enable 4-wire SPI

#endif

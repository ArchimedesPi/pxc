#include "Hubsan.h"

Hubsan::Hubsan(A7105& _radio): radio(_radio) {}

const u32 Hubsan::ID = 0x55201041;
const u8 Hubsan::ALLOWED_CHANNELS[] = {0x14, 0x1e, 0x28, 0x32, 0x3c, 0x46, 0x50, 0x5a, 0x64, 0x6e, 0x78, 0x82}; 
const u32 Hubsan::TX_ID = 0xdb042679;

void Hubsan::begin() {
}

void Hubsan::bind(u32 sessionid_, u8 channel_) {
  Serial << "Resuming session (id: " << sessionid << ", channel: " << _HEX(channel) << ")." << endl;
  sessionid = sessionid_;
  channel = channel_;
  radio.write_id(sessionid);
  radio.set_channel(channel);
}

void Hubsan::bind() {
  Serial << "New binding started." << endl;
  sessionid = randu32();
  channel = randomsample(ALLOWED_CHANNELS, sizeof(ALLOWED_CHANNELS)/sizeof(u8));
  Serial << "Picking channel " << _HEX(channel) << " with session ID " << _HEX(sessionid) << "." << endl;
  radio.write_id(ID);
  radio.set_channel(channel);
  Serial << "Beginning handshake: ";
  handshake();
  delay(500);
  Serial << "Yay, binding complete!" << endl;

  radio.write_register(A7105_REG_CODE_I, 0x0f);
}



// -- low level comms
void Hubsan::send_packet(u8 *packet, unsigned int len) {
  radio.strobe(A7105_STATE_STANDBY);
  radio.write_data(packet, len);
  radio.strobe(A7105_STATE_TX);
  delay(2);

  for (int i=0; i < 4; i++) {
    digitalWrite(A0, HIGH);

    Serial << "mode reg: " << _HEX(radio.read_register(A7105_REG_MODE)) << endl;
    if (radio.read_register(A7105_REG_MODE) & 1 == 0) {
      return;
    }
    delay(2);
  }

  panic("Sending packet did not complete.");
}

void Hubsan::handshake() {
  // TODO: figure out how to do this more elegantly
  u8 scratch_response[16] = {0};

  // phase 1 of the handshake
  while (true) {
    //Serial << ".";

    u8 status = 0;

    status |= bind_stage(1, scratch_response);

    u8 s4_response[16] = {0};
    status |= bind_stage(3, s4_response);

    // pull out the ID
    u32 id = 0;
    id |= s4_response[2] << 24;
    id |= s4_response[3] << 16;
    id |= s4_response[4] << 8;
    id |= s4_response[5];
    radio.write_id(id);

    status |= bind_stage(1, scratch_response);

    if (status == 0)
      break; // we got through the above without having any trouble.
    // otherwise just keep on trying :)
  }

  // phase 2 of the handshake
  while (true) {
    //Serial << ".";

    u8 p2_response[16] = {0};
    if (p2_response[1] == 0x09)
      break;
  }

  Serial << endl;
}

u8 Hubsan::bind_stage(u8 state, u8* response) {
  u8 packet[16];
  packet[0] = state;
  packet[1] = channel;
  packet[2] = (sessionid >> 24) & 0xff;
  packet[3] = (sessionid >> 16) & 0xff;
  packet[4] = (sessionid >>  8) & 0xff;
  packet[5] = (sessionid >>  0) & 0xff;
  for (int i=0; i < 9; i++)
    packet[6+i] = 0;
  packet[15] = calc_checksum(packet, 16); // checksum at the end

  send_packet(packet, 16);
  radio.strobe(A7105_STATE_RX);

  unsigned long sendtime = millis();
  while (sendtime < (sendtime + 15)) {
    if (radio.read_register(A7105_REG_MODE) & 1 == 0) {
      radio.read_data(response, 16);
      if ((response[0] == 0xe0) || (response[0] = 0xe1)) {
        return 1;
      }

      return 0;
    }
  }
  return 2;
}



// -- utilities
u32 Hubsan::randu32() {
  u32 x = rand() & 0xff;
  x |= (rand() & 0xff) << 8;
  x |= (rand() & 0xff) << 16;
  x |= (rand() & 0xff) << 24;

  return x;
}

u8 Hubsan::calc_checksum(u8 *packet, unsigned int len) {
  u8 accumulator = 0;
  for (unsigned int i=0; i < len; i++) {
    accumulator += packet[i];
  }

  return ((256 - (accumulator % 256)) & 0xff);
}

u8 Hubsan::randomsample(const u8 *values, unsigned int len) {
  // get a badly distributed (but wtf we don't really care) random index
  // and return that index in *values.
  return values[rand() % len];
}

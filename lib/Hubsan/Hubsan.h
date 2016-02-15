#ifndef __HUBSAN_H__
#define __HUBSAN_H__

#include <A7105.h>

class Hubsan {
public:
  Hubsan(A7105& radio);

  void begin();

  void safety();

  void control_raw(u8 throttle, u8 rudder, u8 elevator, u8 aileron, bool leds, bool flips);
  void control(double throttle, double rudder, double elevator, double aileron, bool leds, bool flips);

  void send_packet(u8 *packet, unsigned int len);
  u8 bind_stage(u8 state, u8 *response);
  void handshake();
  void bind(u32 session_id, u8 channel);
  void bind();

private:
  A7105& radio;

  double lerp(double t, double min, double max);
  double lerp_random(double t, double min, double max);

  u8 channel;
  u32 sessionid;

  static u32 randu32();
  static u8 randomsample(const u8 *values, unsigned int len);
  static u8 calc_checksum(u8 *packet, unsigned int len);

  static const u32 ID, TX_ID;
  static const u8 ALLOWED_CHANNELS[];
};

#endif

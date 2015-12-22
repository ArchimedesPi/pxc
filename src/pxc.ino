#include <A7105.h>
#include <Streaming.h>

A7105 radio(10); // CS pin is 10

void setup() {
  radio.begin();
  radio.dump_registers();
}

void loop() {
}

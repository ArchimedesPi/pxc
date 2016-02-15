#include <A7105.h>
#include <Hubsan.h>
#include <Streaming.h>

A7105 radio(10); // CS pin is 10
Hubsan quad(radio);

void setup() {
  Serial.begin(115200);
  srand(analogRead(A0));

  radio.begin();
  quad.begin();
  quad.bind();
  //radio.dump_registers();
}

void loop() {
}

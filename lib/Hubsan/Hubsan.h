#ifndef __HUBSAN_H__
#define __HUBSAN_H__

#include <A7105.h>

class Hubsan {
public:
  Hubsan(A7105& radio);

  void begin();
private:
  A7105& radio;
};

#endif

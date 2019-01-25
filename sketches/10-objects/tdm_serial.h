#pragma once

#include <stdarg.h>

/*
 * resulting strings limited to 128 chars
 */
#define SERIAL_MAX_PRINTF 128
#define SERIAL_BAUD_RATE 9600


namespace tdm {
  
  class Serial {
  
    public:
    static void setup(void) {
      ::Serial.begin(SERIAL_BAUD_RATE);    
    };
    
    static void printf(char *fmt, ... ) {
      char buf[SERIAL_MAX_PRINTF];
      va_list args;
      va_start (args, fmt );
      vsnprintf(buf, 128, fmt, args);
      va_end (args);
      ::Serial.print(buf);
    };

  }; //class

}; //namespace

/*
 * In a more traditional environments, one would split into .H and .CPP files. However in Arduino, the ".ino" file
 * comes with certain built-in includes (such as "Arduino.h" and more) which are not automatically added to other CPP
 * files. So, to make things more seamless, I will be putting implementation directly into the header files, which may
 * look unnatural to C++ purists
 */
#pragma once

#include <stdarg.h>

/*
 * resulting strings limited to 128 chars
 */
#define TDM_SERIAL_MAX_PRINTF 128
#define TDM_SERIAL_BAUD_RATE 9600


namespace tdm {

  /*
   * An enhanced serial class that supports things like "printf"
   */
  class Serial {

    private:
      bool _initialized = false;
  
    public:
      /*
       * initialize for first use
       */
      void setup(void) {
        if (!_initialized) {
          ::Serial.begin(TDM_SERIAL_BAUD_RATE);
          _initialized = true;
        }      
      };

      /*
       * Based on ideas from https://playground.arduino.cc/main/printf
       */
      void printf(char *fmt, ... ) {
        char buf[TDM_SERIAL_MAX_PRINTF];
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        ::Serial.print(buf);
      };

  }; //class

}; //namespace


 /*
   * a global instance for shared use
   */
  tdm::Serial _STDOUT;

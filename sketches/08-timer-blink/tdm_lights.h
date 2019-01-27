/*
 * In a more traditional environments, one would split into .H and .CPP files. However in Arduino, the ".ino" file
 * comes with certain built-in includes (such as "Arduino.h" and more) which are not automatically added to other CPP
 * files. So, to make things more seamless, I will be putting implementation directly into the header files, which may
 * look unnatural to C++ purists
 */
#pragma once

/*
 * code for controlling lights
 */

#define TDM_COLOR_OFF 4095            // magic value turns off a color 
#define TDM_COLOR_ON     0            // magic value turns on a color

namespace tdm {

  /* 
   * describes a RGB color
   */
  typedef struct {
      int rPulse;
      int gPulse;
      int bPulse;
  } rgb_color_t;


  /*
  * some popular colours
  */
  rgb_color_t _COLOR_RED =   {TDM_COLOR_ON,  TDM_COLOR_OFF, TDM_COLOR_OFF};
  rgb_color_t _COLOR_GREEN = {TDM_COLOR_OFF, TDM_COLOR_ON,  TDM_COLOR_OFF};
  rgb_color_t _COLOR_BLUE =  {TDM_COLOR_OFF, TDM_COLOR_OFF, TDM_COLOR_ON};    
  
  /* 
   * Describes a LED device  
   */
  class Led {
    
    private:
      PwmIO _pwm;
      uint8_t _rChannel;  // red PWM channel in range of 0..31
      uint8_t _gChannel;  // green PWM channel in range of 0..31
      uint8_t _bChannel;  // blue PWM channel in range of 0..31

    public:

      /*
       * default led channels are 2,3 and 4, this may vary depending on your wiring
       */
      Led() : Led(2, 3, 4 ) {       
      }
      
      Led(uint8_t rChannel, uint8_t gChannel, uint8_t bChannel) {
        _rChannel = rChannel;
        _gChannel = gChannel;
        _bChannel = bChannel;        
      }

      void setColor(rgb_color_t color) {
        _pwm.setPulse(_rChannel, color.rPulse);
        _pwm.setPulse(_gChannel, color.gPulse);
        _pwm.setPulse(_bChannel, color.bPulse);        
      }  
  }; //class Led
    
} //namespace

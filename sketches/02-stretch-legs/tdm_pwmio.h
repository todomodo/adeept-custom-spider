/*
 * In a more traditional environments, one would split into .H and .CPP files. However in Arduino, the ".ino" file
 * comes with certain built-in includes (such as "Arduino.h" and more) which are not automatically added to other CPP
 * files. So, to make things more seamless, I will be putting implementation directly into the header files, which may
 * look unnatural to C++ purists
 */
#pragma once

#include <Adeept_PWMPCA9685.h>

/*
 * Low level I/O functions and definitions to deal with dual PCA9685 PWM controller
 * most of these are for controlling servos and RGB lights
 */

#define TDM_PWM_FREQUENCY          50  //TowerPro MG90S Servos run at 50 HZ
#define TDM_PWM_PIN_OFF                   0  // magic value turns off a pin 
#define TDM_PWM_CHANNELS_PER_CONTROLLER  16  // we use 16 channel controllers
#define TDM_PWM_NUMBER_OF_CONTROLLERS     2  // there's 2 of them


namespace tdm {

  /*
   * This should have been a static member of the Servo class, however given the fact how statics
   * behave from .H files, we'll keep it global
   */
  Adeept_PWMPCA9685 *_CACHED_ADPWM0, *_CACHED_ADPWM1;

   /*
   * A wrapper around Adeept_PWMPCA9685 library object
   */
  class PwmIO {
  
    public:

      /*
       * initialize for first use
       */
      void setup(void) {
        if (_CACHED_ADPWM0==NULL) {
          _CACHED_ADPWM0 = new Adeept_PWMPCA9685(0x40);
          _CACHED_ADPWM0->begin();
          _CACHED_ADPWM0->setPWMFreq(TDM_PWM_FREQUENCY);  
        }
  
        if (_CACHED_ADPWM1==NULL) {
          _CACHED_ADPWM1 = new Adeept_PWMPCA9685(0x41);
          _CACHED_ADPWM1->begin();
          _CACHED_ADPWM1->setPWMFreq(TDM_PWM_FREQUENCY);   
        }        
      };


      /*
       * set PWM pulse on a channel chosing first or second controller based on the
       * channel number
       */
      void setPulse(int channel, int pulse_length){
        if (channel<TDM_PWM_CHANNELS_PER_CONTROLLER) {
          // use first controller
          _CACHED_ADPWM0->setPWM(channel, TDM_PWM_PIN_OFF, pulse_length);  
        } else {
          // use second controller
          _CACHED_ADPWM1->setPWM(channel-TDM_PWM_CHANNELS_PER_CONTROLLER, TDM_PWM_PIN_OFF, pulse_length); 
        }
      }
      
  }; //class PwmIO

}; //namespace

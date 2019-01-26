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

#define TDM_PWM_SERVO_FREQUENCY          50  //TowerPro MG90S Servos run at 50 HZ
#define TDM_PWM_SERVO_ANGLEMIN            0  //Physical servos rotate between 0 .. 180 degrees
#define TDM_PWM_SERVO_ANGLEMAX          180  //Physical servos rotate between 0 .. 180 degrees
#define TDM_PWM_SERVO_PULSEMIN          130  //PWM pulse between 130 and 600
#define TDM_PWM_SERVO_PULSEMAX          600  //PWM pulse between 130 and 600
#define TDM_PWM_PIN_OFF                   0  // magic value turns off a pin 
#define TDM_PWM_CHANNELS_PER_CONTROLLER  16  // we use 16 channel controllers
#define TDM_PWM_NUMBER_OF_CONTROLLERS     2  // there's 2 of them

namespace tdm {

   /*
   * An enhanced serial class that supports things like "printf"
   */
  class PwmIO {
  
    private:
      Adeept_PWMPCA9685 _pwm0 = Adeept_PWMPCA9685(0x40);
      Adeept_PWMPCA9685 _pwm1 = Adeept_PWMPCA9685(0x40);

    public:

      /*
       * initialize for first use
       */
      void setup(void) {
        _pwm0.begin();
        _pwm0.setPWMFreq(TDM_PWM_SERVO_FREQUENCY);  
        
        _pwm1.begin();
        _pwm1.setPWMFreq(TDM_PWM_SERVO_FREQUENCY);   
      };
  

      /*
       * set PWM pulse on a channel chosing first or second controller based on the
       * channel number
       */
      void setPulse(int channel, int pulse_length){
        if (channel<TDM_PWM_CHANNELS_PER_CONTROLLER) {
          // use first controller
          _pwm0.setPWM(channel, TDM_PWM_PIN_OFF, pulse_length);  
        } else {
          // use second controller
          _pwm1.setPWM(channel-TDM_PWM_CHANNELS_PER_CONTROLLER, TDM_PWM_PIN_OFF, pulse_length); 
        }
      }
      
  }; //class PwmIO

}; //namespace

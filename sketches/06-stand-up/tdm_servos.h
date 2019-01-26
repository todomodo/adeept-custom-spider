/*
 * In a more traditional environments, one would split into .H and .CPP files. However in Arduino, the ".ino" file
 * comes with certain built-in includes (such as "Arduino.h" and more) which are not automatically added to other CPP
 * files. So, to make things more seamless, I will be putting implementation directly into the header files, which may
 * look unnatural to C++ purists
 */
#pragma once

#include <stdarg.h>

#define TDM_SERVO_ANGLEMIN            0  //Physical servos rotate between 0 .. 180 degrees
#define TDM_SERVO_ANGLEMAX          180  //Physical servos rotate between 0 .. 180 degrees
#define TDM_SERVO_PULSEMIN          130  //PWM pulse between 130 and 600
#define TDM_SERVO_PULSEMAX          600  //PWM pulse between 130 and 600
#define TDM_SERVO_MAXCOUNT          TDM_PWM_NUMBER_OF_CONTROLLERS * TDM_PWM_CHANNELS_PER_CONTROLLER


namespace tdm {

  /*
   * This should have been a static member of the Servo class, however given the fact how statics
   * behave from .H files, we'll keep it global
   */
  void* _CACHED_SERVOS[TDM_SERVO_MAXCOUNT];

  /*
   * A class for controling single servo
   */
   class Servo {

    private:
      uint8_t _channel;
      PwmIO _pwm;

      Servo(uint8_t channel) {
        _channel = channel; 
      }


    public:
      /*
       * A static builder for creating & caching servos
       */
      static Servo* build(uint8_t channel) {
        if (channel<TDM_SERVO_MAXCOUNT) {
          if (_CACHED_SERVOS[channel]==NULL) {
            _CACHED_SERVOS[channel] = new Servo(channel);
          }
          return (Servo*) _CACHED_SERVOS[channel];          
        }        
        return NULL;
      }
          
      /* 
       * set servo to certain angle 
       */
      void setAngle(int degrees){
          _pwm.setPulse(_channel, computePulseLength(degrees));
      }
      
      
      /* 
       * turn off the servo
       */
      void turnOff(void) {
        _pwm.setPulse(_channel, TDM_PWM_PIN_OFF);
      }
      

      private:
      
        /* 
         * convert from degrees to PWM pulses 
         */
        int computePulseLength (int degrees){
          if(degrees>=TDM_SERVO_ANGLEMAX){degrees=TDM_SERVO_ANGLEMAX;}
          if(degrees<=TDM_SERVO_ANGLEMIN){degrees=TDM_SERVO_ANGLEMIN;}
          return map(degrees,TDM_SERVO_ANGLEMIN,TDM_SERVO_ANGLEMAX,TDM_SERVO_PULSEMIN,TDM_SERVO_PULSEMAX);
        }

   }; //class Servo



  /*
   * A class for controling multiple servos
   */
   class ServoGroup {

      private:
        uint8_t _size; 
        Servo** _servos;

      public:
        ServoGroup(int numargs, ...) {
          _size = numargs;
          _servos = new Servo*[_size];
          va_list args;
          va_start(args, _size);
          for(int i=0; i<_size; i++) {
            _servos[i] = va_arg(args, Servo*);
          }
          va_end(args);          
        }

       /* 
       * set angles 
       */
        void setAngles(int degrees[]){
          for(int i=0; i < _size; i++) {
            _servos[i]->setAngle(degrees[i]);
          }
        }

        /* 
        * turn off all servos
        */
        void turnOffAllServos(void) {
          for(int i=0; i < _size; i++) {
            _servos[i]->turnOff();
          }
        }

        /* 
        * turn off by index
        */
        void turnOffServo(int index) {
          _servos[index]->turnOff();
        }

    
   }; //class ServoGroup

}; //namespace

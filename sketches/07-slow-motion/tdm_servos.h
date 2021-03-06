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
#define TDM_SERVO_MAXCOUNT          (TDM_PWM_NUMBER_OF_CONTROLLERS * TDM_PWM_CHANNELS_PER_CONTROLLER)


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

      /*
       * Get the servo channel
       */
       uint8_t getChannel() {
        return _channel;
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
        Vector<Servo*> _items; 

      public:
        ServoGroup() {
        };
        
        /*
         * Construct a servo group from parameters. Example: (3,0,9,12) describes a group
         * of 3 servos, taking channels 0, 9 and 12
         */
        ServoGroup(int numargs, ...) {
          va_list args;
          va_start(args, numargs);
          for(int i=0; i<numargs; i++) {
            addChannel(va_arg(args, uint8_t));
          }
          va_end(args); 
        }

      /* 
       * add servo channel to group
       */
       void addChannel(uint8_t channel) {
          _items.push_back(Servo::build(channel));
       }

       /*
        * get servo channel at index
        */
       uint8_t getChannelAtIndex(int index) {
        return _items[index]->getChannel();
       }

       /* 
       * set angles 
       */
        void setAngles(int degrees[]){
          for(int i=0; i < _items.size(); i++) {
            _items[i]->setAngle(degrees[i]);
          }
        }

        /* 
        * turn off all servos
        */
        void turnOffAllServos(void) {
          for(int i=0; i < _items.size(); i++) {
            _items[i]->turnOff();
          }
        }

        /* 
        * turn off one servo at index
        */
        void turnOffAtIndex(int index) {
          _items[index]->turnOff();
        }

        /* 
        * get the group size
        */
        int size() {
          return _items.size();
        }

    
   }; //class ServoGroup

}; //namespace

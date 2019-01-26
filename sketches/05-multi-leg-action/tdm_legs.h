/*
 * In a more traditional environments, one would split into .H and .CPP files. However in Arduino, the ".ino" file
 * comes with certain built-in includes (such as "Arduino.h" and more) which are not automatically added to other CPP
 * files. So, to make things more seamless, I will be putting implementation directly into the header files, which may
 * look unnatural to C++ purists
 */
#pragma once

#define TDM_SERVOS_PER_LEG          3 
#define TDM_LEGS_PER_ROBOT          6 

namespace tdm {

  /*
   * because of restrictions about accessing static class members form .H files, we declare these 
   * as a global
   */
  void *_LEG_A, *_LEG_B, *_LEG_C, *_LEG_D, *_LEG_E, *_LEG_F;

  /* 
   * Describes a Leg of 3 servos  
   */
  class Leg : public ServoGroup{
        
    private:
      Leg(Servo* s1, Servo* s2, Servo* s3) : ServoGroup(3) { 
        setServo(s1, 0);
        setServo(s2, 1);
        setServo(s3, 2);
      };

    public:
      static Leg* build(char legName) {
        switch (legName) {
          case 'A': 
            if (_LEG_A==NULL) {
              _LEG_A = new Leg(Servo::build(31), Servo::build(30), Servo::build(29));
            };
            return (Leg*) _LEG_A;
           
          case 'B': 
            if (_LEG_B==NULL) {
              _LEG_B = new Leg(Servo::build(27), Servo::build(26), Servo::build(25));
            };
            return (Leg*) _LEG_B;
            
          case 'C': 
            if (_LEG_C==NULL) {
              _LEG_C = new Leg(Servo::build(23), Servo::build(22), Servo::build(21));
            };
            return (Leg*) _LEG_C;            
          
          case 'D': 
            if (_LEG_D==NULL) {
              _LEG_D = new Leg(Servo::build(13), Servo::build(14), Servo::build(15));
            };
            return (Leg*) _LEG_D;
                      
          case 'E':
            if (_LEG_E==NULL) {
              _LEG_E = new Leg(Servo::build(9), Servo::build(10), Servo::build(11));
            };
            return (Leg*) _LEG_E;
          
          case 'F': 
            if (_LEG_F==NULL) {
              _LEG_F = new Leg(Servo::build(5), Servo::build(6), Servo::build(7));
            };
            return (Leg*) _LEG_F;            
        }
     }
                 
  }; // class Leg

  /*
   * A class for controling multiple legs
   */
   class LegGroup {

      private:
        uint8_t _size; 
        Leg** _legs;

      public:
        LegGroup(int numargs, ...) {
          _size = numargs;
          _legs = new Leg*[_size];
          va_list args;
          va_start(args, _size);
          for(int i=0; i<_size; i++) {
            _legs[i] = va_arg(args, Leg*);
          }
          va_end(args);          
        }

        ~LegGroup() {
          if (_legs!=NULL) {
             delete [] (Leg*[]) _legs;
             _legs= NULL;
          }
        }

       /* 
       * get leg 
       */
       Leg* getLeg(int index) {
          return _legs[index];
       }

       /* 
       * set angles 
       */
        void setAngles(int degrees[][TDM_SERVOS_PER_LEG]){
          for(int i=0; i < _size; i++) {
            _legs[i]->setAngles(degrees[i]);
          }
        }

        /* 
        * turn off all legs
        */
        void turnOffAllLegs(void) {
          for(int i=0; i < _size; i++) {
            _legs[i]->turnOffAllServos();
          }
        }

        /*
         * turn off one servo per leg
         */
         void turnOffServo(int index) {
          for(int i=0; i < _size; i++) {
            _legs[i]->turnOffServo(index);
          }
         }

        /* 
        * get the group size
        */
        int size() {
          return _size;
        } 
    
   }; //class LegGroup
  
}; //namespace

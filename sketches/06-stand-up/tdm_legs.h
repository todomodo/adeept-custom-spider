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
   * Describes a Leg of 3 servos  
   */
  class Leg : public ServoGroup {
        
    private:
      Leg(uint8_t ch1, uint8_t ch2, uint8_t ch3) 
        : ServoGroup(TDM_SERVOS_PER_LEG, ch1, ch2, ch3) { 
      };

    public:
      Leg(char legName) {
        
        switch (legName) {
          
          case 'A': 
            addChannel(31); addChannel(30); addChannel(29);
            break;
          
          case 'B':
            addChannel(27); addChannel(26); addChannel(25);           
            break;             
            
          case 'C': 
            addChannel(23); addChannel(22); addChannel(21);           
            break;             
          
          case 'D': 
            addChannel(13); addChannel(14); addChannel(15);      
            break;                  
                      
          case 'E':
            addChannel(9); addChannel(10); addChannel(11);      
            break;                  
          
          case 'F': 
            addChannel(5); addChannel(6); addChannel(7);      
            break;
        }
     }
                 
  }; // class Leg

  /*
   * A class for controling multiple legs. Example: (3,'A','B','C') describes a group
   * of 3 legs: 'A', 'B' and 'C"
   */
   class LegGroup {

      private:
        Vector<Leg*> _items; 

      public:
        LegGroup(int numargs, ...) {
          va_list args;
          va_start(args, numargs);
          for(int i=0; i<numargs; i++) {
            /*
             * read as int and cast to char to get around compiler bugs 
             */
            char legName = va_arg(args, int);
            _items.push_back(new Leg(legName));            
          }
          va_end(args);          
        }

       /* 
       * get leg at index
       */
       Leg* getLegAtIndex(int index) {
          return _items[index];
       }

       /* 
       * set angles 
       */
        void setAngles(int degrees[][TDM_SERVOS_PER_LEG]){
          for(int i=0; i < size(); i++) {
            _items[i]->setAngles(degrees[i]);
          }
        }

        /* 
        * turn off all legs
        */
        void turnOffAllLegs(void) {
          for(int i=0; i < size(); i++) {
            _items[i]->turnOffAllServos();
          }
        }

        /*
         * turn off all servos at certain index. For example "turnOffAtIndex(0)" turns off
         * all 0-index servos in the group
         */
         void turnOffAtIndex(int index) {
          for(int i=0; i < size(); i++) {
            _items[i]->turnOffAtIndex(index);
          }
         }

        /* 
        * get the group size
        */
        int size() {
          return _items.size();
        }
    
   }; //class LegGroup
  
}; //namespace

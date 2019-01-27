/*
 * Arduino controlled slow motion. Slowness is achieved by programmatic delays 
 * in the code running as timer2 interrupt handler
 */
#include "tdm_serial.h"
#include "tdm_vector.h"
#include "tdm_pwmio.h"
#include "tdm_lights.h"
#include "tdm_servos.h"
#include "tdm_legs.h"
#include "tdm_motion.h"
#include "tdm_timers.h"


/*
 * Define an interrupt handler class 
 */
class Handler : public tdm::Timer2 {

  private:

      tdm::PwmIO _pwm;  
      tdm::Led _led;      
      tdm::LegMover *_mover;      

      /*
       * define legs and their possible states
       */
      tdm::LegGroup _legs = tdm::LegGroup(TDM_LEGS_PER_ROBOT, 'A', 'B', 'C', 'D', 'E', 'F');
      #define NUM_STATES 3
      const int _state[NUM_STATES][TDM_LEGS_PER_ROBOT][TDM_SERVOS_PER_LEG] = {
        { {45, 10,  10}, {50, 12,  10}, {50, 10,  10}, {55, 95, 110}, {50, 90, 110}, {50, 90, 110} },  // robot state 0
        { {45, 70,  40}, {50, 70,  40}, {50, 70,  40}, {55, 30,  70}, {50, 30,  75}, {50, 30,  75} },  // robot state 1
        { {45, 50, 110}, {50, 50, 110}, {50, 50, 110}, {55, 50,  10}, {50, 50,  10}, {50, 50,  10} }   // robot state 2
      };
      int _index = 0;
        
      /*
       * define motion parameters, for example
       * "30 increments per motion, 20ms delay between increments, timer mode"
       */
      tdm::motion_type_t _motion_type = { 30, 20 , true }; 
                 
      /* 
       * if set, forces the interrupt handler to treat the next N cycles as idle. 
       */
      uint16_t _skip_count=0;
      uint16_t _total_interrupts;

  public:

      /*
      * the initializer
      */
      void onSetup(void) {
        _pwm.onSetup();
        _mover = new tdm::LegMover(&_legs,_motion_type);
        _mover->jumpTo(_state[_index++]);
        tdm::Timer2::onSetup();
      }

      /*
      * the worker is called "getFrequency()" times per second
      */
      void doWork(void) {
        _total_interrupts++;
        if (_skip_count >0) {        
           onSkip();
        } else {
           onWork();
        }            
      }

    private:
      
      void onSkip() {
        _skip_count--;
      }


      void onWork() {
        sei();
        if (_mover->doMotion()) {
          /* 
           * if there was motion, keep the light red and insert some skip counts
           * as instructed by the motion_delay value. We need to do this manually since 
           * in timer mode, the motion_delay value is ignored by the leg mover
           */
          _led.setColor(tdm::_COLOR_RED);          
          int skip_increment = ceil(((float)_motion_type.motion_delay / (float)getInterval()));
          _skip_count += skip_increment;
        } else {
          /*
           * if there was no motion, advance to next position, turn the
           * light green and mark the next 2 seconds as skip/idle
           */
          if (_index>=NUM_STATES) _index = 0;    
          _mover->moveTo(_state[_index++]);                  
          _led.setColor(tdm::_COLOR_GREEN);            
           _skip_count = getFrequency() * 2;
        }
      }
      
};// classHandler


Handler _handler;

/* 
* executed once at startup  
*/
void setup() {
   _handler.onSetup();  
}

/*
 * called continuously 
 */
void loop() {    
  /* 
   * do nothing  
   */
}

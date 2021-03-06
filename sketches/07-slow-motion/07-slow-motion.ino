/*
 * Arduino controlled slow motion. Slowness is achieved by programmatic delays 
 * in the man loop
 */
#include "tdm_vector.h"
#include "tdm_pwmio.h"
#include "tdm_lights.h"
#include "tdm_servos.h"
#include "tdm_legs.h"
#include "tdm_motion.h"


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
 * define the motion parameters, for example
 * 30 increments per motion, 20ms delay between increments, loop mode
 */
tdm::motion_type_t _motion_type = { 30, 20, false }; 


// executed once at startup
void setup() {
  _pwm.onSetup();   
  _mover = new tdm::LegMover(&_legs,_motion_type);
  _mover->jumpTo(_state[_index++]);
}

// called continuously at runtime
void loop() {             

  if (_mover->doMotion()) {
    // if there was motion, keep the light red
    _led.setColor(tdm::_COLOR_RED);
  } else {
    // if there was no motion, advance to next position
    if (_index>=NUM_STATES) _index = 0;    
    _mover->moveTo(_state[_index++]);        
    _led.setColor(tdm::_COLOR_GREEN);
    delay(2000);
  }
}

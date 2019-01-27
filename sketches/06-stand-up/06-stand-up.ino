/*
 * stand up on all 6 legs
 */
#include "tdm_vector.h"
#include "tdm_pwmio.h"
#include "tdm_servos.h"
#include "tdm_legs.h"


/*
 * define PWM
 */
tdm::PwmIO _pwm;
tdm::LegGroup *_legs;

/*
 * set leg states to circle between
 */
#define NUM_STATES 3
const int _states[NUM_STATES][TDM_LEGS_PER_ROBOT][TDM_SERVOS_PER_LEG] = {
  { {45, 10, 10 }, {50, 12, 10 }, {50, 10, 10 }, {55, 95, 110 }, {50, 90, 110 }, {50, 90, 110 } },  // robot state 0
  { {45, 70, 10 }, {50, 70, 10 }, {50, 70, 10 }, {55, 30, 110 }, {50, 30, 110 }, {50, 30, 110 } },  // robot state 1
  { {45, 70, 40 }, {50, 70, 40 }, {50, 70, 40 }, {55, 30,  70 }, {50, 30,  75 }, {50, 30,  75 } }   // robot state 2
};
int _index = 0;


/* 
 * executed once at startup 
 */
void setup() {
  _pwm.onSetup(); 
  _legs = new tdm::LegGroup(TDM_LEGS_PER_ROBOT, 'A','B','C','D','E','F');
}

/* 
 * called continuously at runtime 
 */
void loop() {             

  if (_index<NUM_STATES) {
    _legs->setAngles(_states[_index++]); 
    delay(2000);       
  } else if (_index==NUM_STATES) {
    /*
     * turn off non-essential servos to save power
     */
    _legs->turnOffAtIndex(0);
    _index++;
  }    
}

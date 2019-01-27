/*
 * move all 6 legs in a simple way by drectly setting the servos
 */
#include "tdm_serial.h"
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
#define NUM_STATES 2
const int _states[NUM_STATES][TDM_LEGS_PER_ROBOT][TDM_SERVOS_PER_LEG] = {
  { {45, 50, 110 }, {50, 50, 110 }, {50, 50, 110 }, {55, 50, 10 }, {50, 50, 10 }, {50, 50, 10 } },  // robot state 0
  { {30, 70, 80 }, {30, 70, 80 }, {30, 70, 80 }, {70, 30, 30 }, {70, 30, 30 }, {70, 30, 30 } }      // robot state 1
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
  if (_index>=NUM_STATES) _index=0;    
  _legs->setAngles(_states[_index++]); 
  delay(2000);
}

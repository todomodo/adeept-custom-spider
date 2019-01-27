/*
 * A simple one leg motion pattern
 */
#include "tdm_vector.h"
#include "tdm_pwmio.h"
#include "tdm_servos.h"
#include "tdm_legs.h"

/*
 * define PWM
 */
tdm::PwmIO _pwm;
tdm::Leg *_leg;

/*
 * set leg states to cirle between
 */
#define NUM_STATES 4
const int _states[NUM_STATES][TDM_SERVOS_PER_LEG] = {
  {45, 50, 100 }, //stretch
  {45, 50, 10 },  //fold
  {45, 10, 10 },  //up
  {45, 50, 10 }   //down  
};
int _index = 0;

/*
 * executed once at startup
 */
void setup() {
  _pwm.setup(); 
  _leg = new tdm::Leg('A'); 
}

/*
 * called continously at runtime
 */
void loop() {            
  if (_index>=NUM_STATES) { 
    _index = 0;    
  }
  _leg->setAngles(_states[_index++]);
  delay(2000);
}

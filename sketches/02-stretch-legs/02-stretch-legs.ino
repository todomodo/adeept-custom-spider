/*
 * stretch all legs
 */
#include "tdm_vector.h"
#include "tdm_pwmio.h"
#include "tdm_servos.h"
#include "tdm_legs.h"

/*
 * declare PWM
 */
tdm::PwmIO _pwm;

/* 
 * executed once at startup 
 */
void setup() {
  _pwm.setup();  
  
  int stretch_a[] = {45, 50, 100 };
  tdm::Leg('A').setAngles(stretch_a);  
  int stretch_b[] = {50, 50, 95 };
  tdm::Leg('B').setAngles(stretch_b);
  int stretch_c[] = {52, 50, 95 };
  tdm::Leg('C').setAngles(stretch_c);
  int stretch_d[] = {52, 50, 20 };
  tdm::Leg('D').setAngles(stretch_d);
  int stretch_e[] = {35, 50, 25 };
  tdm::Leg('E').setAngles(stretch_e);
  int stretch_f[] = {50, 50, 25 };
  tdm::Leg('F').setAngles(stretch_f);
}

/* 
 * called continously at runtime 
 */
void loop() {         
    
}

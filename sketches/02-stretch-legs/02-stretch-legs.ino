#include <Adeept_PWMPCA9685.h>
#include "custom_geometry.h"

// executed once at startup
void setup() {
  setupPWM();      
  
  leg_state_t stretch_a = {45, 50, 100 };
  setLeg(LC_A, stretch_a);
  leg_state_t stretch_b = {50, 50, 95 };
  setLeg(LC_B, stretch_b);
  leg_state_t stretch_c = {52, 50, 95 };
  setLeg(LC_C, stretch_c);
  leg_state_t stretch_d = {52, 50, 20 };
  setLeg(LC_D, stretch_d);
  leg_state_t stretch_e = {35, 50, 25 };
  setLeg(LC_E, stretch_e);
  leg_state_t stretch_f = {50, 50, 25 };
  setLeg(LC_F, stretch_f);
}

// called continously at runtime
void loop() {         
  delay(200);
  
}

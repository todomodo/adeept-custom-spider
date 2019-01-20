/*
 * A simple one leg motion pattern
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_servos.h"
#include "custom_legs.h"

leg_state_t a1 = {45, 10, 10 };
leg_state_t a2 = {45, 50, 100 };
leg_state_t a3 = {45, 10, 100 };

#define NUM_STATES 3
leg_state_t states[NUM_STATES] = {a1, a2, a3};
int g_CurrentState = 0;

// executed once at startup
void setup() {
  setupPWM();  
}

// called continously at runtime
void loop() {            
  if (++g_CurrentState>=NUM_STATES) { 
    g_CurrentState = 0;    
  }
  setLeg(LEG_A, states[g_CurrentState]);
  delay(2000);
}

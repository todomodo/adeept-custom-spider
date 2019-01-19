/*
 * stand up on all 6 legs
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_geometry.h"

#define NUM_LEGS 6

leg_configuration_t g_Legs[NUM_LEGS] = { LEG_A, LEG_B, LEG_C, LEG_D, LEG_E, LEG_F};

leg_state_t g_Lotus[NUM_LEGS] = {
  {45, 50, 110 }, {50, 50, 110 }, {50, 50, 110 },
  {55, 50, 10 }, {50, 50, 10 }, {50, 50, 10 }
};

// executed once at startup
void setup() {
  setupPWM();
  setLegs(g_Legs, NUM_LEGS, g_Lotus);
}

// called continuously at runtime
void loop() {           
  delay(200);  
  
}

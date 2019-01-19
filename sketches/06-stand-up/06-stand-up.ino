/*
 * stand up on all 6 legs
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_geometry.h"

#define NUM_LEGS 6

leg_configuration_t g_Legs[NUM_LEGS] = { LEG_A, LEG_B, LEG_C, LEG_D, LEG_E, LEG_F};

leg_state_t g_Rdy[NUM_LEGS] = {
  {45, 10, 10 }, {50, 12, 10 }, {50, 10, 10 },
  {55, 95, 110 }, {50, 90, 110 }, {50, 90, 110 }
};

leg_state_t g_Stand[NUM_LEGS] = {
  {45, 70, 30 }, {50, 70, 30 }, {50, 70, 30 },
  {55, 30, 80 }, {50, 30, 80 }, {50, 30, 80 }
};


// executed once at startup
void setup() {
  setupPWM();
  setLegs(g_Legs, NUM_LEGS, g_Rdy);
}

// called continuously at runtime
void loop() {           
  delay(3000);  
  setLegs(g_Legs, NUM_LEGS, g_Stand);
}

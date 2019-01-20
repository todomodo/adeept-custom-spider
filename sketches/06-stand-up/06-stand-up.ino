/*
 * stand up on all 6 legs
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_geometry.h"

#define NUM_LEGS 6

leg_configuration_t g_AllLegs[NUM_LEGS] = { 
  LEG_A, LEG_B, LEG_C, LEG_D, LEG_E, LEG_F 
};

servo_configuration_t g_FirstServos[NUM_LEGS] = { 
  LEG_A.servo1, LEG_B.servo1, LEG_C.servo1, LEG_D.servo1, LEG_E.servo1, LEG_F.servo1
};

leg_state_t g_Stage1[NUM_LEGS] = {
  {45, 10, 10 }, {50, 12, 10 }, {50, 10, 10 },
  {55, 95, 110 }, {50, 90, 110 }, {50, 90, 110 }
};

leg_state_t g_Stage2[NUM_LEGS] = {
  {45, 70, 10 }, {50, 70, 10 }, {50, 70, 10 },
  {55, 30, 110 }, {50, 30, 110 }, {50, 30, 110 }
};

leg_state_t g_Stage3[NUM_LEGS] = {
  {45, 70, 40 }, {50, 70, 40 }, {50, 70, 40 },
  {55, 30, 70 }, {50, 30, 75 }, {50, 30, 75 }
};


// executed once at startup
void setup() {
  setupPWM();

  // turn off all legs, if the spider is standing, this could bring it down softly
  turnOffLegs(g_AllLegs, NUM_LEGS);
  delay(2000);  

  // get up in 3 phases
  setLegs(g_AllLegs, NUM_LEGS, g_Stage1);
  delay(2000);  
  setLegs(g_AllLegs, NUM_LEGS, g_Stage2);
  delay(1000);
  setLegs(g_AllLegs, NUM_LEGS, g_Stage3);
  delay(1000);

  //turn off non-essential servos to save power
  turnOffServos(g_FirstServos, NUM_LEGS);
}

// called continuously at runtime
void loop() {             
}

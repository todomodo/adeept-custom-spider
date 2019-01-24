/*
 * stand up on all 6 legs
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_servos.h"
#include "custom_legs.h"


servo_configuration_t g_FirstServos[LEGS_PER_ROBOT] = { 
  LEG_A.servo1, LEG_B.servo1, LEG_C.servo1, LEG_D.servo1, LEG_E.servo1, LEG_F.servo1
};

leg_state_t g_Stage0[LEGS_PER_ROBOT] = {
  {45, 10, 10 }, {50, 12, 10 }, {50, 10, 10 },
  {55, 95, 110 }, {50, 90, 110 }, {50, 90, 110 }
};

leg_state_t g_Stage1[LEGS_PER_ROBOT] = {
  {45, 70, 10 }, {50, 70, 10 }, {50, 70, 10 },
  {55, 30, 110 }, {50, 30, 110 }, {50, 30, 110 }
};

leg_state_t g_Stage2[LEGS_PER_ROBOT] = {
  {45, 70, 40 }, {50, 70, 40 }, {50, 70, 40 },
  {55, 30, 70 }, {50, 30, 75 }, {50, 30, 75 }
};


// executed once at startup
void setup() {
  pwmSetup();
 
  // get up in 3 phases
  legSet(ALL_LEGS, LEGS_PER_ROBOT, g_Stage0);
  delay(2000);  
  legSet(ALL_LEGS, LEGS_PER_ROBOT, g_Stage1);
  delay(1000);
  legSet(ALL_LEGS, LEGS_PER_ROBOT, g_Stage2);
  delay(1000);

  //turn off non-essential servos to save power
  servoTurnOff(g_FirstServos, LEGS_PER_ROBOT);
}

// called continuously at runtime
void loop() {             
}

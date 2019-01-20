/*
 * stand up on all 6 legs
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_lights.h"
#include "custom_servos.h"
#include "custom_legs.h"
#include "custom_motion.h"


leg_state_t g_Stage1[LEGS_PER_ROBOT] = {
  {45, 10, 10 }, {50, 12, 10 }, {50, 10, 10 },
  {55, 95, 110 }, {50, 90, 110 }, {50, 90, 110 }
};

leg_state_t g_Stage2[LEGS_PER_ROBOT] = {
  {45, 70, 40 }, {50, 70, 40 }, {50, 70, 40 },
  {55, 30, 70 }, {50, 30, 75 }, {50, 30, 75 }
};

leg_state_t g_Stage3[LEGS_PER_ROBOT] = {
  {45, 50, 110 }, {50, 50, 110 }, {50, 50, 110 },
  {55, 50, 10 }, {50, 50, 10 }, {50, 50, 10 }
};

motion_type_t g_MotionType = { 15, 30 }; // 15 steps per motion, 30ms delay

int g_CurrentStage = 1;

// executed once at startup
void setup() {
  //Serial.begin(9600);
  setupPWM();
  beginMotion(ALL_LEGS, LEGS_PER_ROBOT, g_Stage1);  
  delay(2000);  
  // moveTo(g_MotionType, g_Stage2, LEGS_PER_ROBOT);
  
}

// called continuously at runtime
void loop() {             

  bool has_moved = doMotion();
  
  if (!has_moved) {
    switch(g_CurrentStage) {
      case 1:
        moveTo(g_MotionType, g_Stage1, LEGS_PER_ROBOT);
        g_CurrentStage = 2;
        break;
      case 2:
        moveTo(g_MotionType, g_Stage2, LEGS_PER_ROBOT);
        g_CurrentStage = 3;
        break;
      case 3:
        moveTo(g_MotionType, g_Stage3, LEGS_PER_ROBOT);
        g_CurrentStage = 1;
        break;
    }
  }
  

}

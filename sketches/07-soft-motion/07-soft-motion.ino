/*
 * soft motion between several states
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_lights.h"
#include "custom_servos.h"
#include "custom_legs.h"
#include "custom_motion.h"

/*
 * define motion stage to be A collection of leg states
 */
typedef struct {
  leg_state_t legs[LEGS_PER_ROBOT];
} motions_stage_t;

/*
 * Declare several differnt motion stages
 */
motions_stage_t g_Stage0 = {{
  {45, 10, 10 }, {50, 12, 10 }, {50, 10, 10 },
  {55, 95, 110 }, {50, 90, 110 }, {50, 90, 110 }
}};

motions_stage_t g_Stage1 = {{
  {45, 70, 40 }, {50, 70, 40 }, {50, 70, 40 },
  {55, 30, 70 }, {50, 30, 75 }, {50, 30, 75 }
}};

motions_stage_t g_Stage2 = {{
  {45, 50, 110 }, {50, 50, 110 }, {50, 50, 110 },
  {55, 50, 10 }, {50, 50, 10 }, {50, 50, 10 }
}};

/*
 *  one global array of motion stages
 */
#define MAX_MOTION_STAGES 3
motions_stage_t g_MotionStages[MAX_MOTION_STAGES] = { g_Stage0, g_Stage1, g_Stage2 };
int g_CurrentMotionStageIndex = -1;

/*
 * define the motion parameters, for example
 * 15 steps per motion, 30ms delay
 */
motion_type_t g_MotionType = { 10, 30 }; 


// executed once at startup
void setup() {
  setupPWM();
  beginMotion(ALL_LEGS, LEGS_PER_ROBOT, g_MotionStages[g_CurrentMotionStageIndex].legs);    
}

// called continuously at runtime
void loop() {             

  bool in_progress = doMotion();
  
  if (!in_progress) {
    // if there was no motion, advance to next position
    if (++g_CurrentMotionStageIndex>=MAX_MOTION_STAGES) g_CurrentMotionStageIndex = 0;    
    moveTo(g_MotionType, g_MotionStages[g_CurrentMotionStageIndex].legs, LEGS_PER_ROBOT);    
  }
}

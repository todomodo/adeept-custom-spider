/*
 * move all 6 legs in a simple way by drectly setting the servos
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_servos.h"
#include "custom_legs.h"

/*
 * define motion stage to be A collection of leg states
 */
typedef struct {
  leg_state_t legs[LEGS_PER_ROBOT];
} motions_stage_t;

motions_stage_t g_Stage0 = {{
  {45, 50, 110 }, {50, 50, 110 }, {50, 50, 110 },
  {55, 50, 10 }, {50, 50, 10 }, {50, 50, 10 }
}};

motions_stage_t g_Stage1 = {{
  {30, 70, 80 }, {30, 70, 80 }, {30, 70, 80 },
  {70, 30, 30 }, {70, 30, 30 }, {70, 30, 30 }
}};

/*
 *  one global array of motion stages
 */
#define MAX_MOTION_STAGES 2
motions_stage_t g_MotionStages[MAX_MOTION_STAGES] = { g_Stage0, g_Stage1 };
int g_CurrentMotionStageIndex = -1;


// executed once at startup
void setup() {
  setupPWM();
}

// called continuously at runtime
void loop() {           
  if (++g_CurrentMotionStageIndex>=MAX_MOTION_STAGES) g_CurrentMotionStageIndex = 0;    
  setLegs(ALL_LEGS, LEGS_PER_ROBOT, g_MotionStages[g_CurrentMotionStageIndex].legs); 
  delay(2000);     
}

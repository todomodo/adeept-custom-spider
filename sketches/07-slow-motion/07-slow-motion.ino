/*
 * Arduino controlled slow motion. Slowness is achieved by programmatic delays 
 * in the code from the man loop
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_lights.h"
#include "custom_servos.h"
#include "custom_legs.h"
#include "custom_motion.h"

/*
 * define motion frame to be A collection of leg states
 */
typedef struct {
  leg_state_t legs[LEGS_PER_ROBOT];
} motion_frame_t;

/*
 * Declare several differnt motion frames
 */
motion_frame_t g_Frame0 = {{
  {45, 10, 10 }, {50, 12, 10 }, {50, 10, 10 },
  {55, 95, 110 }, {50, 90, 110 }, {50, 90, 110 }
}};

motion_frame_t g_Frame1 = {{
  {45, 70, 40 }, {50, 70, 40 }, {50, 70, 40 },
  {55, 30, 70 }, {50, 30, 75 }, {50, 30, 75 }
}};

motion_frame_t g_Frame2 = {{
  {45, 50, 110 }, {50, 50, 110 }, {50, 50, 110 },
  {55, 50, 10 }, {50, 50, 10 }, {50, 50, 10 }
}};

/*
 *  one global array of motion frames
 */
#define MAX_FRAMES 3
motion_frame_t g_MotionFrames[MAX_FRAMES] = { g_Frame0, g_Frame1, g_Frame2 };
int g_CurrentFrameIndex = 0;

/*
 * define the motion parameters, for example
 * 15 steps per motion, 30ms delay
 */
motion_type_t g_MotionType = { 20, 60 }; 


// executed once at startup
void setup() {
  pwmSetup();
  mgBeginMotion(ALL_LEGS, LEGS_PER_ROBOT, g_MotionFrames[g_CurrentFrameIndex++].legs);    
}

// called continuously at runtime
void loop() {             

  if (mgDoMotion()) {
    // if there was motion, keep the light red
    ledSet(LED_A,LED_COLOR_RED);
  } else {
    // if there was no motion, advance to next position
    if (++g_CurrentFrameIndex>=MAX_FRAMES) g_CurrentFrameIndex = 0;    
    mgMoveTo(g_MotionType, g_MotionFrames[g_CurrentFrameIndex].legs, LEGS_PER_ROBOT);
    ledSet(LED_A,LED_COLOR_GREEN);
    delay(2000);
  }
}

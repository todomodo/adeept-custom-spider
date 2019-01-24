/*
 * Arduino controlled slow motion. Slowness is achieved by programmatic delays 
 * in the code running as timer2 interrupt handler
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_timers.h"
#include "custom_pwmio.h"
#include "custom_lights.h"
#include "custom_servos.h"
#include "custom_legs.h"
#include "custom_motion.h"

/*
 * define motion frame to be collection of leg states
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
 * define motion parameters, for example
 *  "20 phases per motion, 0ms delay between phases, async mode"
 * when using async mode, the phase delays value is ignored and we need to take care
 * of it in the code. See the explanation for g_SkipCount below
 */
motion_type_t g_MotionType = { 20, 0 , true }; 


/* 
 * Tracks the total number of raised interrupts. 
 */
uint16_t g_InterruptCount=0;

/*
 * The LOAD prescaler controlls the ratio between busy and idle interrupts. For example 
 * a value of 3 indicates that every 3th interrupt is busy (does something) - the rest
 * are idle.
 */
#define LOAD_PRESCALER 3

/* 
 * if set, forces the interrupt handler to treat the next N cycles as idle. 
 */
uint16_t g_SkipCount=0;



// executed once at startup
void setup() {
  setupTimers();
  pwmSetup();  
  mgBeginMotion(ALL_LEGS, LEGS_PER_ROBOT, g_MotionFrames[g_CurrentFrameIndex++].legs);    
}

/*
 * called continuously 
 */
void loop() {    
  /* 
   * do nothing  
   */
}

/*
 * called TIMER2_COMPA_FREQUENCY times per second
 */
ISR (TIMER2_COMPA_vect)
{
  /*
   * start by incrementing the main interrupt counter 
   */
   g_InterruptCount++;

   /*
    * check to see if this is a skip cycle
    */
  if (g_SkipCount >0) {
    /*
     * this is a skip cycle, all we do is decrement the skip counter
     */
     g_SkipCount--;
  } else {
    /*
     * this is not a skip cycle. Check if it's busy or idle
     */
    uint16_t reminder = g_InterruptCount++ % LOAD_PRESCALER;    
    if (reminder == 0)
    {               
      /*
       * this is a busy cycle - do some work
       */
      if (mgDoMotion()) {
        /*
         * if there was motion, keep the light red
         */
        ledSet(LED_A,LED_COLOR_RED);
      } else {
        /*
         * There was no motion, advance to next position
         */
        if (++g_CurrentFrameIndex>=MAX_FRAMES) g_CurrentFrameIndex = 0;    
        mgMoveTo(g_MotionType, g_MotionFrames[g_CurrentFrameIndex].legs, LEGS_PER_ROBOT);
  
        /*
         * Turn the light green
         */
        sei();
        ledSet(LED_A,LED_COLOR_GREEN);

        /*
         * Mark the next 2 seconds as idle 
         */
         g_SkipCount = TIMER2_COMPA_FREQUENCY * 2;
      }
    }
  }
}

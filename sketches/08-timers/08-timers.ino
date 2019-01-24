/*
  Blink RGB led using timer interrupts
 */

#include <Adeept_PWMPCA9685.h>
#include "custom_timers.h"
#include "custom_pwmio.h"
#include "custom_lights.h"

/*
 * a simple color changing scheme
 */
#define NUM_COLORS 3
led_state_t g_Colors[NUM_COLORS] = {LED_COLOR_RED, LED_COLOR_GREEN, LED_COLOR_BLUE};
int g_CurrentColorIndex=0;

/* 
 * Tracks the number of handled interrupts. 
 */
uint16_t g_InterruptCount=0;

/*
 * called once at startup
 */
void setup() {  
  setupPWM();
  setupTimers();  
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
   * a simple trick to ensure our loop runs once/second
   */
  uint16_t reminder = g_InterruptCount++ % TIMER2_COMPA_FREQUENCY;    
  if (reminder == 0)
  {      
    /*
     * pick the next color
     */
     if (++g_CurrentColorIndex>=NUM_COLORS) g_CurrentColorIndex=0;      

     /*
      * ATMEL 328/168 has a single level interrupts which means interrupts are disabled inside interrupt
      * handlers. Since I2C uses interrupts, any attempts to communicate with I2C devices (such as the PWM
      * controller's RGB light) from inside the interrupt handler will likely fail. The technique we use
      * here is to re-enable interrupts explicitly. This works but is considered risky
      */
     sei();
     
     /*
      * change the LED color
      */
     setLed(LED_A,g_Colors[g_CurrentColorIndex]);
  }
}

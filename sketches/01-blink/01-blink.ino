/*
  Blink the LED in R/G/B
 */

#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_lights.h"

#define NUM_COLORS 3
led_state_t g_Colors[NUM_COLORS] = {LED_COLOR_RED, LED_COLOR_GREEN, LED_COLOR_BLUE};
int g_CurrentColorIndex=0;


void setup() {  
  setupPWM2();
}

void loop() {  
  if (++g_CurrentColorIndex>=NUM_COLORS) g_CurrentColorIndex=0;
  setLed(LED_A,g_Colors[g_CurrentColorIndex]);
   delay(1500);
}

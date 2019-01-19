/*
  Blink the LED in R/G/B
 */

#include <Adeept_PWMPCA9685.h>
#include "custom_geometry.h"

led_state_t g_Color;
char g_ColorName;

void setup() {  
  setupPWM();
  setNextColor('R');
}

void loop() {  

  switch(g_ColorName) {
    case 'R':
      setLed(LED_A,LED_COLOR_RED);
      setNextColor('G');
      break;
    case 'G':
      setLed(LED_A,LED_COLOR_GREEN);
      setNextColor('B');
      break;
    case 'B':
      setLed(LED_A,LED_COLOR_BLUE);
      setNextColor('R');
      break;
  }
    
   delay(1500);
}

void setNextColor(char colorName) {
  switch(colorName) {
    case 'R' :
      g_ColorName = 'R';
      g_Color = LED_COLOR_RED;
      break;
    case 'G' :
      g_ColorName = 'G';
      g_Color = LED_COLOR_GREEN;
      break;
    case 'B' :
      g_ColorName = 'B';
      g_Color = LED_COLOR_BLUE;
      break;
  }
}

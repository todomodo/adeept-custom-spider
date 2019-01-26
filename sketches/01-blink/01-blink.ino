/*
  Blink the LED in R/G/B
 */
#include "tdm_pwmio.h"
#include "tdm_lights.h"

/*
 * declare PWM and LED
 */
tdm::PwmIO _pwm;
tdm::Led _led;

/*
 * prep some colours for the blink
 */
#define NUM_COLORS 3
tdm::rgb_color_t _colors[NUM_COLORS] = {tdm::_COLOR_RED, tdm::_COLOR_GREEN, tdm::_COLOR_BLUE};
int _index=0;

/*
 * called once at startup
 */
void setup() {
  _pwm.setup();  
}

/*
 * main loop
 */
void loop() {  
  if (_index>=NUM_COLORS) _index=0;   
  _led.setColor(_colors[_index++]);
   delay(1000);
}

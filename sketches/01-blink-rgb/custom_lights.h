/*
 * code for controlling lights
 */

#define PWM_LED_COLOR_OFF 4095            // magic value turns off a color 
#define PWM_LED_COLOR_ON     0            // magic value turns on a color

 // describes an RGB LED configuration 
typedef struct {
  uint8_t rChannel;
  uint8_t gChannel;
  uint8_t bChannel;
} led_configuration_t;

// describes the state of the RGB LED 
typedef struct {  
  int rPulse;
  int gPulse;
  int bPulse;
} led_state_t;

/*
 * define the led in terms of channel numbers. Channel numbers are 0 .. 31 
 * for dual controller setup 
 */
led_configuration_t LED_A = { 2, 3, 4 };

// define some primary colors
led_state_t LED_COLOR_RED =   { PWM_LED_COLOR_ON,  PWM_LED_COLOR_OFF, PWM_LED_COLOR_OFF };
led_state_t LED_COLOR_GREEN = { PWM_LED_COLOR_OFF, PWM_LED_COLOR_ON,  PWM_LED_COLOR_OFF };
led_state_t LED_COLOR_BLUE =  { PWM_LED_COLOR_OFF, PWM_LED_COLOR_OFF, PWM_LED_COLOR_ON };

// set the LED
void setLed(led_configuration_t config, led_state_t state) {
  setPWMPulse(config.rChannel, state.rPulse);
  setPWMPulse(config.gChannel, state.gPulse);
  setPWMPulse(config.bChannel, state.bPulse);
}

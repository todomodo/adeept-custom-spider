/*
  I've wired my servos differently from what the manual says to better organize cables
  This file defines which PWM controller and channel each servo uses.
 */
#define PWM_SERVO_FREQUENCY 			    50  //TowerPro MG90S Servos run at 50 HZ
#define PWM_LED_COLOR_OFF			      4095  // magic value turns off a color 
#define PWM_LED_COLOR_ON				       0  // magic value turns on a color 
#define PWM_PIN_OFF						         0  // magic value turns off a pin 
#define PWM_CHANNELS_PER_CONTROLLER		16  // we use 16 channel controllers

// describes a LED configuration 
typedef struct {
  uint8_t rChannel;
  uint8_t gChannel;
  uint8_t bChannel;
} led_configuration_t;

// describes the state fo the LED 
typedef struct {  
  int rPulse;
  int gPulse;
  int bPulse;
} led_state_t;

// define the leg in terms of channel numbers. Channel numbers are 0 .. 31 
// for dual controller setup
led_configuration_t LED_A = { 2, 3, 4 };

// define some primary colors
led_state_t LED_COLOR_RED =   { PWM_LED_COLOR_ON,  PWM_LED_COLOR_OFF, PWM_LED_COLOR_OFF };
led_state_t LED_COLOR_GREEN = { PWM_LED_COLOR_OFF, PWM_LED_COLOR_ON,  PWM_LED_COLOR_OFF };
led_state_t LED_COLOR_BLUE =  { PWM_LED_COLOR_OFF, PWM_LED_COLOR_OFF, PWM_LED_COLOR_ON };

// global variables for talking with the PWM controller
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40); 
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41); 

// set pwm pulse on a channel chosing first or second controller based on the
// channel number
void setPWMPulse(int channel, int pulse_length){
  if (channel<PWM_CHANNELS_PER_CONTROLLER) {
    // use first controller
    pwm0.setPWM(channel, PWM_PIN_OFF, pulse_length);  
  } else {
    // use second controller
    pwm1.setPWM(channel-PWM_CHANNELS_PER_CONTROLLER, PWM_PIN_OFF, pulse_length); 
  }
}

// set the LED
void setLed(led_configuration_t config, led_state_t state) {
  setPWMPulse(config.rChannel, state.rPulse);
  setPWMPulse(config.gChannel, state.gPulse);
  setPWMPulse(config.bChannel, state.bPulse);
}

// initialize the PWM controllers
void setupPWM(void) {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY);
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY);   
}

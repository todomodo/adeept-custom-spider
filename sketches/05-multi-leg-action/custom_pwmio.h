/*
 * Low level I/O functions and definitions to deal with PWM controllers
 * most of these are required whne controlling servos, legs and RGB lights
 */

#define PWM_SERVO_FREQUENCY          50  //TowerPro MG90S Servos run at 50 HZ
#define PWM_SERVO_ANGLEMIN            0  //Physical servos rotate between 0 .. 180 degrees
#define PWM_SERVO_ANGLEMAX          180  //Physical servos rotate between 0 .. 180 degrees
#define PWM_SERVO_PULSEMIN          130  //PWM pulse between 130 and 600
#define PWM_SERVO_PULSEMAX          600  //PWM pulse between 130 and 600
#define PWM_PIN_OFF                   0  // magic value turns off a pin 
#define PWM_CHANNELS_PER_CONTROLLER  16  // we use 16 channel controllers
#define PWM_NUMBER_OF_CONTROLLERS     2  // there's 2 of them

// global variables for talking with the PWM controller
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40); 
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41); 

// setup the PWM controllers
void setupPWM(void) {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY);
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY);   
}

/*
 * set PWM pulse on a channel chosing first or second controller based on the
 * channel number
 */
void setPWMPulse(int channel, int pulse_length){
  if (channel<PWM_CHANNELS_PER_CONTROLLER) {
    // use first controller
    pwm0.setPWM(channel, PWM_PIN_OFF, pulse_length);  
  } else {
    // use second controller
    pwm1.setPWM(channel-PWM_CHANNELS_PER_CONTROLLER, PWM_PIN_OFF, pulse_length); 
  }
}

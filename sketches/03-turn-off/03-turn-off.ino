/*
 * Power down all servoes
 */
#include <Adeept_PWMPCA9685.h>

#define PWM_SERVO_FREQUENCY 50 //TowerPro MG90S Servos run at 50 HZ

// global variables for talking with the PWM controller
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40);  
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41); 

// executed once at startup
void setup() {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY);
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY);   
  for(int i=0;i<16;i++) {
    pwm0.setPWM(i,0,0); // turn off power
    pwm1.setPWM(i,0,0); // turn off power
  }
}

// called continously at runtime
void loop() {         

}

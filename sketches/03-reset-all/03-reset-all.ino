/*
 * Reset all systems
 */
#include <Adeept_PWMPCA9685.h>

#define PWM_SERVO_FREQUENCY 50 //TowerPro MG90S Servos run at 50 HZ

// global variables for talking with the PWM controller
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40);  //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41);  //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write

// executed once at startup
void setup() {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY);
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY);   
  for(int i=0;i<16;i++) {
    pwm0.setPWM(i,0,0);
    pwm1.setPWM(i,0,0);
  }
}

// called continously at runtime
void loop() {         
  delay(500);
  pwm0.reset();
  pwm1.reset();
}

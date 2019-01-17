/*
 * One leg motion pattern
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_geometry.h"

#define NUM_POINTS 3

leg_descriptor_t legD_point1 = { "D", {LEGD_CONTROLLER,LEGD_SERVO1_CHANNEL, 15},{LEGD_CONTROLLER,LEGD_SERVO2_CHANNEL,50},{LEGD_CONTROLLER,LEGD_SERVO3_CHANNEL,90} };
leg_descriptor_t legD_point2 = { "D", {LEGD_CONTROLLER,LEGD_SERVO1_CHANNEL, 45},{LEGD_CONTROLLER,LEGD_SERVO2_CHANNEL,80},{LEGD_CONTROLLER,LEGD_SERVO3_CHANNEL,115} };
leg_descriptor_t legD_point3 = { "D", {LEGD_CONTROLLER,LEGD_SERVO1_CHANNEL, 60},{LEGD_CONTROLLER,LEGD_SERVO2_CHANNEL,50},{LEGD_CONTROLLER,LEGD_SERVO3_CHANNEL,90} };

leg_descriptor_t points[NUM_POINTS] = {legD_point1, legD_point2, legD_point3};
int current_point_index = 1;

// global variables for talking with the PWM controller
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40);  //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41);  //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write

// executed once at startup
void setup() {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY); 
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY); 
  setLeg(points[current_point_index]);
}

// called continously at runtime
void loop() {           
  delay(2000);
  if (++current_point_index>=NUM_POINTS) { 
    current_point_index = 0;    
  }
  setLeg(points[current_point_index]);
}

// set one leg
void setLeg(leg_descriptor_t leg) {  
  setServo(leg.servo2);
  setServo(leg.servo3);
  setServo(leg.servo1);
}

// set one servo
void setServo(servo_descriptor_t servo){
  if (servo.pwmIndex==0) {
    pwm0.setPWM(servo.channelIndex, 0, pulseLength(servo.angle));
  } else if (servo.pwmIndex==1) {
    pwm1.setPWM(servo.channelIndex, 0, pulseLength(servo.angle));
  }
}

// convert from degrees to PWM pulses
int pulseLength (int degrees){
  if(degrees>=180){degrees=180;}
  if(degrees<=0){degrees=1;}  // for some reason my controler acts out when 0 is used
  return map(degrees,0,180,130,600);
}

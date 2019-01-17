#include <Adeept_PWMPCA9685.h>
#include "custom_geometry.h"

// front stretch configuration
leg_descriptor_t legA_front_stretch = { "A", {LEGA_CONTROLLER,LEGA_SERVO1_CHANNEL,   5},{LEGA_CONTROLLER,LEGA_SERVO2_CHANNEL,50},{LEGA_CONTROLLER,LEGA_SERVO3_CHANNEL,60} };
leg_descriptor_t legB_front_stretch = { "B", {LEGB_CONTROLLER,LEGB_SERVO1_CHANNEL,  10},{LEGB_CONTROLLER,LEGB_SERVO2_CHANNEL,50},{LEGB_CONTROLLER,LEGB_SERVO3_CHANNEL,65} };
leg_descriptor_t legC_front_stretch = { "C", {LEGC_CONTROLLER,LEGC_SERVO1_CHANNEL,   5},{LEGC_CONTROLLER,LEGC_SERVO2_CHANNEL,65},{LEGC_CONTROLLER,LEGC_SERVO3_CHANNEL,55} };
leg_descriptor_t legD_front_stretch = { "D", {LEGD_CONTROLLER,LEGD_SERVO1_CHANNEL, 100},{LEGD_CONTROLLER,LEGD_SERVO2_CHANNEL,50},{LEGD_CONTROLLER,LEGD_SERVO3_CHANNEL,45} };
leg_descriptor_t legE_front_stretch = { "E", {LEGE_CONTROLLER,LEGE_SERVO1_CHANNEL,  85},{LEGE_CONTROLLER,LEGE_SERVO2_CHANNEL,48},{LEGE_CONTROLLER,LEGE_SERVO3_CHANNEL,50} };
leg_descriptor_t legF_front_stretch = { "F", {LEGF_CONTROLLER,LEGF_SERVO1_CHANNEL, 110},{LEGF_CONTROLLER,LEGF_SERVO2_CHANNEL,65},{LEGF_CONTROLLER,LEGF_SERVO3_CHANNEL,45} };
robot_descriptor_t front_stretch  = { { legA_front_stretch, legB_front_stretch, legC_front_stretch, legD_front_stretch, legE_front_stretch, legF_front_stretch } };

// radial stretch configuration
leg_descriptor_t legA_radial_stretch = { "A", {LEGA_CONTROLLER,LEGA_SERVO1_CHANNEL, 45},{LEGA_CONTROLLER,LEGA_SERVO2_CHANNEL,50},{LEGA_CONTROLLER,LEGA_SERVO3_CHANNEL,60} };
leg_descriptor_t legB_radial_stretch = { "B", {LEGB_CONTROLLER,LEGB_SERVO1_CHANNEL, 50},{LEGB_CONTROLLER,LEGB_SERVO2_CHANNEL,50},{LEGB_CONTROLLER,LEGB_SERVO3_CHANNEL,65} };
leg_descriptor_t legC_radial_stretch = { "C", {LEGC_CONTROLLER,LEGC_SERVO1_CHANNEL, 52},{LEGC_CONTROLLER,LEGC_SERVO2_CHANNEL,65},{LEGC_CONTROLLER,LEGC_SERVO3_CHANNEL,55} };
leg_descriptor_t legD_radial_stretch = { "D", {LEGD_CONTROLLER,LEGD_SERVO1_CHANNEL, 50},{LEGD_CONTROLLER,LEGD_SERVO2_CHANNEL,50},{LEGD_CONTROLLER,LEGD_SERVO3_CHANNEL,45} };
leg_descriptor_t legE_radial_stretch = { "E", {LEGE_CONTROLLER,LEGE_SERVO1_CHANNEL, 40},{LEGE_CONTROLLER,LEGE_SERVO2_CHANNEL,48},{LEGE_CONTROLLER,LEGE_SERVO3_CHANNEL,50} };
leg_descriptor_t legF_radial_stretch = { "F", {LEGF_CONTROLLER,LEGF_SERVO1_CHANNEL, 55},{LEGF_CONTROLLER,LEGF_SERVO2_CHANNEL,65},{LEGF_CONTROLLER,LEGF_SERVO3_CHANNEL,45} };
robot_descriptor_t radial_stretch  = { { legA_radial_stretch, legB_radial_stretch, legC_radial_stretch, legD_radial_stretch, legE_radial_stretch, legF_radial_stretch } };

// rear stretch configuration
leg_descriptor_t legA_rear_stretch = { "A", {LEGA_CONTROLLER,LEGA_SERVO1_CHANNEL, 100},{LEGA_CONTROLLER,LEGA_SERVO2_CHANNEL,50},{LEGA_CONTROLLER,LEGA_SERVO3_CHANNEL,60} };
leg_descriptor_t legB_rear_stretch = { "B", {LEGB_CONTROLLER,LEGB_SERVO1_CHANNEL, 100},{LEGB_CONTROLLER,LEGB_SERVO2_CHANNEL,50},{LEGB_CONTROLLER,LEGB_SERVO3_CHANNEL,65} };
leg_descriptor_t legC_rear_stretch = { "C", {LEGC_CONTROLLER,LEGC_SERVO1_CHANNEL, 100},{LEGC_CONTROLLER,LEGC_SERVO2_CHANNEL,65},{LEGC_CONTROLLER,LEGC_SERVO3_CHANNEL,55} };
leg_descriptor_t legD_rear_stretch = { "D", {LEGD_CONTROLLER,LEGD_SERVO1_CHANNEL,   5},{LEGD_CONTROLLER,LEGD_SERVO2_CHANNEL,50},{LEGD_CONTROLLER,LEGD_SERVO3_CHANNEL,45} };
leg_descriptor_t legE_rear_stretch = { "E", {LEGE_CONTROLLER,LEGE_SERVO1_CHANNEL,   0},{LEGE_CONTROLLER,LEGE_SERVO2_CHANNEL,48},{LEGE_CONTROLLER,LEGE_SERVO3_CHANNEL,50} };
leg_descriptor_t legF_rear_stretch = { "F", {LEGF_CONTROLLER,LEGF_SERVO1_CHANNEL,  15},{LEGF_CONTROLLER,LEGF_SERVO2_CHANNEL,65},{LEGF_CONTROLLER,LEGF_SERVO3_CHANNEL,45} };
robot_descriptor_t rear_stretch  = { { legA_rear_stretch, legB_rear_stretch, legC_rear_stretch, legD_rear_stretch, legE_rear_stretch, legF_rear_stretch } };

// robot states to transition between
#define NUM_STATES 4
robot_descriptor_t states[NUM_STATES] = {front_stretch, radial_stretch, rear_stretch, radial_stretch};
int current_state_index = 1;

// global variables for talking with the PWM controller
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40);  //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41);  //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write

// executed once at startup
void setup() {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY); 
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY); 
  setRobot(states[current_state_index]);
}

// called continously at runtime
void loop() {        
  delay(2000);
  if (++current_state_index>=NUM_STATES) { 
    current_state_index = 0;    
  }
  setRobot(states[current_state_index]);
}

// set robot state
void setRobot(robot_descriptor_t robot ) {
  for(int i=0;i<NUM_LEGS;i++) {
    setLeg(robot.legs[i]);
  }
}

// set one leg
void setLeg(leg_descriptor_t leg) {
  setServo(leg.servo1);
  setServo(leg.servo2);
  setServo(leg.servo3);
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

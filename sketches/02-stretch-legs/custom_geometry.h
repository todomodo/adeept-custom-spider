/*
  I've wired my servos differently from what the manual says to better organize cables
  This file defines which PWM controller and channel each servo uses.
 */

#define PWM_SERVO_FREQUENCY          50  //TowerPro MG90S Servos run at 50 HZ
#define PWM_SERVO_ANGLEMIN            0  //Physical servos rotate between 0 .. 180 degrees
#define PWM_SERVO_ANGLEMAX          180  //Physical servos rotate between 0 .. 180 degrees
#define PWM_SERVO_PULSEMIN          130  //PWM pulse between 130 and 600
#define PWM_SERVO_PULSEMAX          600  //PWM pulse between 130 and 600
#define PWM_PIN_OFF                   0  // magic value turns off a pin 
#define PWM_CHANNELS_PER_CONTROLLER  16  // we use 16 channel controllers

// describes a servo configuration 
typedef struct {
  uint8_t channel;
} servo_configuration_t;

// describes a leg of 3 servoes 
typedef struct {
  servo_configuration_t servo1;
  servo_configuration_t servo2;
  servo_configuration_t servo3;
} leg_configuration_t;

// describes the position of the leg 
typedef struct {
  int angle1;
  int angle2;
  int angle3;
} leg_state_t;

// define the legs in terms of servo numbers. Servo numbers are 0 .. 31
leg_configuration_t LEG_A = { {31}, {30}, {29} };
leg_configuration_t LEG_B = { {27}, {26}, {25} };
leg_configuration_t LEG_C = { {23}, {22}, {21} };
leg_configuration_t LEG_D = { {13}, {14}, {15} };
leg_configuration_t LEG_E = { { 9}, {10}, {11} };
leg_configuration_t LEG_F = { { 5}, { 6}, { 7} };

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

// convert from degrees to PWM pulses
int getPulseLength (int degrees){
  if(degrees>=PWM_SERVO_ANGLEMAX){degrees=PWM_SERVO_ANGLEMAX;}
  if(degrees<=PWM_SERVO_ANGLEMIN){degrees=PWM_SERVO_ANGLEMIN;}
  return map(degrees,PWM_SERVO_ANGLEMIN,PWM_SERVO_ANGLEMAX,PWM_SERVO_PULSEMIN,PWM_SERVO_PULSEMAX);
}

// set one servo
void setServo(servo_configuration_t servo, int angle){
    setPWMPulse(servo.channel, getPulseLength(angle));
}

// turn one servo off
void turnOffServo(servo_configuration_t config) {
  setPWMPulse(config.channel, PWM_PIN_OFF);
}

// turn multiple servos off
void turnOffServos(servo_configuration_t configs[], int size) {
  for (int i=0; i<size; i++) {
    turnOffServo(configs[i]);
  }
}

// set one leg
void setLeg(leg_configuration_t config, leg_state_t state) {
  setServo(config.servo1, state.angle1);
  setServo(config.servo2, state.angle2);
  setServo(config.servo3, state.angle3);
}

// set multiple legs
void setLegs(leg_configuration_t configs[], int size, leg_state_t states[]) {
  for (int i=0; i<size; i++) {
    setLeg(configs[i], states[i]);
  }
}

// turn one leg off
void turnOffLeg(leg_configuration_t config) {
  turnOffServo(config.servo1);
  turnOffServo(config.servo2);
  turnOffServo(config.servo3);
}

// turn multiple legs off
void turnOffLegs(leg_configuration_t configs[], int size) {
  for (int i=0; i<size; i++) {
    turnOffLeg(configs[i]);
  }
}

void setupPWM(void) {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY);
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY);   
}

/*
  I've wired my servoes differently from what the manual says to better organize cables
  This file defines which PWM controller and channel each servo uses.
 */
#define PWM_SERVO_FREQUENCY 50 //TowerPro MG90S Servos run at 50 HZ
#define PWM_SERVO_PULSEMIN 130
#define PWM_SERVO_PULSEMAX 600

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
  int servo1_angle;
  int servo2_angle;
  int servo3_angle;
} leg_state_t;

// define the legs in terms of servo numbers. Servo numbers are 0 .. 31
leg_configuration_t LC_A = { {31}, {30}, {29} };
leg_configuration_t LC_B = { {27}, {26}, {25} };
leg_configuration_t LC_C = { {23}, {22}, {21} };
leg_configuration_t LC_D = { {13}, {14}, {15} };
leg_configuration_t LC_E = { { 9}, {10}, {11} };
leg_configuration_t LC_F = { { 5}, { 6}, { 7} };

// global variables for talking with the PWM controller
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40); 
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41); 

// convert from degrees to PWM pulses
int pulseLength (int degrees){
  if(degrees>=180){degrees=180;}
  if(degrees<=0){degrees=1;}  // for some reason my controler acts out when 0 is used
  return map(degrees,0,180,PWM_SERVO_PULSEMIN,PWM_SERVO_PULSEMAX);
}

// set one servo
void setServo(servo_configuration_t servo, int angle){
  if (servo.channel<16) {
    pwm0.setPWM(servo.channel, 0, pulseLength(angle));
  } else {
    pwm1.setPWM(servo.channel-16, 0, pulseLength(angle));
  }
}

// set one leg
void setLeg(leg_configuration_t config, leg_state_t state) {
  setServo(config.servo1, state.servo1_angle);
  setServo(config.servo2, state.servo2_angle);
  setServo(config.servo3, state.servo3_angle);
}

void setupPWM(void) {
  pwm0.begin();
  pwm0.setPWMFreq(PWM_SERVO_FREQUENCY);
  pwm1.begin();
  pwm1.setPWMFreq(PWM_SERVO_FREQUENCY);   
}

/*
 * The legs layer
 */

 #define SERVOS_PER_LEG          3 
 #define LEGS_PER_ROBOT          6 

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

/*
 * Define the legs in terms of servo numbers. Servo numbers are 0 .. 31
 * As you can see, I've wired my servos differently from what manual says to better organize cables 
 */
leg_configuration_t LEG_A = { {31}, {30}, {29} };
leg_configuration_t LEG_B = { {27}, {26}, {25} };
leg_configuration_t LEG_C = { {23}, {22}, {21} };
leg_configuration_t LEG_D = { {13}, {14}, {15} };
leg_configuration_t LEG_E = { { 9}, {10}, {11} };
leg_configuration_t LEG_F = { { 5}, { 6}, { 7} };

leg_configuration_t ALL_LEGS[LEGS_PER_ROBOT] = { 
  LEG_A, LEG_B, LEG_C, LEG_D, LEG_E, LEG_F
};

// set one leg
void legSet(leg_configuration_t config, leg_state_t state) {
  servoSet(config.servo1, state.angle1);
  servoSet(config.servo2, state.angle2);
  servoSet(config.servo3, state.angle3);
}

// set multiple legs
void legSet(leg_configuration_t configs[], int size, leg_state_t states[]) {
  for (int i=0; i<size; i++) {
    legSet(configs[i], states[i]);
  }
}

// turn one leg off
void legTurnOff(leg_configuration_t config) {
  servoTurnOff(config.servo1);
  servoTurnOff(config.servo2);
  servoTurnOff(config.servo3);
}

// turn multiple legs off
void legTurnOff(leg_configuration_t configs[], int size) {
  for (int i=0; i<size; i++) {
    legTurnOff(configs[i]);
  }
}

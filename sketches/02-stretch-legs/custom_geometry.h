/*
  I've wired my servoes differently from what the manual says to better organize cables
  This file defines which PWM controller and channel each servo uses.
 */
#define PWM_SERVO_FREQUENCY 50 //TowerPro MG90S Servos run at 50 HZ

// robot leg "A"
#define LEGA_CONTROLLER 1
#define LEGA_SERVO1_CHANNEL 10  
#define LEGA_SERVO2_CHANNEL  9  
#define LEGA_SERVO3_CHANNEL  8

// robot leg "B"
#define LEGB_CONTROLLER 1
#define LEGB_SERVO1_CHANNEL  6  
#define LEGB_SERVO2_CHANNEL  5  
#define LEGB_SERVO3_CHANNEL  4

// robot leg "C"
#define LEGC_CONTROLLER 1
#define LEGC_SERVO1_CHANNEL 2  
#define LEGC_SERVO2_CHANNEL 1  
#define LEGC_SERVO3_CHANNEL 0

// robot leg "D"
#define LEGD_CONTROLLER 0
#define LEGD_SERVO1_CHANNEL 0  
#define LEGD_SERVO2_CHANNEL 1  
#define LEGD_SERVO3_CHANNEL 2

// robot leg "E"
#define LEGE_CONTROLLER 0
#define LEGE_SERVO1_CHANNEL 4  
#define LEGE_SERVO2_CHANNEL 5  
#define LEGE_SERVO3_CHANNEL 6

// robot leg "F"
#define LEGF_CONTROLLER 0
#define LEGF_SERVO1_CHANNEL 8  
#define LEGF_SERVO2_CHANNEL 9  
#define LEGF_SERVO3_CHANNEL 10

// total nuber of legs
#define NUM_LEGS 6

// describes a servo in given state 
typedef struct {
  uint8_t pwmIndex;
  uint8_t channelIndex;
  uint16_t angle;
} servo_descriptor_t;

// describes a leg of 3 servoes in given state
typedef struct {
  char label;
  servo_descriptor_t servo1;
  servo_descriptor_t servo2;
  servo_descriptor_t servo3;
} leg_descriptor_t;

// describes the state of the entire robot
typedef struct {
  leg_descriptor_t legs[NUM_LEGS];
} robot_descriptor_t; 

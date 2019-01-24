/*
* The servo layer  
*/

// describes a servo configuration 
typedef struct {
  uint8_t channel;
} servo_configuration_t;

// convert from degrees to PWM pulses
int servoComputePulseLength (int degrees){
  if(degrees>=PWM_SERVO_ANGLEMAX){degrees=PWM_SERVO_ANGLEMAX;}
  if(degrees<=PWM_SERVO_ANGLEMIN){degrees=PWM_SERVO_ANGLEMIN;}
  return map(degrees,PWM_SERVO_ANGLEMIN,PWM_SERVO_ANGLEMAX,PWM_SERVO_PULSEMIN,PWM_SERVO_PULSEMAX);
}

// set one servo
void servoSet(servo_configuration_t servo, int angle){
    pwmSetPulse(servo.channel, servoComputePulseLength(angle));
}

// set multiple servos
void servoSet(servo_configuration_t servos[], int size, int angles[]){
    for (int i=0; i<size; i++) {
      servoSet(servos[i], angles[i]);
    }
}

// turn one servo off
void servoTurnOff(servo_configuration_t config) {
  pwmSetPulse(config.channel, PWM_PIN_OFF);
}

// turn multiple servos off
void servoTurnOff(servo_configuration_t configs[], int size) {
  for (int i=0; i<size; i++) {
    servoTurnOff(configs[i]);
  }
}

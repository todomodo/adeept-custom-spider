/*
* The servo layer  
*/

// describes a servo configuration 
typedef struct {
  uint8_t channel;
} servo_configuration_t;

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

// set multiple servos
void setServos(servo_configuration_t servos[], int size, int angles[]){
    for (int i=0; i<size; i++) {
      setServo(servos[i], angles[i]);
    }
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

/*
 * due to problems with adeept driver, switching to adafruit
 * this performs basic GPIO test based on one of their examples
 * you should see the lamp blink and the servos twitch
 */
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define MAGIC_VALUE 4096  

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);

void setup() {
  pwm0.begin();
  pwm1.begin();  
  delay(10);
}

void loop() {  
    for (int i=0; i<32; i++) {
      setPin(i, MAGIC_VALUE, 0); // turns pin fully on
      delay(50);
      setPin(i, 0, MAGIC_VALUE); // turns pin fully off
    };
}

void setPin(int pinNum, int on, int off) {
  if (pinNum<=15) {
    pwm0.setPWM(pinNum, on, off);    
  } else {
    pwm1.setPWM(16-pinNum, on, off);
  }     
}

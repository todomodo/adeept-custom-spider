#include <Adeept_PWMPCA9685.h>

Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40);                //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write

const uint16_t LED_ON = 4095;
const uint8_t LED_GREEN_PIN = 13;
const uint8_t LED_BLUE_PIN = 14;
const uint8_t LED_WHITE_PIN = 15;
char color = '?';

void setup() {
  color = 'R';  
  pwm0.begin();
  pwm0.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

void loop() {
  
   if (color=='R') {
    // switch RGB light to red and set next color to green
    pwm0.setPWM(LED_GREEN_PIN, 0, LED_ON);
    pwm0.setPWM(LED_BLUE_PIN, 0, LED_ON);
    pwm0.setPWM(LED_WHITE_PIN, 0, 0);
    color = 'G'; 
   } else if (color== 'G') {
    // switch RGB light to green and set next color to blue
    pwm0.setPWM(LED_GREEN_PIN, 0, LED_ON);
    pwm0.setPWM(LED_BLUE_PIN, 0, 0);
    pwm0.setPWM(LED_WHITE_PIN, 0, 0);
    color = 'B'; 
   } else if (color== 'B') {
    // switch RGB light to  blue and set next color to white
    pwm0.setPWM(LED_GREEN_PIN, 0, 0);
    pwm0.setPWM(LED_BLUE_PIN, 0, LED_ON);
    pwm0.setPWM(LED_WHITE_PIN, 0, 0);
    color = 'W'; 
   } else if (color== 'W') {
    // switch RGB light to  white and set next color to red
    pwm0.setPWM(LED_GREEN_PIN, 0, 0);
    pwm0.setPWM(LED_BLUE_PIN, 0, 0);
    pwm0.setPWM(LED_WHITE_PIN, 0, LED_ON);
    color = 'R'; 
   }    

   delay(1500);
   
}

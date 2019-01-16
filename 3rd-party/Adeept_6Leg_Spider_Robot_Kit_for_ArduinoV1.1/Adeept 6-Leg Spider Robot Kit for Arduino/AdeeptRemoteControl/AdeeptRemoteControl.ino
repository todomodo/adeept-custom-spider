/***********************************************************
File name:  AdeeptRemoteControl.ino
Description:  
1.Push the left rocker up/forward and the robot will move forward. The farther you push each time, the faster the robot will move. 
2.Pull the left rocker down/backward and the robot will move backward. The farther you pull each time, the faster the robot will move. 
3.Pull the right rocker to the left and the robot will turn left. The farther you pull each time, the faster the robot will move. 
4.Pull the right rocker to the right and the robot will turn right. The farther you pull each time, the faster the robot will move. 
5.Press the button A and the robot will stand/squat. After it stands up, the RGB LED will turn into blue (or green); when it squats, this LED will change to red. 
Note: The robot will only move after it stands up. 
6.Press button B and the robot now enters the remote control mode. At the same time the LED2 on the remote will light up when the LED3 dims. And the RGB LED on the robot's head turns back to blue. 
7.Press button C to change into the mode of automatic obstacle avoidance. The LED3 lights up when LED2 goes out. The RGB LED turns into green. 
8.Press button D to control the buzzer beep of the module on the robot. 
Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2017/08/22 
***********************************************************/
#include <SPI.h>
#include "RF24.h"
RF24 radio(9, 10);            // define the object to control NRF24L01
byte addresses[5] = "00005";  // define communication address which should correspond to remote control
int data[9];                  // define array used to save the communication data
int mode = 0;
const int pot6Pin = 5;        // define R6
const int pot5Pin = 4;        // define R1
const int led1Pin = 6;        // define pin for LED1 which is close to NRF24L01 and used to indicate the state of NRF24L01 
const int led2Pin = 7;        // define pin for LED2 which is the mode is displayed in the Six foot robots remote control mode  
const int led3Pin = 8;        // define pin for LED3 which is the mode is displayed in the Six foot robots auto mode 
const int APin = 2;           // define pin for D2
const int BPin = 3;           // define pin for D3
const int CPin = 4;           // define pin for D4
const int DPin = 5;           // define pin for D5

const int u1XPin = 0;      // define pin for direction X of joystick U1
const int u1YPin = 1;      // define pin for direction Y of joystick U1
const int u2XPin = 2;      // define pin for direction X of joystick U2
const int u2YPin = 3;      // define pin for direction Y of joystick U2

void setup() {
  
  radio.begin();                      // initialize RF24
  radio.setRetries(0, 15);            // set retries times
  radio.setPALevel(RF24_PA_LOW);      // set power
  radio.openWritingPipe(addresses);   // open delivery channel
  radio.stopListening();              // stop monitoring
  pinMode(led1Pin, OUTPUT);           // set led1Pin to output mode
  pinMode(led2Pin, OUTPUT);           // set led2Pin to output mode
  pinMode(led3Pin, OUTPUT);           // set led3Pin to output mode
  pinMode(APin, INPUT_PULLUP);        // set APin to output mode
  pinMode(BPin, INPUT_PULLUP);        // set BPin to output mode
  pinMode(CPin, INPUT_PULLUP);        // set CPin to output mode
  pinMode(DPin, INPUT_PULLUP);        // set DPin to output mode  
  data[3] = 0;
  data[4] = 1;
}

void loop() {
  // put the values of rocker, switch and potentiometer into the array
  data[0] = analogRead(u1XPin);
  data[1] = analogRead(u1YPin);
  if(digitalRead(APin)==LOW){  
    delay(100);
    data[2] = digitalRead(APin);
    }else{
      data[2] = HIGH;
      }
  if( digitalRead(BPin)==LOW){//Switch the working mode
    mode = 0;
    data[3] = 0;
    data[4] = 1;
  }
  if(digitalRead(CPin)==LOW){//Switch the working mode
    mode = 1;
    data[3] = 1;
    data[4] = 0;
  }
  data[5] = digitalRead(DPin);
  data[6] = analogRead(pot5Pin);
  data[7] = analogRead(u2YPin);
  data[8] = analogRead(u2XPin);
  // send array data. If the sending succeeds, open signal LED
  if (radio.write( data, sizeof(data) ))
  digitalWrite(led1Pin,HIGH);

  // delay for a period of time, then turn off the signal LED for next sending
  delay(50);
  digitalWrite(led1Pin,LOW);

  if(mode==0){//LED display status
    digitalWrite(led2Pin,HIGH);
    digitalWrite(led3Pin,LOW);
  }
  if(mode==1){//LED display status
      digitalWrite(led2Pin,LOW);
      digitalWrite(led3Pin,HIGH);
  }
}

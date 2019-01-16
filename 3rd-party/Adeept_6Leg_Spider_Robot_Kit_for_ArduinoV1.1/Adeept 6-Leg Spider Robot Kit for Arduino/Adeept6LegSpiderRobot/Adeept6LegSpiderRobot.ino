/***********************************************************
File name:  Adeept6LegSpiderRobot.ino
Description:  In the example code provided, the robot supports 2 working modes: 
remote control and automatic obstacle avoidance. 
Under the remote control mode, you can make the bot stand, squat, go forward and backward, 
and turn left and right with the remotxe control. 
Under the mode of automatic obstacle avoidance, it will go forward automatically, and bypass 
obstacles in front if any and continue to walk ahead then. 

Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2017/08/23 
***********************************************************/
//                                //PWM31
//                                  head
//           (PWM2)(PWM1)(PWM0)//          //(PWM16)(PWM17)(PWM18)
//             Left forefoot                     right forefoot
//     (PWM5)(PWM4)(PWM3)//                    //(PWM19)(PWM20)(PWM21)
//          Left foot                                right foot
//          (PWM8)(PWM7)(PWM6)//           //（PWM22)(PWM23)(PWM24)
//            Left hind foot                     right hind foot
#include <Adeept_PWMPCA9685.h>
#include <SPI.h>
#include "RF24.h"

char pwm00Calibration = 0;
char pwm01Calibration = 0;
char pwm02Calibration = 0;
char pwm03Calibration = 0;
char pwm04Calibration = 0;
char pwm05Calibration = 0;
char pwm06Calibration = 0;
char pwm07Calibration = 0;
char pwm08Calibration = 0;
char pwm16Calibration = 0;
char pwm17Calibration = 0;
char pwm18Calibration = 0;
char pwm19Calibration = 0;
char pwm20Calibration = 0;
char pwm21Calibration = 0;
char pwm22Calibration = 0;
char pwm23Calibration = 0;
char pwm24Calibration = 0;

char pwm31Calibration = 0;//Ultrasonic steering gear offset
int movementSpeed = 100;  //The speed of movement of the robot
                
Adeept_PWMPCA9685 pwm0 = Adeept_PWMPCA9685(0x40);                //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write
Adeept_PWMPCA9685 pwm1 = Adeept_PWMPCA9685(0x41);                //1+A5 A4 A3 A2 A1 A0+RW, RW is Read and Write
uint8_t servonum = 0;

RF24 radio(9, 10);                // define the object to control NRF24L01
byte addresses[5] = "00005";      // define communication address which should correspond to remote control
int data[9]={512, 512, 1, 0, 1, 1, 512, 512, 512};  // define array used to save the communication data
int mode[1];
int trigPin = 3;                  // define Trig pin for ultrasonic ranging module
int echoPin = 2;                  // define Echo pin for ultrasonic ranging module
float maxDistance = 200;          // define the range(cm) for ultrasonic ranging module, Maximum sensor distance is rated at 400-500cm.
float soundVelocity = 340;        // Sound velocity = 340 m/s
float rangingTimeOut = 2 * maxDistance / 100 / soundVelocity * 1000000; // define the timeout(ms) for ultrasonic ranging module
const int buzzerPin = 8;  // define pin for buzzer

int automatic = 0;
int upOrDown = 0;
int oneTime = 0;
char oneForward = 0; //Avoid repeating the 'start***()' 'stop***()' program several times before proceeding
char oneBackward = 0;//Avoid repeating the 'start***()' 'stop***()' program multiple times in the back
char oneRight = 0;   //Avoid repeating the 'start***()' 'stop***()' program repeatedly on the right turn
char oneLeft = 0;    //Avoid repeating the 'start***()' 'stop***()' program multiple times on the left

char detectCommunication = 0;  //Check whether the communication is connected. Reboot the communication if it is not
char standingWay = 0;          //The setting can only be run when the robot is standing(standingWay=1)
char automaticTime = 0;        //Set the robot's operating mode(automaticTime=1:Automatic mode of operation.  automaticTime=0:Manual mode of operation)
void setup() {
  radio.begin();                      // initialize RF24
  radio.setRetries(0, 15);            // set retries times
  radio.setPALevel(RF24_PA_LOW);      // set power
  radio.openReadingPipe(1, addresses);// open delivery channel
  radio.startListening();             // start monitoring
  delay(100);
  
  pwm0.begin();
  pwm0.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  pwm1.begin();
  pwm1.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  pinMode(trigPin, OUTPUT); // set trigPin to output mode
  pinMode(echoPin, INPUT);  // set echoPin to input mode
  //Control the RGB light to display red
  pwm0.setPWM(13, 0, 4095);
  pwm0.setPWM(14, 0, 4095);
  pwm0.setPWM(15, 0, 0);
  calibration();
  pwm1.setPWM(15, 0, angle(pwm31Calibration + 90));//Control the PWM31 to 90 degrees
}

void loop() {

    receiveData();
    if(!data[2]){//Detect standing or squatting.
      upOrDown++ ;if(oneTime==0){down();oneTime=1;standingWay=1;}
      if(upOrDown>1){upOrDown=0;if(oneTime==1){up();oneTime=0;standingWay=0;}}
    }
    if(standingWay == 1){
        if(automatic == 1){//Automatic obstacle avoidance mode
        //Control RGB lights show green
        pwm0.setPWM(13, 0, 4095);
        pwm0.setPWM(14, 0, 0);
        pwm0.setPWM(15, 0, 4095);
        byte barDistance = maxDistance; // save the minimum measured distance from obstacles
        byte barDegree;                 // save the minimum measured angel from obstacles
        byte distance;                  // save the current the measured distance from obstacles
        // define the initial scanning position servo of pan tilt 
        pwm1.setPWM(15, 0, angle(pwm31Calibration + 90));  
        // start to scan distance. During this progress, we will get the distance and angle from the closest obstacle
        for (byte ultrasonicServoDegree = 40; ultrasonicServoDegree < 140; ultrasonicServoDegree += 10) {
            pwm1.setPWM(15, 0, angle(pwm31Calibration + ultrasonicServoDegree));  // steer pan tilt to corresponding position
            delay(50);                // wait 50ms between pings (about 40 pingsc). 29ms should be the shortest delay between pings.
            receiveData();
            distance = getDistance(); // detect the current distance from obstacle with angle of pan tilt stable
            if (distance < barDistance) { // if the current measured distance is smaller than the previous one, save the data of current measured distance
                barDegree = ultrasonicServoDegree;       // save the measured angle
                barDistance = distance;     // save the measured distance
            }
         }
         // servo of pan tilt turns to default position
         pwm1.setPWM(15, 0, angle(pwm31Calibration + 90)); 
         // According to the result of scanning control action of intelligent vehicles
         if(barDistance >= 30) {   // if the obstacle distance is not close, move on  
             if(automaticTime==0){startForward();automaticTime=1;}
            turnForward();
          }else { // if the obstacle distance is too close, reverse the travelling direction
             stopForward();startBackward();turnBackward();turnBackward();stopBackward();
             if(barDegree < 90){       // choose to reverse direction according to the angle with obstacle
                startLeftTow();turnLeftTow();turnLeftTow();stopLeftTow();automaticTime=0;
              }else{
                startRightTow();turnRightTow();turnRightTow();stopRightTow();automaticTime=0;
               }
            }
         }
        if(automatic == 0){//Remote control mode
          if(automaticTime!=0){
            stopForward();
            automaticTime=0;
          }
          //Control the RGB light to display blue
          pwm0.setPWM(13, 0, 0);
          pwm0.setPWM(14, 0, 4095);
          pwm0.setPWM(15, 0, 4095);
          if(data[1]>600&&oneBackward==0&&oneRight==0&&oneLeft==0){ //Go ahead
            if(oneForward==0){startForward();oneForward=1;}
            turnForward();
          }else if(oneForward==1){oneForward=0;stopForward();}
          
          if(data[1]<400&&oneForward==0&&oneRight==0&&oneLeft==0){//Backwards
            if(oneBackward==0){startBackward();oneBackward=1;}
            turnBackward();   
          }else if(oneBackward==1){oneBackward=0; stopBackward();}
          
          if(data[8]<400&&oneForward==0&&oneBackward==0&&oneLeft==0){//Turn right
            if(oneRight==0){startRightTow();oneRight=1;}
            turnRightTow();
          }else if(oneRight==1){oneRight=0; stopRightTow();} 
          
          if(data[8]>600&&oneForward==0&&oneBackward==0&&oneRight==0){//Turn left
            if(oneLeft==0){startLeftTow();oneLeft=1;}
            turnLeftTow();
          }else if(oneLeft==1){oneLeft=0;stopLeftTow();} 
          
         }
    }else{
       //Control the RGB light to display red
       pwm0.setPWM(13, 0, 4095);
       pwm0.setPWM(14, 0, 4095);
       pwm0.setPWM(15, 0, 0);
      }

     delay(100);
   //  upDown();
   //  standUp();
   //  calibration();
}
int angle(int angle){//Angle conversion
  if(angle>=180){angle=180;}
  if(angle<=0){angle=0;}
  return map(angle,0,180,130,600);//130-600
}
int oppAngle(int oppAngle){// The opposite direction angle conversion
  if(oppAngle>=180){oppAngle=180;}
  if(oppAngle<=0){oppAngle=0;}
  return map(oppAngle,0,180,600,130);//600-130
}

//Control robot back
void startBackward(void){
    int i;
    int j=90;
    pwm0.setPWM(2, 0, angle(pwm02Calibration + 170));  
    pwm0.setPWM(5, 0, angle(pwm05Calibration + 170));  
    pwm0.setPWM(8, 0, angle(pwm08Calibration + 170));  
    pwm1.setPWM(2, 0, oppAngle(pwm18Calibration + 170));  
    pwm1.setPWM(5, 0, oppAngle(pwm21Calibration + 170));  
    pwm1.setPWM(8, 0, oppAngle(pwm24Calibration + 170)); 

    for(i=90;i>50;i--){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        if(i>70){j--;if(j<=55){j=55;}}
        if(i<=70){j++;if(j>=90){j=90;}}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    for(i=90;i>50;i--){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        if(i>70){j--;if(j<=55){j=55;}}
        if(i<=70){j++;if(j>=90){j=90;}}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);  
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    for(i=90;i>50;i--){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        if(i>70){j--;if(j<=55){j=55;}}
        if(i<=70){j++;if(j>=90){j=90;}}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);  
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    j=90;
    for(i=50;i<120;i++){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); 
        delay(movementSpeed);   
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
}
void turnBackward(void){
    int i;
    int j=90;
    for(i=120;i>85;i--){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j--;if(j<=55){j=55;}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    for(i=85;i>50;i--){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j++;if(j>=90){j=90;}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    j=90;
    for(i=120;i>85;i--){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j--;if(j<=55){j=55;}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    for(i=85;i>50;i--){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j++;if(j>=90){j=90;}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    j=90;
    for(i=120;i>85;i--){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j--;if(j<=55){j=55;}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    for(i=85;i>50;i--){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j++;if(j>=90){j=90;}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed); 
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    j=90;
    for(i=50;i<120;i++){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); 
        delay(movementSpeed);  
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
}
void stopBackward(void){
    int i;
    int j=90;
    for(i=120;i>90;i--){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        if(i>105){j--;if(j<=75){j=75;}}
        if(i<=105){j++;if(j>=90){j=90;}}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
    for(i=120;i>90;i--){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        if(i>105){j--;if(j<=75){j=75;}}
        if(i<=105){j++;if(j>=90){j=90;}}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed); 
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        } 
    }
    for(i=120;i>90;i--){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        if(i>105){j--;if(j<=75){j=75;}}
        if(i<=105){j++;if(j>=90){j=90;}}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);  
        receiveData();
        if(data[1]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],0,400,10,100); 
        }
    }
}
//Control the robot forward
void startForward(void){
    int i;
    int j=90;
    pwm0.setPWM(2, 0, angle(pwm02Calibration + 170));  
    pwm0.setPWM(5, 0, angle(pwm05Calibration + 170));  
    pwm0.setPWM(8, 0, angle(pwm08Calibration + 170));  
    pwm1.setPWM(2, 0, oppAngle(pwm16Calibration + 170));  
    pwm1.setPWM(5, 0, oppAngle(pwm21Calibration + 170));  
    pwm1.setPWM(8, 0, oppAngle(pwm24Calibration + 170)); 
    for(i=90;i<130;i++){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        if(i<=110){j--;if(j<=55){j=55;}}
        if(i>110){j++;if(j>=90){j=90;}}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    for(i=90;i<130;i++){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        if(i<=110){j--;if(j<=55){j=55;}}
        if(i>110){j++;if(j>=90){j=90;}}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    for(i=90;i<130;i++){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        if(i<=110){j--;if(j<=55){j=55;}}
        if(i>110){j++;if(j>=90){j=90;}}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }  
    }
    j=90;
    for(i=130;i>60;i--){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); 
        delay(movementSpeed); 
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    } 
}
void turnForward(void){
    int i;
    int j=90;
    for(i=60;i<95;i++){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j--;if(j<=55){j=55;}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    for(i=95;i<130;i++){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j++;if(j>=90){j=90;}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed); 
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }  
    }
    j=90;
    for(i=60;i<95;i++){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j--;if(j<=55){j=55;}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    for(i=95;i<=130;i++){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j++;if(j>=90){j=90;}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);  
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    j=90;
    for(i=60;i<95;i++){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j--;if(j<=55){j=55;}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    for(i=95;i<=130;i++){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j++;if(j>=90){j=90;}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);  
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    j=90;
    for(i=130;i>60;i--){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); 
        delay(movementSpeed);  
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
}
void stopForward(void){
    int i;
    int j=90;
    for(i=60;i<90;i++){
        pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        if(i<=75){j--;if(j<=75){j=75;}}
        if(i>75){j++;if(j>=90){j=90;}}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);  
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
    for(i=60;i<90;i++){
        pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        if(i<=75){j--;if(j<=75){j=75;}}
        if(i>75){j++;if(j>=90){j=90;}}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    } 
    for(i=60;i<90;i++){
        pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        if(i<=75){j--;if(j<=75){j=75;}}
        if(i>75){j++;if(j>=90){j=90;}}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));
        delay(movementSpeed);   
        receiveData();
        if(data[1]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[1],600,1023,100,10);
        }
    }
}
//Control the robot to the left, two legs with the rotation
void startLeftTow(void){
     int i;
     int j=90;
     pwm0.setPWM(2, 0, angle(pwm02Calibration + 170));  
     pwm0.setPWM(5, 0, angle(pwm05Calibration + 170));  
     pwm0.setPWM(8, 0, angle(pwm08Calibration + 170));  
     pwm1.setPWM(2, 0, oppAngle(pwm18Calibration + 170));  
     pwm1.setPWM(5, 0, oppAngle(pwm21Calibration + 170));  
     pwm1.setPWM(8, 0, oppAngle(pwm24Calibration + 170));   
     for(i=90;i<115;i++){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j--;if(j<=65){j=65;}
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j));  
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=115;i<140;i++){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     j=90;
     for(i=90;i<115;i++){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i)); 
        j--;if(j<=65){j=65;}
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j));pwm1.setPWM(4, 0, angle(pwm20Calibration + j));  
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=115;i<140;i++){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     j=90;
     for(i=90;i<115;i++){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i));    pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); 
        j--;if(j<=65){j=65;}
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=115;i<140;i++){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(7, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=140;i>=40;i--){
     pwm0.setPWM(0, 0, angle( pwm00Calibration + i));  
     pwm0.setPWM(3, 0, angle( pwm03Calibration + i)); 
     pwm0.setPWM(6, 0, angle( pwm06Calibration + i)); 
     pwm1.setPWM(0, 0, angle( pwm16Calibration + i)); 
     pwm1.setPWM(3, 0, angle( pwm19Calibration + i)); 
     pwm1.setPWM(6, 0, angle( pwm20Calibration + i)); 
     delay(movementSpeed);
     receiveData();
     if(data[8]<=600){
        movementSpeed = 10;
     }else{
        movementSpeed = map(data[8],600,1023,100,10);
       }
     }
}
void turnLeftTow(void){
     int i;
     int j=90;
     for(i=40;i<90;i++){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i));  pwm1.setPWM(0, 0, angle(pwm16Calibration + i)); 
        j--;if(j<=0){j=40;} 
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=90;i<140;i++){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     j=90;
     for(i=40;i<90;i++){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j--;if(j<=0){j=40;} 
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=90;i<140;i++){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     j=90;
     for(i=40;i<90;i++){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); 
        j--;if(j<=0){j=40;} 
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=90;i<140;i++){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(7, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]<=600){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],600,1023,100,10);
        }
     }
     for(i=140;i>=40;i--){
     pwm0.setPWM(0, 0, angle( pwm00Calibration + i));  
     pwm0.setPWM(3, 0, angle( pwm03Calibration + i)); 
     pwm0.setPWM(6, 0, angle( pwm06Calibration + i)); 
     pwm1.setPWM(0, 0, angle( pwm16Calibration + i)); 
     pwm1.setPWM(3, 0, angle( pwm19Calibration + i)); 
     pwm1.setPWM(6, 0, angle( pwm22Calibration + i)); 
     delay(movementSpeed);
     receiveData();
     if(data[8]<=600){
        movementSpeed = 10;
     }else{
        movementSpeed = map(data[8],600,1023,100,10);
       }
     }
}
void stopLeftTow(void){
     int i;
     for(i=40;i<90;i++){
     pwm0.setPWM(0, 0, angle( pwm00Calibration + i));  
     pwm0.setPWM(3, 0, angle( pwm03Calibration + i)); 
     pwm0.setPWM(6, 0, angle( pwm06Calibration + i)); 
     pwm1.setPWM(0, 0, angle( pwm16Calibration + i)); 
     pwm1.setPWM(3, 0, angle( pwm19Calibration + i)); 
     pwm1.setPWM(6, 0, angle( pwm22Calibration + i)); 
     delay(movementSpeed);
     receiveData();
     if(data[8]<=600){
        movementSpeed = 10;
     }else{
        movementSpeed = map(data[8],600,1023,100,10);
      }
     }
}
//Control the robot to the left, two legs with the rotation
void startRightTow(void){
     int i;
     int j=90;
     pwm0.setPWM(2, 0, angle(pwm02Calibration + 170));  
     pwm0.setPWM(5, 0, angle(pwm05Calibration + 170));  
     pwm0.setPWM(8, 0, angle(pwm08Calibration + 170));  
     pwm1.setPWM(2, 0, oppAngle(pwm18Calibration + 170));  
     pwm1.setPWM(5, 0, oppAngle(pwm21Calibration + 170));  
     pwm1.setPWM(8, 0, oppAngle(pwm24Calibration + 170));  
     for(i=90;i>65;i--){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i)); pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + i));  
        pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); pwm1.setPWM(7, 0, angle(pwm23Calibration + i));
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     j=65;
     for(i=65;i>40;i--){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(7, 0, angle( pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     for(i=90;i>65;i--){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i));  pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + i));  
        pwm1.setPWM(3, 0, angle(pwm19Calibration + i));pwm1.setPWM(4, 0, angle(pwm20Calibration + i));
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     j=65;
     for(i=65;i>40;i--){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     for(i=90;i>65;i--){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i)); pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + i));  
        pwm1.setPWM(0, 0, angle(pwm16Calibration + i)); pwm1.setPWM(1, 0, angle(pwm17Calibration + i));
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     j=65;
     for(i=65;i>40;i--){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     for(i=40;i<=140;i++){
     pwm0.setPWM(0, 0, angle( pwm00Calibration + i));  
     pwm0.setPWM(3, 0, angle( pwm03Calibration + i)); 
     pwm0.setPWM(6, 0, angle( pwm06Calibration + i)); 
     pwm1.setPWM(0, 0, angle( pwm16Calibration + i)); 
     pwm1.setPWM(3, 0, angle( pwm19Calibration + i)); 
     pwm1.setPWM(6, 0, angle( pwm22Calibration + i)); 
     delay(movementSpeed);
     receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
}
void turnRightTow(void){
     int i;
     int j=90;
     for(i=140;i>90;i--){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i)); 
        j--;if(j<=0){j=40;} 
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(7, 0, angle(pwm23Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     for(i=90;i>40;i--){
        pwm0.setPWM(0, 0, angle(pwm00Calibration + i)); pwm1.setPWM(6, 0, angle(pwm22Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + j)); pwm1.setPWM(7, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     j=90;
     for(i=140;i>90;i--){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j--;if(j<=0){j=40;} 
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j));
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     for(i=90;i>40;i--){
        pwm0.setPWM(3, 0, angle(pwm03Calibration + i)); pwm1.setPWM(3, 0, angle(pwm19Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + j)); pwm1.setPWM(4, 0, angle(pwm20Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     j=90;
     for(i=140;i>90;i--){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i));  pwm1.setPWM(0, 0, angle(pwm16Calibration + i)); 
        j--;if(j<=0){j=40;} 
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     for(i=90;i>40;i--){
        pwm0.setPWM(6, 0, angle(pwm06Calibration + i)); pwm1.setPWM(0, 0, angle(pwm16Calibration + i));
        j++;if(j>=90){j=90;}  
        pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + j)); pwm1.setPWM(1, 0, angle(pwm17Calibration + j)); 
        delay(movementSpeed);
        receiveData();
        if(data[8]>=400){
          movementSpeed = 10;
        }else{
          movementSpeed = map(data[8],0,400,10,100); 
        }
     }
     for(i=40;i<=140;i++){
     pwm0.setPWM(0, 0, angle( pwm00Calibration + i));  
     pwm0.setPWM(3, 0, angle( pwm03Calibration + i)); 
     pwm0.setPWM(6, 0, angle( pwm06Calibration + i)); 
     pwm1.setPWM(0, 0, angle( pwm16Calibration + i)); 
     pwm1.setPWM(3, 0, angle( pwm19Calibration + i)); 
     pwm1.setPWM(6, 0, angle( pwm22Calibration + i)); 
     delay(movementSpeed);
     receiveData();
     if(data[8]>=400){
        movementSpeed = 10;
     }else{
         movementSpeed = map(data[8],0,400,10,100); 
       }
     }
}
void stopRightTow(void){
     int i;
     for(i=140;i>=90;i--){
     pwm0.setPWM(0, 0, angle( pwm00Calibration + i));  
     pwm0.setPWM(3, 0, angle( pwm03Calibration + i)); 
     pwm0.setPWM(6, 0, angle( pwm06Calibration + i)); 
     pwm1.setPWM(0, 0, angle( pwm16Calibration + i)); 
     pwm1.setPWM(3, 0, angle( pwm19Calibration + i)); 
     pwm1.setPWM(6, 0, angle( pwm22Calibration + i)); 
     delay(movementSpeed);
     receiveData();
     if(data[8]>=400){
        movementSpeed = 10;
     }else{
         movementSpeed = map(data[8],0,400,10,100); 
       }
     }
}
void down(void){
  int m=0;
  int n=0;
    //Each leg interval of about 60 degrees.
    pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + 90)); pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + 90)); pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + 90));
    pwm1.setPWM(0, 0, angle(pwm16Calibration + 90));    pwm1.setPWM(3, 0, angle(pwm19Calibration + 90));    pwm1.setPWM(6, 0, angle(pwm22Calibration + 90)); 
    m=10;n=30;
    receiveData();
    for(int i=0;i<20;i++){
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + m)); pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + m)); pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + m));
        pwm1.setPWM(1, 0, angle(pwm17Calibration + m));    pwm1.setPWM(4, 0, angle(pwm20Calibration + m));    pwm1.setPWM(7, 0, angle(pwm23Calibration + m));
        pwm0.setPWM(2, 0, oppAngle(pwm02Calibration + n)); pwm0.setPWM(5, 0, oppAngle(pwm05Calibration + n)); pwm0.setPWM(8, 0, oppAngle(pwm08Calibration + n));
        pwm1.setPWM(2, 0, angle(pwm18Calibration + n));    pwm1.setPWM(5, 0, angle(pwm21Calibration + n));    pwm1.setPWM(8, 0, angle(pwm24Calibration + n));
        delay(25);receiveData();delay(25);m++;n--;
    }
    receiveData();
    for(m=30;m<=90;m++){
    pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + m)); pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + m)); pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + m));
    pwm1.setPWM(1, 0, angle(pwm17Calibration + m));    pwm1.setPWM(4, 0, angle(pwm20Calibration + m));    pwm1.setPWM(7, 0, angle(pwm23Calibration + m));
    delay(25);receiveData();delay(25);
    }
}
void up(void){
    int m=0;
    int n=0;
    m=90;n=10;    
    receiveData();
    for(m=90;m>=30;m--){
    pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + m)); pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + m)); pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + m));
    pwm1.setPWM(1, 0, angle(pwm17Calibration + m));    pwm1.setPWM(4, 0, angle(pwm20Calibration + m));    pwm1.setPWM(7, 0, angle(pwm23Calibration + m));
    delay(25);receiveData();delay(25);
    }
    receiveData();
    for(int i=0;i<20;i++){
        pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + m)); pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + m)); pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + m));
        pwm1.setPWM(1, 0, angle(pwm17Calibration + m));    pwm1.setPWM(4, 0, angle(pwm20Calibration + m));    pwm1.setPWM(7, 0, angle(pwm23Calibration + m));
        pwm0.setPWM(2, 0, oppAngle(pwm02Calibration + n)); pwm0.setPWM(5, 0, oppAngle(pwm05Calibration + n)); pwm0.setPWM(8, 0, oppAngle(pwm08Calibration + n));
        pwm1.setPWM(2, 0, angle(pwm18Calibration + n));    pwm1.setPWM(5, 0, angle(pwm21Calibration + n));    pwm1.setPWM(8, 0, angle(pwm24Calibration + n));
        delay(25);receiveData();delay(25);m--;n++;
    }
}
void calibration(void){//6 feet position calibration
    //Six feet straight
    pwm0.setPWM(0, 0, oppAngle(pwm00Calibration + 90));
    pwm0.setPWM(3, 0, oppAngle(pwm03Calibration + 90));
    receiveData();delay(100);
    pwm0.setPWM(6, 0, oppAngle(pwm06Calibration + 90));
    pwm0.setPWM(1, 0, oppAngle(pwm01Calibration + 90));
    receiveData();delay(100);
    pwm0.setPWM(4, 0, oppAngle(pwm04Calibration + 90));
    pwm0.setPWM(7, 0, oppAngle(pwm07Calibration + 90));
    receiveData();delay(100);
    pwm0.setPWM(2, 0, oppAngle(pwm02Calibration + 90));
    pwm0.setPWM(5, 0, oppAngle(pwm05Calibration + 90));
    receiveData();delay(100);
    pwm0.setPWM(8, 0, oppAngle(pwm08Calibration + 90));
    pwm1.setPWM(0, 0, angle(pwm16Calibration + 90));
    receiveData();delay(100);
    pwm1.setPWM(3, 0, angle(pwm19Calibration + 90));
    pwm1.setPWM(6, 0, angle(pwm22Calibration + 90));
    receiveData();delay(100);
    pwm1.setPWM(1, 0, angle(pwm17Calibration + 90));
    pwm1.setPWM(4, 0, angle(pwm20Calibration + 90));
    receiveData();delay(100);
    pwm1.setPWM(7, 0, angle(pwm23Calibration + 90));
    pwm1.setPWM(2, 0, angle(pwm18Calibration + 90));
    receiveData();delay(100);
    pwm1.setPWM(5, 0, angle(pwm21Calibration + 90));
    pwm1.setPWM(8, 0, angle(pwm24Calibration + 90));
}

float getDistance() {
  unsigned long pingTime; // save the high level time returned by ultrasonic ranging module
  float distance;         // save the distance away from obstacle

  // set the trigPin output 10us high level to make the ultrasonic ranging module start to measure
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // get the high level time returned by ultrasonic ranging module
  pingTime = pulseIn(echoPin, HIGH, rangingTimeOut);

  if (pingTime != 0) {  // if the measure is not overtime
    distance = pingTime * soundVelocity / 2 / 10000;  // calculate the obstacle distance(cm) according to the time of high level returned
    return distance;    // return distance(cm)
  }
  else                  // if the measure is overtime
    return maxDistance; // returns the maximum distance(cm)
}
void receiveData(){
   detectCommunication++;
   if ( radio.available()) {             // if receive the data
    while (radio.available()) {          // read all the data
      radio.read( data, sizeof(data) );  // read data
    }
    detectCommunication = 0;
    if(!data[3]){
      automatic = 0;
    }
    if(!data[4]){
      automatic = 1;
    }  
    if (!data[5])// control the buzzer
      tone(buzzerPin, 2000);
    else
      noTone(buzzerPin);
   }
   if(detectCommunication>=100){
      detectCommunication = 0;
      radio.begin();                      // initialize RF24
      radio.setRetries(0, 15);            // set retries times
      radio.setPALevel(RF24_PA_LOW);      // set power
      radio.openReadingPipe(1, addresses);// open delivery channel
      radio.startListening();             // start monitoring
   }
   //movementSpeed = map(data[6],0,1023,0,100);
}


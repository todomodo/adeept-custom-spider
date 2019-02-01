/*
* Transmit remote control signals (via RF24 transmitter) from adeep remote (powered by
* "Arduino Nano").
*  -uses RF24 library version 1.3.0 (vs 1.1.6)
* - make sure to select ATmega328P (old bootloader) when uploading to the Nano
*/
#include <SPI.h>
#include <RF24.h>

/* 
* Configure nRF24L01 radio module on SPI bus plus pins 8 for CE & 9 for CS 
* arguments are (ce, cs), use transmit address of 0xF0F0F0F0E1LL
*/
RF24 _radio(9,10);
const uint64_t _transmit_pipe = 0xF0F0F0F0E1LL;


/*
 * pin mappings 
 */
const int LjXPin = 0;      // Left joystick, direction X
const int LjYPin = 1;      // Left joystick, direction Y
const int RjXPin = 2;      // Right joystick direction X
const int RjYPin = 3;      // Right joystick direction Y
const int led1Pin = 6;     // LED1, blinks of successful transmition
const int led2Pin = 7;     // LED2 - not used
const int led3Pin = 8;     // LED3 - not used
const int BtnAPin = 2;     // Push button A
const int BtnBPin = 3;     // Push button B
const int BtnCPin = 4;     // Push button C
const int BtnDPin = 5;     // Push button D
const int LPotPin = 5;     // Left potentiometer
const int RPotPin = 4;     // Right potentiometer


/*
 * The struct being transmitted
 */
typedef struct {
  int LjX,LjY,RjX,RjY;
  int BtnA,BtnB,BtnC,BtnD;
  int LPot, RPot;
} remote_state_t;
remote_state_t _last_state;

/*
* run once at startup
*/
void setup(void)
{
  //setup radio transmitter
  _radio.begin(); 
  _radio.openWritingPipe(_transmit_pipe); 
  _radio.setPALevel(RF24_PA_LOW);   
  _radio.stopListening();
  
  pinMode(led1Pin, OUTPUT);        // set led1Pin to output mode
  pinMode(led2Pin, OUTPUT);        // set led2Pin to output mode
  pinMode(led3Pin, OUTPUT);        // set led3Pin to output mode
  
  pinMode(BtnAPin, INPUT_PULLUP);  // set BtnAPin to input mode
  pinMode(BtnBPin, INPUT_PULLUP);  // set BtnBPin to input mode
  pinMode(BtnCPin, INPUT_PULLUP);  // set BtnCPin to input mode
  pinMode(BtnDPin, INPUT_PULLUP);  // set BtnDPin to input mode

  readState(&_last_state);
}

/*
* run constantly in the background
*/
void loop(void)
{
  remote_state_t current_state;
  readState(&current_state);
  if (!areEqual(&current_state, &_last_state)) {
    memcpy(&_last_state, &current_state, sizeof(remote_state_t));
    transmitState(&_last_state);
  }
}

/*
 * read the stae of various remote buttons and store it into the 
 * passed paramater
 */
void readState(remote_state_t* pState) {
  memset(pState,0, sizeof(remote_state_t));
  pState->LjX = analogRead(LjXPin);
  pState->LjY = analogRead(LjYPin);
  pState->RjX = analogRead(RjXPin);
  pState->RjY = analogRead(RjYPin);  
  pState->BtnA = digitalRead(BtnAPin);
  pState->BtnB = digitalRead(BtnBPin);
  pState->BtnC = digitalRead(BtnCPin);
  pState->BtnD = digitalRead(BtnDPin);

  /* 
   *  unfortunately the potentiometers appear to be broken
   *     
  pState->LPot = analogRead(LPotPin);
  pState->RPot = analogRead(RPotPin);    
  */
}

/*
 * true if two states are equal
 */
bool areEqual(remote_state_t* pSrc, remote_state_t* pDest) {
  //check joysticks
  const int JoystickNoise = 1;
  if (abs(pSrc->LjX - pDest->LjX) > JoystickNoise) return false;
  if (abs(pSrc->LjY - pDest->LjY) > JoystickNoise) return false;
  if (abs(pSrc->RjX - pDest->RjX) > JoystickNoise) return false;
  if (abs(pSrc->RjY - pDest->RjY) > JoystickNoise) return false;

  //check potentiometers
  const int PotentiometerNoise = 10;
  if (abs(pSrc->LPot - pDest->LPot) > PotentiometerNoise) return false;
  if (abs(pSrc->RPot - pDest->RPot) > PotentiometerNoise) return false;

  //check buttons
  if (pSrc->BtnA != pDest->BtnA) return false;
  if (pSrc->BtnB != pDest->BtnB) return false;
  if (pSrc->BtnC != pDest->BtnC) return false;
  if (pSrc->BtnD != pDest->BtnD) return false;
  
  return true;  
}

/*
 * transmit state over the radio
 */
void transmitState(remote_state_t* pState) {
  if (_radio.write( pState, sizeof(remote_state_t) ))
  {
    digitalWrite(led1Pin,HIGH);
    delay(50);
    digitalWrite(led1Pin,LOW);
  }  
}




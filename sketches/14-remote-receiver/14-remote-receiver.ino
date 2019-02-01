/*
 * Receives signals from the remote transmiiter
 */
#include <SPI.h>
#include <RF24.h>

/* 
* Configure nRF24L01 radio module on SPI bus plus pins 8 for CE & 9 for CS 
* arguments are (ce, cs), use receive address of 0xF0F0F0F0E1LL
*/
RF24 _radio(9,10);
const uint64_t _receive_pipe = 0xF0F0F0F0E1LL;

/*
 * The struct being transmitted
 */
typedef struct {
  int LjX,LjY,RjX,RjY;
  int BtnA,BtnB,BtnC,BtnD;
  int LPot, RPot;
} remote_state_t;

/*
* run once at startup
*/
void setup(void)
{
  Serial.begin(9600);
  _radio.begin(); 
  _radio.openReadingPipe(0,_receive_pipe); 
  _radio.setPALevel(RF24_PA_LOW); 
  _radio.startListening();
}

/*
* run constantly in the background
*/
void loop(void)
{  
  while (_radio.available()) {    
    remote_state_t state;
    _radio.read(&state,sizeof(remote_state_t));
    printState(&state);
  } 
}

void printState(remote_state_t* pState) {
   char buffer[200];
   sprintf(buffer, "LjX=%4d, LjY=%4d, RjX=%4d, RjY=%4d, A=%d, B=%d, C=%d, D=%d, P1=%d, P2=%d", 
      pState->LjX, pState->LjY, pState->RjX, pState->RjY,
      pState->BtnA, pState->BtnB, pState->BtnC, pState->BtnD,
      pState->LPot, pState->RPot);
   Serial.println(buffer);
}


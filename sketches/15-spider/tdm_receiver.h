/*
 * receives sugnals from the remote
 */
#pragma once

#include <SPI.h>
#include <RF24.h>

/* 
* Configure nRF24L01 radio module on SPI bus plus pins 8 for CE & 9 for CS 
* arguments are (ce, cs), use receive address of 0xF0F0F0F0E1LL
*/
#define TDM_RECEIVE_ADDRESS     0xF0F0F0F0E1LL
#define TDM_RECEIVE_CE_PIN      9
#define TDM_RECEIVE_CS_PIN      10
#define TDM_RECEIVE_BUTTON_DOWN 0


namespace tdm {

  /*
  * The struct being transmitted
  */
  typedef struct {
    int LjX,LjY,RjX,RjY;
    int BtnA,BtnB,BtnC,BtnD;
    int LPot, RPot;
  } remote_state_t;

 /*
  * The struct being transmitted
  */
  enum Command {
    STAND_UP,
    STAND_DOWN
  };

  /*
   * This should have been a static member of the Receiver class, however given the fact how statics
   * behave from .H files, we'll keep it global
   */
  RF24 *_CACHED_RADIO;
  Queue<Command> *_CACHED_EVENTS;

   /*
   * A wrapper around RF24 library object living inside a timer
   */
  class Receiver : public Timer2 {
  
    public:
      /*
       * initialize for first use
       */
      void onSetup(void) {
        if (_CACHED_RADIO==NULL) {
          _CACHED_EVENTS = new Queue<Command>();
          _CACHED_RADIO = new RF24(TDM_RECEIVE_CE_PIN,TDM_RECEIVE_CS_PIN);
          _CACHED_RADIO->begin(); 
          _CACHED_RADIO->openReadingPipe(0,TDM_RECEIVE_ADDRESS); 
          _CACHED_RADIO->setPALevel(RF24_PA_LOW); 
          _CACHED_RADIO->startListening();          
          tdm::Timer2::onSetup();
        }
      };

      /*
      * the worker
      */
      void doWork(void) {
        if (_CACHED_RADIO->available()) {    
          remote_state_t state;
          read(&state);
          if (state.BtnA ==TDM_RECEIVE_BUTTON_DOWN) {
            _CACHED_EVENTS->push(Command::STAND_UP);
          } else if (state.BtnD ==TDM_RECEIVE_BUTTON_DOWN) {
            _CACHED_EVENTS->push(Command::STAND_DOWN);
          }
          printState(&state);
        }    
      }

      bool hasCommand() {
        return (_CACHED_EVENTS->count() > 0);
      }

      Command readCommand() {
        return _CACHED_EVENTS->pop();
      }
      
    private:      
      /*
       * read one packet, if available
       */
      void read(remote_state_t *pState) {
        _CACHED_RADIO->read(pState,sizeof(remote_state_t));
      }

      /*
       * debug print function
       */
      void printState(remote_state_t* pState) {
        char buffer[200];
        sprintf(buffer, "LjX=%4d, LjY=%4d, RjX=%4d, RjY=%4d, A=%d, B=%d, C=%d, D=%d, P1=%d, P2=%d", 
            pState->LjX, pState->LjY, pState->RjX, pState->RjY,
            pState->BtnA, pState->BtnB, pState->BtnC, pState->BtnD,
            pState->LPot, pState->RPot);
        //::_STDOUT.println(buffer);
      };

     
  }; //class

}; //namespace

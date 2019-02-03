/*
 * We finally have enough parts to assemble a working spider
 */
#include "tdm_serial.h"
#include "tdm_vector.h"
#include "tdm_pwmio.h"
#include "tdm_lights.h"
#include "tdm_servos.h"
#include "tdm_legs.h"
#include "tdm_motion.h"
#include "tdm_timers.h"
#include "tdm_queue.h"
#include "tdm_receiver.h"


tdm::Receiver _receiver;

/*
* run once at startup
*/
void setup(void)
{
  _receiver.onSetup();  
}

/*
* run constantly in the background
*/
void loop(void)
{  
  if (_receiver.hasCommand()) {
    switch(_receiver.readCommand()) {
      case tdm::Command::STAND_UP:
        _STDOUT.println("STAND_UP");
        break;   
      case tdm::Command::STAND_DOWN:
        _STDOUT.println("STAND_DOWN");
        break;   
      default:
        _STDOUT.println("???");
    }    
  }  
}



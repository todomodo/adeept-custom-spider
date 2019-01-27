/*
  Blink RGB led using timer interrupts
 */

#include "tdm_serial.h"
#include "tdm_timers.h"
#include "tdm_pwmio.h"
#include "tdm_lights.h"


/*
 * Define an interrupt handler class 
 */
class Handler : public tdm::Timer2 {
  
  #define NUM_STATES 3
  tdm::PwmIO _pwm;  
  tdm::Led _led;
  tdm::rgb_color_t _colors[NUM_STATES] = {tdm::_COLOR_RED, tdm::_COLOR_GREEN, tdm::_COLOR_BLUE};
  int _index;
  uint16_t _total_interrupts;

  public:

    /*
     * the initializer
     */
    void onSetup(void) {
      _pwm.onSetup();
      tdm::Timer2::onSetup();
    }


    /*
    * the worker
    */
    void doWork(void) {

      /*
      * Keep record of total interrupts handled by this class
      */
      _total_interrupts++;
  
      /*
      * a simple trick to ensure our loop runs once/second
      */
      uint16_t reminder = _total_interrupts % getFrequency();    
      if (reminder == 0)
      {      
        /*
         * select the next color
         */
         if (_index>=NUM_STATES) _index=0;      
           
         /*
          * ATMEL 328/168 has a single level interrupts which means interrupts are disabled inside interrupt
          * handlers. Since I2C uses interrupts, any attempts to communicate with I2C devices (such as the PWM
          * controller's RGB light) from inside the interrupt handler will likely fail. The technique we use
          * here is to re-enable interrupts explicitly. This works but is considered risky
          */
         sei();
         
         /*
          * change color & output to console
          */
         _led.setColor(_colors[_index++]);
         _STDOUT.printf("doWork: frequency=%d interrupts=%d color=%d\n", getFrequency(), _total_interrupts, _index);
      }
    }
  
}; //class Handler


Handler _handler;


/*
 * called once at startup
 */
void setup() {
   
  _handler.onSetup();  
}

/*
 * called continuously 
 */
void loop() {    
  /* 
   * do nothing  
   */
}

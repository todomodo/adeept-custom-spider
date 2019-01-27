/*
 * In a more traditional environments, one would split into .H and .CPP files. However in Arduino, the ".ino" file
 * comes with certain built-in includes (such as "Arduino.h" and more) which are not automatically added to other CPP
 * files. So, to make things more seamless, I will be putting implementation directly into the header files, which may
 * look unnatural to C++ purists
 */
#pragma once


/*
 * dealing with timers in Arduino Uno (with ATMEL 328/168)
 * 
 * The Uno has three timers called timer0, timer1, and timer2.  Each of the timers has a counter that is 
 * incremented on each tick of the timer's clock.  CTC timer interrupts are triggered when the counter 
 * reaches a specified value, stored in the compare match register (CMR).  Once a timer counter reaches this 
 * value it will clear (reset to zero) on the next tick of the timer's clock, then it will continue to 
 * count up to the compare match value again.  By choosing the compare match value and setting the speed 
 * at which the timer increments the counter, you can control the frequency of timer interrupts.
 * 
 * Timer0 and timer2 are 8 bit timers, meaning they can store a maximum counter value of 255.  
 * Timer1 is a 16 bit timer, meaning it can store a maximum counter value of 65535. 
 * 
 * The increment speed is controlled by a prescaler. The prescaler can be 1, 8, 64, 256, and 1024. 
 *          Prescaler   IncrementSpeed
 *            1             16.0 MHz 
 *            8              2.0 MHz
 *           64            250.0 KHz 
 *          256             62.5 KHz
 *         1024             15.6 KHz 
 * 
 * Thus the minimum frequency for the 8 bit timer is 61Hz (Prescaler=1024, CMR=255) and for the 16 bit one is
 * 0.23 Hz (Prescaler=1024, CMR=65535)
 * 
 * More details at https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 *  
 */


/*
 *  master CPU clock frequency for ATMEL is 16 MHz
 */
#define TDM_CPU_CLOCK_FREQUENCY 16000000.0

namespace tdm {

  /*
   * a global pointer to our Timer2 class
   */   
  void* _TDM_TIMER2_HANDLER;

  class Timer2 {

    const uint16_t _presaler = 1024;
    const uint8_t _cmr = 0xFF;

    public:
      /*
       * Must be defined in derived classes
       */
      virtual void doWork(void);


   protected:   
     /*
      * Interrupt frequency in Hz. For example if _presaler=1024 and _cmr=255, the 
      * interrupt frequency will be 61Hz
      */
      int getFrequency() {
        return TDM_CPU_CLOCK_FREQUENCY / ((float)_presaler * (float) _cmr);
      }
                    
      /*
       * Setup timer2 to handle interrupts in CTC mode 
       */
      virtual void onSetup(void) {
          
          /*
           * stop interrupt handling
           */
          cli();

          /*
           * initi global variable with pointer to this class. Since we dont clear that pointer
           * our class should be a singletom. This is currently not enforced but could be easily done 
           * with factory pattern
           */
          _TDM_TIMER2_HANDLER = this;
      
          /* 
           * clear registers 
           */
          TCCR2A = 0; TCCR2B = 0;
      
          /* 
           * set initial counter value to 0 
           */
          TCNT2  = 0;                      
      
          /* Configure the timer mode.  Three different timer modes are available for Uno: "Normal Mode",
           * "CTC Mode" and "PWM Mode". We want "CTC Mode" 
           */
          TCCR2A |= (1 << WGM21);
      
          /* 
           *  Set for 1024 prescaler. According to some sources timer2 only supports up to 64 prescaler, 
           * however my experiments with recent (2018+) Arduino Uno show that 1024 prescaler is possible
          */
          TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
      
          /* 
           *  Set CMR to its max possible value because we want slow timers. Interrupts will be generated
           *  every time this value is reached. Prescaler 1024 + CMR 255 = InterruptFrequency 61Hz
           */
          OCR2A = _cmr;      
      
          /* 
           * Enable interrupt on compare match. You will need to define a "ISR (TIMER2_COMPA_vect)" function
           * somewhere in your code to handle the interrupt. Note that ATMEL 328/168 has a single level interrupts
           * which means interrupts are disabled inside interrupt handlers. Since I2C uses interrupts, any attempts 
           * to communicate with I2C devices from within the interrupt handler will likely fail. One possible
           * technique of geting around this limitation is to explicitly enable interrupts inside the handlers. 
           * This may work, however is considered risky
           */   
          TIMSK2 |= (1 << OCIE2A);                              
      
          /* 
           * resume interrupt handling 
           */
          sei();
      }
                 
  }; // class Timer2

};  //namespace


/*
 * called TIMER2_COMPA_FREQUENCY times per second by the timer interrupt
 * tranfers the call to Timer2 instance stored at  _TDM_TIMER2_HANDLER global variable
 */
ISR (TIMER2_COMPA_vect)
{
  if (tdm::_TDM_TIMER2_HANDLER!=NULL) {
    ((tdm::Timer2*)tdm::_TDM_TIMER2_HANDLER)->doWork();
  }
}

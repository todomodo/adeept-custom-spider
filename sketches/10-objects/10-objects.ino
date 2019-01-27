/*
  Experiment with a more OO approach
 */

#include "tdm_vector.h"
#include "tdm_serial.h"

/*
 * a simple string-like class
 */
class string {

  private:
    char value_[30];
 
  public:
    string() : string ("") {
    }
    
    string(const char* value) {
     strncpy(value_, value, 30);
    }    

    char* value(void) { return value_; }
};

/*
 * let's have a vector of these
 */
tdm::Vector<string> words;
uint8_t index = 0;
tdm::Serial _serial;

/*
 * called once at startup
 */
void setup() {  
  _serial.onSetup();
  words.push_back(string("Alpha"));
  words.push_back(string("Bravo"));
  words.push_back(string("Delta"));
}

/*
 * called constantly in a loop
 */
void loop() {  
  _serial.printf("%d: %s\n", index, words[index].value());
  if (++index>=words.size()) {
    index =0;
  };
  delay(2000);
}

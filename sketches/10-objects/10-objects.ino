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
tdm::vector<string> words;
uint8_t index = 0;

/*
 * called once at startup
 */
void setup() {  
  tdm::Serial::setup();
  words.push_back(string("Alpha"));
  words.push_back(string("Bravo"));
  words.push_back(string("Delta"));
}

/*
 * called constantly in a loop
 */
void loop() {  
  tdm::Serial::printf("%d: %s\n", index, words[index].value());
  if (++index>=words.size()) {
    index =0;
  };
  delay(2000);
}

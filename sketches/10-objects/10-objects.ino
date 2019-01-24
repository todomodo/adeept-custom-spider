/*
  Experiment with a more OO approach
 */

#include "custom_vector.h"
#include "custom_serial.h"

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
vector<string> words;
uint8_t index = 0;

/*
 * called once at startup
 */
void setup() {  
  serialSetup();
  words.push_back(string("Alpha"));
  words.push_back(string("Bravo"));
  words.push_back(string("Delta"));
}

/*
 * called constantly in a loop
 */
void loop() {  
  cu_printf("%d: %s\n", index, words[index].value());
  if (++index>=words.size()) {
    index =0;
  };
  delay(2000);
}

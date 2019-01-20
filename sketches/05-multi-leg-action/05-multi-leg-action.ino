/*
 * stand up on all 6 legs
 */
#include <Adeept_PWMPCA9685.h>
#include "custom_pwmio.h"
#include "custom_servos.h"
#include "custom_legs.h"


leg_state_t g_Lotus[LEGS_PER_ROBOT] = {
  {45, 50, 110 }, {50, 50, 110 }, {50, 50, 110 },
  {55, 50, 10 }, {50, 50, 10 }, {50, 50, 10 }
};

// executed once at startup
void setup() {
  setupPWM();
  setLegs(ALL_LEGS, LEGS_PER_ROBOT, g_Lotus);
}

// called continuously at runtime
void loop() {           
  delay(200);  
  
}

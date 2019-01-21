/*
 * The motion layer  
*/

/*
 * the global moton group can hold as many servos as there are PWN channels
 */
#define MOTION_GROUP_MAX_SIZE (PWM_NUMBER_OF_CONTROLLERS * PWM_CHANNELS_PER_CONTROLLER)  


typedef struct {
  int motion_steps;
  int motion_delay;  
} motion_type_t;

typedef struct {
  servo_configuration_t servos[MOTION_GROUP_MAX_SIZE];
  int current_angles[MOTION_GROUP_MAX_SIZE];
  int final_angles[MOTION_GROUP_MAX_SIZE];
  int angle_increments[MOTION_GROUP_MAX_SIZE];
  int size;
  motion_type_t motion_type;
} motion_group_t;

motion_group_t MOTION_GROUP;

/*
 * append one servo to the global motion group
 */
void appendToMotionGroup(servo_configuration_t servo, int start_angle) {
  if (MOTION_GROUP.size < MOTION_GROUP_MAX_SIZE)  {
    MOTION_GROUP.servos[MOTION_GROUP.size] = servo;
    MOTION_GROUP.current_angles[MOTION_GROUP.size] = start_angle;    
    MOTION_GROUP.size++; 
  }  
}

/*
 * append one servo to the global motion group
 */
void setMotionGroupFinalAngle(int index, int angle) {  
  if (index < MOTION_GROUP.size) {
    MOTION_GROUP.final_angles[index] = angle;
    MOTION_GROUP.angle_increments[index] = (MOTION_GROUP.final_angles[index]-MOTION_GROUP.current_angles[index]) / MOTION_GROUP.motion_type.motion_steps;    
  }
}

/*
 * begin motion for a random collection of servos. Initial servo position is set
 * immediately
 */
void beginMotion(servo_configuration_t servos[], int size, int start_angle[]) {
  MOTION_GROUP.size = 0;
  for(int i=0; i<size; i++) {
    appendToMotionGroup(servos[i], start_angle[i]);
  }
  setServos(MOTION_GROUP.servos, MOTION_GROUP.size,  MOTION_GROUP.current_angles);
}

/*
 * begin motion for a number of legs. Initial leg position is set
 * immediately
 */
void beginMotion(leg_configuration_t legs[], int size, leg_state_t start_angles[]) {
  MOTION_GROUP.size = 0;
  for(int i=0; i<size; i++) {
    appendToMotionGroup(legs[i].servo1, start_angles[i].angle1);
    appendToMotionGroup(legs[i].servo2, start_angles[i].angle2);
    appendToMotionGroup(legs[i].servo3, start_angles[i].angle3);
  }
  setServos(MOTION_GROUP.servos, MOTION_GROUP.size,  MOTION_GROUP.current_angles);
}


/*
 * Set new motion group target. The group must have been initialized bu calling "beginMotion"
 */
void moveTo(motion_type_t motion_type, int finish_angle[]) {
  MOTION_GROUP.motion_type = motion_type;
  for(int i=0; i<MOTION_GROUP.size; i++) {
    setMotionGroupFinalAngle(i, finish_angle[i]);
  } 
}

/*
 * Set new motion group target. The group must have been initialized bu calling "beginMotion"
 */
void moveTo(motion_type_t motion_type, leg_state_t finish_angles[],int size) {
  MOTION_GROUP.motion_type = motion_type;
  int index =0;
  for(int i=0; i<size; i++) {
    setMotionGroupFinalAngle(index++, finish_angles[i].angle1);
    setMotionGroupFinalAngle(index++, finish_angles[i].angle2);
    setMotionGroupFinalAngle(index++, finish_angles[i].angle3);    
  }  
}

/*
 * execute current motion. needs to be called from the main loop.
 * returns true if motion is still in progress
 */
bool doMotion(void) {
    bool in_progress = false;
    for(int i=0; i<MOTION_GROUP.size; i++) {
      if  ( (MOTION_GROUP.current_angles[i] != MOTION_GROUP.final_angles[i]) &&
            (MOTION_GROUP.angle_increments[i] !=0) ) {
                
        // apply the increment        
        in_progress = true;
        MOTION_GROUP.current_angles[i] += MOTION_GROUP.angle_increments[i];
        
        //correct overshoots to ensure we end up EXACTLY in the final position
        if (MOTION_GROUP.angle_increments[i]>0 && (MOTION_GROUP.current_angles[i] > MOTION_GROUP.final_angles[i])) {
          MOTION_GROUP.current_angles[i] = MOTION_GROUP.final_angles[i]; // correct positive overshoot
        } else if (MOTION_GROUP.angle_increments[i]<0 && (MOTION_GROUP.current_angles[i] < MOTION_GROUP.final_angles[i])) {
          MOTION_GROUP.current_angles[i] = MOTION_GROUP.final_angles[i]; // correct negative overshoot
        }        
      } 
    }

    if (in_progress) {
      setServos(MOTION_GROUP.servos, MOTION_GROUP.size,  MOTION_GROUP.current_angles);  
      delay(MOTION_GROUP.motion_type.motion_delay);
    }
       
    return in_progress;
}

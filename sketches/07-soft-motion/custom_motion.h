/*
 * The motion layer  
*/

/*
 * A frame is a collection of servo positions. The maximum number of servos is determined 
 * by the number of PWM channels
 */
#define MAX_FRAME_SIZE (PWM_NUMBER_OF_CONTROLLERS * PWM_CHANNELS_PER_CONTROLLER)  


typedef struct {
  int motion_steps;
  int motion_delay;  
} motion_type_t;

typedef struct {
  servo_configuration_t servos[MAX_FRAME_SIZE];	//participating servos
  int current_frame[MAX_FRAME_SIZE];			//current frame
  int target_frame[MAX_FRAME_SIZE];				//target frame
  int angle_increments[MAX_FRAME_SIZE];			//increments
  int frame_size;								//number of servos in the frame
  motion_type_t motion_type;					//the type of motion 
} motion_group_t;

motion_group_t MOTION_GROUP;

/*
 *  Append one servo to global motion group. This increases the motion group's 
 *	frame size
 */
void mgAppendServo(servo_configuration_t servo, int start_angle) {
  if (MOTION_GROUP.frame_size < MAX_FRAME_SIZE)  {
    MOTION_GROUP.servos[MOTION_GROUP.frame_size] = servo;
    MOTION_GROUP.current_frame[MOTION_GROUP.frame_size] = start_angle;    
    MOTION_GROUP.frame_size++; 
  }  
}

/*
 * set target angle for single servo
 */
void mgSetServoTarget(int index, int angle) {  
  if (index < MOTION_GROUP.frame_size) {
    MOTION_GROUP.target_frame[index] = angle;
    MOTION_GROUP.angle_increments[index] = (MOTION_GROUP.target_frame[index]-MOTION_GROUP.current_frame[index]) / MOTION_GROUP.motion_type.motion_steps;    
  }
}

/*
 * begin motion for a collection of servos. Initial servo position is set immediately
 */
void mgBeginMotion(servo_configuration_t servos[], int servo_count, int initial_positions[]) {
  MOTION_GROUP.frame_size = 0;
  for(int i=0; i<servo_count; i++) {
    mgAppendServo(servos[i], initial_positions[i]);
  }
  servoSet(MOTION_GROUP.servos, MOTION_GROUP.frame_size,  MOTION_GROUP.current_frame);
}

/*
 * begin motion for a number of legs. Initial leg position is set immediately
 */
void mgBeginMotion(leg_configuration_t legs[], int leg_count, leg_state_t initial_positions[]) {
  MOTION_GROUP.frame_size = 0;
  for(int i=0; i<leg_count; i++) {
    mgAppendServo(legs[i].servo1, initial_positions[i].angle1);
    mgAppendServo(legs[i].servo2, initial_positions[i].angle2);
    mgAppendServo(legs[i].servo3, initial_positions[i].angle3);
  }
  servoSet(MOTION_GROUP.servos, MOTION_GROUP.frame_size,  MOTION_GROUP.current_frame);
}


/*
 * Set new motion target. The group must have been initialized with "mgBeginMotion"
 */
void mgMoveTo(motion_type_t motion_type, int target_positions[]) {
  MOTION_GROUP.motion_type = motion_type;
  for(int i=0; i<MOTION_GROUP.frame_size; i++) {
    mgSetServoTarget(i, target_positions[i]);
  } 
}

/*
 * Set new motion group target. The group must have been initialized bu calling "mgBeginMotion"
 */
void mgMoveTo(motion_type_t motion_type, leg_state_t target_positions[],int size) {
  MOTION_GROUP.motion_type = motion_type;
  int index =0;
  for(int i=0; i<size; i++) {
    mgSetServoTarget(index++, target_positions[i].angle1);
    mgSetServoTarget(index++, target_positions[i].angle2);
    mgSetServoTarget(index++, target_positions[i].angle3);    
  }  
}

/*
 * execute current motion. needs to be called from the main loop.
 * returns true if motion is still in progress
 */
bool mgDoMotion(void) {
    bool in_progress = false;
    for(int i=0; i<MOTION_GROUP.frame_size; i++) {
      if  ( (MOTION_GROUP.current_frame[i] != MOTION_GROUP.target_frame[i]) &&
            (MOTION_GROUP.angle_increments[i] !=0) ) {
                
        // apply the increment        
        in_progress = true;
        MOTION_GROUP.current_frame[i] += MOTION_GROUP.angle_increments[i];
        
        //correct overshoots to ensure we end up EXACTLY in the final position        
        if (MOTION_GROUP.angle_increments[i]>0 && (MOTION_GROUP.current_frame[i] > MOTION_GROUP.target_frame[i])) {
          MOTION_GROUP.current_frame[i] = MOTION_GROUP.target_frame[i]; // correct positive overshoot
        } else if (MOTION_GROUP.angle_increments[i]<0 && (MOTION_GROUP.current_frame[i] < MOTION_GROUP.target_frame[i])) {
          MOTION_GROUP.current_frame[i] = MOTION_GROUP.target_frame[i]; // correct negative overshoot
        }                
      } 
    }

    if (in_progress) {
      servoSet(MOTION_GROUP.servos, MOTION_GROUP.frame_size,  MOTION_GROUP.current_frame);  
      delay(MOTION_GROUP.motion_type.motion_delay);
    }
       
    return in_progress;
}

/*
 * In a more traditional environments, one would split into .H and .CPP files. However in Arduino, the ".ino" file
 * comes with certain built-in includes (such as "Arduino.h" and more) which are not automatically added to other CPP
 * files. So, to make things more seamless, I will be putting implementation directly into the header files, which may
 * look unnatural to C++ purists
 */
#pragma once

namespace tdm {
  /*
   * The motion layer.   
   * 
   * A frame is a collection of servo positions. The maximum number of servos is determined 
   * by the number of PWM channels
   */

  typedef struct {
    int motion_steps;
    int motion_delay;  
  } motion_type_t;
  
  class ServoMover {

    private:
      ServoGroup* _servos;	                      //participating servos
      int _current_frame[TDM_SERVO_MAXCOUNT];			//current frame
      int _target_frame[TDM_SERVO_MAXCOUNT];			//target frame
      int _increment[TDM_SERVO_MAXCOUNT];	        //increments
      motion_type_t _motion_type;					        //the type of motion 
      


    public:

      ServoMover(ServoGroup* servos, motion_type_t motion_type) {
        _servos = servos;
        _motion_type = motion_type;        
      }

     /*
     * begin motion for a collection of servos. Initial servo positions are set immediately
     */
      void jumpTo(int angles[]) {
        for(int i=0; i<_servos->size(); i++) {
          _current_frame[i] = angles[i];
        }
        _servos->setAngles(_current_frame);
      }

      /*
       * Set new motion target. The group must have been initialized with "jumpTo"
       */
      void moveTo(int angles[]) {
        for(int i=0; i<_servos->size(); i++) {
          _target_frame[i] =  angles[i];
          _increment[i] = (_target_frame[i]-_current_frame[i]) / _motion_type.motion_steps;    
        } 
      }

    /*
     * execute current motion. needs to be called from the main loop.
     * returns true if motion is still in progress
     */
    bool doMotion(void) {
        bool in_progress = false;
        for(int i=0; i<_servos->size(); i++) {
          if  ( (_current_frame[i] != _target_frame[i]) && (_increment[i] !=0) ) {
                    
            // apply the increment        
            in_progress = true;
            _current_frame[i] += _increment[i];
            
            //correct overshoots to ensure we end up EXACTLY in the final position        
            if (_increment[i]>0 && (_current_frame[i] > _target_frame[i])) {
              _current_frame[i] = _target_frame[i]; // correct positive overshoot
            } else if (_increment[i]<0 && (_current_frame[i] < _target_frame[i])) {
              _current_frame[i] = _target_frame[i]; // correct negative overshoot
            }                
          } 
        }
    
        if (in_progress) {
          _servos->setAngles(_current_frame);
          delay(_motion_type.motion_delay);
        }
           
        return in_progress;
    }
        
  }; // class ServoMover


  class LegMover : public ServoMover {
    
    private:
      LegGroup* _legs;

    public:
      LegMover(LegGroup* legs, motion_type_t motion_type) : ServoMover(toServoGroup(legs), motion_type) {
        _legs = legs;
      }

      /*
      * begin motion for a collection of legs. Initial leg positions are set immediately
      */
      void jumpTo(int angles[][TDM_SERVOS_PER_LEG]) {
        int servo_angles[_legs->size() * TDM_SERVOS_PER_LEG];
        int index = 0;
        for(int leg=0; leg < _legs->size(); leg++) {
            for(int servo=0; servo < TDM_SERVOS_PER_LEG; servo++) {
              servo_angles[index++] = angles[leg][servo];            
            }
        }
        ServoMover::jumpTo(servo_angles);      
      }


       /*
       * Set new motion target. The group must have been initialized with "jumpTo"
       */
      void moveTo(int angles[][TDM_SERVOS_PER_LEG]) {
        int servo_angles[_legs->size() * TDM_SERVOS_PER_LEG];
        int index = 0;
        for(int leg=0; leg < _legs->size(); leg++) {
            for(int servo=0; servo < TDM_SERVOS_PER_LEG; servo++) {
              servo_angles[index++] = angles[leg][servo];            
            }
        }
        ServoMover::moveTo(servo_angles);      
      }

      /*
       * Convert to ServoGroup
       */
       private:
       static ServoGroup* toServoGroup(LegGroup* legs) {        
          ServoGroup* sg = new ServoGroup(legs->size() * TDM_SERVOS_PER_LEG);
          int index = 0;
          for(int leg=0; leg < legs->size(); leg++) {
            for(int servo=0; servo < TDM_SERVOS_PER_LEG; servo++) {
              sg->setServo(legs->getLeg(leg)->getServo(servo),index++);
            }
          }
          return sg;
       }
  
  }; // class LegMover
  
}; //namespace

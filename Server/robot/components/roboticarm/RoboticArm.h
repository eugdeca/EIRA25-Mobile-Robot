/*The arm has a dedicated thread that waits for a target tilt to be set. If it recives a goel while
a previous one is bein processed it ignores it: you can controll tilt only when the arm is still.
The battery is discharged 8% every second the arm moves.*/
#ifndef ROBOTICARM_H
#define ROBOTICARM_H
#include <iostream>
#include <pthread.h>
#include "../../../utilities/Utilities.h"
#include "../battery/Battery.h"
using namespace std;

void* executeArm(void* arg);

class RoboticArm{
    protected:
      float tilt;
      float targetTilt;
      bool isMoving;

      pthread_t armThread;
      bool running;

      Battery* battery;
      float dischargeAmount;

      pthread_mutex_t arm_mutex;
      pthread_cond_t new_target;

    public:
      RoboticArm(Battery* battery);
      ~RoboticArm();

      bool moveToTilt(float targetTilt);  
      void execute();
      
      float getTilt();
      float getTargetTilt();
      void setTilt(float tilt);
};

#endif //ROBOTICARM_H
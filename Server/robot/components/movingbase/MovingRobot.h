/*The moving base has a dedicated thread. In this part there are not mutex because the reading and 
the writing of the position and other attributes is considered atomic and this part is the only one 
to have acces to the position angle and speed data. 
The battery is discharged 5.3% every second the base moves.*/
#ifndef MOVINGROBOT_H
#define MOVINGROBOT_H
#include <iostream>
#include <pthread.h>
#include <cmath>
#include "../../../utilities/Utilities.h"
#include "../battery/Battery.h"
#include "../../../environment/Environment.h"
using namespace std;

void* Move(void* arg);

class MovingRobot{
    protected:
      struct dataStruct {
        float pos[2];                    // x,y position
        float angle;                     // Angle w.r.t. North, positive counterclockwise
        float speed;
        float maxSpeed;                  // Speed [m/s] 
      };
      dataStruct data;

      pthread_t movingThread;
      bool running;

      Battery* battery;
      Environment* env;
      float dischargeAmount;

    public:
      MovingRobot(Battery* battery, Environment* env);
      MovingRobot(float x, float y,Battery* battery, Environment* env);
      ~MovingRobot();

      void MoveBase();

      float getX();
      float getY();
      float getAngle();
      float getSpeed();

      void setSpeedPercentage(int SpeedPercentage);
      void setAngle(float angle);
      
};

#endif //MOVINGROBOT_H
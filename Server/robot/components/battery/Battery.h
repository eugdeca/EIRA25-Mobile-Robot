#ifndef BATTERY_H
#define BATTERY_H
#include <iostream>
#include <pthread.h>
using namespace std;

class Battery{
    protected:
      float batt_percent;                       // Battery percentage
      pthread_mutex_t batt_mutex;               // Mutex to access battery percentage
      pthread_cond_t cond_discharged;           // Condition if there is no battery
      pthread_cond_t cond_charged;              // Condition if the battery is full
      bool battDischarged;
      bool stop;                                // When the robot is destroyed it sets stop to true so the battery knows it has to stop

    public:
      Battery();
      Battery(float batt_percent);
      ~Battery();

      float charge(float q);
      bool discharge(float q);

      bool getBatteryDischarged();
      float getLevel();
      void print();
      bool getStop();
      void setStop(bool stop);

      pthread_mutex_t& getBatt_mutex();
      pthread_cond_t& getCond_discharged();    
      pthread_cond_t& getCond_charged();  
};

#endif //BATTERY_H
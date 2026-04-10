/*Gripper has a dedicated thread. It is present a queue to handle multiple command sent to the gripper
(FIFO) but this queue has room for maximum 10 consecutive commands (chosen in the Utilities).
The battery is discharged 3.7% every second the base moves. */
#ifndef GRIPPER_H
#define GRIPPER_H
#include <iostream>
#include <pthread.h>
#include "../../../utilities/Utilities.h"
#include "../battery/Battery.h"
using namespace std;

void* executeGripper(void* arg);

class Gripper{
    protected:
      short int gripState;
      Utilities::queue buffer;

      pthread_t gripperThread;
      bool running;

      Battery* battery;
      float dischargeAmount;

      pthread_mutex_t buffer_mutex;
      pthread_cond_t not_empty;
      // The following 2 methods are called by the thread, they should not be accessible by other components
      void openGripper();                           // Opens and discharges and sleeps
      void closeGripper();                          // Closes and discharges and sleeps
    
    public:
      Gripper(Battery* battery);                    // Start thread in constructor
      ~Gripper();                                   // Join to wai for thread to end when the robot is shut down
      
      void execute();                               // Thread loop function that calls open or close
      
      bool addCommand(char cmd);                    // Function to insert a command in the buffer  

      short int getGripState();

      pthread_mutex_t& getBuffer_mutex();               
      pthread_cond_t& getCond_notEmpty();               
};

#endif // GRIPPER_H
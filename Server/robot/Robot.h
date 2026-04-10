/*The robot has a thread to charge the battery and as many threads as the maximum number of controllers
allowed to controll it (this number is chosen by the user in the constructor). The first thread
starts with the robot, the other ones start when startService is called.*/
#ifndef ROBOT_H
#define ROBOT_H
#include "components/battery/Battery.h"
#include "components/gripper/Gripper.h"
#include "components/movingbase/MovingRobot.h"
#include "components/roboticarm/RoboticArm.h"
#include "../environment/Environment.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void* startChargeThread(void* arg);

void* startSockThreads(void* arg);

class Robot{
    protected:
      Battery* battery;
      Gripper* gripper;
      MovingRobot* movingBase;
      RoboticArm* arm;
      Environment* env;

      pthread_t chargerThread;
      bool shutDown;
      bool remoteEnabled;

      int RobotSock;
      int MAX;
      pthread_t* sockThreads;
      pthread_mutex_t robotMutex; 
      pthread_mutex_t acceptMutex;

    public:
      Robot(Environment* env);
      Robot(Environment* env, int MAX);
      ~Robot();

      void Charge();
      //EnergyHarvest
      void solarPanel(float q, float harvest[2]); 
      void windTurbine(float q, float harvest[2]);

      // Socket
      void startService();
      void runController(int id);

      bool kill;
      // --- METODS FOR MANUAL TEST ---
      Battery* getBattery() { return battery; }
      Gripper* getGripper() { return gripper; }
      RoboticArm* getArm() { return arm; }
      MovingRobot* getBase() { return movingBase; }
      // ----------------------------------------------

};

#endif // ROBOT_H
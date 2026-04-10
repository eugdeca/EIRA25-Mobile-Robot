/* Joypad connects to the robot as a client. It does not contain threads because the 
multi controller problem is handled inside the robot. It has a flag "connected" just to 
know if the connection to socket worked or not. It presents all the possible actions that 
can be taken to controll the robot.*/
#ifndef JOYPAD_H
#define JOYPAD_H
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "../utilities/Utilities.h"
using namespace std;

class Joypad{
    protected:
      int sock;
      bool connected;
      void initializeConnection();
    public:
      Joypad();
      ~Joypad();
      void sendActivate();
      void sendDeactivate();
      void sendStop();
      void sendAngleAndSpeed(float angle, uint32_t speedPerc);
      void sendTilt(float tilt);
      void sendClose();
      void sendOpen();
      void sendGetState();
};

#endif //JOYPAD_H
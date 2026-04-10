/* Safety Button connects to the robot as a client. It does not contain threads because the 
multi controller problem is handled inside the robot. It has a flag "connected" just to 
know if the connection to socket worked or not. It presents only activation, disactivation
and stop.*/
#ifndef SAFETYBUTTON_H
#define SAFETYBUTTON_H
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "../utilities/Utilities.h"
using namespace std;

class SafetyButton{
    protected:
      int sock;
      bool connected;
      void initializeConnection();
    public:
      SafetyButton();
      ~SafetyButton();
      void sendActivate();
      void sendDeactivate();
      void sendStop();
};

#endif //SAFETYBUTTON_H
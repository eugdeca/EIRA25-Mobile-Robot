// Utilities needed, client side, to let the program sleep and to manipulate floats
// between Network and Host.

#ifndef UTILITIES_H
#define UTILITIES_H
#include <iostream>
#include <arpa/inet.h>
#include <cstdlib>
#include <time.h>
using namespace std;

class Utilities{      
    public:
      // ----- Socket Utils -
      static uint32_t htonf(float hostfloat);
      static float ntohf(uint32_t netfloat);
      // ----- Sleep --------
      static void sleepMs(int ms);
};

#endif //UTILITIES_H
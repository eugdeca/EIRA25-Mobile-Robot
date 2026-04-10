/*Here there are Utilities to let the process sleep, to convert from degrees to rads (to use 
sin cos functions), to handle queues and to handle floats between network and host.*/
#ifndef UTILITIES_H
#define UTILITIES_H
#include <iostream>
#include <time.h>
#include <arpa/inet.h>
#include <cstdlib>
using namespace std;
typedef char T;

class Utilities{      
    public:
      static const float PI;
      static const int DIM = 10;
      // ----- Sleep --------
      static void sleepMs(int ms);
      // ----- Deg2Rad ------
      static float degToRad(float deg);
      // ----- Queue --------
      struct queue{
        int front, back;
        T queue[DIM];
      };
      static void inic(queue& cc);
      static bool empty(const queue& cc);
      static bool full(const queue& cc);
      static bool insqueue(queue& cc, T s);
      static bool esqueue(queue& cc, T& s);
      static void print(const queue& cc);
      // ----- Socket Utils -
      static uint32_t htonf(float hostfloat);
      static float ntohf(uint32_t netfloat);
};

#endif //UTILITIES_H
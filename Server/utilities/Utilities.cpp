/*Here there are Utilities to let the process sleep, to convert from degrees to rads (to use 
sin cos functions), to handle queues and to handle floats between network and host.*/
#include "Utilities.h"

const float Utilities::PI = 3.1415927f;   // Approximation, there will be small errors

//--------- Sleep ------------------------
void Utilities::sleepMs(int ms){
    struct timespec ts;
    ts.tv_sec= ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

//--------- Deg to rad -------------------
float Utilities::degToRad(float deg){
    float rad;
    rad = deg*PI/180;
    return rad;
}

//--------- Queue primitives -------------
void Utilities::inic(queue& cc){                         // Queue Initialization
    cc.front = cc.back = 0;
}
bool Utilities::empty(const queue& cc){                  // Empty queue?
    if (cc.front == cc.back) return true;
    return false;
}
bool Utilities::full(const queue& cc){                   // Full queue?
    if (cc.front == (cc.back + 1)%DIM) return true;
    return false;
}
bool Utilities::insqueue(queue& cc, T s){                // Insert element
    if (full(cc)) return false;
    cc.queue[cc.back] = s;
    cc.back = (cc.back+1)%DIM;
    return true;
}
bool Utilities::esqueue(queue& cc, T& s){                // Extract element
    if (empty(cc)) return false;
    s = cc.queue[cc.front];
    cc.front = (cc.front + 1)%DIM;
    return true;
}
void Utilities::print(const queue& cc){                  // Print queue
    for (int i = cc.front; i%DIM != cc.back; i++)
        cout << cc.queue[i%DIM] << endl;
}

//--------- Float manipulation -------------
uint32_t Utilities::htonf(float hostfloat) {
  union {
    uint32_t i;
    float f;
  } tmp;
  tmp.f = hostfloat;
  return htonl(tmp.i);
}

float Utilities::ntohf(uint32_t netfloat) {
  union {
    uint32_t i;
    float f;
  } tmp;
  tmp.i = ntohl(netfloat);
  return tmp.f;
}
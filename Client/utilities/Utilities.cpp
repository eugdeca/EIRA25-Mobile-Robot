// Utilities needed, client side, to let the program sleep and to manipulate floats
// between Network and Host.

#include "Utilities.h"

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

//--------- Sleep ------------------------
void Utilities::sleepMs(int ms){
    struct timespec ts;
    ts.tv_sec= ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
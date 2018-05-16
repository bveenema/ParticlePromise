#pragma once

// #include <iostream>
// #include <string.h>


/* ParticlePromise library by Ben Veenema
 */


#include "Particle.h"
#include <functional>

class P_Promise{
public:
  P_Promise(uint8_t charLen = 20){
    responseTopic = new char[charLen];
  }
  bool inUse;
  char* responseTopic;
  uint32_t timeoutTime;
  std::function<void(const char*, const char*)> successFunc;
  std::function<void(const char*, const char*)> errorFunc;
  std::function<void(void)> timeoutFunc;
  std::function<void(void)> finalFunc;
};


class ParticlePromise{
public:
   ParticlePromise(const uint8_t _containerSize = 5, const uint8_t _maxTopicLength = 20){
     PromiseContainer.reserve(_containerSize);
     for(int i=0; i<_containerSize; i++){
       PromiseContainer[i] = new P_Promise(_maxTopicLength);
       PromiseContainer[i].inUse = false;
     }
   }

private:
  std::vector<P_Promise> PromiseContainer;
};

#pragma once

#include <iostream>
#include <string.h>
#include <vector>


/* ParticlePromise library by Ben Veenema
 */


// #include "Particle.h"
#include <functional>
#include <algorithm>
// #include <min>
// #include <max>

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
   ParticlePromise(uint8_t _containerSize = 5, uint8_t _maxTopicLength = 20){
     if(_maxTopicLength < 4) _maxTopicLength = 4;
     PromiseContainer.reserve(_containerSize);
     for(int i=0; i<_containerSize; i++){
       using namespace std::placeholders;
       PromiseContainer[i] = P_Promise(_maxTopicLength);
       PromiseContainer[i].inUse = false;
       strcpy(PromiseContainer[i].responseTopic,"null");
      // PromiseContainer[i].successFunc = std::bind(defaultFuncA, this, _1, _2);
      // PromiseContainer[i].errorFunc = std::bind(defaultFuncA, this, _1, _2);
      // PromiseContainer[i].timeoutFunc = std::bind(defaultFuncB, this);
      // PromiseContainer[i].finalFunc = std::bind(defaultFuncB, this);
     }
     maxTopicLength = _maxTopicLength;
     containerSize = _containerSize;
   }

private:
  unsigned int maxTopicLength;
  unsigned int containerSize;
  std::vector<P_Promise> PromiseContainer;

  template <typename T>
  T clip(const T& n, const T& lower, const T& upper){
    return std::max(lower, std::min(n, upper));
  }

  static void defaultFuncA(const char* doesnt, const char* matter){
    //Serial.printlnf("Function not assigned!");
    std::cout << "Hello World!\n";
  }

  static void defaultFuncB(void){
    //Serial.printlnf("Function not assigned!");
    std::cout << "Hello World!\n";
  }
};

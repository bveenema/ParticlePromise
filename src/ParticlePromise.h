#pragma once

#include <iostream>
#include <string.h>


/* ParticlePromise library by Ben Veenema
 */


#include "Particle.h"
#include <functional>
#include <vector>

class P_Promise{
public:
  P_Promise(unsigned int charLen){
    responseTopic = new char[charLen+1];
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
  ParticlePromise(int _containerSize = 5, int _maxTopicLength = 20);

  ParticlePromise& create(void (*sendWebhookFunc)(void), const char* responseTopic, uint32_t timeout = 0);

  ParticlePromise& then(void (*successFunc)(const char*, const char*));
  ParticlePromise& then(void (*successFunc)(const char*, const char*), void (*errorFunc)(const char*, const char*));

  ParticlePromise& error(void (*errorFunc)(const char*, const char*));

  ParticlePromise& timeout(void (*timeoutFunc)(void), uint32_t timeout = 0);

  bool finally(void (*finalFunc)(void));

  void printBuffer(){
    for(int i=0; i<containerSize; i++){
      Serial.printlnf("PromiseContainer %u", i);
      Serial.printlnf("\tinUse: %u", PromiseContainer[i].inUse);
      Serial.printlnf("\tresponseTopic: %s", PromiseContainer[i].responseTopic);
      Serial.printlnf("\ttimeoutTime: %u", PromiseContainer[i].timeoutTime);
    }
  }

private:
  unsigned int maxTopicLength;
  unsigned int containerSize;
  std::vector<P_Promise> PromiseContainer;
  unsigned int defaultTimeout = 5000;
  unsigned int lastPromise = -1;

  void responseHandler(const char *event, const char *data);

  int8_t findPromiseByTopic(const char* event);

  static void defaultFuncA(const char* doesnt, const char* matter){
    Serial.printlnf("Function not assigned!");
  }

  static void defaultFuncB(void){
    Serial.printlnf("Function not assigned!");
  }
};

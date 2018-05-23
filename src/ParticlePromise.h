#pragma once

#include <iostream>
#include <string.h>


/* ParticlePromise library by Ben Veenema
 */


#include "Particle.h"
#include <functional>
#include <vector>

class P_Promise{
  friend class ParticlePromise;
public:
  P_Promise(unsigned int charLen){
    responseTopic = new char[charLen+1];
  }
  P_Promise& then(void (*_successFunc)(const char*, const char*)){
    successFunc = _successFunc;
    return *this;
  }
  P_Promise& then(void (*_successFunc)(const char*, const char*), void (*_errorFunc)(const char*, const char*)){
    this->then(_successFunc);
    this->error(_errorFunc);
    return *this;
  }
  P_Promise& error(void (*_errorFunc)(const char*, const char*)){
    errorFunc = _errorFunc;
    return *this;
  }
  P_Promise& timeout(void (*_timeoutFunc)(void), uint32_t timeout = 0){
    if(timeout != 0){
      timeoutTime = millis() + timeout;
    }
    timeoutFunc = _timeoutFunc;
    return *this;
  }
  void finally(void (*_finalFunc)(void)){
    finalFunc = _finalFunc;
  }

  bool valid;

private:
  bool inUse;
  char* responseTopic;
  unsigned int timeoutTime;
  std::function<void(const char*, const char*)> successFunc;
  std::function<void(const char*, const char*)> errorFunc;
  std::function<void(void)> timeoutFunc;
  std::function<void(void)> finalFunc;
};


class ParticlePromise{
public:
  /**
   * Constructor: Initializs the PromiseContainer. Optionally set containerSize and maxTopicLength
   */
  ParticlePromise(int _containerSize = 5, int _maxTopicLength = 20);

  /**
   * setTimeout: Ovverride the default timeout time (5 seconds). Unit: milliseconds
   */
  void setTimeout(uint32_t newTimeout);

  /**
   * enable: enable/re-enable the Particle.subscribe call
   */
  void enable(void);

  /**
   * create: Start a new promise by supplying a function to send the webhook and
   *          a response topic.  Optionally supply a timeout.  If no timeout is
   *          supplied, the default will be used.  Returns a reference to the
   *          promise object
   */
  P_Promise& create(void (*sendWebhookFunc)(void), const char* responseTopic, unsigned int timeout = 0);

  void printBuffer(){
    for(int i=0; i<containerSize+1; i++){
      Serial.printlnf("PromiseContainer %u", i);
      Serial.printlnf("\tvalid: %u", PromiseContainer[i].valid);
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

  void responseHandler(const char *event, const char *data);

  int findPromiseByTopic(const char* event);

  static void defaultFuncA(const char* doesnt, const char* matter){
  }

  static void defaultFuncB(void){
  }
};

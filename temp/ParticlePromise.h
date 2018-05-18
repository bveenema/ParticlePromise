#pragma once

//#include <iostream>


/* ParticlePromise library by Ben Veenema
 */


#include "Particle.h"
#include <functional>

class ParticlePromise{
public:
  /**
   * Constructor: Intializes PromiseContainer and sets up Particle.subscribe()
   */
  ParticlePromise();

  /**
   * setTimeout: Ovverride the default timeout time (5 seconds). Unit: milliseconds
   */
  void setTimeout(uint32_t newTimeout){
    if(newTimeout > 60000) return; // prevent timeouts longer than 60 seconds
    defaultTimeout = newTimeout;
  }

  /**
   * create: check for an open slot in PromiseContainer and create new promise
   */
  ParticlePromise& create(void (*sendWebhookFunc)(void), const char* responseTopic, uint32_t timeout = 0);


  ParticlePromise& then(void (*successFunc)(const char*, const char*)){
    if(lastPromise >= 0){
      PromiseContainer[lastPromise].successFunc = successFunc;
    }
    return *this;
  }

  ParticlePromise& then(void (*successFunc)(const char*, const char*), void (*errorFunc)(const char*, const char*)){
    if(lastPromise >= 0){
      PromiseContainer[lastPromise].successFunc = successFunc;
      PromiseContainer[lastPromise].errorFunc = errorFunc;
    }
    return *this;
  }

  ParticlePromise& error(void (*errorFunc)(const char*, const char*)){
    if(lastPromise >= 0){
      PromiseContainer[lastPromise].successFunc = errorFunc;
    }
    return *this;
  }

  ParticlePromise& timeout(void (*timeoutFunc)(void), uint32_t timeout = 0){
    if(lastPromise >= 0){
      if(timeout != 0){
        PromiseContainer[lastPromise].timeoutTime = millis() + timeout;
      }
      PromiseContainer[lastPromise].timeoutFunc = timeoutFunc;
    }
    return *this;
  }

  bool finally(void (*finalFunc)(void)){
    if(lastPromise >= 0){
      PromiseContainer[lastPromise].finalFunc = finalFunc;
      return true;
    }
    return false;
  }

  /**
   * process: checks for expired promises
   */
  void process();

private:
  uint8_t containerSize = 0;
  uint8_t maxTopicLength = 0;
  uint32_t defaultTimeout = 5000; // default timeout time

  struct Promise{
    bool inUse;
    char responseTopic[MAXTOPICLENGTH];
    uint32_t timeoutTime;
    std::function<void(const char*, const char*)> successFunc;
    std::function<void(const char*, const char*)> errorFunc;
    std::function<void(void)> timeoutFunc;
    std::function<void(void)> finalFunc;
  };

  Promise PromiseContainer[BUFFERSIZE];

  int8_t lastPromise = -1;

  void responseHandler(const char *event, const char *data) {
    int8_t promiseID = findPromiseByTopic(event);
    if(promiseID >= 0){
      if(strstr(event, "success")){
        PromiseContainer[promiseID].successFunc(event, data);
      } else if(strstr(event, "error")){
        PromiseContainer[promiseID].errorFunc(event, data);
      }
      PromiseContainer[promiseID].finalFunc();
      PromiseContainer[promiseID].inUse = false;
    }
  }

  int8_t findPromiseByTopic(const char* event){
    uint8_t containerPosition = 0;
    for(containerPosition; containerPosition<containerSize; containerPosition++){
      if(strstr(event, PromiseContainer[containerPosition].responseTopic)) return containerPosition;
    }
    return -1;
  }

  static void defaultFuncA(const char* doesnt, const char* matter){
    Serial.printlnf("Function not assigned!");
    //std::cout << "Hello World!\n";
  }

  static void defaultFuncB(void){
    Serial.printlnf("Function not assigned!");
    //std::cout << "Hello World!\n";
  }
};


// ParticlePromise<{bufferSize}, {maxTopicLength}> promise;  // Creates a buffer for function pointers and response topics. And calls Particle.subscribe() with filter "promise-response"
//
// void promise.create(*sendWebhookFunction, "myResponseTopic", *successFunction, {*errorFunction}, {*timeoutFunction}, {int timeout}, {*finalFunction});
// auto index = promise.create(*sendWebhookFunction, "myResonseTopic", {int timeout});
//                 index.then(*successFunction);
//                 index.catch(*errorFunction);
//                 index.timeout(*timeoutFunction, {int timeout});
//                 index.finally(*finalFunction);
//
// promise.reEnable(); // Re-creates Particle.subscribe() in case of a previous Particle.unsubscribe() call

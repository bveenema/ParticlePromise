#pragma once

/* ParticlePromise library by Ben Veenema
 */

// This will load the definition for common Particle variable types
#include "Particle.h"

// This is your main class that users will import into their application
template<size_t bufferSize = 5, size_t maxTopicLenght = 20>
class ParticlePromise{
public:
  /**
   * Constructor
   */
  ParticlePromise();

  /**
   * Example method
   */
  void setTimeout(uint32_t newTimeout){
    if(newTimeout > 60000) return; // prevent timeouts longer than 60 seconds
    defaultTimeout = newTimeout;
  }

  void create(void (*function)(void), const char* responseTopic, uint32_t timeout = defaultTimeout){

  }

  /**
   * Example method
   */
  void process();

private:
  uint32_t defaultTimeout = 5000; // default timeout time

  struct PromiseContainer{
    char responseTopic[maxTopicLength];
    std::function<void(void)> sendWebhookFunc;
    std::function<void(const char*, const char*)> successFunc;
    std::function<void(const char*, const char*)> errorFunc;
    std::function<void(void)> timeoutFunc;
    std::function<void(void)> finalFunc;
  };

  PromiseContainer Promises[bufferSize];

  /**
   * Example private method
   */
  void doit();
};


ParticlePromise<{bufferSize}, {maxTopicLength}> promise;  // Creates a buffer for function pointers and response topics. And calls Particle.subscribe() with filter "promise-response"

void promise.create(*sendWebhookFunction, "myResponseTopic", *successFunction, {*errorFunction}, {*timeoutFunction}, {int timeout}, {*finalFunction});
auto index = promise.create(*sendWebhookFunction, "myResonseTopic", {int timeout});
                index.then(*successFunction);
                index.catch(*errorFunction);
                index.timeout(*timeoutFunction, {int timeout});
                index.finally(*finalFunction);

promise.reEnable(); // Re-creates Particle.subscribe() in case of a previous Particle.unsubscribe() call

#pragma once

// #include <iostream>
// #include <string.h>


/* ParticlePromise library by Ben Veenema
 *  Creates Javascript-like promises for webhook/API callbacks on the Particle
 *  ecosystem.
 */


#include "Particle.h"
#include "Prom.h"
#include <functional>
#include <vector>

class ParticlePromise{
public:
  /**
   * Constructor: Initializes the PromiseContainer. Optionally set containerSize and maxTopicLength
   */
  ParticlePromise(int _containerSize = 5, int _maxTopicLength = 20);

  /**
   * setTimeout: Ovverride the default timeout time (5 seconds). Unit: milliseconds
   */
  void setTimeout(uint32_t newTimeout);

  /**
   * enable: enable/re-enable the Particle.subscribe call, must be called prior to .create()
   */
  bool enable(void);

  /**
   * cancel: Stop the specified Promise from resolving, must provide the exact response topic
   */
  void cancel(const char* responseTopic);

  /**
   * create: Start a new promise by supplying a function to send the webhook and
   *          a response topic.  Optionally supply a timeout.  If no timeout is
   *          supplied, the default will be used.  Returns a reference to the
   *          promise object
   */
  Prom& create(std::function<void(void)> sendWebhookFunc, const char* responseTopic, unsigned int timeout = 0){
    // return invalid promise if not subscribed
    if(!isSubscribed) return PromiseContainer[containerSize];

    // return invalid promise if duplicate
    if(findPromiseByTopic(responseTopic) >= 0) return PromiseContainer[containerSize];

    unsigned int containerPosition = this->findEmptySlot();

    // return invalid promise (ie PromiseContainer[].valid == false)
    if(containerPosition >= containerSize) return PromiseContainer[containerSize];

    this->resetSlot(containerPosition, responseTopic, true);
    sendWebhookFunc();
    this->setTimeoutTime(containerPosition, timeout);
    return PromiseContainer[containerPosition];
  }

  template <typename T>
  Prom& create(void (T::*sendWebhookFunc)(void), T *instance, const char* responseTopic, unsigned int timeout = 0){
    // return invalid promise if not subscribed
    if(!isSubscribed) return PromiseContainer[containerSize];

    unsigned int containerPosition = this->findEmptySlot();

    // return invalid promise (ie PromiseContainer[].valid == false)
    if(containerPosition >= containerSize) return PromiseContainer[containerSize];

    this->resetSlot(containerPosition, responseTopic, true);
    auto fp = std::bind(sendWebhookFunc,instance);
    fp();
    this->setTimeoutTime(containerPosition, timeout);
    return PromiseContainer[containerPosition];
  }

  void process(void);

  // void printBuffer(){
  //   for(int i=0; i<containerSize+1; i++){
  //     Serial.printlnf("PromiseContainer %u", i);
  //     Serial.printlnf("\tvalid: %u", PromiseContainer[i].valid);
  //     Serial.printlnf("\tpending: %u", PromiseContainer[i].pending);
  //     Serial.printlnf("\tresponseTopic: %s", PromiseContainer[i].responseTopic);
  //     Serial.printlnf("\ttimeoutTime: %u", PromiseContainer[i].timeoutTime);
  //   }
  // }
  //
  // void test(const char* event, const char* data = "null", bool isError = false){
  //   char eventBuffer[1024];
  //   if(isError) strcpy(eventBuffer, "promise-response/error/");
  //   else strcpy(eventBuffer, "promise-response/success/");
  //   strcat(eventBuffer, event);
  //
  //   this->responseHandler(eventBuffer, data);
  // }

private:
  bool isSubscribed = false;
  unsigned int maxTopicLength;
  unsigned int containerSize;
  std::vector<Prom> PromiseContainer;
  unsigned int defaultTimeout = 5000;

  void responseHandler(const char *event, const char *data);
  int findPromiseByTopic(const char*);
  int findPromiseByEvent(const char*);
  unsigned int findEmptySlot(void);
  void setTimeoutTime(unsigned int containerPosition, unsigned int timeout);
  void resetSlot(unsigned int containerPosition, const char* responseTopic = "null", bool pending = false);

  static void defaultFuncA(const char* doesnt, const char* matter){
  }

  static void defaultFuncB(void){
  }
};

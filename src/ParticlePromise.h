#pragma once

// #include <iostream>
// #include <string.h>


/* ParticlePromise library by Ben Veenema
 */


#include "Particle.h"
#include "Prom.h"
#include <functional>
#include <vector>

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
  Prom& create(void (*sendWebhookFunc)(void), const char* responseTopic, unsigned int timeout = 0){
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
  unsigned int maxTopicLength;
  unsigned int containerSize;
  std::vector<Prom> PromiseContainer;
  unsigned int defaultTimeout = 5000;

  void responseHandler(const char *event, const char *data);
  int findPromiseByTopic(const char* event);
  unsigned int findEmptySlot(void);
  void setTimeoutTime(unsigned int containerPosition, unsigned int timeout);
  void resetSlot(unsigned int containerPosition, const char* responseTopic = "null", bool pending = false);

  static void defaultFuncA(const char* doesnt, const char* matter){
  }

  static void defaultFuncB(void){
  }
};

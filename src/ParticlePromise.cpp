#include "ParticlePromise.h"

ParticlePromise<BUFFERSIZE, MAXTOPICLENGTH>::ParticlePromise(){
  // initialize PromiseContainer
  {
  using namespace std::placeholders;
  Promise defaultPromise = {
    false,
    "null",
    0,
    std::bind(defaultFuncA, this, _1, _2),
    std::bind(defaultFuncA, this, _1, _2),
    std::bind(defaultFuncB, this),
    std::bind(defaultFuncB, this)
  };
  Promise defaultPromise;
  defaultPromise.successFunc = std::bind(defaultFuncA, this, _1, _2);
}
  for(uint8_t i=0; i<containerSize; i++){
    PromiseContainer[i] = defaultPromise;
  }

  // Setup Particle.subscribe() handler
  Particle.subscribe(System.deviceID() + "/promise-response", &ParticlePromise::responseHandler, this, MY_DEVICES);
}

ParticlePromise& ParticlePromise<BUFFERSIZE, MAXTOPICLENGTH>::create(void (*sendWebhookFunc)(void), const char* responseTopic, uint32_t timeout = 0){
  // find an empty position in the container
  uint8_t containerPosition = 0;
  for(containerPosition; containerPosition<containerSize+1; containerPosition++){
    if(PromiseContainer[containerPosition].inUse == false) break;
  }
  if(containerPosition >= containerSize){
    lastPromise = -1;
    return *this;
  }

  // Update PromiseContainer with new promise
  PromiseContainer[containerPosition].inUse = true;
  strcpy(PromiseContainer[containerPosition].responseTopic, responseTopic);
  {
    using namespace std::placeholders;
    PromiseContainer[containerPosition].successFunc = std::bind(defaultFuncA, this, _1, _2);
    PromiseContainer[containerPosition].errorFunc = std::bind(defaultFuncA, this, _1, _2);
    PromiseContainer[containerPosition].timeoutFunc = std::bind(defaultFuncB, this);
    PromiseContainer[containerPosition].finalFunc = std::bind(defaultFuncB, this);
  }

  // call sendWebhookFunc
  sendWebhookFunc();

  // calculate the timeout Time
  if(timeout == 0) timeout = defaultTimeout;
  PromiseContainer[containerPosition].timeoutTime = millis() + timeout;

  lastPromise = containerPosition;
  return *this;
}

void ParticlePromise<BUFFERSIZE, MAXTOPICLENGTH>::process(){
  // check for timeouts
  uint32_t timeNow = millis();
  for(uint8_t i=0; i<containerSize; i++){
    if(PromiseContainer[i].timeoutTime >= timeNow){
      PromiseContainer[i].timeoutFunc();
      PromiseContainer[i].finalFunc();
      PromiseContainer[i].inUse = false;
    }
  }
}

#include "ParticlePromise.h"

ParticlePromise::ParticlePromise(int _containerSize, int _maxTopicLength){
  // initialize PromiseContainer
  containerSize = constrain(_containerSize, 1, 50);
  maxTopicLength = constrain(_maxTopicLength, 4, 63);
  PromiseContainer.reserve(containerSize);
  for(int i=0; i<containerSize; i++){
    PromiseContainer[i] = P_Promise(maxTopicLength);
    PromiseContainer[i].inUse = false;
    strcpy(PromiseContainer[i].responseTopic,"null");
    PromiseContainer[i].successFunc = this->defaultFuncA;
    PromiseContainer[i].errorFunc = this->defaultFuncA;
    PromiseContainer[i].timeoutFunc = this->defaultFuncB;
    PromiseContainer[i].finalFunc = this->defaultFuncB;
  }
  maxTopicLength = _maxTopicLength;
  containerSize = _containerSize;

  // Setup Particle.subscribe() handler
  Particle.subscribe(System.deviceID() + "/promise-response", &ParticlePromise::responseHandler, this, MY_DEVICES);
}

ParticlePromise& ParticlePromise::create(void (*sendWebhookFunc)(void), const char* responseTopic, uint32_t timeout){
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
    PromiseContainer[containerPosition].successFunc = this->defaultFuncA;
    PromiseContainer[containerPosition].errorFunc = this->defaultFuncA;
    PromiseContainer[containerPosition].timeoutFunc = this->defaultFuncB;
    PromiseContainer[containerPosition].finalFunc = this->defaultFuncB;
  }

  // call sendWebhookFunc
  sendWebhookFunc();

  // calculate the timeout Time
  if(timeout == 0) timeout = defaultTimeout;
  PromiseContainer[containerPosition].timeoutTime = millis() + timeout;

  lastPromise = containerPosition;
  return *this;
}

ParticlePromise& ParticlePromise::then(void (*successFunc)(const char*, const char*)){
  if(lastPromise >= 0){
    PromiseContainer[lastPromise].successFunc = successFunc;
  }
  return *this;
}

ParticlePromise& ParticlePromise::then(void (*successFunc)(const char*, const char*), void (*errorFunc)(const char*, const char*)){
  if(lastPromise >= 0){
    PromiseContainer[lastPromise].successFunc = successFunc;
    PromiseContainer[lastPromise].errorFunc = errorFunc;
  }
  return *this;
}

ParticlePromise& ParticlePromise::error(void (*errorFunc)(const char*, const char*)){
  if(lastPromise >= 0){
    PromiseContainer[lastPromise].successFunc = errorFunc;
  }
  return *this;
}

ParticlePromise& ParticlePromise::timeout(void (*timeoutFunc)(void), uint32_t timeout){
  if(lastPromise >= 0){
    if(timeout != 0){
      PromiseContainer[lastPromise].timeoutTime = millis() + timeout;
    }
    PromiseContainer[lastPromise].timeoutFunc = timeoutFunc;
  }
  return *this;
}

bool ParticlePromise::finally(void (*finalFunc)(void)){
  if(lastPromise >= 0){
    PromiseContainer[lastPromise].finalFunc = finalFunc;
    return true;
  }
  return false;
}


void ParticlePromise::responseHandler(const char *event, const char *data) {
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

  int8_t ParticlePromise::findPromiseByTopic(const char* event){
    uint8_t containerPosition = 0;
    for(containerPosition; containerPosition<containerSize; containerPosition++){
      if(strstr(event, PromiseContainer[containerPosition].responseTopic)) return containerPosition;
    }
    return -1;
  }

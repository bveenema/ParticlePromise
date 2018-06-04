#include "ParticlePromise.h"

ParticlePromise::ParticlePromise(int _containerSize, int _maxTopicLength){
  // Validate inputs
  containerSize = constrain(_containerSize, 1, 50);
  maxTopicLength = constrain(_maxTopicLength, 4, 63);

  // Initialize PromiseContainer - Create extra container with value: valid = false;
  PromiseContainer.reserve(containerSize+1);
  for(int i=0; i<containerSize+1; i++){
    PromiseContainer[i] = Prom(maxTopicLength);
    PromiseContainer[i].valid = true;
    this->resetSlot(i);
  }
  PromiseContainer[containerSize].valid = false;
}

void ParticlePromise::setTimeout(uint32_t newTimeout){
  if(newTimeout > 60000) return; // prevent timeouts longer than 60 seconds
  defaultTimeout = newTimeout;
}

void ParticlePromise::setTimeoutTime(unsigned int containerPosition, unsigned int timeout){
  if(timeout == 0) timeout = defaultTimeout;
  PromiseContainer[containerPosition].timeoutTime = millis() + timeout;
}

void ParticlePromise::resetSlot(unsigned int containerPosition, const char* responseTopic, bool pending){
  PromiseContainer[containerPosition].pending = pending;
  strcpy(PromiseContainer[containerPosition].responseTopic, responseTopic);
  PromiseContainer[containerPosition].successFunc = this->defaultFuncA;
  PromiseContainer[containerPosition].errorFunc = this->defaultFuncA;
  PromiseContainer[containerPosition].timeoutFunc = this->defaultFuncB;
  PromiseContainer[containerPosition].finalFunc = this->defaultFuncB;
}

unsigned int ParticlePromise::findEmptySlot(void){
  unsigned int containerPosition = 0;
  for(containerPosition; containerPosition<containerSize+1; containerPosition++){
    if(PromiseContainer[containerPosition].pending == false) break;
  }
  return containerPosition;
}

void ParticlePromise::enable(void){
  Particle.subscribe(System.deviceID() + "/promise-response", &ParticlePromise::responseHandler, this, MY_DEVICES);
}

void ParticlePromise::process(void){
  unsigned int currentTime = millis();
  for(int i=0; i<containerSize; i++){
    if(PromiseContainer[i].pending && PromiseContainer[i].timeoutTime < currentTime){
      PromiseContainer[i].timeoutFunc();
      PromiseContainer[i].finalFunc();
      PromiseContainer[i].pending = false;
    }
  }
}

void ParticlePromise::responseHandler(const char *event, const char *data) {
  int promiseID = findPromiseByTopic(event);
  if(promiseID >= 0 && PromiseContainer[promiseID].pending){
    if(strstr(event, "success")){
      PromiseContainer[promiseID].successFunc(event, data);
    } else if(strstr(event, "error")){
      PromiseContainer[promiseID].errorFunc(event, data);
    }
    PromiseContainer[promiseID].finalFunc();
    PromiseContainer[promiseID].pending = false;
  }
}

int ParticlePromise::findPromiseByTopic(const char* event){
  int containerPosition = 0;
  for(containerPosition; containerPosition<containerSize; containerPosition++){
    if(strstr(event, PromiseContainer[containerPosition].responseTopic)) return containerPosition;
  }
  return -1;
}

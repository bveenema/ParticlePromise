#include "ParticlePromise.h"

ParticlePromise::ParticlePromise(int _containerSize, int _maxTopicLength){
  // Validate inputs
  containerSize = constrain(_containerSize, 1, 50);
  maxTopicLength = constrain(_maxTopicLength, 4, 63);

  // Initialize PromiseContainer - Create extra container with value: valid = false;
  PromiseContainer.reserve(containerSize+1);
  for(int i=0; i<containerSize+1; i++){
    PromiseContainer[i] = P_Promise(maxTopicLength);
    PromiseContainer[i].valid = true;
    PromiseContainer[i].inUse = false;
    strcpy(PromiseContainer[i].responseTopic,"null");
    PromiseContainer[i].successFunc = this->defaultFuncA;
    PromiseContainer[i].errorFunc = this->defaultFuncA;
    PromiseContainer[i].timeoutFunc = this->defaultFuncB;
    PromiseContainer[i].finalFunc = this->defaultFuncB;
  }
  PromiseContainer[containerSize].valid = false;

  // Setup Particle.subscribe() handler
  this->enable();
}

/**
 * setTimeout: Ovverride the default timeout time (5 seconds). Unit: milliseconds
 */
void ParticlePromise::setTimeout(uint32_t newTimeout){
  if(newTimeout > 60000) return; // prevent timeouts longer than 60 seconds
  defaultTimeout = newTimeout;
}

P_Promise& ParticlePromise::create(void (*sendWebhookFunc)(void), const char* responseTopic, unsigned int timeout){
    // find an empty position in the container
  unsigned int containerPosition = 0;
  for(containerPosition; containerPosition<containerSize+1; containerPosition++){
    if(PromiseContainer[containerPosition].inUse == false) break;
  }
  if(containerPosition >= containerSize){
    // return invalid promise (ie PromiseContainer[].valid == false)
    return PromiseContainer[containerSize];
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

  return PromiseContainer[containerPosition];
}

void ParticlePromise::enable(void){
  Particle.subscribe(System.deviceID() + "/promise-response", &ParticlePromise::responseHandler, this, MY_DEVICES);
}

void ParticlePromise::process(void){
  unsigned int currentTime = millis();
  for(int i=0; i<containerSize; i++){
    if(PromiseContainer[i].inUse && PromiseContainer[i].timeoutTime < currentTime){
      PromiseContainer[i].timeoutFunc();
      PromiseContainer[i].finalFunc();
      PromiseContainer[i].inUse = false;
    }
  }
}

void ParticlePromise::responseHandler(const char *event, const char *data) {
    int promiseID = findPromiseByTopic(event);
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

int ParticlePromise::findPromiseByTopic(const char* event){
  int containerPosition = 0;
  for(containerPosition; containerPosition<containerSize; containerPosition++){
    if(strstr(event, PromiseContainer[containerPosition].responseTopic)) return containerPosition;
  }
  return -1;
}

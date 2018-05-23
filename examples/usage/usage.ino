// Example usage for ParticlePromise library by Ben Veenema.

#include "ParticlePromise.h"

// Initialize objects from the lib
ParticlePromise promise;

void webhookFunc(){}
void successFunc(const char* doesnt, const char* matter){}
void errorFunc(const char* doesnt, const char* matter){}
void timeoutFunc(){}
void finalFunc(){}

void setup() {
  Serial.begin(9600);
  Serial.printlnf("Free Memory: %u", System.freeMemory());

  promise.create(webhookFunc, "Testhook")
    .then(successFunc);

  auto& p1 = promise.create(webhookFunc, "p1-response");
  if(p1.valid) p1.then(successFunc).timeout(timeoutFunc);

  promise.printBuffer();
}

void loop() {

}

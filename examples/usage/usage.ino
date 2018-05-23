// Example usage for ParticlePromise library by Ben Veenema.

#include "ParticlePromise.h"

// Initialize objects from the lib
ParticlePromise promise;

void webhookFunc();
void successFunc(const char*, const char*);
void errorFunc(const char*, const char*);
void timeoutFunc();
void finalFunc();

void setup() {
  Serial.begin(9600);
  Serial.printlnf("Free Memory: %u", System.freeMemory());
}

void loop() {
  promise.create(webhookFunc, "Testhook")
    .then(successFunc);

  auto& p1 = promise.create(webhookFunc, "p3-response");
  if(p1.valid) p3.then(successFunc).timeout(timeoutFunc);
}

// Basic usage for ParticlePromise library by Ben Veenema.

#include "ParticlePromise.h"

// Initialize ParticlePromise: Creates a PromiseContainer with default size of 5
//    positions (max number of promises) and 20 character maximum topic
ParticlePromise promise;

void setup() {
  Serial.begin(9600);

  // Setup the Particle.subscribe() hook.  Must be called before creating a promise
  promise.enable();

  // Most basic promise
  promise.create(sendRequest, "test").then(successFunc);
}

void loop() {
  // Must call .process() every loop - checks for timeouts
  promise.process();
}






void sendRequest(){
  Serial.printlnf("Sending Request");
  Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
}
void successFunc(const char* event, const char* data){
  Serial.printlnf("Success Response: %s", data);
}

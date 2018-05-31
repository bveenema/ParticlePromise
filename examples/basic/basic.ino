// Basic usage for ParticlePromise library

#include "ParticlePromise.h"

// Initialize ParticlePromise: Creates a PromiseContainer with default size of 5
//    positions (max number of promises) and 20 character maximum topic
ParticlePromise promise;

void setup() {
  Serial.begin(9600);

  // Setup the Particle.subscribe() hook.  Must be called before creating a promise
  promise.enable();

  // Most basic promise
  promise.create(sendRequest, "test") // creates a new promise with a responseTopic name of "test"
          .then(successFunc);         // tells the promise to call successFunc() after the promise responds successfully
}

void loop() {
  // Check for promises that have timed-out.  Should be called every loop.
  promise.process();
}










// the API calling function (sendRequest() in this example) must return void and
//      take no arguments.  It should call Particle.publish() targeting the webhook
//      that makes the API request
void sendRequest(){
  Serial.printlnf("Sending Request");
  Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
}

// the success function (successFunc() in this example) must return void and accept
//      const char* event and const char* data arguments.  This is the function that
//      will be called on successful response from the API request.
void successFunc(const char* event, const char* data){
  Serial.printlnf("Success Response: %s", data);
}

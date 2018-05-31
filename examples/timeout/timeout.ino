// Examples using custom timeouts for ParticlePromise library

#include "ParticlePromise.h"

// Initialize ParticlePromise: Creates a PromiseContainer with default size of 5
//    positions (max number of promises) and 20 character maximum topic
ParticlePromise promise;

void setup() {
  Serial.begin(9600);

  // Setup the Particle.subscribe() hook.  Must be called before creating a promise
  promise.enable();

  // Set a new default Timeout time.  All new promises will be given this timeout
  //    time unless otherwise specified (Note: Unit = milliseconds, Max = 60 seconds)
  promise.setTimeout(10000); // sets default timeouts to 10 seconds

  // Set a timeout for an individual promise in the .create() call
  //    Note: .then() is not strictly necessary
  promise.create(sendRequest, "test", 3500) // sets timeout of 3.5 seconds
          .timeout(timeoutFunc);

  waitForPromiseToTimeout(3500); // safely delay the program while the promise times out

  // Alternatively set an individual promise timeout in the .timeout() call
  //    Note: if timeout is set in .create() AND .timeout(), .timeout() call will
  //          take precedence
  promise.create(sendRequest, "test")
          .timeout(timeoutFunc, 2000); // sets timeout of 2 seconds
}

void loop() {
  // Check for promises that have timed-out.  Should be called every loop.
  promise.process();
}






// The Timeout function must return void and accept no arguments
void timeoutFunc(){
  Serial.printlnf("API Request Timed Out!");
}


void sendRequest(){
  Serial.printlnf("Sending Request");
  Particle.publish("ThisWillTimeout", NULL, 60, PRIVATE);
}
void successFunc(const char* event, const char* data){
  Serial.printlnf("Success Response: %s", data);
}

void waitForPromiseToTimeout(int time){
  unsigned int startTime = millis();
  while(millis()-startTime < time+20){
    Particle.process();
    promise.process();
  }
}

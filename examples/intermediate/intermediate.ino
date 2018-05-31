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
          .then(successFunc);         // tells the promise to call successFunc() when the promise responds successfully

  waitForPromiseToTimeout(5000); // Allows time for the promise to timeout

  // Promise with all avilable options
  promise.create(sendRequest, "test")
          .then(successFunc)
          .error(errorFunc)     // tells the promise to call errorFunc() when the promise responds with an error
          .timeout(timeoutFunc) // tells the promise to call timeoutFunc() when there is no response from the API
          .finally(finalFunc);  // tells the promise to call finalFunc() after any response (success, error or timeout)
                                //    the corresponding function (successFunc, errorFunc, timeoutFunc) will be called first

  waitForPromiseToTimeout(5000); // Allows time for the promise to timeout

  // Promise with combined success and error function setting
  promise.create(sendRequest, "test")
          .then(successFunc,errorFunc);  // tells the promise to call successFunc() on success or errorFunc() on error
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

// the error function (errorFunc() in this example) must return void and accept
//      const char* event and const char* data arguments.  This is the function that
//      will be called on error response from the API request.
void errorFunc(const char* event, const char* data){
  Serial.printlnf("Error Response: %s", data);
}

// the timeout function (timeoutFunc() in this example) must return void and accept
//      no arguments.  This is the function that will be called on if there is no
//      response from the API before the timeout
void timeoutFunc(){
  Serial.printlnf("API Request Timed Out!");
}

// the final function (finalFunc() in this example) must return void and accept
//      no arguments.  This is the function that will be called  after any
//      resolution of the promise (success, error or timeout).  The corresponding
//      success, error or timeout function will be called first (if it exists)
void finalFunc(){
  Serial.printlnf("Promise Resolved");
}



void waitForPromiseToTimeout(int time){
  unsigned int startTime = millis();
  while(millis()-startTime < time+20){
    Particle.process();
    promise.process();
  }
}

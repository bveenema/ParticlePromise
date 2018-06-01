// Class Member Function usage for ParticlePromise library

#include "ParticlePromise.h"

// Initialize ParticlePromise: Creates a PromiseContainer with default size of 5
//    positions (max number of promises) and 20 character maximum topic
ParticlePromise promise;

// **** OUTSIDE CLASSES (Part A) ****
//    Classes that are used in promises but don't have promises created from
//    within (ie. Outside Classes) do not require any special knowledge of
//    ParticlePromise.  The only thing to make sure is that the member functions
//    of the class that will be passed to Particle promise are the correct
//    structure (ie. return void and accept const char* or no arguments)
//    EXAMPLE:
class OutsideClass{
public: // make sure the functions are accessible
  void sendWebhookFunc(void){
    Serial.printlnf("Send Webhook - Outside Class");
    Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
  }
  void successFunc(const char* event, const char* data){
      Serial.printlnf("OutsideClass Success Response: %s", data);
  }
};

OutsideClass Outside;

// **** INSIDE CLASSES (Part A) ****
//    Classes that create promises from within require special setup and
//    knowledge of ParticlePromise.  .create(), .then(), etc must be called with
//    an additional parameter "this" that compiler uses to bind the function
//    from a specific object (instantiation) of the class to the Promise.
//    Normally, individual objects do not "contain" their own functions so that
//    multiple copies are not created (and take up more memory). Unfortunately,
//    in C++, the syntax is messy.
//
//    It is important that the Promise is not created in the constructor. Typically
//    a "begin" or "init" function is used (similar to Serial.begin()) to call a
//    promise, but it could be done at any time.
//
//    NOTE: when declaring an "InsideClass" in a separate file (.h) you MUST
//    include ParticlePromise using the extern keyword to avoid multiple copies
//    of ParticlePromise EXAMPLE: extern ParticlePromise promise;
class InsideClass{
public:
  void begin(){
    promise.create(&InsideClass::sendWebhookFunc, this, "test")
            .then(&InsideClass::successFunc, this);
  }

private:
  void sendWebhookFunc(){
      Serial.printlnf("Send webhook - Inside Class");
      Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
  }
  void successFunc(const char* event, const char* data){
    Serial.printlnf("InsideClass Success Response: %s", data);
  }
};

InsideClass Inside;

void setup() {
  Serial.begin(9600);

  // Setup the Particle.subscribe() hook.  Must be called before creating a promise
  promise.enable();

  // **** OUTSIDE CLASSES (Part B) ****
  //    Creating Promises with Outside Class member functions is not much different
  //    than creating Promises with standard functions, however, it does require
  //    the use of an additional parameter.
  //
  //    A reference to the individual object must passed to ParticlePromise. Each
  //    function (sendWebhookFunc, successFunc, errorFunc etc) can be passed
  //    functions from different classes but requires the additional parameter
  //    with each call.
  auto& p1 = promise.create(&OutsideClass::sendWebhookFunc, &Outside, "test")
          .then(&OutsideClass::successFunc, &Outside);

  // wait for promise to resolve before running next example
  while(p1.getStatus()){
    Particle.process();
    promise.process();
  }

  // **** INSIDE CLASSES (Part B) ****
  //    Inside Classes are much easier to work with in the main program flow.
  //    Simply call the member function that creates the Promise and you're done
  Inside.begin();
}

void loop() {
  // Check for promises that have timed-out.  Should be called every loop.
  promise.process();
}

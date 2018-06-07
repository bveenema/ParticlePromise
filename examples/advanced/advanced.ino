// Advanced usage for ParticlePromise library - NOTE: this example requires a very
//  specific webhook to function properly.  Please see the README for more info.

#include "ParticlePromise.h"

// **** CHANGING PROMISECONTAINER SIZE ****
//    The default PromiseContainer can handle up to 5 Promises at a time and 20
//    character maximum responseTopic names.  This uses about 728 bytes of heap
//    memory:
//      ParticlePromise Class:  ~80   bytes
//      Dummy Promise Object:   ~108  bytes
//      5x Promise Objects:     ~540  bytes (5x 108)
//
//    Users may require to reduce memory usage or increase the number of Promises
//    that can be handled at one time or increase the maximum responseTopic length
//    EXAMPLE: (NOTE: in practice, multiple ParticlePromise object should not be
//              created)

// ParticlePromise promise;
// ParticlePromise promise(3); // create PromiseContainer that can handle 3 promises at a time
ParticlePromise promise(5,60); // create PromiseContainer that can handle responseTopics of 60 characters

void setup() {
  Serial.begin(9600);

  // Setup the Particle.subscribe() hook.  Must be called before creating a promise
  promise.enable();

  // **** WORKING WITH PROMISE OBJECT ****
  //   In order for the dot chaining methods to work, .create(), .then(), .error()
  //   .timeout() and .finally() return a pointer to a promise object (Prom) that can be
  //   stored.  Caution should be taken when using this pointer as the underlying
  //   promise can change without warning due to resolution of the promise
  //   EXAMPLE:
  auto& p1 = promise.create(sendRequest, "test"); // auto is used instead of the promise class for clarity
  auto& p2 = promise.create(sendRequest, "AnotherTest");
  p2.then(successFunc);
  p1.then(successFunc);

  // **** TOO MANY PROMISES ****
  //    In order to limit the amount of heap memory used by ParticlePromise, a
  //    PromiseContainer is created at construction that contains pointers to a
  //    fixed number of Promise (Prom) objects.  If the user code attempts to create
  //    more than the fixed number of promises before they can resolve, ParticlePromise
  //    returns a dummy Promise object.  This dummy object can be tested for by
  //    calling .isValid().  The dummy object prevents unknowing users from calling
  //    .then(), .error() etc. member functions on a null pointer
  //    EXAMPLE:
  int promiseCount = 2;
  char buffer[8];
  sprintf(buffer, "test%u",promiseCount);
  auto& p3 = promise.create(sendRequest, buffer);
  ++promiseCount;
  while(p3.isValid()){ // check if p3 points to a valid Promise object
    sprintf(buffer, "test%u", promiseCount++);
    p3 = promise.create(sendRequest, buffer); // keep creating promises until
      //print the promise
      Serial.printf("Promise %u is ", promiseCount);
      if(p3.isValid()) Serial.println("valid");
      else Serial.println("invalid");
  }
  p3.then(successFunc); // .then() can still be called without error on the invalid promise object

  waitForPromiseToTimeout(5000); // Resolve all promises

  // **** TESTING FOR RESOLUTION ****
  //    The returned Promise object can be tested for resolution by calling
  //    .getStatus(). This is useful for delaying main program execution or calling
  //    a seperate loop() while the waiting for the Promise to resolve.  It should
  //    not be used in place of .finally()
  auto& p4 = promise.create(sendRequest, "ThisWillTimeout");
  while(p4.getStatus()){
    alternateLoop();
  }
  Serial.println("Promise Resolved");
}

void loop() {
  // Check for promises that have timed-out.  Should be called every loop.
  promise.process();
}

void alternateLoop(){
  // Remember to call Particle.process() and promise.process() in a while loop
  Particle.process();
  promise.process();
  static unsigned int startTime = millis();
  static unsigned int lastTime = millis();
  if(millis() - lastTime > 1000){
    lastTime = millis();
    Serial.printlnf("Resolving Promise... Time: %u ms", millis()-startTime);
  }
}





void sendRequest(){
  Serial.printlnf("Sending Request");
  Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
}
void sendRequestError(){
  Serial.printlnf("Sending Request");
  Particle.publish("TestParticlePromise", "{\"param\": \"error\"}", 60, PRIVATE);
}
void sendRequestTimeout(){
  Serial.printlnf("Sending Request");
  Particle.publish("Nowhere", NULL, 60, PRIVATE);
}
void successFunc(const char* event, const char* data){
  Serial.printlnf("Success Response: %s", data);
}
void errorFunc(const char* event, const char* data){
  Serial.printlnf("Error Response: %s", data);
}
void timeoutFunc(){
  Serial.printlnf("ERROR - Server did not respond");
}
void finalFunc(){
  Serial.printlnf("All Set");
}

void waitForPromiseToTimeout(int time){
  unsigned int startTime = millis();
  while(millis()-startTime < time+20){
    Particle.process();
    promise.process();
  }
}

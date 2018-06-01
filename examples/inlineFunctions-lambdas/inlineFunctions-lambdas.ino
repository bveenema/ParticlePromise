// Inline Function (Lambda Expression) usage for ParticlePromise library

#include "ParticlePromise.h"

// Initialize ParticlePromise: Creates a PromiseContainer with default size of 5
//    positions (max number of promises) and 20 character maximum topic
ParticlePromise promise;

void setup() {
  Serial.begin(9600);

  // Setup the Particle.subscribe() hook.  Must be called before creating a promise
  promise.enable();

  // **** BASIC INLINE FUNCTIONS ****
  //    The functions passed to ParticlePromise can be defined inline.
  //    This can make your promise calls concise and simple to reason about once
  //    a user is used to the syntax
  //
  //    Inline functions are more formally known as lambda expressions or functions.
  //    In C++, lambda expressions begin with "[]" instead of a function name
  //    and do not require a return type.
  //
  //    All functions passed to ParticlePromise require void return type, so
  //    functions will take on the forms:
  //       success and error functions:    [](const char* event, const char* data){ FUNCTION BODY }
  //       timeout and finally functions:  []{ FUNCTION BODY}
  //    EXAMPLE:
  auto& p1 = promise.create([]{
            Serial.printlnf("Sending Request");
            Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
          }, "test")
          .then([](const char* event, const char* data){
            Serial.printlnf("Success Response: %s", data);
          });

  // wait for promise to resolve before running next example
  while(p1.getStatus()){
    Particle.process();
    promise.process();
  }

  // **** CAPTURES / VARIABLE SCOPE ****
  //    Lambda expressions/functions have a unique ability to capture variables
  //    into their scope using the "[]" at the beginning of the lambda expression
  //    by placing keywords and symbols into the brackets, variables can be captured
  //    and used in the body of the lambda expression. Designators include:
  //      [=] captures all automatic (non-global) variables by copying the value
  //      [&] captures all automatic (non-global) variables by their address
  //      [var] captures automatic (non-global) variable "var" by copying
  //      [&var] captures automatic (non-global) variable "var" by address
  //      **Multiple variables can be captured by seperating with a comma
  //
  //    A unique property of capturing by copy is it captures the value of the
  //    variable at the time the lambda expression is passed to the function, so
  //    if the variable changes after the lambda expression it will not be seen.
  //    EXAMPLE:
  int var = 42;
  promise.create([]{
            Serial.printlnf("Sending Request");
            Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
          }, "test")
          .then([var](const char* event, const char* data){
            Serial.printlnf("Success Function var = %u", var);
          });
  var = 5; // var is now 5, but when this promise returns, "var = 42" will be printed
  Serial.printlnf("Actual var is %u", var);

}

void loop() {
  // Check for promises that have timed-out.  Should be called every loop.
  promise.process();
}

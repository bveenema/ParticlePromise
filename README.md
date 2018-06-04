# ParticlePromise

A Particle library for for creating Javascript-like Promises that manage your webhook/API callbacks. ParticlePromise makes it possible to write a block of code like:
``` cpp
promise.create(myWebhookCaller, "MyResponseTopic")
       .then(myCallbackFunction)
       .timeout(myTimeoutFunction);
```
and forget about managing the rest.  ParticlePromise will call the appropriate function at the appropriate time. You can also create functions for errors, `.error(myErrorFunction) or .then(myCallbackFunction, myErrorFunction)` and final functions that will always be run no matter the outcome `.finally(myFinalFunction)`

ParticlePromise can also accept inline(lambda) functions so that all of your API calling code can be co-located in one place.
``` cpp
promise.create([]{
          Particle.publish("WebhookName", NULL, 60, PRIVATE);
        }, "MyResponseTopic")
       .then([](const char* event, const char* data){
          Serial.printlnf("My data is: %s", data);
        });
```
This syntax may look odd if you're unfamiliar with Javascript callbacks but it's a convenient way to keep all of your code in one place and make it easier to reason about.

## Usage

Using ParticlePromise can be as simple as

``` cpp
#include "ParticlePromise.h"

ParticlePromise promise;

void setup() {
  Serial.begin(9600);

  promise.enable();

  promise.create(sendRequest, "test")
         .then(onSuccessFunc);
}

void loop() {
  promise.process();
}

void sendRequest(){
  Serial.printlnf("Sending Request");
  Particle.publish("TestParticlePromise", "{\"param\": \"posts/1\"}", 60, PRIVATE);
}

void onSuccessFunc(const char* event, const char* data){
  Serial.printlnf("Success Response: %s", data);
}

```

`.create()` uses one of the open Promise objects in the PromiseContainer created by ParticlePromise and calls sendRequest which is a function you create that must call your webhook (`Particle.publish()`).  It then returns a reference to the Promise object so that you can then call other functions on it, like `.then()` that stores your function to call on successful return of the API call.

`.process()` checks all of your current, open, Promises to see if they have timed out or not and calls your timeout function if it has.

ParticlePromise will automatically call your on-success function when the API responds successfully, or it will call your on-error function on error.  If the API doesn't respond after a set amount of time (configurable on an individual Promise basis), then ParticlePromise will call your on-timeout function.

After any of these completions of the Promise (on-success, on-error, timeout), ParticlePromise will call your `final` function, a function that runs no matter the outcome, which is a great way to set flags so the rest of your program can know that the API-calling part is done.

You can supply all or none of the on-success, on-error, timeout and final functions (or anywhere in between).  If a function is not supplied, ParticlePromise will simply not call anything when the API responds.

## Webhook setup
ParticlePromise requires you to setup your webhook response topics in a specific manner. Your Response Topic must be of the format:
```
{{{PARTICLE_DEVICE_ID}}}/promise-response/success/{myResonseTopic}
```
and your error Response Topic must be of the format:
```
{{{PARTICLE_DEVICE_ID}}}/promise-response/error/{myResonseTopic}
```

Savvy users will recognize that the inclusion of `{{{PARTICLE_DEVICE_ID}}}` means your API response cannot go to multiple devices.  In most cases this is likely the perfect setup, but this may not work for all users.

## Documentation

##### Constructor
------
Creates the PromiseContainer (a std::vector composed of Promise(Prom) objects).  PromiseContainer is statically allocated in heap memory.  The default constructor creates a PromiseContainer with 5 Promises (+1 dummy Promise) that can accept a maximum length Response Topic of 20 characters.  This uses ~728 bytes.  Users can specify differently sized PromiseContainers depending on their needs.  Care should be taken as too-large PromiseContainers tend to result in Hard-Fault errors on the Photon.
``` cpp
ParticlePromise promise; // default constructor: 5 Promises, 20 char Response Topic - ~728 bytes
ParticlePromise promise(3); // 3 Promises, 20 char Response Topic - ~512 bytes
ParticlePromise promise(5,60); // 5 Promises, 60 char Response Topic - ~968 bytes
```

##### Enable
------
Enable must be called prior to [.create()]. Sets up the API response handler.  Uses 1 of 4 Particle.subscribe() slots available to user code
``` cpp
promise.enable();
```
##### Set Timeout
------
Sets a new default timeout that will be used for all subsequent Promises (does not effect current, in-process) Promises.  Default is 5 seconds.
``` cpp
promise.setTimeout(10000); // Update default timeout to 10 seconds (unit: milliseconds)
```

##### Process
------
Checks all in-process Promises for timeout expiration.  Should be called every loop (or more frequently);
``` cpp
promise.process();
```

##### Create
------
Creates a new Promise. Must be passed a function that calls the API/Webhook (ie `Particle.publish()`) and a Response Topic.

The function that calls the API/Webhook (sendWebhookFunc) must return void and accept no arguments in addition to calling `Particle.publish()`

Can additionally be passed a timeout parameter that specifies a custom timeout for the individual Promise

``` cpp
// SYNTAX (Arguments in brackets "{}" are optional)
Prom& create(std::function sendWebhookFunc, const char * responseTopic, {unsigned int timeout});
Prom& create(void (T::*sendWebhookFunc)(void), T *instance, const char* responseTopic, {unsigned int timeout})

// EXAMPLE USAGE
promise.create(myWebhookCaller, const char* "MyResponseTopic"); // Create a Promise named "MyResponseTopic" that calls myWebhookCaller with default timeout
promise.create(myWebhookCaller, "MyResponseTopic", 10000); // Create Promise named "MyResponseTopic" that calls myWebhookCaller with 10 sec timeout
auto& p = promise.create(myWebhookCaller, "MyResponseTopic"); // Create Promise as above and store reference to Promise object in variable named "p"
promise.create([]{
  // FUNCTION BODY that must call
  Particle.publish();
},"MyResponseTopic"); // in-line(lambda) example
```

NOTE: Returns a reference to a Promise (PROM) object that can either be ignored (most cases) or stored in a variable.  It's recommended to use `auto` for creating the variable to keep code easiest to read.

##### Then (PROM)
------
Specifies the function to call on successful response from the API.  Can additionally be passed an error function to be called on-error.

Both success and error functions must return void and accept 2x const char* arguments (same as [Particle.subscribe] handler)
``` cpp
// SYNTAX (Arguments in brackets "{}" are optional)
Prom& then(std::function successFunc, {std::function errorFunc});
Prom& then(void (T::*successFunc)(const char*, const char*), {void (T::*errorFunc)(const char*, const char*)}, T *instance);

// EXAMPLE USAGE
___.then(onSuccessFunction);
___.then(onSuccessFunction, onErrorFunction);
___.then(&MyClass::onSuccessFunc, &MyObject);// or this instead of &MyObject
___.then(&MyClass::onSuccessFunc, &MyClass::onErrorFunc, &MyObject);// or this instead of &MyObject
___.then([](const char* event, const char* data){
  //SUCCESS FUNCTION BODY
}); // in-line(lambda) example
___.then([](const char* event, const char* data){
  // SUCCESS FUNCTION BODY
}, [](const char* event, const char* data){
  // ERROR FUNCTION BODY
}); // in-line(lambda) example
```
NOTE: must be called on a Promise (PROM) object either through method/dot chaining or by storing the object reference (see [.create()]) as a variable and calling on that. Returns reference to same Promise (PROM) object

##### Error (PROM)
------
Specifies the function to call if/when the API returns with an error (such as 404).  Error functions must return void and accept 2x const char* arguments (same as [Particle.subscribe])
``` cpp
// SYNTAX (Arguments in brackets "{}" are optional)
Prom& error(std::function errorFunc);
Prom& error(void (T::*errorFunc)(const char*, const char*), T *instance);

// EXAMPLE USAGE
___.error(onErrorFunction);
___.error(&MyClass::onErrorFunc, &MyObject); // or this instead of &MyObject
___.error([][](const char* event, const char* data){
  // ERROR FUNCTION BODY
}); // in-line(lambda) example
```
NOTE: must be called on a Promise (PROM) object either through method/dot chaining or by storing the object reference (see [.create()]) as a variable and calling on that. Returns reference to same Promise (PROM) object

##### Timeout (PROM)
------
Specifies the function to call if/when the API does not respond (times out).  Timeout functions must return void and accept no arguments.
Can optionally be supplied a timeout parameter which sets a custom timeout for the individual Promise.  If a timeout parameter is supplied, the timeout timer is reset when `.timeout()` is called and if a value was supplied in [.create()] it will be overridden.
``` cpp
// SYNTAX (Arguments in brackets "{}" are optional)
Prom& timeout(std::function timeoutFunc, {unsigned int timeout});
Prom& timeout(void (T::*timeoutFunc)(void), T *instance, {unsigned int timeout})

// EXAMPLE USAGE
___.timeout(onTimeoutFunction); // Add a timeout function to a Promise
___.timeout(onTimeoutFunction, 10000); // Add a timeout function to a Promise and use custom timeout time of 10 sec (ovverrides and resets a timeout time if given in .create())
___.timeout(&MyClass::onTimeoutFunc, &MyObject, {10000}); // Add a timeout member function from a class with optional custom timeout , can use "this" instead of &MyObject
___.timeout([]{
  // TIMEOUT FUNCTION BODY
}, {10000}); // in-line(lambda) example with optional custom timeout
```
NOTE: must be called on a Promise (PROM) object either through method/dot chaining or by storing the object reference (see [.create()]) as a variable and calling on that. Returns reference to same Promise (PROM) object

##### Finally (PROM)
------
Specifies the function to be called after the Promise resolves, no matter what the outcome.  Final functions must return void and accept no arguments. Final functions will always be called after success, error or timeout functions.
Final functions are often used for signaling the completion of the Promise to the rest of the application.
``` cpp
// SYNTAX (Arguments in brackets "{}" are optional)
Prom& finally(std::function finalFunc);
Prom& finally(void (T::*finalFunc)(void), T *instance);

// EXAMPLE USAGE
___.finally(finalFunction); // Add a final function to be called no matter the Promise resolution mode
___.finally(&MyClass::finalFunc, &MyObject); // Add a final funciton from a class, can use "this" instead of &MyObject
___.finally([]{
  // FINAL FUNCTION BODY
}); // in-line(lambda) example
```
NOTE: must be called on a Promise (PROM) object either through method/dot chaining or by storing the object reference (see [.create()]) as a variable and calling on that. Returns reference to same Promise (PROM) object

##### Get Status (PROM)
------
Return the status of the Promise.  Returns true if the Promise is in process. Returns false if the Promise is resolved.
``` cpp
// SYNTAX (Arguments in brackets "{}" are optional)
bool getStatus();

// EXAMPLE USAGE
// Create a Promise and store reference in variable "p"
auto& p = promise.create(myWebhookCaller, "MyResponseTopic");
bool PromiseStatus = p.getStatus(); // Returns true if Promise has not resolved

// getStatus() is useful for specific functions or alternative loops while waiting for a Promise to resolve EX.
while(p.getStatus()){
  alternateFunction();
  // NOTE: be sure to call Particle.process(); and promise.process() in the while loop
}
```
NOTE: must be called on a Promise (PROM) object either through method/dot chaining or by storing the object reference (see [.create()]) as a variable and calling on that.

##### Is Valid (PROM)
------
Returns true if the Promise it is called on is valid.
In order to avoid run-time issues, a dummy, in-valid Promise is returned if a user attempts to create a new Promise when there is no available slots in the PromiseContainer. This way an unknowing user, can still call `.then()`, `.error()`, etc. on the dummy Promise without crashing the application.
The dummy Promise will never resolve and will never call the success, error, timeout or final functions.
``` cpp
// SYNTAX (Arguments in brackets "{}" are optional)
bool isValid();

// EXAMPLE USAGE
auto& p = promise.create(myWebhookCaller, "MyResponseTopic");
bool GoodPromise = p.isValid(); // Returns true if the Promise that was created is valid. An invalid Promise will not be evaluated
```
NOTE: must be called on a Promise (PROM) object either through method/dot chaining or by storing the object reference (see [.create()]) as a variable and calling on that.

## Contributing


## LICENSE
Copyright 2018 Ben Veenema

Licensed under the <insert your choice of license here> license

[.create()]:(#create)
[Particle.subscribe]:(https://docs.particle.io/reference/firmware/photon/#particle-subscribe-)

// Example usage for ParticlePromise library by Ben Veenema.

#include "ParticlePromise.h"

// Initialize objects from the lib
ParticlePromise promise;

void setup() {
  Serial.begin(9600);
  promise.enable();

  // Most basic Request
  promise.create(sendRequest, "test").then(successFunc,errorFunc);
}

void loop() {
  promise.process();
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

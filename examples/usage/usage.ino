// Example usage for ParticlePromise library by Ben Veenema.

#include "ParticlePromise.h"

// Initialize objects from the lib
ParticlePromise promise(5, 60);

void setup() {
  Serial.begin(9600);
  Serial.printlnf("Free Memory: %u", System.freeMemory());
}

void loop() {

}

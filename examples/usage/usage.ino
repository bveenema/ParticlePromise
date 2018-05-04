// Example usage for ParticlePromise library by Ben Veenema.

#include "ParticlePromise.h"

// Initialize objects from the lib
ParticlePromise particlePromise;

void setup() {
    // Call functions on initialized library objects that require hardware
    particlePromise.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    particlePromise.process();
}

#pragma once

/* ParticlePromise library by Ben Veenema
 */

// This will load the definition for common Particle variable types
#include "Particle.h"

// This is your main class that users will import into their application
class ParticlePromise{
public:
  /**
   * Constructor
   */
  ParticlePromise();

  /**
   * Example method
   */
  void begin();

  /**
   * Example method
   */
  void process();

private:
  /**
   * Example private method
   */
  void doit();
};

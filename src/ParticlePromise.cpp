/* ParticlePromise library by Ben Veenema
 */

#include "ParticlePromise.h"

/**
 * Constructor.
 */
ParticlePromise::ParticlePromise()
{
  // be sure not to call anything that requires hardware be initialized here, put those in begin()
}

/**
 * Example method.
 */
void ParticlePromise::begin()
{
    // initialize hardware
    Serial.println("called begin");
}

/**
 * Example method.
 */
void ParticlePromise::process()
{
    // do something useful
    Serial.println("called process");
    doit();
}

/**
* Example private method
*/
void ParticlePromise::doit()
{
    Serial.println("called doit");
}

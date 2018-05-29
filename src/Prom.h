#include "Particle.h"
#include <functional>

class Prom{
  friend class ParticlePromise;
public:
  Prom(unsigned int charLen);
  Prom& then(void (*_successFunc)(const char*, const char*));
  Prom& then(void (*_successFunc)(const char*, const char*), void (*_errorFunc)(const char*, const char*));
  Prom& error(void (*_errorFunc)(const char*, const char*));
  Prom& timeout(void (*_timeoutFunc)(void), uint32_t timeout = 0);
  void finally(void (*_finalFunc)(void));

  bool valid;

private:
  bool inUse;
  char* responseTopic;
  unsigned int timeoutTime;
  std::function<void(const char*, const char*)> successFunc;
  std::function<void(const char*, const char*)> errorFunc;
  std::function<void(void)> timeoutFunc;
  std::function<void(void)> finalFunc;
};

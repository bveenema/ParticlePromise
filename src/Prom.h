#include "Particle.h"
#include <functional>

class Prom{
  friend class ParticlePromise;
public:
  Prom(unsigned int charLen);

  // .then() methods
  Prom& then(void (*_successFunc)(const char*, const char*));
  Prom& then(void (*_successFunc)(const char*, const char*), void (*_errorFunc)(const char*, const char*));

  template <typename T>
  Prom& then(void (T::*_successFunc)(const char*, const char*), T *instance){
    using namespace std::placeholders;
    successFunc = std::bind(_successFunc, instance, _1, _2);
  }

  // .error() methods
  Prom& error(void (*_errorFunc)(const char*, const char*));

  // .timeout() methods
  Prom& timeout(void (*_timeoutFunc)(void), uint32_t timeout = 0);

  // .finally() methods
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

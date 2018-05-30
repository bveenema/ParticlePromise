#include "Particle.h"
#include <functional>

class Prom{
  friend class ParticlePromise;
public:
  Prom(unsigned int charLen){
    responseTopic = new char[charLen+1];
  }

  // .then() methods
  Prom& then(void (*_successFunc)(const char*, const char*)){
    successFunc = _successFunc;
    return *this;
  }
  Prom& then(void (*_successFunc)(const char*, const char*), void (*_errorFunc)(const char*, const char*)){
    this->then(_successFunc);
    this->error(_errorFunc);
    return *this;
  }

  template <typename T>
  Prom& then(void (T::*_successFunc)(const char*, const char*), T *instance){
    using namespace std::placeholders;
    successFunc = std::bind(_successFunc, instance, _1, _2);
  }

  // .error() methods
  Prom& error(void (*_errorFunc)(const char*, const char*)){
    errorFunc = _errorFunc;
    return *this;
  }

  // .timeout() methods
  Prom& timeout(void (*_timeoutFunc)(void), uint32_t timeout = 0){
    if(timeout != 0){
      timeoutTime = millis() + timeout;
    }
    timeoutFunc = _timeoutFunc;
    return *this;
  }

  // .finally() methods
  void finally(void (*_finalFunc)(void)){
    finalFunc = _finalFunc;
  }

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

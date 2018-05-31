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
    return *this;
  }

  template <typename T>
  Prom& then(void (T::*_successFunc)(const char*, const char*), void (T::*_errorFunc)(const char*, const char*), T *instance){
    using namespace std::placeholders;
    successFunc = std::bind(_successFunc, instance, _1, _2);
    errorFunc = std::bind(_errorFunc, instance, _1, _2);
    return *this;
  }

  // .error() methods
  Prom& error(void (*_errorFunc)(const char*, const char*)){
    errorFunc = _errorFunc;
    return *this;
  }

  template <typename T>
  Prom& error(void (T::*_errorFunc)(const char*, const char*), T *instance){
    using namespace std::placeholders;
    errorFunc = std::bind(_errorFunc, instance, _1, _2);
    return *this;
  }

  // .timeout() methods
  Prom& timeout(void (*_timeoutFunc)(void), unsigned int timeout = 0){
    setTimeoutTime(timeout);
    timeoutFunc = _timeoutFunc;
    return *this;
  }

  template <typename T>
  Prom& timeout(void (T::*_timeoutFunc)(void), T *instance, unsigned int timeout = 0){
    setTimeoutTime(timeout);
    using namespace std::placeholders;
    timeoutFunc = std::bind(_timeoutFunc, instance);
    return *this;
  }

  // .finally() methods
  Prom& finally(void (*_finalFunc)(void)){
    finalFunc = _finalFunc;
    return *this;
  }

  template <typename T>
  Prom& finally(void (T::*_finalFunc)(void), T *instance){
    using namespace std::placeholders;
    finalFunc = std::bind(_finalFunc, instance);
    return *this;
  }

  bool getStatus(){
    return this->pending;
  }

  bool isValid(){
    return this->valid;
  };


private:

  bool valid;
  bool pending;
  char* responseTopic;
  unsigned int timeoutTime;
  std::function<void(const char*, const char*)> successFunc;
  std::function<void(const char*, const char*)> errorFunc;
  std::function<void(void)> timeoutFunc;
  std::function<void(void)> finalFunc;

  void setTimeoutTime(unsigned int timeout){
    if(timeout != 0){
      timeoutTime = millis() + timeout;
    }
  }
};

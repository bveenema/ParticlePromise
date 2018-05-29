#include "Prom.h"

Prom::Prom(unsigned int charLen){
  responseTopic = new char[charLen+1];
}
Prom& Prom::then(void (*_successFunc)(const char*, const char*)){
  successFunc = _successFunc;
  return *this;
}
Prom& Prom::then(void (*_successFunc)(const char*, const char*), void (*_errorFunc)(const char*, const char*)){
  this->then(_successFunc);
  this->error(_errorFunc);
  return *this;
}
Prom& Prom::error(void (*_errorFunc)(const char*, const char*)){
  errorFunc = _errorFunc;
  return *this;
}
Prom& Prom::timeout(void (*_timeoutFunc)(void), uint32_t timeout){
  if(timeout != 0){
    timeoutTime = millis() + timeout;
  }
  timeoutFunc = _timeoutFunc;
  return *this;
}
void Prom::finally(void (*_finalFunc)(void)){
  finalFunc = _finalFunc;
}

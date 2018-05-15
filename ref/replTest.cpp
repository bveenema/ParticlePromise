#include <iostream>
#include <functional>

void aFunction(int a, int b){
  printf("%d-%d=%d",a,b,a-b);
}

class aClass
{
public:
  void aTest(int a, int b)
  {
      printf("%d+%d=%d",a,b,a+b);
  }
};

class FuncStore{
  public:
    template <typename T>
    void addFunction(void (T::*handler)(int, int), T *instance){
      using namespace std::placeholders;
      fp = std::bind(handler, instance, _1, _2);
    }
    void addFunction(void (*function)(int,int)){
      fp = function;
    }
    std::function<void(int, int)> returnFunction(){
      return fp;
    }
    void callFunction(int first, int second){
      fp(first, second);
    }
  private:
    std::function<void(int, int)> fp;
};


int main ()
{
    aClass a;
    FuncStore f;

    f.addFunction(&aClass::aTest, &a);
    f.callFunction(1,5);

    f.addFunction(aFunction);
    f.callFunction(1,3);

}


// ---------------------------------------------------
#include <iostream>
#include <string.h>
#include <functional>

// class Promise{
// public:
//   Promise(uint8_t maxTopicLength){
//     responseTopic = new char[maxTopicLength];
//   }
//   const char* responseTopic;
// };

class Foo{
  Foo();

  void Bar(uint8_t test ,uint8_t teste){

  }
};

template<uint8_t N>
struct Promise {
  char responseTopic[N];
  std::function<void(const char*, const char*)> successFunc;
};

void* myArrayOfClasses;

const uint8_t maxTopicLength = 20;

int main() {
  Foo foo();

  Promise<maxTopicLength> * myArrayOfClasses = new Promise<maxTopicLength>[5];

  strcpy(myArrayOfClasses[2].responseTopic, "test");

  //myArrayOfClasses[2].responseTopic = "test";

  std::cout << "Empty Container Position: " << myArrayOfClasses[2].responseTopic << "\n";
}


//------------------------------------------------------------------------------
#pragma once

#include <iostream>
#include <string.h>


/* ParticlePromise library by Ben Veenema
 */


//#include "Particle.h"
#include <functional>

class Promise{
public:
  Promise(uint8_t charLen = 20){
    //responseTopic = new char[charLen];
  }
  bool inUse;
  char responseTopic[5];
  uint32_t timeoutTime;
  std::function<void(const char*, const char*)> successFunc;
  std::function<void(const char*, const char*)> errorFunc;
  std::function<void(void)> timeoutFunc;
  std::function<void(void)> finalFunc;
};


class ParticlePromise{
public:
  /**
   * Constructor: Intializes PromiseContainer and sets up Particle.subscribe()
   */
   ParticlePromise(const uint8_t _containerSize = 5, const uint8_t _maxTopicLength = 20){
     PromiseContainer = new Promise[5];
     PromiseContainer[1].timeoutTime = 100;
     strcpy(PromiseContainer[1].responseTopic, "test12345678901234567890");
     PromiseContainer[0].timeoutTime = 69;
     PromiseContainer[2].timeoutTime = 42;
     PromiseContainer[3].timeoutTime = 42;
     std::cout << "test: " << PromiseContainer[0].timeoutTime << ", " << PromiseContainer[2].timeoutTime << ", " << PromiseContainer[7].timeoutTime << "\n";
     std::cout << "string: " << PromiseContainer[1].responseTopic << "\n";
     std::cout << "time: " << PromiseContainer[1].timeoutTime << "\n";
   }

private:
  Promise* PromiseContainer;
};

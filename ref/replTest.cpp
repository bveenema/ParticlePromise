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

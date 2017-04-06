
#include <iostream>

#include "Int2Type.h"


class MyClass {
};


class MyPolymorphicClass {
public:
    MyPolymorphicClass* Clone() {return NULL;}
};

template <typename T, bool isPolymorphic>
class NiftyContainer {
private:
  void DoSomething(T* pObj, Int2Type<true>) {
    T* pNewObject = pObj->Clone();
    // ... polymorphic algorithm ...
    (void)pNewObject;
  }

  void DoSomething(T* pObj, Int2Type<false>) {
    T* pNewObj = new T(*pObj);
    // ... non-polymorphic algorithm ...
    (void)pNewObj;
  }

public:
  // ...
  void DoSomething() {
    T* pSomeObj = new T;
    DoSomething(pSomeObj, Int2Type<isPolymorphic>());
    (void)pSomeObj;
  }
};



int main(void) {

  NiftyContainer<MyClass, false> myClass;
  myClass.DoSomething();

  /* This doesn't work. No Clone method available
  NiftyContainer<MyClass, true> myClass2;
  myClass2.DoSomething(); */

  NiftyContainer<MyPolymorphicClass, true> myPolymorphicClass;
  myPolymorphicClass.DoSomething();



  return 0;
}

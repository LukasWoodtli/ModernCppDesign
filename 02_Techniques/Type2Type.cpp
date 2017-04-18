#include <iostream>

class Gadget {
public:
  Gadget(int i) {(void) i;}
};

class Widget {
public:
  Widget(int i, int j) {
      (void)i;
    std::cout << j << "\n";
  }
};

template <class T, class U>
T* Create(const U& arg) {
  return new T(arg);
}


// 1.) not working implementation

// You cannot partially specialize a function template:
/* this is illegal
template <class U>
Widget* Create<Widget, U>(const U& arg) {
  return new Widget(arg, -1);
}
*/



// 2.) not nice implementation

template <class T, class U>
T* Create2(const U& arg, T /*dummy */) {
  return new T(arg);
}

template <class U>
Widget * Create2(const U& arg, Widget /* dummy */) {
  return new Widget(arg, -1);
}


// 3.) Type2Type implementation (see header)
#include "Type2Type.h"


int main(void) {
  // 2.)
  Gadget * myGadget2 = Create2(1, Gadget(2)); // the Gadget as argument is not used (dummy)
    (void)myGadget2;

  Widget* myWidget2 = Create2(3, Widget(4, 5)); // the Widget as argument is not used (dummy)
    (void)myWidget2;


  // 3.)
  Gadget * myGadget3 = Create3(6, Type2Type<Gadget>());
  (void)myGadget3;


  Widget* myWidget3 = Create3(7, Type2Type<Widget>());
  (void)myWidget3;


  return 0;
}

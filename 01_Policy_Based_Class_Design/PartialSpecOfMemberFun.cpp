
#include <iostream>

using namespace std;

template <class T>
class Widget
{
public:
  void Fun() {cout << "Generic Widget::Fun() implementation" << endl;}
};

// OK: specialization of a member function
template <>
void Widget<char>::Fun() {cout << "Widget<char>::Fun() implementation" << endl;}

template <class T, class U>
class Gadget
{
public:
  void Fun() {cout << "Generic Gadget::Fun() implementation" << endl;}
};

// Error! Cannot partially specialize member function of Gadget
// This doesn't work!
/*template <class U>
void Gadget<char, U>::Fun()  {cout << "Gadget<char, U>::Fun() implementation" << endl;}
*/

// but this works (specialising the class)
template <class U>
class Gadget<char, U> {
public:
  void Fun()  {cout << "Gadget<char, U>::Fun() implementation" << endl;}
};

int main(void)
{
  Widget<long> genericwidget;
  genericwidget.Fun();

  Widget<char> charWidget;
  charWidget.Fun();

  Gadget<int, int> genericGadget;
  genericGadget.Fun();
  
  // This doesn't work
  // Gadget<int> charGadget;

  // but this works
  Gadget<char, int> charGadget;
  charGadget.Fun();
    
  return 0;
}

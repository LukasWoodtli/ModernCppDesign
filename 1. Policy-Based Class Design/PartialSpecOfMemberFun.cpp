
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
/* This doesn't work!
template <class U>
void Gadget<char, U>::Fun()  {cout << "Gadget<char, U>::Fun() implementation" << endl;}
*/

int main(int argc, char* argv[])
{
  Widget<long> genericwidget;
  genericwidget.Fun();

  Widget<char> charWidget;
  charWidget.Fun();
 
  int a;
  cin >> a;

  return 0;
}


#include "Typelists.h"

/*** Tests **************************/

// comparing types (for testing)
template<class T, class U>
struct is_same {
  enum { value = 0};
};
template<class T>
struct is_same<T,T> {
  enum { value = 1};
};

// create typelist
typedef TYPELIST_4(signed char, short int, int, long int) SignedIntegrals;

// lenght
static_assert(4 == Length<SignedIntegrals>::value, "Length of typelist not correct");

// type at
typedef typename TypeAt<SignedIntegrals, 1>::Result actualTypeAtIndex1;
static_assert(is_same<actualTypeAtIndex1, short int>::value, "Wrong type at index");

// this gives: "error: implicit instantiation of undefined template 'TypeAt<NullType, 2>'"
// meaning "out of bound"
//typedef typename TypeAt<SignedIntegrals, 6>::Result newType;

// type at non strict
typedef typename TypeAtNonStrict<SignedIntegrals, 6, int>::Result actualDefaultType;
static_assert(is_same<actualDefaultType, int>::value, "Wrong default type");

// index of
enum { actualIndexOf = IndexOf<SignedIntegrals, short int>::value };
static_assert(actualIndexOf == 1, "Wrong index for given type");

// append
typedef Append<SignedIntegrals, TYPELIST_3(float, double, long double)>::Result SignedTypes;
static_assert(7 == Length<SignedTypes>::value, "Length of typelist not correct");

// erase
typedef Erase<SignedTypes, float>::Result actualSomeSignedTypes;
typedef TYPELIST_6(signed char, short int, int, long int, double, long double) expectedSomeSignedTypes;
static_assert(is_same<actualSomeSignedTypes, expectedSomeSignedTypes>::value, "Erasing didn't work");


// erase all
typedef TYPELIST_4(char, double, char, long double) someOtherSignedTypes;
typedef EraseAll<someOtherSignedTypes, char>::Result actualSomeOtherSignedTypes;
typedef TYPELIST_2(double, long double) expectedSomeOtherSignedTypes;
static_assert(is_same<actualSomeOtherSignedTypes, expectedSomeOtherSignedTypes>::value, "Erasing all didn't work");


// widget classes
class Widget {};
class Button : public Widget {};
class GraphicButton : public Button {};
class TextField : public Widget {};
class ScrollBar : public Widget {};



// no duplicates
typedef TYPELIST_7(Widget, Button, Button, Widget, TextField, ScrollBar, Button) someWidgets;
typedef TYPELIST_4(Widget, Button, TextField, ScrollBar) expectedSomeWidgets;
typedef NoDuplicates<someWidgets>::Result actualSomeWidgets;
static_assert(is_same<actualSomeWidgets, expectedSomeWidgets>::value, "Removing duplicates didn't work");

// replace
typedef TYPELIST_4(Widget, Button, TextField, ScrollBar) someOtherWidgets;
typedef TYPELIST_4(Widget, GraphicButton, TextField, ScrollBar) expectedSomeOtherWidgets;
typedef Replace<someOtherWidgets, Button, GraphicButton>::Result actualSomeOtherWidgets;
static_assert(is_same<expectedSomeOtherWidgets, actualSomeOtherWidgets>::value, "Replacement didn't work");

typedef TYPELIST_5(Widget, Button, TextField, ScrollBar, Button) someMoreWidgets;
typedef TYPELIST_5(Widget, GraphicButton, TextField, ScrollBar, Button) expectedSomeMoreWidgets;
typedef Replace<someMoreWidgets, Button, GraphicButton>::Result actualSomeMoreWidgets;
static_assert(is_same<expectedSomeMoreWidgets, actualSomeMoreWidgets>::value, "Replacement of only first occurence didn't work");

// replace all
typedef TYPELIST_5(Widget, Button, TextField, ScrollBar, Button) someDuplicateWidgets;
typedef TYPELIST_5(Widget, GraphicButton, TextField, ScrollBar, GraphicButton) expectedSomeDuplicateWidgets;
typedef ReplaceAll<someDuplicateWidgets, Button, GraphicButton>::Result actualSomeDuplicateWidgets;
static_assert(is_same<expectedSomeDuplicateWidgets, actualSomeDuplicateWidgets>::value, "Replacement of all occurences didn't work");

// derived to front
typedef TYPELIST_4(Widget, ScrollBar, Button, GraphicButton) someUnorderedWidgets;
typedef TYPELIST_4(GraphicButton, ScrollBar, Button, Widget) expectedOrderedWidgets;
typedef typename DerivedToFront<someUnorderedWidgets>::Result actualOrderedWidgets;
static_assert(is_same<expectedOrderedWidgets, actualOrderedWidgets>::value, "Ordering widgets didn't work");

// scattered hierarchy
template <class T>
struct Holder {
  T value_;
};

typedef GenScatterHierarchy<TYPELIST_3(int, std::string, Widget), Holder> WidgetInfo;
typedef GenScatterHierarchy<TYPELIST_3(int, int, std::string), Holder> WidgetInfoMultipleInts;

// linear hierarchy
template <class T, class Base>
class EventHandler : public Base {
public:
	virtual void OnEvent(T& obj, int evcentId);
};

typedef GenLinearHierarchy<TYPELIST_3(TextField, Button, ScrollBar), EventHandler> WidgetEventHandler;



int main(void) {
  // scattered hierarchy
  WidgetInfo obj;
  std::string name = (static_cast<Holder<std::string>&>(obj)).value_;
  name = Field<std::string>(obj).value_;
  int i = Field<int>(obj).value_;
  (void)i;

  WidgetInfoMultipleInts multipleInts;
  //int i = Field<int>(multipleInts).value_; // this is not working: ambiguity

  /* TODO
  int x = Field<0>(obj).value_; // first int
  int y = Field<1>(obj).value_; // second int
  */

  return 0;
}

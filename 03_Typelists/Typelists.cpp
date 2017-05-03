#include <string>


#include "../02_Techniques/Convertibility.h"
#include "../02_Techniques/Type2Type.h"
#include "../02_Techniques/Int2Type.h"


class NullType {};

template <class T, class U>
struct Typelist {
  typedef T Head;
  typedef U Tail;
};

// in C++ 11: variadic templates or variadic macros
#define TYPELIST_1(T1) Typelist<T1, NullType>
#define TYPELIST_2(T1, T2) Typelist<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3) Typelist<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4) Typelist<T1, TYPELIST_3(T2, T3, T4)>
#define TYPELIST_5(T1, T2, T3, T4, T5) Typelist<T1, TYPELIST_4(T2, T3, T4, T5)>
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6)>
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7)>
// ....


// Length
template <class TList>
struct Length;

template <>
struct Length<NullType> {
  enum { value = 0 };
};

template <class T, class U>
struct Length<Typelist<T, U>> {
  enum { value = 1 + Length<U>::value };
};


// TypeAt
template <class TList, unsigned int index>
struct TypeAt;

template <class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0> {
  typedef Head Result;
};

template <class Head, class Tail, unsigned int i>
struct TypeAt<Typelist<Head, Tail>, i> {
  typedef typename TypeAt<Tail, i - 1>::Result Result;
};

// TypeAtNonStrict
template <class TList, unsigned int index, class Default>
struct TypeAtNonStrict;

template <class Head, class Tail, class Default>
struct TypeAtNonStrict<Typelist<Head, Tail>, 0, Default> {
  typedef Head Result;
};

template <class Head, class Tail, unsigned int i, class Default>
struct TypeAtNonStrict<Typelist<Head, Tail>, i, Default> {
  typedef typename TypeAtNonStrict<Tail, i - 1, Default>::Result Result;
};

template <class Head, unsigned int i, class Default>
struct TypeAtNonStrict<Typelist<Head, NullType>, i, Default> {
  typedef Default Result;
};


// IndexOf
template <class TList, class T>
struct IndexOf;

template <class T>
struct IndexOf<NullType, T> {
	enum { value = -1 };
};

template <class T, class Tail>
struct IndexOf<Typelist<T, Tail>, T> {
	enum { value = 0 };
};

template <class Head, class Tail, class T>
struct IndexOf<Typelist<Head, Tail>, T> {
private:
	enum { temp = IndexOf<Tail, T>::value };
public:
	enum { value = temp == -1 ? -1 : 1 + temp };
};


// append
template<class TList, class T>
struct Append;

template<>
struct Append<NullType, NullType> {
	typedef NullType Result;
};

template<class T>
struct Append<NullType, T> {
	typedef TYPELIST_1(T) Result;
};

template<class Head, class Tail>
struct Append<NullType, Typelist<Head, Tail>> {
	typedef Typelist<Head, Tail> Result;
};

template<class Head, class Tail, class T>
struct Append<Typelist<Head, Tail>, T> {
	typedef Typelist<Head, typename Append<Tail, T>::Result> Result;
};

// erasing
template <class TList, class T>
struct Erase;

template <class T>
struct Erase<NullType, T> {
  typedef NullType Result;
};

template <class T, class Tail>
struct Erase<Typelist<T, Tail>, T>{
  typedef Tail Result;
};

template <class Head, class Tail, class T>
struct Erase<Typelist<Head, Tail>, T> {
  typedef Typelist<Head, typename Erase<Tail, T>::Result> Result;
};

// erasing all
template <class TList, class T>
struct EraseAll;

template <class T>
struct EraseAll<NullType, T> {
  typedef NullType Result;
};

template <class T, class Tail>
struct EraseAll<Typelist<T, Tail>, T> {
  // removing all occurences all the way down
  typedef typename EraseAll<Tail, T>::Result Result;
};

template <class Head, class Tail, class T>
struct EraseAll<Typelist<Head, Tail>, T> {
  typedef Typelist<Head, typename EraseAll<Tail, T>::Result> Result;
};


// erasing duplicates
template <class TList>
struct NoDuplicates;

template <>
struct NoDuplicates<NullType> {
	typedef NullType Result;
};

template<class Head, class Tail>
struct NoDuplicates<Typelist<Head, Tail>> {
private:
	typedef typename NoDuplicates<Tail>::Result L1;
	typedef typename Erase<L1, Head>::Result L2;
public:
	typedef Typelist<Head, L2> Result;
};

// replacing
template <class TList, class T, class U>
struct Replace;

template <class T, class U>
struct Replace<NullType, T, U> {
	typedef NullType Result;
};

template <class T, class Tail, class U>
struct Replace<Typelist<T, Tail>, T, U> {
	typedef Typelist<U, Tail> Result;
};

template <class Head, class Tail, class T, class U>
struct Replace<Typelist<Head, Tail>, T, U> {
	typedef Typelist<Head, typename Replace<Tail, T, U>::Result> Result;
};


// replace all
template <class TList, class T, class U>
struct ReplaceAll;

template <class T, class U>
struct ReplaceAll<NullType, T, U> {
	typedef NullType Result;
};

template <class T, class Tail, class U>
struct ReplaceAll<Typelist<T, Tail>, T, U> {
private:
	typedef typename ReplaceAll<Tail, T, U>::Result L1;
public:
	typedef Typelist<U, L1> Result;
};

template <class Head, class Tail, class T, class U>
struct ReplaceAll<Typelist<Head, Tail>, T, U> {
	typedef Typelist<Head, typename ReplaceAll<Tail, T, U>::Result> Result;
};


// Select type based on predicate
template <bool flag, typename T, typename U>
struct Select {
	typedef T Result;
};
template <typename T, typename U>
struct Select<false, T, U> {
        typedef U Result;
};

// most derived
template <class TList, class T>
struct MostDerived;

template <class T>
struct MostDerived<NullType, T> {
	typedef T Result;
};

template <class Head, class Tail, class T>
struct MostDerived<Typelist<Head, Tail>, T> {
private:
	typedef typename MostDerived<Tail, T>::Result Candidate;
public:
	typedef typename Select<SUPERSUBCLASS(Candidate, Head), Head, Candidate>::Result Result;
};


// derived to front
template <class T>
struct DerivedToFront;

template <>
struct DerivedToFront<NullType> {
	typedef NullType Result;
};

template <class Head, class Tail>
struct DerivedToFront<Typelist<Head, Tail>> {
private:
	typedef typename MostDerived<Tail, Head>::Result TheMostDerived;
	typedef typename Replace<Tail, TheMostDerived, Head>::Result Temp;
	typedef typename DerivedToFront<Temp>::Result L;

public:
	typedef Typelist<TheMostDerived, L> Result;

};



// Generating Scattered Hierarchies

template <class TList, template <class> class Unit>
class GenScatterHierarchy;

template <class Head, class Tail, template <class> class Unit>
class GenScatterHierarchy<Typelist<Head, Tail>, Unit>
  : public GenScatterHierarchy<Head, Unit>,
    public GenScatterHierarchy<Tail, Unit> {
public:
  typedef Typelist<Head, Tail> TList;
  typedef GenScatterHierarchy<Head, Unit> LeftBase;
  typedef GenScatterHierarchy<Tail, Unit> RightBase;

  template <typename T>
  struct Rebind {
      typedef Unit<T> Result;
  };
};

template <class AtomicType, template <class> class Unit>
class GenScatterHierarchy : public Unit<AtomicType> {
  typedef Unit<AtomicType> LeftBase;

  template <typename T>
  struct Rebind {
      typedef Unit<T> Result;
  };
};

template <template <class> class Unit>
class GenScatterHierarchy<NullType, Unit> {

  template <typename T>
  struct Rebind {
      typedef Unit<T> Result;
  };
};

template <class T, class H>
typename H::template Rebind<T>::Result& Field(H& obj) {
    return obj;
}




// Index based access
template <class H, unsigned int i> struct FieldHelper;
    
template <class H>
struct FieldHelper<H, 0>
{
        typedef typename H::TList::Head ElementType;
        typedef typename H::template Rebind<ElementType>::Result ResultType;
        
        static ResultType& Do(H& obj)
        {
            typename H::LeftBase& leftBase = obj;
            return leftBase;
        }
};

template <class H, unsigned int i>
struct FieldHelper
{
        typedef typename TypeAt<typename H::TList, i>::Result ElementType;
        typedef typename H::template Rebind<ElementType>::Result ResultType;
        
        static ResultType& Do(H& obj)
        {
    	typename H::RightBase& rightBase = obj;
            return FieldHelper<typename H::RightBase, i - 1>::Do(rightBase);
        }
    };

template <int i, class H>
typename FieldHelper<H, i>::ResultType&
Field(H& obj) {
    return FieldHelper<H, i>::Do(obj);
}



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
typedef GenScatterHierarchy<TYPELIST_4(int, int, int, std::string), Holder> WidgetInfoMultipleInts;



int main(void) {
  // scattered hierarchy
  WidgetInfo obj;
  std::string name = (static_cast<Holder<std::string>&>(obj)).value_;
  name = Field<std::string>(obj).value_;
  int i = Field<int>(obj).value_;
  (void)i;

  WidgetInfoMultipleInts multipleInts;
  //int i = Field<int>(multipleInts).value_; // this is not working: ambiguity

  int x = Field<0>(multipleInts).value_; // first int
//  int y = Field<1>(multipleInts).value_; // second int

  return 0;
}

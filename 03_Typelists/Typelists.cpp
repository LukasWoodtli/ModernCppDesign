
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
static_assert(is_same<TypeAt<SignedIntegrals, 1>::Result, short int>::value, "Wrong type at index");

// this gives: "error: implicit instantiation of undefined template 'TypeAt<NullType, 2>'"
// meaning "out of bound"
//typedef typename TypeAt<SignedIntegrals, 6>::Result, short int>::value newType,

int main(void) {
  return 0;
}

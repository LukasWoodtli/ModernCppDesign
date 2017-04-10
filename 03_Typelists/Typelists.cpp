
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




int main(void) {
  return 0;
}

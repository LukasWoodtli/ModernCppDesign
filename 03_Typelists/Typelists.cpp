

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


// test
typedef TYPELIST_4(signed char, short int, int, long int) SignedIntegrals;
static_assert(4 == Length<SignedIntegrals>::value, "Length of typelist not correct");

int main(void) {
  return 0;
}

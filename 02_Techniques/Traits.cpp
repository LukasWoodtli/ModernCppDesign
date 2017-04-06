#include <cstdlib>
#include <vector>
#include <iostream>

#include "Int2Type.h"

class NullType {
private:
  NullType();
};

template <typename T>
class TypeTraits {

private:
  template <class U>
  struct PointerTraits {
    enum { result = false };
    typedef NullType PointeeType;
  };

  template <class U>
  struct PointerTraits<U*> {
    enum { result = true };
    typedef U PointeeType;
  };

  template <class U>
  struct UnConst {
    typedef U Result;
  };

  template <class U>
  struct UnConst<const U> {
    typedef U Result;
  };

public:
  typedef typename PointerTraits<T>::PointeeType PointeeType;

  typedef typename UnConst<T>::Result NonConstType;

  enum { isPointer = PointerTraits<T>::result };
  /*typedef typename Select<isStdArith || is Pointer || isMemberPointer,
                            T, ReferredType&>::Result ParameterType;*/
};


// using TypeTraits
enum CopyAlgoSelector { Conservative, Fast };

// Conservative (works for any type)
template <typename InIt, typename OutIt>
OutIt CopyImpl(InIt first, InIt last, OutIt result, Int2Type<Conservative>) {
  // for (; first != last; ++first, ++result) *result = *first;
  (void)first;
  (void)last;
  std::cout << "Copying conservative\n";
  return result;
}

// Fast (using special library function for some types)
template <typename InIt, typename OutIt>
OutIt CopyImpl(InIt first, InIt last, OutIt result, Int2Type<Fast>) {
  // ... BitBlast(...); ...
  (void)first;
  (void)last;
  std::cout << "Copying fast\n";
  return result;
}

template <typename InIt, typename OutIt>
OutIt Copy(InIt first, InIt last, OutIt result) {
  typedef typename TypeTraits<InIt>::PointeeType SrcPointee;
  typedef typename TypeTraits<OutIt>::PointeeType DestPointee;

  enum { copyAlgo =
    TypeTraits<InIt>::isPointer &&
    TypeTraits<OutIt>::isPointer &&
    /*TypeTraits<SrcPointee>::isStdFundamental &&
    TypeTraits<DestPointee>::isStdFundamental &&
    ... */
    sizeof(SrcPointee) == sizeof(DestPointee) ?
    Fast :
    Conservative };

    return CopyImpl(first, last, result, Int2Type<copyAlgo>());
}



template<typename A, typename B>
void TEST_ASSERT(const A &a, const B &b) { if (a != b) abort(); }

int main(void) {

  TEST_ASSERT(TypeTraits<std::vector<int>::iterator>::isPointer, false);

  // using TypeTraits
  int * ptr1 = 0;
  int * ptr2 = 0;
  unsigned int * r = 0;
  Copy(ptr1, ptr2, r);

  std::vector<int> v1;
  std::vector<int> v2;
Copy(v1.begin(), v1.end(), v1.begin()); // back inserter...

  return 0;
}

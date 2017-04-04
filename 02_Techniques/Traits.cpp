#include <cstdlib>
#include <vector>

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

public:
  typedef typename PointerTraits<T>::PointeeType PointeeType;
  enum { isPointer = PointerTraits<T>::result };
  /*typedef typename Select<isStdArith || is Pointer || isMemberPointer,
                            T, ReferredType&>::Result ParameterType;*/
};


template<typename A, typename B>
void TEST_ASSERT(const A &a, const B &b) { if (a != b) abort(); }

int main(void) {

  TEST_ASSERT(TypeTraits<std::vector<int>::iterator>::isPointer, false);

  return 0;
}

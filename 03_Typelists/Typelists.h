#ifndef __TYPELISTS_H__
#define __TYPELISTS_H__

#include <string>

#include "../02_Techniques/Convertibility.h"
#include "../02_Techniques/Type2Type.h"
#include "../02_Techniques/Int2Type.h"


class NullType {};
class EmptyType {};

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
template <class TList, unsigned int index, class Default = NullType>
struct TypeAtNonStrict {
  typedef Default Result;
};

template <class Head, class Tail, class Default>
struct TypeAtNonStrict<Typelist<Head, Tail>, 0, Default> {
  typedef Head Result;
};

template <class Head, class Tail, unsigned int i, class Default>
struct TypeAtNonStrict<Typelist<Head, Tail>, i, Default> {
  typedef typename TypeAtNonStrict<Tail, i - 1, Default>::Result Result;
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


// reverse
template <class TList>
struct Reverse;

template <>
struct Reverse<NullType> {
  typedef NullType Result;
};

template <class Head, class Tail>
struct Reverse<Typelist<Head, Tail>> {
  typedef typename Append<
    typename Reverse<Tail>::Result, Head>::Result Result;
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
template <class H, typename R>
inline R& FieldHelper(H& obj, Type2Type<R>, Int2Type<0>) {
  typename H::LeftBase& subobj = obj;
  return subobj;
}

template <class H, typename R, int i>
inline R& FieldHelper(H& obj, Type2Type<R> tt, Int2Type<i>) {
  typename H::RightBase& subobj = obj;
  return FieldHelper(subobj, tt, Int2Type<i - 1>());
}

//...


// GenLinearHierarchy
template<class TList, 
	template <class AtomicType, class Base> class Unit,
	class Root = EmptyType>
class GenLinearHierarchy;

template<class T1,
         class T2,
         template <class, class> class Unit,
         class Root>
class GenLinearHierarchy<Typelist<T1, T2>, Unit, Root> : public Unit<T1, GenLinearHierarchy<T2, Unit, Root>>
{
};

template<class T,
         template <class, class> class Unit,
         class Root>
class GenLinearHierarchy<TYPELIST_1(T), Unit, Root> : public Unit<T, Root>
{
};

#endif /* __TYPELISTS_H__ */

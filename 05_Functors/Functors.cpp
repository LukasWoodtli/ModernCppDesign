#include "../03_Typelists/Typelists.h"


// FunctorImpl defines a polymorphic interface that abstracts a function call.
template <typename ResultType, class TList>
class FunctorImpl;

template <typename R>
class FunctorImpl<R, NullType> {
public:
    virtual R operator() () = 0;
    virtual FunctorImpl* Clone() const = 0;
    virtual ~FunctorImpl() {}
};

template <typename R, typename P1>
class FunctorImpl<R, TYPELIST_1(P1)> {
public:
    virtual R operator() (P1) = 0;
    virtual FunctorImpl* Clone() const = 0;
    virtual ~FunctorImpl() {}
};

template <typename R, typename P1, typename P2>
class FunctorImpl<R, TYPELIST_2(P1, P2)> {
public:
    virtual R operator() (P1, P2) = 0;
    virtual FunctorImpl* Clone() const = 0;
    virtual ~FunctorImpl() {}
};

// ... in C++11 and onward variadic templates should be used


template <typename ResultType, class TList>
class Functor  {
private:
    typedef TList PAramList;
    typedef typename TypeAtNonStrict<TList, 0, EmptyType>::Result Param1;
    typedef typename TypeAtNonStrict<TList, 1, EmptyType>::Result Param2;
    typedef typename TypeAtNonStrict<TList, 2, EmptyType>::Result Param3;
    // ...
    typedef FunctorImpl<ResultType, TList> Impl;
public:
    Functor();
    Functor(const Functor&);
    Functor& operator=(const Functor&);
    explicit Functor(std::unique_ptr<Impl> upImpl);

    template <class Fun>
    Functor(const Fun& fun);


    /* implementing all of these overloaded functions work due to a trick:
    "The trick relies on the fact that C++ does not instantiate member 
    functions for templates until they are actually used. Until you call
    the wrong operator(), the compiler doesn’t complain. If you try 
    to call an overload of operator() that doesn’t make sense, the compiler
    tries to generate the body of operator() and discovers the mismatch." 
    - Chapter 5.5. Implementing the Forwarding Functor::operator() */
    ResultType operator()() {
        return (*upImpl_)();
    }

    ResultType operator()(Param1 p1) {
        return (*upImpl_)(p1);
    }

    ResultType operator()(Param1 p1, Param2 p2) {
        return (*upImpl_)(p1, p2);
    }

    // ...
private:
    std::unique_ptr<Impl> upImpl_;
};

int main(void) {

    //Functor<double, TYPELIST_2(int, double)> myFunctor;
    //double result = myFunctor(4, 5.6);
    // this should not work: double result = myFunctor();
    return 0;
}

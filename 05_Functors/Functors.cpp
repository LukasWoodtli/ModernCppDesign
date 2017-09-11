#include <memory>
#include <iostream>

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


template <typename R, class TList>
class Functor  {
public:
    typedef R ResultType;
    typedef TList ParamList;
    typedef typename TypeAtNonStrict<TList, 0, EmptyType>::Result Param1;
    typedef typename TypeAtNonStrict<TList, 1, EmptyType>::Result Param2;
    typedef typename TypeAtNonStrict<TList, 2, EmptyType>::Result Param3;
    typedef typename TypeAtNonStrict<TList, 3, EmptyType>::Result Param4;
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



template <class ParentFunctor, typename Fun>
class FunctorHandler
    : public FunctorImpl<typename ParentFunctor::ResultType,
                         typename ParentFunctor::ParamList> {

public:
    typedef typename ParentFunctor::ResultType ResultType;

    FunctorHandler(const Fun& fun) : fun_(fun) {}
    FunctorHandler* Clone() const {
        return new FunctorHandler(*this);
    }

    ResultType operator() () {
        return fun_();
    }

    ResultType operator() (typename ParentFunctor::Param1 p1) {
        return fun_(p1);
    }

    ResultType operator() (typename ParentFunctor::Param1 p1,
                           typename ParentFunctor::Param2 p2) {
        return fun_(p1, p2);
    }

    // ...

private:
    Fun fun_;
};


// two template parameter sets needed here
template <typename R, class TList> // stands for the class template Functor
template <typename Fun> // stands for the parameter that the constructor itself takes.
Functor<R, TList>::Functor(const Fun& fun)
    : upImpl_(new FunctorHandler<Functor, Fun>(fun)) {

}




// Tests ////////////////////////////////////////////
struct TestFunctor {
    void operator()(int i, double d) {
        std::cout << "TestFunctor::operator()(" << i
            << ", " << d << ") called.\n";
    }
};

void TestFunction(int i, double d) {
    std::cout << "TestFuncton(" << i
    << ", " << d << ") called.\n";

}


void TestFunctionOverloaded(int i, double d) {
    std::cout << "TestFunctionOverloaded(" << i
    << ", " << d << ") called.\n";

}

void TestFunctionOverloaded(int i, int i2) {
    std::cout << "TestFunctionOverloaded(" << i
    << ", " << i2 << ") called.\n";

}

int main(void) {
    
    // functors
    TestFunctor f;
    Functor<void, TYPELIST_2(int, double)> cmd(f);
    cmd(4, 4.5);
    //cmd(23); // this doesn't work: to few arguments


    // free functions (not overloaded)
    Functor<void, TYPELIST_2(int, double)> cmd2(&TestFunction);
    cmd2(4, 4.5);
    
    
    // free functions (overloaded)
    //Functor<void, TYPELIST_2(int, double)> cmd3(&TestFunctionOverloaded); // this doesn't work: ambigous due to overload
    
    typedef void (*TpFun)(int, double);
    // Method 1: initialization
    TpFun pF = TestFunctionOverloaded;
    Functor<void, TYPELIST_2(int, double)> cmd3_1(pF);
    cmd3_1(4, 4.5);

    // Method 2: cast
    Functor<void, TYPELIST_2(int, double)> cmd3_2(static_cast<TpFun>(TestFunctionOverloaded));
    cmd3_2(4, 4.5);

    return 0;
}
#include <memory>
#include <iostream>

#include "../03_Typelists/Typelists.h"

template <typename R>
struct FunctorImplBase {
    typedef R ResultType;

    virtual FunctorImplBase* DoClone() const = 0;

    template<class U>
    static U* Clone(U* pObj) {
        if (!pObj) return NULL;
        U* pClone = static_cast<U*>(pObj->DoClone());
        return pClone;
    }
};

// FunctorImpl defines a polymorphic interface that abstracts a function call.
template <typename ResultType, class TList>
class FunctorImpl;

template <typename R>
class FunctorImpl<R, NullType> : public FunctorImplBase<R> {
public:
    virtual R operator() () = 0;
    virtual ~FunctorImpl() {}
};

template <typename R, typename P1>
class FunctorImpl<R, TYPELIST_1(P1)> : public FunctorImplBase<R> {
public:
    virtual R operator() (P1) = 0;
    virtual ~FunctorImpl() {}
};

template <typename R, typename P1, typename P2>
class FunctorImpl<R, TYPELIST_2(P1, P2)> : public FunctorImplBase<R>  {
public:
    virtual R operator() (P1, P2) = 0;
    virtual ~FunctorImpl() {}
};

// ... in C++11 and onward variadic templates should be used


template <typename R, class TList = NullType>
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
    
    Functor(const Functor& rhs) : upImpl_(Impl::Clone(rhs.upImpl_.get()))
    {}
    
    Functor& operator=(const Functor&);

    Functor(std::unique_ptr<Impl> upImpl) : upImpl_(std::move(upImpl)){}

    template <class Fun>
    Functor(const Fun& fun);

    template <class PtrObj, typename MemFn>
    Functor(const PtrObj& p, MemFn memFn);

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

    virtual FunctorHandler* DoClone() const { return new FunctorHandler(*this); }

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



template <class ParentFunctor, typename PointerToObj, typename PointerToMemFn>
class MemFunHandler : public FunctorImpl <typename ParentFunctor::ResultType,
                                   typename ParentFunctor::ParamList>
{
public:
    typedef typename ParentFunctor::ResultType ResultType;

    MemFunHandler(const PointerToObj& pObj, PointerToMemFn pMemFn)
        : pObj_(pObj), pMemFn_(pMemFn) {}

    virtual MemFunHandler* DoClone() const { return new MemFunHandler(*this); }
    
    ResultType operator()() {
        return ((*pObj_).*pMemFn_)();
    }

    ResultType operator()(typename ParentFunctor::Param1 p1) {
        return ((*pObj_).*pMemFn_)(p1);
    }

    ResultType operator()(typename ParentFunctor::Param1 p1,
                          typename ParentFunctor::Param2 p2) {
        return ((*pObj_).*pMemFn_)(p1, p2);
    }

private:
    PointerToObj pObj_;
    PointerToMemFn pMemFn_;
};


template <typename R, class TList> // stands for the class template Functor
template <class PtrObj, typename MemFn>
Functor<R, TList>::Functor(const PtrObj& p, MemFn memFn)
    : upImpl_(new MemFunHandler<Functor, PtrObj, MemFn>(p, memFn))
{}


// Binding

template <class Incoming>
class BinderFirst : public FunctorImpl<typename Incoming::ResultType,
                                       typename Incoming::ParamList::Tail> {

    typedef Functor<typename Incoming::ResultType,
                    typename Incoming::ParamList::Tail> Outgoing;
    typedef typename Incoming::Param1 Bound;
    typedef typename Incoming::ResultType ResultType;

public:
    BinderFirst(const Incoming& fun, Bound bound)
        : fun_(fun), bound_(bound) {}


    virtual BinderFirst* DoClone() const { return new BinderFirst(*this); }

    ResultType operator()() {
        return fun_(bound_);
    }
    
    ResultType operator()(typename Outgoing::Param1 p1) {
        return fun_(bound_, p1);
    }

    ResultType operator()(typename Outgoing::Param1 p1,
                          typename Outgoing::Param2 p2) {
        return fun_(bound_, p1, p2);
    }

    ResultType operator()(typename Outgoing::Param1 p1,
                          typename Outgoing::Param2 p2,
                          typename Outgoing::Param3 p3) {
        return fun_(bound_, p1, p2, p3);
}

private:
    Incoming fun_;
    Bound bound_;
};


template <class Fctor> struct BinderFirstTraits;

template <typename R, class TList>
struct BinderFirstTraits<Functor<R, TList>> {
    typedef typename Erase<TList, 
        typename TypeAt<TList, 0>::Result>::Result ParmList;
        typedef Functor<R, ParmList> BoundFunctorType;
        typedef typename BoundFunctorType::Impl Impl;
};        


template <class Fctor>
typename BinderFirstTraits<Fctor>::BoundFunctorType
BindFirst(const Fctor& fun, typename Fctor::Param1 bound) {
    typedef typename BinderFirstTraits<Fctor>::BoundFunctorType Outgoing;
    return Outgoing(std::unique_ptr<typename Outgoing::Impl>(
        new BinderFirst<Fctor>(fun, bound)));
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


const char * TestFunctionConversion(double, double) {
   static const char buffer[] = "Hello, world!";
   return buffer;
}


class Parrot {
public:
    void Eat() {
        std::cout << "Tsk, knick, tsk...\n";
    }

    void Speak() {
        std::cout << "Oh Captain, my Captain!\n";
    }
};


const char* Fun(int i, int j) {
    std::cout << "Fun(" << i << "," << j <<") called\n";
    return "0";
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


    // automatic conversion
    Functor<std::string, TYPELIST_2(int, int)> cmd4(&TestFunctionConversion);
    std::cout << cmd4(10, 10).substr(7) << std::endl; // call substing to show that it's a std::string


    // Pointers to memberfunctions (general)
    typedef void (Parrot::* TpMemFun)();
    TpMemFun pActivity = &Parrot::Eat;

    Parrot geronimo;
    Parrot* pGeronimo = &geronimo;

    // invoke (pointer)
    (pGeronimo->*pActivity)();
    // change activity
    pActivity = &Parrot::Speak;
    // invoke (pointer)
    (pGeronimo->*pActivity)();
    // invoke (object)
    (geronimo.*pActivity)();


    // Functor with pointer to member function
    Functor<void> cmd5_1(&geronimo, &Parrot::Eat), cmd5_2(&geronimo, &Parrot::Speak);
    // invoke
    cmd5_1();
    cmd5_2();


    // binding
    Functor<const char*, TYPELIST_2(char, int)> f1(&Fun);
    Functor<std::string, TYPELIST_1(double)> f2(BindFirst(f1, 10));
    f2(15);

    return 0;
}

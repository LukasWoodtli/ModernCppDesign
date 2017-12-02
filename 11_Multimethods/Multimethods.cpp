#include "../03_Typelists/Typelists.h"

#include <vector>
#include <iostream>

namespace Private
{
    template <class SomeLhs, class SomeRhs, 
		class Executor, typename ResultType>
    struct InvocationTraits
    {
        static ResultType 
    DoDispatch(SomeLhs& lhs, SomeRhs& rhs, 
		Executor& exec, Int2Type<false>)
        {
            return exec.Fire(lhs, rhs);
        }
        static ResultType 
    DoDispatch(SomeLhs& lhs, SomeRhs& rhs, 
		Executor& exec, Int2Type<true>)
        {
            return exec.Fire(rhs, lhs);
        }
    };
}

template<class Executor, class BaseLhs, class TypesLhs,
    class BaseRhs = BaseLhs, class TypesRhs = TypesLhs,
    typename ResultType = void>
class StaticDispatcher {

    template <class SomeLhs>
    static void DispatchRhs(SomeLhs& lhs, BaseRhs& rhs, Executor exec, NullType) {
        return exec.OnError(lhs, rhs);
    }

    template <class Head, class Tail, class SomeLhs>
    static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
                                  Executor exec, Typelist<Head, Tail>) {

        if (Head* p2 = dynamic_cast<Head*>(&rhs)) {
            Int2Type<(int(IndexOf<TypesRhs, Head>::value) <
                      int(IndexOf<TypesLhs, SomeLhs>::value))> i2t;

            typedef Private::InvocationTraits<
                    SomeLhs, Head, Executor, ResultType> CallTraits;
            
            return CallTraits::DoDispatch(lhs, *p2, exec, i2t);
        }

        return DispatchRhs(lhs, rhs, exec, Tail());
    }

    static ResultType DispatchLhs(BaseLhs& lhs, BaseRhs& rhs,
        Executor exec, NullType) {
            return exec.OnError(lhs, rhs);
    }

    template <class Head, class Tail>
    static ResultType DispatchLhs(BaseLhs& lhs, BaseRhs& rhs,
        Executor exec, Typelist<Head, Tail>) {
            if (Head* p1 = dynamic_cast<Head*>(&lhs)) {
                return DispatchRhs(*p1, rhs, exec, TypesRhs());
            }

            return DispatchLhs(lhs, rhs, exec, Tail());
    }

public:
    static ResultType Go(BaseLhs& lhs, BaseRhs& rhs, Executor exec) {
        return DispatchLhs(lhs, rhs, exec, TypesLhs());
    }
};


// Basic Fast Dispatcher

#define IMPLEMENT_INDEXABLE_CLASS(SomeClass) \
    static int& GetClassIndexStatic() \
        { static int index = -1; return index; } \
    virutal int& GetClassIndex() \
        { assert(typeid(*this) == typeid(SomeClass)); \
          return GetClassIndexStatic(); }


template <class BaseLhs, class BaseRhs = BaseLhs,
            typename ResultType = void,
            typename CallbackType = ResultType (*)(BaseLhs&, BaseRhs&)>
class BasicFastDispatcher {
    typedef std::vector<CallbackType> Row;
    typedef std::vector<Row> Matrix;
    Matrix callbacks_;
    int nextIndex_;

public:
    BasicFastDispatcher() : nextIndex_(0) {}

    template <class SomeLhs, class SomeRhs>
    void Add(CallbackType pFun) {
        int& idxLhs = SomeLhs::GetClassIndexStatic();
        if (idxLhs < 0) {
            callbacks_.resize(++nextIndex_);
            idxLhs = callbacks_.size() - 1;
        }
        else if (callbacks_.size() <= idxLhs) {
            callbacks_.resize(idxLhs + 1);
        }

        Row& thisRow = callbacks_[idxLhs];
        int& idxRhs = SomeRhs::GetClassIndexStatic();
        if (idxRhs < 0) {
            thisRow.resize(++nextIndex_);
            idxRhs = thisRow.size() - 1;
        }
        else if (thisRow.size() <= idxRhs) {
            thisRow.resize(idxRhs + 1);
        }
        thisRow[idxRhs] = pFun;
    }

    ResultType Go(BaseLhs& lhs, BaseRhs& rhs) {
        int& idxLhs = lhs.GetClassIndex();
        int& idxRhs = rhs.GetClassIndex();

        if (idxLhs < 0 || idxRhs < 0 ||
            idxLhs >= callbacks_.size() ||
            idxRhs >= callbacks_[idxLhs].size() ||
            callbacks_[idxLhs][idxRhs] == 0) {
                // ... error handling
            }

            return callbacks_[idxLhs][idxRhs](lhs, rhs);
    }
};




// Tests
class Shape {
public:
    virtual void print() { std::cout << "Shape\n"; }
};

class Rectangle : public Shape {
public:
    virtual void print() { std::cout << "Rectangle\n"; }
};
class Ellipse : public Shape {
public:
    virtual void print() { std::cout << "Ellipse\n"; }
};
class Poly : public Shape {
public:
    virtual void print() { std::cout << "Poly\n"; }
    
};


class HatchingExecutor {
public:
    void Fire(Rectangle&, Rectangle&) { std::cout << "Rectangle : Rectangle\n"; }
    void Fire(Rectangle&, Ellipse&) { std::cout << "Rectangle : Ellipse\n"; }
    void Fire(Rectangle&, Poly&) { std::cout << "Rectangle : Poly\n"; }
    void Fire(Ellipse&, Poly&) { std::cout << "Ellipse : Poly\n"; }
    void Fire(Ellipse&, Ellipse&) { std::cout << "Ellipse : Ellipse\n"; }
    void Fire(Poly&, Poly&) { std::cout << "Poly : Poly\n"; }

    void OnError(Shape&, Shape&) { std::cout << "ERROR!\n"; }
};

typedef StaticDispatcher<HatchingExecutor, Shape,
    TYPELIST_3(Rectangle, Ellipse, Poly)> Dispatcher;


int main() {
    Shape *p1 = new Rectangle();
    Shape *p2 = new Ellipse();

    HatchingExecutor exec;
    Dispatcher::Go(*p1, *p2, exec);

}

#include "../03_Typelists/Typelists.h"

#include <iostream>

class Shape {
    virtual void print() { std::cout << "Shape\n"; }
};

class Rectangle : public Shape {
    virtual void print() { std::cout << "Rectangle\n"; }
};
class Ellipse : public Shape {
    virtual void print() { std::cout << "Ellipse\n"; }
};
class Poly : public Shape {
    virtual void print() { std::cout << "Poly\n"; }
    
};

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


// Tests

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

#include <iostream>

template <class T>
class SmartPtr {
public:
    explicit SmartPtr(T* pointee) : pointee_(pointee) {
        // ...
    }

    SmartPtr& operator=(const SmartPtr& other);
    ~SmartPtr() {}

    T& operator*() const {
        // ...
        return *pointee_;
    }

    T* operator->() const {
        // ...
        return pointee_;
    }

private:
    T* pointee_;
};




// Tests //
class Widget {
public:
    void Fun() {std::cout << "Fun()\n";}
};



int main(void) {

    SmartPtr<Widget> sp(new Widget);
    sp->Fun();
    (*sp).Fun();
    return 0;
}

#pragma once


template <typename T>
struct Type2Type {
    typedef T OriginalType;
};

template <class T, class U>
T* Create3(const U& arg, Type2Type<T>) {
    return new T(arg);
}

template <class U>
Widget* Create3(const U& arg, Type2Type<Widget>) {
    return new Widget(arg, -1);
}

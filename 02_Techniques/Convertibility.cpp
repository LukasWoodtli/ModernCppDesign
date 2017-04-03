#include <iostream>
#include <vector>

template <class T, class U>
class Conversion {
	typedef char Small;
	class Big { char dummy[2]; };
	// these functions are all declared but not implemented
	static Small Test(const U&); // if conversion is possible this overloaded function is taken
	static Big Test(...); // if conversion is NOT possible this overloaded function is taken
	static T MakeT(); // needed in case T doesn't provide a default ctor

public:
	enum { exists = sizeof(Test(MakeT())) == sizeof(Small) };
	enum { sameType = false };
};

template <class T>
class Conversion<T, T> {

public:
	enum { exists = 1, sameType = 1  };
};


#define SUPERSUBCLASS(T, U) \
	(Conversion<const U*, const T*>::exists && \
	!Conversion<const T*, const void*>::sameType)

#define SUPERSUBCLASS_STRICT(T, U) \
	(SUPERSUBCLASS(T, U) && \
	!Conversion<const T*, const U*>::sameType)

// testing
class A {};
class B : public A {};
class C : public B {};

class U {};
class V : protected U {};
class W : private U {};

int main(void) {

	static_assert(Conversion<double, int>::exists == 1, "Types are not convertible");
	static_assert(Conversion<char, char*>::exists == 0, "Types are not convertible");
	static_assert(Conversion<size_t, std::vector<int>>::exists == 0, "Types are not convertible");


	static_assert(SUPERSUBCLASS(A, B) == true, "Types are not convertible");
	static_assert(SUPERSUBCLASS(A, C) == true, "Types are not convertible");

	static_assert(SUPERSUBCLASS(A, U) == false, "Types are not convertible");

	// these don't work due to nonpublic inheritance
	//static_assert(SUPERSUBCLASS(U, V) == false, "Types are not convertible");
	//static_assert(SUPERSUBCLASS(U, W) == false, "Types are not convertible");

	static_assert(SUPERSUBCLASS(A, A) == true, "Types are not convertible");
	static_assert(SUPERSUBCLASS_STRICT(A, A) == false, "Types are not convertible");

	return 0;
}

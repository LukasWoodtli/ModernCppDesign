#include <iostream>
#include <vector>

#include "Convertibility.h"

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

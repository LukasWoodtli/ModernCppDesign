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
};

// just for testing purposes
template <typename A, typename B>
void TEST_ASSERT(A a, B b) { if(a != b) exit(1); }

int main(void) {
	
	TEST_ASSERT(Conversion<double, int>::exists, 1);
	TEST_ASSERT(Conversion<char, char*>::exists, 0);
	TEST_ASSERT(Conversion<size_t, std::vector<int>>::exists, 0);
	
	return 0;
}


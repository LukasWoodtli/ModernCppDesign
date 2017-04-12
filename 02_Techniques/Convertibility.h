#pragma once

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



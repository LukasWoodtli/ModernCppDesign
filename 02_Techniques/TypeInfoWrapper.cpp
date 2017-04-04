#include <typeinfo>
#include <cstdlib>
#include <iostream>

class TypeInfo {
public:
	TypeInfo() : pInfo_(0) {} // needed for containers
	TypeInfo(const std::type_info& info) : pInfo_(&info) {}
	TypeInfo(const TypeInfo& other) {pInfo_ = other.pInfo_;}
	TypeInfo& operator=(const TypeInfo& other) {
		pInfo_ = other.pInfo_; 
		return *this;
	}

	// Compatibility functions
	bool before(const TypeInfo& rhs) const {
		return pInfo_->before(*(rhs.pInfo_));
	}

	const char* name() const { return pInfo_->name(); }

	friend bool operator==(const TypeInfo&, const TypeInfo&);

private:
	const std::type_info* pInfo_;
};

// Comparison operators
bool operator==(const TypeInfo& first, const TypeInfo& second) {
	return *(first.pInfo_) == *(second.pInfo_);
}
bool operator!=(const TypeInfo& first, const TypeInfo& second) {
	return !(first == second);
}

bool operator<(const TypeInfo& first, const TypeInfo& second) {
	return first.before(second);
	
}
bool operator<=(const TypeInfo& first, const TypeInfo& second) {
	return first < second || (first == second);
}

bool operator>(const TypeInfo& first, const TypeInfo& second) {
	return second.before(first);
}
bool operator>=(const TypeInfo& first, const TypeInfo& second) {
	return first > second || (first == second);
}




// tests

class Base { virtual void foo() {} }; // needs at leas one virtual function: http://en.cppreference.com/w/cpp/language/typeid
class Derived : public Base {};
class Other {};

template<typename A, typename B>
void TEST_ASSERT(const A& a, const B& b) { if (a != b) abort(); }

bool Fun(Base* pObj) {
	TypeInfo info = typeid(Derived);
	std::cout << info.name() << "\n";
	const auto& typeIdObj = typeid(*pObj);
	std::cout << typeIdObj.name() << "\n";
	if (typeIdObj == info) {
		return true;
	}
	
	return false;
}

int main(void) {

	TypeInfo typeInfo1;
	TypeInfo typeInfo2(typeid(Other));
	TypeInfo typeInfo3(typeInfo2);
	
	TEST_ASSERT(typeInfo2 == typeInfo3, true);
	TEST_ASSERT(typeInfo2 == typeInfo2, true);
	TEST_ASSERT(typeInfo2 != typeInfo3, false);
	
	TEST_ASSERT(typeInfo2 <= typeInfo3, true);
	TEST_ASSERT(typeInfo2 >= typeInfo3, true);

	
	
	TypeInfo typeInfoBase = typeid(Base);
	TypeInfo typeInfoOther = typeid(Other);

	TypeInfo &smaller = typeInfoBase < typeInfoOther ? typeInfoBase : typeInfoOther;
	TypeInfo &bigger  = typeInfoOther < typeInfoBase ? typeInfoBase : typeInfoOther;

	TEST_ASSERT(bigger == smaller, false);
	TEST_ASSERT(bigger != smaller, true);
	
	TEST_ASSERT(bigger < smaller, false);
	TEST_ASSERT(bigger <= smaller, false);
	
	TEST_ASSERT(bigger > smaller, true);
	TEST_ASSERT(bigger >= smaller, true);
	
	
	Base base;
	Derived derived;
	TEST_ASSERT(Fun(&base), false);
	TEST_ASSERT(Fun(&derived), true);

	return 0;
}
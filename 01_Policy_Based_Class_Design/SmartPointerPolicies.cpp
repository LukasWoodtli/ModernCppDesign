#include <exception>

template<class T,
	template <class> class CheckingPolicy,
	template <class> class ThreadingModel>
class SmartPtr : public CheckingPolicy<T>, public ThreadingModel<T> {
	// ...
	T* operator->() {
		typename ThreadingModel<SmartPtr>::Lock guard(*this);
		CheckingPolicy<T>::Check(pointee_);
		return pointee_;
	}
private:
	T* pointee_;
};

// checking policies
//
template <class T> struct NoChecking {
	static void Check(T*) {}
};

template <class T> struct EnforceNotNull {
	class NullPointerException : public std::exception { /* ... */};
	static void Check(T*) {
//		if (!ptr) throw NullPointerException();
	}
};

template <class T> struct EnsureNotNull {
private:
	// TODO don't return 0
	static T* GetDefaultValue() { return (T*)0; }
public:
	static void Check(T*& ptr) {
		if (!ptr) { ptr = GetDefaultValue(); }
	}
};



// TODO typedef SmartPtr<Widget, NoChecking, SingleThreaded> WidgetPtr;

int main(void) {

	return 0;
}


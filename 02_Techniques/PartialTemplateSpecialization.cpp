
class Window {};

// primary template
template <class T, class U> T Fun(U);

// overloading (legal
template <class T> T Fun(Window obj);

// this doesn't work: non-type partial specialization is not allowed
// template <class U> void Fun<void, U>(U obj);

int main(void) {

	
	return 0;
}
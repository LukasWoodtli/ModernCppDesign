#include <cstdlib>
#include <new>
#include <algorithm>
#include <cassert>

namespace Private {
class LifetimeTracker {
public:
	LifetimeTracker(unsigned int x) : longevity_(x) {}

	virtual ~LifetimeTracker() = 0;

	static bool Compare(unsigned int longevity,
				  const LifetimeTracker* p) {
		return p->longevity_ < longevity;
	}

private:
	unsigned int longevity_;
};

// Definition required
LifetimeTracker::~LifetimeTracker() {}



typedef LifetimeTracker** TrackerArray;
TrackerArray pTrackerArray;
unsigned int elements;

template <typename T>
struct Deleter {
    static void Delete(T* pObj) {
        delete pObj;
    }
};


template <typename T, typename Destroyer>
class ConcreteLifetimeTracker : public LifetimeTracker {
public:
    ConcreteLifetimeTracker(T* p, unsigned int longevity, Destroyer d)
        : LifetimeTracker(longevity), pTracked_(p), destroyer_(d)
    {}

    ~ConcreteLifetimeTracker() {
        destroyer_(pTracked_);
    }
    
private:
    T* pTracked_;
    Destroyer destroyer_;
};


void AtExitFn();



}


template <typename T, typename Destroyer>
void SetLongevity(T* pDynObject, unsigned int longevity,
		Destroyer d = Private::Deleter<T>::Delete) {
    
    using namespace Private;
            
	TrackerArray pNewArray = static_cast<TrackerArray>(
			std::realloc(pTrackerArray,
				sizeof(*pTrackerArray) * (elements + 1)));

	if (!pNewArray) throw std::bad_alloc();
	pTrackerArray = pNewArray;
	LifetimeTracker* p = new ConcreteLifetimeTracker<T, Destroyer>(pDynObject,
			longevity, d);
	TrackerArray pos = std::upper_bound(pTrackerArray, pTrackerArray + elements, longevity, LifetimeTracker::Compare);
	std::copy_backward(pos, pTrackerArray + elements, pTrackerArray + elements + 1);
	*pos = p;
    ++elements;

	std::atexit(Private::AtExitFn);
}


void Private::AtExitFn() {
    assert(elements > 0 && pTrackerArray != 0);
    LifetimeTracker* pTop = pTrackerArray[elements - 1];
    pTrackerArray = static_cast<TrackerArray>(
        std::realloc(pTrackerArray, sizeof(*pTrackerArray) * --elements));

        delete pTop;
}


class Singleton {
public:
    static Singleton& Instance() {
        if (!pInstance_) {
            if (destroyed_) {
                OnDeadReference();
            }
            else {
                Create();
            }
        }

        return *pInstance_;
    }

private:
    static void Create() {
        static Singleton theInstance;
        pInstance_ = &theInstance;
    }

    static void OnDeadReference() {
        Create();

        new (pInstance_) Singleton;
        atexit(KillPhoenixSingleton);

        destroyed_ = false;
    }

    static void KillPhoenixSingleton() {
        pInstance_->~Singleton();
    }

    virtual ~Singleton() {
        pInstance_ = 0;
        destroyed_ = true;
    }

    static Singleton* pInstance_;
    static bool destroyed_;
};

Singleton* Singleton::pInstance_ = 0;
bool Singleton::destroyed_ = false;


// Policy based SingletonHolder //

template <class T>
class SingleThreaded {
    //...
public:
    typedef T VolatileType; // no volatile keyword for single threaded policy
};

template <class T>
class MultiThreaded {
    //...
public:
    typedef volatile T VolatileType; // no volatile keyword for single threaded policy
};


template <class T>
class CreateUsingNew {
public:
    static T* Create() {
        return new T;
    }
};

template <class T>
class DefaultLifetime {

};

template <class T, template <class> class CreationPolicy = CreateUsingNew,
    template <class> class LifetimePolicy = DefaultLifetime,
    template <class> class ThreadingModel = SingleThreaded>
class SingletonHolder {
public:
    static T& Instance() {
        if (!pInstance_) {
            typename ThreadingModel<T>::Lock guard;
            /* double checked locking pattern (might not be safe on 
                some muliprocessor architectures) */
            if (!pInstance_) {
                if (destroyed_) {
                    LifetimePolicy<T>::OnDeadReference();
                    destroyed_ = false;
                }

                pInstance_ = CreationPolicy<T>::Create();
                LifetimePolicy<T>::ScheduleDestruction(&DestroySingleton);
            }
        }

        return *pInstance_;
    }

private:
    static void DestroySingleton() {
        assert(!destroyed_);
        CreationPolicy<T>::Destoy(pInstance_);
        pInstance_ = 0;
        destroyed_ = true;
    }

    SingletonHolder();

    typedef typename ThreadingModel<T>::VolatileType InstanceType;
    static InstanceType* pInstance_;
    static bool destroyed_;
};



/* The impletmentations in this file are not complete.
   See the code for the book (Loki) for more detail. */

int main(void) {

    return 0;
}

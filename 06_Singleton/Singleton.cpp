#include <cstdlib>
#include <new>
#include <algorithm>

namespace Private {
class LifetimeTracker {
public:
	LifetimeTracker(unsigned int x) : longevity_(x) {}

	virtual ~LifetimeTracker() = 0;

	friend inline bool Compare(unsigned int longevity,
				  const LifetimeTracker* p) {
		return p->longevity_ < longevity;
	}

private:
	unsigned int longevity_;
};

// Definition required
LifetimeTracker::~LifetimeTracker() {}



typedef LifetimeTracker** TrackerArray;
extern TrackerArray pTrackerArray;
extern unsigned int elements;

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

template <typename T, typename Destroyer>
void SetLongevity(T* pDynObject, unsigned int longevity,
		Destroyer d = Private::Deleter<T>::Delete) {
	
	TrackerArray pNewArray = static_cast<TrackerArray>(
			std::realloc(pTrackerArray,
				sizeof(*pTrackerArray) * (elements + 1)));

	if (!pNewArray) throw std::bad_alloc();
	pTrackerArray = pNewArray;
	LifetimeTracker* p = new ConcreteLifetimeTracker<T, Destroyer>(pDynObject,
			longevity, d);
	TrackerArray pos = std::upper_bound(pTrackerArray, pTrackerArray + elements, longevity, &LifetimeTracker::Compare);
	std::copy_backward(pos, pTrackerArray + elements, pTrackerArray + elements + 1);
	*pos = p;
	++elements;
	std::atexit(AtExitFn);
}

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

int main(void) {

    return 0;
}

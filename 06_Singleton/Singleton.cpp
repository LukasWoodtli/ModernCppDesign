#include <cstdlib>
#include <new>

namespace Private {
class LifetimeTracker {
public:
	LifetimeTracker(unsigned int x) : longevity_(x) {}

	virtual ~LifetimeTracker() = 0;

	friend inline bool Comare(unsigned int longevity,
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

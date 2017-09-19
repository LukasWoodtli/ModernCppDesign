#include <cstdlib>
#include <new>

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

#include <iostream>
#include <cassert>
#include <stdexcept>

struct NullPointerException : public std::runtime_error
{
    NullPointerException() : std::runtime_error("")
    { }
    const char* what() const throw()
    { return "Null Pointer Exception"; }
};

/// Ownership Policies /////////////////////

template <class P>
class RefCounted {
public:
    RefCounted() {
        // could use SmallObject here
        pCount_ = new unsigned int;
        *pCount_ = 1;
    }

    RefCounted(const RefCounted& rhs) : pCount_(rhs.pCount_) {}
    
    template <typename P1>
    RefCounted(const RefCounted<P1>& rhs)
        : pCount_(reinterpret_cast<const RefCounted&>(rhs).pCount_) {}

    P Clone(const P& val) {
        ++*pCount_;
        return val;
    }

    bool Release(const P&) {
        if (!--*pCount_) {
            delete pCount_;
            return true;
        }

        return false;
    }

    enum { destructiveCopy = false };

private:
    unsigned int* pCount_;
};


template <template <class> class ThreadingModel>
struct RefCountedMtAdj {
    template <class P>
    class RefCountedMT : public ThreadingModel<RefCountedMT<P>> {
        typedef ThreadingModel<RefCountedMT<P>> base_type;
        typedef typename base_type::IntType CountType;
        typedef volatile CountType *CountPtrType;

    public:
        RefCountedMT() {
            pCount_ = new CountPtrType;
            *pCount_ = 1;
        }

        RefCountedMT(const RefCountedMT& rhs) : pCount_(rhs.pCount_) {}
        
        template <typename P1>
        RefCountedMT(const RefCountedMT<P1>& rhs)
            : pCount_(reinterpret_cast<const RefCountedMT<P>&>(rhs).pCount_) {}

        P Clone(const P& val) {
            ThreadingModel<RefCountedMT>::AtomicIncrement(*pCount_);
            return val;
        }

        bool Release(const P&) {
            if (!ThreadingModel<RefCountedMT>::AtomicDecrement(pCount_)) {
                delete pCount_;
                return true;
            }

            return false;
        }

        enum { destructiveCopy = false };

    private:
        CountPtrType *pCount_;
    };
};

template <class P>
class COMRefCounted {
public:
    COMRefCounted() {}

    template <class U>
    COMRefCounted(const COMRefCounted<U>&) {}

    static P Clone (const P& val) {
        val->AddRef();
        return val;
    }

    static bool Release (const P& val) {
        val->Release();
        return false;
    }

    enum { destructiveCopy = false };
};

namespace Private {
    class RefLinkedBase {
        public:
            RefLinkedBase() {
                prev_ = next_ = this;
            }

            RefLinkedBase(const RefLinkedBase& rhs) {
                prev_ = &rhs;
                next_ = rhs.next_;
                
                prev_->next_ = this;
                next_->prev_ = this;
            }

            bool Release() {
                if (next_ == this) {
                    assert(prev_ == this);
                    return true;
                }

                prev_->next_ = next_;
                next_->prev_ = prev_;

                return false;
            }

            enum { destructiveCopy = false };

        private:
            mutable const RefLinkedBase* prev_;
            mutable const RefLinkedBase* next_;
    };
}

template <class P>
class RefLinked : public Private::RefLinkedBase {
public:
    RefLinked() {}

    template <class P1>
    RefLinked(const RefLinked<P1>& rhs) : Private::RefLinkedBase(rhs) {}

    static P Clone(const P& val) {
        return val;
    }

    bool Release(const P&) {
        return Private::RefLinkedBase::Release();
    }
};



template <class P>
class DestructiveCopy {
public:
    DestructiveCopy() {}

    template <class P1>
    DestructiveCopy(const DestructiveCopy<P1>&) {}

    template <class P1>
    static P Clone(P1& val) {
        P result(val);
        val = P1();
        return result;
    }

    static bool Release(const P&) {
        return true;
    }
    
    enum { destructiveCopy = true };
};

template <class P>
class NoCopy {
public:
    NoCopy() {}

    template <class P1>
    NoCopy(const NoCopy<P1>&) {}

    static P Clone(const P&) {
        // dependend on template parameter
        static const bool DependentFalse = sizeof(P*) == 0;
        static_assert(DependentFalse, "This Policy disallows value copying");
    }

    static bool Release(const P&) {
        return true;
    }

    enum { destructiveCopy = false };
};



/// Conversion Policies /////////////////////

struct AllowConversion {
    enum { allow = true };
};

struct DisallowConversion {

    DisallowConversion() {}

    DisallowConversion(const AllowConversion&) {}

    enum { allow = false };
};


/// Checking Policies /////////////////////


template <class P>
struct NoCheck {
    NoCheck() {}

    template <class P1>
    NoCheck(const NoCheck<P1>&) {}

    static void OnDefault(const P&) {}

    static void OnInit(const P&) {}

    static void OnDereference(const P&) {}
};

template <class P>
struct AssertCheck {
    AssertCheck() {}

    template <class P1>
    AssertCheck(const AssertCheck<P1>&) {}

    template <class P1>
    AssertCheck(const NoCheck<P1>&) {}

    static void OnDefault(const P&) {}

    static void OnInit(const P&) {}

    static void OnDereference(P val) {
        assert(val);
        (void)val;
    }
};



template <class P>
struct AssertCheckStrict {
    AssertCheckStrict() {}

    template <class P1>
    AssertCheckStrict(const AssertCheckStrict<P1>&) {}

    template <class P1>
    AssertCheckStrict(const AssertCheck<P1>&) {}

    template <class P1>
    AssertCheckStrict(const NoCheck<P1>&) {}

    static void OnDefault(const P& val) {
        assert(val);
        (void)val;
    }

    static void OnInit(const P& val) {
        assert(val);
        (void)val;
    
    }

    static void OnDereference(P val) {
        assert(val);
        (void)val;
    }
};





template <class P>
struct RejectNull {
    RejectNull() {}

    template <class P1>
    RejectNull(const RejectNull<P1>&) {}


    static void OnInit(const P& val) {
        if (!val) throw NullPointerException();
    }

    static void OnDefault(const P& val) {
        OnInit(val);
    }

    static void OnDereference(P val) {
        OnInit(val);
    }
};

/// Storage Policies /////////////////////

template <class T>
class DefaultSPStorage {
public:
    typedef T* StoredType;
    typedef T* PointerType;
    typedef T& ReferenceType;

    DefaultSPStorage() : pointee_(Default()) {}

    // initialized by ownership policy
    DefaultSPStorage(const DefaultSPStorage&) {}

    template <class U>
    DefaultSPStorage(const DefaultSPStorage<U>&) {}

    DefaultSPStorage(const StoredType& p) : pointee_(p) {}

    PointerType operator->() const {return pointee_;}

    ReferenceType operator*() const {return *pointee_;}

protected:
    void Destroy() {
        delete pointee_;
    }

    static StoredType Default() { return 0;}

private:
    StoredType pointee_;
};






/// Smart Pointer /////////////////////
template <typename T, 
          template <class> class OwnershipPolicy = RefCounted,
          class ConversionPolicy = DisallowConversion,
          template <class> class CheckingPolicy = AssertCheck,
          template <class> class StoragePolicy = DefaultSPStorage>
class SmartPtr {
public:
    explicit SmartPtr(T* pointee) : pointee_(pointee) {
        // ...
    }

    SmartPtr& operator=(const SmartPtr& other);
    ~SmartPtr() {}

    T& operator*() const {
        // ...
        return *pointee_;
    }

    T* operator->() const {
        // ...
        return pointee_;
    }

private:
    T* pointee_;
};



/// Tests /////////////////////
class Widget {
public:
    void Fun() {std::cout << "Fun()\n";}
};



int main(void) {

    SmartPtr<Widget> sp(new Widget);
    sp->Fun();
    (*sp).Fun();
    return 0;
}

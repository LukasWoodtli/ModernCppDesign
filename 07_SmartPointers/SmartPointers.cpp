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

template <class T>
struct MultiThreadedDummy {
};

template <template <class> class ThreadingModel>
struct RefCountedMTAdj {
    template <class P>
    class RefCountedMT : public ThreadingModel<RefCountedMT<P>> {
        typedef ThreadingModel<RefCountedMT<P>> base_type;
        //typedef typename base_type::IntType CountType;
	typedef unsigned int CountType;
        typedef volatile CountType *CountPtrType;

    public:
        RefCountedMT() {
            pCount_ = new CountType;
            *pCount_ = 1u;
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
        CountPtrType pCount_;
    };
};

template <class T>
using RefCountedMT =RefCountedMTAdj<MultiThreadedDummy>::RefCountedMT<T>;

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
        // TODO assert(val);
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

    PointerType GetImpl() { return pointee_; }

    StoredType GetImplRef() { return pointee_; }

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
class SmartPtr
	: public StoragePolicy<T>,
	  public OwnershipPolicy<typename StoragePolicy<T>::PointerType>,
	  public CheckingPolicy<typename StoragePolicy<T>::StoredType>,
	  public ConversionPolicy {

    typedef StoragePolicy<T> SP;
    typedef OwnershipPolicy<typename StoragePolicy<T>::PointerType> OP;
    typedef CheckingPolicy<typename StoragePolicy<T>::StoredType> KP;
    typedef ConversionPolicy CP;

public:

    typedef typename SP::PointerType PointerType;
    typedef typename SP::StoredType StoredType;
    typedef typename SP::ReferenceType ReferenceType;

private:
    typedef SmartPtr CopyArg;

    typedef const StoredType& ImplicitArg;

public:
    explicit SmartPtr(T* pointee) : SP(pointee) {
        KP::OnInit(SP::GetImpl());
    }

    SmartPtr() {
        KP::OnDefault(SP::GetImpl());
    }

    SmartPtr(CopyArg& rhs) : SP(rhs), OP(rhs), KP(rhs), CP(rhs) {
        SP::GetImplRef() = OP::Clone(rhs.GetImplRef());
    }


    SmartPtr& operator=(const SmartPtr& other);
    
    ~SmartPtr() { /* ... */ }

    /*
    ...
    */
};

/// Smart Pointer with Locking Proxie ////////////

template <class T>
class LockingProxy {
public:
    LockingProxy(T* pObj) : pointee_(pObj) {
        // TODO Do the locking here
        std::cout << "LockingProxy is locking pointee\n";
    }

    ~LockingProxy() {
        std::cout << "LockingProxy is un-locking pointee\n";
    }
    
    T* operator->() const {
        return pointee_;
    }

private:
    LockingProxy& operator=(const LockingProxy&);
    T* pointee_;
};


template <class T>
class SmartPtrWithLockingProxy {
    // ...
public:
    
    SmartPtrWithLockingProxy(T* obj) : pointee_(obj) {}
    
    LockingProxy<T> operator->() const {
        return LockingProxy<T>(pointee_);
    }
    
private:
    T* pointee_;
};



/// Tests /////////////////////
class Widget {
public:
    void Fun() {std::cout << "Fun()\n";}
};

class Button : public Widget { public: void print() {std::cout << "Button\n";}};
class Window : public Widget {};



typedef SmartPtr<Button, RefCounted, AllowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonRefCountedAllowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, RefCounted, AllowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonRefCountedAllowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, RefCounted, AllowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonRefCountedAllowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, RefCounted, AllowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonRefCountedAllowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, RefCounted, DisallowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonRefCountedDisallowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, RefCounted, DisallowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonRefCountedDisallowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, RefCounted, DisallowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonRefCountedDisallowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, RefCounted, DisallowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonRefCountedDisallowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, AllowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonRefCountedMTAllowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, AllowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonRefCountedMTAllowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, AllowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonRefCountedMTAllowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, AllowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonRefCountedMTAllowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, DisallowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonRefCountedMTDisallowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, DisallowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonRefCountedMTDisallowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, DisallowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonRefCountedMTDisallowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, RefCountedMT, DisallowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonRefCountedMTDisallowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, AllowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonCOMRefCountedAllowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, AllowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonCOMRefCountedAllowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, AllowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonCOMRefCountedAllowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, AllowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonCOMRefCountedAllowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, DisallowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonCOMRefCountedDisallowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, DisallowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonCOMRefCountedDisallowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, DisallowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonCOMRefCountedDisallowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, COMRefCounted, DisallowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonCOMRefCountedDisallowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, AllowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonRefLinkedAllowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, AllowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonRefLinkedAllowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, AllowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonRefLinkedAllowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, AllowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonRefLinkedAllowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, DisallowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonRefLinkedDisallowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, DisallowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonRefLinkedDisallowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, DisallowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonRefLinkedDisallowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, RefLinked, DisallowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonRefLinkedDisallowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, AllowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonDestructiveCopyAllowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, AllowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonDestructiveCopyAllowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, AllowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonDestructiveCopyAllowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, AllowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonDestructiveCopyAllowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, DisallowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonDestructiveCopyDisallowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, DisallowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonDestructiveCopyDisallowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, DisallowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonDestructiveCopyDisallowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, DestructiveCopy, DisallowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonDestructiveCopyDisallowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, AllowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonNoCopyAllowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, AllowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonNoCopyAllowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, AllowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonNoCopyAllowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, AllowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonNoCopyAllowConversionNoCheckDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, DisallowConversion, AssertCheck, DefaultSPStorage> SmartPtrButtonNoCopyDisallowConversionAssertCheckDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, DisallowConversion, AssertCheckStrict, DefaultSPStorage> SmartPtrButtonNoCopyDisallowConversionAssertCheckStrictDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, DisallowConversion, RejectNull, DefaultSPStorage> SmartPtrButtonNoCopyDisallowConversionRejectNullDefaultSPStorage;
typedef SmartPtr<Button, NoCopy, DisallowConversion, NoCheck, DefaultSPStorage> SmartPtrButtonNoCopyDisallowConversionNoCheckDefaultSPStorage;


int main(void) {

    SmartPtr<Widget> sp(new Widget);
    sp->Fun();
    (*sp).Fun();
    
    
    SmartPtrWithLockingProxy<Button> pointerWithLocking(new Button);
    // C++ is calling operator-> until it reaches a pointer
    pointerWithLocking->print();
    
    

    SmartPtrButtonRefCountedAllowConversionAssertCheckDefaultSPStorage smartPtrButtonRefCountedAllowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonRefCountedAllowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonRefCountedAllowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonRefCountedAllowConversionRejectNullDefaultSPStorage smartPtrButtonRefCountedAllowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonRefCountedAllowConversionNoCheckDefaultSPStorage smartPtrButtonRefCountedAllowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonRefCountedDisallowConversionAssertCheckDefaultSPStorage smartPtrButtonRefCountedDisallowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonRefCountedDisallowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonRefCountedDisallowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonRefCountedDisallowConversionRejectNullDefaultSPStorage smartPtrButtonRefCountedDisallowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonRefCountedDisallowConversionNoCheckDefaultSPStorage smartPtrButtonRefCountedDisallowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonRefCountedMTAllowConversionAssertCheckDefaultSPStorage smartPtrButtonRefCountedMTAllowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonRefCountedMTAllowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonRefCountedMTAllowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonRefCountedMTAllowConversionRejectNullDefaultSPStorage smartPtrButtonRefCountedMTAllowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonRefCountedMTAllowConversionNoCheckDefaultSPStorage smartPtrButtonRefCountedMTAllowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonRefCountedMTDisallowConversionAssertCheckDefaultSPStorage smartPtrButtonRefCountedMTDisallowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonRefCountedMTDisallowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonRefCountedMTDisallowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonRefCountedMTDisallowConversionRejectNullDefaultSPStorage smartPtrButtonRefCountedMTDisallowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonRefCountedMTDisallowConversionNoCheckDefaultSPStorage smartPtrButtonRefCountedMTDisallowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonCOMRefCountedAllowConversionAssertCheckDefaultSPStorage smartPtrButtonCOMRefCountedAllowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonCOMRefCountedAllowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonCOMRefCountedAllowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonCOMRefCountedAllowConversionRejectNullDefaultSPStorage smartPtrButtonCOMRefCountedAllowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonCOMRefCountedAllowConversionNoCheckDefaultSPStorage smartPtrButtonCOMRefCountedAllowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonCOMRefCountedDisallowConversionAssertCheckDefaultSPStorage smartPtrButtonCOMRefCountedDisallowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonCOMRefCountedDisallowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonCOMRefCountedDisallowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonCOMRefCountedDisallowConversionRejectNullDefaultSPStorage smartPtrButtonCOMRefCountedDisallowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonCOMRefCountedDisallowConversionNoCheckDefaultSPStorage smartPtrButtonCOMRefCountedDisallowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonRefLinkedAllowConversionAssertCheckDefaultSPStorage smartPtrButtonRefLinkedAllowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonRefLinkedAllowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonRefLinkedAllowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonRefLinkedAllowConversionRejectNullDefaultSPStorage smartPtrButtonRefLinkedAllowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonRefLinkedAllowConversionNoCheckDefaultSPStorage smartPtrButtonRefLinkedAllowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonRefLinkedDisallowConversionAssertCheckDefaultSPStorage smartPtrButtonRefLinkedDisallowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonRefLinkedDisallowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonRefLinkedDisallowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonRefLinkedDisallowConversionRejectNullDefaultSPStorage smartPtrButtonRefLinkedDisallowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonRefLinkedDisallowConversionNoCheckDefaultSPStorage smartPtrButtonRefLinkedDisallowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonDestructiveCopyAllowConversionAssertCheckDefaultSPStorage smartPtrButtonDestructiveCopyAllowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonDestructiveCopyAllowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonDestructiveCopyAllowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonDestructiveCopyAllowConversionRejectNullDefaultSPStorage smartPtrButtonDestructiveCopyAllowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonDestructiveCopyAllowConversionNoCheckDefaultSPStorage smartPtrButtonDestructiveCopyAllowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonDestructiveCopyDisallowConversionAssertCheckDefaultSPStorage smartPtrButtonDestructiveCopyDisallowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonDestructiveCopyDisallowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonDestructiveCopyDisallowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonDestructiveCopyDisallowConversionRejectNullDefaultSPStorage smartPtrButtonDestructiveCopyDisallowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonDestructiveCopyDisallowConversionNoCheckDefaultSPStorage smartPtrButtonDestructiveCopyDisallowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonNoCopyAllowConversionAssertCheckDefaultSPStorage smartPtrButtonNoCopyAllowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonNoCopyAllowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonNoCopyAllowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonNoCopyAllowConversionRejectNullDefaultSPStorage smartPtrButtonNoCopyAllowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonNoCopyAllowConversionNoCheckDefaultSPStorage smartPtrButtonNoCopyAllowConversionNoCheckDefaultSPStorage;

    SmartPtrButtonNoCopyDisallowConversionAssertCheckDefaultSPStorage smartPtrButtonNoCopyDisallowConversionAssertCheckDefaultSPStorage;

    SmartPtrButtonNoCopyDisallowConversionAssertCheckStrictDefaultSPStorage smartPtrButtonNoCopyDisallowConversionAssertCheckStrictDefaultSPStorage;

    SmartPtrButtonNoCopyDisallowConversionRejectNullDefaultSPStorage smartPtrButtonNoCopyDisallowConversionRejectNullDefaultSPStorage(new Button);

    SmartPtrButtonNoCopyDisallowConversionNoCheckDefaultSPStorage smartPtrButtonNoCopyDisallowConversionNoCheckDefaultSPStorage;



    return 0;
}


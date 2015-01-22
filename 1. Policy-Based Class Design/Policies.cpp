

class Widget {};
class Gadget {};


template <class T>
struct OpNewCreator
{
  static T Create()
  {
    return new T;
  }
protected:
  ~OpNewCreator() {}
};


template <class T>
struct MallocCreator
{
  static T Create()
  {
    void *buf = std::malloc(sizeof(T));
    if (!buf) return 0;
    return new (buf) T;
  }
protected:
  ~MallocCreator() {} // All policy classes need to have protected non-virtual dtors. So the host classes can't be deleted with a policy pointer to host class.
};

template <class T>
struct PrototypeCreator
{
  PrototypeCreator(T pObj = 0)
    : pPrototype_(pObj) {}
  
  T Create()
  {
    return pPrototype_ ? pPrototype_->Clone() : 0;
  }

  T GetPrototype() {return pPrototype_;}
  void SetPrototype(T pObj) { pPrototype_ = pObj;}
protected:
  ~PrototypeCreator() {}
private:
  T pPrototype_;
};


// Library Code
template <template <class Created> class CreationPolicy = OpNewCreator> // Created can be omitted
class WidgetManager : public CreationPolicy<Widget>
{
public:
  void DoSomething() {
    // due to template template parameters we can also use the same creation policy for gadgets here
    Gadget *pw = CreationPolicy<Gadget>().Create();
  }

private:

};
/* You cannot use Created inside WidgetManagerit is a formal argument for CreationPolicy 
   (not WidgetManager) and can be simply omitted . */



// Application code
typedef WidgetManager<MallocCreator> MyWidgetManager;

int main()
{
  WidgetManager<> widgetManager1;
  MyWidgetManager * widgetManager2 = new MyWidgetManager();
  MallocCreator<Widget> * castedWidgetManager2 = dynamic_cast<MallocCreator<Widget>*>(widgetManager2);
  // not possible when dtor of MallocCreator is protected: delete castedWidgetManager2;
  // but this works
  delete widgetManager2;

  return 0;
}
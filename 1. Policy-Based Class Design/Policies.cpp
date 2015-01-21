

class Widget {};
class Gadget {};


template <class T>
struct OpNewCreator
{
  static T Create()
  {
    return new T;
  }
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
private:
  T pPrototype_;
};


// Library Code
template <template <class Created> class CreationPolicy> // Created can be omitted
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
typedef WidgetManager<OpNewCreator> MyWidgetManager;

int main()
{
  MyWidgetManager widgetManager;
  return 0;
}
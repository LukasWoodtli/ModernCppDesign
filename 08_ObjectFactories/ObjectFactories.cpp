#include <map>
#include <cassert>
#include <string>
#include <iostream>

class Shape {
public:
    virtual void draw() = 0;
};

class Line : public Shape {
public:
    virtual void draw() { std::cout << "Line\n"; }
};


class ShapeFactory {
public:
    typedef Shape* (*CreateShapeCallback)();
    
private:
    ShapeFactory() {}
    typedef std::map<int, CreateShapeCallback> CallbackMap;

public:
    bool RegisterShape(int shapeId, CreateShapeCallback createFn);
    bool UnregisterShape(int shapeId);
        
    Shape* CreateShape(int shapeId);
    
    static ShapeFactory* Instance() {
        if (! factory_) factory_ = new ShapeFactory;
        return factory_;
    }
        
    
private:
    CallbackMap callbacks_;
    static ShapeFactory* factory_;
};

ShapeFactory* ShapeFactory::factory_ = NULL;

bool ShapeFactory::RegisterShape(int shapeId, CreateShapeCallback createFn) {
    return callbacks_.insert(CallbackMap::value_type(shapeId, createFn)).second;
}

Shape* ShapeFactory::CreateShape(int shapeId) {
    CallbackMap::const_iterator i = callbacks_.find(shapeId);
    
    if (i == callbacks_.end()) {
        // not found
        throw std::runtime_error("Unknown Shape ID");
    }
    
    // invoke creation function
    return (i->second)();
}
        


namespace {
    Shape* CreateLine() {
        return new Line;
    }
    
    const int LINE = 1;
    
    const bool registered =
        ShapeFactory::Instance()->RegisterShape(LINE, CreateLine);
}



//// Generic Factory /////////////////////////////////


template<class IdentifierType, class ProductType>
class DefaultFactoryError {
public:
    class Exception : public std::exception {
    public:
        Exception(const IdentifierType& unknownId)
            : unknownId_(unknownId) {}

        virtual const char* what() const noexcept override {
            return "Unknown object type passed to Factory.";
        }

        const IdentifierType& GetID() {
            return unknownId_;
        }

    private:
        IdentifierType unknownId_;
    };

protected:
    static ProductType* OnUnknownType(const IdentifierType& id) {
        throw Exception(id);
    }
};

template<class AbstractProduct, typename IdentifierType, 
    typename ProductCreator = AbstractProduct* (*)(),
    // pointer to function with no param and returning pointer to AbstractProduct
    template<typename, class> class FactoryErrorPolicy = DefaultFactoryError>
class Factory : public FactoryErrorPolicy<IdentifierType, AbstractProduct> {
public:
    bool Register(const IdentifierType& id, ProductCreator creator) {
        return associations_.insert(typename AssocMap::value_type(id, creator)).second;
    }

    bool Unregister(const IdentifierType& id) {
        return associations_.erase(id) == 1;
    }

    AbstractProduct* CreateObject(const IdentifierType& id) {
        typename AssocMap::const_iterator i =
            associations_.find(id);
        
        if (i != associations_.end()) {
            return (i->second)();
        }

        return FactoryErrorPolicy<IdentifierType, AbstractProduct>::OnUnknownType(id);
    }

private:
    typedef std::map<IdentifierType, ProductCreator> AssocMap;
    AssocMap associations_;
};


int main(void) {
    Factory<Shape, std::string> shapeFactory;
    bool success = shapeFactory.Register("Line", []() -> Shape* {return new Line;});
    assert(success);
    
    Shape* shape = shapeFactory.CreateObject("Line");
    shape->draw();
    
    success = shapeFactory.Unregister("Line");
    assert(success);
    
    
    (void)success;
}

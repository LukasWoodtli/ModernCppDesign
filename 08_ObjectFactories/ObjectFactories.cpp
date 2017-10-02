#include <map>
#include <cassert>

class Shape {
};

class Line : public Shape {
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


int main(void) {
    
}

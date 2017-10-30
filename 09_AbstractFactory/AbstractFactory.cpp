#include "../03_Typelists/Typelists.h"

#include <iostream>


class Enemy { public: virtual void print() = 0; };

class Soldier : public Enemy {};
class SillySoldier : public Soldier { public: virtual void print() { std::cout << "SillySoldier\n"; } };
class BadSoldier : public Soldier { public: virtual void print() { std::cout << "BadSoldier\n"; } };

class Monster : public Enemy {};
class SillyMonster : public Monster { public: virtual void print() { std::cout << "SillyMonster\n"; } };
class BadMonster : public Monster { public: virtual void print() { std::cout << "BadMonster\n"; } };

class SuperMonster : public Enemy {};
class SillySuperMonster : public SuperMonster { public: virtual void print() { std::cout << "SillySuperMonster\n"; } };
class BadSuperMonster : public SuperMonster { public: virtual void print() { std::cout << "BadSuperMonster\n"; } };

namespace traditional_implementation {
class AbstractEnemyFactory {
public:
    virtual Soldier* MakeSoldier() = 0;
    virtual Monster* MakeMonster() = 0;
    virtual SuperMonster* MakeSuperMonster() = 0;
};


class EasyLevelEnemyFactory : public AbstractEnemyFactory {
public:
    virtual Soldier* MakeSoldier() {
        return new SillySoldier;
    }
    
    virtual Monster* MakeMonster() {
        return new SillyMonster;
    }
    
    virtual SuperMonster* MakeSuperMonster() {
        return new SillySuperMonster;
    }    
};


class DieHardLevelEnemyFactory : public AbstractEnemyFactory {
public:
    virtual Soldier* MakeSoldier() {
        return new BadSoldier;
    }
    
    virtual Monster* MakeMonster() {
        return new BadMonster;
    }
    
    virtual SuperMonster* MakeSuperMonster() {
        return new BadSuperMonster;
    }
    
};


class GameApp {
public:
    void SelectLevel(bool easy) {
        if (easy)
            pFactory_ = new EasyLevelEnemyFactory;
        else
            pFactory_ = new DieHardLevelEnemyFactory;
    }
    
    AbstractEnemyFactory* getFactory() const {
        return pFactory_;
    }
    
private:
    AbstractEnemyFactory* pFactory_;

};

}

//// Generic Implementation ////////////////////////////

template <class T>
class AbstractFactoryUnit {
public:
    virtual T* DoCreate(Type2Type<T>) = 0;
    virtual ~AbstractFactoryUnit() {}
};

template <class TList, template <class> class Unit = AbstractFactoryUnit>
class AbstractFactory : public GenScatterHierarchy<TList, Unit> {
public:
    typedef TList ProductList;
    template <class T> T* Create() {
        Unit<T>& unit = *this;
        return unit.DoCreate(Type2Type<T>());
    }
};


template <class ConcreteProduct, class Base>
class OpNewFactoryUnit : public Base {
    typedef typename Base::ProductList BaseProductList;
protected:
    typedef typename BaseProductList::Tail ProductList;
public:
    typedef typename BaseProductList::Head AbstractProduct;

    ConcreteProduct* DoCreate(Type2Type<AbstractProduct>) {
        return new ConcreteProduct;
    }
};


template<class AbstractFact,
    template<class, class> class Creator = OpNewFactoryUnit,
    class TList = typename AbstractFact::ProductList>
class ConcreteFactory : public GenLinearHierarchy<
    typename Reverse<TList>::Result, Creator, AbstractFact> {
public:
    typedef typename AbstractFact::ProductList ProductList;
    typedef TList ConcreteProductList;
};



typedef AbstractFactory<TYPELIST_3(Soldier, Monster, SuperMonster)> AbstractEnemyFactory;
typedef ConcreteFactory<AbstractEnemyFactory, OpNewFactoryUnit,
    TYPELIST_3(SillySoldier, SillyMonster, SillySuperMonster)> EasyLevelEnemyFactory;
typedef ConcreteFactory<AbstractEnemyFactory, OpNewFactoryUnit,
  TYPELIST_3(BadSoldier, BadMonster, BadSuperMonster)> DieHardLevelEnemyFactory;

class GameApp {
public:
    void SelectLevel(bool easy) {
        if (easy)
            pFactory_ = new EasyLevelEnemyFactory;
        else
            pFactory_ = new DieHardLevelEnemyFactory;
    }

    AbstractEnemyFactory* getFactory() const {
        return pFactory_;
    }

private:
    AbstractEnemyFactory* pFactory_;

};


//// Tests //////////////////////////////////////


void traditionalImplementaionTest() {
    // traditional implementation
    traditional_implementation::GameApp gameApp;

    gameApp.SelectLevel(true);
    traditional_implementation::AbstractEnemyFactory* factory = gameApp.getFactory();
    
    Soldier* soldier = factory->MakeSoldier();
    soldier->print();
    
    Monster* monster = factory->MakeMonster();
    monster->print();

    SuperMonster* superMonster = factory->MakeSuperMonster();
    superMonster->print();
    
    
    gameApp.SelectLevel(false);
    factory = gameApp.getFactory();
    
    soldier = factory->MakeSoldier();
    soldier->print();
    
    monster = factory->MakeMonster();
    monster->print();

    superMonster = factory->MakeSuperMonster();
    superMonster->print();
}


void genericImplementationTest() {
    // Modern C++
    GameApp modernGameApp;

    modernGameApp.SelectLevel(true);
    AbstractEnemyFactory* modernFactory = modernGameApp.getFactory();
    
    Soldier* soldier = modernFactory->Create<Soldier>();
    soldier->print();
    
    Monster* monster = modernFactory->Create<Monster>();
    monster->print();

    SuperMonster* superMonster = modernFactory->Create<SuperMonster>();
    superMonster->print();
    

    modernGameApp.SelectLevel(false);
    modernFactory = modernGameApp.getFactory();
    
    soldier = modernFactory->Create<Soldier>();
    soldier->print();
    
    monster = modernFactory->Create<Monster>();
    monster->print();

    superMonster = modernFactory->Create<SuperMonster>();
    superMonster->print();
}

int main() {
  traditionalImplementaionTest();
  genericImplementationTest();
}

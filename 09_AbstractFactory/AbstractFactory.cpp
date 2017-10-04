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




//// Tests //////////////////////////////////////

typedef AbstractFactory<TYPELIST_3(Soldier, Monster, SuperMonster)> GenericAbstractEnemyFactory;

int main() {
    GameApp gameApp;
    
    
    gameApp.SelectLevel(true);
    AbstractEnemyFactory* factory = gameApp.getFactory();
    
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

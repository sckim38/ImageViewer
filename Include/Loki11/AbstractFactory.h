#pragma once

#include "HierarchyGenerators.h"

#include <cassert>

namespace Loki11
{
    template <typename T>
    struct Type2Type
    {
        typedef T OriginalType;
    };

    template <class T>
    class AbstractFactoryUnit
    {
    public:
        virtual ~AbstractFactoryUnit() {}
        virtual T* DoCreate(Type2Type<T>) = 0;
    };

    template <class ...TList>
    class AbstractFactory : public GenScatterHierarchy<AbstractFactoryUnit, TList...>
    {
    public:
        static const std::size_t N = sizeof...(TList);

        template <class T> T* Create()
        {
            AbstractFactoryUnit<T>& unit = *this;
            return unit.DoCreate(Type2Type<T>());
        }
    };

    template <class ConcreteProduct, class Base>
    class OpNewFactoryUnit : public Base
    {
    protected:
        static const std::size_t N = Base::N - 1;

    public:
        using AbstractProduct = typename Base::template TypeAt<N>::Type;
        ConcreteProduct* DoCreate(Type2Type<AbstractProduct>)
        {
            return new ConcreteProduct;
        }
    };

    template <class AbstractFact, class ...TList>
    using ConcreteFactory = GenLinearHierarchy<OpNewFactoryUnit, AbstractFact, TList...>;

} // namespace Loki11

///////////////////////////////////////////////////////////////////////////////
// AbstractFactoryTest
///////////////////////////////////////////////////////////////////////////////

class Soldier { public: virtual ~Soldier() {} };
class Monster { public: virtual ~Monster() {} };
class SuperMonster { public: virtual ~SuperMonster() {} };

class SillySoldier : public Soldier {};
class SillyMonster : public Monster {};
class SillySuperMonster : public SuperMonster {};

class BadSoldier : public Soldier {};
class BadMonster : public Monster {};
class BadSuperMonster : public SuperMonster {};

typedef Loki11::AbstractFactory<Soldier, Monster, SuperMonster> AbstractEnemyFactory;
typedef Loki11::ConcreteFactory<AbstractEnemyFactory, SillySoldier, SillyMonster, SillySuperMonster> EasyLevelEnemyFactory;
typedef Loki11::ConcreteFactory<AbstractEnemyFactory, BadSoldier, BadMonster, BadSuperMonster > HardLevelEnemyFactory;

inline void abstractFactoryTest()
{
    using namespace Loki11;

    bool r;

    std::shared_ptr<AbstractEnemyFactory> easyFactory(new EasyLevelEnemyFactory);
    std::shared_ptr<AbstractEnemyFactory> hardFactory(new HardLevelEnemyFactory);

    Soldier *s;

    s = easyFactory->Create<Soldier>();

    r = !!(typeid(*s) == typeid(SillySoldier)); //SGB !! eliminates bool-to-int performance warning

    delete s;

    s = hardFactory->Create<Soldier>(); //BCB bug!!! - always creates SillySoldier

    r = r && typeid(*s) == typeid(BadSoldier);

    delete s;
}

#pragma once

#include <type_traits>

namespace Loki11
{
#if defined(_MSC_VER) && _MSC_VER >= 1300
#pragma warning( push ) 
    // 'class1' : base-class 'class2' is already a base-class of 'class3'
#pragma warning( disable : 4584 )
#endif // _MSC_VER

    template<unsigned int N, class ...TList>
    struct TypeAt;

    template<unsigned int N, class T1, class... T2>
    struct TypeAt<N, T1, T2...>
    {
        using Type = typename TypeAt<N - 1, T2...>::Type;
    };

    template<class T1, class... T2>
    struct TypeAt<0, T1, T2...>
    {
        using Type = T1;
    };

    template < template <class> class Unit, class ...TList>
    class GenScatterHierarchy{};

    template <template <class > class Unit, class T1, class... T2 >
    class GenScatterHierarchy<Unit, T1, T2...>
        : public Unit<T1>
        , public GenScatterHierarchy<Unit, T2...> // 
    {
    public:
        template<std::size_t N>
        using TypeAt = typename Loki11::TypeAt<N, T1, T2...>;
        // Insure that LeftBase is unique and therefore reachable
        using LeftBase = Unit<T1>;
        using RightBase = typename GenScatterHierarchy<Unit, T2...>;
        template <typename T> struct Rebind
        {
            using Type = Unit<T>;
        };
    };

    template <class T, class H>
    typename H::template Rebind<T>::Type& Field(H& obj)
    {
        return obj;
    }

    template <class T, class H>
    const typename H::template Rebind<T>::Type& Field(const H& obj)
    {
        return obj;
    }

    template <class T>
    struct TupleUnit
    {
        T value_;
        operator T&() { return value_; }
        operator const T&() const { return value_; }
    };

    template <class ...TList>
    using Tuple = GenScatterHierarchy<TupleUnit, TList...>;

    template <class H, std::size_t N> struct FieldHelper;

    template <class H>
    struct FieldHelper<H, 0>
    {
        using ElementType = typename H::template TypeAt<0>::Type;// Head;
        using UnitType = typename H::template Rebind<ElementType>::Type;

        enum
        {
            isTuple = std::is_same<UnitType, TupleUnit<ElementType>>::value,
            isConst = std::is_const<H>::value
        };

        using LeftBase = typename std::_If<isConst, const typename H::LeftBase,typename H::LeftBase>::type;
        using UnqualifiedResultType = typename std::_If<isTuple, ElementType, UnitType>::type;
        using ResultType = typename std::_If<isConst, const UnqualifiedResultType, UnqualifiedResultType>::type;

        static ResultType& Do(H& obj)
        {
            LeftBase& leftBase = obj;
            return leftBase;
        }
    };

    template <class H, std::size_t N>
    struct FieldHelper
    {
        using ElementType = typename H::template TypeAt<N>::Type;
        using UnitType = typename H::template Rebind<ElementType>::Type;

        enum
        {
            isTuple = std::is_same<UnitType, TupleUnit<ElementType>>::value,
            isConst = std::is_const<H>::value
        };

        using RightBase = typename std::_If<isConst, const typename H::RightBase, typename H::RightBase>::type;
        using UnqualifiedResultType = typename std::_If<isTuple, ElementType, UnitType>::type;
        using ResultType = typename std::_If<isConst, const UnqualifiedResultType, UnqualifiedResultType>::type;

        static ResultType& Do(H& obj)
        {
            RightBase& rightBase = obj;
            return FieldHelper<RightBase, N - 1>::Do(rightBase);
        }
    };

    template <std::size_t N, class H>
    typename FieldHelper<H, N>::ResultType&
        Field(H& obj)
    {
        return FieldHelper<H, N>::Do(obj);
    }

    //    template <int i, class H>
    //    const typename FieldHelper<H, i>::ResultType&
    //    Field(const H& obj)
    //    {
    //        return FieldHelper<H, i>::Do(obj);
    //    }

    inline void TupleTest()
    {
        using Point3D = Tuple<int, int, int>;
        Point3D pt;

        Field<0>(pt) = 0;
        Field<1>(pt) = 100;
        Field<2>(pt) = 300;

        TRACE(_T("TupleTest:%d, %d, %d\n"), Field<0>(pt), Field<1>(pt), Field<2>(pt));
        //Field<INT>(pt).value_ = 400;
    }

    template <template <class AtomicType, class Base> class Unit, class Root, class... TList>
    class GenLinearHierarchy : public Root {};

    template <template <class, class> class Unit, class Root, class T1, class... T2>
    class GenLinearHierarchy<Unit, Root, T1, T2...>
        : public Unit<T1, GenLinearHierarchy<Unit, Root, T2...>>
    {
    };

#if defined(_MSC_VER) && _MSC_VER >= 1300
#pragma warning( pop ) 
#endif
}   // namespace Loki


#pragma once

#include "HierarchyGenerators.h"

namespace Loki11
{
    class BaseVisitor
    {
    public:
        virtual ~BaseVisitor() {}
    };

    template<class T, typename R, bool is_const>
    struct BaseVisitorUnit
    {
        typedef R ReturnType;
        using ParamType = typename std::_If<is_const, const T, T>::type;
        virtual ~BaseVisitorUnit() {}
        virtual ReturnType Visit(ParamType &) = 0;
    };

    template<typename R, bool is_const>
    struct VisitorHelper
    {
        template<typename T>
        using Type = BaseVisitorUnit<T, R, is_const>;
    };

    template<template<class> class Unit, class ...TList>
    class Visitor
        : public GenScatterHierarchy<Unit, TList...>
    {
    };

    template <typename R, class ...TList>
    class CyclicVisitor : public Visitor<VisitorHelper<R, false>::Type, TList...>
    {
    public:
        typedef R ReturnType;

        template <class T>
        ReturnType GenericVisit(T& host)
        {
            using UnitType = typename VisitorHelper<R, false>::Type<T>;
            UnitType& subObj = *this;

            return subObj.Visit(host);
        }
    };
#define LOKI_DEFINE_CYCLIC_VISITABLE(SomeVisitor) \
    SomeVisitor::ReturnType Accept(SomeVisitor& guest) \
            { return guest.GenericVisit(*this); } 

} // namespace Loki11

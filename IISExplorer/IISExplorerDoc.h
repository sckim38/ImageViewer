
// IISExplorerDoc.h : CIISExplorerDoc 클래스의 인터페이스
//

#pragma once


#ifndef HAS_DESTRUCTOR_H
#define HAS_DESTRUCTOR_H

#include <type_traits>
#include <iostream>
#include <vector>

#ifdef _MSC_VER
namespace std {
    template <typename T>
    typename add_rvalue_reference<T>::type declval();
}
#endif

/*! The template `has_destructor<T>` exports a
boolean constant `value that is true iff `T` has
a public destructor.

N.B. A compile error will occur if T has non-public destructor.
*/
template< typename T>
struct has_destructor
{
    /* Has destructor :) */
    template <typename A>
    static std::true_type test(decltype(std::declval<A>().~A()) *) {
        return std::true_type();
    }

    /* Has no destructor :( */
    template<typename A>
    static std::false_type test(...) {
        return std::false_type();
    }

    /* This will be either `std::true_type` or `std::false_type` */
    typedef decltype(test<T>(0)) type;

    static const bool value = type::value; /* Which is it? */
};

#endif // EOF


template <class T, class Enable = void>
struct is_defined
{
    static const bool value = false;
};

template <class T>
struct is_defined<T, std::enable_if_t<(sizeof(T) > 0)>>
{
    static const bool value = true;
};




#include <iostream>

class bar {}; // Defined
template<
    class CharT,
    class Traits
> class basic_iostream; //Defined
template<typename T>
struct vector; //Undefined
class foo; // Undefined

inline int has_dtor_test()
{
    std::cout << has_destructor<bar>::value << std::endl;
    std::cout << has_destructor<std::basic_iostream<char>>::value
        << std::endl;
    std::cout << has_destructor<foo>::value << std::endl;
    std::cout << has_destructor<vector<int>>::value << std::endl;
    std::cout << has_destructor<int>::value << std::endl;
    std::cout << std::has_trivial_destructor<int>::value << std::endl;
    return 0;
}

class ITest;
class ITest2;

template<typename T>
struct Tester
{
    std::enable_if_t<std::is_same<T, ITest>::value>
    Test(T *pTest)
    {
        pTest->Call();
    }
    std::enable_if_t<std::is_same<T, ITest2>::value>
    Test(T *pTest)
    {
        pTest->Call(sdfad);
    }
};

template<typename U, bool>
struct Tester1
{
    template<typename T = U,
        typename = std::enable_if_t < is_defined<ITest>::value >>
        void Test(ITest *)
    {
        ITest test;
        ITest::aaa();
    }
};

template<typename U>
struct Tester1<U, false>
{
    template<typename T = U,
    typename = std::enable_if_t<!is_defined<ITest>::value>>
    void Test(ITest2 *)
    {
        //typename ITest2::aaa();
    }

};

template<typename T>
struct Tester2
{
    template<typename U = T,
        typename = std::enable_if_t < is_defined<U>::value >>
        void Test()
    {
        U::aaa();
    }
};

enum class iis_type
{
    iis_type,
    aoi_type
};

const iis_type iis_class_type = iis_type::aoi_type;

template<typename T, iis_type = iis_type::iis_type>
struct Tester3
{
    void Test()
    {
        T::aaa();
    }
};

template<typename T>
struct Tester3<T, iis_type::aoi_type>
{
    void Test()
    {
    }
};

struct NoExist;
struct dummy{};

template<typename T>
struct Tester3Type
{
    using type = Tester3<std::_If<iis_class_type == iis_type::iis_type, NoExist, dummy>::type, iis_class_type>;
};

const Tester3Type<dummy>::type abcd;

template<typename T>
struct Tester4Type
{
    using type = Tester3<std::_If<iis_class_type == iis_type::aoi_type, NoExist, dummy>::type, iis_class_type>;
};


template<typename T>
struct Tester5Type
{
    using base = std::_If<iis_class_type == iis_type::aoi_type, NoExist, dummy>::type;
    //using type = Tester3<base, iis_type::iis_type>;
    using type = Tester3<base, iis_type::aoi_type>;
};

inline void test4()
{
    Tester4Type<dummy>::type abcd4;
    abcd4.Test();
}


inline void test5()
{
    Tester5Type<dummy>::type abcd5;
    abcd5.Test();
}


template <typename T> struct Model
{
    vector<T> vertices;

    void transform(Gdiplus::Matrix m)
    {
        for (auto &&vertex : vertices)
        {
            vertex.pos = m * vertex.pos;
            modifyNormal(vertex, m, special_());
        }
    }

private:

    struct general_ {};
    struct special_ : general_ {};
    template<typename> struct int_ { typedef int type; };

    template<typename Lhs, typename Rhs,
        typename int_<decltype(Lhs::normal)>::type = 0>
        void modifyNormal(Lhs &&lhs, Rhs &&rhs, special_) {
        lhs.normal = rhs * lhs.normal;
    }

    template<typename Lhs, typename Rhs>
    void modifyNormal(Lhs &&lhs, Rhs &&rhs, general_) {
        // do nothing
    }
};



struct Foo {
    size_t length() { return 5; }
};

struct Bar {
    void length();
};

//template <typename R, bool result = std::is_same<decltype(((R*)nullptr)->length()), size_t>::value>
//const bool hasLengthHelper(int) {
//    return result;
//}
//
//template <typename R>
//const bool hasLengthHelper(...) { return false; }
//
//template <typename R>
//const bool hasLength() {
//    return hasLengthHelper<R>(0);
//}
//
//// function is only valid if `.length()` is present, with return type `size_t`
//template <typename R>
//typename std::enable_if<hasLength<R>(), size_t>::type lengthOf(R r) {
//    return r.length();
//}

//int main111() {
//    std::cout <<
//        hasLength<Foo>() << "; " <<
//        hasLength<std::vector<int>>() << "; " <<
//        hasLength<Bar>() << ";" <<
//        lengthOf(Foo()) <<
//        std::endl;
//    // 1; 0; 0; 5
//
//    return 0;
//}



//
//namespace detail {
//    template <class Default, class AlwaysVoid,
//        template<class...> class Op, class... Args>
//    struct detector {
//        using value_t = std::false_type;
//        using type = Default;
//    };
//
//    template <class Default, template<class...> class Op, class... Args>
//    struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
//        // Note that std::void_t is a C++17 feature
//        using value_t = std::true_type;
//        using type = Op<Args...>;
//    };
//
//} // namespace detail
//
//
//
//template <template<class...> class Op, class... Args>
//using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;
//
//template <template<class...> class Op, class... Args>
//using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;
//
//template <class Default, template<class...> class Op, class... Args>
//using detected_or = detail::detector<Default, void, Op, Args...>;
//



//#include <experimental/type_traits>
//#include <cstddef>
//
//template<class T>
//using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());
//
//struct Meow { };
//struct Purr { void operator=(const Purr&) = delete; };
//
//static_assert(std::experimental::is_detected<copy_assign_t, Meow>::value,
//    "Meow should be copy assignable!");
//static_assert(!std::experimental::is_detected_v<copy_assign_t, Purr>,
//    "Purr should not be copy assignable!");
//static_assert(std::experimental::is_detected_exact_v<Meow&, copy_assign_t, Meow>,
//    "Copy assignment of Meow should return Meow&!");
//
//template<class T>
//using diff_t = typename T::difference_type;
//
//template <class Ptr>
//using difference_type = std::experimental::detected_or_t<std::ptrdiff_t, diff_t, Ptr>;
//
//struct Woof { using difference_type = int; };
//struct Bark {};
//
//static_assert(std::is_same<difference_type<Woof>, int>::value,
//    "Woof's difference_type should be int!");
//static_assert(std::is_same<difference_type<Bark>, std::ptrdiff_t>::value,
//    "Bark's difference_type should be ptrdiff_t!");
//



class CIISExplorerDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CIISExplorerDoc();
	DECLARE_DYNCREATE(CIISExplorerDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CIISExplorerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};




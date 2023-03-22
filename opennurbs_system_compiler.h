//
// Copyright (c) 1993-2022 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
// Determines what compiler is being used.
//
////////////////////////////////////////////////////////////////


#if !defined(OPENNURBS_SYSTEM_COMPILER_INC_)
#define OPENNURBS_SYSTEM_COMPILER_INC_

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//
// BEGIN - MYON_COMPILER_* defines
//
// MYON_COMPILER_* specifies the C/C++ compiler used.
//   At most one the MYON_COMPILER_* should be defined.
//
*/

/*
// Compilers with a better static_assert declaration
// may change MYON_STATIC_ASSERT accordingly.
*/
#if __cpp_static_assert >= 201411L // C++17
#  define MYON_STATIC_ASSERT(expr) static_assert((expr))
#  define MYON_STATIC_ASSERT_MSG(expr, msg) static_assert((expr), msg)
#elif __cpp_static_assert >= 200410L // C++11
#  define MYON_STATIC_ASSERT(expr) static_assert((expr), #expr)
#  define MYON_STATIC_ASSERT_MSG(expr, msg) static_assert((expr), msg)
#else
#  define MYON_STATIC_ASSERT(expr) typedef char on_static_assert_t[(expr) != 0]
#  define MYON_STATIC_ASSERT_MSG(expr, msg) typedef char on_static_assert_t[(expr) != 0]
#endif

/*
// Compilers that require special declaration of callback functions
// will change MYON_CALLBACK_CDECL accordingly.
*/
#define MYON_CALLBACK_CDECL

/*
// Compilers that require special declaration of callback functions
// will change MYON_CALLBACK_CDECL accordingly.
*/
#define MYON_VARGS_FUNC_CDECL


/*
// Compilers that do not support the C++ 11 noexcept keyword
// or similar will change MYON_NOEXCEPT accordingly.
*/
#define MYON_NOEXCEPT noexcept

/*
// Compilers that do not support the C++ 11 rvalue refs
// will change MYON_HAS_RVALUEREF accordingly.
*/
#define MYON_HAS_RVALUEREF

#if defined (_MSC_VER)
/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//
// MYON_COMPILER_MSC
//
*/

#define MYON_COMPILER_MSC

/* 
// Usage example - disables warning number 1234 - see CL docs for details
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(1234)
...
#pragma MYON_PRAGMA_WARNING_POP
*/
#define MYON_PRAGMA_WARNING_PUSH warning( push )
#define MYON_PRAGMA_WARNING_POP warning( pop )
#define MYON_PRAGMA_WARNING_DISABLE_MSC(MYON_PRAGMA_WARNING_DISABLE_param) warning( disable : MYON_PRAGMA_WARNING_DISABLE_param )  // Microsoft CL warning disable

// Opennurbs warning level is /Wall
// Microsoft, Freetype, and other external header files issue warnings we can't do anything about
#define MYON_PRAGMA_WARNING_BEFORE_DIRTY_INCLUDE warning( push, 1 )
#define MYON_PRAGMA_WARNING_AFTER_DIRTY_INCLUDE warning( pop )

#if !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
/*
// Visual Studio 2005 issues a C4996 warning for lots of
// standard C runtime functions that take string pointers.
// The _CRT_SECURE_NO_DEPRECATE suppresses these warnings.
// TODO - clean up our code and remove do not define _CRT_SECURE_NO_DEPRECATE
*/
#endif

/*
// Microsoft's Visual C/C++ requires functions that use vargs
// to be declared with __cdecl
// Since this code must also compile with non-Micorosoft compilers,
// the MYON_VARGS_FUNC_CDECL macro is used to insert __cdecl when needed.
*/
#undef MYON_VARGS_FUNC_CDECL
#define MYON_VARGS_FUNC_CDECL __cdecl

/*
// Microsoft's Visual C/C++ requires some callback functions
// to be declared with __cdecl
// Since this code must also compile with non-Micorosoft compilers,
// the MYON_VARGS_FUNC_CDECL macro is used to insert __cdecl when needed.
*/
#undef MYON_CALLBACK_CDECL
#define MYON_CALLBACK_CDECL __cdecl

#if _MSC_VER < 1700
#undef MYON_HAS_RVALUEREF
#endif

#if _MSC_VER >= 1700 && _MSC_VER < 1900

// VC 2012 and 2013 does not support the C++11 noexcept specifier.
// Appending throw() indicates the function does not throw
// exceptions.  Using throw() is not 100% equivalent to
// noexcept because you cannot use a bool parameter
// and hence cannot do things like
//   class MyClass : public MyOtherClass
//   {
//     void MyClass() noexcept(std::is_nothrow_default_constructible<MyOtherClass>::value)
//     void MyClass(const MyClass& ) noexcept(std::is_nothrow_copy_constructible<MyOtherClass>::value)
//   };
#undef MYON_NOEXCEPT
#define MYON_NOEXCEPT throw()
#endif

#if _MSC_VER >= 1300
#define MYON_COMPILER_MSC1300
#if _MSC_VER >= 1400
/* Using at least Visual C++ 8.0 (2005) */
#define MYON_COMPILER_MSC1400
#if _MSC_VER >= 1600
/* Using at least Visual C++ 10.0 (2010) */
#define MYON_COMPILER_MSC1600
#if _MSC_VER >= 1700
/* Using at least Visual C++ 11.0 (2012) */
#define MYON_COMPILER_MSC1700
#if _MSC_VER >= 1800
/* Using at least Visual C++ 12.0 (2013) */
#define MYON_COMPILER_MSC1800
#if _MSC_VER >= 1900
/* Using at least Visual C++ 14.0 (2015) */
#define MYON_COMPILER_MSC1900

#if !defined(OPENNURBS_WALL) && !defined(MYON_COMPILING_OPENNURBS)
/* 
// TEMPORARY C4456 SUPPRESSION Feb 17 2016 - WILL BE REMOVED ASAP
// Rhino code is still too dirty to leave 4456 on.
*/
// Suppress Warning C4456 declaration of '...' hides previous local declaration ...	
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(4456)
#endif

// C4100 '...': unreferenced formal parameter ...
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(4100)

// C4061 enumerator '...' in switch of enum '...' is not explicitly handled by a case label
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(4061)

// C4062 enumerator '...' in switch of enum '...' is not handled
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(4062)

// C4711 function '...' selected for inline expansion
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(4711)

// C4820 '...' bytes padding added after construct '...'
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(4820)

/////////////////////////////////////////////////////////////////////////////////////
//
// Dale Lear April 2017.
// The Visual Studio 2017 default is to disable warnings 4263, 4264, 4265, 4266
//
// These are the warnings that help detect abuse of virtual functions and failed attempts
// to override virtual functions.
//
// I've enable them for all Rhino core projects.
// "#pragma warning(default:xxxx): is the way to enable warning xxxx.
//

// 'function' : member function does not override any base class virtual member function
//    A class function definition has the same name as a virtual function in a base 
//    class but not the same number or type of arguments. This effectively hides the
//    virtual function in the base class.
#pragma warning(default:4263)

// 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
#pragma warning(default:4264)

// 'class' : class has virtual functions, but destructor is not virtual
//    When a class has virtual functions but a nonvirtual destructor, objects 
//    of the type might not be destroyed properly when the class is destroyed 
//    through a base class pointer.
// NOTE WELL: Occasionally there are good reasons to have a non-virtual destructor.
//  See Guideline #4 in http://www.gotw.ca/publications/mill18.htm for a nice description of the details.
//  However, those case are generally rare and this warning can explicitly be disabled for those rare
//  cases.
#pragma warning(default:4265)

#if defined(OPENNURBS_WALL)
// The 4266 warning is enabled only when OPENNURBS_WALL is defined.
//
// In the case of Rhino, this warning is not useful.
// It is reasonable to override some but not all base class virtual functions
// that have the same name. 
// For example, almost every class derived from CRhinoObject overrides 
// virtual MYON_BoundingBox BoundingBox() const;
// but most do not override
// virtual  MYON_BoundingBox BoundingBox( const class CRhinoViewport* pViewport ) const;

// 'function' : no override available for virtual member function from base 'type'; function is hidden
//    A derived class did not override all overloads of a virtual function.
#pragma warning(default:4266)
#endif

//
/////////////////////////////////////////////////////////////////////////////////////


#endif
#endif
#endif
#endif
#endif
#endif

#define MYON_SIZEOF_WCHAR_T 2

#elif defined(__clang__) | defined(__clang) |  defined(MYON_COMPILER_CLANG)
/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//
// MYON_COMPILER_CLANG
//
*/

/*
// The __clang__ test must come before the gnu tests because Apple's clang
// uncoditionally defines __GNUC__
*/
#if !defined(MYON_COMPILER_CLANG)
#define MYON_COMPILER_CLANG
#endif

/* 
// Usage example - disables CLang warning xyz - See CLang docs for warning options
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_CLANG("-Wxyz")
...
#pragma MYON_PRAGMA_WARNING_POP
*/
#define MYON_PRAGMA_WARNING_PUSH clang diagnostic push // Apple CLang warning state push
#define MYON_PRAGMA_WARNING_POP clang diagnostic pop // Apple CLang warning state pop
#define MYON_PRAGMA_WARNING_DISABLE_CLANG(MYON_PRAGMA_WARNING_DISABLE_param) clang diagnostic ignored MYON_PRAGMA_WARNING_DISABLE_param  // Apple CLang warning disable


#if defined(__has_feature) && __has_feature(cxx_noexcept)
#undef MYON_NOEXCEPT
#define MYON_NOEXCEPT noexcept
#endif

#if defined(__has_extension) && __has_extension(cxx_rvalue_references) && !defined(MYON_HAS_RVALUEREF)
#define MYON_HAS_RVALUEREF
#elif defined(MYON_HAS_RVALUEREF)
#undef MYON_HAS_RVALUEREF
#endif

#elif defined( MYON_COMPILER_ANDROIDNDK )
/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//
// MYON_COMPILER_ANDROIDNDK
//
*/

/*
// McNeel defines MYON_COMPILER_ANDROIDNDK in makefiles
*/

#if defined(__GNUC__) && (__GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 7))
// C++11 noexcept and Rvalue references are in gcc 4.7 and later
#undef MYON_NOEXCEPT
#define MYON_NOEXCEPT noexcept
#if !defined(MYON_HAS_RVALUEREF)
#define MYON_HAS_RVALUEREF
#endif

#else
#undef MYON_HAS_RVALUEREF
#undef MYON_NOEXCEPT

#endif

// You may need to define __GXX_EXPERIMENTAL_CXX0X__ to get
// C++11 std::shared_ptr to work as you expect when using
// the Android NDK gcc 4.7.  See
// http://stackoverflow.com/questions/14532057/smart-pointers-not-working-with-android-ndk-r8
// for more details.
//
//#define __GXX_EXPERIMENTAL_CXX0X__

#elif defined(__GNUG_) || defined(__GNUG__) || defined(__GNUC_) || defined(__GNUC__) || defined(_GNU_SOURCE) || defined(__GNU_SOURCE)
/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//
// MYON_COMPILER_GNU
//
*/

#define MYON_COMPILER_GNU
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

/* 
// Usage example - disables gcc warning xyz - See Gnu gcc docs for warning options
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_CLANG("-Wxyx")
...
#pragma MYON_PRAGMA_WARNING_POP
*/
#define MYON_PRAGMA_WARNING_PUSH GCC diagnostic push // Gnu gcc warning state push
#define MYON_PRAGMA_WARNING_POP GCC diagnostic pop // Gnu gcc warning state pop
#define MYON_PRAGMA_WARNING_DISABLE_GNU(MYON_PRAGMA_WARNING_DISABLE_param) GCC diagnostic ignored MYON_PRAGMA_WARNING_DISABLE_param  // Apple CLang warning disable


#if defined(__GNUC__) && (__GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 7))
// C++11 noexcept and Rvalue references are in gcc 4.7 and later
#undef MYON_NOEXCEPT
#define MYON_NOEXCEPT noexcept
#if !defined(MYON_HAS_RVALUEREF)
#define MYON_HAS_RVALUEREF
#endif

#else
#undef MYON_HAS_RVALUEREF
#undef MYON_NOEXCEPT

#endif


#elif defined(__BORLANDC__)
/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//
// MYON_COMPILER_BORLANDC
//
*/
#define MYON_COMPILER_BORLANDC

#endif


/*
These defines will be set to something more appropriate when 
opennurbs_system_compiler detects the compiler and platform.
*/
#if !defined(MYON_PRAGMA_WARNING_PUSH) && !defined(MYON_PRAGMA_WARNING_POP)
#define MYON_PRAGMA_WARNING_PUSH
#define MYON_PRAGMA_WARNING_POP
#elif !defined(MYON_PRAGMA_WARNING_PUSH) || !defined(MYON_PRAGMA_WARNING_POP)
#error mistake in the compiler specific define setup above
#endif

#if !defined(MYON_PRAGMA_WARNING_DISABLE_MSC)
#define MYON_PRAGMA_WARNING_DISABLE_MSC(MYON_PRAGMA_WARNING_DISABLE_param)
#endif

#if !defined(MYON_PRAGMA_WARNING_DISABLE_CLANG)
#define MYON_PRAGMA_WARNING_DISABLE_CLANG(MYON_PRAGMA_WARNING_DISABLE_param)
#endif

#if !defined(MYON_PRAGMA_WARNING_DISABLE_GNU)
#define MYON_PRAGMA_WARNING_DISABLE_GNU(MYON_PRAGMA_WARNING_DISABLE_param)
#endif

#if !defined(MYON_PRAGMA_WARNING_BEFORE_DIRTY_INCLUDE) && !defined(MYON_PRAGMA_WARNING_AFTER_DIRTY_INCLUDE)
#define MYON_PRAGMA_WARNING_BEFORE_DIRTY_INCLUDE MYON_PRAGMA_WARNING_PUSH
#define MYON_PRAGMA_WARNING_AFTER_DIRTY_INCLUDE MYON_PRAGMA_WARNING_POP
#endif

#endif



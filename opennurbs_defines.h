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
//   Includes all openNURBS toolkit defines and enums.
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_DEFINES_INC_)
#define OPENNURBS_DEFINES_INC_

#if !defined(OPENNURBS_SYSTEM_INC_)
#error Include opennurbs_system.h before opennurbs_defines.h
#endif

#if defined (cplusplus) || defined(_cplusplus) || defined(__cplusplus) || defined(MYON_CPLUSPLUS)
// C++ extern "C" declaration for C linkage

#if !defined(MYON_CPLUSPLUS)
#define MYON_CPLUSPLUS
#endif
#define MYON_EXTERNC extern "C"
#define MYON_BEGIN_EXTERNC extern "C" {
#define MYON_END_EXTERNC   }

#define MYON_UINT_FROM_ENUM(e) (static_cast<unsigned int>(e))
#define MYON_INT_FROM_ENUM(e) ((int)static_cast<unsigned int>(e))

#else

/* C file - no extern declaration required or permitted */

#define MYON_EXTERNC
#define MYON_BEGIN_EXTERNC
#define MYON_END_EXTERNC  

#endif


/*
// Declarations in header (.H) files look like
//
//   MYON_DECL type function():
//   extern MYON_EXTERN_DECL type global_variable;
//   class MYON_CLASS classname {};
//   MYON_TEMPLATE template class MYON_CLASS template<T>;
//
*/

#define MYON_ENUM_FROM_UNSIGNED_CASE(e) case (unsigned int)e: return(e); break
#define MYON_ENUM_TO_STRING_CASE(e) case e: return( MYON_String(#e) ); break
#define MYON_ENUM_TO_WIDE_STRING_CASE(e) case e: return( MYON_wString(#e) ); break
#define MYON_ENUM_TO_STRING_CASE_SET(e,s) case e: (s)=MYON_String(#e); break
#define MYON_ENUM_TO_WIDE_STRING_CASE_SET(e,s) case e: (s)=MYON_wString(#e); break

/* export/import */
#if defined(OPENNURBS_EXPORTS)
/* compiling opennurbs as some type of dynamic linking library */

#if defined(MYON_RUNTIME_LINUX)
/* Linux defaults to exporting all functions*/
#define MYON_CLASS
#define MYON_DECL
#define MYON_EXTERN_DECL

#else
#if defined(MYON_COMPILER_MSC)
/* compiling OpenNurbs as a Windows DLL - export classes, functions, templates, and globals */
#define MYON_CLASS __declspec(dllexport)
#define MYON_DECL __declspec(dllexport)
#define MYON_EXTERN_DECL __declspec(dllexport)
#define MYON_DLL_TEMPLATE

#elif defined(MYON_COMPILER_CLANG)
/* compiling opennurbs as an Apple shared library */
#define MYON_CLASS __attribute__ ((visibility ("default")))
#define MYON_DECL __attribute__ ((visibility ("default")))
#define MYON_EXTERN_DECL __attribute__ ((visibility ("default")))

#else
#error fill in your compiler dynamic linking decorations
#endif
#endif

#elif defined(OPENNURBS_IMPORTS)
/* dynamically linking with opennurbs in some way */

#if defined(MYON_COMPILER_MSC)
/* using OpenNurbs as a Windows DLL - import classes, functions, templates, and globals */
#define MYON_CLASS __declspec(dllimport)
#define MYON_DECL __declspec(dllimport)
#define MYON_EXTERN_DECL __declspec(dllimport)
#define MYON_DLL_TEMPLATE extern

#elif defined(MYON_COMPILER_CLANG)
/* using opennurbs as an Apple shared library */
#define MYON_CLASS __attribute__ ((visibility ("default")))
#define MYON_DECL __attribute__ ((visibility ("default")))
#define MYON_EXTERN_DECL __attribute__ ((visibility ("default")))

#else
#error fill in your compiler dynamic linking decorations
#endif

#else

/* compiling or using OpenNurbs as a static library */
#define MYON_CLASS
#define MYON_DECL
#define MYON_EXTERN_DECL

#if defined(MYON_DLL_TEMPLATE)
#undef MYON_DLL_TEMPLATE
#endif

#endif


// MYON_DEPRECATED is used to mark deprecated functions.
#if defined(MYON_COMPILER_MSC)
#define MYON_DEPRECATED  __declspec(deprecated)
#define MYON_DEPRECATED_MSG(s) [[deprecated(s)]]
#if defined(OPENNURBS_IN_RHINO)
#define MYON_WIP_SDK
#define MYON_INTERNAL_SDK
#else
#define MYON_WIP_SDK [[deprecated("Do not use! This function is a work in progress and will change.")]]
#define MYON_INTERNAL_SDK [[deprecated("Do not use! This function is internal.")]]
#endif
#elif defined(MYON_COMPILER_CLANG)
#define MYON_DEPRECATED  __attribute__((deprecated))
#define MYON_DEPRECATED_MSG(s) [[deprecated(s)]]
#if defined(OPENNURBS_IN_RHINO)
#define MYON_WIP_SDK
#define MYON_INTERNAL_SDK
#else
#define MYON_WIP_SDK [[deprecated("Do not use! This function is a work in progress and will change.")]]
#define MYON_INTERNAL_SDK [[deprecated("Do not use! This function is internal.")]]
#endif
#else
#define MYON_DEPRECATED
#define MYON_DEPRECATED_MSG(s)
#define MYON_WIP_SDK
#define MYON_INTERNAL_SDK
#endif

#if defined(PI)
/* double precision MYON_PI = 3.141592653589793238462643. MYON_PI radians = 180 degrees */
#define MYON_PI           PI
#else
/* double precision MYON_PI = 3.141592653589793238462643. MYON_PI radians = 180 degrees */
#define MYON_PI 3.141592653589793238462643
#endif

/* double precision MYON_2PI = 2.0*MYON_PI. MYON_2PI radians = 360 degrees. */
#define MYON_2PI (2.0*MYON_PI)

/* double precision MYON_HALFPI = 0.5*MYON_PI. MYON_HALFPI radians = 90 degrees.  */
#define MYON_HALFPI (0.5*MYON_PI)

/* angle_in_degrees = MYON_DEGREES_TO_RADIANS*angle_in_radians */
#define MYON_DEGREES_TO_RADIANS (MYON_PI/180.0)

/* angle_in_radians = MYON_RADIANS_TO_DEGREES*angle_in_degrees */
#define MYON_RADIANS_TO_DEGREES (180.0/MYON_PI)

/*
Parameters:
  angle_in_radians - [in]
    Angle measure in radians
Returns:
  Angle measure in degrees
*/
MYON_DECL
double MYON_DegreesFromRadians(
  double angle_in_radians
);

/*
Parameters:
  angle_in_degrees - [in]
    Angle measure in degrees
Returns:
    Angle measure in radians
*/
MYON_DECL
double MYON_RadiansFromDegrees(
  double angle_in_degrees
);

#define MYON_SQRT2          1.414213562373095048801689
#define MYON_SQRT3          1.732050807568877293527446
#define MYON_SQRT3_OVER_2   0.8660254037844386467637230
#define MYON_1_OVER_SQRT2   0.7071067811865475244008445
#define MYON_SIN_PI_OVER_12 0.2588190451025207623488990
#define MYON_COS_PI_OVER_12 0.9659258262890682867497433

#define MYON_LOG2         0.6931471805599453094172321
#define MYON_LOG10        2.302585092994045684017991

#define MYON_ArrayCount(a) (sizeof(a)/sizeof((a)[0]))

#if defined(DBL_MAX)
#define MYON_DBL_MAX DBL_MAX
#else
#define MYON_DBL_MAX 1.7976931348623158e+308
#endif

#if defined(DBL_MIN)
#define MYON_DBL_MIN DBL_MIN
#else
#define MYON_DBL_MIN 2.22507385850720200e-308
#endif

// MYON_EPSILON = 2^-52
#if defined(DBL_EPSILON)
#define MYON_EPSILON DBL_EPSILON
#else
#define MYON_EPSILON 2.2204460492503131e-16
#endif
#define MYON_SQRT_EPSILON 1.490116119385000000e-8

#if defined(FLT_EPSILON)
#define MYON_FLOAT_EPSILON FLT_EPSILON
#else
#define MYON_FLOAT_EPSILON 1.192092896e-07
#endif
#define MYON_SQRT_FLOAT_EPSILON 3.452669830725202719e-4


#if defined(UINT_MAX)
#define MYON_UINT_MAX UINT_MAX
#else
#define MYON_UINT_MAX (~(0U))
#endif

/*
// In cases where lazy evaluation of a double value is
// performed, b-rep tolerances being a notable example,
// this value is used to indicate the value has not been
// computed.  This value must be < -1.0e308. and > -MYON_DBL_MAX
//
// The reasons MYON_UNSET_VALUE is a valid finite number are:
//
//   1) It needs to round trip through fprintf/sscanf.
//   2) It needs to persist unchanged through assignment
/       and not generate exceptions when assigned.
//   3) MYON_UNSET_VALUE == MYON_UNSET_VALUE needs to be true.
//   4) MYON_UNSET_VALUE != MYON_UNSET_VALUE needs to be false.
//
// Ideally, it would also have these SNaN attributes
//   * When used in a calculation, a floating point exception
//     occurs.
//   * No possibility of a valid calculation would generate
//     MYON_UNSET_VALUE.
//   * float f = (float)MYON_UNSET_VALUE would create an invalid
//     float and generate an exception.
*/
#define MYON_UNSET_POSITIVE_VALUE 1.23432101234321e+308
#define MYON_UNSET_VALUE -MYON_UNSET_POSITIVE_VALUE

/*
// MYON_UNSET_FLOAT is used to indicate a texture coordinate
// value cannot be calculated or is not well defined.  
// In hindsight, this value should have been MYON_FLT_QNAN
// because many calculation convert float texture coordinates
// to doubles and the "unset"ness attribute is lost.
*/
#define MYON_UNSET_POSITIVE_FLOAT 1.234321e+38f
#define MYON_UNSET_FLOAT -MYON_UNSET_POSITIVE_FLOAT

// When unsigned int values are used in a context where 
// 0 is a valid index and there needs to be a value that 
// indicates the index is not set, use MYON_UNSET_UINT_INDEX.
#define MYON_UNSET_UINT_INDEX 0xFFFFFFFFU

// When signed int values are used in a context where 
// 0 and small negative values are valid indices and there needs
// to be a value that indicates the index is not set,
// use MYON_UNSET_INT_INDEX.  This value is INT_MIN+1
#define MYON_UNSET_INT_INDEX ((const int)-2147483647)

MYON_BEGIN_EXTERNC

// IEEE 754 special values

extern MYON_EXTERN_DECL const double MYON_DBL_QNAN;
extern MYON_EXTERN_DECL const double MYON_DBL_PINF;
extern MYON_EXTERN_DECL const double MYON_DBL_NINF;

extern MYON_EXTERN_DECL const float  MYON_FLT_QNAN;
extern MYON_EXTERN_DECL const float  MYON_FLT_PINF;
extern MYON_EXTERN_DECL const float  MYON_FLT_NINF;


/*
The MYON_PTR_SEMAPHORE* values are used in rare cases
when a special signal must be passed as a pointer argument.
The values must be a multiple of 8 to suppress runtime pointer alignment checks.
The values must never be a valid user heap or stack pointer value.
*/
#define MYON_PTR_SEMAPHORE1 ((MYON__UINT_PTR)8)
#define MYON_PTR_SEMAPHORE2 ((MYON__UINT_PTR)16)
#define MYON_PTR_SEMAPHORE3 ((MYON__UINT_PTR)24)
#define MYON_PTR_SEMAPHORE4 ((MYON__UINT_PTR)32)
#define MYON_PTR_SEMAPHORE_MAX ((MYON__UINT_PTR)32)


/*
Description:
Parameters:
  x - [out] returned value of x is an SNan
            (signalling not a number).
Remarks:
  Any time an SNaN passes through an Intel FPU, the result
  is a QNaN (quiet nan) and the invalid operation exception
  flag is set.  If this exception is not masked, then the
  exception handler is invoked.
 
    double x, y;
    MYON_DBL_SNAN(&x);
    y = x;     // y = QNAN and invalid op exception occurs
    z = sin(x) // z = QNAN and invalid op exception occurs

  So, if you want to reliably initialize doubles to SNaNs, 
  you must use memcpy() or some other method that does not
  use the Intel FPU.
*/
MYON_DECL
void MYON_DBL_SNAN( double* x );

MYON_DECL
void MYON_FLT_SNAN( float* x );

/*
Returns:
  MYON_UNSET_FLOAT, if x = MYON_UNSET_VALUE.
  MYON_UNSET_POSITIVE_FLOAT, if x = MYON_UNSET_POSITIVE_VALUE.
  (float)x, otherwise.
*/
MYON_DECL
float MYON_FloatFromDouble(
  double x
);

/*
Returns:
  MYON_UNSET_VALUE, if x = MYON_UNSET_FLOAT.
  MYON_UNSET_POSITIVE_VALUE, if x = MYON_UNSET_POSITIVE_FLOAT.
  (double)x, otherwise.
*/
MYON_DECL
double MYON_DoubleFromFloat(
  float x
);

/*
Returns:
  A nonzero runtime unsigned that is incremented every call to MYON_NextContentSerialNumber().
  This value is useful as a "content serial number" that can be used to detect
  when the content of an object has changed.
*/
MYON__UINT64 MYON_NextContentSerialNumber();

MYON_END_EXTERNC

#if defined(MYON_CPLUSPLUS)
MYON_DECL
bool MYON_IsNullPtr(const void* ptr);

MYON_DECL
bool MYON_IsNullPtr(const MYON__UINT_PTR ptr);

MYON_DECL
bool MYON_IsNullPtr(const MYON__INT_PTR ptr);
#endif

/*
// In cases where lazy evaluation of a color value is
// performed, this value is used to indicate the value
// has not been computed.
*/
#define MYON_UNSET_COLOR 0xFFFFFFFF

/*
// In cases when an absolute "zero" tolerance
// is required to compare model space coordinates,
// MYON_ZERO_TOLERANCE is used.  The value of
// MYON_ZERO_TOLERANCE should be no smaller than
// MYON_EPSILON and should be several orders of
// magnitude smaller than MYON_SQRT_EPSILON
//
*/
//#define MYON_ZERO_TOLERANCE 1.0e-12
// MYON_ZERO_TOLERANCE = 2^-32
#define MYON_ZERO_TOLERANCE 2.3283064365386962890625e-10

/*
// In cases when an relative "zero" tolerance is
// required for comparing model space coordinates,
// (fabs(a)+fabs(b))*MYON_RELATIVE_TOLERANCE is used.
// MYON_RELATIVE_TOLERANCE should be larger than
// MYON_EPSILON and smaller than no larger than
// MYON_ZERO_TOLERANCE*2^-10.
//
*/
// MYON_RELATIVE_TOLERANCE = 2^-42
#define MYON_RELATIVE_TOLERANCE 2.27373675443232059478759765625e-13

/*
// Bugs in geometry calculations involving world coordinates
// values > MYON_MAXIMUM_WORLD_COORDINATE_VALUE
// will be a low priority.
*/
// MYON_MAXIMUM_VALUE = 2^27
#define MYON_MAXIMUM_WORLD_COORDINATE_VALUE 1.34217728e8

/*
// Any 3d coordinate value >= MYON_NONSENSE_WORLD_COORDINATE_VALUE
// will be adjusted as needed. Any calculation creating 3d coordinates
// with values >= MYON_NONSENSE_WORLD_COORDINATE_VALUE should be
// inspected for bugs.
*/
// MYON_NONSENSE_WORLD_COORDINATE_VALUE = 1.0e100
#define MYON_NONSENSE_WORLD_COORDINATE_VALUE 1.0e100

/*
// The default test for deciding if a curvature value should be
// treated as zero is
// length(curvature) <= MYON_ZERO_CURVATURE_TOLERANCE.
// MYON_ZERO_CURVATURE_TOLERANCE must be set so that
// MYON_ZERO_CURVATURE_TOLERANCE >= sqrt(3)*MYON_ZERO_TOLERANCE
// so that K.IsTiny() = true implies |K| <= MYON_ZERO_CURVATURE_TOLERANCE
*/
#define MYON_ZERO_CURVATURE_TOLERANCE 1.0e-8
#define MYON_RELATIVE_CURVATURE_TOLERANCE 0.05

/* default value for angle tolerances = 1 degree */
#define MYON_DEFAULT_ANGLE_TOLERANCE_RADIANS (MYON_PI/180.0)
#define MYON_DEFAULT_ANGLE_TOLERANCE_DEGREES (MYON_DEFAULT_ANGLE_TOLERANCE_RADIANS * 180.0/MYON_PI)
#define MYON_DEFAULT_ANGLE_TOLERANCE MYON_DEFAULT_ANGLE_TOLERANCE_RADIANS
#define MYON_DEFAULT_ANGLE_TOLERANCE_COSINE 0.99984769515639123915701155881391
#define MYON_MINIMUM_ANGLE_TOLERANCE (MYON_DEFAULT_ANGLE_TOLERANCE/10.0)

#define MYON_DEFAULT_DISTANCE_TOLERANCE_MM 0.01

/*
*/
MYON_DECL
MYON__UINT64 MYON_SecondsSinceJanOne1970UTC();

union MYON_U
{
  char      b[8]; // 8 bytes
  MYON__INT64 h;    // 64 bit integer
  MYON__INT32 i;    // 32 bit integer
  int       j[2]; // two 32 bit integers
  void*     p;
  double    d;
};

#if defined(MYON_CPLUSPLUS)

// pair of integer indices.  This
// is intentionally a struct/typedef
// rather than a class so that it
// can be used in other structs.
class MYON_CLASS MYON_2dex
{
public:
  MYON_2dex() = default;
  ~MYON_2dex() = default;
  MYON_2dex(const MYON_2dex&) = default;
  MYON_2dex& operator=(const MYON_2dex&) = default;

public:
  // do not initialize i, j for performance reasons
  int i;
  int j;

  MYON_2dex(int i, int j);

  static const MYON_2dex Unset;  // (MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);
  static const MYON_2dex Zero;  // (0, 0)

  // return a copy of this item, where i is smaller or equal to j
  MYON_2dex AsIncreasing() const;

  // return a copy of this item, where i is larger or equal to j
  MYON_2dex AsDecreasing() const;

  bool operator==(const MYON_2dex& src) const;
  bool operator!=(const MYON_2dex& src) const;
};

class MYON_CLASS MYON_2udex
{
public:
  MYON_2udex() = default;
  ~MYON_2udex() = default;
  MYON_2udex(const MYON_2udex&) = default;
  MYON_2udex& operator=(const MYON_2udex&) = default;

public:
  // do not initialize i, j for performance reasons
  unsigned int i;
  unsigned int j;

  MYON_2udex(unsigned int i, unsigned int j);

  // return a copy of this item, where i is smaller or equal to j
  MYON_2udex AsIncreasing() const;

  // return a copy of this item, where i is larger or equal to j
  MYON_2udex AsDecreasing() const;

  static int DictionaryCompare(
    const MYON_2udex* lhs,
    const MYON_2udex* rhs
  );

  static int CompareFirstIndex(
    const MYON_2udex* lhs,
    const MYON_2udex* rhs
  );

  static int CompareSecondIndex(
    const MYON_2udex* lhs,
    const MYON_2udex* rhs
  );

  static const MYON_2udex Unset;  // (MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX);
  static const MYON_2udex Zero;  // (0, 0)

  bool operator==(const MYON_2udex& src) const;
  bool operator!=(const MYON_2udex& src) const;

  bool operator<(const MYON_2udex& src) const;
  bool operator<=(const MYON_2udex& src) const;
  bool operator>=(const MYON_2udex& src) const;
  bool operator>(const MYON_2udex& src) const;
};

class MYON_CLASS MYON_3dex
{
public:
  MYON_3dex() = default;
  ~MYON_3dex() = default;
  MYON_3dex(const MYON_3dex&) = default;
  MYON_3dex& operator=(const MYON_3dex&) = default;

public:
  // do not initialize i, j, k for performance reasons
  int i;
  int j;
  int k;

  MYON_3dex(int i, int j, int k);

  static const MYON_3dex Unset;  // (MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX);
  static const MYON_3dex Zero;  // (0, 0, 0)
};

class MYON_CLASS MYON_3udex
{
public:
  MYON_3udex() = default;
  ~MYON_3udex() = default;
  MYON_3udex(const MYON_3udex&) = default;
  MYON_3udex& operator=(const MYON_3udex&) = default;

public:
  // do not initialize i, j, k for performance reasons
  unsigned int i;
  unsigned int j;
  unsigned int k;
  
  MYON_3udex(unsigned int i, unsigned int j, unsigned int k);

  static const MYON_3udex Unset;  // (MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX);
  static const MYON_3udex Zero;  // (0, 0, 0)

  static int DictionaryCompare(
    const MYON_3udex* lhs,
    const MYON_3udex* rhs
  );

  static int CompareFirstIndex(
    const MYON_3udex* lhs,
    const MYON_3udex* rhs
  );

  static int CompareSecondIndex(
    const MYON_3udex* lhs,
    const MYON_3udex* rhs
  );

  static int CompareThirdIndex(
    const MYON_3udex* lhs,
    const MYON_3udex* rhs
  );

  static int CompareFirstAndSecondIndex(
    const MYON_3udex* lhs,
    const MYON_3udex* rhs
  );

};

// quadruplet of integer indices.
class MYON_CLASS MYON_4dex
{
public:
  MYON_4dex() = default;
  ~MYON_4dex() = default;
  MYON_4dex(const MYON_4dex&) = default;
  MYON_4dex& operator=(const MYON_4dex&) = default;


	int operator[](int i) const;
	int& operator[](int i);
public:
  // do not initialize i, j, k, l for performance reasons
  int i;
  int j;
  int k;
  int l;

  MYON_4dex(int i, int j, int k, int l);

  // return a copy, where i is smaller or equal to j, j is smaller or equal to k, and k is smaller or equal to l
  MYON_4dex AsIncreasing() const;

  // return a copy, where i is smaller or equal to k; if they are equal, j is smaller or equal to l
  MYON_4dex AsPairwiseIncreasing() const;

  static const MYON_4dex Unset;  // (MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);
  static const MYON_4dex Zero;  // (0, 0, 0, 0)

  bool operator==(const MYON_4dex& src) const;
  bool operator!=(const MYON_4dex& src) const;
};

class MYON_CLASS MYON_4udex
{
public:
  MYON_4udex() = default;
  ~MYON_4udex() = default;
  MYON_4udex(const MYON_4udex&) = default;
  MYON_4udex& operator=(const MYON_4udex&) = default;

public:
  // do not initialize i, j, k, l for performance reasons
  unsigned int i;
  unsigned int j;
  unsigned int k;
  unsigned int l;

  MYON_4udex(unsigned int i, unsigned int j, unsigned int k, unsigned int l);

  // return a copy, where i is smaller or equal to j, j is smaller or equal to k, and k is smaller or equal to l
  MYON_4udex AsIncreasing() const;

  // return a copy, where i is smaller or equal to k; if they are equal, j is smaller or equal to l
  MYON_4udex AsPairwiseIncreasing() const;

  static const MYON_4udex Unset;  // (MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX);
  static const MYON_4udex Zero;  // (0, 0, 0, 0)

  bool operator==(const MYON_4udex& src) const;
  bool operator!=(const MYON_4udex& src) const;
};


enum class MYON_StringMapType : int
{
  Identity = 0,
  UpperCase = 1, 
  LowerCase = 2 
};

enum class MYON_StringMapOrdinalType : int
{
  Identity = 0,
  UpperOrdinal = 1, 
  LowerOrdinal = 2,
  MinimumOrdinal = 3
};

enum class MYON_DateFormat : int
{
  Unset = 0,
  Omit = 1,
  
  ///<summary>
  /// February 1st, 2001 = 2001-2-1
  ///</summary>
  YearMonthDay = 2,
  
  ///<summary>
  /// February 1st, 2001 = 2001-1-2
  ///</summary>
  YearDayMonth = 3,
  
  ///<summary>
  /// February 1st, 2001 = 2-1-2001
  ///</summary>
  MonthDayYear = 4,
  
  ///<summary>
  /// February 1st, 2001 = 1-2-2001
  ///</summary>
  DayMonthYear = 5,
  
  ///<summary>
  /// February 1st, 2001 = 2001-32
  ///</summary>
  YearDayOfYear = 6
};

enum class MYON_TimeFormat : int
{
  Unset = 0,
  Omit = 1,
  HourMinute12 = 2,
  HourMinuteSecond12 = 3,
  HourMinute24 = 4,
  HourMinuteSecond24 = 5
};

MYON_DECL
MYON_StringMapOrdinalType MYON_StringMapOrdinalTypeFromStringMapType(
  MYON_StringMapType map_type
  );

///<summary>
/// MYON_ChainDirection is used to specify directions when building
/// chains of components like edges or faces.
///</summary>
enum class MYON_ChainDirection : unsigned char
{
  Unset = 0,

  ///<summary>
  /// Search for chain links before the current link.
  ///</summary>
  Previous = 1,

  ///<summary>
  /// Search for chain links after the current link.
  ///</summary>
  Next = 2,

  ///<summary>
  /// Search for chain links before and after the current link.
  ///</summary>
  Both = 3
};

///<summary>
///Style of color gradient
///</summary>
enum class MYON_GradientType : int
{
  ///<summary>No gradient</summary>
  None = 0,
  ///<summary>Linear (or axial) gradient between two points</summary>
  Linear = 1,
  ///<summary>Radial (or spherical) gradient using a center point and a radius</summary>
  Radial = 2,
  ///<summary>Disabled linear gradient. Useful for keeping gradient information around, but not having it displayed</summary>
  LinearDisabled = 3,
  ///<summary>Disabled radial gradient. Useful for keeping gradient information around, but not having it displayed</summary>
  RadialDisabled = 4
};


// OpenNurbs enums
class MYON_CLASS MYON
{
public:
  /*
  Description:
    Call before using openNURBS to ensure all class definitions
    are linked.
  */
  static void Begin();


  /*
  Description:
    Call when finished with openNURBS.
  Remarks:
    Currently does nothing.
  */
  static void End();

  /*
  Returns:
    0: not initialized
    1: in the body of MYON:Begin()
    2: MYON::Begin() has finished.
  */
  static unsigned int LibraryStatus();

  /*
  Set the library status
  */
  static void SetLibraryStatus(unsigned int status);

  /*
  Returns:
    The value of OPENNURBS_VERSION_NUMBER, which is defined in opennurbs_version.h.
  Remarks:
    The high bit of this number is set.  Do not cast the result as an int.
  */
  static
  unsigned int Version();

  /*
  Returns:
    The value of OPENNURBS_VERSION_MAJOR, which is defined in opennurbs_version.h
    (0 to 63).
  */
  static
  unsigned int VersionMajor();

  /*
  Returns:
    63 = maximum major version number that opennurbs version number utilities can handle.
  */
  static
  unsigned int VersionMajorMaximum();
  
  /*
  Returns:
    The value of OPENNURBS_VERSION_MINOR, which is defined in opennurbs_version.h
    (0 to 127).
  */
  static
  unsigned int VersionMinor();

  /*
  Returns:
    127 = maximum minor version number that opennurbs version number utilities can handle.
  */
  static
  unsigned int VersionMinorMaximum();

  /*
  Returns:
    The value of OPENNURBS_VERSION_YEAR, which is defined in opennurbs_version.h
    > 2014.
  */
  static
  unsigned int VersionYear();

  /*
  Returns:
    The value of OPENNURBS_VERSION_MONTH, which is defined in opennurbs_version.h
    1 to 12.
  */
  static
  unsigned int VersionMonth();

  /*
  Returns:
    The value of OPENNURBS_VERSION_DAY_OF_MONTH, which is defined in opennurbs_version.h
    (1 to 31).
  */
  static
  unsigned int VersionDayOfMonth();
  
  /*
  Returns:
    The value of OPENNURBS_VERSION_HOUR, which is defined in opennurbs_version.h
    (0 to 23).
  */
  static
  unsigned int VersionHour();
  
  /*
  Returns:
    The value of OPENNURBS_VERSION_MINUTE, which is defined in opennurbs_version.h
    (0 to 59).
  */
  static
  unsigned int VersionMinute();

  /*
  Returns:
    The value of OPENNURBS_VERSION_BRANCH, which is defined in opennurbs_version.h
      0: developer build
      1: Windows Commercial build
      2: Mac Commercial build
      3: Windows BETA build
      4: Mac Beta build
      5: Windows WIP build
      6: Mac WIP build
  */
  static
  unsigned int VersionBranch();

  /*
  Description:
    Get the opennurbs version number as a quartet of values.
  Parameters:
    version_quartet - [out]
      version_quartet[0] = MYON::VersionMajor()
      version_quartet[1] = MYON::VersionMinor()
      version_quartet[2] = (year - 2000)*1000 + day_of_year
      version_quartet[3] = (hour*1000 + minute*10 + OPENNURBS_VERSION_BRANCH)
  Returns:
    The value of OPENNURBS_VERSION_NUMBER, which is defined in opennurbs_version.h.
  Remarks:
    The high bit of the returned value is set. Do not cast the result as an int.
  */
  static
  unsigned int VersionGetQuartet(
    unsigned int version_quartet[4]
    );
  
  
  /*
  Returns:
    The value of OPENNURBS_VERSION_QUARTET_STRING, which is defined in opennurbs_version.h.
  Remarks:
    The high bit of this number is set.  Do not cast the result as an int.
  */
  static
  const char* VersionQuartetAsString();
  
  /*
  Returns:
    The value of OPENNURBS_VERSION_QUARTET_WSTRING, which is defined in opennurbs_version.h.
  Remarks:
    The high bit of this number is set.  Do not cast the result as an int.
  */
  static
  const wchar_t* VersionQuartetAsWideString();

  /*
  Returns:
    Empty string or the git hash of the revision of the source code used to build this application.
    The git hash is a hexadecimal number represented in UTF-8 string.
  Remarks:
    Developer builds return "".
    Build system builds return the git revision hash.
  */
  static const char* SourceGitRevisionHash();

  /*
  Returns:
    Empty string or the name of the git branch containing the source code used to build this application.
  Remarks:
    Developer builds return "".
    Build system builds return the git branch name or "".
  */
  static const char* SourceGitBranchName();

  /*
  Returns:
    A string that identifies the McNeel version control system source code to build this application.
  Remarks:
    Developer builds return "".
    Build system builds return the git <branch name> @ <git revision hash> or "".
  */
  static const char* SourceIdentification();

  //// File open/close for DLL use ///////////////////////////////////////////////

  static
  FILE* OpenFile( // like fopen() - needed when OpenNURBS is used as a DLL
          const char* filename,
          const char* filemode
          );

  static
  FILE* OpenFile( // like fopen() - needed when OpenNURBS is used as a DLL
          const wchar_t* filename,
          const wchar_t* filemode
          );

  static
  int CloseFile( // like fclose() - needed when OpenNURBS is used as a DLL
          FILE* // pointer returned by OpenFile()
          );

  static
  int CloseAllFiles(); // like _fcloseall() - needed when OpenNURBS is used as a DLL

  /*
  Description:
    Uses the flavor of fstat that is appropriate for the platform.
  Parameters:
    filename - [in]
    fp - [in]
    filesize - [out] (can be nullptr if you do not want filesize)
    create_time - [out] (can be nullptr if you do not want last create time)
    lastmodify_time - [out] (can be nullptr if you do not want last modification time)
  Returns:
    True if file exists, can be opened for read, and fstat worked.
  */
  static
  bool GetFileStats( const wchar_t* filename,
                     size_t* filesize,
                     time_t* create_time,
                     time_t* lastmodify_time
                    );

  static
  bool GetFileStats( FILE* fp,
                     size_t* filesize,
                     time_t* create_time,
                     time_t* lastmodify_time
                    );

  /*
  Returns true if pathname is a directory.
  */
  static bool IsDirectory( const wchar_t* pathname );
  static bool IsDirectory( const char* utf8pathname );

  /*
  Returns
    If the file is an opennurbs file, the version of the file
    is returned (2,3,4,50,...).
    If the file is not an opennurbs file, 0 is returned.
  */
  static int IsOpenNURBSFile( const wchar_t* pathname );
  static int IsOpenNURBSFile( const char* utf8pathname );
  static int IsOpenNURBSFile( FILE* fp );

#pragma region RH_C_SHARED_ENUM [MYON::RuntimeEnvironment] [Rhino.RuntimeEnvironment] [byte]
  /////////////////////////////////////////////////////////////////
  /// <summary>
  /// MYON::RuntimeEnvironment identifies a runtime environment (operating system).
  /// This value is saved in binary archives so appropriate adjustments
  /// to resources provided by runtime environments, like fonts, can be made
  /// when an archive created in one runtime environment is used in another.
  /// </summary>
  enum class RuntimeEnvironment : unsigned char
  {
    ///<summary>
    /// MYON::RuntimeEnvironment::Unset indicates no runtime is set.
    ///</summary>
    Unset =  0, 

    ///<summary>
    /// MYON::RuntimeEnvironment::None indicates no runtime.
    /// This is a different condition from MYON::Runtime::Unset. 
    ///</summary>
    None = 1,

    ///<summary>
    /// MYON::RuntimeEnvironment::Windows indicates some version of Microsoft Windows.
    ///</summary>
    Windows = 2,

    ///<summary>
    /// MYON::RuntimeEnvironment::Apple indicates some version of Apple OS X or iOS.
    ///</summary>
    Apple = 3,

    ///<summary>
    /// MYON::RuntimeEnvironment::Android indicates some version of Google Android.
    ///</summary>
    Android =  4,

    ///<summary>
    /// MYON::RuntimeEnvironment::Linux indicates some version of Linux.
    ///</summary>
    Linux = 5
  };
#pragma endregion

  static MYON::RuntimeEnvironment RuntimeEnvironmentFromUnsigned(
    unsigned int runtime_environment_as_unsigned
    );

  /*
  Returns:
    Current runtime environment.
  */
  static MYON::RuntimeEnvironment CurrentRuntimeEnvironment();


#pragma region RH_C_SHARED_ENUM [MYON::ReadFileResult] [Rhino.ReadFileResult] [byte]
  /// <summary>
  /// MYON::ReadFileResult reports what happened when a file read was attempted.
  /// </summary>
  enum class ReadFileResult : unsigned char
  {
    ///<summary>
    /// No result is available.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Read completed with no errors.
    ///</summary>
    Completed = 1,

    ///<summary>
    /// Read completed with non fatal errors.
    ///</summary>
    CompletedWithErrors = 2,

    ///<summary>
    /// Read failed.
    ///</summary>
    Failed = 3
  };
#pragma endregion

  static MYON::ReadFileResult ReadFileResultFromUnsigned(
    unsigned int read_file_result_as_unsigned
    );

  /*
  Returns:
    True if the value of read_file_result is one indicating partial to complete success.
    False if read_file_result is MYON::ReadFileResult::Unset or MYON::ReadFileResult::Failed.
  */
  static bool ReadFileCompleted(
    MYON::ReadFileResult read_file_result
  );

  /*
  Returns:
    True if the value of read_file_result is one indicating total failure.
    False if read_file_result is MYON::ReadFileResult::Unset or a value indicating partial to complete success.
  */
  static bool ReadFileFailed(
    MYON::ReadFileResult read_file_result
  );


  // Defines the current working space.
  enum active_space : unsigned char
  {
    no_space          = 0,
    model_space       = 1, // 3d modeling or "world" space
    page_space        = 2, // page/layout/paper/printing space
    uveditor_space    = 3, // UV Editor space
    blockeditor_space = 4  // Block Editor space
  };

  static active_space ActiveSpace(int); // convert integer to active_space enum
  
#pragma region RH_C_SHARED_ENUM [MYON::LengthUnitSystem] [Rhino.UnitSystem] [byte]
  /// <summary>
  /// MYON::LengthUnitSystem identifies a length unit system
  /// United States customary length units references:
  ///   http://www.nist.gov/pml/wmd/metric/upload/frn-59-5442-1959.pdf
  ///   http://en.wikipedia.org/wiki/United_States_customary_units
  ///   http://en.wikipedia.org/wiki/International_yard_and_pound
  /// </summary>
  enum class LengthUnitSystem : unsigned char
  {
    ///<summary>
    /// MYON::LengthUnitSystem::None indicates no length unit system. The scale factor
    /// when converting between a specified unit system and None is always 1.0.
    /// MYON::LengthUnitSystem::None is used as a unit system for models and
    /// instance definitions that should be imported or referenced with no
    /// scaling applied.
    ///</summary>
    None =  0, 

    ///<summary>
    /// 1 angstroms = 1.0e-10 meters
    ///</summary>
    Angstroms = 12,

    // SI (metric) units

    ///<summary>
    /// 1 nanometer = 1.0e-9 meters
    ///</summary>
    Nanometers = 13,

    ///<summary>
    /// 1 micron = 1.0e-6 meters
    ///</summary>
    Microns =  1,
    
    ///<summary>
    /// 1 millimeter = 1.0e-3 meters
    ///</summary>
    Millimeters =  2,
    
    ///<summary>
    /// 1 centimeter = 1.0e-2 meters
    ///</summary>
    Centimeters =  3,
    
    ///<summary>
    /// 1 decimeter = 1.0e-1 meters
    ///</summary>
    Decimeters = 14,

    ///<summary>
    /// SI meter length unit
    ///</summary>
    Meters =  4,

    ///<summary>
    /// 1 dekameter = 1.0e+1 meters
    ///</summary>
    Dekameters = 15,  // 1.0e+1 meters

    ///<summary>
    /// 1 hectometer = 1.0e+2 meters
    ///</summary>
    Hectometers = 16,

    ///<summary>
    /// 1 kilometer = 1.0e+3 meters
    ///</summary>
    Kilometers =  5,

    ///<summary>
    /// 1 megameter = 1.0e+6 meters
    ///</summary>
    Megameters = 17,

    ///<summary>
    /// 1 gigameter = 1.0e+9 meters
    ///</summary>
    Gigameters = 18,

    ///<summary>
    /// 1 microinches = 2.54e-8 meters = 1.0e-6 inches
    ///</summary>
    Microinches =  6,

    ///<summary>
    /// 1 mil = 2.54e-5 meters = 0.001 inches
    ///</summary>
    Mils =  7,
    
    ///<summary>
    /// 1 inch = 0.0254 meters = 1/12 foot
    ///</summary>
    Inches =  8,
    
    ///<summary>
    /// 1 foot = 0.3048  meters (12 inches)
    ///</summary>
    Feet =  9,
    
    ///<summary>
    /// 1 foot = 0.3048 meters = 12 inches
    ///</summary>
    Yards = 19,
    
    ///<summary>
    /// 1 US statute mile = 1609.344 meters = 5280 feet
    ///</summary>
    Miles = 10,

    ///<summary>
    /// 1 printer point = 1/72 inch
    ///</summary>
    PrinterPoints = 20,
    
    ///<summary>
    /// 1 printer pica = 1/6 inch
    ///</summary>
    PrinterPicas = 21,

    // terrestrial distances

    ///<summary>
    /// 1 nautical mile = 1852 meters
    /// Approximately 1 minute of arc on a terrestrial great circle.
    /// Reference: http://en.wikipedia.org/wiki/Nautical_mile
    ///</summary>
    NauticalMiles = 22,

    // astronomical distances

    ///<summary>
    /// 1 astronomical unit = 1.4959787e+11 meters
    /// An astronomical unit (au) is the mean distance from the 
    /// center of the earth to the center of the sun.
    /// References:
    ///   http://en.wikipedia.org/wiki/Astronomical_unit (1.4959787e+11 meters)
    ///   http://units.nist.gov/Pubs/SP811/appenB9.htm (1.495979e+11 meters)
    ///</summary>
    AstronomicalUnits = 23,

    ///<summary>
    /// 1 light year = 9.4607304725808e+15 meters
    /// A light year is the distance light travels in one Julian year.
    ///    The speed of light is exactly 299792458 meters/second.
    ///    A Julian year is exactly 365.25 * 86400 seconds and is 
    ///    approximately the time it takes for one earth orbit.
    /// References: 
    ///   http://en.wikipedia.org/wiki/Light_year (9.4607304725808e+15 meters)
    ///   http://units.nist.gov/Pubs/SP811/appenB9.htm (9.46073e+15 meters)
    ///</summary>
    LightYears = 24,     

    ///<summary>
    /// 1 parsec = 3.08567758e+16 meters
    /// References: 
    ///   http://en.wikipedia.org/wiki/Parsec (3.08567758e+16 meters)
    ///   http://units.nist.gov/Pubs/SP811/appenB9.htm (3.085678e+16)
    ///</summary>
    Parsecs = 25,

    ///<summary>
    /// The name of a custom unit and the conversion to meters
    /// are saved in the MYON_UnitSystem class.
    ///</summary>
    CustomUnits = 11,

    ///<summary>
    /// The MYON::LengthUnitSystem::Unset is used to indicate no unit system is set.
    /// This is a different condition from MYON::LengthUnitSystem::None. 
    ///</summary>
    Unset = 255
  };
#pragma endregion

  static MYON::LengthUnitSystem LengthUnitSystemFromUnsigned(
    unsigned int length_unit_system_as_unsigned
    );

  /*
  Parameters:
    model_serial_number - [in]
      One good way to get this value is from MYON_ModelComponent::ModelSerialNumber().
      MYON_DimStyle, MYON_Layer, ... are all derived from MYON_ModelComponent.
  Returns:
    The length unit system used by the model
  */
  static MYON::LengthUnitSystem ModelLengthUnitSystem(
    MYON__UINT_PTR model_serial_number
    );


  static void RegisterModelLengthUnitSystemCallback(
    MYON::LengthUnitSystem (*func_ModelLengthUnitSystemCallback)(MYON__UINT_PTR)
  );

public:

  /*
  Returns
    True if the length unit is one of
      LengthUnitSystem::Angstroms
      LengthUnitSystem::Nanometers
      LengthUnitSystem::Microns
      LengthUnitSystem::Millimeters
      LengthUnitSystem::Centimeters
      LengthUnitSystem::Decimeters
      LengthUnitSystem::Meters
      LengthUnitSystem::Dekameters
      LengthUnitSystem::Hectometers
      LengthUnitSystem::Kilometers
      LengthUnitSystem::Megameters
      LengthUnitSystem::Gigameters
      LengthUnitSystem::NauticalMiles
      LengthUnitSystem::AstronomicalUnits
      LengthUnitSystem::LightYears
      LengthUnitSystem::Parsecs
  */
  static bool IsMetricLengthUnit(
    MYON::LengthUnitSystem length_unit_system
  );

  /*
  Returns
    True if the length unit is one of
      LengthUnitSystem::Microinches
      LengthUnitSystem::Mils
      LengthUnitSystem::Inches
      LengthUnitSystem::Feet
      LengthUnitSystem::Yards
      LengthUnitSystem::Miles
      LengthUnitSystem::PrinterPoints
      LengthUnitSystem::PrinterPicas
  */
  static bool IsUnitedStatesCustomaryLengthUnit(
    MYON::LengthUnitSystem length_unit_system
  );

  /*
  Returns
    True if the length unit is one of
      LengthUnitSystem::Millimeters
      LengthUnitSystem::Centimeters
      LengthUnitSystem::Decimeters
      LengthUnitSystem::Meters
      LengthUnitSystem::Dekameters
      LengthUnitSystem::Hectometers
      LengthUnitSystem::Kilometers
      LengthUnitSystem::Inches
      LengthUnitSystem::Feet
      LengthUnitSystem::Yards
      LengthUnitSystem::Miles
      LengthUnitSystem::NauticalMiles
  */
  static bool IsTerrestrialLengthUnit(
    MYON::LengthUnitSystem length_unit_system
  );

  /*
  Returns
    True if the length unit is one of
      LengthUnitSystem::AstronomicalUnits
      LengthUnitSystem::LightYears
      LengthUnitSystem::Parsecs
  */
  static bool IsExtraTerrestrialLengthUnit(
    MYON::LengthUnitSystem length_unit_system
  );

  /*
  Returns
    True if the length unit is one of
      LengthUnitSystem::Angstroms
      LengthUnitSystem::Nanometers
      LengthUnitSystem::Microns
      LengthUnitSystem::Microinches
      LengthUnitSystem::Mils
  */
  static bool IsMicroscopicLengthUnit(
    MYON::LengthUnitSystem length_unit_system
  );

  /*
  Returns
    True if the length unit is one of
      LengthUnitSystem::PrinterPoints
      LengthUnitSystem::PrinterPicas
  */
  static bool IsUnitedStatesPrinterLengthUnit(
    MYON::LengthUnitSystem length_unit_system
  );

  /*
  Description:
    Scale factor for changing unit "standard" systems.
  Parameters:
    us_from - [in]
    us_to - [in] 
  For example:

          100.0 = MYON::UnitScale( MYON::LengthUnitSystem::Meters, MYON::LengthUnitSystem::Centimeters ) 
          2.54  = MYON::UnitScale( MYON::LengthUnitSystem::Inches, MYON::LengthUnitSystem::Centimeters ) 
          12.0  = MYON::UnitScale( MYON::LengthUnitSystem::Feet,   MYON::LengthUnitSystem::Inches ) 

  Remarks:
    If you are using custom unit systems, use the version that takes MYON_UnitSystem 
    or MYON_3dmUnitsAndTolerances  parameters.
    If either parameter is MYON::LengthUnitSystem::Unset, then MYON_DBL_QNAN is returned.
    If either parameter is MYON::LengthUnitSystem::None, then 1.0 is returned.
    If either parameter is MYON::LengthUnitSystem::CustomUnits, then 1.0 is returned.
  */
  static double UnitScale(
      MYON::LengthUnitSystem us_from,
      MYON::LengthUnitSystem us_to
      );
  static double UnitScale(
      const class MYON_UnitSystem& us_from, 
      const class MYON_UnitSystem& us_to
      );
  static double UnitScale(
      MYON::LengthUnitSystem us_from,
      const class MYON_UnitSystem& us_to
      );
  static double UnitScale(
      const class MYON_UnitSystem& us_from, 
      MYON::LengthUnitSystem us_to
      );
  static double UnitScale(
      const class MYON_3dmUnitsAndTolerances& us_from, 
      const class MYON_3dmUnitsAndTolerances& us_to
      );


#pragma region RH_C_SHARED_ENUM [MYON::AngleUnitSystem] [Rhino.AngleUnitSystem] [byte]
  /// <summary>
  /// MYON::AngleUnitSystem identifies an angle unit system
  /// </summary>
  enum class AngleUnitSystem : unsigned char
  {
    ///<summary>
    /// MYON::AngleUnitSystem::None indicates no angle unit system
    /// is specified and model angle unit system should be used.
    ///</summary>
    None = 0,

    ///<summary>
    /// 1 turn = 2pi radians. 
    ///</summary>
    Turns = 1,

    ///<summary>
    /// 1 turn = 2pi radians. 
    ///</summary>
    Radians = 2, // 2pi radians = 1 turn

    ///<summary>
    /// 360 arc degrees  = 1 turn = 2pi radians 
    ///</summary>
    Degrees = 3,

    ///<summary>
    /// 60 arc minutes = 1 arc degree
    ///</summary>
    Minutes = 4,

    ///<summary>
    /// 60 arc seconds = 1 arc minute
    ///</summary>
    Seconds = 5,

    ///<summary>
    /// 400 gradians = 2pi radians. 
    ///</summary>
    Gradians = 6,

    ///<summary>
    /// The MYON::AngleUnitSystem::Unset is used to indicates no angle unit system 
    /// has been specified in user interface code.
    ///</summary>
    Unset = 255
  };
#pragma endregion

  static MYON::AngleUnitSystem AngleUnitSystemFromUnsigned(
    unsigned int angle_unit_system_as_unsigned
    );

  static double AngleUnitScale(
      MYON::AngleUnitSystem us_from,
      MYON::AngleUnitSystem us_to
      );


  /// <summary>
  /// MYON::EarthCoordinateSystem identifies the standard used to define Earth latitude, longitude, and elevation coordinates.
  /// </summary>
  enum class EarthCoordinateSystem : unsigned char
  {
    ///<summary>
    /// MYON::EarthCoordinateSystem::Unset
    ///</summary>
    Unset = 0,

    ///<summary>
    /// MYON::EarthCoordinateSystem::GroundLevel Not well defined, but latitude and longitude will be good enough for architecture sun studies.
    ///</summary>
    GroundLevel = 1,   /// Ground level - coordinates vary with time and location

    ///<summary>
    /// MYON::EarthCoordinateSystem::MeanSeaLevel Not well defined, but latitude and longitude will be good enough for architecture sun studies.
    ///</summary>
    MeanSeaLevel = 2,

    ///<summary>
    /// MYON::EarthCoordinateSystem::CenterOfEarth Not well defined. The Earth's center of mass and center of volume are at different locations.
    ///</summary>
    CenterOfEarth = 3,

    ///<summary>
    /// MYON::EarthCoordinateSystem::WGS1984 World Geodetic System 1984 standard. (Current GPS standard.)
    ///</summary>
    WGS1984 = 5,

    ///<summary>
    /// MYON::EarthCoordinateSystem::EGM2008 Earth Gravitational Model 2008 standard.
    ///</summary>
    EGM2008 = 6
  };

  static MYON::EarthCoordinateSystem EarthCoordinateSystemFromUnsigned(
    unsigned int earth_coordinte_system_as_unsigned
  );
  
  /// <summary>
  /// MYON::ComponentNameConflictResolution identifies a method to use
  /// when components are being added to model, the component name must
  /// be unique, and the name of the new is already in use in the context.
  /// The function MYON_ModelComponent::UniqueNameRequired(MYON_ModelComponent::Type) 
  /// can be used to determine if a component requires a unique name.
  /// </summary>
  enum class ComponentNameConflictResolution : unsigned char
  {
    ///<summary>
    /// A method to resolve name conflicts has not been specified.
    ///</summary>
    Unset = 0,
    
    ///<summary>
    /// Interactively ask the user to choose one of the following methods
    /// to resolve component name conflicts.
    ///</summary>
    QueryMethod = 1,

    ///<summary>
    /// Use the existing component, discard the new component.
    /// All references to the discarded component are changed to reference the
    /// the surviving component.
    ///</summary>
    UseExistingComponent = 2,

    ///<summary>
    /// Replace the existing component with the new component.
    /// All references to the discarded component are changed reference the
    /// the surviving component.
    ///</summary>
    ReplaceExistingComponent = 3, 

    ///<summary>
    /// Keep both components.
    /// Resolve the name conflict by automatically assigning a name new component.
    /// This is typically done by appending an integer to the original name.
    ///</summary>
    KeepBothComponentsAutomaticRename = 4, 

    ///<summary>
    /// Keep both components.
    /// Resolve the name conflict by interactively asking for an unused name 
    /// to assign to the new component.
    ///</summary>
    KeepBothComponentsQueryRename = 5,

    ///<summary>
    /// No name conflict was detected and no special action is required.
    /// This can occur when the names in question are unique or unique names are not required.
    ///</summary>
    NoConflict = 0xFF
  };

  static MYON::ComponentNameConflictResolution ComponentNameConflictResolutionFromUnsigned(
    unsigned int component_name_conflict_resolution_as_unsigned
    );

  //// distance_display_mode ///////////////////////////////////


  // Obsolete - use MYON_DimStyle::DimensionLengthDisplay
  enum class OBSOLETE_DistanceDisplayMode : unsigned char
  {
    // Obsolete - Obsolete - use MYON_DimStyle::DimensionLengthDisplay::ModelUnits
    Decimal    = 0, 

    // Obsolete - Obsolete - use MYON_DimStyle::DimensionLengthDisplay::InchesFractional
    Fractional = 1,

    // Obsolete - Obsolete - use MYON_DimStyle::DimensionLengthDisplay::FeetAndInches
    FeetInches = 2
  };

  static MYON::OBSOLETE_DistanceDisplayMode DistanceDisplayModeFromUnsigned(
    unsigned int distance_display_mode_as_unsigned
  );


  //// point_style ///////////////////////////////////////////////////////////////
  enum point_style 
  {
    unknown_point_style   = 0,
    not_rational          = 1,
    homogeneous_rational  = 2,
    euclidean_rational    = 3,
    intrinsic_point_style = 4, // point format used in definition
    point_style_count     = 5
  };

  static point_style PointStyle(int); // convert integer to point_style enum

  //// knot_style ///////////////////////////////////////////////////////////////
  enum knot_style // if a knot vector meets the conditions of two styles,
  {               // then the style with the lowest value is used
    unknown_knot_style     = 0, // unknown knot style
    uniform_knots          = 1, // uniform knots (ends not clamped)
    quasi_uniform_knots    = 2, // uniform knots (clamped ends, degree >= 2)
    piecewise_bezier_knots = 3, // all internal knots have full multiplicity
    clamped_end_knots      = 4, // clamped end knots (with at least 1 interior non-uniform knot)
    non_uniform_knots      = 5, // known to be none of the above
    knot_style_count       = 6
  };

  static knot_style KnotStyle(int); // convert integer to knot_style enum

  //// continuity ////////////////////////////////////////////////////////////////
  enum class continuity : unsigned int
  {
    unknown_continuity = 0,

    // These test for parametric continuity.  In particular,
    // all types of MYON_Curves are considered infinitely 
    // continuous at the start/end of the evaluation domain.
    C0_continuous =  1, // continuous function
    C1_continuous =  2, // continuous first derivative
    C2_continuous =  3, // continuous first and second derivative
    G1_continuous =  4, // continuous unit tangent
    G2_continuous =  5, // continuous unit tangent and curvature

    // 20 March 2003 Dale Lear added these.
    //
    // Continuity tests using the following enum values
    // are identical to tests using the preceding enum values
    // on the INTERIOR of a curve's domain.  At the END of
    // a curve a "locus" test is performed in place of a 
    // parametric test. In particular, at the END of a domain,
    // all open curves are locus discontinuous.  At the END of
    // a domain, all closed curves are at least C0_locus_continuous.
    // By convention all MYON_Curves are considered 
    // locus continuous at the START of the evaluation domain.
    // This convention is not strictly correct, but is was
    // adopted to make iterative kink finding tools easier to
    // use and so that locus discontinuities are reported once
    // at the end parameter of a curve rather than twice.
    C0_locus_continuous =  6, // locus continuous function
    C1_locus_continuous =  7, // locus continuous first derivative
    C2_locus_continuous =  8, // locus continuous first and second derivative
    G1_locus_continuous =  9, // locus continuous unit tangent
    G2_locus_continuous = 10, // locus continuous unit tangent and curvature

    Cinfinity_continuous = 11, // analytic discontinuity
    Gsmooth_continuous = 12    // aesthetic discontinuity
  };

  /*
  Description:
    Convert int to MYON::continuity enum value
  */
  static continuity Continuity(int);

  /*
  Description:
    Convert int to MYON::continuity enum value and
    convert the locus flavored values to the parametric
    flavored values.
  */
  static continuity ParametricContinuity(int);

  /*
  Description:
    Convert int to MYON::continuity enum value and
    convert the higher order flavored values to 
    the corresponding C1 or G1 values needed to
    test piecewise linear curves.
  */
  static continuity PolylineContinuity(int);

  //// curve_style ///////////////////////////////////////////////////////////////
  enum curve_style 
  {
    unknown_curve_style   =  0,
    line                  =  1,
    circle                =  2,
    ellipse               =  3, // with distinct foci (not a circle)
    parabola              =  4,
    hyperbola             =  5,
    planar_polyline       =  6, // not a line segment
    polyline              =  7, // non-planar polyline
    planar_freeform_curve =  8, // planar but none of the above
    freeform_curve        =  9, // known to be none of the above
    curve_style_count     = 10
  };

  static curve_style CurveStyle(int); // convert integer to curve_style enum

  //// surface_style ///////////////////////////////////////////////////////////////
  enum surface_style 
  {
    unknown_surface_style =  0,
    plane                 =  1,
    circular_cylinder     =  2, // portion of right circular cylinder
    elliptical_cylinder   =  3, // portion of right elliptical cylinder
    circular_cone         =  4, // portion of right circular cone
    elliptical_cone       =  5, // portion of right elliptical cone
    sphere                =  6, // portion of sphere
    torus                 =  7, // portion of torus
    surface_of_revolution =  8, // portion of surface of revolution that is none of the above
    ruled_surface         =  9, // portion of a ruled surface this is none of the above
    freeform_surface      = 10, // known to be none of the above
    surface_style_count   = 11
  };

  static surface_style SurfaceStyle(int); // convert integer to surface_style enum

  //// sort_algorithm ///////////////////////////////////////////////////////////////
  enum class sort_algorithm : unsigned int
  {
    heap_sort  = 0,
    quick_sort = 1
  };

  static sort_algorithm SortAlgorithm(int); // convert integer to sort_method enum

  //// endian-ness ///////////////////////////////////////////////////////////////
  enum class endian : unsigned int
  {
    little_endian = 0, // least significant byte first or reverse byte order - Intel x86, ...
    big_endian    = 1  // most significant byte first - Motorola, Sparc, MIPS, ...
  };

  static endian Endian(int); // convert integer to endian enum
  static endian Endian();    // returns endian-ness of current CPU

  //// archive modes //////////////////////////////////////////////////////////////
  enum class archive_mode : unsigned int
  {
    unset_archive_mode = 0,
    read      = 1, // all read modes have bit 0x0001 set
    write     = 2, // all write modes have bit 0x0002 set
    readwrite = 3,
    read3dm   = 5,
    write3dm  = 6
  };
  static archive_mode ArchiveMode(int); // convert integer to endian enum


  //// view projections ///////////////////////////////////////////////////////////

  // The x/y/z_2pt_perspective_view projections are ordinary perspective
  // projection. Using these values insures the MYON_Viewport member 
  // functions properly constrain the camera up and camera direction vectors
  // to preserve the specified perspective vantage.
  enum view_projection : unsigned int
  { 
    unknown_view       = 0,
    parallel_view      = 1,
    perspective_view   = 2
  };

  /*
  Description:
    Converts integer into MYON::view_projection enum value.
  Parameters:
    i - [in]
  Returns:
    MYON::view_projection enum with same value as i.
    If i is not an MYON::view_projection enum value,
    then MYON::unknow_view is returned.
  */
  static view_projection ViewProjection(int i);

  /*
  Parameters:
    projection - [in]
  Returns:
    True if projection is MYON::perspective_view.
  */
  static bool IsPerspectiveProjection( MYON::view_projection projection );


  /*
  Parameters:
    projection - [in]
  Returns:
    True if projection is MYON::parallel_view.
  */
  static bool IsParallelProjection( MYON::view_projection projection );

  //// view coordinates ///////////////////////////////////////////////////////////

  enum coordinate_system 
  {
    world_cs  = 0, 
    camera_cs = 1, 
    clip_cs   = 2, 
    screen_cs = 3 
  };

  static coordinate_system CoordinateSystem(int); // convert integer to coordinate_system enum

  //// exception types ///////////////////////////////////////////////////////////
	enum exception_type 
  {
    unknown_exception = 0,
		out_of_memory,  
    corrupt_object,               // invalid object encountered - continuing would crash or
                                  // result in corrupt object being saved in archive.
		unable_to_write_archive,      // write operation failed - out of file space/read only mode/...?
		unable_to_read_archive,       // read operation failed - truncated archive/locked file/... ?
		unable_to_seek_archive,       // seek operation failed - locked file/size out of bounds/... ?
		unexpected_end_of_archive,    // truncated archive
		unexpected_value_in_archive   // corrupt archive?
  };
  static exception_type ExceptionType(int); // convert integer to exception_type enum

  //// layer mode ///////////////////////////////////////////////////////////
  // OBSOLETE 
	enum layer_mode 
  {
    normal_layer       = 0, // visible, objects on layer can be selected and changed
    hidden_layer       = 1, // not visible, objects on layer cannot be selected or changed
    locked_layer       = 2, // visible, objects on layer cannot be selected or changed
    layer_mode_count   = 3
  };
  static layer_mode LayerMode(int); // convert integer to layer_mode enum

  //// object mode ///////////////////////////////////////////////////////////
	enum object_mode 
  {
    normal_object    = 0, // object mode comes from layer
    hidden_object    = 1, // not visible, object cannot be selected or changed
    locked_object    = 2, // visible, object cannot be selected or changed
    idef_object      = 3, // object is part of an MYON_InstanceDefinition.  The
                          // MYON_InstanceDefinition m_object_uuid[] array will
                          // contain this object attribute's uuid.
    object_mode_count = 4
  };
  static object_mode ObjectMode(int); // convert integer to object_mode enum

  //// object display color /////////////////////////////////////////////////////////
	enum object_color_source
  {
    color_from_layer    = 0, // use color assigned to layer
    color_from_object   = 1, // use color assigned to object
    color_from_material = 2, // use diffuse render material color
    color_from_parent   = 3  // for objects with parents (like objects in instance references, use parent linetype)
                             // if no parent, treat as color_from_layer
  };
  static object_color_source ObjectColorSource(int); // convert integer to object_color_source enum

  //// object plot color /////////////////////////////////////////////////////////
	enum plot_color_source
  {
    plot_color_from_layer   = 0, // use plot color assigned to layer
    plot_color_from_object  = 1, // use plot color assigned to object
    plot_color_from_display = 2, // use display color
    plot_color_from_parent  = 3  // for objects with parents (like objects in instance references, use parent plot color)
                                 // if no parent, treat as plot_color_from_layer
  };
  static plot_color_source PlotColorSource(int); // convert integer to plot_color_source enum

  //// object plot weight /////////////////////////////////////////////////////////
	enum plot_weight_source
  {
    plot_weight_from_layer   = 0, // use plot color assigned to layer
    plot_weight_from_object  = 1, // use plot color assigned to object
    plot_weight_from_parent  = 3  // for objects with parents (like objects in instance references, use parent plot color)
                                  // if no parent, treat as plot_color_from_layer
  };
  static plot_weight_source PlotWeightSource(int); // convert integer to plot_color_source enum

  //// object linetype /////////////////////////////////////////////////////////
	enum object_linetype_source
  {
    linetype_from_layer  = 0, // use line style assigned to layer
    linetype_from_object = 1, // use line style assigned to object
    linetype_from_parent = 3  // for objects with parents (like objects in instance references, use parent linetype)
                              // if not parent, treat as linetype_from_layer.
  };
  static object_linetype_source ObjectLinetypeSource(int); // convert integer to object_linetype_source enum

  //// object material /////////////////////////////////////////////////////////
	enum object_material_source
  {
    material_from_layer  = 0, // use material assigned to layer
    material_from_object = 1, // use material assigned to object
    material_from_parent = 3  // for objects with parents, like 
                              // definition geometry in instance
                              // references and faces in polysurfaces,
                              // this value indicates the material
                              // definition should come from the parent.
                              // If the object does not have an 
                              // obvious "parent", then treat
                              // it the same as material_from_layer.
  };
  static object_material_source ObjectMaterialSource(int); // convert integer to object_color_source enum

  //// light style /////////////////////////////////////////////////////////////
  enum light_style
  {
    unknown_light_style      = 0,
    //view_directional_light   = 1, // light location and direction in clip coordinates
    //view_point_light         = 2,
    //view_spot_light          = 3,
    camera_directional_light = 4, // light location and direction in camera coordinates
    camera_point_light       = 5, //   +x points to right, +y points up, +z points towards camera
    camera_spot_light        = 6,
    world_directional_light  = 7, // light location and direction in world coordinates
    world_point_light        = 8, 
    world_spot_light         = 9,
    ambient_light            = 10, // pure ambient light
    world_linear_light       = 11,
    world_rectangular_light  = 12,
    light_style_count        = 13
  };
  static light_style LightStyle(int); // convert integer to light_style enum

  //// curvature style /////////////////////////////////////////////////////////
  enum curvature_style
  {
    unknown_curvature_style = 0,
    gaussian_curvature = 1,
    mean_curvature = 2, // unsigned mean curvature
    min_curvature  = 3, // minimum unsigned radius of curvature
    max_curvature  = 4, // maximum unsigned radius of curvature
    curvature_style_count = 5
  };
  static curvature_style CurvatureStyle(int); // convert integer to curvature_style enum

  /////////////////////////////////////////////////////////////////
  //
  // Legacy V3 display mode enum values.
  // Beginning with V4, opennurbs and Rhino us an MYON_UUID to identify
  // display modes. The standard display mode ids are static
  // values in MYON_StandardDisplayModeId.
  enum v3_display_mode
  {
    v3_default_display       = 0, // default display
    v3_wireframe_display     = 1, // wireframe display
    v3_shaded_display        = 2, // shaded display
    v3_renderpreview_display = 3  // render preview display
  };
  static MYON::v3_display_mode V3DisplayMode(int); // convert integer to legacy v3_display_mode enum
  
  enum view_type
  {
    model_view_type  = 0,       // standard model space 3d view
    page_view_type   = 1,       // a.k.a "paper space", "plot view", etc.
                                // A page view must be orthographic,
                                // the camera frame x,y,z direction must be
                                // world x,y,z (which means the camera direction
                                // is always (0,0,-1)).  
    nested_view_type = 2,       // This view is a "model" view that is nested
                                // in another view.  The nesting and parent
                                // information is saved in MYON_3dmView.
    uveditor_view_type = 3,     // This view is a UV Editor view that shows
                                // UV meshes in a special view port.
    blockeditor_view_type = 4,  // This view is a Block Editor view that shows
                                // block definition in a special view port.
  };
  static view_type ViewType(int); // convert integer to display_mode enum


  //// texture mapping mode ///////////////////////////////////////////////////
  //
  // OBSOLETE 
  enum texture_mode
  {
    no_texture = 0,        // texture disabled
    modulate_texture = 1,  // modulate with material diffuse color
    decal_texture = 2      // decal
  };
  // OBSOLETE 
  static texture_mode TextureMode(int); // convert integer to texture_mode enum
  // OBSOLETE 
  //
  /////////////////////////////////////////////////////////////////////////////


  /// <summary>
  /// Rich text style
  ///
  /// The way rich text specifies fonts and other information depends on what
  /// created the rich text. The interpretation of the rich text "specification"
  /// varies widely and depends on the application, platform, and operating system.
  /// </summary>  
  enum class RichTextStyle : unsigned char
  {
    /// <summary>Unset" </summary>
    Unset = 0,

    /// <summary>Rich text for use with the Windows 10 SDK. The font table uses Windows LOGFONT names. </summary>
    Windows10SDK = 1,

    /// <summary>Rich text for use with the Apple OS X SDK. The font table uses Postscript names. </summary>
    AppleOSXSDK = 2,
  };
  static MYON::RichTextStyle RichTextStyleFromUnsigned(unsigned int u);

  /*
  Returns:
    MYON::RichTextStyle::Windows10SDK on Windows and MYON::RichTextStyle::AppleOSXSDK on OS X.
  */
  static MYON::RichTextStyle RichTextStyleFromCurrentPlatform();


  //// object_type ///////////////////////////////////////////////////
  enum object_type
  {
    // Use with MYON_Object::ObjectType() in situations where
    // using a switch() is better than a long string of if else if ...
    // if ( MYON_Curve::Cast() ) ... else if ( MYON_Surface::Cast() ) ...
    // ...
    unknown_object_type  =          0,

    point_object         =          1, // some type of MYON_Point
    pointset_object      =          2, // some type of MYON_PointCloud, MYON_PointGrid, ...
    curve_object         =          4, // some type of MYON_Curve like MYON_LineCurve, MYON_NurbsCurve, etc.
    surface_object       =          8, // some type of MYON_Surface like MYON_PlaneSurface, MYON_NurbsSurface, etc.
    brep_object          =       0x10, // some type of MYON_Brep
    mesh_object          =       0x20, // some type of MYON_Mesh
    layer_object         =       0x40, // some type of MYON_Layer
    material_object      =       0x80, // some type of MYON_Material
    light_object         =      0x100, // some type of MYON_Light
    annotation_object    =      0x200, // some type of MYON_Annotation
    userdata_object      =      0x400, // some type of MYON_UserData
    instance_definition  =      0x800, // some type of MYON_InstanceDefinition
    instance_reference   =     0x1000, // some type of MYON_InstanceRef
    text_dot             =     0x2000, // some type of MYON_TextDot
    grip_object          =     0x4000, // selection filter value - not a real object type
    detail_object        =     0x8000, // some type of MYON_DetailView
    hatch_object         =    0x10000, // some type of MYON_Hatch
    morph_control_object =    0x20000, // some type of MYON_MorphControl
    subd_object          =    0x40000, // some type of MYON_SubD, MYON_SubDRef, MYON_SubDComponentRef, MYON_SubD....
    loop_object          =    0x80000, // some type of MYON_BrepLoop
    brepvertex_filter    =   0x100000, // selection filter value - not a real object type (MYON_BrepVertex)
    polysrf_filter       =   0x200000, // selection filter value - not a real object type
    edge_filter          =   0x400000, // selection filter value - not a real object type (MYON_BrepEdge with associated MYON_BrepTrim)
    polyedge_filter      =   0x800000, // selection filter value - not a real object type


    // NOTE WELL: 
    //  The "mesh" vertex/edge/face filters and "meshcomponent_reference"
    //  are used to identify MYON_Mesh and MYON_SubD components. 
    //  By the time subd_object was added, there were not enough unused bits
    //  for separate subd component filters.
    meshvertex_filter    = 0x01000000, // selection filter value - not a real object type (MYON_MeshTopologyVertex, MYON_SubDVertex)
    meshedge_filter      = 0x02000000, // selection filter value - not a real object type (MYON_MeshTopologyEdge, MYON_SubDEdge)
    meshface_filter      = 0x04000000, // selection filter for MYON_Mesh triangle, quad, ngon, or MYON_SubDFace - not a real object type
    meshcomponent_reference = 0x07000000, // an MYON_MeshComponentRef or MYON_SubDComponentRef)
    
    cage_object          = 0x08000000, // some type of MYON_NurbsCage
    phantom_object       = 0x10000000,
    clipplane_object     = 0x20000000,
    extrusion_object     = 0x40000000, // some type of MYON_Extrusion

    any_object = 0xFFFFFFFF

    // Please discuss any changes with Dale Lear
  };

  static object_type ObjectType(int); // convert integer to object_type enum

  //// bitmap_type ///////////////////////////////////////////////////
  enum bitmap_type
  {
    unknown_bitmap_type = 0,
    windows_bitmap = 1,     // BITMAPINFO style
    opengl_bitmap = 2,      // unpacked OpenGL RGB or RGBA
    png_bitmap = 3
  };
  static bitmap_type BitmapType(int); // convert integer to bitmap_type enum

  enum object_decoration
  {
    no_object_decoration = 0,
    start_arrowhead      = 0x08, // arrow head at start
    end_arrowhead        = 0x10, // arrow head at end
    both_arrowhead       = 0x18  // arrow heads at start and end
  };
  static object_decoration ObjectDecoration(int); // convert integer to line_pattern enum

  enum mesh_type
  {
    default_mesh  = 0,
    render_mesh   = 1,
    analysis_mesh = 2,
    preview_mesh  = 3,
    any_mesh      = 4
  };
  static mesh_type MeshType(int); // convert integer to mesh_type enum


  // Types of object snapping.  
  // In situations where more than one type of snap applies, 
  // snaps with higher value take precedence.
  // enum values must be a power of 2.
  // MYON_ObjRef saves these values in files.  Do not change
  // the values.  The reason for the gaps between the enum
  // values is to leave room for future snaps with prededence
  // falling between existing snaps
  enum osnap_mode
  {
    os_none          =          0,
    os_near          =          2,
    os_focus         =          8,
    os_center        =       0x20,
    os_vertex        =       0x40,
    os_knot          =       0x80,
    os_quadrant      =      0x200,
    os_midpoint      =      0x800,
    os_intersection  =     0x2000,
    os_end           =    0x20000,
    os_perpendicular =    0x80000,
    os_tangent       =   0x200000,
    os_point         = 0x08000000,
    os_all_snaps     = 0xFFFFFFFF
  };
  static osnap_mode OSnapMode(int); // convert integer to osnap_mode enum


  //// Types of Curves ///////////////////////////////////////////////////////////
  enum eCurveType
  {
    ctCurve, // nothing
    ctArc,
    ctCircle,
    ctLine,
    ctNurbs,
    ctOnsurface,
    ctProxy,
    ctPolycurve,
    ctPolyline,
  };


  //// surface_loft_end_condition //////////////////////////////////////////////
  //
  // End condition parameter values for  MYON_Curve::CreateCubicLoft() and
  // MYON_Surface::CreateCubicLoft().
  enum cubic_loft_end_condition
  {
    cubic_loft_ec_quadratic      = 0,
    cubic_loft_ec_linear         = 1,
    cubic_loft_ec_cubic          = 2,
    cubic_loft_ec_natural        = 3,
    cubic_loft_ec_unit_tangent   = 4,
    cubic_loft_ec_1st_derivative = 5,
    cubic_loft_ec_2nd_derivative = 6,
    cubic_loft_ec_free_cv        = 7
  };

  /*
  Description:
    Convert an integer to cubic_loft_end_condition enum.
  Parameters:
    i - [in]
  Returns:
    corresponding cubic_loft_end_condition enum value.
  Remarks:
    If i does not correspond to a cubic_loft_end_condition
    enum value, then cubic_loft_ec_quadratic is returned.
  */
  static 
  cubic_loft_end_condition CubicLoftEndCondition(int i); 


public:

#pragma region RH_C_SHARED_ENUM [MYON::AnnotationType] [Rhino.Geometry.AnnotationType] [byte]

  /// <summary>
  /// MYON::AnnotationType identifies the type of an annotation object derived from MYON_Annotation.
  /// </summary>
  enum class AnnotationType : unsigned char
  {
    ///<summary>
    /// Not a valid annotation type.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Linear distance between two points with dimension line parallel to the dimensioned points.
    ///</summary>
    Aligned = 1,

    ///<summary>
    /// Angle between two lines.
    ///</summary>
    Angular = 2,

    ///<summary>
    /// Arc or circle diameter dimension.
    ///</summary>
    Diameter = 3,

    ///<summary>
    /// Arc or circle radius dimension.
    ///</summary>
    Radius = 4,

    ///<summary>
    /// Linear distance between two points with dimension line horizontal, vertical or rotated by a specified amount.
    ///</summary>
    Rotated = 5,

    ///<summary>
    /// Ordinate dimension. Typically used to document an offset distance between the center of a circle and a reference point.
    ///</summary>
    Ordinate = 6,

    ///<summary>
    /// Arc length of a curve.
    ///</summary>
    ArcLen = 7,

    ///<summary>
    /// Center mark dimension. Typically used to document the center of an arc or circle.
    ///</summary>
    CenterMark = 8,

    ///<summary>
    /// Text. Stand alone text with a wide variety of uses.
    ///</summary>
    Text = 9,

    ///<summary>
    /// Leader. Text and a curve with an arrow head.
    ///</summary>
    Leader = 10,

    ///<summary>
    /// Angular3pt. Angle defined by 3 points.
    ///</summary>
    Angular3pt = 11
  };

#pragma endregion

  static MYON::AnnotationType AnnotationTypeFromUnsigned( 
    unsigned int annotation_type_as_unsigned
    );



#pragma region RH_C_SHARED_ENUM [MYON::TextVerticalAlignment] [Rhino.DocObjects.TextVerticalAlignment] [byte]
  /// <summary>
  /// Vertical location of text attach point relative to text
  /// </summary>
  enum class TextVerticalAlignment : unsigned char
  {
    ///<summary>
    /// Attach to top of an "I" on the first line. (Independent of glyphs being displayed.)
    ///</summary>
    Top = 0,
    ///<summary>
    /// Attach to middle of an "I" on the first line. (Independent of glyphs being displayed.)
    ///</summary>
    MiddleOfTop = 1,
    ///<summary>
    /// Attach to baseline of first line. (Independent of glyphs being displayed.)
    ///</summary>
    BottomOfTop = 2,
    ///<summary>
    /// Attach to middle of text vertical advance. (Independent of glyphs being displayed.)
    ///</summary>
    Middle = 3,
    ///<summary>
    /// Attach to middle of an "I" on the last line. (Independent of glyphs being displayed.)
    ///</summary>
    MiddleOfBottom = 4,
    ///<summary>
    /// Attach to the baseline of the last line. (Independent of glyphs being displayed.)
    ///</summary>
    Bottom = 5,
    ///<summary>
    /// Attach to the bottom of the boudning box of the visible glyphs.
    ///</summary>
    BottomOfBoundingBox = 6, // TODO - changed to BottomOfBoundingBox
  };
#pragma endregion

  static MYON::TextVerticalAlignment TextVerticalAlignmentFromUnsigned(
    unsigned int vertical_alignment_as_unsigned
  );

  static MYON::TextVerticalAlignment TextVerticalAlignmentFromV5Justification(
    unsigned int v5_justification_bits
  );

#pragma region RH_C_SHARED_ENUM [MYON::TextHorizontalAlignment] [Rhino.DocObjects.TextHorizontalAlignment] [byte]
  /// <summary>
  /// Horizontal location of text attach point relative to text
  /// </summary>
  enum class TextHorizontalAlignment : unsigned char
  {
    /// <summary>
    /// Attach at left of text lines (Independent of glyphs being displayed.)
    /// </summary>
    Left = 0,
    /// <summary>
    /// Attach point at center of text horizontal advance (not glyph bounding box)
    /// </summary>
    Center = 1,
    /// <summary>
    /// Attach point at right text horizontal advance (not glyph bounding box)
    /// </summary>
    Right = 2,
    /// <summary>
    /// Used for Leaders only
    /// Attach point adjusts to Right or Left depending on leader tail direction in view
    /// If tail direction is to the Left, alignment is Right
    /// If tail direction is to the Right, alignment is Left
    /// </summary>
    Auto = 3,
  };
#pragma endregion

  static MYON::TextHorizontalAlignment TextHorizontalAlignmentFromUnsigned(
    unsigned int horizontal_alignment_as_unsigned
  );

  static MYON::TextHorizontalAlignment TextHorizontalAlignmentFromV5Justification(
    unsigned int v5_justification_bits
  );

#pragma region RH_C_SHARED_ENUM [MYON::TextOrientation] [Rhino.DocObjects.TextOrientation] [byte]
  /// <summary>
  /// Method for getting rotation for drawing text
  /// </summary>
  enum class TextOrientation : unsigned char
  {
    /// <summary>
    /// Text has fixed rotation on a world coordinate plane
    /// </summary>
    InPlane = 0,
    /// <summary>
    /// Text is drawn on a plane perpendicular to view direction horizontal to the screen
    /// </summary>
    InView = 1,
  };

#pragma endregion

  static MYON::TextOrientation TextOrientationFromUnsigned(
    unsigned int orientation_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON::LineCapStyle] [Rhino.DocObjects.LineCapStyle] [byte]
  /// <summary>
  /// Shape to be used at the ends of open curves
  /// </summary>
  enum class LineCapStyle : unsigned char
  {
    /// <summary> Ends are extended by a half circle with diameter equal to the curve width </summary>
    Round = 0,
    /// <summary> Also known as butt. Ends are flat and do not extend </summary>
    Flat = 1,
    /// <summary> Ends are flat and extended by half of the curve width </summary>
    Square = 2
  };
#pragma endregion

  static LineCapStyle LineCapStyleFromUnsigned(
    unsigned int cap_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON::LineJoinStyle] [Rhino.DocObjects.LineJoinStyle] [byte]
  /// <summary>
  /// Shape to be used at the corners of curves
  /// </summary>
  enum class LineJoinStyle : unsigned char
  {
    /// <summary> Rounds corners </summary>
    Round = 0,
    /// <summary> Sharp corners where outer edges are extended to a point where they meet </summary>
    Miter = 1,
    /// <summary> Beveled corner where outer edges are connected without extending </summary>
    Bevel = 2
  };
#pragma endregion

  static LineJoinStyle LineJoinStyleFromUnsigned(
    unsigned int join_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON::ClipParticipationSource] [Rhino.DocObjects.ObjectClipParticipationSource]
  /// <summary>
  /// The source of clipping plane effects on an object.
  /// </summary>
  /// <since>8.0</since>
  enum class ClipParticipationSource : unsigned char
  {
    /// <summary>Clip participation is defined by object's layer</summary>
    FromLayer = 0,
    /// <summary>Clip participation is specific to the object</summary>
    FromObject = 1,
    /// <summary>Clip participation is defined by object's parent</summary>
    FromParent = 3
  };
#pragma endregion

  static ClipParticipationSource ClipParticipationSourceFromUnsigned(
    unsigned int clip_participation_source_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON::SectionFillRule] [Rhino.DocObjects.ObjectSectionFillRule]
  /// <summary>
  /// For sectioning an object; the decision of when to generate hatches and fills.
  /// </summary>
  /// <since>8.0</since>
  enum class SectionFillRule : unsigned char
  {
    /// <summary>If a the section results in a closed curve, then generate a fill</summary>
    ClosedCurves = 0,
    /// <summary>Only generate a fill for objects that are considered Solid</summary>
    SolidObjects = 1,
  };
#pragma endregion

  static SectionFillRule SectionFillRuleFromUnsigned(
    unsigned int section_fill_rule_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON::SectionAttributesSource] [Rhino.DocObjects.ObjectSectionAttributesSource]
  /// <summary>
  /// The source of sectioning attributes on an object.
  /// </summary>
  /// <since>8.0</since>
  enum class SectionAttributesSource : unsigned char
  {
    /// <summary>Section attributes defined by object's layer</summary>
    FromLayer = 0,
    /// <summary>Section attributes are specific to the object</summary>
    FromObject = 1,
    /// <summary>Section attributes defined by object's parent</summary>
    FromParent = 2,
    /// <summary>Section attributes defined by the cutting object</summary>
    FromSectioner = 3
  };
#pragma endregion

  static SectionAttributesSource SectionAttributesSourceFromUnsigned(
    unsigned int section_attributes_source_as_unsigned
  );


#pragma region RH_C_SHARED_ENUM [MYON::ViewSectionBehavior] [Rhino.DocObjects.ViewSectionBehavior]
  /// <summary>
  /// Defines how views work with clipping planes.
  /// </summary>
  /// <since>8.0</since>
  enum class ViewSectionBehavior : unsigned char
  {
    /// <summary>Default - show geometry on unclipped side of clipping plane and show section</summary>
    ClipAndSection = 0,
    /// <summary>Only show the section at the intersection of a clipping plane</summary>
    SectionOnly = 1,
  };
#pragma endregion

  static ViewSectionBehavior ViewSectionBehaviorFromUnsigned(
    unsigned int view_section_behavior_as_unsigned
  );

private:
  // MYON::Begin() sets m_opennurbs_library_status
  //   0 = not initialized
  //   1 = in the body of MYON::Begin()
  //   2 = MYON:Begin() finished.
  static unsigned int m_opennurbs_library_status;

private:
  // prohibit instantiaion
  //MYON();             // no implementation
  //MYON( const MYON& );  // no implementation
  //~MYON();            // no implementation
};

/*
Description:
  Component indices are used to provide a persistent way
  to identify portions of complex objects.

*/
class MYON_CLASS MYON_COMPONENT_INDEX
{
public:

  // Do not change these values; they are stored in 3dm archives
  // and provide a persistent way to identify components of
  // complex objects.
  enum TYPE
  {
    invalid_type       =   0,

    brep_vertex        =   1,
    brep_edge          =   2,
    brep_face          =   3,
    brep_trim          =   4,
    brep_loop          =   5,

    mesh_vertex        =  11,
    meshtop_vertex     =  12,
    meshtop_edge       =  13,
    mesh_face          =  14,
    mesh_ngon          =  15,

    idef_part          =  21,

    polycurve_segment  =  31,

    pointcloud_point   =  41,

    group_member       =  51,


    extrusion_bottom_profile = 61, // 3d bottom profile curves
                                   //   index identifies profile component
    extrusion_top_profile    = 62, // 3d top profile curves
                                   //   index identifies profile component
    extrusion_wall_edge      = 63, // 3d wall edge curve
                                   //   index/2: identifies profile component
                                   //   index%2: 0 = start, 1 = end
    extrusion_wall_surface   = 64, // side wall surfaces
                                   //   index: identifies profile component
    extrusion_cap_surface    = 65, // bottom and top cap surfaces
                                   //   index: 0 = bottom, 1 = top
    extrusion_path           = 66, // extrusion path (axis line)
                                   //   index -1 = entire path, 0 = start point, 1 = endpoint

    //////////////////////////////////////////////////////
    //
    // MYON_SubD component index
    //
    //   Use MYON_SubD.ComponentPtrFromComponentIndex() to convert an MYON_COMPONENT_INDEX
    //   into a component pointer.
    //   See also
    //     MYON_SubD.VertexFromId()
    //     MYON_SubD.EdgeFromId()
    //     MYON_SubD.FaceFromId()
    //
    subd_vertex = 71, // m_index = MYON_SubDVertex.m_id, use MYON_SubD.ComponentPtrFromComponentIndex()
    subd_edge = 72,   // m_index = MYON_SubDEdge.m_id
    subd_face = 73,   // m_index = MYON_SubDFace.m_id

    hatch_loop = 81,  // m_index = MYON_Hatch::m_loops[] array index

    dim_linear_point       = 100,
    dim_radial_point       = 101,
    dim_angular_point      = 102,
    dim_ordinate_point     = 103,
    dim_text_point         = 104,
    dim_centermark_point   = 105,
    dim_leader_point       = 106,

    no_type           = 0xFFFFFFFF
  };

  /*
  Description:
    Safe conversion of integer value to TYPE enum.
  Parameters:
    i - [in] integer with value equal to one of the TYPE enums.
  Returns:
    The TYPE enum with the same numeric value 
    or MYON_COMPONENT_INDEX::invalid_type if no corresponding enum
    exists.
  */
  static 
  MYON_COMPONENT_INDEX::TYPE Type(int i);

  /*
  Description:
    Compare on m_type (as an int).
  */
  static
  int CompareType( const MYON_COMPONENT_INDEX* lhs, const MYON_COMPONENT_INDEX* rhs);

  /*
  Description:
    Dictionary compare on m_type, m_index as ints.
  Returns:
    < 0: a < b
    = 0: a = b
    > 0: a > b
  */
  static
  int Compare( const MYON_COMPONENT_INDEX* a, const MYON_COMPONENT_INDEX* b);

  /*
  Description:
    UnsetComponentIndex.m_type = invalid_type
    UnsetComponentIndex.m_index = -1 as int
                                = MYON_UNSET_UINT_INDEX as unsigned int
  */
  static const MYON_COMPONENT_INDEX UnsetComponentIndex;
  static const MYON_COMPONENT_INDEX WholeObject;

  /*
  Description:
    Default constructor has value MYON_COMPONENT_INDEX UnsetComponentIndex.
  */
  MYON_COMPONENT_INDEX();

  /*
  Description:
    Sets m_type = type and m_index = index.
  */
  MYON_COMPONENT_INDEX(MYON_COMPONENT_INDEX::TYPE type,int index);

  bool operator==(const MYON_COMPONENT_INDEX& other) const;
  bool operator!=(const MYON_COMPONENT_INDEX& other) const;
  bool operator<(const MYON_COMPONENT_INDEX& other) const;
  bool operator<=(const MYON_COMPONENT_INDEX& other) const;
  bool operator>(const MYON_COMPONENT_INDEX& other) const;
  bool operator>=(const MYON_COMPONENT_INDEX& other) const;

  void Set(MYON_COMPONENT_INDEX::TYPE type,int index);
  void Set(MYON_COMPONENT_INDEX::TYPE type,unsigned int index);

  /*
  Description:
    Sets m_type = invalid_type and m_index = -1.
  */
  void UnSet();

  /*
  Returns:
    True if m_type is set to a TYPE enum value between
    brep_vertex and dim_leader_point.
  */
  bool IsSet() const;

  bool IsNotSet() const;

  /*
  Returns:
    True if m_type is set to one of the mesh or meshtop
    TYPE enum values and m_index >= 0.
  */
  bool IsMeshComponentIndex() const;

  /*
  Returns:
    True if m_type is set to one of the subd
    TYPE enum values and m_index >= 0.
  */
  bool IsSubDComponentIndex() const;

  /*
  Returns:
    True if m_type is set to one of the 
    brep TYPE enum values and m_index >= 0.
  */
  bool IsBrepComponentIndex() const;

  /*
  Returns:
    True if m_type = idef_part and m_index >= 0.
  */
  bool IsIDefComponentIndex() const;

  /*
  Returns:
    True if m_type = polycurve_segment and m_index >= 0.
  */
  bool IsPolyCurveComponentIndex() const;

  /*
  Returns:
    True if m_type = group_member and m_index >= 0.
  */
  bool IsGroupMemberComponentIndex() const;

  /*
  Returns:
    True if m_type = extrusion_bottom_profile or extrusion_top_profile
    and m_index >= 0.
  */
  bool IsExtrusionProfileComponentIndex() const;

  /*
  Returns:
    True if m_type = extrusion_path and -1 <= m_index <= 1.
  */
  bool IsExtrusionPathComponentIndex() const;

  /*
  Returns:
    True if m_type = extrusion_wall_edge and m_index >= 0.
  */
  bool IsExtrusionWallEdgeComponentIndex() const;

  /*
  Returns:
    True if m_type = extrusion_wall_surface and m_index >= 0.
  */
  bool IsExtrusionWallSurfaceComponentIndex() const;

  /*
  Returns:
    True if m_type = extrusion_wall_surface or extrusion_wall_edge
    and m_index >= 0.
  */
  bool IsExtrusionWallComponentIndex() const;

  /*
  Returns:
    True if m_type = extrusion_bottom_profile, extrusion_top_profile,
    extrusion_wall_edge, extrusion_wall_surface, extrusion_cap_surface
    or extrusion_path and m_index is reasonable.
  */
  bool IsExtrusionComponentIndex() const;

  /*
  Returns:
    True if m_type = pointcloud_point and m_index >= 0.
  */
  bool IsPointCloudComponentIndex() const;

  /*
  Returns:
    True if m_type = dim_... and m_index >= 0.
  */
  bool IsAnnotationComponentIndex() const;

  /*
  Returns:
  True if m_type = hatch_loop and m_index >= 0.
  */
  bool IsHatchLoopComponentIndex() const;

  void Dump( 
    class MYON_TextLog& text_log 
    )const;

  void AppendToString( 
    class MYON_String& s
    )const;

  void AppendToString( 
    class MYON_wString& s
    )const;

  
  TYPE m_type;

  /*
  The interpretation of m_index depends on the m_type value.

    m_type             m_index interpretation (0 based indices)

    no_type            used when context makes it clear what array is being index
    brep_vertex        MYON_Brep.m_V[] array index
    brep_edge          MYON_Brep.m_E[] array index
    brep_face          MYON_Brep.m_F[] array index
    brep_trim          MYON_Brep.m_T[] array index
    brep_loop          MYON_Brep.m_L[] array index
    mesh_vertex        MYON_Mesh.m_V[] array index
    meshtop_vertex     MYON_MeshTopology.m_topv[] array index
    meshtop_edge       MYON_MeshTopology.m_tope[] array index
    mesh_face          MYON_Mesh.m_F[] array index
    mesh_ngon          MYON_Mesh.Ngon() array index
    idef_part          MYON_InstanceDefinition.m_object_uuid[] array index
    polycurve_segment  MYON_PolyCurve::m_segment[] array index

    extrusion_bottom_profile  Use MYON_Extrusion::Profile3d() to get 3d profile curve
    extrusion_top_profile     Use MYON_Extrusion::Profile3d() to get 3d profile curve
    extrusion_wall_edge       Use MYON_Extrusion::WallEdge() to get 3d line curve
    extrusion_wall_surface    Use MYON_Extrusion::WallSurface() to get 3d wall surface
    extrusion_cap_surface      0 = bottom cap, 1 = top cap
    extrusion_path            -1 = entire path, 0 = start of path, 1 = end of path

    hatch_loop         MYON_Hatch::m_loops[] array index  

    dim_linear_point   linear dimension point index
    dim_radial_point   radial dimension point index
    dim_angular_point  angular dimension point index
    dim_ordinate_point ordinate dimension point index
    dim_text_point     annotation text object point
  */

  unsigned int UnsignedIndex() const
  {
    return (unsigned int)m_index;
  }

  int m_index;
};

class MYON_CLASS MYON_ComponentIndexAndNumber
{
public:
  MYON_ComponentIndexAndNumber() = default;
  ~MYON_ComponentIndexAndNumber() = default;
  MYON_ComponentIndexAndNumber(const MYON_ComponentIndexAndNumber&) = default;
  MYON_ComponentIndexAndNumber& operator=(const MYON_ComponentIndexAndNumber&) = default;

public:
  static const MYON_ComponentIndexAndNumber UnsetAndNan;
  static const MYON_ComponentIndexAndNumber UnsetAndZero;
  static const MYON_ComponentIndexAndNumber Create(
    MYON_COMPONENT_INDEX ci,
    double x
  );

public:

  /*
  Description:
    Compare Component() using MYON_COMPONENT_INDEX::Compare().
  */
  static int CompareComponent(
    const MYON_ComponentIndexAndNumber* a,
    const MYON_ComponentIndexAndNumber* b
  );


  /*
  Description:
    Compare Number() nans are treated as equal and sort last.
  */
  static int CompareNumber(
    const MYON_ComponentIndexAndNumber* a,
    const MYON_ComponentIndexAndNumber* b
  );

  /*
  Description:
    Dictionary compare Component() 1st and Number() 2nd.
  */
  static int CompareComponentAndNumber(
    const MYON_ComponentIndexAndNumber* a,
    const MYON_ComponentIndexAndNumber* b
  );


public:
  const MYON_COMPONENT_INDEX Component() const;
  void SetComponent(MYON_COMPONENT_INDEX ci);

  double Number() const;
  void SetNumber(double x);

public:
  MYON_COMPONENT_INDEX m_ci = MYON_COMPONENT_INDEX::UnsetComponentIndex;
  double m_x = MYON_DBL_QNAN;
};

#endif

MYON_BEGIN_EXTERNC

// on_wcsicmp() is a wrapper for case insensitive wide string compare
// and calls one of _wcsicmp() or wcscasecmp() depending on OS.
MYON_DECL
int on_wcsicmp( const wchar_t*, const wchar_t* );

// on_wcsupr() calls _wcsupr() or wcsupr() depending on OS
MYON_DECL
wchar_t* on_wcsupr(wchar_t*);

// on_wcslwr() calls _wcslwr() or wcslwr() depending on OS
MYON_DECL
wchar_t* on_wcslwr(wchar_t*);

// on_wcsrev() calls _wcsrev() or wcsrev() depending on OS
MYON_DECL
wchar_t* on_wcsrev(wchar_t*);

// on_stricmp() is a wrapper for case insensitive string compare
// and calls one of _stricmp(), stricmp(), or strcasecmp()
// depending on OS.
MYON_DECL
int on_stricmp(const char*, const char*); 

// on_stricmp() is a wrapper for case insensitive string compare
// and calls one of _strnicmp() or strncasecmp()
// depending on OS.
MYON_DECL
int on_strnicmp(const char * s1, const char * s2, int n);

// on_strupr() calls _strupr() or strupr() depending on OS
MYON_DECL
char* on_strupr(char*);

// on_strlwr() calls _strlwr() or strlwr() depending on OS
MYON_DECL
char* on_strlwr(char*);

// on_strrev() calls _strrev() or strrev() depending on OS
MYON_DECL
char* on_strrev(char*);

/*
Description:
  Calls MYON_ConvertWideCharToUTF8()
*/
MYON_DECL
int on_WideCharToMultiByte(
    const wchar_t*, // lpWideCharStr,
    int,            // cchWideChar,
    char*,          // lpMultiByteStr,
    int             // cchMultiByte,
    );

/*
Description:
  Calls MYON_ConvertUTF8ToWideChar()
*/
MYON_DECL
int on_MultiByteToWideChar(
    const char*, // lpMultiByteStr,
    int,         // cchMultiByte,
    wchar_t*,    // lpWideCharStr,
    int          // cchWideChar
    );

/*
Description:
  Find the locations in a path the specify the drive, directory,
  file name and file extension.
Parameters:
  path - [in]
    UTF-8 encoded string that is a legitimate path to a file.
  volume - [out] (pass null if you don't need the volume)
    If volume is not null and the path parameter begins with 
    a Windows volum specification, the value of *volume will 
    equal the input value of path.  Otherwise *volume will be nullptr.    
    A Windows volume specification can be either a single A-Z or a-z
    letter followed by a colon ( C: ) or a Windows UNC host name
    (\\MY_SERVER).
  dir - [out] (pass null if you don't need the directory)
    If dir is not null and the path parameter contains a
    directory specification, then the returned value of *dir
    will point to the character in path where the directory
    specification begins.
  fname - [out] (pass null if you don't need the file name)
    If fname is not null and the path parameter contains a
    file name specification, then the returned value of *fname
    will point to the character in path where the file name
    specification begins.
  ext - [out] (pass null if you don't need the extension)
    If ext is not null and the path parameter contains a
    file extension specification, then the returned value of
    *ext will point to the '.' character in path where the file
    extension specification begins.
Remarks:
  This function will treat a front slash ( / ) and a back slash
  ( \ ) as directory separators.  Because this function parses
  file names store in .3dm files and the .3dm file may have been
  written on a Windows computer and then read on a another
  computer, it looks for a volume specification even when the
  operating system is not Windows.
  This function will not return an directory that does not
  end with a trailing slash.
  This function will not return an empty filename and a non-empty
  extension.
  This function parses the path string according to these rules.
  It does not check the actual file system to see if the answer
  is correct.
See Also:
  MYON_String::SplitPath
*/
MYON_DECL void on_splitpath(
  const char* path,
  const char** volume,
  const char** dir,
  const char** fname,
  const char** ext
  );

/*
Description:
  Find the locations in a path the specify the drive, directory,
  file name and file extension.
Parameters:
  path - [in]
    A legitimate file system path to a file.
  volume - [out] (pass null if you don't need the volume)
    If volume is not null and the path parameter begins with 
    a Windows volum specification, the value of *volume will 
    equal the input value of path.  Otherwise *volume will be nullptr.    
    A Windows volume specification can be either a single A-Z or a-z
    letter followed by a colon ( C: ) or a Windows UNC host name
    (\\MY_SERVER).
  dir - [out] (pass null if you don't need the directory)
    If dir is not null and the path parameter contains a
    directory specification, then the returned value of *dir
    will point to the character in path where the directory
    specification begins.
  fname - [out] (pass null if you don't need the file name)
    If fname is not null and the path parameter contains a
    file name specification, then the returned value of *fname
    will point to the character in path where the file name
    specification begins.
  ext - [out] (pass null if you don't need the extension)
    If ext is not null and the path parameter contains a
    file extension specification, then the returned value of
    *ext will point to the '.' character in path where the file
    extension specification begins.
Remarks:
  This function will treat a front slash ( / ) and a back slash
  ( \ ) as directory separators.  Because this function parses
  file names store in .3dm files and the .3dm file may have been
  written on a Windows computer and then read on a another
  computer, it looks for a volume specification even when the
  operating system is not Windows.
  This function will not return an directory that does not
  end with a trailing slash.
  This function will not return an empty filename and a non-empty
  extension.
  This function parses the path string according to these rules.
  It does not check the actual file system to see if the answer
  is correct.
See Also:
  MYON_wString::SplitPath
*/
MYON_DECL void on_wsplitpath(
  const wchar_t* path,
  const wchar_t** volume,
  const wchar_t** dir,
  const wchar_t** fname,
  const wchar_t** ext
  );

MYON_END_EXTERNC

#endif

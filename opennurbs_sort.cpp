#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

/*
If the speed of MYON_qsort() functions on arrays that
are nearly sorted is as good as heap sort, then define
MYON__QSORT_FASTER_THAN_HSORT.
*/

#define MYON_COMPILING_OPENNURBS_SORT_CPP

#if defined(MYON_RUNTIME_WIN) && defined(MYON_COMPILER_MSC)

#pragma optimize("t", on)

// have a reliable thread safe CRT qsort.
#define MYON__HAVE_RELIABLE_SYSTEM_QSORT
#define MYON__QSORT_FASTER_THAN_HSORT

#elif defined(_GNU_SOURCE)

// have a reliable thread safe CRT qsort.
#define MYON__HAVE_RELIABLE_SYSTEM_QSORT
#define MYON__QSORT_FASTER_THAN_HSORT

#endif

#if defined(MYON_RUNTIME_WIN) && defined(MYON_COMPILER_MSC)

// have a reliable thread safe CRT qsort with context that is
// faster than the functions below.
#define MYON__HAVE_RELIABLE_SYSTEM_CONTEXT_QSORT
#define MYON__QSORT_FASTER_THAN_HSORT

#elif defined(MYON_COMPILER_CLANG)

#define MYON__HAVE_RELIABLE_SYSTEM_CONTEXT_QSORT
#define MYON__QSORT_FASTER_THAN_HSORT

#elif defined(_GNU_SOURCE)

#define MYON__HAVE_RELIABLE_SYSTEM_CONTEXT_QSORT
#define MYON__QSORT_FASTER_THAN_HSORT

#endif

#define work_size 64

void 
MYON_qsort( void *base, size_t nel, size_t width, int (*compar)(void*,const void *, const void *),void* context)
{
#if defined(MYON__HAVE_RELIABLE_SYSTEM_CONTEXT_QSORT)
  // The call here must be a thread safe system qsort
  // that is faster than the alternative code in this function.
  // In particular, if it uses a random number generator to
  // find pivots, that calculation must be thread safe.
#if defined(MYON_COMPILER_MSC)
  qsort_s(base,nel,width,compar,context);
#elif defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  MYON_hsort(base, nel, width, compar, context);
#elif defined(MYON_COMPILER_CLANG)
  qsort_r(base,nel,width,context,compar);
#elif defined(_GNU_SOURCE)
  qsort_r(base,nel,width,context,compar);
#endif
#else
  MYON_hsort(base, nel, width, compar, context);
#endif
}

void 
MYON_qsort( void *base, size_t nel, size_t width, int (*compar)(const void *, const void *))
{
#if defined(MYON__HAVE_RELIABLE_SYSTEM_QSORT)
  // The call here must be a thread safe system qsort
  // that is faster than the alternative code in this function.
  // In particular, if it uses a random number generator to
  // find pivots, that calculation must be thread safe.
  qsort(base,nel,width,compar);
#else
  MYON_hsort(base, nel, width, compar);
#endif
}

void
MYON_hsort(void *base, size_t nel, size_t width, int (*compar)(const void*,const void*))
{
  size_t
    i_end,k;
  unsigned char
    work_memory[work_size], *e_tmp, *e_end;

  if (nel < 2) return;
  k = nel >> 1;
  i_end = nel-1;
  e_end = ((unsigned char*)base) + i_end*width;
  e_tmp = (width > work_size) ? (unsigned char*)onmalloc(width) : work_memory;
  for (;;) {
    if (k) {
      --k;
      memcpy(e_tmp,((unsigned char*)base)+k*width,width); /* e_tmp = e[k]; */
    } 
    else {      
      memcpy(e_tmp,e_end,width);     /* e_tmp = e[i_end]; */
      memcpy(e_end,base,width);      /* e[i_end] = e[0];  */
      if (!(--i_end)) {
        memcpy(base,e_tmp,width);    /* e[0] = e_tmp;     */
        break;
      }
      e_end -= width;
    }
    { size_t i, j;
      unsigned char *e_i, *e_j;
      i = k;
      j = (k<<1) + 1;
      e_i = ((unsigned char*)base) + i*width;
      while (j <= i_end) {
        e_j = ((unsigned char*)base) + j*width;
        if (j < i_end && compar(e_j,e_j+width)<0 /*e[j] < e[j + 1] */)
          {j++; e_j += width;}
        if (compar(e_tmp,e_j)<0 /* e_tmp < e[j] */) {
          memcpy(e_i,e_j,width); /* e[i] = e[j]; */
          i = j;
          e_i = e_j;
          j = (j<<1) + 1;
        } else j = i_end + 1;
      }
      memcpy(e_i,e_tmp,width); /* e[i] = e_tmp; */
    }
  }
  if (width > work_size) onfree(e_tmp); 
}

void
MYON_hsort(void *base, size_t nel, size_t width, int (*compar)(void*,const void*,const void*), void* context)
{
  size_t
    i_end,k;
  unsigned char
    work_memory[work_size], *e_tmp, *e_end;

  if (nel < 2) return;
  k = nel >> 1;
  i_end = nel-1;
  e_end = ((unsigned char*)base) + i_end*width;
  e_tmp = (width > work_size) ? (unsigned char*)onmalloc(width) : work_memory;
  for (;;) {
    if (k) {
      --k;
      memcpy(e_tmp,((unsigned char*)base)+k*width,width); /* e_tmp = e[k]; */
    } 
    else {      
      memcpy(e_tmp,e_end,width);     /* e_tmp = e[i_end]; */
      memcpy(e_end,base,width);      /* e[i_end] = e[0];  */
      if (!(--i_end)) {
        memcpy(base,e_tmp,width);    /* e[0] = e_tmp;     */
        break;
      }
      e_end -= width;
    }
    { size_t i, j;
      unsigned char *e_i, *e_j;
      i = k;
      j = (k<<1) + 1;
      e_i = ((unsigned char*)base) + i*width;
      while (j <= i_end) {
        e_j = ((unsigned char*)base) + j*width;
        if (j < i_end && compar(context,e_j,e_j+width)<0 /*e[j] < e[j + 1] */)
          {j++; e_j += width;}
        if (compar(context,e_tmp,e_j)<0 /* e_tmp < e[j] */) {
          memcpy(e_i,e_j,width); /* e[i] = e[j]; */
          i = j;
          e_i = e_j;
          j = (j<<1) + 1;
        } else j = i_end + 1;
      }
      memcpy(e_i,e_tmp,width); /* e[i] = e_tmp; */
    }
  }
  if (width > work_size) onfree(e_tmp); 
}

#undef work_size  

#define MYON_COMPILING_OPENNURBS_QSORT_FUNCTIONS
#define MYON_COMPILING_OPENNURBS_HSORT_FUNCTIONS
#define MYON_SORT_TEMPLATE_STATIC_FUNCTION

#define MYON_SORT_TEMPLATE_TYPE double
#define MYON_QSORT_FNAME MYON_qsort_double
#define MYON_HSORT_FNAME MYON_hsort_double
#include "opennurbs_qsort_template.h"
#include "opennurbs_hsort_template.h"
#undef MYON_SORT_TEMPLATE_TYPE
#undef MYON_QSORT_FNAME
#undef MYON_HSORT_FNAME

#define MYON_SORT_TEMPLATE_TYPE double
#define MYON_QSORT_FNAME MYON_qsort_double_decreasing
#define MYON_QSORT_GT(A,B) *A < *B
#define MYON_QSORT_LE(A,B) *A >= *B
#define MYON_QSORT_EQ(A,B) *A == *B
#undef MYON_QSORT_SHORT_SORT_FNAME
#define MYON_QSORT_SHORT_SORT_FNAME MYON__shortsort_double_decreasing
#include "opennurbs_qsort_template.h"
#undef MYON_SORT_TEMPLATE_TYPE
#undef MYON_QSORT_FNAME

void MYON_SortDoubleArray( 
        MYON::sort_algorithm sort_algorithm,
        double* a,
        size_t nel
        )
{
  if ( MYON::sort_algorithm::heap_sort == sort_algorithm )
    MYON_hsort_double(a,nel);
  else
    MYON_qsort_double(a,nel);
}

void MYON_SortDoubleArrayIncreasing(
  double* a,
  size_t nel
  )
{
  MYON_qsort_double(a, nel);
}

MYON_DECL
void MYON_SortDoubleArrayDecreasing(
  double* a,
  size_t nel
  )
{
  MYON_qsort_double_decreasing(a, nel);
}

#define MYON_SORT_TEMPLATE_TYPE float
#define MYON_QSORT_FNAME MYON_qsort_float
#define MYON_HSORT_FNAME MYON_hsort_float
#include "opennurbs_qsort_template.h"
#include "opennurbs_hsort_template.h"
#undef MYON_SORT_TEMPLATE_TYPE
#undef MYON_QSORT_FNAME
#undef MYON_HSORT_FNAME

void MYON_SortFloatArray( 
        MYON::sort_algorithm sort_algorithm,
        float* a,
        size_t nel
        )
{
  if ( MYON::sort_algorithm::heap_sort == sort_algorithm )
    MYON_hsort_float(a,nel);
  else
    MYON_qsort_float(a,nel);
}


#define MYON_SORT_TEMPLATE_TYPE int
#define MYON_QSORT_FNAME MYON_qsort_int
#define MYON_HSORT_FNAME MYON_hsort_int
#include "opennurbs_qsort_template.h"
#include "opennurbs_hsort_template.h"
#undef MYON_SORT_TEMPLATE_TYPE
#undef MYON_QSORT_FNAME
#undef MYON_HSORT_FNAME

void MYON_SortIntArray(
        MYON::sort_algorithm sort_algorithm,
        int* a,
        size_t nel
        )
{
  if ( MYON::sort_algorithm::heap_sort == sort_algorithm )
    MYON_hsort_int(a,nel);
  else
    MYON_qsort_int(a,nel);
}


#define MYON_SORT_TEMPLATE_TYPE unsigned int
#define MYON_QSORT_FNAME MYON_qsort_uint
#define MYON_HSORT_FNAME MYON_hsort_uint
#include "opennurbs_qsort_template.h"
#include "opennurbs_hsort_template.h"
#undef MYON_SORT_TEMPLATE_TYPE
#undef MYON_QSORT_FNAME
#undef MYON_HSORT_FNAME

void MYON_SortUnsignedIntArray(
        MYON::sort_algorithm sort_algorithm,
        unsigned int* a,
        size_t nel
        )
{
  if ( MYON::sort_algorithm::heap_sort == sort_algorithm )
    MYON_hsort_uint(a,nel);
  else
    MYON_qsort_uint(a,nel);
}


#define MYON_SORT_TEMPLATE_TYPE MYON__UINT64
#define MYON_QSORT_FNAME MYON_qsort_uint64
#define MYON_HSORT_FNAME MYON_hsort_uint64
#include "opennurbs_qsort_template.h"
#include "opennurbs_hsort_template.h"
#undef MYON_SORT_TEMPLATE_TYPE
#undef MYON_QSORT_FNAME
#undef MYON_HSORT_FNAME

void MYON_SortUINT64Array(
        MYON::sort_algorithm sort_algorithm,
        MYON__UINT64* a,
        size_t nel
        )
{
  if ( MYON::sort_algorithm::heap_sort == sort_algorithm )
    MYON_hsort_uint64(a,nel);
  else
    MYON_qsort_uint64(a,nel);
}


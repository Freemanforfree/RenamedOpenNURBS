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

#if !defined(MYON_STRING_INC_)
#define MYON_STRING_INC_

/*
Description:
  Sort an index array.
Parameters
  method - [in]
    MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort.
    Use MYON::sort_algorithm::heap_sort only after doing meaningful performance
    testing using optimized release builds that demonstrate
    MYON::sort_algorithm::heap_sort is significantly better.
  index - [out] 
    Pass in an array of count integers.  The returned
    index[] is a permutation of (0,1,..,count-1)
    such that compare(B[index[i]],B[index[i+1]) <= 0
    where B[i] = base + i*sizeof_element
  base - [in]
    array of count elements
  count - [in]
    number of elements in the index[] and base[] arrays
  sizeof_element - [in]
    number of bytes between consecutive elements in the
    base[] array.
  compare - [in]
    Comparison function a la qsort().
*/
MYON_DECL
void MYON_Sort( 
  MYON::sort_algorithm method,
  int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int (*compare)(const void*,const void*) // int compar(const void*,const void*)
  );

MYON_DECL
void MYON_Sort(
  MYON::sort_algorithm method,
  unsigned int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int(*compare)(const void*, const void*) // int compar(const void*,const void*)
  );


/*
Description:
  Sort an index array using a compare function
  that takes an additional pointer that can be used to
  pass extra information.
Parameters
  method - [in]
    MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort.
    Use MYON::sort_algorithm::heap_sort only after doing meaningful performance
    testing using optimized release builds that demonstrate
    MYON::sort_algorithm::heap_sort is significantly better.
  index - [out] 
    Pass in an array of count integers.  The returned
    index[] is a permutation of (0,1,..,count-1)
    such that compare(B[index[i]],B[index[i+1]) <= 0
    where B[i] = base + i*sizeof_element
  base - [in]
    array of count elements
  count - [in]
    number of elements in the index[] and base[] arrays
  sizeof_element - [in]
    number of bytes between consecutive elements in the
    base[] array.
  compare - [in]
    Comparison function a la qsort().  The context parameter
    is pass as the third argument.
  context - [in]
    pointer passed as the third argument to compare().
*/
MYON_DECL
void MYON_Sort( 
  MYON::sort_algorithm method,
  int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int (*compare)(const void*,const void*,void*), // int compar(const void* a,const void* b, void* ptr)
  void* context
  );

MYON_DECL
void MYON_Sort(
  MYON::sort_algorithm method,
  unsigned int* index,
  const void* base,
  size_t count,
  size_t sizeof_element,
  int(*compare)(const void*, const void*, void*), // int compar(const void* a,const void* b, void* ptr)
  void* context
  );

/*
Description:
  Various sorts. When in doubt, use MYON_qsort().
  MYON_qsort - quick sort.
  MYON_hsort = hearp sort.
Parameters
  base - [in]
    array of count elements
  count - [in]
    number of elements in the index[] and base[] arrays
  sizeof_element - [in]
    number of bytes between consecutive elements in the
    base[] array.
  compare - [in]
    Comparison function a la qsort().  The context parameter
    is pass as the third argument.
  context - [in]
    pointer passed as the third argument to compare().
Remarks:
  As a rule, use quick sort unless extensive tests in your case
  prove that heap sort is faster. 
  
  This implementation of quick sort is generally faster than 
  heap sort, even when the input arrays are nearly sorted.
  The only common case when heap sort is faster occurs when
  the arrays are strictly "chevron" (3,2,1,2,3) or "carat" 
  (1,2,3,2,1) ordered, and in these cases heap sort is about
  50% faster.  If the "chevron" or "caret" ordered arrays 
  have a little randomness added, the two algorithms have 
  the same speed.
*/
MYON_DECL
void MYON_hsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(const void*,const void*)
        );

MYON_DECL
void MYON_qsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(const void*,const void*)
        );

MYON_DECL
void MYON_hsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(void*,const void*,const void*),
        void* context
        );

MYON_DECL
void MYON_qsort( 
        void* base,
        size_t count,
        size_t sizeof_element,
        int (*compare)(void*,const void*,const void*),
        void* context
        );

/*
Description:
  Sort an array of doubles in increasing order in place.
Parameters:
  sort_algorithm - [in]  
    MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
    Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
    a[] cannot contain NaNs.
  nel - [in]
    length of array a[]
*/
MYON_DECL
void MYON_SortDoubleArray(
  MYON::sort_algorithm sort_algorithm,
  double* a,
  size_t nel
  );


/*
Description:
  Sort an array of doubles in increasing order in place.
Parameters:
  a - [in / out]
    The values in a[] are sorted so that a[i] <= a[i+1].
    a[] cannot contain NaNs.
  nel - [in]
    length of array a[]
*/
MYON_DECL
void MYON_SortDoubleArrayIncreasing(
  double* a,
  size_t nel
  );

/*
Description:
  Sort an array of doubles in increasing order in place.
Parameters:
  a - [in / out]
    The values in a[] are sorted so that a[i] >= a[i+1].
    a[] cannot contain NaNs.
  nel - [in]
    length of array a[]
*/
MYON_DECL
void MYON_SortDoubleArrayDecreasing(
  double* a,
  size_t nel
  );

/*
Description:
  Sort an array of ints in place.
Parameters:
  sort_algorithm - [in]  
    MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
    Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
  nel - [in]
    length of array a[]
*/
MYON_DECL
void MYON_SortIntArray(
        MYON::sort_algorithm sort_algorithm,
        int* a,
        size_t nel
        );

/*
Description:
  Sort an array of unsigned ints in place.
Parameters:
  sort_algorithm - [in]  
    MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
    Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
  nel - [in]
    length of array a[]
*/
MYON_DECL
void MYON_SortUnsignedIntArray(
        MYON::sort_algorithm sort_algorithm,
        unsigned int* a,
        size_t nel
        );

/*
Description:
  Sort an array of unsigned 64-bit ints in place.
Parameters:
  sort_algorithm - [in]  
    MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
    Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that a[i] <= a[i+1].
  nel - [in]
    length of array a[]
*/
MYON_DECL
void MYON_SortUINT64Array(
  MYON::sort_algorithm sort_algorithm,
  MYON__UINT64* a,
  size_t nel
);



/*
Description:
  Sort an array of unsigned null terminated char strings in place.
Parameters:
  sort_algorithm - [in]  
    MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
    Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
    optimized release builds and are confident heap sort is 
    significantly faster in your case.
  a - [in / out] 
    The values in a[] are sorted so that strcmp(a[i],a[i+1]) <= 0.
  nel - [in]
    length of array a[]
*/
MYON_DECL
void MYON_SortStringArray(
        MYON::sort_algorithm sort_algorithm,
        char** a,
        size_t nel
        );

MYON_DECL
const int* MYON_BinarySearchIntArray( 
          int key, 
          const int* base, 
          size_t nel
          );

MYON_DECL
const unsigned int* MYON_BinarySearchUnsignedIntArray( 
          unsigned int key, 
          const unsigned int* base, 
          size_t nel
          );

MYON_DECL
const void* MYON_BinarySearchArrayForUnsingedInt(
  unsigned int key, 
  const void* base, 
  size_t count,
  size_t sizeof_element,
  size_t key_offset
  );

MYON_DECL
const void* MYON_BinarySearchArrayFirstUnsignedInt(
  unsigned int key,
  const void* base,
  size_t count,
  size_t sizeof_element,
  size_t key_offset
);

MYON_DECL
const void* MYON_BinarySearchArrayFirst2udex(
  MYON_2udex key,
  const void* base,
  size_t count,
  size_t sizeof_element,
  size_t key_offset
);


MYON_DECL
const double* MYON_BinarySearchDoubleArray( 
          double key, 
          const double* base, 
          size_t nel
          );

/*
  This class is intended to be used to determine if a file's
  contents have changed.
*/
class MYON_CLASS MYON_CheckSum
{
public:
  MYON_CheckSum();
  ~MYON_CheckSum();

  static const MYON_CheckSum UnsetCheckSum;

  // zeros all fields.
  void Zero();

  /*
  Returns:
    True if checksum is set.
  */
  bool IsSet() const;

  // C++ default operator=, operator==,
  // and copy constructor work fine.

  /*
  Description:
    Set check sum values for a buffer
  Parameters:
    size - [in] 
      number of bytes in buffer
    buffer - [in]  
    time - [in]
      last modified time in seconds since Jan 1, 1970, UCT
  Returns:
    True if checksum is set.
  */
  bool SetBufferCheckSum( 
    size_t size, 
    const void* buffer,
    time_t time
   );

  /*
  Description:
    Set check sum values for a file.
  Parameters:
    fp - [in] pointer to a file opened with MYON:FileOpen(...,"rb")
  Returns:
    True if checksum is set.
  */
  bool SetFileCheckSum( 
    FILE* fp
   );

  /*
  Description:
    Set check sum values for a file.
  Parameters:
    filename - [in] name of file.
  Returns:
    True if checksum is set.
  */
  bool SetFileCheckSum( 
    const wchar_t* filename
   );

  /*
  Description:
    Test buffer to see if it has a matching checksum.
  Parameters:
    size - [in]   size in bytes
    buffer - [in]
  Returns:
    True if the buffer has a matching checksum.
  */
  bool CheckBuffer( 
    size_t size, 
    const void* buffer
    ) const;

  /*
  Description:
    Test buffer to see if it has a matching checksum.
  Parameters:
    fp - [in] pointer to file opened with MYON::OpenFile(...,"rb")
    bSkipTimeCheck - [in] if true, the time of last
       modification is not checked.
  Returns:
    True if the file has a matching checksum.
  */
  bool CheckFile( 
    FILE* fp,
    bool bSkipTimeCheck = false
    ) const;

  /*
  Description:
    Test buffer to see if it has a matching checksum.
  Parameters:
    filename - [in]
    bSkipTimeCheck - [in] if true, the time of last
       modification is not checked.
  Returns:
    True if the file has a matching checksum.
  */
  bool CheckFile( 
    const wchar_t* filename,
    bool bSkipTimeCheck = false
    ) const;

  bool Write(class MYON_BinaryArchive&) const;
  bool Read(class MYON_BinaryArchive&);

  void Dump(class MYON_TextLog&) const;

public:
  size_t     m_size;   // bytes in the file.
  time_t     m_time;   // last modified time in seconds since Jan 1, 1970, UCT
  MYON__UINT32 m_crc[8]; // crc's
};


/*
Description:
  Get the length of a UTF-8 encoded char string.
Parameters:
  string - [in]
    null terminated char string.
Returns:
  Number of nonzero char elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
MYON_DECL
int MYON_StringLengthUTF8(
  const char* string
  );

/*
Description:
  Get the length of a UTF-16 encoded MYON__UINT16 string.
Parameters:
  string - [in]
    null terminated MYON__UINT16 string.
Returns:
  Number of nonzero MYON__UINT16 elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
MYON_DECL
int MYON_StringLengthUTF16(
  const MYON__UINT16* string
  );

/*
Description:
  Get the length of a UTF-32 encoded MYON__UINT32 string.
Parameters:
  string - [in]
    null terminated MYON__UINT32 string.
Returns:
  Number of nonzero MYON__UINT32 elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
MYON_DECL
int MYON_StringLengthUTF32(
  const MYON__UINT32* string
  );

/*
Description:
  Get the length of a wchar_t string.
Parameters:
  string - [in]
    null terminated wchar_t string.
Returns:
  Number of nonzero wchar_t elements before the null terminator.
  If string is nullptr, then 0 is returned.
*/
MYON_DECL
int MYON_StringLengthWideChar(
  const wchar_t* string
  );

/*
Description:
  Get the length of a UTF-8 encoded char string.
Parameters:
  string - [in]
    null terminated char string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
MYON_DECL
int MYON_StringLengthUTF8(
  const char* string,
  size_t string_capacity
  );

/*
Description:
  Get the length of a UTF-16 encoded MYON__UINT16 string.
Parameters:
  string - [in]
    null terminated MYON__UINT16 string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
MYON_DECL
int MYON_StringLengthUTF16(
  const MYON__UINT16* string,
  size_t string_capacity
  );

/*
Description:
  Get the length of a UTF-32 encoded MYON__UINT32 string.
Parameters:
  string - [in]
    null terminated MYON__UINT32 string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
MYON_DECL
int MYON_StringLengthUTF32(
  const MYON__UINT32* string,
  size_t string_capacity
  );

/*
Description:
  Get the length of a wchar_t string.
Parameters:
  string - [in]
    null terminated wchar_t string.
  string_capacity - [in]
    maximum number of string[] elements to test.
Returns:
  If string is nullptr or string_capacity <=0, then 0 is returned.
  If a null terminator is not found, then string_capacity is returned.
  Otherwise, the number of nonzero char elements before the null terminator is returned.
*/
MYON_DECL
int MYON_StringLengthWideChar(
  const wchar_t* string,
  size_t string_capacity
  );

/*
Description:
  Compare of UTF-8 encoded char strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     MYON_StringCompareOrdinalUTF8, MYON_StringCompareOrdinalUTF16 and MYON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
  4) This function does not compare Unicode code point values the strings have eny elements with values > 127.
*/
MYON_DECL
int MYON_StringCompareOrdinalUTF8(
  const char* string1,
  int element_count1,
  const char* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/*
Description:
  Compare of UTF-16 encoded MYON__UINT16 strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     MYON_StringCompareOrdinalUTF8, MYON_StringCompareOrdinalUTF16 and MYON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
  4) This function does not compare Unicode code point values if the strings have any surrogate pairs.
*/
MYON_DECL
int MYON_StringCompareOrdinalUTF16(
  const MYON__UINT16* string1,
  int element_count1,
  const MYON__UINT16* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/*
Description:
  Compare of UTF-32 encoded MYON__UINT32 strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     MYON_StringCompareOrdinalUTF8, MYON_StringCompareOrdinalUTF16 and MYON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
*/
MYON_DECL
int MYON_StringCompareOrdinalUTF32(
  const MYON__UINT32* string1,
  int element_count1,
  const MYON__UINT32* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/*
Description:
  Compare wchar_t strings element by element.
Parameters:
  string1 - [in]
  element_count1 - [in]
    If element_count1 < 0, then string1 must be null terminated and element_count1
    will be set to the length of string1.
    If element_count1 >= 0, then that number of elements are compared in string1[].
  string2 - [in]
  element_count2 - [in]
    If element_count2 < 0, then string2 must be null terminated and element_count2
    will be set to the length of string2.
    If element_count2 >= 0, then that number of elements are compared in string2[].
  bOrdinalIgnoreCase - [in]
    If bOrdinalIgnoreCase, then letters with a capital and small codepoint value <= 127
    are compared using the smallest codepoint value.  This amounts to converting the 
    letters a-z to A-Z before comparing.
Returns:
  0: the strings are the same
  <0: string1 < string2
  >0: string1 > string2
Remarks:
  1) If linguistic issues are important, then this function is not appropriate.
  2) nullptr values are handled.
  3) There are lists of Unicode code point values such that the results of
     MYON_StringCompareOrdinalUTF8, MYON_StringCompareOrdinalUTF16 and MYON_StringCompareOrdinalUTF32
     are different when applied to UTF-8, UTF-16 and UTF-32 encodings.
  4) This function assumes the sizeof(wchar_t) is 1, 2 or 4,
     that 1 bytes wchar_t strings are UTF-8 encoded, 2 byte wchar_t strings are UTF-16 encoded
     and 4 bytes wchar_t strings are UTF-32 encoded.
*/
MYON_DECL
int MYON_StringCompareOrdinalWideChar(
  const wchar_t* string1,
  int element_count1,
  const wchar_t* string2,
  int element_count2,
  bool bOrdinalIgnoreCase
  );

/////////////////////////////////////////////////////////////////////////////
// 
// MYON_String is a UTF-8 char string on all platforms
// MYON_wString is a UTF-16 encoded wchar_t string on Windows platforms
// MYON_wString is a UTF-32 encoded wchar_t string on Windows platforms
//

class MYON_CLASS MYON_StringBuffer
{
public:
  MYON_StringBuffer();

  MYON_StringBuffer(
    char* stack_buffer,
    size_t stack_buffer_capacity
    );

  ~MYON_StringBuffer();

  bool GrowBuffer(
    size_t buffer_capacity
    );

  char* m_buffer;
  size_t m_buffer_capacity;

private:
  MYON_StringBuffer(const MYON_StringBuffer&);
  MYON_StringBuffer& operator=(const MYON_StringBuffer&);
  char* m_heap_buffer;
  size_t m_heap_buffer_capacity;
};

class MYON_CLASS MYON_wStringBuffer
{
public:
  MYON_wStringBuffer();

  MYON_wStringBuffer(
    wchar_t* stack_buffer,
    size_t stack_buffer_capacity
    );

  ~MYON_wStringBuffer();

  bool GrowBuffer(
    size_t buffer_capacity
    );

  wchar_t* m_buffer;
  size_t m_buffer_capacity;

private:
  MYON_wStringBuffer(const MYON_wStringBuffer&);
  MYON_wStringBuffer& operator=(const MYON_wStringBuffer&);
  wchar_t* m_heap_buffer;
  size_t m_heap_buffer_capacity;
};

MYON_DECL
MYON__UINT32 MYON_UnicodeMapCodePointOrdinal(
  MYON_StringMapOrdinalType map_type,
  MYON__UINT32 unicode_code_point
  );

MYON_DECL
MYON__UINT32 MYON_UnicodeMapCodePoint(
  const MYON_Locale& locale,
  MYON_StringMapType map_type,
  MYON__UINT32 unicode_code_point
  );

/*
Parameters:
  sUTF8 - [in]
    pointer to a UTF-8 encoded string.
  element_count - [in]
    number of char elements to parse.
    if element_count < 0, then sUTF8 must be null terminated
  mapping - [in]
    mapping to apply to unicode code points
Returns:
  A SHA-1 value of the sequence of unicode code points. This value is independent
  of platform endian or UTF encoding.
*/
const MYON_SHA1_Hash MYON_StringContentHash(
  const char* sUTF8,
  int element_count,
  MYON_StringMapOrdinalType mapping
);

/*
Parameters:
  sUTF8 - [in]
    pointer to a UTF-8 encoded string.
  element_count - [in]
    number of char elements to parse.
    if element_count < 0, then sUTF8 must be null terminated
  mapping - [in]
    mapping to apply to unicode code points
Returns:
  A SHA-1 value of the sequence of unicode code points. This value is independent
  of platform endian or UTF encoding.
*/
const MYON_SHA1_Hash MYON_StringContentHash(
  const wchar_t* sWideString,
  int element_count,
  MYON_StringMapOrdinalType mapping
);

/// <summary>
/// A char string. 
/// Any multibyte encoding can be used. If the encoding is unknown, assume it is UTF-8.
/// </summary>
class MYON_CLASS MYON_String
{
public:

// Constructors
	MYON_String() MYON_NOEXCEPT;
	MYON_String( const MYON_String& );

  enum : int
  {
    /// <summary>
    /// This MYON_String::MaximumStringLength value of 100,000,000
    /// is used for string length sanity checking in both MYON_String and
    /// MYON_wString.
    /// The design of the MYON_String and MYON_wString classes could support 
    /// string lengths up to 0xFFFFFFFEU = 4,294,967,294 but
    /// a cap of 100 million seems generous enough for current usage
    /// and is small enough to detect many corrupt memory
    /// situations.
    /// This value is used for both MYON_String and MYON_wString.
    /// </summary>
    MaximumStringLength = 100000000
  };

  /// <summary>
  /// MYON_String::EmptyString has length 0.
  /// const char* s = MYON_String::EmptyString sets s = &quot;&quot;;
  /// </summary>
  static const MYON_String EmptyString;

  /// <summary>
  /// Even though a char string has endian independent byte order, 
  /// it is valid for UTF-8 encoded text to begin with the UTF-8 encoding of U+FEFF. 
  /// A UTF-8 BOM is sometimes used to mark a char string as UTF-8 encoded.
  /// A UTF-8 BOM can occur when UTF-16 and UTF-32 encoded text with a byte 
  /// order mark is converted to UTF-8 encoded text. Conversely a UTF-8 BOM
  /// is sometimes used when UTF-8 encode text will be converted to UTF-16/UTF-32 
  /// encoded text and a BOM is desired in the result.
  /// </summary>
  static const MYON_String ByteOrderMark;

  /// <summary>BACKSPACE control U+0008</summary>
  static const char Backspace = (char)MYON_UnicodeCodePoint::MYON_Backspace;

  /// <summary>CHARACTER TABULATION control U+0009</summary>
  static const char Tab = (char)MYON_UnicodeCodePoint::MYON_Tab;

  /// <summary>LINE FEED control U+000A</summary>
  static const char LineFeed = (char)MYON_UnicodeCodePoint::MYON_LineFeed;

  /// <summary>LINE TABULATION control U+000B</summary>
  static const char VerticalTab = (char)MYON_UnicodeCodePoint::MYON_VerticalTab;

  /// <summary>FORM FEED control U+000C</summary>
  static const char FormFeed = (char)MYON_UnicodeCodePoint::MYON_FormFeed;

  /// <summary>CARRIAGE RETURN control U+000D</summary>
  static const char CarriageReturn = (char)MYON_UnicodeCodePoint::MYON_CarriageReturn;

  /// <summary>ESCAPE control U+001B</summary>
  static const char Escape = (char)MYON_UnicodeCodePoint::MYON_Escape;

  /// <summary>SPACE U+0020</summary>
  static const char Space = (char)MYON_UnicodeCodePoint::MYON_Space;

  /// <summary>QUOTATION MARK U+0022 (&quot;)</summary>
  static const char QuotationMark = (char)MYON_UnicodeCodePoint::MYON_QuotationMark;

  /// <summary>NUMBER SIGN U+0023 (#)</summary>
  static const char NumberSign = (char)MYON_UnicodeCodePoint::MYON_NumberSign;

  /// <summary>PERCENT SIGN U+0025 (%)</summary>
  static const char PercentSign = (char)MYON_UnicodeCodePoint::MYON_PercentSign;

  /// <summary>AMPERSAND U+0026 (&amp;)</summary>
  static const char Ampersand = (char)MYON_UnicodeCodePoint::MYON_Ampersand;

  /// <summary>APOSTROPHE U+0027 (&apos;)</summary>
  static const char Apostrophe = (char)MYON_UnicodeCodePoint::MYON_Apostrophe;

  /// <summary>COMMA U+002C (,)</summary>
  static const char Comma = (char)MYON_UnicodeCodePoint::MYON_Comma;

  /// <summary>HYPHEN-MINUS U+002D (-)</summary>
  static const char HyphenMinus = (char)MYON_UnicodeCodePoint::MYON_HyphenMinus;

  /// <summary>PERIOD U+002E (decimal 46) (.)</summary>
  static const char Period = (char)MYON_UnicodeCodePoint::MYON_Period;

  /// <summary>SOLIDUS U+002F (&#x2f;)</summary>
  static const char Slash = (char)MYON_UnicodeCodePoint::MYON_Slash;

  /// <summary>COLON U+003A (:)</summary>
  static const char Colon = (char)MYON_UnicodeCodePoint::MYON_Colon;

  /// <summary>SEMICOLON U+003B (;)</summary>
  static const char Semicolon = (char)MYON_UnicodeCodePoint::MYON_Semicolon;

  /// <summary>LESS-THAN SIGN U+003C (&#x3c;)</summary>
  static const char LessThanSign = (char)MYON_UnicodeCodePoint::MYON_LessThanSign;

  /// <summary>GREATER-THAN SIGN U+003E (&#x3e;)</summary>
  static const char GreaterThanSign = (char)MYON_UnicodeCodePoint::MYON_GreaterThanSign;

  /// <summary>REVERSE SOLIDUS U+005C (&#x5c;)</summary>
  static const char Backslash = (char)MYON_UnicodeCodePoint::MYON_Backslash;

  /// <summary>// Unicode LOW LINE U+005F (_)</summary>
  static const char Underscore = (char)MYON_UnicodeCodePoint::MYON_Underscore;

  /// <summary>VERTICAL LINE U+007C (&#x7c;)</summary>
  static const char Pipe = (char)MYON_UnicodeCodePoint::MYON_Pipe;

  /// <summary>TILDE U+007E (&#x7e;)</summary>
  static const char Tilde = (char)MYON_UnicodeCodePoint::MYON_Tilde;

  /// <summary>Period decimal point (.)</summary>
  static const char DecimalAsPeriod = (char)MYON_UnicodeCodePoint::MYON_Period;

  /// <summary>Comma decimal point (,)</summary>
  static const char DecimalAsComma = (char)MYON_UnicodeCodePoint::MYON_Comma;


  /*
  Parameters:
    c - [in]
  Returns:
    True if c is '0', '1', ..., '9', 'A', 'B', ..., 'F', 'a', 'b', ..., of 'f'.
  */
  static bool IsHexDigit(char c);

  /*
  Parameters:
    c - [in]
  Returns:
    True if c is '0', '1', ..., or '9'.
  */
  static bool IsDecimalDigit(char c);
   
  /*
  Description:
    Decode this char string using the encoding specified by windows_code_page
    to a UTF encoded wide character string.
  Parameters:
    windows_code_page - [in]
      WIndows code page. For example, big5 = 950.
  Returns:
    A UTF encoded wide character string.
  See Also:
    MYON_wString::MultiByteEncode()
  */
  const MYON_wString MultiByteDecode(int windows_code_page) const;

private:
  // Use IsEmpty() or IsNotEmpty() when you want a bool 
  // to test for the empty string.
  explicit operator bool() const { return IsNotEmpty(); }
public:

#if defined(MYON_HAS_RVALUEREF)
  // Clone constructor
  MYON_String( MYON_String&& ) MYON_NOEXCEPT;

  // Clone Assignment operator
  MYON_String& operator=( MYON_String&& ) MYON_NOEXCEPT;
#endif

	MYON_String( const char* );
	MYON_String( const char*, int /*length*/ );        // from substring
	MYON_String( char, int = 1 /* repeat count */ );   

	MYON_String( const unsigned char* );
	MYON_String( const unsigned char*, int /*length*/ );        // from substring
	MYON_String( unsigned char, int = 1 /* repeat count */ ); 
  
  // construct a UTF-8 string string from a UTF-16 string.
	MYON_String( const wchar_t* src );  // src = UTF-16 string
	MYON_String( const wchar_t* src, int length ); // from a UTF-16 substring
  MYON_String( const MYON_wString& src ); // src = UTF-16 string

#if defined(MYON_RUNTIME_WIN)
  // Windows support
	bool LoadResourceString( HINSTANCE, UINT); // load from Windows string resource
										                         // 2047 chars max
#endif
  
#if defined(MYON_RUNTIME_APPLE_CORE_TEXT_AVAILABLE)
  MYON_String(CFStringRef);
  
  CFStringRef ToAppleCFString() const;
#endif
  
  /*
  Parameters:
    bLengthTest - [in]
      true
        (common case) If this->Length() != MYON_wString::Length(static_cast<const wchar_t*)(this)), then this string is not valid.
      false
        (less common case) There may be null elements in the first this->Length()-1 chars.
  Returns:
    True - this is valid.
    False
      There is a serious memory corruption bug in your code.
      This was not valid and converted to MYON_wString::EmptyString to prevent future crashes.  */
  bool IsValid(
    bool bLengthTest
  ) const;

  void Create();
  void Destroy(); // releases any memory and initializes to default empty string
  void EmergencyDestroy();

  MYON_DEPRECATED_MSG("Obsolete - does nothing.")
  void EnableReferenceCounting( bool bEnable );

  /*
  Returns:
    True if the string is reference counted.
  */
  bool IsReferenceCounted() const;


  // Attributes & Operations

  /*
  Returns:
    number of char elements in string not including the null terminator.
  */
  int Length() const;

  /*
  Returns:
    number of char elements in string not including the null terminator.
  */
  unsigned int UnsignedLength() const;

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static int Length(
    const char* string
    );

  /*
  Parameters:
    string_capacity - [in]
      number of elements that can be read in string[] array.
  Returns:
    number of nonzero elements in string before the first null terminator
    or string_capacity if no null terminator is in string[].
    If string is nullptr, 0 is returned.
  */
  static int Length(
    const char* string,
    size_t string_capacity
  );

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static unsigned int UnsignedLength(
    const char* string
  );

  bool IsEmpty() const; // returns true if length == 0 
  bool IsNotEmpty() const; // returns true if length > 0
  void Empty();   // sets length to zero - if possible, memory is retained

  char& operator[](int);
  char operator[](int) const;
  char GetAt(int) const;
  void SetAt(int, char);
  void SetAt(int, unsigned char);

  operator const char* () const;

  // overloaded assignment
  MYON_String& operator=(const MYON_String&);
  MYON_String& operator=(char);
  MYON_String& operator=(const char*);
  MYON_String& operator=(unsigned char);
  MYON_String& operator=(const unsigned char*);
  MYON_String& operator=(const wchar_t* src); // src = Wide char string, result is a UTF-8 string
  MYON_String& operator=(const MYON_wString& src);  // src = Wide char string, result is a UTF-8 string

  // operator+()
  MYON_String operator+(const MYON_String&) const;
  MYON_String operator+(char) const;
  MYON_String operator+(unsigned char) const;
  MYON_String operator+(const char*) const;
  MYON_String operator+(const unsigned char*) const;

  // string concatenation
  void Append(const char*, int); // append specified number of characters
  void Append(const unsigned char*, int); // append specified number of characters
  const MYON_String& operator+=(const MYON_String&);
  const MYON_String& operator+=(char);
  const MYON_String& operator+=(unsigned char);
  const MYON_String& operator+=(const char*);
  const MYON_String& operator+=(const unsigned char*);

  MYON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
    int Compare(const char*) const;

  MYON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
    int Compare(const unsigned char*) const;

  MYON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
    int CompareNoCase(const char*) const;

  MYON_DEPRECATED_MSG("Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate")
    int CompareNoCase(const unsigned char*) const;

  bool Equal(
    const MYON_String& other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
  ) const;

  bool Equal(
    const char* other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    ) const;

  static bool Equal(
    const char* string1,
    const char* string2,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    );

  static bool Equal(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    const class MYON_Locale& locale,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_string by normalizing (NFC) 
    and using invariant culture ordering.
  Parameters:
    other_string - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  int Compare(
    const MYON_String& other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    ) const;
 
  int Compare(
    const char* other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    ) const;
 
  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    string2 - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  static int Compare(
    const char* string1,
    const char* string2,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of string1[] elements to compare.
      If element_count1 < 0, then string1 must be null terminated.
    string2 - [in]
    element_count2 - [in]
      The number of string2[] elements to compare.
      If element_count2 < 0, then string2 must be null terminated.
    locale - [in]
      Typically MYON_Locale::Ordinal, MYON_Locale::InvariantCulture, or
      MYON_Locale::CurrentCulture.
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */
  static int Compare(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    );

  bool EqualOrdinal(
    const MYON_String& other_string,
    bool bOrdinalIgnoreCase
    ) const;

  bool EqualOrdinal(
    const char* other_string,
    bool bOrdinalIgnoreCase
    ) const;

  static bool EqualOrdinal(
    const char* string1,
    const char* string2,
    bool bOrdinalIgnoreCase
    );

  static bool EqualOrdinal(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_string unsigned byte by unsigned byte.
  Parameters:
    other_string - [in]
    bIgnoreCase - [in]
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    impossible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */  
  int CompareOrdinal(
     const MYON_String& other_string,
     bool bIgnoreCase
     ) const;
 
  int CompareOrdinal(
     const char* other_string,
     bool bIgnoreCase
     ) const;
 
  /*
  Description:
    Compare string1 and string2 unsigned byte by unsigned byte.
  Parameters:
    string1 - [in]
    string2 - [in]
    bIgnoreCase - [in]
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    impossible to ignore case for any other values in a UTF-8 ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const char* string1,
    const char* string2,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare string1 and string2 unsigned byte by unsigned byte.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of elements in string1[] to compare.
      If element_count1 < 1, string1 must be null terminated and every element
      before the null terminator will be compared.
    string2 - [in]
    element_count2 - [in]
      The number of elements in string2[] to compare.
      If element_count2 < 1, string2 must be null terminated and every element
      before the null terminator will be compared.
    bOrdinalIgnoreCase - [in]
      If true, elements with values a-z are compared as if they had values A-Z.
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    impossible to ignore case for any other values in a UTF-8 ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const char* string1,
    int element_count1,
    const char* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_path as file system paths using
    appropriate tests for the current operating system. 
  Parameters:
    other_path - [in]
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  int ComparePath(
    const char* other_path
    ) const;

  bool EqualPath(
    const char* other_path
    ) const;

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
      null terminated string
    path2 - [in]
      null terminated string
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  static int ComparePath(
    const char* path1,
    const char* path2
    );

  static bool EqualPath(
    const char* path1,
    const char* path2
    );

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
    maximum_element_count1 - [in]
    path2 - [in]
    maximum_element_count2 - [in]
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.

    2) Case is ignored when the file system is not case sensitive, like Windows.

    3) String normalization appropriate for the current operating system
    is performed.
  */
  static int ComparePath(
    const char* path1,
    int  element_count1,
    const char* path2,
    int element_count2
    );

  static bool EqualPath(
    const char* path1,
    int  element_count1,
    const char* path2,
    int element_count2
    );

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like MYON_3dmObjectAttributes.m_name, MYON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    other_name - [in]
      null terminated string
  */
  int CompareAttributeName(
    const char* other_name
    ) const;
  bool EqualAttributeName(
    const char* other_name
    ) const;

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like MYON_3dmObjectAttributes.m_name, MYON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    name1 - [in]
      null terminated string
    name2 - [in]
      null terminated string
  */
  static int CompareAttributeName(
    const char* name1,
    const char* name2
    );
  static bool EqualAttributeName(
    const char* name1,
    const char* name2
    );

  // Description:
  //   Simple case sensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   matches zero or more occurrences of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string matches the wild card pattern.
	bool WildCardMatch( const char* ) const;
	bool WildCardMatch( const unsigned char* ) const;

  // Description:
  //   Simple case insensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   matches zero or more occurrences of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string matches the wild card pattern.
	bool WildCardMatchNoCase( const char* ) const;
	bool WildCardMatchNoCase( const unsigned char* ) const;

  /*
  Description:
    Replace all substrings that match token1 with token2
  Parameters:
    token1 - [in]
    token2 - [in]
  Returns:
    Number of times token1 was replaced with token2.
  */
  int Replace( const char* token1, const char* token2 );
  int Replace( const unsigned char* token1, const unsigned char* token2 );
  int Replace(char utf8_single_byte_c1, char utf8_single_byte_c2);
  int Replace(unsigned char utf8_single_byte_c1, unsigned char utf8_single_byte_c2);


	// simple sub-string extraction
	MYON_String Mid(
    int, // index of first char
    int  // count
    ) const;
	MYON_String Mid(
    int // index of first char
    ) const;
	MYON_String Left(
    int // number of chars to keep
    ) const;
	MYON_String Right(
    int // number of chars to keep
    ) const;

  /*
  Description:
    Map a single byte UTF-8 element to upper or lower case.
  Parameters:
    c - [in]
      If c is in the range A to Z or a to z, the map specified
      by map_type is applied.  All other values of c are
      unchanged.   
  Remarks:
    MapCharacterOrdinal is not appropriate for general string mapping.
  */
  static char MapCharacterOrdinal(
    MYON_StringMapOrdinalType map_type,
    char c
    );

  MYON_String MapStringOrdinal(
    MYON_StringMapOrdinalType map_type
    ) const;


  /*
  Description:
    Map a wchar_t string.
  Parameters:
    map_type - [in]
    string - [in]
    element_count - [in]
      number of string[] elements to map.
      If element_count < 0, then MYON_wString::Length(string) elements are mapped.
    mapped_string - [out]
    mapped_string_capacity - [in]
      number of available elements in mapped_string[].
    map_type - [in]
  Returns:
    Number of mapped_string[] elements that were mapped from string[].
    
    When the number of string[] input elements is >= mapped_string_capacity,
    mapped_string_capacity mapped_string[] elements are set and
    mapped_string_capacity is returned.

    When the return value is < mapped_string_capacity, a null terminator
    is appended after the last mapped element.
  */
  static int MapStringOrdinal(
    MYON_StringMapOrdinalType map_type,
    const char* string,
    int element_count,
    char* mapped_string,
    int mapped_string_capacity
    );

  MYON_String MapString(
    const class MYON_Locale& locale,
    MYON_StringMapType map_type
    ) const;

  static MYON_String MapString(
    const class MYON_Locale& locale,
    MYON_StringMapType map_type,
    const char* string,
    int element_count
    );

  /*
  Description:
    Maps element_count elements of string[] to mapped_string[].

  Parameters:
    locale - [in]
      Locale to use when converting case. It is common to pass one of
      the preset locales MYON_Locale::Ordinal, MYON_Locale::InvariantCulture, 
      or MYON_Locale::m_CurrentCulture. 

    map_type - [in]
      selects the mapping

    string - [in]
      input string to map.

    element_count - [in]
      The number of char elements to map from input string[].
      
      If element_count < 1, then string[] must be null terminated and
      MYON_wString::Length(string)+1 elements are mapped. 
      The +1 insures the output is null terminated.

    mapped_string - [out]
      The result of the mapping is returned in mapped_string[].

    mapped_string_capacity - [in]
      Number of char elements available in mapped_string[]
      or 0 to calculate the minimum number of elements needed
      for the mapping.

  Returns:
    If mapped_string_capacity > 0, then the number elements set in mapped_string[]
    is returned.  

    If mapped_string_capacity == 0, then the number elements required to perform
    the mapping is returned.

    When there is room, mapped_string[] is null terminated.

    0: Failure.
  */
  static int MapString(
    const class MYON_Locale& locale,
    MYON_StringMapType map_type,
    const char* string,
    int element_count,
    char* mapped_string,
    int mapped_string_capacity
    );

	// upper/lower/reverse conversion
	/*MYON_DEPRECATED */ void MakeUpper();
	/*MYON_DEPRECATED */void MakeLower();
	void MakeUpperOrdinal();
	void MakeLowerOrdinal();

  const MYON_SHA1_Hash ContentHash(
    MYON_StringMapOrdinalType mapping
  ) const;

  MYON_String Reverse() const;

  static char* Reverse(
    char* string,
    int element_count
    );

	void MakeReverse();

  void TrimLeft(const char* = nullptr);
  void TrimRight(const char* = nullptr);
  void TrimLeftAndRight(const char* = nullptr);

  // remove occurrences of chRemove
  int Remove(const char utf8_single_byte_c);

	// searching (return starting index, or -1 if not found)
	// look for a single character match
	int Find(
    char utf8_single_byte_c
    ) const;
	int Find(
    unsigned char utf8_single_byte_c
    ) const;
	int ReverseFind(
    char utf8_single_byte_c
    ) const;
	int ReverseFind(
    unsigned char utf8_single_byte_c
    ) const;

	// look for a specific sub-string
  int Find(
    const char*
    ) const;
  int Find(
    const unsigned char*
    ) const;
  int Find(
    const char*,
    int start_index
    ) const;
  int Find(
    const unsigned char*,
    int start_index
  ) const;
  int ReverseFind(
    const char*
    ) const;
  int ReverseFind(
    const unsigned char*
    ) const;

  // It is common to format single numbers into strings
  // and the FromNumber and FromDouble functions are
  // the fastest way to do this and provide consistent results.
  // They return a pointer to their buffer so the can be used
  // as function parameters.
  static const MYON_String FromNumber(
    char n
    );
  static const MYON_String FromNumber(
    unsigned char n
    );
  static const MYON_String FromNumber(
    short n
    );
  static const MYON_String FromNumber(
    unsigned short n
    );
  static const MYON_String FromNumber(
    int n
    );
  static const MYON_String FromNumber(
    unsigned int n
    );
  static const MYON_String FromNumber(
    MYON__INT64 n
    );
  static const MYON_String FromNumber(
    MYON__UINT64 n
    );
  static const MYON_String FromNumber(
    double d // "%g" format
    );
  static const MYON_String ApproximateFromNumber(
    double d // "%f" when possible, otherwise "%g"
    );
  static const MYON_String PreciseFromNumber(
    double d // "%.17g"
    );


  /*
  Description:
    Each byte value is converted to 2 hexadecimal digits.
  Parameters:
    bytes - [in]
      list of byte values
    byte_count - [in]
      Number of byte values in bytes[] array.
    bCapitalDigits - [in]
      false: Use 0-9, a - b
      true: Use 0-9, A - F
    bReverse - [in]
      false: 
        The digits in the string will be in the order
        bytes[0], bytes[1], ..., bytes[byte_count-1].
      true: 
        The digits in the string will be in the order
        bytes[byte_count-1], ..., bytes[1], bytes[0].
  */
  static const MYON_String HexadecimalFromBytes(
    const MYON__UINT8* bytes,
    size_t byte_count,
    bool bCapitalDigits,
    bool bReverse
    ); 

  /*
  Parameters:
    format - [in]
      Format control.  
      Positional parameters of the form %N$x where N >= 1 and x
      is the standard format specification are supported.
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
      arguments for replacable items in the format string.
  Returns:
    True if successful.
    False if the string is too long or the format string is not valid.
  Remarks:
    When using Microsoft's compiler and other compilers that provide similar
    locale support, the locale is the invariant culture locale returned by
    MYON_Locale::InvariantCulture::LocalePtr().

    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly recommended that you never use %S to
    include any string that may possibly contain elements with values > 127.
    The following examples illustrate a way to predictably use UTF-8 and wchar_t
    parameters in buffers of the other element type.

    const char* utf8_string = ...;
    // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
    // The code below will treat utf8_string as a UTF-8 encoded string.
    wchar_t wchar_buffer[...];
    const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
    MYON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", MYON_wString(utf8_string));

    const wchar_t* wide_string = ...;
    // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",char_string);
    // The code below will include wide_string as a UTF-8 encoded string.
    char char_buffer[...];
    const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
    MYON_String::Format(char_buffer, char_buffer_capacity, "%s", MYON_String(wide_string));
  */
  bool MYON_VARGS_FUNC_CDECL Format(
    const char* format,
    ...
    );

  bool MYON_VARGS_FUNC_CDECL Format(
    const unsigned char* format,
    ...
    );

  static const MYON_String MYON_VARGS_FUNC_CDECL FormatToString(
    const char* format,
    ...
    );

  bool FormatVargs(
    const char* format, 
    va_list args
    );

  bool FormatVargs(
    const unsigned char* format,
    va_list args
    );

  /*
  Description:
    A platform independent, secure, culture invariant way to format a char string. 
    This function is provide to be used when it is critical that 
    the formatting be platform independent, secure and culture invariant.
  Parameters:
    buffer - [out] 
      not null
    buffer_capacity - [in]
      > 0
      Number of char elements in buffer.
    sFormat - [in]
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
  Returns:
    >= 0: 
      The number of char elements written to buffer[], not including the null terminator.
      A null terminator is always added (buffer[returned value] = 0).
      The last element of buffer[] is always set to zero (buffer[buffer_capacity-1] = 0).
    < 0: failure:
      If buffer is not null and buffer_capacity > 0, then buffer[0] = 0 and buffer[buffer_capacity-1] = 0;
  Remarks:
    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly recommended that you never use %S to
    include any string that may possibly contain elements with values > 127.  
    The following examples illustrate a way to predictably use UTF-8 and wchar_t 
    parameters in buffers of the other element type.

         const char* utf8_string = ...;
         // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
         wchar_t wchar_buffer[...];
         const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
         MYON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", MYON_wString(utf8_string));

         const wchar_t* wide_string = ...;
         // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",char_string);
         char char_buffer[...];
         const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
         MYON_String::Format(char_buffer, char_buffer_capacity, "%s", MYON_String(wide_string));
  */
  static int MYON_VARGS_FUNC_CDECL FormatIntoBuffer(
    char* buffer, 
    size_t buffer_capacity,
    const char* format,
    ...
    );

  static int MYON_VARGS_FUNC_CDECL FormatIntoBuffer(
    MYON_StringBuffer& buffer,
    const char* format,
    ...
    );

  static int FormatVargsIntoBuffer(
    char* buffer,
    size_t buffer_capacity,
    const char* format,
    va_list args
    );

  static int FormatVargsIntoBuffer(
    MYON_StringBuffer& buffer,
    const char* format,
    va_list args
    );

  /*
  Returns:
    >= 0: 
      Number of char elements in the formatted string, not including the null terminator.
    < 0:
      Invalid input
  */
  static int FormatVargsOutputCount(
    const char* format,
    va_list args
    );

  /*
  Parameters:
    format - [in]
      null terminated string to scan
    ... - [out]
      pointers to elements to assign.
  Returns:
    >= 0: number of fields successfully converted and assigned.
    <0:  failure
  */
  int MYON_VARGS_FUNC_CDECL Scan(
    const char* format,
    ...
    );

  int MYON_VARGS_FUNC_CDECL Scan(
    const unsigned char* format,
    ...
    );

  static int MYON_VARGS_FUNC_CDECL ScanBuffer(
    const char* buffer,
    const char* format,
    ...
    );

  static int MYON_VARGS_FUNC_CDECL ScanBuffer(
    const unsigned char* buffer,
    const unsigned char* format,
    ...
    );

  static int ScanBufferVargs(
    const char* buffer,
    const char* format,
    va_list args
    );

  static int ScanBufferVargs(
    const unsigned char* buffer,
    const unsigned char* format,
    va_list args
    );


  /*
  Parameters:
    buffer - [in]
      decimal number
  Returns:
    not zero:
      pointer to the first character that was not scanned
    nullptr:
      failure
  */
  static const char* ToNumber(
    const char* buffer,
    char value_on_failure,
    char* value
    );
  static const char* ToNumber(
    const char* buffer,
    unsigned char value_on_failure,
    unsigned char* value
    );
  static const char* ToNumber(
    const char* buffer,
    short value_on_failure,
    short* value
    );
  static const char* ToNumber(
    const char* buffer,
    unsigned short value_on_failure,
    unsigned short* value
    );
  static const char* ToNumber(
    const char* buffer,
    int value_on_failure,
    int* value
    );
  static const char* ToNumber(
    const char* buffer,
    unsigned int value_on_failure,
    unsigned int* value
    );
  static const char* ToNumber(
    const char* buffer,
    MYON__INT64 value_on_failure,
    MYON__INT64* value
    );
  static const char* ToNumber(
    const char* buffer,
    MYON__UINT64 value_on_failure,
    MYON__UINT64* value
    );
  static const char* ToNumber(
    const char* buffer,
    double value_on_failure,
    double* value
    );

  
  // Low level access to string contents as character array
	char* ReserveArray(size_t); // make sure internal array has at least
                          // the requested capacity.
	void ShrinkArray();     // shrink internal storage to minimum size
  char* SetLength(size_t);    // set length (<=capacity)
  char* Array();
  const char* Array() const;

  /*
  Returns:
    A duplicate of this that does not share memory with any other string.
    (A new array is allocated for the returned string.)
  */
  const MYON_String Duplicate() const;

  /*
  Returns:
    Total number of bytes of memory used by this class.
    (For use in MYON_Object::SizeOf() overrides.
  */
  unsigned int SizeOf() const;

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  /*
  OBSOLETE - use MYON_FileSystemPath::SplitPath
  */
  static void SplitPath( 
    const char* path,
    MYON_String* drive,
    MYON_String* dir,
    MYON_String* fname,
    MYON_String* ext
    );

public:

  enum class Encoding : unsigned int
  {
    /// <summary>
    /// Encoding not set.
    /// </summary>
    Unset = 0,

    /// <summary>
    /// Encoding not known or cannot be determined.
    /// </summary>
    Unknown = 1,

    /// <summary>
    /// All byte values are in the range 0 to 0x7F (127)
    /// </summary>
    ASCII = 2,

    /// <summary>
    /// The sequence of byte values could be a UTF-8 encode string.
    /// UTF-8 extends ASCII encoding.
    /// </summary>
    UTF8 = 3,

    /// <summary>
    /// The sequence of byte values could be a UTF-8 encoding with oversized 
    /// encodings or surrogate pair code points.
    /// </summary>
    SloppyUTF8 = 4,

    /// <summary>
    /// The sequence of byte values could be a BIG5 encode string
    /// that may also contain single byte ASCII code points. 
    /// BIG5  is a double byte encoding and the first byte
    /// always has the high bit set. 
    /// </summary>
    BIG5andASCII = 5,
  };

  /*
  Description:
    Determine if the string's buffer can be parsed using the specified encoding.
  */
  bool IsPossibleEncoding(
    MYON_String::Encoding encoding
  ) const;

  /*
  Description:
    Determine if the buffer can be parsed using the specified encoding.
  */
  static bool IsPossibleEncoding(
    MYON_String::Encoding encoding,
    const char* buffer,
    int buffer_length
  );

  MYON_String::Encoding ProbableEncoding() const;

  static MYON_String::Encoding ProbableEncoding(
    const char* buffer,
    int buffer_length
  );

  /*
  Description:
    Make an educated guess at the encoding and convert the string to UTF-8 encoding.
  Returns:
    If the string encoding could be determined and parsed, a UTF-8 encoded string is returned.
    Otherwise EmptyString is returned.
  */
  static const MYON_String ToUTF8(
    const char* buffer,
    int buffer_length
  );

  /*
  Description:
    Make an educated guess at the encoding and convert the string to UTF-8 encoding.
  Returns:
    If the string encoding could be determined and parsed, a UTF-8 encoded string is returned.
    Otherwise EmptyString is returned.
  */
  const MYON_String ToUTF8() const;

  /*
  Description:
    Make an educated guess at the encoding and convert the string to BIG5 encoding.
  Parameters:
    error_count - [out]
      number of errors in encoding (question mark is used as a replacement character).
  Returns:
    If the string encoding could be determined and parsed, a BIG5 encoded string is returned.
    Otherwise EmptyString is returned.
  */
  static const MYON_String ToBIG5(
    const char* buffer,
    int buffer_length,
    int* error_count
  );

  /*
  Description:
    Make an educated guess at the encoding and convert the string to BIG5 encoding.
  Parameters:
    error_count - [out]
      number of errors in encoding (question mark is used as a replacement character).
  Returns:
    If the string encoding could be determined and parsed, a BIG5 encoded string is returned.
    Otherwise EmptyString is returned.
  */
  const MYON_String ToBIG5(
    int* error_count
  ) const;

public:
	~MYON_String();

protected:
  // Implementation
	char* m_s; // pointer to ref counted string array
             // m_s - 12 bytes points at the string's MYON_aStringHeader

	// implementation helpers
	class MYON_aStringHeader* Header() const;
	class MYON_aStringHeader* IncrementedHeader() const;
	char* CreateArray(int);
  void CopyArray();
  void CopyToArray( const MYON_String& );
  void CopyToArray( int, const char* );
  void CopyToArray( int, const unsigned char* );
  void CopyToArray( int, const wchar_t* );
  void AppendToArray( const MYON_String& );
  void AppendToArray( int, const char* );
  void AppendToArray( int, const unsigned char* );
};

/*
Returns:
  True if lhs and rhs are identical as arrays of char elements.  
*/
MYON_DECL
bool operator==( const MYON_String& lhs, const MYON_String& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of char elements.  
*/
MYON_DECL
bool operator!=(const MYON_String& lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator<(const MYON_String& lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator>(const MYON_String& lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator<=(const MYON_String& lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator>=(const MYON_String& lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of char elements.  
*/
MYON_DECL
bool operator==( const MYON_String& lhs, const char* rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of char elements.  
*/
MYON_DECL
bool operator!=(const MYON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is less than rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator<(const MYON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator>(const MYON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator<=(const MYON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator>=(const MYON_String& lhs, const char* rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of char elements.  
*/
MYON_DECL
bool operator==( const char* lhs, const MYON_String& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of char elements.  
*/
MYON_DECL
bool operator!=(const char* lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator<(const char* lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator>(const char* lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator<=(const char* lhs, const MYON_String& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of unsigned bytes.
*/
MYON_DECL
bool operator>=(const char* lhs, const MYON_String& rhs);

/// <summary>
/// A wide character string. 
/// The default encoding is the encoding the compiler uses for wchar_t* s = L&quot;...&quot;; strings. 
/// This is typically 2 byte wchar_t UTF-16 on Windows and 4 byte wchar_t UTF-32 on MacOS. 
/// However, some MacOS SDK functions return 4 byte wchar_t UTF-16 strings.
/// </summary>
class MYON_CLASS MYON_wString
{
public:
  /// <summary>
  /// MYON_String::EmptyString has length 0.
  /// const char* s = MYON_String::EmptyString sets s to L&quot;&quot;.
  /// </summary>
  static const MYON_wString EmptyString;

  /// <summary>
  /// UTF-16/UTF-32 encoding of the Unicode byte order mark (BOM) U+FEFF.
  /// </summary>
  static const MYON_wString ByteOrderMark;

  /*
  Parameters:
    bom_candidate - [in]
  Returns;
     1: bom_candidate = MYON_UnicodeCodePoint::MYON_ByteOrderMark
    -1: After swapping bytes, bom_candidate = MYON_UnicodeCodePoint::MYON_ByteOrderMark
     0: otherwise
  */
  static int ByteOrder(wchar_t bom_candidate);

  /*
  Parameters:
    w - [in]
      wchar_t value to swap bytes
  Returns:
    w with swapped byte order
  */
  static wchar_t SwapByteOrder(wchar_t w);

  /// <summary>
  /// Identifies a built in string that can be used for testing.
  /// </summary>
  enum class ExampleType : unsigned int
  {
    /// <summary>
    /// MYON_wString::EmptyString
    /// </summary>
    Empty = 0,

    /// <summary>
    /// A wchar_t string that contains code points with 2, 3, 4, and 5 hex digit code points.
    /// Useful for testing what a compiler does with string that have UTF-8 multiple byte encodings and UTF-16 surrogate pair encodings.
    /// &quot;The math teacher said, &#x5c;&quot;It isn&apos;t true that &#x3a3; &gt; 3&#xa2; &amp; &#x3a3; &lt; 2 &#x20bd; &amp; &#x3a3; &gt; &#x20ac;99.&#x5c;&quot; &#x1f5d1;!&quot;
    /// </summary>
    WideChar = 1,

    /// <summary>
    /// The WideChar string UTF-16 encoded.
    /// </summary>
    UTF16 = 51,

    /// <summary>
    /// The rich text string from MYON_wString::RichTextExample(&MYON_FOnt::Default).
    /// </summary>
    RichText = 90,

    /// <summary>
    /// The WideChar string as an XML value with special characters encoded in the &amp;amp; format 
    /// and code points above basic latin UTF-16 encoded.
    /// </summary>
    XML = 101,

    /// <summary>
    /// The WideChar string as an XML value with special characters encoded in the &amp;amp; format 
    /// and code points above basic latin encoded in the &#hhhh; format
    /// using  lower case hex digits (0123456789abcdef).
    /// </summary>
    XMLalternate1 = 102,

    /// <summary>
    /// The WideChar string as an XML value with special characters encoded in the &amp;amp; format 
    /// and code points above basic latin encoded in the hexadecimal &amp;#xhhhh; format
    /// with  upper case hex digits (0123456789ABCDEF).
    /// </summary>
    XMLalternate2 = 103,

    /// <summary>
    /// The WideChar string as an XML value with special characters and code points above 
    /// basic latin encoded in the decimal code point &amp;#nnnn; format.
    /// </summary>
    XMLalternate3 = 104,
  };


  /// <summary>
  /// A selection of strings that can be used for testing.
  /// </summary>
  static const MYON_wString Example(MYON_wString::ExampleType t);

  /// <summary>
  /// Get a rich text example.
  /// </summary>
  /// <param name="rich_text_face_name"> 
  /// The rich text font name. 
  /// This name is not well defined and the best choice can be platform specific. 
  /// For Windows use the LOGFONT always works.
  /// For Mac OS the font family name generally works.
  /// If you have an MYON_Font, then MYON_Font.RichTextFontName() or MYON_Font.FontQuartet().QuartetName() are good choices.
  /// </param>
  /// <param name="bBold">Pass true to include a rich text bold face line.</param>
  /// <param name="bItalic">Pass true to include a rich text italic face line.</param>
  /// <param name="bBoldItalic">Pass true to include a rich text bold-italic face line.</param>
  /// <param name="bUnderline">Pass true to include both plain and underline in the sample.</param>
  /// <returns>
  /// A rich text example using the specified face and the specified 
  /// </returns>
  static const MYON_wString RichTextExample(
    MYON_wString rich_text_font_name,
    bool bBold,
    bool bItalic,
    bool bBoldItalic,
    bool bUnderline
  );

  /// <summary>
  /// Get a rich text example.
  /// </summary>
  /// <param name="font"> 
  /// Every rich text face supported by font will be in the sample.
  /// </param>
  /// <returns>
  /// A rich text example using the specified font in all supported rich text faces (regular/bold/italic/bold-italic) in both plain and underline.
  /// </returns>
  static const MYON_wString RichTextExample(
    const class MYON_Font* font
  );

  /// <summary>
  /// Get a rich text example.
  /// </summary>
  /// <param name="quartet"> 
  /// Every rich text face supported by the font quartet will be in the sample.
  /// </param>
  /// <returns>
  /// A rich text example using the specified font in all supported rich text faces (regular/bold/italic/bold-italic) in both plain and underline.
  /// </returns>
  static const MYON_wString RichTextExample(
    const class MYON_FontFaceQuartet* quartet
  );

public:
  /// <summary>BACKSPACE control U+0008</summary>
  static const wchar_t  Backspace = (wchar_t)MYON_UnicodeCodePoint::MYON_Backspace;

  /// <summary>CHARACTER TABULATION control U+0009</summary>
  static const wchar_t  Tab = (wchar_t)MYON_UnicodeCodePoint::MYON_Tab;

  /// <summary>LINE FEED control U+000A</summary>
  static const wchar_t  LineFeed = (wchar_t)MYON_UnicodeCodePoint::MYON_LineFeed;

  /// <summary>LINE TABULATION control U+000B</summary>
  static const wchar_t  VerticalTab = (wchar_t)MYON_UnicodeCodePoint::MYON_VerticalTab;

  /// <summary>FORM FEED control U+000C</summary>
  static const wchar_t  FormFeed = (wchar_t)MYON_UnicodeCodePoint::MYON_FormFeed;

  /// <summary>CARRIAGE RETURN control U+000D</summary>
  static const wchar_t  CarriageReturn = (wchar_t)MYON_UnicodeCodePoint::MYON_CarriageReturn;

  /// <summary>ESCAPE control U+001B</summary>
  static const wchar_t  Escape = (wchar_t)MYON_UnicodeCodePoint::MYON_Escape;

  /// <summary>SPACE U+0020</summary>
  static const wchar_t  Space = (wchar_t)MYON_UnicodeCodePoint::MYON_Space;

  /// <summary>QUOTATION MARK U+0022 (&quot;)</summary>
  static const wchar_t  QuotationMark = (wchar_t)MYON_UnicodeCodePoint::MYON_QuotationMark;

  /// <summary>NUMBER SIGN U+0023 (#)</summary>
  static const wchar_t  NumberSign = (wchar_t)MYON_UnicodeCodePoint::MYON_NumberSign;

  /// <summary>PERCENT SIGN U+0025 (%)</summary>
  static const wchar_t  PercentSign = (wchar_t)MYON_UnicodeCodePoint::MYON_PercentSign;

  /// <summary>AMPERSAND U+0026 (&amp;)</summary>
  static const wchar_t  Ampersand = (wchar_t)MYON_UnicodeCodePoint::MYON_Ampersand;

  /// <summary>APOSTROPHE U+0027 (&apos;)</summary>
  static const wchar_t  Apostrophe = (wchar_t)MYON_UnicodeCodePoint::MYON_Apostrophe;

  /// <summary>COMMA U+002C (,)</summary>
  static const wchar_t  Comma = (wchar_t)MYON_UnicodeCodePoint::MYON_Comma;

  /// <summary>HYPHEN-MINUS U+002D (-)</summary>
  static const wchar_t  HyphenMinus = (wchar_t)MYON_UnicodeCodePoint::MYON_HyphenMinus;

  /// <summary>PERIOD U+002E (decimal 46) (.)</summary>
  static const wchar_t  Period = (wchar_t)MYON_UnicodeCodePoint::MYON_Period;

  /// <summary>SOLIDUS U+002F (&#x2f;)</summary>
  static const wchar_t  Slash = (wchar_t)MYON_UnicodeCodePoint::MYON_Slash;

  /// <summary>COLON U+003A (:)</summary>
  static const wchar_t  Colon = (wchar_t)MYON_UnicodeCodePoint::MYON_Colon;

  /// <summary>SEMICOLON U+003B (;)</summary>
  static const wchar_t  Semicolon = (wchar_t)MYON_UnicodeCodePoint::MYON_Semicolon;

  /// <summary>LESS-THAN SIGN U+003C (&#x3c;)</summary>
  static const wchar_t  LessThanSign = (wchar_t)MYON_UnicodeCodePoint::MYON_LessThanSign;

  /// <summary>GREATER-THAN SIGN U+003E (&#x3e;)</summary>
  static const wchar_t  GreaterThanSign = (wchar_t)MYON_UnicodeCodePoint::MYON_GreaterThanSign;

  /// <summary>REVERSE SOLIDUS U+005C (&#x5c;)</summary>
  static const wchar_t  Backslash = (wchar_t)MYON_UnicodeCodePoint::MYON_Backslash;

  /// <summary>// Unicode LOW LINE U+005F (_)</summary>
  static const wchar_t  Underscore = (wchar_t)MYON_UnicodeCodePoint::MYON_Underscore;

  /// <summary>VERTICAL LINE U+007C (&#x7c;)</summary>
  static const wchar_t  Pipe = (wchar_t)MYON_UnicodeCodePoint::MYON_Pipe;

  /// <summary>TILDE U+007E (&#x7e;)</summary>
  static const wchar_t  Tilde = (wchar_t)MYON_UnicodeCodePoint::MYON_Tilde;

  /// <summary>Period decimal point (.)</summary>
  static const wchar_t DecimalAsPeriod = (wchar_t)MYON_UnicodeCodePoint::MYON_Period;

  /// <summary>Comma decimal point (,)</summary>
  static const wchar_t DecimalAsComma = (wchar_t)MYON_UnicodeCodePoint::MYON_Comma;

#if defined(MYON_SIZEOF_WCHAR_T) && MYON_SIZEOF_WCHAR_T >= 2
public:
  /// <summary>NEXT LINE (NEL) control U+0085</summary>
  static const wchar_t  NextLine = (wchar_t)MYON_UnicodeCodePoint::MYON_NextLine;

  /// <summary>NO-BREAK SPACE (NBSP) U+00A0</summary>
  static const wchar_t  NoBreakSpace = (wchar_t)MYON_UnicodeCodePoint::MYON_NoBreakSpace;

  /// <summary>NON-BREAKING HYPHEN U+2011</summary>
  static const wchar_t  NoBreakHyphen = (wchar_t)MYON_UnicodeCodePoint::MYON_NoBreakHyphen;

  /// <summary>ZERO WIDTH SPACE (ZWSP) U+200B</summary>
  static const wchar_t  ZeroWidthSpace = (wchar_t)MYON_UnicodeCodePoint::MYON_ZeroWidthSpace;

  /// <summary>zero with non-joiner (ZWNJ) U+200C</summary>
  static const wchar_t  ZeroWidthNonJoiner = (wchar_t)MYON_UnicodeCodePoint::MYON_ZeroWidthNonJoiner;

  /// <summary>zero with joiner (ZWJ) U+200D</summary>
  static const wchar_t  ZeroWidthJoiner = (wchar_t)MYON_UnicodeCodePoint::MYON_ZeroWidthJoiner;

  /// <summary>NARROW NO-BREAK SPACE (NNBSP) U+202F</summary>
  static const wchar_t  NarrowNoBreakSpace = (wchar_t)MYON_UnicodeCodePoint::MYON_NarrowNoBreakSpace;

  /// <summary>LATIN CAPITAL LETTER R U+0052 (decimal 82) (Rhino annotation radius symbol)</summary>
  static const wchar_t  RadiusSymbol = (wchar_t)MYON_UnicodeCodePoint::MYON_RadiusSymbol;

  /// <summary>DEGREE SIGN U+00B0 (X&#xb0;) (Rhino annotation degree symbol)</summary>
  static const wchar_t  DegreeSymbol = (wchar_t)MYON_UnicodeCodePoint::MYON_DegreeSymbol;

  /// <summary>PLUS-MINUS SIGN U+00B1 (&#xb1;) (Rhino annotation plus/minus symbol)</summary>
  static const wchar_t  PlusMinusSymbol = (wchar_t)MYON_UnicodeCodePoint::MYON_PlusMinusSymbol;

  /// <summary>SUPERSCRIPT TWO U+00B2 (X&#xb2;) (Rhino annotation length squared symbol)</summary>
  static const wchar_t  Superscript2 = (wchar_t)MYON_UnicodeCodePoint::MYON_Superscript2;

  /// <summary>SUPERSCRIPT THREE U+00B3 (X&#xb3;) (Rhino annotation length cubed symbol)</summary>
  static const wchar_t  Superscript3 = (wchar_t)MYON_UnicodeCodePoint::MYON_Superscript3;

  /// <summary>LATIN CAPITAL LETTER O WITH STROKE U+00D8 (&#xd8;) (Rhino annotation diametersymbol)</summary>
  static const wchar_t  DiameterSymbol = (wchar_t)MYON_UnicodeCodePoint::MYON_DiameterSymbol;

  /// <summary>LINE SEPARATOR U+2028 unambiguous line separator</summary>
  static const wchar_t  LineSeparator = (wchar_t)MYON_UnicodeCodePoint::MYON_LineSeparator;

  /// <summary>PARAGRAPH SEPARATOR U+2028 unambiguous paragraph separator</summary>
  static const wchar_t  ParagraphSeparator = (wchar_t)MYON_UnicodeCodePoint::MYON_ParagraphSeparator;

  /// <summary>GREEK SMALL LETTER ALPHA (&#x391;)</summary>
  static const wchar_t GreekCapitalAlpha = (wchar_t)MYON_UnicodeCodePoint::MYON_GreekCapitalAlpha;

  /// <summary>GREEK SMALL LETTER ALPHA (&#x3b1;)</summary>
  static const wchar_t GreekAlpha = (wchar_t)MYON_UnicodeCodePoint::MYON_GreekAlpha;

  /// <summary>GREEK CAPITAL LETTER SIGMA U+03A3 (&#x3a3;)</summary>
  static const wchar_t GreekCapitalSigma = (wchar_t)MYON_UnicodeCodePoint::MYON_GreekCapitalSigma;

  /// <summary>GREEK SMALL LETTER SIGMA U+03C3 (&#x3c3;)</summary>
  static const wchar_t GreekSigma = (wchar_t)MYON_UnicodeCodePoint::MYON_GreekSigma;

  /// <summary>GREEK SMALL LETTER OMEGA U+03A9 (&#x3a9;)</summary>
  static const wchar_t  GreekCapitalOmega = (wchar_t)MYON_UnicodeCodePoint::MYON_GreekCapitalOmega;

  /// <summary>GREEK SMALL LETTER OMEGA U+03C9 (&#x3c9;)</summary>
  static const wchar_t  GreekOmega = (wchar_t)MYON_UnicodeCodePoint::MYON_GreekOmega;

  /// <summary>CYRILLIC CAPITAL LETTER YU U+042E (&#x42e;) (Used in Cyrillic code point tests)</summary>
  static const wchar_t  CyrillicCapitalYu = (wchar_t)MYON_UnicodeCodePoint::MYON_CyrillicCapitalYu;

  /// <summary>Simplified Chinese logogram for tree U+6881 (&#x6881;) (Used in CJK code point tests)</summary>
  static const wchar_t  SimplifiedChineseTree = (wchar_t)MYON_UnicodeCodePoint::MYON_SimplifiedChineseTree;

  /// <summary>Traditional Chinese logogram for tree U+6A39 (&#x6a39;) (Used in CJK code point tests)</summary>
  static const wchar_t  TraditionalChineseTree = (wchar_t)MYON_UnicodeCodePoint::MYON_TraditionalChineseTree;

  /// <summary>Japanese logogram for rhinoceros U+7280 (&#x7280;) (Used in CJK code point tests)</summary>
  static const wchar_t  JapaneseRhinoceros = (wchar_t)MYON_UnicodeCodePoint::MYON_JapaneseRhinoceros;

  /// <summary>Japanese logogram for tree U+6728 (&#x6728;) (Used in CJK code point tests)</summary>
  static const wchar_t  JapaneseTree = (wchar_t)MYON_UnicodeCodePoint::MYON_JapaneseTree;

  /// <summary>Korean HAN U+D55C (&#xd55c;) (Used in CJK code point tests)</summary>
  static const wchar_t  KoreanHan = (wchar_t)MYON_UnicodeCodePoint::MYON_KoreanHan;

  /// <summary>Korean JEONG U+C815 (&#xc815;) (Used in CJK code point tests)</summary>
  static const wchar_t  KoreanJeong = (wchar_t)MYON_UnicodeCodePoint::MYON_KoreanJeong;

  /// <summary>DOLLAR SIGN U+0024 ($)</summary>
  static const wchar_t  DollarSign = (wchar_t)MYON_UnicodeCodePoint::MYON_DollarSign;

  /// <summary>CENT SIGN U+00A2 (&#xa2;)</summary>
  static const wchar_t  CentSign = (wchar_t)MYON_UnicodeCodePoint::MYON_CentSign;

  /// <summary>POUND SIGN U+00A3 (&#xa3;)</summary>
  static const wchar_t  PoundSign = (wchar_t)MYON_UnicodeCodePoint::MYON_PoundSign;

  /// <summary>CURRENCY SIGN U+00A4 (&#xa4;)</summary>
  static const wchar_t  CurrencySign = (wchar_t)MYON_UnicodeCodePoint::MYON_CurrencySign;

  /// <summary>YEN SIGN U+00A5 (Chinese yuan; Japanese yen) (&#xa5;)</summary>
  static const wchar_t  YenSign = (wchar_t)MYON_UnicodeCodePoint::MYON_YenSign;

  /// <summary>EURO SIGN U+20AC (&#x20ac;)</summary>
  static const wchar_t  EuroSign = (wchar_t)MYON_UnicodeCodePoint::MYON_EuroSign;

  /// <summary>PESO SIGN U+20B1 (&#x20b1;)</summary>
  static const wchar_t  PesoSign = (wchar_t)MYON_UnicodeCodePoint::MYON_PesoSign;

  /// <summary>RUBLE SIGN U+20BD (&#x20bd;)</summary>
  static const wchar_t  RubleSign = (wchar_t)MYON_UnicodeCodePoint::MYON_RubleSign;

  /// <summary>INFINITY SYMBOL U+221E (&#x221e;)</summary>
  static const wchar_t  InfinitySymbol = (wchar_t)MYON_UnicodeCodePoint::MYON_InfinitySymbol;

  /// <summary>
  /// UNIVERSAL RECYCLING SYMBOL U+2672 (&#x2672;)
  /// This is a good code point for testing glyph substitution.
  /// </summary>
  static const wchar_t  RecyclingSymbol = (wchar_t)MYON_UnicodeCodePoint::MYON_RecyclingSymbol;

  /// <summary>
  /// BLACK UNIVERSAL RECYCLING SYMBOL U+267B (&#x267b;)
  /// This is a good code point for testing glyph substitution.
  /// </summary>
  static const wchar_t  BlackRecyclingSymbol = (wchar_t)MYON_UnicodeCodePoint::MYON_BlackRecyclingSymbol;

  /// <summary>WARNING SIGN U+26A0 (&#x26a0;)</summary>
  static const wchar_t  WarningSign = (wchar_t)MYON_UnicodeCodePoint::MYON_WarningSign;

  /// <summary>
  /// REPLACEMENT CHARACTER U+FFFD (&#xfffd;)
  /// By convention, U+FFFD is used to mark string elements where
  /// an invalid UTF code point encoding was encountered.
  /// </summary>
  static const wchar_t  ReplacementCharacter = (wchar_t)MYON_UnicodeCodePoint::MYON_ReplacementCharacter;
#endif

  /*
  Parameters:
    c - [in]
  Returns:
    True if c is '0', '1', ..., '9', 'A', 'B', ..., 'F', 'a', 'b', ..., of 'f'.
  */
  static bool IsHexDigit(wchar_t c);

  /*
  Parameters:
    c - [in]
  Returns:
    True if c is '0', '1', ..., or '9'.
  */
  static bool IsDecimalDigit(wchar_t c);


  /// <summary>
  /// Determine if c is a decimal digit.
  /// </summary>
  /// <param name="c">
  /// character to test
  /// </param>
  /// <param name="bOrdinaryDigitResult">
  /// Result to return when c is an ordinary decimal digit (0123456789)
  /// </param>
  /// <param name="bSuperscriptDigitResult">
  /// Result to return when c is a superscript decimal digit (0123456789)
  /// </param>
  /// <param name="bSubscriptDigitResult">
  /// Result to return when c is a subscript decimal digit (0123456789)
  /// </param>
  /// <returns>
  /// True if c is a decimal digit.
  /// </returns>
  static bool IsDecimalDigit(
    wchar_t c,
    bool bOrdinaryDigitResult,
    bool bSuperscriptDigitResult,
    bool bSubscriptDigitResult
  );

  static unsigned DecimalDigitFromWideChar(
    wchar_t c, 
    bool bAcceptOrdinaryDigit,
    bool bAcceptSuperscriptDigit,
    bool bAcceptSubscriptDigit,
    unsigned invalid_c_result
  );

  /*
  Parameters:
    c - [in]
      character to test.
    bAcceptOrdinarySign - [in]
      ordinary + and - signs are acceptable.
    bAcceptSuperscriptSign - [in]
      superscript + and - signs are acceptable.
    bAcceptSubscriptSign - [in]
      subscript + and - signs are acceptable.
  Returns:
    +1 if c is an acceptable plus sign.
    -1 if c is an acceptable minus sign.
    Otherwise, 0 is returned.
  */
  static int PlusOrMinusSignFromWideChar(
    wchar_t c,
    bool bAcceptOrdinarySign,
    bool bAcceptSuperscriptSign,
    bool bAcceptSubscriptSign
    );

  /// <summary>
  /// Determine if c is some type of Unicode slash (solidus).
  /// </summary>
  /// <param name="c">
  /// character to test
  /// </param>
  /// <param name="bOrdinarySlashResult">
  /// Result to return when c is an ordinary slash (solidus) U+002F (/)
  /// </param>
  /// <param name="bFractionSlashResult">
  /// Result to return when c is a fraction slash U+2044 (&#x2044;)
  /// </param>
  /// <param name="bDivisionSlashResult">
  /// Result to return when c is a division slash U+2215 (&#x2215;)
  /// </param>
  /// <param name="bMathematicalSlashResult">
  /// Result to return when c is a mathematical rising diagonal slash U+27CB (&#x27CB;)
  /// </param>
  /// <returns></returns>
  static bool IsSlash(
    wchar_t c,
    bool bOrdinarySlashResult,
    bool bFractionSlashResult,
    bool bDivisionSlashResult,
    bool bMathematicalSlashResult
  );

  /*
  Parameters:
    c - [in]
      character to test.
    bTabResult - [in]
      Result to return when c is a horizontal tab.
    bNoBreakSpaceResult - [in]
      Result to return when c is some type of no break space.
    bZeroWidthSpaceResult - [in]
      Result to return when c is a zero width space code point.
  Returns:
    True if c is some type of horizontal space.
  */
  static bool IsHorizontalSpace(wchar_t c, bool bTabResult, bool bNoBreakSpaceResult, bool bZeroWidthSpaceResult);

  /*
  Parameters:
    c - [in]
      character to test.
  Returns:
    True if c is some type of horizontal space, including horizontal tab and zero width spaces.
    If you need a more nuanced test, call the version of IsHorizontalSpace() that
    has bool parameters.
  */
  static bool IsHorizontalSpace(wchar_t c);

  /*
  Description:
    Encode this UTF encoded wide character string to a char string
    using the multibyte character set (MBCS) specified by windows_code_page.
  Parameters:
    windows_code_page - [in]
      Windows code page. For example, big5 = 950.
  Returns:
    A char string with the specified MBCS encoding.
    Carefully control the scope and use of the returned string. Lacking context,
    opennurbs assumes MYON_Strings use the UTF-8 encoded.
  See Also:
    MYON_String::MultiByteDecode()
  */
  const MYON_String MultiByteEncode(int windows_code_page) const;

private:
  // Use IsEmpty() or IsNotEmpty() when you want a bool
  // to test for the empty string.
  explicit operator bool() const { return IsNotEmpty(); }
public:

  // Constructors
  MYON_wString() MYON_NOEXCEPT;
  MYON_wString( const MYON_wString& );

  enum : int
  {
    // This MYON_String::MaximumStringLength value of 100,000,000
    // is used for string length sanity checking in both MYON_String and
    // MYON_wString.
    // The design of the MYON_String and MYON_wString classes could support
    // string lengths up to 0xFFFFFFFEU = 4,294,967,294 but
    // a cap of 100 million seems generous enough for current usage
    // and is small enough to detect many corrupt memory
    // situations.
    // This value is used for both MYON_String and MYON_wString.
    MaximumStringLength = MYON_String::MaximumStringLength
  };

#if defined(MYON_HAS_RVALUEREF)
  // Clone constructor
  MYON_wString( MYON_wString&& ) MYON_NOEXCEPT;

  // Clone Assignment operator
  MYON_wString& operator=( MYON_wString&& ) MYON_NOEXCEPT;
#endif

  MYON_wString(const MYON_String& src); // src = UTF-8 string

  MYON_wString(const char* src); // src = nul; terminated UTF-8 string
  MYON_wString(const char* src, int /*length*/);  // from UTF-8 substring
  MYON_wString(char, int = 1 /* repeat count */);

  MYON_wString(const unsigned char* src); // src = nul; terminated UTF-8 string
  MYON_wString(const unsigned char* src, int /*length*/);        // from UTF-8 substring
  MYON_wString(unsigned char, int = 1 /* repeat count */);

  MYON_wString(const wchar_t*);
  MYON_wString(const wchar_t*, int /*length*/);        // from substring
  MYON_wString(wchar_t, int = 1 /* repeat count */);

#if defined(MYON_RUNTIME_WIN)
  // Windows support
  bool LoadResourceString(HINSTANCE, UINT); // load from string resource
                                            // 2047 characters max
#endif

#if defined(MYON_RUNTIME_APPLE_CORE_TEXT_AVAILABLE)
  MYON_wString(CFStringRef);
  CFStringRef ToAppleCFString() const;
#endif
  /*
  Parameters:
    bLengthTest - [in]
      true
        (common case) If this->Length() != MYON_wString::Length(static_cast<const wchar_t*)(this)), then this string is not valid.
      false
        (less common case) There may be null elements in the first this->Length()-1 chars.
  Returns:
    True - this is valid.
    False
      There is a serious memory corruption bug in your code.
      This was not valid and converted to MYON_wString::EmptyString to prevent future crashes.     
  */
  bool IsValid(
    bool bLengthTest
  ) const;

  void Create();
  void Destroy(); // releases any memory and initializes to default empty string
  void EmergencyDestroy();

  MYON_DEPRECATED_MSG("Obsolete - does nothing.")
  void EnableReferenceCounting( bool bEnable );

  /*
  Returns:
    True if the string is reference counted.
  */
  bool IsReferenceCounted() const;

  // Attributes & Operations

  /*
  Returns:
    number of nonzero elements in string.
  */
  int Length() const;

  /*
  Returns:
    number of nonzero elements in the string.
  */
  unsigned int UnsignedLength() const;

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static int Length(
    const wchar_t* string
    );

  /*
  Parameters:
    string_capacity - [in]
      number of elements that can be read in string[] array.
  Returns:
    number of nonzero elements in string before the first null terminator
    or string_capacity if no null terminator is in string[].
    If string is nullptr, 0 is returned.
  */
  static int Length(
    const wchar_t* string,
    size_t string_capacity
    );

  /*
  Returns:
    number of nonzero elements in string before the first null terminator.
    If string is nullptr, 0 is returned.
  */
  static unsigned int UnsignedLength(
    const wchar_t* string
    );

  bool IsEmpty() const;
  bool IsNotEmpty() const; // returns true if length > 0
  void Empty();   // sets length to zero - if possible, memory is retained

	wchar_t& operator[](int);
	wchar_t operator[](int) const;
  wchar_t GetAt(int) const;
	void SetAt(int, char);
	void SetAt(int, unsigned char);
	void SetAt(int, wchar_t);

	operator const wchar_t*() const;

	// overloaded assignment
	const MYON_wString& operator=(const MYON_wString&);
	const MYON_wString& operator=(const MYON_String& src); // src = UTF-8 string
	const MYON_wString& operator=(char);
	const MYON_wString& operator=(const char* src); // src = UTF-8 string
	const MYON_wString& operator=(unsigned char);
	const MYON_wString& operator=(const unsigned char* src); // src = UTF-8 string
  const MYON_wString& operator=(wchar_t);
  const MYON_wString& operator=(const wchar_t*);

	// string concatenation
  void Append( const char* sUTF8, int ); // append specified number of elements from a UTF-8 string
  void Append( const unsigned char* sUTF8, int ); // append specified number of elements from a UTF-8 string
  void Append( const wchar_t*, int ); // append specified number of elements
	const MYON_wString& operator+=(const MYON_wString&);
	const MYON_wString& operator+=(const MYON_String& sUTF8); // append UTF-8 string
	const MYON_wString& operator+=(char);
	const MYON_wString& operator+=(unsigned char);
	const MYON_wString& operator+=(wchar_t);
	const MYON_wString& operator+=(const char* sUTF8); // append UTF-8 string
	const MYON_wString& operator+=(const unsigned char* sUTF8); // append UTF-8 string
	const MYON_wString& operator+=(const wchar_t*);

  // operator+()
  MYON_wString operator+(const MYON_wString&) const;
  MYON_wString operator+(const MYON_String& sUTF8) const; // concatenate with a UTF-8 string
  MYON_wString operator+(char) const;
  MYON_wString operator+(unsigned char) const;
  MYON_wString operator+(wchar_t) const;
  MYON_wString operator+(const char* sUTF8) const; // concatenate with a UTF-8 string
  MYON_wString operator+(const unsigned char* sUTF8) const; // concatenate with a UTF-8 string
  MYON_wString operator+(const wchar_t*) const;

  // Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate
	//MYON_DEPRECATED // deprecation in progress
	int Compare( const wchar_t* ) const;

  // Use CompareOrdinal(), ComparePath(), CompareAttributeName(), or a test that is linguistically apprropriate
	// MYON_DEPRECATED // deprecation in progress
	int CompareNoCase( const wchar_t* ) const;

  bool Equal(
    const MYON_wString& other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    ) const;

  bool Equal(
    const wchar_t* other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    ) const;

  static bool Equal(
    const wchar_t* string1,
    const wchar_t* string2,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Determine if string 1 and string2 are equal.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of string1[] elements to test.
      If element_count1 < 0, then string1 must nullptr or be null terminated.
    string2 - [in]
    element_count2 - [in]
      The number of string2[] elements to test.
      If element_count1 < 0, then string2 must nullptr or be null terminated.
    locale - [in]
      Typically MYON_Locale::Ordinal, MYON_Locale::InvariantCulture, or
      MYON_Locale::CurrentCulture.
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */
  static bool Equal(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare this string and other_string by normalizing (NFC)
    and using invariant culture ordering.
  Parameters:
    other_string - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  int Compare(
    const MYON_wString& other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    ) const;

  int Compare(
    const wchar_t* other_string,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    ) const;
 
  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    string2 - [in]
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */  
  static int Compare(
    const wchar_t* string1,
    const wchar_t* string2,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    );

  /*
  Description:
    Compare string1 and string2 by normalizing (NFC) and using invariant culture ordering.
  Parameters:
    string1 - [in]
    element_count1 - [in]
      The number of string1[] elements to compare.
      If element_count1 < 0, then string1 must be null terminated.
    string2 - [in]
    element_count2 - [in]
      The number of string2[] elements to compare.
      If element_count2 < 0, then string2 must be null terminated.
    locale - [in]
      Typically MYON_Locale::Ordinal, MYON_Locale::InvariantCulture, or
      MYON_Locale::CurrentCulture.
    bIgnoreCase - [in]
  Remarks:
    1) Ordinal compares are the fastest.
    2) Equal(...) is faster than  Compare(...)
  */
  static int Compare(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    const class MYON_Locale& locale,
    bool bIgnoreCase
    );
  
  bool EqualOrdinal(
    const MYON_wString& other_string,
    bool bOrdinalIgnoreCase
    ) const;
  
  bool EqualOrdinal(
    const wchar_t* other_string,
    bool bOrdinalIgnoreCase
    ) const;

  static bool EqualOrdinal(
    const wchar_t* string1,
    const wchar_t* string2,
    bool bOrdinalIgnoreCase
    );

  static bool EqualOrdinal(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );


  /*
  Description:
    Compare this string and other_string wchar_t element by wchar_t element.
  Parameters:
    other_string - [in]
    bOrdinalIgnoreCase - [in]
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    impossible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */  
  int CompareOrdinal(
     const MYON_wString& other_string,
     bool bOrdinalIgnoreCase
     ) const;
 
  int CompareOrdinal(
     const wchar_t* other_string,
     bool bOrdinalIgnoreCase
     ) const;
 
  /*
  Description:
    Compare this string1 and string2 wchar_t element by wchar_t element.
  Parameters:
    string1 - [in]
    string2 - [in]
    bOrdinalIgnoreCase - [in]
      If true, elements with values a-z are compared as if they had values A-Z.
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    impossible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const wchar_t* string1,
    const wchar_t* string2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string1 and string2 wchar_t element by wchar_t element.
  Parameters:
    string1 - [in]
    maximum_element_count1 - [in]
      maximum number of elements to compare
    string2 - [in]
    maximum_element_count2 - [in]
      maximum number of elements to compare
    bOrdinalIgnoreCase - [in]
      If true, elements with values a-z are compared as if they had values A-Z.
  Remarks:
    1) If the string is UTF-8 encoded and bOrdinalIgnoreCase is true, only
    small latin a - z and capital latin A - Z are considered equal.  It is
    impossible to ignore case for any other values in an ordinal compare.

    2) If you are comparing file system paths, you should use ComparePath().

    3) If locale, linguistic issues, UTF-8 encoding issues or unicode normalization
    or collation issues need to be considered, then CompareOrdinal() is
    the wrong function to use.
  */
  static int CompareOrdinal(
    const wchar_t* string1,
    int element_count1,
    const wchar_t* string2,
    int element_count2,
    bool bOrdinalIgnoreCase
    );

  /*
  Description:
    Compare this string and other_path as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    other_path - [in]
      null terminated string
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.
    2) Case is ignored when the file system is not case sensitive, like on Windows.
    3) String normalization appropriate for the current operating system is performed.
  */
  int ComparePath(
    const wchar_t* other_path
    ) const;
  bool EqualPath(
    const wchar_t* other_path
    ) const;

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
      null terminated string
    path2 - [in]
      null terminated string
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.
    2) Case is ignored when the file system is not case sensitive, like on Windows.
    3) String normalization appropriate for the current operating system is performed.
  */
  static int ComparePath(
    const wchar_t* path1,
    const wchar_t* path2
    );
  static bool EqualPath(
    const wchar_t* path1,
    const wchar_t* path2
    );

  /*
  Description:
    Compare sPath1 and sPath2 as file system paths using
    appropriate tests for the current operating system.
  Parameters:
    path1 - [in]
    maximum_element_count1 - [in]
    path2 - [in]
    maximum_element_count2 - [in]
  Remarks:
    1) Windows and UNIX directory separators (/ and \) are considered equal.
    2) Case is ignored when the file system is not case sensitive, like on Windows.
    3) String normalization appropriate for the current operating system is performed.
  */
  static int ComparePath(
    const wchar_t* path1,
    int element_count1,
    const wchar_t* path2,
    int element_count2
    );
  static bool EqualPath(
    const wchar_t* path1,
    int element_count1,
    const wchar_t* path2,
    int element_count2
    );

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like MYON_3dmObjectAttributes.m_name, MYON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    other_name - [in]
      null terminated string
  */
  int CompareAttributeName(
    const wchar_t* other_name
    ) const;
  bool EqualAttributeName(
    const wchar_t* other_name
    ) const;

  /*
  Description:
    Compare this string and other_name as a name attribute of an object 
    like MYON_3dmObjectAttributes.m_name, MYON_Layer.m_name, and so on.
    These comparisons ignore case and use appropriate string normalization.    
  Parameters:
    name1 - [in]
      null terminated string
    name2 - [in]
      null terminated string
  */
  static int CompareAttributeName(
    const wchar_t* name1,
    const wchar_t* name2
    );
  static bool EqualAttributeName(
    const wchar_t* name1,
    const wchar_t* name2
    );


  // Description:
  //   Simple case sensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   matches zero or more occurrences of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string matches the wild card pattern.
  bool WildCardMatch( const wchar_t* ) const;

  // Description:
  //   Simple case insensitive wildcard matching. A question mark (?) in the
  //   pattern matches a single character.  An asterisk (*) in the pattern
  //   matches zero or more occurrences of any character.
  //
  // Parameters:
  //   pattern - [in] pattern string where ? and * are wild cards.
  //
  // Returns:
  //   true if the string matches the wild card pattern.
	bool WildCardMatchNoCase( const wchar_t* ) const;

  /*
  Description:
    Replace all substrings that match token1 with token2
  Parameters:
    token1 - [in]
    token2 - [in]
  Returns:
    Number of times toke1 was replaced with token2
  */
  int Replace( const wchar_t* token1, const wchar_t* token2 );
  int Replace( wchar_t token1, wchar_t token2 );

  /*
  Description:
    Replaces all characters in the string whose values are
    not '0-9', 'A-Z', or 'a-z' with a percent sign followed
    by a 2 digit hex value.
  */
  void UrlEncode();

  /*
  Description:
    Replaces all %xx where xx a two digit hexadecimal number,
    with a single character. Returns false if the original
    string contained 
  */
  bool UrlDecode();

  /*
  Description:
    Replace all whitespace characters with the token.
    If token is zero, the string will end up with
    internal 0's
  Parameters:
    token - [in]
    whitespace - [in] if not null, this is a 0 terminated
      string that lists the characters considered to be 
      white space. If null, then (1,2,...,32,127) is used.
  Returns:
    Number of whitespace characters replaced.
  See Also:
    MYON_wString::RemoveWhiteSpace
  */
  int ReplaceWhiteSpace( wchar_t token, const wchar_t* whitespace = 0 );

  /*
  Description:
    Removes all whitespace characters from the string.
  Parameters:
    whitespace - [in] if not null, this is a zero-terminated
      string that lists the characters considered to be 
      white space. If null, then (1,2,...,32,127) is used.
  Returns:
    Number of whitespace characters removed.
  See Also:
    MYON_wString::ReplaceWhiteSpace
  */
  int RemoveWhiteSpace( const wchar_t* whitespace = 0 );

  /*
  Parameters: 
    prefix - [in]
    locale - [in]
      When no local is available, pass MYON_Locale::Ordinal.
    bIgnoreCase - [in]
      true to ignore case.
  Returns:
    If the string begins with prefix, the returned string has prefix removed. 
    Otherwise the returned string is identical to the string.
  */
  const MYON_wString RemovePrefix(
    const wchar_t* prefix,
    const class MYON_Locale& locale,
    bool bIgnoreCase
  ) const;

  /*
  Parameters: 
    suffix - [in]
    locale - [in]
      When no local is available, pass MYON_Locale::Ordinal.
    bIgnoreCase - [in]
      true to ignore case.
  Returns:
    If the string ends with suffix, the returned string has suffix removed. 
    Otherwise the returned string is identical to the string.
  */
  const MYON_wString RemoveSuffix(
    const wchar_t* suffix,
    const class MYON_Locale& locale,
    bool bIgnoreCase
  ) const;

  // simple sub-string extraction
	MYON_wString Mid(
    int, // index of first char
    int  // count
    ) const;
	MYON_wString Mid(
    int // index of first char
    ) const;
	MYON_wString Left(
    int // number of chars to keep
    ) const;
	MYON_wString Right(
    int // number of chars to keep
    ) const;
  const MYON_wString SubString(
    int start_index
  ) const;
  const MYON_wString SubString(
    int start_index,
    int count
  ) const;

  /*
  Description:
    Map a single wchar_t element to upper or lower case.
  Parameters:
    c - [in]
      If sizeof(wchar_t) >= 2 and c is not a value used int surrogate pairs,
      the map specified by map_type is applied.  If c is a value used in 
      surrogate pairs, the value is not changed.
  Remarks:
    1) MapCharacterOrdinal is not appropriate for general string mapping
       because it does not correctly handle surrogate pairs.
    2) If sizeof(wchar_t) == sizeof(char), MYON_String::MapCharacterOrdinal()
       and MYON_wString::MapCharacterOrdinal() are identical.
  */
  static wchar_t MapCharacterOrdinal(
    MYON_StringMapOrdinalType map_type,
    wchar_t c
    );

  MYON_wString MapStringOrdinal(
    MYON_StringMapOrdinalType map_type
    ) const;

  /*
  Description:
    Map a wchar_t string.
  Parameters:
    map_type - [in]
    string - [in]
    element_count - [in]
      number of string[] elements to map.
      If element_count < 0, then MYON_wString::Length(string) elements are mapped.
    mapped_string - [out]
    mapped_string_capacity - [in]
      number of available elements in mapped_string[].
      mapped_string_capacity must be >= mapped_element_count where 
      mapped_element_count = (element_count >= 0) element_count ? MYON_wString::Length(string).
    map_type - [in]
  Returns:
    Number of mapped_string[] elements that were mapped from string[].

    When the return value is < mapped_string_capacity, a null terminator
    is appended after the last mapped element.
  */
  static int MapStringOrdinal(
    MYON_StringMapOrdinalType map_type,
    const wchar_t* string,
    int element_count,
    wchar_t* mapped_string,
    int mapped_string_capacity
    );

  MYON_wString MapString(
    const class MYON_Locale& locale,
    MYON_StringMapType map_type
    ) const;

  static MYON_wString MapString(
    const class MYON_Locale& locale,
    MYON_StringMapType map_type,
    const wchar_t* string,
    int element_count
    );

  /*
  Description:
    Maps element_count elements of string[] to mapped_string[].
  Parameters:
    locale - [in]
      Locale to use when converting case. It is common to pass one of
      the preset locales MYON_Locale::Ordinal, MYON_Locale::InvariantCulture, 
      or MYON_Locale::m_CurrentCulture. 

    map_type - [in]
      selects the mapping

    string - [in]
      input string to map.

    element_count - [in]
      The number of wchar_t elements to map from input string[].
      
      If element_count < 1, then string[] must be null terminated and
      MYON_wString::Length(string)+1 elements are mapped. 
      The +1 insures the output is null terminated.

    mapped_string - [out]
      The result of the mapping is returned in mapped_string[].

    mapped_string_capacity - [in]
      Number of wchar_t elements available in mapped_string[]
      or 0 to calculate the minimum number of elements needed
      for the mapping.

  Returns:
    If mapped_string_capacity > 0, then the number elements set in mapped_string[]
    is returned.  

    If mapped_string_capacity == 0, then the number elements required to perform
    the mapping is returned.

    When there is room, mapped_string[] is null terminated.

    0: Failure.
  */
  static int MapString(
    const class MYON_Locale& locale,
    MYON_StringMapType map_type,
    const wchar_t* string,
    int element_count,
    wchar_t* mapped_string,
    int mapped_string_capacity
    );

  /*
  Returns:
    A platform independent SHA-1 of the string content. Independent of platform endian or platform wide string UTF encoding.
  */
  const MYON_SHA1_Hash ContentHash(
    MYON_StringMapOrdinalType mapping
  ) const;


	// upper/lower/reverse conversion
	/*MYON_DEPRECATED */ void MakeUpper();
	/*MYON_DEPRECATED */ void MakeLower();
  
  MYON_wString Reverse() const;

  /*
  Description:
    Convert a literal string into an XML encoded value.

    < (less-than) is replaced with &lt;
    > (greater-than) is replaced with &gt;
    & (ampersand) is replaces with &amp;
    ' (apostrophe or single quote) is replaced with &apos;
    " (double-quote) is replaced with &quot;

    Optionally, unocode code points U+hhhh where hhhh >= 0x80 are replaced 
    with &#xhhhh; using the minimal number of hex digits.
  Parameters:
    bUseUnicodeCodePointsForSpecialCharacters - [in]
      If true, the &#60;, &#62;, &#38;, &#39;, and &#34; encodings are used.
      If false, the &lt; &gt; &amp; &apos; and &quot; encodings are used.
      When in doubt, pass false.
    bEncodeCodePointsAboveBasicLatin
      If true, any code point >= 0x80 is encoded using the XML &xhhhh; format.
      When human readability is important and the XML will be parsed by a
      high quality XML reader, pass false. (The XMLspecification supports text
      files that are UTF=8, UTF-18, or UTF-32 encoded.)
  Returns:
    A string with every instance of an xml special character replaced 
    with its xml encoding and, optionally, every code point > 127 replaced
    with &xhhhh;.
  */
  const MYON_wString EncodeXMLValue() const;

  const MYON_wString EncodeXMLValue(
    bool bEncodeCodePointsAboveBasicLatin
  ) const;

  /*
  Description:
    Decode an XML encoded value.
  Examples:
    &lt; is replaced with < (less-than).
    &gt; is replaced with > (greater-than).
    &amp; is replaced with & (ampersand).
    &apos; is replaced with ' (apostrophe or single quote).
    &quot; is replaced with " (double-quote).
    &#nnnn; where nnnn is a valid decimal unicode code point is replaced with the wide character encoded code point.
    &#xhhhh; where hhhh is a valid hexadecimal unicode code point is replaced with the wide character encoded code point.
  Returns:
    This string with every instance of an xml character encoding replaced
    with the corresponding wide character encoding of the literal unicode code point.
  */
  const MYON_wString DecodeXMLValue() const;

  /*
  Description:
    Parse one of the following XML character encodings.
    &#nnnn; (nnnn = one of more decimal digits) is parsed to the unicode code point with decimal value nnnn
    &#xhhhh; (nnnn = one of more hexadecimal digits) is parsed to the unicode code point with hexadecimal value hhhh
    &lt; is parsed to < (less-than).
    &gt; is parsed to > (greater-than).
    &amp; is parsed to & (ampersand).
    &apos; is parsed to ' (apostrophe or single quote).
    &quot; is parsed to " (double-quote).
  Parameters:
    buffer - [in]
      buffer to parse. The first character of buffer[] must be the leading ampersand.
      The buffer must include the terminating semicolon.
    buffer_length - [in]
      If -1, then buffer[] must be null terminated.
      Otherwise buffer_length specifies the number of whcar_t elements
      that may be parsed.
    value_on_failure - [in]
      *unicode_code_point is set to value_on_failure if parsing fails.
    unicode_code_point - [out]
      parsed unicode code point. If you do not want the code point, you may pass nullptr.
  Returns:
    If parsing is successful, the first element of buffer that was not parsed is returned.
    Otherwise nullptr is returned.
  Remarks:
    Note that the XML 1 (section 2.2 of the WC3 specification) does not permit surrogate pair encodings.
  */
  static const wchar_t* ParseXMLCharacterEncoding(
    const wchar_t* buffer,
    int buffer_length,
    unsigned value_on_failure,
    unsigned* unicode_code_point
  );

  /*
  Description:
    Parse an xml encoded unicode code point.
    &#nnnn; (nnnn = any number of decimal digits)
    &#xhhhh; (hhhh = any muber of hexadecimal digits)
  Parameters:
    buffer - [in]
      buffer to parse. 
      The first character of buffer[] must be the leading ampersand.
      The second character of buffer[] must be the number sign.
      The buffer must include the terminating semicolon.
    buffer_length - [in]
      If -1, then buffer[] must be null terminated.
      Otherwise buffer_length specifies the number of whcar_t elements
      that may be parsed.
    value_on_failure - [in]
      *unicode_code_point is set to value_on_failure if parsing fails.
    unicode_code_point - [out]
      parsed unicode code point. If you do not want the code point, you may pass nullptr.
  Returns:
    If parsing is successful, the first element of buffer that was not parsed is returned.
    Otherwise nullptr is returned.
  */
  static const wchar_t* ParseXMLUnicodeCodePointEncoding(
    const wchar_t* buffer,
    int buffer_length,
    unsigned value_on_failure,
    unsigned* unicode_code_point
  );

  /*
  Description:
    Parse over horizontal space in the string.
  Parameters:
    s - [in]
      string to parse.
    len - [in]
      maximum number of characters to parse.
      You many pass -1 if s is null terminated.
    bParseTab - [in]
      True if a horizontal tab should be treated as horizontal space.
    bParseNoBreakSpace - [in]
      True if no break space code points should be treated as horizontal space.
    bParseZeroWidthSpace - [in]
      True if zero width code points should be treated as horizontal space.
  Returns:
    If horizontal spaces were successfully parsed, first character after
    the horizontal spaces is returned. Otherwise s is returned.
  */
  static const wchar_t* ParseHorizontalSpace(
    const wchar_t* s,
    int len,
    bool bParseTab,
    bool bParseNoBreakSpace,
    bool bParseZeroWidthSpace
  );

  /*
  Description:
    Parse over horizontal space in the string.
    This version of ParseHorizontalSpace() treas tabs and zero width code points
    as horizontal space. If you need more nuanced control, call the override
    with bool parameters.
  Parameters:
    s - [in]
      string to parse.
    len - [in]
      maximum number of characters to parse.
      You many pass -1 if s is null terminated.
  Returns:
    If horizontal spaces were successfully parsed, first character after
    the horizontal spaces is returned. Otherwise s is returned.
  */
  static const wchar_t* ParseHorizontalSpace(
    const wchar_t* s,
    int len
  );

  /*
  Description:
    Parse the string s as a vulgar fraction (1/2).
  Parameters:
    s - [in]
      string to parse.
      s[0] must be a sign or a digit. It can be the ordinary characters or superscripts.
      If the first digit is an ordinary digit, the the numerator and denominator must all
      be ordinary digits.
      If the first digit is a superscript digit, the the numerator must be all superscript
      digits and the denominator be all subscript digits.
    len - [in]
      maximum number of characters to parse.
      You many pass -1 if s is null terminated.
    numerator - [out]
    denominator - [out]
  Returns:
    If a vulgar fraction was successfully parsed, the a pointer to the first character
    after the vulgar fraction is returned. Otherwise nullptr is returned.
  */
  static const wchar_t* ParseVulgarFraction(
    const wchar_t* s,
    int len,
    int& numerator,
    int& denominator
  );

  /*
  Description:
    Checks if a string would need to be encoded before being used in XML.
  Returns:
    True if the string contains any character that is not allowed in an XML string.
    Such characters must be encoded before they can appear in the XML string.
    e.g., a 'less than' angle bracket (<) would be encoded as "&lt;".
  */
  bool NeedsXMLEncode(void) const;

  /*
    Description:
      Returns true if c is one of the five XML special characters:
      & (ampersand),
      < (less than),
      > (greater than),
      " (quotation mark),
      ' (apostrophe).
  */
  static bool IsXMLSpecialCharacter(wchar_t c);

  /*
    Description:
      Returns true if this string is one of the five XML special character encodings:
      &amp; (ampersand),
      &lt; (less than),
      &gt; (greater than),
      &quot; (quotation mark),
      &apos; (apostrophe).

      In addition checks for &#10; and returns true if so.
  */
  bool IsXMLSpecialCharacterEncoding(void) const;

  static wchar_t* Reverse(
    wchar_t* string,
    int element_count
    );

  void MakeUpperOrdinal();
	void MakeLowerOrdinal();
	void MakeReverse();

  /*
  Description:
    Removes leading elements from the string.
  Parameters:
    s - [in]
      All leading wchar_t elements with a value that it found in s[] are removed.
      If s is nullptr, All leading wchar_t element with a value between 1 and space are removed.
  */
  void TrimLeft(const wchar_t* s = nullptr);

  /*
  Description:
    Removes trailing elements from the string.
  Parameters:
    s - [in]
      All trailing wchar_t elements with a value that it found in s[] are removed.
      If s is nullptr, All trailing wchar_t elements with a value between 1 and space are removed.
  */
  void TrimRight(const wchar_t* s = nullptr);

  void TrimLeftAndRight(const wchar_t* s = nullptr);

  /*
  Description:
    Remove all occurrences of the input character.
  Parameters:
    c - [in]
      utf8_single_byte_ct must have a value between 0 and 0x7F.
      When w is a 2 byte UTF-16 wchar_t value (like Microsoft's wchar_t), 
      it must be in the range 0 to 0xD7FF or 0xE000 to 0xFFFF.
      When w is a 4 byte UTF-32 wchar_t value (like Apple's wchar_t), 
      it must be in the range 0 to 0xD7FF or 0xE000 to 0x10FFFF.
      Returns:
    Number of characters removed.
  */
  int Remove(
    char utf8_single_byte_c
    );
  int Remove(
    unsigned char utf8_single_byte_c
    );
  int Remove(
    wchar_t w
    );

  /*
  Description:
    Find the first occurrence of a character or substring.
  Parameters:
    utf8_single_byte_c - [in]
      utf8_single_byte_c must have a value between 0 and 0x7F.
    w - [in]
      When w is a 2 byte UTF-16 wchar_t value (like Microsoft's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0xFFFF.
      When w is a 4 byte UTF-32 wchar_t value (like Apple's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0x10FFFF.
      sTUF8 - [in]
      A null terminated UTF-8 string.
    wcharString - [in]
      A null terminated wchar_t string.
  start_index - [in]
      Index where search should begin.
  Returns:
    -1: invalid input or character is not in the string.
    >=0: Index of the first instance of the character.
  */
  int Find(
    char utf8_single_byte_c
    ) const;
  int Find(
    unsigned char utf8_single_byte_c
    ) const;
  int Find(
    wchar_t w
    ) const;
  int Find(
    const char* sUTF8
    ) const;
  int Find(
    const unsigned char* sUTF8
    ) const;
  int Find(
    const wchar_t* wcharString
    ) const;
  int Find(
    char utf8_single_byte_c,
    size_t start_index
    ) const;
  int Find(
    unsigned char utf8_single_byte_c,
    size_t start_index
    ) const;
  int Find(
    wchar_t w,
    size_t start_index
    ) const;
  int Find(
    wchar_t w,
    int start_index
    ) const;
  int Find(
    const char* sUTF8,
    size_t start_index
    ) const;
  int Find(
    const unsigned char* sUTF8,
    size_t start_index
    ) const;
  int Find(
    const wchar_t* wcharString,
    size_t start_index
    ) const;
  int Find(
    const wchar_t* wcharString,
    int start_index
    ) const;
  int FindOneOf(
    const wchar_t* character_set
    ) const;


  /*
  Description:
    Find the last occurrence of a character or substring.
  Parameters:
    utf8_single_byte_c - [in]
      utf8_single_byte_c must have a value between 0 and 0x7F.
    w - [in]
      When w is a 2 byte UTF-16 wchar_t value (like Microsoft's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0xFFFF.
      When w is a 4 byte UTF-32 wchar_t value (like Apple's wchar_t),
      it must be in the range 0 to 0xD7FF or 0xE000 to 0x10FFFF.
      sUTF8 - [in]
    wideString - [in]
  Returns:
    -1: Invalid input or character or substring was not found.
    >=0: Index of the final occurrence of the character or substring.
  */
  int ReverseFind(
    char utf8_single_byte_c
    ) const;
	int ReverseFind(
    unsigned char utf8_single_byte_c
    ) const;
	int ReverseFind(
    wchar_t w
    ) const;
  int ReverseFind(
    const char* sUTF8
    ) const;

  int ReverseFind(
    const wchar_t* wideString
    ) const;


  // It is common to format single numbers into strings
  // and the FromNumber and FromDouble functions are
  // the fastest way to do this and provide consistent results.
  // They return a pointer to their buffer so the can be used
  // as function parameters.
  static const MYON_wString FromNumber(
    char n
    );
  static const MYON_wString FromNumber(
    unsigned char n
    );
  static const MYON_wString FromNumber(
    short n
    );
  static const MYON_wString FromNumber(
    unsigned short n
    );
  static const MYON_wString FromNumber(
    int n
    );
  static const MYON_wString FromNumber(
    unsigned int n
    );
  static const MYON_wString FromNumber(
    MYON__INT64 n
    );
  static const MYON_wString FromNumber(
    MYON__UINT64 n
    );
  static const MYON_wString FromNumber(
    double d // "%g" format
    );
  static const MYON_wString ApproximateFromNumber(
    double d // "%f" when possible, otherwise "%g"
    );
  static const MYON_wString PreciseFromNumber(
    double d // "%.17g"
    );

  /*
  Description:
    Get the Gregorian calendar current coordinated universal time as a string.
  Parameters:
    date_format - [in]
    date_format - [in]
    time_format - [in]
    date_separator - [in]
      Character placed between the year, month and day values.
      If 0, then MYON_wString::HyphenMinus is used to
    date_time_separator - [in]
      Character placed between the date and time.
      If 0, then MYON_wString::Space is used.
    time_separator - [in]
      Character placed between the hour, minute, and second values.
      If 0, then : (colon) is used.
  Returns:
    A string value for the current coordinated universal time (UTC).
  */  
  static const MYON_wString FromCurrentCoordinatedUniversalTime(
    MYON_DateFormat date_format,
    MYON_TimeFormat time_format,
    wchar_t date_separator,
    wchar_t date_time_separator,
    wchar_t time_separator
  );

  /*
  Description:
    Get the Gregorian calendar date and time as a string.
  Parameters:
    t - [in]
      Gregorian calendar time to format
    date_format - [in]
    date_format - [in]
    time_format - [in]
    date_separator - [in]
      Character placed between the year, month and day values.
      If 0, then MYON_wString::HyphenMinus is used to
    date_time_separator - [in]
      Character placed between the date and time.
      If 0, then MYON_wString::Space is used.
    time_separator - [in]
      Character placed between the hour, minute, and second values.
      If 0, then : (colon) is used.
  */
  static const MYON_wString FromTime(
    const struct tm& t,
    MYON_DateFormat date_format,
    MYON_TimeFormat time_format,
    wchar_t date_separator,
    wchar_t date_time_separator,
    wchar_t time_separator
  );
  
  /*
  Description:
    Get the Gregorian calendar date and time as a string.
  Parameters:
    seconds - [in]
      number of seconds since January 1, 1970 00:00:00.
    date_format - [in]
    date_format - [in]
    time_format - [in]
    date_separator - [in]
      Character placed between the year, month and day values.
      If 0, then MYON_wString::HyphenMinus is used to
    date_time_separator - [in]
      Character placed between the date and time.
      If 0, then MYON_wString::Space is used.
    time_separator - [in]
      Character placed between the hour, minute, and second values.
      If 0, then : (colon) is used.
  */
  static const MYON_wString FromSecondsSinceJanuaryFirst1970(
    MYON__UINT64 seconds_since_jan_first_1970,
    MYON_DateFormat date_format,
    MYON_TimeFormat time_format,
    wchar_t date_separator,
    wchar_t date_time_separator,
    wchar_t time_separator
  );


  /*
  Description:
    Get the Gregorian calendar date and time as a string.
    The year value must be >= 1582 (beginning of Gregorian calendar).
  Parameters:
    year - [in]
      year >= 1582 (beginning of Gregorian calendar)
    month - [in]
      1 to 12
    mday - [in]
      1 to 31
    hour - [in]
     0 to 23
    minute - [in]
     0 to 59
    second - [in]
     0 to 59
    date_format - [in]
    date_format - [in]
    time_format - [in]
    date_separator - [in]
      Character placed between the year, month and day values.
      If 0, then MYON_wString::HyphenMinus is used to
    date_time_separator - [in]
      Character placed between the date and time.
      If 0, then MYON_wString::Space is used.
    time_separator - [in]
      Character placed between the hour, minute, and second values.
      If 0, then : (colon) is used.
  */
  static const MYON_wString FromYearMonthDayHourMinuteSecond(
    int year,
    int month,
    int mday,
    int hour,
    int minute,
    int second,
    MYON_DateFormat date_format,
    MYON_TimeFormat time_format,
    wchar_t date_separator,
    wchar_t date_time_separator,
    wchar_t time_separator
  );

  /*
  Description:
    Get the Gregorian calendar date and time as a string.
    The year value must be >= 1582 (beginning of Gregorian calendar).
  Parameters:
    year - [in]
      year >= 1582 (beginning of Gregorian calendar)
    day_of_year - [in]
     1 to 366
    hour - [in]
     0 to 23
    minute - [in]
     0 to 59
    second - [in]
     0 to 59
    date_format - [in]
    date_format - [in]
    time_format - [in]
    date_separator - [in]
      Character placed between the year, month and day values.
      If 0, then MYON_wString::HyphenMinus is used to
    date_time_separator - [in]
      Character placed between the date and time.
      If 0, then MYON_wString::Space is used.
    time_separator - [in]
      Character placed between the hour, minute, and second values.
      If 0, then : (colon) is used.
  */
  static const MYON_wString FromYearDayHourMinuteSecond(
    int year,
    int day_of_year,
    int hour,
    int minute,
    int second,
    MYON_DateFormat date_format,
    MYON_TimeFormat time_format,
    wchar_t date_separator,
    wchar_t date_time_separator,
    wchar_t time_separator
  );

  /*
  Description:
    Convert a list of Unicode code points into a wide string.
  Parameters:
    code_points - [in]
      array of Unicode code points
    code_point_count - [in]
      number of code points.
      -1 indicates code_points[] is terminated by a 0 value.
    error_code_point - [in]
      If error_code_point is a valid Unicode code point,
      then error_code_point will be used in place of invalid values in code_points[].
      Otherwise, conversion will terminate if code_points[] contains an invalid value.
      The values MYON_UnicodeCodePoint::MYON_ReplacementCharacter and
      MYON_UnicodeCodePoint::MYON_InvalidCodePoint are commonly used for this parameter.
  Returns:
    A wide string encoding of the Unicode code points.
  Remarks:
    If more control over the conversion process is required, 
    then use MYON_ConvertUTF32ToWideChar().
  */
  static const MYON_wString FromUnicodeCodePoints(
    const MYON__UINT32* code_points,
    int code_point_count,
    MYON__UINT32 error_code_point
  );

  static const MYON_wString FromUnicodeCodePoint(
    MYON__UINT32 code_point
  );

  /*
  Description:
    Each byte value is converted to 2 hexadecimal digits.
  Parameters:
    bytes - [in]
      list of byte values
    byte_count - [in]
      Number of byte values in bytes[] array.
    bCapitalDigits - [in]
      false: Use 0-9, a - b
      true: Use 0-9, A - F
    bReverse - [in]
      false: 
        The digits in the string will be in the order
        bytes[0], bytes[1], ..., bytes[byte_count-1].
      true: 
        The digits in the string will be in the order
        bytes[byte_count-1], ..., bytes[1], bytes[0].
  */
  static const MYON_wString HexadecimalFromBytes(
    const MYON__UINT8* bytes,
    size_t byte_count,
    bool bCapitalDigits,
    bool bReverse
    );


  /*
  Parameters:
    format - [in]
      Format control.
      Positional parameters of the form %N$x where N >= 1 and x
      is the standard format specification are supported.
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
      arguments for replacable items in the format string.
  Returns:
    True if successful.
    False if the string is too long or the format string is not valid.
  Remarks:
    When using Microsoft's compiler and other compilers that provide similar
    locale support, the locale is the invariant culture locale returned by
    MYON_Locale::InvariantCulture::LocalePtr().

    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly recommended that you never use %S to
    include any string that may possibly contain elements with values > 127.
    The following examples illustrate a way to predictably use UTF-8 and wchar_t
    parameters in buffers of the other element type.

    const char* utf8_string = ...;
    // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
    // The code below will treat utf8_string as a UTF-8 encoded string.
    wchar_t wchar_buffer[...];
    const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
    MYON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", MYON_wString(utf8_string));

    const wchar_t* wide_string = ...;
    // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",char_string);
    // The code below will include wide_string as a UTF-8 encoded string.
    char char_buffer[...];
    const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
    MYON_String::Format(char_buffer, char_buffer_capacity, "%s", MYON_String(wide_string));
  */
  bool MYON_VARGS_FUNC_CDECL Format(
    const wchar_t* format,
    ...
    );

  static const MYON_wString MYON_VARGS_FUNC_CDECL FormatToString(
    const wchar_t* format,
    ...
    );

  bool FormatVargs(
    const wchar_t* format,
    va_list args
    );

  /*
  Description:
    A platform independent, secure, culture invariant way to format a wchar_t string
    with support for positional format parameters.
    This function is designed to be used when it is critical that 
    the formatting be platform independent, secure and culture invariant.
  Parameters:
    buffer - [out] 
      not null
    buffer_capacity - [in]
      > 0
      Number of wchar_t elements in buffer.
    format - [in]
      Avoid using %S (capital S).  See the Remarks for details.
    ... - [in]
  Returns:
    >= 0: 
      The number of wchar_t elements written to buffer[], not including the null terminator.
      A null terminator is always added (buffer[returned value] = 0).
      The last element of buffer[] is always set to zero (buffer[buffer_capacity-1] = 0).
    < 0: failure:
      If buffer is not null and buffer_capacity > 0, then buffer[0] = 0 and buffer[buffer_capacity-1] = 0;
  Remarks:
    The way Windows handles the %S (capital S) format parameter depends on locale
    and code page settings.  It is strongly recommended that you never use %S to
    include any string that may possibly contain elements with values > 127.
    The following examples illustrate a way to predictably use UTF-8 and wchar_t
    parameters in buffers of the other element type.

         const char* utf8_string = ...;
         // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",utf8_string);
         wchar_t wchar_buffer[...];
         const size_t wchar_buffer_capacity= sizeof(buffer)/sizeof(buffer[0]);
         MYON_wString::Format(wchar_buffer, wchar_buffer_capacity, "%s", MYON_wString(utf8_string));

         const wchar_t* wide_string = ...;
         // UNRELIABLE // MYON_wString::Format(buffer,buffer_capacity,"%S",char_string);
         char char_buffer[...];
         const size_t char_buffer_capacity = sizeof(buffer)/sizeof(buffer[0]);
         MYON_String::Format(char_buffer, char_buffer_capacity, "%s", MYON_String(wide_string));
  */
  static int MYON_VARGS_FUNC_CDECL FormatIntoBuffer(
    wchar_t* buffer, 
    size_t buffer_capacity,
    const wchar_t* format,
    ...
    );

  static int MYON_VARGS_FUNC_CDECL FormatIntoBuffer(
    MYON_wStringBuffer& buffer,
    const wchar_t* format,
    ...
    );

  static int FormatVargsIntoBuffer(
    MYON_wStringBuffer& buffer,
    const wchar_t* format,
    va_list args
    );

  static int FormatVargsIntoBuffer(
    wchar_t* buffer,
    size_t buffer_capacity,
    const wchar_t* format,
    va_list args
    );

  /*
  Returns:
    >= 0:
     Number of char elements in the formatted string, not including the null terminator.
    < 0:
      Invalid input
  */
  static int FormatVargsOutputCount(
    const wchar_t* format,
    va_list args
    );

  /*
  Returns:
    A Unicode encoding of the fraction numerator/denominator with the minimum number of characters.
    The fraction is reduced and the encoding is a short as possible.
  */
  static const MYON_wString FormatToVulgarFraction(int numerator, int denominator);

  /*
  Returns:
    A Unicode encoding of the fraction numerator/denominator with the minimum number of characters.
  bReduced - [in]
    When in doubt, pass true.
    If true, then the reduced fraction will be returned.
    For example, if bReduced is true, then 2/4 reduces to 1/2.
  bProper - [in]
    When in doubt, pass true.
    If true, then proper fractions will be returned when abs(numerator)>=abs(denominator).
    For example, if bProper is true, then 4/3 is converted to 1-1/3. 
    The symbol between the whole number and the proper fraction is specified by
    mixed_fraction_separator_code_point.
  proper_fraction_separator_cp - [in]
    Species the Unicode code point of the symbol used to
    separate the whole number and the proper fraction.

    When in doubt, pass MYON_UnicodeCodePoint::MYON_NullCodePoint (0) which will result in the
    large whole number being next to the proper fraction in a readable and compact manner.

    Other options include:

    Spaces:
      MYON_UnicodeCodePoint::MYON_NarrowNoBreakSpace
      MYON_UnicodeCodePoint::MYON_NoBreakSpace
      MYON_UnicodeCodePoint::MYON_ZeroWidthSpace
      MYON_UnicodeCodePoint::MYON_Space
      MYON_UnicodeCodePoint::MYON_EnSpace
      MYON_UnicodeCodePoint::MYON_EmSpace
      MYON_UnicodeCodePoint::MYON_FigureSpace
      MYON_UnicodeCodePoint::MYON_MediumMathematicalSpace
      MYON_UnicodeCodePoint::MYON_ThinSpace
      MYON_UnicodeCodePoint::MYON_HairSpace
      MYON_UnicodeCodePoint::MYON_PunctuationSpace
      MYON_UnicodeCodePoint::MYON_ThreePerEmSpace
      MYON_UnicodeCodePoint::MYON_FourPerEmSpace
      MYON_UnicodeCodePoint::MYON_SixPerEmSpace

    Hyphens:
      MYON_UnicodeCodePoint::MYON_HyphenMinus
      MYON_UnicodeCodePoint::MYON_UnambiguousHyphen
      MYON_UnicodeCodePoint::MYON_NoBreakHyphen
      MYON_UnicodeCodePoint::MYON_SmallHyphen

    Dashes:
      MYON_UnicodeCodePoint::MYON_FigureDash
      MYON_UnicodeCodePoint::MYON_EnDash
      MYON_UnicodeCodePoint::MYON_EmDash

  bUseVulgarFractionCodePoints - [in]
    When in doubt, pass true.
    If true and a single Unicode code point exists for the vulgar fraction
    (1/2, 1/3, 2/3, 1/4, 3/4, 1/5, 2/5, 3/5, 4/5, 1/6, 5/6, 1/7, 1/8, 3/8, 5/8, 7/8, 1/9, 1/10),
    then that code point will be used.
    Otherwise a Unicode superscript digits, MYON_UnicodeCodePoint::FractionSlash, and Unicode subscript digits
    are used.
  */
  static const MYON_wString FormatToVulgarFraction(
    int numerator,
    int denominator,
    bool bReduced,
    bool bProper,
    unsigned int proper_fraction_separator_cp,
    bool bUseVulgarFractionCodePoints
    );

  /*
  numerator - [in]
    A string (digits, signs, parenthesis).
  denominator - [in]
    A string (digits, signs, parenthesis).
  bUseVulgarFractionCodePoints - [in]
    If true and if Unicode code point exists for the fraction (halves, thirds, fourths, fifths, sixths, eights, ...),
    that code point will be used;    
  Returns:
    A Unicode encoding of the fraction numerator/denominator.
  */
  static const MYON_wString FormatToVulgarFraction(
    const MYON_wString numerator, 
    const MYON_wString denominator
  );

  /*
  numerator - [in]
    A string (digits, signs, parenthesis).
  Returns:
    A Unicode encoding of the fraction's numerator.
  */
  static const MYON_wString FormatToVulgarFractionNumerator(const MYON_wString numerator);

  /*
  denominator - [in]
    A string (digits, signs, parenthesis).
  Returns:
    A Unicode encoding of the fraction's denominator.
  */
  static const MYON_wString FormatToVulgarFractionDenominator(const MYON_wString denominator);

  /*
    Returns a string with the code point U+2044.
  */
  static const MYON_wString VulgarFractionSlash();

  /*
  Parameters:
    format - [in]
      null terminated string to scan
    ... - [out]
      pointers to elements to assign.
  Returns:
    >= 0: number of fields successfully converted and assigned.
    <0:  failure
  */
  int MYON_VARGS_FUNC_CDECL Scan(
    const wchar_t* format,
    ...
    ) const;

  static int MYON_VARGS_FUNC_CDECL ScanBuffer(
    const wchar_t* buffer,
    const wchar_t* format,
    ...
    );

  static int ScanBufferVargs(
    const wchar_t* buffer,
    const wchar_t* format,
    va_list args
    );

  /*
  Description:
    Parses buffer to extract a number.
    Sperscript and supscript numbers are supported.
  Returns:
    not zero:
      pointer to the first character that was not scanned
    nullptr:
      failure
  */
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    char value_on_failure,
    char* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    unsigned char value_on_failure,
    unsigned char* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    short value_on_failure,
    short* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    unsigned short value_on_failure,
    unsigned short* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    int value_on_failure,
    int* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    unsigned int value_on_failure,
    unsigned int* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    MYON__INT64 value_on_failure,
    MYON__INT64* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    MYON__UINT64 value_on_failure,
    MYON__UINT64* value
    );
  static const wchar_t* ToNumber(
    const wchar_t* buffer,
    double value_on_failure,
    double* value
    );

  /*
  Parameters:
    sz - [in]
      number of bytes.
  Returns:
    If sz < 0, "0 bytes" is returned.
    If 0 <= sz <= 9999, "x bytes" is returned where x is an exact decimal value.
    If s > 9999, then a description with 3 significant digits and a suffix indicating
    the order of magnitude is returned. The order of magnitude is described by appending
    KB (1024 bytes), MB (1024 KB), GB (1024 MB), TB (1024 GB), or PB (1024 TB).
  */
  static const MYON_wString ToMemorySize(size_t sz);

  /*
  Description:
    Expert user function to reserve and gain access to string memory.
  Parameters:
    capacity - [in]
      If capacity > MYON_String::MaximumStringLength, then nullptr is returned.
      If capacity <= 0, then nullptr is returned.
  Returns:
    A pointer to the string buffer or nullptr on failure.
  */
  wchar_t* ReserveArray(size_t capacity);

  /*
  Description:
    Shrinks the internal storage to the minimum required size.
  */
  void ShrinkArray();

  /*
  Description:
    Set the length of the string in characters.
  Parameters:
    length - [in]
    If length > MYON_String::MaximumStringLength, then nullptr is returned.
    If length <= 0, then nullptr is returned.
  Returns:
    A pointer to the string buffer or nullptr on failure.
  */
  wchar_t* SetLength(size_t);

  wchar_t* Array();
  const wchar_t* Array() const;

  /*
  Returns:
    A duplicate of this that does not share memory with any other string.
    (A new array is allocated for the returned string.)
  */
  const MYON_wString Duplicate() const;

  /*
  Returns:
    Total number of bytes of memory used by this class.
    For use in MYON_Object::SizeOf() overrides.
  */
  unsigned int SizeOf() const;

  /*
  Returns:
    A CRC of the string.
  */
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  /*
  Returns:
    A CRC of the lower case version of the string.
    Useful for case insensitive CRCs and hash codes.
  */
  MYON__UINT32 DataCRCLower(MYON__UINT32 current_remainder) const;

  /*
  OBSOLETE - Use MYON_FileSystemPath::SplitPath
  */
  static void SplitPath(const  char  * path, MYON_wString* drive, MYON_wString* dir, MYON_wString* fname, MYON_wString* ext);
  static void SplitPath(const wchar_t* path, MYON_wString* drive, MYON_wString* dir, MYON_wString* fname, MYON_wString* ext);

  /*
  Description:
    Sets the string to a copy of a character array.
  Returns:
    This string after being set.
  */
  const MYON_wString& Set(const wchar_t* wsz, int numChars);

  /*
  Description:
    Checks if the string starts with a sub-string (case-sensitive check).
  Returns:
    True if the string starts with wszSub.
  */
  bool StartsWith(const wchar_t* wszSub) const;

  /*
  Description:
    Checks if the string starts with a sub-string (case-insensitive check).
  Returns:
    True if the string starts with wszSub.
  */
  bool StartsWithNoCase(const wchar_t* wszSub) const;

  /*
  Description:
    Counts the number of characters in the string that match a character.
  Returns:
    The number of characters in the string that match ch.
  */
  int Count(wchar_t ch) const;

  /*
  Description:
    Checks if the string contains a substring (case-sensitive check).
  Params:
    wszSub [in] - The substring to check.
  Returns:
    True if the string contains the substring, else false.
  */
  bool Contains(const wchar_t* wszSub) const;

  /*
  Description:
    Checks if the string contains a substring (case-insensitive check).
  Params:
    wszSub [in] - The substring to check.
  Returns:
    True if the string contains the substring, else false.
  */
  bool ContainsNoCase(const wchar_t* wszSub) const;

  /*
  Description:
    Truncates the string to contain only the characters starting at pos.
    e.g., "Hello" with pos=2 results in "llo".
    This is similar to Mid(int) except that the string itself is truncated.
  Params:
    pos [in] - The character position to truncate at. Must be inside the string.
  Returns:
    True if successful, false on failure.
  */
  bool TruncateMid(int pos);

  /*
  Description:
    Insert character 'ch' at 'index', 'count' times.
  Returns:
    True if successful, false if index or count are invalid.
  */
  bool Insert(int index, wchar_t ch, int count);

  /*
  Description:
    Insert string 'wsz' at 'index'.
  Returns:
    True if successful, false if index or wsz is invalid.
  */
  bool Insert(int index, const wchar_t* wsz);

  bool IsValidIntegerNumber(void) const;
  bool IsValidRealNumber(void) const;
  bool IsValid4dPoint(void) const;
  bool IsValid3dPoint(void) const;
  bool IsValid2dPoint(void) const;
  bool IsValidMatrix (void) const;

public:
  ~MYON_wString();

protected:
  // Implementation
  wchar_t* m_s; // pointer to ref counted string array
                // m_s - 12 bytes points at the string's MYON_wStringHeader

  // implementation helpers
  class MYON_wStringHeader* Header() const;
  class MYON_wStringHeader* IncrementedHeader() const;
  wchar_t* CreateArray(int);
  void CopyArray();
  void CopyToArray(const MYON_wString&);
  void CopyToArray(int, const char*);
  void CopyToArray(int, const unsigned char*);
  void CopyToArray(int, const wchar_t*);
  void AppendToArray(const MYON_wString&);
  void AppendToArray(int, const char*);
  void AppendToArray(int, const unsigned char*);
  void AppendToArray(int, const wchar_t*);
};

/*
Returns:
  True if lhs and rhs are identical as arrays of wchar_t elements.  
*/
MYON_DECL
bool operator==( const MYON_wString& lhs, const MYON_wString& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of wchar_t elements.  
*/
MYON_DECL
bool operator!=(const MYON_wString& lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator<(const MYON_wString& lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator>(const MYON_wString& lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator<=(const MYON_wString& lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator>=(const MYON_wString& lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of wchar_t elements.  
*/
MYON_DECL
bool operator==( const MYON_wString& lhs, const wchar_t* rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of wchar_t elements.  
*/
MYON_DECL
bool operator!=(const MYON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is less than rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator<(const MYON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator>(const MYON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator<=(const MYON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator>=(const MYON_wString& lhs, const wchar_t* rhs);

/*
Returns:
  True if lhs and rhs are identical as arrays of wchar_t elements.  
*/
MYON_DECL
bool operator==( const wchar_t* lhs, const MYON_wString& rhs );

/*
Returns:
  True if lhs and rhs are not identical as arrays of wchar_t elements.  
*/
MYON_DECL
bool operator!=(const wchar_t* lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is less than rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator<(const wchar_t* lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is greater than rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator>(const wchar_t* lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is less than or equal to rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator<=(const wchar_t* lhs, const MYON_wString& rhs);

/*
Returns:
  True if lhs is greater than or equal to rhs as an array of wchar_t elements.
*/
MYON_DECL
bool operator>=(const wchar_t* lhs, const MYON_wString& rhs);

/*
Description:
  An MYON_NameHash is designed to help search for and compare attribute names
  like the MYON_ModelComponent.Name() value.

  If a name is wchar_t encoded as wide_char_name and UTF-8 encoded as utf8_name,
  then MYON_NameHash(wide_char_name) == MYON_NameHash(utf8_name).

  Set:
    bool bEqualNameHash = MYON_NameHash::Create(parent_id1,name1) == MYON_NameHash::Create(parent_id2,name2);
    bool bEqualParentId = (parent_id1 == parent_id2)
    bool bEqualAttributeName = MYON_String::EqualAttributeName(name1,name2);
  
  If (bEqualParentId && bEqualAttributeName) is true, then bEqualNameHash is true.
  If bEqualParentId is false, then bEqualNameHash is false.
  With probability 1-epsilon, if bEqualAttributeName is false, then bEqualNameHash is false, 
  where epsilon is an extremely tiny number.
*/
class MYON_CLASS MYON_NameHash
{
public:
  /*
    Default conststruction creates MYON_NameHash::UnsetNameHash.
  */
  MYON_NameHash() = default;
  ~MYON_NameHash() = default;
  MYON_NameHash(const MYON_NameHash&) = default;
  MYON_NameHash& operator=(const MYON_NameHash&) = default;

  static const MYON_NameHash EmptyNameHash;
  static const MYON_NameHash UnsetNameHash;

  /*
  Returns:
    True if the hash is for a valid non-empty name.
  */
  bool IsValidAndNotEmpty() const;

  /*
  Returns:
    True if the hash is for an empty name.
  */
  bool IsEmptyNameHash() const;

  /*
  Returns:
    True if the hash is for an invalid name.
  */
  bool IsInvalidNameHash() const;

  /*
  Returns:
    True if the hash is not empty and was set by calling CreateFilePathHash().
  */
  bool IsFilePathHash() const;

  /*
  Returns:
    True if hash is set and case is ignored
  */
  bool IgnoreCase() const;

  /*
  Returns:
    MYON_NameHash::EmptyNameHash if name is empty.
    MYON_NameHash::UnsetNameHash if name is not valid.
  */
  static MYON_NameHash Create(
    const MYON_wString& name
    );
  static MYON_NameHash Create(
    const wchar_t* name
    );
  static MYON_NameHash Create(
    size_t length,
    const wchar_t* name
    );
  static MYON_NameHash Create(
    const MYON_String& utf8_name
    );
  static MYON_NameHash Create(
    const char* utf8_name
    );
  static MYON_NameHash Create(
    size_t length,
    const char* utf8_name
    );

  static MYON_NameHash Create(
    const MYON_UUID& parent_id, 
    const MYON_wString& name
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    const wchar_t* name
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    size_t length,
    const wchar_t* name
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    const MYON_String& name
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    const char* utf8_name
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    size_t length,
    const char* utf8_name
    );

  static MYON_NameHash Create(
    const MYON_wString& name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const wchar_t* name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    size_t length,
    const wchar_t* name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const MYON_String& name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const char* utf8_name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    size_t length,
    const char* utf8_name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id, 
    const MYON_wString& name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    const wchar_t* name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    size_t length,
    const wchar_t* name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    const MYON_String& utf8_name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    const char* utf8_name,
    bool bIgnoreCase
    );
  static MYON_NameHash Create(
    const MYON_UUID& parent_id,
    size_t length,
    const char* utf8_name,
    bool bIgnoreCase
    );

  static MYON_NameHash CreateIdAndEmptyName(
    const MYON_UUID& parent_id
    );

  static MYON_NameHash CreateIdAndUnsetName(
    const MYON_UUID& parent_id
    );

  static MYON_NameHash CreateFilePathHash(
    const class MYON_FileReference& file_reference
    );

  static MYON_NameHash CreateFilePathHash(
    const wchar_t* file_path
    );

  static MYON_NameHash CreateFilePathHash(
    const char* file_path
    );

  static int CompareNameSHA1(
    const MYON_NameHash& a,
    const MYON_NameHash& b
    );

  static int CompareNameSHA1Ptr(
    const MYON_NameHash* a,
    const MYON_NameHash* b
    );

  static int CompareParentId(
    const MYON_NameHash& a,
    const MYON_NameHash& b
    );

  static int CompareParentIdPtr(
    const MYON_NameHash* a,
    const MYON_NameHash* b
    );

  /*
  Description:
    Compares id, then length, then SHA-1 digest.
  */
  static int Compare(
    const MYON_NameHash& a,
    const MYON_NameHash& b
    );

  /*
  Description:
    Compares id, then length, then SHA-1 digest.
  */
  static int ComparePtr(
    const MYON_NameHash* a,
    const MYON_NameHash* b
    );

  // Number of UTf-32 code point values in mapped name.
  // (0 for file path hashes)
  MYON__UINT32 MappedNameCodePointCount() const;

  // SHA-1 hash of mapped name
  MYON_SHA1_Hash MappedNameSha1Hash() const;

  // 32 bit hash of id and mapped name
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  // 32 bit hash of id
  MYON__UINT32 IdCRC(MYON__UINT32 current_remainder) const;

  // 32 bit hash of id and mapped name
  MYON__UINT32 NameCRC(MYON__UINT32 current_remainder) const;

  MYON_UUID ParentId() const;
  
private:

  enum : unsigned int
  {
    flags_length_mask = 0x0FFFFFFF,
    flags_case_sensitive = 0x80000000,
    flags_file_path = 0xFFFFFFFF
  };
  // m_flags = 0xFFFFFFFFU -> Hash is a file path hash
  // m_flags != 0
  // m_flags &0x0FFFFFFF = number of unicode (UTF-32) code points in name
  // m_flags &0x80000000 = 0 to ignore case, !=0 if case sensitive hash
  MYON__UINT32 m_flags = 0;

  // m_sha1_hash = SHA-1 hash of ordinal minimum mapped unicode (UTF-32) code points
  // If the name is empty, m_length = 0 and m_sha1_hash = MYON_SHA1_Hash::EmptyContentHash.
  // If the name is not valid, m_length = 0 and m_sha1_hash = MYON_SHA1_Hash::ZeroDigest.
  MYON_SHA1_Hash m_sha1_hash = MYON_SHA1_Hash::ZeroDigest; 

  // When names appear in a tree structure, m_parent_id identifies the
  // parent node.
  MYON_UUID m_parent_id = MYON_nil_uuid;

public:
  /*
  Description:
    Internal_CreateFromDotNet() is public for technical reasons. It is used
    in Rhino Common p-invoke code that provides a .NET interface to the 
    services MYON_NameHash provided by the MYON_NameHash class.
    This function should be ignored and never called from ordinary C++ code.
    If you choose to ignore the preceding admonition, you will have to read
    the source code for information about how this function works.
  */
  static MYON_NameHash Internal_DotNetInterfaceSet(
    const MYON_UUID&,
    const MYON_SHA1_Hash&,
    const MYON__UINT32
  );

  MYON__UINT32 Internal_DotNetInterfaceGetFlags() const;
};

/*
Compares id, then length, then SHA-1 digest
*/
MYON_DECL
bool operator==(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
MYON_DECL
bool operator!=(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
MYON_DECL
bool operator<(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
MYON_DECL
bool operator<=(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
MYON_DECL
bool operator>(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  );

/*
Compares id, then length, then SHA-1 digest
*/
MYON_DECL
bool operator>=(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  );

class MYON_CLASS MYON_UnitSystem
{
public:
  // Default construction sets this to MYON_UnitSystem::Meters 
  MYON_UnitSystem() = default;

  ~MYON_UnitSystem() = default;
  MYON_UnitSystem(const MYON_UnitSystem&) = default;
  MYON_UnitSystem& operator=(const MYON_UnitSystem&) = default;

public:
  MYON_UnitSystem(
    MYON::LengthUnitSystem length_unit_system
    );

  MYON_UnitSystem& operator=(
    MYON::LengthUnitSystem length_unit_system
    );

  /*
  Parameters:
    custom_unit_name - [in]
      length unit name (no spaces)
    meters_per_custom_unit - [in]
      a positive number 
  Example:
      // 1 League = 5556 meters
      const MYON_UnitSystem Leagues = MYON_UnitSystem::CreateCustomUnitSystem(L"Leagues", 1.0/5556.0);
  */
  static MYON_UnitSystem CreateCustomUnitSystem(
    const wchar_t* custom_unit_name,
    double meters_per_custom_unit
  );

public:
  // unit system = MYON::LengthUnitSystem::None. Scale to or from any valid unit = 1
  static const MYON_UnitSystem None;

  static const MYON_UnitSystem Angstroms;
  static const MYON_UnitSystem Nanometers;
  static const MYON_UnitSystem Microns;

  // unit system = MYON::LengthUnitSystem::Millimeters and meters/unit = 0.001 exactly
  static const MYON_UnitSystem Millimeters;

  static const MYON_UnitSystem Centimeters;
  static const MYON_UnitSystem Decimeters;

  // unit system = MYON::LengthUnitSystem::Meters and meters/unit = 1
  static const MYON_UnitSystem Meters;
  static const MYON_UnitSystem Dekameters;
  static const MYON_UnitSystem Hectometers;
  static const MYON_UnitSystem Kilometers;
  static const MYON_UnitSystem Megameters;
  static const MYON_UnitSystem Gigameters;
  static const MYON_UnitSystem Microinches;
  static const MYON_UnitSystem Mils;

  // unit system = MYON::LengthUnitSystem::Inches and meters/unit = 0.0254 exactly
  static const MYON_UnitSystem Inches;

  // unit system = MYON::LengthUnitSystem::Feet and meters/unit = 0.3048  exactly
  static const MYON_UnitSystem Feet;

  static const MYON_UnitSystem Yards;
  static const MYON_UnitSystem Miles;
  static const MYON_UnitSystem PrinterPoints;
  static const MYON_UnitSystem PrinterPicas;
  static const MYON_UnitSystem NauticalMiles;
  static const MYON_UnitSystem AstronomicalUnits;
  static const MYON_UnitSystem LightYears;
  static const MYON_UnitSystem Parsecs;

  // unit system = MYON::LengthUnitSystem::Unset and meters/unit = MYON_DBL_QNAN
  static const MYON_UnitSystem Unset;



public:
  bool operator==(const MYON_UnitSystem&) const;
  bool operator!=(const MYON_UnitSystem&) const;

  /*
  Returns 
    False if UnitSystem() is MYON::LengthUnitSystem::Unset.
    False if UnitSystem() is MYON::LengthUnitSystem::CustomUnits and MetersPerUnits() is not positive.
    True if UnitSystem() is MYON::LengthUnitSystem::None.
    True otherwise.
  See Also:
    IsSet()
  */  
  bool IsValid() const;

  bool Read( class MYON_BinaryArchive& );
  bool Write( class MYON_BinaryArchive& ) const;
  void Dump( class MYON_TextLog& ) const;

  const MYON_wString ToString() const;

  /*
  Returns
    True if UnitSystem() is neither MYON::LengthUnitSystem::Unset nor MYON::LengthUnitSystem::None
    and IsValid() is true.
  See Also:
    IsValid()
  */
  bool IsSet() const;

  /*
  Returns
    true If the unit system is MYON::LengthUnitSystem::CustomUnits
  */
  bool IsCustomUnitSystem() const;

  void SetUnitSystem(
    MYON::LengthUnitSystem us
    );

  /*
  Parameters:
    custom_unit_name - [in]
      length unit name (no spaces)
    meters_per_custom_unit - [in]
      a positive number 
  Example:
      // 1 League = 5556 meters
      MYON_UnitSystem Leagues;
      Leagues.SetCustomUnitSystem( L"Leagues", 1.0/5556.0);
      // or
      MYON_UnitSystem Leagues = MYON_UnitSystem::CreateCustomUnitSystem(L"Leagues", 1.0/5556.0);
  */
  void SetCustomUnitSystem(
    const wchar_t* custom_unit_name,
    double meters_per_custom_unit
    );

  /*
  Description:
    Changes the unit system to custom units and sets the custom unit scale.
  Remarks:
    Avoid using this function. Use SetCustomUnitSystem() or SetUnitSystem()
    instead.
  */
  MYON_DEPRECATED_MSG("Use SetCustomUnitSystem()")
  void SetCustomUnitSystemName(
    const wchar_t* custom_unit_name
    );

  /*
  Description:
    Changes the unit system to custom units and sets the custom unit scale.
  Parameters:
    meters_per_custom_unit - [in]
      a positive number
  Remarks:
    Avoid using this function. Use SetCustomUnitSystem() or SetUnitSystem()
    instead.
  */
  MYON_DEPRECATED_MSG("Use SetCustomUnitSystem()")
    void SetCustomUnitSystemScale(
    double meters_per_custom_unit
    );

  /// NOTE WELL:
  ///   For standard units, MYON_UnitSystem::MetersPerUnit() returns the inverse of the correct value.
  ///   The reason is the VRay plug-in for Rhino 6 assumes the incorrect value is returned
  ///   and does not work correctly in Rhino 7 if the correct value is returned.
  MYON_DEPRECATED_MSG("MetersPerUnit() returns the wrong value. Use this->MetersPerUnit(MYON_DBL_QNAN)")
  double MetersPerUnit() const;

  /*
  Parameters:
    unset_return_value - [in]
      Value to return when this->UnitSystem() is MYON::LengthUnitSystem::Unset.
      When in doubt, use MYON_DBL_QNAN.
  Returns:
    If this->UnitSystem() is MYON::LengthUnitSystem::CustomUnits, then the value set
    by SetCustomUnitSystemScale() is returned.
    If this->UnitSystem() is MYON::LengthUnitSystem::Unset, then unset_return_value is returned.
    If this->UnitSystem() is MYON::LengthUnitSystem::None, then 1.0 is returned.
    Otherwise, MYON::UnitScale(this->UnitSystem(), MYON::LengthUnitSystem::Meters) is returned.
  */
  double MetersPerUnit(
    double unset_return_value
  ) const;

  /*
  Parameters:
    unset_return_value - [in]
      Value to return when this->UnitSystem() is MYON::LengthUnitSystem::Unset.
      When in doubt, use MYON_DBL_QNAN.
  Returns:
    If this->UnitSystem() is MYON::LengthUnitSystem::CustomUnits, then the 1000 times the
    value set by SetCustomUnitSystemScale() is returned.
    If this->UnitSystem() is MYON::LengthUnitSystem::Unset, then unset_return_value is returned.
    If this->UnitSystem() is MYON::LengthUnitSystem::None, then 1.0 is returned.
    Otherwise, MYON::UnitScale(this->UnitSystem(), MYON::LengthUnitSystem::Millimeters) is returned.
  */
  double MillimetersPerUnit(
    double unset_return_value
  ) const;

  MYON::LengthUnitSystem UnitSystem() const;

  /*
  Returns:
    US English lower case plural unit system name (meters, inches, etc.).
  */
  const MYON_wString& UnitSystemName() const;
private:
  MYON::LengthUnitSystem m_unit_system = MYON::LengthUnitSystem::Meters;
  unsigned int m_reserved = 0;

  // The m_meters_per_custom_unit and m_custom_unit_name values apply when 
  // m_unit_system = MYON::LengthUnitSystem::CustomUnits.
  // In all other cases they should be ignored.
  double m_meters_per_custom_unit = 1.0;  // 1 meter = m_meters_per_custom_unit custom units
  MYON_wString m_custom_unit_name;   // name of custom units
};
#endif

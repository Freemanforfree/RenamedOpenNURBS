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

#if !defined(MYON_ARRAY_INC_)
#define MYON_ARRAY_INC_


////////////////////////////////////////////////////////////////
//
// The MYON_SimpleArray<> template is more efficient than the
// MYON_ClassArray<> template, but MYON_SimpleArray<> should not
// be used for arrays of classes that require explicit
// construction, destruction, or copy operators.
//
// Elements returned by AppendNew() are memset to zero.
//
// By default, MYON_SimpleArray<> uses onrealloc() to manage
// the dynamic array memory. If you want to use something 
// besides onrealloc() to manage the array memory, then override
// MYON_SimpleArray::Realloc().

template <class T> class MYON_SimpleArray
{
public:
  // construction ////////////////////////////////////////////////////////

  // These constructors create an array that uses onrealloc() to manage
  // the array memory.
  MYON_SimpleArray() MYON_NOEXCEPT;

  virtual
  ~MYON_SimpleArray();

  // Copy constructor
  MYON_SimpleArray( const MYON_SimpleArray<T>& );

  ////// Assignment operator
  //////   Making a virtual operator= was a mistake.
  //////   One reason might have been that the operator is virtual
  //////   so MYON_UuidList::operator= will be called when one is
  //////   passed as an MYON_SimpleArray<MYON_UUID>& to a function?
  ////virtual
  MYON_SimpleArray<T>& operator=( const MYON_SimpleArray<T>& );

#if defined(MYON_HAS_RVALUEREF)
  // Clone constructor
  MYON_SimpleArray( MYON_SimpleArray<T>&& ) MYON_NOEXCEPT;

  // Clone assignment
  MYON_SimpleArray<T>& operator=( MYON_SimpleArray<T>&& ) MYON_NOEXCEPT;
#endif

  MYON_SimpleArray(size_t); // size_t parameter = initial capacity

  // emergency bailout ///////////////////////////////////////////////////
  void EmergencyDestroy(void); // call only when memory used by this array
                               // may have become invalid for reasons beyond
                               // your control. EmergencyDestroy() zeros
                               // anything that could possibly cause
                               // ~MYON_SimpleArray() to crash.

  // query ///////////////////////////////////////////////////////////////
  
	int Count() const;      // number of elements in array
  unsigned int UnsignedCount() const;
	
	int Capacity() const;  // capacity of array

  unsigned int SizeOfArray() const; // amount of memory in the m_a[] array

  unsigned int SizeOfElement() const; // amount of memory in an m_a[] array element

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  // The operator[] does to not check for valid indices.
  // The caller is responsible for insuring that 0 <= i < Capacity()
  T& operator[]( int );
  T& operator[]( unsigned int );
  T& operator[]( MYON__INT64 );
  T& operator[]( MYON__UINT64 );
#if defined(MYON_RUNTIME_APPLE)
  T& operator[]( size_t );
#endif

  const T& operator[]( int ) const;
  const T& operator[]( unsigned int ) const;  
  const T& operator[]( MYON__INT64 ) const;
  const T& operator[]( MYON__UINT64 ) const;  
#if defined(MYON_RUNTIME_APPLE)
  const T& operator[]( size_t ) const;  
#endif

  operator T*();                     // The cast operators return a pointer
  operator const T*() const;         // to the array.  If Count() is zero,
                                     // this pointer is nullptr.

  T* First();
  const T* First() const;             // returns nullptr if count = 0

  // At(index) returns nullptr if index < 0 or index >= count
  T* At( int );
  T* At( unsigned int );
  T* At( MYON__INT64 );
  T* At( MYON__UINT64 );
  const T* At( int ) const;
  const T* At( unsigned int ) const;
  const T* At( MYON__INT64 ) const;
  const T* At( MYON__UINT64 ) const;

  T* Last();
  const T* Last() const;             // returns nullptr if count = 0

  
  // array operations ////////////////////////////////////////////////////

  T& AppendNew();                    // Most efficient way to add a new element 
                                     // to the array.  Increases count by 1.
                                     // Returned element is memset to zero.

  void Append( const T& );           // Append copy of element.
                                     // Increments count by 1.

  void Append( int, const T* );      // Append copy of an array T[count]

  void Prepend( int, const T* );      // Prepend copy of an array T[count]

  void Insert( int, const T& );      // Insert copy of element. Uses
                                     // memmove() to perform any
                                     // necessary moving.
                                     // Increases count by 1.

  void Remove();                     // Removes last element.  Decrements
                                     // count by 1.  Does not change capacity.

  virtual
  void Remove( int );                // Removes element. Uses memmove() to
                                     // perform any necessary shifting.
                                     // Decrements count by 1.  Does not change
                                     // capacity

  void RemoveValue(const T&);        // Removes elements. Uses memcmp() to compare
                                     // Decrements count by removed items.  Does 
                                     // not change capacity

  void RemoveIf(bool predicate(const T& key));
                                     // Removes elements for which predicate 
                                     // returns true. Decrements count 
                                     // by removed items.
                                     // Does not change capacity

  void Empty();           // Sets count to 0, leaves capacity untouched.

  void Reverse();         // reverse order

  void Swap(int,int);     // swap elements i and j

  //////////
  // Search( e ) does a SLOW search of the array starting at array[0]
  // and returns the index "i" of the first element that satisfies 
  // e == array[i]. (== is really memcmp()).  If the search is not 
  // successful, then Search() returns -1.  For Search(T) to work 
  // correctly, T must be a simple type.  Use Search(p,compare())
  // for Ts that are structs/classes that contain pointers.  Search()
  // is only suitable for performing infrequent searches of small 
  // arrays.  Sort the array and use BinarySearch() for performing
  // efficient searches.
  int Search( const T& ) const;

  //////////
  // Search( p, compare ) does a SLOW search of the array starting
  // at array[0] and returns the index "i" of the first element 
  // that satisfies  compare(p,&array[i])==0.  If the search is not
  // successful, then Search() returns -1.  Search() is only suitable
  // for performing infrequent searches of small arrays.  Sort the
  // array and use BinarySearch() for performing efficient searches.
	// See Also: MYON_CompareIncreasing<T> and MYON_CompareDeccreasing<T>
  int Search( const T*, int (*)(const T*,const T*) ) const;

  //////////
  // BinarySearch( p, compare ) does a fast search of a sorted array
  // and returns the smallest index "i" of the element that satisfies
  // 0==compare(p,&array[i]).  
  //
  // BinarySearch( p, compare, count ) does a fast search of the first
  // count element sorted array and returns the smallest index "i" of 
  // the element that satisfies 0==compare(p,&array[i]).  The version
  // that takes a "count" is useful when elements are being appended
  // during a calculation and the appended elements are not sorted.
  //
  // If the search is successful, 
  // BinarySearch() returns the index of the element (>=0).
  // If the search is not successful, BinarySearch() returns -1.  
  // Use QuickSort( compare ) or, in rare cases and after meaningful
  // performance testing using optimzed release builds, 
  // HeapSort( compare ) to sort the array.
	// See Also: MYON_CompareIncreasing<T> and MYON_CompareDeccreasing<T>
  int BinarySearch( const T*, int (*)(const T*,const T*) ) const;
  int BinarySearch( const T*, int (*)(const T*,const T*), int ) const;

  int InsertInSortedList(const T&, int (*)(const T*, const T*));
  int InsertInSortedList(const T&, int (*)(const T*, const T*), int);

  //////////
  // Sorts the array using the heap sort algorithm.
  // QuickSort() is generally the better choice.
  bool HeapSort( int (*)(const T*,const T*) );

  //////////
  // Sorts the array using the quick sort algorithm.
	// See Also: MYON_CompareIncreasing<T> and MYON_CompareDeccreasing<T>
  bool QuickSort( int (*)(const T*,const T*) );

  //////////
  // Sorts the array using the quick sort algorithma and then removes duplicates.
	// See Also: MYON_CompareIncreasing<T> and MYON_CompareDeccreasing<T>
  bool QuickSortAndRemoveDuplicates( int (*)(const T*,const T*) );

  /*
  Description:
    Sort() fills in the index[] array so that 
    array[index[i]] <= array[index[i+1]].  
    The array is not modified.  

  Parameters:
    sort_algorithm - [in]  
      MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
      Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
      optimized release builds and are confident heap sort is 
      significantly faster.
    index - [out] an array of length Count() that is returned with
        some permutation of (0,1,...,Count()-1). 
    compare - [in] compare function compare(a,b,p) should return
        <0 if a<b, 0, if a==b, and >0 if a>b.
  Returns:
    true if successful
  */
  bool Sort( 
    MYON::sort_algorithm sort_algorithm, 
    int* /* index[] */ ,
    int (*)(const T*,const T*) 
    ) const; 

  /*
  Description:
    Sort() fills in the index[] array so that 
    array[index[i]] <= array[index[i+1]].  
    The array is not modified.  

  Parameters:
    sort_algorithm - [in]  
      MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
      Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
      optimized release builds and are confident heap sort is 
      significantly faster.
    index - [out] an array of length Count() that is returned with
        some permutation of (0,1,...,Count()-1). 
    compare - [in] compare function compare(a,b,p) should return
        <0 if a<b, 0, if a==b, and >0 if a>b.
    p - [in] pointer passed as third argument to compare.

  Returns:
    true if successful
  */
  bool Sort( 
    MYON::sort_algorithm sort_algorithm,
    int*, // index[] 
    int (*)(const T*,const T*,void*), // int compare(const T*,const T*,void* p)
    void* // p
    ) const; 

  //////////
  // Permutes the array so that output[i] = input[index[i]].
  // The index[] array should be a permutation of (0,...,Count()-1).
  bool Permute( const int* /*index[]*/ );

  //////////
  // Zeros all array memory. 
  // Count and capacity are not changed.
  void Zero();

  //////////
  // Sets all bytes in array memory to value. 
  // Count and capacity are not changed.
  void MemSet(unsigned char); 

  //////////
  // Sets all specified items in an array range to a value. 
  // Count and capacity are not changed.
  void SetRange(int from, int count, T);
  
  // memory management ////////////////////////////////////////////////////

  T* Reserve( size_t );    // increase capacity to at least the requested value

  void Shrink();          // remove unused capacity

  void Destroy();         // onfree any memory and set count and capacity to zero
    
  // low level memory management //////////////////////////////////////////

  // By default, MYON_SimpleArray<> uses onrealloc() to manage
  // the dynamic array memory. If you want to use something 
  // besides onrealloc() to manage the array memory, then override 
  // Realloc(). The T* Realloc(ptr, capacity) should do the following:
  //
  // 1) If ptr and capacity are zero, return nullptr.
  // 2) If ptr is nullptr, an capacity > 0, allocate a memory block of 
  //    capacity*sizeof(T) bytes and return a pointer to this block.
  //    If the allocation request fails, return nullptr.
  // 3) If ptr is not nullptr and capacity is 0, free the memory block
  //    pointed to by ptr and return nullptr.
  // 4) If ptr is not nullptr and capacity > 0, then reallocate the memory
  //    block and return a pointer to the reallocated block.  If the 
  //    reallocation request fails, return nullptr.
  //
  // NOTE WELL: 
  //    Microsoft's VC 6.0 realloc() contains a bug that can cause
  //    crashes and should be avoided. See MSDN Knowledge Base article
  //    ID Q225099 for more information.
  virtual
  T* Realloc(T*,int); // (re)allocated capacity*sizeof(T) bytes

  T* Array();                         // The Array() function return the 
  
  const T* Array() const;             // m_a pointer value.

  void SetCount( int );               // If value is <= Capacity(), then
                                      // sets count to specified value.

  T* SetCapacity( size_t );            // Shrink/grows capacity.  If value
                                      // is < current Count(), then count
                                      // is reduced to value.
                                      //

  int NewCapacity() const;            // When the dynamic array needs to grow,
                                      // this calculates the new value for m_capacity.

  /*
  Description:
    Expert user tool to take charge of the memory used by 
    the dynamic array.
  Returns:
     A pointer to the array and zeros out this class.
     The returned pointer is on the heap and must be
     deallocated by calling onfree().
  */
  T* KeepArray();

  /*
  Description:
    Do not use this version of SetArray().  Use the one that takes
    a pointer, count and capacity.
  */
  void SetArray(T*);

  /*
  Description:
    Expert user tool to set the memory used by the dynamic array.
  Parameters:
    T* pointer - [in]
    int count [in]
    int capacity - [in]
       m_a is set to pointer, m_count is set to count, and m_capacity
       is set to capacity.  It is critical that the pointer be one 
       returned by onmalloc(sz), where sz >= capacity*sizeof(T[0]).
  */
  void SetArray(T*, int, int);

protected:
  // implementation //////////////////////////////////////////////////////
  void Move( int /* dest index*/, int /* src index */, int /* element count*/ );
	T*   m_a;        // pointer to array memory
	int  m_count;    // 0 <= m_count <= m_capacity
	int  m_capacity; // actual length of m_a[]
};


////////////////////////////////////////////////////////////////
//

#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<bool>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<char>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__INT8>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__UINT8>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__INT16>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__UINT16>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__INT32>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__UINT32>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<float>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<double>;

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<bool*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<char*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__INT8*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__UINT8*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__INT16*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__UINT16*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__INT32*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON__UINT32*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<float*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<double*>;

#endif



////////////////////////////////////////////////////////////////
//
// The MYON_ClassArray<> template is designed to be used with
// classes that require non-trivial construction or destruction.
// Any class used with the MYON_ClassArray<> template must have a
// robust operator=().
//
// By default, MYON_ClassArray<> uses onrealloc() to manage
// the dynamic array memory. If you want to use something 
// besides onrealloc() to manage the array memory, then override
// MYON_ClassArray::Realloc().  In practice this means that if your
// class has members with back-pointers, then you cannot use
// it in the default MYON_ClassArray.  See MYON_ObjectArray
// for an example.
//
template <class T> class MYON_ClassArray
{
public:
  // construction ////////////////////////////////////////////////////////
  MYON_ClassArray() MYON_NOEXCEPT; 
  MYON_ClassArray( size_t ); // size_t parameter = initial capacity

  // Copy constructor
  MYON_ClassArray( const MYON_ClassArray<T>& );

  virtual
  ~MYON_ClassArray(); // override for struct member deallocation, etc.

  // Assignment operator
  MYON_ClassArray<T>& operator=( const MYON_ClassArray<T>& );

#if defined(MYON_HAS_RVALUEREF)
  // Clone constructor
  MYON_ClassArray( MYON_ClassArray<T>&& ) MYON_NOEXCEPT;

  // Clone Assignment operator
  MYON_ClassArray<T>& operator=( MYON_ClassArray<T>&& ) MYON_NOEXCEPT;
#endif
  
  // emergency bailout ///////////////////////////////////////////////////
  void EmergencyDestroy(void); // call only when memory used by this array
                               // may have become invalid for reasons beyond
                               // your control. EmergencyDestroy() zeros
                               // anything that could possibly cause
                               // ~MYON_ClassArray() to crash.

  // query ///////////////////////////////////////////////////////////////
  
	int Count() const;      // number of elements in array
	unsigned int UnsignedCount() const;

	int Capacity() const;  // capacity of array

  unsigned int SizeOfArray() const; // amount of memory in the m_a[] array

  unsigned int SizeOfElement() const; // amount of memory in an m_a[] array element

  // The operator[] does to not check for valid indices.
  // The caller is responsible for insuring that 0 <= i < Capacity()
  T& operator[]( int );
  T& operator[]( unsigned int );
  T& operator[]( MYON__INT64 );
  T& operator[]( MYON__UINT64 );
#if defined(MYON_RUNTIME_APPLE)
  T& operator[]( size_t );
#endif

  const T& operator[]( int ) const;
  const T& operator[]( unsigned int ) const;  
  const T& operator[]( MYON__INT64 ) const;
  const T& operator[]( MYON__UINT64 ) const;  
#if defined(MYON_RUNTIME_APPLE)
  const T& operator[]( size_t ) const;  
#endif

  operator T*();                     // The cast operators return a pointer
  operator const T*() const;         // to the array.  If Count() is zero,
                                     // this pointer is nullptr.
  T* First();
  const T* First() const;             // returns nullptr if count = 0

  // At(index) returns nullptr if index < 0 or index >= count
  T* At( int );
  T* At( unsigned int );
  T* At( MYON__INT64 );
  T* At( MYON__UINT64 );
  const T* At( int ) const;
  const T* At( unsigned int ) const;
  const T* At( MYON__INT64 ) const;
  const T* At( MYON__UINT64 ) const;

  T* Last();
  const T* Last() const;             // returns nullptr if count = 0

  
  // array operations ////////////////////////////////////////////////////

  T& AppendNew();                    // Most efficient way to add a new class 
                                     // to the array.  Increases count by 1.

  void Append( const T& );           // Append copy of element.
                                     // Increments count by 1.

  void Append( int, const T*);       // Append copy of an array T[count]

  void Insert( int, const T& );      // Insert copy of element. Uses
                                     // memmove() to perform any
                                     // necessary moving.
                                     // Increases count by 1.

  void Remove();                     // Removes last element.  Decrements
                                     // count by 1.  Does not change capacity.

  void Remove( int );                // Removes element. Uses memmove() to
                                     // perform any necessary shifting.
                                     // Decrements count by 1.  Does not change
                                     // capacity

  void Empty();           // Sets count to 0, leaves capacity untouched.

  void Reverse();         // reverse order

  void Swap(int,int);     // swap elements i and j

  //////////
  // Search( p, compare ) does a SLOW search of the array starting
  // at array[0] and returns the index "i" of the first element 
  // that satisfies  compare(p,&array[i])==0.  If the search is not
  // successful, then Search() returns -1.  Search() is only suitable
  // for performing infrequent searches of small arrays.  Sort the
  // array and use BinarySearch() for performing efficient searches.
  int Search( const T*, int (*)(const T*,const T*) ) const;

  //////////
  // BinarySearch( p, compare ) does a fast search of a sorted array
  // and returns the smallest index "i" of the element that satisfies
  // 0==compare(p,&array[i]).
  //
  // BinarySearch( p, compare, count ) does a fast search of the first
  // count element sorted array and returns the smallest index "i" of 
  // the element that satisfies 0==compare(p,&array[i]).  The version
  // that takes a "count" is useful when elements are being appended
  // during a calculation and the appended elements are not sorted.
  //
  // If the search is successful, 
  // BinarySearch() returns the index of the element (>=0).
  // If the search is not successful, BinarySearch() returns -1.
  // Use QuickSort( compare ) or, in rare cases and after meaningful
  // performance testing using optimzed release builds, 
  // HeapSort( compare ) to sort the array.
	// See Also: MYON_CompareIncreasing<T> and MYON_CompareDeccreasing<T>
  int BinarySearch( const T*, int (*)(const T*,const T*) ) const;
  int BinarySearch( const T*, int (*)(const T*,const T*), int ) const;

  int InsertInSortedList(const T&, int (*)(const T*, const T*));
  int InsertInSortedList(const T&, int (*)(const T*, const T*), int);

  //////////
  // Sorts the array using the heap sort algorithm.
	// See Also: MYON_CompareIncreasing<T> and MYON_CompareDeccreasing<T>
  // QuickSort() is generally the better choice.
  virtual
  bool HeapSort( int (*)(const T*,const T*) );

  //////////
  // Sorts the array using the heap sort algorithm.
  virtual
  bool QuickSort( int (*)(const T*,const T*) );

  /*
  Description:
    Sort() fills in the index[] array so that 
    array[index[i]] <= array[index[i+1]].  
    The array is not modified.  

  Parameters:
    sort_algorithm - [in]  
      MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
      Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
      optimized release builds and are confident heap sort is 
      significantly faster.
    index - [out] an array of length Count() that is returned with
        some permutation of (0,1,...,Count()-1). 
    compare - [in] compare function compare(a,b) should return
        <0 if a<b, 0, if a==b, and >0 if a>b.

  Returns:
    true if successful
  */
  bool Sort( 
    MYON::sort_algorithm sort_algorithm, 
    int* /* index[] */ ,
    int (*)(const T*,const T*)
    ) const; 

  /*
  Description:
    Sort() fills in the index[] array so that 
    array[index[i]] <= array[index[i+1]].  
    The array is not modified.  

  Parameters:
    sort_algorithm - [in]  
      MYON::sort_algorithm::quick_sort (best in general) or MYON::sort_algorithm::heap_sort
      Use MYON::sort_algorithm::heap_sort only if you have done extensive testing with
      optimized release builds and are confident heap sort is 
      significantly faster.
    index - [out] an array of length Count() that is returned with
        some permutation of (0,1,...,Count()-1). 
    compare - [in] compare function compare(a,b,p) should return
        <0 if a<b, 0, if a==b, and >0 if a>b.
    p - [in] pointer passed as third argument to compare.

  Returns:
    true if successful
  */
  bool Sort( 
    MYON::sort_algorithm sort_algorithm,
    int*, // index[] 
    int (*)(const T*,const T*,void*), // int compare(const T*,const T*,void* p)
    void* // p
    ) const; 

  //////////
  // Permutes the array so that output[i] = input[index[i]].
  // The index[] array should be a permutation of (0,...,Count()-1).
  bool Permute( const int* /*index[]*/ );

  //////////
  // Destroys all elements and fills them with values
  // set by the default constructor.
  // Count and capacity are not changed.
  void Zero();

  // memory management /////////////////////////////////////////////////

  T* Reserve( size_t ); // increase capacity to at least the requested value

  void Shrink();       // remove unused capacity

  void Destroy();      // onfree any memory and set count and capacity to zero
    
  // low level memory management ///////////////////////////////////////

  // By default, MYON_ClassArray<> uses onrealloc() to manage
  // the dynamic array memory. If you want to use something 
  // besides onrealloc() to manage the array memory, then override 
  // Realloc(). The T* Realloc(ptr, capacity) should do the following:
  //
  // 1) If ptr and capacity are zero, return nullptr.
  // 2) If ptr is nullptr, an capacity > 0, allocate a memory block of 
  //    capacity*sizeof(T) bytes and return a pointer to this block.
  //    If the allocation request fails, return nullptr.
  // 3) If ptr is not nullptr and capacity is 0, free the memory block
  //    pointed to by ptr and return nullptr.
  // 4) If ptr is not nullptr and capacity > 0, then reallocate the memory
  //    block and return a pointer to the reallocated block.  If the 
  //    reallocation request fails, return nullptr.
  //
  // NOTE WELL: 
  //    Microsoft's VC 6.0 realloc() contains a bug that can cause
  //    crashes and should be avoided. See MSDN Knowledge Base article
  //    ID Q225099 for more information.
  virtual
  T* Realloc(T*,int); // (re)allocated capacity*sizeof(T) bytes

  T* Array();                         // The Array() function return the 
  
  const T* Array() const;             // m_a pointer value.

  void SetCount( int );               // If value is <= Capacity(), then
                                      // sets count to specified value.

  T* SetCapacity( size_t );            // Shrink/grows capacity.  If value
                                      // is < current Count(), then count
                                      // is reduced to value.

  int NewCapacity() const;            // When the dynamic array needs to grow,
                                      // this calculates the new value for m_capacity.

  T* KeepArray();                     // returns pointer to array and zeros
                                      // out this class.  Caller is responsible
                                      // for calling destructor on each element
                                      // and then using onfree() to release array
                                      // memory.  E.g.,
                                      //
                                      //   for (int i=capacity;i>=0;i--) {
                                      //     array[i].~T();
                                      //   }
                                      //   onfree(array);

  /*
  Description:
    Do not use this version of SetArray().  Use the one that takes
    a pointer, count and capacity: SetArray(pointer,count,capacity)
  */
  void SetArray(T*);

  /*
  Description:
    Expert user tool to set the memory used by the dynamic array.
  Parameters:
    T* pointer - [in]
    int count - [in]  0 <= count <= capacity
    int capacity - [in]
       m_a is set to pointer, m_count is set to count, and m_capacity
       is set to capacity.  It is critical that the pointer be one 
       returned by onmalloc(sz), where sz >= capacity*sizeof(T[0]),
       and that the in-place operator new has been used to initialize
       each element of the array.  
  */
  void SetArray(T*, int, int);

protected:
  // implementation //////////////////////////////////////////////////////
  void Move( int /* dest index*/, int /* src index */, int /* element count*/ );
  void ConstructDefaultElement(T*);
  void DestroyElement(T&);
	T*   m_a;        // pointer to array memory
	int  m_count;    // 0 <= m_count <= m_capacity
	int  m_capacity; // actual length of m_a[]
};

#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_String>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_wString>;

#endif

/*
Description:
  MYON_Object array is used to store lists of classes that are
  derived from MYON_Object.  It differs from MYON_ClassArray in
  that the virtual MYON_Object::MemoryRelocate function is called
  when growing the dynamic array requires changing the location
  of the memory buffer used to store the elements in the array.
*/
template <class T> class MYON_ObjectArray : public MYON_ClassArray<T>
{
public:
  MYON_ObjectArray(); 
  ~MYON_ObjectArray(); // override for struct member deallocation, etc.
  MYON_ObjectArray( size_t ); // size_t parameter = initial capacity
  MYON_ObjectArray( const MYON_ObjectArray<T>& );
  MYON_ObjectArray<T>& operator=( const MYON_ObjectArray<T>& );

#if defined(MYON_HAS_RVALUEREF)
  // Clone constructor
  MYON_ObjectArray( MYON_ObjectArray<T>&& );

  // Clone Assignment operator
  MYON_ObjectArray<T>& operator=( MYON_ObjectArray<T>&& );
#endif

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  // virtual MYON_ClassArray<T> override that 
  // calls MemoryRelocate on each element after
  // the reallocation.
  T* Realloc(T*,int);

  // virtual MYON_ClassArray<T> override that 
  // calls MemoryRelocate on each element after
  // the heap sort.
  // QuickSort() is generally the better choice.
  bool HeapSort( int (*)(const T*,const T*) );

  // virtual MYON_ClassArray<T> override that 
  // calls MemoryRelocate on each element after
  // the quick sort.
  bool QuickSort( int (*)(const T*,const T*) );
};

class MYON_CLASS MYON_UuidPair
{
public:
  /*
  Description:
    Compares m_uuid[0] and ignores m_uuid[1]
  */
  static 
  int CompareFirstUuid(const class MYON_UuidPair*,const class MYON_UuidPair*);

  /*
  Description:
    Compares m_uuid[1] and ignores m_uuid[0]
  */
  static 
  int CompareSecondUuid(const class MYON_UuidPair*,const class MYON_UuidPair*);

  /*
  Description:
    Compares m_uuid[0] then m_uuid[1].
  */
  static 
  int Compare(const class MYON_UuidPair*,const class MYON_UuidPair*);

  MYON_UuidPair();
  MYON_UUID m_uuid[2];
};

#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_UUID>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_UuidIndex>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_UuidPtr>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_UuidPair>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_SimpleArray<int> >;

#endif


/*
Description:
  The MYON_UuidList class provides a tool to efficiently 
  maintain a list of uuids and determine if a uuid is 
  in the list. This class is based on the premise that 
  there are no duplicate uuids in the list.
*/
class MYON_CLASS MYON_UuidList : private MYON_SimpleArray<MYON_UUID>
{
public:
  MYON_UuidList();
  MYON_UuidList(int capacity);
  ~MYON_UuidList();
  MYON_UuidList(const MYON_UuidList& src);
  MYON_UuidList& operator=(const MYON_UuidList& src);

  bool operator==(const MYON_UuidList& other) const;
  bool operator!=(const MYON_UuidList& other) const;

  /*
  Description:
    Fast uuid compare.  Not necessarily the same
    as MYON_UuidCompare().
  */
  static
  int CompareUuid( const MYON_UUID* a, const MYON_UUID* b );

  /*
  Returns:
    Number of active uuids in the list.
  */
  int Count() const;

  /*
  Returns:
    Array of uuids in the list.  Sorted with
    respect to MYON_UuidList::CompareUuid().
  Remarks:
    Calling AddUuid() may grow the dynamic array
    and make the pointer invalid.
  */
  const MYON_UUID* Array() const;

  /*
  Description:
    Provides an efficient way to empty a list so that it
    can be used again.
  */
  void Empty();

  /*
  Description:
    Destroy list.  If list will be reused, Empty() is more
    efficient.
  */
  void Destroy();

  void Reserve(size_t capacity);

  /*
  Description:
    Makes the uuid list as efficient as possible in both search
    speed and memory usage.  Use Compact() when a uuid list
    will be in use but is not likely to be modified.  A list 
    that has been compacted can still be modified.
  */
  void Compact();

  /*
  Description:
    Adds a uuid to the list.
  Parameters:
    uuid - [in] id to add.
    bCheckForDupicates - [in] if true, then the uuid
       is not added if it is already in the list.
       If you are certain that the uuid is not in the
       list and you are going to have a large list of uuids,
       then setting bCheckForDupicates=false will
       speed up the addition of uuids.
  Returns:
    True if uuid was added.  False if uuid was not added
    because it is already in the collection.
  */
  bool AddUuid(MYON_UUID uuid, bool bCheckForDupicates=true);

  /*
  Description:
    Removes a uuid from the list.
  Parameters:
    uuid - [in] id to remove
  Returns:
    True if uuid was in the list and was removed.
    False if uuid was not in the list.
  */
  bool RemoveUuid(MYON_UUID uuid);

  /*
  Description:
    Determine if a uuid is in the list.
  Returns:
    True if uuid is in the list.
  */
  bool FindUuid(MYON_UUID uuid) const;

  /*
  Description:
    Saves the uuid list in an archive.
  Parameters:
    archive - [in] archive to write to.
  Returns:
    true if write was successful.
  */
  bool Write( 
    class MYON_BinaryArchive& archive 
    ) const;

  /*
  Description:
    Saves the uuid list in an archive.
  Parameters:
    archive - [in] archive to write to.
    bSortBeforeWrite - [in]
      True if ids should be sorted before the write
      so future lookups will be fast.  False if
      the current state of the sorted/unsorted bits
      should be preserved.
  Returns:
    true if write was successful.
  */
  bool Write( 
    class MYON_BinaryArchive& archive,
      bool bSortBeforeWrite
    ) const;

  /*
  Description:
    Read the uuid list from an archive.
  Parameters:
    archive - [in] archive to read from.
  Returns:
    true if the read was successful.
  */
  bool Read( 
    class MYON_BinaryArchive& archive 
    );

  /*
  Description:
    Read the uuid list from an archive.
  Parameters:
    archive - [in] 
      archive to read from.
    bool bSortAfterRead - [in]
      True if ids should be sorted after the read
      so future lookups will be fast.  False if
      the state of the sorted/unsorted bits that
      existed at write time should be preserved.
  Returns:
    true if the read was successful.
  */
  bool Read( 
    class MYON_BinaryArchive& archive,
    bool bSortAferRead
    );

  /*
  Description:
    Append the uuids in this class to uuid_list.
  Parameters:
    uuid_list - [in/out]
  Returns:
    Number of uuids added to uuid_list.
  */
  int GetUuids(
     MYON_SimpleArray<MYON_UUID>& uuid_list
     ) const;

  /*
  Description:
    This tool is used in rare situations when the object ids 
    stored in the uuid list need to be remapped.
  Parameters:
    uuid_remap - [in]
      Is it critical that uuid_remap[] be sorted with respect
      to MYON_UuidPair::CompareFirstUuid.
  */
  void RemapUuids( 
    const MYON_SimpleArray<MYON_UuidPair>& uuid_remap 
    );

private:
  void PurgeHelper();
  void SortHelper();
  MYON_UUID* SearchHelper(const MYON_UUID*) const;
  int m_sorted_count;
  int m_removed_count;
};

/*
Description:
  The MYON_UuidList class provides a tool
  to efficiently maintain a list of uuid-index
  pairs and determine if a uuid is in the list.
  This class is based on the premise that there are
  no duplicate uuids in the list.
*/
class MYON_CLASS MYON_UuidIndexList : private MYON_SimpleArray<MYON_UuidIndex>
{
public:
  MYON_UuidIndexList() = default;
  MYON_UuidIndexList(size_t capacity);
  ~MYON_UuidIndexList() = default;
  MYON_UuidIndexList(const MYON_UuidIndexList& src);
  MYON_UuidIndexList& operator=(const MYON_UuidIndexList& src);

  /*
  Returns:
    Number of active uuids in the list.
  */
  unsigned int Count() const;

  /*
  Description:
    Provides an efficient way to empty a list so that it
    can be used again.
  */
  void RemoveAll();

  void Reserve( size_t capacity );

  /*
  Description:
    Adds a uuid-index pair to the list.
  Parameters:
    uuid - [in] id to add.  
      This uuid cannot be MYON_max_uuid because MYON_max_uuid
      is 
    bCheckForDupicates - [in] if true, then the uuid
       is not added if it is already in the list.
       If you are certain that the uuid is not in the list
       and you have a have a large collection of uuids,
       then setting bCheckForDupicates=false will
       speed up the addition of uuids.
  Returns:
    True if uuid was added.  False if uuid was not added
    because it is already in the collection.
  */
  bool AddUuidIndex(
    MYON_UUID uuid, 
    int index, 
    bool bCheckForDupicates=true);

  /*
  Description:
    Removes an element with a matching uuid from the list.
  Parameters:
    uuid - [in] id to remove
  Returns:
    True if an element was removed.  False if the uuid
    was not in the list.
  */
  bool RemoveUuid(
    MYON_UUID uuid
    );

  /*
  Description:
    Determine if an element with a uuid is in the list.
  Parameters:
    index - [out] if not nullptr and a matching uuid is found,
       then *index is set to the value of the index.
  Returns:
    True if an element was found.  Returns false if
    the uuid is not in the list.
  */
  bool FindUuid(MYON_UUID uuid) const;
  bool FindUuid(MYON_UUID uuid, int* index) const;

  /*
  Description:
    Determine if a uuid-index pair is in the list.
  Returns:
    True if the uuid-index pair is in the list.
    Returns false if the uuid-index pair is not
    in the list.
  */
  bool FindUuidIndex(MYON_UUID uuid, int index) const;

  /*
  Description:
    Append the uuids in this class to uuid_list.
  Parameters:
    uuid_list - [in/out]
  Returns:
    Number of uuids added to uuid_list.
  */
  unsigned int GetUuids(
     MYON_SimpleArray<MYON_UUID>& uuid_list
     ) const;

  /*
  Description:
    If you will perform lots of searches before the next
    change to the list, then calling ImproveSearchSpeed()
    will speed up the searches by culling removed objects
    and completely sorting the list so only a binary search
    is required. You may edit the list at any time after 
    calling ImproveSearchSpeed().  If you are performing 
    a few searches between edits, then excessive calling
    of ImproveSearchSpeed() may actually decrease overall
    program performance.
  */
  void ImproveSearchSpeed();

private:
  MYON_UuidIndex* SearchHelper(const MYON_UUID*) const;
  unsigned int m_sorted_count = 0;
  unsigned int m_removed_count = 0;
};


/*
Description:
  The MYON_UuidList class provides a tool
  to efficiently maintain a list of uuid-pointer
  pairs and determine if a uuid is in the list.
  This class is based on the premise that there are
  no duplicate uuids in the list.
*/
class MYON_CLASS MYON_UuidPtrList : private MYON_SimpleArray<MYON_UuidPtr>
{
public:
  MYON_UuidPtrList() = default;
  MYON_UuidPtrList(size_t capacity);
  ~MYON_UuidPtrList() = default;
  MYON_UuidPtrList(const MYON_UuidPtrList& src);
  MYON_UuidPtrList& operator=(const MYON_UuidPtrList& src);

  /*
  Returns:
    Number of active uuids in the list.
  */
  unsigned int Count() const;

  /*
  Description:
    Provides an efficient way to empty a list so that it
    can be used again.
  */
  void RemoveAll();

  void Reserve( size_t capacity );

  /*
  Description:
    Adds a uuid-index pair to the list.
  Parameters:
    uuid - [in] id to add.  
      This uuid cannot be MYON_max_uuid because MYON_max_uuid
      is 
    bCheckForDupicates - [in] if true, then the uuid
       is not added if it is already in the list.
       If you are certain that the uuid is not in the list
       and you have a have a large collection of uuids,
       then setting bCheckForDupicates=false will
       speed up the addition of uuids.
  Returns:
    True if uuid was added.  False if uuid was not added
    because it is already in the collection.
  */
  bool AddUuidPtr(
    MYON_UUID uuid, 
    MYON__UINT_PTR ptr, 
    bool bCheckForDupicates=true);

  /*
  Description:
    Removes an element with a matching uuid from the list.
  Parameters:
    uuid - [in] id to remove
  Returns:
    True if an element was removed.  False if the uuid
    was not in the list.
  */
  bool RemoveUuid(
    MYON_UUID uuid
    );

  /*
  Description:
    Determine if an element with a uuid is in the list.
  Parameters:
    ptr - [out] if not nullptr and a matching uuid is found,
       then *ptr is set to the value of the m_ptr.
  Returns:
    True if an element was found.  Returns false if
    the uuid is not in the list.
  */
  bool FindUuid(MYON_UUID uuid) const;
  bool FindUuid(MYON_UUID uuid, MYON__UINT_PTR* ptr) const;

  /*
  Description:
    Determine if a uuid-index pair is in the list.
  Returns:
    True if the uuid-index pair is in the list.
    Returns false if the uuid-index pair is not
    in the list.
  */
  bool FindUuidPtr(MYON_UUID uuid, MYON__UINT_PTR index) const;

  /*
  Description:
    Append the uuids in this class to uuid_list.
  Parameters:
    uuid_list - [in/out]
  Returns:
    Number of uuids added to uuid_list.
  */
  unsigned int GetUuids(
     MYON_SimpleArray<MYON_UUID>& uuid_list
     ) const;

  /*
  Description:
    If you will perform lots of searches before the next
    change to the list, then calling ImproveSearchSpeed()
    will speed up the searches by culling removed objects
    and completely sorting the list so only a binary search
    is required. You may edit the list at any time after 
    calling ImproveSearchSpeed().  If you are performing 
    a few searches between edits, then excessive calling
    of ImproveSearchSpeed() may actually decrease overall
    program performance.
  */
  void ImproveSearchSpeed();

private:
  MYON_UuidPtr* SearchHelper(const MYON_UUID*) const;
  unsigned int m_sorted_count = 0;
  unsigned int m_removed_count = 0;
};


/*
Description:
  The MYON_UuidPairList class provides a tool
  to efficiently maintain a list of uuid pairs 
  and determine if a uuid is in the list.
  This class is based on the premise that there are
  no duplicate uuids in the list.
*/
class MYON_CLASS MYON_UuidPairList : private MYON_SimpleArray<MYON_UuidPair>
{
public:
  MYON_UuidPairList();
  MYON_UuidPairList(int capacity);
  ~MYON_UuidPairList();
  MYON_UuidPairList(const MYON_UuidPairList& src);
  MYON_UuidPairList& operator=(const MYON_UuidPairList& src);

  static const MYON_UuidPairList EmptyList;

  /*
  Returns:
    Number of active uuids in the list.
  */
  int Count() const;

  /*
  Description:
    Provides an efficient way to empty a list so that it
    can be used again.
  */
  void Empty();

  void Reserve( size_t capacity );

  /*
  Description:
    Adds a uuid-index pair to the list.
  Parameters:
    id1 - [in] id to add.
    id2 - [in] id to add.
    bCheckForDupicates - [in] if true, then the pair
       is not added if id1 is already in the list.
       If you are certain that the id1 is not in the list
       and you have a have a large collection of uuids,
       then setting bCheckForDupicates=false will
       speed up the addition of uuids.
  Returns:
    True if the pair was added.  False if the pair was not added
    because it is already in the collection.
  Remarks:
    You cannot add the pair value ( MYON_max_uuid, MYON_max_uuid ). This
    pair value is used to mark removed elements in the MYON_UuidPairList[].
  */
  bool AddPair(
    MYON_UUID id1, 
    MYON_UUID id2, 
    bool bCheckForDupicates=true
    );

  /*
  Description:
    Removes an element with a matching id1 from the list.
  Parameters:
    id1 - [in] id to remove
  Returns:
    True if an element was removed.  False if the id1
    was not in the list.
  */
  bool RemovePair(
    MYON_UUID id1
    );

  /*
  Description:
    Removes an element with a matching id pair from the list.
  Parameters:
    id1 - [in]
    id2 - [in]
  Returns:
    True if an element was removed.  False if the id pair
    does not appear in the list.
  */
  bool RemovePair(
    MYON_UUID id1,
    MYON_UUID id2
    );

  /*
  Description:
    Determine if an element with a uuid is in the list.
  Parameters:
    id1 - [in]
    id2 - [out] if not nullptr and a matching id1 is found,
       then *id2 is set to the value of the second uuid.
  Returns:
    True if an element was found.  Returns false if
    the id1 is not in the list.
  */
  bool FindId1(MYON_UUID id1, MYON_UUID* id2=0) const;

  /*
  Description:
    Determine if an id pair is in the list.
  Returns:
    True if the id pair is in the list.
    False if the id pair is not in the list.
  */
  bool FindPair(MYON_UUID id1, MYON_UUID id2) const;

  /*
  Description:
    Append the value of the first id in each pair to uuid_list[].
  Parameters:
    uuid_list - [in/out]
  Returns:
    Number of ids appended to uuid_list[].
  */
  int GetId1s(
     MYON_SimpleArray<MYON_UUID>& uuid_list
     ) const;

  /*
  Description:
    If you will perform lots of searches before the next
    change to the list, then calling ImproveSearchSpeed()
    will speed up the searches by culling removed objects
    and completely sorting the list so only a binary search
    is required. You may edit the list at any time after 
    calling ImproveSearchSpeed().  If you are performing 
    a few searches between edits, then excessive calling
    of ImproveSearchSpeed() may actually decrease overall
    program performance.
  */
  void ImproveSearchSpeed();

  bool Write(
    class MYON_BinaryArchive& archive
    ) const;

  bool Read(
    class MYON_BinaryArchive& archive
    );

private:
  MYON_UuidPair* SearchHelper(const MYON_UUID*) const;
  unsigned int m_sorted_count;
  unsigned int m_removed_count;
};

class MYON_CLASS MYON_2dexMap : private MYON_SimpleArray<MYON_2dex>
{
public:
  MYON_2dexMap();
  MYON_2dexMap(int capacity);
  ~MYON_2dexMap();

  int Count() const;

  void Reserve(size_t capacity);

  const MYON_2dex* Array() const;

  MYON_2dex operator[](int i) const;

  /*
  Description:
    Creates an index map with the values
    (i0,j),...,(i0+count-1,j)
  Parameters:
    count - [in]
       number of elements
    i0 - [in]
       i value of first element
    j - [in]
       j value for all elements
  */
  void Create(int count, int i0, int j);

  /*
  Description:
    Searches for an element with a matching i
    and returns its j value.  If no matching
    element is found, then not_found_rc is returned. 
  Parameters:
    i - [in]
       value of i to search for
    not_found_rc - [in]
       value to return if there is not a match.
  Returns:
    j value
  */
  int FindIndex( 
          int i, 
          int not_found_rc
          ) const;

  /*
  Description:
    Adds and element (i,j).  If there is already an entry with
    value (i,*), then no element is added.
  Parameters:
    i - [in]
    i - [in]
  Returns:
    True if and element it added.
  */
  bool AddIndex( 
          int i, 
          int j
          );

  /*
  Description:
    Searches for an element (i,*) and sets its j value to j.
    If there is no element with a matching i, then false
    is returned.
  Parameters:
    i - [in]
    j - [in]
  Returns:
    True if and element exists and was set.
  */
  bool SetIndex( 
          int i, 
          int j
          );

  /*
  Description:
    If an element (i,*) exists, its j value is set.  Otherwise
    a new element with value (i,j) is added.
  Parameters:
    i - [in]
    j - [in]
  */
  void SetOrAddIndex( 
          int i, 
          int j
          );

  /*
  Description:
    If an element (i,*) exists, it is removed.  If there is
    not an element with a matching i value, then false
    is returned.
  Parameters:
    i - [in]
  Returns:
    True if the element was removed
  */
  bool RemoveIndex( 
          int i
          );

  const MYON_2dex* Find2dex(int i) const;

private:
  bool m_bSorted;
};

/* 
Description:
	Compare function for Sort and Search methods.
Returns:
   -1 if *a < *b is true
    1 if *b < *a is true
    0 if neither *a <*b nor *b<*a is true 
Details:
	Use this template functions to sort MYON_SimpleArray and
  MYON_ClassArray objects into increasing order.  The elements
  of the arrays must be a type with an operator < defined.
	In particular it works with built in types like double, 
  int and pointers.
Example:

          MYON_SimpleArray<int> A;
          A = ...;
          // Sort A in increasing order
          A.QuickSort( MYON_CompareIncreasing<double> );		

See Also:
  MYON_CompareDecreasing
*/
template< class T>
static
int MYON_CompareIncreasing( const T* a, const T* b);

/* 
Description:
	Compare function for Sort and Search methods.
Returns:
   -1 if *b < *a is true
    1 if *a < *b is true
    0 if neither *a < *b nor *b < *a is true 
Details:
	Use this template functions to sort MYON_SimpleArray and
  MYON_ClassArray objects into decreasing order.  The elements
  of the arrays must be a type with an operator < defined.
	In particular it works with built in types like double, 
  int and pointers.
Example:

          class C
          {
          public:
            ...
            bool operator<(const C&) const;
          };
          ...
          MYON_ClassArray<C> A;
          A = ...;
          // Sort A in descrasing order
          A.QuickSort( MYON_CompareDecreasing<C> );		

See Also:
  MYON_CompareIncreasing
*/
template< class T>
static
int MYON_CompareDecreasing( const T* a, const T* b);

void MYON_SHA1_Accumulate2fPointArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_2fPoint>& a
);

void MYON_SHA1_Accumulate3fPointArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_3fPoint>& a
);

void MYON_SHA1_Accumulate4fPointArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_4fPoint>& a
);

void MYON_SHA1_Accumulate2fVectorArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_2fVector>& a
);

void MYON_SHA1_Accumulate3fVectorArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_3fVector>& a
);

void MYON_SHA1_Accumulate2dPointArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_2dPoint>& a
);

void MYON_SHA1_Accumulate3dPointArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_3dPoint>& a
);

void MYON_SHA1_Accumulate4dPointArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_4dPoint>& a
);

void MYON_SHA1_Accumulate2dVectorArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_2dVector>& a
);

void MYON_SHA1_Accumulate3dVectorArray(
  class MYON_SHA1& sha1,
  const class MYON_SimpleArray<MYON_3dVector>& a
);

// definitions of the template functions are in a different file
// so that Microsoft's developer studio's autocomplete utility
// will work on the template functions.
#include "opennurbs_array_defs.h"

class MYON_CLASS MYON_Big5UnicodePair
{
public:
  MYON_Big5UnicodePair() = default;
  ~MYON_Big5UnicodePair() = default;
  MYON_Big5UnicodePair(const MYON_Big5UnicodePair&) = default;
  MYON_Big5UnicodePair& operator=(const MYON_Big5UnicodePair&) = default;

public:
  /// <summary>
  /// An array sorted by BIG5 code points and useful for converting BIG5 code points to Unicode code points.
  /// </summary>
  /// <returns>Returns an array sorted by MYON_Big5UnicodePair::CompareBig5AndUnicodeCodePoints()</returns>
  static const MYON_SimpleArray< MYON_Big5UnicodePair >& Big5ToUnicode();

  /// <summary>
  /// An array sorted by Unicode code points and useful for converting Unicode code points to BIG5 code points.
  /// </summary>
  /// <returns>Returns an array sorted by MYON_Big5UnicodePair::CompareUnicodeAndBig5CodePoints()</returns>
  static const MYON_SimpleArray< MYON_Big5UnicodePair >& UnicodeToBig5();

public:
  static const MYON_Big5UnicodePair Null;

  /// <summary>
  /// MYON_Big5UnicodePair::Error.Big5() = MYON_Big5CodePoint::Error and MYON_Big5UnicodePair::Error.Unicode() = MYON_UnicodeShortCodePoint::Error.
  /// </summary>
  static const MYON_Big5UnicodePair Error;

  /// <summary>
  /// Create a BIG5 - Unicode code point pair. 
  /// </summary>
  /// <param name="big5_code_point">
  /// BIG5 code point.
  /// </param>
  /// <param name="unicode_code_point">
  /// Unicode code point.
  /// </param>
  /// <returns></returns>
  static const MYON_Big5UnicodePair Create(
    MYON_Big5CodePoint big5_code_point,
    MYON_UnicodeShortCodePoint unicode_code_point
  );

  static const MYON_Big5UnicodePair Create(
    unsigned int big5_code_point,
    unsigned int unicode_code_point
  );

  /// <summary>
  /// Determine if both code points in this pair are 0.
  /// </summary>
  /// <returns>True if both code points are 0.</returns>
  bool IsNull() const;

  /// <summary>
  /// Determing if the values stored as the BIG5 and Unicode code points are equal nonzero ASCII code points.
  /// ASCII code point are in the range 0-0x7F (127 decimal).
  /// Unicode extends ASCII. Strictly speaking, BIG5 does not extend ASCII, but it is common to mix 
  /// single bytes ASCII and double byte BIG5 encodings in the same char string.
  /// BIG5 is a double byte string encoding with the first byte in the range 0x81 to 0xFE, the
  /// minimum BIG5 code point is 0x8140 and the maximum BIG5 code point is 0xFEFE.
  /// Thus it is possible to mix ASCII and BIG5 encodings in the same char string.
  /// </summary>
  /// <param name="bNullIsASCII">
  /// Value to return if both code points are 0.
  /// </param>
  /// <returns>True if both code points are equal and ASCII code points (0 to 0x7F).</returns>
  bool IsASCII(bool bNullIsASCII) const;

  /// <summary>
  /// Determine if the pair of code points is valid. 
  /// If the values for BIG5 and Unicode code point values are &lt 0xFF and equal, the pair is considered valid. 
  /// Use IsASCII() if you need to treat nonzero ASCII code points differently.
  /// </summary>
  /// <param name="bNullIsValid">
  /// Value to return if this pair is null.
  /// </param>
  /// <param name="bASCIICodePointIsValid">
  /// Value to return if this pair is an ASCII code point.
  /// </param>
  /// <returns>True if the BIG5 and Unicode code points are both valid or IsASCII() is true.</returns>
  bool IsValid(bool bNullIsValid, bool bASCIICodePointIsValid) const;

  const MYON_Big5CodePoint Big5() const;
  const MYON_UnicodeShortCodePoint Unicode() const;

  unsigned int Big5CodePoint() const;
  unsigned int UnicodeCodePoint() const;

  /// <summary>
  /// 
  /// </summary>
  /// </summary>
  /// <param name="bNullIsValid">
  /// Value to return if this pair is null.
  /// </param>
  /// <param name="bASCIICodePointIsValid">
  /// Value to return if this pair is an ASCII code point.
  /// </param>
  /// <returns></returns>
  bool IsStandard(bool bNullIsValid, bool bASCIICodePointIsStandard) const;

  /// <summary>
  /// 
  /// </summary>
  /// <returns>Returns true if this pair is valid and at least one of the code points is a private use code point.
  /// </returns>
  bool IsPrivateUse() const;

  /// <summary>
  /// Compares the BIG5 code point.
  /// </summary>
  /// <param name="lhs"></param>
  /// <param name="rhs"></param>
  /// <returns></returns>
  static int CompareBig5CodePoint(const MYON_Big5UnicodePair* lhs, const MYON_Big5UnicodePair* rhs);

  /// <summary>
  /// Compares the Unicode code point.
  /// </summary>
  /// <param name="lhs"></param>
  /// <param name="rhs"></param>
  /// <returns></returns>
  static int CompareUnicodeCodePoint(const MYON_Big5UnicodePair* lhs, const MYON_Big5UnicodePair* rhs);

  /// <summary>
  /// Dictionary compare (BIG5 code point first, Unicode code point second).
  /// </summary>
  /// <param name="lhs"></param>
  /// <param name="rhs"></param>
  /// <returns></returns>
  static int CompareBig5AndUnicodeCodePoints(const MYON_Big5UnicodePair* lhs, const MYON_Big5UnicodePair* rhs);

  /// <summary>
  /// Dictionary compare (Unicode code point first, BIG5 code point second).
  /// </summary>
  /// <param name="lhs"></param>
  /// <param name="rhs"></param>
  /// <returns></returns>
  static int CompareUnicodeAndBig5CodePoints(const MYON_Big5UnicodePair* lhs, const MYON_Big5UnicodePair* rhs);

private:
  MYON_Big5CodePoint m_big5;
  MYON_UnicodeShortCodePoint m_unicode;
};

MYON_DECL bool operator==(MYON_Big5UnicodePair lhs, MYON_Big5UnicodePair rhs);
MYON_DECL bool operator!=(MYON_Big5UnicodePair lhs, MYON_Big5UnicodePair rhs);

#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Big5UnicodePair>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Big5CodePoint>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_UnicodeShortCodePoint>;

#endif

#endif

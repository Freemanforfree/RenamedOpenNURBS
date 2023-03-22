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

#if !defined(OPENNURBS_MEMORY_INC_)
#define OPENNURBS_MEMORY_INC_

#if defined (cplusplus) || defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

MYON_DECL
size_t MYON_MemoryPageSize();


/*
Allocate memory that is intentionally never returned
should not be considered a memory leak. Typically this is
for an application workspace.
*/
MYON_DECL
void* onmalloc_forever( size_t );

MYON_DECL
void* onmalloc( size_t );

MYON_DECL
void* oncalloc( size_t, size_t );

MYON_DECL
void   onfree( void* );

MYON_DECL
void* onrealloc( void*, size_t );

MYON_DECL
void* onmemdup( const void*, size_t );

MYON_DECL
char* onstrdup( const char* );

MYON_DECL
wchar_t* onwcsdup( const wchar_t* );

MYON_DECL
unsigned char* onmbsdup( const unsigned char* );

#if defined (cplusplus) || defined(_cplusplus) || defined(__cplusplus)
}

class MYON_CLASS MYON_MemoryAllocationTracking
{
public:
  /*
  Description:
    Windows Debug Builds:
      The constructor saves the current state of memory allocation tracking
      and then enables/disables memory allocation tracking.
    Otherwise:
      Does nothting.
  */
  MYON_MemoryAllocationTracking(
    bool bEnableAllocationTracking
  );

  /*
  Description:
    Windows Debug Builds:
      The destructor restores the saved state of memory allocation tracking.
    Otherwise:
      Does nothting.
  */
  ~MYON_MemoryAllocationTracking();

private:
  static unsigned int m_g_stack_depth;
  static int m_g_crt_dbg_flag0;
  const unsigned int m_this_statck_depth;
  const int m_this_crt_dbg_flag0;

private:
  MYON_MemoryAllocationTracking() = delete;
  MYON_MemoryAllocationTracking(const MYON_MemoryAllocationTracking&) = delete;
  MYON_MemoryAllocationTracking& operator=(const MYON_MemoryAllocationTracking&) = delete;
};

#endif

#endif

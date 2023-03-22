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

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

MYON_Workspace::MYON_Workspace() 
: m_pFileBlk(0)
, m_pMemBlk(0)
{}

MYON_Workspace::~MYON_Workspace()
{
  Destroy();
}


struct MYON_Workspace_FBLK 
{
  struct MYON_Workspace_FBLK* pNext;
  FILE* pFile;
} * m_pFileBlk;

struct MYON_Workspace_MBLK 
{
  struct MYON_Workspace_MBLK* pNext;
  void* pMem;
} * m_pMemBlk;

void MYON_Workspace::Destroy()
{
  struct MYON_Workspace_FBLK* pFileBlk = m_pFileBlk;
  while ( pFileBlk ) {
    if ( pFileBlk->pFile )
      fclose( pFileBlk->pFile );
    pFileBlk = pFileBlk->pNext;
  }
  m_pFileBlk = 0;

  struct MYON_Workspace_MBLK* pNext = m_pMemBlk;
  struct MYON_Workspace_MBLK* p = nullptr;
  while ( pNext ) {
    p = pNext;
    pNext = pNext->pNext;
    if ( p->pMem ) {
      onfree(p->pMem);
      p->pMem = nullptr;
    }
    onfree( p );
  }
  m_pMemBlk = 0;
}

void* MYON_Workspace::GetMemory( size_t size )
{
  void* p = nullptr;
  if ( size > 0 ) 
  {
    struct MYON_Workspace_MBLK* pBlk = (struct MYON_Workspace_MBLK*)onmalloc(sizeof(*pBlk));
    if ( pBlk ) 
    {
      pBlk->pMem = p = onmalloc(size);
      pBlk->pNext = m_pMemBlk;
      m_pMemBlk = pBlk;
    }
  }
  return p;
}

void* MYON_Workspace::GrowMemory( void* p, size_t size )
{
  void* newp = nullptr;
  if ( !p ) {
    newp = GetMemory(size);
  }
  else {
    struct MYON_Workspace_MBLK* pBlk = m_pMemBlk;
    while ( pBlk ) {
      if ( pBlk->pMem == p ) {
        if ( size > 0 ) {
          newp = onrealloc(p,size);
        }
        else {
          newp = p;
        }
        pBlk->pMem = newp;
        break;
      }
      pBlk = pBlk->pNext;
    }
  }
  return newp;
}

void MYON_Workspace::KeepAllMemory()
{
  struct MYON_Workspace_MBLK* p;
  struct MYON_Workspace_MBLK* pNext = m_pMemBlk;
  m_pMemBlk = 0;
  while ( pNext )
  {
    p = pNext;
    pNext = pNext->pNext;
    p->pMem = 0; // caller want to manage this heap
    onfree( p );
  }
}

bool MYON_Workspace::KeepMemory( void* p )
{
  int rc = false;
  if ( p ) {
    struct MYON_Workspace_MBLK* pPrevBlk = nullptr;
    struct MYON_Workspace_MBLK* pBlk = m_pMemBlk;
    while ( pBlk ) {
      if ( pBlk->pMem == p ) {
        // Remove pBlk from list so ~MYON_Workspace() won't onfree() its memory
        // and any future GrowMemory...() or KeepMemory() calls won't have
        // to search past it.
        pBlk->pMem = nullptr;
        if ( pPrevBlk ) {
          pPrevBlk->pNext = pBlk->pNext;
        }
        else {
          m_pMemBlk = pBlk->pNext;
        }
        onfree( pBlk );
        rc = true;
        break;
      }
      pPrevBlk = pBlk;
      pBlk = pBlk->pNext;
    }
  }
  return rc;
}

int* MYON_Workspace::GetIntMemory( size_t size )
{
  int* pi = (int*)(GetMemory(size*sizeof(*pi)));
  return pi;
}

double* MYON_Workspace::GetDoubleMemory( size_t size )
{
  double* pd = (double*)(GetMemory(size*sizeof(*pd)));
  return pd;
}

MYON_3dPoint* MYON_Workspace::GetPointMemory( size_t size )
{
  MYON_3dPoint* p3d = (MYON_3dPoint*)(GetMemory(size*sizeof(*p3d)));
  return p3d;
}

MYON_3dVector* MYON_Workspace::GetVectorMemory( size_t size )
{
  MYON_3dVector* v3d = (MYON_3dVector*)(GetMemory(size*sizeof(*v3d)));
  return v3d;
}

int** MYON_Workspace::GetIntMemory( size_t row_count, size_t col_count )
{
  int** p = 0;
  size_t i;
  if ( row_count > 0 && col_count > 0 )
  {
    p = (int**)GetMemory(row_count*(sizeof(*p) + col_count*sizeof(**p)));
    if ( p )
    {
      p[0] = (int*)(p+row_count);
      for( i = 1; i < row_count; i++ )
      {
        p[i] = p[i-1] + col_count;
      }
    }
  }
  return p;
}

double** MYON_Workspace::GetDoubleMemory( size_t row_count, size_t col_count )
{
  double** p = 0;
  size_t i;
  if ( row_count > 0 && col_count > 0 )
  {
    // i keeps doubles aligned
    i = (sizeof(*p) < sizeof(**p))
      ? (row_count + (row_count%2))
      : row_count;
    p = (double**)GetMemory(i*sizeof(*p) + row_count*col_count*sizeof(**p));
    if ( p )
    {
      p[0] = (double*)(p+i);
      for( i = 1; i < row_count; i++ )
      {
        p[i] = p[i-1] + col_count;
      }
    }
  }
  return p;
}


int* MYON_Workspace::GrowIntMemory( int* p, size_t size )
{
  int* pi = (int*)(GrowMemory(p,size*sizeof(*pi)));
  return pi;
}

double* MYON_Workspace::GrowDoubleMemory( double* p, size_t size )
{
  double* pd = (double*)(GrowMemory(p,size*sizeof(*pd)));
  return pd;
}

MYON_3dPoint* MYON_Workspace::GrowPointMemory( MYON_3dPoint* p, size_t size )
{
  MYON_3dPoint* p3d = (MYON_3dPoint*)(GrowMemory(p,size*sizeof(*p3d)));
  return p3d;
}

MYON_3dVector* MYON_Workspace::GrowVectorMemory( MYON_3dVector* p, size_t size )
{
  MYON_3dVector* v3d = (MYON_3dVector*)(GrowMemory(p,size*sizeof(*v3d)));
  return v3d;
}


FILE* MYON_Workspace::OpenFile( const char* sFileName, const char* sMode ) 
{
  FILE* pFile = MYON::OpenFile( sFileName, sMode );
  if ( pFile ) 
  {
    struct MYON_Workspace_FBLK* pFileBlk = (struct MYON_Workspace_FBLK*)GetMemory( sizeof(*pFileBlk) );
    pFileBlk->pNext = m_pFileBlk;
    pFileBlk->pFile = pFile;
    m_pFileBlk = pFileBlk;
  }
  return pFile;
}

FILE* MYON_Workspace::OpenFile( const wchar_t* sFileName, const wchar_t* sMode ) 
{
  FILE* pFile = MYON::OpenFile( sFileName, sMode );
  if ( pFile ) 
  {
    struct MYON_Workspace_FBLK* pFileBlk = (struct MYON_Workspace_FBLK*)GetMemory( sizeof(*pFileBlk) );
    pFileBlk->pNext = m_pFileBlk;
    pFileBlk->pFile = pFile;
    m_pFileBlk = pFileBlk;
  }
  return pFile;
}

bool MYON_Workspace::KeepFile( FILE* pFile )
{
  bool rc = false;
  if ( pFile ) {
    struct MYON_Workspace_FBLK* pFileBlk = m_pFileBlk;
    while ( pFileBlk ) {
      if ( pFileBlk->pFile == pFile ) {
        pFileBlk->pFile = nullptr;
        rc = true;
        break;
      }
      pFileBlk = pFileBlk->pNext;
    }
  }
  return rc;
}

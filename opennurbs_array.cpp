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

MYON_2dPointArray::MYON_2dPointArray() 
{}

MYON_2dPointArray::MYON_2dPointArray(int c) 
                : MYON_SimpleArray<MYON_2dPoint>(c) 
{}

MYON_2dPointArray::MYON_2dPointArray(const MYON_2dPointArray& src) 
                : MYON_SimpleArray<MYON_2dPoint>(src)
{}

bool MYON_2dPointArray::GetBBox( // returns true if successful
       double boxmin[2],
       double boxmax[2],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_2dPointArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_2dPointArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 2, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_2dPointArray& MYON_2dPointArray::operator=(const MYON_2dPointArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_2dPoint>::operator=(src);
  }
  return *this;
}

MYON_3dPointArray::MYON_3dPointArray() 
{}

MYON_3dPointArray::MYON_3dPointArray(int c) : MYON_SimpleArray<MYON_3dPoint>(c) 
{}

MYON_3dPointArray::MYON_3dPointArray(const MYON_SimpleArray<MYON_3dPoint>& src) 
                : MYON_SimpleArray<MYON_3dPoint>(src)
{}

MYON_3dPointArray::MYON_3dPointArray(const MYON_SimpleArray<MYON_3fPoint>& src) 
{
  *this = src;
}

MYON_BoundingBox MYON_3dPointArray::BoundingBox() const
{
  MYON_BoundingBox bbox;
  GetBoundingBox(bbox);
  return bbox;
}

MYON_BoundingBox MYON_3dPointArray::BoundingBox(int from, int count) const
{
  MYON_BoundingBox bbox;
  MYON_GetPointListBoundingBox(3, false, count, 3, (m_a) ? &m_a[from].x : 0, &bbox.m_min.x, &bbox.m_max.x, false);
  return bbox;
}

bool MYON_3dPointArray::GetBoundingBox( 
  MYON_BoundingBox& bbox,
  int bGrowBox
  ) const
{
  return GetBBox( &bbox.m_min.x, &bbox.m_max.x, bGrowBox );
}

bool MYON_3dPointArray::GetBBox( // returns true if successful
       double boxmin[3],
       double boxmax[3],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_3dPointArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_3dPointArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 3, (m_a) ? &m_a[0].x : 0, i, j );
}


bool MYON_3dPointArray::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis_of_rotation,
      const MYON_3dPoint& center_of_rotation
      )
{
  const int count = m_count;
  MYON_Xform rot;
  rot.Rotation( sin_angle, cos_angle, axis_of_rotation, center_of_rotation );
  MYON_SimpleArray<int> fix_index(128);
  int i;
  for ( i = 0; i < count; i++ ) {
    if ( m_a[i] == center_of_rotation )
      fix_index.Append(i);
  }
  const bool rc = Transform( rot );
  for ( i = 0; i < fix_index.Count(); i++ ) {
    m_a[fix_index[i]] = center_of_rotation;
  }
  return rc;
}

bool MYON_3dPointArray::Rotate(
      double angle_in_radians,
      const MYON_3dVector& axis_of_rotation,
      const MYON_3dPoint& center_of_rotation
      )
{
  return Rotate( sin(angle_in_radians), cos(angle_in_radians), axis_of_rotation, center_of_rotation );
}

bool MYON_3dPointArray::Translate(
      const MYON_3dVector& delta
      )
{
  int i;
  for (i=0;i<m_count;i++)
    m_a[i]+=delta;
  return (m_count>0)?true:false;
}


MYON_3dPointArray& MYON_3dPointArray::operator=(const MYON_3dPointArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_3dPoint>::operator=(src);
  }
  return *this;
}


MYON_3dPointArray& MYON_3dPointArray::operator=(const MYON_SimpleArray<MYON_3fPoint>& src)
{
  Create( 3, false, src.Count(), 3, (const float*)src.Array() );
  return *this;
}


bool MYON_3dPointArray::Create( 
  int point_dimension,
  int bRational,
  int point_count,
  int point_stride,
  const double* points
  )
{
  bool rc = false;
  if (     point_dimension >= 2 && point_dimension <= 3 
        && point_count>0 && points 
        && point_stride >= bRational?(point_dimension+1):point_dimension )
  {
    rc = true;
    int i;
    MYON_3dPoint q(0.0,0.0,0.0);
    MYON_4dPoint h(0.0,0.0,0.0,1.0);
    m_count = 0;
    SetCapacity(point_count);
    SetCount(point_count);
    if ( bRational )
    {
      for ( i = 0; i < point_count; i++ )
      {
        h.x = points[0];
        h.y = points[1];
        if ( point_dimension == 3 )
          h.z = points[2];
        h.w = points[point_dimension];
        m_a[i] = h;
        points += point_stride;
      }
    }
    else
    {
      for ( i = 0; i < point_count; i++ )
      {
        q.x = points[0];
        q.y = points[1];
        if ( point_dimension == 3 )
          q.z = points[2];
        m_a[i] = q;
        points += point_stride;
      }
    }
  }
  else
    Destroy();
  return rc;
}


bool MYON_3dPointArray::Create( 
  int point_dimension,
  int bRational,
  int point_count,
  int point_stride,
  const float* points
  )
{
  bool rc = false;
  if (     point_dimension >= 2 && point_dimension <= 3 
        && point_count>0 && points 
        && point_stride >= bRational?(point_dimension+1):point_dimension )
  {
    rc = true;
    int i;
    MYON_3dPoint q(0.0,0.0,0.0);
    MYON_4dPoint h(0.0,0.0,0.0,1.0);
    m_count = 0;
    SetCapacity(point_count);
    SetCount(point_count);
    if ( bRational )
    {
      for ( i = 0; i < point_count; i++ )
      {
        h.x = points[0];
        h.y = points[1];
        if ( point_dimension == 3 )
          h.z = points[2];
        h.w = points[point_dimension];
        m_a[i] = h;
        points += point_stride;
      }
    }
    else
    {
      for ( i = 0; i < point_count; i++ )
      {
        q.x = points[0];
        q.y = points[1];
        if ( point_dimension == 3 )
          q.z = points[2];
        m_a[i] = q;
        points += point_stride;
      }
    }
  }
  else
    Destroy();
  return rc;
}



MYON_4dPointArray::MYON_4dPointArray() 
{}

MYON_4dPointArray::MYON_4dPointArray(int c) : MYON_SimpleArray<MYON_4dPoint>(c) 
{}

MYON_4dPointArray::MYON_4dPointArray(const MYON_4dPointArray& src) : MYON_SimpleArray<MYON_4dPoint>(src)
{}

bool MYON_4dPointArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 3, true, Count(), 4, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_4dPointArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 4, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_4dPointArray& MYON_4dPointArray::operator=(const MYON_4dPointArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_4dPoint>::operator=(src);
  }
  return *this;
}

MYON_2dVectorArray::MYON_2dVectorArray() 
{}

MYON_2dVectorArray::MYON_2dVectorArray(int c) : MYON_SimpleArray<MYON_2dVector>(c) 
{}

MYON_2dVectorArray::MYON_2dVectorArray(const MYON_2dVectorArray& src) : MYON_SimpleArray<MYON_2dVector>(src)
{}

bool MYON_2dVectorArray::GetBBox( // returns true if successful
       double boxmin[2],
       double boxmax[2],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_2dVectorArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_2dVectorArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 2, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_2dVectorArray& MYON_2dVectorArray::operator=(const MYON_2dVectorArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_2dVector>::operator=(src);
  }
  return *this;
}

MYON_3dVectorArray::MYON_3dVectorArray() 
{}

MYON_3dVectorArray::MYON_3dVectorArray(int c) : MYON_SimpleArray<MYON_3dVector>(c) 
{}

MYON_3dVectorArray::MYON_3dVectorArray(const MYON_3dVectorArray& src) : MYON_SimpleArray<MYON_3dVector>(src)
{}

bool MYON_3dVectorArray::GetBBox(
       double boxmin[3],
       double boxmax[3],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_3dVectorArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_3dVectorArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 3, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_3dVectorArray& MYON_3dVectorArray::operator=(const MYON_3dVectorArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_3dVector>::operator=(src);
  }
  return *this;
}

////
////
////
MYON_2fPointArray::MYON_2fPointArray() 
{}

MYON_2fPointArray::MYON_2fPointArray(int c) 
                : MYON_SimpleArray<MYON_2fPoint>(c) 
{}

MYON_2fPointArray::MYON_2fPointArray(const MYON_2fPointArray& src) 
                : MYON_SimpleArray<MYON_2fPoint>(src)
{}

bool MYON_2fPointArray::GetBBox( // returns true if successful
       float boxmin[2],
       float boxmax[2],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_2fPointArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_2fPointArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 2, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_2fPointArray& MYON_2fPointArray::operator=(const MYON_2fPointArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_2fPoint>::operator=(src);
  }
  return *this;
}


MYON_3fPointArray::MYON_3fPointArray() 
{}

MYON_3fPointArray::MYON_3fPointArray(int c) : MYON_SimpleArray<MYON_3fPoint>(c)
{}

MYON_3fPointArray::MYON_3fPointArray(const MYON_3fPointArray& src) : MYON_SimpleArray<MYON_3fPoint>(src)
{}

bool MYON_3fPointArray::GetBBox( // returns true if successful
       float boxmin[3],
       float boxmax[3],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_3fPointArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_3fPointArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 3, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_3fPointArray& MYON_3fPointArray::operator=(const MYON_3fPointArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_3fPoint>::operator=(src);
  }
  return *this;
}

MYON_4fPointArray::MYON_4fPointArray() 
{}

MYON_4fPointArray::MYON_4fPointArray(int c) : MYON_SimpleArray<MYON_4fPoint>(c) 
{}

MYON_4fPointArray::MYON_4fPointArray(const MYON_4fPointArray& src) : MYON_SimpleArray<MYON_4fPoint>(src)
{}

bool MYON_4fPointArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 3, true, Count(), 4, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_4fPointArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 4, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_4fPointArray& MYON_4fPointArray::operator=(const MYON_4fPointArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_4fPoint>::operator=(src);
  }
  return *this;
}

MYON_2fVectorArray::MYON_2fVectorArray() 
{}

MYON_2fVectorArray::MYON_2fVectorArray(int c) : MYON_SimpleArray<MYON_2fVector>(c) 
{}

MYON_2fVectorArray::MYON_2fVectorArray(const MYON_2fVectorArray& src) : MYON_SimpleArray<MYON_2fVector>(src)
{}

bool MYON_2fVectorArray::GetBBox(
       float boxmin[2],
       float boxmax[2],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_2fVectorArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 2, false, Count(), 2, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_2fVectorArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 2, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_2fVectorArray& MYON_2fVectorArray::operator=(const MYON_2fVectorArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_2fVector>::operator=(src);
  }
  return *this;
}

MYON_3fVectorArray::MYON_3fVectorArray() 
{}

MYON_3fVectorArray::MYON_3fVectorArray(int c) : MYON_SimpleArray<MYON_3fVector>(c) 
{}

MYON_3fVectorArray::MYON_3fVectorArray(const MYON_3fVectorArray& src) : MYON_SimpleArray<MYON_3fVector>(src)
{}

bool MYON_3fVectorArray::GetBBox( // returns true if successful
       float boxmin[3],
       float boxmax[3],
       bool bGrowBox
       ) const
{
  return MYON_GetPointListBoundingBox( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, boxmin, boxmax, bGrowBox );
}

bool MYON_3fVectorArray::Transform( const MYON_Xform& xform )
{
  return MYON_TransformPointList( 3, false, Count(), 3, (m_a) ? &m_a[0].x : 0, xform );
}

bool MYON_3fVectorArray::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( Count(), 3, (m_a) ? &m_a[0].x : 0, i, j );
}

MYON_3fVectorArray& MYON_3fVectorArray::operator=(const MYON_3fVectorArray& src)
{
  if ( this != &src ) {
    MYON_SimpleArray<MYON_3fVector>::operator=(src);
  }
  return *this;
}

MYON_UuidPair::MYON_UuidPair()
{
  memset(this,0,sizeof(*this));
}

int MYON_UuidPair::CompareFirstUuid(const class MYON_UuidPair* a,const class MYON_UuidPair* b)
{
  if (!a)
  {
    return (b) ? -1 : 0;
  }
  if (!b)
  {
    return 1;
  }
  return MYON_UuidCompare(a->m_uuid[0],b->m_uuid[0]);
}

int MYON_UuidPair::CompareSecondUuid(const class MYON_UuidPair* a,const class MYON_UuidPair* b)
{
  if (!a)
  {
    return (b) ? -1 : 0;
  }
  if (!b)
  {
    return 1;
  }
  return MYON_UuidCompare(a->m_uuid[1],b->m_uuid[1]);
}

int MYON_UuidPair::Compare(const class MYON_UuidPair* a,const class MYON_UuidPair* b)
{
  int i;
  if (!a)
  {
    return (b) ? -1 : 0;
  }
  if (!b)
  {
    return 1;
  }
  if ( 0 == (i = MYON_UuidCompare(a->m_uuid[0],b->m_uuid[0])) )
  {
    i = MYON_UuidCompare(a->m_uuid[1],b->m_uuid[1]);
  }
  return i;
}

MYON_UuidList::MYON_UuidList() 
                     : MYON_SimpleArray<MYON_UUID>(32),
                       m_sorted_count(0),
                       m_removed_count(0)
{
}

MYON_UuidList::MYON_UuidList(int capacity) 
                     : MYON_SimpleArray<MYON_UUID>(capacity),
                       m_sorted_count(0),
                       m_removed_count(0)
{
}

MYON_UuidList::~MYON_UuidList()
{
  m_sorted_count = 0;
  m_removed_count = 0;
}

MYON_UuidList::MYON_UuidList(const MYON_UuidList& src) 
                     : MYON_SimpleArray<MYON_UUID>(src),
                       m_sorted_count(src.m_sorted_count),
                       m_removed_count(src.m_removed_count)
{
}

MYON_UuidList& MYON_UuidList::operator=(const MYON_UuidList& src)
{
  if ( this != &src)
  {
    MYON_SimpleArray<MYON_UUID>::operator=(src);
    m_sorted_count = src.m_sorted_count;
    m_removed_count = src.m_removed_count;
  }
  return *this;
}

bool MYON_UuidList::operator==(const MYON_UuidList& other) const
{
  int thisCount = Count();
  int otherCount = other.Count();
  if (0 == thisCount && 0 == otherCount)
    return true;
  if (thisCount != otherCount)
    return false;

  const MYON_UUID* thisArray = Array();
  const MYON_UUID* otherArray = other.Array();
  int compare = memcmp(thisArray, otherArray, sizeof(MYON_UUID) * thisCount);
  return 0 == compare;
}

bool MYON_UuidList::operator!=(const MYON_UuidList& other) const
{
  return !MYON_UuidList::operator==(other);
}


bool MYON_UuidList::AddUuid(MYON_UUID uuid, bool bCheckForDupicates)
{
  bool rc = bCheckForDupicates ? !FindUuid(uuid) : true;
  if (rc)
  {
    Append(uuid);
  }
  return rc;
}

int MYON_UuidList::Count() const
{
  return m_count - m_removed_count;
}

int MYON_UuidList::CompareUuid(const MYON_UUID* a, const MYON_UUID* b)
{
  return memcmp(a,b,sizeof(*a));
}


void MYON_UuidList::PurgeHelper()
{
  if ( m_removed_count <= m_count && m_removed_count > 0 )
  {
    m_removed_count = 0;

    // purge removed ids
    int count = 0;
    for ( int i = 0; i < m_count; i++ )
    {
      if ( MYON_max_uuid == m_a[i] )
        continue;

      if ( i > count )
        m_a[count] = m_a[i];

      count++;
    }

    if ( count < m_count )
    {
      m_count = count;
      if ( count > 0 )
      {
        // set m_sorted_count
        for ( m_sorted_count = 1; m_sorted_count < m_count; m_sorted_count++ )
        {
          if ( MYON_UuidCompare(m_a[m_sorted_count-1],m_a[m_sorted_count])> 0 )
            break;
        }
      }
      else
      {
        m_sorted_count = 0;
      }
    }
  }
}

void MYON_UuidList::SortHelper()
{
  if ( m_sorted_count < m_count || m_removed_count > 0 )
  {
    // clean up array
    QuickSort(MYON_UuidList::CompareUuid);
    while ( m_count > 0 && MYON_max_uuid == m_a[m_count-1] )
    {
      m_count--;
    }
    m_removed_count = 0;
    m_sorted_count = m_count;
  }
}

const MYON_UUID* MYON_UuidList::Array() const
{
  const MYON_UUID* array = 0;
  if ( m_count > m_removed_count )
  {
    const_cast<MYON_UuidList*>(this)->SortHelper();
    if (m_count > 0 && m_sorted_count == m_count && 0 == m_removed_count )
      array = m_a;
  }
  return array;
}

void MYON_UuidList::Empty()
{
  m_count = 0;
  m_sorted_count = 0;
  m_removed_count = 0;
}

void MYON_UuidList::Destroy()
{
  MYON_SimpleArray<MYON_UUID>::Destroy();
  m_count = 0;
  m_sorted_count = 0;
  m_removed_count = 0;
}

void MYON_UuidList::Reserve(size_t capacity)
{
  MYON_SimpleArray<MYON_UUID>::Reserve(capacity);
}

bool MYON_UuidList::RemoveUuid(MYON_UUID uuid)
{
  MYON_UUID* p = SearchHelper(&uuid);
  if ( 0 != p )
  {
    *p = MYON_max_uuid;
    m_removed_count++;
  }
  return (0!=p);
}

void MYON_UuidList::Compact()
{
  SortHelper();
  SetCapacity(m_count);
}

bool MYON_UuidList::Write( class MYON_BinaryArchive& archive ) const
{
  return Write(archive,true);
}

bool MYON_UuidList::Write( 
  class MYON_BinaryArchive& archive, 
  bool bSortBeforeWrite 
  ) const
{
  // NOTE:
  // Per bug 101403, this function is called with
  // bSortBeforeWrite = false when writing MYON_HistoryRecord::m_descendants[].
  // All other used call this function with bSortBeforeWrite = true.
  bool rc = archive.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK, 1, 0 );
  if (rc)
  {
    if ( bSortBeforeWrite )
    {
      // clean and sort
      const_cast<MYON_UuidList*>(this)->SortHelper();
    }
    else
    {
      // clean
      const_cast<MYON_UuidList*>(this)->PurgeHelper();
    }
    rc = archive.WriteArray( *this );
    if ( !archive.EndWrite3dmChunk() )
      rc = false;
  }
  return rc;
}

bool MYON_UuidList::Read( class MYON_BinaryArchive& archive )
{
  return  MYON_UuidList::Read(archive,true);
}

bool MYON_UuidList::Read( 
    class MYON_BinaryArchive& archive,
    bool bSortAferRead
    )
{
  // NOTE:
  // Per bug 101403, this function is called with
  // bSortAferRead = false when reading MYON_HistoryRecord::m_descendants[].
  // All other used call this function with bSortAferRead = true.
  m_count = 0;
  m_removed_count = 0;
  m_sorted_count = 0;
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK, 
                                       &major_version, 
                                       &minor_version );
  if (rc)
  {
    if ( 1 != major_version )
      rc = false;

    if (rc)
      rc = archive.ReadArray( *this );

    if ( !archive.EndRead3dmChunk() )
      rc = false;
  }

  if ( bSortAferRead )
  {
    // clean and sort
    SortHelper();
  }
  else
  {
    // clean
    PurgeHelper();
  }

  return rc;
}


bool MYON_UuidList::FindUuid(MYON_UUID uuid) const
{
  return (0!=SearchHelper(&uuid));
}

MYON_UUID* MYON_UuidList::SearchHelper(const MYON_UUID* uuid) const
{
  if ( m_count - m_sorted_count > 8 || m_removed_count > 0 )
  {
    // time to resort the array so that the speedy
    // bsearch() can be used to find uuids
    const_cast<MYON_UuidList*>(this)->SortHelper();
  }

  MYON_UUID* p = (m_sorted_count > 0 )
             ? (MYON_UUID*)bsearch( uuid, m_a, m_sorted_count, sizeof(m_a[0]), 
                                  (int(*)(const void*,const void*))MYON_UuidList::CompareUuid ) 
             : 0;

  if (0 == p)
  {
    // do a slow search on the last m_count-m_sort_count elements
    // in the array.
    int i;
    for ( i = m_sorted_count; i < m_count; i++ )
    {
      if ( 0 == MYON_UuidList::CompareUuid(uuid,m_a+i) )
      {
        p = m_a+i;
        break;
      }
    }
  }

  return p;
}


MYON_UuidIndexList::MYON_UuidIndexList(size_t capacity) 
  : MYON_SimpleArray<MYON_UuidIndex>(capacity>32?capacity:32)
{
}

MYON_UuidIndexList::MYON_UuidIndexList(const MYON_UuidIndexList& src) 
  : MYON_SimpleArray<MYON_UuidIndex>(src)
  , m_sorted_count(src.m_sorted_count)
  ,  m_removed_count(src.m_removed_count)
{
}

MYON_UuidIndexList& MYON_UuidIndexList::operator=(const MYON_UuidIndexList& src)
{
  if ( this != &src)
  {
    MYON_SimpleArray<MYON_UuidIndex>::operator=(src);
    m_sorted_count = src.m_sorted_count;
    m_removed_count = src.m_removed_count;
  }
  return *this;
}

bool MYON_UuidIndexList::AddUuidIndex(MYON_UUID uuid, int index, bool bCheckForDupicates)
{
  bool rc = bCheckForDupicates ? !FindUuid(uuid,nullptr) : true;
  if (rc)
  {
    if ( MYON_max_uuid == uuid )
      rc = 0;
    else
    {
      MYON_UuidIndex& ui = AppendNew();
      ui.m_id = uuid;
      ui.m_i = index;
    }
  }
  return rc;
}

unsigned int MYON_UuidIndexList::Count() const
{
  return m_count - m_removed_count;
}

void MYON_UuidIndexList::RemoveAll()
{
  m_count = 0;
  m_sorted_count = 0;
  m_removed_count = 0;
}

void MYON_UuidIndexList::Reserve( size_t capacity )
{
  MYON_SimpleArray<MYON_UuidIndex>::Reserve(capacity);
}

bool MYON_UuidIndexList::RemoveUuid(MYON_UUID uuid)
{
  MYON_UuidIndex* p = SearchHelper(&uuid);
  if ( 0 != p )
  {
    p->m_id = MYON_max_uuid;
    m_removed_count++;
    unsigned int i = (unsigned int)(p - m_a);
    if ( i < m_sorted_count )
      m_sorted_count = i;
  }
  return (0!=p);
}

static
int compar_uuidindex_uuid(const MYON_UuidIndex* a, const MYON_UuidIndex* b)
{
  return MYON_UuidList::CompareUuid(&a->m_id,&b->m_id);
}

bool MYON_UuidIndexList::FindUuid(MYON_UUID uuid, int* index) const
{
  const MYON_UuidIndex* ui = SearchHelper(&uuid);
  if (ui && index)
  {
    *index = ui->m_i;
  }
  return (0!=ui);
}


bool MYON_UuidIndexList::FindUuid(MYON_UUID uuid) const
{
  return FindUuid(uuid,nullptr);
}


bool MYON_UuidIndexList::FindUuidIndex(MYON_UUID uuid, int index) const
{
  const MYON_UuidIndex* ui = SearchHelper(&uuid);
  if (ui && index != ui->m_i)
  {
    ui = 0;
  }
  return (0!=ui);
}


MYON_UuidPtrList::MYON_UuidPtrList(size_t capacity) 
  : MYON_SimpleArray<MYON_UuidPtr>(capacity>32?capacity:32)
{
}

MYON_UuidPtrList::MYON_UuidPtrList(const MYON_UuidPtrList& src) 
  : MYON_SimpleArray<MYON_UuidPtr>(src)
  , m_sorted_count(src.m_sorted_count)
  ,  m_removed_count(src.m_removed_count)
{
}

MYON_UuidPtrList& MYON_UuidPtrList::operator=(const MYON_UuidPtrList& src)
{
  if ( this != &src)
  {
    MYON_SimpleArray<MYON_UuidPtr>::operator=(src);
    m_sorted_count = src.m_sorted_count;
    m_removed_count = src.m_removed_count;
  }
  return *this;
}

bool MYON_UuidPtrList::AddUuidPtr(MYON_UUID uuid, MYON__UINT_PTR ptr, bool bCheckForDupicates)
{
  bool rc = bCheckForDupicates ? !FindUuid(uuid,nullptr) : true;
  if (rc)
  {
    if ( MYON_max_uuid == uuid )
      rc = 0;
    else
    {
      MYON_UuidPtr& ui = AppendNew();
      ui.m_id = uuid;
      ui.m_ptr = ptr;
    }
  }
  return rc;
}

unsigned int MYON_UuidPtrList::Count() const
{
  return m_count - m_removed_count;
}

void MYON_UuidPtrList::RemoveAll()
{
  m_count = 0;
  m_sorted_count = 0;
  m_removed_count = 0;
}

void MYON_UuidPtrList::Reserve( size_t capacity )
{
  MYON_SimpleArray<MYON_UuidPtr>::Reserve(capacity);
}

bool MYON_UuidPtrList::RemoveUuid(MYON_UUID uuid)
{
  MYON_UuidPtr* p = SearchHelper(&uuid);
  if ( 0 != p )
  {
    p->m_id = MYON_max_uuid;
    m_removed_count++;
    unsigned int i = (unsigned int)(p - m_a);
    if ( i < m_sorted_count )
      m_sorted_count = i;
  }
  return (0!=p);
}

static
int compar_uuidptr_uuid(const MYON_UuidPtr* a, const MYON_UuidPtr* b)
{
  return MYON_UuidList::CompareUuid(&a->m_id,&b->m_id);
}

bool MYON_UuidPtrList::FindUuid(MYON_UUID uuid) const
{
  return FindUuid(uuid,nullptr);
}

bool MYON_UuidPtrList::FindUuid(MYON_UUID uuid, MYON__UINT_PTR* index) const
{
  const MYON_UuidPtr* ui = SearchHelper(&uuid);
  if (ui && index)
  {
    *index = ui->m_ptr;
  }
  return (0!=ui);
}

bool MYON_UuidPtrList::FindUuidPtr(MYON_UUID uuid, MYON__UINT_PTR ptr) const
{
  const MYON_UuidPtr* ui = SearchHelper(&uuid);
  if (ui && ptr != ui->m_ptr)
  {
    ui = 0;
  }
  return (0!=ui);
}

unsigned int MYON_UuidPtrList::GetUuids(
    MYON_SimpleArray<MYON_UUID>& uuid_list
    ) const
{
  const int count0 = uuid_list.Count();
  int i;
  uuid_list.Reserve(uuid_list.Count() + m_count);
  for ( i = 0; i < m_count; i++ )
  {
    if ( MYON_max_uuid == m_a[i].m_id )
      continue;
    uuid_list.Append(m_a[i].m_id);
  }
  return uuid_list.UnsignedCount() - count0;
}

void MYON_UuidPtrList::ImproveSearchSpeed()
{
  if ( ((unsigned int)m_count) > m_sorted_count )
  {
    QuickSort(compar_uuidptr_uuid);
    if ( m_removed_count > 0 )
    {
      // cull removed items.  These get sorted to the
      // end because the removed uuid is the largest
      // possible uuid.
      MYON_UuidPtr removed_uuid;
      removed_uuid.m_id = MYON_max_uuid;
      removed_uuid.m_ptr = 0;
      while ( m_count > 0 && !compar_uuidptr_uuid(&removed_uuid,m_a+(m_count-1)))
      {
        m_count--;
      }
      m_removed_count = 0;
    }
    m_sorted_count = m_count;
  }
}

MYON_UuidPtr* MYON_UuidPtrList::SearchHelper(const MYON_UUID* uuid) const
{
  if ( m_count - m_sorted_count > 8 || m_removed_count > 0 )
  {
    // time to resort the array so that the speedy
    // bsearch() can be used to find uuids
    const_cast<MYON_UuidPtrList*>(this)->ImproveSearchSpeed();
  }

  MYON_UuidPtr* p = (m_sorted_count > 0 )
                   ? (MYON_UuidPtr*)bsearch( uuid, m_a, m_sorted_count, 
                                        sizeof(m_a[0]), 
                                        (int(*)(const void*,const void*))compar_uuidptr_uuid ) 
                   : 0;
  if (0 == p)
  {
    // do a slow search on the last m_count-m_sort_count elements
    // in the array.
    int i;
    for ( i = m_sorted_count; i < m_count; i++ )
    {
      if ( 0 == MYON_UuidList::CompareUuid(uuid,&m_a[i].m_id) )
      {
        p = m_a+i;
        break;
      }
    }
  }

  return p;
}



MYON_UuidPairList::MYON_UuidPairList() 
: MYON_SimpleArray<MYON_UuidPair>(32)
, m_sorted_count(0)
, m_removed_count(0)
{
}

MYON_UuidPairList::MYON_UuidPairList(int capacity) 
: MYON_SimpleArray<MYON_UuidPair>(capacity>32?capacity:32)
, m_sorted_count(0)
, m_removed_count(0)
{
}

MYON_UuidPairList::~MYON_UuidPairList()
{
  m_sorted_count = 0;
  m_removed_count = 0;
}

MYON_UuidPairList::MYON_UuidPairList(const MYON_UuidPairList& src) 
: MYON_SimpleArray<MYON_UuidPair>(src)
, m_sorted_count(src.m_sorted_count)
, m_removed_count(src.m_removed_count)
{
}

MYON_UuidPairList& MYON_UuidPairList::operator=(const MYON_UuidPairList& src)
{
  if ( this != &src)
  {
    MYON_SimpleArray<MYON_UuidPair>::operator=(src);
    m_sorted_count = src.m_sorted_count;
    m_removed_count = src.m_removed_count;
  }
  return *this;
}

bool MYON_UuidPairList::AddPair(MYON_UUID id1, MYON_UUID id2, bool bCheckForDupicates)
{
  bool rc = bCheckForDupicates ? !FindId1(id1,0) : true;
  if (rc)
  {
    if ( MYON_max_uuid == id1 && MYON_max_uuid == id2 )
    {
      // The value pair (MYON_max_uuid,MYON_max_uuid) is used
      // to mark removed elements.
      rc = false;
    }
    else
    {
      MYON_UuidPair& ui = AppendNew();
      ui.m_uuid[0] = id1;
      ui.m_uuid[1] = id2;
    }
  }
  return rc;
}

int MYON_UuidPairList::Count() const
{
  return m_count - m_removed_count;
}

void MYON_UuidPairList::Empty()
{
  m_count = 0;
  m_sorted_count = 0;
  m_removed_count = 0;
}


void MYON_UuidPairList::Reserve( size_t capacity )
{
  MYON_SimpleArray<MYON_UuidPair>::Reserve(capacity);
}

bool MYON_UuidPairList::RemovePair(MYON_UUID id1)
{
  MYON_UuidPair* p = SearchHelper(&id1);
  if ( 0 != p )
  {
    p->m_uuid[0] = MYON_max_uuid;
    p->m_uuid[1] = MYON_max_uuid;
    m_removed_count++;
    unsigned int i = (unsigned int)(p - m_a);
    if ( i < m_sorted_count )
      m_sorted_count = i;
  }
  return (0!=p);
}

bool MYON_UuidPairList::RemovePair(MYON_UUID id1, MYON_UUID id2)
{
  MYON_UuidPair* p = SearchHelper(&id1);
  if ( 0 != p && p->m_uuid[1] == id2)
  {
    p->m_uuid[0] = MYON_max_uuid;
    p->m_uuid[1] = MYON_max_uuid;
    m_removed_count++;
    unsigned int i = (unsigned int)(p - m_a);
    if ( i < m_sorted_count )
      m_sorted_count = i;
  }
  return (0!=p);
}

static
int compar_uuidpair_id1(const MYON_UuidPair* a, const MYON_UuidPair* b)
{
  return MYON_UuidList::CompareUuid(&a->m_uuid[0],&b->m_uuid[0]);
}

static
int compar_uuidpair_id1id2(const MYON_UuidPair* a, const MYON_UuidPair* b)
{
  int i;
  if ( 0 == (i = MYON_UuidList::CompareUuid(&a->m_uuid[0],&b->m_uuid[0])) )
    i = MYON_UuidList::CompareUuid(&a->m_uuid[1],&b->m_uuid[1]);
  return i;
}

bool MYON_UuidPairList::FindId1(MYON_UUID id1, MYON_UUID* id2) const
{
  const MYON_UuidPair* ui = SearchHelper(&id1);
  if (ui && id2)
  {
    *id2 = ui->m_uuid[1];
  }
  return (0!=ui);
}

bool MYON_UuidPairList::FindPair(MYON_UUID id1, MYON_UUID id2) const
{
  const MYON_UuidPair* ui = SearchHelper(&id1);
  if (ui && id2 != ui->m_uuid[1])
  {
    ui = 0;
  }
  return (0!=ui);
}

int MYON_UuidPairList::GetId1s(
    MYON_SimpleArray<MYON_UUID>& uuid_list
    ) const
{
  const int count0 = uuid_list.Count();
  int i;
  uuid_list.Reserve(uuid_list.Count() + m_count - m_removed_count);
  for ( i = 0; i < m_count; i++ )
  {
    if ( MYON_max_uuid == m_a[i].m_uuid[0] && MYON_max_uuid == m_a[i].m_uuid[1] )
      continue;
    uuid_list.Append(m_a[i].m_uuid[0]);
  }
  return uuid_list.Count() - count0;
}

void MYON_UuidPairList::ImproveSearchSpeed()
{
  if ( ((unsigned int)m_count) > m_sorted_count )
  {
    QuickSort(compar_uuidpair_id1id2);
    if ( m_removed_count > 0 )
    {
      // cull removed items.  These get sorted to the
      // end because the removed uuid is the largest
      // possible uuid.
      MYON_UuidPair removed_uuid;
      removed_uuid.m_uuid[0] = MYON_max_uuid;
      removed_uuid.m_uuid[1] = MYON_max_uuid;
      while ( m_count > 0 && !compar_uuidpair_id1id2(&removed_uuid,m_a+(m_count-1)))
      {
        m_count--;
      }
      m_removed_count = 0;
    }
    m_sorted_count = m_count;
  }
}

bool MYON_UuidPairList::Write(
  MYON_BinaryArchive& archive
  ) const
{
  const unsigned int count = UnsignedCount();
  const MYON_UuidPair* pairs = Array();
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0))
    return false;
  bool rc = false;
  for (;;)
  {
    if (!archive.WriteInt(count))
      break;
    unsigned int i = 0;
    for (/*empty init*/; i < count; i++)
    {
      if (!archive.WriteUuid(pairs[i].m_uuid[0]))
        break;
      if (!archive.WriteUuid(pairs[i].m_uuid[1]))
        break;
    }
    if ( i != count)
      break;
    rc = true;
    break;
  }
  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_UuidPairList::Read(
  MYON_BinaryArchive& archive
  )
{
  SetCount(0);
  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version))
    return false;
  bool rc = false;
  for (;;)
  {
    unsigned int count = MYON_UNSET_UINT_INDEX;
    if (!archive.ReadInt(&count))
      break;
    if ( MYON_UNSET_UINT_INDEX == count )
      break;
    Reserve(count);    
    MYON_UuidPair pair;
    for (unsigned int i = 0; i < count; i++)
    {
      if (!archive.ReadUuid(pair.m_uuid[0]))
        break;
      if (!archive.ReadUuid(pair.m_uuid[1]))
        break;
      Append(pair);
    }
    if ( UnsignedCount() != count)
      break;
    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}

MYON_UuidPair* MYON_UuidPairList::SearchHelper(const MYON_UUID* id1) const
{
  if ( m_count - m_sorted_count > 8 || m_removed_count > 0 )
  {
    // time to resort the array so that the speedy
    // bsearch() can be used to find uuids
    const_cast<MYON_UuidPairList*>(this)->ImproveSearchSpeed();
  }

  // NOTE: 
  //   The pair (MYON_max_uuid,MYON_max_uuid) never appears in the sorted portion of the array.
  MYON_UuidPair* p = (m_sorted_count > 0 )
                   ? (MYON_UuidPair*)bsearch( id1, m_a, m_sorted_count, 
                                        sizeof(m_a[0]), 
                                        (int(*)(const void*,const void*))compar_uuidpair_id1 ) 
                   : 0;

  if (0 == p)
  {
    // do a slow search on the last m_count-m_sort_count elements
    // in the array.
    int i;
    for ( i = m_sorted_count; i < m_count; i++ )
    {
      if (    0 == MYON_UuidList::CompareUuid(id1,&m_a[i].m_uuid[0]) 
           && (MYON_max_uuid != m_a[i].m_uuid[0] || MYON_max_uuid != m_a[i].m_uuid[1])
         )
      {
        p = m_a+i;
        break;
      }
    }
  }

  return p;
}

void MYON_UuidList::RemapUuids( const MYON_SimpleArray<MYON_UuidPair>& uuid_remap )
{
  if( m_count > 0 && uuid_remap.Count() > 0 )
  {
    bool bRemapped = false;
    int i, j;
    for ( i = 0; i < m_count; i++ )
    {
      j = uuid_remap.BinarySearch( (const MYON_UuidPair*)&m_a[i], MYON_UuidPair::CompareFirstUuid );
      if ( j >= 0 )
      {
        if ( MYON_max_uuid == m_a[i] )
          continue;
        m_sorted_count = 0;
        bRemapped = true;
        m_a[i] = uuid_remap[j].m_uuid[1];
        if ( MYON_max_uuid == m_a[i] )
          m_removed_count++;
      }
    }

    if ( bRemapped )
    {
      m_sorted_count = 0;
      SortHelper();
      for ( i = m_count-1; i > 0; i-- )
      {
        if ( m_a[i] == m_a[i-1] )
        {
          Remove(i);
          m_sorted_count--;
        }
      }
    }
  }
}

int MYON_UuidList::GetUuids(
    MYON_SimpleArray<MYON_UUID>& uuid_list
    ) const
{
  const int count0 = uuid_list.Count();
  int i;
  uuid_list.Reserve(uuid_list.Count() + (m_count-m_removed_count));
  for ( i = 0; i < m_count; i++ )
  {
    if ( MYON_max_uuid == m_a[i] )
      continue;
    uuid_list.Append(m_a[i]);
  }
  return uuid_list.Count() - count0;
}

unsigned int MYON_UuidIndexList::GetUuids(
    MYON_SimpleArray<MYON_UUID>& uuid_list
    ) const
{
  const int count0 = uuid_list.Count();
  int i;
  uuid_list.Reserve(uuid_list.Count() + m_count);
  for ( i = 0; i < m_count; i++ )
  {
    if ( MYON_max_uuid == m_a[i].m_id )
      continue;
    uuid_list.Append(m_a[i].m_id);
  }
  return uuid_list.UnsignedCount() - count0;
}

void MYON_UuidIndexList::ImproveSearchSpeed()
{
  if ( ((unsigned int)m_count) > m_sorted_count )
  {
    QuickSort(compar_uuidindex_uuid);
    if ( m_removed_count > 0 )
    {
      // cull removed items.  These get sorted to the
      // end because the removed uuid is the largest
      // possible uuid.
      MYON_UuidIndex removed_uuid;
      removed_uuid.m_id = MYON_max_uuid;
      removed_uuid.m_i = 0;
      while ( m_count > 0 && !compar_uuidindex_uuid(&removed_uuid,m_a+(m_count-1)))
      {
        m_count--;
      }
      m_removed_count = 0;
    }
    m_sorted_count = m_count;
  }
}

MYON_UuidIndex* MYON_UuidIndexList::SearchHelper(const MYON_UUID* uuid) const
{
  if ( m_count - m_sorted_count > 8 || m_removed_count > 0 )
  {
    // time to resort the array so that the speedy
    // bsearch() can be used to find uuids
    const_cast<MYON_UuidIndexList*>(this)->ImproveSearchSpeed();
  }

  MYON_UuidIndex* p = (m_sorted_count > 0 )
                   ? (MYON_UuidIndex*)bsearch( uuid, m_a, m_sorted_count, 
                                        sizeof(m_a[0]), 
                                        (int(*)(const void*,const void*))compar_uuidindex_uuid ) 
                   : 0;
  if (0 == p)
  {
    // do a slow search on the last m_count-m_sort_count elements
    // in the array.
    int i;
    for ( i = m_sorted_count; i < m_count; i++ )
    {
      if ( 0 == MYON_UuidList::CompareUuid(uuid,&m_a[i].m_id) )
      {
        p = m_a+i;
        break;
      }
    }
  }

  return p;
}

MYON_2dexMap::MYON_2dexMap() : m_bSorted(0)
{}

MYON_2dexMap::MYON_2dexMap(int capacity) 
            : MYON_SimpleArray<MYON_2dex>(capacity), 
            m_bSorted(0)
{}

MYON_2dexMap::~MYON_2dexMap()
{}

int MYON_2dexMap::Count() const
{
  return m_count;
}

const MYON_2dex* MYON_2dexMap::Array() const
{
  return m_a;
}

void MYON_2dexMap::Reserve(size_t capacity )
{
  MYON_SimpleArray<MYON_2dex>::Reserve(capacity);
}

MYON_2dex MYON_2dexMap::operator[](int i) const
{
  return m_a[i];
}

void MYON_2dexMap::Create(int count, int i0, int j)
{
  if ( count <= 0 )
  {
    m_count = 0;
  }
  else
  {
    MYON_SimpleArray<MYON_2dex>::Reserve(count);
    m_count = count;
    MYON_2dex* a = m_a;
    MYON_2dex d;
    d.j = j;
    count += i0;
    for ( d.i = i0; d.i < count; d.i++ )
    {
      *a++ = d;
    }
  }
  m_bSorted = true;
}

const MYON_2dex* MYON_BinarySearch2dexArray( int key_i, const MYON_2dex* base, size_t nel )
{
  if (nel > 0 && base )
  {
    size_t i;
    int base_i;

    // The end tests are not necessary, but they
    // seem to provide overall speed improvement
    // for the types of searches that call this
    // function.

    // 26 January 2012 Dale Lear
    //   Part of the fix for http://dev.mcneel.com/bugtrack/?q=97693
    //   When the values of key_i and base[].i are large,
    //   key_i - base[].i suffers from integer overflow
    //   and the difference can not be use to compare
    //   magnitudes.

    base_i = base[0].i;
    if ( key_i < base_i )
      return 0;
    if ( key_i == base_i )
      return base;

    base_i = base[nel-1].i;
    if ( key_i > base_i )
      return 0;
    if ( key_i == base_i )
      return (base + (nel-1));

    while ( nel > 0 )
    {
      i = nel/2;
      base_i = base[i].i;
      if ( key_i < base_i )
      {
        nel = i;
      }
      else if ( key_i > base_i )
      {
        i++;
        base += i;
        nel -= i;
      }
      else
      {
        return base+i;
      }
    }
  }
  return 0;
}

const MYON_2udex* MYON_BinarySearch2udexArray(unsigned int key_i, const MYON_2udex* base, size_t nel)
{
  if (nel > 0 && base)
  {
    size_t i;
    unsigned int base_i;

    base_i = base[0].i;
    if (key_i < base_i)
      return 0;
    if (key_i == base_i)
      return base;

    base_i = base[nel - 1].i;
    if (key_i > base_i)
      return 0;
    if (key_i == base_i)
      return (base + (nel - 1));

    while (nel > 0)
    {
      i = nel / 2;
      base_i = base[i].i;
      if (key_i < base_i)
      {
        nel = i;
      }
      else if (key_i > base_i)
      {
        i++;
        base += i;
        nel -= i;
      }
      else
      {
        return base + i;
      }
    }
  }
  return 0;
}

static
int compare_2dex_i(const void* a, const void* b)
{
  const int ai = *((const int*)a);
  const int bi = *((const int*)b);
  if ( ai < bi )
    return -1;
  if ( ai > bi )
    return 1;
  return 0;
}

const MYON_2dex* MYON_2dexMap::Find2dex(int i) const
{
  const MYON_2dex* e = 0;
  if ( m_count > 0 )
  {
    if ( !m_bSorted )
    {
      MYON_qsort(m_a,m_count,sizeof(m_a[0]),compare_2dex_i);
      const_cast<MYON_2dexMap*>(this)->m_bSorted = true;
    }
    e = MYON_BinarySearch2dexArray(i,m_a,m_count);
  }
  return e;
}

int MYON_2dexMap::FindIndex( int i, int not_found_rc) const
{
  const MYON_2dex* e = Find2dex(i);
  return e ? e->j : not_found_rc;
}

bool MYON_2dexMap::AddIndex(  int i, int j )
{
  bool rc = (0 == Find2dex(i));
  if ( rc )
  {
    MYON_2dex& d = AppendNew();
    d.i = i;
    d.j = j;
    m_bSorted = ( m_count < 2 || (m_bSorted && m_a[m_count-2].i < i) );
  }
  return rc;
}

bool MYON_2dexMap::SetIndex( int i, int j )
{
  MYON_2dex* e = const_cast<MYON_2dex*>(Find2dex(i));
  if ( e )
  {
    e->j = j;
  }
  return (0!=e);
}

void MYON_2dexMap::SetOrAddIndex( int i, int j )
{
  MYON_2dex* e = const_cast<MYON_2dex*>(Find2dex(i));
  if ( e )
  {
    e->j = j;
  }
  else
  {
    MYON_2dex& d = AppendNew();
    d.i = i;
    d.j = j;
    m_bSorted = ( m_count < 2 || (m_bSorted && m_a[m_count-2].i < i) );
  }
}

bool MYON_2dexMap::RemoveIndex( int i )
{
  const MYON_2dex* e = Find2dex(i);
  if (e)
  {
    int n = (int)(m_a-e);
    for( m_count--; n < m_count; n++ )
      m_a[n] = m_a[n+1];
  }
  return (0 != e);
}


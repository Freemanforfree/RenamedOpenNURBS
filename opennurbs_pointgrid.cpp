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

MYON_OBJECT_IMPLEMENT(MYON_PointGrid,MYON_Geometry,"4ED7D4E5-E947-11d3-BFE5-0010830122F0");

MYON_3dPoint MYON_PointGrid::m_no_point(0.0,0.0,0.0);

MYON_PointGrid::MYON_PointGrid()
{
  Initialize();
}

MYON_PointGrid::MYON_PointGrid( int c0, int c1 )
{
  Initialize();
  Create(c0,c1);
}

MYON_PointGrid::MYON_PointGrid( const MYON_PointGrid& src )
{
  *this = src;
}

MYON_PointGrid::~MYON_PointGrid()
{
  Destroy();
}

int MYON_PointGrid::Dimension() const
{
  return 3;
}

int MYON_PointGrid::PointCount( int dir ) const
{
  return m_point_count[dir?1:0];
}

int MYON_PointGrid::PointCount( void ) const
{
  return m_point_count[0]*m_point_count[1];
}

MYON_3dPoint& MYON_PointGrid::Point( int i, int j )
{
  return (0 <= i && i < m_point_count[0] && 0 <= j && j < m_point_count[1]) 
         ? m_point[i*m_point_stride0 + j] 
         : m_no_point;
}

MYON_3dPoint MYON_PointGrid::Point( int i, int j ) const
{
  return (0 <= i && i < m_point_count[0] && 0 <= j && j < m_point_count[1]) 
         ? m_point[i*m_point_stride0 + j] 
         : m_no_point;
}

double* MYON_PointGrid::PointArray()
{
  return (m_point_count[0]>0&&m_point_count[1]>0) ? &m_point[0].x : nullptr;
}

const double* MYON_PointGrid::PointArray() const
{
  return (m_point_count[0]>0&&m_point_count[1]>0) ? &m_point[0].x : nullptr;
}

int MYON_PointGrid::PointArrayStride(  // point stride in grid direction
      int dir        // dir 0 = "s", 1 = "t"
      ) const
{
  return ((dir) ? 3 : 3*m_point_stride0);
}


bool MYON_PointGrid::SetPoint( int i, int j, const MYON_3dPoint& point )
{
  bool rc = false;
  if ( 0 <= i && i < m_point_count[0] && 0 <= j && j < m_point_count[1] ) {
    m_point[i*m_point_stride0+j] = point;
    rc = true;
  }
  return rc;
}

bool MYON_PointGrid::GetPoint( int i, int j, MYON_3dPoint& point ) const
{
  bool rc = false;
  if ( 0 <= i && i < m_point_count[0] && 0 <= j && j < m_point_count[1] ) {
    point = m_point[i*m_point_stride0+j];
    rc = true;
  }
  return rc;
}

MYON_3dPoint* MYON_PointGrid::operator[](int i)
{
  return ( 0 <= i && i < m_point_count[0] ) 
         ? m_point.Array() + i*m_point_stride0 : 0;
}

const MYON_3dPoint* MYON_PointGrid::operator[](int i) const
{
  return ( 0 <= i && i < m_point_count[0] ) 
         ? m_point.Array() + i*m_point_stride0 : 0;
}

bool
MYON_PointGrid::Create(
        int point_count0,  // cv count0 (>= order0)
        int point_count1   // cv count1 (>= order1)
        )
{
  if ( point_count0 < 1 )
    return false;
  if ( point_count1 < 1 )
    return false;
  m_point_count[0] = point_count0;
  m_point_count[1] = point_count1;
  m_point_stride0 = m_point_count[1];
  m_point.Reserve(m_point_count[0]*m_point_count[1]);
  return true;
}

void MYON_PointGrid::Destroy()
{
  Initialize();
  m_point.SetCapacity(0);
}

void MYON_PointGrid::EmergencyDestroy()
{
  // call if memory used by point grid becomes invalid
  m_point_count[0] = 0;
  m_point_count[1] = 0;
  m_point_stride0 = 0;
  m_point.EmergencyDestroy();
}

void MYON_PointGrid::Initialize()
{
  m_point_count[0] = 0;
  m_point_count[1] = 0;
  m_point_stride0 = 0;
  m_point.SetCount(0);
}

MYON_PointGrid& MYON_PointGrid::operator=( const MYON_PointGrid& src )
{
  if ( this != &src ) {
    MYON_Geometry::operator=(src);
    m_point_count[0] = src.m_point_count[0];
    m_point_count[1] = src.m_point_count[1];
    m_point_stride0 = m_point_count[1];
    m_point.Reserve(PointCount());
    m_point.SetCount(PointCount());
    if ( PointCount() > 0 ) {
      // copy cv array
      if ( m_point_stride0 == src.m_point_stride0 ) {
        memcpy( m_point.Array(), src.m_point.Array(), PointCount()*sizeof(MYON_3dPoint) );
      }
      else {
        int i, j;
        for ( i = 0; i < m_point_count[0]; i++ ) for ( j = 0; j < m_point_count[1]; j++ ) {
          m_point[i*m_point_stride0+j] = src[i][j];
        }
      }
    }
  }
  return *this;
}

void MYON_PointGrid::Dump( MYON_TextLog& dump ) const
{
  dump.Print( "MYON_PointGrid size = %d X %d\n",
               m_point_count[0], m_point_count[1] );
  if ( m_point.Count() < 1 ) {
    dump.Print("  NO point array\n");
  }
  else {
    dump.PrintPointGrid( 3, false, m_point_count[0], m_point_count[1], 
                         3*m_point_stride0, 3, 
                         &m_point[0].x,
                         "  point" 
                         );
  }
}

bool MYON_PointGrid::IsValid( MYON_TextLog* text_log ) const
{
  bool rc = false;
  if ( MYON_IsValidPointGrid( 3, false, 
                            m_point_count[0], m_point_count[1], 
                            m_point_stride0*3, 3, 
                            &m_point[0].x ) ) 
  {
    if ( m_point.Count() >= m_point_stride0*m_point_count[0] )
      rc = true;
  }
  return rc;
}

bool MYON_PointGrid::GetBBox( // returns true if successful
       double* boxmin,    // minimum
       double* boxmax,    // maximum
       bool bGrowBox  // true means grow box
       ) const
{
  return MYON_GetPointGridBoundingBox( 3, 0, 
            m_point_count[0], m_point_count[1], 
            m_point_stride0*3, 3, &m_point[0].x, 
            boxmin, boxmax, bGrowBox?true:false );
}

bool MYON_PointGrid::GetTightBoundingBox(
         MYON_BoundingBox& tight_bbox,
         bool bGrowBox,
				 const MYON_Xform* xform 
         ) const
{
  if ( bGrowBox && !tight_bbox.IsValid() )
  {
    bGrowBox = false;
  }
  if ( !bGrowBox )
  {
    tight_bbox.Destroy();
  }
  
  int i;
  for ( i = 0; i < m_point_count[0]; i++ )
  {
    if ( MYON_GetPointListBoundingBox( 3, 0, m_point_count[1], 3, &m_point[i].x, tight_bbox, bGrowBox, xform ) )
      bGrowBox = true;
  }
  return bGrowBox?true:false;
}

bool MYON_PointGrid::Transform( const MYON_Xform& xform )
{
  TransformUserData(xform);
  return MYON_TransformPointGrid( 3, false, 
            m_point_count[0], m_point_count[1],
            m_point_stride0*3, 3,
            Point(0,0), 
            xform );
}

// virtual MYON_Geometry::IsDeformable() override
bool MYON_PointGrid::IsDeformable() const
{
  return true;
}

// virtual MYON_Geometry::MakeDeformable() override
bool MYON_PointGrid::MakeDeformable()
{
  return true;
}

bool MYON_PointGrid::SwapCoordinates(
      int i, int j // indices of coords to swap
      )
{
  return MYON_SwapPointGridCoordinates( 
            m_point_count[0], m_point_count[1], 
            m_point_stride0*3, 3, 
            Point(0,0), 
            i, j );

}


bool MYON_PointGrid::Write(
       MYON_BinaryArchive&  // open binary file
     ) const
{
  // TODO
  return false;
}

bool MYON_PointGrid::Read(
       MYON_BinaryArchive&  // open binary file
     )
{
  // TODO
  return false;
}

MYON::object_type MYON_PointGrid::ObjectType() const
{
  return MYON::pointset_object;
}

bool 
MYON_PointGrid::IsClosed( int dir ) const
{
  return MYON_IsPointGridClosed( 3, 0, 
                    m_point_count[0], m_point_count[1], 
                    m_point_stride0*3, 3, 
                    &m_point[0].x, dir );
}

bool
MYON_PointGrid::Reverse(int dir)
{
  return MYON_ReversePointGrid( 3, false, m_point_count[0], m_point_count[1], m_point_stride0*3, 3, Point(0,0), dir );
}

bool
MYON_PointGrid::Transpose()
{
  int i, j;
  bool rc = false;
  if ( IsValid() ) {
    // slow stupid way - can be imporved if necessary
    MYON_PointGrid t(m_point_count[1],m_point_count[0]);
    for ( i = 0; i < m_point_count[0]; i++ ) for ( j = 0; j < m_point_count[1]; j++ ) {
      t[j][i] = Point(i,j);
    }
    *this = t;
    rc = true;
  }
  return rc;
}



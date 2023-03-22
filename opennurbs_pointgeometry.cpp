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

MYON_OBJECT_IMPLEMENT( MYON_Point, MYON_Geometry, "C3101A1D-F157-11d3-BFE7-0010830122F0" );

bool MYON_Point::IsValid( MYON_TextLog* text_log ) const 
{
  bool rc = point.IsValid();
  if ( !rc && text_log )
  {
    text_log->Print("MYON_Point::point is not a valid 3d point.\n");
  }
  return rc;
}

void MYON_Point::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_Point: ");
  dump.Print(point);
  dump.Print("\n");
}

bool MYON_Point::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.Write3dmChunkVersion(1,0);
  if (rc) rc = file.WritePoint( point );
  return rc;
}

bool MYON_Point::Read( MYON_BinaryArchive& file )
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc && major_version==1) {
    // common to all 1.x versions
    rc = file.ReadPoint(point);
  }
  return rc;
}

MYON::object_type MYON_Point::ObjectType() const
{
  return MYON::point_object;
}

int MYON_Point::Dimension() const
{
  return 3;
}

bool MYON_Point::GetBBox( double* boxmin, double* boxmax, bool bGrowBox ) const
{
  return MYON_GetPointListBoundingBox( 3, 0, 1, 3, &point.x, boxmin, boxmax, bGrowBox?true:false );
}

bool MYON_Point::IsDeformable() const
{
  return true;
}

bool MYON_Point::MakeDeformable()
{
  return true;
}

bool MYON_Point::Transform( const MYON_Xform& xform )
{
  TransformUserData(xform);
  return MYON_TransformPointList(3,0,1,3,&point.x,xform);
}

bool MYON_Point::SwapCoordinates( int i, int j )
{
  return MYON_SwapPointListCoordinates( 1, 3, &point.x, i, j );
}

MYON_Point::MYON_Point() : point(0.0,0.0,0.0) 
{}

MYON_Point::MYON_Point(const MYON_Point& src) : MYON_Geometry(src), point(src.point) 
{}

MYON_Point::MYON_Point(const MYON_3dPoint& pt) : point(pt) 
{}

MYON_Point::MYON_Point(double x,double y,double z) : point(x,y,z)
{}

MYON_Point::~MYON_Point() 
{}

MYON_Point& MYON_Point::operator=(const MYON_Point& src) 
{
  if ( this != &src ) {
    MYON_Geometry::operator=(src);
    point=src.point;
  }
  return *this;
}

MYON_Point& MYON_Point::operator=(const MYON_3dPoint& pt) 
{
  point=pt;
  return *this;
}

MYON_Point::operator double*() 
{
  return &point.x;
}

MYON_Point::operator const double*() const 
{
  return &point.x;
}

MYON_Point::operator MYON_3dPoint*() 
{
  return &point;
}

MYON_Point::operator const MYON_3dPoint*() const 
{
  return &point;
}

MYON_Point::operator MYON_3dPoint&() 
{
  return point;
}

MYON_Point::operator const MYON_3dPoint&() const 
{
  return point;
}

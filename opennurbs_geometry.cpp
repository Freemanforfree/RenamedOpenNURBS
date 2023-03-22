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

MYON_OBJECT_IMPLEMENT(MYON_Geometry,MYON_Object,"4ED7D4DA-E947-11d3-BFE5-0010830122F0");

#if defined(MYON_HAS_RVALUEREF)
MYON_Geometry::MYON_Geometry( MYON_Geometry&& src ) MYON_NOEXCEPT
  : MYON_Object(std::move(src))
{}

MYON_Geometry& MYON_Geometry::operator=( MYON_Geometry&& src )
{
  MYON_Object::operator=(std::move(src));
  return *this;
}
#endif

bool MYON_Geometry::IsValid(
  MYON_TextLog*
  ) const
{
  return false;
}


MYON_BoundingBox MYON_Geometry::BoundingBox() const
{
  MYON_BoundingBox bbox;
  if ( !GetBoundingBox( bbox.m_min, bbox.m_max, false ) )
    bbox.Destroy();
  return bbox;
}

bool 
MYON_Geometry::GetBoundingBox( // returns true if successful
  MYON_BoundingBox& bbox,
  bool bGrowBox
  ) const
{
  return (0!=GetBoundingBox( bbox.m_min, bbox.m_max, bGrowBox ));
}

bool 
MYON_Geometry::GetBoundingBox( // returns true if successful
  MYON_3dPoint& boxmin,
  MYON_3dPoint& boxmax,
  bool bGrowBox
  ) const
{
  MYON_Workspace ws;
  const int dim = Dimension();
  double *bmin, *bmax;
  if ( dim <= 3 ) {
    bmin = &boxmin.x;
    bmax = &boxmax.x;
  }
  else {
    bmin = ws.GetDoubleMemory(dim*2);
    bmax = bmin+dim;
    memset( bmin, 0, 2*dim*sizeof(*bmin) );
    if ( bGrowBox ) {
      bmin[0] = boxmin.x; bmin[1] = boxmin.y; bmin[1] = boxmin.z;
      bmax[0] = boxmax.x; bmax[1] = boxmax.y; bmax[1] = boxmax.z;
    }
  }
	// Treat invalid box on input as empty
	bool invalid=false;	//input box invalid=empty
	if(bGrowBox)
		invalid =  boxmin.x>boxmax.x || boxmin.y>boxmax.y|| boxmin.z>boxmax.z;
	if(bGrowBox && invalid)
		bGrowBox=false;

  const bool rc = (0 != GetBBox( bmin, bmax, bGrowBox ));
  if ( dim > 3 ) {
    boxmin.x = bmin[0]; boxmin.y = bmin[1]; boxmin.z = bmin[2];
    boxmax.x = bmax[0]; boxmax.y = bmax[1]; boxmax.z = bmax[2];
  }
  else if ( dim <= 2 ) {
    boxmin.z = 0.0;
    boxmax.z = 0.0;
    if ( dim <= 1 ) {
      boxmin.y = 0.0;
      boxmax.y = 0.0;
    }
  }
  return rc;
}

bool MYON_Geometry::GetTightBoundingBox( 
	MYON_BoundingBox& tight_bbox, 
  bool bGrowBoxAsInt,
	const MYON_Xform* xform
  ) const
{
  //	This implementation should be overridden by classes devived
  //  from MYON_Geometry
  bool bGrowBox = (0!= bGrowBoxAsInt);
  if ( bGrowBox && !tight_bbox.IsValid() )
  {
    bGrowBox = false;
  }
  if ( !bGrowBox )
  {
    tight_bbox.Destroy();
  }

  if ( xform && !xform->IsIdentity() )
  {
    MYON_3dPointArray corners(8);
    MYON_BoundingBox world_bbox;
    if ( GetBoundingBox(world_bbox,false) )
    {
      world_bbox.GetCorners(corners);
      if ( corners.GetTightBoundingBox(tight_bbox,bGrowBox,xform) )
        bGrowBox = true;
    }
  }
  else
  {
    if ( GetBoundingBox(tight_bbox,bGrowBox) )
      bGrowBox = true;
  }

  return bGrowBox;
}

bool MYON_Geometry::SwapCoordinates(
      int i, int j        // indices of coords to swap
      )
{
  bool rc = false;
  const int dim = Dimension();
  if ( dim > 0 && dim <= 3 && i >= 0 && i < 3 && j >= 0 && j < 3 ) {
    if ( i == j ) {
      rc = true;
    }
    else {
      int k;
      MYON_Xform swapij(MYON_Xform::ZeroTransformation);
      for ( k = 0; k < 4; k++ ) {
        if ( i == k )
          swapij[k][j] = 1.0;
        else if ( j == k )
          swapij[k][i] = 1.0;
        else
          swapij[k][k] = 1.0;
      }
      rc = Transform( swapij );
    }
  }
  return rc;
}

bool MYON_Geometry::Rotate(
      double sin_angle,          // sin(angle)
      double cos_angle,          // cos(angle)
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint& center // center of rotation
      )
{
  if ( sin_angle == 0.0 && cos_angle == 1.0 )
    return true;
  MYON_Xform rot;
  rot.Rotation( sin_angle, cos_angle, axis, center );
  return (0!=Transform( rot ));
}

bool MYON_Geometry::Rotate(
      double angle,              // angle in radians
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint& center // center of rotation
      )
{
  if ( angle == 0.0 )
    return true;
  return Rotate( sin(angle), cos(angle), axis, center );
}

bool MYON_Geometry::Translate( const MYON_3dVector& delta )
{
  if ( delta.IsZero() )
    return true;
  if ( false == delta.IsValid() )
    return false;
  const MYON_Xform tr(MYON_Xform::TranslationTransformation( delta ));
  return (0!=Transform( tr ));
}

bool MYON_Geometry::Scale( double x )
{
  if ( x == 1.0 )
    return true;
  if (false == MYON_IS_VALID(x))
    return false;
  MYON_Xform s(MYON_Xform::DiagonalTransformation(x));
  return (0!=Transform( s ));
}

int MYON_Geometry::Dimension() const
{
  return 3;
}

bool MYON_Geometry::GetBBox(
  double* boxmin,
  double* boxmax,
  bool bGrowBox
  ) const
{
  return false;
}

bool MYON_Geometry::IsDeformable() const
{
  return false;
}

bool MYON_Geometry::MakeDeformable()
{
  return false;
}

void MYON_Geometry::ClearBoundingBox()
{
  // default implementation does nothing
}

bool MYON_Geometry::Transform( const MYON_Xform& xform )
{
  TransformUserData(xform);
  return true;
}

bool MYON_Geometry::HasBrepForm() const
{
  // override if specific geoemtry has brep form
  return false;
}

MYON_Brep* MYON_Geometry::BrepForm(MYON_Brep* brep) const
{
  // override if specific geoemtry has brep formw
  return nullptr;
}

MYON_COMPONENT_INDEX MYON_Geometry::ComponentIndex() const
{
  // default constructor sets
  // m_type = MYON_COMPONENT_INDEX::invalid_type and m_index = -1.
  MYON_COMPONENT_INDEX ci;
  return ci;  
}

bool MYON_Geometry::EvaluatePoint( const class MYON_ObjRef& objref, MYON_3dPoint& P ) const
{
  // virtual function default
  P = MYON_3dPoint::UnsetPoint;
  return false;
}


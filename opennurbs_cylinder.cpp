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

MYON_Cylinder::MYON_Cylinder() 
{
  height[0] = 0.0;
  height[1] = 0.0;
}

MYON_Cylinder::MYON_Cylinder(
    const MYON_Circle& c
    )
{
  Create(c);
}

MYON_Cylinder::MYON_Cylinder(
    const MYON_Circle& c,
    double h
    )
{
  Create(c,h);
}

MYON_Cylinder::~MYON_Cylinder()
{}

bool MYON_Cylinder::Create(
    const MYON_Circle& c
    )
{
  return Create( c, 0.0 );
}

bool MYON_Cylinder::Create(
    const MYON_Circle& c,
    double h
    )
{
  circle = c;
  if ( h > 0.0 ) {
    height[0] = 0.0;
    height[1] = h;
  }
  else {
    height[0] = h;
    height[1] = 0.0;
  }
  return IsValid();
}

bool MYON_Cylinder::IsValid() const
{
  return circle.IsValid();
}

bool MYON_Cylinder::IsFinite() const
{
  return height[0] != height[1];
}

const MYON_3dVector& MYON_Cylinder::Axis() const
{
  return circle.plane.zaxis;
}

const MYON_3dPoint& MYON_Cylinder::Center() const
{
  return circle.plane.origin;
}

double MYON_Cylinder::Height() const
{
  return height[1] - height[0];
}

MYON_Circle MYON_Cylinder::CircleAt( 
      double t // linear parameter
      ) const
{
  MYON_Circle c = circle;
  if ( t != 0.0 )
    c.Translate(t*circle.plane.zaxis);
  return c;
}

MYON_Line MYON_Cylinder::LineAt( 
      double s // angular parameter
      ) const
{
  MYON_3dPoint p = circle.PointAt(s);
  MYON_Line line;
  line.from = p + height[0]*circle.plane.zaxis;
  line.to   = p + height[1]*circle.plane.zaxis;
  return line;
}


MYON_3dPoint MYON_Cylinder::PointAt( double s, double t ) const
{
  return ( circle.PointAt(s) + t*circle.plane.zaxis );
}

MYON_3dPoint MYON_Cylinder::NormalAt( double s, double t ) const
{
  MYON_3dVector N = MYON_CrossProduct( circle.TangentAt(s), circle.plane.zaxis );
  N.Unitize();
  return N;
}

// returns parameters of point on cylinder that is closest to given point
bool MYON_Cylinder::ClosestPointTo( 
       MYON_3dPoint point, 
       double* s, // angular parameter
       double* t  // linear parameter
       ) const
{
  bool rc = true;
  //const MYON_3dVector v = point - circle.plane.origin;
  double h = (point - circle.plane.origin)*circle.plane.zaxis;
  if ( s )
    rc = circle.ClosestPointTo( point - h*circle.plane.zaxis, s );
  if ( t ) {
    if ( height[0] < height[1] ) {
      if ( h < height[0] ) h = height[0]; else if ( h > height[1] ) h = height[1];
    }
    else if ( height[0] > height[1] ) {
      if ( h > height[0] ) h = height[0]; else if ( h < height[1] ) h = height[1];
    }
    *t = h;
  }
  return rc;
}

// returns point on cylinder that is closest to given point
MYON_3dPoint MYON_Cylinder::ClosestPointTo( 
       MYON_3dPoint point
       ) const
{
  double s, t;
  ClosestPointTo( point, &s, &t );
  return PointAt( s, t );
}

// rotate plane about its origin
bool MYON_Cylinder::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate( sin_angle, cos_angle, axis, circle.plane.origin );
}

bool MYON_Cylinder::Rotate(
      double angle,               // angle in radians
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate( sin(angle), cos(angle), axis, circle.plane.origin );
}

// rotate plane about a point and axis
bool MYON_Cylinder::Rotate(
      double sin_angle,               // sin(angle)
      double cos_angle,               // cos(angle)
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint& point  // center of rotation
      )
{
  return circle.Rotate( sin_angle, cos_angle, axis, point );
}

bool MYON_Cylinder::Rotate(
      double angle,              // angle in radians
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint&  point  // center of rotation
      )
{
  return Rotate( sin(angle), cos(angle), axis, point );
}

bool MYON_Cylinder::Translate(
      const MYON_3dVector& delta
      )
{
  return circle.Translate( delta );
}

int MYON_Cylinder::GetNurbForm( MYON_NurbsSurface& s ) const
{
  int rc = 0;
  if ( IsValid() && height[0] != height[1] ) {
    MYON_NurbsCurve n0, n1;
    int i;
    MYON_Circle c0 = CircleAt(height[0]);
    MYON_Circle c1 = CircleAt(height[1]);


    if ( height[0] <= height[1] ) {
      c0.GetNurbForm(n0);
      c1.GetNurbForm(n1);
    }
    else {
      c0.GetNurbForm(n1);
      c1.GetNurbForm(n0);
    }

    if (    n0.m_dim != n1.m_dim 
         || n0.m_is_rat != n1.m_is_rat
         || n0.m_order != n1.m_order 
         || n0.m_cv_count != n1.m_cv_count )
      return 0;

    s.Create(3,true, n0.m_order, 2, n0.m_cv_count, 2 );
    if ( height[0] <= height[1] ) {
      s.m_knot[1][0] = height[0];
      s.m_knot[1][1] = height[1]; 
    }
    else {
      s.m_knot[1][0] = height[1];
      s.m_knot[1][1] = height[0]; 
    }

    for ( i = 0; i < n0.KnotCount(); i++ )
      s.m_knot[0][i] = n0.m_knot[i];

    for ( i = 0; i < n0.m_cv_count; i++ ) {
      s.SetCV(i,0,MYON::homogeneous_rational,n0.CV(i));
      s.SetCV(i,1,MYON::homogeneous_rational,n1.CV(i));
    }
    rc = 2;
  }
  return rc;
}

MYON_RevSurface* MYON_Cylinder::RevSurfaceForm( MYON_RevSurface* srf ) const
{
  if ( srf )
    srf->Destroy();
  MYON_RevSurface* pRevSurface = nullptr;
  if ( IsFinite() && IsValid() )
  {
    MYON_Line line;
    line.from = PointAt(0.0,height[0]);
    line.to = PointAt(0.0,height[1]);
    MYON_Interval h(height[0],height[1]); // h = evaluation domain for line (must be increasing)
    if ( h.IsDecreasing() )
      h.Swap();
    MYON_LineCurve* line_curve = new MYON_LineCurve( line, h[0], h[1] );
    if ( srf )
      pRevSurface = srf;
    else
      pRevSurface = new MYON_RevSurface();
    pRevSurface->m_angle.Set(0.0,2.0*MYON_PI);
    pRevSurface->m_t = pRevSurface->m_angle;
    pRevSurface->m_curve = line_curve;
    pRevSurface->m_axis.from = circle.plane.origin;
    pRevSurface->m_axis.to = circle.plane.origin + circle.plane.zaxis;
    pRevSurface->m_bTransposed = false;
    MYON_Circle c0(circle);
    c0.Translate(height[0]*circle.plane.zaxis);
    MYON_Circle c1(circle);
    c1.Translate(height[1]*circle.plane.zaxis);
    pRevSurface->m_bbox = c0.BoundingBox();
    pRevSurface->m_bbox.Union(c1.BoundingBox());
  }
  return pRevSurface;
}

/*
// obsolete use MYON_BrepCylinder 
MYON_Brep* MYON_Cylinder::BrepForm( MYON_Brep* brep ) const
{
  if ( brep )
    brep->Destroy();
  MYON_Brep* pBrep = 0;
  MYON_RevSurface* pRevSurface = RevSurfaceForm();
  if ( pRevSurface )
  {
    if ( brep )
      pBrep = brep;
    else
      pBrep = new MYON_Brep();
    if ( !pBrep->Create(pRevSurface) ) 
    {
      if ( !brep )
        delete pBrep;
      pBrep = 0;
      if (pRevSurface)
      {
        delete pRevSurface;
        pRevSurface = 0;
      }
    }
    else 
    {
      // add caps
      for ( int capcount = 0; capcount < 2; capcount++ )
      {
        // capcount = 0 for bottom cap and 1 for top cap
        MYON_Circle circle = CircleAt(height[capcount]);
        if ( capcount == 0 )
          circle.Reverse();
        double radius = circle.radius;
        MYON_NurbsSurface* pCapSurface = MYON_NurbsSurfaceQuadrilateral( 
          circle.plane.PointAt(-radius,-radius),
          circle.plane.PointAt(+radius,-radius),
          circle.plane.PointAt(+radius,+radius),
          circle.plane.PointAt(-radius,+radius)
          );
        pCapSurface->m_knot[0][0] = -fabs(radius);
        pCapSurface->m_knot[0][1] =  fabs(radius);
        pCapSurface->m_knot[1][0] = pCapSurface->m_knot[0][0];
        pCapSurface->m_knot[1][1] = pCapSurface->m_knot[0][1];
        circle.Create( MYON_xy_plane, MYON_3dPoint::Origin, radius );
        MYON_NurbsCurve* c2 = new MYON_NurbsCurve();
        circle.GetNurbForm(*c2);
        c2->ChangeDimension(2);

        pBrep->m_S.Append(pCapSurface);
        pBrep->m_C2.Append(c2);
        MYON_BrepFace& cap = pBrep->NewFace( pBrep->m_S.Count()-1 );
        MYON_BrepLoop& loop = pBrep->NewLoop( MYON_BrepLoop::outer, cap );
        MYON_BrepEdge& edge = pBrep->m_E[capcount?0:2];
        MYON_BrepTrim& trim = pBrep->NewTrim( edge, true, loop, pBrep->m_C2.Count()-1 );
        for ( int eti = 0; eti < edge.m_ti.Count(); eti++ )
          pBrep->m_T[ edge.m_ti[eti] ].m_type = MYON_BrepTrim::mated;
        trim.m_tolerance[0] = 0.0;
        trim.m_tolerance[1] = 0.0;
        trim.m_pbox.m_min.x = -radius;
        trim.m_pbox.m_min.y = -radius;
        trim.m_pbox.m_min.z = 0.0;
        trim.m_pbox.m_max.x = radius;
        trim.m_pbox.m_max.y = radius;
        trim.m_pbox.m_max.z = 0.0;
        loop.m_pbox = trim.m_pbox;
        pBrep->SetTrimTypeFlags(trim);
        pBrep->SetTrimIsoFlags(trim);
      }
      if ( !pBrep->IsValid() )
      {
        if (brep)
          brep->Destroy();
        else
          delete pBrep;
        pBrep = 0;
      }
    }
  }
  return pBrep;
}
*/


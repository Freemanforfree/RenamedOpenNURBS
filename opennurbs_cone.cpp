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

MYON_Cone::MYON_Cone() 
{
  height = 0.0;
}

MYON_Cone::MYON_Cone(
    const MYON_Plane& p,
    double h,
    double r
    )
{
  Create(p,h,r);
}

MYON_Cone::~MYON_Cone()
{}


bool MYON_Cone::Create(
    const MYON_Plane& p,
    double h,
    double r
    )
{
  plane = p;
  height = h;
  radius = r;
  return IsValid();
}

bool MYON_Cone::IsValid() const
{
  return (plane.IsValid() && height != 0.0 && radius != 0.0);
}


const MYON_3dVector& MYON_Cone::Axis() const
{
  return plane.zaxis;
}

const MYON_3dPoint& MYON_Cone::ApexPoint() const
{
  return plane.origin;
}

MYON_3dPoint MYON_Cone::BasePoint() const
{
  return plane.origin + height*plane.zaxis;
}

double MYON_Cone::AngleInRadians() const
{
  return height == 0.0 ? (radius!=0.0?MYON_PI:0.0) : atan(radius/height);
}

double MYON_Cone::AngleInDegrees() const
{
  return 180.0*AngleInRadians()/MYON_PI;
}

MYON_Circle MYON_Cone::CircleAt( 
      double height_parameter
      ) const
{
  MYON_Circle c(plane,radius);
  c.Translate(height_parameter*plane.zaxis);
  if ( height != 0.0 )
    c.radius *= height_parameter/height;
  else if (height_parameter==0.0)
    c.radius = 0.0;
  return c;
}

MYON_Line MYON_Cone::LineAt( 
      double radial_parameter
      ) const
{
  return MYON_Line(PointAt(radial_parameter,height),ApexPoint());
}


MYON_3dPoint MYON_Cone::PointAt( double radial_parameter, double height_parameter ) const
{
  double r;
  if ( height != 0.0 )
    r = (radius/height)*height_parameter;
  else
    r = (height_parameter == 0.0)?0.0:radius;
  return plane.PointAt(r*cos(radial_parameter),r*sin(radial_parameter)) + height_parameter*plane.zaxis;
}

MYON_3dVector MYON_Cone::NormalAt( double radial_parameter, double height_parameter ) const
{
  double s = sin(radial_parameter);
  double c = cos(radial_parameter);
  if ( radius<0.) {
    c = -c;
    s = -s;
  }
  MYON_3dVector ds = c*plane.yaxis - s*plane.xaxis;
  MYON_3dVector N = MYON_CrossProduct( ((radius<0.0)?-ds:ds),
                                   plane.PointAt(radius*c,radius*s,height) - plane.origin
                                   );
  N.Unitize();
  return N;
}

bool MYON_Cone::Transform( const MYON_Xform& xform )
{
  MYON_Circle xc(plane,radius);
  bool rc = xc.Transform(xform);
  if (rc)
  {
    MYON_3dPoint xH = xform*(plane.origin + height*plane.zaxis);
    double xh = (xH-xc.plane.origin)*xc.plane.zaxis;
    plane = xc.plane;
    radius = xc.radius;
    height = xh;
  }
  return rc;
}

bool MYON_Cone::ClosestPointTo( 
          MYON_3dPoint point, 
          double* radial_parameter,
          double* height_parameter
       ) const
{
  // untested code

  bool rc = false;

  MYON_3dVector v = (point-plane.origin);
  double x = v*plane.xaxis;
  double y = v*plane.yaxis;
  double z = v*plane.zaxis;

  if ( radial_parameter )
  {
    double a = ( 0.0 == y && 0.0 == x ) ? 0.0 : atan2(y,x);

    if (a > 2.0*MYON_PI )
    {
      a -= 2.0*MYON_PI;
    }
    
    if (a < 0.0 )
    {
      a += 2.0*MYON_PI;
    }

    *radial_parameter = a;
  }

  if (height_parameter)
  {
    point.x -= plane.origin.x;
    point.y -= plane.origin.y;
    point.z -= plane.origin.z;
    v.x = x;
    v.y = y;
    v.z = 0.0;
    v.Unitize();
    v.x *= radius;
    v.y *= radius;
    MYON_Line line(MYON_3dPoint::Origin, v.x*plane.xaxis + v.y*plane.yaxis + height*plane.zaxis );
    rc = line.ClosestPointTo(point,&z);
    if (rc)
    {
      *height_parameter = z*height;
    }
  }

  return rc;
}

// returns point on cylinder that is closest to given point
MYON_3dPoint MYON_Cone::ClosestPointTo( 
       MYON_3dPoint point
       ) const
{
  // untested code

  MYON_3dVector v = (point-plane.origin);
  double x = v*plane.xaxis;
  double y = v*plane.yaxis;
  //double z = v*plane.zaxis;

  point.x -= plane.origin.x;
  point.y -= plane.origin.y;
  point.z -= plane.origin.z;
  v.x = x;
  v.y = y;
  v.z = 0.0;
  v.Unitize();
  v.x *= radius;
  v.y *= radius;
  MYON_Line line(MYON_3dPoint::Origin, v.x*plane.xaxis + v.y*plane.yaxis + height*plane.zaxis );
  return line.ClosestPointTo(point);
}

bool MYON_Cone::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis_of_rotation
      )
{
  return Rotate( sin_angle, cos_angle, axis_of_rotation, plane.origin );
}

bool MYON_Cone::Rotate(
      double angle,
      const MYON_3dVector& axis_of_rotation
      )
{
  return Rotate( sin(angle), cos(angle), axis_of_rotation, plane.origin );
}

// rotate plane about a point and axis
bool MYON_Cone::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis_of_rotation,
      const MYON_3dPoint& center_of_rotation
      )
{
  return plane.Rotate( sin_angle, cos_angle, axis_of_rotation, center_of_rotation );
}

bool MYON_Cone::Rotate(
      double angle,              // angle in radians
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint&  point  // center of rotation
      )
{
  return Rotate( sin(angle), cos(angle), axis, point );
}

bool MYON_Cone::Translate(
      const MYON_3dVector& delta
      )
{
  return plane.Translate( delta );
}

int MYON_Cone::GetNurbForm( MYON_NurbsSurface& s ) const
{
  int rc = 0;
  if ( IsValid() ) {
    MYON_Circle c = CircleAt(height);
    MYON_NurbsCurve n;
    c.GetNurbForm(n);
    MYON_3dPoint apex = ApexPoint();
    MYON_4dPoint cv;
    int i, j0, j1;

    s.Create(3,true,3,2,9,2);
    for ( i = 0; i < 10; i++ )
      s.m_knot[0][i] = n.m_knot[i];

    if ( height >= 0.0 ) {
      s.m_knot[1][0] = 0.0;
      s.m_knot[1][1] = height; 
      j0 = 0;
      j1 = 1;
    }
    else {
      s.m_knot[1][0] = height;
      s.m_knot[1][1] = 0.0;
      j0 = 1;
      j1 = 0;
    }

    for ( i = 0; i < 9; i++ ) {
      cv = n.CV(i);
      s.SetCV(i, j1, MYON::homogeneous_rational, &cv.x );
      cv.x = apex.x*cv.w;
      cv.y = apex.y*cv.w;
      cv.z = apex.z*cv.w;
      s.SetCV(i, j0, cv);
    }
    rc = 2;
  }
  return rc;
}

MYON_RevSurface* MYON_Cone::RevSurfaceForm( MYON_RevSurface* srf ) const
{
  if ( srf )
    srf->Destroy();
  MYON_RevSurface* pRevSurface = nullptr;
  if ( IsValid() )
  {
    MYON_Line line;
    MYON_Interval line_domain;
    if ( height >= 0.0 )
      line_domain.Set(0.0,height);
    else
      line_domain.Set(height,0.0);
    line.from = PointAt(0.0,line_domain[0]);
    line.to = PointAt(0.0,line_domain[1]);
    MYON_LineCurve* line_curve = new MYON_LineCurve( line, line_domain[0], line_domain[1] );
    if ( srf )
      pRevSurface = srf;
    else
      pRevSurface = new MYON_RevSurface();
    pRevSurface->m_angle.Set(0.0,2.0*MYON_PI);
    pRevSurface->m_t = pRevSurface->m_angle;
    pRevSurface->m_curve = line_curve;
    pRevSurface->m_axis.from = plane.origin;
    pRevSurface->m_axis.to = plane.origin + plane.zaxis;
    pRevSurface->m_bTransposed = false;
    pRevSurface->m_bbox.m_min = plane.origin;
    pRevSurface->m_bbox.m_max = plane.origin;
    pRevSurface->m_bbox.Union(CircleAt(height).BoundingBox());
  }
  return pRevSurface;
}

/*
// obsolete use MYON_BrepCone
MYON_Brep* MYON_Cone::BrepForm( MYON_Brep* brep ) const
{
  MYON_Brep* pBrep = 0;
  if ( brep )
    brep->Destroy();
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
      if ( !pRevSurface )
      {
        delete pRevSurface;
        pRevSurface = 0;
      }
    }
    else 
    {
      // add cap
      MYON_Circle circle = CircleAt(height);
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
      MYON_BrepEdge& edge = pBrep->m_E[1];
      MYON_BrepTrim& trim = pBrep->NewTrim( edge, true, loop, pBrep->m_C2.Count()-1 );
      trim.m_tolerance[0] = 0.0;
      trim.m_tolerance[1] = 0.0;
      trim.m_pbox.m_min.x = -radius;
      trim.m_pbox.m_min.y = -radius;
      trim.m_pbox.m_min.z = 0.0;
      trim.m_pbox.m_max.x = radius;
      trim.m_pbox.m_max.y = radius;
      trim.m_pbox.m_max.z = 0.0;
      loop.m_pbox = trim.m_pbox;
      pBrep->SetTrimIsoFlags(trim);
      for ( int eti = 0; eti < edge.m_ti.Count(); eti++ )
        pBrep->m_T[ edge.m_ti[eti] ].m_type = MYON_BrepTrim::mated;
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

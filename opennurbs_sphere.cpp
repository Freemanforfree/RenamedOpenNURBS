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

MYON_Sphere::MYON_Sphere() : radius(0.0)
{}

MYON_Sphere::MYON_Sphere( const MYON_3dPoint& center, double r)
{
  Create(center,r);
}

MYON_Sphere::~MYON_Sphere()
{}

bool MYON_Sphere::IsValid() const
{
  return ( MYON_IsValid(radius) && radius > 0.0 && plane.IsValid() ) ? true : false;
}

bool MYON_Sphere::Create( const MYON_3dPoint& center, double r )
{
  plane = MYON_xy_plane;
  plane.origin = center;
  plane.UpdateEquation();
  radius = r;
  return (r > 0.0) ? true : false;
}

MYON_3dPoint MYON_Sphere::PointAt(double longitude, double latitude) const
{
  return radius*NormalAt(longitude,latitude) + plane.origin;
}

MYON_3dVector MYON_Sphere::NormalAt(double longitude, double latitude) const
{
  return cos(latitude)*(cos(longitude)*plane.xaxis + sin(longitude)*plane.yaxis) + sin(latitude)*plane.zaxis;
}

MYON_Circle MYON_Sphere::LatitudeRadians(double a) const
{
  return MYON_Circle(PointAt(0.0,a),PointAt(0.5*MYON_PI,a),PointAt(MYON_PI,a));
}

MYON_Circle MYON_Sphere::LatitudeDegrees(double a) const
{
  return LatitudeRadians(a*MYON_PI/180.0);
}

MYON_Circle MYON_Sphere::LongitudeRadians(double a) const
{
  return MYON_Circle(PointAt(a,0.0),NorthPole(),PointAt(a+MYON_PI,0.0));
}

MYON_Circle MYON_Sphere::LongitudeDegrees(double a) const
{
  return LongitudeRadians(a*MYON_PI/180.0);
}

MYON_3dPoint MYON_Sphere::Center() const
{
  return plane.origin;
}

MYON_3dPoint MYON_Sphere::NorthPole() const
{
  return PointAt(0.0, 0.5*MYON_PI);
}

MYON_3dPoint MYON_Sphere::SouthPole() const
{
  return PointAt(0.0, -0.5*MYON_PI);
}

double MYON_Sphere::Radius() const
{
  return radius;
}

double MYON_Sphere::Diameter() const
{
  return 2.0*radius;
}

bool MYON_Sphere::ClosestPointTo( 
       MYON_3dPoint point, 
       double* longitude,
       double* latitude
       ) const
{
  bool rc = true;
  MYON_3dVector v = point - plane.origin;
  double h = v*plane.zaxis;
  double x = v*plane.xaxis;
  double y = v*plane.yaxis;
  double r = 1.0;
  if ( x == 0.0 && y == 0.0 ) {
    if ( longitude )
      *longitude = 0.0;
    if ( latitude )
      *latitude = (h>=0.0) ? 0.5*MYON_PI : -0.5*MYON_PI;
    if ( h == 0.0 )
      rc = false;
  }
  else {
    if ( fabs(x) >= fabs(y) ) {
      r = y/x;
      r = fabs(x)*sqrt(1.0+r*r);
    }
    else {
      r = x/y;
      r = fabs(y)*sqrt(1.0+r*r);
    }
    if ( longitude ) {
      *longitude = atan2(y,x);
      if ( *longitude < 0.0 )
        *longitude += 2.0*MYON_PI;
      if ( *longitude < 0.0 || *longitude >= 2.0*MYON_PI)
        *longitude = 0.0;
    }
    if ( latitude )
      *latitude = atan(h/r);
  }
  return rc;
}

MYON_BoundingBox MYON_Sphere::BoundingBox() const
{
  MYON_BoundingBox bbox;
  double r = Radius();
  bbox.m_min = Center();
  bbox.m_max = bbox.m_min;
  bbox.m_min.x -= r;
  bbox.m_min.y -= r;
  bbox.m_min.z -= r;
  bbox.m_max.x += r;
  bbox.m_max.y += r;
  bbox.m_max.z += r;
  return bbox;
}

// returns point on cylinder that is closest to given point
MYON_3dPoint MYON_Sphere::ClosestPointTo( MYON_3dPoint point ) const
{
  MYON_3dVector v = point - plane.origin;
  v.Unitize();
  return plane.origin + radius*v;
}


// rotate cylinder about its origin
bool MYON_Sphere::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate(sin_angle, cos_angle, axis, plane.origin );
}

bool MYON_Sphere::Rotate(
      double angle,            // angle in radians
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate(sin(angle), cos(angle), axis, plane.origin );
}

// rotate cylinder about a point and axis
bool MYON_Sphere::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint&  point // center of rotation
      )
{
  return plane.Rotate( sin_angle, cos_angle, axis, point );
}

bool MYON_Sphere::Rotate(
      double angle,             // angle in radians
      const MYON_3dVector& axis,  // axis of rotation
      const MYON_3dPoint&  point  // center of rotation
      )
{
  return Rotate(sin(angle),cos(angle),axis,point);
}

bool MYON_Sphere::Translate(
      const MYON_3dVector& delta
      )
{
  return plane.Translate(delta);
}


bool MYON_Sphere::Transform( const MYON_Xform& xform )
{
  MYON_Circle xc(plane,radius);
  bool rc = xc.Transform(xform);
  if (rc)
  {
    plane = xc.plane;
    radius = xc.radius;
  }
  return rc;
}

int MYON_Sphere::GetNurbForm( MYON_NurbsSurface& s ) const
{
  int rc = 0;
  if ( IsValid() ) {
    s.Create(3,true,3,3,9,5);
    s.m_knot[0][0] = s.m_knot[0][1] = 0.0;
    s.m_knot[0][2] = s.m_knot[0][3] = 0.5*MYON_PI;
    s.m_knot[0][4] = s.m_knot[0][5] = MYON_PI;
    s.m_knot[0][6] = s.m_knot[0][7] = 1.5*MYON_PI;
    s.m_knot[0][8] = s.m_knot[0][9] = 2.0*MYON_PI;

    s.m_knot[1][0] = s.m_knot[1][1] = -0.5*MYON_PI;
    s.m_knot[1][2] = s.m_knot[1][3] = 0.0;
    s.m_knot[1][4] = s.m_knot[1][5] = 0.5*MYON_PI;

    MYON_4dPoint* CV = (MYON_4dPoint*)s.m_cv;
    const MYON_3dVector x = radius*plane.xaxis;
    const MYON_3dVector y = radius*plane.yaxis;
    const MYON_3dVector z = radius*plane.zaxis;

    MYON_3dPoint p[9] = {plane.origin+x,
                       plane.origin+x+y,
                       plane.origin+y,
                       plane.origin-x+y,
                       plane.origin-x,
                       plane.origin-x-y,
                       plane.origin-y,
                       plane.origin+x-y,
                       plane.origin+x};

    const double w = 1.0/sqrt(2.0);
    double w13;
    int i;
    MYON_4dPoint southpole = plane.origin - z;
    MYON_4dPoint northpole = plane.origin + z;
    for ( i = 0; i < 8; i++ ) {
      CV[5*i  ] = southpole;
      CV[5*i+1] = p[i] - z;
      CV[5*i+2] = p[i];
      CV[5*i+3] = p[i] + z;
      CV[5*i+4] = northpole;

      if ( i%2) {
        CV[5*i  ].x *= w;
        CV[5*i  ].y *= w;
        CV[5*i  ].z *= w;
        CV[5*i  ].w = w;
        CV[5*i+2].x *= w;
        CV[5*i+2].y *= w;
        CV[5*i+2].z *= w;
        CV[5*i+2].w = w;
        CV[5*i+4].x *= w;
        CV[5*i+4].y *= w;
        CV[5*i+4].z *= w;
        CV[5*i+4].w = w;
        w13 = 0.5;
      }
      else {
        w13 = w;
      }
      CV[5*i+1].x *= w13;
      CV[5*i+1].y *= w13;
      CV[5*i+1].z *= w13;
      CV[5*i+1].w  = w13;

      CV[5*i+3].x *= w13;
      CV[5*i+3].y *= w13;
      CV[5*i+3].z *= w13;
      CV[5*i+3].w  = w13;
    }
    CV[40] = CV[0];
    CV[41] = CV[1];
    CV[42] = CV[2];
    CV[43] = CV[3];
    CV[44] = CV[4];
    rc = 2;
  }
  return rc;
}

MYON_RevSurface* MYON_Sphere::RevSurfaceForm( MYON_RevSurface* srf ) const
{
  return RevSurfaceForm(false,srf);
}

MYON_RevSurface* MYON_Sphere::RevSurfaceForm( 
  bool bArcLengthParameterization,
  MYON_RevSurface* srf 
  ) const
{
  if ( srf )
    srf->Destroy();
  MYON_RevSurface* pRevSurface = nullptr;
  if ( IsValid() )
  {
    MYON_Arc arc;
    arc.plane.origin = plane.origin;
    arc.plane.xaxis = -plane.zaxis;
    arc.plane.yaxis =  plane.xaxis;
    arc.plane.zaxis = -plane.yaxis;
    arc.plane.UpdateEquation();
    arc.radius = radius;
    arc.SetAngleRadians(MYON_PI);
    MYON_ArcCurve* arc_curve = new MYON_ArcCurve( arc, -0.5*MYON_PI, 0.5*MYON_PI );
    if ( srf )
      pRevSurface = srf;
    else
      pRevSurface = new MYON_RevSurface();
    pRevSurface->m_angle.Set(0.0,2.0*MYON_PI);
    pRevSurface->m_t = pRevSurface->m_angle;
    pRevSurface->m_curve = arc_curve;
    pRevSurface->m_axis.from = plane.origin;
    pRevSurface->m_axis.to = plane.origin + plane.zaxis;
    pRevSurface->m_bTransposed = false;
    pRevSurface->m_bbox.m_min = plane.origin;
    pRevSurface->m_bbox.m_min.x -= radius;
    pRevSurface->m_bbox.m_min.y -= radius;
    pRevSurface->m_bbox.m_min.z -= radius;
    pRevSurface->m_bbox.m_max = plane.origin;
    pRevSurface->m_bbox.m_max.x += radius;
    pRevSurface->m_bbox.m_max.y += radius;
    pRevSurface->m_bbox.m_max.z += radius;
    if ( bArcLengthParameterization )
    {
      double r = fabs(radius);
      if ( !(r > MYON_SQRT_EPSILON) )
        r = 1.0;
      r *= MYON_PI;
      pRevSurface->SetDomain(0,0.0,2.0*r);
      pRevSurface->SetDomain(1,-0.5*r,0.5*r);
    }
  }
  return pRevSurface;
}



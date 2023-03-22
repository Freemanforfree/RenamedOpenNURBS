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

MYON_Ellipse::MYON_Ellipse() 
{
  radius[0] = radius[1] = 0.0;
}


MYON_Ellipse::MYON_Ellipse(
    const MYON_Plane& p,
    double rx, double ry
    )
{
  Create(p,rx,ry);
}


MYON_Ellipse::MYON_Ellipse(
    const MYON_Circle& c
    )
{
  Create(c);
}

MYON_Ellipse::~MYON_Ellipse()
{}

MYON_Ellipse& MYON_Ellipse::operator=(const MYON_Circle& c)
{
  Create( c );
  return *this;
}

bool MYON_Ellipse::Create( const MYON_Plane& p, double rx, double ry )
{
  plane = p;
  radius[0] = rx;
  radius[1] = ry;
  return IsValid();
}

bool MYON_Ellipse::Create( const MYON_Circle& c )
{
  return Create( c.Plane(), c.Radius(), c.Radius() );
}

bool MYON_Ellipse::IsValid() const
{
  return (plane.IsValid() && radius[0] > MYON_ZERO_TOLERANCE && radius[1] > MYON_ZERO_TOLERANCE) ? true : false;
}

bool MYON_Ellipse::IsCircle() const
{
  double r0 = radius[0];
  return ( MYON_IsValid(r0) && fabs(r0-radius[1]) <= fabs(r0)*MYON_ZERO_TOLERANCE && IsValid() ) ? true : false;
}

double MYON_Ellipse::Radius( int i ) const
{
  return radius[(i)?1:0];
}

const MYON_3dPoint& MYON_Ellipse::Center() const
{
  return plane.origin;
}

double MYON_Ellipse::FocalDistance() const
{
  int i = (fabs(radius[0]) >= fabs(radius[1])) ? 0 : 1;
  const double a = fabs(radius[i]);
  const double b = a > 0.0 ? fabs(radius[1-i])/a : 0.0;
  return a*sqrt(1.0 - b*b);
}

bool MYON_Ellipse::GetFoci( MYON_3dPoint& F1, MYON_3dPoint& F2 ) const
{
  const double f = FocalDistance();
  const MYON_3dVector& majorAxis = (radius[0] >= radius[1]) ? plane.xaxis : plane.yaxis;
  F1 = plane.origin + f*majorAxis;
  F2 = plane.origin - f*majorAxis;
  return true;
}

const MYON_3dVector& MYON_Ellipse::Normal() const
{
  return plane.zaxis;
}

const MYON_Plane& MYON_Ellipse::Plane() const
{
  return plane;
}

MYON_3dPoint MYON_Ellipse::PointAt( double t ) const
{
  return plane.PointAt( cos(t)*radius[0], sin(t)*radius[1] );
}

MYON_3dVector MYON_Ellipse::DerivativeAt( 
                 int d, // desired derivative ( >= 0 )
                 double t // parameter
                 ) const
{
  double r0 = radius[0];
  double r1 = radius[1];
  switch ( std::abs(d) % 4 )
  {
  case 0:
    r0 *=  cos(t);
    r1 *=  sin(t);
    break;
  case 1:
    r0 *= -sin(t);
    r1 *=  cos(t);
    break;
  case 2:
    r0 *= -cos(t);
    r1 *= -sin(t);
    break;
  case 3:
    r0 *=  sin(t);
    r1 *= -cos(t);
    break;
  }
  return ( r0*plane.xaxis + r1*plane.yaxis );
}

MYON_3dVector MYON_Ellipse::TangentAt( 
                 double t // parameter
                 ) const
{
  MYON_3dVector T = DerivativeAt( 1, t );
  T.Unitize();
  return T;
}

MYON_3dVector MYON_Ellipse::CurvatureAt( 
                 double t // parameter
                 ) const
{
  MYON_3dVector T, K;
  MYON_EvCurvature(DerivativeAt( 1, t ),DerivativeAt( 2, t ),T,K);
  return K;
}

static int distSqToEllipse(void* p, double t, double* f, double* df )
{
  // used in call to TL_NRdbrent().
  double dx, dy, st, ct;
  const double* a = (const double*)p;
  // a[0], a[1] = x/y radii of 2d ellipse
  // (a[2],a[3]) = 2d point
  // f(t) = distance squared from ellipse(t) to 2d point
  ct = cos(t);
  st = sin(t);
  dx = ct*a[0] - a[2];
  dy = st*a[1] - a[3];
  if ( f ) {
    *f = dx*dx + dy*dy;
  }
  if ( df ) {
    *df = 2.0*(dy*a[1]*ct - dx*a[0]*st);
  }
  return 0;
}

#if defined(MYON_COMPILER_MSC)
// Disable the MSC /W4 warning
//   C4127: conditional expression is constant
// on the line
//   for(...; true; ... )
//
// This source code is used on many compilers and
// I do not trust all of them to get for(..;;...)
// right.
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4127 ) 
#endif

bool MYON_Ellipse::ClosestPointTo( const MYON_3dPoint& point, double* t ) const
{
  bool rc = true;
  if ( t ) {
    MYON_2dPoint uv;
    rc = plane.ClosestPointTo( point, &uv.x, &uv.y );
    if ( uv.x == 0.0 ) {
      if ( uv.y == 0.0 ) {
        *t = (radius[0] <= radius[1]) ? 0.0 : 0.5*MYON_PI;
        return true;
      }
      if ( uv.y >= radius[1] ) {
        *t = 0.5*MYON_PI;
        return true;
      }
      if ( uv.y <= -radius[1] ) {
        *t = 1.5*MYON_PI;
        return true;
      }
    }
    else if ( uv.y == 0.0 ) {
      if ( uv.x >= radius[0] ) {
        *t = 0.0;
        return true;
      }
      if ( uv.x <= -radius[0] ) {
        *t = MYON_PI;
        return true;
      }
    }
    {
      // use circluar approximation to get a seed value
      double t0, t1;
      *t = atan2( uv.y, uv.x );
      if ( *t < 0.0 )
      {
        *t += 2.0*MYON_PI;
        if ( 2.0*MYON_PI <= *t)
        {
          // == happens when atan2() <= 0.5*MYON_EPSILON*2.0*PI
          *t = 0.0;
        }
      }
      if ( radius[0] != radius[1] ) {
        // set limits for search
        if ( uv.x >= 0.0 ) {
          if ( uv.y >= 0.0 ) {
            // search quadrant I
            t0 = 0.0;
            t1 = 0.5*MYON_PI;
          }
          else {
            // search quadrant IV
            t0 = 1.5*MYON_PI;
            t1 = 2.0*MYON_PI;
          }
        }
        else {
          if ( uv.y >= 0.0 ) {
            // search quadrant II
            t0 = 0.5*MYON_PI;
            t1 = MYON_PI;
          }
          else {
            // search quadrant III
            t0 = MYON_PI;
            t1 = 1.5*MYON_PI;
          }
        }

        // solve for closest point using Brent's algorithm
        {
          // 6 October 2003 Dale Lear:
          //    Fixed several serious bugs here.
          // get seed value appropriate for Brent
          double p[4], et, d0, d1, dt;
          int i;
          p[0] = radius[0];
          p[1] = radius[1];
          p[2] = uv.x;
          p[3] = uv.y;
          et = *t;
          if ( et <= t0 )
            et = 0.9*t0 + 0.1*t1;
          else if ( et >= t1 )
            et = 0.9*t1 + 0.1*t0;
          distSqToEllipse( p, t0, &d0, nullptr );
          distSqToEllipse( p, t1, &d1, nullptr );
          if ( d0 == 0.0 ) {
            *t = (t0 == 2.0*MYON_PI) ? 0.0 : t0;
            return true;
          }
          if ( d1 == 0.0 ) {
            *t = (t1 == 2.0*MYON_PI) ? 0.0 : t1;
            return true;
          }
          if ( d0 > d1 ) {
            dt = t0; t0 = t1; t1 = dt;
            dt = d0; d0 = d1; d1 = dt;
          }
          *t = (t0 == 2.0*MYON_PI) ? 0.0 : t0;
          for ( i = 0; true; i++ ) {
            distSqToEllipse( p, et, &dt, nullptr );
            if ( dt < d0 )
            {
              *t = (et >= 2.0*MYON_PI) ? 0.0 : et;
              break;
            }
            if ( i >= 100 ) 
            {
              MYON_3dPoint E0 = PointAt(t0);
              if (    sqrt(d0) <= MYON_ZERO_TOLERANCE 
                   || sqrt(d0) <= MYON_SQRT_EPSILON*E0.DistanceTo(Center()) 
                   )
              {
                // Could not find a seed value for dbrent, 
                // but t0 is pretty close.
                return true;
              }
              MYON_3dVector T = TangentAt(t0);
              MYON_3dVector V = E0 - point;
              if ( V.Unitize() )
              {
                // Could not find a seed value for dbrent, 
                // but V and T are orthoganal, so t0 is 
                // pretty close.
                if ( fabs(V*T) <= 0.087155742747658173558064270837474 )
                  return true;
              }
              return false; // can't get valid seed - bail out
            }
            et = (i) ? (0.5*(t0+et)) : 0.5*(t0+t1);
            if ( et == t0 )
            {
              return true;
            }
          }

          rc = MYON_FindLocalMinimum( distSqToEllipse, p,
                            t0, et, t1, 
                            MYON_EPSILON,  MYON_SQRT_EPSILON, 100,
                            &et );
          if ( rc )
            *t = (et >= 2.0*MYON_PI) ? 0.0 : et;
        }
      }
    }
  }
  return rc;
}

#if defined(MYON_COMPILER_MSC)
#pragma MYON_PRAGMA_WARNING_POP
#endif

MYON_3dPoint MYON_Ellipse::ClosestPointTo( const MYON_3dPoint& point ) const
{
  double t;
  ClosestPointTo( point, &t );
  return PointAt( t );
}

double MYON_Ellipse::EquationAt( 
                 const MYON_2dPoint& p // coordinates in plane
                 ) const
{
  double e, x, y;
  if ( radius[0] != 0.0 && radius[1] != 0.0 ) {
    x = p.x/radius[0];
    y = p.y/radius[1];
    e = x*x + y*y - 1.0;
  }
  else {
    e = 0.0;
  }
  return e;
}

MYON_2dVector MYON_Ellipse::GradientAt( 
                 const MYON_2dPoint& p // coordinates in plane
                 ) const
{
  MYON_2dVector g;
  if ( radius[0] != 0.0 && radius[1] != 0.0 ) {
    g.x = 2.0*p.x/(radius[0]*radius[0]);
    g.y = 2.0*p.y/(radius[1]*radius[1]);
  }
  else {
    g = MYON_2dVector::ZeroVector;
  }
  return g;
}

bool MYON_Ellipse::Rotate( 
                          double sin_angle, double cos_angle, 
                          const MYON_3dVector& axis
                          )
{
  return plane.Rotate( sin_angle, cos_angle, axis );
}

bool MYON_Ellipse::Rotate( 
                          double angle, 
                          const MYON_3dVector& axis
                          )
{
  return plane.Rotate( angle, axis );
}

bool MYON_Ellipse::Rotate( 
                          double sin_angle, double cos_angle, 
                          const MYON_3dVector& axis,
                          const MYON_3dPoint& point
                          )
{
  return plane.Rotate( sin_angle, cos_angle, axis, point );
}

bool MYON_Ellipse::Rotate( 
                          double angle, 
                          const MYON_3dVector& axis,
                          const MYON_3dPoint& point
                          )
{
  return plane.Rotate( angle, axis, point );
}


bool MYON_Ellipse::Translate(
                          const MYON_3dVector& delta
                            )
{
  return plane.Translate( delta );
}

int MYON_Ellipse::GetNurbForm( MYON_NurbsCurve& nurbscurve ) const
{
  int rc = 0;
  if ( IsValid() ) {
    nurbscurve.Create( 3, true, 3, 9 );
    nurbscurve.m_knot[0] = nurbscurve.m_knot[1] = 0.0;
    nurbscurve.m_knot[2] = nurbscurve.m_knot[3] = 0.5*MYON_PI;
    nurbscurve.m_knot[4] = nurbscurve.m_knot[5] = MYON_PI;
    nurbscurve.m_knot[6] = nurbscurve.m_knot[7] = 1.5*MYON_PI;
    nurbscurve.m_knot[8] = nurbscurve.m_knot[9] = 2.0*MYON_PI;
    MYON_4dPoint* CV = (MYON_4dPoint*)nurbscurve.m_cv;

    CV[0] = plane.PointAt( radius[0],        0.0);
    CV[1] = plane.PointAt( radius[0],  radius[1]);
    CV[2] = plane.PointAt(       0.0,  radius[1]);
    CV[3] = plane.PointAt(-radius[0],  radius[1]);
    CV[4] = plane.PointAt(-radius[0],        0.0);
    CV[5] = plane.PointAt(-radius[0], -radius[1]);
    CV[6] = plane.PointAt(       0.0, -radius[1]);
    CV[7] = plane.PointAt( radius[0], -radius[1]);
    CV[8] = CV[0];
    
    const double w = 1.0/sqrt(2.0);
    int i;
    for ( i = 1; i < 8; i += 2 ) {
      CV[i].x *= w;
      CV[i].y *= w;
      CV[i].z *= w;
      CV[i].w = w;
    }
    rc = 2;
  }
  return rc;
}
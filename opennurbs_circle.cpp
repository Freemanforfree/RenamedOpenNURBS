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

MYON_Circle::MYON_Circle( const MYON_Plane& p, double r )
{
  Create( p, r );
}

MYON_Circle::MYON_Circle( const MYON_3dPoint& C, double r )
{
  Create( C, r );
}

MYON_Circle::MYON_Circle( const MYON_Plane& pln, const MYON_3dPoint& C, double r )
{
  Create( pln, C, r );
}

MYON_Circle::MYON_Circle( const MYON_2dPoint& P, const MYON_2dPoint& Q, const MYON_2dPoint& R )
{
  Create(P,Q,R);
}

MYON_Circle::MYON_Circle( const MYON_3dPoint& P, const MYON_3dPoint& Q, const MYON_3dPoint& R )
{
  Create(P,Q,R);
}


double MYON_Circle::Radius() const
{
  return radius;
}

double MYON_Circle::Diameter() const
{
  return 2.0*radius;
}

const MYON_3dPoint& MYON_Circle::Center() const
{
  return plane.origin;
}

const MYON_3dVector& MYON_Circle::Normal() const
{
  return plane.zaxis;
}

const MYON_Plane& MYON_Circle::Plane() const
{
  return plane;
}

MYON_BoundingBox MYON_Circle::BoundingBox() const
{
  MYON_BoundingBox bbox;
  MYON_3dPoint corners[4]; // = corners of square that contains circle
  corners[0] = plane.PointAt( radius, radius );
  corners[1] = plane.PointAt( radius,-radius );
  corners[2] = plane.PointAt(-radius, radius );
  corners[3] = plane.PointAt(-radius,-radius );
  bbox.Set(3,0,4,3,&corners[0].x,false);   
  return bbox;
}

bool MYON_Circle::Transform( const MYON_Xform& xform )
{
  const MYON_Plane plane0(plane);
  const bool rc = plane.Transform(xform);
  if (!rc)
  {
    // restore original
    plane = plane0;
  }
  else
  {
    const double ztol = 1.0e-12;
    double a,b,c,d,r1,r2,s;
    // determine scale factor in circle's plane
    // In practice, transformation are either rotations,
    // the scale factor is clearly distinct from 1,
    // or the transformation does not map a circle
    // to a circle.  The code below has tolerance checks
    // so that anything that is close to a rotation gets
    // treated does not change the radius.  If it is
    // clearly a uniform scale in the plane of the circle
    // the scale factor is calculated without using a
    // determinant.  Sine "2d scales" are common, it doesn't
    // work well use the cubed root of the xform'd determinant.
    MYON_3dVector V = xform*plane0.xaxis;
    a = V*plane.xaxis;
    b = V*plane.yaxis;
    if (fabs(a) >= fabs(b))
    {
      r1 = fabs(a);
      if ( r1 > 0.0)
      {
        a = (a>0.0) ? 1.0 : -1.0;
        b /= r1;
        if ( fabs(b) <= ztol )
        {
          b = 0.0;
          if ( fabs(1.0-r1) <= ztol )
            r1 = 1.0;
        }
      }
    }
    else
    {
      r1 = fabs(b);
      b = (b>0.0) ? 1.0 : -1.0;
      a /= r1;
      if ( fabs(a) <= ztol )
      {
        a = 0.0;
        if ( fabs(1.0-r1) <= ztol )
          r1 = 1.0;
      }
    }
    V = xform*plane0.yaxis;
    c = V*plane.xaxis;
    d = V*plane.yaxis;
    if (fabs(d) >= fabs(c))
    {
      r2 = fabs(d);
      if (r2 > 0.0)
      {
        d = (d>0.0) ? 1.0 : -1.0;
        c /= r2;
        if ( fabs(c) <= ztol )
        {
          c = 0.0;
          if ( fabs(1.0-r2) <= ztol )
            r2 = 1.0;
        }
      }
    }
    else
    {
      r2 = fabs(c);
      c = (c>0.0) ? 1.0 : -1.0;
      d /= r2;
      if ( fabs(d) <= ztol )
      {
        d = 0.0;
        if ( fabs(1.0-r2) <= ztol )
          r2 = 1.0;
      }
    }
    if (    0.0 == b 
         && 0.0 == c 
         && fabs(r1-r2) <= MYON_SQRT_EPSILON*(r1+r2) 
       )
    {
      // transform is a similarity
      s = (r1 == r2) ? r1 : (0.5*(r1+r2)); // = sqrt(r1*r2) but more accurate
    }
    else
    {
      // non-uniform scaling or skew in circle's plane
      // do something reasonable
      s = sqrt(fabs(r1*r2*(a*d-b*c)));
    }

    if ( s > 0.0 )
    {
      //#if defined(MYON_DEBUG) && !defined(MYON_COMPILER_GNU)
      //double det = fabs(xform.Determinant());
      //double s0 = pow(det,1.0/3.0);
      //if ( fabs(s-s0) > MYON_SQRT_EPSILON*s0 )
      //{
      //  // non-uniform scale or a bug
      //  // In the non-uniform scal case, b and c should be
      //  // "zero".
      //  int breakpointhere = 0; // (generates gcc warning)
      //}
      //#endif
      if ( fabs(s-1.0) > MYON_SQRT_EPSILON )
        radius *= s;
    }
  }

  return rc;
}


double MYON_Circle::Circumference() const
{
  return fabs(2.0*MYON_PI*radius);
}

bool MYON_Circle::Create( const MYON_Plane& p, double r )
{
  plane = p;
  if ( !plane.IsValid() )
    plane.UpdateEquation(); // people often forget to set equation
  radius = r;
  //m_point[0] = plane.PointAt( radius, 0.0 );
  //m_point[1] = plane.PointAt( 0.0, radius );
  //m_point[2] = plane.PointAt( -radius, 0.0 );
  return ( radius > 0.0 );
}

bool MYON_Circle::Create( const MYON_3dPoint& C, double r )
{
  MYON_Plane p = MYON_xy_plane;
  p.origin = C;
  p.UpdateEquation();
  return Create( p, r );
}

bool MYON_Circle::Create( const MYON_Plane& pln,
                          const MYON_3dPoint& C,
                          double r 
                          )
{
  MYON_Plane p = pln;
  p.origin = C;
  p.UpdateEquation();
  return Create( p, r );
}

bool MYON_Circle::Create( // circle through three 3d points
    const MYON_2dPoint& P,
    const MYON_2dPoint& Q,
    const MYON_2dPoint& R
    )
{
  return Create(MYON_3dPoint(P),MYON_3dPoint(Q),MYON_3dPoint(R));
}

bool MYON_Circle::Create( // circle through three 3d points
    const MYON_3dPoint& P,
    const MYON_3dPoint& Q,
    const MYON_3dPoint& R
    )
{
  MYON_3dPoint C;
  MYON_3dVector X, Y, Z;
  // return ( radius > 0.0 && plane.IsValid() );
  //m_point[0] = P;
  //m_point[1] = Q;
  //m_point[2] = R;

  // get normal
  for(;;)
  {
    if ( !Z.PerpendicularTo( P, Q, R ) )
      break;

    // get center as the intersection of 3 planes
    MYON_Plane plane0( P, Z );
    MYON_Plane plane1( 0.5*(P+Q), P-Q );
    MYON_Plane plane2( 0.5*(R+Q), R-Q );
    if ( !MYON_Intersect( plane0, plane1, plane2, C ) )
      break;

    X = P - C;
    radius = X.Length();
    if ( !(radius > 0.0) )
      break;

    if ( !X.Unitize() )
      break;
    
    Y = MYON_CrossProduct( Z, X );
    if ( !Y.Unitize() )
      break;

    plane.origin = C;
    plane.xaxis = X;
    plane.yaxis = Y;
    plane.zaxis = Z;

    plane.UpdateEquation();

    return true;
  }

  plane = MYON_Plane::World_xy;
  radius = 0.0;
  return false;
}

//////////
// Create an circle from two 2d points and a tangent at the first point.
bool MYON_Circle::Create(
  const MYON_2dPoint& P,     // [IN] point P
  const MYON_2dVector& Pdir, // [IN] tangent at P
  const MYON_2dPoint& Q      // [IN] point Q
  )
{
  return Create( MYON_3dPoint(P), MYON_3dVector(Pdir), MYON_3dPoint(Q) );
}

//////////
// Create an circle from two 3d points and a tangent at the first point.
bool MYON_Circle::Create(
  const MYON_3dPoint& P,      // [IN] point P
  const MYON_3dVector& Pdir, // [IN] tangent at P
  const MYON_3dPoint& Q      // [IN] point Q
  )
{
  bool rc = false;
  double a, b;
  MYON_3dVector QP, RM, RP, X, Y, Z;
  MYON_3dPoint M, C;
  MYON_Line A, B;

  // n = normal to circle
  QP = Q-P;
  Z = MYON_CrossProduct( QP, Pdir );
  if ( Z.Unitize() ) {
    M = 0.5*(P+Q);
    RM = MYON_CrossProduct( QP, Z ); // vector parallel to center-M
    A.Create(M,M+RM);
    RP = MYON_CrossProduct( Pdir, Z ); //  vector parallel to center-P
    B.Create(P,P+RP);
    if ( MYON_Intersect( A, B, &a, &b ) ) {
      C = A.PointAt( a ); // center = intersection of lines A and B
      X = P-C;
      radius = C.DistanceTo(P);
      if ( X.Unitize() ) {
        Y = MYON_CrossProduct( Z, X );
        if ( Y*Pdir < 0.0 ) 
        {
          Z = -Z;
          Y = -Y;
          RM = -RM;
        }
        plane.origin = C;
        plane.xaxis = X;
        plane.yaxis = Y;
        plane.zaxis = Z;
        plane.UpdateEquation();
        //m_point[0] = P;
        //m_point[1] = C + radius*RM/RM.Length();
        //m_point[2] = Q;
        rc = IsValid();
      }
    }
  }
  return rc;
}


bool MYON_Circle::IsValid() const
{
  bool rc = (    MYON_IsValid(radius) 
              && radius > 0.0
              && plane.IsValid()
            );
  return rc;
}

bool MYON_Circle::IsInPlane( const MYON_Plane& base_plane, double tolerance ) const
{
  double d;
  int i;
  for ( i = 0; i < 8; i++ ) {
    d = base_plane.plane_equation.ValueAt( PointAt(0.25*i*MYON_PI) );
    if ( fabs(d) > tolerance )
      return false;
  }
  return true;
}

MYON_3dPoint MYON_Circle::PointAt( double t ) const
{
  return plane.PointAt( cos(t)*radius, sin(t)*radius );
}

MYON_3dVector MYON_Circle::DerivativeAt( 
                 int d, // desired derivative ( >= 0 )
                 double t // parameter
                 ) const
{
  double r0 = radius;
  double r1 = radius;
  switch (std::abs(d) % 4)
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

MYON_3dVector MYON_Circle::TangentAt( double t ) const
{
  MYON_3dVector T = DerivativeAt(1,t);
  T.Unitize();
  return T;
}

bool MYON_Circle::ClosestPointTo( const MYON_3dPoint& point, double* t ) const
{
  bool rc = true;
  if ( t ) {
    double u, v;
    rc = plane.ClosestPointTo( point, &u, &v );
    if ( u == 0.0 && v == 0.0 ) {
      *t = 0.0;
    }
    else {
      *t = atan2( v, u );
      if ( *t < 0.0 )
        *t += 2.0*MYON_PI;
    }
  }
  return rc;
}

MYON_3dPoint MYON_Circle::ClosestPointTo( const MYON_3dPoint& point ) const
{
  MYON_3dPoint P;
  MYON_3dVector V = plane.ClosestPointTo( point ) - Center();
  if ( V.Unitize() ) {
    V.Unitize();
    P = Center() + Radius()*V;
  }
  else {
    P = PointAt(0.0);
  }
  return P;
}

double MYON_Circle::EquationAt( 
                 const MYON_2dPoint& p // coordinates in plane
                 ) const
{
  double e, x, y;
  if ( radius != 0.0 ) {
    x = p.x/radius;
    y = p.y/radius;
    e = x*x + y*y - 1.0;
  }
  else {
    e = 0.0;
  }
  return e;
}

MYON_2dVector MYON_Circle::GradientAt( 
                 const MYON_2dPoint& p // coordinates in plane
                 ) const
{
  MYON_2dVector g;
  if ( radius != 0.0 ) {
    const double rr = 2.0/(radius*radius);
    g.x = rr*p.x;
    g.y = rr*p.y;
  }
  else {
    g = MYON_2dVector::ZeroVector;
  }
  return g;
}

bool MYON_Circle::Rotate( 
                          double sin_angle, double cos_angle, 
                          const MYON_3dVector& axis
                          )
{
  return plane.Rotate( sin_angle, cos_angle, axis );
}

bool MYON_Circle::Rotate( 
                          double angle, 
                          const MYON_3dVector& axis
                          )
{
  return plane.Rotate( angle, axis );
}

bool MYON_Circle::Rotate( 
                          double sin_angle, double cos_angle, 
                          const MYON_3dVector& axis,
                          const MYON_3dPoint& point
                          )
{
  return plane.Rotate( sin_angle, cos_angle, axis, point );
}

bool MYON_Circle::Rotate( 
                          double angle, 
                          const MYON_3dVector& axis,
                          const MYON_3dPoint& point
                          )
{
  return plane.Rotate( angle, axis, point );
}


bool MYON_Circle::Translate(
                          const MYON_3dVector& delta
                            )
{
  //m_point[0] += delta;
  //m_point[1] += delta;
  //m_point[2] += delta;
  return plane.Translate( delta );
}

bool MYON_Circle::Reverse()
{
  //MYON_3dPoint P = m_point[0];
  //m_point[0] = m_point[2];
  //m_point[2] = P;
  plane.yaxis = -plane.yaxis;
  plane.zaxis = -plane.zaxis;
  plane.UpdateEquation();
  return true;
}

double MYON_Circle::MaximumCoordinate() const
{
  return  plane.origin.MaximumCoordinate() + radius;
}

int MYON_Circle::GetNurbForm( MYON_NurbsCurve& nurbscurve ) const
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

    CV[0] = plane.PointAt( radius,     0.0);
    CV[1] = plane.PointAt( radius,  radius);
    CV[2] = plane.PointAt(    0.0,  radius);
    CV[3] = plane.PointAt(-radius,  radius);
    CV[4] = plane.PointAt(-radius,     0.0);
    CV[5] = plane.PointAt(-radius, -radius);
    CV[6] = plane.PointAt(    0.0, -radius);
    CV[7] = plane.PointAt( radius, -radius);
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



bool MYON_Circle::GetRadianFromNurbFormParameter( double NurbParameter, double* RadianParameter ) const
//returns false unless   0<= NurbParameter,  <= 2*PI*Radius
{
	if(!IsValid()) 
		return false;

	MYON_Arc arc(*this, 2*MYON_PI);
	return arc.GetRadianFromNurbFormParameter( NurbParameter, RadianParameter);
}



bool MYON_Circle::GetNurbFormParameterFromRadian( double RadianParameter, double* NurbParameter) const
{
	if(!IsValid()) 
		return false;

	MYON_Arc arc(*this, 2*MYON_PI);
	return arc.GetNurbFormParameterFromRadian(  RadianParameter, NurbParameter);
}




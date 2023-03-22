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

MYON_Arc::MYON_Arc( const MYON_Circle& c, double angle_in_radians ) 
{
  Create( c, angle_in_radians );
}

MYON_Arc::MYON_Arc( const MYON_Circle& c, MYON_Interval angle_interval_in_radians ) 
{
  Create( c, angle_interval_in_radians );
}

MYON_Arc::MYON_Arc( const MYON_Plane& p, double r, double angle_in_radians )
{
  Create( p, r, angle_in_radians );
}

MYON_Arc::MYON_Arc( const MYON_3dPoint& C, double r, double angle_in_radians )
{
  Create( C, r, angle_in_radians );
}

MYON_Arc::MYON_Arc( const MYON_Plane& pln, const MYON_3dPoint& C, double r, double angle_in_radians )
{
  Create( pln, C, r, angle_in_radians );
}

MYON_Arc::MYON_Arc( const MYON_2dPoint& P, const MYON_2dPoint& Q, const MYON_2dPoint& R ) 
{
  Create( P, Q, R );
}

MYON_Arc::MYON_Arc( const MYON_3dPoint& P, const MYON_3dPoint& Q, const MYON_3dPoint& R )
{
  Create( P, Q, R );
}

MYON_Arc& MYON_Arc::operator=( const MYON_Circle& src )
{
  if (this != &src)
  {
    MYON_Circle::operator=(src);
    m_angle = MYON_Interval::ZeroToTwoPi;
  }
  return *this;
}


bool MYON_Arc::Create(
  const MYON_Circle& circle,
  double angle_radians          // angle in radians
  )
{
  return Create( circle, MYON_Interval( 0.0, angle_radians ) );
}

bool MYON_Arc::Create(
  const MYON_Circle& circle,
  MYON_Interval angle_interval_in_radians
  )
{
  bool rc = true;
  plane = circle.plane;
  plane.UpdateEquation();
  radius = circle.radius;
  m_angle = angle_interval_in_radians;
  if ( m_angle.IsDecreasing() )
  {
    rc = false; // bogus input
    // m_angle must never be decreasing
    m_angle.Swap();
    Reverse();
  }
  if ( m_angle.Length() > 2.0*MYON_PI )
  {
    rc = false; // bogus input
    m_angle.m_t[1] = m_angle.m_t[0] + 2.0*MYON_PI;
  }
  if ( rc )
    rc = IsValid();
  return rc;
}


bool MYON_Arc::Create(
  const MYON_Plane& pl, // circle is in this plane with center at m_origin
  double r,         // radius
  double angle_radians   // angle in radians
  )
{
  return Create( MYON_Circle(pl,r), MYON_Interval( 0.0, angle_radians ) );
}

bool MYON_Arc::Create( // arc is parallel to XY plane
  const MYON_3dPoint& center, // center
  double r,            // radius
  double angle_radians // angle in radians
  )
{
  MYON_Plane p;
  p.CreateFromNormal( center, MYON_3dVector::ZAxis );
  return Create( MYON_Circle(p,r), MYON_Interval( 0.0, angle_radians ) );
}

bool MYON_Arc::Create(        // arc parallel to a plane
  const MYON_Plane& pl,       // circle will be parallel to this plane
  const MYON_3dPoint& center, // center
  double r,                 // radius
  double angle_radians      // angle in radians
  )
{
  MYON_Plane p = pl;
  p.origin = center;
  p.UpdateEquation();
  return Create( MYON_Circle( p, r), MYON_Interval( 0.0, angle_radians ) );
}

bool MYON_Arc::Create( // arc through 3 2d points
  const MYON_2dPoint& P, // point P
  const MYON_2dPoint& Q, // point Q
  const MYON_2dPoint& R // point R
  )
{
  MYON_Circle c(P,Q,R);
  double a = 0.0;
  c.ClosestPointTo( MYON_3dPoint(R), &a );
  return Create( c, MYON_Interval(0.0,a) );
}

bool MYON_Arc::Create( // arc through 3 3d points
  const MYON_3dPoint& P, // point P
  const MYON_3dPoint& Q, // point Q
  const MYON_3dPoint& R  // point R
  )
{
  MYON_Circle c;
  double a = 0.0;

  for (;;)
  {

    if ( !c.Create(P,Q,R) )
      break;

    if ( !c.ClosestPointTo( R, &a ) )
      break;

    if ( !(a > 0.0) )
      break;
    
    if ( !Create( c, MYON_Interval(0.0,a) ) )
      break;

    return true;
  }

  plane = MYON_Plane::World_xy;
  radius = 0.0;
  m_angle.Set(0.0,0.0);

  return false;
}

//////////
// Create an arc from a 2d start point, 2d start direction, and 2d end point.
bool MYON_Arc::Create(
  const MYON_2dPoint& P, // [IN] start point
  const MYON_2dVector& Pdir,  // [IN] arc direction at start
  const MYON_2dPoint&  Q   // [IN] end point
  )
{
  return Create( MYON_3dPoint(P), MYON_3dVector(Pdir), MYON_3dPoint(Q) );
}

//////////
// Create an arc from a 3d start point, 3d start direction, and 3d end point.
bool MYON_Arc::Create(
  const MYON_3dPoint& P, // [IN] start point
  const MYON_3dVector& Pdir,  // [IN] arc direction at start
  const MYON_3dPoint&  Q   // [IN] end point
  )
{
  double a=0.0;
  bool rc = MYON_Circle::Create(P,Pdir,Q);
  if ( rc ) {
    m_angle.m_t[0] = 0.0;
    rc = MYON_Circle::ClosestPointTo(Q,&a);
    m_angle.m_t[1] = a;
    if (a <= MYON_ZERO_TOLERANCE || a >= 2.0*MYON_PI-MYON_ZERO_TOLERANCE )
      rc = false;
  }
  return rc;
}




void MYON_Arc::Dump( MYON_TextLog& dump ) const
{
  dump.Print("Arc: normal = ");
  dump.Print(plane.zaxis);
  dump.Print(" center = ");
  dump.Print(plane.origin);
  dump.Print(" start = ");
  dump.Print( StartPoint() );
  dump.Print(" end = ");
  dump.Print( EndPoint() );
  dump.Print(" radius = ");
  dump.Print(Radius());
  dump.Print(" angle = [");
  dump.Print(m_angle[0]);
  dump.Print(",");
  dump.Print(m_angle[1]);
  dump.Print("]\n");
}

MYON_3dPoint MYON_Arc::StartPoint() const
{
  return PointAt(m_angle[0]);
}

MYON_3dPoint MYON_Arc::MidPoint() const
{
  return PointAt(m_angle.Mid());
}

MYON_3dPoint MYON_Arc::EndPoint() const
{
  return PointAt(m_angle[1]);
}

bool MYON_Arc::IsValid() const
{
  return (    MYON_Circle::IsValid() 
           && m_angle.IsValid()
           && AngleRadians() > MYON_ZERO_TOLERANCE 
           && AngleRadians() <= 2.0*MYON_PI+MYON_ZERO_TOLERANCE) 
         ? true : false;
}

MYON_BoundingBox MYON_Arc::BoundingBox() const
{
  // TODO - compute tight arc bounding box

  // Using these knot[] and cv[] arrays makes this function
  // not use any heap memory.
  double knot[10];
  MYON_4dPoint cv[9];
  MYON_NurbsCurve c;
  c.m_knot = knot;
  c.m_cv = &cv[0].x;
  if ( GetNurbForm(c) )
    return c.BoundingBox();
  return MYON_Circle::BoundingBox();
}

bool MYON_Arc::GetBoundingBox(
       MYON_BoundingBox& bbox,
       int bGrowBox
       ) const
{
  if (bGrowBox)
  {
    MYON_BoundingBox arc_bbox = BoundingBox();
    bbox.Union(arc_bbox);
  }
  else
    bbox = BoundingBox();
  return bbox.IsValid();
}

bool MYON_Arc::IsCircle() const
{
  return (fabs(fabs(AngleRadians()) - 2.0*MYON_PI) <= MYON_ZERO_TOLERANCE) 
         ? true : false;
}

double MYON_Arc::AngleRadians() const
{
  return m_angle[1]-m_angle[0];
}

double MYON_Arc::AngleDegrees() const
{
  return (AngleRadians()/MYON_PI)*180.0;
}

MYON_Interval MYON_Arc::Domain() const
{
  return m_angle;
}

MYON_Interval MYON_Arc::DomainRadians() const
{
  return m_angle;
}

MYON_Interval MYON_Arc::DomainDegrees() const
{
  const double rtd = 180.0/MYON_PI;
  MYON_Interval ad = m_angle;
  ad.m_t[0] *= rtd;
  ad.m_t[1] *= rtd;
  return ad;
}

bool MYON_Arc::SetAngleRadians( double a )
{
  if ( a < 0.0 ) 
  {
    double a0 = m_angle.m_t[0];
    m_angle.Set(a0+a,a0);
    Reverse();
  }
  else
  {
    m_angle.m_t[1] = m_angle.m_t[0] + a;
  }
  return ( fabs(m_angle.Length()) <= 2.0*MYON_PI ) ? true : false;
}

bool MYON_Arc::SetAngleIntervalRadians( MYON_Interval angle_in_radians )
{
  bool rc = angle_in_radians.IsIncreasing() 
            && angle_in_radians.Length() < (1.0+MYON_SQRT_EPSILON)*2.0*MYON_PI;
  if (rc)
  {
    m_angle = angle_in_radians;
  }
  return rc;
}

bool MYON_Arc::SetAngleDegrees( double a )
{
  return SetAngleRadians( (a/180.0)*MYON_PI );
}

bool MYON_Arc::Trim( MYON_Interval domain)
{
  bool ok = false;

  if(domain[0]<domain[1] && domain[1]-domain[0]<=2.0 * MYON_PI+MYON_ZERO_TOLERANCE){
		m_angle = domain;
    if (m_angle.Length() > 2.0*MYON_PI) m_angle[1] = m_angle[0] + 2.0*MYON_PI;
    ok = true;
  }
	return ok;
}

bool MYON_ArcCurve::IsContinuous(
    MYON::continuity c,
    double t, 
    int*,   // hint                - formal parameter intentionally ignored in this virtual function
    double, // point_tolerance     - formal parameter intentionally ignored in this virtual function
    double, // d1_tolerance        - formal parameter intentionally ignored in this virtual function
    double, // d2_tolerance        - formal parameter intentionally ignored in this virtual function
    double, // cos_angle_tolerance - formal parameter intentionally ignored in this virtual function
    double  // curvature_tolerance - formal parameter intentionally ignored in this virtual function
    ) const
{
  // 20 March 2003 Dale Lear
  //      Added this override of IsContinuous() to
  //      speed queries and support the
  //      locus favors of MYON::continuity.

  bool rc = true;

  if ( !IsClosed() )
  {
    switch(c)
    {
    case MYON::continuity::unknown_continuity:
    case MYON::continuity::C0_continuous:
    case MYON::continuity::C1_continuous:
    case MYON::continuity::C2_continuous:
    case MYON::continuity::G1_continuous:
    case MYON::continuity::G2_continuous:
    case MYON::continuity::Cinfinity_continuous:
    case MYON::continuity::Gsmooth_continuous:
      // rc = true;
      break;

    case MYON::continuity::C0_locus_continuous:
    case MYON::continuity::C1_locus_continuous:
    case MYON::continuity::C2_locus_continuous:
    case MYON::continuity::G1_locus_continuous:
    case MYON::continuity::G2_locus_continuous:
      // open arc is locus discontinuous at end parameter.
      // By convention (see MYON::continuity comments) it
      // is locus continuous at start parameter.
      if ( t >= Domain()[1] )
        rc = false;
      break;
    }
  }

  return rc;
}



bool MYON_Arc::Reverse()
{
  m_angle.Reverse();
  plane.yaxis = -plane.yaxis;
  plane.zaxis = -plane.zaxis;
  plane.UpdateEquation();
  return true;
}

double MYON_Arc::Length() const
{
  return fabs(AngleRadians()*radius);
}

double MYON_Arc::SectorArea() const
{
  return fabs(0.5*AngleRadians()*radius*radius);
}

MYON_3dPoint MYON_Arc::SectorAreaCentroid() const
{
  double a = 0.5*fabs(AngleRadians());
  double d = (a > 0.0) ? sin(a)/a : 0.0;
  d *= 2.0*radius/3.0;
  a = 0.5*(m_angle[1]+m_angle[0]);
  return plane.PointAt(d*cos(a),d*sin(a));
}

double MYON_Arc::SegmentArea() const
{
  double a = fabs(AngleRadians());
  return (0.5*(a - sin(a))*radius*radius);
}

MYON_3dPoint MYON_Arc::SegmentAreaCentroid() const
{
  double a = fabs(AngleRadians());
  double sin_halfa = sin(0.5*a);
  double d = 3.0*(a - sin(a));
  if ( d > 0.0 )
    d = (sin_halfa*sin_halfa*sin_halfa)/d;
  d *= 4.0*radius;
  a = 0.5*(m_angle[1]+m_angle[0]);
  return plane.PointAt(d*cos(a),d*sin(a));
}


/* moved to opennurbs_arccurve.cpp


int MYON_Arc::GetNurbForm( MYON_NurbsCurve& nurbscurve ) const
{
  int rc = 0;
  if ( IsValid() ) {
    if ( IsCircle() )
      rc = MYON_Circle::GetNurbForm( nurbscurve );
    else {
      double a, b, c, t, dt, angle;
      int span_count, i;
      angle = m_angle.Length();
      if (angle <= 0.5*MYON_PI + MYON_ZERO_TOLERANCE) {
		    span_count = 1;
        dt = 0.5;
      }
      else if (angle <= MYON_PI + MYON_ZERO_TOLERANCE) {
		    span_count = 2;
  		  angle *= 0.5;
        dt = 0.25;
      }
      else if (angle <= 1.5*MYON_PI + MYON_ZERO_TOLERANCE) {
		    span_count = 3;
  		  angle /= 3.0;
        dt = 1.0/6.0;
      }
      else {
		    span_count = 4;
  		  angle *= 0.25;
        dt = 0.125;
      }
      nurbscurve.Create( 3, true, 3, 2*span_count+1 );
      MYON_4dPoint* CV = (MYON_4dPoint*)nurbscurve.m_cv;
      t = m_angle[0];
      for ( i = 0; i < span_count; i++ ) {
        nurbscurve.m_knot[2*i] = t;
        nurbscurve.m_knot[2*i+1] = t;
        CV[2*i] = PointAt(m_angle.ParameterAt(t));
        t += dt;
        CV[2*i+1] = PointAt(m_angle.ParameterAt(t));
        t += dt;
      }

      span_count *= 2;
      t = m_angle[1];
      CV[span_count] = PointAt(t);
      nurbscurve.m_knot[span_count] = t;
      nurbscurve.m_knot[span_count+1] = t;

	    a = cos(0.5*angle);
	    b = a - 1.0;
	    c = radius*angle;

	    for (i = 1; i < span_count; i += 2) {
		    CV[i].x +=  b * plane.origin.x;
		    CV[i].y +=  b * plane.origin.y;
		    CV[i].z +=  b * plane.origin.z;
		    CV[i].w = a;
	    }
      
      //for ( i = 1; i < span_count; i += 2 ) {
      //  t = CV[i].w;
      //  c = 1.0/t;
      //  a = CV[i].x*c; b = ArcDeFuzz(a); if ( a != b ) CV[i].x = b*t;
      //  a = CV[i].y*c; b = ArcDeFuzz(a); if ( a != b ) CV[i].y = b*t;
      //  a = CV[i].z*c; b = ArcDeFuzz(a); if ( a != b ) CV[i].z = b*t;
      //}
    }
    rc = 2;
  }
	return rc;
}
*/

// returns parameters of point on arc that is closest to given point
bool MYON_Arc::ClosestPointTo( 
       const MYON_3dPoint& pt, 
       double* t
       ) const
{
  /*
  double tt, a;
  if ( !t )
    t =&tt;
  bool rc = MYON_Circle::ClosestPointTo(pt,t);
  if (rc) {
    if ( *t < m_angle[0] ) {
      a = 0.5*(m_angle[0] + m_angle[1] - 2.0*MYON_PI);
      if ( *t < a )
        *t = m_angle[1];
      else 
        *t = m_angle[0];
    }
    else if ( *t > m_angle[1] ) {
      a = 0.5*(m_angle[0] + m_angle[1] + 2.0*MYON_PI);
      if ( *t > a )
        *t = m_angle[0];
      else 
        *t = m_angle[1];
    }
  }
  */
  double s;
  double twopi = 2.0*MYON_PI;
  bool rc = MYON_Circle::ClosestPointTo(pt,&s);
  if (rc){
    s -= m_angle[0];
    while (s < 0.0) s += twopi;

		// Greg Arden April 14 2003. Changed test from ">" to ">=" this ensures that
		// closest point to a circle at the seam will return the least parameter value.
    while (s >= twopi) s -= twopi;

    double s1 = m_angle.Length();

    if (s < 0.0) s = 0.0;//shouldn't happen
    if (s > s1){
      if (s > 0.5*s1 + MYON_PI)
        s = 0.0;
      else
        s = s1;
    }

    if (t)
      *t = m_angle[0] + s;
  }

      
  return rc;
}

// returns point on circle that is arc to given point
MYON_3dPoint MYON_Arc::ClosestPointTo( 
       const MYON_3dPoint& pt
       ) const
{
  double t = m_angle[0];
  ClosestPointTo( pt, &t );
  return PointAt(t);
}


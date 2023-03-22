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

MYON_OBJECT_IMPLEMENT(MYON_ArcCurve,MYON_Curve,"CF33BE2A-09B4-11d4-BFFB-0010830122F0");

MYON_ArcCurve::MYON_ArcCurve() MYON_NOEXCEPT
{}

MYON_ArcCurve::~MYON_ArcCurve()
{}

MYON_ArcCurve::MYON_ArcCurve( const MYON_ArcCurve& src )
  : MYON_Curve(src)
  , m_arc(src.m_arc)
  , m_t(src.m_t)
  , m_dim(src.m_dim)
{}

MYON_ArcCurve& MYON_ArcCurve::operator=( const MYON_ArcCurve& src )
{
  if ( this != &src ) 
  {
    MYON_Curve::operator=(src);
    m_arc = src.m_arc;
    m_t = src.m_t;
    m_dim  = src.m_dim;
  }
  return *this;
}

#if defined(MYON_HAS_RVALUEREF)

MYON_ArcCurve::MYON_ArcCurve( MYON_ArcCurve&& src) MYON_NOEXCEPT
  : MYON_Curve(std::move(src))
  , m_arc(std::move(src.m_arc))
  , m_t(src.m_t)
  , m_dim(src.m_dim)
{
}

MYON_ArcCurve& MYON_ArcCurve::operator=( MYON_ArcCurve&& src)
{
  if ( this != &src ) 
  {
    MYON_Curve::operator=(std::move(src));
    m_arc = std::move(src.m_arc);
    m_t = src.m_t;
    m_dim  = src.m_dim;
  }
  return *this;
}

#endif


MYON_ArcCurve::MYON_ArcCurve( const MYON_Arc& A )
{
  m_arc = A;
  m_t.m_t[0] = 0.0;
  m_t.m_t[1] = m_arc.Length();
  if ( m_t.m_t[1] <= 0.0 )
    m_t.m_t[1] = 1.0;
  m_dim = 3;
}

MYON_ArcCurve::MYON_ArcCurve( const MYON_Circle& circle )
{
  MYON_ArcCurve::operator=(circle);
}

MYON_ArcCurve::MYON_ArcCurve( const MYON_Arc& A, double t0, double t1 )
{
  m_arc = A;
  m_t.m_t[0] = t0;
  m_t.m_t[1] = t1;
  m_dim = 3;
}

MYON_ArcCurve::MYON_ArcCurve( const MYON_Circle& circle, double t0, double t1 )
{
  m_arc = circle;
  m_t.m_t[0] = t0;
  m_t.m_t[1] = t1;
  m_dim = 3;
}



unsigned int MYON_ArcCurve::SizeOf() const
{
  unsigned int sz = MYON_Curve::SizeOf();
  sz += sizeof(*this) - sizeof(MYON_Curve);
  return sz;
}

MYON__UINT32 MYON_ArcCurve::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_arc),&m_arc);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_t),&m_t);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_dim),&m_dim);
  return current_remainder;
}

MYON_ArcCurve& MYON_ArcCurve::operator=( const MYON_Arc& A )
{
  m_arc = A;
  m_t.m_t[0] = 0.0;
  m_t.m_t[1] = A.Length();
  if ( m_t.m_t[1] == 0.0 )
    m_t.m_t[1] = 1.0;
  m_dim = 3;
  return *this;
}

MYON_ArcCurve& MYON_ArcCurve::operator=(const MYON_Circle& circle)
{
  m_arc = circle;
  m_t.m_t[0] = 0.0;
  m_t.m_t[1] = m_arc.Length();
  if ( m_t.m_t[1] <= 0.0 )
    m_t.m_t[1] = 1.0;
  m_dim = 3;
  return *this;
}

int MYON_ArcCurve::Dimension() const
{
  return m_dim;
}

bool MYON_ArcCurve::GetBBox( // returns true if successful
         double* boxmin,    // minimum
         double* boxmax,    // maximum
         bool bGrowBox
         ) const
{
  bool rc = m_arc.IsValid();
  if (rc) {
    MYON_BoundingBox bbox = m_arc.BoundingBox();
    if ( bGrowBox ) {
      if ( boxmin[0] > bbox.m_min.x ) boxmin[0] = bbox.m_min.x;
      if ( boxmin[1] > bbox.m_min.y ) boxmin[1] = bbox.m_min.y;
      if ( boxmax[0] < bbox.m_max.x ) boxmax[0] = bbox.m_max.x;
      if ( boxmax[1] < bbox.m_max.y ) boxmax[1] = bbox.m_max.y;
      if ( m_dim > 2 ) {
        if ( boxmin[2] > bbox.m_min.z ) boxmin[2] = bbox.m_min.z;
        if ( boxmax[2] < bbox.m_max.z ) boxmax[2] = bbox.m_max.z;
      }
    }
    else {
      boxmin[0] = bbox.m_min.x;
      boxmin[1] = bbox.m_min.y;
      boxmax[0] = bbox.m_max.x;
      boxmax[1] = bbox.m_max.y;
      if ( m_dim > 2 ) {
        boxmin[2] = bbox.m_min.z;
        boxmax[2] = bbox.m_max.z;
      }
    }
  }
  return rc;
}

bool
MYON_ArcCurve::Transform( const MYON_Xform& xform )
{
  TransformUserData(xform);
	DestroyCurveTree();
  return m_arc.Transform( xform );
}

bool MYON_ArcCurve::IsValid( MYON_TextLog* text_log ) const
{
  if ( !m_t.IsIncreasing() )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_ArcCurve - m_t=(%g,%g) - it should be an increasing interval.\n",m_t[0],m_t[1]);
    return false;
  }

  if ( !m_arc.IsValid() )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_ArcCurve m_arc is not valid\n");
    return false;
  }
  // 7-May-21.  GBA. Added conditions to define a degenerate MYON_ArcCurve
  // Note: these conditions should be enforced by Trim and Split
  if (m_arc.radius < MYON_ZERO_TOLERANCE )
  {
    if (0 != text_log)
      text_log->Print("MYON_ArcCurve m_arc.radius < MYON_ZERO_TOLERANCE\n");
    return false;
  }
  MYON_3dPoint S = PointAtStart();
  MYON_3dPoint E = PointAtEnd();
  if( S.IsCoincident(E) != IsCircle())
  {
    if (0 != text_log)
    {
      if (IsCircle() )
        text_log->Print("MYON_ArcCurve !Start.IsCoincident(End) an a circle\n");
      else
        text_log->Print("MYON_ArcCurve Start.IsCoincident(End) on open arc curve\n");
    }
    return false;
  }


  return true;
}

void MYON_ArcCurve::Dump( MYON_TextLog& dump ) const
{
  dump.Print( "MYON_ArcCurve:  domain = [%g,%g]\n",m_t[0],m_t[1]);
  dump.PushIndent();
  dump.Print( "center = ");
  dump.Print( m_arc.plane.origin );
  dump.Print( "\nradius = %g\n",m_arc.radius);
  dump.Print( "length = %g\n",m_arc.Length());
  MYON_3dPoint start = PointAtStart();
  MYON_3dPoint end = PointAtEnd();
  dump.Print( "start = "); dump.Print(start);
  dump.Print( "\nend = "); dump.Print(end); dump.Print("\n");
  dump.PopIndent();
}

bool MYON_ArcCurve::Write(
       MYON_BinaryArchive& file // open binary file
     ) const
{
  bool rc = file.Write3dmChunkVersion(1,0);
  if (rc) 
  {
    rc = file.WriteArc( m_arc );
    if (rc) rc = file.WriteInterval( m_t );
    if (rc) rc = file.WriteInt(m_dim);
  }
  return rc;
}

bool MYON_ArcCurve::Read(
       MYON_BinaryArchive& file // open binary file
     )
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc)
  {
    if (major_version==1) 
    {
      // common to all 1.x  versions
      rc = file.ReadArc( m_arc );
      if (rc) 
        rc = file.ReadInterval( m_t );
      if (rc) 
        rc = file.ReadInt(&m_dim);
      if ( m_dim != 2 && m_dim != 3 )
        m_dim = 3;
    }
    else
      rc = 0;
  }
  return rc;
}


bool MYON_ArcCurve::SetDomain( double t0, double t1 )
{
  bool rc = false;
  if ( t0 < t1 )
  {
    m_t.Set(t0,t1);
    rc = true;
  }
	DestroyCurveTree();
  return rc;
}


bool MYON_ArcCurve::ChangeDimension( int desired_dimension )
{
  bool rc = (desired_dimension>=2 && desired_dimension<=3);
  if ( rc && m_dim != desired_dimension )
  {
  	DestroyCurveTree();
    if ( desired_dimension == 2 )
      m_dim = 2;
    else
      m_dim = 3;
  }
  return rc;
}


MYON_Interval MYON_ArcCurve::Domain() const
{
  return m_t;
}

bool MYON_ArcCurve::ChangeClosedCurveSeam( 
            double t ){
	bool rc = false;
	if( IsCircle() ){
		double angle_delta = m_t.NormalizedParameterAt(t);
		angle_delta*= 2*MYON_PI;
		
		m_arc.Rotate(angle_delta, m_arc.plane.Normal());
		m_t = MYON_Interval( t, m_t[1] + t - m_t[0]);
		rc = true;
	}
	return rc;
}


int MYON_ArcCurve::SpanCount() const
{
  return 1;
}

bool MYON_ArcCurve::GetSpanVector( double* s ) const
{
  s[0] = m_t[0];
  s[1] = m_t[1];
  return m_t.IsIncreasing();
}

int MYON_ArcCurve::Degree() const
{
  return 2;
}


bool
MYON_ArcCurve::IsLinear(  // true if curve locus is a line segment
      double // tolerance - formal parameter intentionally ignored in this virtual function
      ) const
{
  return false;
}

bool
MYON_ArcCurve::IsArc( // true if curve locus in an arc or circle
      const MYON_Plane* plane, // if not nullptr, test is performed in this plane
      MYON_Arc* arc,         // if not nullptr and true is returned, then arc
                              // arc parameters are filled in
      double tolerance // tolerance to use when checking linearity
      ) const
{
  bool rc = (plane) ? IsInPlane(*plane,tolerance) : true;
  if (arc) 
    *arc = m_arc;
  if (rc)
    rc = IsValid();
  return rc;
}

bool
MYON_ArcCurve::IsPlanar(
      MYON_Plane* plane, // if not nullptr and true is returned, then plane parameters
                         // are filled in
      double tolerance // tolerance to use when checking linearity
      ) const
{
  if ( m_dim == 2 )
  {
    return MYON_Curve::IsPlanar(plane,tolerance);
  }
  
  if ( plane ) 
    *plane = m_arc.plane;

  return true;
}

bool
MYON_ArcCurve::IsInPlane(
      const MYON_Plane& plane, // plane to test
      double tolerance // tolerance to use when checking linearity
      ) const
{
  return m_arc.IsInPlane( plane, tolerance );
}

bool 
MYON_ArcCurve::IsClosed() const
{
  return m_arc.IsCircle();
}

bool 
MYON_ArcCurve::IsPeriodic() const
{
  return m_arc.IsCircle();
}

bool
MYON_ArcCurve::Reverse()
{
  bool rc = m_arc.Reverse();
  if (rc)
	{
    m_t.Reverse();
		DestroyCurveTree();
	}	
  return true;
}


bool MYON_ArcCurve::SetStartPoint(MYON_3dPoint start_point)
{
  if (MYON_Curve::SetStartPoint(start_point))
    return true;
  if (IsCircle())
    return false;
  bool rc = false;
  if ( m_dim == 3 || start_point.z == 0.0 )
  {
    MYON_3dPoint P;
    MYON_3dVector T;
    double t = Domain()[1];
    Ev1Der( t, P, T );
    T = -T;
    MYON_Arc a;
    rc = a.Create( P, T, start_point );
    if ( rc )
    {
      a.Reverse();
      m_arc = a;
    }
    else {
      MYON_3dPoint end_point = PointAt(Domain()[1]);
      if (end_point.DistanceTo(start_point) < MYON_ZERO_TOLERANCE*m_arc.Radius()){
        //make arc into circle
        m_arc.plane.xaxis = end_point - m_arc.Center();
        m_arc.plane.xaxis.Unitize();
        m_arc.plane.yaxis = MYON_CrossProduct(m_arc.Normal(), m_arc.plane.xaxis);
        m_arc.plane.yaxis.Unitize();
        m_arc.SetAngleRadians(2.0*MYON_PI);
        rc = true;
      }
    }
  }
	DestroyCurveTree();
  return rc;  
}


bool MYON_ArcCurve::SetEndPoint(MYON_3dPoint end_point)
{
  if (MYON_Curve::SetEndPoint(end_point))
    return true;
  if (IsCircle())
    return false;
  bool rc = false;
  if ( m_dim == 3 || end_point.z == 0.0 )
  {
    MYON_3dPoint P;
    MYON_3dVector T;
    double t = Domain()[0];
    Ev1Der( t, P, T );
    MYON_Arc a;
    rc = a.Create( P, T, end_point );
    if ( rc )
    {
      m_arc = a;
    }
    else {
      MYON_3dPoint start_point = PointAt(Domain()[0]);
      if (end_point.DistanceTo(start_point) < MYON_ZERO_TOLERANCE*m_arc.Radius()){
        //make arc into circle
        m_arc.plane.xaxis = start_point - m_arc.Center();
        m_arc.plane.xaxis.Unitize();
        m_arc.plane.yaxis = MYON_CrossProduct(m_arc.Normal(), m_arc.plane.xaxis);
        m_arc.plane.yaxis.Unitize();
        m_arc.SetAngleRadians(2.0*MYON_PI);
        rc = true;
      }
    }
  }
	DestroyCurveTree();
  return rc;  
}

bool MYON_ArcCurve::Evaluate( // returns false if unable to evaluate
       double t,       // evaluation parameter
       int der_count,  // number of derivatives (>=0)
       int v_stride,   // v[] array stride (>=Dimension())
       double* v,      // v[] array of length stride*(ndir+1)
       int, // side - formal parameter intentionally ignored in this virtual function
       int* // hint - formal parameter intentionally ignored in this virtual function
       ) const
{
  // The issue here is that MYON_PI is a rational approximation of the "real" pi. 
  // Ideally sin(N*pi) would be zero and the bugs of July 2012 and RH-26341 that are
  // discussed below would not exist.  When N is large, N*MYON_PI isn't close by any 
  // measure to a multiple of "real" pi.  But, for smallish N that we are likely to
  // encounter in practice, we want sin(N*MYON_PI) to be zero in this evaluator.
  // The multiple 4 is used because we felt is was reasonable for somebody to want 
  // this evaluator to apply the same special case handing to a and a + 2.0*MYON_PI
  // when fabs(a) <= 2.0*MYON_PI.
  static const double sin_of_pi = fabs(sin(4.0*MYON_PI)) > fabs(sin(MYON_PI))
    ? fabs(sin(4.0*MYON_PI)) // the fabs(sin(4.0*MYON_PI)) values is used in the tests we performed.
    : fabs(sin(MYON_PI));

  static const double cos_of_pi_over_2 = fabs(cos(4.5*MYON_PI)) > fabs(cos(0.5*MYON_PI))
    ? fabs(cos(4.5*MYON_PI)) // the fabs(cos(4.5*MYON_PI)) values is used in the tests we performed.
    : fabs(cos(0.5*MYON_PI)); 

  MYON_3dVector d;
  bool rc = false;
  if ( m_t[0] < m_t[1] ) 
  {
    double rat = m_arc.DomainRadians().Length()/m_t.Length();
    double scale = 1.0;
    double a = m_arc.DomainRadians().ParameterAt( m_t.NormalizedParameterAt(t) );

    // 12 July 2012 Dale Lear
    //   When evaluating circles centered at the origin 
    //   a = MYON_PI = 3.1415926535897931, c = -1.0 and s = 1.2246467991473532e-016.
    //   As a result the y coordinates that "should" be zero comes out as
    //   radius*1.2246467991473532e-016.  When the radius is large (1.0e9 in the
    //   bug I was looking at), the y coordinate is big enough to cause other problems.
    //   When I added this comment I failed to insert the bug number, so I cannot
    //   provide more details as of May 6, 2014 and the changes for bug RH-26341.
    double c = cos(a);
    double s = sin(a);


    // This test turned out to be too crude.  The bug RH-26341
    // is one example.  The issue is that the trig function with the
    // largest derivative has more pecise sensitivity to changes in
    // angle and in orger to get as precise an evaluation as possible,
    // it is inportant to allow non-zero values of one trig function
    // even when the other is being rounded to +1 or -1.
    //
    ////if ( fabs(c) < MYON_EPSILON || fabs(s) > 1.0-MYON_EPSILON )
    ////{
    ////  c = 0.0;
    ////  s = s < 0.0 ? -1.0 : 1.0;
    ////}
    ////else if ( fabs(s) < MYON_EPSILON || fabs(c) > 1.0-MYON_EPSILON )
    ////{
    ////  s = 0.0;
    ////  c = c < 0.0 ? -1.0 : 1.0;
    ////}

    if (fabs(c) <= cos_of_pi_over_2)
    {
      c = 0.0;
      s = s < 0.0 ? -1.0 : 1.0;
    }
    else if (fabs(s) <= sin_of_pi)
    {
      s = 0.0;
      c = c < 0.0 ? -1.0 : 1.0;
    }

    c *= m_arc.radius;
    s *= m_arc.radius;

    MYON_3dPoint p = m_arc.plane.origin + c*m_arc.plane.xaxis + s*m_arc.plane.yaxis;
    v[0] = p.x;
    v[1] = p.y;
    if ( m_dim == 3 )
      v[2] = p.z;
    for ( int di = 1; di <= der_count; di++ ) {
      scale*=rat;
      a =  c;
      c = -s;
      s =  a;
      d =  c*m_arc.plane.xaxis + s*m_arc.plane.yaxis;
      v += v_stride;
      v[0] = d.x*scale;
      v[1] = d.y*scale;
      if ( m_dim == 3 )
        v[2] = d.z*scale;
    }
    rc = true;
  }
  return rc;
}

bool MYON_ArcCurve::Trim( const MYON_Interval& trimt )
{
  bool rc = false;
  if (trimt.IsIncreasing())
  {
    if (m_t.Includes(trimt, true))
    {
      MYON_Interval normt = m_t.NormalizedParameterAt(trimt);
      MYON_3dPoint S = PointAt(trimt[0]);
      MYON_3dPoint E = PointAt(trimt[1]);
      MYON_Interval angle = m_arc.DomainRadians().ParameterAt(normt);
      // 7-May-21.  GBA.   New definition of IsValid() enforced.
      // Resulting MYON_Arc and MYON_ArcCurve must pass IsValid()
      if (angle.Length() > MYON_ZERO_TOLERANCE && !S.IsCoincident(E)
        && m_arc.SetAngleIntervalRadians(angle))
      {
        m_t = trimt;
        DestroyCurveTree();
        rc = true;
      }
    }
    /* Allow trim where nothing is removed RH-64768 */
    else if (m_t == trimt)
      rc = true;
  }
  return rc;
}

bool MYON_ArcCurve::Extend(
  const MYON_Interval& domain
  )

{
  if (IsClosed()) return false;

  double s0, s1;
  bool changed = false;
  GetDomain(&s0, &s1);
  if (domain[0] < s0){
    s0 = domain[0];
    changed = true;
  }
  if (domain[1] > s1){
    s1 = domain[1];
    changed = true;
  }
  if (!changed) return false;

  DestroyCurveTree();

  double a0 = m_arc.Domain().ParameterAt(Domain().NormalizedParameterAt(s0));
  double a1 = m_arc.Domain().ParameterAt(Domain().NormalizedParameterAt(s1));
  if (a1 > a0+2.0*MYON_PI) {
    a1 = a0+2.0*MYON_PI;
    s1 = Domain().ParameterAt(m_arc.Domain().NormalizedParameterAt(a1));
  }
  m_arc.Trim(MYON_Interval(a0, a1));
  SetDomain(s0, s1);
  return true;
}

bool MYON_ArcCurve::Split(
    double t,
    MYON_Curve*& left_side,
    MYON_Curve*& right_side
  ) const
{
  // make sure t is strictly inside the arc's domain
  MYON_Interval arc_domain = Domain();
  MYON_Interval arc_angles = m_arc.DomainRadians();
  if ( !arc_domain.Includes(t) )
    return false;
  double a = (arc_domain == arc_angles)
           ? t
           : arc_angles.ParameterAt(arc_domain.NormalizedParameterAt(t));
  if ( !arc_angles.Includes(a) )
    return false;

  // make sure input curves are ok.
  MYON_ArcCurve* left_arc = 0;
  MYON_ArcCurve* right_arc = 0;

  if ( 0 != left_side )
  {
    if ( left_side == right_side )
      return false;
    left_arc = MYON_ArcCurve::Cast(left_side);
    if ( 0 == left_arc )
      return false;
    left_arc->DestroyCurveTree();
  }

  if ( 0 != right_side )
  {
    right_arc = MYON_ArcCurve::Cast(right_side);
    if ( 0 == right_arc )
      return false;
    right_arc->DestroyCurveTree();
  }

  if ( 0 == left_arc )
  {
    left_arc = new MYON_ArcCurve( *this );
  }
  else if ( this != left_arc )
  {
    left_arc->operator=(*this);
  }

  if ( 0 == right_arc )
  {
    right_arc = new MYON_ArcCurve( *this );
  }
  else if ( this != right_arc )
  {
    right_arc->operator=(*this);
  }

  bool rc = false;
  if ( this != left_arc )
  {
    rc = left_arc->Trim( MYON_Interval( arc_domain[0], t ) );
    if (rc)
     rc = right_arc->Trim( MYON_Interval( t, arc_domain[1] ) );
  }
  else
  {
    rc = right_arc->Trim( MYON_Interval( t, arc_domain[1] ) );
    if (rc)
      rc = left_arc->Trim( MYON_Interval( arc_domain[0], t ) );
  }

  if ( rc )
  {
    if ( 0 == left_side )
      left_side = left_arc;
    if ( 0 == right_side )
      right_side = right_arc;
  }
  else
  {
    if ( 0 == left_side && this != left_arc )
    {
      delete left_arc;
      left_arc = 0;
    }
    if ( 0 == right_side && this != right_arc )
    {
      delete right_arc;
      right_arc = 0;
    }
  }
  return rc;
}




static double ArcDeFuzz( double d )
{
  // 0.0078125 = 1.0/128.0 exactly
  // Using 2^n scale factors insures no loss of precision
  // but preserves fractional values that are multiples of 1/128.
  //
  // Fuzz tol should be scale * 2^m * MYON_EPSILON for m >= 1

  double f, i;
  f = modf( d*128.0, &i );
  if ( f != 0.0 && fabs(f) <= 1024.0*MYON_EPSILON ) {
    d = i*0.0078125;
  }  
  return d;
}

static bool NurbsCurveArc ( const MYON_Arc& arc, int dim, MYON_NurbsCurve& nurb )
{ 
  if ( !arc.IsValid() )
    return false;
  // makes a quadratic nurbs arc
  const MYON_3dPoint center = arc.Center();
  double angle = arc.AngleRadians();
  MYON_Interval dom = arc.DomainRadians();
  const double angle0 = dom[0];
  const double angle1 = dom[1];
  MYON_3dPoint start_point = arc.StartPoint();
  //MYON_3dPoint mid_point   = arc.PointAt(angle0 + 0.5*angle);
  MYON_3dPoint end_point   = arc.IsCircle() ? start_point : arc.EndPoint();

  MYON_4dPoint CV[9];
  double knot[10];

	double a, b, c, w, winv;
	double *cv;
	int    j, span_count, cv_count;

	a = (0.5 + MYON_SQRT_EPSILON)*MYON_PI;

	if (angle <= a)
		span_count = 1;
	else if (angle <= 2.0*a)
		span_count = 2;
	else if (angle <= 3.0*a)
		span_count = 4; // TODO - make a 3 span case
	else
		span_count = 4;

	cv_count = 2*span_count + 1;
	
	switch(span_count) {
	case 1:
    CV[0] = start_point;
    CV[1] = arc.PointAt(angle0 + 0.50*angle);
    CV[2] = end_point;
		break;
	case 2:
    CV[0] = start_point;
    CV[1] = arc.PointAt(angle0 + 0.25*angle);
    CV[2] = arc.PointAt(angle0 + 0.50*angle);
    CV[3] = arc.PointAt(angle0 + 0.75*angle);
    CV[4] = end_point;
		angle *= 0.5;
		break;
	default: // 4 spans
    CV[0] = start_point;
    CV[1] = arc.PointAt(angle0 + 0.125*angle);
    CV[2] = arc.PointAt(angle0 + 0.250*angle);
    CV[3] = arc.PointAt(angle0 + 0.375*angle);
    CV[4] = arc.PointAt(angle0 + 0.500*angle);
    CV[5] = arc.PointAt(angle0 + 0.625*angle);
    CV[6] = arc.PointAt(angle0 + 0.750*angle);
    CV[7] = arc.PointAt(angle0 + 0.875*angle);
    CV[8] = end_point;
		angle *= 0.25;
		break;
	}

	a = cos(0.5*angle);
	b = a - 1.0;
	//c = (radius > 0.0) ? radius*angle : angle;
  c = angle;

	span_count *= 2;
	knot[0] = knot[1] = angle0; //0.0;
	for (j = 1; j < span_count; j += 2) {
    CV[j].x += b * center.x;
    CV[j].y += b * center.y;
    CV[j].z += b * center.z;
    CV[j].w = a;
		CV[j+1].w = 1.0;
		knot[j+1] = knot[j+2] = knot[j-1] + c;
	}
  knot[cv_count-1] = knot[cv_count] = angle1;
  for ( j = 1; j < span_count; j += 2 ) {
    w = CV[j].w;
    winv = 1.0/w;
    a = CV[j].x*winv;
    b = ArcDeFuzz(a);
    if ( a != b ) {
      CV[j].x = b*w;
    }
    a = CV[j].y*winv;
    b = ArcDeFuzz(a);
    if ( a != b ) {
      CV[j].y = b*w;
    }
    a = CV[j].z*winv;
    b = ArcDeFuzz(a);
    if ( a != b ) {
      CV[j].z = b*w;
    }
  }

  nurb.m_dim = (dim==2) ? 2 : 3;
  nurb.m_is_rat = 1;
  nurb.m_order = 3;
  nurb.m_cv_count = cv_count;
  nurb.m_cv_stride = (dim==2 ? 3 : 4);
  nurb.ReserveCVCapacity( nurb.m_cv_stride*cv_count );
  nurb.ReserveKnotCapacity( cv_count+1 );
  for ( j = 0; j < cv_count; j++ ) {
    cv = nurb.CV(j);
    cv[0] = CV[j].x;
    cv[1] = CV[j].y;
    if ( dim == 2 ) {
      cv[2] = CV[j].w;
    }
    else {
      cv[2] = CV[j].z;
      cv[3] = CV[j].w;
    }
    nurb.m_knot[j] = knot[j];
  }
  nurb.m_knot[cv_count] = knot[cv_count];
  return true;
}


int MYON_Arc::GetNurbForm( MYON_NurbsCurve& nurbscurve ) const

{
  bool rc = NurbsCurveArc ( *this, 3, nurbscurve );
  return (rc) ? 2 : 0;
}

bool MYON_Arc::GetRadianFromNurbFormParameter(double NurbParameter, double* RadianParameter  ) const
{
	//  TRR#53994.
	// 16-Sept-09  Replaced this code so we dont use LocalClosestPoint.
	// In addition to being slower than neccessary the old method suffered from getting the
	// wrong answer at the seam of a full circle,  This probably only happened with large 
	// coordinates where many digits of precision get lost.

	MYON_NurbsCurve crv;
	
	if( !IsValid()|| RadianParameter==nullptr) 
		return false;

	MYON_Interval dom= Domain();

	if( fabs(NurbParameter- dom[0])<=2.0*MYON_EPSILON*fabs(dom[0]))
	{
		*RadianParameter=dom[0];
		return true;
	} 
	else if(  fabs(NurbParameter- dom[1])<=2.0*MYON_EPSILON*fabs(dom[1]))
	{
		*RadianParameter=dom[1];
		return true;
	}

	if( !dom.Includes(NurbParameter) )
		return false;

	if( !GetNurbForm(crv) )
		return false;
		
	MYON_3dPoint cp;
	cp = crv.PointAt(NurbParameter);
	cp -= Center();

	double x = MYON_DotProduct(Plane().Xaxis(), cp);
	double y = MYON_DotProduct(Plane().Yaxis(), cp);
	double theta = atan2(y,x);

	theta -= floor( (theta-dom[0])/(2*MYON_PI)) * 2* MYON_PI;
	if( theta<dom[0] || theta>dom[1])
	{
		// 24-May-2010 GBA 
		// We got outside of the domain because of a numerical error somewhere.
		// The only case that matters is because we are right near an endpoint.
		// So we need to decide which endpoint to return.  (Other possibilities 
		// are that the radius is way to small relative to the coordinates of the center.
		// In this case the circle is just numerical noise around the center anyway.)
		if( NurbParameter< (dom[0]+dom[1])/2.0)
			theta = dom[0];
		else 
			theta = dom[1];
	}


	// Carefully handle the potential discontinuity of this function
	//  when the domain is a full circle
	if(dom.Length()>.99999*2.0*MYON_PI)
	{
		double np_theta = dom.NormalizedParameterAt(theta);
		double np_nurb = dom.NormalizedParameterAt(NurbParameter);
		if( np_nurb<.01 && np_theta>.99)
			theta = dom[0];
		else if( np_nurb>.99 && np_theta<.01)
			theta = dom[1];
	}

	*RadianParameter = theta;

	return true;
}


bool MYON_Arc::GetNurbFormParameterFromRadian(double RadianParameter, double* NurbParameter ) const
{
	if(!IsValid() || NurbParameter==nullptr) 
		return false;

  MYON_Interval ADomain = DomainRadians();

  double endtol = 10.0*MYON_EPSILON*(fabs(ADomain[0]) + fabs(ADomain[1]));

  double del = RadianParameter - ADomain[0];
	if(del <= endtol && del >= -MYON_SQRT_EPSILON)
  {
		*NurbParameter=ADomain[0];
		return true;
	} 
  else {
    del = ADomain[1] - RadianParameter;
    if(del <= endtol && del >= -MYON_SQRT_EPSILON){
		  *NurbParameter=ADomain[1];
		  return true;
    }
	}

	if( !ADomain.Includes(RadianParameter ) )
		return false;


	MYON_NurbsCurve crv;

	if( !GetNurbForm(crv))
		return false;

	//Isolate a bezier that contains the solution
	int cnt = crv.SpanCount();	
	int si =0;	//get span index
	int ki=0;		//knot index
	double ang = ADomain[0];
	MYON_3dPoint cp;
	cp = crv.PointAt( crv.Knot(0) ) - Center();
	double x = MYON_DotProduct(Plane().Xaxis(),cp);
	double y = MYON_DotProduct(Plane().Yaxis(),cp);
	double at = atan2( y, x);	//todo make sure we dont go to far

	for( si=0, ki=0; si<cnt; si++, ki+=crv.KnotMultiplicity(ki) ){
		cp = crv.PointAt( crv.Knot(ki+2)) - Center();
		x = MYON_DotProduct(Plane().Xaxis(),cp);
		y = MYON_DotProduct(Plane().Yaxis(),cp);
		double at2 = atan2(y,x);
		if(at2>at)
			ang+=(at2-at);
		else
			ang += (2*MYON_PI + at2 - at);
		at = at2;
		if( ang>RadianParameter)
			break;
	} 

	// Crash Protection trr#55679
	if( ki+2>= crv.KnotCount())
	{
		 *NurbParameter=ADomain[1];
		 return true;		
	}
	MYON_Interval BezDomain(crv.Knot(ki), crv.Knot(ki+2));

	MYON_BezierCurve bez;
	if(!crv.ConvertSpanToBezier(ki,bez))
		return false;

 	MYON_Xform COC;
	COC.ChangeBasis( MYON_Plane(),Plane());   

	
	bez.Transform(COC);	// change coordinates to circles local frame
	double a[3];							// Bez coefficients of a quadratic to solve
	for(int i=0; i<3; i++)
		a[i] = tan(RadianParameter)* bez.CV(i)[0] - bez.CV(i)[1];

	//Solve the Quadratic
	double descrim = (a[1]*a[1]) - a[0]*a[2];
	double squared = a[0]-2*a[1]+a[2];
	double tbez;
	if(fabs(squared)> MYON_ZERO_TOLERANCE){
		MYON_ASSERT(descrim>=0);
		descrim = sqrt(descrim);
		tbez = (a[0]-a[1] + descrim)/(a[0]-2*a[1]+a[2]);
		if( tbez<0 || tbez>1){
			double tbez2 = (a[0]-a[1]-descrim)/(a[0] - 2*a[1] + a[2]);
			if( fabs(tbez2 - .5)<fabs(tbez-.5) )
				tbez = tbez2;
		}

		MYON_ASSERT(tbez>=-MYON_ZERO_TOLERANCE && tbez<=1+MYON_ZERO_TOLERANCE);
	}
	else{
		// Quadratic degenerates to linear
		tbez = 1.0;
		if(a[0]-a[2])
			tbez = a[0]/(a[0]-a[2]);
	}	
	if(tbez<0)
		tbez=0.0;
	else if(tbez>1.0)
		tbez=1.0;


		//Debug MYONLY Code  - check the result
//		double aa = a[0]*(1-tbez)*(1-tbez)  + 2*a[1]*tbez*(1-tbez) + a[2]*tbez*tbez;
//		double tantheta= tan(RadianParameter);
//		MYON_3dPoint bezp;
//		bez.Evaluate(tbez, 0, 3, bezp);
//		double yx = bezp.y/bezp.x;


	*NurbParameter = BezDomain.ParameterAt(tbez);
	return true;

}


int MYON_ArcCurve::GetNurbForm( // returns 0: unable to create NURBS representation
                 //            with desired accuracy.
                 //         1: success - returned NURBS parameterization
                 //            matches the curve's to wthe desired accuracy
                 //         2: success - returned NURBS point locus matches
                 //            the curve's to the desired accuracy but, on
                 //            the interior of the curve's domain, the 
                 //            curve's parameterization and the NURBS
                 //            parameterization may not match to the 
                 //            desired accuracy.
      MYON_NurbsCurve& c,
      double tolerance,
      const MYON_Interval* subdomain  // OPTIONAL subdomain of arc
      ) const
{
  int rc = 0;
  if ( subdomain ) 
  {
    MYON_ArcCurve trimmed_arc(*this);
    if ( trimmed_arc.Trim(*subdomain) ) 
    {
      rc = trimmed_arc.GetNurbForm( c, tolerance, nullptr );
    }
  }
  else if ( m_t.IsIncreasing() && m_arc.IsValid() ) 
  {
    if ( NurbsCurveArc( m_arc, m_dim, c ) )
    {
      rc = 2;
      c.SetDomain( m_t[0], m_t[1] );
    }
  }
  return rc;
}

int MYON_ArcCurve::HasNurbForm( // returns 0: unable to create NURBS representation
                 //            with desired accuracy.
                 //         1: success - returned NURBS parameterization
                 //            matches the curve's to wthe desired accuracy
                 //         2: success - returned NURBS point locus matches
                 //            the curve's to the desired accuracy but, on
                 //            the interior of the curve's domain, the 
                 //            curve's parameterization and the NURBS
                 //            parameterization may not match to the 
                 //            desired accuracy.
                 ) const

{
  if (!IsValid())
    return 0;
  return 2;
}

bool MYON_ArcCurve::GetCurveParameterFromNurbFormParameter(
      double nurbs_t,
      double* curve_t
      ) const
{
  double radians;

  double arcnurb_t = m_arc.DomainRadians().ParameterAt(m_t.NormalizedParameterAt(nurbs_t));

  bool rc = m_arc.GetRadianFromNurbFormParameter(arcnurb_t,&radians);
  *curve_t = m_t.ParameterAt( m_arc.DomainRadians().NormalizedParameterAt(radians) );
  
  return rc;
}

bool MYON_ArcCurve::GetNurbFormParameterFromCurveParameter(
      double curve_t,
      double* nurbs_t
      ) const
{
  double radians = m_arc.DomainRadians().ParameterAt(m_t.NormalizedParameterAt(curve_t));
  double arcnurb_t;
  bool rc = m_arc.GetNurbFormParameterFromRadian(radians,&arcnurb_t);
  if (rc)        // Oct 29, 2009 - Dale Lear added condition to set *nurbs_t = curve_t
    *nurbs_t = m_t.ParameterAt(m_arc.DomainRadians().NormalizedParameterAt(arcnurb_t));
  else
    *nurbs_t = curve_t;
  return rc;
}

bool MYON_ArcCurve::IsCircle() const
{
  return m_arc.IsCircle() ? true : false;
}

double MYON_ArcCurve::Radius() const
{
	return m_arc.Radius();
}
  
double MYON_ArcCurve::AngleRadians() const
{
	return m_arc.AngleRadians();
}

double MYON_ArcCurve::AngleDegrees() const
{
	return m_arc.AngleDegrees();
}

/*
Description:
  MYON_CircleCurve is obsolete.  
  This code exists so v2 files can be read.
*/
class MYON__OBSOLETE__CircleCurve : public MYON_ArcCurve
{
public: 
  static const MYON_ClassId m_MYON__OBSOLETE__CircleCurve_class_rtti;
  const MYON_ClassId* ClassId() const;
  bool Read(
         MYON_BinaryArchive&  // open binary file
       );
};

static MYON_Object* CreateNewON_CircleCurve() 
{

  // must create an MYON_CircleCurve so virtual 
  // MYON_CircleCurve::Read will be used to read
  // archive objects with uuid CF33BE29-09B4-11d4-BFFB-0010830122F0
  return new MYON__OBSOLETE__CircleCurve();
} 

const MYON_ClassId MYON__OBSOLETE__CircleCurve::m_MYON__OBSOLETE__CircleCurve_class_rtti(
    "MYON__OBSOLETE__CircleCurve",
    "MYON_ArcCurve",
    CreateNewON_CircleCurve,
    "CF33BE29-09B4-11d4-BFFB-0010830122F0"
    );

const MYON_ClassId* MYON__OBSOLETE__CircleCurve::ClassId() const 
{
  // so write will save MYON_ArcCurve uuid
  return &MYON_CLASS_RTTI(MYON_ArcCurve);
}

bool MYON__OBSOLETE__CircleCurve::Read(
       MYON_BinaryArchive& file // open binary file
     )
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc)
  {
    if (major_version==1) 
    {
      // common to all 1.x versions
      MYON_Circle circle;
      rc = file.ReadCircle( circle );
      m_arc = circle;
      if (rc) 
        rc = file.ReadInterval( m_t );
      if (rc) 
        rc = file.ReadInt(&m_dim);
      if ( m_dim != 2 && m_dim != 3 )
        m_dim = 3;
    }
  }

  return rc;
}

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

MYON_VIRTUAL_OBJECT_IMPLEMENT(MYON_Curve,MYON_Geometry,"4ED7D4D7-E947-11d3-BFE5-0010830122F0");

MYON_Curve::MYON_Curve() MYON_NOEXCEPT
  : MYON_Geometry()
{}

MYON_Curve::MYON_Curve(const MYON_Curve& src)
  : MYON_Geometry(src)
{}

MYON_Curve& MYON_Curve::operator=(const MYON_Curve& src)
{
  if ( this != &src ) 
  {
    this->DestroyCurveTree();
    MYON_Geometry::operator=(src);
  }
  return *this;
}


#if defined(MYON_HAS_RVALUEREF)
MYON_Curve::MYON_Curve( MYON_Curve&& src ) MYON_NOEXCEPT
  : MYON_Geometry(std::move(src))
{
}

MYON_Curve& MYON_Curve::operator=( MYON_Curve&& src )
{
  if ( this != &src ) 
  {
    this->DestroyCurveTree();
    MYON_Geometry::operator=(std::move(src));
  }
  return *this;
}
#endif


MYON_Curve::~MYON_Curve()
{
  // Do not call the (virtual) DestroyRuntimeCache or 
  // DestroyCurveTree (which calls DestroyRuntimeCache()
  // because it opens the potential for crashes in a
  // "dirty" destructors of classes derived from MYON_Curve
  // that to not use DestroyRuntimeCache() in their
  // destructors and to not set deleted pointers to zero.
}

unsigned int MYON_Curve::SizeOf() const
{
  unsigned int sz = MYON_Geometry::SizeOf();
  sz += (sizeof(*this) - sizeof(MYON_Geometry));
  // Currently, the size of m_ctree is not included
  // because this is cached runtime information.
  // Applications that care about object size are 
  // typically storing "inactive" objects for potential
  // future use and should call DestroyRuntimeCache(true)
  // to remove any runtime cache information.
  return sz;
}


MYON_Curve* MYON_Curve::DuplicateCurve() const
{
  // MYON_CurveProxy overrides this virtual function.
  return Duplicate();
}


MYON::object_type MYON_Curve::ObjectType() const
{
  return MYON::curve_object;
}


bool MYON_Curve::GetDomain(double* s0,double* s1) const
{
  bool rc = false;
  MYON_Interval d = Domain();
  if ( d.IsIncreasing() ) {
    if(s0) *s0 = d.Min();
    if (s1) *s1 = d.Max();
    rc = true;
  }
  return rc;
}

void MYON_Curve::DestroyCurveTree()
{
  DestroyRuntimeCache(true);
}

bool MYON_Curve::GetTightBoundingBox( 
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

  // In general, putting start and end point in the box lets me avoid
  // testing lots of nodes.
  MYON_3dPoint P = PointAtStart();
  if ( xform )
    P = (*xform)*P;
  tight_bbox.Set( P, bGrowBox );
  bGrowBox = true;

  P = PointAtEnd();
  if ( xform )
    P = (*xform)*P;
  tight_bbox.Set( P, bGrowBox );

  MYON_BoundingBox curve_bbox = BoundingBox();
  if ( MYON_WorldBBoxIsInTightBBox( tight_bbox, curve_bbox, xform ) )
  {
    // Curve is inside tight_bbox
    return true;
  }


  MYON_NurbsCurve N;
  if ( 0 == GetNurbForm(N) )
    return false;
  if ( N.m_order < 2 || N.m_cv_count < N.m_order )
    return false;

  MYON_BezierCurve B;
  for ( int span_index = 0; span_index <= N.m_cv_count - N.m_order; span_index++ )
  {
    if ( !(N.m_knot[span_index + N.m_order-2] < N.m_knot[span_index + N.m_order-1]) )
      continue;
    if ( !N.ConvertSpanToBezier( span_index, B ) )
      continue;
    if ( !B.GetTightBoundingBox(tight_bbox,bGrowBox,xform) )
      continue;
    bGrowBox = true;
  }


  return (0!=bGrowBox);
}

// overrides virtual MYON_Geometry::Transform()
bool MYON_Curve::Transform( 
  const MYON_Xform& xform
  )
{
  if ( !this->MYON_Geometry::Transform(xform) )
    return false;
	this->DestroyCurveTree();
  return true;
}


bool MYON_Curve::SetDomain( MYON_Interval domain )
{
  return ( domain.IsIncreasing() && SetDomain( domain[0], domain[1] )) ? true : false;
}


bool MYON_Curve::SetDomain( double, double )
{
  // this virtual function is overridden by curves that can change their domain.
  return false;
}

bool MYON_Curve::ChangeClosedCurveSeam( double t, double min_dist)
{
  MYON_3dPoint P = PointAt(t);
  if (min_dist <= 0.0 || P.DistanceTo(PointAtStart()) >= min_dist)
    return ChangeClosedCurveSeam(t);
  return false;
}

bool MYON_Curve::ChangeClosedCurveSeam( double t )
{
  // this virtual function is overridden by curves that can be closed
  return false;
}

//virtual
bool MYON_Curve::ChangeDimension( int desired_dimension )
{
  return (desired_dimension > 0 && desired_dimension == Dimension() );
}


//virtual
bool MYON_Curve::GetSpanVectorIndex(
       double t,                // [IN] t = evaluation parameter
       int side,                // [IN] side 0 = default, -1 = from below, +1 = from above
       int* span_vector_i,      // [OUT] span vector index
       MYON_Interval* span_domain // [OUT] domain of the span containing "t"
       ) const
{
  bool rc = false;
  int i;
  int span_count = SpanCount();
  if ( span_count > 0 ) {
    double* span_vector = (double*)onmalloc((span_count+1)*sizeof(span_vector[0]));
    rc = GetSpanVector( span_vector );
    if (rc) {
      i = MYON_NurbsSpanIndex( 2, span_count+1, span_vector, t, side, 0 );
      if ( i >= 0 && i < span_count ) {
        if ( span_vector_i )
          *span_vector_i = i;
        if ( span_domain )
          span_domain->Set( span_vector[i], span_vector[i+1] );
      }
      else
        rc = false;
    }
    onfree(span_vector);
  }
  return rc;
}


bool MYON_Curve::GetParameterTolerance( // returns tminus < tplus: parameters tminus <= s <= tplus
       double t,       // t = parameter in domain
       double* tminus, // tminus
       double* tplus   // tplus
       ) const
{
  bool rc = false;
  MYON_Interval d = Domain();
  if ( d.IsIncreasing() )
    rc = MYON_GetParameterTolerance( d[0], d[1], t, tminus, tplus );
  return rc;
}

int MYON_Curve::IsPolyline(
      MYON_SimpleArray<MYON_3dPoint>* pline_points, // default = nullptr
      MYON_SimpleArray<double>* pline_t // default = nullptr
      ) const
{
  // virtual function that is overridden
  return 0;
}


bool MYON_Curve::IsLinear( double tolerance ) const
{
  bool rc = false;
  if ( Dimension() == 2 || Dimension() == 3 ) {
    const int span_count = SpanCount();
    const int span_degree = Degree();
    if ( span_count > 0 ) {
      MYON_SimpleArray<double> s(span_count+1);
      s.SetCount(span_count+1);
      if ( GetSpanVector( s.Array() ) ) {
        if ( tolerance == 0.0 )
          tolerance = MYON_ZERO_TOLERANCE;
        MYON_Line line( PointAtStart(), PointAtEnd() );
        if ( line.Length() > tolerance ) {
          double t, t0, d, delta;
          MYON_Interval sp;
          int n, i, span_index;
          rc = true;
          t0 = 0;              //  Domain()[0];
          MYON_3dPoint P;

          for ( span_index = 0; span_index < span_count; span_index++ ) {
            sp.Set( s[span_index], s[span_index+1] );
            n = 2*span_degree+1;
            delta = 1.0/n;
            for ( i = (span_index)?0:1; i < n; i++ ) {
              P = PointAt( sp.ParameterAt(i*delta) );
              if ( !line.ClosestPointTo( P, &t ) )
                rc = false;
              else if ( t < t0 )
                rc = false;
							else if (t > 1.0 + MYON_SQRT_EPSILON)
								rc = false;
              d = P.DistanceTo( line.PointAt(t) );
              if ( d > tolerance )
                rc = false;
              t0 = t;
            }
          }
        }
      }
    }
  }
  return rc;
}

bool MYON_Curve::IsEllipse(
    const MYON_Plane* plane,
    MYON_Ellipse* ellipse,
    double tolerance
    ) const
{
  // virtual function
  MYON_Arc arc;
  bool rc = IsArc(plane,&arc,tolerance)?true:false;
  if (rc && ellipse)
  {
    ellipse->plane = arc.plane;
    ellipse->radius[0] = arc.radius;
    ellipse->radius[1] = arc.radius;
  }
  return rc;
}

bool MYON_Curve::IsArcAt( 
  double t, 
  const MYON_Plane* plane,
  MYON_Arc* arc,
  double tolerance,
  double* t0, 
  double* t1
  ) const
{
  double k, k0, k1;
  int hint;
  if ( !GetDomain(&k0,&k1) )
    return false;
  if ( 0 != t0 )
    *t0 = k0;
  if ( 0 != t1 )
    *t1 = k1;
  if ( !MYON_IsValid(t) )
    return false;
  if ( ! (t <= k1) )
    return false;

  if ( IsArc(plane,arc,tolerance) )
    return true; // entire curve is an arc

  // check sub-segments
  hint = 0;
  for ( k = k0; k0 <= t && GetNextDiscontinuity(MYON::continuity::G2_locus_continuous, k0, k1, &k, &hint); k0 = k )
  {
    if ( !(k > k0) )
      break; // sanity check to prevent infinite loops
    if( t <= k )
    {
      if ( 0 != t0 )
        *t0 = k0;
      if ( 0 != t1 )
        *t1 = k1;
      MYON_CurveProxy subcrv(this,MYON_Interval(k0,k));
      if ( subcrv.IsArc(plane,arc,tolerance) )
        return true;

      // NOTE WELL: 
      //   When t == k, we need to check the next segment as well
      //   (happens when t is the parameter between a line and arc segment.)
      //   The "k0 <= t" test is in the for() condition will
      //   terminate the loop when t < k
    }
  }

  return false;
}


bool MYON_Curve::IsArc( const MYON_Plane* plane, MYON_Arc* arc, double tolerance ) const
{
  bool rc = false;
  double c0, c, t, delta;
  int n, i, span_index;
  MYON_Plane pln;
  MYON_Arc a;
  MYON_3dPoint P, C;

  if ( !plane ) {
    if ( !IsPlanar(&pln,tolerance) )
      return false;
    plane = &pln;
  }
  if ( !arc )
    arc = &a;
  const int span_count = SpanCount();
  const int span_degree = Degree();
  if ( span_count < 1 )
    return false;
  MYON_SimpleArray<double> d(span_count+1);
  d.SetCount(span_count+1);
  if ( !GetSpanVector(d.Array()) )
    return false;

  const bool bIsClosed = IsClosed();

  MYON_3dPoint P0 = PointAt( d[0] );
  t = bIsClosed ? 0.5*d[0] + 0.5*d[span_count] : d[span_count];
  MYON_3dPoint P1 = PointAt( 0.5*d[0] + 0.5*t );
  MYON_3dPoint P2 = PointAt( t );

  if ( !arc->Create(P0,P1,P2) )
    return false;

  if ( bIsClosed )
    arc->SetAngleRadians(2.0*MYON_PI);

  MYON_Interval arc_domain = arc->Domain();
  MYON_3dPoint A0 = arc->PointAt(arc_domain[0]);
  MYON_3dPoint A1 = arc->PointAt(arc_domain[1]);
  MYON_3dPoint C0 = PointAtStart();
  MYON_3dPoint C1 = PointAtEnd();
  if (    false == MYON_PointsAreCoincident(3,0,&A0.x,&C0.x) 
       || false == MYON_PointsAreCoincident(3,0,&A1.x,&C1.x) 
     )
  {
    return false;
  }


  if ( tolerance == 0.0 )
    tolerance = MYON_ZERO_TOLERANCE;
  rc = true;
  c0 = 0.0;
  for ( span_index = 0; rc && span_index < span_count; span_index++ ) {
    n = 2*span_degree+1;
    if ( n < 4 )
      n = 4;
    delta = 1.0/n;
    for ( i = 0; i < n; i++ ) {
      t = i*delta;
      P = PointAt( (1.0-t)*d[span_index] + t*d[span_index+1] );
      if ( !arc->ClosestPointTo(P,&c) ) {
        rc = false;
        break;        
      }
      if ( c < c0 ) {
        rc = false;
        break;
      }
      C = arc->PointAt(c);        
      if ( C.DistanceTo(P) > tolerance ) {
        rc = 0;
        break;
      }
      c0 = c;
    }
  }

  return rc;
}

bool MYON_Curve::IsPlanar( MYON_Plane* plane, double tolerance ) const
{
  bool rc = false;
  const int dim = Dimension();
  if ( dim == 2 ) 
  {
    // all 2d curves use this code to set the plane
    // so that there is consistent behavior.
    rc = true;
    if ( plane )
    {
      *plane = MYON_xy_plane;
      //plane->CreateFromFrame( PointAtStart(), MYON_3dVector::XAxis, MYON_3dVector::YAxis );
    }
  }
  else if ( IsLinear(tolerance) )
  {
    rc = true;
    if ( plane )
    {
      MYON_Line line( PointAtStart(), PointAtEnd() );
      if ( !line.InPlane( *plane, tolerance ) )
        line.InPlane( *plane, 0.0 );
    }
  }
  else if ( dim == 3 ) 
  {
    const int span_count = SpanCount();
    if ( span_count < 1 )
      return false;
    const int span_degree = Degree();
    if ( span_degree < 1 )
      return false;
    MYON_SimpleArray<double> s(span_count+1);
    s.SetCount(span_count+1);
    if ( !GetSpanVector(s.Array()) )
      return false;
    MYON_Interval d = Domain();

    // use initial point, tangent, and evaluated spans to guess a plane
    MYON_3dPoint pt = PointAt(d.ParameterAt(0.0));
    MYON_3dVector x = TangentAt(d.ParameterAt(0.0));
    if ( x.Length() < 0.95 ) {
      return false;
    }
    int n = (span_degree > 1) ? span_degree+1 : span_degree;
    double delta = 1.0/n;
    int i, span_index, hint = 0;
    MYON_3dPoint q;
    MYON_3dVector y;
    bool bNeedY = true;
    for ( span_index = 0; span_index < span_count && bNeedY; span_index++ ) {
      d.Set(s[span_index],s[span_index+1]);
      for ( i = span_index ? 0 : 1; i < n && bNeedY; i++ ) {
        if ( !EvPoint( d.ParameterAt(i*delta), q, 0, &hint ) )
          return false;
        y = q-pt;
        y = y - (y*x)*x;
        bNeedY = ( y.Length()  <= 1.0e-6 );
      }
    }
    if ( bNeedY )
      y.PerpendicularTo(x);
    MYON_Plane pln( pt, x, y );
    if ( plane )
      *plane = pln;

    // test
    rc = true;
    n = 2*span_degree + 1;
    delta = 1.0/n;
    double h = pln.plane_equation.ValueAt(PointAtEnd());
    if ( fabs(h) > tolerance )
      rc = false;
    hint = 0;
    for ( span_index = 0; rc && span_index < span_count; span_index++ ) {
      d.Set(s[span_index],s[span_index+1]);
      for ( i = 0; rc && i < n; i++ ) {
        if ( !EvPoint( d.ParameterAt(i*delta), q, 0, &hint ) )
          rc = false;
        else {
          h = pln.plane_equation.ValueAt(q);
          if ( fabs(h) > tolerance )
            rc = false;
        }
      }
    }
  }
  return rc;
}

bool MYON_Curve::IsClosed() const
{
  bool rc = false;
  double *a, *b, *c, *p, w[12];
  const int dim = Dimension();
  a = 0;
  if ( dim > 1 ) 
  {
    MYON_Interval d = Domain();
    a = (dim>3) ? (double*)onmalloc(dim*4*sizeof(*a)) : w;
    b = a+dim;
    c = b+dim;
    p = c+dim;
    if (    Evaluate( d.ParameterAt(0.0), 0, dim, a, 1 )
         && Evaluate( d.ParameterAt(1.0), 0, dim, p,-1 ) 
       )
    {
      // Note:  The point compare test should be the same
      //        as the one used in MYON_PolyCurve::HasGap().
      // June 2019 - sometime in the past decade MYON_PolyCurve::HasGap()
      // changed and the test there is different from this test.
      // The initial "Note" no longer applies becaue it's no longer
      // clear why the current MYON_PolyCurve::HasGap() was changed.
      if ( MYON_PointsAreCoincident( dim, false, a, p ) ) 
      {
        if (    Evaluate( d.ParameterAt(1.0/3.0), 0, dim, b, 0 ) 
             && Evaluate( d.ParameterAt(2.0/3.0), 0, dim, c, 0 )
           )
        {
          if (    false == MYON_PointsAreCoincident( dim, false, a, b ) 
               && false == MYON_PointsAreCoincident( dim, false, a, c ) 
               && false == MYON_PointsAreCoincident( dim, false, p, b ) 
               && false == MYON_PointsAreCoincident( dim, false, p, c ) 
             )
          {
            rc = true;
          }
        }
      }
    }
    if ( dim > 3 && 0 != a )
      onfree(a);
  }

  return rc;
}


bool MYON_Curve::IsPeriodic() const
{
  // curve types that may be periodic override this virtual function
  return false;
}

bool MYON_Curve::GetNextDiscontinuity( 
                MYON::continuity c,
                double t0,
                double t1,
                double* t,
                int* hint,
                int* dtype,
                double cos_angle_tolerance,
                double curvature_tolerance
                ) const
{
  // this function must be overridden by curve objects that
  // can have parametric discontinuities on the interior of the curve.

  bool rc = false;

  if ( dtype )
    *dtype = 0;
  
  if ( t0 != t1 )
  {
    bool bTestC0 = false;
    bool bTestD1 = false;
    bool bTestD2 = false;
    bool bTestT = false;
    bool bTestK = false;
    switch(c)
    {
    case MYON::continuity::C0_locus_continuous:
      bTestC0 = true;
      break;
    case MYON::continuity::C1_locus_continuous:
      bTestC0 = true;
      bTestD1 = true;
      break;
    case MYON::continuity::C2_locus_continuous:
      bTestC0 = true;
      bTestD1 = true;
      bTestD2 = true;
      break;
    case MYON::continuity::G1_locus_continuous:
      bTestC0 = true;
      bTestT  = true;
      break;
    case MYON::continuity::G2_locus_continuous:
      bTestC0 = true;
      bTestT  = true;
      bTestK  = true;
      break;
    default:
      // other values ignored on purpose.
      break;
    }

    if ( bTestC0 )
    {
      // 20 March 2003 Dale Lear:
      //   Have to look for locus discontinuities at ends.
      //   Must test both ends becuase t0 > t1 is valid input.
      //   In particular, for MYON_CurveProxy::GetNextDiscontinuity() 
      //   to work correctly on reversed "real" curves, the 
      //   t0 > t1 must work right.
      MYON_Interval domain = Domain();

      if ( t0 < domain[1] && t1 >= domain[1] )
        t1 = domain[1];
      else if ( t0 > domain[0] && t1 <= domain[0] )
        t1 = domain[0];

      if ( (t0 < domain[1] && t1 >= domain[1]) || (t0 > domain[0] && t1 <= domain[0]) )
      {
        if ( IsClosed() )
        {
          if ( bTestD1 || bTestT )
          {
            // need to check locus continuity at start/end of closed curve.
            MYON_3dPoint Pa, Pb;
            MYON_3dVector D1a, D1b, D2a, D2b;
            if (    Ev2Der(domain[0],Pa,D1a,D2a,1,nullptr) 
                 && Ev2Der(domain[1],Pb,D1b,D2b,-1,nullptr) )
            {
              Pb = Pa; // IsClosed() = true means assume Pa=Pb;
              if ( bTestD1 )
              {
                if ( !(D1a-D1b).IsTiny(D1b.MaximumCoordinate()*MYON_SQRT_EPSILON ) )
                {
                  if ( dtype )
                    *dtype = 1;
                  *t = t1;
                  rc = true;
                }
                else if ( bTestD2 && !(D2a-D2b).IsTiny(D2b.MaximumCoordinate()*MYON_SQRT_EPSILON) )
                {
                  if ( dtype )
                    *dtype = 2;
                  *t = t1;
                  rc = true;
                }

              }
              else if ( bTestT )
              {
                MYON_3dVector Ta, Tb, Ka, Kb;
                MYON_EvCurvature( D1a, D2a, Ta, Ka );
                MYON_EvCurvature( D1b, D2b, Tb, Kb );
                if ( Ta*Tb < cos_angle_tolerance )
                {
                  if ( dtype )
                    *dtype = 1;
                  *t = t1;
                  rc = true;
                }
                else if ( bTestK )
                {
                  // NOTE: 
                  //  This test must exactly match the one
                  //  used in MYON_NurbsCurve::GetNextDiscontinuity()
                  if ( !MYON_IsG2CurvatureContinuous( Ka, Kb,
                                                    cos_angle_tolerance,
                                                    curvature_tolerance
                                                    )
                     )
                  {
                    if ( dtype )
                      *dtype = 2;
                    *t = t1;
                    rc = true;
                  }
                }
              }
            }
          }
        }
        else
        {
          // open curves are not locus continuous at ends.
          if (dtype )
            *dtype = 0; // locus C0 discontinuity
          *t = t1;
          rc = true;
        }
      }
    }
  }

  return rc;
}



bool MYON_Curve::IsContinuous(
    MYON::continuity desired_continuity,
    double t, 
    int* hint, // default = nullptr,
    double point_tolerance, // default=MYON_ZERO_TOLERANCE
    double d1_tolerance, // default==MYON_ZERO_TOLERANCE
    double d2_tolerance, // default==MYON_ZERO_TOLERANCE
    double cos_angle_tolerance, // default==MYON_DEFAULT_ANGLE_TOLERANCE_COSINE
    double curvature_tolerance  // default==MYON_SQRT_EPSILON
    ) const
{
  MYON_Interval domain = Domain();
  if ( !domain.IsIncreasing() )
  {
    return true;
  }

  MYON_3dPoint Pm, Pp;
  MYON_3dVector D1m, D1p, D2m, D2p, Tm, Tp, Km, Kp;

  bool bIsClosed = false;

  // 20 March 2003 Dale Lear
  //     I added this preable to handle the new
  //     locus continuity values.
  double t0 = t;
  double t1 = t;
  switch(desired_continuity)
  {
  case MYON::continuity::C0_locus_continuous:
  case MYON::continuity::C1_locus_continuous:
  case MYON::continuity::C2_locus_continuous:
  case MYON::continuity::G1_locus_continuous:
  case MYON::continuity::G2_locus_continuous:
    if ( t <= domain[0] )
    {
      // By convention - see comments by MYON::continuity enum.
      return true;
    }
    if ( t == domain[1] )
    {
      if ( !IsClosed() )
      {
        // open curves are not locus continuous at the end parameter
        // see comments by MYON::continuity enum
        return false;
      }
      else 
      {
        if ( MYON::continuity::C0_locus_continuous == desired_continuity )
        {
          return true;
        }
        bIsClosed = true;
      }

      t0 = domain[0];
      t1 = domain[1];
    }
    break;

  case MYON::continuity::unknown_continuity:
  case MYON::continuity::C0_continuous:
  case MYON::continuity::C1_continuous:
  case MYON::continuity::C2_continuous:
  case MYON::continuity::G1_continuous:
  case MYON::continuity::G2_continuous:
  case MYON::continuity::Cinfinity_continuous:
  case MYON::continuity::Gsmooth_continuous:
  default:
    // does not change pre 20 March behavior - just skips the out
    // of domain evaluation on parametric queries.
    if ( t <= domain[0] || t >= domain[1] )
      return true;
    break;
  }

  // at this point, no difference between parametric and locus tests.
  desired_continuity = MYON::ParametricContinuity((int)desired_continuity);


  // this is slow and uses evaluation
  // virtual overrides on curve classes that can have multiple spans
  // are much faster because the avoid evaluation
  switch ( desired_continuity )
  {
  case MYON::continuity::unknown_continuity:
    break;

  case MYON::continuity::C0_continuous:  
    if ( !EvPoint( t1, Pm, -1, hint ) )
      return false;
    if ( !EvPoint( t0, Pp,  1, hint ) )
      return false;
    if ( bIsClosed )
      Pm = Pp;
    if ( !(Pm-Pp).IsTiny(point_tolerance) )
      return false;
    break;

  case MYON::continuity::C1_continuous:
    if ( !Ev1Der( t1, Pm, D1m, -1, hint ) )
      return false;
    if ( !Ev1Der( t0, Pp, D1p,  1, hint ) )
      return false;
    if ( bIsClosed )
      Pm = Pp;
    if ( !(Pm-Pp).IsTiny(point_tolerance) || !(D1m-D1p).IsTiny(d1_tolerance) )
      return false;
    break;

  case MYON::continuity::G1_continuous:
    if ( !EvTangent( t1, Pm, Tm, -1, hint ) )
      return false;
    if ( !EvTangent( t0, Pp, Tp,  1, hint ) )
      return false;
    if ( bIsClosed )
      Pm = Pp;
    if ( !(Pm-Pp).IsTiny(point_tolerance) || Tm*Tp < cos_angle_tolerance )
      return false;
    break;

  case MYON::continuity::C2_continuous:
    if ( !Ev2Der( t1, Pm, D1m, D2m, -1, hint ) )
      return false;
    if ( !Ev2Der( t0, Pp, D1p, D2p,  1, hint ) )
      return false;
    if ( bIsClosed )
      Pm = Pp;
    if ( !(Pm-Pp).IsTiny(point_tolerance) || !(D1m-D1p).IsTiny(d1_tolerance) || !(D2m-D2p).IsTiny(d2_tolerance) )
      return false;
    break;

  case MYON::continuity::G2_continuous:
  case MYON::continuity::Gsmooth_continuous:
    if ( !EvCurvature( t1, Pm, Tm, Km, -1, hint ) )
      return false;
    if ( !EvCurvature( t0, Pp, Tp, Kp,  1, hint ) )
      return false;
    if ( !bIsClosed && !(Pm-Pp).IsTiny(point_tolerance) )
      return false;
    if ( Tm*Tp < cos_angle_tolerance )
      return false; // tangent discontinuity

    if ( desired_continuity == MYON::continuity::Gsmooth_continuous )
    {
      if ( !MYON_IsGsmoothCurvatureContinuous(Km,Kp,cos_angle_tolerance,curvature_tolerance) )
        return false;
    }
    else
    {
      if ( !MYON_IsG2CurvatureContinuous(Km,Kp,cos_angle_tolerance,curvature_tolerance) )
        return false;
    }
    break;

  case MYON::continuity::C0_locus_continuous:
  case MYON::continuity::C1_locus_continuous:
  case MYON::continuity::C2_locus_continuous:
  case MYON::continuity::G1_locus_continuous:
  case MYON::continuity::G2_locus_continuous:
  case MYON::continuity::Cinfinity_continuous:
    break;
  }

  return true;
}


MYON_3dPoint MYON_Curve::PointAt( double t ) const
{
  MYON_3dPoint p(0.0,0.0,0.0);
  if ( !EvPoint(t,p) )
    p = MYON_3dPoint::UnsetPoint;
  return p;
}

MYON_3dPoint MYON_Curve::PointAtStart() const
{
  return PointAt(Domain().Min());
}

MYON_3dPoint MYON_Curve::PointAtEnd() const
{
  return PointAt(Domain().Max());
}


bool MYON_Curve::SetStartPoint(MYON_3dPoint start_point)
{
  MYON_3dPoint S = PointAtStart();
  return (S == start_point) ? true : false;
}

bool MYON_Curve::SetEndPoint(MYON_3dPoint end_point)
{
  MYON_3dPoint E = PointAtEnd();
  return (E == end_point) ? true : false;
}

MYON_3dVector MYON_Curve::DerivativeAt( double t ) const
{
  MYON_3dPoint  p(0.0,0.0,0.0);
  MYON_3dVector d(0.0,0.0,0.0);
  Ev1Der(t,p,d);
  return d;
}

MYON_3dVector MYON_Curve::TangentAt( double t ) const
{
  MYON_3dPoint point;
  MYON_3dVector tangent;
  EvTangent( t, point, tangent );
  return tangent;
}

MYON_3dVector MYON_Curve::CurvatureAt( double t ) const
{
  MYON_3dPoint point;
  MYON_3dVector tangent, kappa;
  EvCurvature( t, point, tangent, kappa );
  return kappa;
}

double MYON_Curve::SignedCurvatureAt(double t, const MYON_3dVector* plane_normal) const
{
  MYON_3dPoint point;
  MYON_3dVector tangent;
  double kappa;
  EvSignedCurvature(t, point, tangent, kappa, plane_normal);
  return kappa;
}

bool MYON_Curve::EvTangent(
       double t,
       MYON_3dPoint& point,
       MYON_3dVector& tangent,
       int side,
       int* hint
       ) const
{
  MYON_3dVector D1, D2;//, K;
  tangent = MYON_3dVector::ZeroVector;
  int rc = Ev1Der( t, point, tangent, side, hint );
  if ( rc && !tangent.Unitize() ) 
  {
    if ( Ev2Der( t, point, D1, D2, side, hint ) )
    {
      // Use l'Hopital's rule to show that if the unit tanget
      // exists, the 1rst derivative is zero, and the 2nd
      // derivative is nonzero, then the unit tangent is equal
      // to +/-the unitized 2nd derivative.  The sign is equal
      // to the sign of D1(s) o D2(s) as s approaches the 
      // evaluation parameter.
      tangent = D2;
      rc = tangent.Unitize();
      if ( rc )
      {
        MYON_Interval domain = Domain();
        double tminus = 0.0;
        double tplus = 0.0;
        if ( domain.IsIncreasing() && GetParameterTolerance( t, &tminus, &tplus ) )
        {
          MYON_3dPoint p;
          MYON_3dVector d1, d2;
          double eps = 0.0;
          double d1od2tol = 0.0; //1.0e-10; // 1e-5 is too big
          double d1od2;
          double tt = t;
          //double dt = 0.0;

          if ( (t < domain[1] && side >= 0) || (t == domain[0]) )
          {
            eps = tplus-t;
            if ( eps <= 0.0 || t+eps > domain.ParameterAt(0.1) )
              return rc;
          }
          else if ( (t > domain[0] && side < 0) || (t == domain[1]) )
          {
            eps = tminus - t;
            if ( eps >= 0.0 || t+eps < domain.ParameterAt(0.9) )
              return rc;
          }

          int i, negative_count=0, zero_count=0;
          int test_count = 3;
          for ( i = 0; i < test_count; i++, eps *= 0.5 )
          {
            tt = t + eps;
            if ( tt == t )
              break;
            if (!Ev2Der( tt, p, d1, d2, side, 0 ))
              break;
            d1od2 = d1*d2;
            if ( d1od2 > d1od2tol )
              break;
            if ( d1od2 < d1od2tol )
              negative_count++;
            else
              zero_count++;
          }
          if ( negative_count > 0 && test_count == negative_count+zero_count )
          {
            // all sampled d1od2 values were <= 0 
            // and at least one was strictly < 0.
            tangent = -tangent;
          }
        }
      }
    }
  }
  return rc;
}

bool MYON_Curve::EvCurvature(
       double t,
       MYON_3dPoint& point,
       MYON_3dVector& tangent,
       MYON_3dVector& kappa,
       int side,
       int* hint
       ) const
{
  MYON_3dVector d1, d2;
  int rc = Ev2Der( t, point, d1, d2, side, hint );
  if ( rc )
    rc = MYON_EvCurvature( d1, d2, tangent, kappa );
  return rc;
}

bool MYON_Curve::EvSignedCurvature(
  double t,
  MYON_3dPoint& point,
  MYON_3dVector& tangent,
  double& kappa,
  const MYON_3dVector* plane_normal,
  int side,
  int* hint
) const
{
  MYON_3dVector K;
  MYON_3dVector zdir(0, 0, 1);
  const MYON_3dVector* N = plane_normal;
  if (!N)
    N = &zdir;
  bool rc = EvCurvature(t, point, tangent, K, side, hint);
  if (rc)
  {
    kappa = MYON_TripleProduct(tangent, K, *N);
  }
  return rc;
}

bool MYON_Curve::FrameAt( double t, MYON_Plane& plane) const
{
  bool rc = false;
  MYON_Interval domain = Domain();
  if( t < domain[0] - MYON_EPSILON || t > domain[1] + MYON_EPSILON)
    return false;

  MYON_3dPoint pt;
  MYON_3dVector d1, d2;
  rc = Ev2Der( t, pt, d1, d2 );
  if (rc)
  {
    MYON_3dVector T, K;
    rc = MYON_EvCurvature(d1, d2, T, K);
    if (rc)
    {
      if (!K.Unitize())
      {
        K.PerpendicularTo(T);
        K.Unitize();
      }
      plane.origin = pt;
      plane.xaxis = T;
      plane.yaxis = K;
      plane.zaxis = MYON_CrossProduct(plane.xaxis, plane.yaxis);
      if (!plane.zaxis.Unitize())
        return false;
      plane.UpdateEquation();
      rc = plane.IsValid();
      if (!rc)
      {
        //26 Aug 2015 - Chuck - If K is extremely short, but can still be unitized, the result may not be perpendicular to T.
        plane.yaxis = MYON_CrossProduct(plane.zaxis, plane.xaxis);
        plane.yaxis.Unitize();
        rc = plane.UpdateEquation();
      }
    }
  }
  return rc;
}

bool MYON_Curve::EvPoint( // returns false if unable to evaluate
       double t,         // evaluation parameter
       MYON_3dPoint& point,   // returns value of curve
       int side,        // optional - determines which side to evaluate from
                       //         0 = default
                       //      <  0 to evaluate from below, 
                       //      >  0 to evaluate from above
       int* hint       // optional - evaluation hint used to speed repeated
                       //            evaluations
       ) const
{
  bool rc = false;
  double ws[128];
  double* v;
  if ( Dimension() <= 3 ) {
    v = &point.x;
    point.x = 0.0;
    point.y = 0.0;
    point.z = 0.0;
  }
  else if ( Dimension() <= 128 ) {
    v = ws;
  }
  else {
    v = (double*)onmalloc(Dimension()*sizeof(*v));
  }
  rc = Evaluate( t, 0, Dimension(), v, side, hint );
  if ( Dimension() > 3 ) {
    point.x = v[0];
    point.y = v[1];
    point.z = v[2];
    if ( Dimension() > 128 )
      onfree(v);
  }
  return rc;
}

bool MYON_Brep::EvaluatePoint( const class MYON_ObjRef& objref, MYON_3dPoint& P ) const
{
  // TODO
  return false;
}

bool MYON_Surface::EvaluatePoint( const class MYON_ObjRef& objref, MYON_3dPoint& P ) const
{
  // TODO
  return false;
}

bool MYON_PolyCurve::EvaluatePoint( const class MYON_ObjRef& objref, MYON_3dPoint& P ) const
{
  // TODO
  return false;
}

bool MYON_Curve::EvaluatePoint( const class MYON_ObjRef& objref, MYON_3dPoint& P ) const
{
  // virtual function default
  bool rc = false;

  MYON_3dPoint Q = MYON_3dPoint::UnsetPoint;
  if ( 1 == objref.m_evp.m_t_type )
  {
    if ( !EvPoint(objref.m_evp.m_t[0],Q) )
      Q = MYON_3dPoint::UnsetPoint;
  }

  switch( objref.m_osnap_mode )
  {
  case MYON::os_center:
    {
      MYON_Ellipse ellipse;
      if ( IsEllipse(0,&ellipse) )
      {
        P = ellipse.plane.origin;
        rc = true;
      }
      else
      {
        MYON_SimpleArray<MYON_3dPoint> pline;
        if ( IsClosed() && IsPolyline(&pline) && pline.Count() >= 4 )
        {
          P = pline[0];
          int i;
          for ( i = pline.Count()-2; i > 0; i-- )
          {
            Q = pline[i];
            P.x += Q.x; P.y += Q.y; P.z += Q.z;
          }
          double s = 1.0/(pline.Count()-1.0);
          P.x *= s; P.y *= s; P.z *= s;
          rc = true;
        }
        else if ( Q.IsValid() )
        {
          MYON_3dVector T, K;
          if ( EvCurvature(objref.m_evp.m_t[0],Q,T,K) )
          {
            double k = K.Length();
            if ( k > 0.0 )
            {
              P = Q + (1.0/(k*k))*K;
              rc = true;
            }
          }
        }
      }
    }
    break;

  case MYON::os_focus:
    {
      MYON_Ellipse ellipse;
      if ( IsEllipse(0,&ellipse) )
      {
        MYON_3dPoint F1, F2;
        if ( ellipse.GetFoci(F1,F2) )
        {
          P = ( F1.DistanceTo(Q) <= F2.DistanceTo(Q)) ? F1 : F2;
          rc = true;
        }
      }
    }
    break;

  case MYON::os_midpoint:
    {
    }
    break;

  case MYON::os_end:
    {
      MYON_SimpleArray<MYON_3dPoint> pline;
      if ( IsPolyline(&pline)  )
      {
        P = pline[0];
        double d = P.DistanceTo(Q);
        int i;
        for ( i = 1; i < pline.Count(); i++ )
        {
          double d1 = pline[i].DistanceTo(Q);
          if ( d1 < d )
          {
            d = d1;
            P = pline[i];
            rc = true;
          }
        }
      }
      else
      {
        P = PointAtStart();
        rc = true;
        if ( !IsClosed() )
        {
          MYON_3dPoint P1 = PointAtEnd();
          if ( P.DistanceTo(Q) > P1.DistanceTo(Q) )
          {
            P = P1;
          }
        }
      }      
    }
    break;

  default:
    if ( Q.IsValid() )
    {
      P = Q;
      rc = true;
    }
    break;
  }

  return rc;
}

bool MYON_Curve::Ev1Der( // returns false if unable to evaluate
       double t,         // evaluation parameter
       MYON_3dPoint& point,
       MYON_3dVector& derivative,
       int side,        // optional - determines which side to evaluate from
                       //      <= 0 to evaluate from below, 
                       //      >  0 to evaluate from above
       int* hint       // optional - evaluation hint used to speed repeated
                       //            evaluations
       ) const
{
  bool rc = false;
  const int dim = Dimension();
  double ws[2*64];
  double* v;
  point.x = 0.0;
  point.y = 0.0;
  point.z = 0.0;
  derivative.x = 0.0;
  derivative.y = 0.0;
  derivative.z = 0.0;
  if ( dim <= 64 ) {
    v = ws;
  }
  else {
    v = (double*)onmalloc(2*dim*sizeof(*v));
  }
  rc = Evaluate( t, 1, dim, v, side, hint );
  point.x = v[0];
  derivative.x = v[dim];
  if ( dim > 1 ) {
    point.y = v[1];
    derivative.y = v[dim+1];
    if ( dim > 2 ) {
      point.z = v[2];
      derivative.z = v[dim+2];
      if ( dim > 64 )
        onfree(v);
    }
  }

  return rc;
}

bool MYON_Curve::Ev2Der( // returns false if unable to evaluate
       double t,         // evaluation parameter
       MYON_3dPoint& point,
       MYON_3dVector& firstDervative,
       MYON_3dVector& secondDervative,
       int side,        // optional - determines which side to evaluate from
                       //      <= 0 to evaluate from below, 
                       //      >  0 to evaluate from above
       int* hint       // optional - evaluation hint used to speed repeated
                       //            evaluations
       ) const
{
  bool rc = false;
  const int dim = Dimension();
  double ws[3*64];
  double* v;
  point.x = 0.0;
  point.y = 0.0;
  point.z = 0.0;
  firstDervative.x = 0.0;
  firstDervative.y = 0.0;
  firstDervative.z = 0.0;
  secondDervative.x = 0.0;
  secondDervative.y = 0.0;
  secondDervative.z = 0.0;
  if ( dim <= 64 ) {
    v = ws;
  }
  else {
    v = (double*)onmalloc(3*dim*sizeof(*v));
  }
  rc = Evaluate( t, 2, dim, v, side, hint );
  point.x = v[0];
  firstDervative.x = v[dim];
  secondDervative.x = v[2*dim];
  if ( dim > 1 ) {
    point.y = v[1];
    firstDervative.y = v[dim+1];
    secondDervative.y = v[2*dim+1];
    if ( dim > 2 ) {
      point.z = v[2];
      firstDervative.z = v[dim+2];
      secondDervative.z = v[2*dim+2];
      if ( dim > 64 )
        onfree(v);
    }
  }

  return rc;
}


////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_Curve::IsShort() 
//


static
MYON::eCurveType MYON_CurveType( const MYON_Curve* curve )
{
  const MYON_ClassId* curve_id = &MYON_CLASS_RTTI(MYON_Curve);
  const MYON_ClassId* id = curve->ClassId();

  // "fake virtual" handling of fast/easy special cases
  while (0 != id && curve_id != id )
  {
    if ( &MYON_CLASS_RTTI(MYON_ArcCurve) == id )
      return MYON::ctArc;
    if ( &MYON_CLASS_RTTI(MYON_LineCurve) == id )
      return MYON::ctLine;
    if ( &MYON_CLASS_RTTI(MYON_PolylineCurve) == id )
      return MYON::ctPolyline;
    if ( &MYON_CLASS_RTTI(MYON_CurveProxy) == id )
      return MYON::ctProxy;
    if ( &MYON_CLASS_RTTI(MYON_PolyCurve) == id )
      return MYON::ctPolycurve;
    if ( &MYON_CLASS_RTTI(MYON_NurbsCurve) == id )
      return MYON::ctNurbs;
    if ( &MYON_CLASS_RTTI(MYON_CurveOnSurface) == id )
      return MYON::ctOnsurface;
    id = id->BaseClass();
  }
  
  return MYON::ctCurve;
}

/*
Description:
  Carefully match curve ends.
Parameters:
  curve0 - [in]
  end0 - [in] 0=match start of curve0, 1 = match end of curve0
  curve1 - [in]
  end1 - [in] 0=match start of curve1, 1 = match end of curve1
  gap_tolerance - [in]
    The match is not performed if the initial gap is <= gap_tolerance.
    If gap_tolerance < 0, then MYON_ComparePoint() is used to
    compare the points.
Returns:
  True if ends of curves are matched to requested gap_tolerance.
*/
bool MYON_MatchCurveEnds( MYON_Curve* curve0,
                        int end0,
                        MYON_Curve* curve1,
                        int end1,
                        double gap_tolerance = 0.0
                        );

bool MYON_MatchCurveEnds( MYON_Curve* curve0,
                        int end0,
                        MYON_Curve* curve1,
                        int end1,
                        double gap_tolerance )
{
  bool rc = false;
  if ( 0 != curve0 && 0 != curve1 
       && end0 >= 0 && end0 <= 1 
       && end1 >= 0 && end1 <= 1 )
  {
    MYON_3dPoint P0 = end0 ? curve0->PointAtEnd() : curve0->PointAtStart();
    MYON_3dPoint P1 = end1 ? curve1->PointAtEnd() : curve1->PointAtStart();
    rc = ( gap_tolerance < 0.0 )
       ? (0==MYON_ComparePoint( 3, false, &P0.x, &P1.x ) )
       : (P0.DistanceTo(P1) <= gap_tolerance);
    if ( !rc )
    {
      // try to close the gap
      MYON_Curve* seg[2] = {0,0};
      int fix[2] = {0,0};
      MYON_3dPoint fixPoint[2];
      fixPoint[0] = MYON_3dPoint::UnsetPoint;
      fixPoint[1] = MYON_3dPoint::UnsetPoint;

      // hurestic for deciding which point gets moved
      int i;
      for ( i = 0; i <= 1; i++ )
      {
        MYON_3dPoint Q0 = MYON_3dPoint::UnsetPoint;
        MYON_3dPoint Q1 = MYON_3dPoint::UnsetPoint;
        MYON_Curve* c = i ? curve1 : curve0;
        MYON::eCurveType ct = MYON_CurveType(c);
        int e = i ? end1 : end0;
        while ( MYON::ctPolycurve == ct )
        {
          c->DestroyRuntimeCache();
          MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(c);
          if ( 0 == polycurve )
            break;
          c = polycurve->SegmentCurve(e?(polycurve->Count()-1):0);
          if( 0 == c )
            return false;
          ct = MYON_CurveType(c);
        }
        seg[i] = c;
        switch(ct)
        {
        case MYON::ctArc: // arc
          if ( c->IsClosed() )
            fix[i] = 200;
          else
          {
            fix[i] = 100;
          }
          Q0 = c->PointAtStart();
          Q1 = c->PointAtEnd();
          break;
        case MYON::ctLine: // line
          fix[i] = 20;
          Q0 = c->PointAtStart();
          Q1 = c->PointAtEnd();
          break;
        case MYON::ctPolyline: // polyline
          fix[i] = 10;
          if ( c->SpanCount() == 1 )
          {
            // same as a line
            fix[i] = 20;
            Q0 = c->PointAtStart();
            Q1 = c->PointAtEnd();
          }
          else
            fix[i] = 10;
          break;
        case MYON::ctProxy: // proxy
          fix[i] = 1000; // cannot change
          break;
        case MYON::ctPolycurve: // polycurve
          fix[i] = 1000; // if this happens, something is bad
          break;
        case MYON::ctNurbs: // nurbs
          {
            if ( c->Degree() == 1 )
            {
              if ( c->SpanCount() == 1 )
              {
                // same as a line
                fix[i] = 20; 
                Q0 = c->PointAtStart();
                Q1 = c->PointAtEnd();
              }
              else
              {
                // same as a polyline
                fix[i] = 10;
              }
            }
            else
              fix[i] = 0;
          }
          break;
        case MYON::ctOnsurface: // curve on surface
          fix[i] = 1000; // cannot change
          break;
        default: // ???
          fix[i] = 50;
          break;
        }

        int j = 0;
        if ( MYON_UNSET_VALUE != Q0.x && Q0.x == Q1.x )
        {
          fixPoint[i].x = Q0.x;
          j++;
        }
        if ( MYON_UNSET_VALUE != Q0.y && Q0.y == Q1.y )
        {
          fixPoint[i].y = Q0.y;
          j++;
        }
        if ( MYON_UNSET_VALUE != Q0.z && Q0.z == Q1.z )
        {
          fixPoint[i].z = Q0.z;
          j++;
        }
        if ( 2 == j )
          fix[i] += 9;
        else if ( 1 == j )
          fix[i] += 1;
      }

      if ( fix[0] >= 1000 || fix[1] < fix[0] )
      {
        if ( fix[1] >= 1000 )
          return false;
        rc =  end1
           ? curve1->SetEndPoint(P0)
           : curve1->SetStartPoint(P0);
      }
      else if ( fix[1] >= 1000 || fix[0] < fix[1] )
      {
        rc =  end0
           ? curve0->SetEndPoint(P1)
           : curve0->SetStartPoint(P1);
      }
      else 
      {
        MYON_3dPoint P = 0.5*(P0+P1);
        if ( P0.x == P1.x )
          P.x = P0.x;
        else if ( fixPoint[0].x != MYON_UNSET_VALUE && fixPoint[1].x == MYON_UNSET_VALUE )
          P.x = fixPoint[0].x;
        else if ( fixPoint[0].x == MYON_UNSET_VALUE && fixPoint[1].x != MYON_UNSET_VALUE )
          P.x = fixPoint[1].x;
        if ( P0.y == P1.y )
          P.y = P0.y;
        else if ( fixPoint[0].y != MYON_UNSET_VALUE && fixPoint[1].y == MYON_UNSET_VALUE )
          P.y = fixPoint[0].y;
        else if ( fixPoint[0].y == MYON_UNSET_VALUE && fixPoint[1].y != MYON_UNSET_VALUE )
          P.y = fixPoint[1].y;
        if ( P0.z == P1.z )
          P.z = P0.z;
        else if ( fixPoint[0].z != MYON_UNSET_VALUE && fixPoint[1].z == MYON_UNSET_VALUE )
          P.z = fixPoint[0].z;
        else if ( fixPoint[0].z == MYON_UNSET_VALUE && fixPoint[1].z != MYON_UNSET_VALUE )
          P.z = fixPoint[1].z;   
        bool rc0 =  end0
           ? curve0->SetEndPoint(P)
           : curve0->SetStartPoint(P);
        bool rc1 =  end1
           ? curve1->SetEndPoint(P)
           : curve1->SetStartPoint(P);
        rc = (rc0 && rc1);
      }
      if ( rc )
      {
        P0 = end0 ? curve0->PointAtEnd() : curve0->PointAtStart();
        P1 = end1 ? curve1->PointAtEnd() : curve1->PointAtStart();
        double d = P0.DistanceTo(P1);
        rc = ( gap_tolerance <= 0.0 ) // <= is correct here
           ? (0==MYON_ComparePoint( 3, false, &P0.x, &P1.x ) )
           : (d <= gap_tolerance);
      }
    }
  }
  return rc?true:false;
}

static bool ForceMatchArcs(MYON_ArcCurve& Arc0, int end0, MYON_ArcCurve& Arc1, int end1)

{
  MYON_3dPoint P0 = (end0) ? Arc0.PointAtEnd() : Arc0.PointAtStart();
  MYON_3dPoint P1 = (end1) ? Arc1.PointAtEnd() : Arc1.PointAtStart();
  MYON_3dPoint P = 0.5*(P0+P1);

  bool bMove[2] = {true,true};
  bool rc = true;

  if (bMove[0]){
    bool brc = (end0) ? Arc0.SetEndPoint(P) : Arc0.SetStartPoint(P);
    if (!brc)
      rc = false;
  }
  if (bMove[1]){
    bool brc = (end1) ? Arc1.SetEndPoint(P) : Arc1.SetStartPoint(P);
    if (!brc)
      rc = false;
  }

  return rc;


}

static bool FastIsShort(const MYON_Curve& crv, double tol)

{
  MYON_3dPoint P[5];
  P[0] = crv.PointAtStart();
  P[4] = crv.PointAtEnd();
  if (P[0].DistanceTo(P[4]) >= tol)
    return false;
  double d = 0.0;
  for (int i=1; i<4; i++){
    P[i] = crv.PointAt(crv.Domain().ParameterAt(0.25*(double)i));
    d += P[i].DistanceTo(P[i-1]);
    if (d >= tol)
      return false;
  }
  d += P[4].DistanceTo(P[3]);
  return (d < tol) ? true : false;
}

bool MYON_ForceMatchCurveEnds(MYON_Curve& Crv0, int end0, MYON_Curve& Crv1, int end1)

{

  if (end0)
    end0 = 1;
  if (end1)
    end1 = 1;

  int i;
  MYON::eCurveType ct[2];
  MYON_Curve* seg[2];
  bool bIsArc[2];
  for ( i = 0; i<2; i++ )
  {
    MYON_Curve* c = i ? &Crv1 : &Crv0;
    ct[i] = MYON_CurveType(c);
    int e = i ? end1 : end0;
    while ( MYON::ctPolycurve == ct[i] )
    {
      c->DestroyRuntimeCache();
      MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(c);
      if ( 0 == polycurve )
        break;
      c = polycurve->SegmentCurve(e?(polycurve->Count()-1):0);
      if( 0 == c )
        return false;
      ct[i] = MYON_CurveType(c);
    }
    if (c->IsClosed())
      return false;
    if (ct[i] == MYON::ctArc)
      bIsArc[i] = true;
    else {
      if (ct[i] == MYON::ctLine || ct[i] == MYON::ctNurbs || ct[i] == MYON::ctPolyline)
        bIsArc[i] = false;
      else
        return false;
    }
    seg[i] = c;
  }

  MYON_3dPoint P;
  bool bMove[2] = {true,true};
  if (bIsArc[0]){
    if (!bIsArc[1]){
      P = (end0) ? seg[0]->PointAtEnd() : seg[0]->PointAtStart();
      bMove[0] = false;
    }
    else {
      MYON_ArcCurve* pArc0 = MYON_ArcCurve::Cast(seg[0]);
      if (!pArc0)
        return false;
      MYON_ArcCurve* pArc1 = MYON_ArcCurve::Cast(seg[1]);
      if (!pArc1)
        return false;
      return ForceMatchArcs(*pArc0, end0, *pArc1, end1);
    }
  }
  else {
    if (bIsArc[1]){
      P = (end1) ? seg[1]->PointAtEnd() : seg[1]->PointAtStart();
      bMove[1] = false;
    }
    else {
      MYON_3dPoint P0 = (end0) ? seg[0]->PointAtEnd() : seg[0]->PointAtStart();
      MYON_3dPoint P1 = (end1) ? seg[1]->PointAtEnd() : seg[1]->PointAtStart();
      P = 0.5*(P0+P1);
    }
  }

  bool rc = true;
  bool bTryAgain = false;
  if (bMove[0]){
    bool brc = (end0) ? seg[0]->SetEndPoint(P) : seg[0]->SetStartPoint(P);
    if (!brc)
      rc = false;
    else {
      //23 Jan 2018 - Chuck - If yanking a polycurve segment at the join causes
      //that seg to be tiny, remove it and try again. See RH-43661
      if (MYON_CurveType(&Crv0) == MYON::ctPolycurve){
        MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(&Crv0);
        if (polycurve->Count() > 1 && FastIsShort(*seg[0], 10.0*MYON_ZERO_TOLERANCE))
          bTryAgain = (end0) ? polycurve->Remove() : polycurve->Remove(0);
      }
    }
  }
  if (bMove[1]){
    bool brc = (end1) ? seg[1]->SetEndPoint(P) : seg[1]->SetStartPoint(P);
    if (!brc)
      rc = false;
    else {
      //23 Jan 2018 - Chuck - If yanking a polycurve segment at the join causes
      //that seg to be tiny, remove it and try again. See RH-43661
      if (MYON_CurveType(&Crv1) == MYON::ctPolycurve){
        MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(&Crv1);
        if (polycurve->Count() > 1 && FastIsShort(*seg[1], 10.0*MYON_ZERO_TOLERANCE)){
          bool bRem = (end1) ? polycurve->Remove() : polycurve->Remove(0);
          if (bRem)
            bTryAgain = true;
        }
      }
    }
  }

  //23 Jan 2018 - Chuck - If yanking a polycurve segment at the join causes
  //that seg to be tiny, remove it and try again. See RH-43661
  if (bTryAgain)//One of these is a polycurve with one less segment thane before, so no infinite recursion.
    return MYON_ForceMatchCurveEnds(Crv0, end0, Crv1, end1);

  return rc;
}



static bool Internal_IsUniformCubic(const MYON_NurbsCurve& curve)
{
  if (4 != curve.m_order)
    return false;
  if (curve.m_cv_count < curve.m_order)
    return false;
  if (0 != curve.m_is_rat)
    return false;
  if (nullptr == curve.m_knot)
    return false;
  const int knot_count = curve.KnotCount();
  for (int i = 0; i < knot_count; i++)
  {
    if (curve.m_knot[i] != (double)(i - 2))
      return false;
  }
  return true;
}

bool MYON_NurbsCurve::RepairBadKnots( double knot_tolerance, bool bRepair )
{
  bool rc = false;
  if ( m_order >= 2 && m_cv_count > m_order
       && 0 != m_cv && 0 != m_knot 
       && m_dim > 0
       && (m_cv_stride >= m_is_rat ? m_dim + 1 : m_dim)
       && m_knot[m_cv_count-1] - m_knot[m_order-2] > knot_tolerance
       )
  {
    //const int cv_count0 = m_cv_count;

    const int sizeof_cv = CVSize()*sizeof(*m_cv);
    //int knot_count = KnotCount();
    int i, j0, j1;

    bool bIsPeriodic = IsPeriodic();

    if ( !bIsPeriodic )
    {
      if ( m_knot[0] != m_knot[m_order-2] || m_knot[m_cv_count-1] != m_knot[m_cv_count+m_order-3] )
      {
        if (false == Internal_IsUniformCubic(*this))
        {
          rc = true;
          if (bRepair)
            ClampEnd(2);
          else
            return rc;
        }
      }
    }

    // make sure last span has m_knot[m_cv_count-1] - m_knot[m_cv_count-2] > knot_tolerance
    for ( i = m_cv_count-2; i > m_order-2; i-- )
    {
      if ( m_knot[m_cv_count-1] - m_knot[i] > knot_tolerance )
      {
        if ( i < m_cv_count-2 )
        {
          // the last span is invalid
          rc = true;
          if ( bRepair )
          {
            // 15-June-2020 
            // Remove degenerate and small spans at the end, with out changing the domain 
            // and trying to maintain parametric curve ( i.e, C: Domain->R^d is invarient).
            DestroyRuntimeCache();
  
            if (knot_tolerance > 0)
            {
              // Tune up knots at  end with 0< spacing <knot_tolerance, by setting to domain end.
              for (j0 = i + 1; j0 < m_cv_count - 1; j0++)  
                m_knot[j0] = m_knot[m_cv_count - 1];
            }
            m_cv_count = i + 2;   // Eliminate knots and CV's from the end.

            // Now that final span is non-degenerate we can ClampEnd
            ClampEnd(1);
 
          }
          else
            return rc;
        }
        break; // the last span is non degenerate
      }
    }

    // make sure first span has m_knot[m_order-1] - m_knot[m_order-2] > knot_tolerance
    for ( i = m_order-1; i < m_cv_count-1; i++ )
    {
      if ( m_knot[i] - m_knot[m_order-2] > knot_tolerance )
      {
        if ( i > m_order-1 )
        {
          // the first span is invalid
          rc = true;
          if ( bRepair )
          {
            // 15-June-2020 
             // Remove degenerate and small spans at the end, with out changing the domain 
             // and trying to maintain parametric curve ( i.e, C: Domain->R^d is invarient). 
            DestroyRuntimeCache();
            if (knot_tolerance > 0)
            {
              // Tune up knots at start with 0< spacing <knot_tolerance, by setting to domain start.
              for (j0 = i - 1; j0 > m_order - 2; j0--)
                m_knot[j0] = m_knot[m_order - 2];
            }

            // This is a hack way of eliminating knots and CV's from the start.
            // Warning!!   m_cv and m_knot need to be restored before we exit this function

            i = i - (m_order - 1);
            m_cv += m_cv_stride * i ;  
            m_knot += i;
            m_cv_count -= i;
            ClampEnd(0);    // ...This hack works because ClampEnd does not reallocate cv or knot arrays
            m_cv -= m_cv_stride * i ;
            m_knot -= i ;

 
            for (j0 = 0, j1 = i; j0 < m_cv_count; j0++, j1++)
              memcpy(CV(j0), CV(j1), sizeof_cv);
          
            for (j0 = 0, j1 = i; j0 < m_cv_count+m_order-2; j0++, j1++)
              m_knot[j0] = m_knot[j1];
            
          }
          else
            return rc;
        }
        break; // there at least one valid span
      }
    }


    if (    m_knot[m_order-1]-m_knot[m_order-2] > knot_tolerance 
         && m_knot[m_cv_count-1]-m_knot[m_cv_count-2] > knot_tolerance )
    {
      // Remove interior knots with mulitiplicity >= m_order
      for ( i = m_cv_count-m_order-1; i >= m_order; i-- )
      {
        if ( m_knot[i+m_order-1] - m_knot[i] <= knot_tolerance )
        {
          rc = true;
          if ( bRepair )
          {
            // empty evaluation span - remove CV and knot
            DestroyRuntimeCache();
            for ( j0 = i, j1 = i+1; j1 < m_cv_count; j0++, j1++ )
              memcpy( CV(j0), CV(j1), sizeof_cv );
            for ( j0 = i, j1 = i+1; j1 < m_cv_count+m_order-2; j0++, j1++ )
              m_knot[j0] = m_knot[j1];
            m_cv_count--;
          }
          else
          {
            // query mode
            return rc;
          }
        }
      }
    }

    if ( bRepair && bIsPeriodic && rc )
    {
      if ( !IsPeriodic() )
        ClampEnd(2);
    }
  }
  return rc;
}



bool MYON_Curve::FirstSpanIsLinear( 
  double min_length,
  double tolerance
  ) const
{
  return FirstSpanIsLinear(min_length,tolerance,0);
}

bool MYON_Curve::FirstSpanIsLinear( 
  double min_length,
  double tolerance,
  MYON_Line* span_line
  ) const
{
  const MYON_NurbsCurve* nurbs_curve = MYON_NurbsCurve::Cast(this);
  if ( 0 != nurbs_curve )
  {
    return nurbs_curve->SpanIsLinear(0,min_length,tolerance,span_line);
  }

  const MYON_PolylineCurve* polyline_curve = MYON_PolylineCurve::Cast(this);
  if ( 0 != polyline_curve )
  {
    bool rc = (polyline_curve->PointCount() >= 2);
    if (rc && 0 != span_line )
    {
      span_line->from = polyline_curve->m_pline[0];
      span_line->to = polyline_curve->m_pline[1];
    }
    return rc;
  }

  const MYON_LineCurve* line_curve = MYON_LineCurve::Cast(this);
  if ( 0 != line_curve )
  {
    if ( span_line )
      *span_line = line_curve->m_line;
    return true;
  }

  const MYON_PolyCurve* poly_curve = MYON_PolyCurve::Cast(this);
  if ( 0 != poly_curve )
  {
    const MYON_Curve* segment = poly_curve->SegmentCurve(0);
    return (0 != segment) ? segment->FirstSpanIsLinear(min_length,tolerance,span_line) : false;
  }

  const MYON_CurveProxy* proxy_curve = MYON_CurveProxy::Cast(this);
  if ( 0 != proxy_curve )
  {
    const MYON_Curve* curve = proxy_curve->ProxyCurve();
    if ( 0 == curve )
      return false;
    bool bProxyCurveIsReversed = proxy_curve->ProxyCurveIsReversed();
    bool rc = bProxyCurveIsReversed
            ? curve->FirstSpanIsLinear(min_length,tolerance,span_line) 
            : curve->LastSpanIsLinear(min_length,tolerance,span_line);
    if ( rc && bProxyCurveIsReversed && 0 != span_line )
      span_line->Reverse();
    return rc;
  }

  return false;
}



bool MYON_Curve::LastSpanIsLinear( 
  double min_length,
  double tolerance
  ) const
{
  return LastSpanIsLinear(min_length,tolerance,0);
}

bool MYON_Curve::LastSpanIsLinear( 
  double min_length,
  double tolerance,
  MYON_Line* span_line
  ) const
{
  const MYON_NurbsCurve* nurbs_curve = MYON_NurbsCurve::Cast(this);
  if ( 0 != nurbs_curve )
  {
    return nurbs_curve->SpanIsLinear(nurbs_curve->m_cv_count-nurbs_curve->m_order,min_length,tolerance,span_line);
  }

  const MYON_PolylineCurve* polyline_curve = MYON_PolylineCurve::Cast(this);
  if ( 0 != polyline_curve )
  {
    int count = polyline_curve->PointCount();
    if ( count >= 2 && 0 != span_line )
    {
      span_line->from = polyline_curve->m_pline[count-2];
      span_line->to = polyline_curve->m_pline[count-1];
    }
    return ( count >= 2);
  }

  const MYON_LineCurve* line_curve = MYON_LineCurve::Cast(this);
  if ( 0 != line_curve )
  {
    if ( span_line )
      *span_line = line_curve->m_line;
    return true;
  }

  const MYON_PolyCurve* poly_curve = MYON_PolyCurve::Cast(this);
  if ( 0 != poly_curve )
  {
    const MYON_Curve* segment = poly_curve->SegmentCurve(poly_curve->Count()-1);
    return (0 != segment) ? segment->LastSpanIsLinear(min_length,tolerance,span_line) : false;
  }

  const MYON_CurveProxy* proxy_curve = MYON_CurveProxy::Cast(this);
  if ( 0 != proxy_curve )
  {
    const MYON_Curve* curve = proxy_curve->ProxyCurve();
    if ( 0 == curve )
      return false;
    bool bProxyCurveIsReversed = proxy_curve->ProxyCurveIsReversed();
    bool rc = bProxyCurveIsReversed
           ? curve->LastSpanIsLinear(min_length,tolerance,span_line) 
           : curve->FirstSpanIsLinear(min_length,tolerance,span_line);
    if ( rc && bProxyCurveIsReversed && 0 != span_line )
      span_line->Reverse();
    return rc;
  }

  return false;
}



bool MYON_NurbsCurve::SpanIsLinear(
    int span_index, 
    double min_length,
    double tolerance
    ) const
{
  return SpanIsLinear(span_index,min_length,tolerance,0);
}

bool MYON_NurbsCurve::SpanIsLinear(
    int span_index, 
    double min_length,
    double tolerance,
    MYON_Line* span_line
    ) const
{
  if ( m_dim < 2 || m_dim > 3 )
    return false;

  if ( -1 == span_index && m_cv_count-m_order+1+span_index >= 0 )
  {
    // negative span indices work from the back
    span_index += m_cv_count-m_order+1;
  }
  else if ( span_index < 0 || span_index > m_cv_count-m_order )
  {
    MYON_ERROR("span_index out of range.");
    return false;
  }

  if ( !(m_knot[span_index+m_order-2] < m_knot[span_index+m_order-1]) )
  {
    // empty span
    MYON_ERROR("empty span.");
    return false;
  }

  if (    m_knot[span_index] == m_knot[span_index+m_order-2]
       && m_knot[span_index+m_order-1] == m_knot[span_index+2*m_order-3]
     )
  {
    MYON_3dPoint P, Q;
    MYON_Line line;
    const int i1 = span_index+m_order-1;
    if ( !GetCV(span_index,line.from) )
      return false;
    if ( !GetCV(i1,line.to) )
      return false;
    if ( !(line.Length() >= min_length) ) 
      return false;
    double t0, t, d;
    t0 = t = 0.0;
    for ( int i = span_index+1; i < i1; i++ )
    {
      if ( !GetCV(i,P) )
        return false;
      if ( !line.ClosestPointTo( P, &t ) )
        return false;
      if ( !(t0 < t) )
        return false;
			if ( !(t <= 1.0 + MYON_SQRT_EPSILON) )
				return false;
      Q = line.PointAt(t);
      if ( false == MYON_PointsAreCoincident(3,0,&P.x,&Q.x) )
      {
        d = P.DistanceTo( line.PointAt(t) );
        if ( !(d <= tolerance) )
          return false;
      }
      t0 = t;
    }
    if ( span_line )
      *span_line = line;
    return true;
  }

  return false;
}


bool MYON_Curve::Trim( const MYON_Interval& in )
{
  // TODO - make this pure virtual
  return false;
}


bool MYON_Curve::Extend(
  const MYON_Interval& domain
  )

{
  return false;
}


MYON_Curve* MYON_TrimCurve( 
            const MYON_Curve& curve,
            MYON_Interval trim_parameters
            )
{
  MYON_Curve* trimmed_curve = 0;

  const MYON_Interval curve_domain = curve.Domain();
  bool bDecreasing = trim_parameters.IsDecreasing();
  trim_parameters.Intersection( curve_domain ); // trim_parameters will be increasing or empty
  if ( bDecreasing )
  {
    trim_parameters.Swap();
    if ( trim_parameters[0] == curve_domain[1] )
    {
      if ( trim_parameters[1] == curve_domain[0] )
        return 0;
      trim_parameters[0] = curve_domain[0];
    }
    else if ( trim_parameters[1] == curve_domain[0] )
      trim_parameters[1] = curve_domain[1];
    else if ( !trim_parameters.IsDecreasing() )
      return 0;
  }

  if ( trim_parameters.IsDecreasing() && curve.IsClosed() )
  {
    MYON_Curve* left_crv = curve.DuplicateCurve();
    if ( !left_crv->Trim(MYON_Interval(trim_parameters[0],curve_domain[1])) )
    {
      delete left_crv;
      return 0;
    }
    MYON_Curve* right_crv = curve.DuplicateCurve();
    if ( !right_crv->Trim(MYON_Interval(curve_domain[0],trim_parameters[1])) )
    {
      delete left_crv;
      delete right_crv;
      return 0;
    }
    MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(left_crv);
    if ( polycurve == nullptr )
    {
      polycurve = new MYON_PolyCurve();      
      polycurve->Append( left_crv );
    }

    MYON_PolyCurve* ptmp = MYON_PolyCurve::Cast(right_crv);
    if ( ptmp )
    {
      int i;
      for ( i = 0; i < ptmp->Count(); i++ )
      {
        MYON_Interval sdom = ptmp->SegmentDomain(i);
        MYON_Curve* segment = ptmp->HarvestSegment(i);
        segment->SetDomain(sdom[0],sdom[1]); // to keep relative parameterization unchanged
        polycurve->Append( segment );
      }
      delete right_crv;
      ptmp = 0;
      right_crv = 0;
    }
    else
    {
      polycurve->Append( right_crv );
    }

    polycurve->SetDomain( trim_parameters[0], trim_parameters[1] + curve_domain.Length() );

    trimmed_curve = polycurve;
  }
  else if ( trim_parameters.IsIncreasing() )
  {
    trimmed_curve = curve.DuplicateCurve();
    if(!trimmed_curve || !trimmed_curve->Trim(trim_parameters) )
    {
      delete trimmed_curve;
      trimmed_curve = 0;
    }
  }

  return trimmed_curve;
}

bool MYON_Curve::Split(
    double, // t = curve parameter to split curve at
    MYON_Curve*&, // left portion returned here (can pass "this" as the pointer)
    MYON_Curve*&  // right portion returned here (can pass "this" as the pointer)
  ) const
{
  // override if curve can split itself
  return false;
}

// virtual
int MYON_Curve::GetNurbForm(
      MYON_NurbsCurve& nurbs_curve,
      double tolerance,
      const MYON_Interval* subdomain
      ) const
{
  return 0;
}

// virtual
int MYON_Curve::HasNurbForm() const
{
  return 0;
}

MYON_NurbsCurve* MYON_Curve::NurbsCurve(
      MYON_NurbsCurve* pNurbsCurve,
      double tolerance,
      const MYON_Interval* subdomain
      ) const
{
  MYON_NurbsCurve* nurbs_curve = pNurbsCurve;
  if ( !nurbs_curve )
    nurbs_curve = new MYON_NurbsCurve();
  int rc = GetNurbForm( *nurbs_curve, tolerance, subdomain );
  if ( !rc )
  {
    if (!pNurbsCurve)
      delete nurbs_curve;
    nurbs_curve = nullptr;
  }
  return nurbs_curve;
}

bool MYON_Curve::GetCurveParameterFromNurbFormParameter(
      double nurbs_t,
      double* curve_t
      ) const
{
  *curve_t = nurbs_t;
  return false;
}

bool MYON_Curve::GetNurbFormParameterFromCurveParameter(
      double curve_t,
      double* nurbs_t
      ) const
{
  *nurbs_t = curve_t;
  return false;
}

MYON_CurveArray::MYON_CurveArray( int initial_capacity ) 
              : MYON_SimpleArray<MYON_Curve*>(initial_capacity)
{}

MYON_CurveArray::~MYON_CurveArray()
{
  Destroy();
}

void MYON_CurveArray::Destroy()
{
  int i = m_capacity;
  while ( i-- > 0 ) {
    if ( m_a[i] ) {
      delete m_a[i];
      m_a[i] = nullptr;
    }
  }
  Empty();
}

bool MYON_CurveArray::Duplicate( MYON_CurveArray& dst ) const
{
  dst.Destroy();
  dst.SetCapacity( Capacity() );

  const int count = Count();
  int i;
  MYON_Curve* curve;
  for ( i = 0; i < count; i++ ) 
  {
    curve = 0;
    if ( m_a[i] ) 
    {
      curve = m_a[i]->Duplicate();
    }
    dst.Append(curve);      
  }
  return true;
}

bool MYON_CurveArray::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK, 0 );
  if (rc) rc = file.Write3dmChunkVersion(1,0);
  if (rc ) {
    int i;
    rc = file.WriteInt( Count() );
    for ( i = 0; rc && i < Count(); i++ ) {
      if ( m_a[i] ) {
        rc = file.WriteInt(1);
        if ( rc ) 
          rc = file.WriteObject( *m_a[i] ); // polymorphic curves
      }
      else {
        // nullptr curve
        rc = file.WriteInt(0);
      }
    }
    if ( !file.EndWrite3dmChunk() )
      rc = false;
  }
  return rc;
}


bool MYON_CurveArray::Read( MYON_BinaryArchive& file )
{
  int major_version = 0;
  int minor_version = 0;
  MYON__UINT32 tcode = 0;
  MYON__INT64 big_value = 0;
  int flag;
  Destroy();
  bool rc = file.BeginRead3dmBigChunk( &tcode, &big_value );
  if (rc) 
  {
    rc = ( tcode == TCODE_ANONYMOUS_CHUNK );
    if (rc) rc = file.Read3dmChunkVersion(&major_version,&minor_version);
    if (rc && major_version == 1) 
    {
      MYON_Object* p;
      int count;
      rc = file.ReadInt( &count );
      if (rc) 
      {
        SetCapacity(count);
        SetCount(count);
        Zero();
        int i;
        for ( i = 0; rc && i < count && rc; i++ ) 
        {
          flag = 0;
          rc = file.ReadInt(&flag);
          if (rc && flag==1) 
          {
            p = 0;
            rc = file.ReadObject( &p ) ? true : false; // polymorphic curves
            m_a[i] = MYON_Curve::Cast(p);
            if ( !m_a[i] )
              delete p;
          }
        }
      }
    }
    else 
    {
      rc = false;
    }
    if ( !file.EndRead3dmChunk() )
    {
      rc = false;
    }
  }
  return rc;
}

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////// utilities for curve joining ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


struct CurveJoinSeg {
  int id;
  bool bRev;
};

static void ReverseSegs(MYON_SimpleArray<CurveJoinSeg>& SArray)

{
  int i;
  for (i=0; i<SArray.Count(); i++)
    SArray[i].bRev = !SArray[i].bRev;
  SArray.Reverse();
  return;
}

//distance from curve[id[0] end[end[0]] to curve[id[1]] end[end[1]] is dist.
struct CurveJoinEndData {
  int id[2];  //index into array of curves
  int end[2]; //0 for start, 1 for end
  double dist;
  double tan_dot;//If start-to-start or end-to-end, this will be -Tan0*Tan1.  Otherwise Tan0*Tan1.
};

struct JoinEndCompareContext

{
public:
  double dist_tol;
  double dot_tol;
  bool bUseTan;
};

static int CompareJoinEnds(void* ctext, const void* aA, const void* bB)

{
  JoinEndCompareContext* context = (JoinEndCompareContext*)ctext;
  const CurveJoinEndData* a = (CurveJoinEndData*)aA;
  const CurveJoinEndData* b = (CurveJoinEndData*)bB;
  if (context->bUseTan){
    if (a->dist < context->dist_tol && b->dist >= context->dist_tol) return -1;
    if (a->dist >= context->dist_tol && b->dist < context->dist_tol) return 1;
    if (a->tan_dot > context->dot_tol && b->tan_dot <= context->dot_tol) return -1;
    if (a->tan_dot <= context->dot_tol && b->tan_dot > context->dot_tol) return 1;
    if (a->dist < b->dist) return -1;
    if (a->dist > b->dist) return 1;
    if (a->id[0] < b->id[0]) return -1;
    if (a->id[0] > b->id[0]) return 1;
    if (a->id[1] < b->id[1]) return -1;
    if (a->id[1] > b->id[1]) return 1;
    return 0;
  }
  else {
    if (a->dist < b->dist) return -1;
    if (a->dist > b->dist) return 1;
    if (a->tan_dot > b->tan_dot) return -1;
    if (a->tan_dot < b->tan_dot) return 1;
    if (a->id[0] < b->id[0]) return -1;
    if (a->id[0] > b->id[0]) return 1;
    if (a->id[1] < b->id[1]) return -1;
    if (a->id[1] > b->id[1]) return 1;
    return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////// end of utilities for curve joining ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


bool MYON_Curve::IsClosable(double tolerance, 
                          double min_abs_size, //0.0
                          double min_rel_size  //10.0
                          ) const

{
  if (IsClosed()) return true;
  if (Degree() + SpanCount() < 4) return false;

  MYON_3dPoint P[6];

  P[0] = PointAtStart();
  P[5] = PointAtEnd();

  double gap = P[0].DistanceTo(P[5]);
  if (gap > tolerance) return false;

  bool abs_ok = (min_abs_size < 0.0) ? true : false;
  bool rel_ok = (min_rel_size <= 1.0) ? true : false;
  bool ok = abs_ok && rel_ok;

  if (!ok){

    //make sure curve is long enough to close off.
    int i;
    double len = 0.0;

    for (i=1; i<6; i++){
      if (i!=5)
        P[i] = PointAt(Domain().ParameterAt(0.2*i));
      if (!abs_ok && P[i].DistanceTo(P[0]) > min_abs_size)
        abs_ok = true;
      if (!rel_ok){
        len += P[i-1].DistanceTo(P[i]);
        if (len >= min_rel_size*gap)
          rel_ok = true;
      }
      ok = abs_ok && rel_ok;
      if (ok) break;
    }
  }

  return ok;
}

//Makes continuous MYON_PolyCurves and MYON_Polycurves. Appends results to OutCurves.
//returns number of curves appended.



int 
MYON_JoinCurves(const MYON_SimpleArray<const MYON_Curve*>& InCurves,
                      MYON_SimpleArray<MYON_Curve*>& OutCurves,
                      double join_tol,
                      bool bPreserveDirection, // = false
                      MYON_SimpleArray<int>* key //=0
                      )

{
  return MYON_JoinCurves(InCurves, OutCurves, join_tol, MYON_UNSET_VALUE, false, bPreserveDirection, key);
}

//////////////////////////////////////
class JoinCurveEnd

{
public:
  JoinCurveEnd();
  JoinCurveEnd(const JoinCurveEnd& src);
  void Create(int cid, const MYON_Curve& crv, int end, bool bDoTan);
  ~JoinCurveEnd();
  JoinCurveEnd& operator=(const JoinCurveEnd& src);
  int m_cid;//into curve array. -1 if not valid
  int m_end;//0, 1
  MYON_3dPoint m_P;
  MYON_3dVector m_T;
  bool m_bTanOK;
};

JoinCurveEnd::JoinCurveEnd()
  :m_cid(-1), m_end(0), m_P(MYON_3dPoint::Origin), m_T(MYON_3dVector::ZeroVector), m_bTanOK(false)
{}

JoinCurveEnd::JoinCurveEnd(const JoinCurveEnd& src)

{
  *this = src;
}

void JoinCurveEnd::Create(int cid, const MYON_Curve& crv, int end, bool bGetTan)

{
  m_cid = cid;
  m_end = (end) ? 1 : 0;
  m_bTanOK = false;
  if (bGetTan){
    if (!crv.EvTangent(crv.Domain()[m_end], m_P, m_T))
      m_P = crv.PointAt(crv.Domain()[m_end]);
    else
      m_bTanOK = true;
  }
  else
    m_P = crv.PointAt(crv.Domain()[m_end]);
}

JoinCurveEnd::~JoinCurveEnd()

{}

JoinCurveEnd& JoinCurveEnd::operator=(const JoinCurveEnd& src)
{
  if ( this != &src ) 
  {
    m_cid = src.m_cid;
    m_end = src.m_end;
    m_bTanOK = src.m_bTanOK;
    m_P = src.m_P;
    m_T = src.m_T;
  }
  return *this;
}

struct JoinEndPair

{
  JoinCurveEnd* a;
  JoinCurveEnd* b;
  double dist;
  double dot;
};


#if defined(MYON_DLL_TEMPLATE)
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4231 )
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<JoinEndPair>;
#pragma MYON_PRAGMA_WARNING_POP
#endif


struct JoinTreeContext

{
  MYON_SimpleArray<JoinEndPair>* Pairs;
  bool bPreserveDirection;
  bool bCheckDot;
  double dot_tol;//Only used if bCheckDot is true
};

void MYON_CALLBACK_CDECL JoinEndCallback(void* a_context, MYON__INT_PTR a_idA, MYON__INT_PTR a_idB)

{
  JoinTreeContext* JTC = (JoinTreeContext*)a_context;
  JoinEndPair JCP;
  JCP.a = (JoinCurveEnd*)a_idA;
  JCP.b = (JoinCurveEnd*)a_idB;
  if (JCP.a->m_cid < 0 || JCP.b->m_cid < 0 || JCP.a->m_cid == JCP.b->m_cid)
    return;
  if (JTC->bPreserveDirection && JCP.a->m_end == JCP.b->m_end)
    return;
  if (JTC->bCheckDot){
    if (!JCP.a->m_bTanOK || !JCP.b->m_bTanOK)
      return;
    double dot = JCP.a->m_T*JCP.b->m_T;
    if (JCP.a->m_end == JCP.b->m_end) dot *= -1.0;
    if (dot < JTC->dot_tol)
      return;
    JCP.dot = dot;
  }
  else
    JCP.dot = JCP.a->m_T*JCP.b->m_T;
  JCP.dist = JCP.a->m_P.DistanceTo(JCP.b->m_P);
  JTC->Pairs->Append(JCP);
}

class JoinCurveEndArray

{
public:
  JoinCurveEndArray();
  ~JoinCurveEndArray();
  bool Create(const MYON_SimpleArray<MYON_Curve*>& Curves, 
                               double dtol,
                               bool bPreserveDirection,
                               bool bGetTan, 
                               bool bCheckDot,
                               double dot_tol);//Only used if bCheckDot is true
  void Destroy();
  int m_CurveCount;
  JoinCurveEnd* m_Ends[2];//2*m_CurveCount
  MYON_SimpleArray<JoinEndPair> m_Pairs;
};

JoinCurveEndArray::JoinCurveEndArray()

{
  m_Ends[0] = m_Ends[1] = 0;
}


JoinCurveEndArray::~JoinCurveEndArray()

{
  Destroy();
}

void JoinCurveEndArray::Destroy()

{
  m_Pairs.Empty();
  for (int i=0; i<2; i++){
    delete [] m_Ends[i];
    m_Ends[i] = 0;
  }
}

bool JoinCurveEndArray::Create(const MYON_SimpleArray<MYON_Curve*>& Curves, 
                               double dtol,
                               bool bPreserveDirection,
                               bool bGetTan, 
                               bool bCheckDot,
                               double dot_tol//Only used if bCheckDot is true
                              )

{
  Destroy();
  if (Curves.Count() == 0)
    return false;
  for (int i=0; i<2; i++){
    m_Ends[i] = new JoinCurveEnd[Curves.Count()];
    if (!m_Ends[i])
      return false;
  }
  bool bGotOne = false;
  for (int i=0; i<Curves.Count(); i++){
    for (int j=0; j<2; j++){
      JoinCurveEnd& J = m_Ends[j][i];
      if (Curves[i]){
        J.Create(i, *Curves[i], j, bGetTan);
        bGotOne = true;
      }
    }
  }

  m_CurveCount = Curves.Count();

  if (!bGotOne)
    return false;

  MYON_RTree Tree;

  for (int i=0; i<m_CurveCount; i++){
    for (int j=0; j<2; j++){
      const JoinCurveEnd& J = m_Ends[j][i];
      if (J.m_cid < 0)
        continue;
      double min[3], max[3];
      for (int k=0; k<3; k++)
        min[k] = max[k] = J.m_P[k];
      if (!Tree.Insert(min, max, (void*)&J))
        return false;
    }
  }
  JoinTreeContext JTC;
  JTC.bCheckDot = bCheckDot;
  JTC.bPreserveDirection = bPreserveDirection;
  JTC.dot_tol = dot_tol;
  JTC.Pairs = &m_Pairs;
  if (!Tree.Search(dtol, JoinEndCallback, (void*)&JTC))
    return false;

  return true;
}

/////////////////////////////////////////

static bool GetCurveEndData(const MYON_SimpleArray<MYON_Curve*>& IC,
                              double join_tol, double dot_tol,
                              bool bUseTanAngle,
                              bool bPreserveDirection,
                              MYON_SimpleArray<CurveJoinEndData>& EData)

{
  JoinCurveEndArray JCA;
  if (!JCA.Create(IC, join_tol, bPreserveDirection, bUseTanAngle, (dot_tol > 0.0) ? true : false, dot_tol))
    return false;
  for (int i=0; i<JCA.m_Pairs.Count(); i++){
    JoinEndPair& Pair = JCA.m_Pairs[i];
    CurveJoinEndData& ED = EData.AppendNew();
    ED.dist = Pair.dist;
    ED.end[0] = Pair.a->m_end;
    ED.end[1] = Pair.b->m_end;
    ED.id[0] = Pair.a->m_cid;
    ED.id[1] = Pair.b->m_cid;
    ED.tan_dot = Pair.dot;
  }
  return true;
}



static bool GetCurveEndData(int count, 
                       const MYON_3dPoint* StartPoints, const MYON_3dPoint* EndPoints,//size count
                       const MYON_3dVector* StartTans, const MYON_3dVector* EndTans,//nullptr or size count 
                       double join_tol, double dot_tol,
                       bool bUseTanAngle,
                       bool bPreserveDirection,
                       MYON_SimpleArray<CurveJoinEndData>& EData)

{
  join_tol = join_tol * join_tol;

  EData.Reserve(count);
  bool bHaveTans = (StartTans && EndTans) ? true : false;
  if (dot_tol < 0.0)
    dot_tol = 0.0;
  bool bCheckDot = (dot_tol > 0.0) ? true : false;
  int i;
  for (i=1; i<count; i++){
    int j;
    for (j=0; j<i; j++){
      bool bDoIt[2][2];
      double dist[2][2];
      double dot[2][2];
      for (int endi=0; endi<2; endi++){
        for (int endj=0; endj<2; endj++){
          bDoIt[endi][endj] = (!bPreserveDirection || endi != endj) ? true : false;
          dist[endi][endj] = dot[endi][endj] = 0.0;
        }
      }
      for (int endi=0; endi<2; endi++){
        const MYON_3dPoint& Pi = (endi) ? EndPoints[i] : StartPoints[i];
        for (int endj=0; endj<2; endj++){
          const MYON_3dPoint& Pj = (endj) ? EndPoints[j] : StartPoints[j];
          dist[endi][endj] = (Pi-Pj).LengthSquared();
          if (dist[endi][endj] >= join_tol)
            bDoIt[endi][endj] = false;
        }
      }

      if (dist[0][0] < dist[0][1])
        bDoIt[0][1] = false;
      else if (dist[0][0] > dist[0][1])
        bDoIt[0][0] = false;
      if (dist[1][0] < dist[1][1])
        bDoIt[1][1] = false;
      else if (dist[1][0] > dist[1][1])
        bDoIt[1][0] = false;

      if (bHaveTans){
        for (int endi=0; endi<2; endi++){
          const MYON_3dVector& Ti = (endi) ? EndTans[i] : StartTans[i];
          for (int endj=0; endj<2; endj++){
            if (!bDoIt[endi][endj])
              continue;
            const MYON_3dVector& Tj = (endj) ? EndTans[j] : StartTans[j];
            dot[endi][endj] = Ti*Tj;
            if (endi==endj)
              dot[endi][endj] *= -1.0;
            if (bCheckDot && dot[endi][endj] <= dot_tol)
              bDoIt[endi][endj] = false;
          }
        }
      }

      for (int endi=0; endi<2; endi++){
        for (int endj=0; endj<2; endj++){
          if (!bDoIt[endi][endj])
            continue;
          CurveJoinEndData& ED = EData.AppendNew();
          ED.id[0] = i;
          ED.end[0] = endi;
          ED.id[1] = j;
          ED.end[1] = endj;
          ED.dist = dist[endi][endj];
          ED.tan_dot = dot[endi][endj];
        }
      }
    }
  }
  return true;
}

static void SortCurveEndData(int count, MYON_SimpleArray<CurveJoinEndData>& EData, 
                             double dist_tol, double dot_tol, bool bUseTanAngle,
                             MYON_ClassArray<MYON_SimpleArray<CurveJoinSeg> >& SegsArray,
                             MYON_SimpleArray<int>& Singles)

{
  //sort possiblities by distance
  JoinEndCompareContext context;
  context.bUseTan = bUseTanAngle;
  context.dot_tol = dot_tol;
  context.dist_tol = dist_tol;
  MYON_qsort((void*)EData.Array(), EData.Count(), sizeof(CurveJoinEndData), CompareJoinEnds, (void*)&context);

  int* endspace = (int*)onmalloc(2*sizeof(int)*count);
  memset(endspace, 0, 2*sizeof(int)*count);
  int** endarray = (int**)onmalloc(sizeof(int*)*count);

  //endarray[i] is an int[2].  if endarray[i][0] > 0, then IC[i] is part of
  //polycurve endarray[i][0] - 1, and the start of IC[i] is interior to the polycurve.
  //if endarray[i][1] > 0, then the end of IC[i] is interior.  if both endarray[i][0] > 0
  //and endarray[i][1] > 0, then they are equal.

  for (int i=0; i<count; i++)
    endarray[i] = &endspace[2*i];

  SegsArray.Reserve(count);

  for (int i=0; i<EData.Count(); i++){
    const CurveJoinEndData& ED = EData[i];
    if (endarray[ED.id[0]][ED.end[0]] > 0 || endarray[ED.id[1]][ED.end[1]] > 0)
      continue; //one of these endpoints has already been join to a closer end
    if (endarray[ED.id[0]][1 - ED.end[0]] == 0){
      if (endarray[ED.id[1]][1 - ED.end[1]] == 0){//new curve
        endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = SegsArray.Count()+1;
        MYON_SimpleArray<CurveJoinSeg>& SArray = SegsArray.AppendNew();
        SArray.Reserve(4);
        CurveJoinSeg& Seg0 = SArray.AppendNew();
        CurveJoinSeg& Seg1 = SArray.AppendNew();
        if (ED.end[0]) {
          Seg0.id = ED.id[0];
          Seg0.bRev = false;
          Seg1.id = ED.id[1];
          Seg1.bRev = (ED.end[1]) ? true : false;
        }
        else {
          Seg1.id = ED.id[0];
          Seg1.bRev = false;
          Seg0.id = ED.id[1];
          Seg0.bRev = (ED.end[1]) ? false : true;
        }
      }

      else {

        //second curve is part of an existing sequence. Insert or append first curve.
        MYON_SimpleArray<CurveJoinSeg>& SArray = SegsArray[endarray[ED.id[1]][1 - ED.end[1]] - 1];
        endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = 
          endarray[ED.id[1]][1 - ED.end[1]];

        if (SArray[0].id == ED.id[1]){
          CurveJoinSeg Seg;
          Seg.id = ED.id[0];
          Seg.bRev = (ED.end[0]) ? false : true;
          SArray.Insert(0, Seg);
        }
        else {
          CurveJoinSeg& Seg = SArray.AppendNew();
          Seg.id = ED.id[0];
          Seg.bRev = (ED.end[0]) ? true : false;
        }
      }
    }
    else if (endarray[ED.id[1]][1 - ED.end[1]] == 0){
    //first curve is part of an existing sequence. Insert or append second curve.
      MYON_SimpleArray<CurveJoinSeg>& SArray = SegsArray[endarray[ED.id[0]][1 - ED.end[0]] - 1];
      endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = 
        endarray[ED.id[0]][1 - ED.end[0]];

      if (SArray[0].id == ED.id[0]){
        CurveJoinSeg Seg;
        Seg.id = ED.id[1];
        Seg.bRev = (ED.end[1]) ? false : true;
        SArray.Insert(0, Seg);
      }
      else {
        CurveJoinSeg& Seg = SArray.AppendNew();
        Seg.id = ED.id[1];
        Seg.bRev = (ED.end[1]) ? true : false;
      }
    }
    else {
      //both are in existing sequences.  join the sequences.
      if (endarray[ED.id[0]][1 - ED.end[0]] == endarray[ED.id[1]][1 - ED.end[1]])
        //closes off this curve
        endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = 
          endarray[ED.id[0]][1 - ED.end[0]];
      else {
        int segid0 = endarray[ED.id[0]][1 - ED.end[0]];
        int segid1 = endarray[ED.id[1]][1 - ED.end[1]];
        MYON_SimpleArray<CurveJoinSeg>& SArray0 = SegsArray[segid0 - 1];
        MYON_SimpleArray<CurveJoinSeg>& SArray1 = SegsArray[segid1 - 1];
        if (SArray0[0].id == ED.id[0]){
          if (SArray1[0].id == ED.id[1]){
            ReverseSegs(SArray0);
            int j;
            for (j=0; j<SArray1.Count(); j++){
              if (endarray[SArray1[j].id][0] > 0) endarray[SArray1[j].id][0] = segid0;
              if (endarray[SArray1[j].id][1] > 0) endarray[SArray1[j].id][1] = segid0;
              SArray0.Append(SArray1[j]);
            }
            endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = segid0;
            SArray1.SetCount(0);
          }
          else {
            int j;
            for (j=0; j<SArray0.Count(); j++){
              if (endarray[SArray0[j].id][0] > 0) endarray[SArray0[j].id][0] = segid1;
              if (endarray[SArray0[j].id][1] > 0) endarray[SArray0[j].id][1] = segid1;
              SArray1.Append(SArray0[j]);
            }
            endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = segid1;
            SArray0.SetCount(0);
          }
        }
        else if (SArray1[0].id == ED.id[1]){
          int j;
          for (j=0; j<SArray1.Count(); j++){
            if (endarray[SArray1[j].id][0] > 0) endarray[SArray1[j].id][0] = segid0;
            if (endarray[SArray1[j].id][1] > 0) endarray[SArray1[j].id][1] = segid0;
            SArray0.Append(SArray1[j]);
          }
          endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = segid0;
          SArray1.SetCount(0);
        }
        else {
          ReverseSegs(SArray1);
          int j;
          for (j=0; j<SArray1.Count(); j++) {
            if (endarray[SArray1[j].id][0] > 0) endarray[SArray1[j].id][0] = segid0;
            if (endarray[SArray1[j].id][1] > 0) endarray[SArray1[j].id][1] = segid0;
            SArray0.Append(SArray1[j]);
          }
          endarray[ED.id[0]][ED.end[0]] = endarray[ED.id[1]][ED.end[1]] = segid0;
          SArray1.SetCount(0);
        }
      }
    }
  }
  for (int i=0; i<count; i++){
    if (endarray[i][0] == 0 && endarray[i][1] == 0)
      Singles.Append(i);
  }

  onfree((void*)endarray);
  onfree((void*)endspace);
}

static bool SortEnds(int count, 
                     const MYON_3dPoint* StartPoints, const MYON_3dPoint* EndPoints,//size count
                     const MYON_3dVector* StartTans, const MYON_3dVector* EndTans,//nullptr or size count 
                     double join_tol, double kink_tol,
                     bool bUseTanAngle,
                     bool bPreserveDirection,
                     MYON_ClassArray<MYON_SimpleArray<CurveJoinSeg> >& SegsArray,
                     MYON_SimpleArray<int>& Singles
                     )

{
  if (!StartPoints || !EndPoints)
    return false;
  //get a list of all possible joins
  MYON_SimpleArray<CurveJoinEndData> EData;
  double dot_tol = (kink_tol > 0.0) ? cos(kink_tol) : 0.0;
  GetCurveEndData(count, StartPoints, EndPoints, StartTans, EndTans,
    join_tol, dot_tol, bUseTanAngle, bPreserveDirection, EData);

  SortCurveEndData(count, EData, 0.3*join_tol, 0.99984, bUseTanAngle, SegsArray, Singles);

  return true;
}

static bool SortEnds(const MYON_SimpleArray<MYON_Curve*>& IC,//Open, non-NULL
                     double join_tol, double kink_tol,
                     bool bUseTanAngle,
                     bool bPreserveDirection,
                     MYON_ClassArray<MYON_SimpleArray<CurveJoinSeg> >& SegsArray,
                     MYON_SimpleArray<int>& Singles
                     )

{
  //get a list of all possible joins
  MYON_SimpleArray<CurveJoinEndData> EData;
  double dot_tol = (kink_tol > 0.0) ? cos(kink_tol) : 0.0;
  if (!GetCurveEndData(IC,join_tol, dot_tol, bUseTanAngle, bPreserveDirection, EData))
    return false;

  SortCurveEndData(IC.Count(), EData, 0.3*join_tol, 0.99984, bUseTanAngle, SegsArray, Singles);

  return true;
}



int 
MYON_JoinCurves(const MYON_SimpleArray<const MYON_Curve*>& InCurves,
                      MYON_SimpleArray<MYON_Curve*>& OutCurves,
                      double join_tol,
                      double kink_tol,
                      bool bUseTanAngle,
                      bool bPreserveDirection, // = false
                      MYON_SimpleArray<int>* key //=0
                      )

{
  bool bGetTans = (bUseTanAngle || kink_tol > 0.0) ? true : false;
  double dot_tol = (kink_tol > 0.0) ? cos(kink_tol) : 0.0;
  if (dot_tol < 0.0)
    dot_tol = 0.0;
  int i, count = OutCurves.Count();
  if (InCurves.Count() < 1)
    return 0;

  int dim = InCurves[0]->Dimension();
  for (i=1; i<InCurves.Count(); i++){
    if (0 != InCurves[i] && InCurves[i]->Dimension() != dim) return 0;
  }

  if (key) {
    key->Reserve(InCurves.Count());
    for (i=0; i<InCurves.Count(); i++) key->Append(-1);
  }

  //Copy curves, take out closed curves. 
  OutCurves.Reserve(InCurves.Count());
  MYON_SimpleArray<MYON_Curve*> IC(InCurves.Count());
  MYON_SimpleArray<int> cmap(InCurves.Count());
  for (i=0; i<InCurves.Count(); i++){
    if (0 == InCurves[i])  // 8 April, 2014 - Lowell - Rh-26021
      continue;
    MYON_Curve* C = InCurves[i]->DuplicateCurve();
    if (!C) continue;
    if (C->IsClosed()) {
      if (key) (*key)[i] = OutCurves.Count();
      OutCurves.Append(C);
    }
    else {
      cmap.Append(i);
      IC.Append(C);
    }
  }

  if (IC.Count() < 1)
    return OutCurves.Count() - count;

  MYON_ClassArray<MYON_SimpleArray<CurveJoinSeg> > SegsArray;
  MYON_SimpleArray<int> Singles;

  //IC is a list of copies of all open curves.  match endpoints and join into polycurves.
  //copy curves that are not joined.

  bool bNewWay = true;

  if (!bNewWay){
    MYON_3dPointArray Start(IC.Count());
    Start.SetCount(IC.Count());
    MYON_SimpleArray<MYON_3dVector> StartTan;
    if (bGetTans){
      StartTan.Reserve(IC.Count());
      StartTan.SetCount(IC.Count());
    }
    MYON_3dPointArray End(IC.Count());
    End.SetCount(IC.Count());
    MYON_SimpleArray<MYON_3dVector> EndTan;
    if (bGetTans){
      EndTan.Reserve(IC.Count());
      EndTan.SetCount(IC.Count());
    }
    for (i=0; i<IC.Count(); i++){
      Start[i] = IC[i]->PointAtStart();
      End[i] = IC[i]->PointAtEnd();
      if (bGetTans){
        StartTan[i] = IC[i]->TangentAt(IC[i]->Domain()[0]);
        EndTan[i] = IC[i]->TangentAt(IC[i]->Domain()[1]);
      }
    }

    SortEnds(IC.Count(), Start.Array(), End.Array(), 
      (bGetTans) ? StartTan.Array() : 0, (bGetTans) ? EndTan.Array() : 0, 
      join_tol, kink_tol, bUseTanAngle, bPreserveDirection, SegsArray, Singles);
  }

  else
    SortEnds(IC, 
      join_tol, kink_tol, bUseTanAngle, bPreserveDirection, SegsArray, Singles);

  //make polycurves out of sequences
  for (i=0; i<SegsArray.Count(); i++){

    MYON_SimpleArray<CurveJoinSeg>& SArray = SegsArray[i];
    if (SArray.Count() < 2) continue;
    if (!bPreserveDirection)
    {
      //if number of reversed segs is more than half, reverse.
      int count_local= 0;
      int j;
      for (j=0; j<SArray.Count(); j++) {
        if (SArray[j].bRev)
          count_local++;
      }
      if (2*count_local > SArray.Count())
        ReverseSegs(SArray);
    }
    MYON_PolyCurve* PC = new MYON_PolyCurve(SArray.Count());
    bool pc_added = false;
    int j;
    int min_seg = 0;
    int min_id = -1;
    for (j=0; j<SArray.Count(); j++){
      if (key) 
        (*key)[cmap[SArray[j].id]] = OutCurves.Count();
      MYON_Curve* C = IC[SArray[j].id];
      if (min_id < 0 || SArray[j].id < min_id){
        min_id = SArray[j].id;
        min_seg = j;
      }
      if (SArray[j].bRev) C->Reverse();
      if (PC->Count()){
        bool bSet = true;
        if (!MYON_ForceMatchCurveEnds(*PC, 1, *C, 0)) {
          MYON_3dPoint P = PC->PointAtEnd();
          MYON_3dPoint Q = C->PointAtStart();
          P = 0.5*(P+Q);
          if (!PC->SetEndPoint(P) || !C->SetStartPoint(P)) {
            MYON_NurbsCurve* NC = C->NurbsCurve();
            if (NC && NC->SetStartPoint(P)){
              delete C;
              C = NC;
            }
            else {
              bSet = false;
              delete NC;
              if (PC->Count()) {
                pc_added = true;
                OutCurves.Append(PC);
              }
              if (key)
                (*key)[cmap[SArray[j].id]]++;
              OutCurves.Append(C);
              int k;
              for (k=j+1; k<SArray.Count(); k++){
                if (key)
                  (*key)[cmap[SArray[k].id]] = OutCurves.Count();
                OutCurves.Append(IC[SArray[k].id]);
              }
              break;
            }
          }
        }
      }
      MYON_PolyCurve* pPoly = MYON_PolyCurve::Cast(C);
      if( pPoly){
        int si;
        for (si=0; si<pPoly->Count(); si++){
          const MYON_Curve* SC = pPoly->SegmentCurve(si);
          MYON_Curve* SCCopy = SC->DuplicateCurve();
          if (SCCopy) PC->Append(SCCopy);
        }
        delete pPoly;
      }
      else PC->Append(C);
    }
    if (!PC->Count()) delete PC;
    else if (!pc_added) {
      if (!PC->IsClosed() && PC->IsClosable(join_tol)){
        if (!MYON_ForceMatchCurveEnds(*PC, 0, *PC, 1))
          PC->SetEndPoint(PC->PointAtStart());
      }
      if (PC->IsClosed() && min_id >= 0){
        //int sc = PC->SpanCount();
        double t = PC->SegmentDomain(min_seg)[0];
        PC->ChangeClosedCurveSeam(t);
      }

      // 28 October 2010 Dale Lear
      //    I added the RemoveNesting() and SynchronizeSegmentDomains()
      //    lines so Rhino will create higher quality geometry.
      PC->RemoveNesting();
      PC->SynchronizeSegmentDomains();

      OutCurves.Append(PC);
    }
  }

  //add in singletons
  for (i=0; i<Singles.Count(); i++){
    if (key) (*key)[cmap[Singles[i]]] = OutCurves.Count();
    OutCurves.Append(IC[Singles[i]]);
  }

	/* This was added by greg to fix big curves that are nearly, but not quite, closed.
     It causes problems when the curve is tiny.
	for(i=0; i<OutCurves.Count(); i++){
		if(!OutCurves[i]->IsClosed()){
			MYON_3dPoint s= OutCurves[i]->PointAtStart();
			MYON_3dPoint e = OutCurves[i]->PointAtEnd();
			if(s.DistanceTo(e)<join_tol)
				OutCurves[i]->SetEndPoint( s );			
		}
	}
  */

  //Chuck added this, 1/16/03.
  for(i=0; i<OutCurves.Count(); i++){
    MYON_Curve* C = OutCurves[i];
    if (!C || C->IsClosed()) continue;
    if (C->IsClosable(join_tol))
      C->SetEndPoint(C->PointAtStart());
  }
    
  return OutCurves.Count() - count;
}

static bool PolylineIsClosable(const MYON_Polyline& pline, double tol)

{
  if (pline.PointCount() < 4 || pline.IsClosed())
    return false;
  int id[6];
  id[0] = 0;
  id[5] = pline.PointCount()-1;

  double gap = pline[id[0]].DistanceTo(pline[id[5]]);
  if (gap > tol) 
    return false;
  double min_rel_size = 10.0;
  if (pline.PointCount() < 6)
    return (pline.Length() < min_rel_size*gap) ? false : true;

  int i;
  double len = 0.0;

  for (i=1; i<6; i++){
    if (i!=5)
      id[i] = (pline.PointCount()*i)/5;
    len += pline[id[i-1]].DistanceTo(pline[id[i]]);
    if (len >= min_rel_size*gap)
      return true;
  }
  return false;
}

int MYON_JoinPolylines(
  const MYON_SimpleArray<const MYON_Polyline*>& InPlines,
  MYON_SimpleArray<MYON_Polyline*>& OutPlines,
  double join_tol,
  double kink_tol,
  bool bUseTanAngle,
  bool bPreserveDirection, // = false
  MYON_SimpleArray<int>* key //=0
)
{
  bool bGetTans = (bUseTanAngle || kink_tol > 0.0) ? true : false;
  double dot_tol = (kink_tol > 0.0) ? cos(kink_tol) : 0.0;
  if (dot_tol < 0.0)
    dot_tol = 0.0;
  int i, count = OutPlines.Count();
  if (InPlines.Count() < 1)
    return 0;

  if (key) {
    key->Reserve(InPlines.Count());
    for (i=0; i<InPlines.Count(); i++) key->Append(-1);
  }

  //Copy curves, take out closed curves. 
  OutPlines.Reserve(InPlines.Count());
  MYON_SimpleArray<MYON_Polyline*> IC(InPlines.Count());
  MYON_SimpleArray<int> cmap(InPlines.Count());
  for (i=0; i<InPlines.Count(); i++){
    if (0 == InPlines[i] || InPlines[i]->PointCount() < 2)  // 8 April, 2014 - Lowell - Rh-26021
      continue;
    MYON_Polyline* C = new MYON_Polyline(*InPlines[i]);
    if (!C) continue;
    if (C->IsClosed()) {
      if (key) (*key)[i] = OutPlines.Count();
      OutPlines.Append(C);
    }
    else {
      cmap.Append(i);
      IC.Append(C);
    }
  }

  if (IC.Count() < 1)
    return OutPlines.Count() - count;

  //IC is a list of copies of all open curves.  match endpoints and join into polycurves.
  //copy curves that are not joined.
  MYON_3dPointArray Start(IC.Count());
  Start.SetCount(IC.Count());
  MYON_SimpleArray<MYON_3dVector> StartTan;
  if (bGetTans){
    StartTan.Reserve(IC.Count());
    StartTan.SetCount(IC.Count());
  }
  MYON_3dPointArray End(IC.Count());
  End.SetCount(IC.Count());
  MYON_SimpleArray<MYON_3dVector> EndTan;
  if (bGetTans){
    EndTan.Reserve(IC.Count());
    EndTan.SetCount(IC.Count());
  }
  for (i=0; i<IC.Count(); i++){
    Start[i] = IC[i]->PointAt(0.0);
    End[i] = IC[i]->PointAt((double)(IC[i]->PointCount())-1.0);
    if (bGetTans){
      StartTan[i] = IC[i]->TangentAt(0.0);
      EndTan[i] = IC[i]->TangentAt((double)(IC[i]->PointCount())-1.0);
    }
  }

  MYON_ClassArray<MYON_SimpleArray<CurveJoinSeg> > SegsArray;
  MYON_SimpleArray<int> Singles;
  SortEnds(IC.Count(), Start.Array(), End.Array(), 
    (bGetTans) ? StartTan.Array() : 0, (bGetTans) ? EndTan.Array() : 0, 
    join_tol, kink_tol, bUseTanAngle, bPreserveDirection, SegsArray, Singles);

  //make polylines out of sequences
  for (i=0; i<SegsArray.Count(); i++){
    MYON_SimpleArray<CurveJoinSeg>& SArray = SegsArray[i];
    if (SArray.Count() < 2) continue;
    if (!bPreserveDirection)
    {
      //if number of reversed segs is more than half, reverse.
      int count_local= 0;
      int j;
      for (j=0; j<SArray.Count(); j++) {
        if (SArray[j].bRev)
          count_local++;
      }
      if (2*count_local > SArray.Count())
        ReverseSegs(SArray);
    }
    MYON_Polyline* Pline = 0;
    int j;
    int min_seg = 0;
    int min_id = -1;
    for (j=0; j<SArray.Count(); j++){
      if (key) 
        (*key)[cmap[SArray[j].id]] = OutPlines.Count();
      MYON_Polyline* C = IC[SArray[j].id];
      if (min_id < 0 || SArray[j].id < min_id){
        min_id = SArray[j].id;
        min_seg = j;
      }
      if (SArray[j].bRev) C->Reverse();
      if (Pline){
        MYON_3dPoint P = (*Pline)[Pline->PointCount()-1];
        MYON_3dPoint& Q = (*C)[0];
        Q = 0.5*(P+Q);
        Pline->Remove();
        Pline->Append(C->PointCount(), C->Array());
        delete C;
        IC[SArray[j].id] = 0;
      }
      else
        Pline = C;
    }
    if (Pline)
      OutPlines.Append(Pline);
  }

  //add in singletons
  for (i=0; i<Singles.Count(); i++){
    if (key) (*key)[cmap[Singles[i]]] = OutPlines.Count();
    OutPlines.Append(IC[Singles[i]]);
  }

  for(i=0; i<OutPlines.Count(); i++){
    MYON_Polyline* C = OutPlines[i];
    if (!C || C->IsClosed()) continue;
    if (PolylineIsClosable(*C, join_tol))
      (*C)[C->PointCount()-1] = (*C)[0];
  }
    
  return OutPlines.Count() - count;
}

int MYON_JoinLines(
  const MYON_SimpleArray<MYON_Line>& InLines,
  MYON_ClassArray<MYON_Polyline>& OutPolylines,
  double tolerance,
  bool bPreserveDirection,
  MYON_SimpleArray<int>* pKey
)
{
  const int inlines_count = InLines.Count();
  if (0 == inlines_count)
    return 0;

  if (pKey)
  {
    pKey->Reserve(inlines_count);
    pKey->SetCount(inlines_count);
    memset((void*)pKey->Array(), -1, (size_t)pKey->SizeOfArray());
  }

  const int outpolylines_count = OutPolylines.Count();
  OutPolylines.Reserve(InLines.Count());

  MYON_SimpleArray<int> index_map(inlines_count);
  index_map.SetCount(inlines_count);
  for (int i = 0; i < inlines_count; i++)
    index_map[i] = i;

  MYON_3dPointArray Start(inlines_count);
  Start.SetCount(inlines_count);
  MYON_3dPointArray End(inlines_count);
  End.SetCount(inlines_count);
  for (int i = 0; i < inlines_count; i++)
  {
    Start[i] = InLines[i].from;
    End[i] = InLines[i].to;
  }

  MYON_ClassArray<MYON_SimpleArray<CurveJoinSeg>> SegsArray;
  MYON_SimpleArray<int> Singles;
  SortEnds(
    inlines_count, 
    Start.Array(), 
    End.Array(), 
    nullptr, 
    nullptr, 
    tolerance, 
    MYON_UNSET_VALUE, 
    false, 
    bPreserveDirection, 
    SegsArray, 
    Singles
  );

  for (int i = 0; i < SegsArray.Count(); i++) 
  {
    MYON_SimpleArray<CurveJoinSeg>& SArray = SegsArray[i];
    if (SArray.Count() < 2) 
      continue;

    if (!bPreserveDirection)
    {
      // If number of reversed segs is more than half, reverse.
      int count_local = 0;
      for (int j = 0; j < SArray.Count(); j++) 
      {
        if (SArray[j].bRev)
          count_local++;
      }
      if (2 * count_local > SArray.Count())
        ReverseSegs(SArray);
    }

    MYON_Polyline& polyline = OutPolylines.AppendNew();
    int min_seg = 0;
    int min_id = -1;
    for (int j = 0; j < SArray.Count(); j++) 
    {
      if (pKey)
        (*pKey)[index_map[SArray[j].id]] = OutPolylines.Count();

      MYON_Line line = InLines[SArray[j].id];
      if (min_id < 0 || SArray[j].id < min_id) 
      {
        min_id = SArray[j].id;
        min_seg = j;
      }

      if (SArray[j].bRev)
        line.Reverse();

      if (0 == polyline.Count())
        polyline.Append(line.from);
      polyline.Append(line.to);
    }
  }

  for (int i = 0; i < OutPolylines.Count(); i++) 
  {
    MYON_Polyline& polyline = OutPolylines[i];
    if (!polyline.IsClosed() && PolylineIsClosable(polyline, tolerance))
      polyline.Append(polyline[0]);
  }

  return OutPolylines.Count() - outpolylines_count;
}

// returns true if t is sufficiently close to m_t[index]
// -1 <= index <= m_t.Count()
bool MYON_Curve::ParameterSearch(double t, int& index, bool bEnableSnap,
																const MYON_SimpleArray<double>& m_t, double RelTol) const{

  // 24 October 2003 Dale Lear - added comments and fixed bugs when t < m_t[0]
  //    If you make changes to this code, please discuss them with Dale Lear.

	bool rc = false;
	int count = m_t.Count();
	MYON_Interval c_dom = Domain();
	index = -1;
	if(count>1 && MYON_IsValid(t))
  {
		
    index = MYON_SearchMonotoneArray(m_t, count, t);
    // index < 0       : means t < m_t[0]
    // index == count-1: means t == m_t[count-1]
    // index == count  : means t > m_t[count-1]

		rc  = (index>=0 && index<=count-1  && t == m_t[index]);
		if( bEnableSnap && !rc)
    {
      // see if t is within "ktol" of a value in m_t[]
			double ktol = RelTol*( MYON_Max(fabs(c_dom[0]) ,fabs(c_dom[1])));

      if (index >= 0 && index < count-1 )
      {
        // If we get here, then m_t[index] < t < m_t[index+1]
        double middle_t = 0.5*(m_t[index] + m_t[index+1]);
			  if( t < middle_t && t - m_t[index] <= ktol)
        {
          // t is a hair bigger than m_t[index]
					rc = true;
			  } 
        else if( t > middle_t && m_t[index+1]-t <= ktol)
        {
          // t is a hair smaller than m_t[index+1]
					rc = true;
					index ++;
			  }	
      }
      else if (index == count)
      {
        // If we get here, then t > m_t[count-1]
				if( t-m_t[count-1]<=ktol)
        {
          // t is a hair bigger than m_t[count-1]
					rc = true;
					index = count-1;
				}
			}
      else if (index<0)
      {
        // 22 October 2003 Dale Lear - added this case to match the index==count case above.
        // If we get here, then t < m_t[0]
				if( m_t[0]-t <= ktol)
        {
          // t is a hair smaller than m_t[count-1]
					rc = true;
					index = 0;
				}
			}
		}
	}
	return rc;
}

bool MYON_SortLines( 
        int line_count, 
        const MYON_Line* line_list, 
        int* index, 
        bool* bReverse 
        )
{
  MYON_3dPoint StartP, EndP, Q;
  double d, startd, endd;
  int Ni, start_i, start_end, end_i, end_end, i, end;

  if ( index ) 
  {
    for ( i = 0; i < line_count; i++ ) 
      index[i] = i;
  }
  if ( bReverse  )
  {
    for ( i = 0; i < line_count; i++ ) 
      bReverse[i] = false;
  }
  if ( line_count < 1 || 0 == line_list || 0 == index || 0 == bReverse )
  {
    MYON_ERROR("MYON_SortLines - illegal input");
    return false;
  }
  if ( 1 == line_count )
  {
    return true;
  }

  // sort lines
  for ( Ni = 1; Ni < line_count; Ni++ ) 
  {
    /* index[] = some permutation of {0,...,line_count-1}
    // N[index[0]], ..., N[index[Ni-1]] are in order
    // N[index[j]] needs to be reversed if bReverse[j] is true
    */
    start_i = end_i = Ni;
    start_end = end_end = 0;
    StartP = line_list[ index[0]    ][(bReverse[0])    ? 1 : 0];
    EndP   = line_list[ index[Ni-1] ][(bReverse[Ni-1]) ? 0 : 1];
    startd = StartP.DistanceTo( line_list[index[start_i]].from ); // "from" is correct here
    endd   = EndP.DistanceTo( line_list[index[end_i]].from );     // "from" is correct here

    for ( i = Ni; i < line_count; i++ ) 
    {
      Q = line_list[index[i]].from;
      for ( end = 0; end < 2; end++ ) 
      {
        d = StartP.DistanceTo( Q );
        if ( d < startd ) 
        {
          start_i = i;
          start_end = end;
          startd = d;
        }

        d = EndP.DistanceTo( Q );
        if ( d < endd ) 
        {
          end_i = i;
          end_end = end;
          endd = d;
        }

        Q = line_list[index[i]].to;
      }
    }

    if ( startd < endd ) 
    {
      // N[index[start_i]] will be first in list
      i = index[Ni];
      index[Ni] = index[start_i];
      index[start_i] = i;
      start_i = index[Ni];
      for ( i = Ni; i > 0; i-- ) 
      {
        index[i] = index[i-1];
        bReverse[i] = bReverse[i-1];
      }
      index[0] = start_i;
      bReverse[0] = (start_end != 1);
    }
    else 
    {
      // N[index[end_i]] will be next in the list
      i = index[Ni];
      index[Ni] = index[end_i];
      index[end_i] = i;
      bReverse[Ni] = (end_end == 1);
    }
  }

  return true;
}



bool MYON_SortLines( 
        const MYON_SimpleArray<MYON_Line>& line_list,
        int* index, 
        bool* bReverse 
        )
{
  return MYON_SortLines(line_list.Count(),line_list.Array(),index,bReverse);
}

bool MYON_SortCurves( int curve_count, const MYON_Curve* const* curve_list, int* index, bool* bReverse )
{
  int i;

  if ( curve_count < 1 || 0 == curve_list || 0 == curve_list[0] || 0 == index || 0 == bReverse )
  {
    if ( index ) 
    {
      for ( i = 0; i < curve_count; i++ ) 
        index[i] = i;
    }
    if ( bReverse  )
    {
      for ( i = 0; i < curve_count; i++ ) 
        bReverse[i] = false;
    }
    MYON_ERROR("MYON_SortCurves - illegal input");
    return false;
  }

  if ( 1 == curve_count )
  {
    index[0] = 0;
    bReverse[0] = false;
    return true;
  }

  // get start and end points
  MYON_SimpleArray< MYON_Line > line_list(curve_count);
  MYON_Interval d;
  bool rc = true;
  for ( i = 0; i < curve_count; i++ ) 
  {
    index[i] = i;
    bReverse[0] = false;
    if ( !rc )
      continue;
    const MYON_Curve* curve = curve_list[i];
    if ( !curve )
    {
      rc = false;
      continue;
    }
    d = curve->Domain();
    if ( !d.IsIncreasing() )
    {
      rc = false;
      continue;
    }
    MYON_Line& line = line_list.AppendNew();
    if ( !curve->EvPoint(d[0],line.from,1) || !curve->EvPoint(d[1],line.to,-1) )
    {
      rc = false;
    }
  }

  if ( !rc )
  {
    MYON_ERROR("MYON_SortCurves - illegal input curve");
  }
  else
  {
    rc = MYON_SortLines( curve_count, line_list, index, bReverse );
  }
  return rc;
}


bool MYON_SortCurves( const MYON_SimpleArray<const MYON_Curve*>& curves, MYON_SimpleArray<int>& index, MYON_SimpleArray<bool>& bReverse )
{
  const int curve_count = curves.Count();
  index.Reserve(curve_count);
  index.SetCount(curve_count);
  bReverse.Reserve(curve_count);
  bReverse.SetCount(curve_count);
  return MYON_SortCurves( curve_count,curves.Array(),index.Array(),bReverse.Array());
}

bool MYON_SortCurves( const MYON_SimpleArray<MYON_Curve*>& curves, MYON_SimpleArray<int>& index, MYON_SimpleArray<bool>& bReverse )
{
  const int curve_count = curves.Count();
  index.Reserve(curve_count);
  index.SetCount(curve_count);
  bReverse.Reserve(curve_count);
  bReverse.SetCount(curve_count);
  return MYON_SortCurves( curve_count,curves.Array(),index.Array(),bReverse.Array());
}


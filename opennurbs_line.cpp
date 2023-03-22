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


bool operator==(const MYON_Line& a, const MYON_Line& b)
{
  // Properly handles nans.
  return (a.from == b.from && a.to == b.to);
}

bool operator!=(const MYON_Line& a, const MYON_Line& b)
{
  // Properly handles nans.
  return (a.from != b.from || a.to != b.to);
}


MYON_Line::MYON_Line()
  : from(MYON_3dPoint::Origin)
  , to(MYON_3dPoint::Origin)
{
}

MYON_Line::MYON_Line( MYON_3dPoint from_pt, MYON_3dPoint to_pt )
  : from(from_pt)
  , to(to_pt)
{}

MYON_Line::MYON_Line( MYON_2dPoint from_pt, MYON_2dPoint to_pt )
  : from(from_pt)
  , to(to_pt)
{}

MYON_Line::~MYON_Line()
{
}

MYON_3dPoint& MYON_Line::operator[](int i)
{
  return (i<=0) ? from : to;
}

const MYON_3dPoint& MYON_Line::operator[](int i) const
{
  return (i<=0) ? from : to;
}

bool MYON_Line::Create( const MYON_3dPoint from_pt, const MYON_3dPoint to_pt )
{
  from = from_pt;
  to = to_pt;
  return IsValid();
}

bool MYON_Line::Create( const MYON_2dPoint from_pt, const MYON_2dPoint to_pt )
{
  from = MYON_3dPoint(from_pt);
  to = MYON_3dPoint(to_pt);
  return IsValid();
}


bool MYON_Line::IsValid() const
{
  return (from != to && from.IsValid() && to.IsValid());
}

double MYON_Line::Length() const
{
  return from.DistanceTo(to);
}

MYON_3dVector MYON_Line::Direction() const
{
  return (MYON_UNSET_VALUE < to.x && to.x < MYON_UNSET_POSITIVE_VALUE && MYON_UNSET_VALUE <from.x && from.x < MYON_UNSET_POSITIVE_VALUE) 
    ? (to-from)
    : (MYON_3dPoint::UnsetPoint == from && MYON_3dPoint::UnsetPoint == to ? MYON_3dVector::UnsetVector : MYON_3dVector::NanVector)
    ;
}

MYON_3dVector MYON_Line::Tangent() const
{
  MYON_3dVector V = Direction();
  V.Unitize();
  return V;
}

MYON_3dPoint MYON_Line::PointAt( double t ) const
{
  // 26 Feb 2003 Dale Lear
  //     Changed 
  //          return (1-t)*from + t*to;
  //     to the following so that axis aligned lines will
  //     return exact answers for large values of t.  
  //     See RR 9683.
 

  //08 May 2022 - Greg. Mikko, Chuck.  This is more accurate than the parameterwise 
  //(1-t)*from + t*to with fudging.  Note that if any coordinate of from is the same as that of to,
  //the answer will be exact in that coord.
  return  (t < 0.5) ? from + t*(to-from) : to + (1.0-t)*(from-to);
}

void MYON_Line::Reverse()
{
  MYON_3dPoint tmp = from;
  from = to;
  to = tmp;
}

bool MYON_Line::ClosestPointTo( const MYON_3dPoint& point, double *t ) const
{
  bool rc = false;
  if ( t ) {
    const MYON_3dVector D = Direction();
    const double DoD = D.LengthSquared();
    if ( DoD > 0.0 ) {
      if ((point - from).LengthSquared() <= (point - to).LengthSquared()) {
        *t = ((point - from)*D)/DoD;
      }
      else {
        *t = 1.0 + ((point - to)*D)/DoD;
      }
      rc = true;
    }
    else {
      *t = 0.0;
			rc = true;			// (GBA) Closet point to a degenerate line works as well
    }
  }
  return rc;
}

MYON_3dPoint MYON_Line::ClosestPointTo( const MYON_3dPoint& point ) const
{
  double t;
  ClosestPointTo( point, &t );
  return PointAt(t);
}

double MYON_Line::DistanceTo( MYON_3dPoint test_point ) const
{
  return test_point.DistanceTo(ClosestPointTo(test_point));
}


bool MYON_Line::Transform( const MYON_Xform& tr )
{
  from = tr*from;
  to = tr*to;
  // 5 June 2003 Dale Lear RR 10493
  //    Always return true.
  //return (from != to) ? true : false;
  return true;
}

// rotate line about a point and axis
bool MYON_Line::Rotate(
      double sin_angle,                  // sin(angle)
      double cos_angle,                  // cos(angle)
      const MYON_3dVector& axis,  // axis of rotation
      const MYON_3dPoint& center  // center of rotation
      )
{
  MYON_Xform rot;
  rot.Rotation( sin_angle, cos_angle, axis, center );
  const bool bFixP0 = (from==center);
  const bool bFixP1 = (to==center);
  const bool rc = Transform( rot );
  if ( bFixP0 )
    from = center;
  if ( bFixP1 )
    to = center;
  return rc;
}

bool MYON_Line::Rotate(
      double a,                 // angle in radians
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint& center // center of rotation
      )
{
  return Rotate( sin(a), cos(a), axis, center );
}

bool MYON_Line::Translate(
      const MYON_3dVector& delta
      )
{
  const MYON_Xform tr(MYON_Xform::TranslationTransformation( delta ));
  return Transform( tr );
}

int MYON_ArePointsOnLine( // returns 0=no, 1 = yes, 2 = pointset is (to tolerance) a single point on the line
        int dim,     // 2 or 3
        bool is_rat,
        int count, 
        int stride, const double* point,
        const MYON_BoundingBox& bbox, // if needed, use MYON_GetBoundingBox(dim,is_rat,count,stride,point)
        const MYON_Line& line,  // line to test
        double tolerance
        )
{
  double w;
  int i, j, k;

  if ( count < 1 )
    return 0;

  if ( !line.IsValid() )
  {
    MYON_ERROR("line parameter not valid");
    return 0;
  }
  if ( !bbox.IsValid() )
  {
    MYON_ERROR("bbox parameter not valid");
    return 0;
  }
  if ( !MYON_IsValid(tolerance) || tolerance < 0.0 )
  {
    MYON_ERROR("tolerance parameter not valid");
    return 0;
  }
  if ( dim < 2 || dim > 3 )
  {
    MYON_ERROR("dim parameter not valid");
    return 0;
  }
  if ( 0 == point )
  {
    MYON_ERROR("point parameter not valid");
    return 0;
  }
  if ( stride < (is_rat?(dim+1):dim) )
  {
    MYON_ERROR("stride parameter not valid");
    return 0;
  }

  int rc = 0;

  if ( tolerance == 0.0 ) {
    tolerance = bbox.Tolerance();
  }

  MYON_3dPoint Q(MYON_3dPoint::Origin);

  // test bounding box to quickly detect the common coordinate axis cases
  rc = (count == 1 || bbox.Diagonal().Length() <= tolerance) ? 2 : 1;
  for ( i = 0; rc && i < 2; i++ ) {
    Q.x = bbox[i].x;
    for ( j = 0; rc && j < 2; j++) {
      Q.y = bbox[j].y;
      for ( k = 0; rc && k < 2; k++) {
        Q.z = bbox[k].z;
        if ( Q.DistanceTo( line.ClosestPointTo( Q ) ) > tolerance )
          rc = 0;
      }
    }
  }

  if ( !rc ) {
    // test points one by one
    Q = MYON_3dPoint::Origin;
    rc = (count == 1 || bbox.Diagonal().Length() <= tolerance) ? 2 : 1;
    if ( is_rat ) {
      for ( i = 0; i < count; i++ ) {
        w = point[dim];
        if ( w == 0.0 ) {
          MYON_ERROR("rational point has zero weight");
          return 0;
        }
        MYON_ArrayScale( dim, 1.0/w, point, &Q.x );
        if ( Q.DistanceTo( line.ClosestPointTo( Q ) ) > tolerance ) {
          rc = 0;
          break;
        }
        point += stride;
      }
    }
    else {
      for ( i = 0; i < count; i++ ) {
        memcpy( &Q.x, point, dim*sizeof(Q.x) );
        if ( Q.DistanceTo( line.ClosestPointTo( Q ) ) > tolerance ) {
          rc = 0;
          break;
        }
        point += stride;
      }
    }
  }

  return rc;
}


MYON_BoundingBox MYON_Line::BoundingBox() const
{
  MYON_BoundingBox bbox;
  bbox.Set( 3, false, 2, 3, &from.x, false );
  return bbox;
}

bool MYON_Line::GetBoundingBox(
       MYON_BoundingBox& bbox,
       int bGrowBox
       ) const
{
  bbox.Set( 3, false, 2, 3, &from.x, bGrowBox );
  return true;
}

bool MYON_Line::InPlane( MYON_Plane& plane, double tolerance ) const
{
  const MYON_3dVector v = to-from;
  const bool bTinyX = fabs(v.x) <= tolerance;
  const bool bTinyY = fabs(v.y) <= tolerance;
  const bool bTinyZ = fabs(v.z) <= tolerance;
  bool rc = true;
  MYON_3dVector X;
  MYON_3dVector Y;
  if ( bTinyZ && ( !bTinyX || !bTinyY ) )
  {
    X = MYON_3dVector::XAxis;
    Y = MYON_3dVector::YAxis;
  }
  else if ( bTinyX && ( !bTinyY || !bTinyZ ) )
  {
    X = MYON_3dVector::YAxis;
    Y = MYON_3dVector::ZAxis;
  }
  else if ( bTinyY && ( !bTinyZ || !bTinyX ) )
  {
    X = MYON_3dVector::ZAxis;
    Y = MYON_3dVector::XAxis;
  }
  else
  {
    X = v;
    X.Unitize();
    Y.PerpendicularTo(X);
    if ( bTinyX && bTinyY && bTinyZ )
    {
      rc = false;
      if ( X.IsZero() )
      {
        X = MYON_3dVector::XAxis;
        Y = MYON_3dVector::YAxis;
      }
    }
  }
  plane.CreateFromFrame( from, X, Y );
  return rc;
}

double MYON_Line::MinimumDistanceTo( const MYON_3dPoint& P ) const
{
  double d, t;
  if (ClosestPointTo(P,&t))
  {
    if ( t < 0.0 ) t = 0.0; else if (t > 1.0) t = 1.0;
    d = PointAt(t).DistanceTo(P);
  }
  else
  {
    // degenerate line
    d = from.DistanceTo(P);
    t = to.DistanceTo(P);
    if ( t < d )
      d = t;
  }
  return d;
}

double MYON_Line::MinimumDistanceTo( const MYON_Line& L ) const
{
  MYON_3dPoint A, B;
  double a, b, t, x, d;
  bool bCheckA, bCheckB;

  bool bGoodX = MYON_Intersect(*this,L,&a,&b);

  bCheckA = true;
  if ( a < 0.0) a = 0.0; else if (a > 1.0) a = 1.0; else bCheckA=!bGoodX;
  bCheckB = true;
  if ( b < 0.0) b = 0.0; else if (b > 1.0) b = 1.0; else bCheckB=!bGoodX;

  A = PointAt(a);
  B = L.PointAt(b);
  d = A.DistanceTo(B);

  if ( bCheckA )
  {
    L.ClosestPointTo(A,&t);
    if (t<0.0) t = 0.0; else if (t > 1.0) t = 1.0;
    x = L.PointAt(t).DistanceTo(A);
    if ( x < d )
      d = x;
  }

  if ( bCheckB )
  {
    ClosestPointTo(B,&t);
    if (t<0.0) t = 0.0; else if (t > 1.0) t = 1.0;
    x = PointAt(t).DistanceTo(B);
    if (x < d )
      d = x;
  }
 
  return d;
}

double MYON_Line::MaximumDistanceTo( const MYON_3dPoint& P ) const
{
  double a, b;
  a = from.DistanceTo(P);
  b = to.DistanceTo(P);
  return ((a<b)?b:a);
}

double MYON_Line::MaximumDistanceTo( const MYON_Line& L ) const
{
  double a, b;
  a = MaximumDistanceTo(L.from);
  b = MaximumDistanceTo(L.to);
  return ((a<b)?b:a);
}

bool MYON_Line::IsFartherThan( double d, const MYON_3dPoint& P ) const
{
  if ( P.x > to.x+d && P.x > from.x+d )
  {
    return true;
  }
  if ( P.x < to.x-d && P.x < from.x-d )
  {
    return true;
  }
  if ( P.y > to.y+d && P.y > from.y+d )
  {
    return true;
  }
  if ( P.y < to.y-d && P.y < from.y-d )
  {
    return true;
  }
  if ( P.z > to.z+d && P.z > from.z+d )
  {
    return true;
  }
  if ( P.z < to.z-d && P.z < from.z-d )
  {
    return true;
  }
  return (MinimumDistanceTo(P) > d);
}


bool MYON_Line::IsFartherThan( double d, const MYON_Line& L ) const
{
  MYON_3dPoint A, B;
  double a, b, t, x;
  bool bCheckA, bCheckB;

  a = from.x; if (to.x < a) {b=a; a = to.x;} else b = to.x;
  if ( b+d < L.from.x && b+d < L.to.x )
    return true;
  if ( a-d > L.from.x && a-d > L.to.x )
    return true;

  a = from.y; if (to.y < a) {b=a; a = to.y;} else b = to.y;
  if ( b+d < L.from.y && b+d < L.to.y )
    return true;
  if ( a-d > L.from.y && a-d > L.to.y )
    return true;

  a = from.z; if (to.z < a) {b=a; a = to.z;} else b = to.z;
  if ( b+d < L.from.z && b+d < L.to.z )
    return true;
  if ( a-d > L.from.z && a-d > L.to.z )
    return true;

  if ( !MYON_Intersect(*this,L,&a,&b) )
  {
    // lines are parallel or anti parallel
    if ( Direction()*L.Direction() >= 0.0 )
    {
      // lines are parallel
      a = 0.0;
      L.ClosestPointTo(from,&b);
      // If ( b >= 0.0), then this->from and L(b) are a pair of closest points.
      if ( b < 0.0 )
      {
        // Othersise L.from and this(a) are a pair of closest points.
        b = 0.0;
        ClosestPointTo(L.from,&a);
      }
    }
    else
    {
      // lines are anti parallel
      a = 1.0;
      L.ClosestPointTo(to,&b);
      // If ( b >= 0.0), then this->to and L(b) are a pair of closest points.
      if ( b < 0.0 )
      {
        // Othersise L.to and this(a) are a pair of closest points.
        b = 0.0;
        ClosestPointTo(L.from,&a);
      }
    }
  }

  A = PointAt(a);
  B = L.PointAt(b);
  x = A.DistanceTo(B);
  if (x > d)
    return true;

  bCheckA = true;
  if ( a < 0.0) a = 0.0; else if (a > 1.0) a = 1.0; else bCheckA=false;
  if (bCheckA )
  {
    A = PointAt(a);
    L.ClosestPointTo(A,&t);
    if (t<0.0) t = 0.0; else if (t > 1.0) t = 1.0;
    x = L.PointAt(t).DistanceTo(A);
  }

  bCheckB = true;
  if ( b < 0.0) b = 0.0; else if (b > 1.0) b = 1.0; else bCheckB=false;
  if ( bCheckB )
  {
    B = L.PointAt(b);
    ClosestPointTo(B,&t);
    if (t<0.0) t = 0.0; else if (t > 1.0) t = 1.0;
    t = PointAt(t).DistanceTo(B);
    if ( bCheckA )
    {
      if ( t<x ) x = t;
    }
    else
    {
      x = t;
    }
  }
 
  return (x > d);
}

MYON_Triangle::MYON_Triangle(const MYON_3dPoint vertices[3])
{
	for(int i=0; i<3; i++)
		m_V[i] = vertices[i];
}

MYON_Triangle::MYON_Triangle(const MYON_3dPoint & a, const MYON_3dPoint & b, const MYON_3dPoint & c)
{
	m_V[0]= a; m_V[1] = b; m_V[2] = c;
}

MYON_Triangle::MYON_Triangle(double x)
{
	// Note this constructor overload is usefull so that MYON_Triangle(0) doesn't
	// get interpreted as MYON_Triangle(nullptr)
	MYON_3dPoint p(x, x, x);
	m_V[0] = m_V[1] = m_V[2] = p;
}

MYON_Triangle::MYON_Triangle(const double vertices[9])
{
  m_V[0] = MYON_3dPoint(vertices);
  m_V[1] = MYON_3dPoint(vertices ? vertices+3 : vertices);
  m_V[2] = MYON_3dPoint(vertices ? vertices+6 : vertices);
}

MYON_Triangle::operator MYON_3dPoint*()
{
	return m_V;
}

MYON_Triangle::operator const MYON_3dPoint*() const
{
	return m_V;
}

bool MYON_Triangle::IsValid() const
{
	return m_V[0].IsValid() && m_V[1].IsValid() && m_V[2].IsValid();
}

MYON_3dPoint& MYON_Triangle::operator[](int i)
{
	return m_V[i];
}

const MYON_3dPoint& MYON_Triangle::operator[](int i) const
{
	return m_V[i];
}

void MYON_Triangle::Create(const MYON_3dPoint  vertices[3])
{
	for (int i = 0; i < 3; i++)
		m_V[i] = vertices[i];
}

void MYON_Triangle::Create(const MYON_3dPoint & a, const MYON_3dPoint & b, const MYON_3dPoint & c)
{
	m_V[0] = a;
	m_V[1] = b; 
	m_V[2] = c;
}

MYON_BoundingBox MYON_Triangle::BoundingBox() const
{
	MYON_BoundingBox bbox;
	bbox.Set(3, false, 3, 3, m_V[0], false);
	return bbox;
}

bool MYON_Triangle::GetBoundingBox(
	MYON_BoundingBox& bbox,
	int bGrowBox
) const
{
	bbox.Set(3, false, 3, 3, m_V[0], bGrowBox);
	return true;
}

bool MYON_Triangle::GetTightBoundingBox(
	MYON_BoundingBox& tight_bbox,
	bool bGrowBox,
	const MYON_Xform* xform ) const
{
	if (bGrowBox && !tight_bbox.IsValid())
	{
		bGrowBox = false;
	}

	bool rc = true;
	if (xform && !xform->IsIdentity())
	{
		for (int i = 0; i < 3; i++)
		{
			MYON_3dPoint P = (*xform)*m_V[i];
			rc = tight_bbox.Set(P, bGrowBox) && rc;
			bGrowBox = true;
		}
	}
	else
	{
		rc =GetBoundingBox(tight_bbox, bGrowBox);
	}

	return rc;
}

unsigned char MYON_Triangle::LongestEdge() const
{
  double l0 = (m_V[1] - m_V[2]).LengthSquared();
  double l1 = (m_V[2] - m_V[0]).LengthSquared();
  double l2 = (m_V[0] - m_V[1]).LengthSquared();

  return l1 < l2 ? ((l2 <= l0) ? 0 : 2) : ((l1 <= l0) ? 0 : 1);
}

unsigned char MYON_Triangle::ShortestEdge() const
{
  double l0 = (m_V[1] - m_V[2]).LengthSquared();
  double l1 = (m_V[2] - m_V[0]).LengthSquared();
  double l2 = (m_V[0] - m_V[1]).LengthSquared();

  return l2 < l1 ? ((l2 < l0) ? 2 : 0) : ((l1 < l0) ? 1 : 0);
}

MYON_Line MYON_Triangle::Edge(int i) const
{
	return  MYON_Line(m_V[(i + 1) % 3], m_V[(i + 2) % 3]);
}

MYON_3dVector MYON_Triangle::Normal() const
{
	int i0 = 0; // base vetex for computation s/b opposite longest side
									 // too minimize roundoff 
									 // ( see Kahan https://people.eecs.berkeley.edu/~wkahan/MathH110/Cross.pdf).
	double max_len = -1;
	for (int i = 0; i < 3; i++)
	{
		MYON_3dVector V = Edge(i).Direction();
		double normV = V.MaximumCoordinate();
		if (normV > max_len)
		{
			max_len = normV;
			i0 = i;
		}
	}

	MYON_3dVector V = m_V[(i0 + 1)%3] - m_V[i0];
	MYON_3dVector W = m_V[(i0 + 2)%3] - m_V[i0];

	return MYON_CrossProduct(V, W);
}

double MYON_Triangle::Area() const
{
	return .5 * Normal().Length();
}


bool MYON_Triangle::IsDegenerate(double area_tol) const
{
	return (Area() < area_tol);
}

MYON_3dVector MYON_Triangle::UnitNormal() const
{
	auto V =  Normal();
	V.Unitize();
	return V;
}

MYON_PlaneEquation MYON_Triangle::PlaneEquation() const
{
	auto U = UnitNormal();
	double d = U*m_V[0];
	return MYON_PlaneEquation(U.x, U.y, U.z, d);
}

MYON_3dPoint MYON_Triangle::PointAt(double s1, double s2) const
{
	return (1 - s1 - s2)* m_V[0] + s1*m_V[1] + s2*m_V[2];
}

MYON_3dPoint MYON_Triangle::Centroid() const
{
	return PointAt(1.0/3.0, 1.0/3.0);
}

bool MYON_Triangle::GetBarycentricCoordinates(
  const MYON_3dPoint& P,
  bool constrainInside,
  double* s1, double* s2
) const
{
	bool rc = false;
	// Choose  base vertex v[i0] is closest to P
	int i0 = 0;
	double Min_norm = MYON_DBL_MAX;
	for (int i = 0; i < 3; i++)
	{
		MYON_3dVector V = P - m_V[i];
		double d = V.MaximumCoordinate();
		if (d < Min_norm)
		{
			i0 = i;
			Min_norm = d;
		}
	}

	MYON_3dVector VP = P - m_V[i0];
	MYON_3dVector V = m_V[(i0 + 1) % 3] - m_V[i0];
	MYON_3dVector W = m_V[(i0 + 2) % 3] - m_V[i0];
	MYON_3dPoint s(0, 0, 0);		// set to barycentric coordinates of solution

	if (MYON_DecomposeVector(VP, V, W, &s[(i0 + 1) % 3], &s[(i0 + 2) % 3]))
	{
		// use decomposition
		s[i0] = 1 - s[(i0 + 1) % 3] - s[(i0 + 2) % 3];

    if (constrainInside)
		  for (int i = 0; i < 3; i++)
			  if (s[i] < 0)
			  {
				  double t;
				  if (Edge(i).ClosestPointTo(P, &t))	
				  {
					  s = MYON_3dPoint( 0,0,0 );
					  if (t < 0)
					  {
						  s[(i + 1) % 3] = 1.0;
					  }

					  else if (t >= 1.0)
					  {
						  s[(i + 2) % 3] = 1.0;
					  }
					  else
					  {
						  s[(i + 1) % 3] = 1 - t;
						  s[(i + 2) % 3] = t;
					  }
				  }
				  break;
			  }
		rc = true;
	}
	else
	{
		// decomposition failed:
		// Find closest point to longest edge i0
		double max = Edge(0).Direction().LengthSquared();
    i0 = 0;
		for (int i = 1; i < 3; i++)
		{
			double lensq = Edge(i).Direction().LengthSquared();
			if (max < lensq)
			{
				i0 = i;
				max = lensq;
			}
		}
		double t;
		if (Edge(i0).ClosestPointTo(P, &t))
		{
			s[(i0 + 1) % 3] = (1 - t);
			s[(i0 + 2) % 3] = t;
			rc = true;
		}
	
	}
	if (s1)
		*s1 = s[1];
	if (s2)
		*s2 = s[2];
	return rc;
}

bool MYON_Triangle::ClosestPointTo(const MYON_3dPoint & P, double * s1, double * s2) const
{
  return GetBarycentricCoordinates(P, true, s1, s2);
}


MYON_3dPoint MYON_Triangle::ClosestPointTo(const MYON_3dPoint& P) const
{
	MYON_3dPoint s(0, 0, 0);
	ClosestPointTo(P, &s[1], &s[2]);
	return PointAt(s[1], s[2]);
}

double MYON_Triangle::DistanceTo(const MYON_3dPoint& P) const
{
	return P.DistanceTo(ClosestPointTo(P));
}

void MYON_Triangle::Reverse(int i)
{
	auto temp = m_V[(i + 1) % 3];
	m_V[(i + 1) % 3] = m_V[(i + 2) % 3];
	m_V[(i + 2) % 3] = temp;
}

bool MYON_Triangle::Transform(const MYON_Xform & xform)
{
	for (int i = 0; i < 3; i++) m_V[i] = xform * m_V[i];
	return true;
}

bool MYON_Triangle::Rotate(double sin_angle, double cos_angle, 
			const MYON_3dVector & axis_of_rotation, 
			const MYON_3dPoint & center_of_rotation)
{
	MYON_Xform R;
	R.Rotation(sin_angle, cos_angle, axis_of_rotation, center_of_rotation);
	return Transform(R);
}

bool MYON_Triangle::Rotate(double angle_in_radians, const MYON_3dVector & axis_of_rotation, const MYON_3dPoint & center_of_rotation)
{
	MYON_Xform R;
	R.Rotation(angle_in_radians, axis_of_rotation, center_of_rotation);
	return Transform(R);
}

bool MYON_Triangle::Translate(const MYON_3dVector & delta)
{
	const MYON_Xform T(MYON_Xform::TranslationTransformation(delta));
	return Transform(T);
}

void MYON_Triangle::Split(unsigned char edge, MYON_3dPoint pt, MYON_Triangle& out_a, MYON_Triangle& out_b) const
{
  switch (edge % 3)
  {
  case 0:
    out_a.m_V[0] = m_V[0];
    out_a.m_V[1] = m_V[1];
    out_a.m_V[2] = pt;
    out_b.m_V[0] = m_V[0];
    out_b.m_V[1] = pt;
    out_b.m_V[2] = m_V[2];
    break;
  case 1:
    out_a.m_V[0] = m_V[0];
    out_a.m_V[1] = m_V[1];
    out_a.m_V[2] = pt;
    out_b.m_V[0] = pt;
    out_b.m_V[1] = m_V[1];
    out_b.m_V[2] = m_V[2];
    break;
  default: //2
    out_a.m_V[0] = m_V[0];
    out_a.m_V[1] = pt;
    out_a.m_V[2] = m_V[2];
    out_b.m_V[0] = pt;
    out_b.m_V[1] = m_V[1];
    out_b.m_V[2] = m_V[2];
    break;
  }
}

void MYON_Triangle::Flip(unsigned char edge)
{
  switch (edge % 3)
  {
  case 0:
    std::swap(m_V[1], m_V[2]);
    break;
  case 1:
    std::swap(m_V[2], m_V[0]);
    break;
  default: //2
    std::swap(m_V[0], m_V[1]);
    break;
  }
}

void MYON_Triangle::Spin(unsigned char move)
{
  MYON_3dPoint tmp;
  switch (move % 3)
  {
  case 0:
    break;
  case 1:
    tmp = m_V[0];
    m_V[0] = m_V[2];
    m_V[2] = m_V[1];
    m_V[1] = tmp;
    break;
  default: //2
    tmp = m_V[0];
    m_V[0] = m_V[1];
    m_V[1] = m_V[2];
    m_V[2] = tmp;
    break;
  }
}


bool operator==(const MYON_Triangle & a, const MYON_Triangle & b)
{
	// Properly handles nans.
	return (a.m_V[0] == b.m_V[0] &&
					a.m_V[1] == b.m_V[1] &&
					a.m_V[2] == b.m_V[2]);
}

bool operator!=(const MYON_Triangle & a, const MYON_Triangle & b)
{
	// Properly handles nans.
	return (a.m_V[0] != b.m_V[0] ||
					a.m_V[1] != b.m_V[1] ||
					a.m_V[2] != b.m_V[2]);
}


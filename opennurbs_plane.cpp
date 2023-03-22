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

MYON_Plane::MYON_Plane() 
        : origin(0.0,0.0,0.0), 
          xaxis(1.0,0.0,0.0), yaxis(0.0,1.0,0.0), zaxis(0.0,0.0,1.0)
{
  plane_equation.x = plane_equation.y = plane_equation.d = 0.0;
  plane_equation.z = 1.0;
}

MYON_Plane::MYON_Plane(
    const MYON_3dPoint&  P, // point on the plane
    const MYON_3dVector& N  // non-zero normal to the plane
    )
{
  CreateFromNormal(P,N);
}

MYON_Plane::MYON_Plane(
    const MYON_3dPoint&  P, // point on the plane
    const MYON_3dVector& X, // non-zero vector in plane
    const MYON_3dVector& Y  // another vector in the plane not parallel to X
    )
{
  CreateFromFrame(P,X,Y);
}

MYON_Plane::MYON_Plane(
    const MYON_3dPoint& P,  // point on the plane
    const MYON_3dPoint& Q,  // point on the plane
    const MYON_3dPoint& R   // point on the plane
    )
{
  CreateFromPoints(P,Q,R);
}

MYON_Plane::MYON_Plane(
    const double e[4]    // equation of plane
    )
{
  CreateFromEquation(e);
}

MYON_Plane::MYON_Plane(
  const MYON_PlaneEquation& plane_equation
  )
{
  CreateFromEquation(plane_equation);
}

MYON_Plane::~MYON_Plane()
{}

MYON_3dPoint MYON_Plane::PointAt( double s, double t ) const
{
  return (origin + s*xaxis + t*yaxis);
}

MYON_3dPoint MYON_Plane::PointAt( double s, double t, double c ) const
{
  return (origin + s*xaxis + t*yaxis + c*zaxis);
}


MYON_Line MYON_Plane::IsoLine( // iso parametric line
       int dir,              // 0 first parameter varies and second parameter is constant
                         //   e.g., line(t) = plane(t,c)
                         // 1 first parameter is constant and second parameter varies
                         //   e.g., line(t) = plane(c,t)
       double c           // c = value of constant parameter 
       ) const
{
  MYON_Line line;
  if ( dir ) {
    line.from = PointAt( 0.0, c );
    line.to   = PointAt( 1.0, c );
  }
  else {
    line.from = PointAt( c, 0.0 );
    line.to   = PointAt( c, 1.0 );
  }
  return line;
}

double MYON_Plane::DistanceTo( const MYON_3dPoint& point ) const
{
  // signed distance
  // N.B. equation may not be normalized
  return ( point - origin)*zaxis;
}

bool MYON_Plane::GetDistanceToBoundingBox(const MYON_BoundingBox& Box,
				                                double* min, double* max) const
{
  //min and max signed distance.  Returns false if plane normal has zero length.

  MYON_3dVector UnitNormal = Normal();
  if (!UnitNormal.Unitize()) 
    return false;

  double mind, maxd;
  mind = maxd = (Box.Min() - Origin())*UnitNormal;
  int i0, i1, i2;
  for (i0=0;i0<2;i0++)
  {
    for(i1=0;i1<2;i1++) 
    {
      for (i2=0;i2<2;i2++) 
      {
        if (i0||i1||i2) 
        {
          MYON_3dPoint P;
          P[0]=(i0)?Box.Max()[0]:Box.Min()[0];
          P[1]=(i1)?Box.Max()[1]:Box.Min()[1];
          P[2]=(i2)?Box.Max()[2]:Box.Min()[2];
          double d = (P - Origin())*UnitNormal;
          //double dd = P.DistanceTo(ClosestPointTo(P));
          if (d < mind) 
            mind=d;
          else if (d > maxd) 
            maxd=d;
        }
      }
    }
  }
  *min = mind;
  *max = maxd;
  return true;
}


//double MYON_Plane::EquationAt( const MYON_3dPoint& p) const
//{
//  return plane_equation.ValueAt(p);
//}

//double MYON_Plane::EquationAt( const MYON_4dPoint& p) const
//{
//  return plane_equation.ValueAt(p);
//}

bool MYON_Plane::UpdateEquation()
{
  // computes equation[] from origin and zaxis.
  return plane_equation.Create(origin,zaxis);
}

const MYON_3dPoint& MYON_Plane::Origin() const
{
  return origin;
}

const MYON_3dVector& MYON_Plane::Xaxis() const
{
  return xaxis;
}

const MYON_3dVector& MYON_Plane::Yaxis() const
{
  return yaxis;
}

const MYON_3dVector& MYON_Plane::Normal() const
{
  return zaxis;
}

void MYON_Plane::SetOrigin( const MYON_3dPoint& origin_point)
{
  origin = origin_point;
  UpdateEquation();
}


bool MYON_Plane::ClosestPointTo( MYON_3dPoint p, double* s, double* t ) const
{
  const MYON_3dVector v = p - origin;
  if ( s )
    *s = v*xaxis;
  if ( t )
    *t = v*yaxis;
  return true;
}

MYON_3dPoint MYON_Plane::ClosestPointTo( MYON_3dPoint p ) const
{
  double s, t;
  ClosestPointTo( p, &s, &t );
  return PointAt( s, t );
}


bool MYON_Plane::operator==(const MYON_Plane& other) const
{
  return (origin==other.origin && xaxis==other.xaxis && yaxis==other.yaxis && zaxis==other.zaxis)
    ? true : false;
}

bool MYON_Plane::operator!=(const MYON_Plane& other) const
{
  return MYON_Plane::operator==(other)?false:true;
}


bool MYON_Plane::CreateFromNormal(
    const MYON_3dPoint&  P, // point on the plane
    const MYON_3dVector& N  // non-zero normal to the plane
    )
{
  origin = P;
  zaxis = N;
  bool b = zaxis.Unitize();
  xaxis.PerpendicularTo( zaxis );
  xaxis.Unitize();
  yaxis = MYON_CrossProduct( zaxis, xaxis );
  yaxis.Unitize();

  UpdateEquation();

  return b;
}

bool MYON_Plane::CreateFromNormalYup(
  const MYON_3dPoint& P,
  const MYON_3dVector& N,
  const MYON_3dVector& Y
)
{
  origin = P;
  zaxis = N;
  bool b = zaxis.Unitize();
  xaxis = MYON_CrossProduct(Y, zaxis);
  b = xaxis.Unitize() && b;
  if (!xaxis.Unitize())
  {
    xaxis.PerpendicularTo(zaxis);
    xaxis.Unitize();
  }

  yaxis = MYON_CrossProduct(zaxis, xaxis);
  yaxis.Unitize();
  UpdateEquation();

  return b;
}

bool MYON_Plane::CreateFromFrame(
    const MYON_3dPoint&  P, // point on the plane
    const MYON_3dVector& X, // non-zero vector in plane
    const MYON_3dVector& Y  // another non-zero vector in the plane
    )
{
  const MYON_3dPoint localP(P);
  const MYON_3dVector localX(X);
  const MYON_3dVector localY(Y);


  origin = localP;

  xaxis = localX;
  xaxis.Unitize();
  yaxis = localY - MYON_DotProduct( localY, xaxis)*xaxis;
  yaxis.Unitize();
  zaxis = MYON_CrossProduct( xaxis, yaxis );
  bool b = zaxis.Unitize();
  UpdateEquation();
  if ( b )
  {
    // 11 February 2004 Dale Lear
    //     Add more validation checks.
    b = IsValid();
    if ( b )
    {
      // make sure zaxis is perp to localY
      if ( fabs(localY*zaxis) > MYON_SQRT_EPSILON*localY.Length() )
        b = false;
    }
  }
  return b;
}

bool MYON_Plane::CreateFromEquation(
  const class MYON_PlaneEquation& eqn
  )
{
  bool b = false;
  
  plane_equation = eqn;

  zaxis.x = plane_equation.x;
  zaxis.y = plane_equation.y;
  zaxis.z = plane_equation.z;

  double d = zaxis.Length();
  if (d > 0.0) {
    d = 1.0 / d;
		zaxis *= d;		
    origin = (-d*plane_equation.d)*zaxis;
    b = true;
  }
  xaxis.PerpendicularTo(zaxis);
  xaxis.Unitize();
  yaxis = MYON_CrossProduct(zaxis, xaxis);
  yaxis.Unitize();
  return b;
}

bool MYON_Plane::CreateFromEquation(
    const double e[4]    // equation of plane
    )
{
  MYON_PlaneEquation eqn(e[0], e[1], e[2], e[3]);
  return CreateFromEquation(eqn);
}

bool MYON_Plane::CreateFromPoints(
          const MYON_3dPoint& P,  // point on the plane
          const MYON_3dPoint& Q,  // point on the plane
          const MYON_3dPoint& R   // point on the plane
          )
{
  origin = P;
  bool rc = zaxis.PerpendicularTo(P,Q,R);
  xaxis = Q - P;
  xaxis.Unitize();
  yaxis = MYON_CrossProduct( zaxis, xaxis );
  yaxis.Unitize();

  if ( !plane_equation.Create(origin,zaxis) )
    rc = false;

  return rc;
}

bool MYON_Plane::IsValid() const
{
  if ( !plane_equation.IsValid() )
    return false;


  double x = plane_equation.ValueAt(origin);
  if ( fabs(x) >  MYON_ZERO_TOLERANCE )
  {
    double tol = fabs(origin.MaximumCoordinate()) + fabs(plane_equation.d);
    if ( tol > 1000.0 && origin.IsValid() )
    {
      // 8 September 2003 Chuck and Dale:
      //   Fixing discrepancy between optimized and debug behavior.
      //   In this case, the MYON_ZERO_TOLERANCE test worked in release
      //   and failed in debug. The principal behind this fix is valid
      //   for release builds too.
      //   For large point coordinates or planes far from the origin,
      //   the best we can hope for is to kill the first 15 or so decimal
      //   places.
      tol *= (MYON_EPSILON*10.0);
      if ( fabs(x) > tol )
        return false;
    }
    else
      return false;
  }

  if ( !MYON_IsRightHandFrame( xaxis, yaxis, zaxis ) )
    return false;

  const MYON_3dVector N = plane_equation.UnitNormal();
  x = MYON_DotProduct( N, zaxis );
  if ( fabs(x-1.0) >  MYON_SQRT_EPSILON )
    return false;

  return true;
}


bool MYON_Plane::Transform( const MYON_Xform& xform )
{
  if ( xform.IsIdentity() )
  {
    // 27 October 2011 Dale Lear
    //    If the plane is valid and the transformation
    //    is the identity, then NO changes should be
    //    made to the plane's values.  In particular
    //    calling CreateFromFrame() can introduce
    //    slight fuzz.
    //
    //    Please discuss any changes with me.
    return IsValid();
  }

  MYON_3dPoint origin_pt = xform*origin;

  // use care tranforming vectors to get
  // maximum precision and the right answer
  bool bUseVectorXform = (    0.0 == xform.m_xform[3][0] 
                           && 0.0 == xform.m_xform[3][1]
                           && 0.0 == xform.m_xform[3][2] 
                           && 1.0 == xform.m_xform[3][3]
                         );

  MYON_3dVector xaxis_vec = bUseVectorXform ? (xform*xaxis) : ((xform*(origin+xaxis)) - origin_pt);
  MYON_3dVector yaxis_vec = bUseVectorXform ? (xform*yaxis) : ((xform*(origin+yaxis)) - origin_pt);

  return CreateFromFrame( origin_pt, xaxis_vec, yaxis_vec );
}



bool MYON_Plane::SwapCoordinates( int i, int j )
{
  bool rc = false;
  if ( 0 <= i && i < 3 && 0 <= j && j < 3 ) {
    MYON_Xform xform(MYON_Xform::IdentityTransformation);
    xform[i][i] = 0.0;
    xform[j][j] = 0.0;
    xform[i][j] = 1.0;
    xform[j][i] = 1.0;
    rc = Transform(xform);
  }
  return rc;
}


// rotate plane about its origin_pt
bool MYON_Plane::Rotate(
      double s,                 // sin(angle)
      double c,                 // cos(angle)
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate( s, c, axis, origin );
}

bool MYON_Plane::Rotate(
      double angle,           // angle in radians
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate( sin(angle), cos(angle), axis );
}

// rotate plane about a point and axis
bool MYON_Plane::Rotate(
      double sin_angle,          // sin(angle)
      double cos_angle,          // cos(angle)
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint& center // center of rotation
      )
{
  bool rc = false;
  MYON_Xform rot;
  if ( center == origin ) {
    rot.Rotation( sin_angle, cos_angle, axis, MYON_3dPoint::Origin );
    xaxis = rot*xaxis;
    yaxis = rot*yaxis;
    if ( !(axis == zaxis) )
      zaxis = rot*zaxis;
    rc = UpdateEquation();
  }
  else {
    rot.Rotation( sin_angle, cos_angle, axis, center );
    rc = Transform( rot );
  }
  return rc;
}

bool MYON_Plane::Rotate(
      double a,                   // angle in radians
      const MYON_3dVector& axis,  // axis of rotation
      const MYON_3dPoint& origin_pt  // center of rotation
      )
{
  return Rotate( sin(a), cos(a), axis, origin_pt );
}

bool MYON_Plane::Translate(
      const MYON_3dVector& delta
      )
{
  const MYON_Xform tr(MYON_Xform::TranslationTransformation(delta));
  return Transform( tr );
}

bool MYON_Plane::Flip()
{
  MYON_3dVector v = xaxis;
  xaxis = yaxis;
  yaxis = v;
  zaxis = -zaxis;
  UpdateEquation();
  return true;
}


int MYON_ArePointsOnPlane( // returns 0=no, 1 = yes, 2 = pointset is (to tolerance) a single point on the line
        int dim,     // 2 or 3
        bool is_rat,
        int count, 
        int stride, const double* point,
        const MYON_BoundingBox& bbox, // if needed, use MYON_GetBoundingBox(dim,is_rat,count,stride,point)
        const MYON_Plane& plane,  // line to test
        double tolerance
        )
{
  double w;
  int i, j, k;

  if ( count < 1 )
    return 0;
  if ( !plane.IsValid() )
  {
    MYON_ERROR("plane parameter is not valid");
    return 0;
  }
  if ( !bbox.IsValid() )
  {
    MYON_ERROR("bbox parameter is not valid");
    return 0;
  }
  if ( !MYON_IsValid(tolerance) || tolerance < 0.0 )
  {
    MYON_ERROR("tolerance must be >= 0.0");
    return 0;
  }
  if ( dim < 2 || dim > 3 )
  {
    MYON_ERROR("dim must be 2 or 3");
    return 0;
  }
  if ( stride < (is_rat?(dim+1):dim) )
  {
    MYON_ERROR("stride parameter is too small");
    return 0;
  }
  if ( 0 == point )
  {
    MYON_ERROR("point parameter is null");
    return 0;
  }

  int rc = 0;

  if ( tolerance == 0.0 ) {
    tolerance = bbox.Tolerance();
  }

  MYON_3dPoint Q;

  // test bounding box to quickly detect the common coordinate axis cases
  rc = (count == 1 || bbox.Diagonal().Length() <= tolerance) ? 2 : 1;
  for ( i = 0; rc && i < 2; i++ ) {
    Q.x = bbox[i].x;
    for ( j = 0; rc && j < 2; j++) {
      Q.y = bbox[j].y;
      for ( k = 0; rc && k < 2; k++) {
        Q.z = bbox[k].z;
        if ( Q.DistanceTo( plane.ClosestPointTo( Q ) ) > tolerance )
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
        if ( Q.DistanceTo( plane.ClosestPointTo( Q ) ) > tolerance ) {
          rc = 0;
          break;
        }
        point += stride;
      }
    }
    else {
      for ( i = 0; i < count; i++ ) {
        memcpy( &Q.x, point, dim*sizeof(Q.x) );
        if ( Q.DistanceTo( plane.ClosestPointTo( Q ) ) > tolerance ) {
          rc = 0;
          break;
        }
        point += stride;
      }
    }
  }

  return rc;
}


#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

MYON_Box::MYON_Box()
{}

MYON_Box::MYON_Box( const MYON_BoundingBox& bbox )
{
  Create(bbox);
}

MYON_Box::~MYON_Box()
{}

void MYON_Box::Destroy()
{
  plane = MYON_xy_plane;
  dx = MYON_Interval::EmptyInterval;
  dy = MYON_Interval::EmptyInterval;
  dz = MYON_Interval::EmptyInterval;
}

bool MYON_Box::IsValid() const
{
  return (    dx.IsIncreasing() 
           && dy.IsIncreasing() 
           && dz.IsIncreasing() 
           && plane.IsValid() 
         );
}

bool MYON_Box::Create( const MYON_BoundingBox& bbox )
{
  plane = MYON_xy_plane;
  dx.Set(bbox.m_min.x,bbox.m_max.x);
  dy.Set(bbox.m_min.y,bbox.m_max.y);
  dz.Set(bbox.m_min.z,bbox.m_max.z);
  return (dx.IsValid() && dy.IsValid() && dz.IsValid());
}

int MYON_Box::IsDegenerate( double tolerance ) const
{
  int rc = 0;
  // 0     box is not degenerate
  // 1     box is a rectangle (degenerate in one direction)
  // 2     box is a line (degenerate in two directions)
  // 3     box is a point (degenerate in three directions)
  // 4     box is not valid
  if ( !dx.IsIncreasing() || !dy.IsIncreasing() || !dz.IsIncreasing() )
  {
    rc = 4;
  }
  else
  {
    const MYON_3dVector diag(dx.Length(),dy.Length(),dz.Length());
    if ( !MYON_IsValid(tolerance) || tolerance < 0.0 )
    {
      // compute scale invarient tolerance
      tolerance = diag.MaximumCoordinate()*MYON_SQRT_EPSILON;
    }
    if ( diag.x <= tolerance )
      rc++;
    if ( diag.y <= tolerance )
      rc++;
    if ( diag.z <= tolerance )
      rc++;
  }
  return rc;
}

MYON_3dPoint MYON_Box::Center() const
{
  return plane.PointAt(dx.Mid(),dy.Mid(),dz.Mid());
}

bool MYON_Box::GetCorners( MYON_3dPoint* corners ) const
{
  int i,j,k,n=0;
  double r,s,t;
  for( i= 0; i < 2; i++ )
  {
    r = dx.m_t[i];
    for ( j = 0; j < 2; j++ )
    {
      s = dy.m_t[j];
      for ( k = 0; k < 2; k++ )
      {
        t = dz.m_t[k];
        corners[n++] = plane.PointAt(r,s,t);
      }
    }
  }
  return true;
}

bool MYON_Box::GetCorners( MYON_SimpleArray<MYON_3dPoint>& corners ) const
{
  corners.Empty();
  corners.Reserve(8);
  bool rc = GetCorners(corners.Array());
  if (rc)
    corners.SetCount(8);
  return rc;
}

MYON_BoundingBox MYON_Box::BoundingBox() const
{
  MYON_BoundingBox bbox;
  MYON_3dPoint corners[8];
  if ( GetCorners(corners) )
    bbox.Set(3,0,8,3,&corners[0].x,false);
  return bbox;
}

MYON_3dPoint MYON_Box::PointAt( 
        double r, 
        double s, 
        double t 
        ) const
{
  // Do not validate - it is too slow.
  return plane.PointAt(r,s,t);
}

// returns point on cylinder that is closest to given point
bool MYON_Box::ClosestPointTo( MYON_3dPoint point, double* r, double* s, double* t ) const
{
  // Do not validate box - it is too slow.
  const MYON_3dVector v = point - plane.origin;

  *r = v*plane.xaxis;
  if ( *r < dx.m_t[0] )
    *r = dx.m_t[0];
  else if ( *r > dx.m_t[1] )
    *r = dx.m_t[1];

  *s = v*plane.yaxis;
  if ( *s < dy.m_t[0] )
    *s = dy.m_t[0];
  else if ( *s > dy.m_t[1] )
    *s = dy.m_t[1];

  *t = v*plane.zaxis;
  if ( *t < dz.m_t[0] )
    *t = dz.m_t[0];
  else if ( *t > dz.m_t[1] )
    *t = dz.m_t[1];

  return true;
}

MYON_3dPoint MYON_Box::ClosestPointTo( MYON_3dPoint point ) const
{
  // Do not validate - it is too slow.
  double r,s,t;
  ClosestPointTo(point,&r,&s,&t);
  return PointAt(r,s,t);
}

// rotate box about its center
bool MYON_Box::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate(sin_angle, cos_angle, axis, Center() );
}

bool MYON_Box::Rotate(
      double angle,           // angle in radians
      const MYON_3dVector& axis // axis of rotation
      )
{
  return Rotate(sin(angle), cos(angle), axis, plane.origin );
}

// rotate box about a point and axis
bool MYON_Box::Rotate(
      double sin_angle,
      double cos_angle,
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint&  point // center of rotation
      )
{
  return plane.Rotate( sin_angle, cos_angle, axis, point );
}

bool MYON_Box::Rotate(
      double angle,             // angle in radians
      const MYON_3dVector& axis,  // axis of rotation
      const MYON_3dPoint&  point  // center of rotation
      )
{
  return Rotate(sin(angle),cos(angle),axis,point);
}

bool MYON_Box::Translate(
      const MYON_3dVector& delta
      )
{
  return plane.Translate(delta);
}


bool MYON_Box::Transform( const MYON_Xform& xform )
{
  MYON_3dPoint corners[8];
  bool rc = GetCorners(corners);
  if ( rc )
  {
    MYON_Plane xplane(plane);
    rc = xplane.Transform(xform);
    if ( rc )
    {
      int i;
      for ( i = 0; i < 8; i++ )
      {
        corners[i] = xform*corners[i];
      }
      double x0,x1,x,y0,y1,y,z0,z1,z;
      MYON_3dVector v = corners[7] - plane.origin;
      x0 = x1 = v*plane.xaxis;
      y0 = y1 = v*plane.yaxis;
      z0 = z1 = v*plane.zaxis;
      for ( i = 0; i < 7; i++ )
      {
        v = corners[i] - plane.origin;
        x = v*plane.xaxis;
        if ( x < x0 ) x0 = x; else if (x > x1 ) x1 = x;
        y = v*plane.yaxis;
        if ( y < y0 ) y0 = y; else if (y > y1 ) y1 = y;
        z = v*plane.zaxis;
        if ( z < z0 ) z0 = z; else if (z > z1 ) z1 = z;
      }
      double tol = MYON_SQRT_EPSILON;
      if ( fabs(dx.ParameterAt(x0)) > tol || fabs(dx.ParameterAt(x1)-1.0) > tol )
        dx.Set(x0,x1);
      if ( fabs(dy.ParameterAt(y0)) > tol || fabs(dy.ParameterAt(y1)-1.0) > tol )
        dy.Set(y0,y1);
      if ( fabs(dz.ParameterAt(z0)) > tol || fabs(dz.ParameterAt(z1)-1.0) > tol )
        dz.Set(z0,z1);
    }
  }
  return rc;
}

double MYON_Box::Volume() const
{
  return dx.Length()*dy.Length()*dz.Length();
}

double MYON_Box::Area() const
{
  double a = dx.Length();
  double b = dy.Length();
  double c = dz.Length();
  return 2.0*(a*b + b*c + c*a);
}

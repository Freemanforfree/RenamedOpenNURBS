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

MYON_Quaternion MYON_CrossProduct( const MYON_Quaternion& p, const MYON_Quaternion& q)
{
  return MYON_Quaternion(0.0, p.c*q.d - p.d*q.c, p.d*q.b - p.b*q.d, p.b*q.c - p.c*q.d);
}

MYON_Quaternion MYON_QuaternionProduct( const MYON_Quaternion& p, const MYON_Quaternion& q)
{
  return MYON_Quaternion( p.a*q.a - p.b*q.b - p.c*q.c - p.d*q.d,
                        p.a*q.b + p.b*q.a + p.c*q.d - p.d*q.c,
                        p.a*q.c - p.b*q.d + p.c*q.a + p.d*q.b,
                        p.a*q.d + p.b*q.c - p.c*q.b + p.d*q.a);
}


/////////////////////////////////////

const MYON_Quaternion MYON_Quaternion::Zero(0.0,0.0,0.0,0.0);
const MYON_Quaternion MYON_Quaternion::Identity(1.0,0.0,0.0,0.0);
const MYON_Quaternion MYON_Quaternion::I(0.0,1.0,0.0,0.0);
const MYON_Quaternion MYON_Quaternion::J(0.0,0.0,1.0,0.0);
const MYON_Quaternion MYON_Quaternion::K(0.0,0.0,0.0,1.0);

void MYON_Quaternion::SetRotation(double angle, const MYON_3dVector& axis)
{
  double s = axis.Length();
  s = (s > 0.0) ? sin(0.5*angle)/s : 0.0;
  a = cos(0.5*angle);
  b = s*axis.x;
  c = s*axis.y;
  d = s*axis.z;
}

MYON_Quaternion MYON_Quaternion::Rotation(double angle, const MYON_3dVector& axis)
{
  double s = axis.Length();
  s = (s > 0.0) ? sin(0.5*angle)/s : 0.0;
  return MYON_Quaternion(cos(0.5*angle),s*axis.x,s*axis.y,s*axis.z);
}

MYON_Quaternion MYON_Quaternion::Exp(MYON_Quaternion q)
{
  // Added 8 Jan 2010 - not tested yet
  double v = ((const MYON_3dVector*)&q.b)->Length();
  if ( !(v > MYON_DBL_MIN) )
    v = 0.0;
  double ea = exp(q.a);
  double z = (v > 0.0) ? ea*sin(v)/v : 0.0;
  return MYON_Quaternion( ea*cos(v), z*q.b, z*q.c, z*q.d );
}

MYON_Quaternion MYON_Quaternion::Log(MYON_Quaternion q)
{
  // Added 8 Jan 2010 - not tested yet
  double lenq = q.Length();
  double v = ((const MYON_3dVector*)&q.b)->Length();
  if ( !(v > MYON_DBL_MIN) )
    v = 0.0;
  double z = (v > 0.0) ? acos(q.a/lenq)/v : 0.0;
  return MYON_Quaternion(log(lenq), z*q.b, z*q.c, z*q.d );
}

MYON_Quaternion MYON_Quaternion::Pow(MYON_Quaternion q,double t)
{
  // Added 8 Jan 2010 - not tested yet
  return MYON_Quaternion::Exp( t * MYON_Quaternion::Log(q) );
}

MYON_Quaternion MYON_Quaternion::Slerp(MYON_Quaternion q0, MYON_Quaternion q1, double t)
{
  MYON_Quaternion q;
  if ( t <= 0.5 )
  {
    q = q0.Inverse()*q1;
    q = q0*MYON_Quaternion::Pow(q,t);
  }
  else
  {
    q = q1.Inverse()*q0;
    q = q1*MYON_Quaternion::Pow(q,1.0-t);
  }
  return q;
}

MYON_Quaternion MYON_Quaternion::RotateTowards(MYON_Quaternion q0, MYON_Quaternion q1, double MaxRadians)
{
  MYON_Quaternion Q = q0.Inverse() * q1;
  MYON_Quaternion logQ = Log(Q);

  double t = MaxRadians / (2.0*logQ.Length());

  return (t < 1.0) ? Slerp(q0, q1, t) : q1;
}


void MYON_Quaternion::Set(double qa, double qb, double qc, double qd)
{
  a = qa;
  b = qb;
  c = qc;
  d = qd;
}

void MYON_Quaternion::SetRotation(const MYON_Plane& plane0, const MYON_Plane& plane1 )
{
  double m[3][3], r, s;
  double* q;
  int i,j,k;

  // set m[][] = rotation matrix (acting on the left)
  m[0][0] = plane1.xaxis.x*plane0.xaxis.x
          + plane1.yaxis.x*plane0.yaxis.x
          + plane1.zaxis.x*plane0.zaxis.x;
  m[0][1] = plane1.xaxis.x*plane0.xaxis.y
          + plane1.yaxis.x*plane0.yaxis.y
          + plane1.zaxis.x*plane0.zaxis.y;
  m[0][2] = plane1.xaxis.x*plane0.xaxis.z
          + plane1.yaxis.x*plane0.yaxis.z
          + plane1.zaxis.x*plane0.zaxis.z;
  m[1][0] = plane1.xaxis.y*plane0.xaxis.x
          + plane1.yaxis.y*plane0.yaxis.x
          + plane1.zaxis.y*plane0.zaxis.x;
  m[1][1] = plane1.xaxis.y*plane0.xaxis.y
          + plane1.yaxis.y*plane0.yaxis.y
          + plane1.zaxis.y*plane0.zaxis.y;
  m[1][2] = plane1.xaxis.y*plane0.xaxis.z
          + plane1.yaxis.y*plane0.yaxis.z
          + plane1.zaxis.y*plane0.zaxis.z;
  m[2][0] = plane1.xaxis.z*plane0.xaxis.x
          + plane1.yaxis.z*plane0.yaxis.x
          + plane1.zaxis.z*plane0.zaxis.x;
  m[2][1] = plane1.xaxis.z*plane0.xaxis.y
          + plane1.yaxis.z*plane0.yaxis.y
          + plane1.zaxis.z*plane0.zaxis.y;
  m[2][2] = plane1.xaxis.z*plane0.xaxis.z
          + plane1.yaxis.z*plane0.yaxis.z
          + plane1.zaxis.z*plane0.zaxis.z;

  k = 1;
  s = MYON_SQRT_EPSILON;
  for ( i = 0; i < 3 && k; i++ ) for ( j = 0; j < 3; j++ )
  {
    if ( i == j )
    {
      if (fabs(m[i][i]-1.0) > s )
      {
        k = 0;
        break;
      }
    }
    else
    {
      if (fabs(m[i][j]) > s )
      {
        k = 0;
        break;
      }
    }
  }

  if ( k )
  {
    // m[][] is the identity matrix
    a = 1.0;
    b = c = d = 0.0;
    return;
  }

  i = (m[0][0] >= m[1][1]) 
    ? ((m[0][0] >= m[2][2])?0:2) 
    : ((m[1][1] >= m[2][2])?1:2);
  j = (i+1)%3;
  k = (i+2)%3;

  // Note: 
  //   For any rotation matrix, the diagonal is
  //     x^2(1-cos(t)) + cos(t), y^2(1-cos(t)) + cos(t), z^2(1-cos(t)) + cos(t),
  //   where (x,y,z) is the unit vector axis of rotation and "t" is the angle.
  //   So the trace = 1 + 2cos(t).
  //
  //   When cos(t) >= 0, m[i][i] corresponds to the axis component that has
  //   the largest absolute value.
  //
  //   
  //
  //   Observe that 
  //     s = 1 + m[i][i] - m[j][j] - m[k][k]
  //       = 1 + 2*m[i][i] - m[i][i] - m[j][j] - m[k][k]
  //       = 1 + 2*m[i][i] - trace
  //       = 2*(m[i][i] - cos(t))
  //       = 2*(w^2(1-cos(t)^2) + cos(t) - cos(t))
  //       = 2*w*w*(sin(t)^2)
  //
  //    When cos(t) >= 0, m[i][i] corresponds to the coordinate of
  //    the rotation axis with largest absolute value.


  s = 1.0 + m[i][i] - m[j][j] - m[k][k];
  if ( s > MYON_DBL_MIN )
  {
    r = sqrt(s);
    s = 0.5/r;
    a = s*(m[k][j] - m[j][k]);
    q = &b;
    q[i] = 0.5*r;
    q[j] = s*(m[i][j] + m[j][i]);
    q[k] = s*(m[k][i] + m[i][k]);
  }
  else
  {
    if ( s < -1.0e-14 )
      MYON_ERROR("noisy rotation matrix");
    a = 1.0;
    b = c = d = 0.0;
  }
}

MYON_Quaternion MYON_Quaternion::Rotation(const MYON_Plane& plane0, const MYON_Plane& plane1)
{
  MYON_Quaternion q;
  q.SetRotation(plane0,plane1);
  return q;
}

bool MYON_Quaternion::GetRotation(MYON_Xform& xform) const
{
  bool rc;
  MYON_Quaternion q(*this);
  if ( q.Unitize() )
  {
    if (    fabs(q.a-a) <= MYON_ZERO_TOLERANCE
         && fabs(q.b-b) <= MYON_ZERO_TOLERANCE
         && fabs(q.c-c) <= MYON_ZERO_TOLERANCE
         && fabs(q.d-d) <= MYON_ZERO_TOLERANCE
         )
    {
      // "this" was already unitized - don't tweak bits
      q = *this;
    }
    xform[1][0] = 2.0*(q.b*q.c + q.a*q.d);
    xform[2][0] = 2.0*(q.b*q.d - q.a*q.c);
    xform[3][0] = 0.0;

    xform[0][1] = 2.0*(q.b*q.c - q.a*q.d);
    xform[2][1] = 2.0*(q.c*q.d + q.a*q.b);
    xform[3][1] = 0.0;

    xform[0][2] = 2.0*(q.b*q.d + q.a*q.c);
    xform[1][2] = 2.0*(q.c*q.d - q.a*q.b);
    xform[3][2] = 0.0;

    q.b = q.b*q.b;
    q.c = q.c*q.c;
    q.d = q.d*q.d;
    xform[0][0] = 1.0 - 2.0*(q.c + q.d);
    xform[1][1] = 1.0 - 2.0*(q.b + q.d);
    xform[2][2] = 1.0 - 2.0*(q.b + q.c);

    xform[0][3] = xform[1][3] = xform[2][3] = 0.0;
    xform[3][3] = 1.0;
    rc = true;
  }
  else if ( IsZero() )
  {
    xform = MYON_Xform::Zero4x4;
    rc = false;
  }
  else
  {
    // something is seriously wrong
    MYON_ERROR("MYON_Quaternion::GetRotation(MYON_Xform) quaternion is invalid");
    xform = MYON_Xform::IdentityTransformation;
    rc = false;
  }

  return rc;
}

bool MYON_Quaternion::GetRotation(MYON_Plane& plane) const
{
  plane.xaxis.x = a*a + b*b - c*c - d*d;
  plane.xaxis.y = 2.0*(a*d + b*c);
  plane.xaxis.z = 2.0*(b*d - a*c);

  plane.yaxis.x = 2.0*(b*c - a*d);
  plane.yaxis.y = a*a - b*b + c*c - d*d;
  plane.yaxis.z = 2.0*(a*b + c*d);

  plane.zaxis.x = 2.0*(a*c + b*d);
  plane.zaxis.y = 2.0*(c*d - a*b);
  plane.zaxis.z = a*a - b*b - c*c + d*d;

  plane.xaxis.Unitize();
  plane.yaxis.Unitize();
  plane.zaxis.Unitize();
  plane.origin.Set(0.0,0.0,0.0);
  plane.UpdateEquation();

  return plane.IsValid();
}

bool MYON_Quaternion::GetRotation(double& angle, MYON_3dVector& axis) const
{
  const double s = Length();
  angle = (s > MYON_DBL_MIN) ? 2.0*acos(a/s) : 0.0;
  axis.x = b;
  axis.y = c;
  axis.z = d;
  return (axis.Unitize() && s > MYON_DBL_MIN);
}

MYON_3dVector MYON_Quaternion::Vector() const
{
  return MYON_3dVector(b,c,d);
}

double MYON_Quaternion::Scalar() const
{
  return a;
}

bool MYON_Quaternion::IsZero() const
{
  return (0.0 == a && 0.0 == b && 0.0 == c && 0.0 == d);
}

bool MYON_Quaternion::IsNotZero() const
{
  return ( (0.0 != a || 0.0 != b || 0.0 != c || 0.0 != d) 
           && MYON_IsValid(a)
           && MYON_IsValid(b)
           && MYON_IsValid(c) 
           && MYON_IsValid(d)
         );
}

bool MYON_Quaternion::IsScalar() const
{
  return (0.0 == b && 0.0 == c && 0.0 == d);
}

bool MYON_Quaternion::IsVector() const
{
  return (0.0 == a && (0.0 != b || 0.0 != c || 0.0 != d));
}

bool MYON_Quaternion::IsValid() const
{
  return ((MYON_IS_VALID(a) && MYON_IS_VALID(b) && MYON_IS_VALID(c) && MYON_IS_VALID(d)) ? true : false);
}

MYON_Quaternion MYON_Quaternion::Conjugate() const
{
  return MYON_Quaternion(a,-b,-c,-d);
}

MYON_Quaternion MYON_Quaternion::Inverse() const
{
  double x = a*a+b*b+c*c+d*d;
  x = ( x > MYON_DBL_MIN ) ? 1.0/x : 0.0;
  return MYON_Quaternion(a*x,-b*x,-c*x,-d*x);
}

bool MYON_Quaternion::Invert()
{
  double x = a*a+b*b+c*c+d*d;
  if ( x <= MYON_DBL_MIN )
    return false;
  x = 1.0/x;
  a *= x;
  x = -x;
  b *= x;
  c *= x;
  d *= x;
  return true;
}

MYON_3dVector MYON_Quaternion::Rotate(MYON_3dVector v) const
{
  // returns q*(0,v.x,v.y,v.z)*Inverse(q)
  double x = a*a + b*b + c*c + d*d;
  x = ( x > MYON_DBL_MIN ) ? 1.0/x : 0.0;
  const MYON_Quaternion qinv(a*x,-b*x,-c*x,-d*x);

  const MYON_Quaternion qv( -b*v.x - c*v.y - d*v.z,
                           a*v.x + c*v.z - d*v.y,
                           a*v.y - b*v.z + d*v.x,
                           a*v.z + b*v.y - c*v.x);

  v.x = qv.a*qinv.b + qv.b*qinv.a + qv.c*qinv.d - qv.d*qinv.c;
  v.y = qv.a*qinv.c - qv.b*qinv.d + qv.c*qinv.a + qv.d*qinv.b;
  v.z = qv.a*qinv.d + qv.b*qinv.c - qv.c*qinv.b + qv.d*qinv.a;
  return v;
}

double MYON_Quaternion::DistanceTo(const MYON_Quaternion& q) const
{
  const MYON_Quaternion pq(q.a-a,q.b-b,q.c-c,q.d-d);
  return pq.Length();
}


double MYON_Quaternion::Distance(const MYON_Quaternion& p, const MYON_Quaternion& q)
{
  const MYON_Quaternion pq(q.a-p.a,q.b-p.b,q.c-p.c,q.d-p.d);
  return pq.Length();
}


double MYON_Quaternion::Length() const
{
  double len;
  double fa = fabs(a);
  double fb = fabs(b);
  double fc = fabs(c);
  double fd = fabs(d);
  if ( fb >= fa && fb >= fc && fb >= fd) 
  {
    len = fa; fa = fb; fb = len;
  }
  else if ( fc >= fa && fc >= fb && fc >= fd) 
  {
    len = fa; fa = fc; fc = len;
  }
  else if ( fd >= fa && fd >= fb && fd >= fc) 
  {
    len = fa; fa = fd; fd = len;
  }

  // 15 September 2003 Dale Lear
  //     For small denormalized doubles (positive but smaller
  //     than DBL_MIN), some compilers/FPUs set 1.0/fa to +INF.
  //     Without the MYON_DBL_MIN test we end up with
  //     microscopic quaternions that have infinte norm!
  //
  //     This code is absolutely necessary.  It is a critical
  //     part of the bug fix for RR 11217.
  if ( fa > MYON_DBL_MIN ) 
  {
    len = 1.0/fa;
    fb *= len;
    fc *= len;
    fd *= len;
    len = fa*sqrt(1.0 + fb*fb + fc*fc + fd*fd);
  }
  else if ( fa > 0.0 && MYON_IS_FINITE(fa) )
    len = fa;
  else
    len = 0.0;

  return len;
}


double MYON_Quaternion::LengthSquared() const
{
  return (a*a + b*b + c*c + d*d);
}


MYON_Xform MYON_Quaternion::MatrixForm() const
{
  double m[4][4];
  m[0][0] =  a; m[0][1] =  b; m[0][2] =  c; m[0][3] =  d; 
  m[1][0] = -b; m[1][1] =  a; m[1][2] = -d; m[1][3] =  c; 
  m[2][0] = -c; m[2][1] =  d; m[2][2] =  a; m[2][3] = -b; 
  m[3][0] = -d; m[3][1] = -c; m[3][2] =  b; m[3][3] =  a; 
  return MYON_Xform(&m[0][0]);
}

bool MYON_Quaternion::Unitize()
{
  double x = Length();

  if (x > MYON_DBL_MIN)
  {
    x = 1.0/x;
    a *= x; b *= x; c *= x; d *= x;
  }
  else if ( x > 0.0 )
  {
    MYON_Quaternion q(a*1.0e300,b*1.0e300,c*1.0e300,c*1.0e300);
    if ( !q.Unitize() )
      return false;
    a = q.a; 
    b = q.b;
    c = q.c;
    d = q.d;
  }
  else
  {
    return false;
  }

  return true;
}


MYON_Quaternion::MYON_Quaternion(double qa, double qb, double qc, double qd) : a(qa),b(qb),c(qc),d(qd) {}

MYON_Quaternion::MYON_Quaternion(const MYON_3dVector& v) : a(0.0),b(v.x),c(v.y),d(v.z) {}

MYON_Quaternion& MYON_Quaternion::operator=(const MYON_3dVector& v)
{
  a = 0.0;
  b = v.x;
  c = v.y;
  d = v.z;
  return *this;
}

MYON_Quaternion MYON_Quaternion::operator*(int x) const
{
  return MYON_Quaternion(a*x,b*x,c*x,d*x);
}

MYON_Quaternion MYON_Quaternion::operator*(float x) const
{
  return MYON_Quaternion(a*x,b*x,c*x,d*x);
}

MYON_Quaternion MYON_Quaternion::operator*(double x) const
{
  return MYON_Quaternion(a*x,b*x,c*x,d*x);
}

MYON_Quaternion MYON_Quaternion::operator/(int y) const
{
  double x = (0!=y) ? 1.0/((double)y) : 0.0;
  return MYON_Quaternion(a*x,b*x,c*x,d*x);
}

MYON_Quaternion MYON_Quaternion::operator/(float y) const
{
  double x = (0.0f!=y) ? 1.0/((double)y) : 0.0;
  return MYON_Quaternion(a*x,b*x,c*x,d*x);
}

MYON_Quaternion MYON_Quaternion::operator/(double y) const
{
  double x = (0.0!=y) ? 1.0/((double)y) : 0.0;
  return MYON_Quaternion(a*x,b*x,c*x,d*x);
}

MYON_Quaternion  MYON_Quaternion::operator+(const MYON_Quaternion& q) const
{
  return MYON_Quaternion(a+q.a,b+q.b,c+q.c,d+q.d);
}

MYON_Quaternion  MYON_Quaternion::operator-(const MYON_Quaternion& q) const
{
  return MYON_Quaternion(a-q.a,b-q.b,c-q.c,d-q.d);
}

MYON_Quaternion  MYON_Quaternion::operator*(const MYON_Quaternion& q) const
{
  return MYON_Quaternion(a*q.a - b*q.b - c*q.c - d*q.d,
                       a*q.b + b*q.a + c*q.d - d*q.c,
                       a*q.c - b*q.d + c*q.a + d*q.b,
                       a*q.d + b*q.c - c*q.b + d*q.a);
}

MYON_Quaternion operator*(int x, const MYON_Quaternion& q)
{
  return MYON_Quaternion(x*q.a,x*q.b,x*q.c,x*q.d);
}

MYON_Quaternion operator*(float x, const MYON_Quaternion& q)
{
  return MYON_Quaternion(x*q.a,x*q.b,x*q.c,x*q.d);
}

MYON_Quaternion operator*(double x, const MYON_Quaternion& q)
{
  return MYON_Quaternion(x*q.a,x*q.b,x*q.c,x*q.d);
}

MYON_Quaternion MYON_Quaternion::RotationZYX(double yaw, double pitch, double roll)
{
  MYON_Xform X;
  X.RotationZYX(yaw, pitch, roll);
  MYON_Quaternion q;
  X.GetQuaternion(q);
  return q;
}

MYON_Quaternion MYON_Quaternion::RotationZYZ(double alpha, double beta, double gamma)
{
  MYON_Xform X;
  X.RotationZYZ(alpha, beta, gamma );
  MYON_Quaternion q;
  X.GetQuaternion(q);
  return q;
}


bool MYON_Quaternion::GetYawPitchRoll(double& yaw, double& pitch, double& roll) const
{
  MYON_Xform X;
  bool rc = GetRotation(X);
  if (rc)
    rc = GetYawPitchRoll(yaw, pitch, roll);
  return rc;
}

bool  MYON_Quaternion::GetEulerZYZ(double& alpha, double& beta, double& gamma) const
{
  MYON_Xform X;
  bool rc = GetRotation(X);
  if (rc)
    rc = GetEulerZYZ(alpha, beta, gamma);
  return rc;

}


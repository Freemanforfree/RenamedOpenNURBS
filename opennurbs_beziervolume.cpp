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

bool MYON_BezierCage::Read(MYON_BinaryArchive& archive)
{
  Destroy();

  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if ( rc )
  {
    while(rc)
    {
      if ( major_version != 1 )
      {
        MYON_ERROR("MYON_BezierCage::Read - old code unable to read new version of chunk");
        rc = false;
        break;
      }

      int dim=0,order0=0,order1=0,order2=0;
      bool is_rat=false;

      rc = archive.ReadInt(&dim);
      if (!rc)
        break;
      if (dim < 1 || dim > 10000)
      {
        MYON_ERROR("MYON_BezierCage::Read - invalid dim");
        rc=false;
        break;
      }

      rc = archive.ReadBool(&is_rat);
      if (!rc)
        break;

      rc = archive.ReadInt(&order0);
      if (!rc)
        break;
      if ( order0 < 2 || order0 > 10000 )
      {
        MYON_ERROR("MYON_BezierCage::Read - invalid order0");
        rc=false;
        break;
      }

      rc = archive.ReadInt(&order1);
      if (!rc)
        break;
      if ( order1 < 2 || order1 > 10000 )
      {
        MYON_ERROR("MYON_BezierCage::Read - invalid order1");
        rc=false;
        break;
      }

      rc = archive.ReadInt(&order2);
      if (!rc)
        break;
      if ( order2 < 2 || order2 > 10000 )
      {
        MYON_ERROR("MYON_BezierCage::Read - invalid order2");
        rc=false;
        break;
      }

      rc = Create(dim,is_rat,order0,order1,order2);
      if (!rc)
        break;

      int i,j,k;
      const int cv_dim = m_is_rat?(m_dim+1):m_dim;
      for(i = 0; i < order0 && rc; i++)
      {
        for(j = 0; j < order1 && rc; j++)
        {
          for ( k = 0; k < order2 && rc; k++)
          {
            rc = archive.ReadDouble(cv_dim,CV(i,j,k));
          }
        }
      }

      break;
    }

    if ( !archive.EndRead3dmChunk() )
    {
      rc = false;
    }
  }
  return rc;
}

bool MYON_BezierCage::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);

  if (rc)
  {
    rc = archive.WriteInt(m_dim);
    if(rc)
      rc = archive.WriteInt(m_is_rat);
    if (rc)
      rc = archive.WriteInt(m_order[0]);
    if (rc)
      rc = archive.WriteInt(m_order[1]);
    if (rc)
      rc = archive.WriteInt(m_order[2]);
    int i,j,k;
    const int cv_dim = m_is_rat?(m_dim+1):m_dim;
    double* bogus_cv = 0;
    for(i = 0; i < m_order[0] && rc; i++)
    {
      for(j = 0; j < m_order[1] && rc; j++)
      {
        for ( k = 0; k < m_order[2] && rc; k++)
        {
          const double* cv = CV(i,j,k);
          if ( !cv )
          {
            if ( 0 == bogus_cv )
            {
              bogus_cv = (double*)onmalloc(cv_dim*sizeof(*bogus_cv));
              for ( int n = 0; n < cv_dim; n++ )
                bogus_cv[n] = MYON_UNSET_VALUE;
            }
            cv = bogus_cv;
          }
          rc = archive.WriteDouble(cv_dim,cv);
        }
      }
    }

    if ( 0 != bogus_cv )
      onfree(bogus_cv);

    if ( !archive.EndWrite3dmChunk() )
    {
      rc = false;
    }
  }

  return rc;
}



MYON_BezierCage::MYON_BezierCage()
: m_dim(0),m_is_rat(0),m_cv_capacity(0),m_cv(0)
{
  m_order[0] = 0;
  m_order[1] = 0;
  m_order[2] = 0;
  m_cv_stride[0] = 0;
  m_cv_stride[1] = 0;
  m_cv_stride[2] = 0;
}

MYON_BezierCage::MYON_BezierCage( int dim, bool is_rat, int order0, int order1, int order2 )
                 : m_dim(0),m_is_rat(0),m_cv_capacity(0),m_cv(0)
{
  m_order[0] = 0;
  m_order[1] = 0;
  m_order[2] = 0;
  m_cv_stride[0] = 0;
  m_cv_stride[1] = 0;
  m_cv_stride[2] = 0;
  Create( dim, is_rat, order0, order1, order2 );
}

MYON_BezierCage::MYON_BezierCage( const MYON_BoundingBox& bbox, int order0, int order1, int order2 )
: m_dim(0),m_is_rat(0),m_cv_capacity(0),m_cv(0)
{
  Create(bbox,order0,order1,order2);
}

MYON_BezierCage::MYON_BezierCage( const MYON_3dPoint* box_corners, int order0, int order1, int order2 )
: m_dim(0),m_is_rat(0),m_cv_capacity(0),m_cv(0)
{
  Create(box_corners,order0,order1,order2);
}

MYON_BezierCage::~MYON_BezierCage()
{
  Destroy();
}
  
MYON_BezierCage::MYON_BezierCage(const MYON_BezierCage& src)
                 : m_dim(0),m_is_rat(0),m_cv_capacity(0),m_cv(0)
{
  m_order[0] = 0;
  m_order[1] = 0;
  m_order[2] = 0;
  m_cv_stride[0] = 0;
  m_cv_stride[1] = 0;
  m_cv_stride[2] = 0;
  *this = src;
}

MYON_BezierCage& MYON_BezierCage::operator=(const MYON_BezierCage& src)
{
  if ( this != &src ) {
    if ( Create( src.m_dim, src.m_is_rat, 
         src.m_order[0], src.m_order[1], src.m_order[2] ) )
    {
      const int sizeof_cv = src.CVSize()*sizeof(m_cv[0]);
      int i, j, k;
      for ( i = 0; i < m_order[0]; i++ ) 
      for ( j = 0; j < m_order[1]; j++ ) 
      for ( k = 0; k < m_order[2]; k++ ) 
      {
        memcpy( CV(i,j,k), src.CV(i,j,k), sizeof_cv );
      }
    }
    else 
    {
      Destroy();
    }
  }
  return *this;
}

bool MYON_BezierCage::IsValid() const
{
  if ( m_cv == nullptr )
    return false;

  if ( m_order[0] < 2 )
    return false;
  if ( m_order[1] < 2 )
    return false;
  if ( m_order[2] < 2 )
    return false;

  if ( m_dim <= 0 )
    return false;
  if ( m_is_rat != 0 && m_is_rat != 1 )
    return false;

  const int cvdim = m_is_rat ? (m_dim+1) : m_dim;

  if ( m_cv_capacity > 0 && m_cv_capacity < cvdim*m_order[0]*m_order[1]*m_order[2] )
    return false;

  int i[3];
  i[0] = (m_cv_stride[0] <= m_cv_stride[1]) ? 0 : 1;
  i[1] = 1-i[0];
  if ( m_cv_stride[2] < m_cv_stride[i[0]] )
  {
    i[2] = i[1];
    i[1] = i[0];
    i[0] = 2;
  }
  else if ( m_cv_stride[2] < m_cv_stride[i[1]] )
  {
    i[2] = i[1];
    i[1] = 2;
  }
  else
  {
    i[2] = 2;
  }

  if ( m_cv_stride[i[0]] < cvdim )
    return false;
  if ( m_cv_stride[i[1]] < m_cv_stride[i[0]]*m_order[i[0]] )
    return false;
  if ( m_cv_stride[i[2]] < m_cv_stride[i[1]]*m_order[i[1]] )
    return false;

  return true;
}

void MYON_BezierCage::Dump( MYON_TextLog& dump ) const
{
  dump.Print( "MYON_BezierCage dim = %d is_rat = %d\n"
               "        order = (%d, %d, %d) \n",
               m_dim, m_is_rat, m_order[0], m_order[1], m_order[2] );
  dump.Print( "Control Points  %d %s points\n"
               "  index               value\n",
               m_order[0]*m_order[1]*m_order[2], 
               (m_is_rat) ? "rational" : "non-rational" );
  if ( !m_cv ) 
  {
    dump.Print("  nullptr cv array\n");
  }
  else 
  {
    int i,j;
    char sPreamble[128] = { 0 };
    const size_t sPremable_capacity = sizeof(sPreamble) / sizeof(sPreamble[0]);
    for ( i = 0; i < m_order[0]; i++ )
    {
      for ( j = 0; j < m_order[1]; j++ )
      {
        if ( i > 0 || j > 0)
          dump.Print("\n");
        MYON_String::FormatIntoBuffer(sPreamble, sPremable_capacity,"  CV[%2d][%2d]", i, j);
        dump.PrintPointList( m_dim, m_is_rat, 
                          m_order[2], m_cv_stride[2],
                          CV(i,j,0), 
                          sPreamble );
      }
      if ( i < m_order[0]-1)
        dump.Print("\n");
    }
  }
}

int MYON_BezierCage::Dimension() const
{
  return m_dim;
}

bool MYON_BezierCage::Create( const MYON_BoundingBox& bbox, int order0, int order1, int order2 )
{
  /*
            7______________6
            |\             |\
            | \            | \
            |  \ _____________\
            |   4          |   5
            |   |          |   |
            |   |          |   |
            3---|----------2   |
            \   |          \   |
             \  |z          \  |
            y \ |            \ |
               \0_____________\1
                       x
  */
  MYON_3dPoint box_corners[8];
  box_corners[0] = bbox.Corner(0,0,0);
  box_corners[1] = bbox.Corner(1,0,0);
  box_corners[2] = bbox.Corner(1,1,0);
  box_corners[3] = bbox.Corner(0,1,0);
  box_corners[4] = bbox.Corner(0,0,1);
  box_corners[5] = bbox.Corner(1,0,1);
  box_corners[6] = bbox.Corner(1,1,1);
  box_corners[7] = bbox.Corner(0,1,1);
  return Create(box_corners,order0,order1,order2);
}

bool MYON_BezierCage::Create( int dim, bool is_rat, int order0, int order1, int order2 )
{
  if ( m_cv_capacity < 1 )
    m_cv = 0;
  m_dim = (dim>0) ? dim : 0;
  m_is_rat = is_rat ? 1 : 0;
  m_order[0] = (order0 >= 2) ? order0 : 0;
  m_order[1] = (order1 >= 2) ? order1 : 0;
  m_order[2] = (order2 >= 2) ? order2 : 0;
  m_cv_stride[2] = (m_dim > 0) ? m_dim+m_is_rat : 0;
  m_cv_stride[1] = m_cv_stride[2]*m_order[2];
  m_cv_stride[0] = m_cv_stride[1]*m_order[1];
  m_cv_capacity = m_cv_stride[0]*m_order[0];
  m_cv = (double*)onrealloc( m_cv, m_cv_capacity*sizeof(m_cv[0]) );
  return IsValid() ? true : false;
}

bool MYON_BezierCage::Create(
  const MYON_3dPoint* box_corners,
  int order0,
  int order1,
  int order2
  )
{
  int i, j, k;
  double r,s,t;

  if ( 0 == box_corners )
    return false;
  for( i = 0; i < 8; i++ )
  {
    if ( !box_corners[i].IsValid() )
      return false;
  }

  // create trilinear "cube" to make it easy
  // to calculate CV locations.
  MYON_BezierCage cube(3,0,2,2,2);
  cube.SetCV(0,0,0,box_corners[0]);
  cube.SetCV(1,0,0,box_corners[1]);
  cube.SetCV(1,1,0,box_corners[2]);
  cube.SetCV(0,1,0,box_corners[3]);
  cube.SetCV(0,0,1,box_corners[4]);
  cube.SetCV(1,0,1,box_corners[5]);
  cube.SetCV(1,1,1,box_corners[6]);
  cube.SetCV(0,1,1,box_corners[7]);

  if ( 2 == order0 && 2 == order1 && 2 == order2 )
  {
    operator=(cube);
  }
  else
  {
    if (!Create(3,0,order0,order1,order2))
      return false;
    const int d0 = Degree(0);
    const int d1 = Degree(1);
    const int d2 = Degree(2);

    for (i = 0; i <= d0; i++)
    {
      r = ((double)i)/((double)d0);
      for (j = 0; j <= d1; j++)
      {
        s = ((double)j)/((double)d1);
        for (k = 0; k <= d2; k++)
        {
          t = ((double)k)/((double)d2);
          SetCV(i,j,k,cube.PointAt(r,s,t));
        }
      }
    }
  }
  return IsValid();
}


void MYON_BezierCage::Destroy()
{
  if ( m_cv && m_cv_capacity > 0 )
    onfree(m_cv);
  m_cv_capacity = 0;
  m_cv_stride[0] = 0;
  m_cv_stride[1] = 0;
  m_cv_stride[2] = 0;
  m_cv = 0;
  m_dim = 0;
  m_is_rat = 0;
  m_order[0] = 0;
  m_order[1] = 0;
  m_order[2] = 0;
}

void MYON_BezierCage::EmergencyDestroy()
{
  m_cv_capacity = 0;
  m_cv_stride[0] = 0;
  m_cv_stride[1] = 0;
  m_cv_stride[2] = 0;
  m_cv = 0;
  m_dim = 0;
  m_is_rat = 0;
  m_order[0] = 0;
  m_order[1] = 0;
  m_order[2] = 0;
}


bool MYON_BezierCage::GetBBox( // returns true if successful
       double* boxmin,    // minimum
       double* boxmax,    // maximum
       bool bGrowBox  // true means grow box
       ) const
{
  int i, j;
  bool rc = (m_order[0] > 0 && m_order[1] > 0 && m_order[2] > 0) ? true : false;
  for ( i = 0; rc && i < m_order[0]; i++ ) 
  for ( j = 0; rc && j < m_order[1]; j++ ) 
  {
    rc = MYON_GetPointListBoundingBox( m_dim, m_is_rat, m_order[2], m_cv_stride[2],
                                    CV(i,j,0), boxmin, boxmax, bGrowBox );
    bGrowBox = true;
  }
  return rc;
}

bool MYON_BezierCage::Transform( const MYON_Xform& xform )
{
  int i,j;
  bool rc = (m_order[0] > 0 && m_order[1] > 0 && m_order[2]) ? true : false;
  if (rc)
  {  
    if ( 0 == m_is_rat )
    {
      if ( xform.m_xform[3][0] != 0.0 || xform.m_xform[3][1] != 0.0 || xform.m_xform[3][2] != 0.0 )
      {
        MakeRational();
      }
    }
  
    for ( i = 0; rc && i < m_order[0]; i++ ) 
    {
      for ( j = 0; rc && j < m_order[1]; j++ ) 
      {
        rc = MYON_TransformPointList( m_dim, m_is_rat, 
                                    m_order[2], m_cv_stride[2], 
                                    CV(i,j,0), xform );
      }
    }
  }
  return rc;
}

bool MYON_BezierCage::Rotate(
      double sin_angle,          // sin(angle)
      double cos_angle,          // cos(angle)
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint& center // center of rotation
      )
{
  MYON_Xform rot;
  rot.Rotation( sin_angle, cos_angle, axis, center );
  return Transform( rot );
}

bool MYON_BezierCage::Rotate(
      double angle,              // angle in radians
      const MYON_3dVector& axis, // axis of rotation
      const MYON_3dPoint& center // center of rotation
      )
{
  return Rotate( sin(angle), cos(angle), axis, center );
}

bool MYON_BezierCage::Translate( const MYON_3dVector& delta )
{
  MYON_Xform tr(MYON_Xform::TranslationTransformation( delta ));
  return Transform( tr );
}

bool MYON_BezierCage::Scale( double x )
{
  MYON_Xform s(MYON_Xform::DiagonalTransformation(x));
  return Transform( s );
}

MYON_Interval MYON_BezierCage::Domain( 
      int // dir - formal parameter intentionally ignored in this virtual function
      ) const
{
  return MYON_Interval(0.0,1.0);
}

bool MYON_BezierCage::Evaluate( // returns false if unable to evaluate
       double r, double s, double t,       // evaluation parameter
       int der_count,            // number of derivatives (>=0)
       int v_stride,             // array stride (>=Dimension())
       double* v                 // array of length stride*(ndir+1)*(ndir+2)/2
       ) const
{  
  const int cvdim = m_is_rat?(m_dim+1):m_dim;
  int i,j,k,n;
  double Barray[64], vtmparray[10*4], Bi, Bij, Bijk, *Bj, *Bk, *vtmp;
  const double* CVi;
  const double* CVij;
  const double* CVijk;
  void* freeme1 = 0;
  void* freeme2 = 0;
  size_t sz;

  if ( der_count > 0 )
  {
    // TODO - time to add support for derivative evaluation
    MYON_ERROR("MYON_BezierCage::Evaluate does not evaluate derivatives");
  }

  sz = cvdim*sizeof(*vtmp);
  vtmp = m_is_rat 
       ? (sz <= sizeof(vtmparray) ? vtmparray : (double*)(freeme1 = onmalloc(sz))) 
       : v;
  memset(vtmp,0,sz);

  // get arrays to hold values of Bernstein basis functions
  sz = (m_order[1]+m_order[2])*sizeof(*Bj);
  Bj = (sz <= sizeof(Barray))
     ? Barray 
     : ((double*)(freeme2=onmalloc( sz ) ));
  Bk = Bj + m_order[1];

  const int d2 = m_order[2]-1;
  for ( k = 0; k <= d2; k++)
  {
    Bk[k] = MYON_EvaluateBernsteinBasis(d2,k,t);
  }

  const int d1 = m_order[1]-1;
  for ( j = 0; j <= d1; j++)
  {
    Bj[j] = MYON_EvaluateBernsteinBasis(d1,j,s);
  }

  const int d0 = m_order[0]-1;

  for ( i = 0; i <= d0; i++ )
  {
    CVi = m_cv + i*m_cv_stride[0];
    Bi = MYON_EvaluateBernsteinBasis(d0,i,r);
    for ( j = 0; j <= d1; j++ )
    {
      CVij = CVi + j*m_cv_stride[1];
      Bij = Bi*Bj[j];
      for ( k = 0; k <= d2; k++ )
      {
        CVijk = CVij + k*m_cv_stride[2];
        Bijk = Bij*Bk[k];
        n = cvdim;
        while(n--)
        {
          *vtmp++ += *CVijk++ * Bijk;
        }
        vtmp -= cvdim;
      }
    }
  }

  if ( m_is_rat )
  {

    Bi = (vtmp[m_dim] != 0.0) ? (1.0/vtmp[m_dim]) : 1.0;
    for ( n = 0; n < m_dim; n++ )
    {
      v[n] = vtmp[n]*Bi;
    }
  }

  if ( 0 != freeme1 )
    onfree(freeme1);
  
  if ( 0 != freeme2 )
    onfree(freeme2);

  return (0 == der_count);
}

MYON_3dPoint MYON_BezierCage::PointAt(
        double r, 
        double s, 
        double t
        ) const
{
  MYON_3dPoint pt;
  if ( m_dim <= 3 )
  {
    pt.x  = 0.0;
    pt.y  = 0.0;
    pt.z  = 0.0;
    Evaluate(r,s,t,0,3,&pt.x);
  }
  else
  {
    double stack_buffer[16];
    double* v;
    size_t sizeof_buffer = m_dim*sizeof(*v);
    v = (sizeof_buffer <= sizeof(stack_buffer)) ? stack_buffer : (double*)onmalloc(sizeof_buffer);
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
    Evaluate(r,s,t,0,m_dim,v);
    pt.x = v[0];
    pt.y = v[1];
    pt.z = v[2];
    if ( v != stack_buffer )
      onfree(v);
  }
  return pt;
}

MYON_3dPoint MYON_BezierCage::PointAt( MYON_3dPoint rst ) const
{
  MYON_3dPoint pt;
  if ( m_dim <= 3 )
  {
    pt.x  = 0.0;
    pt.y  = 0.0;
    pt.z  = 0.0;
    Evaluate(rst.x,rst.y,rst.z,0,3,&pt.x);
  }
  else
  {
    double stack_buffer[16];
    double* v;
    size_t sizeof_buffer = m_dim*sizeof(*v);
    v = (sizeof_buffer <= sizeof(stack_buffer)) ? stack_buffer : (double*)onmalloc(sizeof_buffer);
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
    Evaluate(rst.x,rst.y,rst.z,0,m_dim,v);
    pt.x = v[0];
    pt.y = v[1];
    pt.z = v[2];
    if ( v != stack_buffer )
      onfree(v);
  }
  return pt;
}

bool MYON_BezierCage::IsRational() const
{
  return m_is_rat ? true : false;
}

int MYON_BezierCage::CVSize() const
{
  return ( m_is_rat && m_dim>0 ) ? m_dim+1 : m_dim;
}

int MYON_BezierCage::Order( int dir ) const
{
  return (dir>=0&&dir<=2) ? m_order[dir] : 0;
}

int MYON_BezierCage::Degree(int dir) const
{
  int order = Order(dir);
  return (order>=2) ? order-1 : 0;
}

double* MYON_BezierCage::CV( int i, int j, int k ) const
{
#if defined(MYON_DEBUG)
  if ( 0 == m_cv )
  {
    MYON_ERROR("MYON_BezierCage::CV - nullptr m_cv");
    return 0;
  }
  if ( i < 0 || i >= m_order[0] || j< 0 || j >= m_order[1] || k < 0 || k >= m_order[2])
  {
    MYON_ERROR("MYON_BezierCage::CV - (i,j,k) out of range");
    return 0;
  }
#endif
  return (m_cv) ? (m_cv + i*m_cv_stride[0] + j*m_cv_stride[1] + k*m_cv_stride[2]) : 0;
}

MYON::point_style MYON_BezierCage::CVStyle() const
{
  return m_is_rat ? MYON::homogeneous_rational : MYON::not_rational;
}

double MYON_BezierCage::Weight( int i, int j, int k ) const
{
  return (m_cv && m_is_rat) ? m_cv[i*m_cv_stride[0] + j*m_cv_stride[1] + k*m_cv_stride[2] + + m_dim] : 1.0;
}


bool MYON_BezierCage::SetWeight( int i, int j, int k, double w )
{
  bool rc = false;
  if ( m_is_rat ) 
  {
    double* cv = CV(i,j,k);
    if (cv) 
    {
      cv[m_dim] = w;
      rc = true;
    }
  }
  else if ( w == 1.0 ) 
  {
    rc = true;
  }
  return rc;
}

bool MYON_BezierCage::SetCV( int i, int j, int k, MYON::point_style style, const double* Point )
{
  bool rc = true;
  int n;
  double w;

  double* cv = CV(i,j,k);
  if ( !cv )
    return false;

  switch ( style ) {

  case MYON::not_rational:  // input Point is not rational
    memcpy( cv, Point, m_dim*sizeof(*cv) );
    if ( IsRational() ) {
      // NURBS surface is rational - set weight to one
      cv[m_dim] = 1.0;
    }
    break;

  case MYON::homogeneous_rational:  // input Point is homogeneous rational
    if ( IsRational() ) {
      // NURBS surface is rational
      memcpy( cv, Point, (m_dim+1)*sizeof(*cv) );
    }
    else {
      // NURBS surface is not rational
      w = (Point[m_dim] != 0.0) ? 1.0/Point[m_dim] : 1.0;
      for ( n = 0; n < m_dim; n++ ) {
        cv[n] = w*Point[n];
      }
    }
    break;

  case MYON::euclidean_rational:  // input Point is euclidean rational
    if ( IsRational() ) {
      // NURBS surface is rational - convert euclean point to homogeneous form
      w = Point[m_dim];
      for ( n = 0; n < m_dim; n++ )
        cv[i] = w*Point[i];
      cv[m_dim] = w;
    }
    else {
      // NURBS surface is not rational
      memcpy( cv, Point, m_dim*sizeof(*cv) );
    }
    break;

  case MYON::intrinsic_point_style:
    n = m_is_rat?m_dim+1:m_dim;
    memcpy(cv,Point,n*sizeof(*cv));
    break;
    
  default:
    rc = false;
    break;
  }
  return rc;
}

bool MYON_BezierCage::SetCV( int i, int j, int k, const MYON_3dPoint& point )
{
  bool rc = false;
  double* cv = CV(i,j,k);
  if ( cv ) {
    cv[0] = point.x;
    if ( m_dim > 1 ) {
      cv[1] = point.y;
      if ( m_dim > 2 )
        cv[2] = point.z;
    }
    if ( m_is_rat ) {
      cv[m_dim] = 1.0;
    }
    rc = true;
  }
  return rc;
}

bool MYON_BezierCage::SetCV( int i, int j, int k, const MYON_4dPoint& point )
{
  bool rc = false;
  double* cv = CV(i,j,k);
  if ( cv ) {
    if ( m_is_rat ) {
      cv[0] = point.x;
      if ( m_dim > 1 ) {
        cv[1] = point.y;
        if ( m_dim > 2 )
          cv[2] = point.z;
      }
      cv[m_dim] = point.w;
      rc = true;
    }
    else {
      double w;
      if ( point.w != 0.0 ) {
        w = 1.0/point.w;
        rc = true;
      }
      else {
        w = 1.0;
      }
      cv[0] = w*point.x;
      if ( m_dim > 1 ) {
        cv[1] = w*point.y;
        if ( m_dim > 2 ) {
          cv[2] = w*point.z;
        }
      }
    }
  }
  return rc;
}

bool MYON_BezierCage::GetCV( int i, int j, int k, MYON::point_style style, double* Point ) const
{
  const double* cv = CV(i,j,k);
  if ( !cv )
    return false;
  int dim = Dimension();
  double w = ( IsRational() ) ? cv[dim] : 1.0;
  switch(style) {
  case MYON::euclidean_rational:
    Point[dim] = w;
    // no break here
  case MYON::not_rational:
    if ( w == 0.0 )
      return false;
    w = 1.0/w;
    while(dim--) *Point++ = *cv++ * w;
    break;
  case MYON::homogeneous_rational:
    Point[dim] = w;
    memcpy( Point, cv, dim*sizeof(*Point) );
    break;
  default:
    return false;
  }
  return true;
}

bool MYON_BezierCage::GetCV( int i, int j, int k, MYON_3dPoint& point ) const
{
  bool rc = false;
  const double* cv = CV(i,j,k);
  if ( cv ) {
    if ( m_is_rat ) {
      if (cv[m_dim] != 0.0) {
        const double w = 1.0/cv[m_dim];
        point.x = cv[0]*w;
        point.y = (m_dim>1)? cv[1]*w : 0.0;
        point.z = (m_dim>2)? cv[2]*w : 0.0;
        rc = true;
      }
    }
    else {
      point.x = cv[0];
      point.y = (m_dim>1)? cv[1] : 0.0;
      point.z = (m_dim>2)? cv[2] : 0.0;
      rc = true;
    }
  }
  return rc;
}

bool MYON_BezierCage::GetCV( int i, int j, int k, MYON_4dPoint& point ) const
{
  bool rc = false;
  const double* cv = CV(i,j,k);
  if ( cv ) {
    point.x = cv[0];
    point.y = (m_dim>1)? cv[1] : 0.0;
    point.z = (m_dim>2)? cv[2] : 0.0;
    point.w = (m_is_rat) ? cv[m_dim] : 1.0;
    rc = true;
  }
  return rc;
}

bool MYON_BezierCage::ZeroCVs()
{
  // zeros control vertices and, if rational, sets weights to 1
  bool rc = false;
  int i,j,k;
  if ( m_cv ) {
    if ( m_cv_capacity > 0 ) {
      memset( m_cv, 0, m_cv_capacity*sizeof(*m_cv) );
      if ( m_is_rat ) {
        for ( i = 0; i < m_order[0]; i++ ) {
          for ( j = 0; j < m_order[1]; j++ ) {
            for ( k = 0; k < m_order[2]; k++ ) {
              SetWeight( i,j,k, 1.0 );
            }
          }
        }
      }
      rc = true;
    }
    else {
      double* cv;
      int s = CVSize()*sizeof(*cv);
      for ( i = 0; i < m_order[0]; i++ ) {
        for ( j = 0; j < m_order[1]; j++ ) {
          for ( k = 0; k < m_order[2]; k++ ) {
            cv = CV(i,j,k);
            memset(cv,0,s);
            if ( m_is_rat )
              cv[m_dim] = 1.0;
          }
        }
      }
      rc = (i>0) ? true : false;
    }
  }
  return rc;
}

bool MYON_BezierCage::MakeRational()
{
  if ( !IsRational() ) 
  {
    MYON_ERROR("TODO: fill in MYON_BezierCage::MakeRational()");
    /*
    const int dim = Dimension();
    if ( m_order[0] > 0 && m_order[1] > 0 && m_order[2] > 0 && dim > 0 ) {
      const double* old_cv;
      double* new_cv;
      int cvi, cvj, j, cvstride;
      if ( m_cv_stride[0] < m_cv_stride[1] ) {
        cvstride = m_cv_stride[0] > dim ? m_cv_stride[0] : dim+1;
        ReserveCVCapacity( cvstride*m_order[0]*m_order[1] );
        new_cv = m_cv + cvstride*m_order[0]*m_order[1]-1;
				for ( cvj = m_order[1]-1; cvj >= 0; cvj-- ) {
          for ( cvi = m_order[0]-1; cvi >= 0; cvi-- ) {
            old_cv = CV(cvi,cvj)+dim-1;
            *new_cv-- = 1.0;
            for ( j = 0; j < dim; j++ ) {
              *new_cv-- = *old_cv--;
            }
          }
        }
        m_cv_stride[0] = dim+1;
        m_cv_stride[1] = (dim+1)*m_order[0];
      }
      else {
        cvstride = m_cv_stride[1] > dim ? m_cv_stride[1] : dim+1;
        ReserveCVCapacity( cvstride*m_order[0]*m_order[1] );
        new_cv = m_cv + cvstride*m_order[0]*m_order[1]-1;
        for ( cvi = m_order[0]-1; cvi >= 0; cvi-- ) {
          for ( cvj = m_order[1]-1; cvj >= 0; cvj-- ) {
            old_cv = CV(cvi,cvj)+dim-1;
            *new_cv-- = 1.0;
            for ( j = 0; j < dim; j++ ) {
              *new_cv-- = *old_cv--;
            }
          }
        }
        m_cv_stride[1] = dim+1;
        m_cv_stride[0] = (dim+1)*m_order[1];
      }
      m_is_rat = 1;
    }
    */
  }
  return IsRational();
}

bool MYON_BezierCage::MakeNonRational()
{
  if ( IsRational() ) 
  {
    MYON_ERROR("TODO: fill in MYON_BezierCage::MakeNonRational()");
    /*
    const int dim = Dimension();
    if ( m_order[0] > 0 && m_order[1] > 0 && dim > 0 ) {
      double w;
      const double* old_cv;
      double* new_cv = m_cv;
      int cvi, cvj, j;
      if ( m_cv_stride[0] < m_cv_stride[1] ) {
        for ( cvj = 0; cvj < m_order[1]; cvj++ ) {
          for ( cvi = 0; cvi < m_order[0]; cvi++ ) {
            old_cv = CV(cvi,cvj);
            w = old_cv[dim];
            w = ( w != 0.0 ) ? 1.0/w : 1.0;
            for ( j = 0; j < dim; j++ ) {
              *new_cv++ = w*(*old_cv++);
            }
          }
        }
        m_cv_stride[0] = dim;
        m_cv_stride[1] = dim*m_order[0];
      }
      else {
        for ( cvi = 0; cvi < m_order[0]; cvi++ ) {
          for ( cvj = 0; cvj < m_order[1]; cvj++ ) {
            old_cv = CV(cvi,cvj);
            w = old_cv[dim];
            w = ( w != 0.0 ) ? 1.0/w : 1.0;
            for ( j = 0; j < dim; j++ ) {
              *new_cv++ = w*(*old_cv++);
            }
          }
        }
        m_cv_stride[1] = dim;
        m_cv_stride[0] = dim*m_order[1];
      }
      m_is_rat = 0;
    }
    */
  }
  return ( !IsRational() ) ? true : false;
}

/////////////////////////////////////////////////////////////////
// Tools for managing CV and knot memory
bool MYON_BezierCage::ReserveCVCapacity(
  int capacity// number of doubles to reserve
  )
{
  if ( m_cv_capacity < capacity ) {
    if ( m_cv ) {
      if ( m_cv_capacity ) {
        m_cv = (double*)onrealloc( m_cv, capacity*sizeof(*m_cv) );
        m_cv_capacity = (m_cv) ? capacity : 0;
      }
      // else user supplied m_cv[] array
    }
    else {
      m_cv = (double*)onmalloc( capacity*sizeof(*m_cv) );
      m_cv_capacity = (m_cv) ? capacity : 0;
    }
  }
  return ( m_cv ) ? true : false;
}



bool MYON_BezierCage::IsSingular(		 // true if surface side is collapsed to a point
       int side														 // side of parameter space to test
																			// 0 = south, 1 = east, 2 = north, 3 = west, 4 = bottom, 5 =top
				) const
{
  MYON_ERROR("TODO: fill in MYON_BezierCage::IsSingular\n");
  return false;
}



const MYON_Xform& MYON_BezierCageMorph::WorldToUnitCube() const
{
  // maps world point into unit cube that can be morphed by
  // evaluation of the BezierCage() function.
  return m_xyz2rst;
}

const MYON_BezierCage& MYON_BezierCageMorph::BezierCage() const
{
  // morphs a point in the unit cube into a world point.
  return m_rst2xyz;
}


bool MYON_BezierCageMorph::Transform(const MYON_Xform& xform)
{
  // transforms the VolumeMorph as a piece of geometry
  return m_rst2xyz.Transform(xform);
}

bool MYON_BezierCageMorph::Read(MYON_BinaryArchive& archive)
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if ( rc )
  {
    if ( major_version != 1 )
      rc = false;
    if (rc)
      rc = archive.ReadXform(m_xyz2rst);
    if(rc)
      rc = m_rst2xyz.Read(archive);
    if ( !archive.EndRead3dmChunk() )
      rc = false;
  }
  return rc;
}

bool MYON_BezierCageMorph::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if (rc)
  {
    if (rc)
      rc = archive.WriteXform(m_xyz2rst);
    if(rc)
      rc = m_rst2xyz.Write(archive);
    if ( !archive.EndWrite3dmChunk() )
      rc = false;
  }
  return rc;
}


MYON_BezierCageMorph::MYON_BezierCageMorph() : m_bValid(0)
{
}

MYON_BezierCageMorph::~MYON_BezierCageMorph()
{
}


bool MYON_BezierCageMorph::Create(
    MYON_3dPoint P0,
    MYON_3dPoint P1,
    MYON_3dPoint P2,
    MYON_3dPoint P3,
    int point_countX,
    int point_countY,
    int point_countZ
    )
{
  if ( point_countX < 2 || point_countY < 2 || point_countZ < 2 
      || !P0.IsValid()
      || !P1.IsValid()
      || !P2.IsValid()
      || !P3.IsValid() )
  {
    MYON_ERROR("MYON_BezierCageMorph::Create - invalid input");
  }

  m_bValid = false;
  MYON_3dVector X = P1-P0;
  MYON_3dVector Y = P2-P0;
  MYON_3dVector Z = P3-P0;
  MYON_Xform xform(MYON_Xform::IdentityTransformation);
  xform[0][0] = X.x;
  xform[1][0] = X.y;
  xform[2][0] = X.z;
  xform[0][1] = Y.x;
  xform[1][1] = Y.y;
  xform[2][1] = Y.z;
  xform[0][2] = Z.x;
  xform[1][2] = Z.y;
  xform[2][2] = Z.z;
  xform[0][3] = P0.x;
  xform[1][3] = P0.y;
  xform[2][3] = P0.z;
  double min_pivot = 0.0;
  m_bValid = xform.Invert(&min_pivot);
  if (m_bValid)
  {
    MYON_3dPoint box_corners[8];
    box_corners[0] = P0;
    box_corners[1] = P1;
    box_corners[2] = P0+X+Y;
    box_corners[3] = P2;
    box_corners[4] = P3;
    box_corners[5] = P3+X;
    box_corners[6] = P3+X+Y;
    box_corners[7] = P3+Y;
    m_bValid = m_rst2xyz.Create(box_corners,point_countX,point_countY,point_countZ);
    m_xyz2rst = xform;
  }
  else
  {
    MYON_ERROR("MYON_BezierCageMorph::Create - invalid input - P0,P1,P2,P3 are coplanar");
    m_rst2xyz.Destroy();
  }
  return m_bValid;
}

bool MYON_BezierCageMorph::SetXform( MYON_Xform world2unitcube )
{
  m_xyz2rst = world2unitcube;
  m_bValid = m_xyz2rst.IsValid() && m_rst2xyz.IsValid();
  return m_bValid;
}

bool MYON_BezierCageMorph::SetBezierCage( MYON_BezierCage& unitcube2world )
{
  m_rst2xyz = unitcube2world;
  m_bValid = m_xyz2rst.IsValid() && m_rst2xyz.IsValid();
  return m_bValid;
}

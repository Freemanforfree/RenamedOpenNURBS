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

MYON_OBJECT_IMPLEMENT(MYON_SurfaceProxy,MYON_Surface,"4ED7D4E2-E947-11d3-BFE5-0010830122F0");

MYON_SurfaceProxy::MYON_SurfaceProxy() : m_surface(0), m_bTransposed(0)
{}

MYON_SurfaceProxy::MYON_SurfaceProxy( const MYON_Surface* s ) : m_surface(s), m_bTransposed(0)
{}

MYON_SurfaceProxy::MYON_SurfaceProxy( const MYON_SurfaceProxy& src ) : MYON_Surface(src), m_surface(0), m_bTransposed(0)
{
  *this = src;
}

unsigned int MYON_SurfaceProxy::SizeOf() const
{
  unsigned int sz = MYON_Surface::SizeOf();
  sz += (sizeof(*this) - sizeof(MYON_Surface));
  // Do not add in size of m_surface - its memory is not
  // managed by this class.
  return sz;
}

MYON__UINT32 MYON_SurfaceProxy::DataCRC(MYON__UINT32 current_remainder) const
{
  if ( m_surface )
    current_remainder = m_surface->DataCRC(current_remainder);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_bTransposed),&m_bTransposed);
  return current_remainder;
}

MYON_SurfaceProxy& MYON_SurfaceProxy::operator=( const MYON_SurfaceProxy& src )
{
  if ( this != &src ) {
    MYON_Surface::operator=(src);
    m_surface = src.m_surface;
    m_bTransposed = src.m_bTransposed;
  }
  return *this;
}

MYON_SurfaceProxy::~MYON_SurfaceProxy()
{
  m_surface = 0;
}

void MYON_SurfaceProxy::SetProxySurface( const MYON_Surface* proxy_surface )
{
  // setting m_surface=0 prevents crashes if user has deleted
  // "real" surface before calling SetProxySurface().
  m_surface = 0;

  DestroySurfaceTree();
  if ( proxy_surface == this )
    proxy_surface = 0;
  m_surface = proxy_surface;
  m_bTransposed = false;
}

const MYON_Surface* MYON_SurfaceProxy::ProxySurface() const
{
  return m_surface;
}

bool MYON_SurfaceProxy::ProxySurfaceIsTransposed() const
{
  return m_bTransposed;
}


MYON_Surface* MYON_SurfaceProxy::DuplicateSurface() const
{
  MYON_Surface* dup_srf = 0;
  if ( m_surface )
  {
    dup_srf = m_surface->Duplicate();
    if ( m_bTransposed && dup_srf )
      dup_srf->Transpose();
  }
  return dup_srf;
}


bool MYON_SurfaceProxy::IsValid( MYON_TextLog* text_log ) const
{
  return ( m_surface ) ? m_surface->IsValid(text_log) : false;
}

void
MYON_SurfaceProxy::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_SurfaceProxy uses %x\n",m_surface);
  if (m_surface ) 
    m_surface->Dump(dump);
}

bool 
MYON_SurfaceProxy::Write(
       MYON_BinaryArchive&  // open binary file
     ) const
{
  return false;
}

bool 
MYON_SurfaceProxy::Read(
       MYON_BinaryArchive&  // open binary file
     )
{
  return false;
}

int 
MYON_SurfaceProxy::Dimension() const
{
  return ( m_surface ) ? m_surface->Dimension() : 0;
}

bool MYON_SurfaceProxy::GetBBox( // returns true if successful
         double* boxmin,    // minimum
         double* boxmax,    // maximum
         bool bGrowBox
         ) const
{
  return ( m_surface ) ? m_surface->GetBBox(boxmin,boxmax,bGrowBox) : false;
}

bool
MYON_SurfaceProxy::Transform( 
    const MYON_Xform& // xform - formal parameter intentionally ignored in this virtual function
    )
{
  return false; // cannot modify m_surface
}


MYON_Interval
MYON_SurfaceProxy::Domain( int dir ) const
{
  MYON_Interval d;
  if ( m_bTransposed ) {
    dir = (dir) ? 0 : 1;
  }
  if ( m_surface ) 
    d = m_surface->Domain(dir);
  return d;
}

bool MYON_SurfaceProxy::GetSurfaceSize( 
    double* width, 
    double* height 
    ) const
{
  bool rc = false;
  if ( m_surface )
  {
    if ( m_bTransposed )
    {
      double* ptr = width;
      width = height;
      height = ptr;
    }
    rc = m_surface->GetSurfaceSize(width,height);
  }
  else
  {
    if ( width )
      *width = 0.0;
    if ( height )
      *height = 0.0;
  }
  return rc;
}

int
MYON_SurfaceProxy::SpanCount( int dir ) const
{
  if ( m_bTransposed ) {
    dir = (dir) ? 0 : 1;
  }
  return ( m_surface ) ? m_surface->SpanCount(dir) : false;
}

bool
MYON_SurfaceProxy::GetSpanVector( int dir, double* s ) const
{
  if ( m_bTransposed ) {
    dir = (dir) ? 0 : 1;
  }
  return ( m_surface ) ? m_surface->GetSpanVector(dir,s) : false;
}

int
MYON_SurfaceProxy::Degree( int dir ) const
{
  if ( m_bTransposed ) {
    dir = (dir) ? 0 : 1;
  }
  return ( m_surface ) ? m_surface->Degree(dir) : false;
}


bool 
MYON_SurfaceProxy::GetParameterTolerance(
         int dir,
         double t,  // t = parameter in domain
         double* tminus, // tminus
         double* tplus   // tplus
         ) const
{
  if ( m_bTransposed ) {
    dir = (dir) ? 0 : 1;
  }
  return ( m_surface ) ? m_surface->GetParameterTolerance(dir,t,tminus,tplus) : false;
}

bool 
MYON_SurfaceProxy::IsClosed( int dir ) const
{
  if ( m_bTransposed ) {
    dir = (dir) ? 0 : 1;
  }
  return ( m_surface ) ? m_surface->IsClosed( dir ) : false;
}

MYON_Surface::ISO 
MYON_SurfaceProxy::IsIsoparametric( // returns isoparametric status of 2d curve
        const MYON_Curve& crv,
        const MYON_Interval* subdomain
        ) const
{
  // this is a virtual overide of an MYON_Surface::IsIsoparametric

  const MYON_Curve* pC = &crv;
	MYON_Curve* pTranC = nullptr;
	if(m_bTransposed)
  {
		pTranC = crv.DuplicateCurve();
		pTranC->SwapCoordinates(0,1);
		pC = pTranC;
	}	

	MYON_Surface::ISO iso = m_surface->IsIsoparametric( *pC, subdomain);

	if (pTranC)
  {
		switch(iso)
    {
		case x_iso:
			iso = y_iso;
      break;
		case y_iso:
			iso = x_iso;
      break;
		case W_iso:
			iso = S_iso;
      break;
		case S_iso:
			iso = W_iso;
      break;
		case N_iso:
			iso = E_iso;
      break;
		case E_iso:
			iso = N_iso;
      break;
    default:
      // intentionally ignoring other MYON_Surface::ISO enum values
      break;
		}
    delete pTranC;
	}

	return iso;
}

MYON_Surface::ISO 
MYON_SurfaceProxy::IsIsoparametric( // returns isoparametric status based on bounding box
        const MYON_BoundingBox& box
        ) const
{	
  // this is a virtual overide of an MYON_Surface::IsIsoparametric
	const MYON_BoundingBox* pbox = &box;
	MYON_BoundingBox Tbox(	MYON_3dPoint( box.m_min[1],box.m_min[0],0.0),
												MYON_3dPoint( box.m_max[1],box.m_max[0],0.0) );
	if(m_bTransposed)
		pbox = &Tbox;

	MYON_Surface::ISO iso = m_surface->IsIsoparametric( *pbox);

	if( m_bTransposed){
		switch(iso)
    {
		case x_iso:
			iso = y_iso;
      break;
		case y_iso:
			iso = x_iso;
      break;
		case W_iso:
			iso = S_iso;
      break;
		case S_iso:
			iso = W_iso;
      break;
		case N_iso:
			iso = E_iso;
      break;
		case E_iso:
			iso = N_iso;
      break;
    default:
      // intentionally ignoring other MYON_Surface::ISO enum values
      break;
		}
	}
	return iso;
}




bool MYON_SurfaceProxy::IsPlanar(
      MYON_Plane* plane,
      double tolerance
      ) const
{
  bool rc = false;
  if ( m_surface )
  {
    rc = m_surface->IsPlanar( plane, tolerance );
    if (rc && m_bTransposed && plane )
      plane->Flip();
  }
  return rc;
}

bool 
MYON_SurfaceProxy::IsPeriodic( int dir ) const
{
  if ( m_bTransposed ) {
    dir = (dir) ? 0 : 1;
  }
  return ( m_surface ) ? m_surface->IsPeriodic( dir ) : false;
}

bool MYON_SurfaceProxy::GetNextDiscontinuity( 
                int dir,
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
  // untested code
  bool rc = false;

  if ( 0 != m_surface && dir >= 0 && dir <= 1 )
  {
    rc = m_surface->GetNextDiscontinuity(m_bTransposed?1-dir:dir,c,t0,t1,t,hint,dtype,cos_angle_tolerance,curvature_tolerance);
  }

  return rc;
}

bool 
MYON_SurfaceProxy::IsSingular( int side ) const
{
  if ( m_bTransposed ) {
    switch(side) {
    case 0:
      side = 3;
      break;
    case 1:
      side = 2;
      break;
    case 2:
      side = 1;
      break;
    case 3:
      side = 0;
      break;
    }
  }
  return ( m_surface ) ? m_surface->IsSingular( side ) : false;
}

bool
MYON_SurfaceProxy::Reverse( 
    int // dir - formal parameter intentionally ignored in this virtual function
    )
{
  return false; // cannot modify m_surface
}

bool
MYON_SurfaceProxy::Transpose()
{
  DestroySurfaceTree();
  m_bTransposed = (m_bTransposed) ? false : true;
  return true;
}

bool MYON_SurfaceProxy::IsContinuous(
    MYON::continuity desired_continuity,
    double s, 
    double t, 
    int* hint, // default = nullptr,
    double point_tolerance, // default=MYON_ZERO_TOLERANCE
    double d1_tolerance, // default==MYON_ZERO_TOLERANCE
    double d2_tolerance, // default==MYON_ZERO_TOLERANCE
    double cos_angle_tolerance, // default==MYON_DEFAULT_ANGLE_TOLERANCE_COSINE
    double curvature_tolerance  // default==MYON_SQRT_EPSILON
    ) const
{
  bool rc = true;
  if ( m_surface )
  {
    if ( m_bTransposed )
    {
      double x = s;
      s = t;
      t = x;
    }
    rc = m_surface->IsContinuous( desired_continuity, s, t, hint,
                                      point_tolerance, d1_tolerance, d2_tolerance,
                                      cos_angle_tolerance, curvature_tolerance );
  }
  return rc;
}



bool 
MYON_SurfaceProxy::Evaluate( // returns false if unable to evaluate
       double s, double t, // evaluation parameters
       int der_count,  // number of derivatives (>=0)
       int v_stride,   // v[] array stride (>=Dimension())
       double* v,      // v[] array of length stride*(ndir+1)
       int side,       // optional - determines which side to evaluate from
                       //         0 = default
                       //      <  0 to evaluate from below, 
                       //      >  0 to evaluate from above
       int* hint       // optional - evaluation hint (int) used to speed
                       //            repeated evaluations
       ) const
{
  if ( m_bTransposed ) {
    double x = s; s = t; t = x;
  }
  return ( m_surface ) ? m_surface->Evaluate(s,t,der_count,v_stride,v,side,hint) : false;
}


MYON_Curve* MYON_SurfaceProxy::IsoCurve(
       int dir,
       double c
       ) const
{
  MYON_Curve* isocurve = 0;

  if ( m_bTransposed )
  {
    dir = 1-dir;
  }

  if ( 0 != m_surface && dir >= 0 && dir <= 1 )
  {
    isocurve = m_surface->IsoCurve( dir, c );
  }

  return isocurve;
}


int MYON_SurfaceProxy::GetNurbForm( // returns 0: unable to create NURBS representation
                   //            with desired accuracy.
                   //         1: success - returned NURBS parameterization
                   //            matches the surface's to wthe desired accuracy
                   //         2: success - returned NURBS point locus matches
                   //            the surfaces's to the desired accuracy but, on
                   //            the interior of the surface's domain, the 
                   //            surface's parameterization and the NURBS
                   //            parameterization may not match to the 
                   //            desired accuracy.
        MYON_NurbsSurface& nurbs,
        double tolerance
        ) const
{
  int rc = ( m_surface ) ? m_surface->GetNurbForm(nurbs,tolerance) : 0;
  if ( rc && m_bTransposed ) {
		if (!nurbs.Transpose())
			rc = 0;
  }
  return rc;
}

int 
MYON_SurfaceProxy::HasNurbForm( // returns 0: unable to create NURBS representation
                   //            with desired accuracy.
                   //         1: success - returned NURBS parameterization
                   //            matches the surface's to wthe desired accuracy
                   //         2: success - returned NURBS point locus matches
                   //            the surfaces's to the desired accuracy but, on
                   //            the interior of the surface's domain, the 
                   //            surface's parameterization and the NURBS
                   //            parameterization may not match to the 
                   //            desired accuracy.
        ) const

{
  if (!m_surface)
    return 0;
  return m_surface->HasNurbForm();
}

bool MYON_SurfaceProxy::GetSurfaceParameterFromNurbFormParameter(
      double nurbs_s, double nurbs_t,
      double* surface_s, double* surface_t
      ) const
{
  bool rc = false;
  if ( m_surface )
  {
    rc = m_bTransposed 
       ? m_surface->GetSurfaceParameterFromNurbFormParameter(nurbs_t,nurbs_s,surface_t,surface_s)
       : m_surface->GetSurfaceParameterFromNurbFormParameter(nurbs_s,nurbs_t,surface_s,surface_t);
  }
  return rc;
}

bool MYON_SurfaceProxy::GetNurbFormParameterFromSurfaceParameter(
      double surface_s, double surface_t,
      double* nurbs_s,  double* nurbs_t
      ) const
{
  bool rc = false;
  if ( m_surface )
  {
    rc = m_bTransposed 
       ? m_surface->GetNurbFormParameterFromSurfaceParameter(surface_t,surface_s,nurbs_t,nurbs_s)
       : m_surface->GetNurbFormParameterFromSurfaceParameter(surface_s,surface_t,nurbs_s,nurbs_t);
  }
  return rc;
}

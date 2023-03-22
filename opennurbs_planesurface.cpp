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

MYON_OBJECT_IMPLEMENT(MYON_PlaneSurface,MYON_Surface,"4ED7D4DF-E947-11d3-BFE5-0010830122F0");
MYON_OBJECT_IMPLEMENT(MYON_ClippingPlaneSurface,MYON_PlaneSurface,"DBC5A584-CE3F-4170-98A8-497069CA5C36");


MYON_PlaneSurface::MYON_PlaneSurface()
{}

MYON_PlaneSurface::MYON_PlaneSurface( const MYON_PlaneSurface& src )
{
  *this = src;
}

MYON_PlaneSurface& MYON_PlaneSurface::operator=( const MYON_PlaneSurface& src )
{
  if ( this != &src ) {
    MYON_Surface::operator=(src);
    m_plane = src.m_plane;
    m_domain[0] = src.m_domain[0];
    m_domain[1] = src.m_domain[1];
    m_extents[0] = src.m_extents[0];
    m_extents[1] = src.m_extents[1];
  }
  return *this;
}

MYON_PlaneSurface::MYON_PlaneSurface( const MYON_Plane& src )
{
  *this = src;
}


unsigned int MYON_PlaneSurface::SizeOf() const
{
  unsigned int sz = MYON_Surface::SizeOf();
  sz += (sizeof(*this) - sizeof(MYON_Surface));
  return sz;
}

MYON__UINT32 MYON_PlaneSurface::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_plane),&m_plane);
  current_remainder = MYON_CRC32(current_remainder,2*sizeof(m_domain[0]),&m_domain[0]);
  current_remainder = MYON_CRC32(current_remainder,2*sizeof(m_extents[0]),&m_extents[0]);
  return current_remainder;
}


MYON_PlaneSurface& MYON_PlaneSurface::operator=( const MYON_Plane& src )
{
  m_plane = src;
  m_domain[0].Set(0.0,1.0);
  m_domain[1].Set(0.0,1.0);
  m_extents[0] = m_domain[0];
  m_extents[1] = m_domain[1];
  return *this;
}

MYON_PlaneSurface::~MYON_PlaneSurface()
{}

bool MYON_PlaneSurface::IsValid( MYON_TextLog* text_log ) const
{
  return (   m_plane.IsValid() 
           && m_domain[0].IsIncreasing() && m_domain[1].IsIncreasing() 
           && m_extents[0].IsIncreasing() && m_extents[1].IsIncreasing() 
           ) ? true : false;
}

void
MYON_PlaneSurface::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_PlaneSurface\n");
}

bool 
MYON_PlaneSurface::Write(
       MYON_BinaryArchive& file  // open binary file
     ) const
{
  bool rc = file.Write3dmChunkVersion(1,1);

  // version 1.0 chunks
  if (rc)
    rc = file.WritePlane( m_plane );
  if (rc)
    rc = file.WriteInterval( m_domain[0] );
  if (rc)
    rc = file.WriteInterval( m_domain[1] );
  
  // added to version 1.1 chunks
  if (rc)
    rc = file.WriteInterval( m_extents[0] );
  if (rc)
    rc = file.WriteInterval( m_extents[1] );
  return rc;
}

bool 
MYON_PlaneSurface::Read(
       MYON_BinaryArchive& file // open binary file
     )
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc && major_version == 1) {
    // common to all 1.x formats
    if (rc)
      rc = file.ReadPlane( m_plane );
    if (rc)
      rc = file.ReadInterval( m_domain[0] );
    if (rc)
      rc = file.ReadInterval( m_domain[1] );
    m_extents[0] = m_domain[0];
    m_extents[1] = m_domain[1];
    if ( minor_version >= 1 )
    {
      if (rc)
        rc = file.ReadInterval( m_extents[0] );
      if (rc)
        rc = file.ReadInterval( m_extents[1] );
    }
  }
  return rc;
}

int 
MYON_PlaneSurface::Dimension() const
{
  return 3;
}

bool MYON_PlaneSurface::GetBBox( // returns true if successful
         double* boxmin,    // minimum
         double* boxmax,    // maximum
         bool bGrowBox
         ) const
{
  int i,j,k=0;
  MYON_3dPoint corner[4];
  for ( i = 0; i < 2; i++ ) for ( j = 0; j < 2; j++ ) {
    corner[k++] = PointAt( m_domain[0].m_t[i], m_domain[1].m_t[j] );
  }
  return MYON_GetPointListBoundingBox( 3, 0, 4, 3, 
                                     &corner[0].x, 
                                     boxmin, 
                                     boxmax, bGrowBox?true:false );
}

bool
MYON_PlaneSurface::Transform( const MYON_Xform& xform )
{
  TransformUserData(xform);
  MYON_3dPoint p = m_plane.origin + m_extents[0][0]*m_plane.xaxis + m_extents[1][0]*m_plane.yaxis;
  MYON_3dPoint q = m_plane.origin + m_extents[0][1]*m_plane.xaxis + m_extents[1][1]*m_plane.yaxis;
  bool rc = m_plane.Transform( xform )?true:false;
  if (rc && fabs(fabs(xform.Determinant())-1.0) > MYON_SQRT_EPSILON )
  {
    p = xform*p;
    q = xform*q;
    double x0, x1, y0, y1;
    rc = false;
    if ( m_plane.ClosestPointTo(p,&x0,&y0) && m_plane.ClosestPointTo(q,&x1,&y1) )
    {
      if ( x0 < x1 && y0 < y1 )
      {
        m_extents[0].Set(x0,x1);
        m_extents[1].Set(y0,y1);
        rc = true;
      }
    }
  }
  return rc;
}

MYON_Interval MYON_PlaneSurface::Domain( int dir ) const
{
  // evaluation domain - do not confuse with m_extents
  return dir ? m_domain[1] : m_domain[0];
}

int MYON_PlaneSurface::SpanCount( int dir ) const
{
  return 1;
}

bool MYON_PlaneSurface::GetSurfaceSize( 
    double* width, 
    double* height 
    ) const
{
  if ( width ) 
    *width = Extents(0).Length();
  if ( height ) 
    *height = Extents(1).Length();
  return true;
}


bool MYON_PlaneSurface::GetSpanVector( int dir, double* s ) const
{
  MYON_Interval d = Domain(dir);
  s[0] = d.Min();
  s[1] = d.Max();
  return d.IsIncreasing();
}

int MYON_PlaneSurface::Degree( int dir ) const
{
  return 1;
}

bool 
MYON_PlaneSurface::GetParameterTolerance(
         int dir,
         double t,  // t = parameter in domain
         double* tminus, // tminus
         double* tplus   // tplus
         ) const
{
  dir = (dir)?1:0;
  return MYON_GetParameterTolerance( m_domain[dir][0], m_domain[dir][1], t, tminus, tplus );
}

bool MYON_PlaneSurface::IsPlanar( MYON_Plane* plane, double tolerance ) const
{
  if ( plane )
    *plane = this->m_plane;
  return true;
}

bool 
MYON_PlaneSurface::IsClosed( int dir ) const
{
  return false;
}

bool 
MYON_PlaneSurface::IsPeriodic( int dir ) const
{
  return false;
}

bool 
MYON_PlaneSurface::IsSingular( int side ) const
{
  return false;
}

bool MYON_PlaneSurface::GetNextDiscontinuity( 
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
  return MYON_Surface::GetNextDiscontinuity(dir,c,t0,t1,t,hint,dtype,cos_angle_tolerance,curvature_tolerance);
}

bool
MYON_PlaneSurface::Reverse( int dir )
{
  if ( dir < 0 || dir > 1 )
    return false;
  m_extents[dir].Reverse();
  m_domain[dir].Reverse();
  if (dir)
    m_plane.yaxis = -m_plane.yaxis;
  else
    m_plane.xaxis = -m_plane.xaxis;
  m_plane.zaxis = -m_plane.zaxis;
  m_plane.UpdateEquation();
  return true;
}

bool MYON_PlaneSurface::IsContinuous(
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
  return true;
}

bool
MYON_PlaneSurface::Transpose()
{
  // swaps x and y axes and reverses zaxis
  m_plane.Flip();

  MYON_Interval i = m_domain[0];
  m_domain[0] = m_domain[1];
  m_domain[1] = i;

  i = m_extents[0];
  m_extents[0] = m_extents[1];
  m_extents[1] = i;

  return true;
}

bool 
MYON_PlaneSurface::Evaluate( // returns false if unable to evaluate
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
  double ds = 1.0;
  double dt = 1.0;
  if ( m_extents[0] != m_domain[0] )
  {
    s = m_extents[0].ParameterAt( m_domain[0].NormalizedParameterAt(s) );
    ds = m_extents[0].Length()/m_domain[0].Length();
  }
  if ( m_extents[1] != m_domain[1] )
  {
    t = m_extents[1].ParameterAt( m_domain[1].NormalizedParameterAt(t) );
    dt = m_extents[1].Length()/m_domain[1].Length();
  }
  MYON_3dPoint P = m_plane.PointAt( s, t );
  v[0] = P.x;
  v[1] = P.y;
  v[2] = P.z;
  v += v_stride;
  if ( der_count >= 1 ) 
  {
    v[0] = ds*m_plane.xaxis.x;
    v[1] = ds*m_plane.xaxis.y;
    v[2] = ds*m_plane.xaxis.z;
    v += v_stride;

    v[0] = dt*m_plane.yaxis.x;
    v[1] = dt*m_plane.yaxis.y;
    v[2] = dt*m_plane.yaxis.z;
    v += v_stride;

    if ( der_count > 1 ) 
    {
      // zero higher partials
      memset( v, 0, (((der_count+1)*(der_count+2)/2-4)*v_stride+3)*sizeof(*v) );
    }
  }
  return true;
}

MYON_Curve* MYON_PlaneSurface::IsoCurve( int dir, double c ) const
{
  MYON_LineCurve* line_curve = 0;
  if ( (dir == 0 || dir == 1) && IsValid() ) 
  {
    MYON_Line line;
    MYON_Interval domain = Domain(dir);
    if ( dir == 1 )
    {
      line.from = PointAt( c, domain[0] );
      line.to = PointAt( c, domain[1] );
    }
    else
    {
      line.from = PointAt( domain[0], c );
      line.to = PointAt( domain[1], c );
    }
    line_curve = new MYON_LineCurve(line);
    line_curve->m_dim = 3;
    line_curve->m_t = domain;
  }
  return line_curve;
}


bool MYON_PlaneSurface::Trim(
       int dir,
       const MYON_Interval& domain
       )
{
  if ( dir < 0 || dir > 1 )
    return false;
  MYON_Interval current_domain = Domain(dir);
  if ( current_domain[0] == MYON_UNSET_VALUE && current_domain[1] == MYON_UNSET_VALUE )
    current_domain = domain;
  MYON_Interval trim_domain, trim_extents = m_extents[dir];
  trim_domain.Intersection(domain, Domain(dir) );
  if ( !trim_domain.IsIncreasing() )
    return false;
  if ( m_domain[dir] == m_extents[dir] )
    trim_extents = trim_domain;
  else
  {
    double x0 = m_extents[dir].ParameterAt( m_domain[dir].NormalizedParameterAt( trim_domain[0] ) );
    double x1 = m_extents[dir].ParameterAt( m_domain[dir].NormalizedParameterAt( trim_domain[1] ) );
    trim_extents.Set(x0,x1);
  }
  if ( !trim_extents.IsIncreasing() )
    return false;
  m_extents[dir] = trim_extents;
  m_domain[dir] = trim_domain;
  return true;
}

bool MYON_PlaneSurface::Extend(
      int dir,
      const MYON_Interval& domain
      )
{
  if ( dir < 0 || dir > 1 ) return false;
  bool changed = false;
  MYON_Interval tdom = Domain(dir);
  MYON_Interval xdom = m_extents[dir];

  if (domain[0] < Domain(dir)[0]){
    changed = true;
    tdom[0] = domain[0];
    xdom[0] = m_extents[dir].ParameterAt( m_domain[dir].NormalizedParameterAt(domain[0]));
  }
  if (domain[1] > Domain(dir)[1]){
    changed = true;
    tdom[1] = domain[1];
    xdom[1] = m_extents[dir].ParameterAt( m_domain[dir].NormalizedParameterAt(domain[1]));
  }
  if (!changed) return false;
  DestroySurfaceTree();

  m_domain[dir] = tdom;
  m_extents[dir] = xdom;
  return true;
}

bool MYON_PlaneSurface::Split(
       int dir,
       double c,
       MYON_Surface*& west_or_south_side,
       MYON_Surface*& east_or_north_side
       ) const
{
  MYON_PlaneSurface* ws_side = 0;
  MYON_PlaneSurface* en_side = 0;

  if ( dir < 0 || dir > 1 )
    return false;
  if ( !Domain(dir).Includes(c,true) )
    return false;

  double t;
  if ( Domain(dir) == Extents(dir) )
    t = c;
  else
  {
    t = Extents(dir).ParameterAt( Domain(dir).NormalizedParameterAt(c) );
    if ( !Extents(dir).Includes(t,true) )
      return false;
  }

  if ( west_or_south_side )
  {
    if ( west_or_south_side == east_or_north_side )
      return false;
    ws_side = MYON_PlaneSurface::Cast(west_or_south_side);
    if ( !ws_side )
      return false;
  }

  if ( east_or_north_side )
  {
    en_side = MYON_PlaneSurface::Cast(east_or_north_side);
    if ( !en_side )
      return false;
  }

  if ( !ws_side )
    ws_side = new MYON_PlaneSurface();
  if ( !en_side )
    en_side = new MYON_PlaneSurface();

  *ws_side = *this;
  *en_side = *this;
  ws_side->m_domain[dir].m_t[1] = c;
  en_side->m_domain[dir].m_t[0] = c;
  ws_side->m_extents[dir].m_t[1] = t;
  en_side->m_extents[dir].m_t[0] = t;

  west_or_south_side = ws_side;
  east_or_north_side = en_side;

  return true;
}


int MYON_PlaneSurface::GetNurbForm( // returns 0: unable to create NURBS representation
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
  bool rc = IsValid();

  if( !rc )
  {
    if (    m_plane.origin.x != MYON_UNSET_VALUE 
         && m_plane.xaxis.x != MYON_UNSET_VALUE 
         && m_plane.yaxis.x != MYON_UNSET_VALUE
         && m_domain[0].IsIncreasing() && m_domain[1].IsIncreasing()
         && m_extents[0].Length() > 0.0 && m_extents[1].Length() > 0.0
         )
    {
      MYON_3dVector N = MYON_CrossProduct(m_plane.xaxis,m_plane.yaxis);
      if ( N.Length() <= 1.0e-4 )
      {
        MYON_WARNING("MYON_PlaneSurface::GetNurbForm - using invalid surface.");
        rc = true;
      }
    }
  }

  if ( rc ) 
  {
    nurbs.m_dim = 3;
    nurbs.m_is_rat = 0;
    nurbs.m_order[0] = nurbs.m_order[1] = 2;
    nurbs.m_cv_count[0] = nurbs.m_cv_count[1] = 2;
    nurbs.m_cv_stride[1] = nurbs.m_dim;
    nurbs.m_cv_stride[0] = nurbs.m_cv_stride[1]*nurbs.m_cv_count[1];
    nurbs.ReserveCVCapacity(12);
    nurbs.ReserveKnotCapacity(0,2);
    nurbs.ReserveKnotCapacity(1,2);
    nurbs.m_knot[0][0] = m_domain[0][0];
    nurbs.m_knot[0][1] = m_domain[0][1];
    nurbs.m_knot[1][0] = m_domain[1][0];
    nurbs.m_knot[1][1] = m_domain[1][1];
    nurbs.SetCV( 0, 0, PointAt( m_domain[0][0], m_domain[1][0] ));
    nurbs.SetCV( 0, 1, PointAt( m_domain[0][0], m_domain[1][1] ));
    nurbs.SetCV( 1, 0, PointAt( m_domain[0][1], m_domain[1][0] ));
    nurbs.SetCV( 1, 1, PointAt( m_domain[0][1], m_domain[1][1] ));
  }

  return rc;
}

int 
MYON_PlaneSurface::HasNurbForm( // returns 0: unable to create NURBS representation
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
  if (!IsValid())
    return 0;
  return 1;
}

bool MYON_PlaneSurface::SetExtents( 
       int dir,
       MYON_Interval extents,
       bool bSyncDomain
       )
{
  if ( dir < 0 || dir > 1 || !extents.IsIncreasing() )
    return false;
  m_extents[dir] = extents;
  if ( bSyncDomain )
    m_domain[dir] = m_extents[dir];
  return true;
}

MYON_Interval MYON_PlaneSurface::Extents(
       int dir
       ) const
{
  // rectangle extents - do not confuse with m_domain
  return dir ? m_extents[1] : m_extents[0];
}

bool MYON_PlaneSurface::CreatePseudoInfinitePlane( 
        MYON_PlaneEquation plane_equation,
        const MYON_BoundingBox& bbox,
        double padding
        )
{

  MYON_Plane plane(&plane_equation.x);
  return CreatePseudoInfinitePlane(plane,bbox,padding);
}

bool MYON_PlaneSurface::CreatePseudoInfinitePlane( 
        const MYON_Plane& plane,
        const MYON_BoundingBox& bbox,
        double padding
        )
{
  MYON_3dPoint bbox_corners[8];
  if ( !bbox.GetCorners(bbox_corners) )
    return false;
  return CreatePseudoInfinitePlane(plane,8,bbox_corners,padding);
}


bool MYON_PlaneSurface::CreatePseudoInfinitePlaneTight(
  const MYON_Plane& plane,
  const MYON_BoundingBox& bbox,
  double padding
)
{
  if (!plane.IsValid() || !bbox.IsValid())
    return false;

  MYON_Interval uext(MYON_DBL_PINF, MYON_DBL_NINF);
  MYON_Interval vext(MYON_DBL_PINF, MYON_DBL_NINF);
  double t, u, v;
  for (int i = 0; i < 12; i++)
  {
    MYON_Line edge = bbox.Edge(i);
    if (!MYON_Intersect(edge, plane.plane_equation, &t)) continue;
    if (t < 0.0 || t > 1.0) continue;
    MYON_3dPoint pt = edge.PointAt(t);
    plane.ClosestPointTo(pt, &u, &v);
    if (u < uext[0]) uext[0] = u;
    else if (u > uext[1]) uext[1] = u;
    if (v < vext[0]) vext[0] = v;
    else if (v > vext[1]) vext[1] = v;
  }
  *this = plane;
  uext.Expand(padding * uext.Length() + padding);
  vext.Expand(padding * vext.Length() + padding);
  SetExtents(0, uext, true);
  SetExtents(1, vext, true);
  return true;
}


bool MYON_PlaneSurface::CreatePseudoInfinitePlaneTight(
  const MYON_PlaneEquation& plane_equation,
  const MYON_BoundingBox& bbox,
  double padding
)
{
  MYON_Plane plane(plane_equation);
  MYON_3dPoint center = plane.ClosestPointTo(bbox.Center());
  plane.origin = center;
  return CreatePseudoInfinitePlaneTight(plane, bbox, padding);
}


bool MYON_PlaneSurface::CreatePseudoInfinitePlane( 
        const MYON_Plane& plane,
        int point_count,
        const MYON_3dPoint* point_list,
        double padding
        )
{
  if ( !plane.IsValid() )
    return false;
  if ( point_count < 1 )
    return false;
  if ( 0 == point_list )
    return false;
  if ( !MYON_IsValid(padding) || padding < 0.0 )
    return false;

  MYON_Interval plane_domain[2];
  double s, t;
  s = MYON_UNSET_VALUE;
  t = MYON_UNSET_VALUE;
  if ( !plane.ClosestPointTo( point_list[0], &s, &t ) || !MYON_IsValid(s) || !MYON_IsValid(t) )
    return 0;
  plane_domain[0].m_t[1] = plane_domain[0].m_t[0] = s;
  plane_domain[1].m_t[1] = plane_domain[1].m_t[0] = t;
  
  for ( int i = 1; i < point_count; i++ )
  {
    s = MYON_UNSET_VALUE;
    t = MYON_UNSET_VALUE;
    if ( !plane.ClosestPointTo( point_list[i], &s, &t ) || !MYON_IsValid(s) || !MYON_IsValid(t) )
      return 0;
    if ( s < plane_domain[0].m_t[0] ) plane_domain[0].m_t[0] = s; else if ( s > plane_domain[0].m_t[1] ) plane_domain[0].m_t[1] = s;
    if ( t < plane_domain[1].m_t[0] ) plane_domain[1].m_t[0] = t; else if ( t > plane_domain[1].m_t[1] ) plane_domain[1].m_t[1] = t;
  }

  s = padding*plane_domain[0].Length() + padding;
  if ( !(s > 0.0) && !plane_domain[0].IsIncreasing() )
    s = 1.0;
  plane_domain[0].m_t[0] -= s;
  plane_domain[0].m_t[1] += s;

  t = padding*plane_domain[1].Length() + padding;
  if ( !(t > 0.0) && !plane_domain[1].IsIncreasing() )
    t = 1.0;
  plane_domain[1].m_t[0] -= t;
  plane_domain[1].m_t[1] += t;

  m_plane = plane;
  m_domain[0] = plane_domain[0];
  m_domain[1] = plane_domain[1];
  m_extents[0] = plane_domain[0];
  m_extents[1] = plane_domain[1];

  return IsValid()?true:false;
}

bool MYON_PlaneSurface::SetDomain( 
  int dir, 
  double t0, 
  double t1
  )
{
  bool rc = false;
  if ( dir >= 0 && dir <= 1 && t0 < t1 )
  {
    rc = true;
    m_domain[dir].Set(t0,t1);
    DestroySurfaceTree();
  }
  return rc;
}

MYON_Mesh* MYON_PlaneSurface::CreateMesh(
  MYON_Mesh* mesh
) const
{
  MYON_Mesh* rc = nullptr;

  if (m_plane.IsValid() && m_domain[0].IsValid() && m_domain[1].IsValid())
  {
    bool vn = false, fn = false, sp = true;

    if (mesh == nullptr) mesh = new MYON_Mesh();
    else
    {
      if (!mesh->HasDoublePrecisionVertices()) mesh->UpdateDoublePrecisionVertices();
      if (mesh->HasVertexNormals()) vn = true;
      if (mesh->HasFaceNormals()) fn = true;
      if (mesh->HasMeshTopology()) mesh->DestroyTopology();
    }

    mesh->m_dV.Append(PointAt(m_domain[0][0], m_domain[1][0]));
    mesh->m_dV.Append(PointAt(m_domain[0][1], m_domain[1][0]));
    mesh->m_dV.Append(PointAt(m_domain[0][1], m_domain[1][1]));
    mesh->m_dV.Append(PointAt(m_domain[0][0], m_domain[1][1]));
    mesh->UpdateSinglePrecisionVertices();

    {
      MYON_MeshFace mf;
      mf.vi[0] = mesh->VertexCount() - 4;
      mf.vi[1] = mesh->VertexCount() - 3;
      mf.vi[2] = mesh->VertexCount() - 2;
      mf.vi[3] = mesh->VertexCount() - 1;
      mesh->m_F.Append(mf);
    }

    if (vn)
    {
      mesh->m_N.Append((MYON_3fVector)m_plane.Normal());
      mesh->m_N.Append((MYON_3fVector)m_plane.Normal());
      mesh->m_N.Append((MYON_3fVector)m_plane.Normal());
      mesh->m_N.Append((MYON_3fVector)m_plane.Normal());
    }

    if (fn)
    {
      mesh->m_FN.Append((MYON_3fVector)m_plane.Normal());
    }

    if (sp)
    {
      mesh->m_S.Append(MYON_2dPoint(m_domain[0][0], m_domain[1][0]));
      mesh->m_S.Append(MYON_2dPoint(m_domain[0][1], m_domain[1][0]));
      mesh->m_S.Append(MYON_2dPoint(m_domain[0][1], m_domain[1][1]));
      mesh->m_S.Append(MYON_2dPoint(m_domain[0][0], m_domain[1][1]));
    }

    rc = mesh;
  }
  return rc;
}

static double MYON_ClippingPlaneDistanceHelper(float d)
{
  if (MYON_IsValidFloat(d))
    return d;
  return MYON_DBL_MAX;
}

static void MYON_ClippingPlaneSetDistanceHelper(double distance, float& valueToChange)
{
  if (MYON_IsValid(distance) && distance < MYON_UNSET_POSITIVE_FLOAT)
    valueToChange = (float)distance;
  else
    valueToChange = MYON_UNSET_POSITIVE_FLOAT;
}

double MYON_ClippingPlaneInfo::Distance() const
{
  return MYON_ClippingPlaneDistanceHelper(m_distance);
}
void MYON_ClippingPlaneInfo::SetDistance(double distance)
{
  MYON_ClippingPlaneSetDistanceHelper(distance, m_distance);
}

void MYON_ClippingPlaneInfo::Default()
{
  memset(this,0,sizeof(*this));
  m_distance = MYON_UNSET_POSITIVE_FLOAT;
}

bool MYON_ClippingPlaneInfo::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,2);
  if (!rc)
    return false;
  
  for(;;)
  {
    rc = file.WritePlaneEquation(m_plane_equation);
    if (!rc) break;

    rc = file.WriteUuid(m_plane_id);
    if (!rc) break;

    rc = file.WriteBool(m_bEnabled);
    if (!rc) break;

    // version 1.1 and 1.2 - write distance as a double
    // In version 1.1 we wrote -1 as the default value for m_distance.
    // This default has changed to MYON_UNSET_POSITIVE_FLOAT. Bumping the minor
    // version so we can properly handle the default case when reading.
    rc = file.WriteDouble(m_distance);
    if (!rc) break;

    break;
  }

  if ( !file.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_ClippingPlaneInfo::Read( MYON_BinaryArchive& file )
{
  Default();

  int major_version = 0;
  int minor_version = 0;
  bool rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;
  
  for(;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    rc = file.ReadPlaneEquation(m_plane_equation);
    if (!rc) break;

    rc = file.ReadUuid(m_plane_id);
    if (!rc) break;

    rc = file.ReadBool(&m_bEnabled);
    if (!rc) break;

    if (minor_version > 0)
    {
      double d = -1;
      rc = file.ReadDouble(&d);
      if (!rc) break;

      if (1 == minor_version)
      {
        // negative values in 1.1 chunk meant that the distance was unset
        if (d < 0.0)
          d = MYON_UNSET_VALUE;
      }

      SetDistance(d);
    }

    break;
  }

  if ( !file.EndRead3dmChunk() )
    rc = false;

  return rc;
}


void MYON_ClippingPlane::Default()
{
  m_plane = MYON_xy_plane;
  m_viewport_ids.Empty();
  m_plane_id = MYON_nil_uuid;
  m_bEnabled = true;
  m_distance = MYON_UNSET_POSITIVE_FLOAT;
}

MYON_ClippingPlane::MYON_ClippingPlane()
{
  Default();
}

MYON_ClippingPlane::~MYON_ClippingPlane()
{
}

MYON_ClippingPlaneInfo MYON_ClippingPlane::ClippingPlaneInfo() const
{
  MYON_ClippingPlaneInfo info;
  info.m_plane_equation = m_plane.plane_equation;
  info.m_plane_id = m_plane_id;
  info.m_bEnabled = m_bEnabled;
  info.SetDistance(m_distance);
  return info;
}

double MYON_ClippingPlane::Distance() const
{
  return MYON_ClippingPlaneDistanceHelper(m_distance);
}
void MYON_ClippingPlane::SetDistance(double distance)
{
  MYON_ClippingPlaneSetDistanceHelper(distance, m_distance);
}

bool MYON_ClippingPlane::Read( MYON_BinaryArchive& file )
{
  Default();

  int major_version = 0;
  int minor_version = 0;
  
  bool rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    MYON_UUID viewport_id;
    rc = file.ReadUuid(viewport_id);
    if(!rc) break;

    if( 0 == minor_version )
      m_viewport_ids.AddUuid( viewport_id );

    rc = file.ReadUuid(m_plane_id);
    if (!rc) break;

    rc = file.ReadPlane(m_plane);
    if (!rc) break;

    rc = file.ReadBool(&m_bEnabled);
    if (!rc) break;

    if( minor_version > 0 )
    {
      rc = m_viewport_ids.Read(file);
      if (!rc) break;
    }

    if (minor_version > 1)
    {
      double d = -1;
      rc = file.ReadDouble(&d);
      if (!rc) break;

      if (2 == minor_version)
      {
        if (d < 0.0)
          d = MYON_UNSET_VALUE;
      }
      SetDistance(d);
    }

    break;
  }

  if ( !file.EndRead3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_ClippingPlane::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,3);
  if (!rc)
    return false;

  for(;;)
  {
    //version 1.1 - write list of viewport uuids instead of single uuid
    MYON_UUID viewport_id = ::MYON_nil_uuid;
    if( m_viewport_ids.Count() > 0 )
      viewport_id = *(m_viewport_ids.Array());
    rc = file.WriteUuid(viewport_id);
    if (!rc) break;

    rc = file.WriteUuid(m_plane_id);
    if (!rc) break;

    rc = file.WritePlane(m_plane);
    if (!rc) break;

    rc = file.WriteBool(m_bEnabled);
    if (!rc) break;

    //version 1.1 - write list of viewport uuids instead of single uuid
    rc = m_viewport_ids.Write(file);
    if (!rc) break;

    //version 1.2 - write distance as double
    //version 1.3 - continue to write distance, but the reader now knows to
    //              interpret the distance value in a different way
    rc = file.WriteDouble(m_distance);
    if (!rc) break;

    break;
  }

  if ( !file.EndWrite3dmChunk() )
    rc = false;

  return rc;
}


void MYON_ClippingPlaneSurface::Default()
{
  m_clipping_plane.Default();
  m_plane = m_clipping_plane.m_plane;
  m_domain[0].Set(0.0,1.0);
  m_domain[1].Set(0.0,1.0);
  m_extents[0].Set(-1.0,1.0);
  m_extents[1].Set(-1.0,1.0);
}


MYON::object_type MYON_ClippingPlaneSurface::ObjectType() const
{
  return MYON::clipplane_object;
}

MYON_ClippingPlaneSurface::MYON_ClippingPlaneSurface()
{
  Default();
}

MYON_ClippingPlaneSurface::~MYON_ClippingPlaneSurface()
{
}

MYON_ClippingPlaneSurface::MYON_ClippingPlaneSurface(const MYON_PlaneSurface& src)
                        : MYON_PlaneSurface(src)
{
  m_clipping_plane.m_plane = m_plane;
}

MYON_ClippingPlaneSurface::MYON_ClippingPlaneSurface(const MYON_Plane& src)
                        : MYON_PlaneSurface(src)
{
  m_clipping_plane.m_plane = m_plane;
}

MYON_ClippingPlaneSurface& MYON_ClippingPlaneSurface::operator=(const MYON_Plane& src)
{
  m_plane = src;
  m_clipping_plane.m_plane = m_plane;
  return *this;
}

MYON_ClippingPlaneSurface& MYON_ClippingPlaneSurface::operator=(const MYON_PlaneSurface& src)
{
  if ( this != &src )
  {
    MYON_PlaneSurface::operator=(src);
    m_clipping_plane.m_plane = m_plane;
  }
  return *this;
}

unsigned int MYON_ClippingPlaneSurface::SizeOf() const
{
  return MYON_PlaneSurface::SizeOf() + sizeof(m_clipping_plane);
}

MYON__UINT32 MYON_ClippingPlaneSurface::DataCRC(MYON__UINT32 current_remainder) const
{
  MYON__UINT32 crc = MYON_PlaneSurface::DataCRC(current_remainder);
  crc = MYON_CRC32(crc,sizeof(m_clipping_plane),&m_clipping_plane);
  return crc;
}

void MYON_ClippingPlaneSurface::Dump( MYON_TextLog& text_log ) const
{
  text_log.Print("Clipping plane surface\n");
  text_log.PushIndent();  
  text_log.Print("Enabled = %d",m_clipping_plane.m_bEnabled);
  text_log.Print("View IDs =\n");
  {
    text_log.PushIndent();
    MYON_SimpleArray<MYON_UUID> uuid_list;
    m_clipping_plane.m_viewport_ids.GetUuids(uuid_list);
    for( int i=0; i<uuid_list.Count(); i++ )
    {
      text_log.Print( uuid_list[i] );
      text_log.Print("\n");
    }
    text_log.PopIndent();
  }
  text_log.Print("Plane ID = ");
  text_log.Print(m_clipping_plane.m_plane_id);
  text_log.Print("\n");  

  text_log.Print("Plane surface\n");
  text_log.PushIndent();  
  MYON_PlaneSurface::Dump(text_log);
  text_log.PopIndent();  
  text_log.PopIndent();  
}

bool MYON_ClippingPlaneSurface::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if (!rc)
    return false;

  for(;;)
  {
    rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,0);
    if (rc)
    {
      rc = MYON_PlaneSurface::Write(file)?true:false;
      if (!file.EndWrite3dmChunk())
        rc = false;
    }
    if (!rc) break;

    rc = m_clipping_plane.Write(file);
    if (rc) break;

    break;
  }

  if (!file.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_ClippingPlaneSurface::Read( MYON_BinaryArchive& file )
{
  Default();

  int major_version = 0;
  int minor_version = 0;

  bool rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    rc =  ( 1 == major_version );
    if (!rc) break;

    MYON__UINT32 tcode = 0;
    MYON__INT64 big_value = 0;

    rc = file.BeginRead3dmBigChunk(&tcode,&big_value)?true:false;
    if (rc)
    {
      rc = (TCODE_ANONYMOUS_CHUNK == tcode);
      if (rc)
        rc = (MYON_PlaneSurface::Read(file)?true:false);
      if (!file.EndRead3dmChunk())
        rc = false;
    }
    if (!rc) break;

    rc = m_clipping_plane.Read(file);
    if (rc) break;

    break;
  }

  if (!file.EndRead3dmChunk() )
    rc = false;

  return rc;
}



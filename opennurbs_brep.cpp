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

#include <memory>
#include <array>

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif




////////////////////////////////////////////////////////////////
//   Class MYON_BrepVertex
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT_NO_COPYCTOR(MYON_BrepVertex,MYON_Point,"60B5DBC0-E660-11d3-BFE4-0010830122F0");

static bool MYON_BrepIsNotValid()
{
  return MYON_IsNotValid(); // <-- good place for a breakpoint
}

//bool MYON_Brep::GetLock()
//{
//  return m_sleep_lock.GetLock();
//}
//
//bool MYON_Brep::GetLockOrReturnFalse()
//{
//  return m_sleep_lock.GetLockOrReturnFalse();
//}
//
//bool MYON_Brep::ReturnLock()
//{
//  return m_sleep_lock.ReturnLock();
//}

MYON_SleepLockGuard::MYON_SleepLockGuard(const class MYON_Brep& brep)
  : m_sleep_lock(brep.m_sleep_lock)
{
  m_bIsManagingLock = m_sleep_lock.GetLock();
}

MYON_BrepVertex::MYON_BrepVertex()
{
  memset(&m_vertex_user,0,sizeof(m_vertex_user));
}

MYON_BrepVertex::MYON_BrepVertex( int vertex_index ) 
  : m_vertex_index(vertex_index)
{
  memset(&m_vertex_user,0,sizeof(m_vertex_user));
}

unsigned int MYON_BrepVertex::SizeOf() const
{
  unsigned int sz = MYON_Geometry::SizeOf();
  sz += (sizeof(*this) - sizeof(MYON_Geometry));
  sz += m_ei.SizeOfArray();
  return sz;
}

MYON_BrepVertex& MYON_BrepVertex::operator=(const MYON_BrepVertex& src)
{
  if ( &src != this ) {
    MYON_Point::operator=(src);
    m_vertex_user   = src.m_vertex_user;
    m_status        = src.m_status;
    m_vertex_index  = src.m_vertex_index;
    m_ei            = src.m_ei;
    m_tolerance     = src.m_tolerance;
  }
  return *this;
}

bool MYON_BrepVertex::IsValid( MYON_TextLog* text_log ) const
{
  if (m_vertex_index < 0)
  {
    if ( text_log )
      text_log->Print("MYON_BrepVertex m_vertex_index = %d.  Should be >= 0\n",m_vertex_index);
    return MYON_BrepIsNotValid();
  }
  const int ve_count = EdgeCount();
  int vei, ei;
  for ( vei = 0; vei < ve_count; vei++ ) {
    ei = m_ei[vei];
    if ( ei < 0 )
    {
      if ( text_log )
        text_log->Print("MYON_BrepVertex m_ei[%d] = %d.  m_ei[] values should be >= 0\n",vei,ei);
      return MYON_BrepIsNotValid();
    }
  }
  return MYON_Point::IsValid(text_log);
}

void 
MYON_BrepVertex::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_BrepVertex[%d]: ",m_vertex_index);
  dump.Print( point );
  dump.Print("\n");
}


bool
MYON_BrepVertex::SetPoint( const MYON_3dPoint& p )
{
  point = p;
  m_tolerance = MYON_UNSET_VALUE;
  return true;
}

MYON_3dPoint
MYON_BrepVertex::Point() const
{
  return point;
}

double
MYON_BrepVertex::Tolerance() const
{
  return m_tolerance;
}

int
MYON_BrepVertex::EdgeCount() const
{
  return m_ei.Count();
}

////////////////////////////////////////////////////////////////
//   Class MYON_BrepEdge
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT_NO_COPYCTOR(MYON_BrepEdge,MYON_CurveProxy,"60B5DBC1-E660-11d3-BFE4-0010830122F0");

MYON_BrepEdge::MYON_BrepEdge() 
  : MYON_CurveProxy(0)
{
  memset(&m_edge_user,0,sizeof(m_edge_user));
  m_vi[0] = m_vi[1] = -1;
}

MYON_BrepEdge::MYON_BrepEdge(int edge_index ) 
  : MYON_CurveProxy(0),
    m_edge_index(edge_index)
{
  memset(&m_edge_user,0,sizeof(m_edge_user));
  m_vi[0] = m_vi[1] = -1;
}

MYON::object_type MYON_BrepEdge::ObjectType() const
{
  // This MUST return MYON::curve_object.
  // NEVER change this to MYON::edge_object.
  return MYON::curve_object;
}

unsigned int MYON_BrepEdge::SizeOf() const
{
  unsigned int sz = MYON_CurveProxy::SizeOf();
  sz = (sizeof(*this) - sizeof(MYON_CurveProxy));
  sz += m_ti.SizeOfArray();
  return sz;
}

MYON_BrepEdge& MYON_BrepEdge::operator=(const MYON_BrepEdge& src)
{
  if ( &src != this ) 
  {
    // do not copy m_brep pointer
    MYON_CurveProxy::operator=(src);
    m_edge_user   = src.m_edge_user;
    m_status      = src.m_status;
    m_edge_index  = src.m_edge_index;
    m_c3i         = src.m_c3i;
    m_vi[0]       = src.m_vi[0];
    m_vi[1]       = src.m_vi[1];
    m_ti          = src.m_ti;
    m_tolerance   = src.m_tolerance;
  }
  return *this;
}

bool MYON_BrepEdge::IsValid( MYON_TextLog* text_log ) const
{
  bool rc = MYON_CurveProxy::IsValid(text_log) ? true : false;

  if ( !rc )
  {
    if ( text_log )
    {
      text_log->Print("MYON_BrepEdge is not a valid curve proxy\n");
    }
  }
  else if (m_edge_index < 0)
  {
    if ( text_log )
    {
      text_log->Print("MYON_BrepEdge.m_edge_index = %d (should be >= 0 )\n",m_edge_index);
    }
    rc = false;
  }
  else if ( m_c3i < 0 )
  {
    if ( text_log )
    {
      text_log->Print("MYON_BrepEdge.m_c3i = %d (should be >= 0 )\n",m_c3i);
    }
    rc = false;
  }
  else if ( m_vi[0] < 0 )
  {
    if ( text_log )
    {
      text_log->Print("MYON_BrepEdge.m_vi[0] = %d (should be >= 0 )\n",m_vi[0]);
    }
    rc = false;
  }
  else if ( m_vi[1] < 0 )
  {
    if ( text_log )
    {
      text_log->Print("MYON_BrepEdge.m_vi[1] = %d (should be >= 0 )\n",m_vi[1]);
    }
    rc = false;
  }
  else if ( !m_brep )
  {
    if ( text_log )
    {
      text_log->Print("MYON_BrepEdge.m_brep = nullptr (should point to parent MYON_Brep)\n");
    }
    rc = false;
  }

  return rc;

}

bool MYON_BrepEdge::IsClosed() const
{
  // This function must return true if MYON_CurveProxy::IsClosed() is true.
  bool rc = MYON_CurveProxy::IsClosed();
  if ( 0 == rc 
       && m_vi[0] >= 0 
       && m_vi[0] == m_vi[1]
       && 0 != ProxyCurve()
       && ProxyCurveDomain() == ProxyCurve()->Domain()
       && 0 != m_brep
       && m_vi[0] < m_brep->m_V.Count()
       )
  {
    // When MYON_CurveProxy::IsClosed() is false and the topology
    // indicates the edge is closed, we need to verify that its
    // geometry is within tolerance of being closed.
    const MYON_BrepVertex& v = m_brep->m_V[m_vi[0]];
    MYON_3dPoint P = PointAtStart();
    MYON_3dPoint Q = PointAtEnd();
    MYON_3dPoint V = v.point;
    double vtx_tol = v.m_tolerance;
    if ( P.DistanceTo(Q) <= m_tolerance 
         && V.DistanceTo(P) <= vtx_tol
         && V.DistanceTo(Q) <= vtx_tol )
      rc = true;
  }
  return rc;
}

MYON_Brep* MYON_BrepEdge::Brep() const
{
  return m_brep;
}

MYON_BrepTrim* MYON_BrepEdge::Trim( int eti ) const
{
  return (m_brep && eti >= 0 && eti < m_ti.Count()) ? m_brep->Trim(m_ti[eti]) : 0;
}

int MYON_BrepEdge::TrimCount() const
{
  return m_ti.Count();
}

void MYON_BrepEdge::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_BrepEdge[%d]: ",m_edge_index);
}


// virtual MYON_Curve::Reverse override
bool MYON_BrepEdge::Reverse()
{
  bool rc = false;
  if ( m_brep )
  {
    MYON_Interval edge_domain = Domain();
    if ( m_brep->StandardizeEdgeCurve( m_edge_index, false ) )
    {
      MYON_Curve* c3 = const_cast<MYON_Curve*>(EdgeCurveOf());
      if ( c3 )
      {
        rc = c3->Reverse();
        edge_domain.Reverse();
        c3->SetDomain(edge_domain);
        SetProxyCurve(c3);
      }
    }
  }

  if ( !rc )
    rc = MYON_CurveProxy::Reverse();

  if (rc)
  {
    int i = m_vi[0];
    m_vi[0] = m_vi[1];
    m_vi[1] = i;
    if ( m_brep )
    {
      const int tcount = m_brep->m_T.Count();
      int ti, eti;
      for ( eti = m_ti.Count()-1; eti >= 0; eti-- ) {
        ti = m_ti[eti];
        if ( ti >= 0 && ti < tcount ) 
        {
          MYON_BrepTrim& trim = m_brep->m_T[ti];
          trim.m_bRev3d = trim.m_bRev3d ? false : true;
          trim.UnsetPlineEdgeParameters();
        }
      }
    }
  }

  return rc;
}


////////////////////////////////////////////////////////////////
//   Class MYON_BrepTrim
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT_NO_COPYCTOR(MYON_BrepTrim,MYON_CurveProxy,"60B5DBC2-E660-11d3-BFE4-0010830122F0");

MYON_BrepTrim::MYON_BrepTrim()
{
  memset(&m_trim_user,0,sizeof(m_trim_user));
  m_vi[0] = m_vi[1] = -1; 
  m_tolerance[0] = m_tolerance[1] = MYON_UNSET_VALUE;
  m_pline.Reserve(4); // This is a stopgap fix to insures the memory 
                      // pool used for pline segments is the same as
                      // the memory pool used for the rest of this brep.
  //m_P[0] = MYON_3dPoint::UnsetPoint;
  //m_P[1] = MYON_3dPoint::UnsetPoint;
}

MYON_BrepTrim::MYON_BrepTrim(int trim_index) 
  : m_trim_index(trim_index)
{
  memset(&m_trim_user,0,sizeof(m_trim_user));
  m_vi[0] = m_vi[1] = -1; 
  m_tolerance[0] = m_tolerance[1] = MYON_UNSET_VALUE;
  m_pline.Reserve(4); // This is a stopgap fix to insures the memory 
                      // pool used for pline segments is the same as
                      // the memory pool used for the rest of this brep.
  //m_P[0] = MYON_3dPoint::UnsetPoint;
  //m_P[1] = MYON_3dPoint::UnsetPoint;
}


unsigned int MYON_BrepTrim::SizeOf() const
{
  unsigned int sz = MYON_CurveProxy::SizeOf();
  sz = (sizeof(*this) - sizeof(MYON_CurveProxy));
  // runtime m_pline is not counted
  return sz;
}


MYON_BrepTrim& MYON_BrepTrim::operator=(const MYON_BrepTrim& src)
{
  if ( &src != this ) 
  {
    // do not copy m_brep pointer
    MYON_CurveProxy::operator=(src);
    m_trim_user   = src.m_trim_user;
    m_status      = src.m_status;
    m_trim_index  = src.m_trim_index;
    m_c2i    = src.m_c2i;
    //m_t      = src.m_t;
    m_ei     = src.m_ei;
    m_vi[0]  = src.m_vi[0];
    m_vi[1]  = src.m_vi[1];
    m_bRev3d = src.m_bRev3d;
    m_type   = src.m_type;
    m_iso    = src.m_iso;
    m_li     = src.m_li;
    m_tolerance[0] = src.m_tolerance[0];
    m_tolerance[1] = src.m_tolerance[1];
    //m_P[0] = src.m_P[0];
    //m_P[1] = src.m_P[1];
    m__legacy_2d_tol = src.m__legacy_2d_tol;
    m__legacy_3d_tol = src.m__legacy_3d_tol;
    m__legacy_flags = src.m__legacy_flags;
    m_pline = src.m_pline;
    m_pbox = src.m_pbox;
  }
  return *this;
}

MYON_Brep* MYON_BrepTrim::Brep() const
{
  return m_brep;
}

MYON_BrepLoop* MYON_BrepTrim::Loop() const
{
  MYON_BrepLoop* loop = 0;
  if ( m_brep && m_li >= 0 && m_li < m_brep->m_L.Count() )
    loop = &m_brep->m_L[m_li];
  return loop;  
}

MYON_BrepFace* MYON_BrepTrim::Face() const
{
  MYON_BrepFace* face = 0;
  if ( m_brep && m_li >= 0 && m_li < m_brep->m_L.Count() )
  {
    int fi = m_brep->m_L[m_li].m_fi;
    if ( fi >= 0 && fi < m_brep->m_F.Count() )
      face = &m_brep->m_F[fi];
  }
  return face;
}

MYON_BrepEdge* MYON_BrepTrim::Edge() const
{
  MYON_BrepEdge* edge = 0;
  if ( m_brep && m_ei >= 0 && m_ei < m_brep->m_E.Count() )
    edge = &m_brep->m_E[m_ei];
  return edge;  
}

MYON_BrepVertex* MYON_BrepTrim::Vertex(int tvi) const
{
  MYON_BrepVertex* vertex = 0;
  if ( 0 != m_brep && 0 <= tvi && tvi <= 1 )
  {
    int vi = m_vi[tvi];
    if ( 0 <= vi && vi < m_brep->m_V.Count() )
    {
      vertex = &m_brep->m_V[vi];
    }
  }
  return vertex;  
}

MYON_BrepVertex* MYON_BrepEdge::Vertex(int evi) const
{
  MYON_BrepVertex* vertex = 0;
  if ( 0 != m_brep && 0 <= evi && evi <= 1 )
  {
    int vi = m_vi[evi];
    if ( 0 <= vi && vi < m_brep->m_V.Count() )
    {
      vertex = &m_brep->m_V[vi];
    }
  }
  return vertex;  
}


bool MYON_BrepTrim::IsValid( MYON_TextLog* text_log ) const
{
  if ( m_trim_index < 0 )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_trim_index < 0.\n");
    }
    return MYON_BrepIsNotValid();
  }

  if ( m_c2i < 0 )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_c2i = %d is not valid\n",m_c2i);
    }
    return MYON_BrepIsNotValid();
  }

  if ( !MYON_CurveProxy::IsValid(text_log) )
  {
    if ( text_log )
    {
      text_log->Print("trim curve proxy settings are not valid.\n");
    }
    return MYON_BrepIsNotValid();
  }

  if ( m_ei < 0 ) 
  {
    if ( m_type != singular )
    {
      if ( text_log )
      {
        text_log->Print("trim.m_ei = %d but trim.mtype != singular\n",m_ei);
      }
      return MYON_BrepIsNotValid();
    }
  }

  if ( m_vi[0] < 0 )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_v[0] = %d is not valid\n",m_vi[0]);
    }
    return MYON_BrepIsNotValid();
  }

  if ( m_vi[1] < 0 )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_v[1] = %d is not valid\n",m_vi[1]);
    }
    return MYON_BrepIsNotValid();
  }

  unsigned int i = m_type;
  if ( i >= trim_type_count )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_type = %d is not valid\n",i);
    }
    return MYON_BrepIsNotValid();
  }

  if ( i == MYON_BrepTrim::slit )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_type = MYON_BrepTrim::slit is not valid. REserved for future use.\n",i);
    }
    return MYON_BrepIsNotValid();
  }

  i = m_iso;
  if ( i >= MYON_Surface::iso_count )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_iso = %d is not valid\n",i);
    }
    return MYON_BrepIsNotValid();
  }

  if ( m_li < 0 )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_li = %d is not valid\n",m_li);
    }
    return MYON_BrepIsNotValid();
  }

  if ( !m_brep )
  {
    if ( text_log )
    {
      text_log->Print("trim.m_brep is null.\n");
    }
    return MYON_BrepIsNotValid();
  }

  return true;
}

void MYON_BrepTrim::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_BrepTrim[%d]:\n",m_trim_index);
}

bool MYON_BrepTrim::Reverse()
{
  m_pline.Destroy();
  DestroyCurveTree();

  bool rc = false;
  if ( m_brep )
  {
    MYON_Interval trim_domain = Domain();
    if ( m_brep->StandardizeTrimCurve( m_trim_index ) )
    {
      MYON_Curve* c2 = const_cast<MYON_Curve*>(TrimCurveOf());
      if ( c2 )
      {
        rc = c2->Reverse();
        trim_domain.Reverse();
        c2->SetDomain(trim_domain);
        SetProxyCurve(c2);
      }
    }
  }

  if ( !rc )
    rc = MYON_CurveProxy::Reverse();

  if (rc)
  {
    int i = m_vi[0];
    m_vi[0] = m_vi[1];
    m_vi[1] = i;
    if ( m_ei >= 0 )
      m_bRev3d = m_bRev3d ? false : true;
  }
  return rc;
}


////////////////////////////////////////////////////////////////
//   Class MYON_BrepLoop
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT_NO_COPYCTOR(MYON_BrepLoop,MYON_Geometry,"60B5DBC3-E660-11d3-BFE4-0010830122F0");

MYON_BrepLoop::MYON_BrepLoop()
{
  memset(&m_loop_user,0,sizeof(m_loop_user));
}

MYON_BrepLoop::MYON_BrepLoop(int loop_index) 
  : m_loop_index(loop_index)
{
  memset(&m_loop_user,0,sizeof(m_loop_user));
}

MYON_Brep* MYON_BrepLoop::Brep() const
{
  return m_brep;
}

MYON_BrepFace* MYON_BrepLoop::Face() const
{
  return m_brep ? m_brep->Face(m_fi) : 0;
}


unsigned int MYON_BrepLoop::SizeOf() const
{
  unsigned int sz = MYON_Object::SizeOf();
  sz += (sizeof(MYON_BrepLoop) - sizeof(MYON_Object));
  sz += m_ti.SizeOfArray();
  return sz;
}

MYON_BrepTrim* MYON_BrepLoop::Trim( int lti ) const
{
  MYON_BrepTrim* trim = ( m_brep && lti >= 0 && lti < m_ti.Count() )
                    ? m_brep->Trim(m_ti[lti])
                    : 0;
  return trim;
}

int MYON_BrepLoop::TrimCount() const
{
  return m_ti.Count();
}

MYON_BrepLoop& MYON_BrepLoop::operator=(const MYON_BrepLoop& src)
{
  if ( &src != this ) 
  {
    // do not copy m_brep pointer
    MYON_Object::operator=(src);
    m_loop_user   = src.m_loop_user;
    m_status      = src.m_status;
    m_loop_index  = src.m_loop_index;
    m_ti    = src.m_ti;
    m_type  = src.m_type;
    m_fi    = src.m_fi;
    m_pbox  = src.m_pbox;
  }
  return *this;
}

static void BadLoopMessage( int loop_index, MYON_TextLog* text_log )
{
  if ( text_log )
  {
    text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
  }
}

bool MYON_BrepLoop::IsValid( MYON_TextLog* text_log ) const
{
  if ( m_loop_index < 0 )
  {
    BadLoopMessage(m_loop_index,text_log);
    if ( text_log )
      text_log->Print("loop.m_loop_index < 0.\n");
    return MYON_BrepIsNotValid();
  }

  if ( m_ti.Count() < 1 )
  {
    BadLoopMessage(m_loop_index,text_log);
    if ( text_log )
      text_log->Print("loop.m_ti[] is empty.\n");
    return MYON_BrepIsNotValid();
  }
  int i = m_type;
  if ( i < 0 || i > type_count )
  {
    BadLoopMessage(m_loop_index,text_log);
    if ( text_log )
      text_log->Print("loop.m_type = %d is not a valid value.\n",i);
    return MYON_BrepIsNotValid();
  }
  if ( m_fi < 0 )
  {
    BadLoopMessage(m_loop_index,text_log);
    if ( text_log )
      text_log->Print("loop.m_fi = %d (should be >= 0 ).\n",m_fi);
    return MYON_BrepIsNotValid();
  }
  if ( !m_brep )
  {
    BadLoopMessage(m_loop_index,text_log);
    if ( text_log )
      text_log->Print("loop.m_brep is nullptr.\n");
    return MYON_BrepIsNotValid();
  }
  return true;
}

void MYON_BrepLoop::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_BrepLoop[%d]: m_fi = %d, m_type = %d m_ti.Count() = %d\n",
             m_loop_index,m_fi,m_type,m_ti.Count()
             );
}

int MYON_BrepLoop::IndexOfTrim( const MYON_BrepTrim& trim ) const
{
  const int count = m_ti.Count();
  int lti;
  for ( lti = 0; lti < count; lti++ )
  {
    if ( m_ti[lti] == trim.m_trim_index )
      return lti;
  }
  return -1;
}

////////////////////////////////////////////////////////////////
//   Class MYON_BrepFace
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT_NO_COPYCTOR(MYON_BrepFace,MYON_SurfaceProxy,"60B5DBC4-E660-11d3-BFE4-0010830122F0");

class MYON_BrepFace::Impl
{
public:
  std::shared_ptr<const MYON_Mesh> m_render_mesh;
  std::shared_ptr<const MYON_Mesh> m_analysis_mesh;
  std::shared_ptr<const MYON_Mesh> m_preview_mesh;
};

MYON_BrepFace::MYON_BrepFace()
  : MYON_SurfaceProxy(0),
    m_pImpl(new Impl)
{
  memset(&m_face_user,0,sizeof(m_face_user));
}

MYON_BrepFace::MYON_BrepFace(int face_index) 
  : MYON_SurfaceProxy(0),
  m_face_index(face_index),
  m_pImpl(new Impl)
{
  memset(&m_face_user,0,sizeof(m_face_user));
}


unsigned int MYON_BrepFace::SizeOf() const
{
  unsigned int sz = MYON_SurfaceProxy::SizeOf();
  sz += (sizeof(*this) - sizeof(MYON_SurfaceProxy));
  sz += m_li.SizeOfArray();
  if ( m_pImpl->m_render_mesh )
    sz += m_pImpl->m_render_mesh->SizeOf();
  if (m_pImpl->m_analysis_mesh )
    sz += m_pImpl->m_analysis_mesh->SizeOf();
  if (m_pImpl->m_preview_mesh )
    sz += m_pImpl->m_preview_mesh->SizeOf();
  return sz;
}


MYON_BrepFace& MYON_BrepFace::operator=(const MYON_BrepFace& src)
{
  if ( &src != this ) 
  {
    // do not copy m_brep pointer
    MYON_SurfaceProxy::operator=(src);
    m_face_user   = src.m_face_user;
    m_status      = src.m_status;
    m_face_index  = src.m_face_index;
    m_li    = src.m_li;
    m_si    = src.m_si;
    m_bRev  = src.m_bRev;
    m_face_material_channel = src.m_face_material_channel;
    m_face_uuid = src.m_face_uuid;
    m_per_face_color = src.m_per_face_color;
    m_pImpl->m_render_mesh   = src.m_pImpl->m_render_mesh   ? src.m_pImpl->m_render_mesh   : nullptr;
    m_pImpl->m_analysis_mesh = src.m_pImpl->m_analysis_mesh ? src.m_pImpl->m_analysis_mesh : nullptr;
    m_pImpl->m_preview_mesh  = src.m_pImpl->m_preview_mesh  ? src.m_pImpl->m_preview_mesh  : nullptr;
  }
  return *this;
}

MYON_BrepFace::~MYON_BrepFace()
{
  DestroyMesh(MYON::any_mesh);
  m_li.Destroy();
  delete m_pImpl;
}

MYON_Brep* MYON_BrepFace::Brep() const
{
  return m_brep;
}

MYON_BrepLoop* MYON_BrepFace::Loop( int lti ) const
{
  return (m_brep && lti >= 0 && lti < m_li.Count()) ? m_brep->Loop( m_li[lti]) : 0;
}

int MYON_BrepFace::LoopCount() const
{
  return m_li.Count();
}

MYON_BrepLoop* MYON_BrepFace::OuterLoop() const
{
  int li, lti;
  for ( lti = 0; lti < m_li.Count(); lti++ )
  {
    li = m_li[lti];
    if ( li >= 0 && li < m_brep->m_L.Count() )
    {
      if ( MYON_BrepLoop::outer == m_brep->m_L[li].m_type  )
      {
        return &m_brep->m_L[li];
      }
    }
  }
  return 0;
}

unsigned int MYON_BrepFace::PackId() const
{
  return  0x10000U * ((unsigned int)m_pack_id_high) + ((unsigned int)m_pack_id_low);
}

void MYON_BrepFace::ClearPackId()
{
  m_pack_id_low = 0;
  m_pack_id_high = 0;
}

void MYON_BrepFace::SetPackIdForExperts(
  unsigned int pack_id
)
{
  if (0 == pack_id)
    ClearPackId();
  else
  {
    m_pack_id_low = (MYON__UINT16)(pack_id % 0x10000U);
    m_pack_id_high = (MYON__UINT16)(pack_id / 0x10000U);
  }
}

bool MYON_BrepFace::IsValid( MYON_TextLog* text_log ) const
{
  if ( m_face_index < 0 )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_BrepFace m_face_index = %d.  Should be >= 0.\n",m_face_index);
    return false;
  }

  if ( m_li.Count() < 1 )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_BrepFace m_li.Count() = 0  Should be > 0.\n");
    return false;
  }

  if ( m_si < 0 )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_BrepFace m_si = %d.  Should be >= 0.\n",m_si);
    return false;
  }

  if ( 0 == m_brep )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_BrepFace m_brep = 0.  Should point to parent brep.\n");
    return false;

  }

  return true;
}

void MYON_BrepFace::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_BrepFace[%d]:",m_face_index);
  bool bNeedComma = false;
  if ( MYON_UuidCompare(m_face_uuid,MYON_nil_uuid) )
  {
    if (bNeedComma)
      dump.Print(",");
    dump.Print(" FaceId=");
    dump.Print(m_face_uuid);
    bNeedComma = true;
  }
  if (MYON_Color::UnsetColor != m_per_face_color)
  {
    if (bNeedComma)
      dump.Print(",");
    dump.Print(" PerFaceColor=(");
    dump.PrintColor(m_per_face_color);
    dump.Print(")");
  }
  if (0 != m_face_material_channel)
  {
    if (bNeedComma)
      dump.Print(",");
    dump.Print(" PerFaceMaterialChannel=");
    dump.Print("%d", m_face_material_channel);
  }
  dump.Print("\n");
}

void MYON_BrepFace::SetMaterialChannelIndex(int material_channel_index) const
{
  if (material_channel_index > 0 && material_channel_index <= MYON_Material::MaximumMaterialChannelIndex)
  {
    const_cast<MYON_BrepFace*>(this)->m_face_material_channel = material_channel_index;
  }
  else
  {
    if (0 != material_channel_index)
    {
      MYON_ERROR("Invalid material_channel_index value.");
    }
    ClearMaterialChannelIndex(); // makes it easier to detect when the per face setting is cleared.
  }
}

void MYON_BrepFace::ClearMaterialChannelIndex() const
{
  const_cast<MYON_BrepFace*>(this)->m_face_material_channel = 0;
}

int MYON_BrepFace::MaterialChannelIndex() const
{
  return m_face_material_channel;
}

void MYON_BrepFace::SetPerFaceColor(
  MYON_Color color
  ) const
{
  if (MYON_Color::UnsetColor == color)
    ClearPerFaceColor(); // makes it easier to detect when the per face setting is cleared.
  else
    m_per_face_color = color;
}

void MYON_BrepFace::ClearPerFaceColor() const
{
  m_per_face_color = MYON_Color::UnsetColor;
}


const MYON_Color MYON_BrepFace::PerFaceColor() const
{
  return m_per_face_color;
}


unsigned int MYON_Brep::ClearPerFaceMaterialChannelIndices()
{
  unsigned change_count = 0;
  const unsigned face_count = m_F.UnsignedCount();
  const MYON_BrepFace* f = m_F.Array();
  for (unsigned fi = 0; fi < face_count; ++fi)
  {
    if (0 != f[fi].m_face_material_channel)
    {
      f[fi].ClearMaterialChannelIndex();
      ++change_count;
    }
  }
  return change_count;
}

bool MYON_Brep::HasPerFaceMaterialChannelIndices() const
{
  const unsigned face_count = m_F.UnsignedCount();
  const MYON_BrepFace* f = m_F.Array();
  for (unsigned fi = 0; fi < face_count; ++fi)
  {
    if (0 != f[fi].m_face_material_channel)
      return true;
  }
  return false;
}

unsigned int MYON_Brep::ClearPerFaceColors() const
{
  unsigned change_count = 0;
  const unsigned face_count = m_F.UnsignedCount();
  const MYON_BrepFace* f = m_F.Array();
  for (unsigned fi = 0; fi < face_count; ++fi)
  {
    if (MYON_Color::UnsetColor != f[fi].m_per_face_color)
    {
      f[fi].ClearPerFaceColor();
      ++change_count;
    }
  }
  return change_count;
}

bool MYON_Brep::HasPerFaceColors() const
{
  const unsigned face_count = m_F.UnsignedCount();
  const MYON_BrepFace* f = m_F.Array();
  for (unsigned fi = 0; fi < face_count; ++fi)
  {
    if (MYON_Color::UnsetColor != f[fi].m_per_face_color)
      return true;
  }
  return false;
}

const std::shared_ptr<const MYON_Mesh>& MYON_BrepFace::UniqueMesh(MYON::mesh_type mesh_type)
{
  const auto& sp = SharedMesh(mesh_type);

  if (sp && sp.use_count() > 1)
  {
    SetMesh(mesh_type, std::make_shared<const MYON_Mesh>(*sp));
    return SharedMesh(mesh_type);
  }

  return sp;
}


const std::shared_ptr<const MYON_Mesh>& MYON_BrepFace::SharedMesh(MYON::mesh_type mesh_type) const
{
  std::shared_ptr<const MYON_Mesh>* pMesh = nullptr;

  switch (mesh_type)
  {
  case MYON::render_mesh:
    pMesh = &m_pImpl->m_render_mesh;
    break;
  case MYON::analysis_mesh:
    pMesh = &m_pImpl->m_analysis_mesh;
    break;
  case MYON::preview_mesh:
    pMesh = &m_pImpl->m_preview_mesh;
    break;
  default:
    pMesh = m_pImpl->m_render_mesh ? &m_pImpl->m_render_mesh : &m_pImpl->m_analysis_mesh;
    if (pMesh->get() == nullptr)
    {
      pMesh = &m_pImpl->m_preview_mesh;
    }
    break;
  }

  if (pMesh && pMesh->get())
  {
    const_cast<MYON_Mesh*>(pMesh->get())->m_parent = this;
  }

  return *pMesh;
}

const MYON_Mesh* MYON_BrepFace::Mesh( MYON::mesh_type mt ) const
{
  const auto& sp = SharedMesh(mt);

  return sp ? sp.get() : nullptr;
}

bool MYON_BrepFace::SetMesh(MYON::mesh_type mt, const std::shared_ptr<const MYON_Mesh>& mesh)
{
  bool rc = true;
  switch (mt)
  {
  case MYON::render_mesh:
    m_pImpl->m_render_mesh = mesh;
    break;
  case MYON::analysis_mesh:
    m_pImpl->m_analysis_mesh = mesh;
    break;
  case MYON::preview_mesh:
    m_pImpl->m_preview_mesh = mesh;
    break;
  default:
    rc = false;
  }

  return rc;
}

bool MYON_BrepFace::SetMesh(MYON::mesh_type mt, MYON_Mesh* mesh)
{
  bool rc = true;
  switch (mt)
  {
  case MYON::render_mesh:
    m_pImpl->m_render_mesh.reset(mesh);
    break;
  case MYON::analysis_mesh:
    m_pImpl->m_analysis_mesh.reset(mesh);
    break;
  case MYON::preview_mesh:
    m_pImpl->m_preview_mesh.reset(mesh);
    break;
  default:
    rc = false;
  }

  return rc;
}

void MYON_BrepFace::DestroyMesh( MYON::mesh_type mt, bool bDeleteMesh )
{
  return DestroyMesh(mt);
}

void MYON_BrepFace::DestroyMesh(MYON::mesh_type mt)
{
  switch(mt) {
  case MYON::render_mesh:
    m_pImpl->m_render_mesh.reset();
    break;
  case MYON::analysis_mesh:
    m_pImpl->m_analysis_mesh.reset();
    break;
  case MYON::preview_mesh:
    m_pImpl->m_preview_mesh.reset();
    break;
  default:
    m_pImpl->m_render_mesh.reset();
    m_pImpl->m_analysis_mesh.reset();
    m_pImpl->m_preview_mesh.reset();
    break;
  }
}


unsigned int MYON_BrepVertexArray::SizeOf() const
{
  unsigned int sz = 0;
  int i, count = Count();
  for ( i = 0; i < count; i++ )
  {
    sz += m_a[i].SizeOf();
  }
  sz += (m_capacity - m_count)*sizeof(m_a[0]);
  return sz;
}

unsigned int MYON_BrepEdgeArray::SizeOf() const
{
  unsigned int sz = 0;
  int i, count = Count();
  for ( i = 0; i < count; i++ )
  {
    sz += m_a[i].SizeOf();
  }
  sz += (m_capacity - m_count)*sizeof(m_a[0]);
  return sz;
}

unsigned int MYON_BrepTrimArray::SizeOf() const
{
  unsigned int sz = 0;
  int i, count = Count();
  for ( i = 0; i < count; i++ )
  {
    sz += m_a[i].SizeOf();
  }
  sz += (m_capacity - m_count)*sizeof(m_a[0]);
  return sz;
}

unsigned int MYON_BrepLoopArray::SizeOf() const
{
  unsigned int sz = 0;
  int i, count = Count();
  for ( i = 0; i < count; i++ )
  {
    sz += m_a[i].SizeOf();
  }
  sz += (m_capacity - m_count)*sizeof(m_a[0]);
  return sz;
}

unsigned int MYON_BrepFaceArray::SizeOf() const
{
  unsigned int sz = 0;
  int i, count = Count();
  for ( i = 0; i < count; i++ )
  {
    sz += m_a[i].SizeOf();
  }
  sz += (m_capacity - m_count)*sizeof(m_a[0]);
  return sz;
}

////////////////////////////////////////////////////////////////
//   Class MYON_Brep
////////////////////////////////////////////////////////////////

MYON_BrepVertexArray::MYON_BrepVertexArray()
{}

MYON_BrepVertexArray::~MYON_BrepVertexArray()
{}


MYON_BrepEdgeArray::MYON_BrepEdgeArray()
{}

MYON_BrepEdgeArray::~MYON_BrepEdgeArray()
{}


MYON_BrepTrimArray::MYON_BrepTrimArray()
{}

MYON_BrepTrimArray::~MYON_BrepTrimArray()
{}


MYON_BrepLoopArray::MYON_BrepLoopArray()
{}

MYON_BrepLoopArray::~MYON_BrepLoopArray()
{}

MYON_BrepFaceArray::MYON_BrepFaceArray()
{}

MYON_BrepFaceArray::~MYON_BrepFaceArray()
{}


MYON_OBJECT_IMPLEMENT(MYON_Brep,MYON_Geometry,"60B5DBC5-E660-11d3-BFE4-0010830122F0");

void MYON_Brep::Initialize()
{
  memset(&m_brep_user,0,sizeof(m_brep_user));
  m_is_solid = 0;
  m_bbox.Destroy();
  m_region_topology = nullptr;
}

MYON_Brep* MYON_Brep::New() 
{
  // use instead of new MYON_Brep()
  // (When openNURBS is used as a Windows DLL, 
  // this forces the call to new to happen in the openNURBS DLL.)
  return new MYON_Brep();
}

MYON_Brep* MYON_Brep::New(const MYON_Brep& src) 
{
  // use instead of new MYON_Brep(const MYON_Brep&)
  // (When openNURBS is used as a Windows DLL, 
  // this forces the call to new to happen in the openNURBS DLL.)
  return new MYON_Brep(src);
}

MYON_Brep::MYON_Brep()
{ 
  MYON__SET__THIS__PTR(m_s_MYON_Brep_ptr);
  Initialize();

  /*
  const size_t sz = sizeof(*this);
  const char* p0 = (char*)(this);
  const char* p1 = (char*)(&m_bbox);
  const char* p2 = (char*)(&m_region_topology);
  const char* p3 = (char*)(&m_aggregate_status);
  const char* p4 = (char*)(&m_is_solid);
  const char* p5 = (char*)(&m_sleep_lock);
  const char* p100 = (char*)(this+1);

  const size_t offset1 = (p1 - p0);
  const size_t offset2 = (p2 - p0);
  const size_t offset3 = (p3 - p0);
  const size_t offset4 = (p4 - p0);
  const size_t offset5 = (p5 - p0);
  const size_t offset100 = (p100 - p0);
  MYON_TextLog::Null.Print(L"", sz, offset1, offset2, offset3, offset4, offset5, offset100);

  //BEFORE:
  //  offset1	216
  //  offset2	264
  //  offset3	272
  //  offset4	304
  //  offset5	308
  //  offset100	312
  //  sz	312

  //  AFTER
  //  ...
  */
}

MYON_Brep::MYON_Brep(const MYON_Brep& src) : MYON_Geometry(src)
{
  MYON__SET__THIS__PTR(m_s_MYON_Brep_ptr);
  Initialize();
  *this = src;
}

MYON_Brep::~MYON_Brep()
{ 
  DestroyMesh(MYON::any_mesh);
  // everything is in array classes that destroy themselves.
  delete m_region_topology;
  m_region_topology = 0;
}

unsigned int MYON_Brep::SizeOf() const
{
  int i, count;

  unsigned int sz = MYON_Geometry::SizeOf();
  sz += (sizeof(*this) - sizeof(MYON_Geometry));
  sz += m_C2.SizeOfArray();
  sz += m_C3.SizeOfArray();
  sz += m_S.SizeOfArray();

  count = m_C2.Count();
  for ( i = 0; i < count; i++ )
  {
    const MYON_Curve* c2 = m_C2[i];
    if ( c2 )
      sz += c2->SizeOf();
  }

  count = m_C3.Count();
  for ( i = 0; i < count; i++ )
  {
    const MYON_Curve* c3 = m_C3[i];
    if ( c3 )
      sz += c3->SizeOf();
  }

  count = m_S.Count();
  for ( i = 0; i < count; i++ )
  {
    const MYON_Surface* s = m_S[i];
    if ( s )
      sz += s->SizeOf();
  }

  sz += m_V.SizeOf();
  sz += m_E.SizeOf();
  sz += m_T.SizeOf();
  sz += m_L.SizeOf();
  sz += m_F.SizeOf();

  return sz;
}

MYON__UINT32 MYON_BrepVertex::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_vertex_index),&m_vertex_index);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_tolerance),&m_tolerance);
  current_remainder = m_ei.DataCRC(current_remainder);
  return current_remainder;
}

MYON__UINT32 MYON_BrepEdge::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = MYON_CurveProxy::DataCRC(current_remainder);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_edge_index),&m_edge_index);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_c3i),&m_c3i);
  current_remainder = MYON_CRC32(current_remainder,2*sizeof(m_vi[0]),&m_vi[0]);
  current_remainder = m_ti.DataCRC(current_remainder);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_tolerance),&m_tolerance);

  return current_remainder;
}

MYON__UINT32 MYON_BrepFace::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = MYON_SurfaceProxy::DataCRC(current_remainder);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_face_index),&m_face_index);
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_bRev),&m_bRev);
  current_remainder = m_li.DataCRC(current_remainder);

  return current_remainder;
}

MYON__UINT32 MYON_Brep::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = m_V.DataCRC(current_remainder);
  current_remainder = m_E.DataCRC(current_remainder);
  current_remainder = m_F.DataCRC(current_remainder);
  return current_remainder;
}

void MYON_Brep::DestroyMesh( MYON::mesh_type mt, bool bDeleteMesh )
{
  DestroyMesh(mt);
}

void MYON_Brep::DestroyMesh(MYON::mesh_type mt)
{
  const int fcnt = m_F.Count();
  int fi;
  for (fi = 0; fi < fcnt; fi++) {
    m_F[fi].DestroyMesh(mt);
  }
}

int MYON_Brep::GetMesh( MYON::mesh_type mt, MYON_SimpleArray<const MYON_Mesh*>& meshes ) const
{
  int fcnt = m_F.Count();
  int fi;
  int null_count = 0;
  meshes.Reserve( meshes.Count() + fcnt );
  for ( fi = 0; fi < fcnt; fi++ )
  {
    const MYON_Mesh* mesh = m_F[fi].Mesh(mt);
    meshes.Append( mesh );
    if ( !mesh )
    {
      // If some meshes are missing, we have to put
      // a null in the return array so the face-to-mesh
      // correspondence is preserved.
      null_count++;
    }
  }
  if ( null_count == fcnt )
  {
    // If ALL the meshes are missing, return 0.
    meshes.SetCount(meshes.Count()-fcnt);
    fcnt = 0;
  }
  return fcnt;
}


int MYON_Brep::Dimension() const
{
  return (m_V.Count() > 0) ? 3 : 0;
}

static void MYON_BrepTransformSwapSrfHelper( MYON_Brep& brep, MYON_NurbsSurface* nurbs_srf, int si )
{
  // Replace plane surface which could not be properly transformed
  // with nurbs_surface.
  MYON_Surface* old_srf = brep.m_S[si];
  MYON_UserDataHolder udholder;
  udholder.MoveUserDataFrom(*old_srf);
  udholder.MoveUserDataTo(*nurbs_srf,false);
  brep.m_S[si] = nurbs_srf;
  
  // Update faces to use new surface.
  const int fcount = brep.m_F.Count();
  MYON_BrepFace* f = brep.m_F.Array();
  for ( int fi = 0; fi < fcount; fi++ )
  {
    if (f[fi].m_si == si || f[fi].ProxySurface() == old_srf )
    {
      const bool bIsTransposed = f[fi].ProxySurfaceIsTransposed();
      f[fi].SetProxySurface(nurbs_srf);
      if (bIsTransposed)
        f[fi].MYON_SurfaceProxy::Transpose();
    }
  }

  delete old_srf;
}

bool MYON_Brep::Transform( const MYON_Xform& xform )
{
  int i, count;
  bool rc = true;
  
  DestroyRuntimeCache();

  int is_similarity = xform.IsSimilarity();
  const double det = xform.Determinant();
  bool bUniformScale = (0 != is_similarity && det != 0.0 && MYON_IsValid(det) && fabs(fabs(det) - 1.0) > 1.0e-6);
  const double uniform_scale = bUniformScale ? pow(fabs(det), 1.0 / 3.0) : 1.0;

  if ( 1 != is_similarity )
  {
    // this will cause the solid flag to be
    // recaclulated the next time it is needed.
    m_is_solid = 0;
  }


  // 13 Feb 2003 Dale Lear:
  // Transforming the bbox makes it grow too large under repeated
  // rotations.  So, we will destroy it here and reset it below.
  //m_bbox.Transform(xform);
  m_bbox.Destroy();

  count = m_C3.Count();
  for ( i = 0; i < count; i++ ) 
  {
    if ( m_C3[i] ) 
    {
      if ( !m_C3[i]->Transform(xform) )
        rc = false;
    }
  }

  count = m_S.Count();
  for ( i = 0; i < count; i++ ) {
    if ( m_S[i] ) 
    
    {
      MYON_NurbsSurface* nurbs_srf = 0;
      if ( !is_similarity )
      {
        if (    1 == m_S[i]->Degree(0) // degree tests reduce calls to
             && 1 == m_S[i]->Degree(1) // slow MYON_PlaneSurface::Cast()
             && 0 != MYON_PlaneSurface::Cast(m_S[i]) )
        {
          nurbs_srf = MYON_NurbsSurface::New();
          if ( !m_S[i]->GetNurbForm(*nurbs_srf) )
          {
            delete nurbs_srf;
            nurbs_srf = 0;
          }
          else if ( !nurbs_srf->Transform(xform) )
          {
            delete nurbs_srf;
            nurbs_srf = 0;
          }
        }
      }

      if ( !m_S[i]->Transform(xform) )
      {
        if ( nurbs_srf )
        {
          MYON_BrepTransformSwapSrfHelper(*this,nurbs_srf,i);
          nurbs_srf = 0;
        }
        else
        {
          rc = false;
        }
      }
      else if ( nurbs_srf )
      {
        // make sure transformation was good
        MYON_Interval u = nurbs_srf->Domain(0);
        MYON_Interval v = nurbs_srf->Domain(1);
        for ( int ui = 0; ui < 2 && nurbs_srf; ui++ ) for (int vi = 0; vi < 2 && nurbs_srf; vi++)
        {
          MYON_3dPoint P = nurbs_srf->PointAt(u[ui],v[vi]);
          MYON_3dPoint Q = m_S[i]->PointAt(u[ui],v[vi]);
          if ( P.DistanceTo(Q) > MYON_ZERO_TOLERANCE )
          {
            MYON_BrepTransformSwapSrfHelper(*this,nurbs_srf,i);
            nurbs_srf = 0;
            break;
          }
        }
        if ( nurbs_srf )
        {
          delete nurbs_srf;
          nurbs_srf = 0;
        }
      }
    }
  }

  count = m_V.Count();
  for ( i = 0; i < count; i++ ) {
    if ( !m_V[i].Transform(xform) )
      rc = false;
  }

  count = m_E.Count();
  for ( i = 0; i < count; i++ ) {
    MYON_BrepEdge& edge = m_E[i];
    edge.TransformUserData(xform);
    // 2014-05-13 Dale Lear RH-26852
    //   Based on a conversation with Chuck, we will
    //   test scaling edge tolerances.  The bug
    //   bailed down to a unit system change from a 
    //   "long" unit to mm made a valid brep invalid
    //   because the edge tolerances were too small.
    //   This fix does ot handle non-uniform scaling.
    //   In the case of a non-uniform scale, the edge
    //   tolerance should be recalculated.
    if (edge.m_tolerance > 0.0 && uniform_scale > 0.0 && uniform_scale != 1.0)
    {
      edge.m_tolerance *= uniform_scale;
    }
  }

  count = m_F.Count();
  for ( i = 0; i < count; i++ ) 
  {
    MYON_BrepFace& face = m_F[i];
    face.TransformUserData(xform);

    // 13 Feb 2003 Dale Lear:
    // Transforming the bbox makes it grow too large under repeated
    // rotations.  So, we need to reset it.
    face.m_bbox.Destroy();

    //GBA 20 May 2020. Brep box now computed from face boxes, instead of surface boxes.
    face.m_bbox = face.BoundingBox();    
    if ( face.m_face_index != -1 )
      m_bbox.Union( face.m_bbox );
    

    // 12 May 2003 Dale Lear - RR 10528
    //     Use surface evaluation to update rendermesh when 
    //     calling MYON_Mesh::Transform() will map mesh normals
    //     to some thing different that the "true" surface
    //     normal.
    bool bEvMesh = ( fabs(det) <= MYON_SQRT_EPSILON
                     || xform[3][0] != 0.0
                     || xform[3][1] != 0.0
                     || xform[3][2] != 0.0
                     || xform[3][3] != 1.0
                     );
    const MYON_Surface* srf = face.SurfaceOf();

    if ( 0 == srf )
      bEvMesh = false;

    //Render meshes
    {
      auto spMesh = face.UniqueMesh(MYON::render_mesh);
      if (spMesh)
      {
        auto pMesh = const_cast<MYON_Mesh*>(spMesh.get());

        if ( bEvMesh && pMesh->EvaluateMeshGeometry(*srf) )
        {
          if ( face.m_bRev )
          {
            // 29 September 2003 Dale Lear
            //     Normals on render meshes (and face orientations)
            //     take face.m_bRev into account so that two sided
            //     materials work as expected.  EvaluateMeshGeometry()
            //     does not take face.m_bRev into account, so we need
            //     to reverse the face normals here.
            int ni, ncnt = pMesh->m_N.Count();
            for ( ni = 0; ni < ncnt; ni++ )
            {
              pMesh->m_N[ni] = -(pMesh->m_N[ni]);
            }
          }
        }
        else
        {
          pMesh->Transform(xform);
        }
      }
    }

    //Analysis meshes
    {
      auto spMesh = face.UniqueMesh(MYON::analysis_mesh);
      if (spMesh)
      {
        auto pMesh = const_cast<MYON_Mesh*>(spMesh.get());

        // Dale Lear 30 March 2009 - bug 46766
        //   Evaluate analysis meshes when the transform involves scaling
        //   so curvature values are properly updated.
        bool bEvAnalysisMesh = bEvMesh;
        if ( !bEvAnalysisMesh )
        {
          MYON_Xform tmp(xform);
          tmp.m_xform[0][3] = 0.0;
          tmp.m_xform[1][3] = 0.0;
          tmp.m_xform[2][3] = 0.0;
          if ( 1 != tmp.IsSimilarity() )
            bEvAnalysisMesh = true;
        }
        // 1 Sept 2021, Mikko, RH-65468:
        // Added null check to prevent a crash.
        if ( bEvAnalysisMesh && nullptr != srf && pMesh->EvaluateMeshGeometry(*srf) )
        {
          // 28 Sept 2012, Mikko:
          // Apply the "29 September 2003 Dale Lear" fix above also to analysis meshes.
          if ( face.m_bRev )
          {
            int ni, ncnt = pMesh->m_N.Count();
            for ( ni = 0; ni < ncnt; ni++ )
            {
              pMesh->m_N[ni] = -(pMesh->m_N[ni]);
            }
          }
        }
        else
        {
          pMesh->Transform(xform);
        }
      }
    }

    //Preview meshes
    {
      auto spMesh = face.UniqueMesh(MYON::preview_mesh);
      if (spMesh)
      {
        auto pMesh = const_cast<MYON_Mesh*>(spMesh.get());
        if ( bEvMesh && pMesh->EvaluateMeshGeometry(*srf) )
        {
          if ( face.m_bRev )
          {
            // 10 June 2021, Mikko, RH-64582:
            // Fixed typo that caused a crash, changed "m_analysis_mesh" to "m_preview_mesh".
            int ni, ncnt = pMesh->m_N.Count();
            for ( ni = 0; ni < ncnt; ni++ )
            {
              pMesh->m_N[ni] = -(pMesh->m_N[ni]);
            }
          }
        }
        else
        {
          pMesh->Transform(xform);
        }
      }
    }
  }

  // The call to transform user data needs to be last
  // so that the rest of the brep is in position.
  // In particular, MYON_BrepRegionTopologyUserData::Transform
  // assumes the face bounding boxes are up to date.
  TransformUserData(xform);

  return rc;
}

/////////////////////////////////////////////////////////////////
// MYON_Brep Creation Interface

int 
MYON_Brep::AddTrimCurve( MYON_Curve* pC )
{
  int c2i = -1;

  if ( 0 != pC )
  {
    // 7 April 2003 Dale Lear:
    //    There are too many cases where bugs are caused by
    //    people attempting to use 3d curves for trims.  In
    //    all the cases encountered so far, the intent was
    //    to pass in a 2d curve, so...

    int dim = pC->Dimension();

    if ( dim != 2 )
    {
      MYON_ERROR("MYON_Brep::AddTrimCurve() go a non-2d curve - changing dim to 2.");
      pC->ChangeDimension(2);
      dim = pC->Dimension();
    }

    if ( 2 == dim )
    {
      c2i = m_C2.Count();
      m_C2.Append(pC);
    }
  }
  return c2i;
}

int 
MYON_Brep::AddEdgeCurve( MYON_Curve* pC )
{
  int c3i = -1;
  if ( 0 != pC )
  {

    int dim = pC->Dimension();

    if ( dim != 3 )
    {
      // 7 April 2003 Dale Lear: (See comment in MYON_Brep::AddTrimCurve().)
      MYON_ERROR("MYON_Brep::AddEdgeCurve() got a non-3d curve - changing dim to 3.");
      pC->ChangeDimension(3);
      dim = pC->Dimension();
    }
    
    if ( 3 == dim )
    {
      c3i = m_C3.Count();
      m_C3.Append(pC);
    }
  }
  return c3i;
}

int 
MYON_Brep::AddSurface( MYON_Surface* pS )
{
  int si = -1;
  if ( pS && pS->Dimension() == 3 ) 
  {
    si = m_S.Count();
    m_S.Append(pS);
  }
  m_bbox.Destroy();
  m_is_solid = 0;
  return si;
}

MYON_BrepVertex& 
MYON_Brep::NewVertex()
{
  int vi = m_V.Count();
  m_V.Reserve(vi+1);
  m_V.SetCount(vi+1);
  MYON_BrepVertex& vertex = m_V.Array()[vi];
  vertex.m_vertex_index = vi;
  vertex.point = MYON_3dPoint::UnsetPoint;
  vertex.m_tolerance = MYON_UNSET_VALUE;
  return vertex;
}

MYON_BrepVertex& 
MYON_Brep::NewVertex( MYON_3dPoint vertex_point, double vertex_tolerance )
{
  MYON_BrepVertex& vertex = NewVertex();
  vertex.point = vertex_point;
  vertex.m_tolerance = vertex_tolerance;
  return vertex;
}

MYON_BrepEdge& 
MYON_Brep::NewEdge( int c3i )
{
  int ei = m_E.Count();
  MYON_BrepEdge& edge = m_E.AppendNew();
  edge.m_tolerance = MYON_UNSET_VALUE;
  edge.m_edge_index = ei;
  edge.m_c3i = c3i;
  if ( edge.m_c3i >= 0 && edge.m_c3i < m_C3.Count() ) 
  {
    edge.SetProxyCurve(m_C3[edge.m_c3i]);
  }
  edge.m_brep = this;
  return edge;
}

MYON_BrepEdge& 
MYON_Brep::NewEdge( MYON_BrepVertex& v0, MYON_BrepVertex& v1, 
                  int c3i, const MYON_Interval* edomain,
                  double edge_tolerance )
{
  MYON_BrepEdge& edge = NewEdge(c3i);
  edge.m_vi[0] = v0.m_vertex_index;
  edge.m_vi[1] = v1.m_vertex_index;
  v0.m_ei.Append(edge.m_edge_index);
  v1.m_ei.Append(edge.m_edge_index);
  if ( edomain && edomain->IsIncreasing() ) {
    MYON_Interval edom;
    edom.Intersection( edge.ProxyCurveDomain(), *edomain );
    if ( edom.IsIncreasing() )
      edge.SetProxyCurveDomain(edom);
  }
  edge.m_tolerance = edge_tolerance;
  return edge;
}

bool MYON_Brep::SetEdgeCurve( 
  MYON_BrepEdge& edge,
  int c3_index,
  const MYON_Interval* sub_domain
  )
{
  bool rc = false;
  if ( c3_index == - 1 && !sub_domain )
  {
    edge.m_c3i = -1;
    edge.SetProxyCurve(0);
    rc = true;
  }
  else if ( c3_index >= 0 && c3_index <= m_C3.Count() && m_C3[c3_index] )
  {
    MYON_Interval curve_domain = m_C3[c3_index]->Domain();
    if ( !sub_domain || (sub_domain->IsIncreasing() && curve_domain.Includes(*sub_domain)) )
    {
      edge.m_c3i = c3_index;
      edge.SetProxyCurve( m_C3[c3_index], 
                          (sub_domain) ? *sub_domain : curve_domain
                          );
      rc = true;
    }
  }
  return rc;
}

bool MYON_Brep::SetTrimCurve( 
  MYON_BrepTrim& trim,
  int c2_index,
  const MYON_Interval* sub_domain
  )
{
  bool rc = false;
  if ( c2_index == - 1 && !sub_domain )
  {
    trim.m_c2i = -1;
    trim.SetProxyCurve(0);
    //June 18 2013 - Chuck - nuke the bounding box and pline since it came from the old 2d curve.
    trim.DestroyPspaceInformation();
    rc = true;
  }
  else if ( c2_index >= 0 && c2_index <= m_C2.Count() && m_C2[c2_index] )
  {
    MYON_Interval curve_domain = m_C2[c2_index]->Domain();
    if ( !sub_domain || (sub_domain->IsIncreasing() && curve_domain.Includes(*sub_domain)) )
    {
      trim.m_c2i = c2_index;
      trim.SetProxyCurve( m_C2[trim.m_c2i], (sub_domain) ? *sub_domain : curve_domain );
      trim.m_pbox = m_C2[trim.m_c2i]->BoundingBox();
      trim.m_pbox.m_min.z = 0.0;
      trim.m_pbox.m_max.z = 0.0;
      rc = true;
    }
  }
  return rc;
}

MYON_BrepTrim& 
MYON_Brep::NewTrim( int c2i )
{
  m_is_solid = 0;
  int ti = m_T.Count();
  MYON_BrepTrim& trim = m_T.AppendNew();
  trim.m_brep = this;
  trim.m_trim_index = ti;
  trim.m_ei = -1;
  trim.m_type = MYON_BrepTrim::unknown;
  trim.m_bRev3d = false;
  trim.m_c2i = c2i;
  trim.m_iso = MYON_Surface::not_iso;
  trim.m_li = -1;
  trim.m_tolerance[0] = MYON_UNSET_VALUE;
  trim.m_tolerance[1] = MYON_UNSET_VALUE;
  trim.m__legacy_2d_tol = MYON_UNSET_VALUE;
  trim.m__legacy_3d_tol = MYON_UNSET_VALUE;
  trim.m__legacy_flags = 0;
  const MYON_Curve* c2 = (c2i >= 0 && c2i < m_C2.Count()) 
                     ? m_C2[c2i] 
                     : 0;
  if ( c2 ) 
  {
    trim.SetProxyCurve( c2 );
    trim.m_pbox = c2->BoundingBox();
    trim.m_pbox.m_min.z = 0.0;
    trim.m_pbox.m_max.z = 0.0;
  }

  return trim;
}

MYON_BrepTrim& 
MYON_Brep::NewTrim( MYON_BrepEdge& edge, bool bRev3d, int c2i )
{
  m_is_solid = 0;
  MYON_BrepTrim& trim = NewTrim( c2i );
  trim.m_ei = edge.m_edge_index;
  edge.m_ti.Append(trim.m_trim_index);
  trim.m_vi[0] = edge.m_vi[bRev3d?1:0];
  trim.m_vi[1] = edge.m_vi[bRev3d?0:1];
  trim.m_bRev3d = bRev3d?true:false;
  return trim;
}


MYON_BrepTrim& 
MYON_Brep::NewTrim( MYON_BrepEdge& edge, bool bRev3d, MYON_BrepLoop& loop, int c2i )
{
  m_is_solid = 0;
  const int edge_trim_count0 = edge.m_ti.Count();
  MYON_BrepTrim& trim = NewTrim( edge, bRev3d, c2i );
  trim.m_li = loop.m_loop_index;
  loop.m_ti.Append(trim.m_trim_index);
  if ( c2i >= 0 && c2i < m_C2.Count() )
  {
    MYON_Curve* c2 = m_C2[c2i];
    if ( c2 )
    {
      MYON_BoundingBox c2_bbox;
      if ( c2->GetBoundingBox(c2_bbox) )
      {
        c2_bbox.m_min.z = 0.0;
        c2_bbox.m_max.z = 0.0;
        if ( loop.m_ti.Count() == 1 )
          loop.m_pbox = c2_bbox;
        else
          loop.m_pbox.Union(c2_bbox);
      }
    }
  }

  if ( edge_trim_count0 == 0 )
  {
    // This is the only trim using this edge.
    //
    // At the moment it's a boundary trim.  The type
    // will be changed to seam or mated when
    // another trim is added that uses this edge.
    trim.m_type = MYON_BrepTrim::boundary;
  }
  else if ( edge_trim_count0 == 1 )
  {
    // there are now two trims using this edge
    MYON_BrepTrim::TYPE trim_type = MYON_BrepTrim::mated;
    MYON_BrepTrim& other_trim = m_T[edge.m_ti[0]];
    /*
       GBA RH-60512 April-21
       New seam requirements.
         Mate is on on the same face 
            Loop is an outer loop
            This and Mate are paired side isos ( e.g. W and East)
            matching non iso dir parameters in opposite directions
            Underlying surface is closed in the correct direction

      Remarks
         If constructing a brep with mated trims on the same face that 
         don't meet these requirements.   Then split the face so all
         trims are validly mated
    */

    if ( trim.Face() && trim.Face() == other_trim.Face() )
    {
      if( other_trim.Loop()==trim.Loop())
        trim_type = MYON_BrepTrim::seam;
      else
      {
        // trim_type is left as unknown  this will cause IsValid to fail  
        trim_type = MYON_BrepTrim::unknown;
      }
    }
    else 
      trim_type = MYON_BrepTrim::mated;
    trim.m_type = trim_type;
    other_trim.m_type = trim_type;
  }
  else
  {
    //  edge_trim_count0 >1
    // non-manifold edge - need to check for mated or seam
    bool matchedseam = false;
    MYON_BrepTrim::TYPE trim_type = MYON_BrepTrim::mated;
    for ( int eti = 0; eti < edge_trim_count0; eti++ )
    {
      MYON_BrepTrim& other_trim = m_T[edge.m_ti[eti]];
      if (trim.Face() && trim.Face() == other_trim.Face())
      {
        if (other_trim.Loop() == trim.Loop() && !matchedseam && 
            ( other_trim.m_type == MYON_BrepTrim::mated || other_trim.m_type == MYON_BrepTrim::seam))
        {
          trim_type = MYON_BrepTrim::seam;
          other_trim.m_type = MYON_BrepTrim::seam;
          matchedseam = true;
        }
        else
        {
          // Trims from from same edge on same face but different loops is invalid
          trim_type = MYON_BrepTrim::unknown;
          other_trim.m_type = MYON_BrepTrim::unknown;
          break;
        }
      }
    }
    trim.m_type = trim_type;
  }
  return trim;
}


MYON_BrepTrim& 
MYON_Brep::NewTrim( bool bRev3d, MYON_BrepLoop& loop, int c2i )
{
  m_is_solid = 0;
  MYON_BrepTrim& trim = NewTrim( c2i );
  trim.m_bRev3d = bRev3d ? true : false;
  trim.m_li = loop.m_loop_index;
  loop.m_ti.Append(trim.m_trim_index);
  if ( c2i >= 0 && c2i < m_C2.Count() )
  {
    const MYON_Curve* c2 = m_C2[c2i];
    if ( c2 )
    {
      MYON_BoundingBox c2_bbox;
      if ( c2->GetBoundingBox(c2_bbox) )
      {
        c2_bbox.m_min.z = 0.0;
        c2_bbox.m_max.z = 0.0;
        if ( loop.m_ti.Count() == 1 )
          loop.m_pbox = c2_bbox;
        else
          loop.m_pbox.Union( c2_bbox );
      }
    }
  }
  return trim;
}

MYON_BrepTrim&
MYON_Brep::NewSingularTrim(const MYON_BrepVertex& vertex,MYON_BrepLoop& loop, MYON_Surface::ISO iso, int c2i)
{
  MYON_BrepTrim& trim = NewTrim(false,loop,c2i);
  trim.m_vi[0] = vertex.m_vertex_index;
  trim.m_vi[1] = trim.m_vi[0];
  trim.m_type = MYON_BrepTrim::singular;
  trim.m_iso = iso;
  trim.m_tolerance[0] = 0.0;
  trim.m_tolerance[1] = 0.0;
  trim.m__legacy_2d_tol = 0.0;
  trim.m__legacy_3d_tol = 0.0;
  trim.m__legacy_flags_Set(-1,1);
  return trim;
}

void MYON_Brep::Append( const MYON_Brep& b )
{
  int i, j, jcnt;

  const int vcount0  = m_V.Count();
  const int ecount0  = m_E.Count();
  const int fcount0  = m_F.Count();
  const int tcount0  = m_T.Count();
  const int lcount0  = m_L.Count();
  const int c2count0 = m_C2.Count();
  const int c3count0 = m_C3.Count();
  const int scount0  = m_S.Count();

  const int vcount1  = b.m_V.Count();
  const int ecount1  = b.m_E.Count();
  const int fcount1  = b.m_F.Count();
  const int tcount1  = b.m_T.Count();
  const int lcount1  = b.m_L.Count();
  const int c2count1 = b.m_C2.Count();
  const int c3count1 = b.m_C3.Count();
  const int scount1  = b.m_S.Count();

  // need to duplicate geometry
  MYON_Object* obj;
  MYON_Curve* c;
  MYON_Surface* s;
  for ( i = 0; i < scount1; i++ ) {
    s = b.m_S[i];
    if ( s ) {
      obj = s->Duplicate();
      s = MYON_Surface::Cast(obj);
      if ( !s )
        delete obj;
    }
    m_S.Append(s);
  }
  for ( i = 0; i < c2count1; i++ ) {
    c = b.m_C2[i];
    if ( c ) {
      obj = c->Duplicate();
      c = MYON_Curve::Cast(obj);
      if ( !c )
        delete obj;
    }
    m_C2.Append(c);
  }
  for ( i = 0; i < c3count1; i++ ) {
    c = b.m_C3[i];
    if ( c ) {
      obj = c->Duplicate();
      c = MYON_Curve::Cast(obj);
      if ( !c )
        delete obj;
    }
    m_C3.Append(c);
  }

  // copy topology info
  m_V.Append( b.m_V.Count(), b.m_V.Array() );
  m_E.Append( b.m_E.Count(), b.m_E.Array() );
  m_F.Append( b.m_F.Count(), b.m_F.Array() );
  m_T.Append( b.m_T.Count(), b.m_T.Array() );
  m_L.Append( b.m_L.Count(), b.m_L.Array() );

  // update indices
  for ( i = 0; i < vcount1; i++ ) {
    MYON_BrepVertex& vertex = m_V[vcount0+i];
    if ( vertex.m_vertex_index >= 0 )
      vertex.m_vertex_index += vcount0;
    else
      vertex.m_vertex_index = -1;
    jcnt = vertex.m_ei.Count();
    for ( j = 0; j < jcnt; j++ ) {
      if ( vertex.m_ei[j] >=0 )
        vertex.m_ei[j] += ecount0;
    }
  }

  for ( i = 0; i < ecount1; i++ ) 
  {
    MYON_BrepEdge& edge = m_E[ecount0+i];
    if ( edge.m_edge_index >= 0 )
      edge.m_edge_index += ecount0;
    else
      edge.m_edge_index = -1;
    if ( edge.m_c3i >= 0 ) 
      edge.m_c3i += c3count0;
    if ( edge.m_vi[0] >= 0 )
      edge.m_vi[0] += vcount0;
    if ( edge.m_vi[1] >= 0 )
      edge.m_vi[1] += vcount0;
    jcnt = edge.m_ti.Count();
    for ( j = 0; j < jcnt; j++ ) {
      if ( edge.m_ti[j] >= 0 )
        edge.m_ti[j] += tcount0;
    }
    edge.m_brep = this;
    if (edge.m_c3i >= 0)
      edge.SetProxyCurve( m_C3[edge.m_c3i], b.m_E[i].ProxyCurveDomain() );
    else 
      edge.SetProxyCurve( 0, b.m_E[i].ProxyCurveDomain() );
    if ( b.m_E[i].ProxyCurveIsReversed() != edge.ProxyCurveIsReversed() )
      edge.MYON_CurveProxy::Reverse();
    edge.SetDomain( b.m_E[i].Domain() );
  }

  for ( i = 0; i < tcount1; i++ ) {
    MYON_BrepTrim& trim = m_T[tcount0+i];
    trim.m_brep = this;
    if ( trim.m_trim_index == i )
      trim.m_trim_index = tcount0+i;
    else
      trim.m_trim_index = -1;
    if ( trim.m_c2i >= 0 )
      trim.m_c2i += c2count0;
    if ( trim.m_ei >= 0 )
      trim.m_ei += ecount0;
    if ( trim.m_vi[0] >= 0 )
      trim.m_vi[0] += vcount0;
    if ( trim.m_vi[1] >= 0 )
      trim.m_vi[1] += vcount0;
    if ( trim.m_li >= 0 )
      trim.m_li += lcount0;
    if (trim.m_c2i >= 0)
      trim.SetProxyCurve( m_C2[trim.m_c2i], b.m_T[i].ProxyCurveDomain() );
    else 
      trim.SetProxyCurve( 0, b.m_T[i].ProxyCurveDomain() );
    if ( b.m_T[i].ProxyCurveIsReversed() != trim.ProxyCurveIsReversed() )
      trim.MYON_CurveProxy::Reverse();
    trim.SetDomain( b.m_T[i].Domain() );
  }

  for ( i = 0; i < lcount1; i++ ) 
  {
    MYON_BrepLoop& loop = m_L[lcount0+i];
    if ( loop.m_loop_index >= 0 )
      loop.m_loop_index += lcount0;
    else
      loop.m_loop_index = -1;
    jcnt = loop.m_ti.Count();
    for ( j = 0; j < jcnt; j++ ) {
      if ( loop.m_ti[j] >= 0)
        loop.m_ti[j] += tcount0;
    }
    if ( loop.m_fi >= 0 )
      loop.m_fi += fcount0;
    loop.m_brep = this;
  }

  for ( i = 0; i < fcount1; i++ ) {
    MYON_BrepFace& face = m_F[fcount0+i];
    if ( face.m_face_index >= 0 )
      face.m_face_index += fcount0;
    else
      face.m_face_index = -1;
    jcnt = face.m_li.Count();
    for ( j = 0; j < jcnt; j++ ) {
      if ( face.m_li[j] >= 0 )
        face.m_li[j] += lcount0;
    }
    if ( face.m_si >= 0 )
    {
      face.m_si += scount0;
      face.SetProxySurface(m_S[face.m_si]);
    }
    else
    {
      face.SetProxySurface( 0 );
    }
    face.m_brep = this;
  }

  //grow bounding box if possible.  otherwise invalidate it.
  if (m_bbox.IsValid() && b.BoundingBox().IsValid())
    m_bbox.Union(b.BoundingBox());
  else m_bbox.Destroy();

  m_is_solid = 0;

  DestroyMesh(MYON::any_mesh);

  return;
}

MYON_BrepLoop& 
MYON_Brep::NewLoop( MYON_BrepLoop::TYPE looptype )
{
  // 2 Sept 2020 S. Baer (RH-59952)
  // Destroy cached bounding box on breps when messing around with loops
  m_bbox.Destroy();
  m_is_solid = 0;
  int li = m_L.Count();
  m_L.Reserve(li+1);
  m_L.SetCount(li+1);
  MYON_BrepLoop& loop =  m_L.Array()[li];
  loop.m_loop_index = li;
  loop.m_type = looptype;
  loop.m_brep = this;
  return loop;
}

MYON_BrepLoop& 
MYON_Brep::NewLoop( MYON_BrepLoop::TYPE looptype, MYON_BrepFace& face )
{
  // 2 Sept 2020 S. Baer (RH-59952)
  // Destroy cached bounding box on breps when messing around with loops
  m_bbox.Destroy();
  m_is_solid = 0;
  MYON_BrepLoop& loop = NewLoop( looptype );
  loop.m_fi = face.m_face_index;
  if ( MYON_BrepLoop::outer == looptype )
  {
    // the index of the outer loop is always 
    // in face.m_li[0]
    face.m_li.Insert(0,loop.m_loop_index);
  }
  else
  {
    face.m_li.Append(loop.m_loop_index);
  }
  loop.m_brep = this;
  return loop;
}

MYON_BrepLoop* MYON_Brep::NewOuterLoop( int face_index )
{
  m_is_solid = 0;
  int vid[4] = {-1,-1,-1,-1};
  int eid[4] = {-1,-1,-1,-1};
  bool bRev3d[4] = {false, false, false, false};
  return NewOuterLoop( face_index,vid,eid,bRev3d);
}

MYON_BrepFace& MYON_Brep::NewFace( int si )
{
  m_bbox.Destroy();
  // GBA 28-MAy-2020 RH-58462.  m_bbox is now left unset after this function
  // This works since MYON_BrepFace::BoundingBox() supports lazy evaluation 
  m_is_solid = 0;
  int fi = m_F.Count();
  m_F.Reserve(fi+1);
  m_F.SetCount(fi+1);
  MYON_BrepFace& face = m_F.Array()[fi];
  face.m_face_index = fi;
  face.m_si = si;
  face.m_brep = this;
  if ( si >= 0 && si < m_S.Count() )
    face.SetProxySurface(m_S[si]);
  
  return face;
}

MYON_BrepFace* MYON_Brep::NewFace( const MYON_Surface& surface )
{
  m_bbox.Destroy();
  m_is_solid = 0;
  MYON_BrepFace* face = nullptr;
  MYON_Surface* pSurface = surface.DuplicateSurface();
  if ( pSurface )
  {
    int vid[4] = {-1,-1,-1,-1};
    int eid[4] = {-1,-1,-1,-1};
    bool bRev3d[4] = {false,false,false,false};
    face = NewFace(pSurface,vid,eid,bRev3d);
  }
  return face;
}



bool
MYON_Brep::SetTrimIsoFlags()
{
  bool rc = true;
  int fi;
  const int fcnt = m_F.Count();
  for ( fi = 0; fi < fcnt; fi++ ) {
    if ( !SetTrimIsoFlags( m_F[fi] ) )
      rc = false;
  }
  return rc;
}

bool
MYON_Brep::SetTrimIsoFlags( MYON_BrepFace& face )
{
  bool rc = true;
  int fli;
  const int face_loop_count = face.m_li.Count();
  for ( fli = 0; fli < face_loop_count; fli++ ) {
    if ( !SetTrimIsoFlags( m_L[face.m_li[fli]] ) )
      rc = false;
  }
  return rc;
}

bool
MYON_Brep::SetTrimIsoFlags( MYON_BrepLoop& loop )
{
  bool rc = true;
  int lti;
  const int loop_trim_count = loop.m_ti.Count();
  for ( lti = 0; lti < loop_trim_count; lti++ ) {
    if ( !SetTrimIsoFlags( m_T[loop.m_ti[lti]] ) )
      rc = false;
  }
  return rc;
}

bool
MYON_Brep::SetTrimIsoFlags( MYON_BrepTrim& trim )
{
  bool rc = false;
  if ( trim.m_li >= 0 && trim.m_li < m_L.Count() )
  {
    const int fi = m_L[trim.m_li].m_fi;
    if ( fi >= 0 && fi < m_F.Count() )
    {
      const MYON_Surface* pS = m_F[fi].SurfaceOf();
      if ( pS )
      {
        const MYON_Curve* pC = (trim.m_c2i >= 0 && trim.m_c2i < m_C2.Count()) 
                           ? m_C2[trim.m_c2i]
                           : 0;
        if ( pC ) 
        {
          MYON_Interval PD = trim.ProxyCurveDomain(); 
          trim.m_iso = pS->IsIsoparametric( *pC, &PD);
          rc = true;
        }
      }
    }
  }
  return rc;
}

bool
MYON_Brep::SetTrimTypeFlags( bool bLazy )
{
  bool rc = true;
  int fi;
  const int fcnt = m_F.Count();
  for ( fi = 0; fi < fcnt; fi++ ) {
    if ( !SetTrimTypeFlags( m_F[fi], bLazy ) )
      rc = false;
  }
  return rc;
}

bool
MYON_Brep::SetTrimTypeFlags( MYON_BrepFace& face, bool bLazy )
{
  bool rc = true;
  int fli;
  const int face_loop_count = face.m_li.Count();
  for ( fli = 0; fli < face_loop_count; fli++ ) {
    if ( !SetTrimTypeFlags( m_L[face.m_li[fli]], bLazy ) )
      rc = false;
  }
  return rc;
}

bool
MYON_Brep::SetTrimTypeFlags( MYON_BrepLoop& loop, bool bLazy )
{
  bool rc = true;
  int lti;
  const int loop_trim_count = loop.m_ti.Count();
  for ( lti = 0; lti < loop_trim_count; lti++ ) {
    if ( !SetTrimTypeFlags( m_T[loop.m_ti[lti]], bLazy ) )
      rc = false;
  }
  return rc;
}

MYON_BrepTrim::TYPE MYON_Brep::TrimType( const MYON_BrepTrim& trim, bool bLazy ) const
{
  MYON_BrepTrim::TYPE trim_type = bLazy ? trim.m_type : MYON_BrepTrim::unknown;
  int eti, other_ti;

  if ( trim_type == MYON_BrepTrim::unknown && trim.m_li >= 0 && trim.m_li < m_L.Count() ) 
  {
    const MYON_BrepLoop& loop = m_L[trim.m_li];
    if ( loop.m_type == MYON_BrepLoop::ptonsrf )
      trim_type = MYON_BrepTrim::ptonsrf;
    else if (loop.m_type == MYON_BrepLoop::crvonsrf )
      trim_type = MYON_BrepTrim::crvonsrf;
    else if ( trim.m_ei == -1 ) 
    {
      trim_type = MYON_BrepTrim::singular;
    }
    else if ( trim.m_ei >= 0 && trim.m_ei < m_E.Count() ) 
    {
      const MYON_BrepEdge& edge = m_E[trim.m_ei];
      if ( edge.m_ti.Count() == 1 && edge.m_ti[0] == trim.m_trim_index ) 
      {
        trim_type = MYON_BrepTrim::boundary;
      }
      else if ( edge.m_ti.Count() > 1 ) 
      {
        trim_type = MYON_BrepTrim::mated;
        // check for seam
        for ( eti = 0; eti < edge.m_ti.Count(); eti++ ) 
        {
          other_ti = edge.m_ti[eti];
          if ( other_ti != trim.m_trim_index && other_ti >= 0 && other_ti < m_T.Count() ) 
          {
            if ( m_T[other_ti].m_li == trim.m_li ) 
            {
              trim_type = MYON_BrepTrim::seam;
              break;
            }
          }
        }
      }
    }
  }
  return trim_type;
}

bool
MYON_Brep::SetTrimTypeFlags( MYON_BrepTrim& trim, bool bLazy )
{
  if ( !bLazy || trim.m_type == MYON_BrepTrim::unknown)
    trim.m_type = TrimType(trim,false);
  return ((trim.m_type != MYON_BrepTrim::unknown)?true:false);
}

bool
MYON_Brep::GetTrim2dStart(int trim_index,
                        MYON_2dPoint& P
                        ) const

{
  if (trim_index < 0 || trim_index >= m_T.Count())
    return false;
  const MYON_BrepTrim& trim = m_T[trim_index];
  MYON_3dPoint pp;
  if (!trim.EvPoint(trim.Domain()[0], pp)) 
    return false;
  P = pp;
  return true;
}

bool
MYON_Brep::GetTrim2dEnd(int trim_index,
                        MYON_2dPoint& P
                        ) const

{
  if (trim_index < 0 || trim_index >= m_T.Count())
    return false;
  const MYON_BrepTrim& trim = m_T[trim_index];
  MYON_3dPoint pp;
  if (!trim.EvPoint(trim.Domain()[1], pp))
    return false;
  P = pp;
  return true;
}

bool
MYON_Brep::GetTrim3dStart(int trim_index,
                        MYON_3dPoint& P
                        ) const
{
  const MYON_Surface* srf = nullptr;
  MYON_3dPoint uv(MYON_3dPoint::NanPoint);
  if ( trim_index >= 0 && trim_index < m_T.Count() )
  {
    const MYON_BrepTrim& trim = m_T[trim_index];
    if ( trim.m_li >= 0 && trim.m_li < m_L.Count() )
    {
      const int fi = m_L[trim.m_li].m_fi;
      if ( fi >= 0 && fi < m_F.Count() )
      {
        if ( trim.Evaluate(trim.Domain()[0],0,3,&uv.x) )
        {
          srf = m_F[fi].SurfaceOf();
        }
      }
    }
  }
  return (nullptr != srf && srf->EvPoint(uv.x, uv.y, P) ? true : false);
}

bool
MYON_Brep::GetTrim3dEnd(int trim_index,
                        MYON_3dPoint& P
                        ) const

{
  const MYON_Surface* srf = nullptr;
  MYON_3dPoint uv(MYON_3dPoint::NanPoint);
  if ( trim_index >= 0 && trim_index < m_T.Count() )
  {
    const MYON_BrepTrim& trim = m_T[trim_index];
    if ( trim.m_li >= 0 && trim.m_li < m_L.Count() )
    {
      const int fi = m_L[trim.m_li].m_fi;
      if ( fi >= 0 && fi < m_F.Count() )
      {
        if ( trim.Evaluate(trim.Domain()[1],0,3,&uv.x) )
        {
          srf = m_F[fi].SurfaceOf();
        }
      }
    }
  }
  return (nullptr != srf && srf->EvPoint(uv.x, uv.y, P) ? true : false);
}


MYON_BrepLoop::TYPE 
MYON_Brep::ComputeLoopType( const MYON_BrepLoop& loop ) const
{
  // This function must always compute the type from the
  // 2d trim geometry.  NEVER modify this function to
  // return the input value of loop.m_type.

  MYON_BrepLoop::TYPE loop_type = MYON_BrepLoop::unknown;

  int loop_dir =  LoopDirection( loop );
  if ( 1 == loop_dir )
    loop_type = MYON_BrepLoop::outer;
  else if ( -1 == loop_dir )
    loop_type = MYON_BrepLoop::inner;

  // TODO check for gaps, slits, etc.

  /*
  int ugap_count = 0;
  int vgap_count = 0;
  double d, utol0, vtol0, loop_start_utol, loop_start_vtol;
  MYON_3dPoint p0, p1, loop_start;
  MYON_Interval srf_domain[2];
  if ( loop.m_fi >= 0 && loop.m_fi < m_F.Count() ) {
    const MYON_BrepFace& face = m_F[loop.m_fi];
    if ( face.m_si >= 0 && face.m_si < m_S.Count() ) {
      MYON_Surface* srf = m_S[face.m_si];
      srf_domain[0] = srf->Domain(0);
      srf_domain[1] = srf->Domain(1);
    }
  }
  const MYON_2dPoint basePt( srf_domain[0].ParameterAt(0.5), srf_domain[1].ParameterAt(0.5) );

  const int trim_count = loop.m_ti.Count();
  for ( lti = 0; lti < trim_count; lti++ ) 
  {
    ti = loop.m_ti[lti];
    const MYON_BrepTrim& trim = m_T[ti];
    p0 = trim.PointAtStart();
    u_tol0 = trim.m_tolerance[0];
    v_tol0 = trim.m_tolerance[1];
    if ( !lti ) 
    {
      loop_start = p0;
      loop_start_utol = trim.m_tolerance[0];
      loop_start_vtol = trim.m_tolerance[1];
    }
    else 
    {
      d = fabs(p0.x-p1.x);
      if ( d > utol0 + trim.m_tolerance[0] )
        ugap_count++;
      d = fabs(p0.y-p1.y);
      if ( d > vtol0 + trim.m_tolerance[1] )
        vgap_count++;
    }
    p1 = c2->PointAtEnd();
  }
  */

  return loop_type;
}


bool
MYON_Brep::IsValidTrim( int trim_index, MYON_TextLog* text_log ) const
{
  if ( trim_index < 0 || trim_index >= m_T.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep trim_index = %d (should be >=0 and <%d=brep.m_T.Count()).\n",
                      trim_index, m_T.Count());
    }
    return MYON_BrepIsNotValid();
  }
  const MYON_BrepTrim& trim = m_T[trim_index];
  if ( trim.m_trim_index != trim_index )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_trim_index = %d (should be %d).\n",
                       trim.m_trim_index, trim_index );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( !trim.IsValid(text_log) )
  {
    if ( text_log )
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
    return MYON_BrepIsNotValid();
  }
  if ( trim.m_c2i < 0 || trim.m_c2i >= m_C2.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print( "trim.m_c2i = %d (should be >=0 and <%d).\n", trim.m_c2i, 0, m_C2.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  const MYON_Curve* pC = m_C2[trim.m_c2i];
  if ( !pC )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_c2i = %d and MYON_Brep.m_C2[%d] is nullptr\n", trim.m_c2i, trim.m_c2i );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  int c2_dim = pC->Dimension();
  if ( c2_dim != 2 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_c2i = %d and MYON_Brep.m_C2[%d]->Dimension() = %d (should be 2).\n", trim.m_c2i, trim.m_c2i, c2_dim );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if ( pC != trim.ProxyCurve() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.ProxyCurve() != m_C2[trim.m_c2i].\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  //if ( trim.ProxyCurveIsReversed() )
  //{
  //  if ( text_log )
  //  {
  //    text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
  //    text_log->PushIndent();
  //    text_log->Print("trim.ProxyCurveIsReversed() is true\n");
  //    text_log->PopIndent();
  //  }
  //  return MYON_BrepIsNotValid();
  //}

  MYON_Interval trim_domain = trim.Domain();
  MYON_Interval c2_domain = pC->Domain();
  if ( !trim_domain.IsIncreasing() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.Domain() = (%g,%g) (should be an increasing interval).\n", trim_domain[0], trim_domain[1] );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( !c2_domain.Includes(trim_domain) )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.Domain() = (%g,%g) is not included in brep.m_C2[trim.m_c2i=%d]->Domain() = (%g,%g)\n",
                      trim_domain[0], trim_domain[1], trim.m_c2i, c2_domain[0], c2_domain[1] );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  int vi0 = trim.m_vi[0];
  int vi1 = trim.m_vi[1];
  if ( vi0 < 0 || vi0 >= m_V.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_vi[0] = %d (should be >= 0 and < %d=brep.m_V.Count()).\n",
                       trim_index, vi0, m_V.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( vi1 < 0 || vi1 >= m_V.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_vi[1] = %d (should be >= 0 and < %d=brep.m_V.Count()).\n",
                       trim_index, vi1, m_V.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  const int ei = trim.m_ei;
  int trim_eti = -1;
  if ( trim.m_type == MYON_BrepTrim::singular ) 
  {
    // singular trim - no edge and 3d v0 = 3d v1
    if ( ei != -1 )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = singular but trim.m_ei = %d (should be -1)\n",ei);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( vi0 != vi1 )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = singular but trim.m_vi[] = [%d,%d] (the m_vi[] values should be equal).\n",
                        vi0,vi1);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( pC->IsClosed() )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = singular but brep.m_C2[trim.m_c2i=%d]->IsClosed() is true.\n",
                        trim.m_c2i,trim.m_c2i);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
  }
  else if ( trim.m_type != MYON_BrepTrim::ptonsrf )
  {
    // non-singular non-ptonsrf trim must have valid edge
    if ( ei < 0 || ei >= m_E.Count() )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type != singular and trim.m_ei = %d (m_ei should be >=0 and <brep.m_E.Count()=%d\n",
                        trim.m_ei,m_E.Count());
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    const MYON_BrepEdge& edge = m_E[ei];
    if ( edge.m_vi[trim.m_bRev3d?1:0] != vi0 )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_vi[0] != brep.m_E[trim.m_ei=%d].m_vi[trim.m_bRev3d?1:0]\n",ei);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( edge.m_vi[trim.m_bRev3d?0:1] != vi1 )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_vi[1] != brep.m_E[trim.m_ei=%d].m_vi[trim.m_bRev3d?0:1]\n",ei);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( trim_domain == c2_domain && pC->IsClosed() ) 
    {
      // (open 2d trims can still have vi0 = vi1 on closed surfaces)
      if ( vi0 != vi1 )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_vi[] = [%d,%d] but brep.m_C2[trim.m_c2i=%d]->IsClosed()=true\n",
                          vi0, vi1, trim.m_c2i );
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
    else if ( vi0 == vi1 )
    {
      // TODO: check that trim start/end is a closed surface seam point.
    }
    else
    {
      // vi0 != vi1
      // TODO: check that trim start/end is not a closed surface seam point.
    }
    int i;
    for ( i = 0; i < edge.m_ti.Count(); i++ ) 
    {
      if ( edge.m_ti[i] == trim_index )
      {
        trim_eti = i;
        break;
      }
    }

    if ( trim_eti < 0 )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim index %d is not in brep.m_E[trim.m_ei=%d].m_ti[]\n",
                        trim_index, trim.m_ei );
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }

    if ( edge.m_ti.Count() == 2 )
    {
      int other_ti = edge.m_ti[ (edge.m_ti[0]==trim_index)?1:0 ];
      if ( other_ti >= 0 && other_ti < m_T.Count() && other_ti != trim_index )
      {
        const MYON_BrepTrim& other_trim = m_T[other_ti];
        if ( other_trim.m_li == trim.m_li )
        {
          if ( trim.m_type != MYON_BrepTrim::seam )
          {
            if ( text_log )
            {
              text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
              text_log->PushIndent();
              text_log->Print("trim.m_type!=seam but brep.m_E[trim.m_ei=%d] references two trims in loop trim.m_li=%d.\n",
                              trim.m_ei,trim.m_li);
              text_log->PopIndent();
            }
            return MYON_BrepIsNotValid();
          }
        }
      }
    }
  }
  if ( trim.m_li < 0 || trim.m_li >= m_L.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_li = %d (should be >= 0 and <brep.m_L.Count()=%d\n", trim.m_li,m_L.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if ( trim.m_ei >= 0 && trim_eti < 0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("brep.m_E[trim.m_ei=%d].m_ti[] does not reference the trim.\n",trim.m_ei);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }


  switch ( trim.m_type )
  {
  case MYON_BrepTrim::unknown:
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = unknown (should be set to the correct MYON_BrepTrim::TYPE value)\n");
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    break;
  case MYON_BrepTrim::boundary:
    {
      const MYON_BrepLoop& loop = m_L[trim.m_li];
      const MYON_BrepEdge& edge = m_E[trim.m_ei];
      if ( edge.m_ti.Count() > 1 )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_type = boundary but brep.m_E[trim.m_ei=%d] has 2 or more trims.\n",trim.m_ei);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
      if ( loop.m_type != MYON_BrepLoop::outer && loop.m_type != MYON_BrepLoop::inner )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_type = boundary but brep.m_L[trim.m_li=%d].m_type is not inner or outer.\n",trim.m_li);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
    break;
  case MYON_BrepTrim::mated:
    {
      const MYON_BrepLoop& loop = m_L[trim.m_li];
      const MYON_BrepEdge& edge = m_E[trim.m_ei];
      if ( edge.m_ti.Count() < 2 )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_type = mated but brep.m_E[trim.m_ei=%d] only references this trim.\n",trim.m_ei);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
      if ( loop.m_type != MYON_BrepLoop::outer && loop.m_type != MYON_BrepLoop::inner )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_type = mated but brep.m_L[trim.m_li=%d].m_type is not inner or outer.\n",trim.m_li);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
    break;
  case MYON_BrepTrim::seam:
    {
      const MYON_BrepLoop& loop = m_L[trim.m_li];
      const MYON_BrepEdge& edge = m_E[trim.m_ei];
      if ( edge.m_ti.Count() < 2 )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_type = seam but brep.m_E[trim.m_ei=%d] < 2.\n",trim.m_ei);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
      int other_ti = -1;
      for ( int eti = 0; eti < edge.m_ti.Count(); eti++ ) 
      {
        if ( trim_eti == eti )
          continue;
        int i = edge.m_ti[eti];
        if ( i == trim_index )
        {
          if ( text_log )
          {
            text_log->Print("brep.m_E[%d] trim is not valid.\n",trim.m_ei);
            text_log->PushIndent();
            text_log->Print("edge.m_ti[%d] = m_ti[%d] = %d.\n",trim_eti,eti,trim_index);
            text_log->PopIndent();
          }
          return MYON_BrepIsNotValid();
        }

        if ( i < 0 || i >= m_T.Count() )
        {
          if ( text_log )
          {
            text_log->Print("brep.m_E[%d] trim is not valid.\n",trim.m_ei);
            text_log->PushIndent();
            text_log->Print("edge.m_ti[%d]=%d is not a valid m_T[] index.\n",eti,i);
            text_log->PopIndent();
          }
          return MYON_BrepIsNotValid();
        }

        const MYON_BrepTrim& other_trim = m_T[i];
        if ( other_trim.m_type == MYON_BrepTrim::seam && other_trim.m_li == trim.m_li )
        {
          if ( other_ti < 0 )
            other_ti = i;
          else
          {
            if ( text_log )
            {
              text_log->Print("brep.m_T[%d,%d, or %d] trim is not valid.\n",trim_index,other_ti,i);
              text_log->PushIndent();
              text_log->Print("All three trims have m_type = seam m_ei=%d and m_li = %d.\n",trim.m_ei,trim.m_li);
              text_log->PopIndent();
            }
            return MYON_BrepIsNotValid();
          }
        }
      }

      if ( other_ti < 0 )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_type = seam but its other trim is not in the loop.\n");
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }

      if ( loop.m_type != MYON_BrepLoop::outer && edge.m_ti.Count() <= 2 )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
          text_log->PushIndent();
          text_log->Print("trim.m_type = seam, the edge is manifold, but brep.m_L[trim.m_li=%d].m_type is not outer.\n",trim.m_li);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
      // 31 Jan 2002 - The definition of a seam trim is a trim that is connected to
      //               an edge, is part of loop, and exactly one other trim in the
      //               same loop is connected to the same edge.  This can happen
      //               on the interior of a surface (like an annulus in a plane)
      //               and on non-manifold edges.
      //if ( trim.m_iso != MYON_Surface::W_iso && trim.m_iso != MYON_Surface::N_iso &&
      //     trim.m_iso != MYON_Surface::E_iso && trim.m_iso != MYON_Surface::S_iso )
      //{
      //  if ( text_log )
      //  {
      //    text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      //    text_log->PushIndent();
      //    text_log->Print("trim.m_type = seam but trim.m_iso != N/S/E/W_iso\n");
      //    text_log->PopIndent();
      //  }
      //  return MYON_BrepIsNotValid();
      //}
    }
    break;
  case MYON_BrepTrim::singular:
    // most requirements are checked above
    if ( trim.m_iso != MYON_Surface::W_iso && trim.m_iso != MYON_Surface::N_iso &&
         trim.m_iso != MYON_Surface::E_iso && trim.m_iso != MYON_Surface::S_iso )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = singular but trim.m_iso != N/S/E/W_iso\n");
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    break;
  case MYON_BrepTrim::crvonsrf:
    {
      const MYON_BrepLoop& loop = m_L[trim.m_li];
      if ( loop.m_type != MYON_BrepLoop::crvonsrf )
      {
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_c2i < 0 || trim.m_c2i >= m_C2.Count() )
      {
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_ei < 0 || trim.m_ei >= m_E.Count() )
      {
        return MYON_BrepIsNotValid();
      }
    }
    break;
  case MYON_BrepTrim::ptonsrf:
    {
      const MYON_BrepLoop& loop = m_L[trim.m_li];
      if ( loop.m_type != MYON_BrepLoop::ptonsrf )
      {
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_ei != -1 )
      {
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_c2i != -1 )
      {
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_pbox.m_min.x != trim.m_pbox.m_max.x || trim.m_pbox.m_min.y != trim.m_pbox.m_max.y || trim.m_pbox.m_min.z != trim.m_pbox.m_max.z ) 
      {
        // m_pbox must be a single point that defines surface parameters of the point.
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_pbox.m_min.x == MYON_UNSET_VALUE || trim.m_pbox.m_min.y == MYON_UNSET_VALUE || trim.m_pbox.m_min.z != 0.0 ) 
      {
        // m_pbox must be a single point that defines surface parameters of the point.
        return MYON_BrepIsNotValid();
      }
    }
    break;

  case MYON_BrepTrim::slit:
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = MYON_BrepTrim::slit (should be set to the correct MYON_BrepTrim::TYPE value)\n");
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    break;

  case MYON_BrepTrim::trim_type_count:
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = type_count (should be set to the correct MYON_BrepTrim::TYPE value)\n");
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    break;

  default:
    {
      if ( text_log )
      {
        text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
        text_log->PushIndent();
        text_log->Print("trim.m_type = garbage (should be set to the correct MYON_BrepTrim::TYPE value)\n");
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    break;
  }

  if ( trim.m_tolerance[0] < 0.0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_tolerance[0] = %g (should be >= 0.0)\n",trim.m_tolerance[0]);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( trim.m_tolerance[1] < 0.0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_tolerance[1] = %g (should be >= 0.0)\n",trim.m_tolerance[1]);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if ( !trim.m_pbox.IsValid() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_pbox is not valid.\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if ( trim.m_brep != this )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_T[%d] trim is not valid.\n",trim_index);
      text_log->PushIndent();
      text_log->Print("trim.m_brep does not point to parent brep.\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  return true;
}

bool
MYON_Brep::IsValidLoop( int loop_index, MYON_TextLog* text_log  ) const
{
  if ( loop_index < 0 || loop_index >= m_L.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep loop_index = %d (should be >=0 and <%d=brep.m_L.Count()).\n",
                      loop_index, m_L.Count());
    }
    return MYON_BrepIsNotValid();
  }
  const MYON_BrepLoop& loop = m_L[loop_index];
  if ( loop.m_loop_index != loop_index )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
      text_log->PushIndent();
      text_log->Print("loop.m_loop_index = %d (should be %d).\n",
                       loop.m_loop_index, loop_index );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( loop.m_fi < 0 || loop.m_fi >= m_F.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
      text_log->PushIndent();
      text_log->Print("loop.m_fi = %d (should be >= 0 and <brep.m_F.Count()=%d\n", loop.m_fi, m_F.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  const int loop_trim_count = loop.m_ti.Count();
  if ( loop_trim_count <= 0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
      text_log->PushIndent();
      text_log->Print("loop.m_ti.Count() is <= 0  (should be > 0)\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if (    loop.m_type != MYON_BrepLoop::outer 
       && loop.m_type != MYON_BrepLoop::inner 
       && loop.m_type != MYON_BrepLoop::slit 
       )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
      text_log->PushIndent();
      text_log->Print("loop.m_type = %d (must be %d=outer, %d=inner, or %d=slit)\n",
                      loop.m_type,MYON_BrepLoop::outer,MYON_BrepLoop::inner,MYON_BrepLoop::slit);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }


  if ( loop.m_brep != this )
  {
    if ( text_log )
    {
      text_log->Print("loop.m_L[%d] loop is not valid.\n",loop_index);
      text_log->PushIndent();
      text_log->Print("loop.m_brep does not point to parent brep.\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  // make sure trims are valid
  int i, lti, ti;
  for ( lti = 0; lti < loop_trim_count; lti++ ) {
    ti = loop.m_ti[lti];
    for ( i = 0; i < lti; i++ ) {
      if ( loop.m_ti[i] == ti )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
          text_log->PushIndent();
          text_log->Print("loop.m_ti[%d] = loop.m_ti[%d] = %d (trim index can only appear once)\n",
                          lti, i, ti);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
    if ( !IsValidTrim( ti, text_log ) )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
        text_log->PushIndent();
        text_log->Print("brep.m_T[loop.m_ti[%d]=%d] is not valid.\n",
                        lti, ti);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( m_T[ti].m_li != loop_index )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
        text_log->PushIndent();
        text_log->Print("brep.m_T[loop.m_ti[%d]=%d].m_li=%d (m_li should be %d).\n",
                        lti, ti, m_T[ti].m_li, loop_index );
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
  }


  if ( MYON_BrepLoop::slit == loop.m_type )
  {
    if ( loop.m_ti.Count() < 2 || 0 != (loop.m_ti.Count() % 2) )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
        text_log->PushIndent();
        text_log->Print("loop.m_type = slit but loop has %d trims\n",loop.m_ti.Count());
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }

    for ( int lti_for_loop = 0; lti_for_loop < loop.m_ti.Count(); lti_for_loop++ )
    {
      int ti_for_loop = loop.m_ti[lti_for_loop];
      const MYON_BrepTrim& trim = m_T[ti_for_loop];
      if ( trim.m_type != MYON_BrepTrim::seam )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_L[%d] slit loop is not valid.\n",loop_index);
          text_log->PushIndent();
          text_log->Print("brep.m_T[loop.m_ti[%d]=%d].m_type = %d (should be %d = seam)\n",
                          lti_for_loop,ti_for_loop,trim.m_type,MYON_BrepTrim::seam);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
      switch( trim.m_iso )
      {
      case MYON_Surface::W_iso:
      case MYON_Surface::E_iso:
      case MYON_Surface::S_iso:
      case MYON_Surface::N_iso:
        {
          if ( text_log )
          {
            text_log->Print("brep.m_L[%d] slit loop is not valid.\n",loop_index);
            text_log->PushIndent();
            text_log->Print("brep.m_T[loop.m_ti[%d]=%d].m_iso = E/W/N/S_iso (should be interior)\n",
                            lti_for_loop,ti_for_loop);
            text_log->PopIndent();
          }
          return MYON_BrepIsNotValid();
        }
        break;

      case MYON_Surface::not_iso:
      case MYON_Surface::x_iso:
      case MYON_Surface::y_iso:
      case MYON_Surface::iso_count:
        break;
      }
    }   
  }


  // make sure ends of trims jibe
  int ci0, ci1, next_lti;
  MYON_3dPoint P0, P1;
  const MYON_Curve *pC0, *pC1;
  for ( lti = 0; lti < loop_trim_count; lti++ ) 
  {
    //double x_tol = MYON_ZERO_TOLERANCE;
    //double y_tol = MYON_ZERO_TOLERANCE;
    const MYON_BrepTrim& trim0 = m_T[loop.m_ti[lti]];
    next_lti = (lti+1)%loop_trim_count;
    const MYON_BrepTrim& trim1 = m_T[loop.m_ti[next_lti]];
    MYON_Interval trim0_domain = trim0.Domain();
    MYON_Interval trim1_domain = trim1.Domain();
    ci0 = trim0.m_c2i;
    ci1 = trim1.m_c2i;
    pC0 = m_C2[ci0];
    pC1 = m_C2[ci1];
    P0 = pC0->PointAt( trim0_domain[1] ); // end of this 2d trim
    P1 = pC1->PointAt( trim1_domain[0] ); // start of next 2d trim
    if ( !(P0-P1).IsTiny() )
    {
      // 16 September 2003 Dale Lear - RR 11319
      //    Added relative tol check so cases with huge
      //    coordinate values that agreed to 10 places
      //    didn't get flagged as bad.
      double xtol = (fabs(P0.x) + fabs(P1.x))*1.0e-10;
      double ytol = (fabs(P0.y) + fabs(P1.y))*1.0e-10;
      if ( xtol < MYON_ZERO_TOLERANCE )
        xtol = MYON_ZERO_TOLERANCE;
      if ( ytol < MYON_ZERO_TOLERANCE )
        ytol = MYON_ZERO_TOLERANCE;
      double dx = fabs(P0.x-P1.x);
      double dy = fabs(P0.y-P1.y);
      if ( dx > xtol || dy > ytol )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
          text_log->PushIndent();
          text_log->Print("end of brep.m_T[loop.m_ti[%d]=%d]=(%g,%g) and start \n", lti, loop.m_ti[lti],P0.x,P0.y);
          text_log->Print("of brep.m_T[loop.m_ti[%d]=%d]=(%g,%g) do not match.\n",next_lti, loop.m_ti[next_lti],P1.x,P1.y);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
  }

  if ( !loop.m_pbox.IsValid() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_L[%d] loop is not valid.\n",loop_index);
      text_log->PushIndent();
      text_log->Print("loop.m_pbox is not valid\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  return true; 
}


bool
MYON_Brep::IsValidFace( int face_index, MYON_TextLog* text_log  ) const
{
  if ( face_index < 0 || face_index >= m_F.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep face_index = %d (should be >=0 and <%d=brep.m_F.Count()).\n",
                      face_index, m_F.Count());
    }
    return MYON_BrepIsNotValid();
  }
  const MYON_BrepFace& face = m_F[face_index];
  if ( face.m_face_index != face_index )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
      text_log->PushIndent();
      text_log->Print("face.m_face_index = %d (should be %d).\n",
                       face.m_face_index, face_index );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( face.m_brep != this )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
      text_log->PushIndent();
      text_log->Print("face.m_brep does not point to parent brep.\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  const int face_loop_count = face.m_li.Count();
  if ( face_loop_count <= 0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
      text_log->PushIndent();
      text_log->Print("face.m_li.Count() <= 0 (should be >= 1)\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  int i, fli, li;
  for ( fli = 0; fli < face_loop_count; fli++ ) 
  {
    li = face.m_li[fli];
    for ( i = 0; i < fli; i++ ) 
    {
      if ( face.m_li[i] == li )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
          text_log->PushIndent();
          text_log->Print("face.m_li[%d]=face.m_li[%d]=%d (a loop index should appear once in face.m_li[])\n",
                          fli,i,li);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
    if ( !IsValidLoop( li, text_log ) )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
        text_log->PushIndent();
        text_log->Print("brep.m_L[face.m_li[%d]=%d] is not valid.\n",fli,li);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    const MYON_BrepLoop& loop = m_L[li];
    if ( loop.m_loop_index != li )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
        text_log->PushIndent();
        text_log->Print("face.m_li[%d]=%d is a deleted loop\n",
                        fli,li);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( loop.m_fi != face_index )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
        text_log->PushIndent();
        text_log->Print("face.m_li[%d]=%d but brep.m_L[%d].m_fi=%d (m_fi should be %d)\n",
                        fli,li,li,loop.m_fi,face_index);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( fli == 0 ) 
    {
      if ( loop.m_type != MYON_BrepLoop::outer )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
          text_log->PushIndent();
          text_log->Print("brep.m_L[face.m_li[0]=%d].m_type is not outer.\n",li);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
    else
    {
      if (   loop.m_type != MYON_BrepLoop::slit 
           && loop.m_type != MYON_BrepLoop::inner 
         )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
          text_log->PushIndent();
          text_log->Print("brep.m_L[face.m_li[%d]=%d].m_type is not inner or slit.\n",fli,li);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
  }

  const int si = face.m_si;
  if ( si < 0 || si >= m_S.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
      text_log->PushIndent();
      text_log->Print("face.m_si=%d (should be >=0 and <%d=m_S.Count())\n",
                      face.m_si,m_S.Count());                      
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if ( !m_S[si] )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
      text_log->PushIndent();
      text_log->Print("brep.m_S[face.m_si=%d] is nullptr\n",face.m_si);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if ( m_S[si] != face.ProxySurface() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
      text_log->PushIndent();
      text_log->Print("brep.m_S[face.m_si=%d] != face.ProxySurface().\n",si);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  if ( face.ProxySurfaceIsTransposed() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_F[%d] face is not valid.\n",face_index);
      text_log->PushIndent();
      text_log->Print("face.ProxySurfaceIsTransposed() is true.\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  return true; 
}

bool
MYON_Brep::IsValidEdge( int edge_index, MYON_TextLog* text_log ) const
{
  if ( edge_index < 0 || edge_index >= m_E.Count() )
  {
    if ( text_log )
      text_log->Print("brep edge_index = %d (should be >=0 and <%d=brep.m_E.Count() ).\n",
                      edge_index, m_E.Count());
    return MYON_BrepIsNotValid();
  }
  const MYON_BrepEdge& edge = m_E[edge_index];
  if ( edge.m_brep != this )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_brep does not point to parent brep\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( edge.m_edge_index != edge_index )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_edge_index = %d (should be %d).\n",
                       edge.m_edge_index, edge_index );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( !edge.IsValid(text_log) )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge is not a valid.\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  const int ci = edge.m_c3i;
  if ( ci < 0 || ci >= m_C3.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_c3i = %d (should be >=0 and <%d=m_C3.Count()\n",
                      edge.m_c3i,m_C3.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  
  if ( m_C3[ci] != edge.ProxyCurve() || 0 == m_C3[ci] )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_curve != brep.m_C3[edge.m_c3i=%d]\n", edge.m_c3i );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  
  //if ( edge.ProxyCurveIsReversed() )
  //{
  //  if ( text_log )
  //  {
  //    text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
  //    text_log->PushIndent();
  //    text_log->Print("edge.ProxyCurveIsReversed() is true.\n" );
  //    text_log->PopIndent();
  //  }
  //  return MYON_BrepIsNotValid();
  //}

  double t0, t1;
  if ( !edge.GetDomain( &t0, &t1 ) )
  {
    if ( text_log )
    {
      MYON_Interval edom = edge.ProxyCurveDomain();
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print( "edge.m_domain=(%g,%g) is not valid\n", edom[0], edom[1]);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  const int vi0 = edge.m_vi[0];
  const int vi1 = edge.m_vi[1];
  if ( vi0 < 0 || vi0 >= m_V.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_vi[0]=%d (should be >=0 and <%d=m_V.Count()\n",
                       vi0, m_V.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  if ( vi1 < 0 || vi1 >= m_V.Count() )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_vi[1]=%d (should be >=0 and <%d=m_V.Count()\n",
                       vi1, m_V.Count() );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  int evi;
  for ( evi = 0; evi < 2; evi++ ) 
  {
    const MYON_BrepVertex& vertex = m_V[edge.m_vi[evi]];

    if ( edge.m_vi[evi] != vertex.m_vertex_index )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
        text_log->PushIndent();
        text_log->Print("edge.m_vi[%d]=%d is a deleted vertex\n",
                         evi,edge.m_vi[evi] );
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }


    const int vertex_edge_count = vertex.m_ei.Count();
    bool bFoundIt = false;
    int vei;
    for ( vei = 0; vei < vertex_edge_count && !bFoundIt; vei++ ) {
      bFoundIt = (vertex.m_ei[vei] == edge_index);
    }
    if ( !bFoundIt )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
        text_log->PushIndent();
        text_log->Print("edge.m_vi[%d]=%d but edge is not referenced in m_V[%d].m_ei[]\n",
                         evi,edge.m_vi[evi],edge.m_vi[evi] );
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
  }

  if ( edge.IsClosed() ) {
    if ( vi0 != vi1 )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
        text_log->PushIndent();
        text_log->Print("edge.m_vi[]=(%d,%d) but edge.IsClosed() is true\n",
                         vi0,vi1);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
  }
  else {
    if ( vi0 == vi1 )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
        text_log->PushIndent();
        text_log->Print("edge.m_vi[0]=edge.m_vi[1]=%d but edge.IsClosed() is false.\n",
                         vi0);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
  }

  const int edge_trim_count = edge.m_ti.Count();
  if ( edge_trim_count < 0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_ti.Count() < 0\n");
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  int i, eti, ti;
  for (eti = 0; eti < edge_trim_count; eti++ )
  {
    ti = edge.m_ti[eti];
    if ( ti < 0 || ti >= m_T.Count() )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
        text_log->PushIndent();
        text_log->Print("edge.m_ti[%d]=%d (should be >=0 and <%d=m_T.Count())\n",eti,ti);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    if ( m_T[ti].m_trim_index != ti )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
        text_log->PushIndent();
        text_log->Print("edge.m_ti[%d]=%d is a deleted trim\n",eti,ti);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    for ( i = 0; i < eti; i++ ) 
    {
      if ( edge.m_ti[i] == ti )
      {
        if ( text_log )
        {
          text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
          text_log->PushIndent();
          text_log->Print("edge.m_ti[%d]=edge.m_ti[%d]=%d (a trim should be referenced once).\n",i,eti,ti);
          text_log->PopIndent();
        }
        return MYON_BrepIsNotValid();
      }
    }
    const MYON_BrepTrim& trim = m_T[ti];
    if ( trim.m_ei != edge_index )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
        text_log->PushIndent();
        text_log->Print("edge.m_ti[%d]=%d but brep.m_T[%d].m_ei=%d\n",eti,ti,ti,trim.m_ei);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
  }

  if ( edge.m_tolerance < 0.0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_E[%d] edge is not valid.\n",edge_index);
      text_log->PushIndent();
      text_log->Print("edge.m_tolerance=%g (should be >= 0.0)\n",edge.m_tolerance);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  return true;
}

bool
MYON_Brep::IsValidVertex( int vertex_index, MYON_TextLog* text_log ) const
{
  if ( vertex_index < 0 || vertex_index >= m_V.Count() )
  {
    if ( text_log )
      text_log->Print("brep vertex_index = %d (should be >=0 and <%d=brep.m_V.Count() ).\n",
                      vertex_index, m_V.Count());
    return MYON_BrepIsNotValid();
  }
  const MYON_BrepVertex& vertex = m_V[vertex_index];
  if ( vertex.m_vertex_index != vertex_index )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_V[%d] vertex is not valid.\n",vertex_index);
      text_log->PushIndent();
      text_log->Print("vertex.m_vertex_index = %d (should be %d).\n",
                       vertex.m_vertex_index, vertex_index );
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }

  const int vertex_edge_count = vertex.m_ei.Count();
  int i, j, vei, ei;
  for ( vei = 0; vei < vertex_edge_count; vei++ ) {
    ei = vertex.m_ei[vei];
    if ( ei < 0 || ei >= m_E.Count() )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_V[%d] vertex is not valid.\n",vertex_index);
        text_log->PushIndent();
        text_log->Print("vertex.m_ei[%d] = %d (should be >=0 and <%d).\n", vei, ei, m_E.Count());
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    const MYON_BrepEdge& edge = m_E[ei];
    if ( ei != edge.m_edge_index )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_V[%d] vertex is not valid.\n",vertex_index);
        text_log->PushIndent();
        text_log->Print("vertex.m_ei[%d] = %d is a deleted edge.\n", vei, ei);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
    for ( i = 0; i < vei; i++ ) 
    {
      if ( vertex.m_ei[i] == ei ) 
      {
        // edge should be closed
        if ( edge.m_vi[0] != vertex_index || edge.m_vi[1] != vertex_index )
        {
          if ( text_log )
          {
            text_log->Print("brep.m_V[%d] vertex is not valid.\n",vertex_index);
            text_log->PushIndent();
            text_log->Print("vertex.m_ei[%d] and vertex.m_ei[%d] = %d but brep.m_E[%d].m_vi[0] = %d",
                             i,vei,ei,ei,edge.m_vi[0]);
            text_log->Print("and MYON_Brep.m_E[%d].m_vi[1] = %d (both m_vi[] values should be %d).\n",
                            ei,edge.m_vi[1],vertex_index);
            text_log->PopIndent();
          }
          return MYON_BrepIsNotValid();
        }
        for (j = i+1; j < vei; j++ ) 
        {
          if ( vertex.m_ei[j] == ei )
          {
            if ( text_log )
            {
              text_log->Print("brep.m_V[%d] vertex is not valid.\n",vertex_index);
              text_log->PushIndent();
              text_log->Print("vertex.m_ei[%d,%d,%d] = %d. An open edge index should appear once\n",i,vei,j,ei);
              text_log->Print("in vertex.m_ei[] and a closed edge index should appear twice.\n");
              text_log->PopIndent();
            }
            return MYON_BrepIsNotValid();
          }
        }
        break;
      }
    }
    if ( edge.m_vi[0] != vertex_index && edge.m_vi[1] != vertex_index )
    {
      if ( text_log )
      {
        text_log->Print("brep.m_V[%d] vertex is not valid.\n",vertex_index);
        text_log->PushIndent();
        text_log->Print("vertex.m_ei[%d] = %d but MYON_Brep.m_E[%d].m_vi[] = [%d,%d]. "
                        "At least one edge m_vi[] value should be %d.\n",
                        vei,ei,ei,edge.m_vi[0],edge.m_vi[1],vertex_index);
        text_log->PopIndent();
      }
      return MYON_BrepIsNotValid();
    }
  }

  if ( vertex.m_tolerance < 0.0 )
  {
    if ( text_log )
    {
      text_log->Print("brep.m_V[%d] vertex is not valid.\n",vertex_index);
      text_log->PushIndent();
      text_log->Print("vertex.m_tolerace = %g (should be >= 0.0)\n",vertex.m_tolerance);
      text_log->PopIndent();
    }
    return MYON_BrepIsNotValid();
  }
  return true;
}



static
bool TestTrimPBox( const MYON_BrepTrim& trim, MYON_TextLog* text_log )
{
  MYON_3dPoint pt;
  double d;
  MYON_BoundingBox pbox = trim.m_pbox;

  
  d = MYON_SQRT_EPSILON*(fabs(pbox.m_min.x)+fabs(pbox.m_max.x));
  if ( d < MYON_ZERO_TOLERANCE )
    d = MYON_ZERO_TOLERANCE;
  pbox.m_min.x -= d;
  pbox.m_max.x += d;

  d = MYON_SQRT_EPSILON*(fabs(pbox.m_min.y)+fabs(pbox.m_max.y));
  if ( d < MYON_ZERO_TOLERANCE )
    d = MYON_ZERO_TOLERANCE;
  pbox.m_min.y -= d;
  pbox.m_max.y += d;

  pt = trim.PointAtStart();
  if ( !pbox.IsPointIn(pt) )
  {
    if ( text_log )
       text_log->Print("MYON_Brep.m_T[%d].m_pbox does not contain start of trim.\n",trim.m_trim_index);
    return false;
  }

  pt = trim.PointAtEnd();

  if ( !pbox.IsPointIn(pt) )
  {
    if ( text_log )
       text_log->Print("MYON_Brep.m_T[%d].m_pbox does not contain end of trim.\n",trim.m_trim_index);
    return false;
  }

  pt = trim.PointAt(trim.Domain().ParameterAt(0.5));
  if ( !pbox.IsPointIn(pt) )
  {
    if ( text_log )
       text_log->Print("MYON_Brep.m_T[%d].m_pbox does not contain middle of trim.\n",trim.m_trim_index);
    return false;
  }

  return true;
}

static
bool CheckTrimOnSrfHelper( const MYON_Interval& srf_domain0,
                           const MYON_Interval& srf_domain1,
                           const MYON_BrepTrim& trim,
                           MYON_TextLog* text_log
                           )
{
  // this check only works if the pline exists.
  const MYON_BrepTrimPoint* tp = trim.m_pline.Array();
  int i, count = trim.m_pline.Count();

  /*
  June 08 2012 - Chuck - Changing a and b to -MYON_ZERO_TOLERANCE and 1+MYON_ZERO_TOLERANCE. Done so RR 106304 is considered a bad object.  Split, Trim, etc expect uv curves to
  be on the surface.  If you get a Check error here and feel the need to change this, 
  discuss first with Chuck or Dale L and document with RR item numbers. 
  const double a = -0.01;
  const double b =  1.01;

  26 June 2012 Chuck and Dale Lear
    Concerning bugs 
      http://dev.mcneel.com/bugtrack/?q=106304
      http://dev.mcneel.com/bugtrack/?q=107842

    We are moving the relative fuzz back to 1%
    because making it smaller is generating
    more tech support than it is worth at this 
    point.

    We did learn that 2d trim curves leak off the
    surface domain a bit fairly often and that
    forcing the 2d pline to be in the surface domain
    but leaving the 2d trim curve as is does not
    fix the bugs cited above.
  */
  const double a = -0.01;
  const double b =  1.01;

  double s,t;
  for ( i = 0; i < count; i++ )
  {
    s = srf_domain0.NormalizedParameterAt(tp[i].p.x);
    t = srf_domain1.NormalizedParameterAt(tp[i].p.y);
    if ( s < a || s > b || t < a || t > b )
    {
      if ( text_log )
      {
        text_log->Print("MYON_Brep.m_T[%d] 2d curve is not inside surface domain.\n",trim.m_trim_index);
      }
      return false;
    }
  }
  return true;
}

static
bool CheckLoopOnSrfHelper( const MYON_Brep& brep, 
                           const MYON_Interval& srf_domain0,
                           const MYON_Interval& srf_domain1,
                           const MYON_BrepLoop& loop,
                           MYON_TextLog* text_log
                           )
{
  for ( int lti = 0; lti < loop.m_ti.Count(); lti++ )
  {
    int ti = loop.m_ti[lti];
    if ( ti < 0 || ti >= brep.m_T.Count() )
      continue;
    if ( ! CheckTrimOnSrfHelper( srf_domain0, srf_domain1, brep.m_T[ti], text_log ) )
      return false;
  }
  return true;
}


static void Internal_ValidateBrepIndex(
  MYON__UINT_PTR text_log_ptr_plus,
  const wchar_t* corruption_descirption,
  bool& bCorrupt,
  const int max_idx,
  const int& idx
)
{
  if (idx < max_idx)
    return;
  const bool bSilentError = (0 != (text_log_ptr_plus & 1));
  const bool bRepair = (0 != (text_log_ptr_plus & 2));
  MYON_TextLog* text_log = (MYON_TextLog*)(text_log_ptr_plus & (~((MYON__UINT_PTR)3)) );
  if (false == bCorrupt && false == bSilentError)
  {
    MYON_ERROR("MYON_Brep has corrupt indices that will cause crashes.");
  }
  bCorrupt = true;
  if (nullptr != text_log)
    text_log->PrintString(corruption_descirption);
  if (bRepair)
    const_cast<int&>(idx) = -1; // prevents crashes because code checks for -1 as an unset index value
}


static void Internal_ValidateBrepIndexArray(
  MYON__UINT_PTR text_log_ptr_plus,
  const wchar_t* corruption_descirption,
  bool& bCorrupt,
  const int max_idx,
  const int idex_count,
  const int* idex_a
)
{
  for (int j = 0; j < idex_count; j++)
  {
    Internal_ValidateBrepIndex(text_log_ptr_plus, corruption_descirption, bCorrupt, max_idx, idex_a[j]);
  }
}


static void Internal_ValidateBrepIndexSimpleArray(
  MYON__UINT_PTR text_log_ptr_plus,
  const wchar_t* corruption_descirption,
  bool& bCorrupt,
  const int max_idx,
  const MYON_SimpleArray<int>& idx_array
)
{
  const int idex_count = idx_array.Count();
  const int* idex_a = idx_array.Array();
  Internal_ValidateBrepIndexArray(text_log_ptr_plus, corruption_descirption, bCorrupt, max_idx, idex_count, idex_a);
}

static void Internal_ValidateBrepComponentBackPtr(
  MYON__UINT_PTR text_log_ptr_plus,
  const wchar_t* corruption_descirption,
  bool& bCorrupt,
  const MYON_Brep* this_brep,
  MYON_Brep*const* brep_back_ptr,
  const int idx,
  const int& component_back_idx
)
{
  if (this_brep != *brep_back_ptr)
  {
    const bool bSilentError = (0 != (text_log_ptr_plus & 1));
    const bool bRepair = (0 != (text_log_ptr_plus & 2));
    MYON_TextLog* text_log = (MYON_TextLog*)(text_log_ptr_plus & (~((MYON__UINT_PTR)3)) );

    if (false == bCorrupt && false == bSilentError)
    {
      MYON_ERROR("MYON_Brep has corrupt indices that will cause crashes.");
    }
    bCorrupt = true;
    if (nullptr != text_log)
      text_log->Print(corruption_descirption);
    if (bRepair)
      *const_cast<MYON_Brep**>(brep_back_ptr) = const_cast<MYON_Brep*>(this_brep);
  }

  if (idx != component_back_idx)
  {
    const bool bSilentError = (0 != (text_log_ptr_plus & 1));
    const bool bRepair = (0 != (text_log_ptr_plus & 2));
    MYON_TextLog* text_log = (MYON_TextLog*)(text_log_ptr_plus & (~((MYON__UINT_PTR)3)));

    if (false == bCorrupt && false == bSilentError)
    {
      MYON_ERROR("MYON_Brep has corrupt indices that will cause crashes.");
    }
    bCorrupt = true;
    if (nullptr != text_log)
      text_log->Print(corruption_descirption);
    if (bRepair)
      const_cast<int&>(component_back_idx) = idx;
  }
}

bool MYON_Brep::IsCorrupt(
  bool bRepair,
  bool bSilentError,
  class MYON_TextLog* text_log
) const
{
  bool bCorrupt = false;

  const int C2_count = m_C2.Count();
  const int C3_count = m_C3.Count();
  const int S_count = m_S.Count();
  const int V_count = m_V.Count();
  const int E_count = m_E.Count();
  const int T_count = m_T.Count();
  const int L_count = m_L.Count();
  const int F_count = m_F.Count();

  MYON__UINT_PTR text_log_ptr = (MYON__UINT_PTR)text_log;
  if (bSilentError)
    text_log_ptr |= 1;
  if (bRepair)
    text_log_ptr |= 2;

  MYON_Brep* ignored_this_ptr = const_cast<MYON_Brep*>(this);

  for (int vi = 0; vi < V_count; vi++)
  {
    const MYON_BrepVertex& v = m_V[vi];
    Internal_ValidateBrepComponentBackPtr(
      text_log_ptr,
      L"Corrupt MYON_BrepVertex.m_vertex_index back pointer.\n",
      bCorrupt,
      this, &ignored_this_ptr,
      vi, v.m_vertex_index
    );

    Internal_ValidateBrepIndexSimpleArray(
      text_log_ptr,
      L"Corrupt MYON_BrepVertex.m_ei[] index.\n",
      bCorrupt,
      E_count,
      v.m_ei
    );
  }
  
  for (int ei = 0; ei < E_count; ei++)
  {
    const MYON_BrepEdge& e = m_E[ei];
    Internal_ValidateBrepComponentBackPtr(
      text_log_ptr,
      L"Corrupt MYON_BrepEdge m_brep or m_edge_index back pointers.\n",
      bCorrupt,
      this, &e.m_brep,
      ei, e.m_edge_index
    );

    Internal_ValidateBrepIndexArray(
      text_log_ptr,
      L"Corrupt MYON_BrepEdge.m_vi[] index.\n",
      bCorrupt,
      V_count,
      2,
      e.m_vi
    );
    Internal_ValidateBrepIndex(
      text_log_ptr,
      L"Corrupt MYON_BrepEdge.m_c3i index.\n",
      bCorrupt,
      C3_count,
      e.m_c3i
    );
    Internal_ValidateBrepIndexSimpleArray(
      text_log_ptr,
      L"Corrupt MYON_BrepEdge.m_ti[] index.\n",
      bCorrupt,
      T_count,
      e.m_ti
    );
  }
  
  for (int ti = 0; ti < T_count; ti++)
  {
    const MYON_BrepTrim& t = m_T[ti];
    Internal_ValidateBrepComponentBackPtr(
      text_log_ptr,
      L"Corrupt MYON_BrepTrim m_brep or m_trim_index back pointers.\n",
      bCorrupt,
      this, &t.m_brep,
      ti, t.m_trim_index
    );

    Internal_ValidateBrepIndex(
      text_log_ptr,
      L"Corrupt MYON_BrepTrim.m_c2i index.\n",
      bCorrupt,
      C2_count,
      t.m_c2i
    );
    Internal_ValidateBrepIndex(
      text_log_ptr,
      L"Corrupt MYON_BrepTrim.m_ei index.\n",
      bCorrupt,
      E_count,
      t.m_ei
    );
    Internal_ValidateBrepIndex(
      text_log_ptr,
      L"Corrupt MYON_BrepTrim.m_li index.\n",
      bCorrupt,
      L_count,
      t.m_li
    );
    Internal_ValidateBrepIndexArray(
      text_log_ptr,
      L"Corrupt MYON_BrepTrim.m_vi[] index.\n",
      bCorrupt,
      V_count,
      2,
      t.m_vi
    );
  }
  
  for (int li = 0; li < L_count; li++)
  {
    const MYON_BrepLoop& l = m_L[li];
    Internal_ValidateBrepComponentBackPtr(
      text_log_ptr,
      L"Corrupt MYON_BrepLoop m_brep or m_loop_index back pointers.\n",
      bCorrupt,
      this, &l.m_brep,
      li, l.m_loop_index
    );

    Internal_ValidateBrepIndexSimpleArray(
      text_log_ptr,
      L"Corrupt MYON_BrepLoop.m_ti[] index.\n",
      bCorrupt,
      T_count,
      l.m_ti
    );
    Internal_ValidateBrepIndex(
      text_log_ptr,
      L"Corrupt MYON_BrepLoop.m_fi index.\n",
      bCorrupt,
      F_count,
      l.m_fi
    );
  }
  
  for (int fi = 0; fi < F_count; fi++)
  {
    const MYON_BrepFace& f = m_F[fi];
    Internal_ValidateBrepComponentBackPtr(
      text_log_ptr,
      L"Corrupt MYON_BrepFace m_brep or m_face_index back pointers.\n",
      bCorrupt,
      this, &f.m_brep,
      fi, f.m_face_index
    );

    Internal_ValidateBrepIndexSimpleArray(
      text_log_ptr,
      L"Corrupt MYON_BrepFace.m_li[] index.\n",
      bCorrupt,
      L_count,
      f.m_li
    );
    Internal_ValidateBrepIndex(
      text_log_ptr,
      L"Corrupt MYON_BrepFace.m_si index.\n",
      bCorrupt,
      S_count,
      f.m_si
    );
  }

  return bCorrupt;
}


bool MYON_Brep::IsValid( MYON_TextLog* text_log ) const
{
  if (IsCorrupt(false, true, text_log))
    return false;

  const int curve2d_count = m_C2.Count();
  const int curve3d_count = m_C3.Count();
  const int surface_count = m_S.Count();
  const int vertex_count  = m_V.Count();
  const int edge_count    = m_E.Count();
  const int trim_count    = m_T.Count();
  const int loop_count    = m_L.Count();
  const int face_count    = m_F.Count();

  int c2i, c3i, si, vi, ei, fi, ti, li;

  if ( 0 == face_count && 0 == edge_count && 0 == vertex_count )
  {
    if ( text_log )
      text_log->Print( "MYON_Brep has no faces, edges, or vertices\n");
    return MYON_BrepIsNotValid();
  }

  if ( 0 != face_count )
  {
    if ( 0 == edge_count )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep has no edges.\n");
      return MYON_BrepIsNotValid();
    }
    if ( 0 == loop_count )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep has no loops.\n");
      return MYON_BrepIsNotValid();
    }
    if ( 0 == surface_count )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep has no surfaces.\n");
      return MYON_BrepIsNotValid();
    }
    if ( 0 == trim_count )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep has no trims.\n");
      return MYON_BrepIsNotValid();
    }
    if ( 0 == curve2d_count )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep has no 2d curves.\n");
      return MYON_BrepIsNotValid();
    }
  }

  if ( 0 != edge_count )
  {
    if ( 0 == curve3d_count )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep has no 3d curves.\n");
      return MYON_BrepIsNotValid();
    }
    if ( 0 == vertex_count )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep has no vertices.\n");
      return MYON_BrepIsNotValid();
    }
  }

  // check element indices match array positions
  for ( vi = 0; vi < vertex_count; vi++ ) 
  {
    if ( m_V[vi].m_vertex_index == -1 )
    {
      const MYON_BrepVertex& vertex = m_V[vi];
      if ( vertex.m_ei.Count() > 0 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_V[%d] is deleted (m_vertex_index = -1) but vertex.m_ei.Count() = %d.\n",
                           vi, vertex.m_ei.Count() );
        return MYON_BrepIsNotValid();
      }
    }
    else if ( m_V[vi].m_vertex_index != vi )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep.m_V[%d].m_vertex_index = %d (should be %d)\n",
                         vi, m_V[vi].m_vertex_index, vi );
      return MYON_BrepIsNotValid();
    }
  }

  for ( ei = 0; ei < edge_count; ei++ ) 
  {
    if ( m_E[ei].m_edge_index == -1 )
    {
      const MYON_BrepEdge& edge = m_E[ei];
      if ( edge.m_ti.Count() > 0 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_E[%d] is deleted (m_edge_index = -1) but edge.m_ti.Count() = %d.\n",
                           ei, edge.m_ti.Count() );
        return MYON_BrepIsNotValid();
      }
      if ( edge.m_c3i != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_E[%d] is deleted (m_edge_index = -1) but edge.m_c3i=%d (should be -1).\n",
                           ei, edge.m_c3i );
        return MYON_BrepIsNotValid();
      }
      if ( edge.ProxyCurve() )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_E[%d] is deleted (m_edge_index = -1) but edge.m_curve is not nullptr.\n",
                           ei );
        return MYON_BrepIsNotValid();
      }
      if ( edge.m_vi[0] != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_E[%d] is deleted (m_edge_index = -1) but edge.m_vi[0]=%d (should be -1).\n",
                           ei, edge.m_vi[0] );
        return MYON_BrepIsNotValid();
      }
      if ( edge.m_vi[1] != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_E[%d] is deleted (m_edge_index = -1) but edge.m_vi[1]=%d (should be -1).\n",
                           ei, edge.m_vi[1] );
        return MYON_BrepIsNotValid();
      }
    }
    else if ( m_E[ei].m_edge_index != ei )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep.m_E[%d].m_edge_index = %d (should be %d)\n",
                         ei, m_E[ei].m_edge_index, ei );
      return MYON_BrepIsNotValid();
    }
  }

  for ( ti = 0; ti < trim_count; ti++ ) 
  {
    if ( m_T[ti].m_trim_index == -1 )
    {
      const MYON_BrepTrim& trim = m_T[ti];
      if ( trim.m_ei != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_T[%d] is deleted (m_trim_index = -1) but trim.m_ei=%d (should be -1).\n",
                           ti, trim.m_ei );
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_li != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_T[%d] is deleted (m_trim_index = -1) but trim.m_li=%d (should be -1).\n",
                           ti, trim.m_li );
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_c2i != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_T[%d] is deleted (m_trim_index = -1) but trim.m_c2i=%d (should be -1).\n",
                           ti, trim.m_c2i );
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_vi[0] != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_T[%d] is deleted (m_trim_index = -1) but trim.m_vi[0]=%d (should be -1).\n",
                           ti, trim.m_vi[0] );
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_vi[1] != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_T[%d] is deleted (m_trim_index = -1) but trim.m_vi[1]=%d (should be -1).\n",
                           ti, trim.m_vi[1] );
        return MYON_BrepIsNotValid();
      }
    }
    else if ( m_T[ti].m_trim_index != ti  )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep.m_T[%d].m_trim_index = %d (should be %d)\n",
                         ti, m_T[ti].m_trim_index, ti );
      return MYON_BrepIsNotValid();
    }
    else if ( !m_T[ti].IsValid( text_log ) )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep.m_T[%d] is not valid\n",ti );
      return MYON_BrepIsNotValid();
    }
  }

  for ( li = 0; li < loop_count; li++ ) 
  {
    if ( m_L[li].m_loop_index == -1 )
    {
      const MYON_BrepLoop& loop = m_L[li];
      if ( loop.m_fi != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_L[%d] is deleted (m_loop_index = -1) but loop.m_fi=%d (should be -1).\n",
                           li, loop.m_fi );
        return MYON_BrepIsNotValid();
      }
      if ( loop.m_ti.Count() > 0 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_L[%d] is deleted (m_loop_index = -1) but loop.m_ti.Count()=%d.\n",
                           li, loop.m_ti.Count() );
        return MYON_BrepIsNotValid();
      }
    }
    else if ( m_L[li].m_loop_index != li )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep.m_L[%d].m_loop_index = %d (should be %d)\n",
                         li, m_L[li].m_loop_index, li );
      return MYON_BrepIsNotValid();
    }
  }

  for ( fi = 0; fi < face_count; fi++ ) 
  {
    if ( m_F[fi].m_face_index == -1 )
    {
      const MYON_BrepFace& face = m_F[fi];
      if ( face.m_si != -1 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_F[%d] is deleted (m_face_index = -1) but face.m_si=%d (should be -1).\n",
                           fi, face.m_si );
        return MYON_BrepIsNotValid();
      }
      if ( face.ProxySurface() )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_F[%d] is deleted (m_face_index = -1) but face.ProxySurface() is not nullptr.\n",
                           fi );
        return MYON_BrepIsNotValid();
      }
      if ( face.m_li.Count() > 0 )
      {
        if ( text_log )
          text_log->Print( "MYON_Brep.m_F[%d] is deleted (m_face_index = -1) but face.m_li.Count()=%d.\n",
                           fi, face.m_li.Count() );
        return MYON_BrepIsNotValid();
      }
    }
    else if ( m_F[fi].m_face_index != fi )
    {
      if ( text_log )
        text_log->Print( "MYON_Brep.m_F[%d].m_face_index = %d (should be %d)\n",
                         fi, m_F[fi].m_face_index, fi );
      return MYON_BrepIsNotValid();
    }
  }

  // check 2d curve geometry
  for ( c2i = 0; c2i < curve2d_count; c2i++ ) {
    if ( !m_C2[c2i] )
    {
      continue;
      // nullptr 2d curves are ok if they are not referenced
      //if ( text_log )
      //  text_log->Print("MYON_Brep.m_C2[%d] is nullptr.\n",c2i);
      //return MYON_BrepIsNotValid();
    }
    if ( !m_C2[c2i]->IsValid(text_log) )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_C2[%d] is invalid.\n",c2i);
      return MYON_BrepIsNotValid();
    }
    int c2_dim = m_C2[c2i]->Dimension();
    if ( c2_dim != 2 )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_C2[%d]->Dimension() = %d (should be 2).\n", c2i, c2_dim );
      return MYON_BrepIsNotValid();
    }
    const MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(m_C2[c2i]);
    if ( polycurve && polycurve->IsNested() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_C2[%d] is a nested polycurve.\n", c2i );
      return MYON_BrepIsNotValid();
    }
  }

  // check 3d curve geometry
  for ( c3i = 0; c3i < curve3d_count; c3i++ ) {
    if ( !m_C3[c3i] )
    {
      continue;
      // nullptr 3d curves are ok if they are not referenced
      //if ( text_log )
      //  text_log->Print("MYON_Brep.m_C3[%d] is nullptr.\n",c3i);
      //return MYON_BrepIsNotValid();
    }
    if ( !m_C3[c3i]->IsValid(text_log) )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_C3[%d] is invalid.\n",c3i);
      return MYON_BrepIsNotValid();
    }
    int c3_dim = m_C3[c3i]->Dimension();
    if ( c3_dim != 3 )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_C3[%d]->Dimension() = %d (should be 3).\n", c3i, c3_dim );
      return MYON_BrepIsNotValid();
    }
    const MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(m_C3[c3i]);
    if ( polycurve && polycurve->IsNested() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_C3[%d] is a nested polycurve.\n", c3i );
      return MYON_BrepIsNotValid();
    }
  }

  // check 3d surface geometry
  for ( si = 0; si < surface_count; si++ ) {
    if ( !m_S[si] )
    {
      continue;
      // nullptr 3d surfaces are ok if they are not referenced
      //if ( text_log )
      //  text_log->Print("MYON_Brep.m_S[%d] is nullptr.\n",si);
      //return MYON_BrepIsNotValid();
    }
    if ( !m_S[si]->IsValid(text_log) )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_S[%d] is invalid.\n",si);
      return MYON_BrepIsNotValid();
    }
    int dim = m_S[si]->Dimension();
    if ( dim != 3 )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_S[%d]->Dimension() = %d (should be 3).\n", si, dim );
      return MYON_BrepIsNotValid();
    }
  }

  // check vertices
  for ( vi = 0; vi < vertex_count; vi++ ) {
    if ( m_V[vi].m_vertex_index == -1 )
      continue;
    if ( !IsValidVertex( vi, text_log ) ) {
      if ( text_log )
        text_log->Print("MYON_Brep.m_V[%d] is invalid.\n",vi);
      return MYON_BrepIsNotValid();
    }
  }

  // check edges
  for ( ei = 0; ei < edge_count; ei++ ) 
  {
    if ( m_E[ei].m_edge_index == -1 )
      continue;
    if ( !IsValidEdge( ei, text_log ) ) {
      if ( text_log )
        text_log->Print("MYON_Brep.m_E[%d] is invalid.\n",ei);
      return MYON_BrepIsNotValid();
    }
  }

  // check faces
  for ( fi = 0; fi < face_count; fi++ ) 
  {
    if ( m_F[fi].m_face_index == -1 )
      continue;
    if ( !IsValidFace( fi, text_log ) ) {
      if ( text_log )
        text_log->Print("MYON_Brep.m_F[%d] is invalid.\n",fi);
      return MYON_BrepIsNotValid();
    }
  }

  // Check loops - this check is necessary at the brep level
  // to make sure there are no orphaned loops.
  // MYON_Brep::IsValidLoop(), which is called by MYON_Brep::IsValidFace(),
  // performs loop-trim bookkeeping checks on all loops that are referenced
  // by a face.
  for ( li = 0; li < loop_count; li++ )
  {
    const MYON_BrepLoop& loop = m_L[li];
    if ( m_L[li].m_loop_index == -1 )
      continue;
    if ( loop.m_fi < 0 || loop.m_fi >= m_F.Count() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_L[%d].m_fi = %d is not invalid.\n",li,loop.m_fi);
      return MYON_BrepIsNotValid();
    }
    if ( m_F[loop.m_fi].m_face_index != loop.m_fi )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_L[%d].m_fi = %d is a deleted face.\n",li,loop.m_fi);
      return MYON_BrepIsNotValid();
    }

    // This for() loop check is performed in IsValidLoop() which is 
    // called by IsValidFace() in the "check faces" loop above.  
    // I think it can be removed.  If anybody every sees this code
    // find a flaw, please tell Dale Lear.
    for ( int lti = 0; lti < loop.m_ti.Count(); lti++ )
    {
      ti = loop.m_ti[lti];
      if ( ti < 0 || ti >= m_T.Count() )
      {
        if ( text_log )
          text_log->Print("MYON_Brep.m_L[%d].m_ti[%d] = %d is not invalid.\n",li,lti,ti);
        return MYON_BrepIsNotValid();
      }
      if ( m_T[ti].m_trim_index != ti )
      {
        if ( text_log )
          text_log->Print("MYON_Brep.m_L[%d].m_ti[%d] = %d is a deleted trim.\n",li,lti,ti);
        return MYON_BrepIsNotValid();
      }
    }
  }

  // check trims - this check is necessary at the brep 
  // level to make sure there are no orphan trims and 
  // to test tolerances.  Most of these tests are duplicates 
  // of ones in MYON_Brep::IsValidTrim, which is called by 
  // MYON_Brep::IsValidLoop, which is called by MYON_Brep::IsValidFace.
  int seam_trim_count = 0;
  for ( ti = 0; ti < trim_count; ti++ )
  {
    const MYON_BrepTrim& trim = m_T[ti];
    if ( trim.m_trim_index == -1 )
      continue;

    if ( trim.m_vi[0] < 0 || trim.m_vi[0] >= m_V.Count() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_vi[0] = %d is not invalid.\n",ti,trim.m_vi[0]);
      return MYON_BrepIsNotValid();
    }
    if ( trim.m_vi[1] < 0 || trim.m_vi[1] >= m_V.Count() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_vi[1] = %d is not invalid.\n",ti,trim.m_vi[1]);
      return MYON_BrepIsNotValid();
    }

    if ( m_V[trim.m_vi[0]].m_vertex_index != trim.m_vi[0] )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_vi[0] is deleted.\n",ti);
      return MYON_BrepIsNotValid();
    }
    if ( m_V[trim.m_vi[1]].m_vertex_index != trim.m_vi[1] )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_vi[1] is deleted.\n",ti);
      return MYON_BrepIsNotValid();
    }

    if ( trim.m_c2i < 0 || trim.m_c2i >= m_C2.Count() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_c2i = %d is not valid.\n",ti,trim.m_c2i);
      return MYON_BrepIsNotValid();
    }

    if ( 0 == m_C2[trim.m_c2i] )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_c2i = %d, but m_C2[%d] is nullptr.\n",ti,trim.m_c2i,trim.m_c2i);
      return MYON_BrepIsNotValid();
    }

    if ( trim.m_li < 0 || trim.m_li >= m_L.Count() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_li = %d is not valid.\n",ti,trim.m_li);
      return MYON_BrepIsNotValid();
    }

    if ( m_L[trim.m_li].m_loop_index != trim.m_li )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_li = %d is a deleted loop.\n",ti,trim.m_li);
      return MYON_BrepIsNotValid();
    }

    {
      const MYON_Curve* c2 = m_C2[trim.m_c2i];
      const MYON_Surface* srf = m_S[m_F[m_L[trim.m_li].m_fi].m_si];
      if ( srf )
      {
        MYON_Interval PD = trim.ProxyCurveDomain();
        MYON_Surface::ISO iso = srf->IsIsoparametric(*c2, &PD);
        if ( trim.m_iso != iso )
        {
          if ( text_log )
            text_log->Print("MYON_Brep.m_T[%d].m_iso = %d and it should be %d\n",ti,trim.m_iso,iso);
          return MYON_BrepIsNotValid();
        }
      }
    }

    if ( trim.m_type == MYON_BrepTrim::singular )
    {
      if ( trim.m_ei != -1 )
      {
        if ( text_log )
          text_log->Print("MYON_Brep.m_T[%d].m_type = singular, but m_ei = %d (should be -1).\n",ti,trim.m_ei);
        return MYON_BrepIsNotValid();
      }
      continue;
    }

    if ( trim.m_ei < 0 || trim.m_ei >= m_E.Count() )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_ei = %d is not invalid.\n",ti,trim.m_ei);
      return MYON_BrepIsNotValid();
    }
    
    const MYON_BrepEdge& edge = m_E[trim.m_ei];
    if ( edge.m_edge_index != trim.m_ei )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_ei is deleted.\n",ti);
      return MYON_BrepIsNotValid();
    }

    const int evi0 = trim.m_bRev3d ? 1 : 0;
    const int evi1 = trim.m_bRev3d ? 0 : 1;
    if ( trim.m_vi[0] != edge.m_vi[evi0] )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_bRev3d = %d, but m_vi[0] != m_E[m_ei].m_vi[%d].\n",ti,trim.m_bRev3d,evi0);
      return MYON_BrepIsNotValid();
    }
    if ( trim.m_vi[1] != edge.m_vi[evi1] )
    {
      if ( text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_bRev3d = %d, but m_vi[0] != m_E[m_ei].m_vi[%d].\n",ti,trim.m_bRev3d,evi1);
      return MYON_BrepIsNotValid();
    }

    // check tolerances and closed curve directions
    {
      MYON_3dPoint trim_pt0, trim_pt1, srf_pt0, srf_pt1;
      MYON_3dVector trim_der0, trim_der1, srf_du0, srf_dv0, srf_du1, srf_dv1;
      MYON_Interval trim_domain = trim.Domain();
      // trim_pt0 should be closed to trim_pt1 except when
      // trim starts and ends on opposite sides of a surface 
      // seam.  Even when the trim curve is closed, the 
      // derivatives can be different when there is
      // a kink at the start/end of a trim.
      trim.Ev1Der( trim_domain[0], trim_pt0, trim_der0 );
      trim.Ev1Der( trim_domain[1], trim_pt1, trim_der1 );

      const MYON_Surface* trim_srf = m_F[ m_L[trim.m_li].m_fi ].SurfaceOf();
      trim_srf->Ev1Der( trim_pt0.x, trim_pt0.y, srf_pt0, srf_du0, srf_dv0 );
      trim_srf->Ev1Der( trim_pt1.x, trim_pt1.y, srf_pt1, srf_du1, srf_dv1 );

      // estimate 3d tolerances from 2d trim tolerances
      double t0_tol = srf_du0.Length()*trim.m_tolerance[0] + srf_dv0.Length()*trim.m_tolerance[1];
      double t1_tol = srf_du1.Length()*trim.m_tolerance[0] + srf_dv1.Length()*trim.m_tolerance[1];
      MYON_3dVector trim_tangent0 = trim_der0.x*srf_du0 + trim_der0.y*srf_dv0;
      trim_tangent0.Unitize();
      MYON_3dVector trim_tangent1 = trim_der1.x*srf_du1 + trim_der1.y*srf_dv1;
      trim_tangent1.Unitize();
      MYON_3dVector edge_tangent0 = edge.TangentAt( edge.Domain()[trim.m_bRev3d ? 1 : 0] );
      MYON_3dVector edge_tangent1 = edge.TangentAt( edge.Domain()[trim.m_bRev3d ? 0 : 1] );
      double d0 = trim_tangent0*edge_tangent0;
      double d1 = trim_tangent1*edge_tangent1;
      if ( trim.m_bRev3d )
      {
        d0 = -d0;
        d1 = -d1;
      }
      if (    trim.m_vi[0] == trim.m_vi[1] 
           && edge.m_vi[0] == edge.m_vi[1] 
           && trim.m_vi[0] == edge.m_vi[0] 
           )
      {
        // For high quality models, d0 and d1 should be close to +1.
        // If both are close to -1, the trim.m_bRev3d flag is most
        // likely set opposite of what it should be.

        // check start tangent to see if m_bRev3d is set correctly
        if ( d0 < 0.0 || d1 < 0.0)
        {
          if ( text_log )
          {
            if ( trim.m_bRev3d )
              text_log->Print("MYON_Brep.m_T[%d].m_bRev3d = true, but closed curve directions are the same.\n",ti);
            else
              text_log->Print("MYON_Brep.m_T[%d].m_bRev3d = false, but closed curve directions are opposite.\n",ti);
          }
          return MYON_BrepIsNotValid();
        }
      }

      // Make sure edge and tolerances are realistic
      MYON_3dPoint EdgeEnd[2];
      EdgeEnd[trim.m_bRev3d?1:0] = edge.PointAtStart();
      EdgeEnd[trim.m_bRev3d?0:1] = edge.PointAtEnd();
      d0 = EdgeEnd[0].DistanceTo(srf_pt0);
      d1 = EdgeEnd[1].DistanceTo(srf_pt1);
      double etol = edge.m_tolerance;
      double dtol = 10.0*(etol + t0_tol + t1_tol);
      if ( dtol < 0.01 )
        dtol = 0.01;
      if ( d0 > dtol  )
      {
        if ( text_log )
        {
          text_log->Print("Distance from start of MYON_Brep.m_T[%d] to 3d edge is %g.  (edge tol = %g, trim tol ~ %g).\n",
                          ti, d0, etol,t0_tol);
        }
        return MYON_BrepIsNotValid();
      }
      if ( d1 > dtol )
      {
        if ( text_log )
        {
          text_log->Print("Distance from end of MYON_Brep.m_T[%d] to 3d edge is %g.  (edge tol = %g, trim tol ~ %g).\n",
                          ti, d1, etol,t1_tol);
        }
        return MYON_BrepIsNotValid();
      }
    }

    // check trim's m_pbox
    {
      if ( trim.m_pbox.m_min.z != 0.0 )
      {
        if ( text_log )
           text_log->Print("MYON_Brep.m_T[%d].m_pbox.m_min.z = %g (should be zero).\n",ti,trim.m_pbox.m_min.z);
        return MYON_BrepIsNotValid();
      }
      if ( trim.m_pbox.m_max.z != 0.0 )
      {
        if ( text_log )
           text_log->Print("MYON_Brep.m_T[%d].m_pbox.m_max.z = %g (should be zero).\n",ti,trim.m_pbox.m_max.z);
        return MYON_BrepIsNotValid();
      }
      
      if ( !TestTrimPBox( trim, text_log ) )
        return MYON_BrepIsNotValid();

    }

    if ( MYON_BrepTrim::seam == trim.m_type )
    {
      // trim must be on a surface edge
      switch ( trim.m_iso )
      {
      case MYON_Surface::S_iso:
        break;
      case MYON_Surface::E_iso:
        break;
      case MYON_Surface::N_iso:
        break;
      case MYON_Surface::W_iso:
        break;
      default:
        if ( text_log )
          text_log->Print("MYON_Brep.m_T[%d].m_type = MYON_BrepTrim::seam but m_iso is not N/E/W/S_iso.\n",ti);
        return MYON_BrepIsNotValid();
      }
      seam_trim_count++;
    }
  }

  // check loop m_pboxes
  for ( li = 0; li < loop_count; li++ )
  {
    const MYON_BrepLoop& loop = m_L[li];
    if ( loop.m_loop_index != li )
      continue;
    if ( loop.m_pbox.m_min.z != 0.0 )
    {
      if ( text_log )
         text_log->Print("MYON_Brep.m_L[%d].m_pbox.m_min.z = %g (should be zero).\n",li,loop.m_pbox.m_min.z);
      return MYON_BrepIsNotValid();
    }
    if ( loop.m_pbox.m_max.z != 0.0 )
    {
      if ( text_log )
         text_log->Print("MYON_Brep.m_L[%d].m_pbox.m_max.z = %g (should be zero).\n",li,loop.m_pbox.m_max.z);
      return MYON_BrepIsNotValid();
    }
    int first_trim_ti = -4;
    int first_trim_vi0 = -3;
    int prev_trim_vi1 = -2;
    int prev_trim_ti=-9;
    int lti;
    for ( lti = 0; lti < loop.m_ti.Count(); lti++ )
    {
      const MYON_BrepTrim& trim = m_T[loop.m_ti[lti]];
      if ( !loop.m_pbox.IsPointIn(trim.m_pbox.m_min) || !loop.m_pbox.IsPointIn(trim.m_pbox.m_max) )
      {
        if ( text_log )
           text_log->Print("MYON_Brep.m_L[%d].m_pbox does not contain m_T[loop.m_ti[%d]].m_pbox.\n",li,lti);
        return MYON_BrepIsNotValid();
      }
      if ( 0 == lti )
      {
        first_trim_ti = loop.m_ti[lti];
        first_trim_vi0 = trim.m_vi[0];
      }
      else if ( prev_trim_vi1 != trim.m_vi[0] )
      {
        // 23 May 2003 Dale Lear
        //     Added this test to make sure adjacent trims
        //     in a loop shared vertices.
        if ( text_log )
           text_log->Print("MYON_Brep.m_L[%d] loop has trim vertex mismatch:\n  m_T[loop.m_ti[%d]=%d].m_vi[1] = %d != m_T[loop.m_ti[%d]=%d].m_vi[0]=%d.\n",li,lti-1,prev_trim_ti,prev_trim_vi1,lti,loop.m_ti[lti],trim.m_vi[0]);
        return MYON_BrepIsNotValid();
      }
      prev_trim_ti = loop.m_ti[lti];
      prev_trim_vi1 = trim.m_vi[1];
    }

    if ( first_trim_ti >= 0 && first_trim_vi0 != prev_trim_vi1 )
    {
      // 23 May 2003 Dale Lear
      //     Added this test to make sure adjacent trims
      //     in a loop shared vertices.
      if ( text_log )
         text_log->Print("MYON_Brep.m_L[%d] loop has trim vertex mismatch:\n  m_T[loop.m_ti[%d]=%d].m_vi[1] = %d != m_T[loop.m_ti[%d]=%d].m_vi[0]=%d.\n",
                         li,lti-1,prev_trim_ti,prev_trim_vi1,0,first_trim_ti,first_trim_vi0);
      return MYON_BrepIsNotValid();
    }
  }

  // 21 October 2003 Dale Lear - fix RR 11980 - check for split seams
  // This block of code assumes the preceding checks have all passed.
  // It looks for boundary trims on seams that should be joined as a seam trim.
  MYON_Interval srf_domain[2];
  for ( fi = 0; fi < face_count; fi++ )
  {
    const MYON_BrepFace& face = m_F[fi];
    if ( face.m_face_index < 0 )
      continue;
    const MYON_Surface* srf = m_S[face.m_si];
    if ( 0 == srf )
      continue;

    srf_domain[0] = srf->Domain(0);
    srf_domain[1] = srf->Domain(1);
    for ( int fli = 0; fli < face.m_li.Count(); fli++ )
    {
      int li_local = face.m_li[fli];
      if ( li_local < 0 || li_local >= m_L.Count() )
        continue;
      if ( !CheckLoopOnSrfHelper(*this,srf_domain[0],srf_domain[1],m_L[li_local],text_log) )
        return MYON_BrepIsNotValid();
    }

    const MYON_BrepLoop* outer_loop = face.OuterLoop();
    if ( 0 == outer_loop )
      continue;

    bool bClosed[2];
    bClosed[0] = srf->IsClosed(0);
    bClosed[1] = srf->IsClosed(1);
    if ( !bClosed[0] && !bClosed[1] )
      continue;

    const int outer_trim_count = outer_loop->m_ti.Count();
    int lti, lti1;
    int endpt_index = 0;
    MYON_Surface::ISO iso_type;
    MYON_Interval side_interval;
    double s0, s1;
    const double side_tol = 1.0e-4;

    for ( lti = 0; lti < outer_trim_count; lti++ )
    {
      const MYON_BrepTrim& trim = m_T[outer_loop->m_ti[lti]];
      if ( MYON_BrepTrim::boundary !=  trim.m_type )
        continue;
      if ( MYON_Surface::E_iso == trim.m_iso && bClosed[0] )
      {
        iso_type = MYON_Surface::W_iso;
        endpt_index = 1;
      }
      else if ( MYON_Surface::W_iso == trim.m_iso && bClosed[0] )
      {
        iso_type = MYON_Surface::E_iso;
        endpt_index = 1;
      }
      else if( MYON_Surface::S_iso == trim.m_iso && bClosed[1] )
      {
        iso_type = MYON_Surface::N_iso;
        endpt_index = 0;
      }
      else if( MYON_Surface::N_iso == trim.m_iso && bClosed[1] )
      {
        iso_type = MYON_Surface::S_iso;
        endpt_index = 0;
      }
      else
        continue;

      side_interval.Set(trim.PointAtStart()[endpt_index],trim.PointAtEnd()[endpt_index]);
      if ( MYON_Surface::N_iso == iso_type || MYON_Surface::W_iso == iso_type )
      {
        if ( !side_interval.IsIncreasing() )
          continue;
      }
      else
      {
        if ( !side_interval.IsDecreasing() )
          continue;
      }

      // search for seam
      for ( lti1 = 0; lti1 < outer_trim_count; lti1++ )
      {
        if ( lti1 == lti )
          continue;
        const MYON_BrepTrim& trim1 = m_T[outer_loop->m_ti[lti1]];
        if ( iso_type != trim1.m_iso )
          continue;
        if ( MYON_BrepTrim::boundary != trim1.m_type )
          continue;
        
        s1 = side_interval.NormalizedParameterAt(trim1.PointAtStart()[endpt_index]);
        if ( fabs(s1-1.0) > side_tol )
          continue;
        s0 = side_interval.NormalizedParameterAt(trim1.PointAtEnd()[endpt_index]);
        if ( fabs(s0) > side_tol )
          continue;

        if ( text_log )
        {
         text_log->Print("MYON_Brep.m_F[%d] is on a closed surface. Outer loop m_L[%d] contains boundary trims %d and %d.  They should be seam trims connected to the same edge.\n",
                         face.m_face_index,outer_loop->m_loop_index,
                         trim.m_trim_index,trim1.m_trim_index
                         );
        }
        return MYON_BrepIsNotValid();                
      }
    }
  }

  // make sure seam trims are properly matched.
  for ( ti = 0; seam_trim_count > 0 && ti < trim_count; ti++ )
  {
    const MYON_BrepTrim& trim = m_T[ti];
    if ( trim.m_trim_index == -1 )
      continue;
    if ( MYON_BrepTrim::seam != trim.m_type )
      continue;
    seam_trim_count--;
    if ( trim.m_ei < 0 || trim.m_ei >= edge_count )
    {
      if ( text_log )
      {
        text_log->Print("MYON_Brep.m_T[%d] is a seam trim with an invalid m_ei.\n",ti);
        return MYON_BrepIsNotValid();
      }
    }

    const MYON_BrepEdge& edge = m_E[trim.m_ei];
    int trim1_index = -1;
    for ( int eti = 0; eti < edge.m_ti.Count(); eti++ )
    {
      const int ti1 = edge.m_ti[eti];
      if ( ti1 == ti 
           || ti < 0
           || ti >= trim_count 
         )
      {
        continue;
      }
      const MYON_BrepTrim& trim1 = m_T[ti1];
      if ( trim1.m_trim_index == -1 )
        continue;
      if ( MYON_BrepTrim::seam != trim1.m_type )
        continue;
      if ( trim1.m_li != trim.m_li )
        continue;
      if ( -1 == trim1_index )
      {
        trim1_index = ti1;
        continue;
      }
      text_log->Print("MYON_Brep.m_T[%d,%d,%d] are three seam trims with the same edge in the same loop.\n",ti,trim1_index,ti1);
      return MYON_BrepIsNotValid();
    }

    if ( trim1_index < 0 || trim1_index >= trim_count )
    {
      text_log->Print("MYON_Brep.m_T[%d] is a seam trim with no matching seam trim in the same loop.\n",ti);
      return MYON_BrepIsNotValid();
    }

    // previous validation step insures trim.m_iso = N/S/E/W_iso
    switch(trim.m_iso)
    {
    case MYON_Surface::S_iso:
      if ( MYON_Surface::N_iso != m_T[trim1_index].m_iso )
      {
        if (text_log )
          text_log->Print("Seam trim MYON_Brep.m_T[%d].m_iso = S_iso but matching seam MYON_Brep.m_T[%d].m_iso != N_iso.\n",ti,trim1_index);
        return MYON_BrepIsNotValid();
      }
      break;

    case MYON_Surface::E_iso:
      if ( MYON_Surface::W_iso != m_T[trim1_index].m_iso )
      {
        if (text_log )
          text_log->Print("Seam trim MYON_Brep.m_T[%d].m_iso = E_iso but matching seam MYON_Brep.m_T[%d].m_iso != W_iso.\n",ti,trim1_index);
        return MYON_BrepIsNotValid();
      }
      break;

    case MYON_Surface::N_iso:
      if ( MYON_Surface::S_iso != m_T[trim1_index].m_iso )
      {
        if (text_log )
          text_log->Print("Seam trim MYON_Brep.m_T[%d].m_iso = N_iso but matching seam MYON_Brep.m_T[%d].m_iso != S_iso.\n",ti,trim1_index);
        return MYON_BrepIsNotValid();
      }
      break;

    case MYON_Surface::W_iso:
      if ( MYON_Surface::E_iso != m_T[trim1_index].m_iso )
      {
        if (text_log )
          text_log->Print("Seam trim MYON_Brep.m_T[%d].m_iso = W_iso but matching seam MYON_Brep.m_T[%d].m_iso != E_iso.\n",ti,trim1_index);
        return MYON_BrepIsNotValid();
      }
      break;

    case MYON_Surface::not_iso:
    case MYON_Surface::x_iso:
    case MYON_Surface::y_iso:
    case MYON_Surface::iso_count:
      break; // keep gcc quiet
    }


  }

  // Dale Lear Fix https://mcneel.myjetbrains.com/youtrack/issue/RH-64277
  // The commented out tests might have helped in debugging make2d, but they are making a mess of things now.
  // In my view, destroying/remaking a box is beyond the scope of an appropriate IsValid() test.
  // In addintion to replacing the test below with a warning Greg can use while debugging
  // make2d, I've fixed the IO code in MYON_Brep::Read() so old boxes are now reliably updated
  // to their smaller versions.

  //////// GBA 28-Aug-20 RH-60112 and RH-58462
  //////// Adding bounding box (m_bbox) validation tests.  
  //////// Brep bounding box is cached and persists across sessions (since at least Rhino 6).
  //////// In Rhino6 and earlier, the bounding box included entire underlying surface.
  //////// Rhino7 bounding box calculation now uses "shrinked" surfaces.
  //////// A bounding box is now reported as invalid if it could be significantly 
  //////// reduced by being recalculated.
  //////if (!m_bbox.IsEmpty())
  //////{
  //////  if (!m_bbox.IsValid())
  //////  {
  //////    if (text_log)
  //////      text_log->Print("Bounding Box is not valid.\n");
  //////    return MYON_BrepIsNotValid();
  //////  }
  //////  else
  //////  {
  //////    MYON_BoundingBox orig_box = m_bbox;
  //////    MYON_BoundingBox computed_box;
  //////    {
  //////      MYON_Brep* this_nonconst = const_cast<MYON_Brep*>(this);
  //////      this_nonconst->ClearBoundingBox();
  //////      computed_box = BoundingBox();
  //////      this_nonconst->m_bbox = orig_box;    // restore box as it as it was.
  //////    }
  //////    // expand the computed_box before we do the incusion test
  //////    // I'm trying to avoid making a lot of objects created in Rhino 6 and earlier
  //////    // reporting as Invalid objects in Rhino 7.
  //////    computed_box.Expand(computed_box.Diagonal() + MYON_3dVector(1.0, 1.0, 1.0));

  //////    if (!computed_box.Includes(orig_box))
  //////    {
  //////      if (text_log)
  //////        text_log->Print("Stored Bounding Box extends far outside of computed bounding box.\n");
  //////      return MYON_BrepIsNotValid();

  //////    }

  //////  }
  //////}

  if (m_bbox.IsNotEmpty())
  {

    // new_brep_bbox is calculated from scratch WITHOUT changing existing values on MYON_BrepFace.m_bbox and MYON_Brep.m_bbox.
    const MYON_BoundingBox new_brep_bbox = this->InternalBrepBoundingBox(false, false);
    if (new_brep_bbox.IsNotEmpty())
    {
      MYON_BoundingBox triple_size_new_brep_bbox = new_brep_bbox;
      triple_size_new_brep_bbox.Expand(new_brep_bbox.Diagonal() + MYON_3dVector(1.0, 1.0, 1.0));
      const MYON_BoundingBox cached_brep_bbox = m_bbox;
      if (false == triple_size_new_brep_bbox.Includes(cached_brep_bbox, false))
      {
        if (nullptr != text_log)
          text_log->Print("WARNING: The cached MYON_Brep.m_bbox is much larger than one cacluated from current MYON_BrepFace extents.\n");
        MYON_WARNING("The cached MYON_Brep.m_bbox is much larger than one cacluated from current MYON_BrepFace extents. This might effect make2d performance.");
        // DO NOT RETURN FALSE HERE!
      }
    }
  }


#if 0
  // validate MYON_BrepTrim.m_pline
  for ( ti = 0; ti < trim_count; ti++ )
  {
    const MYON_BrepTrim& trim = m_T[ti];
    if ( trim.m_trim_index == -1 )
      continue;
    const int pline_count = trim.m_pline.Count();
    if ( 0 == pline_count )
      continue;
    if ( pline_count <= 1 )
    {
      if (text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_pline.Count() = 1. It should be 0 or >= 2\n",ti);
      return MYON_BrepIsNotValid();
    }

    const MYON_Interval trim_domain = trim.Domain();
    if ( !(trim.m_pline[0].t == trim_domain[0]) )
    {
      if (text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_pline[0].t != start of trim domain.\n",ti);
      return MYON_BrepIsNotValid();
    }
    if ( !(trim.m_pline[pline_count-1].t == trim_domain[1]) )
    {
      if (text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_pline[%d].t != end of trim domain.\n",ti,pline_count-1);
      return MYON_BrepIsNotValid();
    }
    for ( int i = 1; i < pline_count; i++ )
    {
      // pline trim "t" values must be valid
      if ( !MYON_IsValid(trim.m_pline[i].t) )
      {
        if (text_log )
          text_log->Print("MYON_Brep.m_T[%d].m_pline[%d].t is not a valid double.\n",ti,i);
        return MYON_BrepIsNotValid();
      }
      if ( !(trim.m_pline[i-1].t < trim.m_pline[i].t) )
      {
        if (text_log )
          text_log->Print("MYON_Brep.m_T[%d].m_pline[%d].t must be < m_pline[%d].t.\n",ti,i-1,i);
        return MYON_BrepIsNotValid();
      }
    }

    if (    MYON_UNSET_VALUE == trim.m_pline[0].e 
         && MYON_UNSET_VALUE == trim.m_pline[pline_count-1].e 
       )
    {
      // the "e" values are not set.
      // This is permitted. The are set when extensive
      // trim-edge parameter correspondence is needed.
      // Meshing is an example of a calculation that sets
      // the "e" paramters.
      continue;
    }

    if ( trim.m_ei < 0 || trim.m_ei >= m_E.Count() )
    {
      if (text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_pline has e parameters but trim.m_ei is not valid.\n",ti);
      return MYON_BrepIsNotValid();
    }
    const MYON_BrepEdge& edge = m_E[trim.m_ei];
    const MYON_Interval edge_domain = edge.Domain();
    const int i0 = trim.m_bRev3d ? pline_count-1 : 0;
    const int i1 = trim.m_bRev3d ? 0 : pline_count-1;
    const int di = trim.m_bRev3d ? -1 : 1;
    if ( !(trim.m_pline[i0].e == edge_domain[0]) )
    {
      if (text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_pline[%d].e != start of edge domain.\n",ti,i0);
      return MYON_BrepIsNotValid();
    }
    if ( !(trim.m_pline[i1].e == edge_domain[1]) )
    {
      if (text_log )
        text_log->Print("MYON_Brep.m_T[%d].m_pline[%d].e != end of edge domain.\n",ti,i1);
      return MYON_BrepIsNotValid();
    }
    int prev_valid_i = i0;
    for ( int i = i0+di; i >= 0 && i < pline_count && i-di >= 0 && i-di < pline_count; i += di )
    {
      if ( !MYON_IsValid(trim.m_pline[i].e) )
      {
        // internal "e" may be invalid when the setter
        // had troubles.  This is a symptom of a 
        // bad trim or edge curve, but is not conclusive
        // proof.
        continue;
      }
      if ( !(trim.m_pline[prev_valid_i].e < trim.m_pline[i].e) )
      {
        if (text_log )
          text_log->Print("MYON_Brep.m_T[%d].m_pline[%d].t must be < m_pline[%d].t.\n",ti,prev_valid_i,i);
        return MYON_BrepIsNotValid();
      }
      prev_valid_i = i;
    }
  }
#endif

  return true;
}



bool MYON_Brep::SetEdgeVertex( const int ei, const int evi, const int vi )
{
  if ( ei < 0 || vi < 0 || evi < 0 || evi > 1 || ei >= m_E.Capacity() )
    return false;
  MYON_BrepEdge& edge = m_E[ei];
  if ( edge.m_vi[evi] != vi ) {
    edge.m_vi[evi] = vi;
    MYON_BrepVertex& vertex = m_V[vi];
    vertex.m_ei.Append(ei);
  }
  const int trim_count = edge.m_ti.Count();
  int eti, ti, tvi;
  for ( eti = 0; eti < trim_count; eti++ ) {
    ti = edge.m_ti[eti];
    if ( ti < 0 )
      continue;
    MYON_BrepTrim& trim = m_T[ti];
    tvi = trim.m_bRev3d ? 1-evi : evi;
    trim.m_vi[tvi] = vi;
  }
  return true;
}

bool MYON_Brep::HopAcrossEdge( int& ti, int& tvi ) const
{
  // Tf ti is a trim associated with an interior manifold edge,
  // then ti is set to twin.
  int ei, evi, new_ti, new_tvi;
  if ( ti < 0 )
    return false;
  ei = m_T[ti].m_ei;
  if ( ei < 0 )
    return false;
  const MYON_BrepEdge& edge = m_E[ei];
  if ( edge.m_ti.Count() < 2 )
    return false;
  evi = (m_T[ti].m_bRev3d) ? 1-tvi : tvi;
  new_ti = edge.m_ti[(edge.m_ti[0] == ti)?1:0];
  if ( new_ti < 0 )
    return false;
  new_tvi = (m_T[new_ti].m_bRev3d) ? 1-evi : evi;
  ti = new_ti;
  tvi = new_tvi;
  return true;
}

bool MYON_Brep::SetTrimStartVertex( const int ti0, const int vi )
{
  // Do not use NextEdge(), PrevEdge() because they require
  // the information we are in the act of creating.
  if ( ti0 < 0 || vi < 0 )
    return false;
  int next_ti, ti, ei, evi, tvi, counter;

  const int edge_count = m_E.Count();

  // Step counter clockwise around vertex until we hit a boundary
  // or we get back to where we started.
  for ( ti = ti0, tvi = 0, counter = 0; ti >= 0 && counter < 512; counter++ ) {
    if ( counter > 0 ) {
      if ( ti == ti0 && tvi == 0 )
        return true; // vertex was interior
    }
    MYON_BrepTrim& trim = m_T[ti];
    if ( trim.m_type == MYON_BrepTrim::singular ) {
      trim.m_vi[0] = trim.m_vi[1] = vi;
      tvi = 1-tvi;
      next_ti = (tvi) ? NextTrim(ti) : PrevTrim(ti);
      ti = next_ti;
      tvi = 1-tvi;
      if ( ti == ti0 && tvi == 0 )
        return true; // vertex was interior
      if ( m_T[ti].m_type != MYON_BrepTrim::singular )
        HopAcrossEdge( ti, tvi ); // OK if hop fails because ti is a boundary
      continue;
    }

    ei = trim.m_ei;
    evi = (trim.m_bRev3d) ? 1-tvi : tvi;
    if ( !SetEdgeVertex( ei, evi, vi ) )
      return false;
    next_ti = (tvi) ? NextTrim(ti) : PrevTrim(ti);
    ti = next_ti;
    tvi = 1-tvi;
    if ( ti < 0 )
      return false; // should not happen

    if ( m_T[ti].m_type == MYON_BrepTrim::singular )
      continue;
    ei = m_T[ti].m_ei;
    if ( ei < 0 || ei >= edge_count )
      return false; // should not happen
    evi = (m_T[ti].m_bRev3d) ? 1-tvi : tvi;
    const int edge_trim_count = m_E[ei].m_ti.Count();
    if ( edge_trim_count < 1 )
      break; // should not happen
    if ( edge_trim_count == 1 ) {
      SetEdgeVertex( ei, evi, vi );
      break; // ran into boundary
    }
    if ( !HopAcrossEdge( ti, tvi ) )
      return false;
  }


  // Get ready to step counter clockwise around vertex until
  // we hit a boundary.
  ti = ti0;
  tvi = 0;
  if ( m_T[ti].m_type == MYON_BrepTrim::singular ) {
    // back up until we get to a non-singular trim
    while ( m_T[ti].m_type == MYON_BrepTrim::singular ) {
      if ( ti != ti0 ) {
        m_T[ti].m_vi[0] = vi;
        m_T[ti].m_vi[1] = vi;
      }
      ti = PrevTrim(ti);
      tvi = 1;
      if ( ti == ti0 )
        break;
    }
    ei = m_T[ti].m_ei;
    if ( ei >= 0 ) {
      evi = (m_T[ti].m_bRev3d) ? 1-tvi : tvi;
      SetEdgeVertex( ei, evi, vi );
    }
  }
  else {
    ei = m_T[ti].m_ei;
  }
  if ( ei < 0 ) {
    // did the best we could - return true so setter keeps going
    // but the fact we are here means the brep is bogus.
    return true;
  }
  if ( m_E[ei].m_ti.Count() < 2 )
    return true; // ti0 is a boundary - we're done.
  if ( !HopAcrossEdge( ti, tvi ) )
    return false;
  next_ti = (tvi) ? NextTrim(ti) : PrevTrim(ti);
  if ( next_ti < 0 )
    return false;
  ti = next_ti;
  tvi = 1-tvi;
  if ( m_T[ti].m_type != MYON_BrepTrim::singular ) {
    ei = m_T[ti].m_ei;
    if ( ei < 0 )
      return false;
    if ( m_E[ei].m_ti.Count() == 1 ) {
      evi = (m_T[ti].m_bRev3d)? 1-tvi : tvi;
      SetEdgeVertex( ei, evi, vi );
      return true;
    }
    if ( !HopAcrossEdge( ti, tvi ) )
      return false;
  }

  const int ti1 = ti;
  const int tvi1 = tvi;
  
  for ( ti = ti1, tvi = tvi1, counter = 0; ti >= 0 && counter < 512; counter++ ) {
    if ( counter > 0 ) {
      if ( ti == ti1 && tvi == tvi1 )
        return false; // vertex is not interior - so this should not happen
    }
    MYON_BrepTrim& trim = m_T[ti];
    if ( trim.m_type == MYON_BrepTrim::singular ) {
      trim.m_vi[0] = trim.m_vi[1] = vi;
      tvi = 1-tvi;
      next_ti = (tvi) ? NextTrim(ti) : PrevTrim(ti);
      ti = next_ti;
      tvi = 1-tvi;
      if ( ti == ti1 && tvi == tvi1 )
        return false; // vertex is not interior - so this should not happen
      if ( m_T[ti].m_type != MYON_BrepTrim::singular )
        HopAcrossEdge( ti, tvi );  // OK if hop fails because ti is a boundary
      continue;
    }

    ei = trim.m_ei;
    evi = (trim.m_bRev3d) ? 1-tvi : tvi;
    if ( !SetEdgeVertex( ei, evi, vi ) )
      return false;
    next_ti = (tvi) ? NextTrim(ti) : PrevTrim(ti);
    ti = next_ti;
    tvi = 1-tvi;
    if ( ti < 0 )
      return false; // should not happen
    
    if ( m_T[ti].m_type == MYON_BrepTrim::singular )
      continue;
    ei = m_T[ti].m_ei;
    if ( ei < 0 )
      return false; // should not happen
    evi = (m_T[ti].m_bRev3d) ? 1-tvi : tvi;
    const int edge_trim_count = m_E[ei].m_ti.Count();
    if ( edge_trim_count < 1 )
      break; // should not happen
    if ( edge_trim_count == 1 ) {
      SetEdgeVertex( ei, evi, vi );
      return true; // ran into boundary - expected
    }
    if ( !HopAcrossEdge( ti, tvi ) )
      return false;
  }

  return false; // should have exited by hitting "expected" boundary ~10 lines above
}

void MYON_Brep::SetLoopVertices( const int li )
{
  MYON_BrepLoop& loop = m_L[li];
  const int loop_trim_count = loop.m_ti.Count();
  int lti;
  for ( lti = 0; lti < loop_trim_count; lti++ ) {
    const int ti = loop.m_ti[lti];
    MYON_BrepTrim& trim = m_T[ti];
    int vi = trim.m_vi[0];
    if ( vi >= 0 )
      continue;
    MYON_BrepVertex& v = NewVertex();
    SetTrimStartVertex( ti, v.m_vertex_index );
  }
}

void MYON_Brep::ClearTrimVertices()
{
  int ti;
  const int tcnt = m_T.Count();
  for ( ti = 0; ti < tcnt; ti++ ) {
    MYON_BrepTrim& trim = m_T[ti];
    trim.m_vi[0] = -1;
    trim.m_vi[1] = -1;
  }
}

void MYON_Brep::ClearEdgeVertices()
{
  int ei;
  const int ecnt = m_E.Count();
  for ( ei = 0; ei < ecnt; ei++ ) {
    MYON_BrepEdge& edge = m_E[ei];
    edge.m_vi[0] = -1;
    edge.m_vi[1] = -1;
  }
}

int MYON_Brep::NextTrim(int ti) const
{
  const MYON_BrepTrim& trim = m_T[ti];
  const int li = trim.m_li;
  if (li < 0 || li >= m_L.Count())
    return -1;
  const MYON_BrepLoop& loop = m_L[li];
  const int trim_count = loop.m_ti.Count();
  int lti;
  for ( lti = 0;  lti < trim_count && loop.m_ti[lti] != ti; lti++)
    ;/* empty for*/
  if ( lti < 0 || lti >= trim_count )
    return -1;
  return loop.m_ti[(lti+1)%trim_count];
}

int MYON_Brep::PrevTrim(int ti) const
{
  const MYON_BrepTrim& trim = m_T[ti];
  const int li = trim.m_li;
  if (li < 0 || li >= m_L.Count())
    return -1;
  const MYON_BrepLoop& loop = m_L[li];
  const int trim_count = loop.m_ti.Count();
  int lti;
  for ( lti = 0; lti < trim_count && loop.m_ti[lti] != ti; lti++)
    ;/* empty for*/
  if ( lti < 0 || lti >= trim_count )
    return -1;
  return loop.m_ti[(lti+trim_count-1)%trim_count];
}

int MYON_Brep::NextNonsingularTrim(int tid) const

{
  if (tid >= m_T.Count() || tid < 0)
    return -1;
  bool bSing =  (m_T[tid].m_type == MYON_BrepTrim::singular) ? true : false;
  int ntid;
  ntid = NextTrim(tid);
  while (ntid >=0 && m_T[ntid].m_type == MYON_BrepTrim::singular){
    ntid = NextTrim(ntid);
    if (ntid == tid && bSing)
      return -1;
  }
  return ntid;
}

int MYON_Brep::PrevNonsingularTrim(int tid) const

{
  if (tid >= m_T.Count() || tid < 0)
    return -1;
  bool bSing =  (m_T[tid].m_type == MYON_BrepTrim::singular) ? true : false;
  int ntid;
  ntid = PrevTrim(tid);
  while (ntid >= 0 && m_T[ntid].m_type == MYON_BrepTrim::singular){
    ntid = PrevTrim(ntid);
    if (ntid == tid && bSing)
      return -1;
  }
  return ntid;
}


int MYON_Brep::NextEdge(int ei, int endi, int* next_endi ) const
{
  const MYON_BrepEdge& edge = m_E[ei];
  const int vi = edge.m_vi[endi];
  const MYON_BrepVertex& vertex = m_V[vi];
  const int edge_count = vertex.m_ei.Count();
  int vei;
  if ( edge_count < 2 )
    return -1;
  if ( next_endi )
    *next_endi = 0;
  for ( vei = 0; vertex.m_ei[vei] != ei && vei < edge_count; vei++)
    ;/* empty for*/
  if ( edge.m_vi[0] == edge.m_vi[1]  && endi ) {
    // get next occurance of edge index
    //
    // On closed edges, edge.m_vi[0] = edge.m_vi[1] and edge.edge_index 
    // appears TWICE in vertex.m_ei[].  The first occurance of edge.edge_index
    // in vertex.m_ei[] corresponds to edge.m_vi[0].  The second occurance
    // of edge.edge_index in vertex.m_ei[] corresponds to edge.m_vi[1].
    vei++;
    while ( vei < edge_count && vertex.m_ei[vei] != ei )
      vei++;
  }
  if ( vei < 0 || vei >= edge_count )
    return -1;
  vei = (vei+1)%edge_count;
  const int next_ei = vertex.m_ei[vei];
  if ( next_endi ) {
    if ( m_E[next_ei].m_vi[0] == m_E[next_ei].m_vi[1] ) {
      *next_endi = 1;
      for ( vei++; vei < edge_count; vei++ ) {
        if ( vertex.m_ei[vei] == next_ei ) {
          *next_endi = 0;
          break;
        }
      }
    }
    else if ( m_E[next_ei].m_vi[1] == vi )
      *next_endi = 1;
  }
  return next_ei;
}

int MYON_Brep::PrevEdge(int ei, int endi, int* prev_endi ) const
{
  const MYON_BrepEdge& edge = m_E[ei];
  const int vi = edge.m_vi[endi];
  const MYON_BrepVertex& vertex = m_V[vi];
  const int edge_count = vertex.m_ei.Count();
  if ( edge_count < 2 )
    return -1;
  int vei;
  if ( prev_endi )
    *prev_endi = 0;
  for ( vei = 0; vertex.m_ei[vei] != ei && vei < edge_count; vei++)
    ;/* empty for*/
  if ( edge.m_vi[0] == edge.m_vi[1] && endi ) {
    // get next occurance of edge index
    //
    // On closed edges, edge.m_vi[0] = edge.m_vi[1] and edge.edge_index 
    // appears TWICE in vertex.m_ei[].  The first occurance of edge.edge_index
    // in vertex.m_ei[] corresponds to edge.m_vi[0].  The second occurance
    // of edge.edge_index in vertex.m_ei[] corresponds to edge.m_vi[1].
    vei++;
    while ( vei < edge_count && vertex.m_ei[vei] != ei )
      vei++;
  }
  if ( vei < 0 || vei >= edge_count )
    return -1;
  vei = (vei+edge_count-1)%edge_count;
  const int prev_ei = vertex.m_ei[(vei+edge_count-1)%edge_count];
  if ( prev_endi ) {
    if ( m_E[prev_ei].m_vi[0] == m_E[prev_ei].m_vi[1] ) {
      *prev_endi = 1;
      for ( vei++; vei < edge_count; vei++ ) {
        if ( vertex.m_ei[vei] == prev_ei ) {
          *prev_endi = 0;
          break;
        }
      }
    }
    else if ( m_E[prev_ei].m_vi[1] == vi ) {
      *prev_endi = 1;
    }
  }
  return prev_ei;
}

// HELPER CLASS - DO NOT PUT DEFINITION IN A HEADER FILE
class MYON__EDGE_ENDS
{
public:
  // used to sort vertices of closed edges that need
  // to be combined.
  int vi0; // smallest edge vertex index
  int vi1; // largest edge vertex index
  int ei;  // index of closed edge
  bool operator<(const MYON__EDGE_ENDS& other) const
  {
    int i = other.vi0 - vi0;
    if ( i < 0 ) return true;
    if ( i > 0 ) return false;
    i = other.vi1 - vi1;
    if ( i < 0 ) return true;
    if ( i > 0 ) return false;
    i = other.ei - ei;
    if ( i < 0 ) return true;
    return false;
  }
};

void MYON_Brep::SetVertices(void)
{
  const int face_count = m_F.Count();
  int fi;
  
  ClearEdgeVertices();
  ClearTrimVertices();
  m_V.Empty();
  m_V.Shrink();
  fi = m_E.Count() - m_F.Count() + 8;
  if ( fi < 32 )
    fi = 32;
  m_V.Reserve( fi );
  for ( fi = 0; fi < face_count; fi++ ) {
    MYON_BrepFace& face = m_F[fi];
    const int loop_count = face.m_li.Count();
    int fli;
    for ( fli = 0; fli < loop_count; fli++ ) {
      SetLoopVertices( face.m_li[fli] );
    }
  }

  // average edges' end location to get vertex location
  const int vertex_count = m_V.Count();
  int vi;
  MYON_3dPoint VP(MYON_3dPoint::Origin), EP;
  for ( vi = 0; vi < vertex_count; vi++ ) 
  {
    VP = MYON_3dPoint::Origin;
    double d = 0.0;
    MYON_BrepVertex& vertex = m_V[vi];
    const int edge_count = vertex.m_ei.Count();
    int vei;
    for ( vei = 0; vei < edge_count; vei++ ) 
    {
      const int ei = vertex.m_ei[vei];
      if ( ei < 0 ) 
        continue;
      const MYON_BrepEdge& edge = m_E[ei];
      if ( edge.m_c3i < 0 )
        continue;
      const MYON_Curve* pC = edge.EdgeCurveOf();
      if ( !pC )
        continue;
      if ( edge.m_vi[0] == vi )
        EP = edge.PointAtStart();
      else if ( edge.m_vi[1] == vi )
        EP = edge.PointAtEnd();
      else
        continue;
      VP.x += EP.x;
      VP.y += EP.y;
      VP.z += EP.z;
      d += 1.0;
    }
    if ( d > 0.0 ) 
    {
      d = 1.0/d;
      vertex.point = d*VP;
    }
  }

  const int edge_count = m_E.Count();
  int ei;
  MYON_SimpleArray<MYON__EDGE_ENDS> edge_ends(edge_count/4 + 2);
  for ( ei = 0; ei < edge_count; ei++ )
  {
    // see if we have any 3d edges that are closed as 3d curves
    // but have distinct end vertices
    const MYON_BrepEdge& edge = m_E[ei];
    if (    edge.m_vi[0] >= 0 
         && edge.m_vi[1] >= 0
         && edge.m_vi[0] != edge.m_vi[1]
         && 0 != edge.EdgeCurveOf()
         && edge.IsClosed() )
    {
      MYON__EDGE_ENDS& ee = edge_ends.AppendNew();
      if ( edge.m_vi[0] < edge.m_vi[1] )
      {
        ee.vi0 = edge.m_vi[0];
        ee.vi1 = edge.m_vi[1];
      }
      else
      {
        ee.vi0 = edge.m_vi[1];
        ee.vi1 = edge.m_vi[0];
      }
      ee.ei = ei;
    }
  }

  if ( edge_ends.Count() > 0 )
  {
    // we need to combine some vertices and the ends of closed edges
    edge_ends.QuickSort( MYON_CompareIncreasing<MYON__EDGE_ENDS> );
    int edge_ends_count = edge_ends.Count();
    int i0, i1, vi0, vi1, i;

    // adjust indices of chained closed edges
    for ( i = 1; i < edge_ends_count; i++ )
    {
      bool bSortAgain = false;
      for ( i0 = 0; i0 < edge_ends_count; i0++ )
      {
        vi0 = edge_ends[i0].vi0;
        vi1 = edge_ends[i0].vi1;
        for ( i1 = i0+1; i1 < edge_ends_count; i1++ )
        {
          MYON__EDGE_ENDS& ee = edge_ends[i1];
          if ( ee.vi0 == vi1 )
          {
            ee.vi0 = vi0;
            bSortAgain = true;
          }
          if ( ee.vi1 == vi1 )
          {
            ee.vi1 = ee.vi0;
            ee.vi0 = vi0;
            bSortAgain = true;
          }
        }
      }
      if ( bSortAgain )
      {
        edge_ends.QuickSort( MYON_CompareIncreasing<MYON__EDGE_ENDS> );
      }
      else
        break;
    }

    // combine vertices at ends of closed edges into a single vertex
    bool bCullUnusedVertices = false;
    for ( i0 = 0, i1 = 1; i0 < edge_ends.Count(); i0 = i1 )
    {
      vi0 = edge_ends[i0].vi0;
      for ( i1 = i0+1; i1 < edge_ends.Count() && vi0 == edge_ends[i1].vi0; i1++ )
      {
        // empty body
      }
      vi1 = vi0;
      for ( i = i0; i < i1; i++ )
      {
        if ( edge_ends[i].vi1 > vi1 )
        {
          vi1 = edge_ends[i].vi1;
          if ( 0 <= vi0 && vi0 < vi1 && vi1 < m_V.Count())
          {
            CombineCoincidentVertices(m_V[vi0],m_V[vi1] );
            bCullUnusedVertices = true;
          }
        }
      }
    }
    if ( bCullUnusedVertices )
      CullUnusedVertices();
  }
}

void MYON_BrepTrim::m__legacy_flags_Set(int gcon, int mono)
{
  m__legacy_flags = 0;
  switch(gcon)
  {
  case -1:
    m__legacy_flags |= 1;
    break;
  case 0:
    m__legacy_flags |= 2;
    break;
  case 1:
    m__legacy_flags |= 3;
    break;
  case 2:
    m__legacy_flags |= 4;
    break;
  }
  if (mono)
    m__legacy_flags |= 8;
  else
    m__legacy_flags |= 16;
}

bool MYON_BrepTrim::m__legacy_flags_Get(int* gcon, int* mono) const
{
  if ( gcon ) {
    switch ( m__legacy_flags & 7 )
    {
    case 1:
      *gcon = -1;
      break;
    case 2:
      *gcon = 0;
      break;
    case 3:
      *gcon = 1;
      break;
    case 4:
      *gcon = 2;
      break;
    default:
      *gcon = -1;
    }
  }
  if ( mono ) {
    if ( 0 != (m__legacy_flags&8) )
      *mono = 1;
    else
      *mono = 0;
  }
  return m__legacy_flags ? true : false;
}
void MYON_Brep::SetTolsFromLegacyValues()
{
  // use m_2d_tol and m_3d_tol read from file to set public tolerances
  const int vcnt = m_V.Count();
  const int tcnt = m_T.Count();
  MYON_3dPoint endP;
  double d;
  int vi, ti, vei, evi, vecnt;
  
  // set trim and edge tolerances from values in file
  for ( ti = 0; ti < tcnt; ti++ ) {
    MYON_BrepTrim& trim = m_T[ti];
    trim.m_tolerance[0] = trim.m__legacy_2d_tol; // "pe_tol"
    trim.m_tolerance[1] = trim.m__legacy_2d_tol; // "pe_tol"
    if ( trim.m_ei >= 0 ) {
      MYON_BrepEdge& edge = m_E[trim.m_ei];
      if ( edge.m_tolerance < trim.m__legacy_3d_tol )
        edge.m_tolerance = trim.m__legacy_3d_tol; // "e_tol"
    }
  }

  // set vertex tolerances from edge tols and end evaluations
  for ( vi = 0; vi < vcnt; vi++ ) {
    MYON_BrepVertex& vertex = m_V[vi];
    vecnt = vertex.m_ei.Count();
    for ( vei = 0; vei < vecnt; vei++ ) {
      const MYON_BrepEdge& edge = m_E[vertex.m_ei[vei]];
      if ( vertex.m_tolerance < edge.m_tolerance )
        vertex.m_tolerance = edge.m_tolerance;
      const MYON_Curve* c = m_C3[edge.m_c3i];
      evi = 0;
      if ( edge.m_vi[0] != vi )
        evi = 1;
      if ( edge.m_vi[evi] == vi ) {
        endP = c->PointAt( c->Domain()[evi] );
        d = vertex.point.DistanceTo( endP );
        if ( d > vertex.m_tolerance ) {
          vertex.m_tolerance = d;      
        }
      }
    }
  }
}

MYON::object_type MYON_Brep::ObjectType() const
{
  // This must ALWAYS return MYON::brep_object.
  // NEVER modify this function to return any
  // other value.
  return MYON::brep_object;
}

bool MYON_Brep::GetTightBoundingBox(MYON_BoundingBox& tight_bbox, bool bGrowBox, const MYON_Xform* xform) const
{
  if (bGrowBox && !tight_bbox.IsValid())
  {
    bGrowBox = false;
  }

  if (!bGrowBox)
  {
    tight_bbox.Destroy();
  }

  MYON_BoundingBox local_bbox;

  // Test vertexes first, this will quickly give us a base bbox to work with. 
  int vct = m_V.Count();
  for (int i = 0; vct > i; i++)
  {
    if (m_V[i].GetTightBoundingBox(local_bbox, bGrowBox, xform))
      bGrowBox = true;
  }

  MYON_SimpleArray<MYON_Curve*> iso_curves;
  MYON_SimpleArray<double> greville_abcissae;
  int fct = m_F.Count();
  for (int i = 0; fct > i; i++)
  {
    const MYON_BrepFace& face = m_F[i];

    //15 July 2018 - Chuck - If a face has been deleted, it will not have an associated surface.
    if (face.SurfaceOf() == NULL)
      continue;

    MYON_NurbsSurface nsrf;
    if (0 == face.SurfaceOf()->GetNurbForm(nsrf) || false == nsrf.IsValid())
      return false;

    // See if entire surface bbox is included in current local bbox, it is
    // not necessary to calculate the tight bounding box. 
    // Continue to next face if it is.
    if (local_bbox.Includes(nsrf.BoundingBox()))
      continue;

    const MYON_Mesh* mesh = face.Mesh(MYON::render_mesh);
    if (nullptr == mesh)
      mesh = face.Mesh(MYON::analysis_mesh);
    if (nullptr == mesh)
      mesh = face.Mesh(MYON::any_mesh);
    if (nullptr != mesh)
    {
      if (mesh->GetTightBoundingBox(local_bbox, bGrowBox, xform))
        bGrowBox = true;
    }

    for (int j = 0; 2 > j; j++)
    {
      int k, kct = nsrf.CVCount(j);
      greville_abcissae.Reserve(kct);
      greville_abcissae.SetCount(kct);
      nsrf.GetGrevilleAbcissae(j, greville_abcissae);
      kct = greville_abcissae.Count();
      for (k = 0; kct > k; k++)
      {
        iso_curves.Append(nsrf.IsoCurve(j == 0 ? 1 : 0, greville_abcissae[k]));
      }
    }

    int ict = iso_curves.Count();
    for (int j = 0; ict > j; j++)
    {
      if (nullptr == iso_curves[j])
        continue;

      // See if entire iso_curve bbox is included in current local bbox, it is
      // not necessary to calculate the tight bounding box. 
      // Continue to next iso_curve if it is.
      if (false == local_bbox.Includes(iso_curves[j]->BoundingBox()))
      {
        if (iso_curves[j]->GetTightBoundingBox(local_bbox, bGrowBox, xform))
          bGrowBox = true;
      }

      delete iso_curves[j];
      iso_curves[j] = nullptr;
    }
  }

  int ect = m_E.Count();
  for (int i = 0; ect > i; i++)
  {
    // See if entire edge bbox is included in current local bbox, it is
    // not necessary to calculate the tight bounding box. 
    // Continue to next edge if it is.
    if (local_bbox.Includes(m_E[i].BoundingBox()))
      continue;

    if (m_E[i].GetTightBoundingBox(local_bbox, bGrowBox, xform))
      bGrowBox = true;
  }


  if (bGrowBox)
    tight_bbox.Union(tight_bbox, local_bbox);
  else
  {
    tight_bbox = local_bbox;
    bGrowBox = tight_bbox.IsValid();
  }

  return 0 != bGrowBox;
}

void MYON_Brep::ClearBoundingBox()
{
  m_bbox.Destroy();
}

void MYON_BrepFace::ClearBoundingBox()
{
  m_bbox.Destroy();
}


const MYON_BoundingBox MYON_BrepFace::InternalFaceBoundingBox(bool bLazy, bool bUpdateCachedBBox) const
{
  if (bLazy && m_bbox.IsNotEmpty())
    return m_bbox;

  for (;;)
  {
    // Make sure this face is valid enough to query the trims and proxy surface
    if (nullptr == m_brep)
      break;
    if (m_face_index < 0)
      break;
    if (m_face_index >= m_brep->m_F.Count())
      break;
    if (&m_brep->m_F[m_face_index] != this)
      break;

    const MYON_Surface* proxy_srf = ProxySurface();
    if (nullptr == proxy_srf)
      break;
    if (proxy_srf == this)
      break;

    MYON_BoundingBox pbox = MYON_BoundingBox::NanBoundingBox;
    for (int li = 0; li < LoopCount(); li++)
    {
      MYON_BrepLoop* loop = Loop(li);
      if (loop && loop->m_type == MYON_BrepLoop::outer)
      {
        m_brep->SetTrimBoundingBoxes(*loop, true);       // sets loop->m_pbox 
        if (false == loop->GetBoundingBox(pbox, pbox.IsValid()))
          pbox = MYON_BoundingBox::UnsetBoundingBox;
        break;
      }
    }


    MYON_BoundingBox face_bbox = MYON_BoundingBox::NanBoundingBox;
    if (pbox.IsNotEmpty())
    {
      MYON_Interval pudom(pbox[0].x, pbox[1].x);
      MYON_Interval pvdom(pbox[0].y, pbox[1].y);
      // fatten up invervals to get slightly larger boxes...
      pudom.Expand(0.1 * pudom.Length());
      pvdom.Expand(0.1 * pvdom.Length());
      MYON_Interval Sdom[] = { Domain(0), Domain(1) };
      // but don't let the fattened intervals extend beyond Sdom
      pudom.Intersection(Sdom[0]);
      pvdom.Intersection(Sdom[1]);
      if (pbox.IsValid() &&
        (Sdom[0].Includes(pudom, true) || Sdom[1].Includes(pvdom, true))
        )
      {
        MYON_Surface* temp_srf = DuplicateSurface();
        if (nullptr != temp_srf)
        {
          if (Sdom[0].Includes(pudom, true))
            temp_srf->Trim(0, pudom);
          if (Sdom[1].Includes(pvdom, true))
            temp_srf->Trim(1, pvdom);
          if (false == temp_srf->GetBoundingBox(face_bbox, false))
            face_bbox = MYON_BoundingBox::NanBoundingBox;
          delete temp_srf;
          temp_srf = nullptr;
        }
      }
    }

    if (false == face_bbox.IsNotEmpty())
    {
      if (false == proxy_srf->GetBoundingBox(face_bbox, false))
        break;
      if (false == face_bbox.IsNotEmpty())
        break;
    }

    if (bUpdateCachedBBox)
      const_cast<MYON_BrepFace*>(this)->m_bbox = face_bbox;
    return face_bbox;
  }

  // MYON_Brep code has always used MYON_BoundingBox::EmptyBoundingBox 
  // to indicate a bounding box is not set. If it were to be written
  // in modern times, it would have used Nans.
  return MYON_BoundingBox::EmptyBoundingBox;
}

const MYON_BoundingBox MYON_Brep::InternalBrepBoundingBox(bool bLazy, bool bUpdateCachedBBox) const
{
  if (bLazy && m_bbox.IsNotEmpty())
    return m_bbox;

  MYON_BoundingBox brep_bbox;
  const int face_count = m_F.Count();
  int fi;
  for (fi = 0; fi < face_count; fi++)
  {
    if (m_F[fi].m_face_index == -1)
      continue;

    //GBA 20 May 2020. RH-58462. Brep box now computed from face boxes, instead of surface boxes.
    const MYON_BrepFace* f = Face(fi);
    if (nullptr == f)
      continue;

    const MYON_BoundingBox face_bbox = f->InternalFaceBoundingBox(bLazy, bUpdateCachedBBox);
    if (false == face_bbox.IsNotEmpty())
      continue;

    brep_bbox.Union(face_bbox);
  }

  if (false == brep_bbox.IsNotEmpty())
  {
    // MYON_Brep code has always used MYON_BoundingBox::EmptyBoundingBox 
    // to indicate a bounding box is not set. If it were to be written
    // in modern times, it would have used Nans.
    return MYON_BoundingBox::EmptyBoundingBox;
  }

  if (bUpdateCachedBBox)
    const_cast<MYON_Brep*>(this)->m_bbox = brep_bbox;
  return brep_bbox;
}

// MYON_BrepFace::GetBBox performs lazy evaluation.  
// Namely, if m_bbox is invalid then the bounding box is
// computed and the value is stored in m_bbox to speed future calls.
bool MYON_BrepFace::GetBBox(
          double* box_min, // [3],
          double* box_max, // [3],
          bool bGrowBox     // = false
          ) const
{
  MYON_BoundingBox bbox = this->InternalFaceBoundingBox(true, true);
  
  bool rc = bbox.IsValid();
  if (rc)
  {
    if ( bGrowBox && box_min && box_max && box_min[0] <= box_max[0] )
    {
      bbox.Union( MYON_BoundingBox( MYON_3dPoint(box_min), MYON_3dPoint(box_max) ) );
    }
    if ( box_min )
    {
      box_min[0] = bbox.m_min.x;
      box_min[1] = bbox.m_min.y;
      box_min[2] = bbox.m_min.z;
    }
    if ( box_max )
    {
      box_max[0] = bbox.m_max.x;
      box_max[1] = bbox.m_max.y;
      box_max[2] = bbox.m_max.z;
    }
  }
  return rc;
}


bool MYON_Brep::GetBBox(
          double* box_min, // [3],
          double* box_max, // [3],
          bool bGrowBox     // = false
          ) const
{
  MYON_BoundingBox bbox = this->InternalBrepBoundingBox(true, true);

  bool rc = bbox.IsValid();
  if (rc)
  {
    bbox = m_bbox;
    if ( bGrowBox && box_min && box_max && box_min[0] <= box_max[0] )
    {
      bbox.Union( MYON_BoundingBox( MYON_3dPoint(box_min), MYON_3dPoint(box_max) ) );
    }
    if ( box_min )
    {
      box_min[0] = bbox.m_min.x;
      box_min[1] = bbox.m_min.y;
      box_min[2] = bbox.m_min.z;
    }
    if ( box_max )
    {
      box_max[0] = bbox.m_max.x;
      box_max[1] = bbox.m_max.y;
      box_max[2] = bbox.m_max.z;
    }
  }
  return rc;
}

bool
MYON_Brep::SwapCoordinates( int i, int j )
{
  bool rc = false;
  // swap surface coordinates
  const int srf_count = m_S.Count();
  int si;
  for ( si = 0; si < srf_count; si++ ) {
    if ( !m_S[si] ) 
      continue;
    rc = m_S[si]->SwapCoordinates(i,j);
    if ( !rc ) {
      while ( --si >= 0 ) {
        // undo any changes;
        if ( m_S[si] )
          m_S[si]->SwapCoordinates(i,j);
      }
      return false;
    }
  }
  // swap 3d curve coordinates
  const int crv_count = m_S.Count();
  int ci;
  for ( ci = 0; ci < crv_count; ci++ ) {
    if ( !m_C3[ci] ) 
      continue;
    rc = m_C3[ci]->SwapCoordinates(i,j);
    if ( !rc ) {
      // undo any changes;
      while ( --ci >= 0 ) {
        if ( m_C3[ci] )
          m_C3[ci]->SwapCoordinates(i,j);
        for ( si = 0; si < srf_count; si++ ) {
          if ( m_S[si] )
            m_S[si]->SwapCoordinates(i,j);
        }
      }
      return false;
    }
  }
  return rc;
}

bool
MYON_Brep::SwapTrimParameters(
        int trim_index
        )
{
  // helper for SwapLoopParameters
  if ( trim_index < 0 || trim_index >= m_T.Count() )
    return false;
  MYON_BrepTrim& trim = m_T[trim_index];

  StandardizeTrimCurve(trim_index);

  const int ci = trim.m_c2i;
  if ( ci < 0 || ci >= m_C2.Count() )
    return false;
  MYON_Curve* pC = m_C2[ci];
  if ( !pC )
    return false;

  //MYON_Interval pdom = trim.ProxyCurveDomain();
  //MYON_Interval trimdom = trim.Domain();

  // have to call SwapCoordinates on pC because
  // MYON_CurveProxy does not permit modification
  // of "real" curve.
  bool rc = pC->SwapCoordinates(0,1); // "u" <-> "v"
  if ( !rc )
    return false;

  // reverse 2d curve
  rc = pC->Reverse();
  if (rc)
  {
    // take care of proxy house keeping, m_vi[] swapping, and toggle m_bRev3d.
    trim.SetProxyCurve(pC);
    int i = trim.m_vi[0];
    trim.m_vi[0] = trim.m_vi[1];
    trim.m_vi[1] = i;
    if ( trim.m_ei >= 0 )
      trim.m_bRev3d = trim.m_bRev3d ? false : true;
  }
  else
  {
    // undo changes
    rc = pC->SwapCoordinates(0,1); // "u" <-> "v"
    return false;
  }

  // reflect iso type
  switch ( trim.m_iso ) 
  {
  case MYON_Surface::not_iso:
    trim.m_iso = MYON_Surface::not_iso;
    break;
  case MYON_Surface::x_iso:
    trim.m_iso = MYON_Surface::y_iso;
    break;
  case MYON_Surface::y_iso:
    trim.m_iso = MYON_Surface::x_iso;
    break;
  case MYON_Surface::W_iso:
    trim.m_iso = MYON_Surface::S_iso;
    break;
  case MYON_Surface::S_iso:
    trim.m_iso = MYON_Surface::W_iso;
    break;
  case MYON_Surface::E_iso:
    trim.m_iso = MYON_Surface::N_iso;
    break;
  case MYON_Surface::N_iso:
    trim.m_iso = MYON_Surface::E_iso;
    break;
  default:
    trim.m_iso = MYON_Surface::not_iso;
    break;
  }

  return true;
}

bool
MYON_Brep::SwapLoopParameters(
        int loop_index
        )
{
  bool rc = false;
  if ( loop_index < 0 || loop_index >= m_L.Count() )
    return false;
  MYON_BrepLoop& L = m_L[loop_index];
  const int loop_trim_count = L.m_ti.Count();
  if ( loop_trim_count < 1 )
    return false;
  int lti, ti;
  for ( lti = 0; lti < loop_trim_count; lti++ ) {
    ti = L.m_ti[lti];
    rc = SwapTrimParameters( ti );
    if ( !rc ) {
      while ( --lti >= 0 ) {
        // undo any changes
        ti = L.m_ti[lti];
        SwapTrimParameters( ti );
      }
      return false;
    }
  }

  // reverse order of trimming curves
  if ( rc )
    L.m_ti.Reverse();
  return rc;
}

bool
MYON_Brep::IsSolid() const
{
  bool bIsOriented = false;
  bool bHasBoundary = true;
  bool bIsManifold = IsManifold( &bIsOriented, &bHasBoundary );
  return (bIsManifold && bIsOriented && !bHasBoundary) ? true : false;
}

void MYON_Brep::SetSolidOrientationForExperts(int solid_orientation)
{
  switch (solid_orientation)
  {
  case 0: // not a solid
    m_is_solid = 3;
    break;
  case 1: // solid with normals pointing out
    m_is_solid = 1;
    break;
  case -1: // solid with normals pointing in
    m_is_solid = 2;
    break;
  default:
    break;
  }
}

int MYON_Brep::SolidOrientation() const
{
  // m_is_solid values:
  //   0 = unset
  //   1 = solid with normals pointing out
  //   2 = solid with normals pointing in
  //   3 = not solid
  int rc = 0;
  switch( m_is_solid )
  {
  case 1: // solid with normals pointing out
    rc = 1;
    break;
  case 2: // solid with normals pointing in
    rc = -1;
    break;
  case 3: // not a solid
    rc = 0;
    break;
  
  default:
    if ( IsSolid() )
    {
      // this virtual function is overridden in Rhino SDK
      // and sets m_is_solid to appropriate values.  This
      // stand-alone version cannot tell the difference
      // between solids with inward pointing normals and
      // solids with outwards pointing normals.
      //MYON_Brep* p = const_cast<MYON_Brep*>(this);
      //p->m_is_solid = 1;
      rc = 2;
    }
    else
    {
      MYON_Brep* p = const_cast<MYON_Brep*>(this);
      p->m_is_solid = 3;
      rc = 0;
    }
  }
  return rc;
}

bool
MYON_Brep::IsManifold( bool* pbIsOriented, bool* pbHasBoundary ) const
{
  const int fcnt = m_F.Count();
  bool bIsManifold = (fcnt > 0) ? true : false;
  bool bIsOriented = bIsManifold;
  bool bHasBoundary = false;
  int fi, other_ti, lcnt, tcnt, fli, lti;
  if ( pbIsOriented )
    *pbIsOriented = bIsOriented;
  if ( pbHasBoundary )
    *pbHasBoundary = bHasBoundary;
  const int brep_loop_count = m_L.Count();
  const int brep_trim_count = m_T.Count();
  const int brep_edge_count = m_E.Count();

  bool bKeepGoing = bIsManifold;

  for ( fi = 0; fi < fcnt && bKeepGoing; fi++ ) 
  {
    const MYON_BrepFace& face = m_F[fi];
    if ( -1 == face.m_face_index )
    {
      // 28 October 2010 - Dale Lear and Chuck
      //    Do not test deleted faces. The join
      //    command calls is manifold with some
      //    deleted faces to avoid calling Compact
      //    lots of times during a join.
      continue;
    }

    lcnt = face.m_li.Count();
    if ( lcnt < 1 ) {
      bIsManifold = false;
      if (!pbHasBoundary)
        bKeepGoing = false;
    }

    for ( fli = 0; fli < lcnt && bKeepGoing; fli++ ) 
    {
      const int li = face.m_li[fli];
      if ( li < 0 || li >= brep_loop_count )
      {
        MYON_ERROR("Bogus loop index in face.m_li[]");
        continue;
      }
      const MYON_BrepLoop& loop = m_L[li];
      tcnt = loop.m_ti.Count();
      if (tcnt < 1 ) {
        bIsManifold = false;
        if (!pbHasBoundary)
          bKeepGoing = false;
      }
      for ( lti = 0; lti < tcnt && bKeepGoing; lti++ ) 
      {
        const int ti = loop.m_ti[lti];
        if ( ti < 0 || ti >= brep_trim_count )
        {
          MYON_ERROR("Bogus loop index in loop.m_ti[]");
          continue;
        }
        const MYON_BrepTrim& trim = m_T[ti];
        switch ( trim.m_type ) 
        {
        case MYON_BrepTrim::boundary:
          bHasBoundary = true;
          break;
        case MYON_BrepTrim::mated:
        case MYON_BrepTrim::seam:
          // make sure we have a manifold join
          if ( trim.m_ei >= 0 && trim.m_ei < brep_edge_count )
          {
            const MYON_BrepEdge& edge = m_E[trim.m_ei];
            if ( edge.m_ti.Count() != 2 ) {
              bIsManifold = false;
              if (!pbHasBoundary)
                bKeepGoing = false;
            }
            else 
            {
              other_ti = edge.m_ti[0];
              if ( other_ti == ti )
                other_ti = edge.m_ti[1];
              if ( other_ti == ti )
              {
                bIsManifold = false;
                if (!pbHasBoundary)
                  bKeepGoing = false;
              }
              else 
              {
                const MYON_BrepTrim& other_trim = m_T[other_ti];

                // Nov 9, 2011 Tim - Fix for crash bug RR 93743
                // Better index checking.

                bool bFlipTrim = trim.m_bRev3d;
                if (0 <= trim.m_li && brep_loop_count > trim.m_li)
                {
                  if (m_L[trim.m_li].m_fi >= 0 && m_L[trim.m_li].m_fi < m_F.Count()){
                    if ( m_F[m_L[trim.m_li].m_fi].m_bRev )
                      bFlipTrim = !bFlipTrim;
                  }
                  else
                  {
                    MYON_ERROR("Bogus face index in m_L[trim.m_li]");
                    continue;
                  }
                }
                else
                {
                  MYON_ERROR("Bogus loop index in trim.m_li");
                  continue;
                }

                bool bFlipOther = other_trim.m_bRev3d;
                if (0 <= other_trim.m_li && brep_loop_count > other_trim.m_li)
                {
                  if (m_L[other_trim.m_li].m_fi >= 0 && m_L[other_trim.m_li].m_fi < m_F.Count()){
                    if ( m_F[m_L[other_trim.m_li].m_fi].m_bRev )
                      bFlipOther = !bFlipOther;
                  }
                  else
                  {
                    MYON_ERROR("Bogus face index in m_L[other_trim.m_li]");
                    continue;
                  }
                }
                else
                {
                  MYON_ERROR("Bogus loop index in other_trim.m_li");
                  continue;
                }

                if ( bFlipTrim && bFlipOther )
                {
                  bIsOriented = false;
                }
                else if ( !bFlipTrim && !bFlipOther )
                {
                  bIsOriented = false;
                }
              }
            }
          }
          else
          {
            MYON_ERROR("Bogus trim.m_ei or trim.m_type value");
          }
          break;
        case MYON_BrepTrim::singular:
          // nothing to check here
          break;
        default:
          bIsManifold = false;
          if (!pbHasBoundary)
            bKeepGoing = false;

          break;
        }
      }
    }
  }
  if ( !bIsManifold ) {
    bIsOriented = false;
    //21 June 2017 - Chuck - bHasBoundary just says if there are any naked edges.
    //Even if the brep is non-manifold, this should still be the case.
    //bHasBoundary = false;
  }
  if ( pbIsOriented )
    *pbIsOriented = bIsOriented;
  if ( pbHasBoundary )
    *pbHasBoundary = bHasBoundary;
  if ( !bIsManifold || bHasBoundary )
  {
    if ( m_is_solid != 3 )
    {
      // lazy evaluation used on m_is_solid
      const_cast<MYON_Brep*>(this)->m_is_solid = 3;
    }
  }

  return bIsManifold;
}


bool
MYON_Brep::IsSurface() const
{
  // returns true if the b-rep has a single face
  // and that face is geometrically the same
  // as the underlying surface.  I.e., the face
  // has trivial trimming.  In this case, the
  // surface is m_S[0].
  return (m_F.Count() == 1 && FaceIsSurface(0));
}

bool 
MYON_Brep::FaceIsSurface( int face_index ) const
{
  // returns true if the face has a single
  // outer boundary and that boundary runs
  // along the edges of the underlying surface.
  // In this case the geometry of the surface
  // is the same as the geometry of the face.

  bool bTrivialFace = false;
  if ( face_index >= 0 && face_index < m_F.Count() ) {
    const MYON_BrepFace& face = m_F[face_index];
    if ( face.m_li.Count() == 1 ) {
      bTrivialFace = LoopIsSurfaceBoundary( face.m_li[0] );
    }
  }
  return bTrivialFace;
}

bool 
MYON_Brep::LoopIsSurfaceBoundary( int loop_index ) const
{
  // returns true if the loop's trims run along the underlying surface boundary
  bool bTrivialLoop = false;
  if ( loop_index >= 0 && loop_index < m_L.Count() ) {
    const MYON_BrepLoop& loop = m_L[loop_index];
    const int trim_count = loop.m_ti.Count();
    if ( trim_count > 0 ) {
      bTrivialLoop = true;
      for ( int lti = 0; lti < trim_count && bTrivialLoop; lti++ ) 
      {
        int ti = loop.m_ti[lti];
        if ( ti < 0 || ti >= m_T.Count() )
        {
          MYON_ERROR("Bogus trim index in loop.m_ti[]");
          return false;
        }
        const MYON_BrepTrim& trim = m_T[ti];
        if ( trim.m_iso == MYON_Surface::W_iso )
          continue;
        if ( trim.m_iso == MYON_Surface::S_iso )
          continue;
        if ( trim.m_iso == MYON_Surface::N_iso )
          continue;
        if ( trim.m_iso == MYON_Surface::E_iso )
          continue;
        bTrivialLoop = false;
      }
    }
  }
  return bTrivialLoop;
}

bool
MYON_Brep::FlipReversedSurfaces()
{
  // Clears all MYON_BrepFace.m_bRev flags
  // by calling SwapFaceParameters() on each
  // face with a true m_bRev.
  //
  // Returns true if successful.

  // 11 April 2008 Dale Lear and Tim:
  //   face.Transpose() is clearing the m_is_solid
  //   flag but we are not changing the orientation
  //   of the brep.  This prevents having to perform
  //   the expensive step of calculating this flag
  //   again.
  int saved_is_solid = m_is_solid;

  const int face_count = m_F.Count();

  bool rc = true;
  int fi;
  for ( fi = 0; fi < face_count; fi++ ) 
  {
    MYON_BrepFace& face = m_F[fi];
    if ( face.m_bRev )
    {
      if ( !face.Transpose() )
        rc = false;
    }
  }

  m_is_solid = saved_is_solid;

  return rc;
}

////////
// Change the domain of a trim
bool MYON_Brep::SetTrimDomain(
       int trim_index, // index of trim in m_T[] array
       const MYON_Interval& domain
       )
{
  bool rc = false;
  if ( trim_index >= 0 && trim_index < m_T.Count() && domain.IsIncreasing() ) 
  {
    MYON_BrepTrim& trim = m_T[trim_index];
    rc = trim.SetDomain(domain);
  }
  return rc;
}

////////
// Change the domain of an edge
bool MYON_Brep::SetEdgeDomain(
       int edge_index, // index of edge in m_E[] array
       const MYON_Interval& domain
       )
{
  bool rc = false;
  if ( edge_index >= 0 && edge_index < m_E.Count() && domain.IsIncreasing() ) 
  {
    MYON_BrepEdge& edge = m_E[edge_index];
    rc = edge.SetDomain(domain);
  }
  return rc;
}

bool MYON_BrepFace::Reverse(int dir)
{
  if ( dir < 0 || dir > 1 || 0 == m_brep )
    return false;
  MYON_Surface* srf = const_cast<MYON_Surface*>(SurfaceOf());
  if ( !srf )
    return false;
  MYON_Interval dom0 = srf->Domain(dir);
  if ( !dom0.IsIncreasing() )
    return false;

// 2/18/03 GBA.  Destroy surface cache on face.
	DestroyRuntimeCache(true);

  if ( m_brep->SurfaceUseCount( m_si, 2 ) > 1 )
  {
    srf = srf->DuplicateSurface();
    m_si = m_brep->AddSurface( srf );
    SetProxySurface(srf);
  }

  if ( !srf->Reverse(dir) )
    return false;

  MYON_Interval dom1 = dom0;
  dom1.Reverse();
  if ( dom1 != srf->Domain(dir) )
  {
    srf->SetDomain( dir, dom1 );
    dom1 = srf->Domain(dir);
  }

  // adjust location of 2d trim curves
  MYON_Xform xform(MYON_Xform::IdentityTransformation);
  xform.IntervalChange(dir,dom0,MYON_Interval(dom1[1],dom1[0]));
  TransformTrim(xform);

  // reverse loop orientations.
  int fli;
  for ( fli = 0; fli < m_li.Count(); fli++ )
  {
    MYON_BrepLoop* loop = m_brep->Loop(m_li[fli]);
    if ( loop )
      m_brep->FlipLoop( *loop );
  }
  
  m_bRev = m_bRev ? false : true;

  if (m_brep->m_is_solid == 1 || m_brep->m_is_solid == 2) m_brep->m_is_solid = 0;

	// Greg Arden 10 April 2003.  Fix TRR#9624.  
	// Update analysis and render meshes.
  {
    auto rm = UniqueMesh(MYON::render_mesh);
	  if(rm)
    {
      auto p = const_cast<MYON_Mesh*>(rm.get());
      p->ReverseSurfaceParameters(dir);
      p->ReverseTextureCoordinates(dir);
    }
  }

  {
    auto rm = UniqueMesh(MYON::analysis_mesh);
    if (rm)
    {
      auto p = const_cast<MYON_Mesh*>(rm.get());
      p->ReverseSurfaceParameters(dir);
      p->ReverseTextureCoordinates(dir);
    }
  }

  return true;
}

bool MYON_BrepFace::Transpose()
{
  if ( 0 == m_brep )
    return false;

  MYON_Surface* srf = const_cast<MYON_Surface*>(SurfaceOf());
  if ( 0 == srf )
    return false;

  // 2/18/03 GBA.  Destroy cache on the face.
	DestroyRuntimeCache(true);

  // make sure only one face uses this surface
  if ( m_brep->SurfaceUseCount( m_si, 2 ) > 1 )
  {
    srf = srf->DuplicateSurface();
    m_si = m_brep->AddSurface(srf);
    SetProxySurface(srf);
  }

  //MYON_Interval u0 = srf->Domain(0);
  //MYON_Interval v0 = srf->Domain(1);

  // swap surface "u" and "v"
  bool rc = srf->Transpose();
  if ( !rc )
    return false;

  //MYON_Interval u1 = srf->Domain(0);
  //MYON_Interval v1 = srf->Domain(1);

  MYON_Xform xform(MYON_Xform::IdentityTransformation);
  xform[0][0] = 0.0;
  xform[0][1] = 1.0;
  xform[1][0] = 1.0;
  xform[1][1] = 0.0;

  TransformTrim(xform);

  // reverse loop orientations.
  int fli;
  for ( fli = 0; fli < m_li.Count(); fli++ )
  {
    MYON_BrepLoop* loop = m_brep->Loop(m_li[fli]);
    if ( loop )
      m_brep->FlipLoop( *loop );
  }
  
  m_bRev = m_bRev ? false : true;

  
  // 11 April 2008 Dale Lear:
  //    Transposing the surface and then toggling the m_bRev
  //    does not alter the brep's orientation.  Setting this flag
  //    to zero means we will have to do an unnecessary and
  //    expensive calculation in the future.
  //if (m_brep->m_is_solid == 1 || m_brep->m_is_solid == 2) m_brep->m_is_solid = 0;

	// Update analysis mesh and render mesh.
  // (Greg Arden 10 April 2003.  Fix TRR#9624.)
  {
    auto rm = UniqueMesh(MYON::render_mesh);
    if (rm)
    {
      auto p = const_cast<MYON_Mesh*>(rm.get());
      p->TransposeSurfaceParameters();
      p->TransposeTextureCoordinates();
    }
  }

  {
    auto rm = UniqueMesh(MYON::analysis_mesh);
    if (rm)
    {
      auto p = const_cast<MYON_Mesh*>(rm.get());
      p->TransposeSurfaceParameters();
      p->TransposeTextureCoordinates();
    }
  }

  return true;
}

bool MYON_BrepFace::SetDomain(
       MYON_Interval u_dom,
       MYON_Interval v_dom
       )
{
  if ( 0 == m_brep )
    return false;
  if ( !u_dom.IsIncreasing() )
    return false;
  if ( !v_dom.IsIncreasing() )
    return false;
  
  MYON_Surface* srf = const_cast<MYON_Surface*>(SurfaceOf());
  if ( 0 == srf )
    return false;

  MYON_Interval u0_dom = srf->Domain(0);
  MYON_Interval v0_dom = srf->Domain(1);
  if ( u0_dom == u_dom && v0_dom == v_dom )
    return true;

  MYON_Xform xform(MYON_Xform::IdentityTransformation);
  {
    MYON_Xform ux(MYON_Xform::IdentityTransformation), vx(MYON_Xform::IdentityTransformation);
    if ( u0_dom != u_dom )
    {
      if ( !ux.IntervalChange(0,u0_dom,u_dom) )
        return false;
    }
    if ( v0_dom != v_dom )
    {
      if ( !vx.IntervalChange(1,v0_dom,v_dom) )
        return false;
    }
    xform = ux*vx;
  }

// 2/18/03 GBA.  Destroy cache on the face.
	DestroyRuntimeCache(true);

  if ( m_brep->SurfaceUseCount( m_si, 2 ) > 1 )
  {
    srf = srf->DuplicateSurface();
    m_si = m_brep->AddSurface(srf);
    SetProxySurface(srf);
  }

  if ( u_dom != u0_dom )
  {
    if ( !srf->SetDomain( 0, u_dom ) )
      return false;
  }

  if ( v_dom != v0_dom )
  {
    if ( !srf->SetDomain( 1, v_dom ) )
    {
      srf->SetDomain(0,u0_dom );
      return false;
    }
  }

  // just to be sure 2d curves are in synch with actual surface
  // domain in case srf->SetDomain() does something weird.
  u_dom = srf->Domain(0);
  v_dom = srf->Domain(1);
  {
    MYON_Xform ux(MYON_Xform::IdentityTransformation), vx(MYON_Xform::IdentityTransformation);
    if ( u0_dom != u_dom )
    {
      if ( !ux.IntervalChange(0,u0_dom,u_dom) )
        return false;
    }
    if ( v0_dom != v_dom )
    {
      if ( !vx.IntervalChange(1,v0_dom,v_dom) )
        return false;
    }
    xform = ux*vx;
  }

  if ( !TransformTrim(xform) )
    return false;

  std::array<std::shared_ptr<const MYON_Mesh>, 3> meshes { UniqueMesh(MYON::analysis_mesh),  UniqueMesh(MYON::render_mesh),  UniqueMesh(MYON::preview_mesh) };

  for (auto& m : meshes)
  {
    if (m)
    {
      auto pMesh = const_cast<MYON_Mesh*>(m.get());
      for ( int dir = 0; dir < 2; dir++ )
      {
        MYON_Interval& mdom = pMesh->m_srf_domain[dir];

        const MYON_Interval dom0 = dir ? v0_dom : u0_dom;
        const MYON_Interval dom1 = dir ? v_dom : u_dom;

        if ( mdom.IsIncreasing() && dom0 != dom1 )
        {
          if ( mdom == dom0 )
          {
            mdom = dom1;
          }
          else
          {
            double t0 = dom1.ParameterAt(dom0.NormalizedParameterAt(mdom[0]));
            double t1 = dom1.ParameterAt(dom0.NormalizedParameterAt(mdom[1]));
            mdom.Set(t0,t1);
          }
        }
      }
    }
  }

  return true;
}

bool MYON_BrepFace::SetDomain( 
    int dir, // 0 sets first parameter's domain, 1 gets second parameter's domain
    double t0, 
    double t1
    )
{
  if (    dir < 0 
       || dir > 1 
       || t0 == MYON_UNSET_VALUE 
       || t1 == MYON_UNSET_VALUE 
       || t0 >= t1
       || 0 == m_brep )
    return false;

  MYON_Surface* srf = const_cast<MYON_Surface*>(SurfaceOf());
  if ( 0 == srf )
    return false;
  MYON_Interval udom = srf->Domain(0);
  MYON_Interval vdom = srf->Domain(1);
  if ( dir )
    vdom.Set(t0,t1);
  else
    udom.Set(t0,t1);

  return SetDomain( udom, vdom );
}



//bool MYON_Brep::ReverseFaceParameter(
//      int face_index, // index of face
//      int dir // dir = 0 reverse "u", 1 reverse "v"
//      )
//{
//  // OBSOLETE - use MYON_BrepFace::Reverse(dir)
//  bool rc = false;
//  MYON_BrepFace* face = Face(face_index);
//  if ( face )
//    rc = face->Reverse(dir)?true:false;
//  return rc;
//}

int MYON_Brep::TrimCurveUseCount( int c2_index, int max_count ) const
{
  int ti, use_count = 0;
  if ( max_count < 1 )
    max_count = m_T.Count();
  for ( ti = 0; ti < m_T.Count() && use_count < max_count; ti++ )
  {
    if ( m_T[ti].m_c2i == c2_index )
      use_count++;
  }
  return use_count;
}

int MYON_Brep::EdgeCurveUseCount( int c3_index, int max_count ) const
{
  int ei, use_count = 0;
  if ( max_count < 1 )
    max_count = m_T.Count();
  for ( ei = 0; ei < m_E.Count() && use_count < max_count; ei++ )
  {
    if ( m_E[ei].m_c3i == c3_index )
      use_count++;
  }
  return use_count;
}

int MYON_Brep::SurfaceUseCount( int surface_index, int max_count ) const
{
  int fi, use_count = 0;
  if ( max_count < 1 )
    max_count = m_F.Count();
  for ( fi = 0; fi < m_F.Count() && use_count < max_count; fi++ )
  {
    if ( m_F[fi].m_si == surface_index )
      use_count++;
  }
  return use_count;
}

////////
// Change the domain of a face
// This also transforms the "u" and "v" coordinates of all the 
// face's parameter space trimming curves.
//bool MYON_Brep::SetFaceDomain(
//       int face_index, // index of face in m_F[] array
//       const MYON_Interval& u_dom,
//       const MYON_Interval& v_dom
//       )
//{
//  // OBSOLETE
//  bool rc = false;
//  MYON_BrepFace* face = Face(face_index);
//  if ( face )
//    rc = face->SetDomain(u_dom,v_dom);
//  return rc;
//}

//bool
//MYON_Brep::SwapFaceParameters( int face_index )
//{
//  // OBSOLETE
//  bool rc = false;
//  MYON_BrepFace* face = Face(face_index);
//  if ( face )
//    rc = face->Transpose()?true:false;
//  return rc;
//}

void
MYON_Brep::Flip()
{
  const int fcnt = m_F.Count();
  int fi;
  int missolid = m_is_solid;
  for ( fi = 0; fi < fcnt; fi++ ) {
    FlipFace(m_F[fi]);
  }
  if (missolid==1) m_is_solid = 2;
  else if (missolid==2) m_is_solid = 1;
}

//void
//MYON_Brep::FlipEdge( MYON_BrepEdge& edge )
//{
//  edge.Reverse();
//}

void
MYON_Brep::FlipFace( MYON_BrepFace& face )
{
  face.m_bRev = (face.m_bRev) ? false : true;

  std::array<std::shared_ptr<const MYON_Mesh>, 3> meshes{ face.UniqueMesh(MYON::analysis_mesh),  face.UniqueMesh(MYON::render_mesh),  face.UniqueMesh(MYON::preview_mesh) };
  
  for (auto& m : meshes)
  {
    auto pMesh = const_cast<MYON_Mesh*>(m.get());
    if (pMesh)
    {
      pMesh->Flip();
    }
  }

  //Jun 16 2011 - Chuck - m_is_solid==3 for a brep with inconsistent normals. 
  //Flipping a face could make the normals consistent.
  //if (m_is_solid == 1 || m_is_solid == 2)
  if (0 != m_is_solid)
    m_is_solid = 0;
}

//void 
//MYON_Brep::FlipTrim(MYON_BrepTrim& trim)
//{
//  trim.Reverse();
//}

void 
MYON_Brep::FlipLoop(MYON_BrepLoop& loop)
{
  int ti, lti;
  const int brep_trim_count = m_T.Count();
  const int loop_trim_count = loop.m_ti.Count();

  // reverse order of trimming curves
  loop.m_ti.Reverse();
  // reverse direction of individual trimming curves
  for ( lti = 0; lti < loop_trim_count; lti++ ) 
  {
    ti = loop.m_ti[lti];
    if ( ti >= 0 && ti < brep_trim_count ) 
    {
      m_T[ti].Reverse();
    }
  }
}

static int curve_area( MYON_3dPoint& start_point, const MYON_Curve* curve, const MYON_Interval& curve_domain, const MYON_Xform* xform, double *area )
{
  // ges a CRUDE approximation of curve area to use for
  // determining if a simple closed curve 2d has 
  // clockwise or couterclockwise orientation.
  MYON_Workspace ws;
  MYON_Interval span_domain;
  double *span_vector, *t, twice_area = 0.0;
  MYON_3dPoint p0, p1;
  int degree, span_count, span_i, j;
  if ( !area )
    return false;
  *area = 0;
  if ( !curve )
    return false;
  const MYON_PolyCurve* polycurve = MYON_PolyCurve::Cast(curve);
  if ( polycurve )
  {
    span_count = polycurve->Count();  
    MYON_Interval segment_curve_domain;
    double s0, s1;
    for ( span_i = 0; span_i < span_count; span_i++ )
    {
      span_domain = polycurve->SegmentDomain(span_i);
      if ( span_domain[1] <= curve_domain[0] )
        continue;
      if ( span_domain[0] >= curve_domain[1] )
        break;
      const MYON_Curve* segment_curve = polycurve->SegmentCurve(span_i);
      segment_curve_domain = segment_curve->Domain();
      if ( curve_domain[0] > span_domain[0] || curve_domain[1] < span_domain[1] )
      {
        s0 = (curve_domain[0] > span_domain[0]) ? curve_domain[0] : span_domain[0];
        s1 = (curve_domain[1] < span_domain[1]) ? curve_domain[1] : span_domain[1];
        if ( segment_curve_domain != span_domain )
        {
          s0 = span_domain.NormalizedParameterAt(s0);
          s1 = span_domain.NormalizedParameterAt(s1);
          s0 = segment_curve_domain.ParameterAt(s0);
          s1 = segment_curve_domain.ParameterAt(s1);
        }
        segment_curve_domain.Set(s0,s1);
      }
      if ( !curve_area( start_point, segment_curve, segment_curve_domain, xform, &twice_area ) )
      {
        *area = 0.0;
        return false;
      }
      *area += twice_area;
    }
  }
  else 
  {
    span_count = curve->SpanCount();
    if ( span_count < 1 )
      return false;
    degree = curve->Degree();
    if ( degree <= 1 )
    {
      degree = 1;
    }
    else if ( degree < 4)
    {
      degree = 4;
      // 6 January 2006 Dale Lear
      //     Every time you find a closed curve that
      //     gets the wrong dir, increase the number
      //     after the < by one until it works.  Add
      //     the curve to RR and list the RR number here.

      // 17 October 2012 Dale Lear
      //   To fix http://dev.mcneel.com/bugtrack/?q=113316
      //   I changed "< 16" to "< 17".
      while ( span_count*degree < 17 )
        degree *= 2;
    }

    span_vector = ws.GetDoubleMemory(span_count+1+degree);
    t = span_vector+(span_count+1);
    t[0] = 0.0;
    for ( j = 1; j < degree; j++ ) {
      t[j] = ((double)(j))/(degree);
    }
    if ( !curve->GetSpanVector( span_vector ) )
      return false;

    p1 = xform ? (*xform)*start_point : start_point;
    for ( span_i = 0; span_i < span_count; span_i++ ) {
      span_domain.Set( span_vector[span_i], span_vector[span_i+1] );
      if ( span_domain[1] <= curve_domain[0] )
        continue;
      if ( span_domain[0] >= curve_domain[1] )
        break;
      if ( span_domain[1] > curve_domain[1] )
        span_domain.m_t[1] = curve_domain[1];
      if ( span_domain[0] < curve_domain[0] )
        span_domain.m_t[0] = curve_domain[0];
      if ( span_domain[0] >= span_domain[1] )
        continue;
      for ( j = 0; j < degree; j++ ) {
        p0 = p1;
        p1 = curve->PointAt(span_domain.ParameterAt(t[j]));
        if ( xform )
          p1 = (*xform)*p1;
        twice_area += (p0.x-p1.x)*(p0.y+p1.y);
        if ( !span_i && !j ) {
          // check gap
        }
      }
    }
    p0 = p1;
    p1 = curve->PointAt(curve_domain[1]);
    start_point = p1;
    if ( xform )
      p1 = (*xform)*p1;
    twice_area += (p0.x-p1.x)*(p0.y+p1.y);
    *area = 0.5*twice_area;
  }  
  
  return true;
}

int MYON_ClosedCurveOrientation( const MYON_Curve& curve, const MYON_Xform* xform )
{
  int curve_orientation = 0;
  double area = 0.0;
  MYON_3dPoint start_point = curve.PointAtEnd();
  const MYON_Interval curve_domain = curve.Domain();
  if ( xform && xform->IsIdentity() )
    xform = 0;
  if (curve_area( start_point, &curve, curve_domain, xform, &area ))
  {
    double noise = 0.0;
    if ( area > noise )
      curve_orientation = 1;
    else if (area < noise )
      curve_orientation = -1;
  }
  return curve_orientation;
}


int MYON_ClosedCurveOrientation(const MYON_Curve& curve, const MYON_Plane& plane)
{
	MYON_Xform x;
	x.Rotation(plane, MYON_Plane::World_xy);
	return MYON_ClosedCurveOrientation(curve, &x);
}


double MYON_CurveOrientationArea( 
  const MYON_Curve* curve,
  const MYON_Interval* domain,
  const MYON_Xform* xform,
  bool bReverseCurve
  )
{
  if ( 0 == curve )
    return 0.0;

  MYON_Interval local_domain = curve->Domain();
  if ( 0 != domain && domain->IsIncreasing() )
    local_domain.Intersection(*domain);

  MYON_3dPoint start_point = curve->PointAt(local_domain[0]);
  double a = 0.0;
  if ( !curve_area( start_point, curve, local_domain, xform, &a ) ) 
    a = 0.0;
  else if ( bReverseCurve && 0.0 != a )
    a = -a;

  return a;
}


static int loop_type_compar(const MYON_BrepLoop *const* ppLoopA, const MYON_BrepLoop *const* ppLoopB )
{
  const MYON_BrepLoop* loopA = *ppLoopA;
  const MYON_BrepLoop* loopB = *ppLoopB;
  if ( loopA->m_type == loopB->m_type )
    return 0;
  if ( loopA->m_type == MYON_BrepLoop::unknown )
    return 1;
  if ( loopB->m_type == MYON_BrepLoop::unknown )
    return -1;
  if ( loopA->m_type < loopB->m_type )
    return -1;
  if ( loopA->m_type > loopB->m_type )
    return 1;
  return 0;
}

bool MYON_Brep::SortFaceLoops( MYON_BrepFace& face ) const
{
  int fli, li, loop_type;
  const int face_loop_count = face.m_li.Count();
  const int loop_count = m_L.Count();
  if ( face_loop_count < 1 || loop_count < 1 )
    return false;
  bool rc = true;
  MYON_SimpleArray<const MYON_BrepLoop*> loop_ptr(face_loop_count);
  for ( fli = 0; fli < face_loop_count; fli++ )
  {
    li = face.m_li[fli];
    if ( li < 0 || li >= loop_count )
      return false;
    const MYON_BrepLoop& loop = m_L[li];
    if ( loop.m_loop_index != li )
      return false;
    loop_type = loop.m_type;
    if ( loop_type <= MYON_BrepLoop::unknown || loop_type >= MYON_BrepLoop::type_count )
      rc = false;
    loop_ptr.Append( &m_L[li] );
  }
  loop_ptr.QuickSort( loop_type_compar );
  for ( fli = 0; fli < face_loop_count; fli++ )
  {
    face.m_li[fli] = loop_ptr[fli]->m_loop_index;
  }
  return rc;
}


int 
MYON_Brep::LoopDirection( const MYON_BrepLoop& loop ) const
{
  MYON_3dPoint start_point;
  double d, a = 0.0;
  int ti, lti, c2i;
  const int brep_trim_count = m_T.Count();
  const int brep_C2_count = m_C2.Count();
  const int loop_trim_count = loop.m_ti.Count();

  double noise = 0.0;

  // reverse direction of individual trimming curves
  for ( lti = 0; lti < loop_trim_count; lti++ ) {
    ti = loop.m_ti[lti];
    if ( ti < 0 || ti >= brep_trim_count ) {
      a = 0.0;
      break;
    }
    c2i =  m_T[ti].m_c2i;
    if ( c2i < 0 || c2i >= brep_C2_count ) {
      a = 0.0;
      break;
    }
    if ( lti == 0 ) 
    {
      // evaluate start of first trim
      if ( m_C2[c2i] )
        start_point = m_T[ti].PointAtStart(); //m_C2[c2i]->PointAt(m_T[ti].m_t[0]);
    }
    if ( !curve_area( start_point, &m_T[ti], m_T[ti].Domain(), 0, &d ) ) {
      a = 0.0;
      break;
    }
    //noise += fabs(d);
    a += d;
  }

  //this fixes trr 9351.  Change at your own risk.
  //noise *= 10.0*MYON_EPSILON;

  if (a > noise)
    return 1;
  else if (a < -noise)
    return -1;
  return 0;
}

bool MYON_Brep::SetVertexTolerances( bool bLazy )
{
  bool rc = true;
  int vi, vertex_count = m_V.Count();
  for ( vi = 0; vi < vertex_count; vi++ )
  {
    if ( !SetVertexTolerance( m_V[vi], bLazy ) )
      rc = false;
  }
  return rc;
}

bool
MYON_Brep::SetVertexTolerance( MYON_BrepVertex& vertex,
  bool bLazySet // default = false
                // false: recompute tolerance even if
                //        its current value is positive
                // true:  recompute tolerance only if
                //        its current value is nonpositive
  ) const
{
  if ( vertex.m_tolerance < 0.0 || !bLazySet ) {
    const int vertex_edge_count = vertex.EdgeCount();
    if ( vertex_edge_count < 1 ) {
      vertex.m_tolerance = 0.0;
    }
    else {
      vertex.m_tolerance = MYON_UNSET_VALUE;
      double tolerance = 0.0;
      double d;
      MYON_3dPoint uv;
      MYON_Interval edge_domain;
      //const MYON_Curve* c=0;
      const MYON_Surface* s=0;
      int vei, ei, eti, endi;
      const int vertex_index = vertex.m_vertex_index;
      for ( vei = 0; vei < vertex_edge_count; vei++ ) 
      {
        ei = vertex.m_ei[vei];
        if ( ei < 0 )
          return false;
        const MYON_BrepEdge& edge = m_E[ei];
        if ( !edge.ProxyCurve() )
          return false;
        edge_domain = edge.Domain();
        for ( endi = 0; endi < 2; endi++ )
        {
          if ( edge.m_vi[endi] == vertex_index ) 
          {
            d = vertex.point.DistanceTo( edge.PointAt(edge_domain[endi]) );
            if ( tolerance < d )
              tolerance = d;
          }
        }
        const int edge_trim_count = edge.m_ti.Count();
        for ( eti = 0; eti < edge_trim_count; eti++ ) 
        {
          const MYON_BrepTrim* trim = Trim(edge.m_ti[eti]);
          if ( 0 == trim )
            continue;
          if ( 0 == trim->TrimCurveOf() )
            continue;
          s = trim->SurfaceOf();
          if ( 0 == s )
            continue;
          for ( endi = 0; endi < 2; endi++ ) {
            if ( edge.m_vi[endi] == vertex_index ) {
              uv = trim->PointAt( trim->Domain()[trim->m_bRev3d?1-endi:endi] );
              d = vertex.point.DistanceTo( s->PointAt(uv.x,uv.y) );
              if ( tolerance < d )
                tolerance = d;
            }
          }
        }
      }
      vertex.m_tolerance = (tolerance <= MYON_ZERO_TOLERANCE) ? 0.0 : 1.001*tolerance;
    }
  }
  return (vertex.m_tolerance >= 0.0) ? true : false;
}

bool
MYON_Brep::SetTrimTolerance( MYON_BrepTrim& trim, bool bLazy ) const
{
  // The TL_Brep::SetTrimTolerance override of this virtual function
  // sets MYON_BrepTrim::m_tolerance[] correctly.
  double ds, de, d;
  int dir, lti, prev_ti, next_ti;
  if ( trim.m_tolerance[0] < 0.0 || trim.m_tolerance[1] < 0.0 || !bLazy )
  {
    // set trim tolerance
    if ( trim.m_li >= 0 && trim.m_li < m_L.Count() )
    {
      const MYON_BrepLoop& loop = m_L[trim.m_li];
      const int loop_trim_count = loop.m_ti.Count();
      for ( lti = 0; lti < loop_trim_count; lti++ )
      {
        if ( loop.m_ti[lti] == trim.m_trim_index )
        {
          prev_ti = loop.m_ti[(lti-1+loop_trim_count)%loop_trim_count];
          next_ti = loop.m_ti[(lti+1)%loop_trim_count];
          if ( prev_ti >= 0 && next_ti >= 0 && prev_ti < m_T.Count() && next_ti < m_T.Count() )
          {
            const MYON_BrepTrim& prev_trim = m_T[prev_ti];
            const MYON_BrepTrim& next_trim = m_T[next_ti];
            const MYON_Curve* prev_c2 = prev_trim.TrimCurveOf();
            const MYON_Curve* next_c2 = next_trim.TrimCurveOf();
            const MYON_Curve* c2 = trim.TrimCurveOf();
            if ( prev_c2 && c2 && next_c2 )
            {
              MYON_3dPoint prev_end = prev_trim.PointAtEnd(); //prev_c2->PointAt( prev_trim.m_t[1] );
              MYON_3dPoint this_start = trim.PointAtStart(); //c2->PointAt( trim.m_t[0] );
              MYON_3dPoint this_end = trim.PointAtEnd(); // c2->PointAt( trim.m_t[1] );
              MYON_3dPoint next_start = next_trim.PointAtStart(); //prev_c2->PointAt( next_trim.m_t[0] );
              for ( dir = 0; dir < 2; dir++ )
              {
                if ( trim.m_tolerance[dir] < 0.0 || !bLazy )
                {
                  ds = fabs(prev_end[dir] - this_start[dir] );
                  de = fabs(this_end[dir] - next_start[dir] );
                  d = (ds >= de) ? ds : de;
                  trim.m_tolerance[dir] = ( d > MYON_ZERO_TOLERANCE ) ? 1.001*d : 0.0;
                }
              }
            }
          }
          break;
        }
      }
    }
  }
  return (trim.m_tolerance[0] >= 0.0 && trim.m_tolerance[1] >= 0.0) ? true : false;
}

bool
MYON_Brep::SetEdgeTolerance( MYON_BrepEdge& edge, bool bLazySet ) const
{
  if ( edge.m_tolerance < 0.0 || !bLazySet ) 
  {
    const int edge_trim_count = edge.m_ti.Count();
    if ( edge_trim_count < 1 ) 
    {
      edge.m_tolerance = 0.0;
    }
    else 
    {
      edge.m_tolerance = MYON_UNSET_VALUE;
      // TL_Brep::SetEdgeTolerance overrides MYON_Brep::SetEdgeTolerance
      // and sets teh tolerance correctly.
    }
  }
  return (edge.m_tolerance >= 0.0) ? true : false;
}

bool MYON_Brep::SetTrimTolerances( bool bLazy )
{
  bool rc = true;
  int ti, trim_count = m_T.Count();
  for ( ti = 0; ti < trim_count; ti++ )
  {
    if ( !SetTrimTolerance( m_T[ti], bLazy ) )
      rc = false;
  }
  return rc;
}

bool MYON_Brep::SetEdgeTolerances( bool bLazy )
{
  bool rc = true;
  int ei, edge_count = m_E.Count();
  for ( ei = 0; ei < edge_count; ei++ )
  {
    if ( !SetEdgeTolerance( m_E[ei], bLazy ) )
      rc = false;
  }
  return rc;
}


void MYON_Brep::Dump( MYON_TextLog& dump ) const
{
  dump.Print("MYON_Brep:\n");

  if ( IsSurface() ) {
    dump.Print("(B-rep geometry is the same as underlying surface.)\n");
  }

  dump.Print("surfaces:   %d\n",m_S.Count());
  dump.Print("3d curve:   %d\n",m_C3.Count());
  dump.Print("2d curves:  %d\n",m_C2.Count());
  dump.Print("vertices:   %d\n",m_V.Count());
  dump.Print("edges:      %d\n",m_E.Count());
  dump.Print("trims:      %d\n",m_T.Count());
  dump.Print("loops:      %d\n",m_L.Count());
  dump.Print("faces:      %d\n",m_F.Count());
  if (nullptr != m_region_topology)
  {
    dump.Print("regions:    %d\n", m_region_topology->m_R.Count());
    dump.Print("face sides: %d\n", m_region_topology->m_FS.Count());
  }

  int c2i;
  for ( c2i = 0; c2i < m_C2.Count(); c2i++ )
  {
    const MYON_Curve* c2 = m_C2[c2i];
    if ( c2 )
    {
      MYON_Interval cdom = c2->Domain();
      MYON_3dPoint c_start, c_end;
      c_start = c2->PointAtStart();
      c_end = c2->PointAtEnd();
      const char* s = c2->ClassId()->ClassName();
      if ( !s )
        s = "";
      dump.Print("curve2d[%2d]: %s domain(%g,%g) start(%g,%g) end(%g,%g)\n",
                  c2i, s, cdom[0], cdom[1],
                  c_start.x, c_start.y,
                  c_end.x, c_end.y);
    }
    else
    {
      dump.Print("curve2d[%2d]: nullptr\n",c2i);
    }
  }

  int c3i;
  for ( c3i = 0; c3i < m_C3.Count(); c3i++ )
  {
    const MYON_Curve* c3 = m_C3[c3i];
    if ( c3 )
    {
      MYON_Interval cdom = c3->Domain();
      MYON_3dPoint c_start, c_end;
      c_start = c3->PointAtStart();
      c_end = c3->PointAtEnd();
      const char* s = c3->ClassId()->ClassName();
      if ( !s )
        s = "";
      dump.Print("curve3d[%2d]: %s domain(%g,%g) start(%g,%g,%g) end(%g,%g,%g)\n",
                 c3i, s, cdom[0], cdom[1],
                 c_start.x, c_start.y, c_start.z,
                 c_end.x, c_end.y, c_end.z);
    }
    else
    {
      dump.Print("curve2d[%2d]: nullptr\n",c3i);
    }
  }

  int si;
  for ( si = 0; si < m_S.Count(); si++ )
  {
    const MYON_Surface* srf = m_S[si];
    if ( srf )
    {
      MYON_Interval udom = srf->Domain(0);
      MYON_Interval vdom = srf->Domain(1);
      const char* s = srf->ClassId()->ClassName();
      if ( !s )
        s = "";
      dump.Print("surface[%2d]: %s u(%g,%g) v(%g,%g)\n",
                 si, s, 
                 udom[0], udom[1], 
                 vdom[0], vdom[1]
                 );
      if ( m_S.Count() == 1 && IsSurface() )
      {
        dump.PushIndent();
        dump.Print("surface details:\n");
        dump.PushIndent();
        srf->Dump(dump);
        dump.PopIndent();
        dump.PopIndent();
      }
    }
    else
    {
      dump.Print("surface[%2d]: nullptr\n",si);
    }
  }

  int vi;
  for ( vi = 0; vi < m_V.Count(); vi++ ) {
    const MYON_BrepVertex& vertex = m_V[vi];
    dump.Print("vertex[%2d]: (%f %f %f) tolerance(%g)\n",
               vi,vertex.point.x,vertex.point.y,vertex.point.z,
               vertex.m_tolerance);
    if ( vertex.m_ei.Count() > 0 ) {
      int vei;
      dump.PushIndent();
      dump.Print("edges (");
      for ( vei = 0; vei < vertex.m_ei.Count(); vei++ ) {
        dump.Print( (vei)?",%d":"%d", vertex.m_ei[vei] );
      }
      dump.PopIndent();
      dump.Print(")\n");
    }
  }

  int ei,ti;
  for ( ei = 0; ei < m_E.Count(); ei++ ) {
    const MYON_BrepEdge& edge = m_E[ei];
    dump.Print("edge[%2d]: v0(%2d) v1(%2d) 3d_curve(%d) tolerance(%g)\n",
               ei,edge.m_vi[0],edge.m_vi[1],edge.m_c3i,edge.m_tolerance);

    dump.PushIndent();

    const MYON_Curve* c3 = edge.EdgeCurveOf();
    if ( c3 )
    {
      MYON_3dPoint edge_start = edge.PointAtStart();
      MYON_3dPoint edge_end = edge.PointAtEnd();
      dump.Print("domain(%g,%g) start(%g,%g,%g) end(%g,%g,%g)\n",
           edge.Domain()[0],edge.Domain()[1],
           edge_start.x,edge_start.y,edge_start.z,
           edge_end.x,edge_end.y,edge_end.z
           );
    }
    else
    {
      dump.Print("domain(%g,%g) start(?,?,?) end(?,?,?)\n",
           edge.Domain()[0],edge.Domain()[1]);
    }

    if ( edge.m_ti.Count() > 0 ) 
    {

      dump.Print("trims (",edge.m_ti.Count());
      int eti;
      for ( eti = 0; eti < edge.m_ti.Count(); eti++ )
      {
        ti = edge.m_ti[eti];
        const char* sign = "?";
        if ( ti >= 0 && ti < m_T.Count() ) {
          sign = m_T[ti].m_bRev3d ? "-" : "+";
        }
        dump.Print( (eti)?",%s%d":"%s%d", sign,edge.m_ti[eti]);
      }

      dump.Print(")\n");
    }

    dump.PopIndent();
  }

  int fi;
  for ( fi = 0; fi < m_F.Count(); fi++ ) {
    const MYON_BrepFace& face = m_F[fi];
    const MYON_Surface* face_srf = face.SurfaceOf();
    dump.Print("face[%2d]: surface(%d) reverse(%d) loops(",
               fi,face.m_si,face.m_bRev);
    int fli;
    for ( fli = 0; fli < face.m_li.Count(); fli++ ) {
      dump.Print( (fli)?",%d":"%d", face.m_li[fli]);
    }
    dump.Print(")\n");
    dump.PushIndent();

    if (auto sp = face.SharedMesh(MYON::render_mesh))
    {
      const MYON_MeshParameters* mp = sp->MeshParameters();
      MYON_wString mp_description = (nullptr != mp) ? mp->Description() : MYON_wString(L"Unknown");
      dump.Print(L"Render mesh = %ls. %d polygons\n", static_cast<const wchar_t*>(mp_description), sp->FaceCount());
    }
    else
    {
      dump.Print("Render mesh = nullptr\n");
    }

    if (auto sp = face.SharedMesh(MYON::analysis_mesh))
    {
      const MYON_MeshParameters* mp = sp->MeshParameters();
      MYON_wString mp_description = (nullptr != mp) ? mp->Description() : MYON_wString(L"Unknown");
      dump.Print(L"Analysis mesh = %ls. %d polygons\n", static_cast<const wchar_t*>(mp_description), sp->FaceCount());
    }
    else
    {
      dump.Print("Analysis mesh = nullptr\n");
    }

    if (auto sp = face.SharedMesh(MYON::preview_mesh))
    {
      const MYON_MeshParameters* mp = sp->MeshParameters();
      MYON_wString mp_description = (nullptr != mp) ? mp->Description() : MYON_wString(L"Unknown");
      dump.Print(L"Preview mesh = %ls. %d polygons\n", static_cast<const wchar_t*>(mp_description), sp->FaceCount());
    }
    else
      dump.Print("Preview mesh = nullptr\n");

    if ( FaceIsSurface(fi) ) {
      dump.Print("(Face geometry is the same as underlying surface.)\n");
    }


    for ( fli = 0; fli < face.m_li.Count(); fli++ ) 
    {
      const int li = face.m_li[fli];
      const MYON_BrepLoop& loop = m_L[li];
      const char* sLoopType = 0;
      switch( loop.m_type ) 
      {
      case MYON_BrepLoop::unknown:
        sLoopType = "unknown";
        break;
      case MYON_BrepLoop::outer:
        sLoopType = "outer";
        break;
      case MYON_BrepLoop::inner:
        sLoopType = "inner";
        break;
      case MYON_BrepLoop::slit:
        sLoopType = "slit";
        break;
      case MYON_BrepLoop::crvonsrf:
        sLoopType = "crvonsrf";
        break;
      default:
        sLoopType = "unknown";
        break;
      }
      dump.Print("loop[%2d]: type(%s) %d trims(",
                 li, sLoopType, loop.m_ti.Count());
      int lti;
      for ( lti = 0; lti < loop.m_ti.Count(); lti++ ) 
      {
        dump.Print( (lti)?",%d":"%d", loop.m_ti[lti]);
      }
      dump.Print(")\n");
      dump.PushIndent();
      for ( lti = 0; lti < loop.m_ti.Count(); lti++ ) 
      {
        const int ti_for_loop = loop.m_ti[lti];
        const MYON_BrepTrim& trim = m_T[ti_for_loop];
        const char* sTrimType = "?";
        const char* sTrimIso = "-?";
        const MYON_Curve* c2 = trim.TrimCurveOf();
        MYON_3dPoint trim_start, trim_end;
        switch( trim.m_type ) {
        case MYON_BrepTrim::unknown:
          sTrimType = "unknown ";
          break;
        case MYON_BrepTrim::boundary:
          sTrimType = "boundary";
          break;
        case MYON_BrepTrim::mated:
          sTrimType = "mated   ";
          break;
        case MYON_BrepTrim::seam:
          sTrimType = "seam    ";
          break;
        case MYON_BrepTrim::singular:
          sTrimType = "singular";
          break;
        case MYON_BrepTrim::crvonsrf:
          sTrimType = "crvonsrf";
          break;
        default:
          sTrimType = "unknown";
          break;
        }
        switch( trim.m_iso ) {
        case MYON_Surface::not_iso:
          sTrimIso = "";
          break;
        case MYON_Surface::x_iso:
          sTrimIso = "-u iso";
          break;
        case MYON_Surface::W_iso:
          sTrimIso = "-west side iso";
          break;
        case MYON_Surface::E_iso:
          sTrimIso = "-east side iso";
          break;
        case MYON_Surface::y_iso:
          sTrimIso = "-v iso";
          break;
        case MYON_Surface::S_iso:
          sTrimIso = "-south side iso";
          break;
        case MYON_Surface::N_iso:
          sTrimIso = "-north side iso";
          break;
        default:
          sTrimIso = "-unknown_iso_flag";
          break;
        }
        dump.Print("trim[%2d]: edge(%2d) v0(%2d) v1(%2d) tolerance(%g,%g)\n",
                   ti_for_loop,
                   trim.m_ei,trim.m_vi[0],trim.m_vi[1],
                   trim.m_tolerance[0],trim.m_tolerance[1]);
        dump.PushIndent();
        dump.Print("type(%s%s) rev3d(%d) 2d_curve(%d)\n",
                   sTrimType, sTrimIso, trim.m_bRev3d, trim.m_c2i);
        if ( c2 )
        {
          trim_start = trim.PointAtStart();
          trim_end = trim.PointAtEnd();
          dump.Print("domain(%g,%g) start(%g,%g) end(%g,%g)\n",
               trim.Domain()[0],trim.Domain()[1],
               trim_start.x,trim_start.y,
               trim_end.x,trim_end.y);
          if ( 0 != face_srf )
          {
            MYON_3dPoint trim_srfstart = face_srf->PointAt(trim_start.x,trim_start.y);
            MYON_3dPoint trim_srfend = face_srf->PointAt(trim_end.x,trim_end.y);
            dump.Print("surface points start(%g,%g,%g) end(%g,%g,%g)\n",
                 trim_srfstart.x,trim_srfstart.y,trim_srfstart.z,
                 trim_srfend.x,trim_srfend.y,trim_srfend.z);
          }
        }
        else
        {
          dump.Print("domain(%g,%g) start(?,?) end(?,?)\n",
               trim.Domain()[0],trim.Domain()[1]);
        }
        dump.PopIndent();
      }
      dump.PopIndent();
    }
    dump.PopIndent();
  }

  //int si;
  //for ( si = 0; si < m_S.Count(); si++ ) {
  //  dump.Print("surface[%d]:\n",si);
  //  dump.PushIndent();
  //  if ( m_S[si] )
  //    m_S[si]->Dump(dump);
  //  else
  //    dump.Print("nullptr\n");
  //  dump.PopIndent();
  //}

}



//int MYON_Brep::FaceIndexOf( const MYON_BrepTrim& trim ) const
//{
//  int fi = -1;
//  if ( trim.m_li >= 0 && trim.m_li < m_L.Count() )
//  {
//    fi = m_L[trim.m_li].m_fi;
//    if ( fi < 0 || fi >= m_F.Count() )
//      fi = -1;
//  }
//  return fi;
//}

//int MYON_Brep::FaceIndexOf( const MYON_BrepLoop& loop ) const
//{
//  int fi = -1;
//  if ( loop.m_fi >= 0 && loop.m_fi < m_F.Count() )
//    fi = loop.m_fi;
//  return fi;
//}

//const MYON_BrepFace* MYON_Brep::FaceOf( const MYON_BrepTrim& trim ) const
//{
//  // OBSOLETE
//  return trim.Face();
//}

//const MYON_BrepFace* MYON_Brep::FaceOf( const MYON_BrepLoop& loop ) const
//{
//  // OBSOLETE
//  return loop.Face();
//}


//int MYON_Brep::SurfaceIndexOf( const MYON_BrepTrim& trim ) const
//{
//  // OBSOLETE
//  return trim.SurfaceIndexOf();
//}

//int MYON_Brep::SurfaceIndexOf( const MYON_BrepLoop& loop ) const
//{
//  // OBSOLETE
//  return loop.SurfaceIndexOf();
//}

//int MYON_Brep::SurfaceIndexOf( const MYON_BrepFace& face ) const
//{
//  // OBSOLETE FUNCTION
//  return face.m_si;
//}

//MYON_Surface* MYON_Brep::SurfaceOf( const MYON_BrepTrim& trim ) const
//{
//  // OBSOLETE FUNCTION
//  const int si = trim.SurfaceIndexOf();
//  return (si>=0)?m_S[si]:0;
//}

//MYON_Surface* MYON_Brep::SurfaceOf( const MYON_BrepLoop& loop ) const
//{
//  // OBSOLETE FUNCTION
//  return const_cast<MYON_Surface*>(loop.SurfaceOf());
//}

//MYON_Surface* MYON_Brep::SurfaceOf( const MYON_BrepFace& face ) const
//{
//  // OBSOLETE FUNCTION
//  return const_cast<MYON_Surface*>(face.SurfaceOf());
//}

//int MYON_Brep::EdgeCurveIndexOf( const MYON_BrepTrim& trim ) const
//{
//  // OBSOLETE FUNCTION
//  return trim.EdgeCurveIndexOf();
//}

//int MYON_Brep::EdgeCurveIndexOf( const MYON_BrepEdge& edge ) const
//{
//  // OBSOLETE FUNCTION
//  return edge.m_c3i;
//}

//MYON_Curve* MYON_Brep::EdgeCurveOf( const MYON_BrepTrim& trim ) const
//{
//  // OBSOLETE FUNCTION
//  return const_cast<MYON_Curve*>(trim.EdgeCurveOf());
//}

//MYON_Curve* MYON_Brep::EdgeCurveOf( const MYON_BrepEdge& edge ) const
//{
//  return ((edge.m_c3i>=0&&edge.m_c3i<m_C3.Count())?m_C3[edge.m_c3i]:0);
//}

//int MYON_Brep::TrimCurveIndexOf( const MYON_BrepTrim& trim ) const
//{
//  return trim.m_c2i;
//}

//MYON_Curve* MYON_Brep::TrimCurveOf( const MYON_BrepTrim& trim ) const
//{
//  return ((trim.m_c2i>=0&&trim.m_c2i<m_C2.Count())?m_C2[trim.m_c2i]:0);
//}

void MYON_Brep::DeleteVertex(MYON_BrepVertex& vertex)
{
  const int vi = vertex.m_vertex_index;
  vertex.m_vertex_index = -1;
  if ( vi >= 0 && vi < m_V.Count() ) {
    int vei, ei;
    for ( vei = vertex.m_ei.Count()-1; vei>=0; vei-- ) {
      ei = vertex.m_ei[vei];
      if ( ei >= 0 && ei < m_E.Count() ) {
        MYON_BrepEdge& edge = m_E[ei];
        if ( edge.m_vi[0] == vi )
          edge.m_vi[0] = -1;
        if ( edge.m_vi[1] == vi )
          edge.m_vi[1] = -1;
        DeleteEdge( edge, false );
      }
    }
  }
  vertex.m_ei.Empty();
  vertex.m_tolerance = MYON_UNSET_VALUE;
}

int MYON_Brep::Loop3dCurve( 
  const MYON_BrepLoop& loop,
  MYON_SimpleArray<MYON_Curve*>& curve_list,
  bool bRevCurveIfFaceRevIsTrue
  ) const
{
  int curve_list_count0 = curve_list.Count();
  MYON_PolyCurve* poly_curve = nullptr;
  MYON_Curve* loop_curve = nullptr;
  MYON_SimpleArray<int> trim_index( 2*loop.m_ti.Count() + 8);
  int i, lti, ti;
  int loop_trim_count = loop.m_ti.Count();
  if ( loop_trim_count < 1 )
    return 0;


  int seam_lti = -1; // index of first seam
  int wire_lti = -1;
  for ( lti = 0; lti < loop_trim_count; lti++ )
  {
    ti = loop.m_ti[lti];
    if ( ti >= 0 && ti < m_T.Count() )
    {
      const MYON_BrepTrim& trim = m_T[ti];
      if ( seam_lti < 0 && trim.m_type == MYON_BrepTrim::seam )
        seam_lti = lti;
      else if ( wire_lti < 0 && trim.m_type != MYON_BrepTrim::singular )
        wire_lti = lti;
    }
  }

  if ( wire_lti < 0 )
    return 0; // sphere boundary, torus boundary, etc.

  if ( seam_lti < 0 )
  {
    // simple case;
    loop_curve = Loop3dCurve(loop,bRevCurveIfFaceRevIsTrue);
    if ( loop_curve )
      curve_list.Append(loop_curve);
    return curve_list.Count() - curve_list_count0;
  }

  bool bOnSeam = true;
  for ( lti = seam_lti; lti < seam_lti+loop_trim_count; lti++ )
  {
    ti = loop.m_ti[lti%loop_trim_count];

    if ( ti < 0 || ti >= m_T.Count() )
      ti = loop.m_ti[seam_lti]; // treat bogus indices as trims

    const MYON_BrepTrim& trim = m_T[ti];
    if ( trim.m_type == MYON_BrepTrim::seam )
    {
      if (!bOnSeam)
      {
        trim_index.Append(-1);
        bOnSeam = true;
      }
      continue;
    }
    // 10-1-03 Lowell - fixed typo
    if ( trim.m_type == MYON_BrepTrim::singular )
      continue;
    bOnSeam = false;
    trim_index.Append(ti);
  }

  for ( i = 0; i < trim_index.Count(); i++ )
  {
    ti = trim_index[i];
    if ( ti < 0 )
    {
      if ( loop_curve )
        curve_list.Append(loop_curve);
      loop_curve = 0;
      poly_curve = 0;
      continue;
    }

    // get 3d curve associated with this trim's edge
    const MYON_BrepTrim& trim = m_T[ti];
    const MYON_BrepEdge& edge = m_E[trim.m_ei];
    MYON_Curve* segment_curve = edge.DuplicateCurve();
    if ( !segment_curve )
      continue;
    if ( trim.m_bRev3d )
      segment_curve->Reverse();

    if ( !loop_curve )
      loop_curve = segment_curve;
    else if ( !poly_curve )
    {
      poly_curve = new MYON_PolyCurve();
      poly_curve->Append(loop_curve);
      poly_curve->Append(segment_curve);
      loop_curve = poly_curve;
    }
    else
      poly_curve->Append( segment_curve );
  }

  // 10-1-03 Lowell - add the last non-seam segment
  if ( loop_curve )
    curve_list.Append(loop_curve);

  if ( bRevCurveIfFaceRevIsTrue )
  {
    int fi = loop.m_fi;
    if ( fi >= 0 && fi < m_F.Count() && m_F[fi].m_bRev )
    {
      for ( i = curve_list_count0; i < curve_list.Count(); i++ )
        curve_list[i]->Reverse();
    }
  }

  return curve_list.Count() - curve_list_count0;
}


MYON_Curve* MYON_Brep::Loop3dCurve( const MYON_BrepLoop& loop, bool bRevCurveIfFaceRevIsTrue ) const
{
  MYON_PolyCurve* poly_curve = nullptr;
  MYON_Curve* loop_curve = nullptr;
  MYON_SimpleArray<int> trim_index( loop.m_ti.Count() );
  int i, lti, ti;
  for ( lti = 0; lti < loop.m_ti.Count(); lti++ )
  {
    ti = loop.m_ti[lti];
    if ( ti >= 0 && ti < m_T.Count() )
    {
      const MYON_BrepTrim& trim = m_T[ti];
      if ( trim.EdgeCurveOf() )
        trim_index.Append(ti);
    }
  }

  for ( i = 0; i < trim_index.Count(); i++ )
  {
    // get 3d curve associated with this trim's edge
    const MYON_BrepTrim& trim = m_T[trim_index[i]];
    const MYON_BrepEdge& edge = m_E[trim.m_ei];
    MYON_Curve* segment_curve = edge.DuplicateCurve();
    if ( !segment_curve )
      continue;
    if ( trim.m_bRev3d )
      segment_curve->Reverse();

    if ( !loop_curve )
      loop_curve = segment_curve;
    else if ( !poly_curve )
    {
      poly_curve = new MYON_PolyCurve();
      poly_curve->Append(loop_curve);
      poly_curve->Append(segment_curve);
      loop_curve = poly_curve;
    }
    else
      poly_curve->Append( segment_curve );
  }
  if ( loop_curve && bRevCurveIfFaceRevIsTrue )
  {
    int fi = loop.m_fi;
    if ( fi >= 0 && fi < m_F.Count() && m_F[fi].m_bRev )
    {
      loop_curve->Reverse();
    }
  }
  return loop_curve;
}

MYON_Curve* MYON_Brep::Loop2dCurve( const MYON_BrepLoop& loop ) const
{
  MYON_PolyCurve* poly_curve = nullptr;
  MYON_Curve* loop_curve = nullptr;
  MYON_SimpleArray<int> trim_index( loop.m_ti.Count() );
  int i, lti, ti;
  for ( lti = 0; lti < loop.m_ti.Count(); lti++ )
  {
    ti = loop.m_ti[lti];
    if ( ti >= 0 && ti < m_T.Count() )
    {
      const MYON_BrepTrim& trim = m_T[ti];
      if ( trim.TrimCurveOf() )
        trim_index.Append(ti);
    }
  }
  for ( i = 0; i < trim_index.Count(); i++ )
  {
    // get 2d curve associated with this trim's edge
    const MYON_BrepTrim& trim = m_T[trim_index[i]];
    MYON_Curve* segment_curve = trim.DuplicateCurve();
    if ( !segment_curve )
      continue;

    if ( !loop_curve )
      loop_curve = segment_curve;
    else if ( !poly_curve )
    {
      poly_curve = new MYON_PolyCurve();
      poly_curve->Append(loop_curve);
      poly_curve->Append(segment_curve);
      loop_curve = poly_curve;
    }
    else
      poly_curve->Append( segment_curve );
  }
  return loop_curve;
}



void MYON_Brep::DeleteEdge(MYON_BrepEdge& edge, bool bDeleteEdgeVertices )
{
  const int ei = edge.m_edge_index;
  edge.m_edge_index = -1;

  if ( ei >= 0 && ei < m_E.Count() ) {
    int eti, ti, evi, vei, vi;
    for ( eti = edge.m_ti.Count()-1; eti >= 0; eti-- ) {
      ti = edge.m_ti[eti];
      if ( ti >= 0 && ti < m_T.Count() ) {
        MYON_BrepTrim& trim = m_T[ti];
        trim.m_ei = -1;
        if ( trim.m_li >= 0 && trim.m_li < m_L.Count() ) {
          MYON_BrepLoop& loop = m_L[trim.m_li];
          if ( loop.m_fi >= 0 && loop.m_fi < m_F.Count() ) {
            DeleteFace( m_F[loop.m_fi], bDeleteEdgeVertices );
          }
        }
        DeleteTrim(trim,false);
      }
    }

    for (evi = 0; evi < 2; evi++ ) 
    {
      vi = edge.m_vi[evi];
      if ( vi >= 0 && vi < m_V.Count() ) 
      {
        MYON_BrepVertex& v = m_V[vi];
        for ( vei = v.m_ei.Count()-1; vei >= 0; vei-- ) 
        {
          if ( v.m_ei[vei] == ei )
            v.m_ei.Remove(vei);
        }
        if ( bDeleteEdgeVertices && v.m_ei.Count() <= 0 )
        {
          v.m_ei.Destroy();
          DeleteVertex(v);
        }
      }
    }
  }

  edge.m_c3i = -1;
  edge.m_vi[0] = -1;
  edge.m_vi[1] = -1;
  edge.m_ti.Empty();
  edge.m_tolerance = MYON_UNSET_VALUE;
  edge.m_brep = 0;
  edge.SetProxyCurve(0);
}

void MYON_Brep::DeleteTrim(MYON_BrepTrim& trim, bool bDeleteTrimEdges )
{
  m_is_solid = 0;

  const int ti = trim.m_trim_index;
  trim.m_trim_index = -1;

  if ( ti >= 0 && ti < m_T.Count() ) 
  {
    const int ei = trim.m_ei;
    if ( ei >= 0 && ei < m_E.Count() ) 
    {
      MYON_BrepEdge& edge = m_E[ei];
      if ( bDeleteTrimEdges && edge.m_ti.Count() == 1 && edge.m_ti[0] == ti ) 
      {
        edge.m_ti.Empty();
        DeleteEdge(edge,bDeleteTrimEdges);
      }
      else 
      {
        int mate_ti = (trim.m_type == MYON_BrepTrim::mated) ? -1 : -2; // set to >= 0 if a single mate exists
        int seam_ti = (trim.m_type == MYON_BrepTrim::seam) ? -1 : -2; // set to >= 0 if a single seam partner exists
        int eti;
        for ( eti = edge.m_ti.Count()-1; eti >= 0; eti-- ) 
        {
          int other_ti = edge.m_ti[eti];
          if ( other_ti == ti ) 
          {
            edge.m_ti.Remove(eti);
            if ( edge.m_ti.Count() == 0 )
              edge.m_tolerance = 0.0;
            continue;
          }
          
          if ( (mate_ti >= -1 || seam_ti >= -1 ) && other_ti >= 0 && other_ti < m_T.Count() )
          {
            const MYON_BrepTrim& other_trim = m_T[other_ti];
            if ( other_trim.m_trim_index != other_ti )
              continue;
            if ( mate_ti >= -1 
                 && (other_trim.m_type == MYON_BrepTrim::mated || other_trim.m_type == MYON_BrepTrim::slit || (other_trim.m_type == MYON_BrepTrim::seam && other_trim.m_li != trim.m_li) )
                 )
            {
              // see if other_trim is the only mate of trim
              if ( mate_ti == -1 )
                mate_ti = other_ti;
              else
                mate_ti = -2;
            }
            else if ( seam_ti >= -1 && other_trim.m_type == MYON_BrepTrim::seam && other_trim.m_li == trim.m_li )
            {
              // trim and other_trim are both seam trims in the same loop connected
              // to the same edge.
              if ( seam_ti == -1 )
                seam_ti = other_ti;
              else
                seam_ti = -2;
            }
          }
        }

        if ( seam_ti >= 0  )
        {
          // m_T[seam_ti] used to be a seam partner with trim.
          // Now it is either a boundary trim or is mated to m_T[mate_ti]
          m_T[seam_ti].m_type = (mate_ti>=0) 
                              ? MYON_BrepTrim::mated     // m_T[mate_ti] is mated to m_T[seam_ti]
                              : MYON_BrepTrim::boundary; // m_T[seam_ti] is all alone
        }
        else if ( mate_ti >= 0 )
        {
          // m_T[mate_ti] just lost its only mate and is now a boundary
          m_T[mate_ti].m_type = MYON_BrepTrim::boundary;
        }
      }
    }

    const int li = trim.m_li;
    if ( li >= 0 && li < m_L.Count() ) {
      MYON_BrepLoop& loop = m_L[li];
      int lti;
      for ( lti = loop.m_ti.Count()-1; lti >= 0; lti-- ) {
        if ( loop.m_ti[lti] == ti )
          loop.m_ti.Remove(lti);
      }
    }
  }

  trim.m_c2i = -1;
  trim.m_ei = -1;
  trim.m_vi[0] = -1;
  trim.m_vi[1] = -1;
  trim.m_bRev3d = 0;
  trim.m_type = MYON_BrepTrim::unknown;
  trim.m_iso = MYON_Surface::not_iso;
  trim.m_li = -1;
  trim.m_tolerance[0] = MYON_UNSET_VALUE;
  trim.m_tolerance[1] = MYON_UNSET_VALUE;
  trim.m__legacy_2d_tol = MYON_UNSET_VALUE;
  trim.m__legacy_3d_tol = MYON_UNSET_VALUE;
  trim.m__legacy_flags = 0;
  trim.m_pbox.Destroy();
  trim.m_brep = 0;
  trim.SetProxyCurve(0);
}

void MYON_Brep::DeleteLoop(MYON_BrepLoop& loop,  bool bDeleteLoopEdges  )
{
  // 2 Sept 2020 S. Baer (RH-59952)
  // Destroy cached bounding box on breps when messing around with loops
  m_bbox.Destroy();
  m_is_solid = 0;

  const int li = loop.m_loop_index;
  loop.m_loop_index = -1;

  if ( loop.m_fi >= 0 )
    DestroyMesh(MYON::any_mesh);

  if ( li >= 0 && li < m_L.Count() ) 
  {
    const int tcount = m_T.Count();
    int lti, ti;
    for ( lti = loop.m_ti.Count()-1; lti >= 0; lti-- )
    {
      ti = loop.m_ti[lti];
      if ( ti >= 0 && ti < tcount )
      {
        MYON_BrepTrim& trim = m_T[ti];
        trim.m_li = -1;
        DeleteTrim(trim,bDeleteLoopEdges);
      }
    }

    const int fi = loop.m_fi;
    if ( fi >= 0 && fi < m_F.Count() ) 
    {
      MYON_BrepFace& face = m_F[fi];
      int fli;
      for ( fli = face.m_li.Count()-1; fli >= 0; fli-- )
      {
        if ( face.m_li[fli] == li ) 
        {
          face.m_li.Remove(fli);
          // 2 Sept 2020 S. Baer (RH-59952)
          // clear cached face bbox when a loop is removed
          face.m_bbox.Destroy();
        }
      }
    }
  }

  loop.m_type = MYON_BrepLoop::unknown;
  loop.m_ti.Empty();
  loop.m_fi = -1;
  loop.m_pbox.Destroy();
  loop.m_brep = 0;
}

void MYON_Brep::DeleteFace(MYON_BrepFace& face, bool bDeleteFaceEdges )
{
  m_bbox.Destroy();
  m_is_solid = 0;

  const int fi = face.m_face_index;
  face.m_face_index = -1;

  if ( fi >= 0 && fi < m_F.Count() ) {
    const int lcount = m_L.Count();
    int fli, li;
    for ( fli = face.m_li.Count()-1; fli >= 0; fli-- ) {
      li = face.m_li[fli];
      if ( li >= 0 && li < lcount ) {
        MYON_BrepLoop& loop = m_L[li];
        loop.m_fi = -1;
        DeleteLoop(loop,bDeleteFaceEdges);
      }
    }
    DestroyRegionTopology();
  }

  face.m_si = -1;
  face.m_li.Empty();
  face.SetProxySurface(0);
  face.m_brep = 0;
  face.m_bbox.Destroy();
}

static void PropagateLabel(const MYON_Brep& B, 
  const MYON_SimpleArray<int>& fids,
  int label, 
  MYON_SimpleArray<int>& new_fids
)

//on input, each face in fids must have m_face_user.i = label
{
  if (fids.Count() == 0) 
    return;
  new_fids.SetCount(0);
  new_fids.Reserve(B.m_F.Count());
  for (int face_i=0; face_i<fids.Count(); face_i++)
  {
    const MYON_BrepFace& F = B.m_F[fids[face_i]];
    for (int loop_i=0; loop_i<F.m_li.Count(); loop_i++)
    {
      const MYON_BrepLoop& L = B.m_L[F.m_li[loop_i]];
      memset(&L.m_loop_user,0,sizeof(L.m_loop_user));
      L.m_loop_user.i = label;
      for (int edge_i=0; edge_i<L.m_ti.Count(); edge_i++)
      {
        const MYON_BrepTrim& T = B.m_T[L.m_ti[edge_i]];
        memset(&T.m_trim_user,0,sizeof(T.m_trim_user));
        T.m_trim_user.i = label;
        if (T.m_ei < 0) 
          continue;
        const MYON_BrepEdge& E = B.m_E[T.m_ei];
        memset(&E.m_edge_user,0,sizeof(E.m_edge_user));
        E.m_edge_user.i = label;
        for (int vertex_i=0; vertex_i<2; vertex_i++)
        {
          if (E.m_vi[vertex_i] >= 0) 
          {
            const MYON_BrepVertex& V = B.m_V[E.m_vi[vertex_i]];
            memset(&V.m_vertex_user,0,sizeof(V.m_vertex_user));
            V.m_vertex_user.i = label;
          }
        }
        for (int trim_i=0; trim_i<E.m_ti.Count(); trim_i++)
        {
          int fi = B.m_T[E.m_ti[trim_i]].FaceIndexOf();
          if (fi < 0 || B.m_F[fi].m_face_user.i == label) 
            continue;
          const MYON_BrepFace& F_local = B.m_F[fi];
          memset(&F_local.m_face_user,0,sizeof(F_local.m_face_user));
          F_local.m_face_user.i = label;
          new_fids.Append(fi);
        }
      }
    }
  }
  return;
}

static void PropagateLabel(const MYON_Brep& B, 
  MYON_SimpleArray<int>& fids,
  int label
)

{
  MYON_SimpleArray<int> new_fids;
  for (int i=0; i<B.m_F.Count(); i++){
    PropagateLabel(B, fids, label, new_fids);
    if (new_fids.Count() == 0)
      return;
    fids = new_fids;
  }
}


void MYON_Brep::LabelConnectedComponent(int face_index, int label) const

{
  if (face_index < 0 || face_index >= m_F.Count())
    return;

  MYON_SimpleArray<int> fids(1);
  fids.Append(face_index);
  const MYON_BrepFace& F = m_F[face_index];
  memset(&F.m_face_user,0,sizeof(F.m_face_user));
  F.m_face_user.i = label;
  PropagateLabel(*this, fids, label);
  return;
}

int MYON_Brep::LabelConnectedComponents() const
{
  Clear_user_i();
  int i;
  for (i=0; i<m_F.Count(); i++){
    if (m_F[i].m_face_index < 0)
      m_F[i].m_face_user.i = -1;
  }

  int label = 0;
  bool keep_going = true;
  while (keep_going)
  {
    int face_index = -1;
    for (int j=0; j<m_F.Count(); j++)
    {
      if (m_F[j].m_face_user.i == 0)
      {
        face_index = j;
        break;
      }
    }
    if (face_index == -1)
    {
      keep_going = false;
      continue;
    }
    label++;
    LabelConnectedComponent(face_index, label);
  }
  return label;
}

int MYON_Brep::GetConnectedComponents( MYON_SimpleArray< MYON_Brep* >& components, bool bDuplicateMeshes ) const
{
  const int count0 = components.Count();
  MYON_Brep brep(*this);
  int count = brep.LabelConnectedComponents();
  if ( count > 1 )
  {
    int cci;
    MYON_SimpleArray<int> fi(brep.m_F.Count());
    for ( cci = 1; cci <= count; cci++ )
    {
      fi.SetCount(0);
      for ( int j = 0; j < brep.m_F.Count(); j++ )
      {
        if ( brep.m_F[j].m_face_user.i == cci )
          fi.Append(j);
      }
      if ( fi.Count() > 0 )
      {
        MYON_Brep* cc = brep.DuplicateFaces( fi.Count(), fi.Array(), bDuplicateMeshes );
        if ( cc )
          components.Append(cc);
      }
    }
  }

  return components.Count() - count0;
}

MYON_Brep* MYON_Brep::DuplicateFace( int face_index, bool bDuplicateMeshes ) const
{
  return DuplicateFaces( 1, &face_index, bDuplicateMeshes );
}

MYON_Brep* MYON_Brep::DuplicateFaces( int face_count, const int* face_index, bool bDuplicateMeshes ) const
{
  int fi, si, fli, lti, li, ti, i;
  bool rc = false;
  MYON_Brep* brep_copy = 0;
  MYON_Object* dup = 0;

  // mark vertices, edges, faces, surfaces, and curves to duplicate
  const int s_ct = m_S.Count();
  MYON_SimpleArray<int> s_remap(s_ct);
  s_remap.SetCount(s_ct);
  s_remap.Zero();
  
  const int f_ct = m_F.Count();
  MYON_SimpleArray<int> f_remap(f_ct);
  f_remap.SetCount(f_ct);
  f_remap.Zero();
  
  const int c2_ct = m_C2.Count();
  MYON_SimpleArray<int> c2_remap(c2_ct);
  c2_remap.SetCount(c2_ct);
  c2_remap.Zero();
  
  const int c3_ct = m_C3.Count();
  MYON_SimpleArray<int> c3_remap(c3_ct);
  c3_remap.SetCount(c3_ct);
  c3_remap.Zero();
  
  const int e_ct = m_E.Count();
  MYON_SimpleArray<int> e_remap(e_ct);
  e_remap.SetCount(e_ct);
  e_remap.Zero();
  
  const int v_ct = m_V.Count();
  MYON_SimpleArray<int> v_remap(v_ct);
  v_remap.SetCount(v_ct);
  v_remap.Zero();
  
  const int t_ct = m_T.Count();
  const int l_ct = m_L.Count();

  for (i = 0; i < face_count; i++ )
  {
    fi = face_index[i];
    if (fi >= 0 && fi < f_ct )
    {
      const MYON_BrepFace& face = m_F[fi];
      rc = true;
      f_remap[fi] = 1;
      si = face.m_si;
      if ( si >= 0 && si < s_ct )
      {
        s_remap[si] = 1;
      }
      for ( fli = 0; fli < face.m_li.Count(); fli++ ) 
      {
        li = face.m_li[fli];
        if ( li < 0 || li >= l_ct )
          continue;
        const MYON_BrepLoop& loop = m_L[li];
        for ( lti = 0; lti < loop.m_ti.Count(); lti++ ) 
        {
          ti = loop.m_ti[lti];
          if ( ti < 0 || ti >= t_ct )
            continue;
          const MYON_BrepTrim& trim = m_T[ti];
          if ( trim.m_ei >= 0 && trim.m_ei < e_ct )
          {
            int vi;
            e_remap[trim.m_ei] = 1;
            vi = m_E[trim.m_ei].m_vi[0];
            if ( vi >= 0 && v_ct > vi)
              v_remap[vi] = 1;
            vi = m_E[trim.m_ei].m_vi[1];
            if ( vi >= 0 && v_ct > vi)
              v_remap[vi] = 1;
          }
          if ( trim.m_vi[0] >= 0 && v_ct > trim.m_vi[0])
            v_remap[trim.m_vi[0]] = 1;
          if ( trim.m_vi[1] >= 0 && v_ct > trim.m_vi[1])
            v_remap[trim.m_vi[1]] = 1;
          int ci = trim.EdgeCurveIndexOf();
          if ( ci >= 0 && c3_ct > ci)
            c3_remap[ci] = 1;
          ci = trim.TrimCurveIndexOf();
          if ( ci >= 0 && c2_ct > ci)
            c2_remap[ci] = 1;
        }
      }
    }
  }
  if ( !rc )
    return nullptr;

  brep_copy = new MYON_Brep();

  // duplicate surfaces
  for ( i = 0; i < s_ct && rc; i++ )
  {
    if ( s_remap[i] ) {
      if ( !m_S[i] )
        break;
      dup = m_S[i]->Duplicate();
      MYON_Surface* srf_copy = MYON_Surface::Cast(dup);
      if ( !srf_copy )
        break;
      s_remap[i] = brep_copy->AddSurface(srf_copy);
      dup = 0;
    }
    else
      s_remap[i] = -1;
  }
  rc = ( rc && i == s_ct );

  // duplicate 2d curves
  for ( i = 0; i < c2_ct && rc; i++ )
  {
    if ( c2_remap[i] ) {
      if ( !m_C2[i] )
        break;
      dup = m_C2[i]->Duplicate();
      MYON_Curve* crv_copy = MYON_Curve::Cast(dup);
      if ( !crv_copy )
        break;
      c2_remap[i] = brep_copy->AddTrimCurve(crv_copy);
      dup = 0;
    }
    else
      c2_remap[i] = -1;
  }
  rc = ( rc && i == c2_ct );

  // duplicate 3d curves
  for ( i = 0; i < c3_ct && rc; i++ )
  {
    if ( c3_remap[i] ) {
      if ( !m_C3[i] )
        break;
      dup = m_C3[i]->Duplicate();
      MYON_Curve* crv_copy = MYON_Curve::Cast(dup);
      if ( !crv_copy )
        break;
      c3_remap[i] = brep_copy->AddEdgeCurve(crv_copy);
      dup = 0;
    }
    else
      c3_remap[i] = -1;
  }
  rc = ( rc && i == c3_ct );

  // duplicate vertices
  for (i = 0; i < v_ct && rc; i++ )
  {
    if (v_remap[i]) 
    {
      MYON_BrepVertex& vertex_copy = brep_copy->NewVertex(m_V[i].point);
      memset(&vertex_copy.m_vertex_user,0,sizeof(vertex_copy.m_vertex_user));
      vertex_copy.m_vertex_user.i = i;
      v_remap[i] = vertex_copy.m_vertex_index;
    }
    else
      v_remap[i] = -1;
  }
  rc = ( rc && i == v_ct );

  // duplicate edges
  for (i = 0; i < e_ct && rc; i++ )
  {
    if (e_remap[i]) 
    {
      const MYON_BrepEdge& edge = m_E[i];
      //int vi0 = edge.m_vi[0];
      if ( edge.m_vi[0] < 0 || edge.m_vi[1] < 0 || edge.m_c3i < 0 )
        break;
      if ( v_remap[edge.m_vi[0]] < 0 || v_remap[edge.m_vi[1]] < 0 || c3_remap[edge.m_c3i] < 0)
        break;
      MYON_BrepEdge& edge_copy = brep_copy->NewEdge( brep_copy->m_V[v_remap[edge.m_vi[0]]], 
                                                   brep_copy->m_V[v_remap[edge.m_vi[1]]], 
                                                   c3_remap[edge.m_c3i]
                                                   );
      edge_copy.SetProxyCurveDomain( edge.ProxyCurveDomain());
      if ( edge.ProxyCurveIsReversed() )
        edge_copy.MYON_CurveProxy::Reverse();
      edge_copy.SetDomain(edge.Domain());

      memset(&edge_copy.m_edge_user,0,sizeof(edge_copy.m_edge_user));
      edge_copy.m_edge_user.i = i;
      edge_copy.m_tolerance = edge.m_tolerance;
      e_remap[i] = edge_copy.m_edge_index;
    }
    else
      e_remap[i] = -1;
  }
  rc = ( rc && i == e_ct );

  //03/11/2010 Tim
  //More checking to prevent crashes
  //from bogus array indices
  bool bFoundBadIdx = false;

  // duplicate faces
  for ( fi = 0; rc && fi < f_ct && rc && !bFoundBadIdx; fi++ )
  {
    if ( f_remap[fi] == 0 )
      continue;
    rc = false;
    const MYON_BrepFace& face = m_F[fi];

    // duplicate face
    si = (face.m_si>=0 && s_ct > face.m_si) ? s_remap[face.m_si] : -1;
    if ( si < 0 )
      break;

    MYON_BrepFace& face_copy = brep_copy->NewFace(si);
    memset(&face_copy.m_face_user,0,sizeof(face_copy.m_face_user));
    face_copy.m_face_user.i = fi;
    face_copy.m_bRev = face.m_bRev;
    face_copy.m_bbox = face.m_bbox;
    face_copy.m_domain[0] = face.m_domain[0];
    face_copy.m_domain[1] = face.m_domain[1];
    face_copy.m_per_face_color = face.m_per_face_color;
    face_copy.SetMaterialChannelIndex(face.MaterialChannelIndex());
    // do NOT duplicate meshes here

    // duplicate loops and trims
    for ( fli = 0; fli < face.m_li.Count() && !bFoundBadIdx; fli++ )
    {
      li = face.m_li[fli];
      if (0 > li || l_ct <= li)
      {
        bFoundBadIdx = true;
        break;
      }

      const MYON_BrepLoop& loop = m_L[li];
      MYON_BrepLoop& loop_copy = brep_copy->NewLoop( loop.m_type, face_copy );
      memset(&loop_copy.m_loop_user,0,sizeof(loop_copy.m_loop_user));
      loop_copy.m_loop_user.i = li;
      for ( lti = 0; lti < loop.m_ti.Count() && !bFoundBadIdx; lti++ )
      {
        ti = loop.m_ti[lti];
        if (0 > ti || t_ct <= ti)
        {
          bFoundBadIdx = true;
          break;
        }
        const MYON_BrepTrim& trim = m_T[ti];
        i = (trim.m_c2i >= 0 && c2_ct > trim.m_c2i) ? c2_remap[trim.m_c2i] : -1;
        if ( trim.m_ei >= 0 && e_ct > trim.m_ei)
        {
          i = brep_copy->NewTrim( brep_copy->m_E[e_remap[trim.m_ei]], trim.m_bRev3d, loop_copy, i ).m_trim_index;
        }
        else
        {
          i = brep_copy->NewTrim( trim.m_bRev3d, loop_copy, i ).m_trim_index;
          int vi0 = (trim.m_vi[0] >= 0  && v_ct > trim.m_vi[0]) ? v_remap[trim.m_vi[0]] : -1;
          int vi1 = (trim.m_vi[1] >= 0  && v_ct > trim.m_vi[1]) ? v_remap[trim.m_vi[1]] : -1;
          brep_copy->m_T[i].m_vi[0] = vi0;
          brep_copy->m_T[i].m_vi[1] = vi1;
        }
        MYON_BrepTrim& trim_copy = brep_copy->m_T[i];
        
        //trim_copy.m_t = trim.m_t;
        trim_copy.SetProxyCurveDomain( trim.ProxyCurveDomain());
        if ( trim.ProxyCurveIsReversed() )
          trim_copy.MYON_CurveProxy::Reverse();
        trim_copy.SetDomain(trim.Domain());

        memset(&trim_copy.m_trim_user,0,sizeof(trim_copy.m_trim_user));
        trim_copy.m_trim_user.i = ti;
        trim_copy.m_iso = trim.m_iso;
        trim_copy.m_tolerance[0] = trim.m_tolerance[0];
        trim_copy.m_tolerance[1] = trim.m_tolerance[1];
        trim_copy.m_pline = trim.m_pline;
        trim_copy.m_pbox = trim.m_pbox;
        trim_copy.m__legacy_2d_tol = trim.m__legacy_2d_tol;
        trim_copy.m__legacy_3d_tol = trim.m__legacy_3d_tol;
        trim_copy.m__legacy_flags = trim.m__legacy_flags;
      }

      if (bFoundBadIdx)
        break;
      
      loop_copy.m_pbox = loop.m_pbox;
    }

    if (bFoundBadIdx)
      break;

    if ( bDuplicateMeshes ) 
    {
    
      {
        auto sp = face.SharedMesh(MYON::render_mesh);
        if (sp)
        {
          face_copy.SetMesh(MYON::render_mesh, new MYON_Mesh(*sp.get()));
        }
      }

      {
        auto sp = face.SharedMesh(MYON::analysis_mesh);
        if (sp)
        {
          face_copy.SetMesh(MYON::analysis_mesh, new MYON_Mesh(*sp.get()));
        }
      }

      {
        auto sp = face.SharedMesh(MYON::preview_mesh);
        if (sp)
        {
          face_copy.SetMesh(MYON::preview_mesh, new MYON_Mesh(*sp.get()));
        }
      }
    }

    rc = true;
  }
  rc = ( rc && fi == m_F.Count() );
  
  if ( !rc ) {
    if ( dup ) {
      delete dup;
      dup = 0;
    }
    if ( brep_copy ) {
      delete brep_copy;
      brep_copy = 0;
    }
  }
  else 
  {
    // set flags, tolerances, etc. that have changed
    brep_copy->SetTrimTypeFlags();
    brep_copy->SetVertexTolerances();
  }
  return brep_copy;
}

MYON_Brep* MYON_Brep::ExtractFace( int face_index )
{
  MYON_Brep* brep_copy = DuplicateFace(face_index,false);
  if ( brep_copy ) {
    MYON_BrepFace& face = m_F[face_index];
    MYON_BrepFace& face_copy = brep_copy->m_F[0];
    face_copy.SetMesh(MYON::render_mesh, face.SharedMesh(MYON::render_mesh));
    face_copy.SetMesh(MYON::analysis_mesh, face.SharedMesh(MYON::analysis_mesh));
    face_copy.SetMesh(MYON::preview_mesh, face.SharedMesh(MYON::preview_mesh));
    DeleteFace( face, true );
  }
  return brep_copy;
}

void MYON_Brep::DeleteSurface(int si)
{
  if ( si >= 0 && si < m_S.Count() ) {
    delete m_S[si];
    m_S[si] = 0;
  }
}

void MYON_Brep::Delete2dCurve(int c2i)
{
  if ( c2i >= 0 && c2i < m_C2.Count() ) {
    delete m_C2[c2i];
    m_C2[c2i] = 0;
  }
}

void MYON_Brep::Delete3dCurve(int c3i)
{
  if ( c3i >= 0 && c3i < m_C3.Count() ) {
    delete m_C3[c3i];
    m_C3[c3i] = 0;
  }
}


bool MYON_Brep::CullUnusedFaces()
{
  bool rc = true;
  int fcount = m_F.Count();
  if (fcount > 0 ) {
    MYON_Workspace ws;
    int *fmap = ws.GetIntMemory(fcount+1);
    *fmap++ = -1;
    memset( fmap, 0, fcount*sizeof(*fmap) );
    const int lcount = m_L.Count();
    int fi, li, mi = 0;

    // if face.m_face_index is -1, cull face
    for ( fi = 0; fi < fcount; fi++ ) {
      MYON_BrepFace& face = m_F[fi];
      if ( face.m_face_index == -1)
        fmap[fi] = -1;
      else if ( face.m_face_index == fi )
        fmap[fi] = face.m_face_index = mi++;
      else {
        MYON_ERROR("Brep face has illegal m_face_index.");
        rc = false;
        fmap[fi] = face.m_face_index;
      }
    }

    if ( mi == 0 ) {
      m_F.Destroy();
    }
    else if ( mi < fcount ) {
      // set new face indices
      mi = 0;
      for ( fi = fcount-1; fi >= 0; fi-- ) {
        if ( m_F[fi].m_face_index  == -1 )
          m_F.Remove(fi);
        else
          m_F[fi].m_face_index = fmap[fi];
      }

      // remap loop.m_fi indices
      for ( li = 0; li < lcount; li++ ) {
        MYON_BrepLoop& loop = m_L[li];
        fi = loop.m_fi;
        if ( fi < -1 || fi >= fcount ) {
          MYON_ERROR("Brep loop has illegal m_fi.");
          rc = false;
        }
        else 
          loop.m_fi = fmap[fi];
      }

    }
  }
  m_F.Shrink();
  if (m_F.Count() < fcount)
    DestroyRegionTopology();
  return rc;
}

bool MYON_Brep::CullUnusedSurfaces()
{
  // remove unused surfaces
  bool rc = true;
  const int fcount = m_F.Count();
  int scount = m_S.Count();
  int si, fi, mi;

  if ( scount > 0 ) {
    MYON_Workspace ws;
    int* smap = ws.GetIntMemory(scount+1);
    *smap++ = -1;
    memset(smap,0,scount*sizeof(*smap));
    mi = 0;
    for ( fi = 0; fi < fcount; fi++ ) {
      MYON_BrepFace& face = m_F[fi];
      if ( face.m_face_index == -1 ) {
        face.m_si = -1;
        continue;
      }
      si = face.m_si;
      if ( si == -1 )
        continue;
      if ( si < 0 || si >= scount ) {
        MYON_ERROR("Brep face has illegal m_si.");
        rc = false;
      }
      else {
        if ( !smap[si] )
          mi++;
        smap[si]++;
      }
    }

    if ( mi == 0 ) {
      m_S.Destroy();
    }
    else if ( mi < scount ) {
      mi = 0;
      for ( si = 0; si < scount; si++ ) {
        if ( smap[si] )
          smap[si] = mi++;
        else {
          delete m_S[si];
          m_S[si] = 0;
          smap[si] = -1;
        }
      }

      for ( fi = 0; fi < fcount; fi++ ) {
        MYON_BrepFace& face = m_F[fi];
        si = face.m_si;
        if ( si >= 0 && si < scount )
          face.m_si = smap[si];
      }

      for ( si = scount-1; si >= 0; si-- ) {
        if ( smap[si] < 0 ) {
          m_S.Remove(si);
          scount--;
        }
      }
    }
  }
  m_S.Shrink();
  return rc;
}

bool MYON_Brep::CullUnused3dCurves()
{
  // remove unused surfaces
  bool rc = true;
  const int ecount = m_E.Count();
  int c3count = m_C3.Count();
  int c3i, ei, mi;

  if ( c3count > 0 ) {
    MYON_Workspace ws;
    int* c3map = ws.GetIntMemory(c3count+1);
    *c3map++ = -1;
    memset(c3map,0,c3count*sizeof(*c3map));
    mi = 0;
    for ( ei = 0; ei < ecount; ei++ ) {
      MYON_BrepEdge& edge = m_E[ei];
      if ( edge.m_edge_index == -1 ) {
        edge.m_c3i = -1;
        continue;
      }
      c3i = edge.m_c3i;
      if ( c3i == -1 )
        continue;
      if ( c3i < -1 || c3i >= c3count ) {
        MYON_ERROR("Brep edge has illegal m_c3i.");
        rc = false;
      }
      else {
        if ( !c3map[c3i] )
          mi++;
        c3map[c3i]++;
      }
    }

    if ( mi == 0 ) {
      m_C3.Destroy();
    }
    else if ( mi < c3count ) {
      mi = 0;
      for ( c3i = 0; c3i < c3count; c3i++ ) {
        if ( c3map[c3i] )
          c3map[c3i] = mi++;
        else {
          delete m_C3[c3i];
          m_C3[c3i] = 0;
          c3map[c3i] = -1;
        }
      }

      for ( ei = 0; ei < ecount; ei++ ) {
        MYON_BrepEdge& edge = m_E[ei];
        c3i = edge.m_c3i;
        if ( c3i >= 0 && c3i < c3count )
          edge.m_c3i = c3map[c3i];
      }

      for ( c3i = c3count-1; c3i >= 0; c3i-- ) {
        if ( c3map[c3i] < 0 ) {
          m_C3.Remove(c3i);
          c3count--;
        }
      }
    }
  }
  m_C3.Shrink();
  return rc;
}


bool MYON_Brep::CullUnused2dCurves()
{
  // remove unused surfaces
  bool rc = true;
  const int tcount = m_T.Count();
  int c2count = m_C2.Count();
  int c2i, ti, mi;

  if ( c2count > 0 ) 
  {
    MYON_Workspace ws;
    int* c2map = ws.GetIntMemory(c2count+1);
    *c2map++ = -1;
    memset(c2map,0,c2count*sizeof(*c2map));
    mi = 0;
    for ( ti = 0; ti < tcount; ti++ ) {
      MYON_BrepTrim& trim = m_T[ti];
      if ( trim.m_trim_index == -1 ) {
        trim.m_c2i = -1;
        continue;
      }
      c2i = trim.m_c2i;
      if ( c2i == -1 )
        continue;
      if ( c2i < -1 || c2i >= c2count ) {
        MYON_ERROR("Brep trim has illegal m_c2i.");
        rc = false;
      }
      else {
        if ( !c2map[c2i] )
          mi++;
        c2map[c2i]++;
      }
    }

    if ( mi == 0 ) {
      m_C2.Destroy();
    }
    else if ( mi < c2count ) {
      mi = 0;
      for ( c2i = 0; c2i < c2count; c2i++ ) {
        if ( c2map[c2i] )
          c2map[c2i] = mi++;
        else {
          delete m_C2[c2i];
          m_C2[c2i] = 0;
          c2map[c2i] = -1;
        }
      }

      for ( ti = 0; ti < tcount; ti++ ) {
        MYON_BrepTrim& trim = m_T[ti];
        c2i = trim.m_c2i;
        if ( c2i >= 0 && c2i < c2count )
          trim.m_c2i = c2map[c2i];
      }

      for ( c2i = c2count-1; c2i >= 0; c2i-- ) {
        if ( c2map[c2i] < 0 ) {
          m_C2.Remove(c2i);
          c2count--;
        }
      }
    }
  }
  m_C2.Shrink();
  return rc;
}


bool MYON_Brep::CullUnusedLoops()
{
  bool rc = true;
  const int lcount = m_L.Count();
  if ( lcount > 0 ) {
    MYON_Workspace ws;
    int* lmap = ws.GetIntMemory(lcount+1);
    *lmap++ = -1;
    memset( lmap, 0, lcount*sizeof(*lmap) );
    const int fcount = m_F.Count();
    const int tcount = m_T.Count();
    int li, fli, flcnt, fi, ti, mi;

    mi = 0;
    for ( li = 0; li < lcount; li++ ) {
      MYON_BrepLoop& loop = m_L[li];
      if ( loop.m_loop_index == -1)
        lmap[li] = -1;
      else if ( loop.m_loop_index == li )
        lmap[li] = loop.m_loop_index = mi++;
      else {
        MYON_ERROR("Brep loop has illegal m_loop_index.");
        rc = false;
        lmap[li] = loop.m_loop_index;
      }
    }

    if ( mi == 0 ) {
      m_L.Destroy();
    }
    else if ( mi < lcount ) {

      // remap loops
      for ( li = lcount-1; li >= 0; li-- ) {
        if ( m_L[li].m_loop_index == -1 )
          m_L.Remove(li);
        else
          m_L[li].m_loop_index = lmap[li];
      }

      // remap MYON_BrepFace.m_li[] indices
      for ( fi = 0; fi < fcount; fi++ ) {
        MYON_BrepFace& face = m_F[fi];
        flcnt = face.m_li.Count();
        for ( fli = flcnt-1; fli >= 0; fli-- ) {
          li = face.m_li[fli];
          if ( li < -1 || li >= lcount ) {
            MYON_ERROR("Brep face m_li[] has illegal loop index.");
            rc = false;
          }
          else {
            li = lmap[li];
            if (li >= 0 ) {
              face.m_li[fli] = li;
            }
            else {
              face.m_li.Remove(fli);
            }
          }
        }
      }

      // remap MYON_BrepTrim.m_li indices
      for ( ti = 0; ti < tcount; ti++ ) {
        MYON_BrepTrim& trim = m_T[ti];
        li = trim.m_li;
        if ( li < -1 || li >= lcount ) {
          MYON_ERROR("Brep trim has illegal m_li.");
          rc = false;
        }
        else {
          trim.m_li = lmap[li];
        }
      }
    }
  }
  m_L.Shrink();
  return rc;
}

bool MYON_Brep::CullUnusedTrims()
{
  bool rc = true;
  const int tcount = m_T.Count();
  if ( tcount > 0 ) {
    MYON_Workspace ws;
    int *tmap = ws.GetIntMemory(tcount+1);
    *tmap++ = -1;
    memset( tmap, 0, tcount*sizeof(*tmap));
    const int lcount = m_L.Count();
    const int ecount = m_E.Count();
    int ti, li, ei, mi, ltcnt, lti, etcnt, eti;

    mi = 0;
    for ( ti = 0; ti < tcount; ti++ ) {
      MYON_BrepTrim& trim = m_T[ti];
      if ( trim.m_trim_index == -1)
        tmap[ti] = -1;
      else if ( trim.m_trim_index == ti )
        tmap[ti] = trim.m_trim_index = mi++;
      else {
        MYON_ERROR("Brep trim has illegal m_trim_index.");
        rc = false;
        tmap[ti] = trim.m_trim_index;
      }
    }

    if ( mi == 0 ) {
      m_T.Destroy();
    }
    else if ( mi < tcount ) {
      // remap trim indices
      for ( ti = tcount-1; ti >= 0; ti-- ) {
        if ( m_T[ti].m_trim_index == -1 ) {
          m_T.Remove(ti);
        }
        else {
          m_T[ti].m_trim_index = tmap[ti];
        }
      }

      // remap loop.m_ti[] indicies
      for ( li = 0; li < lcount; li++ ) {
        MYON_BrepLoop& loop = m_L[li];
        ltcnt = loop.m_ti.Count();
        for ( lti = ltcnt-1; lti >= 0; lti-- ) {
          ti = loop.m_ti[lti];
          if ( ti < -1 || ti >= tcount ) {
            MYON_ERROR("Brep loop.m_ti[] has illegal index.");
            rc = false;
          }
          else {
            ti = tmap[ti];
            if (ti >= 0 ) {
              loop.m_ti[lti] = ti;
            }
            else {
              loop.m_ti.Remove(lti);
            }
          }
        }
      }

      // remap edge.m_ti[] indicies
      for ( ei = 0; ei < ecount; ei++ ) {
        MYON_BrepEdge& edge = m_E[ei];
        etcnt = edge.m_ti.Count();
        for ( eti = etcnt-1; eti >= 0; eti-- ) {
          ti = edge.m_ti[eti];
          if ( ti < -1 || ti >= tcount ) {
            MYON_ERROR("Brep edge.m_ti[] has illegal index.");
            rc = false;
          }
          else {
            ti = tmap[ti];
            if (ti >= 0 ) {
              edge.m_ti[eti] = ti;
            }
            else {
              edge.m_ti.Remove(eti);
            }
          }
        }
      }
    }
  }
  m_T.Shrink();
  return rc;
}

bool MYON_Brep::CullUnusedEdges()
{
  bool rc = true;
  const int ecount = m_E.Count();
  if ( ecount > 0 ) {
    MYON_Workspace ws;
    int* emap = ws.GetIntMemory(ecount+1);
    *emap++ = -1;
    memset( emap, 0, ecount*sizeof(*emap) );
    const int vcount = m_V.Count();
    const int tcount = m_T.Count();
    int ei, ti, vi, mi, vecnt, vei;

    mi = 0;
    for ( ei = 0; ei < ecount; ei++ ) {
      MYON_BrepEdge& edge = m_E[ei];
      if ( edge.m_edge_index == -1)
        emap[ei] = -1;
      else if ( edge.m_edge_index == ei )
        emap[ei] = edge.m_edge_index = mi++;
      else {
        MYON_ERROR("Brep edge has illegal m_edge_index.");
        rc = false;
        emap[ei] = edge.m_edge_index;
      }
    }

    if ( mi == 0 ) {
      m_E.Destroy();
    }
    else if ( mi < ecount ) 
    {
      // remap edge indices
      for ( ei = ecount-1; ei >= 0; ei-- ) {
        if ( m_E[ei].m_edge_index == -1 ) {
          m_E.Remove(ei);
        }
        else {
          m_E[ei].m_edge_index = emap[ei];
        }
      }

      // remap trim.m_ei
      for ( ti = 0; ti < tcount; ti++ ) {
        MYON_BrepTrim& trim = m_T[ti];
        ei = trim.m_ei;
        if ( ei < -1 || ei >= ecount ) {
          MYON_ERROR("Brep trim.m_ei has illegal index.");
          rc = false;
        }
        else {
          trim.m_ei = emap[ei];
        }
      }

      // remap vertex.m_ei[]
      for ( vi = 0; vi < vcount; vi++ ) {
        MYON_BrepVertex& vertex = m_V[vi];
        vecnt = vertex.m_ei.Count();
        for ( vei = vecnt-1; vei >= 0; vei-- ) {
          ei = vertex.m_ei[vei];
          if ( ei < -1 || ei >= ecount ) {
            MYON_ERROR("Brep vertex.m_ei[] has illegal index.");
            rc = false;
          }
          else {
            ei = emap[ei];
            if (ei >= 0 ) {
              vertex.m_ei[vei] = ei;
            }
            else {
              vertex.m_ei.Remove(vei);
            }
          }
        }
      }
    }
  }
  m_E.Shrink();
  return rc;
}


bool MYON_Brep::CullUnusedVertices()
{
  bool rc = true;
  const int vcount = m_V.Count();
  if ( vcount > 0 ) 
  {
    MYON_Workspace ws;
    int* vmap = ws.GetIntMemory(vcount+1);
    *vmap++ = -1;
    memset(vmap,0,vcount*sizeof(*vmap));
    const int tcount = m_T.Count();
    const int ecount = m_E.Count();
    int vi, ei, ti, mi, j;

    if ( tcount > 0 )
    {
      // 11 Nov 2009 Dale Lear
      //  I added this code to fix bugs 55879 and 56191.
      for ( ti = 0; ti < tcount; ti++ )
      {
        const MYON_BrepTrim& trim = m_T[ti];
        if ( -1 == trim.m_trim_index )
          continue;
        vi = trim.m_vi[0];
        if ( vi >= 0 && vi < vcount && -1 == m_V[vi].m_vertex_index )
        {
          // undelete this vertex
          // This error happens when the MYON_Brep is invalid to begin with.
          // However, in order to prevent crashes, we have to refuse to delete
          // the vertex.  See bugs 55879 and 56191.
          MYON_ERROR("MYON_Brep::CullUnusedVertices() - deleted vertex referenced by trim.m_vi[0]");
          m_V[vi].m_vertex_index = vi;
        }
        vi = trim.m_vi[1];
        if ( vi >= 0 && vi < vcount && -1 == m_V[vi].m_vertex_index )
        {
          // undelete this vertex
          // This error happens when the MYON_Brep is invalid to begin with.
          // However, in order to prevent crashes, we have to refuse to delete
          // the vertex.  See bugs 55879 and 56191.
          MYON_ERROR("MYON_Brep::CullUnusedVertices() - deleted vertex referenced by trim.m_vi[1]");
          m_V[vi].m_vertex_index = vi;
        }
      }
    }

    mi = 0;
    for ( vi = 0; vi < vcount; vi++ ) {
      MYON_BrepVertex& vertex = m_V[vi];
      if ( vertex.m_vertex_index == -1)
        vmap[vi] = -1;
      else if ( vertex.m_vertex_index == vi )
        vmap[vi] = vertex.m_vertex_index = mi++;
      else {
        MYON_ERROR("Brep vertex has illegal m_vertex_index.");
        rc = false;
        vmap[vi] = vertex.m_vertex_index;
      }
    }

    if ( mi == 0 ) 
    {
      m_V.Destroy();
    }
    else if ( mi < vcount ) 
    {
      // remap vertex indices
      for ( vi = vcount-1; vi >= 0; vi-- ) 
      {
        if ( m_V[vi].m_vertex_index == -1 )
        {
          m_V.Remove(vi);
        }
        else {
          m_V[vi].m_vertex_index = vmap[vi];
        }
      }

      // remap edge indices
      for ( ei = 0; ei < ecount; ei++ ) 
      {
        MYON_BrepEdge& edge = m_E[ei];
        for ( j = 0; j < 2; j++ ) 
        {
          vi = edge.m_vi[j];
          if ( vi < -1 || vi >= vcount ) 
          {
            MYON_ERROR("Brep edge.m_vi[] has illegal index.");
            rc = false;
          }
          else {
            edge.m_vi[j] = vmap[vi];
          }
        }
      }

      // remap trim indices
      for ( ti = 0; ti < tcount; ti++ )
      {
        MYON_BrepTrim& trim = m_T[ti];
        for ( j = 0; j < 2; j++ ) 
        {
          vi = trim.m_vi[j];
          if ( vi < -1 || vi >= vcount ) 
          {
            MYON_ERROR("Brep trim.m_vi[] has illegal index.");
            rc = false;
          }
          else {
            trim.m_vi[j] = vmap[vi];
          }
        }
      }

    }
  }
  m_V.Shrink();
  return rc;
}

bool MYON_Brep::Compact()
{
  // Removes any unreferenced objects from arrays,
  // reindexes as needed, and shrinks arrays to
  // minimum required size.

  bool rc = true;
  if (!CullUnusedFaces())
    rc = false;
  if (!CullUnusedEdges())
    rc = false;
  if (!CullUnusedVertices())
    rc = false;
  if (!CullUnusedLoops())
    rc = false;
  if (!CullUnusedTrims())
    rc = false;

  if (!CullUnusedSurfaces())
    rc = false;
  if (!CullUnused3dCurves())
    rc = false;
  if (!CullUnused2dCurves())
    rc = false;

  // If 1-1 relationships exist between geometry and topology,
  // the synchronize the geometry and topology indices.  This
  // helps confused users of breps not have to understand the
  // differences between geometry and topology data.
  MYON_SimpleArray<bool> used;
  bool bSyncUp;

  if ( m_C2.Count() == m_T.Count() )
  {
    int i, count = m_C2.Count();
    used.Reserve(count);
    used.SetCount(count);
    used.Zero();
    bSyncUp = true;
    for ( i = 0; i < count && bSyncUp; i++ )
    {
      const MYON_BrepTrim& trim = m_T[i];
      if ( trim.m_trim_index != i || trim.m_c2i < 0 || trim.m_c2i >= count )
        bSyncUp = false;
      else
      {
        if (used[trim.m_c2i])
          bSyncUp = false;
        else
          used[trim.m_c2i] = true;
      }
    }
    if ( bSyncUp )
    {
      MYON_SimpleArray< MYON_Curve* > ptr(count);
      for( i = 0; i < count; i++ )
      {
        MYON_BrepTrim& trim = m_T[i];
        ptr[i] = m_C2[trim.m_c2i];
        trim.m_c2i = i;
      }
      for( i = 0; i < count; i++ )
      {
        m_C2[i] = ptr[i];
      }
    }
  }

  if ( m_C3.Count() == m_E.Count() )
  {
    int i, count = m_C3.Count();
    used.Reserve(count);
    used.SetCount(count);
    used.Zero();
    bSyncUp = true;
    for ( i = 0; i < count && bSyncUp; i++ )
    {
      const MYON_BrepEdge& edge = m_E[i];
      if ( edge.m_edge_index != i || edge.m_c3i < 0 || edge.m_c3i >= count )
        bSyncUp = false;
      else
      {
        if (used[edge.m_c3i])
          bSyncUp = false;
        else
          used[edge.m_c3i] = true;
      }
    }
    if ( bSyncUp )
    {
      MYON_SimpleArray< MYON_Curve* > ptr(count);
      for( i = 0; i < count; i++ )
      {
        MYON_BrepEdge& edge = m_E[i];
        ptr[i] = m_C3[edge.m_c3i];
        edge.m_c3i = i;
      }
      for( i = 0; i < count; i++ )
      {
        m_C3[i] = ptr[i];
      }
    }
  }

  if ( m_S.Count() == m_F.Count() )
  {
    int i, count = m_S.Count();
    used.Reserve(count);
    used.SetCount(count);
    used.Zero();
    bSyncUp = true;
    for ( i = 0; i < count && bSyncUp; i++ )
    {
      const MYON_BrepFace& face = m_F[i];
      if ( face.m_face_index != i || face.m_si < 0 || face.m_si >= count )
        bSyncUp = false;
      else
      {
        if (used[face.m_si])
          bSyncUp = false;
        else
          used[face.m_si] = true;
      }
    }
    if ( bSyncUp )
    {
      MYON_SimpleArray< MYON_Surface* > ptr(count);
      for( i = 0; i < count; i++ )
      {
        MYON_BrepFace& face = m_F[i];
        ptr[i] = m_S[face.m_si];
        face.m_si = i;
      }
      for( i = 0; i < count; i++ )
      {
        m_S[i] = ptr[i];
      }
    }
  }

  return rc;
}


MYON_Brep& MYON_Brep::operator=(const MYON_Brep& src)
{
  if ( this != &src ) 
  {
    Destroy();
    MYON_Geometry::operator=(src);

    m_V.SetCapacity(src.m_V.Count());
    m_E.SetCapacity(src.m_E.Count());
    m_F.SetCapacity(src.m_F.Count());
    m_T.SetCapacity(src.m_T.Count());
    m_L.SetCapacity(src.m_L.Count());

    m_V.SetCount(src.m_V.Count());
    m_E.SetCount(src.m_E.Count());
    m_F.SetCount(src.m_F.Count());
    m_T.SetCount(src.m_T.Count());
    m_L.SetCount(src.m_L.Count());

    src.m_C2.Duplicate( m_C2 );
    src.m_C3.Duplicate( m_C3 );
    src.m_S.Duplicate( m_S );

    const int C2_count = m_C2.Count();
    const int C3_count = m_C3.Count();
    const int S_count = m_S.Count();

    int i, count = m_V.Count();
    for ( i = 0; i < count; i++ ) 
    {
      m_V[i] = src.m_V[i];
    }

    count = m_E.Count();
    for ( i = 0; i < count; i++ ) 
    {
      m_E[i] = src.m_E[i];
      MYON_BrepEdge& e = m_E[i];
      e.m_brep = this;

      if (e.m_c3i >= C3_count)
      {
        MYON_ERROR("src brep has invalid MYON_BrepEdge.m_c3i value.");
        e.m_c3i = -1;
      }

      // update curve proxy info to point at 3d curve in this brep
      e.SetProxyCurve( ( e.m_c3i >= 0 ) ? m_C3[e.m_c3i] : 0, 
                       src.m_E[i].ProxyCurveDomain()
                       );
      if ( src.m_E[i].ProxyCurveIsReversed() )
        e.MYON_CurveProxy::Reverse();
      e.SetDomain( src.m_E[i].Domain() );
    }

    count = m_L.Count();
    for ( i = 0; i < count; i++ ) 
    {
      m_L[i].m_brep = this;
    }

    count = m_F.Count();
    for ( i = 0; i < count; i++ ) 
    {
      m_F[i] = src.m_F[i];
      MYON_BrepFace& f = m_F[i];
      f.m_brep = this;

      if (f.m_si >= S_count)
      {
        MYON_ERROR("src brep has invalid MYON_BrepFace.m_si value.");
        f.m_si = -1;
      }

      // update surface proxy info to point at 3d surface in this brep
      f.SetProxySurface(( f.m_si >= 0 ) ? m_S[f.m_si] : 0);
      f.m_bbox = src.m_F[i].m_bbox; // because SetProxySurface destroys it
    }

    count = m_T.Count();
    for ( i = 0; i < count; i++ ) 
    {
      m_T[i] = src.m_T[i];
      MYON_BrepTrim& trim = m_T[i];
      trim.m_brep = this;

      if (trim.m_c2i >= C2_count)
      {
        MYON_ERROR("src brep has invalid MYON_BrepTrim.m_c2i value.");
        trim.m_c2i = -1;
      }

      // update curve proxy info to point at 2d curve in this brep
      trim.SetProxyCurve( ( trim.m_c2i >= 0 ) ? m_C2[trim.m_c2i] : 0, 
                          src.m_T[i].ProxyCurveDomain()
                         );
      if ( src.m_T[i].ProxyCurveIsReversed() )
        trim.MYON_CurveProxy::Reverse();
      trim.SetDomain( src.m_T[i].Domain() );
    }

    count = m_L.Count();
    for ( i = 0; i < count; i++ ) 
    {
      m_L[i] = src.m_L[i];
    }

    m_bbox = src.m_bbox;
    m_is_solid = src.m_is_solid;

    if (nullptr != src.m_region_topology){
      m_region_topology = new MYON_BrepRegionTopology(*src.m_region_topology);
      m_region_topology->m_brep = this;
    }
  }
  return *this;
}

void MYON_Brep::Destroy()
{
  m_aggregate_status = MYON_AggregateComponentStatus::Empty;

  if (nullptr != m_region_topology)
  {
    delete m_region_topology;
    m_region_topology = nullptr;
  }

  m_V.Empty();
  m_E.Empty();
  m_F.Empty();
  m_T.Empty();
  m_L.Empty();

  int i, count = m_C2.Count();
  for ( i = 0; i < count; i++ ) {
    delete m_C2[i];
    m_C2[i] = 0;
  }
  m_C2.Empty();
  m_C2.Zero();

  count = m_C3.Count();
  for ( i = 0; i < count; i++ ) {
    delete m_C3[i];
    m_C3[i] = 0;
  }
  m_C3.Empty();
  m_C3.Zero();

  count = m_S.Count();
  for ( i = 0; i < count; i++ ) {
    delete m_S[i];
    m_S[i] = 0;
  }
  m_S.Empty();
  m_S.Zero();

  m_bbox.Destroy();
  m_is_solid = 0;
  // returns Brep to state it has after default construction
}

void MYON_Brep::EmergencyDestroy()
{ 
  // call if memory pool used by b-rep members becomes invalid
  // but MYON_Brep class memory is in a valid pool
  m_V.EmergencyDestroy();
  m_E.EmergencyDestroy();
  m_F.EmergencyDestroy();
  m_T.EmergencyDestroy();
  m_L.EmergencyDestroy();
  m_C2.EmergencyDestroy();
  m_C3.EmergencyDestroy();
  m_S.EmergencyDestroy();
  m_bbox.Destroy();
  m_is_solid = 0;
  m_region_topology = nullptr;
}

bool MYON_Brep::CombineCoincidentVertices(MYON_BrepVertex& vertex0, MYON_BrepVertex& vertex1)
{

  bool rc = false;
  if (&vertex0 == &vertex1) {
    MYON_ERROR("MYON_Brep::CombineCoincidentVertices - vertex0 = vertex1.");
    return rc;
  }
  // moves information to vertex0 and deletes vertex1
  int runaway, vei, vecnt, ei, eti, etcnt, ti, prev_ti, next_ti;

  if ( vertex0.m_vertex_index >= 0 && vertex1.m_vertex_index != vertex0.m_vertex_index ) {
    rc = true;
    // update edges and trim references from vertex0 to vertex1
    vecnt = vertex1.m_ei.Count();
    for ( vei = 0; vei < vecnt; vei++ ) {
      ei = vertex1.m_ei[vei];
      if ( ei >= 0 ) {
        // update edge vertex indices
        MYON_BrepEdge& edge = m_E[ei];
        if ( edge.m_vi[0] == vertex1.m_vertex_index )
          edge.m_vi[0] = vertex0.m_vertex_index;
        if ( edge.m_vi[1] == vertex1.m_vertex_index )
          edge.m_vi[1] = vertex0.m_vertex_index;

        // update trim vertex indices
        etcnt = edge.m_ti.Count();
        for (eti = 0; eti < etcnt; eti++ ) {
          ti = edge.m_ti[eti];
          if (ti >= 0 ) {
            MYON_BrepTrim& trim = m_T[ti];
            if ( trim.m_vi[0] == vertex1.m_vertex_index ) {
              trim.m_vi[0] = vertex0.m_vertex_index;
              // check for previous singular trims using vertex0
              for (prev_ti = PrevTrim(ti), runaway=0;prev_ti >= 0 && prev_ti != ti && runaway < 1024;prev_ti=PrevTrim(prev_ti),runaway++) {
                MYON_BrepTrim& prevtrim = m_T[prev_ti];
                if ( prevtrim.m_ei >= 0 )
                  break;
                if ( prevtrim.m_vi[0] == vertex1.m_vertex_index )
                  prevtrim.m_vi[0] = vertex0.m_vertex_index;
                if ( prevtrim.m_vi[1] == vertex1.m_vertex_index )
                  prevtrim.m_vi[1] = vertex0.m_vertex_index;
              }
            }
            if ( trim.m_vi[1] == vertex1.m_vertex_index ) {
              trim.m_vi[1] = vertex0.m_vertex_index;
               // check for previous singular trims using vertex0
              for (next_ti = NextTrim(ti), runaway=0;next_ti >= 0 && next_ti != ti && runaway < 1024;next_ti=NextTrim(next_ti),runaway++) {
                MYON_BrepTrim& nexttrim = m_T[next_ti];
                if ( nexttrim.m_ei >= 0 )
                  break;
                if ( nexttrim.m_vi[0] == vertex1.m_vertex_index )
                  nexttrim.m_vi[0] = vertex0.m_vertex_index;
                if ( nexttrim.m_vi[1] == vertex1.m_vertex_index )
                  nexttrim.m_vi[1] = vertex0.m_vertex_index;
              }
            }
          }
        }
        vertex0.m_ei.Append(ei);
      }
    }
  }

  // update vertex tolerances
  if ( vertex0.m_tolerance != MYON_UNSET_VALUE) 
    SetVertexTolerance(vertex0);

  vertex1.m_vertex_index = -1;
  vertex1.m_ei.Destroy();
  DeleteVertex(vertex1);

  return rc;
}


MYON_BrepEdge* MYON_Brep::CombineContiguousEdges( 
  int ei0, 
  int ei1, 
  double angle_tolerance_radians
  )
{
  // Bug fixers:
  //
  // Lots of (fast)testing is done to ensure the brep is
  // 100% valid at the merge vertex.  Do not change the
  // return 0 bail outs unless you are 100% sure of what
  // you are doing.

  // get edges to be merged
  const MYON_BrepEdge* edge0 = Edge(ei0);
  const MYON_BrepEdge* edge1 = Edge(ei1);
  if ( !edge0 || !edge1 )
    return 0;

  // clear any component index bits
  ei0 = edge0->m_edge_index;
  ei1 = edge1->m_edge_index;
  if ( ei0 < 0 || ei1 < 0 || ei0 == ei1 )
    return 0;

  // make sure edges have same number of trims
  if ( edge0->m_ti.Count() != edge1->m_ti.Count() )
    return 0;

  // figure out which edge ends to merge
	// GBA 1/6/03 Fixed TRR#8951.
	// Check that the vertex to be eliminated has exactly 2 incident edges.
  int end0 = 1, end1 = 0;
	bool MatchFound = false;			
	for(end0=1; !MatchFound && end0>=0; /* empty */){
		int vi = edge0->m_vi[end0];
		const MYON_BrepVertex* v =  Vertex(vi);
		if(v && v->m_ei.Count()==2 ){
			for(end1=0; !MatchFound && end1<2; /*empty*/){
				MatchFound = (vi == edge1->m_vi[end1]);
				if(!MatchFound)
					end1++; 
			}	
		}
		if(!MatchFound)
			end0--;
	}
	if(!MatchFound)
		return 0;

  // vi_mid = index of vertex to be eliminated
  const int vi_mid = edge0->m_vi[end0];
  {
    const MYON_BrepVertex* v = Vertex(vi_mid);
    if ( !v )
      return 0;
    if ( v->m_ei.Count() != 2 )
      return 0;
    if ( v->m_ei[0] != ei0 && v->m_ei[1] != ei0 )
      return 0;
    if ( v->m_ei[0] != ei1 && v->m_ei[1] != ei1 )
      return 0;
  }

  // evi0 = vertex index and other end of edge0
  const int evi0 = edge0->m_vi[1-end0];

  // evi = vertex index and other end of edge1
  const int evi1 = edge1->m_vi[1-end1];
  if ( evi0 == vi_mid )
    return 0;
  if ( evi1 == vi_mid )
    return 0;

  // new edge will start at vi0 and end at vi1
  const int vi0 = (end0==1) ? evi0 : evi1;
  const int vi1 = (end0==1) ? evi1 : evi0;

  // make sure the 3d kink angle at the merge point is <= angle_tolerance
  {
    MYON_3dVector tan0 = edge0->TangentAt( edge0->Domain()[end0] );
    if (end0 == 0)
      tan0 = -tan0;
    MYON_3dVector tan1 = edge1->TangentAt( edge1->Domain()[end1] );
    if (end1 == 1)
      tan1 = -tan1;
    double d = tan0*tan1;
    if ( d < cos(angle_tolerance_radians) )
      return 0;
  }

  // get corresponding pairs of trims to merge
  int trim_count = edge0->m_ti.Count();
  MYON_SimpleArray<int> trim0_index(trim_count);
  MYON_SimpleArray<int> trim1_index(trim_count);
  MYON_SimpleArray<int> loop_lti0(trim_count);
  MYON_SimpleArray<int> loop_lti1(trim_count);

  int eti;
  for ( eti = 0; eti < trim_count; eti++ )
  {
    const MYON_BrepTrim* trim0 = Trim( edge0->m_ti[eti] );
    if ( !trim0 )
      return 0;
    int ti0 = trim0->m_trim_index;
    const MYON_BrepLoop* loop = trim0->Loop();
    if ( !loop )
      return 0;
    if ( loop->m_ti.Count() < 2 )
      return 0;

    // get index of next/prev trim that corresponds to edge1
    bool bRev = (end0==0);
    if ( trim0->m_bRev3d )
      bRev = !bRev;
    int lti1 = -1;
    int lti0 = loop->m_ti.Search( &ti0, MYON_CompareIncreasing<int> );
    if ( lti0 < 0 )
      return 0;
    if ( bRev )
      lti1 = lti0 - 1 + loop->m_ti.Count();
    else
      lti1 = lti0 +1;
    lti1 %= loop->m_ti.Count();
    const MYON_BrepTrim* trim1 = loop->Trim(lti1);
    if ( !trim1 )
      return 0;
    if ( trim1->m_ei != ei1 )
      return 0;
    if ( trim0->m_trim_index == trim1->m_trim_index )
      return 0;

    // test for valid trim vertices and orientations
    int tend0 = trim0->m_bRev3d ? (1-end0) : end0;
    int tend1 = trim1->m_bRev3d ? (1-end1) : end1;
    if ( tend0 == tend1 )
      return 0;
    if ( trim0->m_vi[tend0] != vi_mid )
      return 0;
    if ( trim1->m_vi[tend1] != vi_mid )
      return 0;
    if ( trim0->m_vi[1-tend0] != evi0 )
      return 0;
    if ( trim1->m_vi[1-tend1] != evi1 )
      return 0;
    trim0_index.Append(trim0->m_trim_index);
    trim1_index.Append(trim1->m_trim_index);
    loop_lti0.Append(lti0);
    loop_lti1.Append(lti1);
  }

  // create new 3d edge curve geometry
  // new edge goes same direction as edge0
  MYON_PolyCurve* ec = 0;
  {
    MYON_Curve* ec0 = edge0->DuplicateCurve();
    if ( !ec0 )
      return 0;
    MYON_Curve* ec1 = edge1->DuplicateCurve();
    if ( !ec1 )
    {
      delete ec0;
      return 0;
    }
    if ( end0 == end1 )
    {
      if ( !ec1->Reverse() )
      {
        delete ec0;
        delete ec1;
        return 0;
      }
    }
    ec = new MYON_PolyCurve();
    if ( end0 == 1 )
    {
      ec->Append(ec0);
      ec->AppendAndMatch(ec1);
    }
    else
    {
      ec->Append(ec1);
      ec->AppendAndMatch(ec0);
    }
    ec->RemoveNesting();

    //23 March 2022 - Chuck - Added here to match MYON_PolyCurve::Read().  Otherwise 
    //after saving and reopening, the edge will be invalid if the start or end parameter
    //is changed in the SanitzeDomain() call in Read().  See RH-67919.
    ec->SanitizeDomain();
  }

  // create new 2d trim curve geometry
  MYON_SimpleArray<MYON_Curve*> tc(trim_count);
  for ( eti = 0; eti < trim_count; eti++ )
  {
    const MYON_BrepTrim* trim0 = Trim(trim0_index[eti]);
    if ( !trim0 )
      break;
    const MYON_BrepTrim* trim1 = Trim(trim1_index[eti]);
    if ( !trim1 )
      break;
    MYON_NurbsCurve* c0 = trim0->NurbsCurve();
    if ( !c0 )
      break;
    MYON_NurbsCurve* c1 = trim1->NurbsCurve();
    if ( !c1 )
    {
      delete c0;
      break;
    }
    if ( trim0->m_vi[1] == vi_mid && trim1->m_vi[0] == vi_mid )
    {
      if ( !c0->Append(*c1) )
      {
        delete c0;
        delete c1;
        break;
      }
      delete c1;
      c1 = 0;
      tc.Append(c0);
    }
    else if ( trim0->m_vi[0] == vi_mid && trim1->m_vi[1] == vi_mid )
    {
      if ( !c1->Append(*c0) )
      {
        delete c0;
        delete c1;
        break;
      }
      delete c0;
      c0 = c1;
      c1 = 0;
      tc.Append(c0);
    }
  }

  if ( eti < trim_count )
  {
    delete ec;
    for ( eti = 0; eti < tc.Count(); eti++ )
      delete tc[eti];
    return 0;
  }

  // Add new edge from vi0 to vi1 that has the same orientation
  // as edge0.  Adding the new edge may change pointer values,
  // so the edge0 and edge1 pointers are reset.
  edge0 = 0;
  edge1 = 0;

  const int c3i = AddEdgeCurve(ec);
  MYON_BrepEdge& edge = NewEdge( m_V[vi0], m_V[vi1], c3i );
  edge0 = Edge(ei0);
  edge1 = Edge(ei1);

	// Set edge tolerance
	if(edge0->m_tolerance<0 || edge1->m_tolerance<0)
		edge.m_tolerance= MYON_UNSET_VALUE;
	else if ( edge0->m_tolerance> edge1->m_tolerance)
		edge.m_tolerance= edge0->m_tolerance;
	else 	
		edge.m_tolerance= edge1->m_tolerance;
		

  // dynamic m_T[] is grown to full size here.
  // Trim refs are good after NewTrim()
  m_T.Reserve( m_T.Count() + trim_count );
  for ( eti = 0; eti < trim_count; eti++ )
  {
    int c2i = AddTrimCurve( tc[eti] );
    MYON_BrepTrim& trim0 = m_T[trim0_index[eti]];
    MYON_BrepTrim& trim1 = m_T[trim1_index[eti]];
    MYON_BrepTrim& trim = NewTrim( edge, trim0.m_bRev3d, c2i );
		// Set trim tolerance
		for(int i=0; i<2; i++){
			if( trim0.m_tolerance[i]<0 || trim1.m_tolerance[i]<0)
				trim.m_tolerance[i] = MYON_UNSET_VALUE;
			else if(trim0.m_tolerance[i]>trim1.m_tolerance[i])
				trim.m_tolerance[i] = trim0.m_tolerance[i];
			else
				trim.m_tolerance[i] = trim1.m_tolerance[i];
		}
    trim.m_li = trim0.m_li;
    MYON_BrepLoop& loop = m_L[trim.m_li];
    loop.m_ti[loop_lti0[eti]] = trim.m_trim_index;
    loop.m_ti.Remove( loop_lti1[eti] );

		//GBA 1/29/03 Fixes TRR#9233.  Removing an item from loop.m_ti
		//will cause loop indicies stored in loop_lti0[] and loop_lti1[]
		//to be wrong. So they must be reindexed
		int ri = loop_lti1[eti];			// removed index
    int li = loop.m_loop_index;
		for(int ii=0; ii<trim_count; ii++){
      if(loop_lti0[ii]>ri && m_T[trim0_index[ii]].m_li == li) 
        loop_lti0[ii]--;
			if(loop_lti1[ii]>ri && m_T[trim1_index[ii]].m_li == li) 
        loop_lti1[ii]--;
		}
		
		    
		trim.m_type = trim0.m_type;
		trim.m_iso = MYON_Surface::not_iso;
		if( trim0.m_iso==trim1.m_iso) 
			trim.m_iso = trim0.m_iso;
    trim0.m_li = -1;
    trim1.m_li = -1;
  }

  // delete old edges
  DeleteEdge(m_E[ei0],true);
  DeleteEdge(m_E[ei1],true);

  return &m_E[edge.m_edge_index];
}


bool MYON_Brep::CombineCoincidentEdges(MYON_BrepEdge& edge0, MYON_BrepEdge& edge1)
{
  bool rc = false;
  if ( edge0.m_edge_index == edge1.m_edge_index ) 
  {
    MYON_ERROR("MYON_Brep::CombineCoincidentEdges - edge0 = edge1.");
    return rc;
  }
  int ti, eti, etcnt;
  if (    edge0.m_edge_index >= 0 
       && edge1.m_edge_index >= 0 
       && edge0.m_edge_index != edge1.m_edge_index
       && edge0.m_vi[0] == edge1.m_vi[0] 
       && edge0.m_vi[1] == edge1.m_vi[1] ) 
  {
    bool bIsGoodIso0 = false;
    if (edge0.m_tolerance == 0.0){
      for (eti=0; eti<edge0.m_ti.Count(); eti++){
        const MYON_BrepTrim& T = m_T[edge0.m_ti[eti]];
        if (T.m_iso)
        {
          bIsGoodIso0 = true;
          break;
        }
      }
    }
    bool bIsGoodIso1 = false;
    if (edge1.m_tolerance == 0.0){
      for (eti=0; eti<edge1.m_ti.Count(); eti++){
        const MYON_BrepTrim& T = m_T[edge1.m_ti[eti]];
        if (T.m_iso)
        {
          bIsGoodIso1 = true;
          break;
        }
      }
    }
    bool bKeep0 = (edge0.m_tolerance <= edge1.m_tolerance) ? true : false;
    if (edge0.m_tolerance == edge1.m_tolerance && edge0.m_tolerance == 0.0){
      if (bIsGoodIso1){
        if (!bIsGoodIso0)
          bKeep0 = false;
        else {//both are good.  Take the one with the lowest degree.
          if (edge1.Degree() < edge0.Degree())
            bKeep0 = false;
          else if (edge1.Degree() == edge0.Degree() && edge1.SpanCount() < edge0.SpanCount())
            bKeep0 = false;
        }
      }
    }

    MYON_BrepEdge& EKeep = (bKeep0) ? edge0 : edge1;
    MYON_BrepEdge& EToss = (bKeep0) ? edge1 : edge0;

    /*
    rc = true;
    etcnt = edge1.m_ti.Count();
    int tcount = m_T.Count();
    for ( eti = 0; eti < etcnt; eti++ ) {
      ti = edge1.m_ti[eti];
      if ( ti >= 0 && ti < tcount ) {
        MYON_BrepTrim& trim = m_T[ti];
        trim.m_ei = edge0.m_edge_index;
        edge0.m_ti.Append(ti);
        // TODO - tolerances ?

        //set edge tolerance
        if (edge0.m_tolerance != MYON_UNSET_VALUE && edge1.m_tolerance != MYON_UNSET_VALUE)
          SetEdgeTolerance(edge0, false); 
        else edge0.m_tolerance = MYON_UNSET_VALUE;
      }
    }
    edge1.m_ti.Destroy();
    DeleteEdge( edge1, false );

    etcnt =  edge0.m_ti.Count();
    if ( etcnt >= 2 ) for ( eti = 0; eti < etcnt; eti++ )
    {
      ti = edge0.m_ti[eti];
      if ( ti >= 0 && ti < tcount ) 
      {
        MYON_BrepTrim& trim = m_T[ti];
        if ( trim.m_type == MYON_BrepTrim::boundary )
          trim.m_type = MYON_BrepTrim::mated;
      }
    }
    */
    rc = true;
    etcnt = EToss.m_ti.Count();
    int tcount = m_T.Count();
    for ( eti = 0; eti < etcnt; eti++ ) {
      ti = EToss.m_ti[eti];
      if ( ti >= 0 && ti < tcount ) {
        MYON_BrepTrim& trim = m_T[ti];
        trim.m_ei = EKeep.m_edge_index;
        EKeep.m_ti.Append(ti);
        trim.UnsetPlineEdgeParameters();  
        // TODO - tolerances ?

        //set edge tolerance
        if (EKeep.m_tolerance != MYON_UNSET_VALUE && EToss.m_tolerance != MYON_UNSET_VALUE)
          SetEdgeTolerance(EKeep, false); 
        else EKeep.m_tolerance = MYON_UNSET_VALUE;
      }
    }
    EToss.m_ti.Destroy();
    DeleteEdge( EToss, false );

    etcnt =  EKeep.m_ti.Count();
    if ( etcnt >= 2 ) for ( eti = 0; eti < etcnt; eti++ )
    {
      ti = EKeep.m_ti[eti];
      if ( ti >= 0 && ti < tcount ) 
      {
        MYON_BrepTrim& trim = m_T[ti];
        if ( trim.m_type == MYON_BrepTrim::boundary )
          trim.m_type = MYON_BrepTrim::mated;
      }
    }
  }
  return rc;
}

bool MYON_Brep::DisconnectEdgeFaces(int eid)

{
  if (eid < 0 || eid > m_E.Count())
    return false;
  MYON_BrepEdge& E = m_E[eid];
  if (E.m_edge_index < 0 || E.m_ti.Count() < 1)
    return false;
  int c3 = E.m_c3i;
  MYON_Interval cdom = E.ProxyCurveDomain();
  MYON_3dPoint VP[2];
  bool bClosed = E.m_vi[0] == E.m_vi[1];
  MYON_BrepVertex* pEV0 = E.Vertex(0);
  if (!pEV0)
    return false;
  VP[0] = pEV0->Point();
  MYON_BrepVertex* pEV1 = 0;
  if (!bClosed){
    pEV1 = E.Vertex(1);
    if (!pEV1)
      return false;
    VP[1] = pEV1->Point();
  }
  else {
    pEV1 = pEV0;
    VP[1] = VP[0];
  }
  int evid[2];
  evid[0] = E.m_vi[0];
  evid[1] = E.m_vi[1];
  int tc = E.m_ti.Count();
  MYON_SimpleArray<MYON_2dex> tids(tc);
  MYON_SimpleArray<bool> bUsed(tc);
  int i;
  for (i=0; i<tc; i++)
    bUsed.Append(false);
  //Separate trims into singles or pairs from the same face. 
  for (i=0; i<tc; i++){
    if (bUsed[i])
      continue;
    MYON_2dex& dex = tids.AppendNew();
    dex.i = E.m_ti[i];
    dex.j = -1;
    const MYON_BrepTrim* pTi = E.Trim(i);
    if (!pTi)
      return false;
    int fi = pTi->FaceIndexOf();
    if (fi < 0)
      return false;
    int j;
    for (j=i+1; j<tc; j++){
      if (bUsed[j])
        continue;
      const MYON_BrepTrim* pTj = E.Trim(j);
      if (!pTj)
        return false;
      if (pTj->FaceIndexOf() == fi){
        bUsed[j] = true;
        if (dex.j == -1)
          dex.j = E.m_ti[j];
        else // 3 trims from the same face.  Not good.
          return false;
      }
    }
  }
  if (tids.Count() < 2)
    return false;
  //Disconnect trims not part of tids[0] from E
  E.m_ti.Empty();
  E.m_ti.Append(tids[0].i);
  if (tids[0].j >= 0)
    E.m_ti.Append(tids[0].i);
  /*
  int nvc = tids.Count()-1;
  if (!bClosed)
    nvc *= 2;
  m_V.Reserve(m_V.Count() + nvc);
  */
  //E will no longer be a valid reference. Use the info saved above.
  for (i=1; i<tids.Count(); i++){//Make a new edge for each member of tids.
    //New vertices
    //MYON_BrepVertex& V0 = NewVertex(VP[0]);
    //MYON_BrepVertex& V1 = (bClosed) ? V0 : NewVertex(VP[1]);
    MYON_BrepVertex& V0 = m_V[evid[0]];
    MYON_BrepVertex& V1 = m_V[evid[1]];
    //New edge
    MYON_BrepEdge& NE = NewEdge(V0, V1, c3, &cdom);
    NE.m_ti.Append(tids[i].i);
    MYON_BrepTrim& Ti = m_T[tids[i].i];
    Ti.m_ei = NE.m_edge_index;
    Ti.m_vi[0] = (Ti.m_bRev3d) ? NE.m_vi[1] : NE.m_vi[0];
    Ti.m_vi[1] = (Ti.m_bRev3d) ? NE.m_vi[0] : NE.m_vi[1];
    int ptid = PrevTrim(tids[i].i);
    if (ptid != tids[i].i)
      m_T[ptid].m_vi[1] = Ti.m_vi[0];
    int ntid = NextTrim(tids[i].i);
    if (ntid != tids[i].i)
      m_T[ntid].m_vi[0] = Ti.m_vi[1];
    if (tids[i].j >= 0){
      NE.m_ti.Append(tids[i].j);
      MYON_BrepTrim& Tj = m_T[tids[i].j];
      Tj.m_ei = NE.m_edge_index;
      Tj.m_vi[0] = (Tj.m_bRev3d) ? NE.m_vi[1] : NE.m_vi[0];
      Tj.m_vi[1] = (Tj.m_bRev3d) ? NE.m_vi[0] : NE.m_vi[1];
      ptid = PrevTrim(tids[i].j);
      if (ptid != tids[i].j)
        m_T[ptid].m_vi[1] = Tj.m_vi[0];
      ntid = NextTrim(tids[i].j);
      if (ntid != tids[i].j)
        m_T[ntid].m_vi[0] = Tj.m_vi[1];
    }
  }
  return true;
}


bool MYON_Brep::Create( MYON_Surface*& pS )
{
  bool rc = false;
  Destroy();
  MYON_Surface* p = pS;
  if (p)
  {
    int vid[4] = {-1,-1,-1,-1};
    int eid[4] = {-1,-1,-1,-1};
    bool bRev3d[4] = {0,0,0,0};
    MYON_BrepFace* face = NewFace(p,vid,eid,bRev3d);
    if ( face )
    {
      rc = true;
      pS = 0;
    }
  }
  return rc;
}

/*
bool MYON_Brep::FaceTool( MYON_Surface* pS )
{
  // private face adding tool
  if (!pS) 
    return false;
  double u[2], v[2];
  if (!pS->GetDomain(0, &u[0], &u[1])) 
      return false;
  if (!pS->GetDomain(1, &v[0], &v[1])) 
      return false;

  MYON_3dPoint srf_P[2][2];
  if ( !pS->EvPoint(u[0],v[0],srf_P[0][0] )
    return false;
  if ( !pS->EvPoint(u[1],v[0],srf_P[1][0] )
    return false;
  if ( !pS->EvPoint(u[0],v[1],srf_P[0][1] )
    return false;
  if ( !pS->EvPoint(u[1],v[1],srf_P[1][1] )
    return false;

  int sw_vi, se_vi, ne_vi, nw_vi;

  m_F.Reserve( m_F.Count() + 1 );
  m_T.Reserve( m_T.Count() + 4 );
  m_L.Reserve( m_L.Count() + 1 );
  m_V.Reserve( m_V.Count() + 4 );
  m_E.Reserve( m_E.Count() + 4 );
  m_S.Reserve( m_S.Count() + 1 );
  m_C2.Reserve( m_C2.Count() + 1 );
  m_C3.Reserve( m_C3.Count() + 1 );

  sw_vi = NewVertex( srf_P[0][0], 0.0 ).m_vertex_index;

  bool bIsClosed[2];
  bIsClosed[0] = pS->IsClosed(0);
  bIsClosed[1] = pS->IsClosed(1);

  bool bIsSingular[4];
  bIsSingular[0] = pS->IsSingular(0);
  bIsSingular[1] = pS->IsSingular(1);
  bIsSingular[2] = pS->IsSingular(2);
  bIsSingular[3] = pS->IsSingular(3);


  if (bIsSingular[0] || bIsClosed[0]) 
    se_vi = sw_vi;
  else 
    se_vi = NewVertex( srf_P[1][0], 0.0 ).m_vertex_index;

  if (bIsSingular[1] || bIsClosed[1]) 
    ne_vi = se_vi;
  else 
    ne_vi = NewVertex( srf_P[1][1], 0.0 ).m_vertex_index;

  if (bIsSingular[2] || bIsClosed[0]) 
    nw_vi = ne_vi;
  else if (bIsSingular[3] || bIsClosed[1]) 
    nw_vi = sw_vi;
  else 
    nw_vi = NewVertex( srf_P[0][1], 0.0 ).m_vertex_index;

  MYON_BrepVertex& sw_vertex = m_V[sw_vi];
  MYON_BrepVertex& se_vertex = m_V[se_vi];
  MYON_BrepVertex& ne_vertex = m_V[ne_vi];
  MYON_BrepVertex& nw_vertex = m_V[nw_vi];

  MYON_BrepFace& face = NewFace(AddSurface(pS));
  MYON_BrepLoop& loop = NewLoop(MYON_BrepLoop::outer, face);

  loop.m_pbox.m_min.x = u[0];
  loop.m_pbox.m_min.y = v[0];
  loop.m_pbox.m_min.z = 0.0;

  loop.m_pbox.m_max.x = u[1];
  loop.m_pbox.m_max.y = v[1];
  loop.m_pbox.m_max.z = 0.0;

  int id3[4] = {-1,-1,-1,-1};
  int eid[4] = {-1,-1,-1,-1};
  int c2i;

  MYON_2dPoint sw_corner(u[0],v[0]);
  MYON_2dPoint se_corner(u[1],v[0]);
  MYON_2dPoint ne_corner(u[1],v[1]);
  MYON_2dPoint nw_corner(u[0],v[1]);

  {//south side
    c2i = AddTrimCurve(new MYON_LineCurve(sw_corner,se_corner));
    if (bIsSingular[0]) {
      NewSingularTrim(sw_vertex,loop,MYON_Surface::S_iso,c2i);
    }
    else {
      id3[0] = AddEdgeCurve( pS->IsoCurve(0, v[0]) );
      MYON_BrepEdge& edge = NewEdge(sw_vertex, se_vertex, id3[0]);
      edge.m_tolerance = 0.0;
      eid[0] = edge.m_edge_index;
      MYON_BrepTrim& trim = NewTrim(edge, false, loop, c2i);
      trim.m_iso = MYON_Surface::S_iso;
      if (bIsClosed[1]) 
        trim.m_type = MYON_BrepTrim::seam;
      else 
        trim.m_type = MYON_BrepTrim::boundary;
    }
  }

  { //east side
    c2i = AddTrimCurve(new MYON_LineCurve(se_corner,ne_corner));
    if (bIsSingular[1]) {
      NewSingularTrim(se_vertex,loop,MYON_Surface::E_iso,c2i);
    }
    else {
      id3[1] = AddEdgeCurve(pS->IsoCurve(1, u[1]));
      MYON_BrepEdge& edge = NewEdge(se_vertex, ne_vertex, id3[1]);
      edge.m_tolerance = 0.0;
      eid[1] = edge.m_edge_index;
      MYON_BrepTrim& trim = NewTrim(edge, false, loop, c2i);
      trim.m_iso = MYON_Surface::E_iso;
      if (bIsClosed[0]) 
        trim.m_type = MYON_BrepTrim::seam;
      else 
        trim.m_type = MYON_BrepTrim::boundary;
    }
  }

  { //north side
    c2i = AddTrimCurve(new MYON_LineCurve(ne_corner,nw_corner));
    bool rev = false;
    if (bIsSingular[2]) {
      NewSingularTrim(ne_vertex,loop,MYON_Surface::N_iso,c2i);
    }
    else{
      if (bIsClosed[1]) {
        id3[2] = id3[0];
        eid[2] = eid[0];
        rev = true;
      }
      else {
        MYON_Curve* pC3 = pS->IsoCurve(0, v[1]);
        if (pC3) pC3->Reverse();
        id3[2] = AddEdgeCurve(pC3);
        MYON_BrepEdge& edge = NewEdge(ne_vertex, nw_vertex, id3[2]);
        edge.m_tolerance = 0.0;
        eid[2] = edge.m_edge_index;
      }
      MYON_BrepTrim& trim = NewTrim(m_E[eid[2]], rev, loop, c2i);
      trim.m_iso = MYON_Surface::N_iso;
      if (bIsClosed[1]) 
        trim.m_type = MYON_BrepTrim::seam;
      else 
        trim.m_type = MYON_BrepTrim::boundary;
    }
  }

  { //west side
    c2i = AddTrimCurve(new MYON_LineCurve(nw_corner,sw_corner));
    bool rev = false;
    if (bIsSingular[3]){
      NewSingularTrim(nw_vertex,loop,MYON_Surface::W_iso,c2i);
    }
    else {
      if (bIsClosed[0]){
      id3[3] = id3[1];
        eid[3] = eid[1];
        rev = true;
      }
      else {
        MYON_Curve* pC3 = pS->IsoCurve(1, u[0]);
        if (pC3) pC3->Reverse();
        id3[3] = AddEdgeCurve(pC3);
        MYON_BrepEdge& edge = NewEdge(nw_vertex, sw_vertex, id3[3]);
        edge.m_tolerance = 0.0;
        eid[3] = edge.m_edge_index;
      }
      MYON_BrepTrim& trim = NewTrim( m_E[eid[3]], rev, loop, c2i );
      trim.m_iso = MYON_Surface::W_iso;
      if (bIsClosed[0]) 
        trim.m_type = MYON_BrepTrim::seam;
      else 
        trim.m_type = MYON_BrepTrim::boundary;
    }
  }

  for ( int lti = 0; lti < 4; lti++ )
  {
    ti = loop.m_ti[lti];
    MYON_BrepTrim& trim = m_T[ti];
    trim.m_tolerance[0] = 0.0;
    trim.m_tolerance[1] = 0.0;
    trim.m__legacy_2d_tol = 0.0;
    trim.m__legacy_3d_tol = 0.0;
    trim.m__legacy_flags_Set(-1,1);
  }

  return true;
}
*/

bool MYON_Brep::Create( MYON_NurbsSurface*& pNurbsSurface )
{
  MYON_Surface* pSurface = pNurbsSurface;
  bool rc = Create(pSurface);
  if ( !pSurface )
    pNurbsSurface = 0;
  return rc;
}

bool MYON_Brep::Create( MYON_PlaneSurface*& pPlaneSurface )
{
  MYON_Surface* pSurface = pPlaneSurface;
  bool rc = Create(pSurface);
  if ( !pSurface )
    pPlaneSurface = 0;
  return rc;
}

bool MYON_Brep::Create( MYON_RevSurface*& pRevSurface )
{
  MYON_Surface* pSurface = pRevSurface;
  bool rc = Create(pSurface);
  if ( !pSurface )
    pRevSurface = 0;
  return rc;
}

bool MYON_Brep::Create( MYON_SumSurface*& pSumSurface )
{
  MYON_Surface* pSurface = pSumSurface;
  bool rc = Create(pSurface);
  if ( !pSurface )
    pSumSurface = 0;
  return rc;
}


bool MYON_Brep::HasBrepForm() const
{
  return true;
}

MYON_Brep* MYON_Brep::BrepForm( MYON_Brep* brep ) const
{
  if ( brep )
  {
    if ( brep != this )
    {
      *brep = *this;      
      brep->DestroyMesh(MYON::any_mesh);
    }
  }
  else
  {
    brep = new MYON_Brep(*this);
    brep->DestroyMesh(MYON::any_mesh);
  }
  return brep;
}

void MYON_Brep::Clear_vertex_user_i() const
{
  int vi;
  int vertex_count = m_V.Count();
  for ( vi = 0; vi < vertex_count; vi++ ) 
  {
    memset(&m_V[vi].m_vertex_user,0,sizeof(MYON_U));
  }
}

void MYON_Brep::Clear_edge_user_i() const
{
  int ei;
  int edge_count = m_E.Count();
  for ( ei = 0; ei < edge_count; ei++ ) 
  {
    memset(&m_E[ei].m_edge_user,0,sizeof(MYON_U));
  }
}

void MYON_Brep::Clear_edge_user_i(int i) const
{
  int ei;
  int edge_count = m_E.Count();
  for ( ei = 0; ei < edge_count; ei++ ) 
  {
    memset(&m_E[ei].m_edge_user,0,sizeof(MYON_U));
    m_E[ei].m_edge_user.i = i;
  }
}

void MYON_Brep::Clear_trim_user_i() const
{
  int ti;
  int trim_count = m_T.Count();
  for ( ti = 0; ti < trim_count; ti++ ) {
    memset(&m_T[ti].m_trim_user,0,sizeof(MYON_U));
  }
}

void MYON_Brep::Clear_loop_user_i() const
{
  int li;
  int loop_count = m_L.Count();
  for ( li = 0; li < loop_count; li++ ) {
    memset(&m_L[li].m_loop_user,0,sizeof(MYON_U));
  }
}

void MYON_Brep::Clear_face_user_i() const
{
  int fi;
  int face_count = m_F.Count();
  for ( fi = 0; fi < face_count; fi++ ) {
    memset(&m_F[fi].m_face_user,0,sizeof(MYON_U));
  }
}

void MYON_Brep::Clear_user_i() const
{
  memset(&m_brep_user,0,sizeof(m_brep_user));
  Clear_vertex_user_i();
  Clear_edge_user_i();
  Clear_trim_user_i();
  Clear_loop_user_i();
  Clear_face_user_i();
}

void MYON_Brep::Set_user(MYON_U u) const
{
  int i, count;
  m_brep_user=u;
  
  count = m_V.Count();
  const MYON_BrepVertex* V = m_V.Array();
  for ( i = 0; i < count; i++ )
  {
    V[i].m_vertex_user = u;
  }
  
  count = m_E.Count();
  const MYON_BrepEdge* E = m_E.Array();
  for ( i = 0; i < count; i++ )
  {
    E[i].m_edge_user = u;
  }

  
  count = m_T.Count();
  const MYON_BrepTrim* T = m_T.Array();
  for ( i = 0; i < count; i++ )
  {
    T[i].m_trim_user = u;
  }
  
  count = m_L.Count();
  const MYON_BrepLoop* L = m_L.Array();
  for ( i = 0; i < count; i++ )
  {
    L[i].m_loop_user = u;
  }
  
  count = m_F.Count();
  const MYON_BrepFace* F = m_F.Array();
  for ( i = 0; i < count; i++ )
  {
    F[i].m_face_user = u;
  }
}



MYON_BrepVertex& MYON_Brep::NewPointOnFace( 
  MYON_BrepFace& face,
  double s,
  double t
  )
{
  MYON_3dPoint point = face.PointAt(s,t);
  
  MYON_BrepVertex& vertex = NewVertex( point );
  MYON_BrepLoop& loop = NewLoop( MYON_BrepLoop::ptonsrf, face );
  MYON_BrepTrim& trim = NewTrim(false, loop, -1);

  vertex.m_tolerance = 0.0;
  trim.m_type = MYON_BrepTrim::ptonsrf;
  trim.m_pbox.m_min.Set(s,t,0.0);
  trim.m_pbox.m_max.Set(s,t,0.0);
  trim.m_tolerance[0] = 0.0;
  trim.m_tolerance[1] = 0.0;
  loop.m_pbox = trim.m_pbox;
  trim.m_vi[0] = trim.m_vi[1] = vertex.m_vertex_index;

  return vertex;
}


MYON_BrepTrim& MYON_Brep::NewCurveOnFace( MYON_BrepFace& face, MYON_BrepEdge& edge, bool bRev3d, int c2i )
{
  MYON_BrepLoop& loop = NewLoop( MYON_BrepLoop::crvonsrf, face );
  MYON_BrepTrim& trim = NewTrim( edge, bRev3d, loop, c2i );
  trim.m_type = MYON_BrepTrim::crvonsrf;
  const MYON_Curve* trimcurve = trim.TrimCurveOf();
  if (trimcurve)
  {
    trimcurve->GetBoundingBox( trim.m_pbox );
    loop.m_pbox = trim.m_pbox;
  }
  return trim;
}

//For each i, let ti be the parameter along the chord (Points[0], Points[last])
//of the closest point to Points[i], and let di be the distance to the chord.
//Transform Points so that Points[0] = P0, Points[last] = P1, 
//and the new ti and di remain the same.  Don't do anything if the chord is short 
//relative to the cummulative dist between consecutive points on input.

static bool AdjustPointListAlongChord(MYON_3dPointArray& Points, 
                                      const MYON_3dPoint& P0, 
                                      const MYON_3dPoint& P1)

{
  int count = Points.Count();
  if (count < 2)
    return false;

  MYON_3dPoint A0 = Points[0];
  MYON_3dPoint A1 = Points[count-1];
  double chord_dist = A0.DistanceTo(A1);
  if (chord_dist < MYON_SQRT_EPSILON)
    return false;
  double cum_dist = 0.0;
  int i; 
  for (i=1; i<count; i++)
    cum_dist += Points[i-1].DistanceTo(Points[i]);
  if (chord_dist < 0.01*cum_dist)
    return false;
  MYON_3dVector V0 = P0-A0;
  MYON_3dVector V1 = P1-A1;
  MYON_Line Aline(A0, A1);
  Points[0] = P0;
  Points[count-1] = P1;
  for (i=1; i<count-1; i++){
    double t;
    Aline.ClosestPointTo(Points[i], &t);
    Points[i] = Points[i] + (1.0-t)*V0 + t*V1;
  }
  return true;
}

static void AdjustNurbsCurve(MYON_NurbsCurve& crv,
                             const MYON_3dPoint& P0, 
                             const MYON_3dPoint& P1)

{
  if (crv.Dimension() > 3)
    return;

  crv.ClampEnd(2);
  int cvc = crv.CVCount();
  MYON_3dPointArray Points(cvc);
  int i;
  for (i=0; i<cvc; i++)
    crv.GetCV(i, Points.AppendNew());

  if (!AdjustPointListAlongChord(Points, P0, P1)){
    crv.SetStartPoint(P0);
    crv.SetEndPoint(P1);
    return;
  }

  bool rat = crv.IsRational();
  for (i=0; i<cvc; i++){
    double w = 1.0;
    if (rat){
      w = crv.Weight(i);
      Points[i] *= w;
    }
    crv.SetCV(i, Points[i]);
    if (rat)
      crv.SetWeight(i, w);
  }
  
  return;
}

static void AdjustPolylineCurve(MYON_PolylineCurve& crv,
                             const MYON_3dPoint& P0, 
                             const MYON_3dPoint& P1)

{
  AdjustPointListAlongChord(crv.m_pline, P0, P1);
  crv.SetStartPoint(P0);
  crv.SetEndPoint(P1);
  return;
}

static bool AdjustCurve(MYON_Curve& crv, 
                        const MYON_3dPoint& P0,
                        const MYON_3dPoint& P1);

static bool AdjustPolyCurve(MYON_PolyCurve& crv,
                             const MYON_3dPoint& P0, 
                             const MYON_3dPoint& P1)

{
  if (crv.Count() == 1){
    MYON_Curve* pSeg = crv.SegmentCurve(0);
    if (!pSeg)
      return false;
    return AdjustCurve(*pSeg, P0, P1);
  }

  MYON_3dPointArray Points(crv.Count() + 1);
  Points.Append(crv.PointAtStart());

  int i;
  for (i=0; i<crv.Count(); i++)
    Points.Append(crv.SegmentCurve(i)->PointAtEnd());

  if (!AdjustPointListAlongChord(Points, P0, P1)){
    return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
  }

  bool rc = false;
  for (i=0; i<crv.Count(); i++){
    MYON_Curve* pSeg = crv.SegmentCurve(i);
    if (!pSeg)
      return rc;
    if (AdjustCurve(*pSeg, Points[i], Points[i+1]))
      rc = true;
    Points[i+1] = pSeg->PointAtEnd();
  }

  return rc;
}

//Afterwards it is up to caller to check to see if the endpoints are where they should be.
static bool AdjustCurve(MYON_Curve& crv, 
                        const MYON_3dPoint& P0,
                        const MYON_3dPoint& P1)

{
  MYON_LineCurve* lc = MYON_LineCurve::Cast(&crv);
  if (lc){
    lc->SetStartPoint(P0);
    lc->SetEndPoint(P1);
    return true;
  }

  MYON_CurveProxy* pc = MYON_CurveProxy::Cast(&crv);
  if (pc)
    return false;

  if (crv.IsClosed()){
    if (P0 != P1)
      return false;
    MYON_3dPoint P = crv.PointAtStart();
    MYON_3dVector TVec = P0-P;
    if (TVec.Length() > MYON_SQRT_EPSILON){
      MYON_Xform T(MYON_Xform::TranslationTransformation(TVec));
      crv.Transform(T);
    }
    else
      return false;
    return true;
  }

  MYON_PolylineCurve* plc = MYON_PolylineCurve::Cast(&crv);
  if (plc) {
    AdjustPolylineCurve(*plc, P0, P1);
    return true;
  }

  MYON_NurbsCurve* nc = MYON_NurbsCurve::Cast(&crv);
  if (nc){
    AdjustNurbsCurve(*nc, P0, P1);
    return true;
  }

  MYON_PolyCurve* plyc = MYON_PolyCurve::Cast(&crv);
  if (plyc){
    return AdjustPolyCurve(*plyc, P0, P1);
  }

  MYON_3dPoint A0 = crv.PointAtStart();
  MYON_3dPoint A1 = crv.PointAtEnd();

  if (A0.DistanceTo(P0) < MYON_SQRT_EPSILON && A1.DistanceTo(P1) < MYON_SQRT_EPSILON){
    return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
  }

  double alen = A0.DistanceTo(A1);
  double plen = P0.DistanceTo(P1);
  if (alen < 0.1*plen || plen < 0.1*alen){
    return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
  }

  MYON_3dPoint Ac = 0.5*(A0+A1);
  MYON_3dPoint Pc = 0.5*(P0+P1);
  MYON_3dVector TVec = Pc-Ac;
  if (TVec.Length() > MYON_SQRT_EPSILON){
    const MYON_Xform T(MYON_Xform::TranslationTransformation(TVec));
    crv.Transform(T);
  }

  A0 = crv.PointAtStart();
  A1 = crv.PointAtEnd();
  if (A0.DistanceTo(P0) < MYON_SQRT_EPSILON && A1.DistanceTo(P1) < MYON_SQRT_EPSILON){
    return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
  }

  if (fabs(plen - alen) > MYON_SQRT_EPSILON){
    double scale = plen/alen;
    Ac = 0.5*(A0+A1);
    MYON_Xform T(MYON_Xform::ScaleTransformation(Ac, scale));
    crv.Transform(T);
  }

  A0 = crv.PointAtStart();
  A1 = crv.PointAtEnd();
  if (A0.DistanceTo(P0) < MYON_SQRT_EPSILON && A1.DistanceTo(P1) < MYON_SQRT_EPSILON){
    return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
  }

  if (plen < MYON_SQRT_EPSILON){
    return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
  }

  MYON_3dPoint C = 0.5*(Pc+Ac);
  MYON_3dVector VA = A0-C;
  VA.Unitize();
  MYON_3dVector VP = P0-C;
  VP.Unitize();

  MYON_3dVector Axis = MYON_CrossProduct(VA, VP);
  double sina = Axis.Length();
  if (sina < MYON_SQRT_EPSILON){
    return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
  }
  Axis.Unitize();
  double cosa = VA*VP;

  MYON_Xform T;
  T.Rotation(sina, cosa, Axis, C);
  crv.Transform(T);
  return (crv.SetStartPoint(P0) || crv.SetEndPoint(P1)) ? true : false;
}

static void AdjustEdgeEnds(MYON_BrepEdge& edge)

{
  MYON_Brep* pB = edge.Brep();
  if (!pB)
    return;
  MYON_Curve* c3 = const_cast<MYON_Curve*>(edge.EdgeCurveOf());
  if( c3 )
  {
    MYON_3dPoint A0 = c3->PointAtStart();
    MYON_3dPoint P0 = A0;
    if (edge.m_vi[0] >= 0){
      MYON_BrepVertex& V = pB->m_V[edge.m_vi[0]];
      if (V.IsValid())
        P0 = V.Point();
    }
    MYON_3dPoint A1 = c3->PointAtEnd();
    MYON_3dPoint P1 = A1;
    if (edge.m_vi[1] >= 0){
      MYON_BrepVertex& V = pB->m_V[edge.m_vi[1]];
      if (V.IsValid())
        P1 = V.Point();
    }

    bool bQuit = true;
    if (P0 != A0 && edge.m_vi[0] >= 0){
      MYON_BrepVertex& V = pB->m_V[edge.m_vi[0]];
      V.m_tolerance = MYON_UNSET_VALUE;
      bQuit = false;
    }
    if (P1 != A1 && edge.m_vi[1] >= 0){
      MYON_BrepVertex& V = pB->m_V[edge.m_vi[1]];
      V.m_tolerance = MYON_UNSET_VALUE;
      bQuit = false;
    }
    if (bQuit)
      return;

    if (AdjustCurve(*c3, P0, P1))
      edge.m_tolerance = MYON_UNSET_VALUE;
  }
  return;
}

bool MYON_Brep::StandardizeEdgeCurve( int edge_index, bool bAdjustEnds )
{
  return StandardizeEdgeCurve(edge_index, bAdjustEnds, 0);
}

bool MYON_Brep::StandardizeEdgeCurve( int edge_index, bool bAdjustEnds, int EdgeCurveUse )
{
  bool rc = false;
  MYON_BrepEdge* edge = Edge(edge_index);
  if ( 0 != edge && edge->m_edge_index >= 0 )
  {
    edge_index = edge->m_edge_index;
    const MYON_Curve* c3 = edge->EdgeCurveOf();
    if( c3 )
    {
      MYON_Interval c3dom = c3->Domain();
      MYON_Interval pdom = edge->ProxyCurveDomain();
      MYON_Interval edom = edge->Domain();
      bool bNewCurve = false;
      if ( edge->ProxyCurveIsReversed() )
        bNewCurve = true;
      else if ( c3dom != pdom )
        bNewCurve = true; // curve proxy is trimmed
      else if ( EdgeCurveUse > 1 || (EdgeCurveUse < 1 && EdgeCurveUseCount( edge->m_c3i,2 ) > 1 ))
        bNewCurve = true; // 2 or more edges use c3
      else if ( edom != c3dom )
      {
        // can fix this problem by changing c3 domain
        // and proxy settings
        if ( m_C3[edge->m_c3i]->SetDomain(edom) )
        {
          edge->SetProxyCurveDomain(edom);
          edge->SetDomain(edom);
          rc = true;
        }
        else
        {
          bNewCurve = true;
        }
      }
      else
        rc = true;
      
      if ( bNewCurve )
      {
        MYON_Curve* newc3 = c3->Duplicate();
        if ( !newc3 )
          return false;
        if ( !newc3->Trim(pdom) )
        {
          delete newc3;
          return false;
        }
        if ( edge->ProxyCurveIsReversed() )
        {
          if ( !newc3->Reverse() )
          {
            delete newc3;
            return false;
          }
        }
        newc3->SetDomain(edom);
        if ( newc3->Domain() != edom )
        {
          delete newc3;
          return false;
        }
        int c3i = AddEdgeCurve(newc3);
        edge->m_c3i = c3i;
        edge->SetProxyCurve(newc3);
      }
    }
  }
  if (rc && bAdjustEnds)
    AdjustEdgeEnds(*edge);
    /*
  {
    MYON_Curve* c3 = const_cast<MYON_Curve*>(edge->EdgeCurveOf());
    if( c3 )
    {
      if (edge->m_vi[0] >= 0)
      {
        const MYON_BrepVertex& V = m_V[edge->m_vi[0]];
        if (V.IsValid())
          c3->SetStartPoint(V.Point());
      }
      if (edge->m_vi[1] >= 0)
      {
        const MYON_BrepVertex& V = m_V[edge->m_vi[1]];
        if (V.IsValid())
          c3->SetEndPoint(V.Point());
      }
    }
  }
  */

  return rc;
} 

static int sort_ci(const MYON_BrepEdge* E0, const MYON_BrepEdge* E1)

{
  if (E0->m_c3i < E1->m_c3i)
    return -1;
  if (E0->m_c3i < E1->m_c3i)
    return 1;
  return 0;
}


void MYON_Brep::StandardizeEdgeCurves( bool bAdjustEnds)
{

  //The ends will not adjust properly unless 
  //all of the edge curves have been standardized first.
  //So call standardize on all edges without adjusting, then do the adjusting
  //chuck - 9/5/2006
  int ei, edge_count = m_E.Count();


  //chuck - 10/13/2008.  The edge curve use counter called in StandardizeEdgeCurves(int,bool)
  //searches through the entire edge array.  In huge breps, this takes a long time.
  int* index = (int*)onmalloc(edge_count*sizeof(int));
  m_E.Sort(MYON::sort_algorithm::quick_sort, index, sort_ci);

  for ( ei = 0; ei < edge_count; ei++ ){
    int ecc = (ei==edge_count-1 || m_E[index[ei+1]].m_c3i == m_E[index[ei]].m_c3i) ? 2 : 1;
    StandardizeEdgeCurve( index[ei], false, ecc);
  }

  onfree((void*)index);

  /*
  for ( ei = 0; ei < edge_count; ei++ )
  {
    StandardizeEdgeCurve( ei, false );
  }
  */

  if (bAdjustEnds){
    for ( ei = 0; ei < edge_count; ei++ )
      AdjustEdgeEnds(m_E[ei]);
    SetVertexTolerances(true);
    SetEdgeTolerances(true);
  }
}

bool MYON_Brep::StandardizeTrimCurve( int trim_index )
{
  bool rc = false;
  MYON_BrepTrim* trim = Trim(trim_index);
  if ( 0 != trim && trim->m_trim_index >= 0 )
  {
    trim_index = trim->m_trim_index;
    const MYON_Curve* c2 = trim->TrimCurveOf();
    if( c2 )
    {
      MYON_Interval c2dom = c2->Domain();
      MYON_Interval pdom = trim->ProxyCurveDomain();
      MYON_Interval tdom = trim->Domain();
      bool bNewCurve = false;
      if ( trim->ProxyCurveIsReversed() )
        bNewCurve = true;
      else if ( c2dom != pdom )
        bNewCurve = true; // curve proxy is trimmed
      else if ( TrimCurveUseCount( trim->m_c2i, 2 ) > 1 )
        bNewCurve = true; // 2 or more edges use c3
      else if ( tdom != c2dom )
      {
        // can fix this problem by changing c3 domain
        // and proxy settings
        if ( m_C2[trim->m_c2i]->SetDomain(tdom) )
        {
          trim->SetProxyCurveDomain(tdom);
          trim->SetDomain(tdom);
          rc = true;
        }
        else
        {
          bNewCurve = true;
        }
      }
      else
        rc = true;
      
      if ( bNewCurve )
      {
        MYON_Curve* newc2 = c2->Duplicate();
        if ( !newc2 )
          return false;
        if ( !newc2->Trim(pdom) )
        {
          delete newc2;
          return false;
        }
        if ( trim->ProxyCurveIsReversed() )
        {
          if ( !newc2->Reverse() )
          {
            delete newc2;
            return false;
          }
        }
        newc2->SetDomain(tdom);
        if ( newc2->Domain() != tdom )
        {
          delete newc2;
          return false;
        }
        int c2i = AddTrimCurve(newc2);
        trim->m_c2i = c2i;
        trim->SetProxyCurve(newc2);
        rc = true;
      }
    }
  }
  return rc;
}

void MYON_Brep::StandardizeTrimCurves()
{
  int ti, trim_count = m_T.Count();
  for ( ti = 0; ti < trim_count; ti++ )
  {
    StandardizeTrimCurve( ti );
  }
}

bool MYON_Brep::StandardizeFaceSurface( int face_index )
{
  bool rc = false;
  MYON_BrepFace* face = Face(face_index);
  if ( 0 != face && face->m_face_index >= 0 )
  {
    face_index = face->m_face_index;
    const MYON_Surface* srf = face->SurfaceOf();
    if ( srf )
    {
      //Feb 9 2013 - Chuck - Old code doesn't do anything if bRev is false
      /*
      if ( face->m_bRev )
      {
        if ( SurfaceUseCount( face->m_si, 2 ) >= 2 )
        {
          MYON_Surface* newsrf = srf->Duplicate();
          face->m_si = AddSurface(newsrf);
          face->SetProxySurface(m_S[face->m_si]);
          srf = newsrf;
        }
        rc = face->Transpose() ? true : false;
      }
      else
        rc = true;
        */
      if ( face->m_bRev )
        rc = face->Transpose() ? true : false; //Transpose does the SurfaceUseCount check
      else 
      {
        if ( SurfaceUseCount( face->m_si, 2 ) >= 2 )
        {
          MYON_Surface* newsrf = srf->Duplicate();
          face->m_si = AddSurface(newsrf);
          face->SetProxySurface(m_S[face->m_si]);
          srf = newsrf;
        }
        rc = true;
      }
    }
  }
  return rc;
}


void MYON_Brep::StandardizeFaceSurfaces()
{
  int fi, face_count = m_F.Count();
  for ( fi = 0; fi < face_count; fi++ )
  {
    StandardizeFaceSurface( fi );
  }
}

void MYON_Brep::Standardize()
{
  StandardizeFaceSurfaces();
  StandardizeEdgeCurves(true);
  StandardizeTrimCurves();
}



bool MYON_Brep::ShrinkSurface( MYON_BrepFace& face, int DisableMask )
{
  MYON_Surface* srf = const_cast<MYON_Surface*>(face.SurfaceOf());
  if ( !srf )
    return false;

  MYON_Interval srf_udom = srf->Domain(0);
  MYON_Interval srf_vdom = srf->Domain(1);

  int fli, li, si=-1;
  int lti, ti;
  int outer_loop_li=-1;
  const int loop_count = m_L.Count();
  const int trim_count = m_T.Count();
  MYON_BoundingBox outer_pbox;

  bool bAllTrimsAreIsoTrims = true; 
  bool bSomeTrimsAreIsoTrims = false;

  // 4 April 2003 Dale Lear:
  //    Shrink srf fix.
  MYON_BoundingBox trim_iso_endbox; // bounding box of iso curve trim ends

  int face_loop_count = face.m_li.Count();
  bool bIsSrfEdge[4];
  int sei;
  for (sei=0; sei<4; sei++)
    bIsSrfEdge[sei] = false;
  for ( fli = 0; fli < face_loop_count; fli++ )
  {
    li = face.m_li[fli];
    if ( li < 0 )
      continue;
    if ( li >= loop_count )
      continue;
    const MYON_BrepLoop& loop = m_L[li];
    if ( loop.m_type == MYON_BrepLoop::outer )
    {
      // may be more than one outer loop
      if ( outer_loop_li )
        outer_loop_li = li;
      outer_pbox.Union( loop.m_pbox );

      int loop_trim_count = loop.m_ti.Count();
      for ( lti = 0; lti < loop_trim_count; lti++ )
      {
        ti = loop.m_ti[lti];
        if ( ti >= 0 && ti < trim_count )
        {
          bool bIsIso = false;
          switch( m_T[ti].m_iso )
          {
          case MYON_Surface::x_iso:
          case MYON_Surface::y_iso:
            bIsIso = true;
            break;
          case MYON_Surface::W_iso:
            bIsIso = true;
            bIsSrfEdge[0] = true;
            break;
          case MYON_Surface::S_iso:
            bIsIso = true;
            bIsSrfEdge[1] = true;
            break;
          case MYON_Surface::E_iso:
            bIsIso = true;
            bIsSrfEdge[2] = true;
            break;
          case MYON_Surface::N_iso:
            bIsIso = true;
            bIsSrfEdge[3] = true;
            break;
          default:
            // it's not an iso curve trim
            bAllTrimsAreIsoTrims = false;
          }
          if (bIsIso){
            // it's an iso curve trim
            trim_iso_endbox.Set( m_T[ti].PointAtStart(), true );
            trim_iso_endbox.Set( m_T[ti].PointAtEnd(), true );
            bSomeTrimsAreIsoTrims = true;
          }
        }
      }

    }
  }

  if ( !outer_pbox.IsValid() )
    return false;
  
  bool rc = false;
  MYON_Interval outer_udom( outer_pbox.m_min.x, outer_pbox.m_max.x );
  MYON_Interval outer_vdom( outer_pbox.m_min.y, outer_pbox.m_max.y );

  if ( !bAllTrimsAreIsoTrims )
  {
    // 4 April 2003 Dale Lear:
    //    Prevent shrinking surface to
    //    interior edge of wiggly trims so that
    //    3d edge curves will pullback correctly and
    //    brep-brep intersections will be
    //    transverse along complicated trims.
    double d;

    d = outer_udom.Length()*0.01;
    if ( (!bSomeTrimsAreIsoTrims || outer_udom[0] < trim_iso_endbox.m_min.x) && !bIsSrfEdge[0] )
      outer_udom[0] -= d;
    if ( (!bSomeTrimsAreIsoTrims || outer_udom[1] > trim_iso_endbox.m_max.x) && !bIsSrfEdge[2])
      outer_udom[1] += d;

    d = outer_vdom.Length()*0.01;
    if ( (!bSomeTrimsAreIsoTrims || outer_vdom[0] < trim_iso_endbox.m_min.y) && !bIsSrfEdge[1] )
      outer_vdom[0] -= d;
    if ( (!bSomeTrimsAreIsoTrims || outer_vdom[1] > trim_iso_endbox.m_max.y) && !bIsSrfEdge[3] )
      outer_vdom[1] += d;
  }

  outer_udom.Intersection( srf_udom );
  outer_vdom.Intersection( srf_vdom );

  bool bShrinkIt = false;

  /*
  // removed 4 April 2003 Dale Lear
  if ( outer_udom.IsIncreasing() && outer_vdom.IsIncreasing() )
  {
    if ( outer_udom.Length() < 0.99*srf_udom.Length() || outer_vdom.Length() < 0.99*srf_vdom.Length())
    {
      bShrinkIt = true;
    }
    else if ( outer_udom.Length() < srf_udom.Length() || outer_vdom.Length() < srf_vdom.Length())
    {
      // 13 Feb 2003 Dale Lear added this --
      // if all trims are isos, then perform micro shrink
      // so iso trims will lie on surface boundaries
      bShrinkIt = bAllTrimsAreIsoTrims;
    }
  }
  */

  // GBA 8 May 2006.  Added DiasbleMask 
  if( DisableMask & 0x0001)     // West
    outer_udom[0] = srf_udom[0];
  if( DisableMask & 0x0002)     // South
    outer_vdom[0] = srf_vdom[0];
  if( DisableMask & 0x0004)     // East
    outer_udom[1] = srf_udom[1];
  if( DisableMask & 0x0008)     // North
    outer_vdom[1] = srf_vdom[1];


  // added 4 April 2003 Dale Lear
  if ( outer_udom.IsIncreasing() && outer_vdom.IsIncreasing() )
  {
    //TRR #33381 28-April-08 GBA
    //  Make sure we don't keep allowing the surface to be shrunk.
    if ( outer_udom.Length()*MYON_ZERO_TOLERANCE < (srf_udom.Length() - outer_udom.Length()) || 
         outer_vdom.Length()*MYON_ZERO_TOLERANCE < (srf_vdom.Length() - outer_vdom.Length())  )  
      bShrinkIt = true;
  }

  if ( bShrinkIt )
  {
    int srf_use = SurfaceUseCount( face.m_si, 2);
    MYON_Surface* small_srf = srf->Duplicate();
    if ( small_srf->Trim( 0, outer_udom ) )
    {
      if ( small_srf->Trim( 1, outer_vdom) )
        si = AddSurface(small_srf);
      if ( si >= 0 )
      {
				int srf_index = face.m_si;
        face.m_si = si;
        face.SetProxySurface( m_S[face.m_si] );

        // 5 Dec 2002 Chuck - dont delete original surface if used by more than one face
				if (srf_use == 1) DeleteSurface(srf_index);

        // 1 Nov 2002 Dale Lear - reset face bbox and destroy brep too big bounding box
        face.m_bbox = small_srf->BoundingBox();
        m_bbox.Destroy();

				// Set trim.m_iso flags
				for(int li_for_loop=0; li_for_loop<face.LoopCount(); li_for_loop++)
        {
					MYON_BrepLoop& loop = *face.Loop(li_for_loop);
					for(int ti_for_loop=0; ti_for_loop<loop.TrimCount(); ti_for_loop++)
          {
						MYON_BrepTrim& trim = *loop.Trim(ti_for_loop);
            //Since the slop used in calculating m_iso depends on the srf domain
            //all isos should be rechecked after shrinking

            /*
						if(	trim.m_iso==MYON_Surface::x_iso || 
								trim.m_iso==MYON_Surface::y_iso )
                */
            if (trim.m_iso != MYON_Surface::not_iso)
							trim.m_iso = face.IsIsoparametric(trim);	
					}
				}
        rc = true;
      }
    }
    if ( !rc )
      delete small_srf;
  }

  return rc;
}

bool MYON_Brep::ShrinkSurfaces()
{
  bool rc = true;
  int fi, face_count = m_F.Count();
  for ( fi = 0; fi < face_count; fi++ )
  {
    if ( !ShrinkSurface( m_F[fi] ) )
      rc = false;
  }
	Compact();
  return rc;
}

/*
int MYON_Brep::ComponentIndex( const MYON_BrepVertex& vertex ) const
{
  int component_index = vertex.m_vertex_index;
  if ( component_index >= 0 )
    component_index += brep_vertex;
  else
    component_index = -1;
  return component_index;
}

int MYON_Brep::ComponentIndex( const MYON_BrepEdge& edge ) const
{
  int component_index = edge.m_edge_index;
  if ( component_index >= 0 )
    component_index += brep_edge;
  else
    component_index = -1;
  return component_index;
}

int MYON_Brep::ComponentIndex( const MYON_BrepTrim& trim ) const
{
  int component_index = trim.m_trim_index;
  if ( component_index >= 0 )
    component_index += brep_trim;
  else
    component_index = -1;
  return component_index;
}

int MYON_Brep::ComponentIndex( const MYON_BrepLoop& loop ) const
{
  int component_index = loop.m_loop_index;
  if ( component_index >= 0 )
    component_index += brep_loop;
  else
    component_index = -1;
  return component_index;
}

int MYON_Brep::ComponentIndex( const MYON_BrepFace& face ) const
{
  int component_index = face.m_face_index;
  if ( component_index >= 0 )
    component_index += brep_face;
  else
    component_index = -1;
  return component_index;
}

MYON_Brep::COMPONENT_TYPE MYON_Brep::ComponentIndexType( int component_index )
{
  switch( brep_component_mask & component_index )
  {
  case brep_vertex: return brep_vertex;
  case brep_edge: return brep_edge;
  case brep_trim: return brep_trim;
  case brep_loop: return brep_loop;
  case brep_face: return brep_face;
  }
  return brep_component_unset;
}
*/

const MYON_Geometry* MYON_Brep::BrepComponent( 
  MYON_COMPONENT_INDEX ci
  ) const
{
  const MYON_Geometry* component = 0;
  switch ( ci.m_type )
  {
  case MYON_COMPONENT_INDEX::brep_vertex:
    component = Vertex(ci.m_index);
    break;
  case MYON_COMPONENT_INDEX::brep_edge:
    component = Edge(ci.m_index);
    break;
  case MYON_COMPONENT_INDEX::brep_face:
    component = Face(ci.m_index);
    break;
  case MYON_COMPONENT_INDEX::brep_trim:
    component = Trim(ci.m_index);
    break;
  case MYON_COMPONENT_INDEX::brep_loop:
    component = Loop(ci.m_index);
    break;
  default:
    // other enum values skipped on purpose
    break;
  }
  return component;
}

/*
const MYON_Geometry* MYON_Brep::BrepComponent( 
  int component_index
  ) const
{
  const MYON_Geometry* component = 0;
  if ( -1 != component_index && 0 != component_index)
  {
    switch( MYON_Brep::ComponentIndexType(component_index) )
    {
    case brep_vertex: 
      component = Vertex(component_index);
      break;
    case brep_edge:
      component = Edge(component_index);
      break;
    case brep_trim:
      component = Trim(component_index);
      break;
    case brep_loop:
      component = Loop(component_index);
      break;
    case brep_face:
      component = Face(component_index);
      break;
    }
  }
  return component;
}
*/


MYON_BrepVertex* MYON_Brep::Vertex( int vertex_index ) const
{
  MYON_BrepVertex* vertex = 0;
  if ( vertex_index>=0 && vertex_index < m_V.Count() )
    vertex = const_cast<MYON_BrepVertex*>(&m_V[vertex_index]);
  return vertex;
}

MYON_BrepVertex* MYON_Brep::Vertex( MYON_COMPONENT_INDEX vertex_index ) const
{
  MYON_BrepVertex* vertex = 0;
  if ( MYON_COMPONENT_INDEX::brep_vertex == vertex_index.m_type 
       && vertex_index.m_index >= 0 
       && vertex_index.m_index < m_V.Count() )
  {
    vertex = const_cast<MYON_BrepVertex*>(&m_V[vertex_index.m_index]);
  }
  return vertex;
}


MYON_BrepEdge* MYON_Brep::Edge( int edge_index ) const
{
  MYON_BrepEdge* edge = 0;
  if ( edge_index>=0 && edge_index < m_E.Count() )
    edge = const_cast<MYON_BrepEdge*>(&m_E[edge_index]);
  return edge;
}

MYON_BrepEdge* MYON_Brep::Edge( MYON_COMPONENT_INDEX edge_index ) const
{
  MYON_BrepEdge* edge = 0;
  if ( MYON_COMPONENT_INDEX::brep_edge == edge_index.m_type 
       && edge_index.m_index >= 0 
       && edge_index.m_index < m_E.Count() )
  {
    edge = const_cast<MYON_BrepEdge*>(&m_E[edge_index.m_index]);
  }
  return edge;
}

MYON_BrepTrim* MYON_Brep::Trim( int trim_index ) const
{
  MYON_BrepTrim* trim = 0;
  if ( trim_index>=0 && trim_index < m_T.Count() )
    trim = const_cast<MYON_BrepTrim*>(&m_T[trim_index]);
  return trim;
}

MYON_BrepTrim* MYON_Brep::Trim( MYON_COMPONENT_INDEX trim_index ) const
{
  MYON_BrepTrim* trim = 0;
  if ( MYON_COMPONENT_INDEX::brep_trim == trim_index.m_type 
       && trim_index.m_index >= 0 
       && trim_index.m_index < m_T.Count() )
  {
    trim = const_cast<MYON_BrepTrim*>(&m_T[trim_index.m_index]);
  }
  return trim;
}

MYON_BrepLoop* MYON_Brep::Loop( int loop_index ) const
{
  MYON_BrepLoop* loop = 0;
  if ( loop_index>=0 && loop_index < m_L.Count() )
    loop = const_cast<MYON_BrepLoop*>(&m_L[loop_index]);
  return loop;
}

MYON_BrepLoop* MYON_Brep::Loop( MYON_COMPONENT_INDEX loop_index ) const
{
  MYON_BrepLoop* loop = 0;
  if ( MYON_COMPONENT_INDEX::brep_loop == loop_index.m_type 
       && loop_index.m_index >= 0 
       && loop_index.m_index < m_L.Count() )
  {
    loop = const_cast<MYON_BrepLoop*>(&m_L[loop_index.m_index]);
  }
  return loop;
}

MYON_BrepFace* MYON_Brep::Face( int face_index ) const
{
  MYON_BrepFace* face = 0;
  if ( face_index>=0 && face_index < m_F.Count() )
    face = const_cast<MYON_BrepFace*>(&m_F[face_index]);
  return face;
}

MYON_BrepFace* MYON_Brep::Face( MYON_COMPONENT_INDEX face_index ) const
{
  MYON_BrepFace* face = 0;
  if ( MYON_COMPONENT_INDEX::brep_face == face_index.m_type 
       && face_index.m_index >= 0 
       && face_index.m_index < m_F.Count() )
  {
    face = const_cast<MYON_BrepFace*>(&m_F[face_index.m_index]);
  }
  return face;
}

const MYON_Surface* MYON_BrepFace::SurfaceOf() const
{
  const MYON_Surface* srf = ProxySurface();
  if ( 0 == srf && 0 != m_brep && m_si >= 0 && m_si < m_brep->m_S.Count() )
  {
    srf = m_brep->m_S[m_si];
  }
  return srf;
}



void MYON_BrepTrim::DestroyPspaceInformation()
{
  m_pline.Destroy();
  m_pbox.Destroy();
}

bool MYON_BrepTrim::ChangeTrimCurve( int c2i )
{
  if ( 0 == m_brep )
    return 0;
  if ( c2i < 0 || c2i >= m_brep->m_C2.Count() )
    return 0;
  const MYON_Curve* c2 = m_brep->m_C2[c2i];
  m_c2i = c2i;
  DestroyPspaceInformation();
  SetProxyCurve(c2);
  if ( c2 )
  {
    m_pbox = c2->BoundingBox();
    m_pbox.m_min.z = 0.0;
    m_pbox.m_max.z = 0.0;
  }
  return true;
}

bool MYON_BrepTrim::RemoveFromEdge( 
      bool bRemoveFromStartVertex,
      bool bRemoveFromEndVertex
      )
{
  bool rc = false;
  if ( 0 != m_brep || m_ei < 0  )
  {
    UnsetPlineEdgeParameters();
    if ( 0 != m_brep )
    {
      MYON_BrepEdge* edge = m_brep->Edge(m_ei);
      if ( 0 != edge )
      {
        int eti = 0;
        while( eti < edge->m_ti.Count() )
        {
          if ( edge->m_ti[eti] == m_trim_index )
            edge->m_ti.Remove(eti);
          else
            eti++;
        }
      }
    }
    m_ei = -1;
    if (bRemoveFromStartVertex)
      m_vi[0] = -1;
    if (bRemoveFromEndVertex)
      m_vi[1] = -1;
    rc = true;
  }
  return rc;
}

bool MYON_BrepTrim::AttachToEdge(
      int edge_index,
      bool bRev3d
      )
{
  bool rc = false;
  if ( 0 != m_brep )
  {
    MYON_BrepEdge* edge = m_brep->Edge(edge_index);
    if ( 0 != edge )
    {
      rc = RemoveFromEdge(true,true);
      if (rc)
      {
        edge->m_ti.Append(m_trim_index);
        m_ei = edge->m_edge_index;
        m_bRev3d = bRev3d ? true : false;
        m_vi[0] = edge->m_vi[bRev3d?1:0];
        m_vi[1] = edge->m_vi[bRev3d?0:1];
      }
    }
  }
  return rc;
}



const MYON_Curve* MYON_BrepEdge::EdgeCurveOf() const
{
  const MYON_Curve* c3 = ProxyCurve();
  if ( !c3 && m_brep && m_c3i >= 0 && m_c3i < m_brep->m_C3.Count())
  {
    // fallback to get answer if developer forgot to
    // set proxy ptr.
    c3 = m_brep->m_C3[m_c3i];
    if ( c3 )
    {
      MYON_ERROR("MYON_BrepEdge ProxyCurve() is nullptr but m_c3i is valid");
    }
  }
  return c3;
}

int MYON_BrepEdge::EdgeCurveIndexOf() const
{
  return (m_brep && m_c3i >= 0 && m_c3i < m_brep->m_C3.Count()) ? m_c3i : -1;
}

int MYON_BrepTrim::EdgeCurveIndexOf() const
{
  int c3i = -1;
  if ( m_brep && m_ei >= 0 && m_ei < m_brep->m_E.Count() )
  {
    c3i = m_brep->m_E[m_ei].m_c3i;
    if ( c3i < 0 || c3i >= m_brep->m_C3.Count() )
      c3i = -1;
  }
  return c3i;
}

int MYON_BrepTrim::TrimCurveIndexOf() const
{
  return ((m_brep && m_c2i >= 0 && m_c2i < m_brep->m_C2.Count()) ? m_c2i : -1);
}

const MYON_Curve* MYON_BrepTrim::EdgeCurveOf() const
{
  const MYON_Curve* c3 = 0;
  if ( m_brep && m_ei >= 0 && m_ei < m_brep->m_E.Count() )
  {
    c3 = m_brep->m_E[m_ei].EdgeCurveOf();
  }
  return c3;
}

bool MYON_BrepEdge::ChangeEdgeCurve( int c3i )
{
  if ( 0 == m_brep )
    return 0;
  if ( c3i < 0 || c3i >= m_brep->m_C3.Count() )
    return 0;
  const MYON_Curve* c3 = m_brep->m_C3[c3i];
  m_c3i = c3i;
  SetProxyCurve(c3);
  UnsetPlineEdgeParameters();
  return true;
}

const MYON_Curve* MYON_BrepTrim::TrimCurveOf() const
{
  const MYON_Curve* c2 = ProxyCurve();
  if ( !c2 && m_brep && m_c2i >= 0 && m_c2i < m_brep->m_C2.Count() )
  {
    // fallback to get answer if developer forgot to
    // set proxy ptr.
    c2 = m_brep->m_C2[m_c2i];
    if ( c2 )
    {
      MYON_ERROR("MYON_BrepTrim ProxyCurve() = nullptr but m_c2i is valid");
    }
  }
  return c2;
}

const MYON_Surface* MYON_BrepTrim::SurfaceOf() const
{
  const MYON_Surface* srf = 0;
  if ( m_brep && m_li >= 0 && m_li < m_brep->m_L.Count() )
  {
    const int fi = m_brep->m_L[m_li].m_fi;
    if ( fi >= 0 && fi < m_brep->m_F.Count() )
    {
      srf = m_brep->m_F[fi].SurfaceOf();
    }
  }
  return srf;
}

const MYON_Surface* MYON_BrepLoop::SurfaceOf() const
{
  const MYON_Surface* srf = 0;
  if ( m_brep && m_fi >= 0 && m_fi < m_brep->m_F.Count() )
  {
    srf = m_brep->m_F[m_fi].SurfaceOf();
  }
  return srf;
}

int MYON_BrepTrim::SurfaceIndexOf() const
{
  int si = -1;
  if ( m_brep && m_li >= 0 && m_li < m_brep->m_L.Count() )
  {
    const int fi = m_brep->m_L[m_li].m_fi;
    if ( fi >= 0 && fi < m_brep->m_F.Count() )
    {
      si = m_brep->m_F[fi].m_si;
      if ( si < 0 || si >= m_brep->m_S.Count() )
        si = -1;
    }
  }
  return si;
}


int MYON_BrepTrim::FaceIndexOf() const
{
  int fi = -1;
  if ( m_brep && m_li >= 0 && m_li < m_brep->m_L.Count() )
  {
    fi = m_brep->m_L[m_li].m_fi;
    if ( fi< 0 || fi >= m_brep->m_F.Count() )
    {
      fi = -1;
    }
  }
  return fi;
}

static
const MYON_BrepTrim* SlitSeamMateHelper( const MYON_BrepTrim& trim )
{
  if ( MYON_BrepTrim::seam  != trim.m_type )
    return 0;
  if ( trim.m_li < 0 )
    return 0;
  if ( trim.m_ei < 0 )
    return 0;
  const MYON_Brep* brep = trim.Brep();
  if ( !brep )
    return 0;
  if ( trim.m_ei >= brep->m_E.Count() )
    return 0;
  const MYON_BrepEdge& edge = brep->m_E[trim.m_ei];
  int other_ti = -1;
  for ( int eti = 0; eti < edge.m_ti.Count(); eti++ )
  {
    int ti = edge.m_ti[eti];
    if ( trim.m_trim_index == ti )
      continue;
    if ( ti < 0 || ti >= brep->m_T.Count() )
      continue;
    if ( trim.m_li == brep->m_T[ti].m_li )
    {
      if (other_ti >= 0 )
        return 0;
      other_ti = ti;
    }
  }
  if ( other_ti < 0 )
    return 0;
  return &brep->m_T[other_ti];
}

bool MYON_BrepTrim::IsSlit() const
{
  // 17 Nov 2006
  //     At this point in the development cycle, I cannot
  //     add a "slit" type to trim.  So, I will use this 
  //     function to distinguish between "slit" and "seam"
  //     trims.
  switch(m_iso)
  {
  case MYON_Surface::E_iso:
  case MYON_Surface::N_iso:
  case MYON_Surface::S_iso:
  case MYON_Surface::W_iso:
    return false;
    break;

  case MYON_Surface::not_iso:
  case MYON_Surface::x_iso:
  case MYON_Surface::y_iso:
  case MYON_Surface::iso_count:
    // anything else might be a slit
    break;
  }
  const MYON_BrepTrim* other_trim = SlitSeamMateHelper(*this);
  if ( !other_trim )
    return false;
  return ( other_trim->m_iso == m_iso );
}

bool MYON_BrepTrim::IsSeam() const
{
  // 17 Nov 2006
  //     At this point in the development cycle, I cannot
  //     add a "slit" type to trim.  So, I will use this 
  //     function to distinguish between "slit" and "seam"
  //     trims.
  MYON_Surface::ISO other_iso = MYON_Surface::not_iso;
  switch(m_iso)
  {
  case MYON_Surface::E_iso:
    other_iso = MYON_Surface::W_iso;
    break;
  case MYON_Surface::N_iso:
    other_iso = MYON_Surface::S_iso;
    break;
  case MYON_Surface::S_iso:
    other_iso = MYON_Surface::N_iso;
    break;
  case MYON_Surface::W_iso:
    other_iso = MYON_Surface::E_iso;
    break;
  default:
    return false;
  }
  const MYON_BrepTrim* other_trim = SlitSeamMateHelper(*this);
  if ( !other_trim )
    return false;

  return ( other_trim->m_iso == other_iso );
}


int MYON_BrepLoop::SurfaceIndexOf() const
{
  const MYON_BrepFace* face = Face();
  return face ? face->m_si : -1;
}

int MYON_BrepFace::SurfaceIndexOf() const
{
  return (m_brep && m_si >= 0 && m_si < m_brep->m_S.Count()) ? m_si : -1;
}

void MYON_BrepTrim::UnsetPlineEdgeParameters()
{
  int count = m_pline.Count();
  if ( count > 0 )
  {
    MYON_BrepTrimPoint* pline = m_pline.Array();
    while ( count-- )
      (pline++)->e = MYON_UNSET_VALUE;
  }
}

void MYON_BrepEdge::UnsetPlineEdgeParameters()
{
  int edge_trim_count, brep_trim_count, eti, ti;
  if ( 0 != m_brep )
  {
    edge_trim_count = m_ti.Count();
    if ( edge_trim_count > 0 )
    {
      brep_trim_count = m_brep->m_T.Count();
      for ( eti = 0; eti < edge_trim_count; eti++ )
      {
        ti = m_ti[eti];
        if ( ti >= 0 && ti < brep_trim_count )
        {
          m_brep->m_T[ti].UnsetPlineEdgeParameters();
        }
      }
    }
  }
}





bool MYON_BrepFace::TransformTrim( const MYON_Xform& xform )
{
  if ( !m_brep )
    return false;
  int fli;
  for ( fli = 0; fli < m_li.Count(); fli++ )
  {
    MYON_BrepLoop* loop = m_brep->Loop( m_li[fli] );
    if ( loop )
    {
      if ( !loop->TransformTrim(xform) )
        return false;
    }
  }
  return true;
}

bool MYON_BrepLoop::TransformTrim( const MYON_Xform& xform )
{
  if ( !m_brep )
    return false;
  int lti;
  m_pbox.Destroy();
  for ( lti = 0; lti < m_ti.Count(); lti++ )
  {
    MYON_BrepTrim* trim = m_brep->Trim( m_ti[lti] );
    if ( trim )
    {
      if ( !trim->TransformTrim(xform) )
        return false;
      m_pbox.Union( trim->m_pbox );
    }
  }
  return true;
}

bool MYON_BrepTrim::TransformTrim( const MYON_Xform& xform )
{
  // destroy cached information used to accelerate calculations
  DestroyCurveTree();
  m_pline.Destroy();

  if ( !m_brep )
    return false;

  // make sure only one trim uses the 2d curve
  if ( !m_brep->StandardizeTrimCurve( m_trim_index ) )
    return false;

  // transform 2d curve geometry
  MYON_Curve* c2 = const_cast<MYON_Curve*>(TrimCurveOf());
  if ( !c2 )
    return true;
  if ( !c2->Transform(xform) )
    return false;

  // update bounding box stored on trim
  m_pbox = c2->BoundingBox();
  m_pbox.m_min.z = 0.0;
  m_pbox.m_max.z = 0.0;

  // update 2d tolerances
  // Trim transforms can translate, scale and/or swap parameters.
  // The tolerances need to be adjusted for scaling and swapping.
  // Since the determinant can be < 0, fabs() must be applied.
  double tol0 = xform[0][0]*m_tolerance[0] + xform[0][1]*m_tolerance[1];
  double tol1 = xform[1][0]*m_tolerance[0] + xform[1][1]*m_tolerance[1];
  m_tolerance[0] = fabs(tol0);
  m_tolerance[1] = fabs(tol1);

  if ( m_iso != MYON_Surface::not_iso )
  {
    m_iso = MYON_Surface::not_iso;
    m_brep->SetTrimIsoFlags(*this);
  }

  return true;
}


void MYON_BrepTrim::DestroyRuntimeCache( bool bDelete )
{
  MYON_CurveProxy::DestroyRuntimeCache(bDelete);

  // This doesn't work right as of 30 Oct 2002 because
  // the pline is getting destroyed while it is still
  // valid and needed due to excessive calling
  // of DestroyRuntimeCache();

  //if ( bDelete )
  //  m_pline.Destroy();
  //else
  //  m_pline.EmergencyDestroy();

  // m_pbox.Destroy(); do not do this - it is not a runtime setting
  //                   and you will break the copy operators
}

void MYON_BrepLoop::DestroyRuntimeCache( bool bDelete )
{
  MYON_Object::DestroyRuntimeCache(bDelete);

  // m_pbox.Destroy(); do not do this - it is not a runtime setting
  //                   and you will break the copy operators
}

void MYON_BrepFace::DestroyRuntimeCache( bool bDelete )
{
  MYON_SurfaceProxy::DestroyRuntimeCache(bDelete);

  // 15 August 2003 Dale Lear:
  //    I added the line to destroy the face's m_bbox.
  //    Since m_bbox is private, it will be recalculated
  //    when it is needed.  (We hope.)  The fact the face
  //    m_bbox is private and recalculated as needed makes
  //    it different than the m_pbox info on trims and loops.
  m_bbox.Destroy();
}


int MYON_BrepLoop::Dimension() const
{
  return 2;
}

bool MYON_BrepLoop::GetBBox(
        double* boxmin,
        double* boxmax,
        bool bGrowBox
        ) const
{
  bool rc = m_pbox.IsValid();
  if (rc)
  {
    MYON_BoundingBox bbox;
    if ( bGrowBox )
    {
      bbox.m_min.x = boxmin[0];
      bbox.m_min.y = boxmin[1];
      bbox.m_min.z = 0.0;
      bbox.m_max.x = boxmax[0];
      bbox.m_max.y = boxmax[1];
      bbox.m_max.z = 0.0;
      bbox.Union(m_pbox);
      boxmin[0] = bbox.m_min.x;
      boxmin[1] = bbox.m_min.y;
      boxmax[0] = bbox.m_max.x;
      boxmax[1] = bbox.m_max.y;
    }
    else
    {
      boxmin[0] = m_pbox.m_min.x;
      boxmin[1] = m_pbox.m_min.y;
      boxmax[0] = m_pbox.m_max.x;
      boxmax[1] = m_pbox.m_max.y;
    }
  }
  return rc;
}

bool MYON_BrepLoop::Transform( const MYON_Xform& )
{
  return false;
}

MYON_COMPONENT_INDEX MYON_BrepVertex::ComponentIndex() const
{
  MYON_COMPONENT_INDEX ci(MYON_COMPONENT_INDEX::brep_vertex,m_vertex_index);
  return ci;
}

MYON_COMPONENT_INDEX MYON_BrepEdge::ComponentIndex() const
{
  MYON_COMPONENT_INDEX ci(MYON_COMPONENT_INDEX::brep_edge,m_edge_index);
  return ci;
}

MYON_COMPONENT_INDEX MYON_BrepFace::ComponentIndex() const
{
  MYON_COMPONENT_INDEX ci(MYON_COMPONENT_INDEX::brep_face,m_face_index);
  return ci;
}


MYON_COMPONENT_INDEX MYON_BrepTrim::ComponentIndex() const
{
  MYON_COMPONENT_INDEX ci(MYON_COMPONENT_INDEX::brep_trim,m_trim_index);
  return ci;
}



MYON_COMPONENT_INDEX MYON_BrepLoop::ComponentIndex() const
{
  MYON_COMPONENT_INDEX ci(MYON_COMPONENT_INDEX::brep_loop,m_loop_index);
  return ci;
}


#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_polyedgecurve.h"

MYON_OBJECT_IMPLEMENT(MYON_PolyEdgeSegment,MYON_CurveProxy,"42F47A87-5B1B-4e31-AB87-4639D78325D6");

MYON_PolyEdgeSegment::MYON_PolyEdgeSegment()
{
  Init();
}

MYON_PolyEdgeSegment::~MYON_PolyEdgeSegment()
{
  Init();
}

void MYON_PolyEdgeSegment::Init()
{
  m_component_index.UnSet();
  m_object_id = MYON_nil_uuid;
  m_brep = 0;
  m_trim = 0;
  m_edge = 0;
  m_face = 0;
  m_surface = 0;
  m_edge_domain = MYON_Interval::EmptyInterval;
  m_trim_domain = MYON_Interval::EmptyInterval;
  MYON_CurveProxy::SetProxyCurve(0);

  ClearEvalCacheHelper();
}

bool MYON_PolyEdgeSegment::Create( 
                const MYON_BrepTrim* trim,
                const MYON_UUID& object_id
                )
{
  Init();
  if ( !trim )
    return false;
  if ( trim->m_ei >= 0 )
    return false;
  const MYON_Brep* brep = trim->Brep();
  if ( !brep )
    return false;
  const MYON_BrepEdge* edge = trim->Edge();
  if ( !edge )
    return false;
  const MYON_Curve* c3 = edge->EdgeCurveOf();
  if ( !c3 )
    return false;
  m_brep = brep;
  m_trim = trim;
  m_edge = edge;
  m_face = trim->Face();
  if ( m_face )
    m_surface = m_face->SurfaceOf();
  m_edge_domain = m_edge->Domain();
  m_trim_domain = m_trim->Domain();
  MYON_CurveProxy::SetProxyCurve( 
                    c3,
                    edge->ProxyCurveDomain()
                    );    
  if ( m_edge->ProxyCurveIsReversed() )
    MYON_CurveProxy::Reverse();
  MYON_CurveProxy::SetDomain( m_edge_domain[0], m_edge_domain[1] );
  m_component_index = trim->ComponentIndex();
  m_object_id = object_id;
  return true;
}


bool MYON_PolyEdgeSegment::ReversedEdgeDir() const
{
  bool rc = false;
  if ( m_edge )
  {
    rc = m_edge->ProxyCurveIsReversed() != MYON_CurveProxy::ProxyCurveIsReversed();
  }
  return rc;
}

bool MYON_PolyEdgeSegment::ReversedTrimDir() const
{
  bool rc = false;
  if ( m_trim && m_edge )
  {
    rc = ReversedEdgeDir();
    if ( m_trim->m_bRev3d )
      rc = !rc;
  }
  return rc;
}

bool MYON_PolyEdgeSegment::Create( 
          const MYON_Curve* curve,
          const MYON_UUID& object_id 
          )
{
  //bool rc = false;
  Init();
  if ( !curve )
    return false;
  const MYON_BrepEdge* edge = MYON_BrepEdge::Cast(curve);
  if ( edge )
  {
    const MYON_Brep* brep = edge->Brep();
    if ( !brep )
      return false;
    const MYON_Curve* c3 = edge->EdgeCurveOf();
    if ( !c3 )
      return false;
    m_edge = edge;
    m_brep = brep;
    m_component_index = edge->ComponentIndex();
    m_edge_domain = m_edge->Domain();
    m_trim_domain = m_trim->Domain();
    MYON_CurveProxy::SetProxyCurve( 
                      c3,
                      edge->ProxyCurveDomain()
                      );    
    if ( m_edge->ProxyCurveIsReversed() )
      MYON_CurveProxy::Reverse();
    MYON_CurveProxy::SetDomain( m_edge_domain[0], m_edge_domain[1] );
  }
  else
  {
    MYON_CurveProxy::SetProxyCurve(const_cast<MYON_Curve*>(curve));   
  }
  m_object_id = object_id;
  return true;
}

const MYON_BrepEdge* MYON_PolyEdgeSegment::BrepEdge() const
{
  return m_edge;
}

const MYON_BrepTrim* MYON_PolyEdgeSegment::BrepTrim() const
{
  return m_trim;
}

const MYON_Brep* MYON_PolyEdgeSegment::Brep() const
{
  return m_brep;
}

const MYON_BrepFace* MYON_PolyEdgeSegment::BrepFace() const
{
  return m_face;
}

const MYON_Surface*  MYON_PolyEdgeSegment::Surface() const
{
  return m_surface;
}

MYON_Surface::ISO MYON_PolyEdgeSegment::IsoType() const
{
  return m_trim ? m_trim->m_iso : MYON_Surface::not_iso;
}

MYON_Interval MYON_PolyEdgeSegment::EdgeDomain() const
{
  return m_edge_domain;
}

MYON_Interval MYON_PolyEdgeSegment::TrimDomain() const
{
  return m_trim_domain;
}


void MYON_PolyEdgeSegment::ClearEvalCacheHelper()
{
  m_t = MYON_UNSET_VALUE;
  m_edge_t = MYON_UNSET_VALUE;
  m_trim_t = MYON_UNSET_VALUE;
  m_srf_uv[0] = MYON_UNSET_VALUE;
  m_srf_uv[1] = MYON_UNSET_VALUE;
  m_trim_hint = 0;
  m_edge_hint = 0;
  m_evsrf_hint[0] = 0;
  m_evsrf_hint[1] = 0;
  m_evsrf_uv[0] = MYON_UNSET_VALUE;
  m_evsrf_uv[1] = MYON_UNSET_VALUE;
  m_evsrf_pt = MYON_3dPoint::UnsetPoint;
}


double MYON_PolyEdgeSegment::EdgeParameter(double t) const
{
  double edge_t = MYON_UNSET_VALUE;
  if ( m_edge )
  {
    if ( m_t == t && m_edge_t != MYON_UNSET_VALUE )
      edge_t = m_edge_t;
    else
    {
      MYON_PolyEdgeSegment* p = const_cast<MYON_PolyEdgeSegment*>(this);
      if ( t != m_t )
      {
        p->m_t = t;
        p->m_trim_t = MYON_UNSET_VALUE;
        p->m_srf_uv[0] = MYON_UNSET_VALUE;
        p->m_srf_uv[1] = MYON_UNSET_VALUE;
      }
      MYON_Interval d = Domain();
      bool bReversedEdgeDir = ReversedEdgeDir();
      if ( bReversedEdgeDir || m_edge_domain != d )
      {
        double s = d.NormalizedParameterAt(t);
        if ( bReversedEdgeDir )
          s = 1.0 - s;
        edge_t = m_edge_domain.ParameterAt(s);
      }
      else
        edge_t = t;
      p->m_edge_t = edge_t;
    }
  }
  return edge_t;
}




MYON_OBJECT_IMPLEMENT(MYON_PolyEdgeCurve,MYON_PolyCurve,"39FF3DD3-FE0F-4807-9D59-185F0D73C0E4");

MYON_PolyEdgeCurve::MYON_PolyEdgeCurve()
{
}

MYON_PolyEdgeCurve::~MYON_PolyEdgeCurve()
{
}

bool MYON_PolyEdgeCurve::SetStartPoint( MYON_3dPoint start_point )
{
  return MYON_Curve::SetStartPoint(start_point); // cannot change edges
}

bool MYON_PolyEdgeCurve::SetEndPoint( MYON_3dPoint end_point )
{
  return MYON_Curve::SetEndPoint(end_point); // cannot change edges
}

bool MYON_PolyEdgeCurve::ChangeClosedCurveSeam( double t )
{
  //int saved_is_closed_helper = m_is_closed_helper;

  if ( SegmentCount() == 1 )
  {
    // A MYON_PolyEdgeSegment cannot have its start/end
    // changed. Split it into two segments and let 
    // MYON_PolyCurve::ChangeClosedCurveSeam() do the work.
    if ( !IsClosed() )
      return false;

    MYON_Interval crvd = Domain();
    double s = crvd.NormalizedParameterAt(t);

    if ( s <= MYON_SQRT_EPSILON || s >= (1.0 - MYON_SQRT_EPSILON) )
    {
      s = fmod(s,1.0);
      if ( s < 0.0 )
        s += 1.0;
      if ( fabs(s) <= MYON_SQRT_EPSILON || fabs(1.0-s) <= MYON_SQRT_EPSILON )
      {
        // split parameter is at start/end of this segemnt
        if ( t != crvd[0] )
        {
          DestroyRuntimeCache();
          SetDomain(t,t+crvd.Length() );
          //m_is_closed_helper = saved_is_closed_helper;
        }
        return true;
      }
      return false;
    }

    MYON_PolyEdgeSegment* left_seg = SegmentCurve(0);
    if ( 0 == left_seg )
      return false;

    DestroyRuntimeCache();

    MYON_Curve* left = left_seg;
    MYON_Curve* right = 0;
    double segt = SegmentCurveParameter(t);
    if ( !left_seg->Split(segt,left,right) )
      return false;
    SetDomain(crvd[0],t);
    
    MYON_PolyEdgeSegment* right_seg = MYON_PolyEdgeSegment::Cast(right);
    if ( 0 == right_seg )
      return false;
    Append(right_seg);

    double st[3];
    st[0] = crvd[0];
    st[1] = t;
    st[2] = crvd[1];
    SetParameterization( st );
  }

  // MYON_PolyCurve::ChangeClosedCurveSeam works fine on
  // two or more segments.
  bool rc = MYON_PolyCurve::ChangeClosedCurveSeam(t);
  //if ( saved_is_closed_helper )
  //  m_is_closed_helper = saved_is_closed_helper;

  return rc;
}

bool MYON_PolyEdgeCurve::PrependAndMatch(MYON_Curve*)
{
  return false; // cannot change edges
}


bool MYON_PolyEdgeCurve::AppendAndMatch(MYON_Curve*)
{
  return false; // cannot change edges
}

MYON_Curve* MYON_PolyEdgeSegment::DuplicateCurve() const
{
  return MYON_CurveProxy::DuplicateCurve();

  // 21 December 2004 Dale Lear
  //     This is wrong.  I did it some time ago as a quick
  //     fix for one of Lowell's early uses of CRhinoPolyEdges
  //     however, this will cause lots of crashes now that
  //     all commands have to deal with polyedges and the code
  //     in those commands assumes that DuplicateCurve() returns
  //     a valid stand-alone curve.  If you end up here and
  //     wish this code still worked the old way, please get
  //     in touch with Dale Lear and we'll find a way to get
  //     your code to work.
  // NO // MYON_PolyEdgeSegment* dup = Duplicate();
  // NO // return dup;
}

MYON_Curve* MYON_PolyEdgeCurve::DuplicateCurve() const
{
  return MYON_PolyCurve::DuplicateCurve();

  // 21 December 2004 Dale Lear
  //     The code below is wrong.  I wrote it some time ago as a quick
  //     fix for one of Lowell's early uses of CRhinoPolyEdges
  //     however, this will cause lots of crashes now that
  //     all commands have to deal with polyedges and the code
  //     in those commands assumes that DuplicateCurve() returns
  //     a valid stand-alone curve.  If you end up here and
  //     wish this code still worked the old way, please get
  //     in touch with Dale Lear and we'll find a way to get
  //     your code to work.

	// NO //  int cnt = Count();
  // NO //  MYON_SimpleArray<double> t(cnt+1);
	// NO //  MYON_PolyEdgeCurve* dup_crv = new MYON_PolyEdgeCurve();
  // NO //  
  // NO //  t.Append(Domain()[0]);
  // NO // 
  // NO // for( int i=0; i<cnt; i++)
  // NO //  {
  // NO // 	const MYON_Curve* seg = SegmentCurve(i);
  // NO //    if ( seg )
  // NO //    {
  // NO //      t.Append(SegmentDomain(i)[1]);
  // NO //      dup_crv->MYON_PolyCurve::Append( seg->DuplicateCurve() );
  // NO //    }
  // NO // }	
  // NO // 
  // NO //  if( cnt > 0 && cnt+1 == t.Count() )
  // NO //  {
  // NO //    dup_crv->SetParameterization( t.Array() );
  // NO //  }
  // NO // 
  // NO //  dup_crv->m_ev_srf_tan_mode = m_ev_srf_tan_mode;
  // NO //  dup_crv->m_is_closed_helper = m_is_closed_helper;
  // NO // 
  // NO // return dup_crv;
}

bool MYON_PolyEdgeSegment::IsClosed(void) const
{
  bool rc = MYON_CurveProxy::IsClosed();
  if ( !rc
       && m_edge
       && m_edge->m_vi[0] == m_edge->m_vi[1]
       && m_edge->ProxyCurve() == ProxyCurve()
       && m_edge->ProxyCurveDomain() == ProxyCurveDomain()
       && 0 != ProxyCurve()
       && ProxyCurve()->Domain() == ProxyCurveDomain()
       )
  {
    rc = m_edge->IsClosed();
  }
  return rc;
}

// 7-1-03 lw added override to unset cached closed flag
// when a segment is removed
bool MYON_PolyEdgeCurve::Remove( int segment_index )
{
  bool rc = MYON_PolyCurve::Remove( segment_index);
  //if( rc)
  //  m_is_closed_helper = 0;  // Cached closed flag...
  return rc;
}
bool MYON_PolyEdgeCurve::Remove( )
{
  return Remove(Count()-1);
}

bool MYON_PolyEdgeCurve::IsClosed(void) const
{
  bool rc = MYON_PolyCurve::IsClosed();

  if ( !rc && SegmentCount() > 1 )
  {
    // Since the segments that make up a MYON_PolyEdgeCurve
    // cannot have their ends matched (becuase the curves
    // belong to objects alread in the rhino model), 
    // the IsClosed() test has to tolerate larger gaps
    // in brep topology.
    //
    // If the start and end segments are edges that belong
    // to the same brep, then they "join" if and only if 
    // they share a vertex.
    const MYON_PolyEdgeSegment* seg0 = SegmentCurve(0);
    const MYON_PolyEdgeSegment* seg1 = SegmentCurve(SegmentCount()-1);

    const MYON_BrepEdge* edge0 = seg0->BrepEdge();
    const MYON_BrepEdge* edge1 = seg1->BrepEdge();
    
    if ( edge0 && edge1 && edge0->Brep() == edge1->Brep() )
    {
      // check for topological closure
      //
      // Do NOT add a test for sloppy geometric closure here.
      // If the edges are in the same brep and they don't 
      // meet topologicially, then there is something in the
      // brep that is separating the edges.
      int evi0 = seg0->ReversedEdgeDir() ? 1 : 0;
      int evi1 = seg1->ReversedEdgeDir() ? 0 : 1;
      double et0 = seg0->EdgeParameter(seg0->Domain()[0]);
      double et1 = seg1->EdgeParameter(seg1->Domain()[1]);
      if ( et0 != MYON_UNSET_VALUE && et1 != MYON_UNSET_VALUE )
      {
        MYON_Interval edom0 = edge0->Domain();
        MYON_Interval edom1 = edge1->Domain();
        if ( et0 == edom0[evi0] && et1 == edom1[evi1] )
        {
          // The polyedge starts at the (evi0?end:start) of edge0
          // and ends at the (ev1?end:start) of edge1.
          if ( edge0->m_vi[evi0] == edge1->m_vi[evi1] )
          {
            // the polyedge start/ends at a common vertex
            rc = true;
          }
        }
        else if ( edge0 == edge1 
                  && fabs(et0-et1) <= MYON_ZERO_TOLERANCE 
                  && edom0.Includes(et0,true)
                  && edom1.Includes(et1,true)
                  )
        {
          // The start/end of the polyedge is an interior point
          // of a single edge.  (This happens when the "seam" gets
          // adjusted to be inside of an edge.)  It is unlikely that
          // MYON_PolyCurve::IsClosed() would return false in this
          // case, but this check should keep everybody happy.
          rc = true;
        }
      }
    }
  }

  return rc;
}

bool MYON_PolyEdgeCurve::Create( const MYON_BrepTrim* trim, const MYON_UUID& object_id )
{
  bool rc = false;
  Destroy();
  //m_is_closed_helper = 0;
  if ( trim )
  {
    MYON_PolyEdgeSegment* segment = new MYON_PolyEdgeSegment();
    rc = segment->Create(trim,object_id);
    if (rc )
      Append(segment);
    else
      delete segment;
  }
  return rc;
}

bool MYON_PolyEdgeCurve::Create( const MYON_Curve* curve, const MYON_UUID& object_id )
{
  bool rc = false;
  Destroy();
  //m_is_closed_helper = 0;
  if ( curve )
  {
    MYON_PolyEdgeSegment* segment = new MYON_PolyEdgeSegment();
    rc = segment->Create(curve,object_id);
    if (rc )
      Append(segment);
    else
      delete segment;
  }
  return rc;
}

int MYON_PolyEdgeCurve::SegmentCount() const
{
  return MYON_PolyCurve::Count();
}

MYON_PolyEdgeSegment* MYON_PolyEdgeCurve::SegmentCurve(
  int segment_index
  ) const
{
  return MYON_PolyEdgeSegment::Cast(MYON_PolyCurve::SegmentCurve(segment_index));
}


MYON_PolyEdgeSegment* MYON_PolyEdgeCurve::operator[](int segment_index) const
{
  return SegmentCurve(segment_index);
}

void MYON_PolyEdgeCurve::DestroyRuntimeCache( bool bDelete )
{
  //m_is_closed_helper = 0;
  MYON_PolyCurve::DestroyRuntimeCache(bDelete);
}

void MYON_PolyEdgeSegment::DestroyRuntimeCache( bool bDelete )
{
  ClearEvalCacheHelper();
  MYON_CurveProxy::DestroyRuntimeCache(bDelete);
}


bool MYON_PolyEdgeCurve::Prepend( MYON_PolyEdgeSegment* new_segment )
{
  DestroyRuntimeCache();
  bool rc = false;
  if ( new_segment )
  {
    if ( Count() > 0 )
    {
      // keep segment domains in synch with polycurve domain
      // so that parameter bookkeeping is easy.
      MYON_Interval cdom = Domain();
      MYON_Interval sdom = new_segment->Domain();
      if ( sdom[1] != cdom[0] )
      {
        sdom[0] = cdom[0] - sdom.Length();
        sdom[1] = cdom[0];
        new_segment->SetDomain(sdom[0],sdom[1]);
      }
    }
    rc = MYON_PolyCurve::Prepend(new_segment);
  }
  return rc;
}

bool MYON_PolyEdgeCurve::Append( MYON_PolyEdgeSegment* new_segment )
{
  DestroyRuntimeCache();
  bool rc = false;
  if ( new_segment )
  {
    //m_is_closed_helper = 0;
    if ( Count() > 0 )
    {
      // keep segment domains in synch with polycurve domain
      // so that parameter bookkeeping is easy.
      MYON_Interval cdom = Domain();
      MYON_Interval sdom = new_segment->Domain();
      if ( sdom[0] != cdom[1] )
      {
        sdom[1] = cdom[1] + sdom.Length();
        sdom[0] = cdom[1];
        new_segment->SetDomain(sdom[0],sdom[1]);
      }
    }
    rc = MYON_PolyCurve::Append(new_segment);
  }
  return rc;
}

bool MYON_PolyEdgeCurve::Insert( 
         int segment_index,
         MYON_PolyEdgeSegment* new_segment
         )
{
  DestroyRuntimeCache();
  bool rc = false;
  if ( segment_index > 0 )
  {
    //m_is_closed_helper = 0;
    rc = MYON_PolyCurve::Insert(segment_index,new_segment);
    if ( rc )
    {
      int i;
      for ( i = segment_index; i < Count(); i++ )
      {
        MYON_PolyEdgeSegment* seg = SegmentCurve(i);
        MYON_Interval d = SegmentDomain(i);
        seg->SetDomain(d[0],d[1]);
      }
    }
  }
  else if ( segment_index == 0 )
    rc = Prepend(new_segment);
  return rc;
}

const MYON_BrepEdge* MYON_PolyEdgeCurve::EdgeAt(double t) const
{
  MYON_PolyEdgeSegment* seg = SegmentCurve( SegmentIndex(t) );
  return seg ? seg->BrepEdge() : 0;
}

const MYON_BrepTrim* MYON_PolyEdgeCurve::TrimAt(double t) const
{
  MYON_PolyEdgeSegment* seg = SegmentCurve( SegmentIndex(t) );
  return seg ? seg->BrepTrim() : 0;
}

const MYON_Brep*     MYON_PolyEdgeCurve::BrepAt(double t) const
{
  MYON_PolyEdgeSegment* seg = SegmentCurve( SegmentIndex(t) );
  return seg ? seg->Brep() : 0;
}

const MYON_BrepFace* MYON_PolyEdgeCurve::FaceAt(double t) const
{
  MYON_PolyEdgeSegment* seg = SegmentCurve( SegmentIndex(t) );
  return seg ? seg->BrepFace() : 0;
}

const MYON_Surface*  MYON_PolyEdgeCurve::SurfaceAt(double t) const
{
  MYON_PolyEdgeSegment* seg = SegmentCurve( SegmentIndex(t) );
  return seg ? seg->Surface() : 0;
}

MYON_Surface::ISO MYON_PolyEdgeCurve::IsoType( double t) const
{
  MYON_PolyEdgeSegment* seg = SegmentCurve( SegmentIndex(t) );
  return seg ? seg->IsoType() : MYON_Surface::not_iso;
}


double MYON_PolyEdgeCurve::EdgeParameter(double t) const
{
  double edge_t = MYON_UNSET_VALUE;
  int segment_index = SegmentIndex(t);
  MYON_PolyEdgeSegment* seg = SegmentCurve( segment_index );
  if ( seg )
  {
    MYON_Interval pdom = SegmentDomain(segment_index);
    MYON_Interval sdom = seg->Domain();
    if ( sdom != pdom )
    {
      double s = pdom.NormalizedParameterAt(t);
      t = sdom.ParameterAt(s);
    }
    edge_t = seg->EdgeParameter(t);
  }
  return edge_t;
}


// Test if there are any surface edges in the polyedge
bool MYON_PolyEdgeCurve::ContainsAnyEdges() const
{
  int i, count = SegmentCount();
  for( i = 0; i < count; i++)
  {
    MYON_PolyEdgeSegment* segment = SegmentCurve(i);
    if( 0 != segment && nullptr != segment->BrepEdge())
    {
      return true;
    }
  }
  return false;
}

// Test if all segments of the polyedge are surface edges
bool MYON_PolyEdgeCurve::ContainsAllEdges() const
{
  int i, count = SegmentCount();
  for( i = 0; i < count; i++)
  {
    MYON_PolyEdgeSegment* segment = SegmentCurve(i);
    if( nullptr == segment || nullptr == segment->BrepEdge())
    {
      return false;
    }
  }
  return true;
}

int MYON_PolyEdgeCurve::FindEdge( const MYON_BrepEdge* edge) const
{
  int rc = -1;
  if ( 0 != edge )
  {
    int i, count = SegmentCount();
    for( i = 0; i < count; i++)
    {
      MYON_PolyEdgeSegment* segment = SegmentCurve(i);
      if ( 0 != segment && edge == segment->BrepEdge() )
      {
        rc = i;
        break;
      }
    }
  }
  return rc;
}

int MYON_PolyEdgeCurve::FindTrim( const MYON_BrepTrim* trim) const
{
  int rc = -1;
  if ( 0 != trim )
  {
    int i, count = SegmentCount();
    for( i = 0; i < count; i++)
    {
      MYON_PolyEdgeSegment* segment = SegmentCurve(i);
      if ( 0 != segment && trim == segment->BrepTrim() )
      {
        rc = i;
        break;
      }
    }
  }
  return rc;
}

int MYON_PolyEdgeCurve::FindCurve( const MYON_Curve* curve) const
{
  int rc = -1;
  if ( 0 != curve )
  {
    int i, count = SegmentCount();
    for( i = 0; i < count; i++)
    {
      MYON_PolyEdgeSegment* segment = SegmentCurve(i);
      if (    0 != segment 
           && (curve == segment || curve == segment->ProxyCurve() || curve == segment->BrepEdge()) )
      {
        rc = i;
        break;
      }
    }
  }
  return rc;
}

bool MYON_PolyEdgeSegment::Write( MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if (!rc)
    return false;
  for(;;)
  {
    rc = archive.WriteUuid(m_object_id);
    if (!rc) break;
    rc = archive.WriteComponentIndex(m_component_index);
    if (!rc) break;
    rc = archive.WriteInterval(m_edge_domain);
    if (!rc) break;
    rc = archive.WriteInterval(m_trim_domain);
    if (!rc) break;
    bool b = MYON_CurveProxy::ProxyCurveIsReversed();
    rc = archive.WriteBool(b);
    if (!rc) break;
    rc = archive.WriteInterval(MYON_CurveProxy::Domain());
    if (!rc) break;
    rc = archive.WriteInterval(MYON_CurveProxy::ProxyCurveDomain());
    if (!rc) break;

    break;
  }
  if ( !archive.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

bool MYON_PolyEdgeSegment::Read( MYON_BinaryArchive& archive )
{
  Init();
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;
  for(;;)
  {
    rc = (1 == major_version);
    if ( !rc )
      break;

    rc = archive.ReadUuid(m_object_id);
    if (!rc) break;
    rc = archive.ReadComponentIndex(m_component_index);
    if (!rc) break;
    rc = archive.ReadInterval(m_edge_domain);
    if (!rc) break;
    rc = archive.ReadInterval(m_trim_domain);
    if (!rc) break;

    // Read MYON_ProxyCurve values we need
    bool bReversed = false;
    rc = archive.ReadBool(&bReversed);
    if (!rc) break;
    MYON_Interval this_domain;
    rc = archive.ReadInterval(this_domain);
    if (!rc) break;
    MYON_Interval real_curve_domain;
    rc = archive.ReadInterval(real_curve_domain);
    if (!rc) break;

    if ( bReversed)
      MYON_CurveProxy::Reverse();
    MYON_CurveProxy::SetDomain(this_domain);
    MYON_CurveProxy::SetProxyCurveDomain(real_curve_domain);

    break;
  }
  if ( !archive.EndRead3dmChunk() )
    rc = false;
  return rc;
}


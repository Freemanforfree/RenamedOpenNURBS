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

#if !defined(MYON_POLYEDGECURVE_INC_)
#define MYON_POLYEDGECURVE_INC_

class MYON_PolyEdgeSegment;

class MYON_CLASS MYON_PolyEdgeCurve : public MYON_PolyCurve
{
  MYON_OBJECT_DECLARE(MYON_PolyEdgeCurve);

public:
  MYON_PolyEdgeCurve();
  ~MYON_PolyEdgeCurve();
  // default copy constructor and operator= are fine.

  // virtual MYON_Object::DestroyRuntimeCache override
  void DestroyRuntimeCache( bool bDelete = true ) override;

  // virtual MYON_Curve::IsClosed override
  bool IsClosed() const override;

  // virtual MYON_Curve::DuplicateCurve override
  MYON_Curve* DuplicateCurve() const override;

  /*
  Description:
    Create a one segment MYON_PolyEdgeCurve curve that uses a 
    single edge.
  */
  bool Create(
           const MYON_BrepTrim* trim,
           const MYON_UUID& object_id
           );

  /*
  Description:
    Create a one segment MYON_PolyEdgeCurve curve that uses a 
    single curve.
  */
  bool Create(
           const MYON_Curve* curve,
           const MYON_UUID& object_id
           );

  int SegmentCount() const;

  MYON_PolyEdgeSegment* SegmentCurve(
    int segment_index
    ) const;

  MYON_PolyEdgeSegment* operator[](int) const;

  bool Prepend( MYON_PolyEdgeSegment* new_segment ); // Prepend curve.
  bool Append( MYON_PolyEdgeSegment* new_segment );  // Append curve.
  bool Insert( 
           int segment_index,
           MYON_PolyEdgeSegment* new_segment
           );

  // if the segment is an edge, the following
  // return non-nullptr pointers.
  const MYON_BrepEdge* EdgeAt(double t) const;
  const MYON_BrepTrim* TrimAt(double t) const;
  const MYON_Brep*     BrepAt(double t) const;
  const MYON_BrepFace* FaceAt(double t) const;
  const MYON_Surface*  SurfaceAt(double t) const;
  MYON_Surface::ISO    IsoType( double t) const;

  double EdgeParameter(double t) const;

  // Test if there are any surface edges in the polyedge
  bool ContainsAnyEdges() const;
  // Test if all segments of the polyedge are surface edges
  bool ContainsAllEdges() const;
  

  /*
  Description:
    See if this polyedge has an edge as one of its segments
  Parameters:
    edge - [in] the edge to look for
  Returns:
     -1: edge is not in the polyedge
    >=0: index of first segment that uses the edge
  */
  int FindEdge( const MYON_BrepEdge* edge) const;

  /*
  Description:
    See if this polyedge has a trim as one of its segments
  Parameters:
    trim - [in] the trim to look for
  Returns:
     -1: trim is not in the polyedge
    >=0: index of first segment that uses the trim
  */
  int FindTrim( const MYON_BrepTrim* trim) const;

  /*
  Description:
    See if this polyedge has a wire curve as one of its segments
  Parameters:
    curve - [in] the curve to look for
  Returns:
     -1: trim is not in the polyedge
    >=0: index of first segment that uses the curve
  */
  int FindCurve( const MYON_Curve* curve) const;


  // OBSOLETE and unreliable.  Use FindCurve, FindEdge, or FindTrim
  //bool Contains( const MYON_Curve* curve) const;

  // virtual MYON_Curve overrides do nothing
  // to prevent changing edge
  bool SetStartPoint(MYON_3dPoint start_point) override;
  bool SetEndPoint(MYON_3dPoint end_point) override;
  bool ChangeClosedCurveSeam( double t ) override;
  bool PrependAndMatch(MYON_Curve*);
  bool AppendAndMatch(MYON_Curve*);

  // 7-1-03 lw added override to unset cached closed flag
  // when a segment is removed
  bool Remove(); // remove last
  bool Remove( int index);
};

class MYON_CLASS MYON_PolyEdgeSegment : public MYON_CurveProxy
{
  MYON_OBJECT_DECLARE(MYON_PolyEdgeSegment);
public:
  // construction

  MYON_PolyEdgeSegment();
  ~MYON_PolyEdgeSegment();
  // default copy constructor and operator= are fine.

  // virtual MYON_Object::DestroyRuntimeCache override
  void DestroyRuntimeCache( bool bDelete = true ) override;

  bool Write( MYON_BinaryArchive& ) const override;

  bool Read( MYON_BinaryArchive& ) override;

  // virtual MYON_Curve::IsClosed override
  bool IsClosed() const override;


  // virtual MYON_Curve::DuplicateCurve override
  MYON_Curve* DuplicateCurve() const override;

  /*
  Description:
    Creates a polyedge segment that uses the entire edge
    and has the same domain as the edge.
  Parameters:
    trim - [in] 
  Returns:
    true if successful (edge was valid and trim_index was valid)
  Remarks:
    Use MYON_Curve::SetDomain, MYON_Curve::Trim, MYON_Curve::Reverse,
    etc., to tweak the domain, support, direction etc.
  */
  bool Create( 
          const MYON_BrepTrim* trim,
          const MYON_UUID& object_id
          );

  /*
  Description:
    Creates a polyedge segment that uses the entire curve
    and has the same domain as the curve.
  Parameters:
    curve - [in] 
  Remarks:
    Use MYON_Curve::SetDomain, MYON_Curve::Trim, MYON_Curve::Reverse,
    etc., to tweak the domain, support, direction etc.
  */
  bool Create( 
          const MYON_Curve* curve,
          const MYON_UUID& object_id
          );

  const MYON_BrepEdge* BrepEdge() const;
  const MYON_BrepTrim* BrepTrim() const;
  const MYON_Brep*     Brep() const;
  const MYON_BrepFace* BrepFace() const;
  const MYON_Surface*  Surface() const;
  MYON_Surface::ISO    IsoType() const;

  double EdgeParameter(double t) const;

  /*
  Returns:
    True if this segment has an MYON_BrepEdge and the direction of
    the MYON_BrepEdge is the reverse of the direction of the segment.
  */
  bool ReversedEdgeDir() const;

  /*
  Returns:
    True if this segment has an MYON_BrepTrim and the direction of
    the MYON_BrepTrime is the reverse of the direction of the segment.
  */
  bool ReversedTrimDir() const;

  /*
  Returns:
    subdomain of the edge that this segment uses.  This can
    be different than the domain returned by this->Domain().
  */
  MYON_Interval EdgeDomain() const;

  /*
  Returns:
    subdomain of the trim that this segment uses.  This can
    be different than the domain returned by this->Domain().
  */
  MYON_Interval TrimDomain() const;


  // m_object_id = id of a brep or curve object in Rhino
  MYON_UUID m_object_id; 
  // When the Rhino object is a brep, m_component_index
  // refers to either an edge or a trim.
  MYON_COMPONENT_INDEX m_component_index;
  // corresponding domain of the edge - see note below
  MYON_Interval m_edge_domain = MYON_Interval::EmptyInterval;  
  // corresponding domain of the trim - see note below
  MYON_Interval m_trim_domain = MYON_Interval::EmptyInterval;   


  // When m_component_index refers to an MYON_BrepTrim, there
  // are four domains and 4 classes derived from MYON_Curve
  // that play a role in the polyedge segment.  It is possible
  // for all 4 of these domains to be different.
  //
  // "this" MYON_PolyEdgeSegment is an MYON_ProxyCurve.  The
  // evaluation domain of "this" is 
  //   = this->Domain()
  //   = MYON_ProxyCurve::m_this_domain
  //
  // MYON_ProxyCurve::m_real_curve points to the curve in the
  // c3 = MYON_Brep::m_C3[edge.m_c3i].  "this" is a proxy for some 
  // sub-interval of c3.
  //   = this->ProxyCurveDomain()
  //   = MYON_ProxyCurve::m_real_curve_domain
  //
  // The edge, an MYON_BrepEdge, is also a proxy based on c3,
  // and the edge's evaluation domain is edge.m_this_domain.  
  // MYON_PolyEdgeSegment::m_edge_domain records the increasing
  // subinterval of edge.m_this_domain that corresponds
  // to the portion of c3 "this" is using.
  // 
  // The trim, an MYON_BrepTrim, is a proxy based on a curve
  // in MYON_Brep::m_C2[].  Some portion of the trim corresponds
  // to the portion of the edge we are using.  m_trim_domain
  // is an increasing, possible subinterval, of the trim's domain
  // MYON_BrepTrim::m_this_domain.

  // Runtime helpers
  const void* m_parent_object; // CRhinoBrepObject or CRhinoCurveObject
  const MYON_Brep* m_brep;
  const MYON_BrepTrim* m_trim;  // 2d trim in m_brep
  const MYON_BrepEdge* m_edge;  // 3d edge in m_brep
  const MYON_BrepFace* m_face;
  const MYON_Surface* m_surface;


private:
  friend class MYON_PolyEdgeCurve;
  void ClearEvalCacheHelper();

  // parameter evaluation cache
  double m_t;
  double m_edge_t;
  double m_trim_t;
  double m_srf_uv[2];
  int m_trim_hint;
  int m_edge_hint;

  // surface evaluation cache
  int m_evsrf_hint[2];
  double m_evsrf_uv[2];
  MYON_3dPoint m_evsrf_pt;
  MYON_3dVector m_evsrf_du;
  MYON_3dVector m_evsrf_dv;
  MYON_3dVector m_evsrf_duu;
  MYON_3dVector m_evsrf_duv;
  MYON_3dVector m_evsrf_dvv;
  MYON_3dVector m_evsrf_tan;

  void Init();
};

#endif

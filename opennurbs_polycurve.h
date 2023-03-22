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

////////////////////////////////////////////////////////////////
//
//   Definition of poly curve (composite curve)
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_POLYCURVE_INC_)
#define OPENNURBS_POLYCURVE_INC_

/*
 Description: 
		An MYON_PolyCurve is an MYON_Curve represented by a sequence of 
	contiguous MYON_Curve segments.    A valid polycurve is represented 
	by an array m_segment of Count()>=1 curve objects	and a strictly
	increasing array m_t of Count()+1 parameter values.  The i-th 
	curve segment,  when considered as part of the polycurve, is affinely 
	reparamaterized from m_t[i] to m_t[i+1], i.e., m_segment[i].Domain()[0] 
	is mapped to 	m_t[i] and m_segment[i].Domain()[1] is mapped to m_t[i+1]. 
*/
class MYON_CLASS MYON_PolyCurve : public MYON_Curve
{
  MYON_OBJECT_DECLARE(MYON_PolyCurve);

public:
  MYON_PolyCurve() MYON_NOEXCEPT;
  virtual ~MYON_PolyCurve();
  MYON_PolyCurve(const MYON_PolyCurve&);
	MYON_PolyCurve& operator=(const MYON_PolyCurve&);

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_PolyCurve( MYON_PolyCurve&& ) MYON_NOEXCEPT;

  // The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
  // which could throw exceptions.  See the implementation of
  // MYON_Object::operator=(MYON_Object&&) for details.
  MYON_PolyCurve& operator=( MYON_PolyCurve&& );
#endif

public:
  // virtual MYON_Object::DestroyRuntimeCache override
  void DestroyRuntimeCache( bool bDelete = true ) override;

  MYON_PolyCurve( int ); // int = initial capacity - use when a good estimate
                        // of the number of segments is known.

  void Destroy();


  void EmergencyDestroy(); // call if memory used by MYON_PolyCurve becomes invalid

  
  /////////////////////////////////////////////////////////////////
  // MYON_Object overrides

  // virtual MYON_Object::SizeOf override
  unsigned int SizeOf() const override;

  // virtual MYON_Object::DataCRC override
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  /*
  Description:
    Tests an object to see if its data members are correctly
    initialized.
  Parameters:
    bAllowGaps - [in]
      If true, gaps are allowed between polycurve segments.
      If false, gaps are not allowed between polycurve segments.
    text_log - [in] if the object is not valid and text_log
        is not nullptr, then a brief englis description of the
        reason the object is not valid is appened to the log.
        The information appended to text_log is suitable for 
        low-level debugging purposes by programmers and is 
        not intended to be useful as a high level user 
        interface tool.
  Returns:
    @untitled table
    true     object is valid
    false    object is invalid, uninitialized, etc.
  Remarks:
    Overrides virtual MYON_Object::IsValid
  */
  bool IsValid( bool bAllowGaps, MYON_TextLog* text_log ) const;


  void Dump( MYON_TextLog& ) const override; // for debugging

  bool Write(
         MYON_BinaryArchive&  // open binary file
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // open binary file
       ) override;

  /////////////////////////////////////////////////////////////////
  // MYON_Geometry overrides

  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  // virtual MYON_Geometry GetTightBoundingBox override		
  bool GetTightBoundingBox( class MYON_BoundingBox& tight_bbox, bool bGrowBox = false, const class MYON_Xform* xform = nullptr ) const override;

  bool Transform( 
         const MYON_Xform&
         ) override;

  // virtual MYON_Geometry::IsDeformable() override
  bool IsDeformable() const override;

  // virtual MYON_Geometry::MakeDeformable() override
  bool MakeDeformable() override;

  bool SwapCoordinates(
        int, int        // indices of coords to swap
        ) override;

  // remove "fuzz" in m_t[] domain array that is in some older files.
  // Called in Read() and in brep edge merging.  
  void SanitizeDomain();


  // virtual MYON_Geometry override
  bool EvaluatePoint( const class MYON_ObjRef& objref, MYON_3dPoint& P ) const override;

  /////////////////////////////////////////////////////////////////
  // MYON_Curve overrides

  MYON_Curve* DuplicateCurve() const override;

  MYON_Interval Domain() const override;

  // Description:
  //   virtual MYON_Curve::SetDomain override.
  //   Set the domain of the curve
  // Parameters:
  //   t0 - [in]
  //   t1 - [in] new domain will be [t0,t1]
  // Returns:
  //   true if successful.
  bool SetDomain(
        double t0, 
        double t1 
        ) override;

  bool ChangeDimension(
          int desired_dimension
          ) override;

  /*
  Description:
    If this curve is closed, then modify it so that
    the start/end point is at curve parameter t.
  Parameters:
    t - [in] curve parameter of new start/end point.  The
             returned curves domain will start at t.
  Returns:
    true if successful.
  Remarks:
    Overrides virtual MYON_Curve::ChangeClosedCurveSeam
  */
  bool ChangeClosedCurveSeam( 
            double t 
            ) override;

  int SpanCount() const override; // number of smooth spans in curve

  bool GetSpanVector( // span "knots" 
         double* // array of length SpanCount() + 1 
         ) const override; // 

  int Degree( // returns maximum algebraic degree of any span 
                  // ( or a good estimate if curve spans are not algebraic )
    ) const override; 

  bool IsLinear( // true if curve locus is a line segment between
                 // between specified points
        double = MYON_ZERO_TOLERANCE // tolerance to use when checking linearity
        ) const override;

  /*
  Description:
    Several types of MYON_Curve can have the form of a polyline including
    a degree 1 MYON_NurbsCurve, an MYON_PolylineCurve, and an MYON_PolyCurve
    all of whose segments are some form of polyline.  IsPolyline tests
    a curve to see if it can be represented as a polyline.
  Parameters:
    pline_points - [out] if not nullptr and true is returned, then the
        points of the polyline form are returned here.
    t - [out] if not nullptr and true is returned, then the parameters of
        the polyline points are returned here.
  Returns:
    @untitled table
    0        curve is not some form of a polyline
    >=2      number of points in polyline form
  */
  int IsPolyline(
        MYON_SimpleArray<MYON_3dPoint>* pline_points = nullptr,
        MYON_SimpleArray<double>* pline_t = nullptr
        ) const override;

  bool IsArc( // MYON_Arc.m_angle > 0 if curve locus is an arc between
              // specified points
        const MYON_Plane* = nullptr, // if not nullptr, test is performed in this plane
        MYON_Arc* = nullptr, // if not nullptr and true is returned, then arc parameters
                         // are filled in
        double = MYON_ZERO_TOLERANCE    // tolerance to use when checking
        ) const override;

  bool IsPlanar(
        MYON_Plane* = nullptr, // if not nullptr and true is returned, then plane parameters
                           // are filled in
        double = MYON_ZERO_TOLERANCE    // tolerance to use when checking
        ) const override;

  bool IsInPlane(
        const MYON_Plane&, // plane to test
        double = MYON_ZERO_TOLERANCE    // tolerance to use when checking
        ) const override;

  bool IsClosed(  // true if curve is closed (either curve has
        void      // clamped end knots and euclidean location of start
        ) const override;  // CV = euclidean location of end CV, or curve is
                  // periodic.)

  bool IsPeriodic(  // true if curve is a single periodic segment
        void 
        ) const override;
  
  /*
  Description:
    Search for a derivatitive, tangent, or curvature discontinuity.
  Parameters:
    c - [in] type of continity to test for.  If MYON::continuity::C1_continuous
    t0 - [in] search begins at t0
    t1 - [in] (t0 < t1) search ends at t1
    t - [out] if a discontinuity is found, the *t reports the
          parameter at the discontinuity.
    hint - [in/out] if GetNextDiscontinuity will be called repeatedly,
       passing a "hint" with initial value *hint=0 will increase the speed
       of the search.       
    dtype - [out] if not nullptr, *dtype reports the kind of discontinuity
        found at *t.  A value of 1 means the first derivative or unit tangent
        was discontinuous.  A value of 2 means the second derivative or
        curvature was discontinuous.
    cos_angle_tolerance - [in] default = cos(1 degree) Used only when
        c is MYON::continuity::G1_continuous or MYON::continuity::G2_continuous.  If the cosine
        of the angle between two tangent vectors 
        is <= cos_angle_tolerance, then a G1 discontinuity is reported.
    curvature_tolerance - [in] (default = MYON_SQRT_EPSILON) Used only when
        c is MYON::continuity::G2_continuous or MYON::continuity::Gsmooth_continuous.  
        MYON::continuity::G2_continuous:
          If K0 and K1 are curvatures evaluated
          from above and below and |K0 - K1| > curvature_tolerance,
          then a curvature discontinuity is reported.
        MYON::continuity::Gsmooth_continuous:
          If K0 and K1 are curvatures evaluated from above and below
          and the angle between K0 and K1 is at least twice angle tolerance
          or ||K0| - |K1|| > (max(|K0|,|K1|) > curvature_tolerance,
          then a curvature discontinuity is reported.
  Returns:
    true if a discontinuity was found on the interior of the interval (t0,t1).
  Remarks:
    Overrides MYON_Curve::GetNextDiscontinuity.
  */
  bool GetNextDiscontinuity( 
                  MYON::continuity c,
                  double t0,
                  double t1,
                  double* t,
                  int* hint=nullptr,
                  int* dtype=nullptr,
                  double cos_angle_tolerance=MYON_DEFAULT_ANGLE_TOLERANCE_COSINE,
                  double curvature_tolerance=MYON_SQRT_EPSILON
                  ) const override;

  /*
  Description:
    Test continuity at a curve parameter value.
  Parameters:
    c - [in] continuity to test for
    t - [in] parameter to test
    hint - [in] evaluation hint
    point_tolerance - [in] if the distance between two points is
        greater than point_tolerance, then the curve is not C0.
    d1_tolerance - [in] if the difference between two first derivatives is
        greater than d1_tolerance, then the curve is not C1.
    d2_tolerance - [in] if the difference between two second derivatives is
        greater than d2_tolerance, then the curve is not C2.
    cos_angle_tolerance - [in] default = cos(1 degree) Used only when
        c is MYON::continuity::G1_continuous or MYON::continuity::G2_continuous.  If the cosine
        of the angle between two tangent vectors 
        is <= cos_angle_tolerance, then a G1 discontinuity is reported.
    curvature_tolerance - [in] (default = MYON_SQRT_EPSILON) Used only when
        c is MYON::continuity::G2_continuous or MYON::continuity::Gsmooth_continuous.  
        MYON::continuity::G2_continuous:
          If K0 and K1 are curvatures evaluated
          from above and below and |K0 - K1| > curvature_tolerance,
          then a curvature discontinuity is reported.
        MYON::continuity::Gsmooth_continuous:
          If K0 and K1 are curvatures evaluated from above and below
          and the angle between K0 and K1 is at least twice angle tolerance
          or ||K0| - |K1|| > (max(|K0|,|K1|) > curvature_tolerance,
          then a curvature discontinuity is reported.
  Returns:
    true if the curve has at least the c type continuity at the parameter t.
  Remarks:
    Overrides MYON_Curve::IsContinuous.
  */
  bool IsContinuous(
    MYON::continuity c,
    double t, 
    int* hint = nullptr,
    double point_tolerance=MYON_ZERO_TOLERANCE,
    double d1_tolerance=MYON_ZERO_TOLERANCE,
    double d2_tolerance=MYON_ZERO_TOLERANCE,
    double cos_angle_tolerance=MYON_DEFAULT_ANGLE_TOLERANCE_COSINE,
    double curvature_tolerance=MYON_SQRT_EPSILON
    ) const override;

  bool Reverse() override;       // reverse parameterizatrion
                        // Domain changes from [a,b] to [-b,-a]

  /*
  Description:
    Force the curve to start at a specified point.
  Parameters:
    start_point - [in]
  Returns:
    true if successful.
  Remarks:
    Some start points cannot be moved.  Be sure to check return
    code.
  See Also:
    MYON_Curve::SetEndPoint
    MYON_Curve::PointAtStart
    MYON_Curve::PointAtEnd
  */
  // virtual
  bool SetStartPoint(
          MYON_3dPoint start_point
          ) override;

  /*
  Description:
    Force the curve to end at a specified point.
  Parameters:
    end_point - [in]
  Returns:
    true if successful.
  Remarks:
    Some end points cannot be moved.  Be sure to check return
    code.
  See Also:
    MYON_Curve::SetStartPoint
    MYON_Curve::PointAtStart
    MYON_Curve::PointAtEnd
  */
  //virtual
  bool SetEndPoint(
          MYON_3dPoint end_point
          ) override;

  bool Evaluate( // returns false if unable to evaluate
         double,         // evaluation parameter
         int,            // number of derivatives (>=0)
         int,            // array stride (>=Dimension())
         double*,        // array of length stride*(ndir+1)
         int = 0,        // optional - determines which side to evaluate from
                         //         0 = default
                         //      <  0 to evaluate from below, 
                         //      >  0 to evaluate from above
         int* = 0        // optional - evaluation hint (int) used to speed
                         //            repeated evaluations
         ) const override;


  // Description:
  //   virtual MYON_Curve::Trim override.
  //   Removes portions of the curve outside the specified interval.
  // Parameters:
  //   domain - [in] interval of the curve to keep.  Portions of the
  //      curve before curve(domain[0]) and after curve(domain[1]) are
  //      removed.
  // Returns:
  //   true if successful.
  bool Trim(
    const MYON_Interval& domain
    ) override;

  // Description:
  //   Where possible, analytically extends curve to include domain.
  // Parameters:
  //   domain - [in] if domain is not included in curve domain, 
  //   curve will be extended so that its domain includes domain.  
  //   Will not work if curve is closed. Original curve is identical
  //   to the restriction of the resulting curve to the original curve domain, 
  // Returns:
  //   true if successful.
  bool Extend(
    const MYON_Interval& domain
    ) override;

  // Description:
  //   virtual MYON_Curve::Split override.
  //   Divide the curve at the specified parameter.  The parameter
  //   must be in the interior of the curve's domain.  The pointers
  //   passed to Split must either be nullptr or point to an MYON_Curve
  //   object of the same of the same type.  If the pointer is nullptr,
  //   then a curve will be created in Split().  You may pass "this"
  //   as one of the pointers to Split().
  // Parameters:
  //   t - [in] parameter in interval Domain().
  //   left_side - [out] left portion of curve
  //   right_side - [out] right portion of curve
  // Example:
  //   For example, if crv were an MYON_NurbsCurve, then
  //
  //     MYON_NurbsCurve right_side;
  //     crv.Split( crv.Domain().Mid() &crv, &right_side );
  //
  //   would split crv at the parametric midpoint, put the left side
  //   in crv, and return the right side in right_side.
  bool Split(
      double t,    // t = curve parameter to split curve at
      MYON_Curve*& left_side, // left portion returned here
      MYON_Curve*& right_side // right portion returned here
    ) const override;

  int GetNurbForm( // returns 0: unable to create NURBS representation
                   //            with desired accuracy.
                   //         1: success - returned NURBS parameterization
                   //            matches the curve's to wthe desired accuracy
                   //         2: success - returned NURBS point locus matches
                   //            the curve's to the desired accuracy but, on
                   //            the interior of the curve's domain, the 
                   //            curve's parameterization and the NURBS
                   //            parameterization may not match to the 
                   //            desired accuracy.
        MYON_NurbsCurve&,
        double = 0.0,
        const MYON_Interval* = nullptr     // OPTIONAL subdomain of polycurve
        ) const override;

  int HasNurbForm( // returns 0: unable to create NURBS representation
                   //            with desired accuracy.
                   //         1: success - returned NURBS parameterization
                   //            matches the curve's to wthe desired accuracy
                   //         2: success - returned NURBS point locus matches
                   //            the curve's to the desired accuracy but, on
                   //            the interior of the curve's domain, the 
                   //            curve's parameterization and the NURBS
                   //            parameterization may not match to the 
                   //            desired accuracy.
        ) const override;

  // virtual MYON_Curve::GetCurveParameterFromNurbFormParameter override
  bool GetCurveParameterFromNurbFormParameter(
        double, // nurbs_t
        double* // curve_t
        ) const override;

  // virtual MYON_Curve::GetNurbFormParameterFromCurveParameter override
  bool GetNurbFormParameterFromCurveParameter(
        double, // curve_t
        double* // nurbs_t
        ) const override;

  /////////////////////////////////////////////////////////////////
  // Interface

  int Count() const; // number of segment curves

  // These operator[] functions return nullptr if index is out of range
  MYON_Curve* operator[](int) const;

  /*
  Description:
    Returns a pointer to a segment curve.
  Parameters:
    segment_index - [in] 0 based index  (0 <= segment_index < Count() )
  Returns:
    A pointer to the segment curve.  Returns nullptr if segment_index < 0
    or segment_index >= Count().
  */
  MYON_Curve* SegmentCurve(
    int segment_index
    ) const;

  /*
  Description:
    Converts a polycurve parameter to a segment curve parameter.
  Parameters:
    polycurve_parameter - [in] 
  Returns:
    Segment curve evaluation parameter or MYON_UNSET_VALUE if the
    segment curve parameter cannot be computed.
  See Also:
    MYON_PolyCurve::PolyCurveParameter
  */
  double SegmentCurveParameter(
    double polycurve_parameter
    ) const;

  /*
  Description:
    Converts a segment curve parameter to a polycurve parameter.
  Parameters:
    segment_index - [in]
    segmentcurve_parameter - [in] 
  Returns:
    Polycurve evaluation parameter or MYON_UNSET_VALUE if the
    polycurve curve parameter cannot be computed.
  See Also:
    MYON_PolyCurve::SegmentCurveParameter
  */
  double PolyCurveParameter(
    int segment_index,
    double segmentcurve_parameter
    ) const;

  /*
  Description:
    Returns the polycurve subdomain assigned to a segment curve.
  Parameters:
    segment_index - [in] 0 based index  (0 <= segment_index < Count() )
  Returns:
    The polycurve subdomain assigned to a segment curve.
    Returns ([MYON_UNSET_VALUE,MYON_UNSET_VALUE) if segment_index < 0  
    or segment_index >= Count().
  */
  MYON_Interval SegmentDomain( 
    int segment_index
    ) const;

  /*
  Description:
    Find the segment used for evaluation at polycurve_parameter.
  Parameters:
    polycurve_parameter - [in]
  Returns:
    index of the segment used for evaluation at polycurve_parameter.
    If polycurve_parameter < Domain.Min(), then 0 is returned.
    If polycurve_parameter > Domain.Max(), then Count()-1 is returned.
  */
  int SegmentIndex(
    double polycurve_parameter
    ) const;

  /*
  Description:
    Find the segments with support on sub_domain.
  Parameters:
    sub_domain - [in] increasing interval
    segment_index0 - [out] 
    segment_index1 - [out] segments with index i where
      *segment_index0 <= i < *segment_index1 are the segments
      with support on the sub_domain
  Returns:
    number of segments with support on sub_domain.
  */
  int SegmentIndex(
    MYON_Interval sub_domain,
    int* segment_index0,
    int* segment_index1
    ) const;

  MYON_Curve* FirstSegmentCurve() const; // returns nullptr if count = 0

  MYON_Curve* LastSegmentCurve() const;  // returns nullptr if count = 0

  /*
  Description:
    Search the curve for gaps between the sub curve segments. 
  Parameters:
    segment_index0 - [in]
      The search for gaps starts at with the comparing
      the end of segment[segment_index0] and the start of
      segment[segment_index0+1].
  Returns:
    0:     
      No gaps were found.
    i > segment_index0:
      The end of polycuve segment[i-1] is not coincident
      with the start of polycurve segment[i].
  */
  int FindNextGap( int segment_index0 ) const;

  /*
  Description:
    Determine if there is a gap between the end of 
    segment[segment_index] and the start of segment[segment_index+1].
  Parameters:
    segment_index - [in]
      >= 0
  Returns:
    true: 
      segment_index was valid and there is a gap between
      the end of segment[segment_index] and the start of
      segment[segment_index+1].
  */
  bool HasGapAt( int segment_index ) const;
  
  MYON_DEPRECATED_MSG("Replace calls to HasGap() with FindNextGap(0)")
  int HasGap() const;

  /*
  Description:
    Modify the one or both locations at the end of 
    segment[gap_index-1] and the start of segment[gap_index]
    so they are coindicent.  
  Parameters:
    gap_index - [in] 1 <= gap_index < Count()
      If the locations at the end of segment[gap_index-1] and 
      the start of segment[gap_index] are not identical, then
      an attempt is made to modify the segments so these
      locations are closer.
    ends_to_modify - [in]
      0: (suggested)
        The code will decide what segments to modify.
      1: 
        modify the end location of segment[gap_index-1]
      2:
        modify the start location of segment[gap_index]
  Returns:
    True if a modification was performed and HasGap(gap_index-1)
    returns 0 after the modification.
    False if no modification was performed because there
    was no gap or because one could not be performed.
  Remarks:
    Note that passing the return value from FindNextGap() will 
    close the gap found by FindNextGap().
  */
  bool CloseGap( int gap_index, int segments_to_modify );

  /*
  Description:
    Searches for and closes all gaps that can be found.
  Returns:
    Number of gaps that were closed.
  */
  int CloseGaps();

  void Reserve( int ); // make sure capacity is at least the specified count

  // MYON_Curve pointers added with Prepend(), Append(), PrependAndMatch(), AppendANdMatch(),and Insert() are deleted
  // by ~MYON_PolyCurve(). Use MYON_CurveProxy( MYON_Curve*) if you want
  // the original curve segment to survive ~MYON_PolyCurve().
  bool Prepend( MYON_Curve* ); // Prepend curve.
  bool Append( MYON_Curve* );  // Append curve.
  bool Insert( 
           int, // segment_index,
           MYON_Curve*
           );

  //PrependAndMatch() and AppendAndMatch() return false if this->IsCLosed() or 
  //this->Count() > 0 and curve is closed
  bool PrependAndMatch(MYON_Curve*); //Prepend and match end of curve to start of polycurve
  bool AppendAndMatch(MYON_Curve*);  //Append and match start of curve to end of polycurve

  bool Remove(); // delete last segment and reduce count by 1
  bool Remove( int ); // delete specified segment and reduce count by 1

  //////////
  // Use the HarvestSegment() function when you want to prevent a
  // segment from being destroyed by ~MYON_PolyCurve().  HarvestSegment()
  // replaces the polycurve segment with a nullptr.  Count() and parameter
  // information remains unchanged.
  MYON_Curve* HarvestSegment( int );

	/*
  Returns:
    True if a curve in the m_segment[] array is an MYON_PolyCurve.
  */
  bool IsNested() const;

  /*
  Description:
    If this has a single segment, return that single segment with user_data copied, 
    reversed if necessary so the sense of the result is the same as this,
    and domain adjusted to match this->Domain. If the single segment is a polycurve,
    RemoveNesting should be called before calling ExplodeSingleSegmentCurve.
  Returns:
    NULL if not a single span polycurve
    The single span, adjusted as described above;
  */

  MYON_Curve* ExplodeSingleSegmentCurve() const;

  /* 
  Description:
    Removes the nested of polycurves. The result will have not
    have an  MYON_PolyCurve  as a segment but will have identical
    locus and parameterization.
  Returns:
    True if a nested polycurve was removed.  False
    if no nested polycurves were found.
  */
  bool RemoveNesting();

  MYON_DEPRECATED_MSG("Use RemoveNesting(). RemoveNestingEx() will be removed in the next release.")
	bool RemoveNestingEx();

  /* 
  Returns:
    True if the domains of the curves in the m_segment[] array exactly
    match the domains of the segments specified in the m_t[] array.
    Put another way, returns true if SegmentDomain(i) = SegmentCurve(i).Domain()
    for every segment index.
  */
	bool HasSynchronizedSegmentDomains() const;

  /* 
  Description:
    Sets the domain of the curve int the m_segment[] array to exactly
    match the domain defined in the m_t[] array.  This is not required,
    but can simplify some coding situations.
  Returns:
    True if at least one segment was reparameterized. False if no
    changes were made.
  */
	bool SynchronizeSegmentDomains();




	//////////
	// Expert user function  
	//   Sets the m_segment[index] to crv. 
	void SetSegment(int index, MYON_Curve* crv);

	//////////
  /*
  Description:
	  Expert user function to set the m_t[] array.
  Parameters:
    t - [in] increasing array of SegmentCount()+1 parameters.
  Returns
    True if successful.
  */
  bool SetParameterization( const double* t );

/*
	Description:
		Lookup a parameter in the m_t array, optionally using a built in snap tolerance to 
		snap a parameter value to an element of m_t.
	Parameters:
		t    - [in]	  	parameter
		index -[out]	index into m_t such that if the function returns true then t is equal 
									to, or is within tolerance of m_t[index]. 
					  			if function returns false then the value of index is

									 @table  
												condition									value of index
						  			t<m_t[0] or m_t is empty				-1
										m_t[i] < t < m_t[i+1]				i for 0<=i<=m_t.Count()-2
										t>m_t[ m_t.Count()-1]				m_t.Count()-1
									
		bEnableSnap -[in]  if true use tolerance when comparing to m_t values 
	Returns		
		true if the t is exactly equal to, or within tolerance of
		(only if bEnableSnap==true) m_t[index]. 
*/ 
	bool ParameterSearch(double t, int& index, bool bEnableSnap) const;

  /*
  Returns:
    Reference to m_segment.
  */
  const MYON_CurveArray& SegmentCurves() const;

  /*
  Returns:
    Reference to m_t.
  */
  const MYON_SimpleArray<double>& SegmentParameters() const;

  /////////////////////////////////////////////////////////////////
  // Implementation
private:
  // The curves in this array are deleted by ~MYON_PolyCurve().
  // Use MYON_CurveProxy classes if you don't want MYON_PolyCurve()
  // to destroy the curve.

  MYON_CurveArray m_segment;  // array of pointers to curves
                             // all have the same dimension
                             // and are contiguous to tolerance

  MYON_SimpleArray<double> m_t; // MYON_PolyCurve segment parameterizations
};


#endif

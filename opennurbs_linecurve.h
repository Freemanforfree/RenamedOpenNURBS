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

#if !defined(MYON_GEOMETRY_CURVE_LINE_INC_)
#define MYON_GEOMETRY_CURVE_LINE_INC_

class MYON_LineCurve;
class MYON_CLASS MYON_LineCurve : public MYON_Curve
{
  MYON_OBJECT_DECLARE(MYON_LineCurve);

public:
  MYON_LineCurve() MYON_NOEXCEPT;
  virtual ~MYON_LineCurve();
  MYON_LineCurve(const MYON_LineCurve&);
	MYON_LineCurve& operator=(const MYON_LineCurve&);

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_LineCurve( MYON_LineCurve&& ) MYON_NOEXCEPT;

  // The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
  // which could throw exceptions.  See the implementation of
  // MYON_Object::operator=(MYON_Object&&) for details.
  MYON_LineCurve& operator=( MYON_LineCurve&& );
#endif

  MYON_LineCurve(const MYON_2dPoint&,const MYON_2dPoint&); // creates a 2d line curve
  MYON_LineCurve(const MYON_3dPoint&,const MYON_3dPoint&); // creates a 3d line curve
  MYON_LineCurve(const MYON_Line&);
  MYON_LineCurve(const MYON_Line&,
                double,double    // domain
                );



	MYON_LineCurve& operator=(const MYON_Line&);
  
  /////////////////////////////////////////////////////////////////
  // MYON_Object overrides

  // virtual MYON_Object::SizeOf override
  unsigned int SizeOf() const override;

  // virtual MYON_Object::DataCRC override
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;

  /*
      Returns true if !MYON_ArePointsCoincident(Start,End) and
      m_t.IsIncreasing().
  */
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

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


  /////////////////////////////////////////////////////////////////
  // MYON_Curve overrides

  MYON_Interval Domain() const override;

  // Description:
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
  //virtual
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
    Force the curve to start at a specified point.
  Parameters:
    start_point - [in]
  Returns:
    true if successful.
  Remarks:
    Some end points cannot be moved.  Be sure to check return
    code.
  See Also:
    MYON_Curve::SetEndPoint
    MYON_Curve::PointAtStart
    MYON_Curve::PointAtEnd
  */
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
  bool SetEndPoint(
          MYON_3dPoint end_point
          ) override;

  bool Reverse() override;       // reverse parameterizatrion
                        // Domain changes from [a,b] to [-b,-a]

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
  //   Original curve is identical
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

  // Description:
  //   virtual MYON_Curve::GetNurbForm override.
  //   Get a NURBS curve representation of this curve.
  // Parameters:
  //   nurbs_curve - [out] NURBS representation returned here
  //   tolerance - [in] tolerance to use when creating NURBS
  //       representation.
  //   subdomain - [in] if not nullptr, then the NURBS representation
  //       for this portion of the curve is returned.
  // Returns:
  //   0   unable to create NURBS representation
  //       with desired accuracy.
  //   1   success - returned NURBS parameterization
  //       matches the curve's to wthe desired accuracy
  //   2   success - returned NURBS point locus matches
  //       the curve's to the desired accuracy but, on
  //       the interior of the curve's domain, the 
  //       curve's parameterization and the NURBS
  //       parameterization may not match to the 
  //       desired accuracy.
  int GetNurbForm(
        MYON_NurbsCurve&,
        double = 0.0,
        const MYON_Interval* = nullptr
        ) const override;

  // Description:
  //   virtual MYON_Curve::HasNurbForm override.
  //   Does a NURBS curve representation of this curve exist.
  // Parameters:
  // Returns:
  //   0   unable to create NURBS representation
  //       with desired accuracy.
  //   1   success - returned NURBS parameterization
  //       matches the curve's to wthe desired accuracy
  //   2   success - returned NURBS point locus matches
  //       the curve's to the desired accuracy but, on
  //       the interior of the curve's domain, the 
  //       curve's parameterization and the NURBS
  //       parameterization may not match to the 
  //       desired accuracy.
  int HasNurbForm(
        ) const override;

  // Description:
  //   virtual MYON_Curve::GetCurveParameterFromNurbFormParameter override.
  //   Convert a NURBS curve parameter to a curve parameter
  //
  // Parameters:
  //   nurbs_t - [in] nurbs form parameter
  //   curve_t - [out] curve parameter
  //
  // Remarks:
  //   If GetNurbForm returns 2, this function converts the curve
  //   parameter to the NURBS curve parameter.
  //
  // See Also:
  //   MYON_Curve::GetNurbForm, MYON_Curve::GetNurbFormParameterFromCurveParameter
  //virtual
  bool GetCurveParameterFromNurbFormParameter(
        double nurbs_t,
        double* curve_t
        ) const override;

  // Description:
  //   virtual MYON_Curve::GetNurbFormParameterFromCurveParameter override.
  //   Convert a curve parameter to a NURBS curve parameter.
  //
  // Parameters:
  //   curve_t - [in] curve parameter
  //   nurbs_t - [out] nurbs form parameter
  //
  // Remarks:
  //   If GetNurbForm returns 2, this function converts the curve
  //   parameter to the NURBS curve parameter.
  //
  // See Also:
  //   MYON_Curve::GetNurbForm, MYON_Curve::GetCurveParameterFromNurbFormParameter
  //virtual
  bool GetNurbFormParameterFromCurveParameter(
        double curve_t,
        double* nurbs_t
        ) const override;

  /////////////////////////////////////////////////////////////////
  // Interface

  MYON_Line m_line;
  MYON_Interval m_t;  // domain
  int      m_dim;   // 2 or 3 (2 so MYON_LineCurve can be uses as a trimming curve)
};


#endif

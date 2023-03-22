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

#if !defined(MYON_GEOMETRY_CURVE_ARC_INC_)
#define MYON_GEOMETRY_CURVE_ARC_INC_


/*
Description:
  MYON_ArcCurve is used to represent arcs and circles.
  MYON_ArcCurve.IsCircle() returns true if the curve
  is a complete circle.
Remarks:
	- An MYON_ArcCurve is a subcurve of a circle, with a 
	  constant speed parameterization. The parameterization is
	  an affine linear reparameterzation of the underlying arc
	  m_arc onto the domain m_t.
	- A valid MYON_ArcCurve has  a strictly increasing Domain() and
		MYON_ArePointsCoincident(Start,End) == IsCircle()
*/
class MYON_CLASS MYON_ArcCurve : public MYON_Curve
{
  MYON_OBJECT_DECLARE(MYON_ArcCurve);

public:
  MYON_ArcCurve() MYON_NOEXCEPT;
  virtual ~MYON_ArcCurve();
  MYON_ArcCurve(const MYON_ArcCurve&);
	MYON_ArcCurve& operator=(const MYON_ArcCurve&);

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_ArcCurve( MYON_ArcCurve&& ) MYON_NOEXCEPT;

  // The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
  // which could throw exceptions.  See the implementation of
  // MYON_Object::operator=(MYON_Object&&) for details.
  MYON_ArcCurve& operator=( MYON_ArcCurve&& );
#endif

  // virtual MYON_Object::SizeOf override
  unsigned int SizeOf() const override;

  // virtual MYON_Object::DataCRC override
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;

  /*
  Description:
    Create an arc curve with domain (0,arc.Length()).
  */
  MYON_ArcCurve( 
      const MYON_Arc& arc 
      );

  /*
  Description:
    Create an arc curve with domain (t0,t1)
  */
  MYON_ArcCurve(
      const MYON_Arc& arc,
      double t0,
      double t1
      );

  /*
  Description:
    Creates a curve that is a complete circle with 
    domain (0,circle.Length()).
  */
  MYON_ArcCurve(
      const MYON_Circle& circle
      );

  /*
  Description:
    Creates a curve that is a complete circle with domain (t0,t1).
  */
  MYON_ArcCurve(
      const MYON_Circle& circle,
      double t0,
      double t1
      );

  /*
  Description:
    Create an arc curve with domain (0,arc.Length()).
  */
	MYON_ArcCurve& operator=(const MYON_Arc& arc);

  /*
  Description:
    Creates a curve that is a complete circle with 
    domain (0,circle.Length()).
  */
	MYON_ArcCurve& operator=(const MYON_Circle& circle);
  
  /////////////////////////////////////////////////////////////////
  // MYON_Object overrides

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override;

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

  /////////////////////////////////////////////////////////////////
  // MYON_Curve overrides

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

  MYON_Interval Domain() const override;

  bool ChangeDimension(
          int desired_dimension
          ) override;

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

  bool IsArc( // MYON_Arc.m_angle > 0 if curve locus is an arc between
              // specified points
        const MYON_Plane* = nullptr, // if not nullptr, test is performed in this plane
        MYON_Arc* = nullptr, // if not nullptr and true is returned, then arc parameters
                         // are filled in
        double = 0.0    // tolerance to use when checking
        ) const override;

  bool IsPlanar(
        MYON_Plane* = nullptr, // if not nullptr and true is returned, then plane parameters
                           // are filled in
        double = 0.0    // tolerance to use when checking
        ) const override;

  bool IsInPlane(
        const MYON_Plane&, // plane to test
        double = 0.0    // tolerance to use when checking
        ) const override;

  bool IsClosed(  // true if curve is closed (either curve has
        void      // clamped end knots and euclidean location of start
        ) const override;  // CV = euclidean location of end CV, or curve is
                  // periodic.)

  bool IsPeriodic(  // true if curve is a single periodic segment
        void 
        ) const override;
  
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

  bool Trim( const MYON_Interval& ) override;

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

  /*
  Description:
    Splits (divides) the arc at the specified parameter.  
    The parameter must be in the interior of the arc's domain.
    The MYON_Curve pointers passed to MYON_ArcCurve::Split must 
    either be nullptr or point to MYON_ArcCurve objects.
    If a pointer is nullptr, then an MYON_ArcCurve will be created 
    in Split().  You may pass "this" as left_side or right_side.
  Parameters:
    t - [in] parameter to split the curve at in the
             interval returned by Domain().
    left_side - [out] left portion of curve returned here.
       If not nullptr, left_side must point to an MYON_ArcCuve.
    right_side - [out] right portion of curve returned here
       If not nullptr, right_side must point to an MYON_ArcCuve.
  Remarks:
    Overrides virtual MYON_Curve::Split.
  */
  bool Split(
      double t,
      MYON_Curve*& left_side,
      MYON_Curve*& right_side
    ) const override;


  // virtual MYON_Curve::GetNurbForm override
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
        const MYON_Interval* = nullptr     // OPTIONAL subdomain of arc curve
        ) const override;

  // virtual MYON_Curve::HasNurbForm override
  int HasNurbForm( // returns 0: unable to create NURBS representation
                   //            with desired accuracy.
                   //         1: success - NURBS parameterization
                   //            matches the curve's
                   //         2: success - returned NURBS point locus matches
                   //            the curve'sbut, on
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
  
  
  /*
  Description:
    Returns true if this arc curve is a complete circle.
  */
  bool IsCircle() const;

  // Returns:
  //   The arc's radius.
	double Radius() const;

  // Returns:
  //   The arc's subtended angle in radians.
  double AngleRadians() const;

  // Returns:
  //   The arc's subtended angle in degrees.
  double AngleDegrees() const;


  /////////////////////////////////////////////////////////////////

  MYON_Arc m_arc = MYON_Arc::UnitCircle; // default = radius 1 circle in x-y plane
  
  MYON_Interval m_t = MYON_Interval::ZeroToTwoPi;

  // The dimension of a arc curve can be 2 or 3.
  // (2 so MYON_ArcCurve can be used as a trimming curve)
  int m_dim = 3;
};


#endif

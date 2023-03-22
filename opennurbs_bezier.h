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

#if !defined(OPENNURBS_BEZIER_INC_)
#define OPENNURBS_BEZIER_INC_

class MYON_PolynomialCurve;
class MYON_PolynomialSurface;
class MYON_BezierCurve;
class MYON_BezierSurface;
class MYON_TextLog;
class MYON_NurbsCurve;
class MYON_NurbsSurface;
class MYON_X_EVENT;

class MYON_CLASS MYON_PolynomialCurve
{
public:
  MYON_PolynomialCurve();

  // Description:
  //   See MYON_PolynomialCurve::Create.
  // Parameters:
  //   dim - [in] dimension of the curve
  //   bIsRational - [in] true if rational
  //   order - [in] (>=2) order = degree+1
  MYON_PolynomialCurve(
    int dim,
    bool bIsRational,
    int order
    );

  ~MYON_PolynomialCurve();

  MYON_PolynomialCurve(const MYON_PolynomialCurve&);

  MYON_PolynomialCurve(const MYON_BezierCurve&);

  MYON_PolynomialCurve& operator=(const MYON_PolynomialCurve&);

  MYON_PolynomialCurve& operator=(const MYON_BezierCurve&);

  // Description:
  //   Initializes fields and allocates the m_cv array. 
  // Parameters:
  //   dim - [in] dimension of the curve
  //   bIsRational - [in] true if rational
  //   order - [in] (>=2) order = degree+1
  bool Create(
    int dim,
    bool bIsRational,
    int order
    );

  // Description:
  //   Deallocates the m_cv array and sets fields to zero.
  void Destroy();

  // Description:
  //   Evaluate a polynomial curve.
  // Parameters:
  //   t - [in] evaluation parameter ( usually in Domain() ).
  //   der_count - [in] (>=0) number of derivatives to evaluate
  //   v_stride - [in] (>=Dimension()) stride to use for the v[] array
  //   v - [out] array of length (der_count+1)*v_stride
  //       curve(t) is returned in (v[0],...,v[m_dim-1]),
  //       curve'(t) is returned in (v[v_stride],...,v[v_stride+m_dim-1]),
  //       curve"(t) is returned in (v[2*v_stride],...,v[2*v_stride+m_dim-1]),
  //       etc.
  // Returns:
  //   false if unable to evaluate.
  bool Evaluate(
         double t,
         int der_count,
         int v_stride,
         double* v
         ) const;

  // dimension of polynomial curve (1,2, or 3)
  int m_dim;

  // 1 if polynomial curve is rational, 0 if polynomial curve is not rational
  int m_is_rat;

  // order (=degree+1) of polynomial
  int m_order;

  // coefficients ( m_cv.Count() = order of monomial )
  MYON_4dPointArray m_cv;

  // domain of polynomial
  MYON_Interval m_domain;
};

class MYON_CLASS MYON_PolynomialSurface
{
public:
  MYON_PolynomialSurface();
  MYON_PolynomialSurface(
    int,  // dim,
    bool, // true if rational
    int,  // "u" order
    int   // "v" order
    );
  ~MYON_PolynomialSurface();
  MYON_PolynomialSurface(const MYON_PolynomialSurface&);
  MYON_PolynomialSurface(const MYON_BezierSurface&);
  MYON_PolynomialSurface& operator=(const MYON_PolynomialSurface&);
  MYON_PolynomialSurface& operator=(const MYON_BezierSurface&);

  bool Create(
    int,  // dim,
    bool, // true if rational
    int,  // "u" order
    int   // "v" order
    );
  void Destroy();

  bool Evaluate(          // returns false if unable to evaluate
         double s, 
         double t,        // evaluation parameter
         int der_count,   // number of derivatives (>=0)
         int v_stride,    // array stride (>=Dimension())
         double* v        // array of length stride*(ndir+1)*(ndir+2)/2
         ) const;

  int m_dim;    // 1,2, or 3
  int m_is_rat; // 1 if rational, 0 if not rational
  int m_order[2];
  MYON_4dPointArray m_cv; // coefficients ( m_C.Length() = m_order[0]*m_order[1]
                        // coefficient of s^m*t^n = m_cv[m_order[1]*m+n]
  MYON_Interval m_domain[2];
};

class MYON_CLASS MYON_BezierCurve
{
public:

  MYON_BezierCurve();

  // Description:
  //   Creates a bezier with cv memory allocated.
  // Parameters:
  //   dim - [in] (>0) dimension of bezier curve
  //   bIsRational - [in] true for a rational bezier
  //   order - [in] (>=2) order (=degree+1) of bezier curve
  MYON_BezierCurve(
    int dim,
    bool bIsRational,
    int order
    );

  ~MYON_BezierCurve();
  MYON_BezierCurve(const MYON_BezierCurve&);
  MYON_BezierCurve(const MYON_PolynomialCurve&);
  MYON_BezierCurve(const MYON_2dPointArray&); // sets control points
  MYON_BezierCurve(const MYON_3dPointArray&); // sets control points
  MYON_BezierCurve(const MYON_4dPointArray&); // sets control points
  MYON_BezierCurve& operator=(const MYON_BezierCurve&);
  MYON_BezierCurve& operator=(const MYON_PolynomialCurve&);


  MYON_BezierCurve& operator=(const MYON_2dPointArray&); // sets control points
  MYON_BezierCurve& operator=(const MYON_3dPointArray&); // sets control points
  MYON_BezierCurve& operator=(const MYON_4dPointArray&); // sets control points

  bool IsValid() const;

  void Dump( MYON_TextLog& ) const; // for debugging

  // Returns:
  //   Dimension of bezier.
  int Dimension() const;

  // Description:
  //   Creates a bezier with cv memory allocated.
  // Parameters:
  //   dim - [in] (>0) dimension of bezier curve
  //   bIsRational - [in] true for a rational bezier
  //   order - [in] (>=2) order (=degree+1) of bezier curve
  // Returns:
  //   true if successful.
  bool Create(
    int dim,
    bool bIsRational,
    int order
    );

  // Description:
  //   Deallocates m_cv memory.
  void Destroy();

  void EmergencyDestroy(); // call if memory used by MYON_NurbsCurve becomes invalid

  // Description:
  //   Loft a bezier curve through a list of points.
  // Parameters:
  //   points - [in] an array of 2 or more points to interpolate
  // Returns:
  //   true if successful
  // Remarks:
  //   The result has order = points.Count() and the loft uses the 
  //   uniform parameterizaton curve( i/(points.Count()-1) ) = points[i].
  bool Loft( 
    const MYON_3dPointArray& points
    );

  // Description:
  //   Loft a bezier curve through a list of points.
  // Parameters:
  //   pt_dim - [in] dimension of points to interpolate
  //   pt_count - [in] number of points (>=2)
  //   pt_stride - [in] (>=pt_dim) pt[] array stride
  //   pt - [in] array of points
  //   t_stride - [in] (>=1) t[] array stride
  //   t - [in] strictly increasing array of interpolation parameters
  // Returns:
  //   true if successful
  // Remarks:
  //   The result has order = points.Count() and the loft uses the 
  //   parameterizaton curve( t[i] ) = points[i].
  bool Loft(
    int pt_dim,
    int pt_count,
    int pt_stride,
    const double* pt,
    int t_stride,
    const double* t
    );

  // Description:
  //   Gets bounding box.
  // Parameters:
  //   box_min - [out] minimum corner of axis aligned bounding box
  //       The box_min[] array must have size m_dim.
  //   box_max - [out] maximum corner of axis aligned bounding box
  //       The box_max[] array must have size m_dim.
  //   bGrowBox - [in] if true, input box_min/box_max must be set
  //      to valid bounding box corners and this box is enlarged to
  //      be the union of the input box and the bezier's bounding
  //      box.
  // Returns:
  //   true if successful.
  bool GetBBox( // returns true if successful
         double* box_min,
         double* box_max,
         bool bGrowBox = false
         ) const;

  // Description:
  //   Gets bounding box.
  // Parameters:
  //   bbox - [out] axis aligned bounding box returned here.
  //   bGrowBox - [in] if true, input bbox must be a valid
  //      bounding box and this box is enlarged to
  //      be the union of the input box and the 
  //      bezier's bounding box.
  // Returns:
  //   true if successful.
  bool GetBoundingBox(
         MYON_BoundingBox& bbox,
         int bGrowBox = false
         ) const;

  // Description:
  //   Gets bounding box.
  // Returns:
  //   Axis aligned bounding box.
  MYON_BoundingBox BoundingBox() const;

  /*
	Description:
    Get tight bounding box of the bezier.
	Parameters:
		tight_bbox - [in/out] tight bounding box
		bGrowBox -[in]	(default=false)			
      If true and the input tight_bbox is valid, then returned
      tight_bbox is the union of the input tight_bbox and the 
      tight bounding box of the bezier curve.
		xform -[in] (default=nullptr)
      If not nullptr, the tight bounding box of the transformed
      bezier is calculated.  The bezier curve is not modified.
	Returns:
    True if the returned tight_bbox is set to a valid 
    bounding box.
  */
	bool GetTightBoundingBox( 
			MYON_BoundingBox& tight_bbox, 
      bool bGrowBox = false,
			const MYON_Xform* xform = nullptr
      ) const;

  // Description:
  //   Transform the bezier.
  // Parameters:
  //   xform - [in] transformation to apply to bezier
  // Returns:
  //   true if successful.  false if bezier is invalid
  //   and cannot be transformed.
  bool Transform( 
         const MYON_Xform& xform
         );


  // Description:
  //   Rotates the bezier curve about the specified axis.  A positive
  //   rotation angle results in a counter-clockwise rotation
  //   about the axis (right hand rule).
  // Parameters:
  //   sin_angle - [in] sine of rotation angle
  //   cos_angle - [in] sine of rotation angle
  //   rotation_axis - [in] direction of the axis of rotation
  //   rotation_center - [in] point on the axis of rotation
  // Returns:
  //   true if bezier curve successfully rotated
  // Remarks:
  //   Uses MYON_BezierCurve::Transform() function to calculate the result.
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& rotation_axis,
        const MYON_3dPoint& rotation_center
        );

  // Description:
  //   Rotates the bezier curve about the specified axis.  A positive
  //   rotation angle results in a counter-clockwise rotation
  //   about the axis (right hand rule).
  // Parameters:
  //   rotation_angle - [in] angle of rotation in radians
  //   rotation_axis - [in] direction of the axis of rotation
  //   rotation_center - [in] point on the axis of rotation
  // Returns:
  //   true if bezier curve successfully rotated
  // Remarks:
  //   Uses MYON_BezierCurve::Transform() function to calculate the result.
  bool Rotate(
        double rotation_angle,
        const MYON_3dVector& rotation_axis,
        const MYON_3dPoint& rotation_center
        );

  // Description:
  //   Translates the bezier curve along the specified vector.
  // Parameters:
  //   translation_vector - [in] translation vector
  // Returns:
  //   true if bezier curve successfully translated
  // Remarks:
  //   Uses MYON_BezierCurve::Transform() function to calculate the result.
  bool Translate( 
    const MYON_3dVector& translation_vector
    );

  // Description:
  //   Scales the bezier curve by the specified facotor.  The scale is
  //   centered at the origin.
  // Parameters:
  //   scale_factor - [in] scale factor
  // Returns:
  //   true if bezier curve successfully scaled
  // Remarks:
  //   Uses MYON_BezierCurve::Transform() function to calculate the result.
  bool Scale( 
    double scale_factor
    );

  // Returns:
  //   Domain of bezier (always [0,1]).
  MYON_Interval Domain() const;

  // Description:
  //   Reverses bezier by reversing the order
  //   of the control points.
  bool Reverse();

  // Description:
  //   Evaluate point at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   Point (location of curve at the parameter t).
  MYON_3dPoint  PointAt( 
                double t 
                ) const;

  // Description:
  //   Evaluate first derivative at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   First derivative of the curve at the parameter t.
  // Remarks:
  //   No error handling.
  // See Also:
  //   MYON_Curve::Ev1Der
  MYON_3dVector DerivativeAt(
                double t 
                ) const;

  // Description:
  //   Evaluate unit tangent vector at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   Unit tangent vector of the curve at the parameter t.
  // Remarks:
  //   No error handling.
  // See Also:
  //   MYON_Curve::EvTangent
  MYON_3dVector TangentAt(
                double t 
                ) const;

  // Description:
  //   Evaluate the curvature vector at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   curvature vector of the curve at the parameter t.
  // Remarks:
  //   No error handling.
  // See Also:
  //   MYON_Curve::EvCurvature
  MYON_3dVector CurvatureAt(
                double t
                ) const;

  // Description:
  //   Evaluate point at a parameter with error checking.
  // Parameters:
  //   t - [in] evaluation parameter
  //   point - [out] value of curve at t
  // Returns:
  //   false if unable to evaluate.
  bool EvPoint(
         double t,
         MYON_3dPoint& point
         ) const;

  // Description:
  //   Evaluate first derivative at a parameter with error checking.
  // Parameters:
  //   t - [in] evaluation parameter
  //   point - [out] value of curve at t
  //   first_derivative - [out] value of first derivative at t
  // Returns:
  //   false if unable to evaluate.
  bool Ev1Der(
         double t,
         MYON_3dPoint& point,
         MYON_3dVector& first_derivative
         ) const;

  // Description:
  //   Evaluate second derivative at a parameter with error checking.
  // Parameters:
  //   t - [in] evaluation parameter
  //   point - [out] value of curve at t
  //   first_derivative - [out] value of first derivative at t
  //   second_derivative - [out] value of second derivative at t
  // Returns:
  //   false if unable to evaluate.
  bool Ev2Der(
         double t,
         MYON_3dPoint& point,
         MYON_3dVector& first_derivative,
         MYON_3dVector& second_derivative
         ) const;

  /*
  Description:
    Evaluate unit tangent at a parameter with error checking.
  Parameters:
    t - [in] evaluation parameter
    point - [out] value of curve at t
    tangent - [out] value of unit tangent
  Returns:
    false if unable to evaluate.
  See Also:
    MYON_Curve::TangentAt
    MYON_Curve::Ev1Der
  */
  bool EvTangent(
         double t,
         MYON_3dPoint& point,
         MYON_3dVector& tangent
         ) const;

  /*
  Description:
    Evaluate unit tangent and curvature at a parameter with error checking.
  Parameters:
    t - [in] evaluation parameter
    point - [out] value of curve at t
    tangent - [out] value of unit tangent
    kappa - [out] value of curvature vector
  Returns:
    false if unable to evaluate.
  */
  bool EvCurvature(
         double t,
         MYON_3dPoint& point,
         MYON_3dVector& tangent,
         MYON_3dVector& kappa
         ) const;

  // Description:
  //   Evaluate a bezier.
  // Parameters:
  //   t - [in] evaluation parameter (usually 0 <= t <= 1)
  //   der_count - [in] (>=0) number of derivatives to evaluate
  //   v_stride - [in] (>=m_dim) stride to use for the v[] array
  //   v - [out] array of length (der_count+1)*v_stride
  //       bez(t) is returned in (v[0],...,v[m_dim-1]),
  //       bez'(t) is returned in (v[v_stride],...,v[v_stride+m_dim-1]),
  //       bez"(t) is returned in (v[2*v_stride],...,v[2*v_stride+m_dim-1]),
  //       etc.
  // Returns:
  //   true if successful
  bool Evaluate(
         double t,
         int der_count,
         int v_stride,
         double* v
         ) const;

  // Description:
  //   Get MYON_NurbsCurve form of a bezier.
  // Parameters:
  //   nurbs_curve - [out] NURBS curve form of a bezier.
  //       The domain is [0,1].
  // Returns:
  //   0 = failure
  //   1 = success
  int GetNurbForm( 
    MYON_NurbsCurve& nurbs_curve
    ) const;

  // Returns:
  //   true if bezier is rational.
  bool IsRational() const;
  
  // Returns:
  //   Number of doubles per control vertex.
  //   (= IsRational() ? Dim()+1 : Dim())
  int CVSize() const;

  // Returns:
  //   Number of control vertices in the bezier.
  //   This is always the same as the order of the bezier.
  int CVCount() const;
  
  // Returns:
  //   Order of the bezier. (order=degree+1)
  int Order() const;        // order = degree + 1
	
  // Returns:
  //   Degree of the bezier. (degree=order-1)
  int Degree() const;

  /*
  Description:
    Expert user function to get a pointer to control vertex
    memory.  If you are not an expert user, please use
    MYON_BezierCurve::GetCV( MYON_3dPoint& ) or 
    MYON_BezierCurve::GetCV( MYON_4dPoint& ).
  Parameters:
    cv_index - [in] (0 <= cv_index < m_order)
  Returns:
    Pointer to control vertex.
  Remarks:
    If the Bezier curve is rational, the format of the 
    returned array is a homogeneos rational point with
    length m_dim+1.  If the Bezier curve is not rational, 
    the format of the returned array is a nonrational 
    euclidean point with length m_dim.
  See Also
    MYON_BezierCurve::CVStyle
    MYON_BezierCurve::GetCV
    MYON_BezierCurve::Weight
  */
  double* CV(
        int cv_index
        ) const;

  /*
  Parameters:
    cv_index - [in]
      zero based control point index
  Returns:
    Control point as an MYON_4dPoint.
  Remarks:
    If cv_index or the bezier is not valid, then MYON_4dPoint::Nan is returned.
    If dim < 3, unused coordinates are zero.
    If dim >= 4, the first three coordinates are returned.
    If is_rat is false, the weight is 1.
  */
  const MYON_4dPoint ControlPoint(
    int cv_index
  ) const;

  /*
  Description:
    Returns the style of control vertices in the m_cv array.
  Returns:
    @untitled table
    MYON::not_rational                m_is_rat is false
    MYON::homogeneous_rational        m_is_rat is true
  */
  MYON::point_style CVStyle() const;

  // Parameters:
  //   cv_index - [in] control vertex index (0<=i<m_order)
  // Returns:
  //   Weight of the i-th control vertex.
  double Weight(
        int cv_index
        ) const;

  // Description:
  //   Set weight of a control vertex.
  // Parameters:
  //   cv_index - [in] control vertex index (0 <= cv_index < m_order)
  //   weight - [in] weight
  // Returns:
  //   true if the weight can be set.  If weight is not 1 and
  //   the bezier is not rational, then false is returned.
  //   Use MYON_BezierCurve::MakeRational to make a bezier curve
  //   rational.
  // See Also:
  //   MYON_BezierCurve::SetCV, MYON_BezierCurve::MakeRational,
  //   MYON_BezierCurve::IsRational, MYON_BezierCurve::Weight
  bool SetWeight(
        int cv_index,
        double weight
        );

  // Description:
  //   Set control vertex
  // Parameters:
  //   cv_index - [in] control vertex index (0 <= cv_index < m_order)
  //   pointstyle - [in] specifies what kind of values are passed
  //      in the cv array.
  //        MYON::not_rational 
  //          cv[] is an array of length m_dim that defines
  //          a euclidean (world coordinate) point
  //        MYON::homogeneous_rational
  //          cv[] is an array of length (m_dim+1) that defines
  //          a rational homogeneous point.
  //        MYON::euclidean_rational
  //          cv[] is an array of length (m_dim+1).  The first
  //          m_dim values define the euclidean (world coordinate) 
  //          location of the point.  cv[m_dim] is the weight
  //        MYON::intrinsic_point_style
  //          If m_is_rat is true, cv[] has MYON::homogeneous_rational
  //          point style.  If m_is_rat is false, cv[] has 
  //          MYON::not_rational point style.
  //   cv - [in] array with control vertex value.
  // Returns:
  //   true if the point can be set.
  bool SetCV(
        int cv_index,
        MYON::point_style pointstyle,
        const double* cv
        );

  // Description:
  //   Set location of a control vertex.
  // Parameters:
  //   cv_index - [in] control vertex index (0 <= cv_index < m_order)
  //   point - [in] control vertex location.  If the bezier
  //      is rational, the weight will be set to 1.
  // Returns:
  //   true if successful.
  // See Also:
  //   MYON_BezierCurve::CV, MYON_BezierCurve::SetCV,
  //   MYON_BezierCurve::SetWeight, MYON_BezierCurve::Weight
  bool SetCV(
        int cv_index,
        const MYON_3dPoint& point
        );

  // Description:
  //   Set value of a control vertex.
  // Parameters:
  //   cv_index - [in] control vertex index (0 <= cv_index < m_order)
  //   point - [in] control vertex value.  If the bezier
  //      is not rational, the euclidean location of
  //      homogenoeous point will be used.
  // Returns:
  //   true if successful.
  // See Also:
  //   MYON_BezierCurve::CV, MYON_BezierCurve::SetCV,
  //   MYON_BezierCurve::SetWeight, MYON_BezierCurve::Weight
  bool SetCV(
        int cv_index,
        const MYON_4dPoint& point
        );

  // Description:
  //   Get location of a control vertex.
  // Parameters:
  //   cv_index - [in] control vertex index (0 <= cv_index < m_order)
  //   pointstyle - [in] specifies what kind of values to get
  //        MYON::not_rational 
  //          cv[] is an array of length m_dim that defines
  //          a euclidean (world coordinate) point
  //        MYON::homogeneous_rational
  //          cv[] is an array of length (m_dim+1) that defines
  //          a rational homogeneous point.
  //        MYON::euclidean_rational
  //          cv[] is an array of length (m_dim+1).  The first
  //          m_dim values define the euclidean (world coordinate) 
  //          location of the point.  cv[m_dim] is the weight
  //        MYON::intrinsic_point_style
  //          If m_is_rat is true, cv[] has MYON::homogeneous_rational
  //          point style.  If m_is_rat is false, cv[] has 
  //          MYON::not_rational point style.
  //   cv - [out] array with control vertex value.
  // Returns:
  //   true if successful.  false if cv_index is invalid.
  bool GetCV(
        int cv_index,
        MYON::point_style pointstyle,
        double* cv
        ) const;

  // Description:
  //   Get location of a control vertex.
  // Parameters:
  //   cv_index - [in] control vertex index (0 <= cv_index < m_order)
  //   point - [out] Location of control vertex.  If the bezier
  //      is rational, the euclidean location is returned.
  // Returns:
  //   true if successful.
  bool GetCV(
        int cv_index,
        MYON_3dPoint& point
        ) const;

  // Description:
  //   Get value of a control vertex.
  // Parameters:
  //   cv_index - [in] control vertex index (0 <= cv_index < m_order)
  //   point - [out] Homogeneous value of control vertex.
  //      If the bezier is not rational, the weight is 1.
  // Returns:
  //   true if successful.
  bool GetCV(
        int cv_index,
        MYON_4dPoint& point
        ) const;

  // Description:
  //   Zeros control vertices and, if rational, sets weights to 1.
  bool ZeroCVs(); 

  // Description:
  //   Make beizer rational.
  // Returns:
  //   true if successful.
  // See Also:
  //   MYON_Bezier::MakeNonRational
  bool MakeRational();

  // Description:
  //   Make beizer not rational by setting all control
  //   vertices to their euclidean locations and setting
  //   m_is_rat to false.
  // See Also:
  //   MYON_Bezier::MakeRational
  bool MakeNonRational();

  // Description:
  //   Increase degree of bezier.
  // Parameters:
  //   desired_degree - [in]
  // Returns:
  //   true if successful.  false if desired_degree < current degree.
  bool IncreaseDegree(
          int desired_degree
          );

  // Description:
  //   Change dimension of bezier.
  // Parameters:
  //   desired_dimension - [in]
  // Returns:
  //   true if successful.  false if desired_dimension < 1
  bool ChangeDimension(
          int desired_dimension
          );

  /////////////////////////////////////////////////////////////////
  // Tools for managing CV and knot memory

  // Description:
  //   Make sure m_cv array has a certain length.
  // Parameters:
  //   desired_cv_capacity - [in] minimum length of m_cv array.
  // Returns:
  //   true if successful.
  bool ReserveCVCapacity(
    int desired_cv_capacity
    );

  // Description:
  //   Trims (or extends) the bezier so the bezier so that the 
  //   result starts bezier(interval[0]) and ends at 
  //   bezier(interval[1]) (Evaluation performed on input bezier.)
  // Parameters:
  //   interval -[in]
  // Example:
  //   An interval of [0,1] leaves the bezier unchanged.  An
  //   interval of [0.5,1] would trim away the left half.  An
  //   interval of [0.0,2.0] would extend the right end.
  bool Trim( 
    const MYON_Interval& interval 
    );

  // Description:
  //   Split() divides the Bezier curve at the specified parameter.
  //   The parameter must satisfy 0 < t < 1.  You may pass *this as
  //   one of the curves to be returned.
  // Parameters:
  //   t - [in] (0 < t < 1 ) parameter to split at
  //   left_side - [out]
  //   right_side - [out]  
  // Example:
  //       MYON_BezierCurve crv = ...;
  //       MYON_BezierCurve right_side;
  //       crv.Split( 0.5, crv, right_side );
  //   would split crv at the 1/2, put the left side in crv,
  //   and return the right side in right_side.
  bool Split( 
         double t,
         MYON_BezierCurve& left_side,
         MYON_BezierCurve& right_side
         ) const;

  // Description:
  //   returns the length of the control polygon
  double ControlPolygonLength() const;

  /*
  Description:
    Use a linear fractional transformation for [0,1] to reparameterize
    the bezier.  The locus of the curve is not changed, but the
    parameterization is changed.
  Parameters:
    c - [in]
      reparameterization constant (generally speaking, c should be > 0).
      If c != 1, then the returned bezier will be rational.
  Returns:
    true if successful.
  Remarks:
    The reparameterization is performed by composing the input Bezier with
    the function lambda: [0,1] -> [0,1] given by

          t ->  c*t / ( (c-1)*t + 1 )

    Note that lambda(0) = 0, lambda(1) = 1, lambda'(t) > 0, 
    lambda'(0) = c and lambda'(1) = 1/c.

    If the input Bezier has control vertices {B_0, ..., B_d}, then the 
    output Bezier has control vertices 

          (B_0, ... c^i * B_i, ..., c^d * B_d).

    To derive this formula, simply compute the i-th Bernstein polynomial
    composed with lambda().

    The inverse parameterization is given by 1/c.  That is, the 
    cumulative effect of the two calls

          Reparameterize(c)
          Reparameterize(1.0/c)

    is to leave the bezier unchanged.
  See Also:
    MYON_Bezier::ScaleConrolPoints
  */
  bool Reparameterize(
          double c
          );

  // misspelled function name is obsolete
  MYON_DEPRECATED_MSG("misspelled - use Reparameterize")
  bool Reparametrize(double);

  /*
  Description:
    Scale a rational Bezier's control vertices to set a weight to a 
    specified value.
  Parameters:
    i - [in] (0 <= i < order)
    w - [in] w != 0.0
  Returns:
    True if successful.  The i-th control vertex will have weight w.
  Remarks:
    Each control point is multiplied by w/w0, where w0 is the
    input value of Weight(i).
  See Also:
    MYON_Bezier::Reparameterize
    MYON_Bezier::ChangeWeights
  */
  bool ScaleConrolPoints( 
          int i, 
          double w
          );

  /*
  Description:
    Use a combination of scaling and reparameterization to set two 
    rational Bezier weights to specified values.
  Parameters:
    i0 - [in] control point index (0 <= i0 < order, i0 != i1)
    w0 - [in] Desired weight for i0-th control point
    i1 - [in] control point index (0 <= i1 < order, i0 != i1)
    w1 - [in] Desired weight for i1-th control point
  Returns:
    True if successful.  The returned bezier has the same locus but
    probably has a different parameterization.
  Remarks:
    The i0-th cv will have weight w0 and the i1-rst cv will have 
    weight w1.  If v0 and v1 are the cv's input weights, 
    then  v0, v1, w0 and w1 must all be nonzero, and w0*v0 
    and w1*v1 must have the same sign.

    The equations

          s * r^i0 = w0/v0
          s * r^i1 = w1/v1

    determine the scaling and reparameterization necessary to 
    change v0,v1 to w0,w1. 

    If the input Bezier has control vertices 
    
          (B_0, ..., B_d),

    then the output Bezier has control vertices 
    
          (s*B_0, ... s*r^i * B_i, ..., s*r^d * B_d).
  See Also:
    MYON_Bezier::Reparameterize
    MYON_Bezier::ScaleConrolPoints
  */
  bool ChangeWeights(
          int i0, 
          double w0, 
          int i1, 
          double w1
          );




  /////////////////////////////////////////////////////////////////
  // Implementation
public:
  // NOTE: These members are left "public" so that expert users may efficiently
  //       create bezier curves using the default constructor and borrow the
  //       knot and CV arrays from their native NURBS representation.
  //       No technical support will be provided for users who access these
  //       members directly.  If you can't get your stuff to work, then use
  //       the constructor with the arguments and the SetKnot() and SetCV()
  //       functions to fill in the arrays.


  // dimension of bezier (>=1)
  int m_dim;

  // 1 if bezier is rational, 0 if bezier is not rational
  int m_is_rat;

  // order = degree+1
  int m_order;

  // Number of doubles per cv ( >= ((m_is_rat)?m_dim+1:m_dim) )
  int m_cv_stride;

  // The i-th cv begins at cv[i*m_cv_stride].
  double* m_cv;

  // Number of doubles in m_cv array.  If m_cv_capacity is zero
  // and m_cv is not nullptr, an expert user is managing the m_cv
  // memory.  ~MYON_BezierCurve will not deallocate m_cv unless
  // m_cv_capacity is greater than zero.
  int m_cv_capacity;

#if 8 == MYON_SIZEOF_POINTER
  // pad to a multiple of 8 bytes so custom allocators
  // will keep m_cv aligned and tail-padding reuse will
  // not be an issue.
  int m_reserved_MYON_BezierCurve;
#endif
};


class MYON_CLASS MYON_BezierSurface
{
public:
  MYON_BezierSurface();
  MYON_BezierSurface(
    int dim,
    bool is_rat,
    int order0,
    int order1
    );

  ~MYON_BezierSurface();
  MYON_BezierSurface(const MYON_BezierSurface&);
  MYON_BezierSurface(const MYON_PolynomialSurface&);
  MYON_BezierSurface& operator=(const MYON_BezierSurface&);
  MYON_BezierSurface& operator=(const MYON_PolynomialSurface&);

  bool IsValid() const;
  void Dump( MYON_TextLog& ) const; // for debugging
  int Dimension() const;

  bool Create(
    int dim,
    bool is_rat,
    int order0,
    int order1
    );

  void Destroy();
  void EmergencyDestroy(); // call if memory used by MYON_NurbsCurve becomes invalid

  /*
  Description:
    Loft a bezier surface through a list of bezier curves.
  Parameters:
    curve_list - [in]  list of curves that have the same degree.
  Returns:
    True if successful.
  */
  bool Loft( const MYON_ClassArray<MYON_BezierCurve>& curve_list );

  /*
  Description:
    Loft a bezier surface through a list of bezier curves.
  Parameters:
    curve_count - [in] number of curves in curve_list
    curve_list - [in]  array of pointers to curves that have the same degree.
  Returns:
    True if successful.
  */
  bool Loft( 
    int count, 
    const MYON_BezierCurve* const* curve_list 
    );

  bool GetBBox(        // returns true if successful
         double*,      // minimum
         double*,      // maximum
         bool bGrowBox = false  // true means grow box
         ) const;

  bool GetBoundingBox(
        MYON_BoundingBox& bbox,
        int bGrowBox
        ) const;

  MYON_BoundingBox BoundingBox() const;

  bool Transform( 
         const MYON_Xform&
         );


  // Description:
  //   Rotates the bezier surface about the specified axis.  A positive
  //   rotation angle results in a counter-clockwise rotation
  //   about the axis (right hand rule).
  // Parameters:
  //   sin_angle - [in] sine of rotation angle
  //   cos_angle - [in] sine of rotation angle
  //   rotation_axis - [in] direction of the axis of rotation
  //   rotation_center - [in] point on the axis of rotation
  // Returns:
  //   true if bezier surface successfully rotated
  // Remarks:
  //   Uses MYON_BezierSurface::Transform() function to calculate the result.
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& rotation_axis,
        const MYON_3dPoint& rotation_center
        );

  // Description:
  //   Rotates the bezier surface about the specified axis.  A positive
  //   rotation angle results in a counter-clockwise rotation
  //   about the axis (right hand rule).
  // Parameters:
  //   rotation_angle - [in] angle of rotation in radians
  //   rotation_axis - [in] direction of the axis of rotation
  //   rotation_center - [in] point on the axis of rotation
  // Returns:
  //   true if bezier surface successfully rotated
  // Remarks:
  //   Uses MYON_BezierSurface::Transform() function to calculate the result.
  bool Rotate(
        double rotation_angle,
        const MYON_3dVector& rotation_axis,
        const MYON_3dPoint& rotation_center
        );

  // Description:
  //   Translates the bezier surface along the specified vector.
  // Parameters:
  //   translation_vector - [in] translation vector
  // Returns:
  //   true if bezier surface successfully translated
  // Remarks:
  //   Uses MYON_BezierSurface::Transform() function to calculate the result.
  bool Translate( 
    const MYON_3dVector& translation_vector
    );

  // Description:
  //   Scales the bezier surface by the specified facotor.  The scale is
  //   centered at the origin.
  // Parameters:
  //   scale_factor - [in] scale factor
  // Returns:
  //   true if bezier surface successfully scaled
  // Remarks:
  //   Uses MYON_BezierSurface::Transform() function to calculate the result.
  bool Scale( 
    double scale_factor
    );

  MYON_Interval Domain(
    int // 0 = "u" domain, 1 = "v" domain
    ) const;

  bool Reverse( int );  // reverse parameterizatrion
                        // Domain changes from [a,b] to [-b,-a]
  
  bool Transpose(); // transpose surface parameterization (swap "s" and "t")

  bool Evaluate( // returns false if unable to evaluate
         double, double, // evaluation parameter
         int,            // number of derivatives (>=0)
         int,            // array stride (>=Dimension())
         double*         // array of length stride*(ndir+1)*(ndir+2)/2
         ) const;

  MYON_3dPoint PointAt(double s, double t) const;

  /*
  Returns:
    0 = failure.
    1 = success.
  */
  int GetNurbForm( MYON_NurbsSurface& ) const;

  bool IsRational() const;  // true if NURBS curve is rational
  
  int CVSize() const;       // number of doubles per control vertex 
                // = IsRational() ? Dim()+1 : Dim()
  
  int Order(        // order = degree + 1
        int // dir
        ) const;
	
  int Degree(       // degree = order - 1
        int // dir
        ) const;

  /*
  Description:
    Expert user function to get a pointer to control vertex
    memory.  If you are not an expert user, please use
    MYON_BezierSurface::GetCV( MYON_3dPoint& ) or 
    MYON_BezierSurface::GetCV( MYON_4dPoint& ).
  Parameters:
    cv_index0 - [in] (0 <= cv_index0 < m_order[0])
    cv_index1 - [in] (0 <= cv_index1 < m_order[1])
  Returns:
    Pointer to control vertex.
  Remarks:
    If the Bezier surface is rational, the format of the 
    returned array is a homogeneos rational point with
    length m_dim+1.  If the Bezier surface is not rational, 
    the format of the returned array is a nonrational 
    euclidean point with length m_dim.
  See Also
    MYON_BezierSurface::CVStyle
    MYON_BezierSurface::GetCV
    MYON_BezierSurface::Weight
  */
  double* CV(
        int cv_index0,
        int cv_index1
        ) const;

  /*
  Description:
    Returns the style of control vertices in the m_cv array.
  Returns:
    @untitled table
    MYON::not_rational                m_is_rat is false
    MYON::homogeneous_rational        m_is_rat is true
  */
  MYON::point_style CVStyle() const;

  double Weight(        // get value of control vertex weight
        int,int          // CV index ( >= 0 and < CVCount() )
        ) const;

  bool SetWeight(      // set value of control vertex weight
        int,int,         // CV index ( >= 0 and < CVCount() )
        double
        );

  bool SetCV(              // set a single control vertex
        int,int,         // CV index ( >= 0 and < CVCount() )
        MYON::point_style, // style of input point
        const double*     // value of control vertex
        );

  bool SetCV(               // set a single control vertex
        int,int,         // CV index ( >= 0 and < CVCount() )
        const MYON_3dPoint& // value of control vertex
                           // If NURBS is rational, weight
                           // will be set to 1.
        );

  bool SetCV(              // set a single control vertex
        int,int,         // CV index ( >= 0 and < CVCount() )
        const MYON_4dPoint& // value of control vertex
                          // If NURBS is not rational, euclidean
                          // location of homogeneous point will
                          // be used.
        );

  bool GetCV(              // get a single control vertex
        int,int,          // CV index ( >= 0 and < CVCount() )
        MYON::point_style, // style to use for output point
        double*           // array of length >= CVSize()
        ) const;

  bool GetCV(              // get a single control vertex
        int,int,         // CV index ( >= 0 and < CVCount() )
        MYON_3dPoint&      // gets euclidean cv when NURBS is rational
        ) const;

  bool GetCV(              // get a single control vertex
        int,int,         // CV index ( >= 0 and < CVCount() )
        MYON_4dPoint&      // gets homogeneous cv
        ) const;

  bool ZeroCVs(); // zeros control vertices and, if rational, sets weights to 1

  bool MakeRational();

  bool MakeNonRational();

  bool Split( 
         int, // 0 split at "u"=t, 1= split at "v"=t
         double, // t = splitting parameter must 0 < t < 1
         MYON_BezierSurface&, // west/south side returned here (can pass *this)
         MYON_BezierSurface&  // east/north side returned here (can pass *this)
         ) const;

  bool Trim(
       int dir,
       const MYON_Interval& domain
       );

	// returns the isocurve.  
	MYON_BezierCurve* IsoCurve(
		   int dir,    // 0 first parameter varies and second parameter is constant
                   //   e.g., point on IsoCurve(0,c) at t is srf(t,c)
                   // 1 first parameter is constant and second parameter varies
                   //   e.g., point on IsoCurve(1,c) at t is srf(c,t)
       double c,    // value of constant parameter
			 MYON_BezierCurve* iso=nullptr	// When nullptr result is constructed on the heap.
			 ) const;

	bool IsSingular( // true if surface side is collapsed to a point
         int        // side of parameter space to test
                   // 0 = south, 1 = east, 2 = north, 3 = west
         ) const;
 

  /////////////////////////////////////////////////////////////////
  // Tools for managing CV and knot memory
  bool ReserveCVCapacity(
    int // number of doubles to reserve
    );


	/*
	Description:
		Get an estimate of the size of the rectangle that would
		be created if the 3d surface where flattened into a rectangle.
	Parameters:
		width - [out]  (corresponds to the first surface parameter)
		height - [out] (corresponds to the first surface parameter)
	Returns:
		true if successful.
	*/
	bool GetSurfaceSize(double* width, double* height) const;

  /////////////////////////////////////////////////////////////////
  // Implementation
public:
  // NOTE: These members are left "public" so that expert users may efficiently
  //       create bezier curves using the default constructor and borrow the
  //       knot and CV arrays from their native NURBS representation.
  //       No technical support will be provided for users who access these
  //       members directly.  If you can't get your stuff to work, then use
  //       the constructor with the arguments and the SetKnot() and SetCV()
  //       functions to fill in the arrays.


  int     m_dim;           // >= 1
  int     m_is_rat;        // 0 = no, 1 = yes
  int     m_order[2];      // order = degree+1 >= 2
  int     m_cv_stride[2];  
  double* m_cv;
  int     m_cv_capacity;   // if 0, then destructor does not free m_cv
#if 8 == MYON_SIZEOF_POINTER
  // pad to a multiple of 8 bytes so custom allocators
  // will keep m_cv aligned and tail-padding reuse will
  // not be an issue.
  int m_reserved_MYON_BezierSurface;
#endif
};




class MYON_CLASS MYON_BezierCage
{
public:
  MYON_BezierCage();

  MYON_BezierCage(
    int dim,
    bool is_rat,
    int order0,
    int order1,
    int order2
    );


  /*
  Description:
    Construct a bezier volume that maps the unit cube
    to a bounding box.
  Parameters:
    bbox - [in] target bounding box
    order0 - [in]
    order1 - [in]
    order2 - [in]
  */
  MYON_BezierCage( 
    const MYON_BoundingBox& bbox,
    int order0,
    int order1,
    int order2
    );


  /*
  Description:
    Construct a bezier volume that maps the unit cube
    to an eight sided box.
  Parameters:
    box_corners - [in] 8 points that define corners of the
                       target volume.

            7______________6
            |\             |\
            | \            | \
            |  \ _____________\
            |   4          |   5
            |   |          |   |
            |   |          |   |
            3---|----------2   |
            \   |          \   |
             \  |t          \  |
            s \ |            \ |
               \0_____________\1
                       r

    order0 - [in]
    order1 - [in]
    order2 - [in]
  */
  MYON_BezierCage( 
    const MYON_3dPoint* box_corners,
    int order0,
    int order1,
    int order2
    );

  ~MYON_BezierCage();

  MYON_BezierCage(const MYON_BezierCage& src);

  MYON_BezierCage& operator=(const MYON_BezierCage& src);


  /*
  Description:
    Tests class to make sure members are correctly initialized.
  Returns:
    True if the orders are all >= 2, dimension is positive,
    and the rest of the members have settings that are
    valid for the orders and dimension.
  */
  bool IsValid() const;

  void Dump( MYON_TextLog& text_log) const;


  /*
  Description:
    The dimension of the image of the bazier volume map.
    This is generally three, but can be any positive
    integer.
  Returns:
    Dimesion of the image space.
  */
  int Dimension() const;


  /*
  Description:
    Creates a bezier volume with specified orders.
  Parameters:
    dim - [in]
    is_rat - [in]
    order0 - [in]
    order1 - [in]
    order2 - [in]
  Returns:
    True if input was valid and creation succeeded.
  */
  bool Create(
    int dim,
    bool is_rat,
    int order0,
    int order1,
    int order2
    );

  /*
  Description:
    Create a Bezier volume with corners defined by a bounding box.
  Parameters:
    bbox - [in] target bounding box - the bezier will
               map the unit cube onto this bounding box.
    order0 - [in]
    order1 - [in]
    order2 - [in]
  */
  bool Create(
    const MYON_BoundingBox& bbox,
    int order0,
    int order1,
    int order2
    );

  /*
  Description:
    Create a bezier volume from a 3d box
  Parameters:
    box_corners - [in] 8 points that define corners of the volume

            7______________6
            |\             |\
            | \            | \
            |  \ _____________\
            |   4          |   5
            |   |          |   |
            |   |          |   |
            3---|----------2   |
            \   |          \   |
             \  |t          \  |
            s \ |            \ |
               \0_____________\1
                       r

  */
  bool Create(
    const MYON_3dPoint* box_corners,
    int order0,
    int order1,
    int order2
    );


  /*
  Description:
    Frees the CV array and sets all members to zero.
  */
  void Destroy();

  /*
  Description:
    Sets all members to zero.  Does not free the CV array
    even when m_cv is not nullptr.  Generally used when the
    CVs were allocated from a memory pool that no longer
    exists and the free done in ~MYON_BezierCage would
    cause a crash.
  */
  void EmergencyDestroy();


  /*
  Description:
    Reads the definition of this class from an
    archive previously saved by MYON_BezierVolue::Write.
  Parameters:
    archive - [in] target archive
  Returns:
    True if successful.
  */
  bool Read(MYON_BinaryArchive& archive);

  /*
  Description:
    Saves the definition of this class in serial binary
    form that can be read by MYON_BezierVolue::Read.
  Parameters:
    archive - [in] target archive
  Returns:
    True if successful.
  */
  bool Write(MYON_BinaryArchive& archive) const;


  /*
  Description:
    Gets the axis aligned bounding box that contains
    the bezier's control points.  The bezier volume
    maps the unit cube into this box.
  Parameters:
    boxmin - [in] array of Dimension() doubles
    boxmax - [in] array of Dimension() doubles
    bGrowBox =  [in] if true and the input is a valid box
                          then the input box is grown to
                          include this object's bounding box.
  Returns:
    true if successful.
  */
  bool GetBBox(
         double* boxmin,
         double* boxmax,
         bool bGrowBox = false 
         ) const;

  bool Transform( 
         const MYON_Xform& xform
         );


  // Description:
  //   Rotates the bezier surface about the specified axis.  A positive
  //   rotation angle results in a counter-clockwise rotation
  //   about the axis (right hand rule).
  // Parameters:
  //   sin_angle - [in] sine of rotation angle
  //   cos_angle - [in] sine of rotation angle
  //   rotation_axis - [in] direction of the axis of rotation
  //   rotation_center - [in] point on the axis of rotation
  // Returns:
  //   true if bezier surface successfully rotated
  // Remarks:
  //   Uses MYON_BezierCage::Transform() function to calculate the result.
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& rotation_axis,
        const MYON_3dPoint& rotation_center
        );

  // Description:
  //   Rotates the bezier surface about the specified axis.  A positive
  //   rotation angle results in a counter-clockwise rotation
  //   about the axis (right hand rule).
  // Parameters:
  //   rotation_angle - [in] angle of rotation in radians
  //   rotation_axis - [in] direction of the axis of rotation
  //   rotation_center - [in] point on the axis of rotation
  // Returns:
  //   true if bezier surface successfully rotated
  // Remarks:
  //   Uses MYON_BezierCage::Transform() function to calculate the result.
  bool Rotate(
        double rotation_angle,
        const MYON_3dVector& rotation_axis,
        const MYON_3dPoint& rotation_center
        );

  // Description:
  //   Translates the bezier surface along the specified vector.
  // Parameters:
  //   translation_vector - [in] translation vector
  // Returns:
  //   true if bezier surface successfully translated
  // Remarks:
  //   Uses MYON_BezierCage::Transform() function to calculate the result.
  bool Translate( 
    const MYON_3dVector& translation_vector
    );

  // Description:
  //   Scales the bezier surface by the specified facotor.  The scale is
  //   centered at the origin.
  // Parameters:
  //   scale_factor - [in] scale factor
  // Returns:
  //   true if bezier surface successfully scaled
  // Remarks:
  //   Uses MYON_BezierCage::Transform() function to calculate the result.
  bool Scale( 
    double scale_factor
    );

  MYON_Interval Domain(
    int // 0 = "u" domain, 1 = "v" domain, 2 = "w" domain
    ) const;

  // returns false if unable to evaluate
  bool Evaluate( 
         double r, 
         double s, 
         double t,
         int der_count,
         int v_stride,
         double* v        // array of length stride*(ndir+1)*(ndir+2)/2
         ) const;

  /*
  Description:
    Evaluates bezer volume map.
  Parameters:
    rst - [in]
  Returns:
    Value of the bezier volume map at (r,s,t).
  */
  MYON_3dPoint PointAt(
         double r, 
         double s, 
         double t
         ) const;

  /*
  Description:
    Evaluates bezer volume map.
  Parameters:
    rst - [in]
  Returns:
    Value of the bezier volume map at (rst.x,rst.y,rst.z).
  */
  MYON_3dPoint PointAt(
         MYON_3dPoint rst
         ) const;

  bool IsRational() const;  // true if NURBS curve is rational
  
  bool IsSingular( // true if surface side is collapsed to a point
        int        // side of parameter space to test
                   // 0 = south, 1 = east, 2 = north, 3 = west
        ) const;

  int CVSize() const;       // number of doubles per control vertex 
                // = IsRational() ? Dim()+1 : Dim()
  
  int Order(        // order = degree + 1
        int // dir
        ) const;
	
  int Degree(       // degree = order - 1
        int // dir
        ) const;

  /*
  Description:
    Expert user function to get a pointer to control vertex
    memory.  If you are not an expert user, please use
    MYON_BezierCage::GetCV( MYON_3dPoint& ) or 
    MYON_BezierCage::GetCV( MYON_4dPoint& ).
  Parameters:
    cv_index0 - [in] (0 <= cv_index0 < m_order[0])
    cv_index1 - [in] (0 <= cv_index1 < m_order[1])
  Returns:
    Pointer to control vertex.
  Remarks:
    If the Bezier surface is rational, the format of the 
    returned array is a homogeneos rational point with
    length m_dim+1.  If the Bezier surface is not rational, 
    the format of the returned array is a nonrational 
    euclidean point with length m_dim.
  See Also
    MYON_BezierCage::CVStyle
    MYON_BezierCage::GetCV
    MYON_BezierCage::Weight
  */
  double* CV(
        int i,
        int j,
        int k
        ) const;

  /*
  Description:
    Returns the style of control vertices in the m_cv array.
  Returns:
    @untitled table
    MYON::not_rational                m_is_rat is false
    MYON::homogeneous_rational        m_is_rat is true
  */
  MYON::point_style CVStyle() const;

  double Weight(        // get value of control vertex weight
        int i,
        int j,
        int k
        ) const;

  bool SetWeight(      // set value of control vertex weight
        int i,
        int j,
        int k,
        double w
        );

  bool SetCV(              // set a single control vertex
        int i,
        int j,
        int k,
        MYON::point_style, // style of input point
        const double*     // value of control vertex
        );

  // set a single control vertex
  // If NURBS is rational, weight
  // will be set to 1.
  bool SetCV(
        int i,
        int j,
        int k,
        const MYON_3dPoint& point
        );

  // set a single control vertex
  // value of control vertex
  // If NURBS is not rational, euclidean
  // location of homogeneous point will
  // be used.
  bool SetCV(
        int i,
        int j,
        int k,
        const MYON_4dPoint& hpoint
        );

  bool GetCV(              // get a single control vertex
        int i,
        int j,
        int k,
        MYON::point_style, // style to use for output point
        double*           // array of length >= CVSize()
        ) const;

  bool GetCV(              // get a single control vertex
        int i,
        int j,
        int k,
        MYON_3dPoint&      // gets euclidean cv when NURBS is rational
        ) const;

  bool GetCV(              // get a single control vertex
        int i,
        int j,
        int k,
        MYON_4dPoint&      // gets homogeneous cv
        ) const;

  bool ZeroCVs(); // zeros control vertices and, if rational, sets weights to 1

  bool MakeRational();

  bool MakeNonRational();


  /////////////////////////////////////////////////////////////////
  // Tools for managing CV and knot memory

  /*
  Description:
    cv_capacity - [in] number of doubles to reserve
  */
  bool ReserveCVCapacity(
    int cv_capacity
    );

  /////////////////////////////////////////////////////////////////
  // Implementation
public:
  // NOTE: These members are left "public" so that expert users may efficiently
  //       create bezier curves using the default constructor and borrow the
  //       knot and CV arrays from their native NURBS representation.
  //       No technical support will be provided for users who access these
  //       members directly.  If you can't get your stuff to work, then use
  //       the constructor with the arguments and the SetKnot() and SetCV()
  //       functions to fill in the arrays.


  int     m_dim;
  bool    m_is_rat;
  int     m_order[3];
  int     m_cv_stride[3];
  int     m_cv_capacity;
  double* m_cv;
};


class MYON_CLASS MYON_BezierCageMorph : public MYON_SpaceMorph
{
public:
  MYON_BezierCageMorph();
  virtual ~MYON_BezierCageMorph();


  /*
  Description:
    Create a Bezier volume.
  Parameters:
    P0 - [in] 
    P1 - [in] 
    P2 - [in] 
    P3 - [in] 
         P0,P1,P2,P3 defines a parallepiped in world space. The morph
         maps this parallepiped to the (0,1)x(0,1)x(0,1) unit cube
         and then applies the BezierCage map.
              

             ______________
            |\             |\
            | \            | \
            |  \P3____________\
            |   |          |   |
            |   |          |   |
            |   |          |   |
           P2---|----------    |
            \   |          \   |
             \  |z          \  |
            y \ |            \ |
               \P0____________P1
                       x


    point_countX - [in]
    point_countY - [in]
    point_countZ - [in]
      Number of control points in the bezier volume map.  The 
      bezier volume in the returned morph is the identity map
      which can be modified as needed.
  Returns:
    True if resulting morph is valid.
  See Also:
    MYON_BezierCage::SetBezierCage
    MYON_BezierCage::SetXform
  */
  bool Create( 
    MYON_3dPoint P0,
    MYON_3dPoint P1,
    MYON_3dPoint P2,
    MYON_3dPoint P3,
    int point_countX,
    int point_countY,
    int point_countZ
    );

  /*
  Description:
    Set the world to unit cube map.
  Parameters:
    world2unitcube - [in]
      Transformation matrix that maps world coordinates
      to the unit cube (0,1)x(0,1)x(0,1).
  Returns
    True if current bezier volum and input transformation
    matrix are valid.  In all cases, the morph's m_xyz2rst
    member is set.
  See Also:
    MYON_BezierCage::Create
    MYON_BezierCage::SetBezierCage
  */
  bool SetXform( MYON_Xform world2unitcube );

  /*
  Description:
    Set the unit cube to world map.
  Parameters:
    world2unitcube - [in]
      Bezier volume map from the unit cube (0,1)x(0,1)x(0,1)
      to world space.
  Returns
    True if current transformation matrix and input
    bezier volume are valid.  In all cases, the 
    morph's m_rst2xyz member is set.
  See Also:
    MYON_BezierCage::Create
    MYON_BezierCage::SetXform
  */
  bool SetBezierCage( MYON_BezierCage& unitcube2world );

  const MYON_Xform& WorldToUnitCube() const;
  const MYON_BezierCage& BezierCage() const;

  bool Read(MYON_BinaryArchive& archive);
  bool Write(MYON_BinaryArchive& archive) const;

  /*
  Description:
    Transforms the morph by transforming the bezier volume map.
  Parameters:
    xform - [in]
  Returns
    True if input is valid.
  */
  bool Transform(const MYON_Xform& xform);

private:
  bool m_bValid;

  // transforms world (x,y,z) coordinate into
  // unit cube.
  MYON_Xform m_xyz2rst;

  // function that maps unit cube into world
  MYON_BezierCage m_rst2xyz;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_BezierCurve>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_BezierCurve*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_BezierSurface>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_BezierSurface*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_BezierCage>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_BezierCage*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_BezierCageMorph>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_BezierCageMorph*>;
#endif

#endif

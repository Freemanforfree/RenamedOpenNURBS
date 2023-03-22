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

#if !defined(OPENNURBS_SUM_SURFACE_INC_)
#define OPENNURBS_SUM_SURFACE_INC_

class MYON_SumSurface;

// surface of revolution
class MYON_CLASS MYON_SumSurface : public MYON_Surface
{
  MYON_OBJECT_DECLARE(MYON_SumSurface);

public:
  // virtual MYON_Object::DestroyRuntimeCache override
  void DestroyRuntimeCache( bool bDelete = true ) override;

public:

  // for expert users
  // surface-PointAt(s,t) 
  //  = m_curve[0]->PointAt(s) + m_curve[1]->PointAt(t) + m_basepoint;
  MYON_Curve* m_curve[2] = {}; // m_curve[0] and m_curve[1] are deleted by ~MYON_SumSuface.  
                         // Use a MYON_ProxyCurve if this is problem.
  MYON_3dVector m_basepoint = MYON_3dPoint::Origin;
  MYON_BoundingBox m_bbox = MYON_BoundingBox::EmptyBoundingBox; // lazy evaluation used in MYON_SumSurface::BoundingBox()

public:

  /*
  Description:
    Use MYON_SumSurface::New(...) instead of new MYON_SumSurface(...)
  Returns:
    Pointer to an MYON_SumSurface.  Destroy by calling delete.
  Remarks:
    See static MYON_Brep* MYON_Brep::New() for details.
  */
  static MYON_SumSurface* New();
  static MYON_SumSurface* New( const MYON_SumSurface& rev_surface );

  MYON_SumSurface();
  virtual ~MYON_SumSurface();
  MYON_SumSurface( const MYON_SumSurface& );
  MYON_SumSurface& operator=(const MYON_SumSurface&);

private:
  void Internal_CopyFrom(const MYON_SumSurface&);
public:
  /*
  Description:
    Extrude a curve to create a surface.
  Parameters:
    curve - [in] curve is copied.
    extrusion_vector - [in] extrusion vector (must be nonzero)
  Returns:
    true if a valid surface is created.
  */
  bool Create( const MYON_Curve& curve, MYON_3dVector extrusion_vector );

  /*
  Description:
    Extrude a curve to create a surface.
  Parameters:
    pCurve - [in] pointer to a curve. This pointer will
      be assigned to m_curve[0] and will be deleted
      by ~MYON_SumSurface.
    extrusion_vector - [in] extrusion vector (must be nonzero)
  Returns:
    true if a valid surface is created.
  */
  bool Create( MYON_Curve* pCurve, MYON_3dVector extrusion_vector );

  /*
  Description:
    Extrude a curve along a path to create a surface.
  Parameters:
    curve - [in] curve is copied.
    path_curve - [in] path_curve is copied.
  Returns:
    true if a valid surface is created.
  */
  bool Create( const MYON_Curve& curve, 
               const MYON_Curve& path_curve
               );

  /*
  Description:
    Extrude a curve to create a surface.
  Parameters:
    pCurve - [in] pointer to a curve. This pointer will
      be assigned to m_curve[0] and will be deleted
      by ~MYON_SumSurface.
    pPathCurve - [in] pointer to a path curve. This pointer will
      be assigned to m_curve[1] and will be deleted
      by ~MYON_SumSurface.
  Returns:
    true if a valid surface is created.
  */
  bool Create( 
          MYON_Curve* pCurve, 
          MYON_Curve* pPathCurve
          );

  void Destroy();

  void EmergencyDestroy();


  ////////////////////////////////////////////////////////////
  //
  // overrides of virtual MYON_Object functions
  //

  // virtual MYON_Object::SizeOf override
  unsigned int SizeOf() const override;

  // virtual MYON_Object::DataCRC override
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override; // for debugging

  // Use MYON_BinaryArchive::WriteObject() and MYON_BinaryArchive::ReadObject()
  // for top level serialization.  These Read()/Write() members should just
  // write/read specific definitions.  In particular, they should not write/
  // read any chunk typecode or length information.  The default 
  // implementations return false and do nothing.
  bool Write(
         MYON_BinaryArchive&  // serialize definition to binary archive
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // restore definition from binary archive
       ) override;

  ////////////////////////////////////////////////////////////
  //
  // overrides of virtual MYON_Geometry functions
  //
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  void ClearBoundingBox() override;

  bool Transform( 
         const MYON_Xform&
         ) override;

  // virtual MYON_Geometry::IsDeformable() override
  bool IsDeformable() const override;

  // virtual MYON_Geometry::MakeDeformable() override
  bool MakeDeformable() override;

  ////////////////////////////////////////////////////////////
  //
  // overrides of virtual MYON_Surface functions
  //

  bool SetDomain( 
    int dir, // 0 sets first parameter's domain, 1 gets second parameter's domain
    double t0, 
    double t1
    ) override;

  MYON_Interval Domain(
    int // 0 gets first parameter's domain, 1 gets second parameter's domain
    ) const override;

  /*
  Description:
    Get an estimate of the size of the rectangle that would
    be created if the 3d surface where flattened into a rectangle.
  Parameters:
    width - [out]  (corresponds to the first surface parameter)
    height - [out] (corresponds to the first surface parameter)
  Remarks:
    overrides virtual MYON_Surface::GetSurfaceSize
  Returns:
    true if successful.
  */
  bool GetSurfaceSize( 
      double* width, 
      double* height 
      ) const override;

  int SpanCount(
    int // 0 gets first parameter's domain, 1 gets second parameter's domain
    ) const override; // number of smooth spans in curve

  bool GetSpanVector( // span "knots" 
    int, // 0 gets first parameter's domain, 1 gets second parameter's domain
    double* // array of length SpanCount() + 1 
    ) const override; // 

  int Degree( // returns maximum algebraic degree of any span 
                  // ( or a good estimate if curve spans are not algebraic )
    int // 0 gets first parameter's domain, 1 gets second parameter's domain
    ) const override; 

  bool GetParameterTolerance( // returns tminus < tplus: parameters tminus <= s <= tplus
         int,     // 0 gets first parameter, 1 gets second parameter
         double,  // t = parameter in domain
         double*, // tminus
         double*  // tplus
         ) const override;

  /*
  Description:
    Test a surface to see if it is planar.
  Parameters:
    plane - [out] if not nullptr and true is returned,
                  the plane parameters are filled in.
    tolerance - [in] tolerance to use when checking
  Returns:
    true if there is a plane such that the maximum distance from
    the surface to the plane is <= tolerance.
  Remarks:
    Overrides virtual MYON_Surface::IsPlanar.
  */
  bool IsPlanar(
        MYON_Plane* plane = nullptr,
        double tolerance = MYON_ZERO_TOLERANCE
        ) const override;

  bool IsClosed(   // true if surface is closed in direction
        int        // dir  0 = "s", 1 = "t"
        ) const override;

  bool IsPeriodic( // true if surface is periodic in direction
        int        // dir  0 = "s", 1 = "t"
        ) const override;

  bool IsSingular( // true if surface side is collapsed to a point
        int        // side of parameter space to test
                   // 0 = south, 1 = east, 2 = north, 3 = west
        ) const override;
  
  /*
  Description:
    Search for a derivatitive, tangent, or curvature 
    discontinuity.
  Parameters:
    dir - [in] If 0, then "u" parameter is checked.  If 1, then
               the "v" parameter is checked.
    c - [in] type of continity to test for.
    t0 - [in] Search begins at t0. If there is a discontinuity
              at t0, it will be ignored.  This makes it 
              possible to repeatedly call GetNextDiscontinuity
              and step through the discontinuities.
    t1 - [in] (t0 != t1)  If there is a discontinuity at t1 is 
              will be ignored unless c is a locus discontinuity
              type and t1 is at the start or end of the curve.
    t - [out] if a discontinuity is found, then *t reports the
          parameter at the discontinuity.
    hint - [in/out] if GetNextDiscontinuity will be called 
       repeatedly, passing a "hint" with initial value *hint=0
       will increase the speed of the search.       
    dtype - [out] if not nullptr, *dtype reports the kind of 
        discontinuity found at *t.  A value of 1 means the first 
        derivative or unit tangent was discontinuous.  A value 
        of 2 means the second derivative or curvature was 
        discontinuous.  A value of 0 means the curve is not
        closed, a locus discontinuity test was applied, and
        t1 is at the start of end of the curve.
    cos_angle_tolerance - [in] default = cos(1 degree) Used only
        when c is MYON::continuity::G1_continuous or MYON::continuity::G2_continuous.  If the
        cosine of the angle between two tangent vectors is 
        <= cos_angle_tolerance, then a G1 discontinuity is reported.
    curvature_tolerance - [in] (default = MYON_SQRT_EPSILON) Used 
        only when c is MYON::continuity::G2_continuous.  If K0 and K1 are 
        curvatures evaluated from above and below and 
        |K0 - K1| > curvature_tolerance, then a curvature 
        discontinuity is reported.
  Returns:
    Parametric continuity tests c = (C0_continuous, ..., G2_continuous):

      true if a parametric discontinuity was found strictly 
      between t0 and t1. Note well that all curves are 
      parametrically continuous at the ends of their domains.

    Locus continuity tests c = (C0_locus_continuous, ...,G2_locus_continuous):

      true if a locus discontinuity was found strictly between
      t0 and t1 or at t1 is the at the end of a curve.
      Note well that all open curves (IsClosed()=false) are locus
      discontinuous at the ends of their domains.  All closed 
      curves (IsClosed()=true) are at least C0_locus_continuous at 
      the ends of their domains.
  */
  bool GetNextDiscontinuity( 
                  int dir,
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
    Test continuity at a surface parameter value.
  Parameters:
    c - [in] continuity to test for
    s - [in] surface parameter to test
    t - [in] surface parameter to test
    hint - [in] evaluation hint
    point_tolerance - [in] if the distance between two points is
        greater than point_tolerance, then the surface is not C0.
    d1_tolerance - [in] if the difference between two first derivatives is
        greater than d1_tolerance, then the surface is not C1.
    d2_tolerance - [in] if the difference between two second derivatives is
        greater than d2_tolerance, then the surface is not C2.
    cos_angle_tolerance - [in] default = cos(1 degree) Used only when
        c is MYON::continuity::G1_continuous or MYON::continuity::G2_continuous.  If the cosine
        of the angle between two normal vectors 
        is <= cos_angle_tolerance, then a G1 discontinuity is reported.
    curvature_tolerance - [in] (default = MYON_SQRT_EPSILON) Used only when
        c is MYON::continuity::G2_continuous.  If K0 and K1 are curvatures evaluated
        from above and below and |K0 - K1| > curvature_tolerance,
        then a curvature discontinuity is reported.
  Returns:
    true if the surface has at least the c type continuity at the parameter t.
  Remarks:
    Overrides virtual MYON_Surface::IsContinuous
  */
  bool IsContinuous(
    MYON::continuity c,
    double s, 
    double t, 
    int* hint = nullptr,
    double point_tolerance=MYON_ZERO_TOLERANCE,
    double d1_tolerance=MYON_ZERO_TOLERANCE,
    double d2_tolerance=MYON_ZERO_TOLERANCE,
    double cos_angle_tolerance=MYON_DEFAULT_ANGLE_TOLERANCE_COSINE,
    double curvature_tolerance=MYON_SQRT_EPSILON
    ) const override;

  bool Reverse(  // reverse parameterizatrion, Domain changes from [a,b] to [-b,-a]
    int // dir  0 = "s", 1 = "t"
    ) override;

  bool Transpose() override; // transpose surface parameterization (swap "s" and "t")

  bool Evaluate( // returns false if unable to evaluate
         double, double, // evaluation parameters
         int,            // number of derivatives (>=0)
         int,            // array stride (>=Dimension())
         double*,        // array of length stride*(ndir+1)*(ndir+2)/2
         int = 0,        // optional - determines which quadrant to evaluate from
                         //         0 = default
                         //         1 from NE quadrant
                         //         2 from NW quadrant
                         //         3 from SW quadrant
                         //         4 from SE quadrant
         int* = 0        // optional - evaluation hint (int[2]) used to speed
                         //            repeated evaluations
         ) const override;

  MYON_Curve* IsoCurve(
         int,              // 0 first parameter varies and second parameter is constant
                           //   e.g., point on IsoCurve(0,c) at t is srf(t,c)
                           // 1 first parameter is constant and second parameter varies
                           //   e.g., point on IsoCurve(1,c) at t is srf(c,t)
         double            // value of constant parameter 
         ) const override;

  int GetNurbForm( // returns 0: unable to create NURBS representation
                   //            with desired accuracy.
                   //         1: success - returned NURBS parameterization
                   //            matches the surface's to wthe desired accuracy
                   //         2: success - returned NURBS point locus matches
                   //            the surfaces's to the desired accuracy but, on
                   //            the interior of the surface's domain, the 
                   //            surface's parameterization and the NURBS
                   //            parameterization may not match to the 
                   //            desired accuracy.
        MYON_NurbsSurface&,
        double = 0.0
        ) const override;

  int HasNurbForm( // returns 0: unable to create NURBS representation
                   //            with desired accuracy.
                   //         1: success - returned NURBS parameterization
                   //            matches the surface's to wthe desired accuracy
                   //         2: success - returned NURBS point locus matches
                   //            the surfaces's to the desired accuracy but, on
                   //            the interior of the surface's domain, the 
                   //            surface's parameterization and the NURBS
                   //            parameterization may not match to the 
                   //            desired accuracy.
        ) const override;

  bool GetSurfaceParameterFromNurbFormParameter(
        double nurbs_s, double nurbs_t,
        double* surface_s, double* surface_t
        ) const override;

  bool GetNurbFormParameterFromSurfaceParameter(
        double surface_s, double surface_t,
        double* nurbs_s,  double* nurbs_t
        ) const override;


  /*
  Description:
    Removes the portions of the surface outside of the specified interval.

  Parameters:
    dir - [in] 0  The domain specifies an sub-interval of Domain(0)
                  (the first surface parameter).
               1  The domain specifies an sub-interval of Domain(1)
                  (the second surface parameter).
    domain - [in] interval of the surface to keep. If dir is 0, then
        the portions of the surface with parameters (s,t) satisfying
        s < Domain(0).Min() or s > Domain(0).Max() are trimmed away.
        If dir is 1, then the portions of the surface with parameters
        (s,t) satisfying t < Domain(1).Min() or t > Domain(1).Max() 
        are trimmed away.
  */
  bool Trim(
         int dir,
         const MYON_Interval& domain
         ) override;

  /*
   Description:
     Where possible, analytically extends surface to include domain.
   Parameters:
     dir - [in] 0  new Domain(0) will include domain.
                   (the first surface parameter).
                1  new Domain(1) will include domain.
                   (the second surface parameter).
     domain - [in] if domain is not included in surface domain, 
     surface will be extended so that its domain includes domain.  
     Will not work if surface is closed in direction dir. 
     Original surface is identical to the restriction of the
     resulting surface to the original surface domain, 
   Returns:
     true if successful.
     */
  bool Extend(
    int dir,
    const MYON_Interval& domain
    ) override;

  /*
  Description:
    Splits (divides) the surface into two parts at the 
    specified parameter.

  Parameters:
    dir - [in] 0  The surface is split vertically.  The "west" side
                  is returned in "west_or_south_side" and the "east"
                  side is returned in "east_or_north_side".
               1  The surface is split horizontally.  The "south" side
                  is returned in "west_or_south_side" and the "north"
                  side is returned in "east_or_north_side".
    c - [in] value of constant parameter in interval returned
               by Domain(dir)
    west_or_south_side - [out] west/south portion of surface returned here
    east_or_north_side - [out] east/north portion of surface returned here

  Example:

          MYON_SumSurface srf = ...;
          int dir = 1;
          MYON_SumSurface* south_side = 0;
          MYON_SumSurface* north_side = 0;
          srf.Split( dir, srf.Domain(dir).Mid() south_side, north_side );

  */
  bool Split(
         int dir,
         double c,
         MYON_Surface*& west_or_south_side,
         MYON_Surface*& east_or_north_side
         ) const override;
};

#endif

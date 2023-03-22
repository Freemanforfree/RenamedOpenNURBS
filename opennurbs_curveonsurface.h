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

#if !defined(OPENNURBS_CURVE_MYON_SURFACE_INC_)
#define OPENNURBS_CURVE_MYON_SURFACE_INC_

class MYON_CLASS MYON_CurveOnSurface : public MYON_Curve
{
  MYON_OBJECT_DECLARE(MYON_CurveOnSurface);

public:
  MYON_CurveOnSurface() MYON_NOEXCEPT;

  /*
  Remarks:
    Deletes m_c2, m_c3, and m_s.  Use MYON_CurveProxy or MYON_SurfaceProxy
    if you need to use curves or a surface that you do not want deleted.
  */
  virtual ~MYON_CurveOnSurface();

private:
  MYON_CurveOnSurface(const MYON_CurveOnSurface&); // no implementation

private:
  MYON_CurveOnSurface& operator=(const MYON_CurveOnSurface&); // no implementation

#if defined(MYON_HAS_RVALUEREF)
public:
  // rvalue copy constructor
  MYON_CurveOnSurface( MYON_CurveOnSurface&& ) MYON_NOEXCEPT;

  // The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
  // which could throw exceptions.  See the implementation of
  // MYON_Object::operator=(MYON_Object&&) for details.
  MYON_CurveOnSurface& operator=( MYON_CurveOnSurface&& );
#endif

public:
  /*
  Parameters:
    p2dCurve - [in] ~MYON_CurveOnSurface() will delete this curve.
       Use an MYON_CurveProxy if you don't want the original deleted.
    p3dCurve - [in] ~MYON_CurveOnSurface() will delete this curve.
       Use an MYON_CurveProxy if you don't want the original deleted.
    pSurface - [in] ~MYON_CurveOnSurface() will delete this surface.
       Use an MYON_SurfaceProxy if you don't want the original deleted.
  */
  MYON_CurveOnSurface( MYON_Curve* p2dCurve,  // required 2d curve
                     MYON_Curve* p3dCurve,  // optional 3d curve
                     MYON_Surface* pSurface // required surface
                     );

  // virtual MYON_Object::SizeOf override
  unsigned int SizeOf() const override;


  /////////////////////////////////////////////////////////////////
  // MYON_Object overrides

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

  bool Transform( 
         const MYON_Xform&
         ) override;

  // (optional - default uses Transform for 2d and 3d objects)
  bool SwapCoordinates(
        int, int        // indices of coords to swap
        ) override;

  /////////////////////////////////////////////////////////////////
  // MYON_Curve overrides

  MYON_Interval Domain() const override;

  int SpanCount() const override; // number of smooth spans in curve

  bool GetSpanVector( // span "knots" 
         double* // array of length SpanCount() + 1 
         ) const override; // 

  int Degree( // returns maximum algebraic degree of any span 
                  // ( or a good estimate if curve spans are not algebraic )
    ) const override; 


  // (optional - override if curve is piecewise smooth)
  bool GetParameterTolerance( // returns tminus < tplus: parameters tminus <= s <= tplus
         double,  // t = parameter in domain
         double*, // tminus
         double*  // tplus
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
        const MYON_Interval* = nullptr     // OPTIONAL subdomain of 2d curve
        ) const override;

  /////////////////////////////////////////////////////////////////
  // Interface

  // ~MYON_CurveOnSurface() deletes these classes.  Use a
  // MYON_CurveProxy and/or MYON_SurfaceProxy wrapper if you don't want
  // the destructor to destroy the curves
  MYON_Curve* m_c2;  // REQUIRED parameter space (2d) curve
  MYON_Curve* m_c3;  // OPTIONAL 3d curve (approximation) to srf(crv2(t))
  MYON_Surface* m_s; 
};


#endif

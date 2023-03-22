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

#if !defined(MYON_TORUS_INC_)
#define MYON_TORUS_INC_

class MYON_RevSurface;
class MYON_TextLog;

/*
Description:
  The torus is defined by a major circle and minor radius.  The
  torus is parameterized by (major_angle,minor_angle).  The angles
  are specified in radians.  The domain of both parameters is (0,2pi).
*/
class MYON_CLASS MYON_Torus
{

public:
  // for expert users

  MYON_Plane plane; // major circle plane
  double major_radius;  // > minor_radius
  double minor_radius;  // > 0

public:

  MYON_Torus();
  MYON_Torus( const MYON_Plane& major__plane, double major__radius, double minor__radius );
  MYON_Torus( const MYON_Circle& major__circle, double minor__radius );
  ~MYON_Torus();

  bool IsValid( MYON_TextLog* text_log = nullptr ) const;

  bool Create( const MYON_Plane& major__plane, double major__radius, double minor__radius );
  bool Create( const MYON_Circle& major__circle, double minor__radius);

  /*
  Description:
    Get the circle that is the isocurve on the torus
    at a specified minor angle.
  Parameteters:
    minor_angle_radians - [in]
  Returns:
    A circle with normal major_circle.plane.zaxis that starts
    at PointAt( 0.0, minor_angle_radians ).
  See Also:
    MYON_Torus::MajorCircleRadians
    MYON_Torus::MajorCircleDegrees
    MYON_Torus::MinorCircleRadians
    MYON_Torus::MinorCircleDegrees
  */
  MYON_Circle MajorCircleRadians(double minor_angle_radians ) const;

  /*
  Description:
    Get the circle that is the isocurve on the torus
    at a specified minor angle.
  Parameteters:
    minor_angle_degrees - [in]
  Returns:
    A circle with normal major_circle.plane.zaxis that starts
    at PointAt( 0.0, minor_angle_degrees*MYON_PI/180.0 ).
  See Also:
    MYON_Torus::MajorCircleRadians
    MYON_Torus::MajorCircleDegrees
    MYON_Torus::MinorCircleRadians
    MYON_Torus::MinorCircleDegrees
  */
  MYON_Circle MajorCircleDegrees(double minor_angle_degrees) const;

  /*
  Description:
    Get the minor circle that is the isocurve on the torus
    at a specified major angle.
  Parameteters:
    major_angle_radians - [in]
  Returns:
    A circle with radius = minor_radis, 
    center = major_circle.PointAt(major_angle_radians), and
    starting point PointAt( major_angle_radians, 0.0 ).
  See Also:
    MYON_Torus::MajorCircleRadians
    MYON_Torus::MajorCircleDegrees
    MYON_Torus::MinorCircleRadians
    MYON_Torus::MinorCircleDegrees
  */
  MYON_Circle MinorCircleRadians(double major_angle_radians) const;

  /*
  Description:
    Get the minor circle that is the isocurve on the torus
    at a specified major angle.
  Parameteters:
    major_angle_degrees - [in]
  Returns:
    A circle with radius = minor_radis, 
    center = major_circle.PointAt(major_angle_degrees*MYON_PI/180.0), and
    starting point PointAt( major_angle_degrees*MYON_PI/180.0, 0.0 ).
  See Also:
    MYON_Torus::MajorCircleRadians
    MYON_Torus::MajorCircleDegrees
    MYON_Torus::MinorCircleRadians
    MYON_Torus::MinorCircleDegrees
  */
  MYON_Circle MinorCircleDegrees(double major_angle_degrees) const;

  MYON_3dPoint Center() const;
  MYON_3dVector Axis() const;
  double MajorRadius() const;
  double MinorRadius() const;

  MYON_3dPoint PointAt(
    double major_angle_radians, 
    double minor_angle_radians
    ) const;

  MYON_3dVector NormalAt(
    double major_angle_radians, 
    double minor_angle_radians
    ) const;

  // returns parameters of point on torus that is closest to test_point.
  bool ClosestPointTo( 
         MYON_3dPoint test_point, 
         double* major_angle_radians, 
         double* minor_angle_radians
         ) const;

  // returns point on torus that is closest to test_point
  MYON_3dPoint ClosestPointTo( 
         MYON_3dPoint test_point
         ) const;

  // rotate torus about its origin
  bool Rotate(
        double sin_angle,               // sin(angle)
        double cos_angle,               // cos(angle)
        const MYON_3dVector& axis_of_rotation // axis of rotation
        );

  bool Rotate(
        double angle_radians,               // angle in radians
        const MYON_3dVector& axis_of_rotation // axis of rotation
        );

  // rotate torus about a point and axis
  bool Rotate(
        double sin_angle,               // sin(angle)
        double cos_angle,               // cos(angle)
        const MYON_3dVector& axis_of_rotation, // axis of rotation
        const MYON_3dPoint& center_of_rotation  // center of rotation
        );

  bool Rotate(
        double angle_radians,               // angle in radians
        const MYON_3dVector& axis_of_rotation, // axis of rotation
        const MYON_3dPoint& center_of_rotation  // center of rotation
        );

  bool Translate(
        const MYON_3dVector&
        );

  bool Transform( const MYON_Xform& );

  // parameterization of NURBS surface does not match torus's transcendental paramaterization
  int GetNurbForm( MYON_NurbsSurface& ) const; // returns 0=failure, 2=success

  /*
  Description:
    Creates a surface of revolution definition of the torus.
  Parameters:
    srf - [in] if not nullptr, then this srf is used.
  Result:
    A surface of revolution or nullptr if the torus is not valid.
  */
  MYON_RevSurface* RevSurfaceForm( MYON_RevSurface* srf = nullptr ) const;
};

#endif

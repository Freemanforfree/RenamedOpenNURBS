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

#if !defined(MYON_SPHERE_INC_)
#define MYON_SPHERE_INC_

class MYON_RevSurface;

class MYON_CLASS MYON_Sphere
{
public:
  
  MYON_Plane plane; // equatorial plane
  double radius;  // > 0

  MYON_Sphere();
  MYON_Sphere( const MYON_3dPoint& center, double radius ); // center, radius
  ~MYON_Sphere();

  bool IsValid() const;

  bool Create( const MYON_3dPoint& center, double radius); // center radius

  MYON_Circle LatitudeRadians(double latitude_radians ) const;
  MYON_Circle LatitudeDegrees(double latitude_degrees) const;
  MYON_Circle LongitudeRadians(double longitude_radians) const;
  MYON_Circle LongitudeDegrees(double longitude_degrees) const;

  MYON_3dPoint Center() const;
  MYON_3dPoint NorthPole() const;
  MYON_3dPoint SouthPole() const;
  double Diameter() const;
  double Radius() const;

  MYON_3dPoint PointAt(
    double longitude_radians, 
    double latitude_radians
    ) const;   // longitude [0,2pi], latitude [-pi/2,pi/2] in radians

  MYON_3dVector NormalAt(
    double longitude_radians, 
    double latitude_radians
    ) const;   // longitude [0,2pi], latitude [-pi/2,pi/2] in radians

  MYON_BoundingBox BoundingBox() const;

  // returns parameters of point on sphere that is closest to given point
  bool ClosestPointTo( 
         MYON_3dPoint test_point, 
         double* longitude_radians, // longitude  [0,2pi)
         double* latitude_radians // latitude   [-pi/2,pi/2]
         ) const;

  // returns point on sphere that is closest to given point
  MYON_3dPoint ClosestPointTo( 
         MYON_3dPoint test_point
         ) const;

  // For intersections see MYON_Intersect();

  // rotate sphere about its origin
  bool Rotate(
        double sin_angle,               // sin(angle)
        double cos_angle,               // cos(angle)
        const MYON_3dVector& axis_of_rotation // axis of rotation
        );

  bool Rotate(
        double angle_radians,               // angle in radians
        const MYON_3dVector& axis_of_rotation // axis of rotation
        );

  // rotate sphere about a point and axis
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

  // parameterization of NURBS surface does not match sphere's transcendental paramaterization
  int GetNurbForm( MYON_NurbsSurface& ) const; // returns 0=failure, 2=success

  /*
  Description:
    Creates a surface of revolution definition of the sphere.
  Parameters:
    bArcLengthParameterization - [in]
      true: 
        The domain will be set to (0,radius*2*pi)x(-radius*pi/2,radius*pi/2)
      false: 
        The domain will be set to (0,2*pi)x(-pi/2,pi/2)
    srf - [in]
      if not nullptr, then this srf is used.
  Result:
    A surface of revolution or nullptr if the sphere is not valid.
  */
  MYON_RevSurface* RevSurfaceForm( bool bArcLengthParameterization, MYON_RevSurface* srf = nullptr ) const;

  MYON_DEPRECATED_MSG("Call RevSurfaceForm(false,srf)")
  MYON_RevSurface* RevSurfaceForm( MYON_RevSurface* srf = nullptr ) const;
};

#endif

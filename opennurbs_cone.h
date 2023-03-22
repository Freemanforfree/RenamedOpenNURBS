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

#if !defined(MYON_CONE_INC_)
#define MYON_CONE_INC_

class MYON_NurbsSurface;
class MYON_Brep;

// Description:
//   Lightweight right circular cone.  Use MYON_ConeSurface if
//   you need MYON_Cone geometry as a virtual MYON_Surface.
class MYON_CLASS MYON_Cone
{
public:

  // Creates a cone with world XY plane as the base plane,
  // center = (0,0,0), radius = 0.0, height = 0.0.
  MYON_Cone();

  // See MYON_Cone::Create.
  MYON_Cone(
    const MYON_Plane& plane,
    double height,
    double radius
    );

  ~MYON_Cone();

  // Description:
  //   Creates a right circular cone from a plane, height,
  //   and radius.
  //  plane - [in] The apex of cone is at plane.origin and
  //      the axis of the cone is plane.zaxis.
  //  height - [in] The center of the base is height*plane.zaxis.
  //  radius - [in] tan(cone angle) = radius/height
  bool Create(
    const MYON_Plane& plane,
    double height,
    double radius
    );

  // Returns true if plane is valid, height is not zero, and
  // radius is not zero.
  bool IsValid() const;

  // Returns:
  //   Center of base circle.
  // Remarks:
  //   The base point is plane.origin + height*plane.zaxis.
  MYON_3dPoint BasePoint() const;

  // Returns:
  //   Point at the tip of the cone.
  // Remarks:
  //   The apex point is plane.origin.
  const MYON_3dPoint& ApexPoint() const;

  // Returns:
  //   Unit vector axis of cone.
  const MYON_3dVector& Axis() const;

  // Returns:
  //   The angle (in radians) between the axis and the 
  //   side of the cone.
  //   The angle and the height have the same sign.
  double AngleInRadians() const;

  // Returns:
  //   The angle Iin degrees) between the axis and the side.
  //   The angle and the height have the same sign.
  double AngleInDegrees() const;           

  // evaluate parameters and return point
  // Parameters:
  //   radial_parameter - [in] 0.0 to 2.0*MYON_PI
  //   height_parameter - [in] 0 = apex, height = base
  MYON_3dPoint PointAt(
    double radial_parameter,
    double height_parameter
    ) const;

  // Parameters:
  //   radial_parameter - [in] (in radians) 0.0 to 2.0*MYON_PI
  //   height_parameter - [in] 0 = apex, height = base
  // Remarks:
  //   If radius>0 and height>0, then the normal points "out"
  //   when height_parameter >= 0.
  MYON_3dVector NormalAt(
    double radial_parameter,
    double height_parameter
    ) const;

  // Description:
  //   Get iso curve circle at a specified height.
  // Parameters:
  //   height_parameter - [in] 0 = apex, height = base
  MYON_Circle CircleAt( 
    double height_parameter
    ) const;

  // Description:
  //   Get iso curve line segment at a specified angle.
  // Parameters:
  //   radial_parameter - [in] (in radians) 0.0 to 2.0*MYON_PI
  MYON_Line LineAt( 
    double radial_parameter 
    ) const;

  // returns parameters of point on cone that is closest to given point
  bool ClosestPointTo( 
          MYON_3dPoint point, 
          double* radial_parameter,
          double* height_parameter
         ) const;

  // returns point on cone that is closest to given point
  MYON_3dPoint ClosestPointTo( 
         MYON_3dPoint 
         ) const;

  bool Transform( const MYON_Xform& );

  // rotate cone about its origin
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& axis_of_rotation
        );

  bool Rotate(
        double angle_in_radians,
        const MYON_3dVector& axis_of_rotation
        );

  // rotate cone about a point and axis
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& axis_of_rotation,
        const MYON_3dPoint& center_of_rotation
        );
  bool Rotate(
        double angle_in_radians,
        const MYON_3dVector& axis_of_rotation,
        const MYON_3dPoint& center_of_rotation
        );

  bool Translate(
        const MYON_3dVector& delta
        );

  /*
  returns:
    0 = failure
    2 = success
  */
  int GetNurbForm( MYON_NurbsSurface& ) const;

  /*
  Description:
    Creates a surface of revolution definition of the cylinder.
  Parameters:
    srf - [in] if not nullptr, then this srf is used.
  Result:
    A surface of revolution or nullptr if the cylinder is not 
    valid or is infinite.
  */
  MYON_RevSurface* RevSurfaceForm( MYON_RevSurface* srf = nullptr ) const;

public:
  MYON_Plane plane; // apex = plane.origin, axis = plane.zaxis
  double   height; // not zero
  double   radius; // not zero
};

#endif

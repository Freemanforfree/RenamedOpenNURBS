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

#if !defined(MYON_BOX_INC_)
#define MYON_BOX_INC_

class MYON_CLASS MYON_Box
{
public:
  MYON_Plane plane; 
  // intervals are finite and increasing when the box is valid
  MYON_Interval dx;
  MYON_Interval dy;
  MYON_Interval dz;

  MYON_Box();
  MYON_Box( const MYON_BoundingBox& bbox );
  ~MYON_Box();

  bool IsValid() const;

  bool Create( const MYON_BoundingBox& bbox );

  void Destroy();

  MYON_3dPoint Center() const;
  bool GetCorners( MYON_3dPoint* corners ) const;
  bool GetCorners( MYON_SimpleArray<MYON_3dPoint>& corners ) const;

  MYON_BoundingBox BoundingBox() const;

  MYON_3dPoint PointAt( 
          double r, 
          double s, 
          double t 
          ) const;

  bool ClosestPointTo( 
          MYON_3dPoint point, 
          double* r, 
          double* s, 
          double* t 
          ) const;

  // returns point on box that is closest to given point
  MYON_3dPoint ClosestPointTo( 
         MYON_3dPoint test_point
         ) const;

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

  /*
  Description:
    Test the box to see if it is degenerate (flat)
    in one or more directions.
  Parameters:
    tolerance - [in] Distances <= tolerance will be considered
        to be zero.  If tolerance is negative (default), then
        a scale invariant tolerance is used.
  Returns:
    @untitled table
    0     box is not degenerate
    1     box is a rectangle (degenerate in one direction)
    2     box is a line (degenerate in two directions)
    3     box is a point (degenerate in three directions)
    4     box is not valid
  */
  int IsDegenerate( 
    double tolerance = MYON_UNSET_VALUE
    ) const;

  double Volume() const;

  double Area() const;
};

#endif

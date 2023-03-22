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

#if !defined(OPENNURBS_CYLINDER_INC_)
#define OPENNURBS_CYLINDER_INC_

class MYON_NurbsSurface;
class MYON_RevSurface;
class MYON_Brep;

/*
Description:
  MYON_Cylinder is a right circular cylinder.
*/
class MYON_CLASS MYON_Cylinder
{
public:
  MYON_Cylinder(); // zeros all fields - cylinder is invalid

  MYON_Cylinder( // infinite cylinder
    const MYON_Circle&  // point on the bottom plane
    );

  MYON_Cylinder( // infinite cylinder
    const MYON_Circle&,  // point on the bottom plane
    double             // height
    );

  ~MYON_Cylinder();

  bool Create(
    const MYON_Circle&  // point on the bottom plane
    );

  bool Create(
    const MYON_Circle&,  // point on the bottom plane
    double               // height
    );

  bool IsValid() const; // returns true if all fields contain reasonable
                        // information and equation jibes with point and Z.

  bool IsFinite() const; // returns true if the cylinder is finite
                         // (height[0] != height[1]) and false if the
                         // cylinder is infinite.

  const MYON_3dVector& Axis() const;
  const MYON_3dPoint& Center() const;
  double Height() const; // returns 0 for infinite cylinder
  MYON_Circle CircleAt( 
        double // linear parameter
        ) const;
  MYON_Line LineAt( 
        double // angular parameter
        ) const;

  // evaluate parameters and return point
  MYON_3dPoint PointAt(
    double, // angular parameter [0,2pi]
    double  // linear parameter (height from base circle's plane)
    ) const;
  MYON_3dPoint NormalAt(
    double, // angular parameter [0,2pi]
    double  // linear parameter (height from base circle's plane)
    ) const;

  // returns parameters of point on cylinder that is closest to given point
  bool ClosestPointTo( 
         MYON_3dPoint, 
         double*, // angular parameter [0,2pi]
         double*  // linear parameter (height from base circle's plane)
         ) const;
  // returns point on cylinder that is closest to given point
  MYON_3dPoint ClosestPointTo( 
         MYON_3dPoint 
         ) const;

  // For intersections see MYON_Intersect();

  // rotate cylinder about its origin
  bool Rotate(
        double,               // sin(angle)
        double,               // cos(angle)
        const MYON_3dVector&  // axis of rotation
        );
  bool Rotate(
        double,               // angle in radians
        const MYON_3dVector&  // axis of rotation
        );

  // rotate cylinder about a point and axis
  bool Rotate(
        double,               // sin(angle)
        double,               // cos(angle)
        const MYON_3dVector&, // axis of rotation
        const MYON_3dPoint&   // center of rotation
        );
  bool Rotate(
        double,              // angle in radians
        const MYON_3dVector&, // axis of rotation
        const MYON_3dPoint&   // center of rotation
        );

  bool Translate(
        const MYON_3dVector&
        );

  // parameterization of NURBS surface does not match cylinder's transcendental paramaterization
  int GetNurbForm( MYON_NurbsSurface& ) const; // returns 0=failure, 2=success

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

public: // members left public
  // base circle
  MYON_Circle  circle;

  
  // If height[0] = height[1], the cylinder is infinite,
  // Otherwise, height[0] < height[1] and the center of
  // the "bottom" cap is 
  //
  //          circle.plane.origin + height[0]*circle.plane.zaxis,
  //
  // and the center of the top cap is 
  //
  //          circle.plane.origin + height[1]*circle.plane.zaxis.
  double height[2];
};

#endif

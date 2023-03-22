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

#if !defined(MYON_PLANE_INC_)
#define MYON_PLANE_INC_

class MYON_CLASS MYON_Plane
{
public:

  /*
  Description:
    The default constructor creates a plane
    with orgin=(0,0,0), xaxis=(1,0,0), yaxis=(0,1,0)
    zaxis=(0,0,1), and equation=(0,0,1,0).
  */
  MYON_Plane();

  /*
  Description:
    Construct a plane from a point and normal vector.
  Parameters:
    origin - [in] point on the plane
    normal - [in] non-zero normal to the plane
  Remarks:
    origin = point, zaxis = unitized normal, xaxis
    xaxis set with xaxis.PerpindicularTo(zaxis).
  See Also:
    MYON_Plane::CreateFromNormal
  */
  MYON_Plane(
    const MYON_3dPoint& origin,
    const MYON_3dVector& normal
    );



  /*
  Description:
    Construct a plane from a point, and two vectors in
    the plane.
  Parameters:
    origin - [in] point on the plane
    x_dir - [in] non-zero vector in the plane that
        determines the xaxis direction.
    y_dir - [in] non-zero vector not parallel to x_dir
        that is used to determine the yaxis direction.
        y_dir does not have to be perpendicular to x_dir.
  */
  MYON_Plane(
    const MYON_3dPoint& origin,
    const MYON_3dVector& x_dir,
    const MYON_3dVector& y_dir
    );

  /*
  Description:
    Construct a plane from three non-collinear points.
  Parameters:
    origin - [in] point on the plane
    x_point - [in] second point in the plane.
        The xaxis will be parallel to x_point-origin.
    y_point - [in] third point on the plane that is
        not collinear with the first two points.
        yaxis*(y_point-origin) will be > 0.
  */
  MYON_Plane(
    const MYON_3dPoint& origin,
    const MYON_3dPoint& x_point,
    const MYON_3dPoint& y_point
    );

  /*
  Description:
    Construct a plane from an equation.
  Parameters:
    equation - [in] an array of 4 doubles with
       one of equation[0], equation[1], or equation[2]
       being non-zero.
  */
  MYON_Plane(
    const double equation[4]
    );

  MYON_Plane(
    const MYON_PlaneEquation& plane_equation
    );

  ~MYON_Plane();

  bool operator==(const MYON_Plane&) const;
  bool operator!=(const MYON_Plane&) const;

  /*
  Description:
    Create a plane from a point and normal vector.
  Parameters:
    origin - [in] point on the plane
    normal - [in] non-zero normal to the plane
  Remarks:
    origin = point, zaxis = unitized normal, xaxis
    xaxis set with xaxis.PerpindicularTo(zaxis).
  Returns:
    true if valid plane is created.
  See Also:
    MYON_Plane::CreateFromNormalYup
  */
  bool CreateFromNormal(
    const MYON_3dPoint& origin,
    const MYON_3dVector& normal
    );

  /*
Description:
  Construct a plane from a point and normal vector and a vector that
  projects to the positive y-axis.
Parameters:
  origin - [in] point on the plane
  normal - [in] non-zero normal to the plane
  y-up - [in] vector linearly independent from normal that projects
              to the positive y-axis of the plane
Remarks:
  origin = point,
  zaxis = unitized normal,
  xaxis = unitized ( y-up X normal )
  yaxis = zaxis X xaxis
See Also:
  MYON_Plane::CreateFromNormal
*/
 bool CreateFromNormalYup(
    const MYON_3dPoint& origin,
    const MYON_3dVector& normal,
    const MYON_3dVector& y_up
  );


  /*
  Description:
    Construct a plane from a point, and two vectors in
    the plane.
  Parameters:
    origin - [in] point on the plane
    x_dir - [in] non-zero vector in the plane that
        determines the xaxis direction.
    y_dir - [in] non-zero vector not parallel to x_dir
        that is used to determine the yaxis direction.
        y_dir does not have to be perpendicular to x_dir.
  Returns:
    true if valid plane is created.
  */
  bool CreateFromFrame(
    const MYON_3dPoint& origin,
    const MYON_3dVector& x_dir,
    const MYON_3dVector& y_dir
    );

  /*
  Description:
    Construct a plane from three non-collinear points.
  Parameters:
    origin - [in] point on the plane
    point_on_x - [in] second point in the plane.
        The xaxis will be parallel to x_point-origin.
    point_on - [in] third point on the plane that is
        not collinear with the first two points.
        yaxis*(y_point-origin) will be > 0.
  Returns:
    true if valid plane is created.
  */
  bool CreateFromPoints(
    const MYON_3dPoint& origin,
    const MYON_3dPoint& point_on_x,
    const MYON_3dPoint& point_on
    );

  /*
  Description:
    Construct a plane from an equation.
  Parameters:
    equation - [in] an array of 4 doubles with
       one of equation[0], equation[1], or equation[2]
       being non-zero.
  Remarks:
    points on the plane will satisfy 
    x*equation[0] +y*equation[1] + z*equation[2] + equation[3] = 0
  Returns:
    true if valid plane is created.
  */
  bool CreateFromEquation(
    const double equation[4]
    );

  bool CreateFromEquation(
    const class MYON_PlaneEquation& plane_equation
    );

  /*
  Returns:
    MYON_Plane::UnsetPlane if input is not valid.
  */
  static MYON_Plane FromPointList(
    size_t point_list_count,
    const MYON_3dPoint* point_list
    );

  /*
  Returns:
    MYON_Plane::UnsetPlane if input is not valid.
  */
  static MYON_Plane FromPointList(
    const MYON_SimpleArray< MYON_3dPoint >& point_list
    );


  /*
  Returns:
    MYON_Plane::UnsetPlane if input is not valid.
  */
  static MYON_Plane FromPointList(
    size_t point_list_count,
    const MYON_3fPoint* point_list
    );

  /*
  Returns:
    MYON_Plane::UnsetPlane if input is not valid.
  */
  static MYON_Plane FromPointList(
    const MYON_SimpleArray< MYON_3fPoint >& point_list
    );

  /*
  Returns:
    MYON_Plane::UnsetPlane if input is not valid.
  */
  static MYON_Plane FromPointList(
    const class MYON_3dPointListRef& point_list
    );

  /*
  Returns:
    MYON_Plane::UnsetPlane if input is not valid.
  */
  static MYON_Plane FromPointList(
    size_t point_index_count,
    const unsigned int* point_index_list,
    const class MYON_3dPointListRef& point_list
    );

  static MYON_Plane FromPointList(
    size_t point_index_count,
    size_t point_index_stride,
    const unsigned int* point_index_list,
    const class MYON_3dPointListRef& point_list
    );

  /*
  Description:
    Test plane to see if it is valid.
  Returns:
    true if all fields contain reasonable
    information and equation jibes with point and zaxis.
  */
  bool IsValid() const;

  /*
  Returns:
    Plane origin.
  */
  const MYON_3dPoint& Origin() const;

  /*
  Returns:
    Plane unit x-axis.
  */
  const MYON_3dVector& Xaxis() const;

  /*
  Returns:
    Plane unit y-axis.
  */
  const MYON_3dVector& Yaxis() const;

  /*
  Returns:
    Plane unit normal.
  */
  const MYON_3dVector& Normal() const;


  /*
  Description:
    Set the origin and update the plane equation
  Parameters:
    origin - [in] the new origin
  */
  void SetOrigin( const MYON_3dPoint& origin );
  
  /*
  Description:
    Evaluate a point on the plane
  Parameters:
    u - [in]
    v - [in] evaluation parameters
  Returns:
    plane.origin + u*plane.xaxis + v*plane.yaxis
  */
  MYON_3dPoint PointAt(
    double u,
    double v
    ) const;

  /*
  Description:
    Evaluate a point on the plane
  Parameters:
    u - [in]
    v - [in] evaluation parameters
    w - [in] elevation parameter
  Returns:
    plane.origin + u*plane.xaxis + v*plane.yaxis + z*plane.zaxis
  */
  MYON_3dPoint PointAt(
    double u,
    double v,
    double w
    ) const;

  /*
  Description:
    Get an isoparameteric line on the plane.
  Parameters:
    dir - [in] direction of iso-parametric line
        0: first parameter varies and second parameter is constant
           e.g., line(t) = plane(t,c)
        1: first parameter is constant and second parameter varies
           e.g., line(t) = plane(c,t)
    c - [in] value of constant parameter 
  Returns:
    iso-parametric line
  */
  MYON_Line IsoLine(
         int dir,
         double c
         ) const;

  /*
  Description:
    Get signed distance from the plane to a point.
  Parameters:
    point - [in]
  Returns:
    Signed distance from a point to a plane.
  Remarks:
    If the point is on the plane, the distance is 0.
    If the point is above the plane, the distance is > 0.
    If the point is below the plane the distance is < 0.
    The zaxis determines the plane's orientation.
  */
  double DistanceTo( 
        const MYON_3dPoint& point
        ) const;


  bool GetDistanceToBoundingBox(
           //returns false if plane has zero length normal
				   const MYON_BoundingBox&, // Box

           //output
				   double* min,    // min signed dist from plane to box 
           double* max     //max signed dist from plane to box
           ) const;

  /*
  Description:
    Update the plane equation based on the current values
    of the origin and zaxis.
  Returns:
    true if successful.  false if zaxis is zero.
  Remarks:
    If you modify a plane's origin or zaxis, call UpdateEquation()
    to set equation[]. 
  */
  bool UpdateEquation();

  /*
  Description:
    Get point on plane that is closest to a given point.
  Parameters:
    world_point - [in] 3d point
    u - [out] 
    v - [out] The point MYON_Plane::PointAt(*u,*v) is the point
              on the plane that is closest to world_point.
  Returns:
    true if successful.
  */
  bool ClosestPointTo( 
         MYON_3dPoint world_point,
         double* u,
         double* v
         ) const;

  /*
  Description:
    Get point on plane that is closest to a given point.
  Parameters:
    point - [in]
  Returns:
    A 3d point on the plane that is closest to world_point.
  */
  MYON_3dPoint ClosestPointTo( 
         MYON_3dPoint point
         ) const;

  // For intersections see MYON_Intersect();

  /*
  Description:
    Transform plane.
  Parameters:
    xform - [in] transformation to apply to plane
  Returns:
    true if successful
  */
  bool Transform( 
        const MYON_Xform& xform
        );


  /*
  Description:
    Transform a plane by swapping coordinates.
  Parameters:
    i - [in]
    j - [in] indices of coordinates to swap.
        0 = x coordinate, 1 = y coordinate, 2 = z coordinate.
  Returns:
    true if successful.
  */
  bool SwapCoordinates(
        int i,
        int j
        );

  /*
  Description:
    Rotate a plane about its origin.
  Parameters:
    sin_angle - [in] sine of rotation angle
    cos_angle - [in] cosine of rotation angle
    axis - [in] axis of rotation
  Returns:
    true if successful
  */
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& axis
        );

  /*
  Description:
    Rotate a plane about its origin.
  Parameters:
    angle - [in] rotation angle in radians
    axis - [in] axis of rotation
  Returns:
    true if successful
  */
  bool Rotate(
        double angle,
        const MYON_3dVector& axis
        );

  /*
  Description:
    Rotate a plane about a point.
  Parameters:
    sin_angle - [in] sine of rotation angle
    cos_angle - [in] cosine of rotation angle
    axis - [in] axis of rotation
    center - [in] center of rotation
  Returns:
    true if successful
  */
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& axis,
        const MYON_3dPoint&  center
        );

  /*
  Description:
    Rotate a plane about a point.
  Parameters:
    angle - [in] rotation angle in radians
    axis - [in] axis of rotation
    center - [in] center of rotation
  Returns:
    true if successful
  */
  bool Rotate(
        double angle,
        const MYON_3dVector& axis,
        const MYON_3dPoint& center
        );

  /*
  Description:
    Translate a plane.
  Parameters:
    delta - [in] translation vector
  Returns:
    true if successful
  */
  bool Translate(
        const MYON_3dVector&  delta
        );

  /*
  Description:
    Flip plane orientation by swapping x and y axes,
    reversing the zaxis, and updating the equation.
  Returns:
    true if successful
  */
  bool Flip();

  // world coordinate system MYON_Plane(MYON_3dPoint::Origin, MYON_3dVector::XAxis, MYON_3dVector::YAxis); 
  const static MYON_Plane World_xy;

  // world coordinate system MYON_Plane(MYON_3dPoint::Origin, MYON_3dVector::YAxis, MYON_3dVector::ZAxis); 
  const static MYON_Plane World_yz;

  // world coordinate system MYON_Plane(MYON_3dPoint::Origin, MYON_3dVector::ZAxis, MYON_3dVector::XAxis); 
  const static MYON_Plane World_zx;

  // All values are MYON_UNSET_VALUE.
	const static
	MYON_Plane UnsetPlane;

  // All values are MYON_DBL_QNAN.
	const static MYON_Plane NanPlane;

public:
  // origin of plane
  MYON_3dPoint  origin;

  // unit X axis of plane
  MYON_3dVector xaxis;

  // unit Y axis of plane
  MYON_3dVector yaxis;

  // unit Z axis of plane
  MYON_3dVector zaxis;

  // equation of plane
  MYON_PlaneEquation plane_equation;
  //double equation[4];
};

class MYON_CLASS MYON_ClippingPlaneInfo
{
public:
  // C++ defaults for construction, destruction, copy construction
  // and operator= work fine.

  // A point is visible if m_plane_equation.ValueAt(point) <= 0.
  // (This is the opposite convention from what OpenGL uses.)
  MYON_PlaneEquation m_plane_equation;
  MYON_UUID m_plane_id;
  bool m_bEnabled;

  // A distance where the clipping plane does not clip geometry.
  // By default, distance is MYON_DBL_MAX which indicates that there is no
  // distance being applied
  // A positive value is equivalent to placing another clipping plane at a
  // distance from this clipping plane along it's normal and then flipping it
  double Distance() const;
  void SetDistance(double distance);

  void Default();
  bool Write( MYON_BinaryArchive& ) const;
  bool Read( MYON_BinaryArchive& );

private:
  char m_reserved[3];

  // This should be a double, but is a float in order to not change
  // the class size. When the Rhino SDK can break, this data type should change.
  float m_distance = MYON_UNSET_POSITIVE_FLOAT;
};

class MYON_CLASS MYON_ClippingPlane
{
public:
  MYON_ClippingPlane();
  ~MYON_ClippingPlane();

  void Default();

  MYON_Plane m_plane;
  MYON_UuidList m_viewport_ids; //ids of viewports that this clipping plane "clips"
  MYON_UUID m_plane_id;
  bool m_bEnabled; // true if this clipping plane is active

  MYON_ClippingPlaneInfo ClippingPlaneInfo() const;

  // A distance where the clipping plane does not clip geometry.
  // By default, distance is a negative value which indicates that there is no
  // distance being applied
  // A positive value is equivalent to placing another clipping plane at a
  // distance from this clipping plane along it's normal and then flipping it
  double Distance() const;
  void SetDistance(double distance);

  bool Read( class MYON_BinaryArchive& );
  bool Write( class MYON_BinaryArchive& ) const;

private:
  char m_reserved[3];

  // This should be a double, but is a float in order to not change
  // the class size. When the Rhino SDK can break, this data type should change.
  float m_distance = MYON_UNSET_POSITIVE_FLOAT;
};


#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Plane>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_ClippingPlane>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_ClippingPlaneInfo>;
#endif

extern MYON_EXTERN_DECL const MYON_Plane MYON_xy_plane;
extern MYON_EXTERN_DECL const MYON_Plane MYON_yz_plane;
extern MYON_EXTERN_DECL const MYON_Plane MYON_zx_plane;

/*
Description:
  Get a convex hull of a set of 3d points.
Parameters:
  points - [in]
    List of points.  This function can handle tens of points
    but is too slow for hundreds of points.
  hull -[out]
    Equations of the sides of the convex hull are appended to
    this list.  
    A point P is inside the hull if hull[i].ValueAt(P) <= 0 for
    every plane equation.
Returns:
  Number of equations appended to hull[] array.
  If 0, then the points are coincident or collinear.
  If 2, then the points are coplanar and the returned
  planes are parallel.
  If >= 4, then the points are in a 3d convex hull.
*/
MYON_DECL
int MYON_Get3dConvexHull( 
          const MYON_SimpleArray<MYON_3dPoint> & points, 
          MYON_SimpleArray<MYON_PlaneEquation> & hull 
          );

#endif

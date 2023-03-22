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

#if !defined(OPENNURBS_PHOTOGRAMMETRY_INC_)
#define OPENNURBS_PHOTOGRAMMETRY_INC_

class MYON_CLASS MYON_AerialPhotoImageFrustum
{
public:
  MYON_AerialPhotoImageFrustum();

  bool IsSet() const;

  bool HeightIsSet() const;
  bool CornersAreSet() const;
  bool UnitSystemIsSet() const;

  void Unset();

  // The frustum unit system must be set. It is common
  // for the frustum unit system to be millimeters.
  MYON_UnitSystem m_unit_system;

  // The frustum's apex height must be positive.
  // It is common for the image frustum height
  // to be a camera's focal length.
  double m_height;

  // The corners must be the vertices of a 3 or 4 sided
  // convex region and must have counter-clockwise order
  // in the 2d plane.  It is common for the corners to be
  // a rectangle (lower left, lower right, upper right, 
  // upper left) and for the rectangle to be symmetric
  // about (0,0).  However, when the image has been cropped,
  // the frustum can be skew.  The frustum's apex point is 
  // always directly above (0,0).
  MYON_2dPoint m_corners[4];
};

class MYON_CLASS MYON_AerialPhotoCameraPosition
{
public:
  MYON_AerialPhotoCameraPosition();

  /*
  Returns:
    True if both the location and orientation are set.
  */
  bool IsSet() const;

  void Unset();

  /////////////////////////////////////////////////////////
  //
  // Camera position unit system
  //
  bool UnitSystemIsSet() const;
  bool SetUnitSystem( MYON::LengthUnitSystem unit_system );
  bool SetUnitSystem ( MYON_UnitSystem unit_system );
  MYON_UnitSystem UnitSystem() const;
  bool GetUnitSystem( MYON_UnitSystem& unit_system ) const;
  void UnsetUnitSystem();


  /////////////////////////////////////////////////////////
  //
  // Location interface
  //
  bool LocationIsSet() const;

  bool SetLocation( 
    MYON_3dPoint camera_location 
    );

  bool GetLocation(
    MYON_3dPoint& camera_location
    ) const;

  MYON_3dPoint Location() const;

  void UnsetLocation();

  /////////////////////////////////////////////////////////
  //
  // Orientation interface
  //
  bool OrientationIsSet() const;    

  bool GetOrientationAnglesDegrees(
    double* omega_degrees, 
    double* phi_degrees, 
    double* kappa_degrees
    ) const;

  bool GetOrientationAnglesRadians(
    double* omega_radians, 
    double* phi_radians, 
    double* kappa_radians
    ) const;

  /*
  Description:
    Get a right handed ortho normal camera frame.
  Parameters:
    camera_X - [out]
      world coordinate unit vector pointing to the right in the camera image
    camera_Y - [out]
      world coordinate unit vector in the camera up direction.
    camera_Z - [out]
      world coordinate unit vector pointing into the cameara (from the
      image toward the camera).
  */
  bool GetOrientationFrame(
    MYON_3dVector& camera_X,
    MYON_3dVector& camera_Y,
    MYON_3dVector& camera_Z
    ) const;

  bool GetOrientationUp(
    MYON_3dVector& camera_up
    ) const;

  bool GetOrientationRight(
    MYON_3dVector& camera_right
    ) const;

  bool GetOrientationDirection(
    MYON_3dVector& camera_direction
    ) const;


  /*
  Returns:
    A rotation transformation "R" such that
    camera right = R*MYON_3dVector::XAxis
    camera up = R*MYON_3dVector::YAxis
    camera direction = -R*MYON_3dVector::ZAxis
  */
  bool GetOrientationRotation(
    MYON_Xform& camera_rotaion 
    ) const;

  MYON_Xform OrientationRotation() const;

  /*
  Description:
    Set camera orientation information from rotation angles
    in radians.

  Remarks:
    There are four ways to specify the camera's orientation.
    1) Use SetOrientationAnglesRadians() to set
       camera orientation information from rotation angles
       in radians.
    2) Use SetOrientationAnglesDegrees() to set
       camera orientation information from rotation angles
       in degrees.
    3) Use SetOrientationVectors() to set 
       camera orientation information from vectors
       that report the camera's up, right and direction.
    3) Use SetOrientationRotation() to set 
       camera orientation information from a rotation
       matrix.
    Use the method for which you have the most accurate input
    and the other values will be calculated as accurately as
    possible.
  */
  bool SetOrientationAnglesRadians(
    double omega_radians, 
    double phi_radians, 
    double kappa_radians
    );

  /*
  Description:
    Set camera orientation information from rotation angles
    in degrees.

  Remarks:
    There are four ways to specify the camera's orientation.
    1) Use SetCameraOrientationAnglesRadians() to set
       camera orientation information from rotation angles
       in radians.
    2) Use SetCameraOrientationAnglesDegrees() to set
       camera orientation information from rotation angles
       in degrees.
    3) Use SetCameraOrientationVectors() to set 
       camera orientation information from vectors
       that report the camera's up, right and direction.
    3) Use SetCameraOrientationRotation() to set 
       camera orientation information from a rotation
       matrix.
    Use the method for which you have the most accurate input
    and the other values will be calculated as accurately as
    possible.
  */
  bool SetOrientationAnglesDegrees(
    double omega_degrees, 
    double phi_degrees, 
    double kappa_degrees
    );

  /*
  Description:
    Set camera orientation information from up, right
    and direction vectors.

  Remarks:
    There are four ways to specify the camera's orientation.
    1) Use SetCameraOrientationAnglesRadians() to set
       camera orientation information from rotation angles
       in radians.
    2) Use SetCameraOrientationAnglesDegrees() to set
       camera orientation information from rotation angles
       in degrees.
    3) Use SetCameraOrientationVectors() to set 
       camera orientation information from vectors
       that report the camera's up, right and direction.
    3) Use SetCameraOrientationRotation() to set 
       camera orientation information from a rotation
       matrix.
    Use the method for which you have the most accurate input
    and the other values will be calculated as accurately as
    possible.
  */
  bool SetOrientationVectors(
    MYON_3dVector camera_up,
    MYON_3dVector camera_right,
    MYON_3dVector camera_direction
    );

  /*
  Description:
    Set camera orientation information from a rotation matrix.

  Remarks:
    There are four ways to specify the camera's orientation.
    1) Use SetCameraOrientationAnglesRadians() to set
       camera orientation information from rotation angles
       in radians.
    2) Use SetCameraOrientationAnglesDegrees() to set
       camera orientation information from rotation angles
       in degrees.
    3) Use SetCameraOrientationVectors() to set 
       camera orientation information from vectors
       that report the camera's up, right and direction.
    3) Use SetCameraOrientationRotation() to set 
       camera orientation information from a rotation
       matrix.
    Use the method for which you have the most accurate input
    and the other values will be calculated as accurately as
    possible.
  */
  bool SetOrientationRotation(
    MYON_Xform camera_rotation
    );

  void UnsetOrientation();
  
private:
  unsigned char m_status;
  unsigned char m_reserved1[3];
  unsigned int m_reserved2;

  MYON_UnitSystem m_unit_system;

  MYON_3dPoint m_location;

  MYON_3dVector m_orientation_angles_degrees;
  MYON_3dVector m_orientation_angles_radians;

  MYON_3dVector m_orientation_direction;
  MYON_3dVector m_orientation_up;
  MYON_3dVector m_orientation_right;

  MYON_Xform m_orientation_rotation;
};




class MYON_CLASS MYON_AerialPhotoImage
{
public:
  MYON_AerialPhotoImage();

  void Unset();

  bool NameIsSet() const;

  void SetName(
    const wchar_t* name
    );

  void GetName(
    MYON_wString& name
    ) const;

  void UnsetName();

  void SetId( MYON_UUID image_id );

  MYON_UUID Id() const;

  /////////////////////////////////////////////////////////
  //
  // Camera position interface
  //
  bool CameraPositionIsSet() const;
  bool CameraLocationIsSet() const;
  bool CameraOrientationIsSet() const;

  void SetCameraPosition(
    MYON_AerialPhotoCameraPosition camera_position
    );

  void GetCameraPosition(
    MYON_AerialPhotoCameraPosition& camera_position
    ) const;

  void UnsetCameraPosition();
  
  /////////////////////////////////////////////////////////
  //
  // Image frustum interface
  //
  bool ImageFrustumIsSet() const;

  void SetImageFrustum(
    MYON_AerialPhotoImageFrustum image_frustum
    );

  void GetImageFrustum(
    MYON_AerialPhotoImageFrustum& image_frustum
    ) const;

  void UnsetImageFrustum();

  /////////////////////////////////////////////////////////
  //
  // Image frustum interface
  //
  bool ImageFileNameIsSet() const;

  void SetImageFileName(
    const wchar_t* image_file_name
    );

  void GetImageFileName(
    MYON_wString& image_file_name
    ) const;

  void UnsetImageFileName();

  bool ImageSizeIsSet() const;

  bool SetImageSize(
    int width_pixels,
    int height_pixels
    );

  bool GetImageSize(
    int* width_pixels,
    int* height_pixels
    ) const;

  void UnsetImageSize();

  /////////////////////////////////////////////////////////
  //
  // General tools
  //
  bool GetViewProjection( 
    MYON_BoundingBox target_bbox,
    MYON_Viewport& viewport
    ) const;
  
private:
  MYON_wString m_name;

  MYON_UUID m_id;

  MYON_AerialPhotoCameraPosition m_camera_position;

  MYON_AerialPhotoImageFrustum m_image_frustum;

  MYON_wString m_image_file_name;
  
  int m_image_width_pixels;
  int m_image_height_pixels;
};

#endif

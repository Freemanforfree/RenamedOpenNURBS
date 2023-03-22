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

////////////////////////////////////////////////////////////////
//
//   defines float precision point, vector, and array classes
//
////////////////////////////////////////////////////////////////
#if !defined(MYON_FPOINT_INC_)
#define MYON_FPOINT_INC_

class MYON_Xform;

class MYON_2fPoint;
class MYON_3fPoint;
class MYON_4fPoint;

class MYON_2fVector;
class MYON_3fVector;

////////////////////////////////////////////////////////////////
//
//   MYON_2fPoint
//
class MYON_CLASS MYON_2fPoint
{
public:
  float x, y;

public:
  // x,y not initialized
  MYON_2fPoint() = default;
  ~MYON_2fPoint() = default;
  MYON_2fPoint(const MYON_2fPoint&) = default;
  MYON_2fPoint& operator=(const MYON_2fPoint&) = default;

public:
  static const MYON_2fPoint Origin; // (0.0f,0.0f)
  static const MYON_2fPoint NanPoint; // (MYON_FLT_QNAN,MYON_FLT_QNAN)

public:
  explicit MYON_2fPoint(float x,float y);

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_2fPoint& lhs,
    const MYON_2fPoint& rhs
    );

  /*
  Returns:
    (A+B)/2
  Remarks:
    Exact when coordinates are equal and prevents overflow.
  */
  static const MYON_2fPoint Midpoint(const MYON_2fPoint& A, const MYON_2fPoint& B);

  explicit MYON_2fPoint(const MYON_3fPoint& );     // from 3f point
  explicit MYON_2fPoint(const MYON_4fPoint& );     // from 4f point
  explicit MYON_2fPoint(const MYON_2fVector& );    // from 2f vector
  explicit MYON_2fPoint(const MYON_3fVector& );    // from 3f vector
  explicit MYON_2fPoint(const float*);           // from float[2] array

  explicit MYON_2fPoint(const MYON_2dPoint& );     // from 2d point
  explicit MYON_2fPoint(const MYON_3dPoint& );     // from 3d point
  explicit MYON_2fPoint(const MYON_4dPoint& );     // from 4d point
  explicit MYON_2fPoint(const MYON_2dVector& );    // from 2d vector
  explicit MYON_2fPoint(const MYON_3dVector& );    // from 3d vector
  explicit MYON_2fPoint(const double*);          // from double[2] array

  // (float*) conversion operators
  operator float*();
  operator const float*() const;

  // use implicit operator=(const MYON_2fPoint&)
  MYON_2fPoint& operator=(const MYON_3fPoint&);
  MYON_2fPoint& operator=(const MYON_4fPoint&);
  MYON_2fPoint& operator=(const MYON_2fVector&);
  MYON_2fPoint& operator=(const MYON_3fVector&);
  MYON_2fPoint& operator=(const float*);  // point = float[2] support

  MYON_2fPoint& operator=(const MYON_2dPoint&);
  MYON_2fPoint& operator=(const MYON_3dPoint&);
  MYON_2fPoint& operator=(const MYON_4dPoint&);
  MYON_2fPoint& operator=(const MYON_2dVector&);
  MYON_2fPoint& operator=(const MYON_3dVector&);
  MYON_2fPoint& operator=(const double*); // point = double[2] support

  MYON_2fPoint& operator*=(float);
  MYON_2fPoint& operator/=(float);
  MYON_2fPoint& operator+=(const MYON_2fVector&);
  MYON_2fPoint& operator-=(const MYON_2fVector&);

  MYON_2fPoint  operator*(int) const;
  MYON_2fPoint  operator/(int) const;
  MYON_2fPoint  operator*(float) const;
  MYON_2fPoint  operator/(float) const;
  MYON_2dPoint  operator*(double) const;
  MYON_2dPoint  operator/(double) const;

  MYON_2fPoint  operator+(const MYON_2fPoint&) const;
  MYON_2fPoint  operator+(const MYON_2fVector&) const;
  MYON_2fVector operator-(const MYON_2fPoint&) const;
  MYON_2fPoint  operator-(const MYON_2fVector&) const;
  MYON_3fPoint  operator+(const MYON_3fPoint&) const;
  MYON_3fPoint  operator+(const MYON_3fVector&) const;
  MYON_3fVector operator-(const MYON_3fPoint&) const;
  MYON_3fPoint  operator-(const MYON_3fVector&) const;

  MYON_2dPoint  operator+(const MYON_2dPoint&) const;
  MYON_2dPoint  operator+(const MYON_2dVector&) const;
  MYON_2dVector operator-(const MYON_2dPoint&) const;
  MYON_2dPoint  operator-(const MYON_2dVector&) const;
  MYON_3dPoint  operator+(const MYON_3dPoint&) const;
  MYON_3dPoint  operator+(const MYON_3dVector&) const;
  MYON_3dVector operator-(const MYON_3dPoint&) const;
  MYON_3dPoint  operator-(const MYON_3dVector&) const;

  float operator*(const MYON_2fPoint&) const; // for points acting as vectors
  float operator*(const MYON_2fVector&) const; // for points acting as vectors

  bool operator==(const MYON_2fPoint&) const;
  bool operator!=(const MYON_2fPoint&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_2fPoint&) const;
  bool operator>=(const MYON_2fPoint&) const;
  bool operator<(const MYON_2fPoint&) const;
  bool operator>(const MYON_2fPoint&) const;

  // index operators mimic float[2] behavior
  float& operator[](int);
  float operator[](int) const;
  float& operator[](unsigned int);
  float operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is MYON_UNSET_FLOAT, MYON_UNSET_POSITIVE_FLOAT, nan, or infinite.
    True, otherwise.
  */
  bool IsValid() const;
  
  /*
  Returns:
    True if any coordinate is MYON_UNSET_FLOAT or MYON_UNSET_POSITIVE_FLOAT
  */
  bool IsUnset() const;

  // set 2d point value
  void Set(float,float);

  double DistanceTo( const MYON_2fPoint& ) const;
  double DistanceToSquared(const MYON_2fPoint&) const;

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  MYON_DEPRECATED_MSG("Use p = MYON_2fPoint::Origin;")
  void Zero(); // set all coordinates to zero;

  /*
  Returns:
    true if all coordinates are not zero and no coordinates are nans.
    false otherwise.
  */
  bool IsZero() const;

  /*
  Returns:
    true if at lease one coordinate is not zero and no coordinates are unset or nans.
  */
  bool IsNotZero() const;

  // These transform the point in place. The transformation matrix acts on
  // the left of the point; i.e., result = transformation*point
  void Transform( 
        const MYON_Xform&
        );

  void Rotate( // rotatation in XY plane
        double,              // angle in radians
        const MYON_2fPoint&   // center of rotation
        );

  void Rotate( // rotatation in XY plane
        double,              // sin(angle)
        double,              // cos(angle)
        const MYON_2fPoint&   // center of rotation
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_2fPoint operator*(int, const MYON_2fPoint&);

MYON_DECL
MYON_2fPoint operator*(float, const MYON_2fPoint&);

MYON_DECL
MYON_2dPoint operator*(double, const MYON_2fPoint&);

////////////////////////////////////////////////////////////////
//
//   MYON_3fPoint
//
class MYON_CLASS MYON_3fPoint
{
public:
  float x, y, z;

public:
  // x,y,z not initialized
  MYON_3fPoint() = default;
  ~MYON_3fPoint() = default;
  MYON_3fPoint(const MYON_3fPoint&) = default;
  MYON_3fPoint& operator=(const MYON_3fPoint&) = default;

public:
  static const MYON_3fPoint Origin; // (0.0f,0.0f,0.0f)
  static const MYON_3fPoint NanPoint; // (MYON_FLT_QNAN,MYON_FLT_QNAN,MYON_FLT_QNAN)

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_3fPoint& lhs,
    const MYON_3fPoint& rhs
    );

  /*
  Returns:
    (A+B)/2
  Remarks:
    Exact when coordinates are equal and prevents overflow.
  */
  static const MYON_3fPoint Midpoint(const MYON_3fPoint& A, const MYON_3fPoint& B);

  explicit MYON_3fPoint(float x,float y,float z);
  explicit MYON_3fPoint(const MYON_2fPoint& );     // from 2f point
  explicit MYON_3fPoint(const MYON_4fPoint& );     // from 4f point
  explicit MYON_3fPoint(const MYON_2fVector& );    // from 2f vector
  explicit MYON_3fPoint(const MYON_3fVector& );    // from 3f vector
  explicit MYON_3fPoint(const float*);           // from float[3] array

  explicit MYON_3fPoint(const MYON_2dPoint& );     // from 2d point
  explicit MYON_3fPoint(const MYON_3dPoint& );     // from 3d point
  explicit MYON_3fPoint(const MYON_4dPoint& );     // from 4d point
  explicit MYON_3fPoint(const MYON_2dVector& );    // from 2d vector
  explicit MYON_3fPoint(const MYON_3dVector& );    // from 3d vector
  explicit MYON_3fPoint(const double*);          // from double[3] array

  // (float*) conversion operators
  operator float*();
  operator const float*() const;

  // use implicit operator=(const MYON_3fPoint&)
  MYON_3fPoint& operator=(const MYON_2fPoint&);
  MYON_3fPoint& operator=(const MYON_4fPoint&);
  MYON_3fPoint& operator=(const MYON_2fVector&);
  MYON_3fPoint& operator=(const MYON_3fVector&);
  MYON_3fPoint& operator=(const float*);  // point = float[3] support

  MYON_3fPoint& operator=(const MYON_2dPoint&);
  MYON_3fPoint& operator=(const MYON_3dPoint&);
  MYON_3fPoint& operator=(const MYON_4dPoint&);
  MYON_3fPoint& operator=(const MYON_2dVector&);
  MYON_3fPoint& operator=(const MYON_3dVector&);
  MYON_3fPoint& operator=(const double*); // point = double[3] support

  MYON_3fPoint& operator*=(float);
  MYON_3fPoint& operator/=(float);
  MYON_3fPoint& operator+=(const MYON_3fVector&);
  MYON_3fPoint& operator-=(const MYON_3fVector&);

  MYON_3fPoint  operator*(int) const;
  MYON_3fPoint  operator/(int) const;
  MYON_3fPoint  operator*(float) const;
  MYON_3fPoint  operator/(float) const;
  MYON_3dPoint  operator*(double) const;
  MYON_3dPoint  operator/(double) const;

  MYON_3fPoint  operator+(const MYON_3fPoint&) const;
  MYON_3fPoint  operator+(const MYON_3fVector&) const;
  MYON_3fVector operator-(const MYON_3fPoint&) const;
  MYON_3fPoint  operator-(const MYON_3fVector&) const;
  MYON_3fPoint  operator+(const MYON_2fPoint&) const;
  MYON_3fPoint  operator+(const MYON_2fVector&) const;
  MYON_3fVector operator-(const MYON_2fPoint&) const;
  MYON_3fPoint  operator-(const MYON_2fVector&) const;

  MYON_3dPoint  operator+(const MYON_3dPoint&) const;
  MYON_3dPoint  operator+(const MYON_3dVector&) const;
  MYON_3dVector operator-(const MYON_3dPoint&) const;
  MYON_3dPoint  operator-(const MYON_3dVector&) const;
  MYON_3dPoint  operator+(const MYON_2dPoint&) const;
  MYON_3dPoint  operator+(const MYON_2dVector&) const;
  MYON_3dVector operator-(const MYON_2dPoint&) const;
  MYON_3dPoint  operator-(const MYON_2dVector&) const;

  float operator*(const MYON_3fPoint&) const; // for points acting as vectors
  float operator*(const MYON_3fVector&) const; // for points acting as vectors

  bool operator==(const MYON_3fPoint&) const;
  bool operator!=(const MYON_3fPoint&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_3fPoint&) const;
  bool operator>=(const MYON_3fPoint&) const;
  bool operator<(const MYON_3fPoint&) const;
  bool operator>(const MYON_3fPoint&) const;

  // index operators mimic float[3] behavior
  float& operator[](int);
  float operator[](int) const;
  float& operator[](unsigned int);
  float operator[](unsigned int) const;

    /*
  Returns:
    False if any coordinate is MYON_UNSET_FLOAT, MYON_UNSET_POSITIVE_FLOAT, nan, or infinite.
    True, otherwise.
  */
  bool IsValid() const;
  
  /*
  Returns:
    True if any coordinate is MYON_UNSET_FLOAT or MYON_UNSET_POSITIVE_FLOAT
  */
  bool IsUnset() const;

  // set 3d point value
  void Set(float,float,float);

  double DistanceTo( const MYON_3fPoint& ) const;
  double DistanceToSquared(const MYON_3fPoint&) const;

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate
  double Fuzz( double = MYON_ZERO_TOLERANCE ) const; // tolerance to use when comparing 3d points

  MYON_DEPRECATED_MSG("Use p = MYON_3fPoint::Origin;")
  void Zero(); // set all coordinates to zero;

  /*
  Returns:
    true if all coordinates are not zero and no coordinates are nans.
    false otherwise.
  */
  bool IsZero() const;

  /*
  Returns:
    true if at lease one coordinate is not zero and no coordinates are unset or nans.
  */
  bool IsNotZero() const;

  // These transform the point in place. The transformation matrix acts on
  // the left of the point; i.e., result = transformation*point
  void Transform( 
        const MYON_Xform&
        );

  void Rotate( 
        double,               // angle in radians
        const MYON_3fVector&, // axis of rotation
        const MYON_3fPoint&   // center of rotation
        );

  void Rotate( 
        double,               // sin(angle)
        double,               // cos(angle)
        const MYON_3fVector&, // axis of rotation
        const MYON_3fPoint&   // center of rotation
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_3fPoint operator*(int, const MYON_3fPoint&);

MYON_DECL
MYON_3fPoint operator*(float, const MYON_3fPoint&);

MYON_DECL
MYON_3dPoint operator*(double, const MYON_3fPoint&);

////////////////////////////////////////////////////////////////
//
//   MYON_4fPoint (homogeneous coordinates)
//
class MYON_CLASS MYON_4fPoint
{
public:
  float x, y, z, w;

  /*
  Returns:
    MYON_UNSET_VALUE, if x or w is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
    and neither x nor w is a nan.
    x/w, otherwise
  Remarks:
    If w is 0.0 or nan, the result will be a nan.
  */
  float EuclideanX() const;

  /*
  Returns:
    MYON_UNSET_VALUE, if y or w is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
    and neither y nor w is a nan.
    y/w, otherwise
  Remarks:
    If w is 0.0 or nan, the result will be a nan.
  */
  float EuclideanY() const;

  /*
  Returns:
    MYON_UNSET_VALUE, if z or w is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
    and neither z nor w is a nan.
    z/w, otherwise
  Remarks:
    If w is 0.0 or nan, the result will be a nan.
  */
  float EuclideanZ() const;

public:
  // x,y,z,w not initialized
  MYON_4fPoint() = default;
  ~MYON_4fPoint() = default;
  MYON_4fPoint(const MYON_4fPoint&) = default;
  MYON_4fPoint& operator=(const MYON_4fPoint&) = default;

public:
  static const MYON_4fPoint Zero; // (0,0,0,0)
  static const MYON_4fPoint Nan; // (MYON_FLT_QNAN,MYON_FLT_QNAN,MYON_FLT_QNAN,MYON_FLT_QNAN)

  /*
  Description:
    A well ordered projective compare function that is nan aware and can
    be used for robust sorting.
  Remarks:
    float c = non-nan value.
    MYON_4fPoint h0 = ...;
    MYON_4fPoint h1(c*h0.x,c*h0.x,c*h0.x,c*h0.x);
    0 == MYON_4fPoint::ProjectiveCompare(h0,ha);
  */
  static int ProjectiveCompare(
    const MYON_4fPoint& lhs,
    const MYON_4fPoint& rhs
    );

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int DictionaryCompare(
    const MYON_4fPoint& lhs,
    const MYON_4fPoint& rhs
    );

  /*
  Returns:
    True if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w).
  */
  bool operator==(const MYON_4fPoint& rhs) const;

  /*
  Returns:
    True if lhs.* != rhs.* for some coordinate and no values are nans.
  */
  bool operator!=(const MYON_4fPoint& rhs) const;

  explicit MYON_4fPoint(float x,float y,float z,float w);

  MYON_4fPoint(const MYON_2fPoint& );     // from 2f point
  MYON_4fPoint(const MYON_3fPoint& );     // from 3f point
  MYON_4fPoint(const MYON_2fVector& );    // from 2f vector
  MYON_4fPoint(const MYON_3fVector& );    // from 3f vector

  // Require explicit construction when dev must insure array has length >= 4.
  explicit MYON_4fPoint(const float*);           // from float[4] array

  // Require explicit construction when losing precision
  explicit MYON_4fPoint(const MYON_2dPoint& );     // from 2d point
  explicit MYON_4fPoint(const MYON_3dPoint& );     // from 3d point
  explicit MYON_4fPoint(const MYON_4dPoint& );     // from 4d point
  explicit MYON_4fPoint(const MYON_2dVector& );    // from 2d vector
  explicit MYON_4fPoint(const MYON_3dVector& );    // from 3d vector
  explicit MYON_4fPoint(const double*);          // from double[4] array

  // (float*) conversion operators
  operator float*();
  operator const float*() const;

  // use implicit operator=(const MYON_4fPoint&)
  MYON_4fPoint& operator=(const MYON_2fPoint&);
  MYON_4fPoint& operator=(const MYON_3fPoint&);
  MYON_4fPoint& operator=(const MYON_2fVector&);
  MYON_4fPoint& operator=(const MYON_3fVector&);
  MYON_4fPoint& operator=(const float*);  // point = float[4] support

  MYON_4fPoint& operator=(const MYON_2dPoint&);
  MYON_4fPoint& operator=(const MYON_3dPoint&);
  MYON_4fPoint& operator=(const MYON_4dPoint&);
  MYON_4fPoint& operator=(const MYON_2dVector&);
  MYON_4fPoint& operator=(const MYON_3dVector&);
  MYON_4fPoint& operator=(const double*); // point = double[4] support

  MYON_4fPoint& operator*=(float);
  MYON_4fPoint& operator/=(float);
  MYON_4fPoint& operator+=(const MYON_4fPoint&);
  MYON_4fPoint& operator-=(const MYON_4fPoint&);

  MYON_4fPoint  operator*(float) const;
  MYON_4fPoint  operator/(float) const;
  MYON_4fPoint  operator+(const MYON_4fPoint&) const; // sum w = sqrt(w1*w2)
  MYON_4fPoint  operator-(const MYON_4fPoint&) const; // difference w = sqrt(w1*w2)

public:
  // index operators mimic float[4] behavior
  float& operator[](int);
  float operator[](int) const;
  float& operator[](unsigned int);
  float operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is MYON_UNSET_FLOAT, MYON_UNSET_POSITIVE_FLOAT, nan, or infinite.
    True, otherwise.
  */
  bool IsValid() const;
  
  /*
  Returns:
    True if any coordinate is MYON_UNSET_FLOAT or MYON_UNSET_POSITIVE_FLOAT
  */
  bool IsUnset() const;

  // set 4d point value
  void Set(float,float,float,float);

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  bool Normalize(); // set so x^2 + y^2 + z^2 + w^2 = 1

  // These transform the point in place. The transformation matrix acts on
  // the left of the point; i.e., result = transformation*point
  void Transform( 
        const MYON_Xform&
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_4fPoint operator*(float, const MYON_4fPoint&);

MYON_DECL
MYON_4dPoint operator*(double, const MYON_4fPoint&);

////////////////////////////////////////////////////////////////
//
//   MYON_2fVector
//
class MYON_CLASS MYON_2fVector
{
public:
  float x, y;

public:
  // x,y not initialized
  MYON_2fVector() = default;
  ~MYON_2fVector() = default;
  MYON_2fVector(const MYON_2fVector&) = default;
  MYON_2fVector& operator=(const MYON_2fVector&) = default;

public:
  static const MYON_2fVector NanVector; // (MYON_FLT_QNAN,MYON_FLT_QNAN)
  static const MYON_2fVector ZeroVector; // (0.0f,0.0f)
  static const MYON_2fVector XAxis;      // (1.0f,0.0f)
  static const MYON_2fVector YAxis;      // (0.0f,1.0f)

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_2fVector& lhs,
    const MYON_2fVector& rhs
    );

  // Description:
  //   A index driven function to get unit axis vectors.
  // Parameters:
  //   index - [in] 0 returns (1,0), 1 returns (0,1)
  // Returns:
  //   Unit 3d vector with vector[i] = (i==index)?1:0;
  static const MYON_2fVector& UnitVector(
    int // index
    );

  explicit MYON_2fVector(float x,float y);
  explicit MYON_2fVector(const MYON_2fPoint& );     // from 2f point
  explicit MYON_2fVector(const MYON_3fPoint& );     // from 3f point
  explicit MYON_2fVector(const MYON_3fVector& );    // from 3f vector
  explicit MYON_2fVector(const float*);           // from float[2] array

  explicit MYON_2fVector(const MYON_2dPoint& );     // from 2d point
  explicit MYON_2fVector(const MYON_3dPoint& );     // from 3d point
  explicit MYON_2fVector(const MYON_2dVector& );    // from 2d vector
  explicit MYON_2fVector(const MYON_3dVector& );    // from 3d vector
  explicit MYON_2fVector(const double*);          // from double[2] array

  // (float*) conversion operators
  operator float*();
  operator const float*() const;

  // use implicit operator=(const MYON_2fVector&)
  MYON_2fVector& operator=(const MYON_2fPoint&);
  MYON_2fVector& operator=(const MYON_3fPoint&);
  MYON_2fVector& operator=(const MYON_3fVector&);
  MYON_2fVector& operator=(const float*);  // point = float[2] support

  MYON_2fVector& operator=(const MYON_2dPoint&);
  MYON_2fVector& operator=(const MYON_3dPoint&);
  MYON_2fVector& operator=(const MYON_2dVector&);
  MYON_2fVector& operator=(const MYON_3dVector&);
  MYON_2fVector& operator=(const double*); // point = double[2] support

  MYON_2fVector  operator-() const;

  MYON_2fVector& operator*=(float);
  MYON_2fVector& operator/=(float);
  MYON_2fVector& operator+=(const MYON_2fVector&);
  MYON_2fVector& operator-=(const MYON_2fVector&);

  float operator*(const MYON_2fVector&) const; // inner (dot) product
  float operator*(const MYON_2fPoint&) const; // inner (dot) product point acting as a vector
  double operator*(const MYON_2dVector&) const; // inner (dot) product
  
  MYON_2fVector  operator*(int) const;
  MYON_2fVector  operator/(int) const;
  MYON_2fVector  operator*(float) const;
  MYON_2fVector  operator/(float) const;
  MYON_2dVector  operator*(double) const;
  MYON_2dVector  operator/(double) const;

  MYON_2fVector  operator+(const MYON_2fVector&) const;
  MYON_2fPoint   operator+(const MYON_2fPoint&) const;
  MYON_2fVector  operator-(const MYON_2fVector&) const;
  MYON_2fPoint   operator-(const MYON_2fPoint&) const;
  MYON_3fVector  operator+(const MYON_3fVector&) const;
  MYON_3fPoint   operator+(const MYON_3fPoint&) const;
  MYON_3fVector  operator-(const MYON_3fVector&) const;
  MYON_3fPoint   operator-(const MYON_3fPoint&) const;

  MYON_2dVector  operator+(const MYON_2dVector&) const;
  MYON_2dPoint   operator+(const MYON_2dPoint&) const;
  MYON_2dVector  operator-(const MYON_2dVector&) const;
  MYON_2dPoint   operator-(const MYON_2dPoint&) const;
  MYON_3dVector  operator+(const MYON_3dVector&) const;
  MYON_3dPoint   operator+(const MYON_3dPoint&) const;
  MYON_3dVector  operator-(const MYON_3dVector&) const;
  MYON_3dPoint   operator-(const MYON_3dPoint&) const;

  bool operator==(const MYON_2fVector&) const;
  bool operator!=(const MYON_2fVector&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_2fVector&) const;
  bool operator>=(const MYON_2fVector&) const;
  bool operator<(const MYON_2fVector&) const;
  bool operator>(const MYON_2fVector&) const;

  // index operators mimic float[2] behavior
  float& operator[](int);
  float operator[](int) const;
  float& operator[](unsigned int);
  float operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is MYON_UNSET_FLOAT, MYON_UNSET_POSITIVE_FLOAT, nan, or infinite.
    True, otherwise.
  */
  bool IsValid() const;
  
  /*
  Returns:
    True if any coordinate is MYON_UNSET_FLOAT or MYON_UNSET_POSITIVE_FLOAT
  */
  bool IsUnset() const;

  // set 2d vector value
  void Set(float,float);

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  double LengthSquared() const;
  double Length() const;

  bool Decompose( // Computes a, b such that this vector = a*X + b*Y
         // Returns false if unable to solve for a,b.  This happens
         // when X,Y is not really a basis.
         //
         // If X,Y is known to be an orthonormal frame,
         // then a = V*X, b = V*Y will compute
         // the same result more quickly.
         const MYON_2fVector&, // X
         const MYON_2fVector&, // Y
         double*, // a
         double*  // b
         ) const;

  int IsParallelTo( 
        // returns  1: this and other vectors are parallel
        //         -1: this and other vectors are anti-parallel
        //          0: this and other vectors are not parallel
        //             or at least one of the vectors is zero
        const MYON_2fVector&,                 // other vector     
        double = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  bool IsPerpendicularTo(
        // returns true:  this and other vectors are perpendicular
        //         false: this and other vectors are not perpendicular
        //                or at least one of the vectors is zero
        const MYON_2fVector&,                 // other vector     
        double = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  MYON_DEPRECATED_MSG("Use p = MYON_2fVector::ZeroVector;")
  void Zero(); // set all coordinates to zero;

  MYON_DEPRECATED_MSG("Use v = -v;")
  void Reverse(); // negate all coordinates

  bool Unitize();  // returns false if vector has zero length

  bool IsUnitVector() const;

  /*
  Returns:
    If this is a valid non-zero vector, a unit vector parallel to this is returned.
    Otherwise the zero vector is returned.
  */
  MYON_2fVector UnitVector() const;

  // Description:
  //   Test a vector to see if it is very short
  //
  // Parameters:
  //   tiny_tol - [in] (default = MYON_ZERO_TOLERANCE) a nonzero
  //              value used as the coordinate zero tolerance.
  //
  // Returns:
  //   ( fabs(x) <= tiny_tol && fabs(y) <= tiny_tol )
  //
  bool IsTiny(
         double = MYON_ZERO_TOLERANCE // tiny_tol
         ) const;

  // Returns:
  //   true if vector is the zero vector.
  bool IsZero() const;

  /*
  Returns:
    true if at lease one coordinate is not zero and no coordinates are unset or nans.
  */
  bool IsNotZero() const;

  // set this vector to be perpendicular to another vector
  bool PerpendicularTo( // Result is not unitized. 
                        // returns false if input vector is zero
        const MYON_2fVector& 
        );

  // set this vector to be perpendicular to a line defined by 2 points
  bool PerpendicularTo( 
        const MYON_2fPoint&, 
        const MYON_2fPoint& 
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_2fVector operator*(int, const MYON_2fVector&);

MYON_DECL
MYON_2fVector operator*(float, const MYON_2fVector&);

MYON_DECL
MYON_2dVector operator*(double, const MYON_2fVector&);

///////////////////////////////////////////////////////////////
//
// MYON_2fVector utilities
//

MYON_DECL
float 
MYON_DotProduct( 
    const MYON_2fVector&, 
    const MYON_2fVector& 
    );

MYON_DECL
MYON_3fVector 
MYON_CrossProduct(
    const MYON_2fVector&, 
    const MYON_2fVector& 
    );

MYON_DECL
bool 
MYON_IsOrthogonalFrame( // true if X, Y are nonzero and mutually perpendicular
    const MYON_2fVector&, // X
    const MYON_2fVector&  // Y
    );

MYON_DECL
bool 
MYON_IsOrthonormalFrame( // true if X, Y are orthogonal and unit length
    const MYON_2fVector&, // X
    const MYON_2fVector&  // Y
    );

MYON_DECL
bool 
MYON_IsRightHandFrame( // true if X, Y are orthonormal and right handed
    const MYON_2fVector&, // X
    const MYON_2fVector&  // Y
    );

////////////////////////////////////////////////////////////////
//
//   MYON_3fVector
//
class MYON_CLASS MYON_3fVector
{
public:
  float x, y, z;

public:
  // x,y,z not initialized
  MYON_3fVector() = default;
  ~MYON_3fVector() = default;
  MYON_3fVector(const MYON_3fVector&) = default;
  MYON_3fVector& operator=(const MYON_3fVector&) = default;

public:
  static const MYON_3fVector NanVector; // (MYON_FLT_QNAN,MYON_FLT_QNAN,MYON_FLT_QNAN)
  static const MYON_3fVector ZeroVector; // (0.0f,0.0f,0.0f)
  static const MYON_3fVector XAxis;      // (1.0f,0.0f,0.0f)
  static const MYON_3fVector YAxis;      // (0.0f,1.0f,0.0f)
  static const MYON_3fVector ZAxis;      // (0.0f,0.0f,1.0f)

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_3fVector& lhs,
    const MYON_3fVector& rhs
    );

  // Description:
  //   A index driven function to get unit axis vectors.
  // Parameters:
  //   index - [in] 0 returns (1,0,0), 1 returns (0,1,0)
  //                2 returns (0,0,1)
  // Returns:
  //   Unit 3d vector with vector[i] = (i==index)?1:0;
  static const MYON_3fVector& UnitVector(
    int // index
    );

  explicit MYON_3fVector(float x,float y,float z);

  explicit MYON_3fVector(const MYON_2fPoint& );     // from 2f point
  explicit MYON_3fVector(const MYON_3fPoint& );     // from 3f point
  explicit MYON_3fVector(const MYON_2fVector& );    // from 2f vector
  explicit MYON_3fVector(const float*);           // from float[3] array

  explicit MYON_3fVector(const MYON_2dPoint& );     // from 2d point
  explicit MYON_3fVector(const MYON_3dPoint& );     // from 3d point
  explicit MYON_3fVector(const MYON_2dVector& );    // from 2d vector
  explicit MYON_3fVector(const MYON_3dVector& );    // from 3d vector
  explicit MYON_3fVector(const double*);          // from double[3] array

  // (float*) conversion operators
  operator float*();
  operator const float*() const;

  // use implicit operator=(const MYON_3fVector&)
  MYON_3fVector& operator=(const MYON_2fPoint&);
  MYON_3fVector& operator=(const MYON_3fPoint&);
  MYON_3fVector& operator=(const MYON_2fVector&);
  MYON_3fVector& operator=(const float*);  // point = float[3] support

  MYON_3fVector& operator=(const MYON_2dPoint&);
  MYON_3fVector& operator=(const MYON_3dPoint&);
  MYON_3fVector& operator=(const MYON_2dVector&);
  MYON_3fVector& operator=(const MYON_3dVector&);
  MYON_3fVector& operator=(const double*); // point = double[3] support
  
  MYON_3fVector  operator-() const;

  MYON_3fVector& operator*=(float);
  MYON_3fVector& operator/=(float);
  MYON_3fVector& operator+=(const MYON_3fVector&);
  MYON_3fVector& operator-=(const MYON_3fVector&);

  float operator*(const MYON_3fVector&) const; // inner (dot) product
  float operator*(const MYON_3fPoint&) const; // inner (dot) product (point acting as a vector)
  double operator*(const MYON_3dVector&) const; // inner (dot) product

  MYON_3fVector  operator*(int) const;
  MYON_3fVector  operator/(int) const;
  MYON_3fVector  operator*(float) const;
  MYON_3fVector  operator/(float) const;
  MYON_3dVector  operator*(double) const;
  MYON_3dVector  operator/(double) const;

  MYON_3fVector  operator+(const MYON_3fVector&) const;
  MYON_3fPoint   operator+(const MYON_3fPoint&) const;
  MYON_3fVector  operator-(const MYON_3fVector&) const;
  MYON_3fPoint   operator-(const MYON_3fPoint&) const;
  MYON_3fVector  operator+(const MYON_2fVector&) const;
  MYON_3fPoint   operator+(const MYON_2fPoint&) const;
  MYON_3fVector  operator-(const MYON_2fVector&) const;
  MYON_3fPoint   operator-(const MYON_2fPoint&) const;

  MYON_3dVector  operator+(const MYON_3dVector&) const;
  MYON_3dPoint   operator+(const MYON_3dPoint&) const;
  MYON_3dVector  operator-(const MYON_3dVector&) const;
  MYON_3dPoint   operator-(const MYON_3dPoint&) const;
  MYON_3dVector  operator+(const MYON_2dVector&) const;
  MYON_3dPoint   operator+(const MYON_2dPoint&) const;
  MYON_3dVector  operator-(const MYON_2dVector&) const;
  MYON_3dPoint   operator-(const MYON_2dPoint&) const;

  bool operator==(const MYON_3fVector&) const;
  bool operator!=(const MYON_3fVector&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_3fVector&) const;
  bool operator>=(const MYON_3fVector&) const;
  bool operator<(const MYON_3fVector&) const;
  bool operator>(const MYON_3fVector&) const;

  // index operators mimic float[3] behavior
  float& operator[](int);
  float operator[](int) const;
  float& operator[](unsigned int);
  float operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is MYON_UNSET_FLOAT, MYON_UNSET_POSITIVE_FLOAT, nan, or infinite.
    True, otherwise.
  */
  bool IsValid() const;
  
  /*
  Returns:
    True if any coordinate is MYON_UNSET_FLOAT or MYON_UNSET_POSITIVE_FLOAT
  */
  bool IsUnset() const;

  // set 3d vector value
  void Set(float,float,float);

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  double LengthSquared() const;
  double Length() const;

  bool IsPerpendicularTo(
        // returns true:  this and other vectors are perpendicular
        //         false: this and other vectors are not perpendicular
        //                or at least one of the vectors is zero
        const MYON_3fVector&,                 // other vector     
        double = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  double Fuzz( double = MYON_ZERO_TOLERANCE ) const; // tolerance to use when comparing 3d vectors

  MYON_DEPRECATED_MSG("Use p = MYON_3fVector::ZeroVector;")
  void Zero(); // set all coordinates to zero

  MYON_DEPRECATED_MSG("Use v = -v;")
  void Reverse(); // negate all coordinates

  bool Unitize();  // returns false if vector has zero length

  bool IsUnitVector() const;

  /*
  Returns:
    If this is a valid non-zero vector, a unit vector parallel to this is returned.
    Otherwise the zero vector is returned.
  */
  MYON_3fVector UnitVector() const;


  // Description:
  //   Test a vector to see if it is very short
  //
  // Parameters:
  //   tiny_tol - [in] (default = MYON_ZERO_TOLERANCE) a nonzero
  //              value used as the coordinate zero tolerance.
  //
  // Returns:
  //   ( fabs(x) <= tiny_tol && fabs(y) <= tiny_tol && fabs(z) <= tiny_tol )
  //
  bool IsTiny(
         double = MYON_ZERO_TOLERANCE // tiny_tol
         ) const;

  // Returns:
  //   true if vector is the zero vector.
  bool IsZero() const;

  /*
  Returns:
    true if at lease one coordinate is not zero and no coordinates are unset or nans.
  */
  bool IsNotZero() const;

  // set this vector to be perpendicular to another vector
  bool PerpendicularTo( // Result is not unitized. 
                        // returns false if input vector is zero
        const MYON_3fVector& 
        );

  // These transform the vector in place. The transformation matrix acts on
  // the left of the vector; i.e., result = transformation*vector
  void Transform( 
        const MYON_Xform& // can use MYON_Xform here
        );

  void Rotate( 
        double,             // angle in radians
        const MYON_3fVector&  // axis of rotation
        );

  void Rotate( 
        double,             // sin(angle)
        double,             // cos(angle)
        const MYON_3fVector&  // axis of rotation
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_3fVector operator*(int, const MYON_3fVector&);

MYON_DECL
MYON_3fVector operator*(float, const MYON_3fVector&);

MYON_DECL
MYON_3dVector operator*(double, const MYON_3fVector&);

///////////////////////////////////////////////////////////////
//
// MYON_3fVector utilities
//

MYON_DECL
float 
MYON_DotProduct( 
    const MYON_3fVector&, 
    const MYON_3fVector& 
    );


MYON_DECL
MYON_3fVector 
MYON_CrossProduct(
    const MYON_3fVector&, 
    const MYON_3fVector& 
    );

MYON_DECL
MYON_3fVector 
MYON_CrossProduct( // 3d cross product for old fashioned arrays
    const float*, // array of 3d floats
    const float*  // array of 3d floats
    );

MYON_DECL
float 
MYON_TripleProduct( 
    const MYON_3fVector&,
    const MYON_3fVector&,
    const MYON_3fVector&
    );

MYON_DECL
float 
MYON_TripleProduct(  // 3d triple product for old fashioned arrays
    const float*, // array of 3d floats
    const float*, // array of 3d floats
    const float*  // array of 3d floats
    );

MYON_DECL
bool 
MYON_IsOrthogonalFrame( // true if X, Y, Z are nonzero and mutually perpendicular
    const MYON_3fVector&, // X
    const MYON_3fVector&, // Y
    const MYON_3fVector&  // Z 
    );

MYON_DECL
bool 
MYON_IsOrthonormalFrame( // true if X, Y, Z are orthogonal and unit length
    const MYON_3fVector&, // X
    const MYON_3fVector&, // Y
    const MYON_3fVector&  // Z 
    );

MYON_DECL
bool 
MYON_IsRightHandFrame( // true if X, Y, Z are orthonormal and right handed
    const MYON_3fVector&, // X
    const MYON_3fVector&, // Y
    const MYON_3fVector&  // Z 
    );

#endif

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
//   defines double precision point, vector, and array classes
//
////////////////////////////////////////////////////////////////
#if !defined(MYON_POINT_INC_)
#define MYON_POINT_INC_

class MYON_BoundingBox;
class MYON_Xform;
class MYON_Line;
class MYON_Plane;

class MYON_2dPoint;
class MYON_3dPoint;
class MYON_4dPoint;

class MYON_2dVector;
class MYON_3dVector;

class MYON_2fVector;
class MYON_3fVector;

class MYON_Interval;

////////////////////////////////////////////////////////////////
//
//   MYON_Interval
//
class MYON_CLASS MYON_Interval
{
public:
  static const MYON_Interval EmptyInterval; // (MYON_UNSET_VALUE,MYON_UNSET_VALUE)
  static const MYON_Interval ZeroToOne; // (0.0, 1.0)
  static const MYON_Interval ZeroToTwoPi; // (0.0, 2.0*MYON_PI)
  static const MYON_Interval Nan; // (MYON_DBL_QNAN,MYON_DBL_QNAN)
  
public:
  // The default constructor creates MYON_Interval::EmptyInterval (MYON_UNSET_VALUE,MYON_UNSET_VALUE)
  MYON_Interval();
  ~MYON_Interval() = default;
  MYON_Interval(const MYON_Interval&) = default;
  MYON_Interval& operator=(const MYON_Interval&) = default;

  MYON_Interval(double t0,double t1);

  /*
  Returns:
    MYON_Interval(t,t).
  */
  static const MYON_Interval Singleton(double t);

public:
  // Interval = (m_t[0], m_t[1])
  double m_t[2];

#if defined(OPENNURBS_WALL)
  // Goal is to eventually require = as the only way to modify MYON_Interval values.
  MYON_DEPRECATED_MSG("Use interval = MYON_Interval::EmptyInterval;")
#endif
  void Destroy();

  /*
  Description:
    Sets interval to [t0,t1]
  Parameters:
    t0 - [in]
    t1 - [in]
  See Also:
    MYON_Interval::MYON_Interval( double, double )
  */
  void Set(
    double t0, 
    double t1
    );

  /*
  Description:
    Convert normalized parameter to interval value, or pair of values.
  Parameters:
    normalized_parameter - [in] 
  Returns:
    Interval parameter
    min*(1.0-normalized_parameter) + max*normalized_parameter
  See Also:
    MYON_Interval::NormalizedParameterAt
  */
  double ParameterAt (
    double normalized_parameter
    ) const; 
  MYON_Interval ParameterAt (
    MYON_Interval normalized_interval
    ) const; 
  
  /*
  Description:
    Convert interval value, or pair of values, to normalized parameter.
  Parameters:
    interval_parameter - [in] value in interval
  Returns:
    Normalized parameter x so that 
    min*(1.0-x) + max*x = interval_parameter.
  See Also:
    MYON_Interval::ParameterAt
  */
  double NormalizedParameterAt (
    double interval_parameter
    ) const;
  MYON_Interval NormalizedParameterAt (
    MYON_Interval interval_parameter
    ) const;

  double& operator[](int); // returns (index<=0) ? m_t[0] : m_t[1]
  double operator[](int) const; // returns (index<=0) ? m_t[0] : m_t[1]
  double& operator[](unsigned int); // returns (index<=0) ? m_t[0] : m_t[1]
  double operator[](unsigned int) const; // returns (index<=0) ? m_t[0] : m_t[1]

  double Min() const; // returns smaller of m_t[0] and m_t[1]
  double Max() const; // returns larger of m_t[0] and m_t[1]
  double Mid() const; // returns 0.5*(m_t[0] + m_t[1])
  double Length() const;  // returns signed length, m_t[1]-m_t[0]

  bool IsIncreasing() const; // returns true if m_t[0] < m_t[1]
  bool IsDecreasing() const; // returns true if m_t[0] > m_t[0];
  bool IsInterval() const;   // returns truc if m_t[0] != m_t[1]
  bool IsSingleton() const;  // returns true if m_t[0] == m_t[1] != MYON_UNSET_VALUE
  bool IsEmptyInterval() const;   // returns true if m_t[0] == m_t[1] == MYON_UNSET_VALUE
  bool IsValid() const;      // returns MYON_IsValid(m_t[0]) && MYON_IsValid(m_t[1])

#if defined(OPENNURBS_WALL)
  MYON_DEPRECATED_MSG("Use IsEmptyInterval()")
#endif
  bool IsEmptySet() const;   // returns true if m_t[0] == m_t[1] == MYON_UNSET_VALUE

	bool MakeIncreasing();		// returns true if resulting interval IsIncreasing() 

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_Interval& lhs, 
    const MYON_Interval& rhs
  );


  /*
  Returns:
    True if (lhs.m_t[0] != rhs.m_t[0]) or (lhs.m_t[1] != rhs.m_t[1]) and
    no values are nans.
  */
  bool operator!=(const MYON_Interval& rhs) const;

  /*
  Returns:
    True if (lhs.m_t[0] == rhs.m_t[0]) and (lhs.m_t[1] === rhs.m_t[1]).
  */
  bool operator==(const MYON_Interval& rhs) const;

  /* 
  Description:
    Test a value t to see if it is inside the interval.
  Parameters:
    t - [in] value to test
    bTestOpenInterval - [in] 
        If false, t is tested to see if it satisfies min <= t <= max.
        If true, t is tested to see if it satisfies min < t < max.
  Returns:
    true if t is in the interval and false if t is not
    in the interval.
  */
  bool Includes(
    double t,
    bool bTestOpenInterval = false
    ) const;

  /* 
  Description:
    Test an interval to see if it is contained in this interval.
  Parameters:
    other - [in] interval to test
    bProperSubSet - [in] if true, then the test is for a proper subinterval.
  Returns:
    If bProperSubSet is false, then the result is true when
    this->Min() <= other.Min() and other.Max() <= this->Max().
    If bProperSubSet is true, then the result is true when
    this->Min() <= other.Min() and other.Max() <= this->Max()
    and at least one of the inequalites is strict.
  */
  bool Includes( 
    const MYON_Interval& other,
    bool bProperSubSet = false
    ) const;

  /*
  Description:
    value is clamped to the range of this interval.  
    Assumes this interval is not Decreasing().
  Parameters:
    value -[in/out]  value is set to x in interval 
        such that |x - value|<= |x - y| for all y in this interval
  Returns:
    -1 if value<Min()
    0  if value in interval
    +1 if value>Max()
  */
  int Clamp(double& value) const;

  /* 
  Description:
    Test a pair of interval to see if they have a non-empty intersection.
  Parameters:
    A - [in] interval to test
    B - [in] interval to test
  Returns:
    true if the intersection is non-empty, 
    including if the intersection is a single point.
    false otherwise. 
  */
  static bool IntervalsOverlap(const MYON_Interval& A, const MYON_Interval& B);

  /*
  Description:
    Changes interval to [-m_t[1],-m_t[0]].
  */
  void Reverse();

  /*
  Description:
    Swaps m_t[0] and m_t[1].
  */
  void Swap();

  //////////
  // If the intersection is not empty, then 
  // intersection = [max(this.Min(),arg.Min()), min(this.Max(),arg.Max())]
  // Intersection() returns true if the intersection is not empty.
  // The interval [MYON_UNSET_VALUE,MYON_UNSET_VALUE] is considered to be
  // the empty set interval.  The result of any intersection involving an
  // empty set interval or disjoint intervals is the empty set interval.
  bool Intersection( // this = this intersect arg
         const MYON_Interval&
         );

  //////////
  // If the intersection is not empty, then 
  // intersection = [max(argA.Min(),argB.Min()), min(argA.Max(),argB.Max())]
  // Intersection() returns true if the intersection is not empty.
  // The interval [MYON_UNSET_VALUE,MYON_UNSET_VALUE] is considered to be
  // the empty set interval.  The result of any intersection involving an
  // empty set interval or disjoint intervals is the empty set interval.
  bool Intersection( // this = intersection of two args
         const MYON_Interval&, 
         const MYON_Interval&
         );

  //////////
  // The union of an empty set and an increasing interval is the increasing
  // interval.  The union of two empty sets is empty. The union of an empty
  // set an a non-empty interval is the non-empty interval.
  // The union of two non-empty intervals is
  // union = [min(this.Min(),arg.Min()), max(this.Max(),arg.Max()),]
  // Union() returns true if the union is not empty.
  bool Union( // this = this union arg
         const MYON_Interval&
         );

  bool Union( // this = this union arg
         double t
         );

  bool Union( // this = this union arg
         int count,
         const double* t
         );

  //////////
  // The union of an empty set and an increasing interval is the increasing
  // interval.  The union of two empty sets is empty. The union of an empty
  // set an a non-empty interval is the non-empty interval.
  // The union of two non-empty intervals is
  // union = [min(argA.Min(),argB.Min()), max(argA.Max(),argB.Max()),]
  // Union() returns true if the union is not empty.
  bool Union( // this = union of two args
         const MYON_Interval&, 
         const MYON_Interval&
         );

  /////////////////////////
  // Expand the interval by adding delta to m_t[1] and subtracting
  // it from m_t[0].  So, when delta is positive and the interval is 
  // increasing this function expands the interval on each side.
  // returns true if the result is increasing.  
  bool Expand(double delta);
};


////////////////////////////////////////////////////////////////
//
//   MYON_2dPoint
//
class MYON_CLASS MYON_2dPoint
{
public:
  double x, y;

public:
  // x,y not initialized
  MYON_2dPoint() = default;
  ~MYON_2dPoint() = default;
  MYON_2dPoint(const MYON_2dPoint&) = default;
  MYON_2dPoint& operator=(const MYON_2dPoint&) = default;

public:
  static const MYON_2dPoint Origin;     // (0.0,0.0)
  static const MYON_2dPoint UnsetPoint; // (MYON_UNSET_VALUE,MYON_UNSET_VALUE)
  static const MYON_2dPoint NanPoint; // (MYON_DBL_QNAN,MYON_DBL_QNAN)

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_2dPoint& lhs,
    const MYON_2dPoint& rhs
    );

  /*
  Returns:
    (A+B)/2
  Remarks:
    Exact when coordinates are equal and prevents overflow.
  */
  static const MYON_2dPoint Midpoint(const MYON_2dPoint& A, const MYON_2dPoint& B);


  explicit MYON_2dPoint(double x,double y);
#if defined(OPENNURBS_WALL)
  // Goal is to eventually have all constructors that discard information be explicit.
  explicit 
#endif
  MYON_2dPoint(const MYON_3dPoint& );       // from 3d point

  explicit MYON_2dPoint(const MYON_4dPoint& h);       // from 4d point - h.w must be non-zero
  MYON_2dPoint(const MYON_2dVector& );      // from 2d vector
  explicit MYON_2dPoint(const MYON_3dVector& );      // from 3d vector

  explicit MYON_2dPoint(const double*);            // from double[2] array

  MYON_2dPoint(const class MYON_2fPoint&);  // from 2f point
  explicit MYON_2dPoint(const class MYON_3fPoint&);  // from 3f point
  explicit MYON_2dPoint(const class MYON_4fPoint& h);  // from 4f point - h.w must be non-zero
  explicit MYON_2dPoint(const class MYON_2fVector&); // from 2f vector
  explicit MYON_2dPoint(const class MYON_3fVector&); // from 3f vector

  explicit MYON_2dPoint(const float*);             // from float[2] array

  // (double*) conversion operators
  operator double*();
  operator const double*() const;

  // use implicit operator=(const MYON_2dPoint&)
  MYON_2dPoint& operator=(const MYON_3dPoint&);
  MYON_2dPoint& operator=(const MYON_4dPoint&);
  MYON_2dPoint& operator=(const MYON_2dVector&);
  MYON_2dPoint& operator=(const MYON_3dVector&);
  MYON_2dPoint& operator=(const double*); // point = double[2] support

  MYON_2dPoint& operator=(const MYON_2fPoint&);
  MYON_2dPoint& operator=(const MYON_3fPoint&);
  MYON_2dPoint& operator=(const MYON_4fPoint&);
  MYON_2dPoint& operator=(const MYON_2fVector&);
  MYON_2dPoint& operator=(const MYON_3fVector&);
  MYON_2dPoint& operator=(const float*);  // point = float[2] support

  MYON_2dPoint& operator*=(double);
  MYON_2dPoint& operator/=(double);
  MYON_2dPoint& operator+=(const MYON_2dVector&);
  MYON_2dPoint& operator-=(const MYON_2dVector&);

  MYON_2dPoint  operator*(int) const;
  MYON_2dPoint  operator/(int) const;
  MYON_2dPoint  operator*(float) const;
  MYON_2dPoint  operator/(float) const;
  MYON_2dPoint  operator*(double) const;
  MYON_2dPoint  operator/(double) const;

  MYON_2dPoint  operator+(const MYON_2dPoint&) const;
  MYON_2dPoint  operator+(const MYON_2dVector&) const;
  MYON_2dVector operator-(const MYON_2dPoint&) const;
  MYON_2dPoint  operator-(const MYON_2dVector&) const;
  MYON_3dPoint  operator+(const MYON_3dPoint&) const;
  MYON_3dPoint  operator+(const MYON_3dVector&) const;
  MYON_3dVector operator-(const MYON_3dPoint&) const;
  MYON_3dPoint  operator-(const MYON_3dVector&) const;

  MYON_2dPoint  operator+(const MYON_2fPoint&) const;
  MYON_2dPoint  operator+(const MYON_2fVector&) const;
  MYON_2dVector operator-(const MYON_2fPoint&) const;
  MYON_2dPoint  operator-(const MYON_2fVector&) const;
  MYON_3dPoint  operator+(const MYON_3fPoint&) const;
  MYON_3dPoint  operator+(const MYON_3fVector&) const;
  MYON_3dVector operator-(const MYON_3fPoint&) const;
  MYON_3dPoint  operator-(const MYON_3fVector&) const;

  double operator*(const MYON_2dPoint&) const; // dot product for points acting as vectors
  double operator*(const MYON_2dVector&) const; // dot product for points acting as vectors

  MYON_2dPoint operator*(const MYON_Xform&) const;

  bool operator==(const MYON_2dPoint&) const;
  bool operator!=(const MYON_2dPoint&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_2dPoint&) const;
  bool operator>=(const MYON_2dPoint&) const;
  bool operator<(const MYON_2dPoint&) const;
  bool operator>(const MYON_2dPoint&) const;

  // index operators mimic double[2] behavior
  double& operator[](int);
  double operator[](int) const;
  double& operator[](unsigned int);
  double operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is infinite, a nan, or MYON_UNSET_VALUE.
  */
  bool IsValid() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
  */
  bool IsUnset() const;

  /*
  Returns:
    True if any coordinate is a nan.
  */
  bool IsNan() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE, MYON_UNSET_POSITIVE_VALUE, or a nan
  */
  bool IsUnsetOrNan() const;

  // set 2d point value
  void Set(double x,double y);

  double DistanceTo( const MYON_2dPoint& ) const;
  double DistanceToSquared(const MYON_2dPoint&) const;

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  int MinimumCoordinateIndex() const;
  double MinimumCoordinate() const; // absolute value of minimum coordinate

  MYON_DEPRECATED_MSG("Use p = MYON_2dPoint::Origin;")
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
        double angle,              // angle in radians
        const MYON_2dPoint& center   // center of rotation
        );

  void Rotate( // rotatation in XY plane
        double sin_angle,          // sin(angle)
        double cos_angle,          // cos(angle)
        const MYON_2dPoint& center   // center of rotation
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_2dPoint operator*(int, const MYON_2dPoint&);

MYON_DECL
MYON_2dPoint operator*(float, const MYON_2dPoint&);

MYON_DECL
MYON_2dPoint operator*(double, const MYON_2dPoint&);

////////////////////////////////////////////////////////////////
//
//   MYON_3dPoint
//
class MYON_CLASS MYON_3dPoint
{
public:
  double x, y, z;

public:
  // x,y,z not initialized
  MYON_3dPoint() = default;
  ~MYON_3dPoint() = default;
  MYON_3dPoint(const MYON_3dPoint&) = default;
  MYON_3dPoint& operator=(const MYON_3dPoint&) = default;

public:
  static const MYON_3dPoint Origin;     // (0.0,0.0,0.0)
  static const MYON_3dPoint UnsetPoint; // (MYON_UNSET_VALUE,MYON_UNSET_VALUE,MYON_UNSET_VALUE)
  static const MYON_3dPoint NanPoint; // (MYON_DBL_QNAN,MYON_DBL_QNAN,MYON_DBL_QNAN)

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_3dPoint& lhs,
    const MYON_3dPoint& rhs
    );

  /*
  Returns:
    (A+B)/2
  Remarks:
    Exact when coordinates are equal and prevents overflow.
  */
  static const MYON_3dPoint Midpoint(const MYON_3dPoint& A, const MYON_3dPoint& B);

public:
  explicit MYON_3dPoint(double x,double y,double z);
#if defined(OPENNURBS_WALL)
  // Goal is to eventually have all constructors that discard information be explicit.
  explicit 
#endif
  MYON_3dPoint(const MYON_2dPoint& );       // from 2d point
  explicit MYON_3dPoint(const MYON_4dPoint& h); // from 4d point - h.w must be non-zero
  explicit MYON_3dPoint(const MYON_2dVector& );      // from 2d vector
  MYON_3dPoint(const MYON_3dVector& );      // from 3d vector
  explicit MYON_3dPoint(const double*);            // from double[3] array

  explicit MYON_3dPoint(const class MYON_2fPoint&);  // from 2f point
  MYON_3dPoint(const class MYON_3fPoint&);  // from 3f point
  explicit MYON_3dPoint(const class MYON_4fPoint& h );  // from 4f point- h.w must be non-zero
  explicit MYON_3dPoint(const class MYON_2fVector&); // from 2f point
  explicit MYON_3dPoint(const class MYON_3fVector&); // from 3f point
  explicit MYON_3dPoint(const float*);             // from float[3] array

  // (double*) conversion operators
  operator double*();
  operator const double*() const;

  // use implicit operator=(const MYON_3dPoint&)
  MYON_3dPoint& operator=(const MYON_2dPoint&);
  MYON_3dPoint& operator=(const MYON_4dPoint&);
  MYON_3dPoint& operator=(const MYON_2dVector&);
  MYON_3dPoint& operator=(const MYON_3dVector&);
  MYON_3dPoint& operator=(const double*); // point = double[3] support

  MYON_3dPoint& operator=(const class MYON_2fPoint&);
  MYON_3dPoint& operator=(const class MYON_3fPoint&);
  MYON_3dPoint& operator=(const class MYON_4fPoint&);
  MYON_3dPoint& operator=(const class MYON_2fVector&);
  MYON_3dPoint& operator=(const class MYON_3fVector&);
  MYON_3dPoint& operator=(const float*);  // point = float[3] support

  MYON_3dPoint& operator*=(double);
  MYON_3dPoint& operator/=(double);
  MYON_3dPoint& operator+=(const MYON_3dVector&);
  MYON_3dPoint& operator-=(const MYON_3dVector&);

  MYON_3dPoint  operator*(int) const;
  MYON_3dPoint  operator/(int) const;
  MYON_3dPoint  operator*(float) const;
  MYON_3dPoint  operator/(float) const;
  MYON_3dPoint  operator*(double) const;
  MYON_3dPoint  operator/(double) const;

  MYON_3dPoint  operator+(const MYON_3dPoint&) const;
  MYON_3dPoint  operator+(const MYON_3dVector&) const;
  MYON_3dVector operator-(const MYON_3dPoint&) const;
  MYON_3dPoint  operator-(const MYON_3dVector&) const;
  MYON_3dPoint  operator+(const MYON_2dPoint&) const;
  MYON_3dPoint  operator+(const MYON_2dVector&) const;
  MYON_3dVector operator-(const MYON_2dPoint&) const;
  MYON_3dPoint  operator-(const MYON_2dVector&) const;

  MYON_3dPoint  operator+(const MYON_3fPoint&) const;
  MYON_3dPoint  operator+(const MYON_3fVector&) const;
  MYON_3dVector operator-(const MYON_3fPoint&) const;
  MYON_3dPoint  operator-(const MYON_3fVector&) const;
  MYON_3dPoint  operator+(const MYON_2fPoint&) const;
  MYON_3dPoint  operator+(const MYON_2fVector&) const;
  MYON_3dVector operator-(const MYON_2fPoint&) const;
  MYON_3dPoint  operator-(const MYON_2fVector&) const;

  double operator*(const MYON_3dPoint&) const; // dot product for points acting as vectors
  double operator*(const MYON_3dVector&) const; // dot product for points acting as vectors
  
  MYON_3dPoint operator*(const MYON_Xform&) const;

  bool operator==(const MYON_3dPoint&) const;
  bool operator!=(const MYON_3dPoint&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_3dPoint&) const;
  bool operator>=(const MYON_3dPoint&) const;
  bool operator<(const MYON_3dPoint&) const;
  bool operator>(const MYON_3dPoint&) const;

  // index operators mimic double[3] behavior
  double& operator[](int);
  double operator[](int) const;
  double& operator[](unsigned int);
  double operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is infinite, a nan, or MYON_UNSET_VALUE.
  */
  bool IsValid() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
  */
  bool IsUnset() const;

  /*
  Returns:
    True if any coordinate is a nan.
*/
  bool IsNan() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE, MYON_UNSET_POSITIVE_VALUE, or a nan
  */
  bool IsUnsetOrNan() const;

  // set 3d point value
  void Set(double x,double y,double z);

  double DistanceTo( const MYON_3dPoint& ) const;
  double DistanceToSquared(const MYON_3dPoint&) const;

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate
  
  int MinimumCoordinateIndex() const;
  double MinimumCoordinate() const; // absolute value of minimum coordinate

  double Fuzz( double tolerance = MYON_ZERO_TOLERANCE ) const; // tolerance to use when comparing 3d points

#if defined(OPENNURBS_WALL)
  // Goal is to eventually remove all functions that modify points in place.
  MYON_DEPRECATED_MSG("Use p = MYON_3dPoint::Origin;")
#endif
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

  /*
  In  opennurbs points within MYON_ZERO_TOLERANCE are generally considered
  to be the same.
  Returns:
    True if for each coordinate pair,
    |a-b| <= MYON_ZERO_TOLERANCE 
    or |a-b| <= (fabs(a)+fabs(b))*MYON_RELATIVE_TOLERANCE.
  */
  bool IsCoincident(const MYON_3dPoint& P) const;

  // These transform the point in place. The transformation matrix acts on
  // the left of the point; i.e., result = transformation*point
  void Transform( 
        const MYON_Xform&
        );

  void Rotate( 
        double angle,             // angle in radians
        const MYON_3dVector& axis,  // axis of rotation
        const MYON_3dPoint& center  // center of rotation
        );

  void Rotate( 
        double sin_angle,         // sin(angle)
        double cos_angle,         // cos(angle)
        const MYON_3dVector& axis,  // axis of rotation
        const MYON_3dPoint& center  // center of rotation
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};
MYON_DECL
bool MYON_PointsAreCoincident(const MYON_3dPoint&, const MYON_3dPoint&);

MYON_DECL
MYON_3dPoint operator*(int, const MYON_3dPoint&);

MYON_DECL
MYON_3dPoint operator*(float, const MYON_3dPoint&);

MYON_DECL
MYON_3dPoint operator*(double, const MYON_3dPoint&);

////////////////////////////////////////////////////////////////
//
//   MYON_4dPoint (homogeneous coordinates)
//
class MYON_CLASS MYON_4dPoint
{
public:
  double x, y, z, w;

  /*
  Returns:
    MYON_UNSET_VALUE, if x or w is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
    and neither x nor w is a nan.
    x/w, otherwise
  Remarks:
    If w is 0.0 or nan, the result will be a nan.
  */
  double EuclideanX() const;

  /*
  Returns:
    MYON_UNSET_VALUE, if y or w is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
    and neither y nor w is a nan.
    y/w, otherwise
  Remarks:
    If w is 0.0 or nan, the result will be a nan.
  */
  double EuclideanY() const;

  /*
  Returns:
    MYON_UNSET_VALUE, if z or w is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
    and neither z nor w is a nan.
    z/w, otherwise
  Remarks:
    If w is 0.0 or nan, the result will be a nan.
  */
  double EuclideanZ() const;

public:
  // x,y,z,w not initialized
  MYON_4dPoint() = default;
  ~MYON_4dPoint() = default;
  MYON_4dPoint(const MYON_4dPoint&) = default;
  MYON_4dPoint& operator=(const MYON_4dPoint&) = default;

public:
  static const MYON_4dPoint Zero; // (0,0,0,0)
  static const MYON_4dPoint Nan; // (MYON_DBL_QNAN,MYON_DBL_QNAN,MYON_DBL_QNAN,MYON_DBL_QNAN)

  explicit MYON_4dPoint(double x,double y,double z,double w);

  // These constructors are not explicit because no information is lost.
  MYON_4dPoint(const MYON_2dPoint& );     // from 2d point (z,y,0,1)
  MYON_4dPoint(const MYON_3dPoint& );     // from 3d point (x,y,z,1)
  MYON_4dPoint(const MYON_2dVector& );    // from 2d vector (x,y,0,0)
  MYON_4dPoint(const MYON_3dVector& );    // from 3d vector (x,y,z,0)

  explicit MYON_4dPoint(const double*);          // from double[4] array

  // These constructors are not explicit because no information is lost.
  MYON_4dPoint(const MYON_2fPoint& );     // from 2f point (z,y,0,1)
  MYON_4dPoint(const MYON_3fPoint& );     // from 3f point (x,y,z,1)
  MYON_4dPoint(const MYON_4fPoint& );     // from 4f point
  MYON_4dPoint(const MYON_2fVector& );    // from 2f vector (z,y,0,0)
  MYON_4dPoint(const MYON_3fVector& );    // from 3f vector (x,y,z,0)

  explicit MYON_4dPoint(const float*);           // from float[4] array

  /*
  Description:
    This function is provided because in rare cases it makes sense.
    If you are not certain why you want this value, think carefully
    or work with vectors and points in Euclidean coordinates.
  Returns:
    lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
  Remark:
    It is intentional that there is no operator* override for MYON_4dPoint.
    This intentional omission helps people pause and think before calling
    MYON_4dPoint::InnerProduct().
  */
  static double InnerProduct(
    const MYON_4dPoint& lhs,
    const MYON_4dPoint& rhs
  );

  // (double*) conversion operators
  operator double*();
  operator const double*() const;

  // use implicit operator=(const MYON_4dPoint&)
  MYON_4dPoint& operator=(const MYON_2dPoint&);
  MYON_4dPoint& operator=(const MYON_3dPoint&);
  MYON_4dPoint& operator=(const MYON_2dVector&);
  MYON_4dPoint& operator=(const MYON_3dVector&);
  MYON_4dPoint& operator=(const double*); // point = double[4] support

  MYON_4dPoint& operator=(const class MYON_2fPoint&);
  MYON_4dPoint& operator=(const class MYON_3fPoint&);
  MYON_4dPoint& operator=(const class MYON_4fPoint&);
  MYON_4dPoint& operator=(const class MYON_2fVector&);
  MYON_4dPoint& operator=(const class MYON_3fVector&);
  MYON_4dPoint& operator=(const float*);  // point = float[4] support

  MYON_4dPoint& operator*=(double);
  MYON_4dPoint& operator/=(double);
  MYON_4dPoint& operator+=(const MYON_4dPoint&); // sum w = sqrt(|w1*w2|)
  MYON_4dPoint& operator-=(const MYON_4dPoint&); // difference w = sqrt(|w1*w2|)

  MYON_4dPoint  operator*(double) const;
  MYON_4dPoint  operator/(double) const;
  MYON_4dPoint  operator+(const MYON_4dPoint&) const; // sum w = sqrt(|w1*w2|)
  MYON_4dPoint  operator-(const MYON_4dPoint&) const; // difference w = sqrt(|w1*w2|)

  MYON_4dPoint operator*(const MYON_Xform&) const;

  /*
  Description:
    A well ordered projective compare function that is nan aware and can
    be used for robust sorting.
  Remarks:
    double c = non-nan value.
    MYON_4dPoint h0 = ...;
    MYON_4dPoint h1(c*h0.x,c*h0.x,c*h0.x,c*h0.x);
    0 == MYON_4dPoint::ProjectiveCompare(h0,ha);
  */
  static int ProjectiveCompare(
    const MYON_4dPoint& lhs,
    const MYON_4dPoint& rhs
  );

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int DictionaryCompare(
    const MYON_4dPoint& lhs,
    const MYON_4dPoint& rhs
  );

  /*
  Returns:
    True if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w).
  */
  bool operator==(const MYON_4dPoint& rhs) const;

  /*
  Returns:
    True if lhs.* != rhs.* for some coordinate and no values are nans.
  */
  bool operator!=(const MYON_4dPoint& rhs) const;

public:
  // index operators mimic double[4] behavior
  double& operator[](int);
  double operator[](int) const;
  double& operator[](unsigned int);
  double operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is infinite, a nan, or MYON_UNSET_VALUE.
  */
  bool IsValid() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
  */
  bool IsUnset() const;

  /*
  Returns:
    True if any coordinate is a nan.
*/
  bool IsNan() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE, MYON_UNSET_POSITIVE_VALUE, or a nan
  */
  bool IsUnsetOrNan() const;

  // set 4d point value
  void Set(double x,double y,double z,double w);

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  int MinimumCoordinateIndex() const;
  double MinimumCoordinate() const; // absolute value of minimum coordinate

  bool Normalize(); // set so x^2 + y^2 + z^2 + w^2 = 1

  // These transform the point in place. The transformation matrix acts on
  // the left of the point; i.e., result = transformation*point
  void Transform( 
        const MYON_Xform&
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_4dPoint operator*(double, const MYON_4dPoint&);

////////////////////////////////////////////////////////////////
//
//   MYON_2dVector
//
class MYON_CLASS MYON_2dVector
{
public:
  double x, y;
  
public:
  // x,y not initialized
  MYON_2dVector() = default;
  ~MYON_2dVector() = default;
  MYON_2dVector(const MYON_2dVector&) = default;
  MYON_2dVector& operator=(const MYON_2dVector&) = default;

public:
  static const MYON_2dVector ZeroVector;  // (0.0,0.0)
  static const MYON_2dVector XAxis;       // (1.0,0.0)
  static const MYON_2dVector YAxis;       // (0.0,1.0)
  static const MYON_2dVector UnsetVector; // (MYON_UNSET_VALUE,MYON_UNSET_VALUE)
  static const MYON_2dVector NanVector;   // (MYON_DBL_QNAN,MYON_DBL_QNAN)

  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_2dVector& lhs,
    const MYON_2dVector& rhs
    );

  // Description:
  //   A index driven function to get unit axis vectors.
  // Parameters:
  //   index - [in] 0 returns (1,0), 1 returns (0,1)
  // Returns:
  //   Unit 2d vector with vector[i] = (i==index)?1:0;
  static const MYON_2dVector& UnitVector(
    int // index
    );

  explicit MYON_2dVector(double x,double y);

  explicit MYON_2dVector(const MYON_3dVector& ); // from 3d vector
  MYON_2dVector(const MYON_2dPoint& );  // from 2d point
  explicit MYON_2dVector(const MYON_3dPoint& );  // from 3d point
  explicit MYON_2dVector(const double*);       // from double[2] array

  MYON_2dVector(const MYON_2fVector& ); // from 2f vector
  explicit MYON_2dVector(const MYON_3fVector& ); // from 3f vector
  explicit MYON_2dVector(const MYON_2fPoint& );  // from 2f point
  explicit MYON_2dVector(const MYON_3fPoint& );  // from 3f point
  explicit MYON_2dVector(const float*);        // from double[2] array

  // (double*) conversion operators
  operator double*();
  operator const double*() const;

  // use implicit operator=(const MYON_2dVector&)
  MYON_2dVector& operator=(const MYON_3dVector&);
  MYON_2dVector& operator=(const MYON_2dPoint&);
  MYON_2dVector& operator=(const MYON_3dPoint&);
  MYON_2dVector& operator=(const double*); // vector = double[2] support

  MYON_2dVector& operator=(const MYON_2fVector&);
  MYON_2dVector& operator=(const MYON_3fVector&);
  MYON_2dVector& operator=(const MYON_2fPoint&);
  MYON_2dVector& operator=(const MYON_3fPoint&);
  MYON_2dVector& operator=(const float*);  // vector = float[2] support

  MYON_2dVector  operator-() const;

  MYON_2dVector& operator*=(double);
  MYON_2dVector& operator/=(double);
  MYON_2dVector& operator+=(const MYON_2dVector&);
  MYON_2dVector& operator-=(const MYON_2dVector&);
  // DO NOT ADD ANY MORE overrides of += or -=

  double operator*(const MYON_2dVector&) const; // inner (dot) product
  double operator*(const MYON_2dPoint&) const; // inner (dot) product (point acting as vector)
  double operator*(const MYON_2fVector&) const; // inner (dot) product	

  MYON_2dVector  operator*(int) const;
  MYON_2dVector  operator/(int) const;
  MYON_2dVector  operator*(float) const;
  MYON_2dVector  operator/(float) const;
  MYON_2dVector  operator*(double) const;
  MYON_2dVector  operator/(double) const;

  MYON_2dVector  operator+(const MYON_2dVector&) const;
  MYON_2dPoint   operator+(const MYON_2dPoint&) const;
  MYON_2dVector  operator-(const MYON_2dVector&) const;
  MYON_2dPoint   operator-(const MYON_2dPoint&) const;
  MYON_3dVector  operator+(const MYON_3dVector&) const;
  MYON_3dPoint   operator+(const MYON_3dPoint&) const;
  MYON_3dVector  operator-(const MYON_3dVector&) const;
  MYON_3dPoint   operator-(const MYON_3dPoint&) const;

  MYON_2dVector  operator+(const MYON_2fVector&) const;
  MYON_2dPoint   operator+(const MYON_2fPoint&) const;
  MYON_2dVector  operator-(const MYON_2fVector&) const;
  MYON_2dPoint   operator-(const MYON_2fPoint&) const;
  MYON_3dVector  operator+(const MYON_3fVector&) const;
  MYON_3dPoint   operator+(const MYON_3fPoint&) const;
  MYON_3dVector  operator-(const MYON_3fVector&) const;
  MYON_3dPoint   operator-(const MYON_3fPoint&) const;

  MYON_2dVector operator*(const MYON_Xform&) const;

  bool operator==(const MYON_2dVector&) const;
  bool operator!=(const MYON_2dVector&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_2dVector&) const;
  bool operator>=(const MYON_2dVector&) const;
  bool operator<(const MYON_2dVector&) const;
  bool operator>(const MYON_2dVector&) const;

  // index operators mimic double[2] behavior
  double& operator[](int);
  double operator[](int) const;
  double& operator[](unsigned int);
  double operator[](unsigned int) const;

  /*
  Returns:
    False if any coordinate is infinite, a nan, or MYON_UNSET_VALUE.
  */
  bool IsValid() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
  */
  bool IsUnset() const;

  /*
  Returns:
    True if any coordinate is a nan.
*/
  bool IsNan() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE, MYON_UNSET_POSITIVE_VALUE, or a nan
  */
  bool IsUnsetOrNan() const;

  // set 2d vector value
  void Set(double x,double y);

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  int MinimumCoordinateIndex() const;
  double MinimumCoordinate() const; // absolute value of minimum coordinate

  double LengthSquared() const;
  double Length() const;

	// Signed area of the parallelagram.  The volume element.
	// returns x*B.y - y*B.x
	double WedgeProduct(const MYON_2dVector& B) const;

  bool Decompose( // Computes a, b such that this vector = a*X + b*Y
         // Returns false if unable to solve for a,b.  This happens
         // when X,Y is not really a basis.
         //
         // If X,Y is known to be an orthonormal frame,
         // then a = V*X, b = V*Y will compute
         // the same result more quickly.
         const MYON_2dVector&, // X
         const MYON_2dVector&, // Y
         double*, // a
         double*  // b
         ) const;

  int IsParallelTo( 
        // returns  1: this and other vectors are parallel
        //         -1: this and other vectors are anti-parallel
        //          0: this and other vectors are not parallel
        //             or at least one of the vectors is zero
        const MYON_2dVector& other,                           // other vector     
        double angle_tolerance = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  bool IsPerpendicularTo(
        // returns true:  this and other vectors are perpendicular
        //         false: this and other vectors are not perpendicular
        //                or at least one of the vectors is zero
        const MYON_2dVector& other,                           // other vector     
        double angle_tolerance = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  MYON_DEPRECATED_MSG("Use v = MYON_2dVector::ZeroVector;")
  void Zero(); // set all coordinates to zero;

  MYON_DEPRECATED_MSG("Use v = -v;")
  void Reverse(); // negate all coordinates

  bool Unitize();  // returns false if vector has zero length

  /*
  Returns:
  If this is a valid non-zero vector, a unit vector parallel to this is returned.
  Otherwise the zero vector is returned.
  */
  MYON_2dVector UnitVector() const;


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
         double tiny_tol = MYON_ZERO_TOLERANCE // tiny_tol
         ) const;

  // Returns:
  //   true if vector is the zero vector.
  bool IsZero() const;

  /*
  Returns:
    true if at lease one coordinate is not zero and no coordinates are unset or nans.
  */
  bool IsNotZero() const;

  // Returns:
  //   true if vector is valid and has length 1.
  bool IsUnitVector() const;

  // set this vector to be perpendicular to another vector
  bool PerpendicularTo( // Result is not unitized. 
                        // returns false if input vector is zero
        const MYON_2dVector& 
        );

  // set this vector to be perpendicular to a line defined by 2 points
  bool PerpendicularTo( 
        const MYON_2dPoint&, 
        const MYON_2dPoint& 
        );
  /*
   Signed Angle (in radians) to A from B. A and B must be non-zero vectors.
  Returns
    -MYON_PI <= angle <= MYON_PI
   Note: The function is anti-symmetric, i.e Angle(A,B) = -  Angle(A,B)
 */
  static double SignedAngle(const MYON_2dVector& A, const MYON_2dVector& B);

  // These transform the vector in place. The transformation matrix acts on
  // the left of the vector; i.e., result = transformation*vector
  void Transform( 
        const MYON_Xform& // can use MYON_Xform here
        );

  void Rotate( 
        double angle            // angle in radians
        );

  void Rotate( 
        double sin_angle,       // sin(angle)
        double cos_angle        // cos(angle)
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

MYON_DECL
MYON_2dVector operator*(int, const MYON_2dVector&);

MYON_DECL
MYON_2dVector operator*(float, const MYON_2dVector&);

MYON_DECL
MYON_2dVector operator*(double, const MYON_2dVector&);

///////////////////////////////////////////////////////////////
//
// MYON_2dVector utilities
//

MYON_DECL
double 
MYON_DotProduct( 
    const MYON_2dVector&, 
    const MYON_2dVector& 
    );

MYON_DECL
MYON_3dVector 
MYON_CrossProduct(
    const MYON_2dVector&, 
    const MYON_2dVector& 
    );

MYON_DECL
double			 
MYON_WedgeProduct(		// signed area of the parallelagram.  Volume element.
    const MYON_2dVector& A, // returns A.x * B.y - A.y * B.x 
    const MYON_2dVector& B 
    );

MYON_DECL
bool 
MYON_IsOrthogonalFrame( // true if X, Y are nonzero and mutually perpendicular
    const MYON_2dVector&, // X
    const MYON_2dVector&  // Y
    );

MYON_DECL
bool 
MYON_IsOrthonormalFrame( // true if X, Y are orthogonal and unit length
    const MYON_2dVector&, // X
    const MYON_2dVector&  // Y
    );

MYON_DECL
bool 
MYON_IsRightHandFrame( // true if X, Y are orthonormal and right handed
    const MYON_2dVector&, // X
    const MYON_2dVector&  // Y
    );

////////////////////////////////////////////////////////////////
//
//   MYON_3dVector
//
class MYON_CLASS MYON_3dVector
{
public:
  double x, y, z;

public:
  // x,y,z not initialized
  MYON_3dVector() = default;
  ~MYON_3dVector() = default;
  MYON_3dVector(const MYON_3dVector&) = default;
  MYON_3dVector& operator=(const MYON_3dVector&) = default;

public:
  static const MYON_3dVector ZeroVector;  // (0.0,0.0,0.0)
  static const MYON_3dVector XAxis;       // (1.0,0.0,0.0)
  static const MYON_3dVector YAxis;       // (0.0,1.0,0.0)
  static const MYON_3dVector ZAxis;       // (0.0,0.0,1.0)
  static const MYON_3dVector UnsetVector; // (MYON_UNSET_VALUE,MYON_UNSET_VALUE,MYON_UNSET_VALUE)
  static const MYON_3dVector NanVector; // (MYON_DBL_QNAN,MYON_DBL_QNAN,MYON_DBL_QNAN)
  
  /*
  Description:
    A well ordered dictionary compare function that is nan aware and can
    be used for robust sorting.
  */
  static int Compare(
    const MYON_3dVector& lhs,
    const MYON_3dVector& rhs
    );

  // Description:
  //   A index driven function to get unit axis vectors.
  // Parameters:
  //   index - [in] 0 returns (1,0,0), 1 returns (0,1,0), 
  //                2 returns (0,0,1)
  // Returns:
  //   Unit 3d vector with vector[i] = (i==index)?1:0;
  static const MYON_3dVector& UnitVector(
    int // index
    );

  explicit MYON_3dVector(double x,double y,double z);
#if defined(OPENNURBS_WALL)
  // Goal is to eventually have all constructors that change dimension be explicit.
  explicit 
#endif
  MYON_3dVector(const MYON_2dVector& );  // from 2d vector
  explicit MYON_3dVector(const MYON_2dPoint& );   // from 2d point
  MYON_3dVector(const MYON_3dPoint& );   // from 3d point
  explicit MYON_3dVector(const double*);        // from double[3] array

  explicit MYON_3dVector(const MYON_2fVector& );  // from 2f vector
  MYON_3dVector(const MYON_3fVector& );  // from 3f vector
  explicit MYON_3dVector(const MYON_2fPoint& );   // from 2f point
  explicit MYON_3dVector(const MYON_3fPoint& );   // from 3f point
  explicit MYON_3dVector(const float*);         // from float[3] array

  // (double*) conversion operators
  operator double*();
  operator const double*() const;

  // use implicit operator=(const MYON_3dVector&)
  MYON_3dVector& operator=(const MYON_2dVector&);
  MYON_3dVector& operator=(const MYON_2dPoint&);
  MYON_3dVector& operator=(const MYON_3dPoint&);
  MYON_3dVector& operator=(const double*); // vector = double[3] support
  
  MYON_3dVector& operator=(const MYON_2fVector&);
  MYON_3dVector& operator=(const MYON_3fVector&);
  MYON_3dVector& operator=(const MYON_2fPoint&);
  MYON_3dVector& operator=(const MYON_3fPoint&);
  MYON_3dVector& operator=(const float*);  // vector = float[3] support

  MYON_3dVector  operator-() const;

  MYON_3dVector& operator*=(double);
  MYON_3dVector& operator/=(double);
  MYON_3dVector& operator+=(const MYON_3dVector&);
  MYON_3dVector& operator-=(const MYON_3dVector&);
  // DO NOT ADD ANY MORE overrides of += or -=

  double operator*(const MYON_3dVector&) const; // inner (dot) product
  double operator*(const MYON_3dPoint&) const; // inner (dot) product
  double operator*(const MYON_3fVector&) const; // inner (dot) product

  MYON_3dVector  operator*(int) const;
  MYON_3dVector  operator/(int) const;
  MYON_3dVector  operator*(float) const;
  MYON_3dVector  operator/(float) const;
  MYON_3dVector  operator*(double) const;
  MYON_3dVector  operator/(double) const;

  MYON_3dVector  operator+(const MYON_3dVector&) const;
  MYON_3dPoint   operator+(const MYON_3dPoint&) const;
  MYON_3dVector  operator-(const MYON_3dVector&) const;
  MYON_3dPoint   operator-(const MYON_3dPoint&) const;
  MYON_3dVector  operator+(const MYON_2dVector&) const;
  MYON_3dPoint   operator+(const MYON_2dPoint&) const;
  MYON_3dVector  operator-(const MYON_2dVector&) const;
  MYON_3dPoint   operator-(const MYON_2dPoint&) const;

  MYON_3dVector  operator+(const MYON_3fVector&) const;
  MYON_3dPoint   operator+(const MYON_3fPoint&) const;
  MYON_3dVector  operator-(const MYON_3fVector&) const;
  MYON_3dPoint   operator-(const MYON_3fPoint&) const;
  MYON_3dVector  operator+(const MYON_2fVector&) const;
  MYON_3dPoint   operator+(const MYON_2fPoint&) const;
  MYON_3dVector  operator-(const MYON_2fVector&) const;
  MYON_3dPoint   operator-(const MYON_2fPoint&) const;

  MYON_3dVector operator*(const MYON_Xform&) const;

  bool operator==(const MYON_3dVector&) const;
  bool operator!=(const MYON_3dVector&) const;

  // dictionary order comparisons
  bool operator<=(const MYON_3dVector&) const;
  bool operator>=(const MYON_3dVector&) const;
  bool operator<(const MYON_3dVector&) const;
  bool operator>(const MYON_3dVector&) const;

  // index operators mimic double[3] behavior
  double& operator[](int);
  double operator[](int) const;
  double& operator[](unsigned int);
  double operator[](unsigned int) const;

  static double DotProduct(
    MYON_3dVector A,
    MYON_3dVector B
  );

  static const MYON_3dVector CrossProduct(
    MYON_3dVector A,
    MYON_3dVector B
  );

  /*
  Returns:
    False if any coordinate is infinite, a nan, or MYON_UNSET_VALUE.
  */
  bool IsValid() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
  */
  bool IsUnset() const;

  /*
  Returns:
    True if any coordinate is a nan.
  */
  bool IsNan() const;

  /*
  Returns:
    True if any coordinate is MYON_UNSET_VALUE, MYON_UNSET_POSITIVE_VALUE, or a nan
  */
  bool IsUnsetOrNan() const;

  // set 3d vector value
  void Set(double x,double y,double z);

  int MaximumCoordinateIndex() const;
  double MaximumCoordinate() const; // absolute value of maximum coordinate

  int MinimumCoordinateIndex() const;
  double MinimumCoordinate() const; // absolute value of minimum coordinate

  double LengthSquared() const;
  double Length() const;

  bool Decompose( // Computes a, b, c such that this vector = a*X + b*Y + c*Z
         // Returns false if unable to solve for a,b,c.  This happens
         // when X,Y,Z is not really a basis.
         //
         // If X,Y,Z is known to be an orthonormal frame,
         // then a = V*X, b = V*Y, c = V*Z will compute
         // the same result more quickly.
         const MYON_3dVector&, // X
         const MYON_3dVector&, // Y
         const MYON_3dVector&, // Z
         double*, // a
         double*, // b
         double*  // c
         ) const;

  int IsParallelTo( 
        // returns  1: this and other vectors are parallel
        //         -1: this and other vectors are anti-parallel
        //          0: this and other vectors are not parallel
        //             or at least one of the vectors is zero
        const MYON_3dVector& other,                           // other vector     
        double angle_tolerance = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  bool IsPerpendicularTo(
        // returns true:  this and other vectors are perpendicular
        //         false: this and other vectors are not perpendicular
        //                or at least one of the vectors is zero
        const MYON_3dVector& other,                           // other vector     
        double angle_tolerance = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  double Fuzz( double tolerance = MYON_ZERO_TOLERANCE ) const; // tolerance to use when comparing 3d vectors

#if defined(OPENNURBS_WALL)
  // Goal is to eventually remove all functions that modify vectors in place.
  MYON_DEPRECATED_MSG("Use v = MYON_3dVector::ZeroVector;")
#endif
  void Zero(); // set all coordinates to zero;

#if defined(OPENNURBS_WALL)
  // Goal is to eventually remove all functions that modify vectors in place.
  MYON_DEPRECATED_MSG("Use v = -v;")
#endif
  void Reverse(); // negate all coordinates

  bool Unitize();  // returns false if vector has zero length
  double LengthAndUnitize(); // unitizes and returns initial length

  /*
  Returns:
    If this is a valid non-zero vector, a unit vector parallel to this is returned.
    Otherwise the zero vector is returned.
  */
  MYON_3dVector UnitVector() const;

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
         double tiny_tol = MYON_ZERO_TOLERANCE // tiny_tol
         ) const;

  // Returns:
  //   true if vector is the zero vector.
  bool IsZero() const;

  /*
  Returns:
    true if at lease one coordinate is not zero and no coordinates are unset or nans.
  */
  bool IsNotZero() const;


  // Returns:
  //   true if vector is valid and has length 1.
  bool IsUnitVector() const;

  // set this vector to be perpendicular to another vector
  bool PerpendicularTo( // Result is not unitized. 
                        // returns false if input vector is zero
        const MYON_3dVector& 
        );

  // set this vector to be perpendicular to a plane defined by 3 points
  bool PerpendicularTo(
               // about 3 times slower than
               //    MYON_3dVector N = MYON_CrossProduct(P1-P0,P2-P0); 
               //    N.Unitize();
               // returns false if points are coincident or collinear
         const MYON_3dPoint&, const MYON_3dPoint&, const MYON_3dPoint& 
         );

  /*
  Parameters:
    failure_result - [in]
      vector to return if this is zero or unset.
      When in doubt, pass MYON_3dVector::NanVector.
  Returns:
    If this is nonzero, a vector perpendicular to this.
    The returned vector is not unitized.
    Otherwise failure_result is returned.
  */
  const MYON_3dVector Perpendicular(
    MYON_3dVector failure_result
  ) const;

  /*
    Angle (in radians) between two vectors. A and B must be non-zero vectors
   Returns 
     0<= angle <= MYON_PI
    Note: The function is symmetric, i.e Angle(A,B) = Angle(B,A)
          This function is always accurate, in particular for nearly parallel,
          anti-parallel or  perpendicular vectors
  */
  static double Angle(const MYON_3dVector& A, const MYON_3dVector& B);

  // These transform the vector in place. The transformation matrix acts on
  // the left of the vector; i.e., result = transformation*vector
  void Transform( 
        const MYON_Xform& // can use MYON_Xform here
        );

  void Rotate( 
        double angle,           // angle in radians
        const MYON_3dVector& axis // axis of rotation
        );

  void Rotate( 
        double sin_angle,        // sin(angle)
        double cos_angle,        // cos(angle)
        const MYON_3dVector& axis  // axis of rotation
        );

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;
};

class MYON_CLASS MYON_3dRay
{
public:
  MYON_3dPoint  m_P;
  MYON_3dVector m_V;
};

/*
Description:
  Typically the vector portion is a unit vector and
  m_d = -(x*P.x + y*P.y + z*P.z) for a point P on the plane.
*/
class MYON_CLASS MYON_PlaneEquation
{
public:
  // C++ defaults for construction, destruction, copies, and operator=
  // work fine.

  static const MYON_PlaneEquation UnsetPlaneEquation; // (MYON_UNSET_VALUE,MYON_UNSET_VALUE,MYON_UNSET_VALUE,MYON_UNSET_VALUE)
  static const MYON_PlaneEquation ZeroPlaneEquation; // (0.0,0.0,0.0,0.0)
  static const MYON_PlaneEquation NanPlaneEquation; // (MYON_DBL_QNAN,MYON_DBL_QNAN,MYON_DBL_QNAN,MYON_DBL_QNAN)

  static const MYON_PlaneEquation WorldXY; // (0,0,1,0)
  static const MYON_PlaneEquation WorldYZ; // (1,0,0,0)
  static const MYON_PlaneEquation WorldZX; // (0,1,0,0)

  /*
  Returns:
    If the three points are valid and not colinear, a unitized plane equation is returned.
    Otherwise MYON_PlaneEquation::NanPlaneEquation is returned.
  */
  static const MYON_PlaneEquation CreateFromThreePoints(
    MYON_3dPoint pointA,
    MYON_3dPoint pointB,
    MYON_3dPoint pointC
    );

  /*
  Returns:
    If point is valid and normal is nonzero, a unitized plane equation is returned.
    Otherwise MYON_PlaneEquation::NanPlaneEquation is returned.
  */
  static const MYON_PlaneEquation CreateFromPointAndNormal(
    MYON_3dPoint point,
    MYON_3dVector normal
    );

  MYON_PlaneEquation();

  MYON_PlaneEquation(
    double xx, double yy, double zz, double dd
    );

  static void Set(
    MYON_PlaneEquation& plane_equation,
    double x, double y, double z, double d
    );

  double MaximumCoefficient() const;

  /*
  Returns:
    The plane equation whose coefficient values are 
    the negative of the coefficient values in this,
    provided the coefficient value is valid. Any invalid
    coefficient values are copied.
  */
  MYON_PlaneEquation NegatedPlaneEquation() const;


  /*
  Returns:
    The plane equation whose first three coefficient values 
    are a unit vector.
  */
  MYON_PlaneEquation UnitizedPlaneEquation() const;



  /*
  Description:
    returns true if x, y, z, d are valid, finite doubles.
  Remarks:
    this function will return true if x, y and z are all zero.
  See Also:
    MYON_PlaneEquation::IsSet().
  */
  bool IsValid() const;

  /*
  Description:
    returns true if x, y, z, d are valid, finite doubles and at least one of x, y or z is not zero.
  */
  bool IsSet() const;

  /*
  Description:
    returns true if x, y, z, d are valid, finite doubles and x^2 + y^2 + z^2 = 1.
  */
  bool IsUnitized() const;

  /*
  Description:
    Sets (x,y,z) to a unitized N and then sets
    d = -(x*P.x + y*P.y + z*P.z).
  Parameters:
    P - [in] point on the plane
    N - [in] vector perpendicular to the plane
  Returns:
     true if input is valid.
  */
  bool Create( MYON_3dPoint P, MYON_3dVector N );

  // index operators mimic double[4] behavior
  // Return null refs or MYON_UNSET_VALUE for out-of-range indices
  double& operator[](int);
  double& operator[](unsigned int);
  double operator[](int) const;
  double operator[](unsigned int) const;

  /*
  Returns:
    Direction (x,y,z)
  */
  MYON_3dVector Direction() const;

  double DirectionLength() const;

  /*
  Returns:
    Unitized direction or zero vector if not set.
  */
  MYON_3dVector UnitNormal() const;

  /*
    Returns  1: this and other vectors are parallel
            -1: this and other vectors are anti-parallel
             0: this and other vectors are not parallel
                or at least one of the vectors is zero
  */
  int IsParallelTo( 
        const MYON_PlaneEquation& other,                      // other plane equation
        double angle_tolerance = MYON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
        ) const;

  /*
  Description:
    Evaluate the plane at a point.
  Parameters:
    P - [in]
  Returns:
    x*P.x + y*P.y + z*P.z + d;
  */
  double ValueAt(MYON_3dPoint P) const;
  double ValueAt(MYON_4dPoint P) const;
  double ValueAt(MYON_3dVector P) const;
  double ValueAt(double x, double y, double z) const;

  /*
  Returns:
    MYON_Interval::EmptyInterval if input is not valid.
  */
  MYON_Interval ValueRange(
    size_t point_list_count,
    const MYON_3dPoint* point_list
    ) const;

  /*
  Returns:
    MYON_Interval::EmptyInterval if input is not valid.
  */
  MYON_Interval ValueRange(
    const MYON_SimpleArray< MYON_3dPoint >& point_list
    ) const;

  /*
  Returns:
    MYON_Interval::EmptyInterval if input is not valid.
  */
  MYON_Interval ValueRange(
    size_t point_list_count,
    const MYON_3fPoint* point_list
    ) const;

  /*
  Returns:
    MYON_Interval::EmptyInterval if input is not valid.
  */
  MYON_Interval ValueRange(
    const MYON_SimpleArray< MYON_3fPoint >& point_list
    ) const;

  /*
  Returns:
    MYON_Interval::EmptyInterval if input is not valid.
  */
  MYON_Interval ValueRange(
    const class MYON_3dPointListRef& point_list
    ) const;

  /*
  Returns:
    MYON_Interval::EmptyInterval if input is not valid.
  */
  MYON_Interval ValueRange(
    size_t point_index_count,
    const unsigned int* point_index_list,
    const class MYON_3dPointListRef& point_list
    ) const;

  MYON_Interval ValueRange(
    size_t point_index_count,
    size_t point_index_stride,
    const unsigned int* point_index_list,
    const class MYON_3dPointListRef& point_list
    ) const;

  /*
  Description:
    Evaluate the plane at a list of point values.
  Parameters:
    Pcount - [in]
      number of points
    P - [in]
      points
    value - [in]
      If not null, value[] must be an array of length at least Pcount.
      The values will be stored in this array.  If null, the an array
      will be allocated with onmalloc() and returned.
    value_range - [out]
      If not null, the range of values will be returned here.
  Returns:
    An array of Pcount values.  If the input parameter value was null,
    then the array is allocated on the heap using onmalloc() and the 
    caller is responsible for calling onfree() when finished.  If the 
    input is not valid, null is returned.
  */
  double* ValueAt(
        int Pcount,
        const MYON_3fPoint* P,
        double* value,
        double value_range[2]
        ) const;

  double* ValueAt(
        int Pcount,
        const MYON_3dPoint* P,
        double* value,
        double value_range[2]
        ) const;

  /*
  Description:
    This function calculates and evaluates points that 
    would be exactly on the plane if double precision
    arithmetic were mathematically perfect and returns
    the largest value of the evaluations.
  */
  double ZeroTolerance() const;

  /*
  Description:
    Transform the plane equation so that, if e0 is the initial
    equation, e1 is transformed equation and P is a point,
    then e0.ValueAt(P) = e1.ValueAt(xform*P).
  Parameters:
    xform - [in]
      Invertable transformation.
  Returns:
    True if the plane equation was successfully transformed.
    False if xform is not invertable or the equation is not
    valid.
  Remarks:
    This function has to invert xform.  If you have apply the
    same transformation to a bunch of planes, then it will be
    more efficient to calculate xform's inverse transpose
    and apply the resultingt transformation to the equation's
    coefficients as if they were 4d point coordinates.
  */
  bool Transform( const MYON_Xform& xform );

  /*
  Description:
    Get point on plane that is closest to a given point.
  Parameters:
    point - [in]
  Returns:
    A 3d point on the plane that is closest to the input point.
  */
  MYON_3dPoint ClosestPointTo( MYON_3dPoint point ) const;

  /*
  Description:
    Get the minimum value of the plane equation
    on a bounding box.
  Parameters:
    bbox - [in] 
  Returns:
    Minimum value of the plane equation on the bounding box.
  */
  double MinimumValueAt(const MYON_BoundingBox& bbox) const;

  /*
  Description:
    Get the maximum value of the plane equation
    on a bounding box.
  Parameters:
    bbox - [in] 
  Returns:
    Maximum value of the plane equation on the bounding box.
  */
  double MaximumValueAt(const MYON_BoundingBox& bbox) const;


  /*
  Description:
    Get the maximum value of the plane equation on a set of 3d points.
  Parameters:
    bRational - [in]
      False if the points are euclidean (x,y,z)
      True if the points are homogeneous rational (x,y,z,w)
      (x/w,y/w,z/w) is used to evaluate the value.
    point_count - [in]
    point_stride - [in]
      i-th point's x coordinate = points[i*point_stride]
    points - [in]
      coordinates of points
    stop_value - [in]
      If stop_value is valid and not MYON_UNSET_VALUE, then the 
      evaluation stops if a value > stop_value is found. 
      If stop_value = MYON_UNSET_VALUE, then stop_value is ignored.
  Returns:
    Maximum value of the plane equation on the point list.
    If the input is not valid, then MYON_UNSET_VALUE is returned.
  */
  double MaximumValueAt(
    bool bRational,
    int point_count,
    int point_stride,
    const double* points,
    double stop_value
    ) const;

  /*
  Description:
    Get the minimum value of the plane equation on a set of 3d points.
  Parameters:
    bRational - [in]
      False if the points are euclidean (x,y,z)
      True if the points are homogeneous rational (x,y,z,w)
      (x/w,y/w,z/w) is used to evaluate the value.
    point_count - [in]
    point_stride - [in]
      i-th point's x coordinate = points[i*point_stride]
    points - [in]
      coordinates of points
    stop_value - [in]
      If stop_value is valid and not MYON_UNSET_VALUE, then the 
      evaluation stops if a value < stop_value is found. 
      If stop_value = MYON_UNSET_VALUE, then stop_value is ignored.
  Returns:
    Maximum value of the plane equation on the point list.
    If the input is not valid, then MYON_UNSET_VALUE is returned.
  */
  double MinimumValueAt(
    bool bRational,
    int point_count,
    int point_stride,
    const double* points,
    double stop_value
    ) const;

  /*
  Description:
    Get the maximum absolute value of the plane equation 
    on a set of 3d points.
  Parameters:
    bRational - [in]
      False if the points are euclidean (x,y,z)
      True if the points are homogeneous rational (x,y,z,w)
      (x/w,y/w,z/w) is used to evaluate the value.
    point_count - [in]
    point_stride - [in]
      i-th point's x coordinate = points[i*point_stride]
    points - [in]
      coordinates of points
    stop_value - [in]
      If stop_value >= 0.0, then the evaluation stops if an
      absolute value > stop_value is found. If stop_value < 0.0 
      or stop_value is invalid, then stop_value is ignored.
  Returns:
    Maximum value of the plane equation on the point list.
    If the input is not valid, then MYON_UNSET_VALUE is returned.
  */
  double MaximumAbsoluteValueAt(
    bool bRational,
    int point_count,
    int point_stride,
    const double* points,
    double stop_value
    ) const;

  /*
  Description:
    Test points on a bezier curve to see if they are near the plane.
  Parameters:
    bezcrv - [in]
    s0 - [in]
    s1 - [in] the interval from s0 to s1 is tested (s0 < s1)
    sample_count - [in] number of interior points to test.  
                Numbers like 1, 3, 7, 15, ... work best.
    endpoint_tolerance - [in] If >= 0, then the end points are 
              tested to see if the distance from the endpoints 
              is <= endpoint_tolerance.
    interior_tolerance - [in] (>=0 and >=endpoint_tolerance) 
              This tolerance is used to test the interior sample points.
    smin - [put]  If not nullptr, *smin = bezier parameter of nearest
                  test point.
    smax - [put]  If not nullptr, *smax = bezier parameter of farthest
                  test point.  If false is returned, this is the
                  parameter of the test point that failed.
  Returns:
    True if all the tested points passed the tolerance test.
    False if at least one tested point failed the tolerance test.
    (The test terminates when the first failure is encountered.)
  */
  bool IsNearerThan( 
          const class MYON_BezierCurve& bezcrv,
          double s0,
          double s1,
          int sample_count,
          double endpoint_tolerance,
          double interior_tolerance,
          double* smin,
          double* smax
          ) const;
  
  bool operator==(const MYON_PlaneEquation&) const;
  bool operator!=(const MYON_PlaneEquation&) const;

  double x;
  double y;
  double z;
  double d; // 4th coefficient of the plane equation.

  void Dump(class MYON_TextLog&) const;
};

MYON_DECL
const MYON_PlaneEquation operator*(const MYON_Xform&, const MYON_PlaneEquation&);


#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_PlaneEquation>;

#endif

MYON_DECL
MYON_3dVector operator*(int, const MYON_3dVector&);

MYON_DECL
MYON_3dVector operator*(float, const MYON_3dVector&);

MYON_DECL
MYON_3dVector operator*(double, const MYON_3dVector&);

///////////////////////////////////////////////////////////////
//
// MYON_3dVector utilities
//

MYON_DECL
double 
MYON_DotProduct( 
    const MYON_3dVector&, 
    const MYON_3dVector& 
    );


MYON_DECL
MYON_3dVector 
MYON_CrossProduct(
    const MYON_3dVector&, 
    const MYON_3dVector& 
    );

MYON_DECL
MYON_3dVector 
MYON_CrossProduct( // 3d cross product for old fashioned arrays
    const double*, // array of 3d doubles
    const double*  // array of 3d doubles
    );

MYON_DECL
double 
MYON_TripleProduct( 
    const MYON_3dVector&,
    const MYON_3dVector&,
    const MYON_3dVector&
    );

MYON_DECL
double 
MYON_TripleProduct(  // 3d triple product for old fashioned arrays
    const double*, // array of 3d doubles
    const double*, // array of 3d doubles
    const double*  // array of 3d doubles
    );

MYON_DECL
bool 
MYON_IsOrthogonalFrame( // true if X, Y, Z are nonzero and mutually perpendicular
    const MYON_3dVector&, // X
    const MYON_3dVector&, // Y
    const MYON_3dVector&  // Z 
    );

MYON_DECL
bool 
MYON_IsOrthonormalFrame( // true if X, Y, Z are orthogonal and unit length
    const MYON_3dVector&, // X
    const MYON_3dVector&, // Y
    const MYON_3dVector&  // Z 
    );

MYON_DECL
bool 
MYON_IsRightHandFrame( // true if X, Y, Z are orthonormal and right handed
    const MYON_3dVector&, // X
    const MYON_3dVector&, // Y
    const MYON_3dVector&  // Z 
    );

// Find the largest absolute value of coordinates from an array of points (possibly homogeneous).
MYON_DECL
double MYON_MaximumCoordinate(const double* data, int dim, bool is_rat, int count);

///////////////////////////////////////////////////////////////
//
// common points and vectors
//
// MYON_unset_point is obsolete - use MYON_3dPoint::UnsetPoint
#if !defined(OPENNURBS_WALL)

// OBSOLETE - use MYON_3dPoint::UnsetPoint
#define MYON_unset_point MYON_3dPoint::UnsetPoint
// OBSOLETE - use MYON_3dPoint::UnsetPoint
#define MYON_UNSET_POINT MYON_3dPoint::UnsetPoint
// OBSOLETE - use MYON_3dPoint::UnsetVector
#define MYON_UNSET_VECTOR MYON_3dVector::UnsetVector
// OBSOLETE - use MYON_3dPoint::Origin
#define MYON_origin MYON_3dPoint::Origin
// OBSOLETE - use MYON_3dVector::XAxis
#define MYON_xaxis MYON_3dVector::XAxis
// OBSOLETE - use MYON_3dVector::YAxis
#define MYON_yaxis MYON_3dVector::YAxis
// OBSOLETE - use MYON_3dVector::ZAxis
#define MYON_zaxis MYON_3dVector::ZAxis
// OBSOLETE  - use MYON_3fPoint::Origin
#define MYON_forigin MYON_3fPoint::Origin
// OBSOLETE  - use MYON_3fVector::XAxis
#define MYON_fxaxis MYON_3fVector::XAxis
// OBSOLETE  - use MYON_3fVector::YAxis
#define MYON_fyaxis MYON_3fVector::YAxis
// OBSOLETE  - use MYON_3fVector::ZAxis
#define MYON_fzaxis MYON_3fVector::ZAxis

#endif

#include "opennurbs_fpoint.h"

////////////////////////////////////////////////////////////////
//
//   MYON_SurfaceCurvature
//
class MYON_CLASS MYON_SurfaceCurvature
{
public:
  static const MYON_SurfaceCurvature CreateFromPrincipalCurvatures(
    double k1,
    double k2
  );

  static const MYON_SurfaceCurvature Nan;
  static const MYON_SurfaceCurvature Zero;

public:
  double k1, k2; // principal curvatures

public:
  bool IsSet() const;
  bool IsZero() const;
  bool IsUnset() const;

public:
  double GaussianCurvature() const;
  double MeanCurvature() const;
  double MinimumRadius() const;
  double MaximumRadius() const;
};


#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_2dPoint>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_3dPoint>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_4dPoint>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_2dVector>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_3dVector>;

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_2fPoint>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_3fPoint>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_4fPoint>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_2fVector>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_3fVector>;

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Color>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SurfaceCurvature>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Interval>;

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_2dex>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_3dex>;

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_COMPONENT_INDEX>;

#endif

/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_2dPointArray : public MYON_SimpleArray<MYON_2dPoint>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_2dPointArray();
  MYON_2dPointArray(int);
  MYON_2dPointArray( const MYON_2dPointArray& );
  MYON_2dPointArray& operator=( const MYON_2dPointArray& );

  bool GetBBox( // returns true if successful
         double boxmin[2],
         double boxmax[2],
         bool bGrowBox = false  // true means grow box
         ) const;

  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_2fPointArray : public MYON_SimpleArray<MYON_2fPoint>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_2fPointArray();
  MYON_2fPointArray(int);
  MYON_2fPointArray(const MYON_2fPointArray&);
  MYON_2fPointArray& operator=( const MYON_2fPointArray& );

  bool GetBBox( // returns true if successful
         float boxmin[2],
         float boxmax[2],
         bool bGrowBox = false  // true means grow box
         ) const;
  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_3dPointArray : public MYON_SimpleArray<MYON_3dPoint>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_3dPointArray();
  MYON_3dPointArray(int);
  MYON_3dPointArray(const MYON_SimpleArray<MYON_3dPoint>&);
  MYON_3dPointArray& operator=( const MYON_3dPointArray& );
  MYON_3dPointArray(const MYON_SimpleArray<MYON_3fPoint>&);
  MYON_3dPointArray& operator=( const MYON_SimpleArray<MYON_3fPoint>& );

  // Description:
  //   Create 3d point list
  // Parameters:
  //   point_dimension - [in] dimension of input points (2 or 3)
  //   bRational - [in] true if points are in homogeneous rational form
  //   point_count - [in] number of points
  //   point_stride - [in] number of doubles to skip between points
  //   points - [in] array of point coordinates
  bool Create(
    int point_dimension,
    int bRational,
    int point_count,
    int point_stride,
    const double* points
    );

  // Description:
  //   Create 3d point list
  // Parameters:
  //   point_dimension - [in] dimension of input points (2 or 3)
  //   bRational - [in] true if points are in homogeneous rational form
  //   point_count - [in] number of points
  //   point_stride - [in] number of doubles to skip between points
  //   points - [in] array of point coordinates
  bool Create(
    int point_dimension,
    int bRational,
    int point_count,
    int point_stride,
    const float* points
    );

  // Description: 
  //   Get 3d axis aligned bounding box.
  // Returns:
  //   3d bounding box of point list.
  MYON_BoundingBox BoundingBox() const;

  // Description: 
  //   Get 3d axis aligned bounding box
  //   of a subset of the points.
  // Parameters:
  //   from - [in] start of the box calculation
  //   count - [in] number of items computed
  // Returns:
  //   3d bounding box of point list.
  MYON_BoundingBox BoundingBox(int from, int count) const;

  // Description:
  //   Get 3d axis aligned bounding box or the union
  //   of the input box with the point list's bounding box.
  // Parameters:
  //   bbox - [in/out] 3d axis aligned bounding box
  //   bGrowBox - [in] (default=false) 
  //     If true, then the union of the input bbox and the 
  //     point list's bounding box is returned in bbox.  
  //     If false, the point list's bounding box is returned in bbox.
  // Returns:
  //   true if successful.
  bool GetBoundingBox( 
    MYON_BoundingBox& bbox,
    int bGrowBox = false
    ) const;

  // Description:
  //   Get axis aligned bounding box.
  // Parameters:
  //   boxmin - [in/out] array of 3 doubles
  //   boxmax - [in/out] array of 3 doubles
  //   bGrowBox - [in] (default=false) 
  //     If true, then the union of the input bounding box and the 
  //     object's bounding box is returned.
  //     If false, the object's bounding box is returned.
  // Returns:
  //   true if object has bounding box and calculation was successful
  bool GetBBox(
         double boxmin[3],
         double boxmax[3],
         bool bGrowBox = false
         ) const;

  /*
	Description:
    Get tight bounding box of the point list.
	Parameters:
		tight_bbox - [in/out] tight bounding box
		bGrowBox -[in]	(default=false)			
      If true and the input tight_bbox is valid, then returned
      tight_bbox is the union of the input tight_bbox and the 
      tight bounding box of the point list.
		xform -[in] (default=nullptr)
      If not nullptr, the tight bounding box of the transformed
      point list is calculated.  The point list is not modified.
	Returns:
    True if the returned tight_bbox is set to a valid 
    bounding box.
  */
	bool GetTightBoundingBox( 
			MYON_BoundingBox& tight_bbox, 
      bool bGrowBox = false,
			const MYON_Xform* xform = nullptr
      ) const;

  // Description:
  //   Transform points by applying xform to each point.
  // Parameters:
  //   xform - [in] transformation matrix
  // Returns:
  //   true if successful.
  bool Transform( 
    const MYON_Xform& xform 
    );

  // Description:
  //   Swaps point coordinate values with indices i and j.
  // Parameters:
  //   i - [in] coordinate index
  //   j - [in] coordinate index
  // Returns:
  //   true if successful.
  // Example:
  //   The call SwapCoordinates(0,2) would swap the x and z
  //   coordinates of each point in the array.
  bool SwapCoordinates(
    int i,
    int j
    );

  // Description:
  //   Rotate points about a center and axis.  A positive angle
  //   results in a counter-clockwise rotation about the axis
  //   of rotation.
  // Parameters:
  //   sin_angle - [in] sine of rotation angle
  //   cos_angle - [in] cosine of rotation angle
  //   axis_of_rotation - [in] axis of rotation
  //   center_of_rotation - [in] center (fixed point) of rotation
  // Returns:
  //   true if successful.
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const MYON_3dVector& axis_of_rotation,
        const MYON_3dPoint& center_of_rotation
        );

  // Description:
  //   Rotate points about a center and axis.  A positive angle
  //   results in a counter-clockwise rotation about the axis
  //   of rotation.
  // Parameters:
  //   angle - [in] angle in radians.  Polsine of rotation angle
  //   cos_angle - [in] cosine of rotation angle
  //   axis_of_rotation - [in] axis of rotation
  //   center_of_rotation - [in] center (fixed point) of rotation
  // Returns:
  //   true if successful.
  bool Rotate(
        double angle_in_radians,
        const MYON_3dVector& axis_of_rotation,
        const MYON_3dPoint& center_of_rotation
        );

  // Description:
  //   Translate a polyline
  // Parameters:
  //   delta - [in] translation vectorsine of rotation angle
  // Returns:
  //   true if successful.
  bool Translate(
        const MYON_3dVector& delta
        );

  /*
  Description:
    Get the index of the point in the array that is closest
    to P.
  Parameters:
    P - [in]
    closest_point_index - [out]
    maximum_distance - [in] optional distance constraint.
        If maximum_distance > 0, then only points Q with
        |P-Q| <= maximum_distance are returned.
  Returns:
    True if a point is found; in which case *closest_point_index
    is the index of the point.  False if no point is found
    or the input is not valid.
  See Also:
    MYON_GetClosestPointInPointList
    MYON_PointCloud::GetClosestPoint
  */
  bool GetClosestPoint( 
          MYON_3dPoint P,
          int* closest_point_index,
          double maximum_distance = 0.0
          ) const;


};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_3fPointArray : public MYON_SimpleArray<MYON_3fPoint>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_3fPointArray();
  MYON_3fPointArray(int);
  MYON_3fPointArray(const MYON_3fPointArray&);
  MYON_3fPointArray& operator=( const MYON_3fPointArray& );

  bool GetBBox(
         float boxmin[3],
         float boxmax[3],
         bool bGrowBox = false
         ) const;

  bool Transform( const MYON_Xform& );

  bool SwapCoordinates(int,int);
};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_4dPointArray : public MYON_SimpleArray<MYON_4dPoint>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_4dPointArray();
  MYON_4dPointArray(int);
  MYON_4dPointArray(const MYON_4dPointArray&);
  MYON_4dPointArray& operator=( const MYON_4dPointArray& );

  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_4fPointArray : public MYON_SimpleArray<MYON_4fPoint>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_4fPointArray();
  MYON_4fPointArray(int);
  MYON_4fPointArray(const MYON_4fPointArray&);
  MYON_4fPointArray& operator=( const MYON_4fPointArray& );

  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_2dVectorArray : public MYON_SimpleArray<MYON_2dVector>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_2dVectorArray();
  MYON_2dVectorArray(int);
  MYON_2dVectorArray(const MYON_2dVectorArray&);
  MYON_2dVectorArray& operator=( const MYON_2dVectorArray& );

  bool GetBBox(
         double boxmin[2],
         double boxmax[2],
         bool bGrowBox = false
         ) const;

  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_2fVectorArray : public MYON_SimpleArray<MYON_2fVector>
{
public:
  // see MYON_SimpleArray class definition comments for constructor documentation
  MYON_2fVectorArray();
  MYON_2fVectorArray(int);
  MYON_2fVectorArray(const MYON_2fVectorArray&);
  MYON_2fVectorArray& operator=( const MYON_2fVectorArray& );

  bool GetBBox(
         float boxmin[2],
         float boxmax[2],
         bool = false
         ) const;

  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};


/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_3dVectorArray : public MYON_SimpleArray<MYON_3dVector>
{
public:
  MYON_3dVectorArray();
  MYON_3dVectorArray(int);
  MYON_3dVectorArray(const MYON_3dVectorArray&);
  MYON_3dVectorArray& operator=( const MYON_3dVectorArray& );

  bool GetBBox(
         double boxmin[3],
         double boxmax[3],
         bool bGrowBow = false
         ) const;

  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};

/////////////////////////////////////////////////////////////////
//

class MYON_CLASS MYON_3fVectorArray : public MYON_SimpleArray<MYON_3fVector>
{
public:
  MYON_3fVectorArray();
  MYON_3fVectorArray(int);
  MYON_3fVectorArray(const MYON_3fVectorArray&);
  MYON_3fVectorArray& operator=( const MYON_3fVectorArray& );

  bool GetBBox(
         float boxmin[3],
         float boxmax[3],
         bool bGrowBox = false
         ) const;

  bool Transform( const MYON_Xform& );
  bool SwapCoordinates(int,int);
};

class MYON_CLASS MYON_3dPointListRef
{
public:


  MYON_3dPointListRef()
    : m_point_count(0)
    , m_point_stride(0)
    , m_dP(0)
    , m_fP(0)
  {}

  /*
  Description:
    Construct a point list that references the mesh vertex list.
  Remarks:
    If the mesh has double precision vertices, then the point
    list will refer to them; otherwise, the point list will
    refer to the single precision vertices.
  */
  MYON_3dPointListRef(
    const class MYON_Mesh* mesh
    );

  /*
  Description:
    Construct a point list that references the points
    in a simple array of MYON_3dPoint objects.
  */
  MYON_3dPointListRef(
    const class MYON_SimpleArray<MYON_3dPoint>& point_array
    );

  /*
  Description:
    Construct a point list that references the points
    in a simple array of MYON_3fPoint objects.
  */
  MYON_3dPointListRef(
    const class MYON_SimpleArray<MYON_3fPoint>& point_array
    );

  static const MYON_3dPointListRef EmptyPointList;

  static
  MYON_3dPointListRef FromDoubleArray(
    size_t point_count,
    size_t point_stride,
    const double* point_array
    );

  static
  MYON_3dPointListRef FromFloatArray(
    size_t point_count,
    size_t point_stride,
    const float* point_array
    );

  static
  MYON_3dPointListRef FromPointArray(
    const class MYON_SimpleArray<MYON_3dPoint>& point_array
    );

  static
  MYON_3dPointListRef FromPointArray(
    const class MYON_SimpleArray<MYON_3fPoint>& point_array
    );

  static
  MYON_3dPointListRef FromMesh(
    const class MYON_Mesh* mesh
    );

  /*
  Returns:
    A copy of the refenced points in an MYON_SimpleArray<MYON_3dPoint>.
  */
  MYON_SimpleArray<MYON_3dPoint> To3dPointArray() const;

  /*
  Returns:
    A copy of the refenced points in an MYON_SimpleArray<MYON_3fPoint>.
  */
  MYON_SimpleArray<MYON_3fPoint> To3fPointArray() const;

  /*
  Description:
    Set this point list to reference points with double coordinates.
  Parameters:
    point_count - [in]
      number of points
    point_stride - [in] (>= 3)
      number of doubles between points.
    point_array - [in]
      pointer to the first coordinate of the first point.
  Returns:
    Number of points in the list.
  */
  unsigned int SetFromDoubleArray(
    size_t point_count,
    size_t point_stride,
    const double* point_array
    );

  /*
  Description:
    Set this point list to reference points with float coordinates.
  Parameters:
    point_count - [in]
      number of points
    point_stride - [in] (>= 3)
      number of floats between points.
    point_array - [in]
      pointer to the first coordinate of the first point.
  Returns:
    Number of points in the list.
  */
  unsigned int SetFromFloatArray(
    size_t point_count,
    size_t point_stride,
    const float* point_array
    );

  /*
  Description:
    Set this point list to reference a mesh vertex list.
  Parameters:
    mesh - [in]
  Returns:
    Number of points in the list.
  */
  unsigned int SetFromMesh(
    const class MYON_Mesh* mesh
    );

  /*
  Returns:
    0: no points
    1: single precision points (float coordinates)
    2: double precision points (double coordinates)
  */
  unsigned int Precision() const;

  /*
  Returns:
    true if the points are double precision
  */
  bool DoublePrecision() const;

  /*
  Returns:
    true if the points are single precision
  */
  bool SinglePrecision() const;

  /*
  Description:
    Copy point location into buffer and return it as an MYON_3dPoint.
  Parameters:
    point_index - [in]
    buffer - [out]
      If point_index is a valid index, the point coordinates
      are copied to buffer[]; otherwise the buffer coordinates
      are set to MYON_UNSET_VALUE.
      You must insure buffer is not null, has proper alignment
      for storing doubles, and is large enough to store three
      doubles.
  Returns:
    A reference to an MYON_3dPoint which is a cast of buffer.
  Remarks:
    This is the fastest and most efficient way to get a the
    location of a point into memory you are managing.
  */
  inline const class MYON_3dPoint& GetPoint(
    unsigned int point_index, 
    double buffer[3]
    ) const
  {
    if ( point_index < m_point_count )
    {
      if ( m_dP )
      {
        const double* p = m_dP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
      else
      {
        const float* p = m_fP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
    }
    else
    {
      buffer[0] = buffer[1] = buffer[2] = MYON_UNSET_VALUE;
    }
    return *((const MYON_3dPoint*)buffer);
  }

  /*
  Parameters:
    point_index - [in]
  Returns:
    If point_index is a valid index, the point location is returned.
    Otherwise MYON_3dPoint::UnsetPoint is returned.
  */
  inline MYON_3dPoint Point(
    unsigned int point_index
    ) const
  {
    double buffer[3];
    if ( point_index < m_point_count )
    {
      if ( m_dP )
      {
        const double* p = m_dP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
      else
      {
        const float* p = m_fP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
    }
    else
    {
      buffer[0] = buffer[1] = buffer[2] = MYON_UNSET_VALUE;
    }
    return *((const MYON_3dPoint*)buffer);
  }

  inline MYON_3dPoint operator[](int point_index) const
  {
    double buffer[3];
    if ( point_index >= 0 && ((unsigned int)point_index) < m_point_count )
    {
      if ( m_dP )
      {
        const double* p = m_dP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
      else
      {
        const float* p = m_fP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
    }
    else
    {
      buffer[0] = buffer[1] = buffer[2] = MYON_UNSET_VALUE;
    }
    return *((const MYON_3dPoint*)buffer);  
  }

  inline MYON_3dPoint operator[](unsigned int point_index) const
  {
    double buffer[3];
    if ( point_index < m_point_count )
    {
      if ( m_dP )
      {
        const double* p = m_dP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
      else
      {
        const float* p = m_fP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
    }
    else
    {
      buffer[0] = buffer[1] = buffer[2] = MYON_UNSET_VALUE;
    }
    return *((const MYON_3dPoint*)buffer);
  }

  inline MYON_3dPoint operator[](MYON__INT64 point_index) const
  {
    double buffer[3];
    if ( point_index >= 0 && ((MYON__UINT64)point_index) < m_point_count )
    {
      if ( m_dP )
      {
        const double* p = m_dP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
      else
      {
        const float* p = m_fP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
    }
    else
    {
      buffer[0] = buffer[1] = buffer[2] = MYON_UNSET_VALUE;
    }
    return *((const MYON_3dPoint*)buffer);  
  }

  inline MYON_3dPoint operator[](MYON__UINT64 point_index) const
  {
    double buffer[3];
    if ( point_index < m_point_count )
    {
      if ( m_dP )
      {
        const double* p = m_dP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
      else
      {
        const float* p = m_fP + (point_index*m_point_stride);
        buffer[0]  = *p++;
        buffer[1]  = *p++;
        buffer[2]  = *p;
      }
    }
    else
    {
      buffer[0] = buffer[1] = buffer[2] = MYON_UNSET_VALUE;
    }
    return *((const MYON_3dPoint*)buffer);  
  }

  inline unsigned int PointCount() const
  {
    return m_point_count;
  }

  inline unsigned int PointStride() const
  {
    return m_point_stride;
  }

  inline const double* PointDoubleArray() const
  {
    return m_dP;
  }

  inline const float* PointFloatArray() const
  {
    return m_fP;
  }

  /*
  Returns:
    Number of points copied to face_points[] array.
  */
  unsigned int GetMeshFacePoints(
    const class MYON_MeshFace* mesh_face,
    MYON_3dPoint face_points[4]
    ) const;

  /*
  Returns:
    Number of points copied to ngon_points[] array.
  */
  unsigned int GetMeshNgonPoints(
    const class MYON_MeshNgon* mesh_ngon,
    size_t ngon_points_capacity,
    class MYON_3dPoint* ngon_points
    ) const;

  /*
  Returns:
    Number of points copied to ngon_points[] array.
  */
  unsigned int GetMeshNgonPoints(
    const class MYON_MeshNgon* mesh_ngon,
    MYON_SimpleArray<MYON_3dPoint>& ngon_points
    ) const;

  /*
  Returns:
    Number of points copied to quad_points[] array.
  */
  unsigned int GetQuadPoints(
    const int quad_point_indices[4],
    class MYON_3dPoint quad_points[4]
    ) const;

  /*
  Returns:
    Number of points copied to quad_points[] array.
  */
  unsigned int GetQuadPoints(
    const unsigned int quad_point_indices[4],
    class MYON_3dPoint quad_points[4]
    ) const;

  /*
  Returns:
    Number of points copied to triangle_points[] array.
  */
  unsigned int GetTrianglePoints(
    const int triangle_point_indices[3],
    class MYON_3dPoint triangle_points[3]
    ) const;

  /*
  Returns:
    Number of points copied to triangle_points[] array.
  */
  unsigned int GetTrianglePoints(
    const unsigned int triangle_point_indices[3],
    class MYON_3dPoint triangle_points[3]
    ) const;  

  /*
  Returns:
    Number of points copied to points[] array.
  */
  unsigned int GetPoints(
    int point_index_count,
    const int* point_index_list,
    class MYON_3dPoint* points
    ) const;

  /*
  Returns:
    Number of points copied to points[] array.
  */
  unsigned int GetPoints(
    unsigned int point_index_count,
    const unsigned int* point_index_list,
    class MYON_3dPoint* points
    ) const;

  /*
  Returns:
    Number of points copied to points[] array.
  */
  unsigned int GetPoints(
    const MYON_SimpleArray<int>& point_index_list,
    MYON_SimpleArray<MYON_3dPoint>& points
    ) const;

  /*
  Returns:
    Number of points copied to points[] array.
  */
  unsigned int GetPoints(
    int point_index_count,
    const int* point_index_list,
    MYON_SimpleArray<MYON_3dPoint>& points
    ) const;

  /*
  Returns:
    Number of points copied to points[] array.
  */
  unsigned int GetPoints(
    unsigned int point_index_count,
    const unsigned int* point_index_list,
    MYON_SimpleArray<MYON_3dPoint>& points
    ) const;
  
private:
  unsigned int m_point_count;
  unsigned int m_point_stride;
  const double* m_dP;
  const float* m_fP;
};


/*
Class MYON_2dSize
*/
class MYON_CLASS MYON_2dSize
{
public:
  // Default construction intentionally leaves x and y uninitialized.
  // Use something like
  //   MYON_2dSize pt(1.0,2.0);
  // or 
  //  MYON_2dSize pt = MYON_2dSize::Zero;
  // when you need an initialized MYON_2dSize.
  MYON_2dSize() = default;

  ~MYON_2dSize() = default;
  MYON_2dSize(const MYON_2dSize& ) = default;
  MYON_2dSize& operator=(const MYON_2dSize& ) = default;

  MYON_2dSize(
    double cx,
    double cy
  );

  /*
  Dictionary compare.
  Returns: 
  -1: lhs < rhs
   0: lsh == rsh
  +1: lhs > rhs
  */
  static int Compare(
    const MYON_2dSize& lhs,
    const MYON_2dSize& rhs
  );

  /*
  Dictionary compare.
  Returns: 
  -1: lhs < rhs
   0: lsh == rsh
  +1: lhs > rhs
  */
  static int ComparePointer(
    const MYON_2dSize* lhs,
    const MYON_2dSize* rhs
  );

public:
  static const MYON_2dSize Zero;  // (0.0,0.0)
  static const MYON_2dSize Unset; // (MYON_UNSET_DOUBLE,MYON_UNSET_DOUBLE)
  
public:
  /*
  Returns:
    true if both cx and cy are 0.0
  */
  bool IsZero() const;

  /*
  Returns:
    true if neither cx nor cy are MYON_UNSET_DOUBLE.
  */
  bool IsSet() const;
  
public:
  double cx;
  double cy;
};

MYON_DECL
bool operator==(
  const MYON_2dSize& lhs,
  const MYON_2dSize& rhs
  );

MYON_DECL
bool operator!=(
  const MYON_2dSize& lhs,
  const MYON_2dSize& rhs
  );

#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_2dSize>;

#endif

/*
Class MYON_4iRect
  For those situations where a Windows SDK RECT or MFC CRect 
  value needs to be used in code that does not link with MFC.
  If you want a traditional bounding box, use MYON_2dBoundingBox.
*/
class MYON_CLASS MYON_4dRect
{
public:
  // Default construction intentionally leaves x and y uninitialized.
  // Use something like
  //   MYON_4dRect pt(1.0,2.0,3.0,4.0);
  // or 
  //  MYON_4dRect pt = MYON_4dRect::Zero;
  // when you need an initialized MYON_4dRect.
  MYON_4dRect() = default;

  ~MYON_4dRect() = default;
  MYON_4dRect(const MYON_4dRect& ) = default;
  MYON_4dRect& operator=(const MYON_4dRect& ) = default;

  MYON_4dRect(
  double left,
  double top,
  double right,
  double bottom
  );

  MYON_4dRect(const MYON_2dPoint topLeft, const MYON_2dPoint& bottomRight);
  MYON_4dRect(const MYON_2dPoint& point, const MYON_2dSize& size);

public:
  static const MYON_4dRect Zero;  // (0.0,0.0,0.0,0.0)
  static const MYON_4dRect Unset; // (MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX)

public:
  /*
  Returns:
    true if all of left, top, right, and bottom are set to 0.
  */
  bool IsZero() const;

  void SetZero();

  /*
  Returns:
    true if none of left, top, right, or bottom is set to MYON_UNSET_INT_INDEX
  */
  bool IsSet() const;

  double Width(void) const;
  double Height(void) const;

  const MYON_2dSize Size(void) const;
  
  const MYON_2dPoint CenterPoint(void) const;
  const MYON_2dPoint TopLeft(void) const;
  const MYON_2dPoint BottomRight(void) const;

  bool  IntersectRect(const MYON_4dRect* r1, const MYON_4dRect* r2);
  bool  IntersectRect(const MYON_4dRect& r1, const MYON_4dRect& r2);

  bool IsRectEmpty(void) const;
  bool IsRectNull(void) const;
  void SetRectEmpty(void) { *this = Zero; }
  void SetRect(double l, double t, double r, double b);

  bool PtInRect(const MYON_2dPoint& pt) const;

  void OffsetRect(double, double);
  void OffsetRect(const MYON_2dVector&);
  void InflateRect(double, double);
  void InflateRect(double, double, double, double);
  void DeflateRect(double, double);
  bool SubtractRect(const MYON_4dRect* rect1, const MYON_4dRect* rect2);

  void NormalizeRect();

public:
  double left;
  double top;
  double right;
  double bottom;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_4dRect>;
#endif


MYON_DECL
bool operator==(const MYON_4dRect&, const MYON_4dRect&);

MYON_DECL
bool operator!=(const MYON_4dRect&, const MYON_4dRect&);

/*
Description:
  Tool for efficiently calculating a boundary polyline winding number 
  with input tools that do not require the boundary polyline to be
  a contiguous array of points.
*/
class MYON_CLASS MYON_WindingNumber
{
public:
  MYON_WindingNumber() = default;
  ~MYON_WindingNumber() = default;
  MYON_WindingNumber(const MYON_WindingNumber&) = default;
  MYON_WindingNumber& operator=(const MYON_WindingNumber&) = default;

public:
  static const MYON_WindingNumber Unset;

public:
  /*
  Description:
    The calculation of the winding number begins with a call to SetWindingPoint().
  Parameters:
    x - [in]
    y - [in]
      The coordinates of the winding point are (x,y).
  Remarks:
    Calling SetWindingPoint() erases results of any previous calculations.
  */
  void SetWindingPoint(double x, double y);

  /*
  Description:
    The calculation of the winding number begins with a call to SetWindingPoint().
  Parameters:
    winding_point - [in]
  Remarks:
    Calling SetWindingPoint() erases results of any previous calculations.
  */
  void SetWindingPoint(MYON_2dPoint winding_point);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    p - [in]
      next point in boundary.
  Returns:
    Number of boundary edge segments added.
  */
  MYON__UINT32 AddBoundary(MYON_2dPoint p);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    p - [in]
      start of edge segment. 
      Caller is responsible for insuring coordinates of p are valid doubles.
    q - [in]
      end of edge segment
      Caller is responsible for insuring coordinates of q are valid doubles.
      The calculation tolerates p==q.
  Returns:
    Number of boundary edge segments added.
  */
  MYON__UINT32 AddBoundary(MYON_2dPoint p, MYON_2dPoint q);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    point_count - [in] >= 2
      number of points in boundary_points[] array.
    point_stride - [in] >= 2
      The i-th point has coordinates (boundary_points[i*point_stride],boundary_points[i*point_stride+1]).
    boundary_points - [in]
      Boundary points.
    bCloseBoundary - [in]
      If true, an edge segment is added from the last boundary point to the first boundary point.
  Returns:
    Number of boundary edge segments added.
  Remarks:
    The calculation tolerates zero length edge segments. The caller is responsible for insuring the
    coordinates in boundary_points[] are valid doubles.
  */
  MYON__UINT32 AddBoundary(size_t point_count, size_t point_stride, const double* boundary_points, bool bCloseBoundary);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    point_count - [in] >= 2
      number of points in boundary_points[] array.
    point_stride - [in] >= 2
      The i-th point has coordinates (boundary_points[i*point_stride],boundary_points[i*point_stride+1]).
    boundary_points - [in]
      Boundary points.
    bCloseBoundary - [in]
      If true, an edge segment is added from the last boundary point to the first boundary point.
  Returns:
    Number of boundary edge segments added.
  Remarks:
    The calculation tolerates zero length edge segments. The caller is responsible for insuring the
    coordinates in boundary_points[] are valid doubles.
  */
  MYON__UINT32 AddBoundary(size_t point_count, size_t point_stride, const float* boundary_points, bool bCloseBoundary);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    point_count - [in] >= 2
      number of points in boundary_points[] array.
    point_stride - [in] >= 2
      The i-th point has coordinates (boundary_points[i*point_stride],boundary_points[i*point_stride+1]).
    boundary_points - [in]
      Boundary points.
    bCloseBoundary - [in]
      If true, an edge segment is added from the last boundary point to the first boundary point.
  Returns:
    Number of boundary edge segments added.
  Remarks:
    The calculation tolerates zero length edge segments. The caller is responsible for insuring the
    coordinates in boundary_points[] are valid doubles.
  */
  MYON__UINT32 AddBoundary(size_t point_count, size_t point_stride, const int* boundary_points, bool bCloseBoundary);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    point_count - [in] >= 2
      number of points in boundary_points[] array.
    boundary_points - [in]
      Boundary points.
    bCloseBoundary - [in]
      If true, an edge segment is added from the last boundary point to the first boundary point.
  Returns:
    Number of boundary edge segments added.
  Remarks:
    The calculation tolerates zero length edge segments. The caller is responsible for insuring the
    coordinates in boundary_points[] are valid doubles.
  */
  MYON__UINT32 AddBoundary(size_t point_count, const MYON_2dPoint* boundary_points, bool bCloseBoundary);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    point_count - [in] >= 2
      number of points in boundary_points[] array.
    boundary_points - [in]
      Boundary points.
    bCloseBoundary - [in]
      If true, an edge segment is added from the last boundary point to the first boundary point.
  Returns:
    Number of boundary edge segments added.
  Remarks:
    The calculation tolerates zero length edge segments. The caller is responsible for insuring the
    coordinates in boundary_points[] are valid doubles.
  */
  MYON__UINT32 AddBoundary(size_t point_count, const MYON_3dPoint* boundary_points, bool bCloseBoundary);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    point_count - [in] >= 2
      number of points in boundary_points[] array.
    boundary_points - [in]
      Boundary points.
    bCloseBoundary - [in]
      If true, an edge segment is added from the last boundary point to the first boundary point.
  Returns:
    Number of boundary edge segments added.
  Remarks:
    The calculation tolerates zero length edge segments. The caller is responsible for insuring the
    coordinates in boundary_points[] are valid doubles.
  */
  MYON__UINT32 AddBoundary(size_t point_count, const MYON_2fPoint* boundary_points, bool bCloseBoundary);

  /*
  Description:
    After calling SetWindingPoint(), the boundary may be specified by one or more calls to
    various AddBoundary functions.
    The boundary may be specified one point at a time, one edge at a time, portions of the boundary
    at a time, or the entire boundary in a single call. The edges may be added in any order.
    The caller is responsible for insuring the collection calls to AddBoundary() results in a
    a continuous, oriented, and closed polyline.
  Parameters:
    point_count - [in] >= 2
      number of points in boundary_points[] array.
    boundary_points - [in]
      Boundary points.
    bCloseBoundary - [in]
      If true, an edge segment is added from the last boundary point to the first boundary point.
  Returns:
    Number of boundary edge segments added.
  Remarks:
    The calculation tolerates zero length edge segments. The caller is responsible for insuring the
    coordinates in boundary_points[] are valid doubles.
  */
  MYON__UINT32 AddBoundary(size_t point_count, const MYON_3fPoint* boundary_points, bool bCloseBoundary);

  /*
  Returns:
    The winding number of the boundary about the winding point.
  */
  MYON__INT32 WindingNumber() const;

  /*
  Returns:
    The winding point.
  */
  const MYON_2dPoint WindingPoint() const;

  /*
  Returns:
    Number of segments in the boundary.
  */
  MYON__UINT32 BoundarySegmentCount() const;

  /*
  Returns:
    The end of the previous call to AddBoundary()
  */
  const MYON_2dPoint PreviousBoundaryPoint() const;

private:
  // Location of the winding point.
  MYON_2dPoint m_winding_point = MYON_2dPoint::NanPoint;

  // Location of the last boundary point added. This is used 
  // by AddBoundary(MYON_2dPoint p) to generate a segment
  // from m_prev_boundary_point to p in situations where
  // points are streamed so the caller doesn't have to
  // deal with accumulating the previous point and can
  // mix streamed points with other forms of boundary input.
  MYON_2dPoint m_prev_boundary_point = MYON_2dPoint::NanPoint;

  // Number of boundary segments in the polyline
  MYON__UINT32 m_boundary_segment_count = 0;

  // In the comments below, H is the horizontal line throught the winding point
  // and V is the vertical line through the winding point.

  // signed net number of times polyline crosses H to the left of the winding point.
  // A below to above crossing is -1.
  MYON__INT32 m_left_crossing_number = 0;

  // signed net number of times polyline crosses H to the right of the winding point.
  // A below to above crossing is +1.
  MYON__INT32 m_right_crossing_number = 0;

  // signed net number of times polyline crosses V to the below of the winding point.
  // A left to right crossing is +1.
  MYON__INT32 m_below_crossing_number = 0;

  // signed net number of times polyline crosses V to the above of the winding point.
  // A left to right crossing is -1.
  MYON__INT32 m_above_crossing_number = 0;

  // 0 != (m_status_bits & 1): left crossing occurred
  // 0 != (m_status_bits & 2): right crossing occurred
  // 0 != (m_status_bits & 4): below crossing occurred
  // 0 != (m_status_bits & 8): above crossing occurred
  // 0 != (m_status_bits & 16): winding point on horizontal segment
  // 0 != (m_status_bits & 32): winding point on vertical segment
  MYON__INT32 m_status_bits = 0;

  void Internal_AddBoundarySegment(const double* p, const double* q);

  // Input: p and q are 2d points with p.y <= 0 and q.y > 0
  //
  // Returns:
  //   Sign of the x coordinate of the intersection of the line segment from p to q 
  //   and the x axis.
  static int Internal_SignOfX(const MYON_2dPoint& p, const MYON_2dPoint& q);

  // Input: p and q are 2d points with p.x <= 0 and q.x > 0
  //
  // Returns:
  //   Sign of the y coordinate of the intersection of the line segment from p to q 
  //   and the y axis.
  static int Internal_SignOfY(const MYON_2dPoint& p, const MYON_2dPoint& q);

  bool Internal_HaveWindingPoint() const;
};


/*
MYON_PeriodicDomain is a helper class for dealing with closed or periodic surfaces using the idea of a covering space.

If a surface is closed in the u-direction (or v respectively),  a curve on the surface that crosses the seam
will not have a continuous pull back to parameter space.  However, if we extend the surface domain
in the u-coordinates and allow the surface to cover itself periodicly, i.e S(u,v) = S(u + T, v) with period T,  then  we
can pull back the curve to the domain covering space (-inf,inf) x dom[1].

*/
class MYON_CLASS MYON_PeriodicDomain
{
public:
	MYON_PeriodicDomain() = default;
	MYON_PeriodicDomain(const MYON_Interval dom[2], const bool closed[2], double normband = 1.0 / 3.0);
	void Initialize(const MYON_Interval dom[2], const bool closed[2], double normband = 1.0 / 3.0);
	/*
	Construction or Initialization
	Parameters:
		dom -[in] surface domain
		closed -[in] closed[0] is true if the surface is closed in u direction (similarly for 1 and v)
						Use  MYON_IsG1Closed(...) to test for G1-closed surfaces.
		normband - [in] 0<normband<.5 is a normalized coordinate defining a band on each side of the seam.
						The bands are {(u,v): dom[0].NormalizedParameterAt(u)< normband } and
													{(u,v): dom[0].NormalizedParameterAt(u)> 1.0 - normband }
	The point sequence crosses the seam if consecutive points are in opposite bands along the seam.
	*/

	// Repeatedly call LiftToCover( Pin) with Pin in the domain covering space.  The resulting 
	// output sequence will be lifted to the domain covering space, and will be 'continuous' in that 
	// consecutive points will be in the same or an adjacent band.
	// Use stealth=true to lift this point without affecting the state,  this allows one to compute a trial end 
	// of sequence.  
	// see also MYON_LiftToCover(...).
	MYON_2dPoint LiftToCover(MYON_2dPoint Pin, bool stealth = false);

	// The projection from covering space back to domain.  LiftInverse(LiftToCover(p))==p
	MYON_2dPoint LiftInverse(MYON_2dPoint p);

	MYON_Interval m_dom[2];
	bool m_closed[2];
	double m_normband;
private:
	int m_deck[2];
	MYON_2dPoint m_nprev = MYON_2dPoint::UnsetPoint;
};


/*
Lift a sequence of surface points to the covering space.

If a surface is closed in the u-direction (or v respectively),  a curve on the surface that crosses the seam
will not have a continuous pull back to parameter space.  However, if we extend the surface domain
in the u-coordinates and allow the surface to cover itself periodiclly  then  we
we can pull back the curve to the covering space (-inf,inf) x dom[1].
Parameters
in - [in] surface parameter points in dom[0] x dom[1]
dom -[in] surface domain
closed -[in] closed[0] is true if the surface is closed in u direction (similarly for 1 and v)
normband - [in] 0<normband<.5 is a normalized coordinate defining a band on each side of the seam.
The point sequence crosses the seam if consecutive points are in opposite bands
along the seam.
Returns
A sequence out with out[0] = in[0] and out.Count()==in.Count()
*/
MYON_DECL
MYON_SimpleArray<MYON_2dPoint> MYON_LiftToCover(
	const MYON_SimpleArray<MYON_2dPoint>& in,
	const MYON_Interval dom[2], bool closed[2],
	double normband = 1.0 / 3.0);

/*
LiftInverse is the projection map that inverts MYON_LiftToCover 
Parameters
	P -[in]  A point in the domain covering space.
Returns a point in dom.  
*/
MYON_2dPoint MYON_DECL MYON_LiftInverse(MYON_2dPoint P, MYON_Interval dom[2], bool closed[2]);

#endif


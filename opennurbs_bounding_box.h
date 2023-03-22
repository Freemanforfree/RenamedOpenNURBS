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

#if !defined(MYON_BOUNDING_BOX_INC_)
#define MYON_BOUNDING_BOX_INC_

////////////////////////////////////////////////////////////////
//
//   MYON_BoundingBox  - axis aligned bounding box
//

class MYON_CLASS MYON_BoundingBox
{
public:
  static const MYON_BoundingBox EmptyBoundingBox; // ((1.0,0.0,0.0),(-1.0,0.0,0.0))
  static const MYON_BoundingBox UnsetBoundingBox; // all coordinates are MYON_UNSET_VALUE
  static const MYON_BoundingBox NanBoundingBox;   // all coordinates are MYON_DBL_QNAN

  MYON_BoundingBox() MYON_NOEXCEPT; // creates EmptyBoundingBox
  ~MYON_BoundingBox() = default;
  MYON_BoundingBox(const MYON_BoundingBox&) = default;
  MYON_BoundingBox& operator=(const MYON_BoundingBox&) = default;

	explicit MYON_BoundingBox(
    const MYON_3dPoint&, // min corner of axis aligned bounding box
    const MYON_3dPoint&  // max corner of axis aligned bounding box
    );


  // OBSOLETE
  // temporary - use MYON_ClippingRegion - this function will be removed soon.
  int IsVisible( 
    const MYON_Xform& bbox2c
    ) const;


  // OBSOLETE
  void Destroy(); // set this = MYON_BoundingBox::EmptyBoundingBox

  // operator[] returns min if index <= 0 and max if index >= 1
  MYON_3dPoint& operator[](int);
  const MYON_3dPoint& operator[](int) const;

  MYON_3dPoint Min() const;
  MYON_3dPoint Max() const;
  MYON_3dVector Diagonal() const; // max corner - min corner
  MYON_3dPoint Center() const;
  MYON_3dPoint Corner( // 8 corners of box
    int, // x_index   0 = Min().x, 1 = Max().x
    int, // y_index   0 = Min().y, 1 = Max().y
    int  // z_index   0 = Min().z, 1 = Max().z
    ) const;
  bool GetCorners( 
    MYON_3dPointArray& box_corners // returns list of 8 corner points
    ) const;
  bool GetCorners( 
    MYON_3dPoint box_corners[8] // returns list of 8 corner points
    ) const;


  /*
  Parameters:
    index - in
      the index of the edge
  Returns:
    Line segments, some possibly a single point.
  */
  MYON_Line Edge(
    unsigned int index  // the index of the requested edge.
  ) const;

  /*
  Parameters:
    edges[] - out
      12 edge lines. If the bounding box has no height, width or depth,
      then the corresponding edges will have the same "from" and "to"
      points.
  Returns:
    If the bounding box is valid, then true is returned and
    12 line segments, some possibly a single point, are returned.
    Otherwise false is returned and 12 line segments with "from"
    and "to" points set to MYON_3dPoint::UnsetPoint are returned.
  */
  bool GetEdges( 
    MYON_Line edges[12]  // returns list of 12 edge segments
    ) const;

  // OBSOLETE IsValid() = IsNotEmpty()
  bool IsValid() const; // empty boxes are not valid

  bool IsSet() const;        // every coordinate is a finite, valid double, not MYON_UNSET_VALUE and not MYON_UNSET_POSITIVE_VALUE
  bool IsUnset() const;      // some coordinate is MYON_UNSET_VALUE or MYON_UNSET_POSITIVE_VALUE
  bool IsNan() const;        // some coordinate is a NAN
  bool IsUnsetOrNan() const; // = IsUnset() or IsNan()

  bool IsEmpty() const;      // (m_min.x > m_max.x || m_min.y > m_max.y || m_min.z > m_max.z) && IsSet();
  bool IsNotEmpty() const;   // (m_min.x <= m_max.x && m_min.y <= m_max.y && m_min.z <= m_max.z) && IsSet()
  bool IsPoint() const;      // (m_min.x == m_max.x && m_min.y == m_max.y && m_min.z == m_max.z) && IsSet()
  
  void Dump(class MYON_TextLog&) const;
  
  /*
  Description:
    Test a bounding box to see if it is degenerate (flat)
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


  //////////
  // MYON_BoundingBox::Transform() updates the bounding box
  // to be the smallest axis aligned bounding box that contains
  // the transform of the eight corner points of the input
  // bounding box.
  bool Transform( const MYON_Xform& );

  double Tolerance() const; // rough guess at a tolerance to use for comparing
                            // objects in this bounding box


  // All of these Set() functions set or expand a box to enclose the points in the arguments
  // If bGrowBox is true, the existing box is expanded, otherwise it is only set to the current point list
  bool Set(     
    int dim,
    bool is_rat,
    int count,
    int stride,
    const double* point_array,
    int bGrowBox = false
    );

  bool Set(
    const MYON_3dPoint& point,
    int bGrowBox = false
    );

  bool Set(
    const MYON_2dPoint& point,
    int bGrowBox = false
    );

  bool Set(     
    const MYON_SimpleArray<MYON_4dPoint>& point_array,
    int bGrowBox = false
    );

  bool Set(     
    const MYON_SimpleArray<MYON_3dPoint>& point_array,
    int bGrowBox = false
    );

  bool Set(     
    const MYON_SimpleArray<MYON_2dPoint>& point_array,
    int bGrowBox = false
    );

  bool Set(     
    int dim,
    bool is_rat,
    int count,
    int stride,
    const float* point_array,
    int bGrowBox = false
    );

  bool Set(
    const MYON_3fPoint& point,
    int bGrowBox = false
    );

  bool Set(
    const MYON_2fPoint& point,
    int bGrowBox = false
    );

  bool Set(     
    const MYON_SimpleArray<MYON_4fPoint>& point_array,
    int bGrowBox = false
    );

  bool Set(     
    const MYON_SimpleArray<MYON_3fPoint>& point_array,
    int bGrowBox = false
    );

  bool Set(     
    const MYON_SimpleArray<MYON_2fPoint>& point_array,
    int bGrowBox = false
    );

	bool IsPointIn(
         const MYON_3dPoint& test_point, // point to test
         int bStrictlyIn = false 
                   // true to test for strict ( min < point < max )
                   // false to test for (min <= point <= max)
                   //       
         ) const;

  //////////
  // Point on or in the box that is closest to test_point.
  // If test_point is in or on the box, the test_point is returned.
  MYON_3dPoint ClosestPoint( 
    const MYON_3dPoint& test_point
    ) const;


  /*
  Description:
    Quickly find a lower bound on the distance 
    between the point and this bounding box.
  Parameters:
    P - [in]
  Returns:
    A distance that is less than or equal to the shortest
    distance from the line to this bounding box.
    Put another way, if Q is any point in this bounding box,
    then P.DistanceTo(Q) >= MinimumDistanceTo(bbox).
  */
  double MinimumDistanceTo( const MYON_3dPoint& P ) const;

  /*
  Description:
    Quickly find an upper bound on the distance 
    between the point and this bounding box.
  Parameters:
    P - [in]
  Returns:
    A distance that is greater than or equal to the 
    longest distance from the point P to this bounding box.
    Put another way, if Q is any point in this bounding box,
    then P.DistanceTo(Q) <= MaximumDistanceTo(bbox).
  */
  double MaximumDistanceTo( const MYON_3dPoint& P ) const;


  /*
  Description:
    Quickly find a lower bound on the distance 
    between this and the other bounding box.
  Parameters:
    other - [in]
  Returns:
    A distance that is less than or equal to the shortest
    distance between the bounding boxes.
    Put another way, if Q is any point in this bounding box
    and P is any point in the other bounding box,
    then P.DistanceTo(Q) >= MinimumDistanceTo(bbox).
  */
  double MinimumDistanceTo( const MYON_BoundingBox& other ) const;

  /*
  Description:
    Quickly find an upper bound on the distance 
    between this and the other bounding box.
  Parameters:
    other - [in]
  Returns:
    A distance that is greater than or equal to the longest
    distance between the bounding boxes.
    Put another way, if Q is any point in this bounding box
    and P is any point in the other bounding box,
    then P.DistanceTo(Q) <= MaximumDistanceTo(bbox).
  */
  double MaximumDistanceTo( const MYON_BoundingBox& other ) const;

  /*
  Description:
    Quickly find a lower bound on the distance 
    between the line segment and this bounding box.
  Parameters:
    line - [in]
  Returns:
    A distance that is less than or equal to the shortest
    distance from the line to this bounding box.
    Put another way, if Q is any point on line
    and P is any point in this bounding box, then
    P.DistanceTo(Q) >= MinimumDistanceTo(bbox).
  */
  double MinimumDistanceTo( const MYON_Line& line ) const;

  /*
  Description:
    Quickly find a tight lower bound on the distance 
    between the plane and this bounding box.
  Parameters:
    plane - [in]
  Returns:
    The minimum distance between a point on the plane
    and a point on the bounding box.
  See Also:
    MYON_PlaneEquation::MimimumValueAt
    MYON_PlaneEquation::MaximumValueAt
  */
  double MinimumDistanceTo( const MYON_Plane& plane ) const;
  double MinimumDistanceTo( const MYON_PlaneEquation& plane_equation ) const;

  /*
  Description:
    Quickly find an upper bound on the distance 
    between the line segment and this bounding box.
  Parameters:
    line - [in]
  Returns:
    A distance that is greater than or equal to the 
    longest distance from the line to this bounding box.
    Put another way, if Q is any point on the line
    and P is any point in this bounding box, then
    P.DistanceTo(Q) <= MaximumDistanceTo(bbox).
  */
  double MaximumDistanceTo( const MYON_Line& line ) const;

  /*
  Description:
    Quickly find a tight upper bound on the distance 
    between the plane and this bounding box.
  Parameters:
    plane - [in]
  Returns:
    A distance that is equal to the longest distance from
    the plane to this bounding box.  Put another way, 
    if Q is any point on the plane and P is any point 
    in this bounding box, then 
    P.DistanceTo(Q) <= MaximumDistanceTo(bbox) and there
    is at least one point on the bounding box where the
    distance is equal to the returned value.
  See Also:
    MYON_PlaneEquation::MaximumValueAt
  */
  double MaximumDistanceTo( const MYON_Plane& plane ) const;
  double MaximumDistanceTo( const MYON_PlaneEquation& plane_equation ) const;


  /*
  Description:
    Quickly determine if the shortest distance from
    the point P to the bounding box is greater than d.
  Parameters:
    d - [in] distance (> 0.0)
    P - [in] 
  Returns:
    True if if the shortest distance from the point P
    to the bounding box is greater than d. 
  */
  bool IsFartherThan( double d, const MYON_3dPoint& P ) const;

  /*
  Description:
    Quickly determine if the shortest distance from the line
    to the bounding box is greater than d.
  Parameters:
    d - [in] distance (> 0.0)
    line - [in] 
  Returns:
    True if the shortest distance from the line
    to the bounding box is greater than d. It is not the
    case that false means that the shortest distance
    is less than or equal to d.
  */
  bool IsFartherThan( double d, const MYON_Line& line ) const;

  /*
  Description:
    Quickly determine if the shortest distance from the plane
    to the bounding box is greater than d.
  Parameters:
    d - [in] distance (> 0.0)
    plane - [in] 
  Returns:
    True if the shortest distance from the plane
    to the bounding box is greater than d, and false
    if the shortest distance is less than or equal to d.
  */
  bool IsFartherThan( double d, const MYON_Plane& plane ) const;

  /*
  Description:
    Quickly determine if the shortest distance from the plane
    to the bounding box is greater than d.
  Parameters:
    d - [in] distance (> 0.0)
    plane_equation - [in] (the first three coefficients 
                           are assumed to be a unit vector.
                           If not, adjust your d accordingly.)
  Returns:
    True if the shortest distance from the plane
    to the bounding box is greater than d, and false
    if the shortest distance is less than or equal to d.
  */
  bool IsFartherThan( double d, const MYON_PlaneEquation& plane_equation ) const;

  /*
  Description:
    Quickly determine if the shortest distance this bounding
    box to another bounding box is greater than d.
  Parameters:
    d - [in] distance (> 0.0)
    other - [in] other bounding box
  Returns:
    True if if the shortest distance from this bounding
    box to the other bounding box is greater than d. 
  */
  bool IsFartherThan( double d, const MYON_BoundingBox& other ) const;


  // Description:
  //   Get point in a bounding box that is closest to a line
  //   segment.
  // Parameters:
  //   line - [in] line segment
  //   box_point - [out] point in box that is closest to line
  //       segment point at t0.
  //   t0 - [out] parameter of point on line that is closest to
  //       the box.
  //   t1 - [out] parameter of point on line that is closest to
  //       the box.
  // Returns:
  //   3 success - line segments intersects box in a segment
  //               from line(t0) to line(t1) (t0 < t1)
  //   2 success - line segments intersects box in a single point
  //               at line(t0) (t0==t1)
  //   1 success - line segment does not intersect box.  Closest
  //               point on the line is at line(t0) (t0==t1)
  //   0 failure - box is invalid.
  // Remarks:
  //   The box is treated as a solid box.  If the intersection
  //   of the line segment, then 3 is returned.
  int GetClosestPoint( 
    const MYON_Line&, // line
    MYON_3dPoint&,    // box_point
    double*,        // t0
    double*         // t1
    ) const;

  //////////
  // Get points on bounding boxes that are closest to each other.
  // If the boxes intersect, then the point at the centroid of the
  // intersection is returned for both points.
  bool GetClosestPoint( 
         const MYON_BoundingBox&, // "other" bounding box
         MYON_3dPoint&, // point on "this" box that is closest to "other" box
         MYON_3dPoint&  // point on "other" box that is closest to "this" box
         )  const;

  //////////
  // Point on the box that is farthest from the test_point.
  MYON_3dPoint FarPoint( 
    const MYON_3dPoint& // test_point
    ) const;

  //////////
  // Get points on bounding boxes that are farthest from each other.
  bool GetFarPoint( 
         const MYON_BoundingBox&, // "other" bounding box
         MYON_3dPoint&, // point on "this" box that is farthest from "other" box
         MYON_3dPoint&  // point on "other" box that is farthest from "this" box
         )  const;

  /* 
  Description:
    Intersect this with other_bbox and save intersection in this.
  Parameters:
    other_bbox - [in]
  Returns:
    True if this-intesect-other_bbox is a non-empty valid bounding box
    and this is set.  False if the intersection is empty, in which case
    "this" is set to an invalid bounding box.
  Remarks:
    If "this" or other_bbox is invalid, they are treated as
    the empty set, and false is returned.
  */
  bool Intersection(
         const MYON_BoundingBox& other_bbox
         );

  /* 
  Description:
    Set "this" to the intersection of bbox_A and bbox_B.
  Parameters:
    bbox_A - [in] 
    bbox_B - [in]
  Returns:
    True if the "this" is a non-empty valid bounding box.
    False if the intersection is empty, in which case
    "this" is set to an invalid bounding box.
  Remarks:
    If bbox_A or bbox_B is invalid, they are treated as
    the empty set, and false is returned.
  */
  bool Intersection( // this = intersection of two args
         const MYON_BoundingBox& bbox_A, 
         const MYON_BoundingBox& bbox_B
         );

	bool Intersection(				//Returns true when intersect is non-empty. 
				 const MYON_Line&,		//Infinite Line segment to intersect with 
				 double* =nullptr ,			// t0  parameter of first intersection point
				 double* =nullptr       // t1  parameter of last intersection point (t0<=t1)   
				 ) const;			 

  /* 
  Description:
    Test a box to see if it is contained in this box.
  Parameters:
    other - [in] box to test
    bProperSubSet - [in] if true, then the test is for a proper inclusion.
  Returns:
    If bProperSubSet is false, then the result is true when
			this->m_min[i] <= other.m_min[i] and other.m_max[i] <= this->m_max[i].
			for i=0,1 and 2.
    If bProperSubSet is true, then the result is true when
			the above condition is true and at least one of the inequalities is strict.
  */
  bool Includes( 
    const MYON_BoundingBox& other,
    bool bProperSubSet = false
    ) const;

	double Volume() const;

  double Area() const;

  // Union() returns true if union is not empty.
  // Invalid boxes are treated as the empty set.
  bool Union( // this = this union arg
         const MYON_BoundingBox&
         );

  bool Union( // this = union of two args
         const MYON_BoundingBox&, 
         const MYON_BoundingBox&
         );
                 
  /* 
  Description:
    Test to see if "this" and other_bbox are disjoint (do not intersect).
  Parameters:
    other_bbox - [in]
  Returns:
    True if "this" and other_bbox are disjoint.
  Remarks:
    If "this" or other_bbox is invalid, then true is returned.
  */
  bool IsDisjoint(
    const MYON_BoundingBox& other_bbox
    ) const;

  /*
  Description:
    Test to see if "this" and line are disjoint (do not intersect or line is included).
  Parameters:
    line - [in]
    infinite - [in] if false or not provided, then the line is considered bounded by start and end points.
  Returns:
    True if "this" and line are disjoint.
  */
  bool IsDisjoint(const MYON_Line& line) const;
  bool IsDisjoint(const MYON_Line& line, bool infinite) const;

  bool SwapCoordinates( int, int );

 /*
 Description:
   Expand the box by adding delta to m_max and subtracting
   it from m_min.  So, when delta is positive and the interval is 
   increasing this function expands the box on each side.
 Returns:
   true if the result is Valid.  
 */
  bool Expand(MYON_3dVector delta);

  /*
Description:
  Shrinks the box by subtracting delta to m_max and adding
  it from m_min.  This is not allowed to create an inverse bounding box.
Returns:
  true if the result is Valid.
*/
  bool Shrink(MYON_3dVector delta);

  MYON_3dPoint m_min;
  MYON_3dPoint m_max;
};

/*
Returns:
  True if lhs and rhs are identical.
*/
MYON_DECL
bool operator==( const MYON_BoundingBox& lhs, const MYON_BoundingBox& rhs );

/*
Returns:
  True if lhs and rhs are not equal.
*/
MYON_DECL
bool operator!=( const MYON_BoundingBox& lhs, const MYON_BoundingBox& rhs );

class MYON_CLASS MYON_BoundingBoxAndHash
{
public:
  MYON_BoundingBoxAndHash() = default;
  ~MYON_BoundingBoxAndHash() = default;
  MYON_BoundingBoxAndHash(const MYON_BoundingBoxAndHash&) = default;
  MYON_BoundingBoxAndHash& operator=(const MYON_BoundingBoxAndHash&) = default;

public:
  // This hash depends on the context and is a hash
  // of the information used to calculte the bounding box.
  // It is not the hash of the box values

  void Set(
    const MYON_BoundingBox& bbox,
    const MYON_SHA1_Hash& hash
  );

  const MYON_BoundingBox& BoundingBox() const;

  const MYON_SHA1_Hash& Hash() const;

  /*
  Returns:
   True if bounding box IsSet() is true and hash is not EmptyContentHash.
  */
  bool IsSet() const;

  bool Write(
    class MYON_BinaryArchive& archive
  ) const;

  bool Read(
    class MYON_BinaryArchive& archive
  );

 private:
  MYON_BoundingBox m_bbox = MYON_BoundingBox::UnsetBoundingBox;
  MYON_SHA1_Hash m_hash = MYON_SHA1_Hash::EmptyContentHash;
};

/*
A class that caches 8 bounding box - hash pairs and keeps the most frequently
used bounding boxes.
*/
class MYON_CLASS MYON_BoundingBoxCache
{
public:
  MYON_BoundingBoxCache() = default;
  ~MYON_BoundingBoxCache() = default;
  MYON_BoundingBoxCache(const MYON_BoundingBoxCache&) = default;
  MYON_BoundingBoxCache& operator=(const MYON_BoundingBoxCache&) = default;

public:
  /*
  Description:
    Add a bounding box that can be found from a hash value.
  Parameters:
    bbox - [in]
    hash - [in]
      A hash of the information needed to create this bounding box.
  */
  void AddBoundingBox(
    const MYON_BoundingBox& bbox,
    const MYON_SHA1_Hash& hash
  );

  void AddBoundingBox(
    const MYON_BoundingBoxAndHash& bbox_and_hash
  );

  /*
  Description:
    Get a cached bounding box.
  Parameters:
    hash - [in]
    bbox - [out]
      If the hash identifies a bounding box in the cache, then
      that bounding box is returned. Otherwise MYON_BoundingBox::NanBoundingBox
      is returned.
  Returns:
    true - cached bounding box returned
    false - bounding box not in cache.
  */
  bool GetBoundingBox(
    const MYON_SHA1_Hash& hash,
    MYON_BoundingBox& bbox
  ) const;

  /*
  Description:
    Remove a bounding box that can be found from a hash value.
  Parameters:
    hash - [in]
  Returns:
    true - hash was in the cache and removed.
    false - hash was not in the cache.
  Remarks:
    If the hash values you are using are correctly computed and include
    all information that the bounding box depends on, then
    you never need to remove bounding boxes. Unused ones will get
    removed as new ones are added.
  */
  bool RemoveBoundingBox(
    const MYON_SHA1_Hash& hash
  );

  /*
  Description:
    Removes all bounding boxes.
  Remarks:
    If the hash values you are using are correctly computed and include
    all information that the bounding box depends on, then
    you never need to remove bounding boxes. Unused ones will get
    removed as new ones are added.
    If the hash does not include all information required to compute
    the bounding boxes, then call RemoveAllBoundingBoxes() when the
    non-hashed information changes.
  */
  void RemoveAllBoundingBoxes();

  /*
  Returns:
    Number of cached boxes.
  */
  unsigned int BoundingBoxCount() const;

  bool Write(
    class MYON_BinaryArchive& archive
  ) const;

  bool Read(
    class MYON_BinaryArchive& archive
  );

private:
  // number of boxes set in m_cache[]
  unsigned int m_count = 0; 

  // capacity of m_cache[] - set when needed
  unsigned int m_capacity = 0;

  // Bounding box cache. Most recently used boxes are first.
  mutable MYON_BoundingBoxAndHash m_cache[8];

  /*
  Returns:
    m_cache[] array index of box with the hash.
    MYON_UNSET_UINT_INDEX if hash is not present in m_cache[] array.
  */
  unsigned int Internal_CacheIndex(const MYON_SHA1_Hash& hash) const;
};

#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_BoundingBox>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_BoundingBoxAndHash>;

#endif

/*
Description:
  Get a tight bounding box that contains the points.
Parameters:
 dim - [in] (>=1)
 is_rat - [in] true if points are rational
 count - [in] number of points
 stride - [in] stride between points
 point_list - [in]
 bbox - [in/out]
 bGrowBox - [in] (default = false)
   If the input bbox is valid and bGrowBox is true,
   then the output bbox is the union of the input
   bbox and the bounding box of the point list.
 xform - [in] (default = nullptr)
   If not null, the bounding box of the transformed
   points is calculated.  The points are not modified.
Returns:
  True if the output bbox is valid.
*/
MYON_DECL
bool MYON_GetPointListBoundingBox(
    int dim,
    bool is_rat,
    int count,
    int stride,
    const double* point_list,
    MYON_BoundingBox& bbox,
    int bGrowBox = false,
    const MYON_Xform* xform = 0
    );

MYON_DECL
bool MYON_GetPointListBoundingBox(
    int dim,
    bool is_rat,
    int count,
    int stride,
    const float* point_list,
    MYON_BoundingBox& bbox,
    int bGrowBox = false,
    const MYON_Xform* xform = 0
    );

MYON_DECL
bool MYON_GetPointListBoundingBox(
    int dim,
    bool is_rat,
    int count,
    int stride,
    const double* point_list,
    double* boxmin,       // min[dim]
    double* boxmax,       // max[dim]
    int bGrowBox
    );

MYON_DECL
MYON_BoundingBox MYON_PointListBoundingBox(
    int dim,
    bool is_rat,
    int count,
    int stride,
    const double* point_list
    );

MYON_DECL
bool MYON_GetPointListBoundingBox(
    int dim,
    bool is_rat,
    int count,
    int stride,
    const float* point_list,
    float* boxmin,       // min[dim]
    float* boxmax,       // max[dim]
    int bGrowBox
    );

MYON_DECL
MYON_BoundingBox MYON_PointListBoundingBox( // low level workhorse function
    int dim,
    bool is_rat,
    int count,
    int stride,
    const float* point_list
    );

MYON_DECL
bool MYON_GetPointGridBoundingBox(
        int dim,
        bool is_rat,
        int point_count0, int point_count1,
        int point_stride0, int point_stride1,
        const double* point_grid,
        double* boxmin,       // min[dim]
        double* boxmax,       // max[dim]
        int bGrowBox
    );

MYON_DECL
MYON_BoundingBox MYON_PointGridBoundingBox(
        int dim,
        bool is_rat,
        int point_count0, int point_count1,
        int point_stride0, int point_stride1,
        const double* point_grid
    );

MYON_DECL
double MYON_BoundingBoxTolerance(
        int dim,
        const double* bboxmin,
        const double* bboxmax
        );

/*
Description:
  Determine if an object is too large or too far 
  from the origin for single precision coordinates
  to be useful.
Parameters:
  bbox - [in]
    Bounding box of an object with single precision
    coordinates.  An MYON_Mesh is an example of an
    object with single precision coordinates.
  xform - [out]
    If this function returns false and xform is not
    null, then the identity transform is returned.
    If this function returns true and xform is not
    null, then the transform moves the region
    contained in bbox to a location where single 
    precision coordinates will have enough
    information for the object to be useful.
Returns:
  true:
    The region contained in bbox is too large
    or too far from the origin for single 
    precision coordinates to be useful.
  false:
    A single precision object contained in bbox
    will be satisfactory for common calculations.
*/
MYON_DECL
bool MYON_BeyondSinglePrecision( const MYON_BoundingBox& bbox, MYON_Xform* xform );

MYON_DECL
bool MYON_WorldBBoxIsInTightBBox( 
          const MYON_BoundingBox& tight_bbox, 
          const MYON_BoundingBox& world_bbox,
          const MYON_Xform* xform
          );

#endif

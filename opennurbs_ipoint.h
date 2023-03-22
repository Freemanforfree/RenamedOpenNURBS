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


#if !defined(OPENNURBS_IPOINT_INC_)
#define OPENNURBS_IPOINT_INC_

/*
A 2 dimensional point with integer coordinates.
Clear code will distinguish between situation where (x,y) is a 
location (MYON_2iPoint) or a direction (MYON_2iVector) and use 
the appropriate class.
*/
class MYON_CLASS MYON_2iPoint
{
public:
  // Default construction intentionally leaves x and y uninitialized.
  // Use something like
  //   MYON_2iPoint pt(1,2);
  // or 
  //  MYON_2iPoint pt = MYON_2iPoint::Origin;
  // when you need an initialized MYON_2iPoint.
  MYON_2iPoint() = default;

  ~MYON_2iPoint() = default;
  MYON_2iPoint(const MYON_2iPoint& ) = default;
  MYON_2iPoint& operator=(const MYON_2iPoint& ) = default;

  MYON_2iPoint(
    int x,
    int y
  );

public:
  static const MYON_2iPoint Origin; // (0,0)
  static const MYON_2iPoint Unset;  // (MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX)

  /*
  Dictionary order compare.
  */
  static int Compare(
    const MYON_2iPoint& lhs,
    const MYON_2iPoint& rhs
  );

public:
  MYON_2iPoint& operator+=(const class MYON_2iVector&);
  MYON_2iPoint& operator-=(const class MYON_2iVector&);

  // It is intentional that points are not added to points to encourage
  // code that is clear about what is a location and what is displacement.

public:
  /*
  For those times when a location was incorrectly represented by a vector.
  It is intentional that ther is not an MYON_2iPoint constructor from an MYON_2iVector.
  */
  static const MYON_2iPoint FromVector(const class MYON_2iVector& v);

  static const MYON_2iPoint From2dex(const class MYON_2dex& src);

public:
  /*
  Returns:
    (0 == x && 0 == y)
  */
  bool IsOrigin() const;

  /*
  Returns:
    (MYON_UNSET_INT_INDEX == x || MYON_UNSET_INT_INDEX ==y)
  */
  bool IsSet() const;

public:
  MYON__INT32 x;
  MYON__INT32 y;
};

MYON_DECL
bool operator==(const MYON_2iPoint&, const MYON_2iPoint&);

MYON_DECL
bool operator!=(const MYON_2iPoint&, const MYON_2iPoint&);


/*
A 2 dimensional vector with integer coordinates.
Clear code will distinguish between situation where (x,y) is a 
location (MYON_2iPoint) or a direction (MYON_2iVector) and use 
the appropriate class.
*/
class MYON_CLASS MYON_2iVector
{
public:
  // Default construction intentionally leaves x and y uninitialized.
  // Use something like
  //   MYON_2iVector pt(1,2);
  // or 
  //  MYON_2iVector pt = MYON_2iVector::Zero;
  // when you need an initialized MYON_2iVector.
  MYON_2iVector() = default;

  ~MYON_2iVector() = default;
  MYON_2iVector(const MYON_2iVector& ) = default;
  MYON_2iVector& operator=(const MYON_2iVector& ) = default;

  MYON_2iVector(
    int x,
    int y
  );
  
  /*
  For those times when a direction was incorrectly represented by a point.
  It is intentional that ther is not an MYON_2iVector constructor from an MYON_2iPoint.
  */
  static const MYON_2iVector FromPoint(const class MYON_2iPoint& p);

  static const MYON_2iVector From2dex(const class MYON_2dex& src);

public:
  static const MYON_2iVector Zero; // (0,0)
  static const MYON_2iVector UnitX; // (1,0)
  static const MYON_2iVector UnitY; // (0,1)
  static const MYON_2iVector Unset;  // (MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX)
  
  /*
  Dictionary order compare.
  */
  static int Compare(
    const MYON_2iVector& lhs,
    const MYON_2iVector& rhs
  );

public:
  MYON_2iVector& operator+=(const class MYON_2iVector&);
  MYON_2iVector& operator-=(const class MYON_2iVector&);
  MYON_2iVector& operator*=(int);

  MYON_2iVector operator-() const;
  
public:
  /*
  Returns:
    (0 == x && 0 == y)
  */
  bool IsZero() const;

  /*
  Returns:
    IsSet() && (0 != x || 0 != y)
  */
  bool IsNotZero() const;

  /*
  Returns:
    (MYON_UNSET_INT_INDEX == x || MYON_UNSET_INT_INDEX ==y)
  */
  bool IsSet() const;

public:
  MYON__INT32 x;
  MYON__INT32 y;
};

MYON_DECL
bool operator==(const MYON_2iVector&, const MYON_2iVector&);

MYON_DECL
bool operator!=(const MYON_2iVector&, const MYON_2iVector&);

MYON_DECL
MYON_2iPoint operator+(const MYON_2iPoint&, const MYON_2iVector&);

MYON_DECL
MYON_2iPoint operator-(const MYON_2iPoint&, const MYON_2iVector&);

MYON_DECL
MYON_2iVector operator+(const MYON_2iVector&, const MYON_2iVector&);

MYON_DECL
MYON_2iVector operator-(const MYON_2iVector&, const MYON_2iVector&);

MYON_DECL
MYON_2iVector operator*(int, const MYON_2iVector&);

class MYON_CLASS MYON_2iBoundingBox
{
public:
  // Default construction intentionally leaves m_min and m_max uninitialized.
  // Use something like
  //   MYON_2iBoundingBox bbox(min_pt,max_pt);
  // or 
  //  MYON_2iBoundingBox bbox = MYON_2iBoundingBox::Unset;
  MYON_2iBoundingBox() = default;

  ~MYON_2iBoundingBox() = default;
  MYON_2iBoundingBox(const MYON_2iBoundingBox& ) = default;
  MYON_2iBoundingBox& operator=(const MYON_2iBoundingBox& ) = default;

  MYON_2iBoundingBox(
    const class MYON_2iPoint bbox_min,
    const class MYON_2iPoint bbox_max
  );

public:
  static const MYON_2iBoundingBox Zero; // (MYON_2iPoint::Origin,MYON_2iPoint::Origin);
  static const MYON_2iBoundingBox Unset;  // (MYON_2iPoint::Unset,MYON_2iPoint::Unset)
  
public:
  /*
  Returns:
    m_min.IsSet() && m_max.IsSet() && m_min.x <= m_max.x && m_min.y <= m_max.y.
  */
  bool IsSet() const;

  const MYON_2iPoint Min() const;
  const MYON_2iPoint Max() const;

public:
  MYON_2iPoint m_min;
  MYON_2iPoint m_max;
};

MYON_DECL
bool operator==(const MYON_2iBoundingBox&, const MYON_2iBoundingBox&);

MYON_DECL
bool operator!=(const MYON_2iBoundingBox&, const MYON_2iBoundingBox&);

/*
Class MYON_2iSize
  For those situations where a Windows SDK SIZE or MFC CSize 
  value needs to be used in code that does not link with MFC.
*/
class MYON_CLASS MYON_2iSize
{
public:
  // Default construction intentionally leaves x and y uninitialized.
  // Use something like
  //   MYON_2iSize pt(1,2);
  // or 
  //  MYON_2iSize pt = MYON_2iSize::Zero;
  // when you need an initialized MYON_2iSize.
  MYON_2iSize() = default;

  ~MYON_2iSize() = default;
  MYON_2iSize(const MYON_2iSize& ) = default;
  MYON_2iSize& operator=(const MYON_2iSize& ) = default;

  MYON_2iSize(
    int cx,
    int cy
  );

  /*
  Dictionary compare.
  Returns: 
  -1: lhs < rhs
   0: lsh == rsh
  +1: lhs > rhs
  */
  static int Compare(
    const MYON_2iSize& lhs,
    const MYON_2iSize& rhs
  );

  /*
  Dictionary compare.
  Returns: 
  -1: lhs < rhs
   0: lsh == rsh
  +1: lhs > rhs
  */
  static int ComparePointer(
    const MYON_2iSize* lhs,
    const MYON_2iSize* rhs
  );

public:
  static const MYON_2iSize Zero;  // (0,0)
  static const MYON_2iSize Unset; // (MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX)
  
public:
  /*
  Returns:
    true if both cx and cy are 0.
  */
  bool IsZero() const;

  /*
  Returns:
    true if neither cx nor cy are MYON_UNSET_INT_INDEX.
  */
  bool IsSet() const;
  
public:
  MYON__INT32 cx;
  MYON__INT32 cy;
};

MYON_DECL
bool operator==(
  const MYON_2iSize& lhs,
  const MYON_2iSize& rhs
  );

MYON_DECL
bool operator!=(
  const MYON_2iSize& lhs,
  const MYON_2iSize& rhs
  );

#if defined(MYON_DLL_TEMPLATE)

MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_2iSize>;

#endif

/*
Class MYON_4iRect
  For those situations where a Windows SDK RECT or MFC CRect 
  value needs to be used in code that does not link with MFC.
  If you want a traditional bounding box, use MYON_2dBoundingBox.
*/
class MYON_CLASS MYON_4iRect
{
public:
  // Default construction intentionally leaves x and y uninitialized.
  // Use something like
  //   MYON_4iRect pt(1,2,3,4);
  // or 
  //  MYON_4iRect pt = MYON_4iRect::Zero;
  // when you need an initialized MYON_4iRect.
  MYON_4iRect() = default;

  ~MYON_4iRect() = default;
  MYON_4iRect(const MYON_4iRect& ) = default;
  MYON_4iRect& operator=(const MYON_4iRect& ) = default;

  MYON_4iRect(
  int left,
  int top,
  int right,
  int bottom
  );

  MYON_4iRect(const MYON_2iPoint topLeft, const MYON_2iPoint& bottomRight);
  MYON_4iRect(const MYON_2iPoint& point, const MYON_2iSize& size);

public:
  static const MYON_4iRect Zero;  // (0,0,0,0)
  static const MYON_4iRect Unset; // (MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX,MYON_UNSET_INT_INDEX)

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

  int Width(void) const;
  int Height(void) const;

  const MYON_2iSize Size(void) const;
  
  const MYON_2iPoint CenterPoint(void) const;
  const MYON_2iPoint TopLeft(void) const;
  const MYON_2iPoint BottomRight(void) const;

  bool  IntersectRect(const MYON_4iRect* r1, const MYON_4iRect* r2);
  bool  IntersectRect(const MYON_4iRect& r1, const MYON_4iRect& r2);

  bool IsRectEmpty(void) const;
  bool IsRectNull(void) const;
  void SetRectEmpty(void) { *this = Zero; }
  void SetRect(int l, int t, int r, int b);

  bool PtInRect(const MYON_2iPoint& pt) const;

  void OffsetRect(int, int);
  void OffsetRect(const MYON_2iVector&);
  void InflateRect(int, int);
  void InflateRect(int, int, int, int);
  void DeflateRect(int, int);
  bool SubtractRect(const MYON_4iRect* rect1, const MYON_4iRect* rect2);

  void NormalizeRect();

public:
  // NOTE WELL:
  // Windows 2d integer device coordinates have a 
  // strong y-down bias and it is common for top < bottom.
  // General 2d bounding boxes have a strong lower < upper / min < max bias.
  // Take care when converting between MYON_2iBoundingBox and MYON_4iRect.
  // It is intentional that no automatic conversion between bounding box
  // and MYON_4iRect is supplied because each case must be carefully considered.
  MYON__INT32 left;
  MYON__INT32 top;
  MYON__INT32 right;
  MYON__INT32 bottom;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_4iRect>;
#endif


MYON_DECL
bool operator==(const MYON_4iRect&, const MYON_4iRect&);

MYON_DECL
bool operator!=(const MYON_4iRect&, const MYON_4iRect&);

#endif

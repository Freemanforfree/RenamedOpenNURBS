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

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

// MYON_2iPoint
MYON_2iPoint::MYON_2iPoint(int xValue, int yValue)
  : x(xValue)
  , y(yValue)
{}

const MYON_2iPoint MYON_2iPoint::From2dex(const class MYON_2dex& src)
{
  return MYON_2iPoint(src.i, src.j);
}

bool MYON_2iPoint::IsSet() const
{
  return (MYON_UNSET_INT_INDEX != x && MYON_UNSET_INT_INDEX != y );
}

bool MYON_2iPoint::IsOrigin() const
{
  return (0 == x && 0 == y);
}

MYON_2iPoint& MYON_2iPoint::operator+=(const class MYON_2iVector& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

MYON_2iPoint& MYON_2iPoint::operator-=(const class MYON_2iVector& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

bool operator==(const MYON_2iPoint& lhs, const MYON_2iPoint& rhs)
{
  return (lhs.x == rhs.x && lhs.y == rhs.y);
}

bool operator!=(const MYON_2iPoint& lhs, const MYON_2iPoint& rhs)
{
  return (lhs.x != rhs.x || lhs.y != rhs.y);
}

int MYON_2iPoint::Compare(
  const MYON_2iPoint& lhs,
  const MYON_2iPoint& rhs
)
{
  if (lhs.x < rhs.x)
    return -1;
  if (lhs.x > rhs.x)
    return 1;
  if (lhs.y < rhs.y)
    return -1;
  if (lhs.y > rhs.y)
    return 1;
  return 0;
}

const MYON_2iPoint MYON_2iPoint::FromVector(const class MYON_2iVector& v)
{
  return MYON_2iPoint(v.x, v.y);
}

// MYON_2iVector
MYON_2iVector::MYON_2iVector(int xValue, int yValue)
  : x(xValue)
  , y(yValue)
{}

const MYON_2iVector MYON_2iVector::From2dex(const class MYON_2dex& src)
{
  return MYON_2iVector(src.i, src.j);
}

bool MYON_2iVector::IsSet() const
{
  return (MYON_UNSET_INT_INDEX != x && MYON_UNSET_INT_INDEX != y );
}

bool MYON_2iVector::IsZero() const
{
  return (0 == x && 0 == y);
}

bool MYON_2iVector::IsNotZero() const
{
  return ((0 != x || 0 != y) && IsSet());
}


MYON_2iVector& MYON_2iVector::operator+=(const class MYON_2iVector& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

MYON_2iVector& MYON_2iVector::operator-=(const class MYON_2iVector& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

MYON_2iVector& MYON_2iVector::operator*=(int s)
{
  x *= s;
  y *= s;
  return *this;
}

MYON_2iVector MYON_2iVector::operator-() const
{
	return MYON_2iVector(-x, -y);
}

bool operator==(const MYON_2iVector& lhs, const MYON_2iVector& rhs)
{
  return (lhs.x == rhs.x && lhs.y == rhs.y);
}

bool operator!=(const MYON_2iVector& lhs, const MYON_2iVector& rhs)
{
  return (lhs.x != rhs.x || lhs.y != rhs.y);
}

int MYON_2iVector::Compare(
  const MYON_2iVector& lhs,
  const MYON_2iVector& rhs
)
{
  if (lhs.x < rhs.x)
    return -1;
  if (lhs.x > rhs.x)
    return 1;
  if (lhs.y < rhs.y)
    return -1;
  if (lhs.y > rhs.y)
    return 1;
  return 0;
}

const MYON_2iVector MYON_2iVector::FromPoint(const class MYON_2iPoint& p)
{
  return MYON_2iVector(p.x, p.y);
}

//////////////////////////////////////////////////////

MYON_2iPoint operator+(const MYON_2iPoint& lhs, const MYON_2iVector& rhs)
{
  MYON_2iPoint p(lhs.x + rhs.x, lhs.y + rhs.y);
  return p;
}

MYON_2iPoint operator-(const MYON_2iPoint& lhs, const MYON_2iVector& rhs)
{
  MYON_2iPoint p(lhs.x - rhs.x, lhs.y - rhs.y);
  return p;
}

MYON_2iVector operator+(const MYON_2iVector& lhs, const MYON_2iVector& rhs)
{
  MYON_2iVector v(lhs.x + rhs.x, lhs.y + rhs.y);
  return v;
}

MYON_2iVector operator-(const MYON_2iVector& lhs, const MYON_2iVector& rhs)
{
  MYON_2iVector v(lhs.x - rhs.x, lhs.y - rhs.y);
  return v;
}

MYON_2iVector operator*(int lhs, const MYON_2iVector& rhs)
{
  MYON_2iVector v(lhs*rhs.x, lhs*rhs.y);
  return v;
}

// MYON_2iBoundingBox
MYON_2iBoundingBox::MYON_2iBoundingBox(
  const class MYON_2iPoint bbox_min,
  const class MYON_2iPoint bbox_max
)
  : m_min(bbox_min)
  , m_max(bbox_max)
{}

bool MYON_2iBoundingBox::IsSet() const
{
  return (
    m_min.x <= m_max.x
    && m_min.y <= m_max.y
    && m_min.IsSet()
    && m_max.IsSet()
    );
}

const MYON_2iPoint MYON_2iBoundingBox::Min() const
{
  return m_min;
}

const MYON_2iPoint MYON_2iBoundingBox::Max() const
{
  return m_max;
}

bool operator==(const MYON_2iBoundingBox& lhs, const MYON_2iBoundingBox& rhs)
{
  return (lhs.m_min == rhs.m_min && lhs.m_max == rhs.m_max);
}

bool operator!=(const MYON_2iBoundingBox& lhs, const MYON_2iBoundingBox& rhs)
{
  return (lhs.m_min != rhs.m_min || lhs.m_max != rhs.m_max);
}

// MYON_2iSize

int MYON_2iSize::Compare(
  const MYON_2iSize& lhs,
  const MYON_2iSize& rhs
)
{
  if (lhs.cx < rhs.cx)
    return -1;
  if (lhs.cx > rhs.cx)
    return 1;
  if (lhs.cy < rhs.cy)
    return -1;
  if (lhs.cy > rhs.cy)
    return 1;
  return 0;
}

int MYON_2iSize::ComparePointer(
  const MYON_2iSize* lhs,
  const MYON_2iSize* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return -1;
  if (nullptr == rhs)
    return 1;
  if (lhs->cx < rhs->cx)
    return -1;
  if (lhs->cx > rhs->cx)
    return 1;
  if (lhs->cy < rhs->cy)
    return -1;
  if (lhs->cy > rhs->cy)
    return 1;
  return 0;
}

MYON_2iSize::MYON_2iSize(int cxValue, int cyValue)
  : cx(cxValue)
  , cy(cyValue)
{}

bool MYON_2iSize::IsSet() const
{
  return (MYON_UNSET_INT_INDEX != cx && MYON_UNSET_INT_INDEX != cy);
}

bool MYON_2iSize::IsZero() const
{
  return (0 == cx && 0 == cy);
}

bool operator==(
  const MYON_2iSize& lhs,
  const MYON_2iSize& rhs
  )
{
  return (lhs.cx == rhs.cx && lhs.cy == rhs.cy);
}

bool operator!=(const MYON_2iSize& lhs, const MYON_2iSize& rhs)
{
  return (lhs.cx != rhs.cx || lhs.cy != rhs.cy);
}

// MYON_4iRect
MYON_4iRect::MYON_4iRect(int leftValue, int topValue, int rightValue, int bottomValue)
  : left(leftValue)
  , top(topValue)
  , right(rightValue)
  , bottom(bottomValue)
{}

MYON_4iRect::MYON_4iRect(const MYON_2iPoint topLeft, const MYON_2iPoint& bottomRight)
	: left(topLeft.x)
	, top(topLeft.y)
	, right(bottomRight.x)
	, bottom(bottomRight.y)
{}

MYON_4iRect::MYON_4iRect(const MYON_2iPoint& point, const MYON_2iSize& size)
{
	left = point.x;
	top = point.y;
	right = left + size.cx;
	bottom = top + size.cy;
}

bool MYON_4iRect::IsSet() const
{
  return (
    MYON_UNSET_INT_INDEX != left 
    && MYON_UNSET_INT_INDEX != top
    && MYON_UNSET_INT_INDEX != right
    && MYON_UNSET_INT_INDEX != bottom
    );
}

int MYON_4iRect::Width(void) const { return std::abs(right - left); }

int MYON_4iRect::Height(void) const { return std::abs(bottom - top); }

const MYON_2iSize MYON_4iRect::Size(void) const { return MYON_2iSize(Width(), Height()); }

const MYON_2iPoint MYON_4iRect::CenterPoint(void) const { return MYON_2iPoint((left + right) / 2, (top + bottom) / 2); }

const MYON_2iPoint MYON_4iRect::TopLeft(void) const { return MYON_2iPoint(left, top); }

const MYON_2iPoint MYON_4iRect::BottomRight(void) const { return MYON_2iPoint(right, bottom); }

bool MYON_4iRect::IntersectRect(const MYON_4iRect * r1, const MYON_4iRect * r2)
{
  left = MYON_Max(r1->left, r2->left);
  right = MYON_Min(r1->right, r2->right);
  if (right > left)
  {
    top = MYON_Max(r1->top, r2->top);
    bottom = MYON_Min(r1->bottom, r2->bottom);
    if (bottom > top)
      return true;
  }

  // degenerate rectangle at this point...
  SetRectEmpty();
  return false;
}

bool MYON_4iRect::IntersectRect(const MYON_4iRect & r1, const MYON_4iRect & r2) { return IntersectRect(&r1, &r2); }

bool MYON_4iRect::IsRectEmpty(void) const 
{ 
	return 0 == Width() || 0 == Height(); 
}

bool MYON_4iRect::IsRectNull(void) const
{ 
	return 0 == left &&
		   0 == top  &&
		   0 == bottom &&
		   0 == right; 
}

void MYON_4iRect::SetRect(int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }

bool MYON_4iRect::PtInRect(const MYON_2iPoint & pt) const
{
	return pt.x >= left && pt.y >= top && pt.x < right && pt.y < bottom;
}

void MYON_4iRect::OffsetRect(int x, int y)
{
	left += x;
	right += x;
	top += y;
	bottom += y;
}

void MYON_4iRect::OffsetRect(const MYON_2iVector& v)
{
	left += v.x;
	right += v.x;
	top += v.y;
	bottom += v.y;
}

void MYON_4iRect::InflateRect(int x, int y)
{
	left -= x;
	top -= y;
	right += x;
	bottom += y;
}

void MYON_4iRect::InflateRect(int l, int t, int r, int b)
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}

void MYON_4iRect::DeflateRect(int x, int y)
{
	left += x;
	top += y;
	right -= x;
	bottom -= y;
}

bool MYON_4iRect::SubtractRect(const MYON_4iRect* rect1, const MYON_4iRect* rect2)
{
	if (rect1 == nullptr)
		return false;

	*this = *rect1;

	if (rect1->IsRectEmpty() || rect2 == nullptr || rect2->IsRectEmpty())
	{
		return true;
	}

	if (rect2->top <= rect1->top && rect2->bottom >= rect1->bottom)
	{
		if (left < rect2->right)
		{
			left = MYON_Min(rect2->right, right);
		}
		if (right > rect2->left)
		{
			right = MYON_Max(left, rect2->left);
		}
	}

	if (rect2->left <= rect1->left && rect2->right >= rect1->right)
	{
		if (top < rect2->bottom)
		{
			top = MYON_Min(rect2->bottom, bottom);
		}
		if (bottom > rect2->top)
		{
			bottom = MYON_Max(top, rect2->top);
		}
	}

	return true;
}

void MYON_4iRect::NormalizeRect()
{
	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}
	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}

bool MYON_4iRect::IsZero() const
{
  return (0 == left && 0 == top && 0 == right && 0 == bottom);
}

void MYON_4iRect::SetZero() { *this = Zero; }


bool operator==(const MYON_4iRect& lhs, const MYON_4iRect& rhs)
{
  return (lhs.left == rhs.left
    && lhs.top == rhs.top 
    && lhs.right == rhs.right
    && lhs.bottom == rhs.bottom);
}

bool operator!=(const MYON_4iRect& lhs, const MYON_4iRect& rhs)
{
  return (lhs.left != rhs.left
    || lhs.top != rhs.top 
    || lhs.right != rhs.right
    || lhs.bottom != rhs.bottom);
}

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

MYON_3dSimplex::MYON_3dSimplex() { m_n = 0; }

/* this function checks the validity of ClosetPoint results*/
bool ClosestPointIsValid(const MYON_ConvexPoly& AHull, const MYON_ConvexPoly& BHull,
  MYON_4dex Adex, MYON_4dex Bdex, MYON_4dPoint ABBary, double atmost, MYON_TextLog* log = nullptr);


MYON_3dSimplex::MYON_3dSimplex(const MYON_3dPoint& a) { m_n = 1; m_V[0] = a; }
MYON_3dSimplex::MYON_3dSimplex(const MYON_3dPoint& a, const MYON_3dPoint& b) { m_n = 2; m_V[0] = a; m_V[1] = b; }
MYON_3dSimplex::MYON_3dSimplex(const MYON_3dPoint& a, const MYON_3dPoint& b, const MYON_3dPoint& c) { m_n = 3; m_V[0] = a; m_V[1] = b;  m_V[2] = c; }
MYON_3dSimplex::MYON_3dSimplex(const MYON_3dPoint& a, const MYON_3dPoint& b, const MYON_3dPoint& c, const MYON_3dPoint& d) { m_n = 4; m_V[0] = a; m_V[1] = b; m_V[2] = c; m_V[3] = d; }

int MYON_3dSimplex::Count() const { return m_n; };

bool MYON_3dSimplex::IsValid(double eps) const		// true if the Verticies are affinely independent
{
  bool rc = true;
  if (m_n >= 2)
  {
    MYON_3dVector V = m_V[1] - m_V[0];
    if( m_n==2)
      rc = ( V.Length() > eps );
    else
    {
      MYON_3dVector W = m_V[2] - m_V[0];
      MYON_3dVector X = MYON_CrossProduct(V, W);
      // TODO put something smart here....
      if( m_n==3)
        rc = (X.Length() > eps);
      else
      {
        // TODO and  here....
        double triple = X * (m_V[3] - m_V[0]);
        rc = (fabs(triple) > eps);
      }
    }
  }
  return rc;
}

const MYON_3dPoint& MYON_3dSimplex::operator[](int i) const {
  return *reinterpret_cast<const MYON_3dPoint*>(m_V + i);
}

MYON_3dPoint& MYON_3dSimplex::operator[](int i) {
  return *reinterpret_cast<MYON_3dPoint*>(m_V + i);
}


MYON_3dPoint MYON_3dSimplex::Vertex(int i) const { return MYON_3dPoint(m_V[i]); }
MYON_3dPoint& MYON_3dSimplex::Vertex(int i) { return *reinterpret_cast<MYON_3dPoint*>(&m_V[i]); }



MYON_3dPoint MYON_3dSimplex::Evaluate(const double* b) const
{
  MYON_3dVector p(0, 0, 0);
  for (int i = 0; i < m_n; i++)
    p += b[i] * m_V[i];
  return p;
}

MYON_3dPoint MYON_3dSimplex::Evaluate(const MYON_4dPoint& b)  const { return Evaluate(&b.x); }

double MYON_3dSimplex::Volume() const
{
  double vol = 0.0;
  int n = Count();
  if (n >= 2)
  {
    MYON_3dVector V = m_V[1] - m_V[0];
    if (n == 2) 
      vol = V.Length();
    else
    {
      MYON_3dVector X = MYON_CrossProduct(V, m_V[2]-m_V[0]);
      if (n == 3)
        vol = 0.5 * X.Length();
      else
        vol = 1.0/6.0 * fabs(X*(m_V[3] - m_V[0]));
    }
  }
  return vol;
}

double MYON_3dSimplex::SignedVolume() const
{
  double vol = MYON_UNSET_VALUE;
  if (Count() == 3)
  {
    MYON_3dVector V = m_V[1] - m_V[0];
    MYON_3dVector X = MYON_CrossProduct(V, m_V[2] - m_V[0]);
    vol = 1.0 / 6.0 * (X*(m_V[3] - m_V[0]));
   } 
   return vol;
}

double MYON_3dSimplex::MaximumCoordinate() const 
{
  double max = 0.0;
  for (int i = 0; i < Count(); i++)
  {
    double m = m_V[i].MaximumCoordinate();
    if (m > max)
      max = m;
  }
  return max;
}

MYON_BoundingBox MYON_3dSimplex::BoundingBox() const
{
  MYON_BoundingBox box;
  box.Set(3, false, m_n, 3, m_V[0], false);
  return box;
}

bool  MYON_3dSimplex::GetBoundingBox(
  MYON_BoundingBox& bbox, int bGrowBox  ) const
{
  return bbox.Set(3, false, m_n, 3, m_V[0], bGrowBox);
}

bool  MYON_3dSimplex::GetTightBoundingBox(
  MYON_BoundingBox& tight_bbox, bool bGrowBox,
  const MYON_Xform* xform  ) const
{
  if (bGrowBox && !tight_bbox.IsValid())
  {
    bGrowBox = false;
  }
  if (!bGrowBox)
  {
    tight_bbox.Destroy();
  }

  int i;
  for (i = 0; i < m_n; i++)
  {
    if (MYON_GetPointListBoundingBox(3, 0, m_n, 3, m_V[i], tight_bbox, bGrowBox, xform))
      bGrowBox = true;
  }
  return bGrowBox ? true : false;
}

bool MYON_3dSimplex::Transform(
  const MYON_Xform& xform)
{
  for (int i = 0; i < m_n; i++)
    m_V[i].Transform(xform);
  return true;
}

bool MYON_3dSimplex::Rotate(
  double sin_angle,
  double cos_angle,
  const MYON_3dVector& axis_of_rotation,
  const MYON_3dPoint& center_of_rotation)
{
  MYON_Xform R;
  R.Rotation(sin_angle, cos_angle, axis_of_rotation, center_of_rotation);
  return Transform(R);
}

bool MYON_3dSimplex::Rotate(
  double angle_in_radians,
  const MYON_3dVector& axis_of_rotation,
  const MYON_3dPoint& center_of_rotation )
{
  MYON_Xform R;
  R.Rotation(angle_in_radians, axis_of_rotation, center_of_rotation);
  return Transform(R);
}

bool MYON_3dSimplex::Translate(  const MYON_3dVector& delta )
{
  MYON_Xform T = MYON_Xform::TranslationTransformation(delta);
  return Transform(T);
}


bool MYON_3dSimplex::RemoveVertex(int i)
{
  bool rc = false;
  if (i < m_n)
  {
    m_n--;
    for (/**/; i < m_n; i++)
      m_V[i] = m_V[i + 1];
  }
  return rc;
}

bool MYON_3dSimplex::AddVertex(const MYON_3dPoint& P)
{
  bool rc = false;
  if (m_n < 4)
  {
    m_V[m_n++] = P;
  }
  return rc;
}


bool MYON_3dSimplex::SetVertex(int i, MYON_3dPoint P0)
{
  bool rc = false;
  if (i >= 0 && i < Count())
  {
    m_V[i] = P0;
    // todo clear any cashed data
    rc = true;
  }
  return rc;
}

MYON_3dVector MYON_3dSimplex::Edge(int e0, int e1) const
{
  MYON_3dVector V = MYON_3dVector::UnsetVector;
  if (e0 < Count() && e1 < Count())
  {
    V = Vertex(e1) - Vertex(e0);
  }
  return V;
}


/*
    This is a carefull implementation of cross product that tries to get an accurate result
*/
MYON_3dVector MYON_CrossProductwCare(const MYON_3dVector& A, MYON_3dVector& B)
{
  double norm[3];
  norm[0] = A.MaximumCoordinate();
  norm[1] = B.MaximumCoordinate();
  MYON_3dVector AxB = MYON_CrossProduct(A, B);
  const double thresh = 1.0e-8;		// sin(A,B) ~< thresh^(1/2) 
  double ab = norm[0] * norm[1];
  double ab2 = ab * ab;
  if (AxB.LengthSquared() < ab2*thresh)
  {
    // TODO - this is a good start but we could do something better...
    MYON_3dVector V[3] = { A, B, A - B };
    norm[2] = V[2].MaximumCoordinate();
    int maxi = (norm[0] > norm[1]) ? 0 : 1;
    if (norm[2] < norm[maxi])		// else C is longest so we are done.
    {
      AxB = MYON_CrossProduct(V[(maxi + 1) % 3], V[(maxi + 2) % 3]);
      if (maxi == 0)
        AxB =  - AxB;
    }
  }
  return AxB;
}


MYON_3dVector MYON_3dSimplex::FaceNormal(int noti) const
{
  MYON_3dVector N = MYON_3dVector::UnsetVector;
  if (Count() == 3 || (Count() == 4 && noti <= 3 && noti >= 0))
  {
    int ind[3] = { 0,1,2 };
    if (Count() == 4 && noti < 3)
    {
      for (int ii = 0; ii < 3; ii++)
        ind[ii] = (noti + 1 + ii) % 4;
    }
    MYON_3dVector A = Vertex(ind[1]) - Vertex(ind[0]);
    MYON_3dVector B = Vertex(ind[2]) - Vertex(ind[0]);
    N = MYON_CrossProductwCare(A, B );
  }
  return N;
}

MYON_3dVector MYON_3dSimplex::FaceUnitNormal(int noti) const
{
  MYON_3dVector N = FaceNormal(noti);
  if (N != MYON_3dVector::UnsetVector && N != MYON_3dVector::ZeroVector)
    N.Unitize();
  return N;
}

bool MYON_3dSimplex::GetClosestPoint(const MYON_3dPoint& P0, MYON_4dPoint& Bary, double atmost) const
{
  MYON_3dVector V = P0;
  MYON_3dSimplex Trans;
  bool toofar = ((atmost <= 0.0) ? false : true);
  bool rval = false;
  for (int i = 0; i < Count(); i++)
  {

    Trans.AddVertex(Vertex(i) - V);
    if (toofar && Trans[i].MaximumCoordinate() < .5 * atmost)
      toofar = false;
  }
  if (!toofar)
  {
    rval = Trans.GetClosestPointToOrigin(Bary);
    if (rval && atmost >= 0)
    {
      MYON_3dVector CP = Trans.Evaluate(Bary);
      if (CP.LengthSquared() > atmost*atmost)
        rval = false;
    }
  }
  return rval;
}

bool MYON_3dSimplex::GetClosestPointToOrigin(MYON_4dPoint& Bary) const
{
  bool rc = false;
  if (m_n == 4)
    rc = Closest3plex(Bary);
  else if (m_n == 3)
    rc = Closest2plex(Bary);
  else if (m_n == 2)
    rc = Closest1plex(Bary);
  else if (m_n == 1)
  {
    Bary = MYON_4dPoint(1.0, 0, 0, 0);
    rc = true;
  }
  return rc;
}

// return true if a and b are non zero and of same sign
static bool SameSign(double a, double b)
{
  return (a*b) > 0;
}


// closest point to a 1-simplex
bool MYON_3dSimplex::Closest1plex(MYON_4dPoint& Bary) const
{
  bool rc = false;
  MYON_3dVector Del = m_V[1] - m_V[0];
  double Del2 = Del.LengthSquared();
  if (Del2 > 0.0)
  {
    rc = true;
    double dot = -m_V[0] * Del;
    if (dot >= Del2)
      Bary = MYON_4dPoint(0, 1, 0, 0);
    else if (dot <= 0)
      Bary = MYON_4dPoint(1, 0, 0, 0);
    else
    {
      double b0 = dot / Del2;
      b0 = 1 - (1 - b0);      // ensure b0 + ( 1- b0) == 1.0 without rounding 
      Bary = MYON_4dPoint(1 - b0, b0, 0, 0);
    }
  }
  return rc;
}


// Rounds barycentric coordinates so that the coordinates sum to 1.0 in exact arithmetic.
bool MYON_3dSimplex::RoundBarycentricCoordinate(MYON_4dPoint& Bary)
{
  int mini = -1;
  double minc = MYON_UNSET_VALUE;
  for (int i = 0; i < 4; i++)
  {
    if (Bary[i] == 0.0) continue;
    Bary[i] = 1 - (1 - Bary[i]);
    if (mini < 0 || Bary[i] < minc)
    {
      mini = i;
      minc = Bary[i];
    }
  }
  if (mini >= 0)
  {
    Bary[mini] = 1.0 - (Bary[(mini + 1) % 4] + Bary[(mini + 2) % 4] + Bary[(mini + 3) % 4]);
  }
  return true;
}

// closest point to origin for a 2-simplex
bool MYON_3dSimplex::Closest2plex(MYON_4dPoint& Bary) const
{
  bool rc = false;
  MYON_3dVector N = FaceNormal();
  double N2 = N.LengthSquared();
  if (N2 > 0)
  {
    MYON_3dPoint P3 = (m_V[0] * N)*N / N2;										// origin projected to Affine_Span < V_0, V_1, V_2 >
    int J = N.MaximumCoordinateIndex();

    MYON_3dPoint Planar[3];		// We reduce to a planar closest point to origin problem
    for (int i = 0; i < 3; i++)
      Planar[i] = Vertex(i) - P3;

    // Finding the barycentric coordintes of the origin will guild the rest of the algorithm
    // We simplify this by projecting to Not J plane

    double DetM = 0.0;
    double C3[3];
    int j0 = (J + 1) % 3;
    int j1 = (J + 2) % 3;
    for (int i = 0; i < 3; i++)
    {
      int i0 = (i + 1) % 3;
      int i1 = (i + 2) % 3;
      C3[i] = Planar[i0][j0] * Planar[i1][j1] - Planar[i1][j0] * Planar[i0][j1];
      DetM += C3[i];
    }

    if (DetM != 0.0)
    {
      bool interior = true;
      for (int j = 0; interior && j < 3; j++)
        interior = SameSign(DetM, C3[j]);
      Bary[3] = 0.0;
      if (interior)
      {
        for (int i = 0; i < 3; i++)
          Bary[i] = C3[i] / DetM;

        RoundBarycentricCoordinate(Bary);
        rc = true;
      }
      else
      {
        for (int j = 0; j < 3; j++)
        {
          if (!SameSign(DetM, C3[j]))
          {
            MYON_3dSimplex S(Planar[(j + 1) % 3], Planar[(j + 2) % 3]);		// S is a 1-simplex
            MYON_4dPoint bary;
            if (S.GetClosestPointToOrigin(bary))
            {
              rc = true;
              bool OnEnd = (bary[0] == 1.0 || bary[1] == 1.0);
              Bary[j] = 0.0;
              Bary[3] = 0.0;
              for (int i = 0; i < 2; i++)
                Bary[(j + 1 + i) % 3] = bary[i];
              if (!OnEnd)
                break;
            }
          }
        }
      }
    }
  }
  return rc;
}

// closest point to a 3-simplex
bool MYON_3dSimplex::Closest3plex(MYON_4dPoint& Bary) const
{
  bool rc = false;
  //  Solving 
  //        [ V_0  V_1  V_2  V_3 ]          [ 0 ] 
  //  M*B = [  1    1    1    1  ]  * B  =  [ 1 ]

  int ind[3] = { 1,2,3 };
  double detM = 0.0;
  double C4[4];  // C4[j] = C_{4,j} is a cofactor of M
  double sign = 1.0;
  for (int j = 0; j < 4; j++)
  {
    C4[j] = sign * MYON_TripleProduct(m_V[ind[0]], m_V[ind[1]], m_V[ind[2]]);
    if (j < 3)
    {
      ind[j] = j;  // {1,2,3}, {0,2,3}, {0,1,3}, {0,1,2}
      sign *= -1.0;
    }
    detM += C4[j];
  }


  if (detM != 0.0)
  {
    bool interior = true;
    int j = 0;
    for (j = 0; interior && j < 4; j++)
      interior = SameSign(detM, C4[j]);
    if (interior)
    {
      for (int i = 0; i < 4; i++)
        Bary[i] = C4[i] / detM;

      RoundBarycentricCoordinate(Bary);

      rc = true;
    }
    else
    {
      j--;
      double D2 = MYON_DBL_MAX;		// best answer so far
      int N = 5;								// size of support
      do
      {
        if (!SameSign(detM, C4[j]))
        {
          MYON_3dSimplex S = (*this);
          S.RemoveVertex(j);
          MYON_4dPoint bary;
          if (S.Closest2plex(bary))
          {
            int n = 0;	// size of support
            for (int i = 0; i < 3; i++) if (bary[i] > 0)n++;
            if (n == 3)
            {
              for (int i = 3; i > j; i--)
                bary[i] = bary[i - 1];
              bary[j] = 0.0;
              Bary = bary;
              rc = true;
              break;
            }
            else
            {
              MYON_3dVector cp = S.Evaluate(bary);
              double d2 = cp.LengthSquared();

              if (d2 < D2 || (d2 == D2 && n < N))
              {
                D2 = d2;
                N = n;
                for (int i = 3; i > j; i--)
                  bary[i] = bary[i - 1];
                bary[j] = 0.0;
                rc = true;
                Bary = bary;
              }
            }
          }
          else
            rc = false;
        }
      } while (++j < 4);
    }
  }
  return rc;
}

bool MYON_ConvexPoly::Standardize(MYON_4dex& dex, MYON_4dPoint& B)
{
  bool rc = true;
  MYON_4dex rdex = { -1,-1,-1,-1 };	// results
  MYON_4dPoint rB(0, 0, 0, 0);
  int ri = 0; // index into result
  for (int ii = 0; ii < 4; ii++)	// index in input
  {
    while ((dex[ii] < 0 || B[ii] == 0.0) && ii < 4) ii++;
    if (ii == 4)
      break;
    int j = 0;
    while (j < ri && rdex[j] != dex[ii]) j++;
    if (j == ri)
    {
      rdex[ri] = dex[ii];
      rB[ri++] = 0.0;
    }
    rB[j] += B[ii];
  }

  if (rc)
  {
    dex = rdex;
    B = rB;
  }
  return rc;
}


void MYON_ConvexHullRef::Initialize(const MYON_3dVector* V0, int n)
{
  m_n = n;
  m_v = *V0;
  m_is_rat = false;
  m_stride = 3;
}

void MYON_ConvexHullRef::Initialize(const MYON_4dPoint* V0, int  n)
{
  m_n = n;
  m_v = *V0;
  m_is_rat = true;
  m_stride = 4;
}

// style must be either not_rational or homogeneous_rational = 2,
void MYON_ConvexHullRef::Initialize(const double* V0, MYON::point_style style, int  count)
{
	if (style == MYON::homogeneous_rational)
		Initialize(reinterpret_cast<const MYON_4dPoint*>(V0), count);
	else
			Initialize(reinterpret_cast<const MYON_3dVector*>(V0), count);
}

MYON_ConvexHullRef::MYON_ConvexHullRef(const MYON_3dVector* V0, int n)
{
  m_n = n;
  m_v = *V0;
  m_is_rat = false;
  m_stride = 3;
}

MYON_ConvexHullRef::MYON_ConvexHullRef(const MYON_3dPoint* P0, int n)
{
  m_n = n;
  m_v = *P0;
  m_is_rat = false;
  m_stride = 3;
}

MYON_ConvexHullRef::MYON_ConvexHullRef(const MYON_4dPoint* V0, int  n)
{
  m_n = n;
  m_v = *V0;
  m_is_rat = true;
  m_stride = 4;
}

MYON_ConvexHullRef::MYON_ConvexHullRef(const double* V0, bool is_rat, int  n)
{
  m_n = n;
  m_v = V0;
  m_is_rat = is_rat;
  m_stride = is_rat ? 4 : 3;
}

MYON_ConvexHullRef::MYON_ConvexHullRef(const double* V0, bool is_rat, int  n, int stride)
{
  m_n = n;
  m_v = V0;
  m_is_rat = is_rat;
  m_stride = stride;
}

MYON_3dVector MYON_ConvexHullRef::Vertex(int j) const
{
  MYON_3dVector v;
  if (m_is_rat)
  {
    MYON_4dPoint hv = *(reinterpret_cast<const MYON_4dPoint*>(m_v + m_stride*j));
    v = MYON_3dVector(hv.EuclideanX(), hv.EuclideanY(), hv.EuclideanZ());
  }
  else
  {
    v = *(reinterpret_cast<const MYON_3dVector*>(m_v + m_stride*j));
  }
  return v;
}

int MYON_ConvexHullRef::SupportIndex(MYON_3dVector W, int) const
{
  int j0 = 0;
  double dot = Vertex(0)*W;
  for (int j = 1; j < m_n; j++)
  {
    MYON_3dVector v = Vertex(j);
    double d = v * W;
    if (d > dot)
    {
      dot = d;
      j0 = j;
    }
  }
  return j0;
}

double MYON_ConvexHullRef::MaximumCoordinate() const
{
  return MYON_MaximumCoordinate(m_v, 3, m_is_rat, m_n);
}

int MYON_ConvexHullPoint2::AppendVertex(const MYON_3dPoint& P) // return index of new vertex.  must set Adjacent Indicies.
{
  m_Vert.Append(P);
  Ref.Initialize(m_Vert, m_Vert.Count());
  return m_Vert.Count()-1;
}

void MYON_ConvexHullPoint2::Empty()
{
	Ref.Initialize(m_Vert, 0);
	m_Vert.Empty();
}

double MYON_ConvexHullPoint2::MaximumCoordinate() const
{
  return MYON_MaximumCoordinate(m_Vert[0], 3, false, m_Vert.Count());
}

bool MYON_ConvexPoly::GetClosestPointSeeded(MYON_3dPoint P0,
  MYON_4dex& dex, MYON_4dPoint& Bary, double atmost ) const
{
  MYON_ConvexHullRef CvxPt(&P0, 1);
  // Set pdex to match the support of dex
  MYON_4dex pdex = dex;
  for (int i = 0; i < 4; i++)
  {
    if (dex[i] >= 0) 
      pdex[i] = 0;
  }
  bool rc =  GetClosestPointSeeded(CvxPt, dex, pdex, Bary, atmost);
  MYON_ConvexPoly::Standardize(dex, Bary);
  return rc;
}

// MatchingSupport(A, B) retuns a positive number if
//  A[i]<0 iff B[i]<0 and at least one coordinate pair has valid indicies A[i]>=0 and B[i]>=0.
static int MatchingSupport(const MYON_4dex& A, const MYON_4dex& B)
{
  int nsup  = 0;
  int i =0;
  for (; i < 4; i++)
  {
    if ((A[i] < 0) != (B[i] < 0))
      break;
    if (A[i] >= 0)
      nsup++;
  }
  return (i == 4) ? nsup : -1;
}


// Gilbert Johnson Keerthi  algorithm
bool MYON_ConvexPoly::GetClosestPoint(const MYON_ConvexPoly& B,
  MYON_4dex& Adex, MYON_4dex& Bdex, MYON_4dPoint& bary,
  double maximum_distance) const
{
  Adex = Bdex = MYON_4dex::Unset;
  return GetClosestPointSeeded(B, Adex, Bdex, bary, maximum_distance);
};


bool MYON_ConvexPoly::GetClosestPoint(MYON_3dPoint P0,
  MYON_4dex& dex, MYON_4dPoint& bary,
  double maximum_distance ) const
{
  dex = MYON_4dex::Unset;
  return GetClosestPointSeeded(P0, dex, bary, maximum_distance);
}

// Class for a pair of simplicies from a pair of MYON_ConvexPoly's
class GJK_Simplex
{
public:
  MYON_3dSimplex Simp;                   // Minkowski sum simplex A - B 
  MYON_4dPoint Bary = MYON_4dPoint::Zero;  // represents a point in Simp
  int Aind[4] = { -1,-1,-1,-1 };        
  int Bind[4] = { -1,-1,-1,-1 };
 
  // Append new vertex at end 
  bool AddVertex(const MYON_3dVector& v, int aind, int bind);
  bool RemoveVertex(int i);           // index of vertex pair to remove

  bool Includes(int aind, int bind);  // true if (aind, bind) is a vertex pair in this simplex
};

bool GJK_Simplex::AddVertex(const MYON_3dVector& v, int aind, int bind)
{
  bool rc = false;
  int n0 = Simp.Count();
  if (n0 < 4)
  {
    Simp.AddVertex(v);
    Aind[n0] = aind;
    Bind[n0] = bind;
    if (n0 > 0)
      Bary[n0] = 0.0;
    else
      Bary[n0] = 1.0;
    rc = true;
  }
  return rc;
}

bool GJK_Simplex::RemoveVertex(int i)
{
  bool rc = false;
  int n0 = Simp.Count();
  if (i < n0)
  {
    Simp.RemoveVertex(i);

    for (int j = i; j < n0-1; j++)
    {
      Bary[j] = Bary[j + 1];
      Aind[j] = Aind[j + 1];
      Bind[j] = Bind[j + 1];
    }
    Bary[n0-1] = 0.0;
    Aind[n0-1] = Bind[n0-1] = -1;
  }
  return rc;
}

bool GJK_Simplex::Includes(int aind, int bind) // true if (aind, bind) is a vertex pair in this simplex
{
  int n0 = Simp.Count();
  for (int i = 0; i < n0; i++)
    if (Aind[i] == aind && Bind[i] == bind)
      return true;
  return false;
}

// To supply an inital seed simplex Adex and Bdex must be valid and 
// have matching support specifically
//     Adex[i]<A.Count() and Bdex[i]<B.Count()   for all i
//     Adex[i]<0 iff Bdex[i]<0  for all i
//     Adex[i]>=0 for some i    for some i
// By satisfying this condition Adex and Bdex will define a simplex in A - B 
// Note the result of a ClosestPoint calculation Adex and Bdex satisfy these conditions 
bool MYON_ConvexPoly::GetClosestPointSeeded(const MYON_ConvexPoly& B,
  MYON_4dex& Adex, MYON_4dex& Bdex, MYON_4dPoint& Bary,
  double atmost) const
{
  const  MYON_ConvexPoly& A = *this;
  bool rc = false;
  if (Count() == 0 || B.Count() == 0)
    return false;

  GJK_Simplex GJK;

  MYON_3dVector v(0,0,0);


  // If Adex and Bdex are valid on entry we use them as an inital 
  // seed for the trial simplex.  This case is indicated by setting
  // bFirstPass 
  bool bFirstPass = false;       
  if (A.IsValid4Dex(Adex) && B.IsValid4Dex(Bdex) && MatchingSupport(Adex, Bdex)>0 )
  {
    // Set the initial condition for GJK from Adex and Bdex
    int i = 0;
    for (i = 0; i < 4; i++)
    {
      if (Adex[i] < 0 || Bdex[i] < 0)
        continue;

      if (GJK.Includes(Adex[i], Bdex[i]))
        break;
 
      MYON_3dVector vert = Vertex(Adex[i]) - B.Vertex(Bdex[i]);
      GJK.AddVertex(vert, Adex[i], Bdex[i]);
    }
 
    bFirstPass = (i==4);
  }
	
	bool done = false;
	double vlen = MYON_DBL_MAX;
	double vlenlast = MYON_DBL_MAX;
	while (!done)
	{
		if (!bFirstPass)
		{
      // Default initial simplex is a point  A.Vertex(0) - B.Vertex(0);
			v = A.Vertex(0) - B.Vertex(0);
      GJK.AddVertex(v, 0, 0);
      GJK.Bary[0] = 1.0;
      vlenlast = MYON_DBL_MAX;
			vlen = v.Length();
		}

		double mu = 0.0;
		const double epsilon = 10000.0 * MYON_EPSILON;

		int wA = 0, wB = 0;
		while (!done && (bFirstPass || vlen > 0))
		{
			MYON_3dVector W;
			if (!bFirstPass)
			{
				wA = A.SupportIndex(-v, wA);
				wB = B.SupportIndex(v, wB);
				W = A.Vertex(wA) - B.Vertex(wB);
        MYON_3dVector unit_v = 1 / vlen * v;
				double del = unit_v * W ;		// this is lower bound on the distance
				if (del > mu)
					mu = del;

				// 18-July-19  Considered Adding vlen>=vlenlast to ensure distance is decreasing
				//    See RH-47044 for a case that got hung up here
        // See RH-30343 for a case where the 2.0 factor is needed
        // WRONG!!!  RH-30343 again.  the 2.0 factor doest't work either
        // TODO: testing... If the support vertex is already in simplex were done
 
        done = GJK.Simp.Count() == 4 || GJK.Includes(wA, wB);

        // See 100818MatchPoints.3dm if Bary>0 then we are done since closest point is
        // in the interior of a Simplex 
        double SimpNorm = GJK.Simp.MaximumCoordinate();

          // RH-59237. 24-June-20 Add term  ||Simp||*epsilon to allow for roundoff error in Simp evaluation 
          // RH-59494. 13-Aug-20 Added a factor of 4.0 to  ||Simp||*epsilon  term. 
          // RH-59494.CSXBugB 19-Aug-20 increased a factor of 20.0 to  ||Simp||*epsilon  term. 
        if(!done)
          done = ((vlen - mu) <= 2.0* mu *epsilon + SimpNorm*20.0*MYON_EPSILON ) || mu > atmost  || vlen >= vlenlast ;
      }
			if (!done)
			{
	
				if (!bFirstPass)
					GJK.AddVertex(W, wA, wB);

        // The key to the implemetation of this algorithm is contained in Simplex::GetClosestPointToOrigin()
				if (GJK.Simp.GetClosestPointToOrigin(GJK.Bary))
				{
					bFirstPass = false;
					v = GJK.Simp.Evaluate(GJK.Bary);
					vlenlast = vlen;
					vlen = v.Length();
          if (true)
          {
            for (int i = GJK.Simp.Count() - 1; i >= 0; i--)
            {
              if (GJK.Bary[i] == 0.0)
                GJK.RemoveVertex(i);
            }
          }
          else
          {
            /* this is error recovery code. it is currenly DISABLED
            // The last step resulted in crap lets undo it and set done
            int n = GJK.Simp.Count() - 1;
            GJK.RemoveVertex(n);
            GJK.Simp.GetClosestPointToOrigin(GJK.Bary);
            v = GJK.Simp.Evaluate(GJK.Bary);
            vlen = v.Length();
            done = true;
            */
          }
				}
				else
				{
					// In this case I am going to terminte the iteration.  If this was a FirstPass with user supplied initial guess
					// then we restart the algorithm without the initial guess.
					break;
				}
			}
		}
		if (!done)
		{
			if (bFirstPass)
				bFirstPass = false;
			else
				done = true;

		}

    /* TODO
    RH-54751
    vlen is nearly 0. but it should be 0.0 
    If (0,0,0) s in the interior of A-B then solution is vlen == 0.0 
    */
    if (GJK.Simp.Count() == 4 && GJK.Simp.Volume() > MYON_SQRT_EPSILON)
    {
      vlen = 0.0;
    }
		rc = (vlen <= atmost);
		if (rc)
		{
			if (GJK.Simp.Count() > 0)
			{
				for (int i = GJK.Simp.Count(); i < 4; i++)
				{
					GJK.Bary[i] = 0.0;
					GJK.Aind[i] = GJK.Bind[i] = -1;
				}
			}



			Adex = MYON_4dex(GJK.Aind[0], GJK.Aind[1], GJK.Aind[2], GJK.Aind[3]);
			Bdex = MYON_4dex(GJK.Bind[0], GJK.Bind[1], GJK.Bind[2], GJK.Bind[3]);
      Bary = GJK.Bary;
		}
	}
  return rc;
}


// Is PQR a right hnd turn
static bool IsLeftTurn(const MYON_2dPoint& P, const MYON_2dPoint& Q, const MYON_2dPoint& R)
{
	MYON_2dVector A = R - Q;
	MYON_2dVector B = P - Q;
	double det = A.x* B.y - B.x*A.y;
	return det > 0.0;
}


int MYON_ConvexHull2d(const MYON_SimpleArray<MYON_2dPoint>& Pnt, MYON_SimpleArray<MYON_2dPoint>& Hull, MYON_SimpleArray< int>* PntInd)
{
	int rval = -1;
	Hull.Empty();
	MYON_SimpleArray<int> Ind(Pnt.Count());
	Ind.SetCount(Pnt.Count());
	if (PntInd)
		PntInd->Empty();
	if (!Pnt.Sort(MYON::sort_algorithm::quick_sort, Ind,
		[](const MYON_2dPoint* A, const MYON_2dPoint* B) { return MYON_2dPoint::Compare(*A, *B); }))
		return  rval;
	if (Pnt.Count() == 0)
		return rval;

	Hull.Append(Pnt[Ind[0]]);
	if (PntInd)
		PntInd->Append(Ind[0]);
	int fixed = 1;    // This is the count of Hull that is fixed
	int ri = 1;

	for (int inc = 1; inc >= -1; inc -= 2)
	{
		for ( /*empty*/; ri < Pnt.Count() && ri >= 0; ri += inc)
		{
			MYON_2dPoint R = Pnt[Ind[ri]];

			if (Hull.Count() == fixed)
			{
				if (R != *Hull.Last())
				{
					Hull.Append(R);
					if (PntInd)
						PntInd->Append(Ind[ri]);
				}
			}
			else
			{
				int pi = Hull.Count() - 2;
				MYON_2dPoint P = Hull[pi];
				MYON_2dPoint Q = *Hull.Last();
				if (IsLeftTurn(P, Q, R))
				{
					Hull.Append(R);
					if (PntInd)
						PntInd->Append(Ind[ri]);
				}
				else
				{
					bool done = false;
					while (!done)
					{
						Hull.Remove();
						if (PntInd)
							PntInd->Remove();
						Q = P;
						done = (pi == ((inc==1)?0:fixed-1));
						if (!done)
						{
							P = Hull[--pi];
							done = IsLeftTurn(P, Q, R);
						}

					}
					Hull.Append(R);
					if (PntInd)
						PntInd->Append(Ind[ri]);
				}
			}
		}
		if (Hull.Count() == 1) {
			rval = 0;
			break;
		}
		fixed = Hull.Count();
		ri = Pnt.Count() - 2;
	}

	if (Hull.Count() == 2)
		rval = 1;
	else
		rval = 2;
	return rval;
}




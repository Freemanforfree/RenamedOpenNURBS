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

#include "opennurbs_subd_data.h"


enum
{
  // change bAllFragmentsHaveCurvature to true when principal and sectional curvatures are needed
  // or delete this enum and always make room for curvatures.
  MYON_Internal_bAllFragmentsHaveCurvature = 0
};



const size_t MYON_SubDHeap::g_sizeof_fragment[MYON_SubDDisplayParameters::MaximumDensity + 1] =
{
  MYON_SubDMeshFragment::SizeofFragment(0, MYON_Internal_bAllFragmentsHaveCurvature),
  MYON_SubDMeshFragment::SizeofFragment(1, MYON_Internal_bAllFragmentsHaveCurvature),
  MYON_SubDMeshFragment::SizeofFragment(2, MYON_Internal_bAllFragmentsHaveCurvature),
  MYON_SubDMeshFragment::SizeofFragment(3, MYON_Internal_bAllFragmentsHaveCurvature),
  MYON_SubDMeshFragment::SizeofFragment(4, MYON_Internal_bAllFragmentsHaveCurvature),
  MYON_SubDMeshFragment::SizeofFragment(5, MYON_Internal_bAllFragmentsHaveCurvature),
  MYON_SubDMeshFragment::SizeofFragment(MYON_SubDDisplayParameters::MaximumDensity, MYON_Internal_bAllFragmentsHaveCurvature)
};

static void* MYON_SubD__Allocate(size_t sz)
{
  if (0 == sz)
    return nullptr;

  // double array allocation is used to insure the memory
  // returned by new is properly aligned for any type.
  double* a;
  size_t sz1 = sz % sizeof(a[0]);
  if (sz1 > 0)
    sz += (sizeof(a[0]) - sz1);
  a = new(std::nothrow) double[sz];

  if (nullptr == a)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  return a;
}

static void MYON_SubD__Free(void* p)
{
  if (nullptr != p)
  {
    double* a = (double*)p;
    delete[] a;
  }
}



//////////////////////////////////////////////////////////////////////////
//
// MYON_SubD_FixedSizeHeap
//

unsigned int MYON_SubD_FixedSizeHeap::m__sn_factory = 0;

MYON_SubD_FixedSizeHeap::~MYON_SubD_FixedSizeHeap()
{
  Destroy();
}


void MYON_SubD_FixedSizeHeap::Destroy()
{
  Reset();
  m_v_capacity = 0;
  m_e_capacity = 0;
  m_f_capacity = 0;
  m_p_capacity = 0;
  m_h_capacity = 0;
  m_h_count = 0;
  void* p[6] = { m_v, m_e, m_f, m_p, m_hash_table, m_hash_elements };
  m_v = nullptr;
  m_e = nullptr;
  m_f = nullptr;
  m_p = nullptr;
  m_hash_table = nullptr;
  m_hash_elements = nullptr;
  MYON_SubD__Free(p[0]);
  MYON_SubD__Free(p[1]);
  MYON_SubD__Free(p[2]);
  MYON_SubD__Free(p[3]);
  MYON_SubD__Free(p[4]);
  MYON_SubD__Free(p[5]);
}

void MYON_SubD_FixedSizeHeap::Reset()
{
  if (m_h_capacity > 0)
    memset(m_hash_table, 0, m_h_capacity * sizeof(*m_hash_table));
  m_v_index = 0;
  m_e_index = 0;
  m_f_index = 0;
  m_p_index = 0;
  m_h_count = 0;
}

bool MYON_SubD_FixedSizeHeap::InUse() const
{
  return (m_v_index > 0 || m_e_index > 0 || m_f_index>0 || m_p_index>0);
}

class MYON_SubD_FixedSizeHeap_ComponentPairHashElement
{
public:
  //static const MYON_SubD_FixedSizeHeap_ComponentPairHashElement Empty;
  MYON_SubDComponentPtrPair m_pair;
  MYON_SubD_FixedSizeHeap_ComponentPairHashElement* m_next;

};

bool MYON_SubD_FixedSizeHeap::Internal_ReserveSubDWorkspace_HashTable()
{
  const unsigned int hash_capacity = (m_v_capacity > 0) ? (m_v_capacity / 4 + 1) : 0;
  m_h_count = 0;
  if (hash_capacity > m_h_capacity)
  {
    m_h_capacity = 0;
    if (nullptr != m_hash_elements)
    {
      MYON_SubD__Free(m_hash_elements);
      m_hash_elements = nullptr;
    }
    if (nullptr != m_hash_table)
    {
      MYON_SubD__Free(m_hash_table);
      m_hash_table = nullptr;
    }
    m_hash_table = (MYON_SubD_FixedSizeHeap_ComponentPairHashElement**)MYON_SubD__Allocate(hash_capacity * sizeof(*m_hash_table));
    if (nullptr == m_hash_table)
      return false;
    m_hash_elements = (MYON_SubD_FixedSizeHeap_ComponentPairHashElement*)MYON_SubD__Allocate(m_v_capacity * sizeof(*m_hash_elements));
    if (nullptr == m_hash_elements)
    {
      MYON_SubD__Free(m_hash_table);
      m_hash_table = nullptr;
      return false;
    }
    m_h_capacity = hash_capacity;
  }
  if ( m_h_capacity > 0 && nullptr != m_hash_table)
    memset(m_hash_table, 0, m_h_capacity * sizeof(*m_hash_table));
  return true;
}

bool MYON_SubD_FixedSizeHeap::Internal_ReserveSubDWorkspace(
  size_t vertex_capacity,
  size_t face_capacity,
  size_t array_capacity,
  bool bEnableHash
  )
{
  if ( vertex_capacity <= 0 || face_capacity <= 0 || array_capacity <= 0)
  {
    Destroy();
    return MYON_SUBD_RETURN_ERROR(false);
  }
  const size_t edge_capacity = vertex_capacity + face_capacity - 1; // Euler formula
  if (m_v_capacity >= vertex_capacity
    && m_e_capacity >= edge_capacity
    && m_f_capacity >= face_capacity
    && m_p_capacity >= array_capacity
    )
  {
    Reset();
    if (bEnableHash)
      Internal_ReserveSubDWorkspace_HashTable();
    else
      m_h_count = MYON_SubD_FixedSizeHeap::DisabledHashCount;
    return true;
  }

  Destroy();

  size_t max_capacity = 0xFFFFFFU;
  if (vertex_capacity > max_capacity || edge_capacity > max_capacity || face_capacity > max_capacity || array_capacity > max_capacity)
    return MYON_SUBD_RETURN_ERROR(false);

  for (;;)
  {
    m_v = (MYON_SubDVertex*)MYON_SubD__Allocate(vertex_capacity*sizeof(m_v[0]));
    if (nullptr == m_v && vertex_capacity > 0)
      break;
    m_e = (MYON_SubDEdge*)MYON_SubD__Allocate(edge_capacity*sizeof(m_e[0]));
    if (nullptr == m_e && edge_capacity > 0)
      break;
    m_f = (MYON_SubDFace*)MYON_SubD__Allocate(face_capacity*sizeof(m_f[0]));
    if (nullptr == m_f && face_capacity > 0)
      break;
    m_p = (MYON__UINT_PTR*)MYON_SubD__Allocate(array_capacity*sizeof(m_p[0]));
    if (nullptr == m_p && array_capacity > 0)
      break;

    m_v_capacity = (unsigned int)vertex_capacity;
    m_e_capacity = (unsigned int)edge_capacity;
    m_f_capacity = (unsigned int)face_capacity;
    m_p_capacity = (unsigned int)array_capacity;

    if (bEnableHash)
      Internal_ReserveSubDWorkspace_HashTable();
    else
      m_h_count = MYON_SubD_FixedSizeHeap::DisabledHashCount;
    return true;
  }

  Destroy();

  return MYON_SUBD_RETURN_ERROR(false);
}

bool MYON_SubD_FixedSizeHeap::ReserveSubDWorkspace(
  unsigned int sector_edge_count
  )
{
  if (0 == sector_edge_count)
  {
    Destroy();
    return true;
  }

  const unsigned int k = (sector_edge_count <= 4) ? 0 : (sector_edge_count - 4);
  const unsigned int v_capacity = 16 + 2 * k;
  const unsigned int f_capacity = 9 + k;
  const unsigned int p_capacity = 8*v_capacity + 2 * k;

  return Internal_ReserveSubDWorkspace(v_capacity, f_capacity, p_capacity, false);
}

static unsigned int Internal_AtLeast4(unsigned int n)
{
  return (n > 4U) ? n : 4U;
}

bool MYON_SubD_FixedSizeHeap::ReserveSubDWorkspace(
  const MYON_SubDFace* center_face0
  )
{
  unsigned int v_capacity =  0;
  unsigned int f_capacity =  0;
  unsigned int a_capacity =  0;

  for (;;)
  {
    if (nullptr == center_face0)
      break;

    const unsigned int N = center_face0->m_edge_count;
    if (N <= 2)
      break;

    unsigned int S = 0;  // Set S = sum of the number of edges attached to each vertex of center_face0.
    unsigned int T = Internal_AtLeast4(N);  // Set T = capacity required for vertex edge arrays on face subdivision vertices
    unsigned int X = 0;
    bool bValenceTwoVertices = false; // bValenceTwoVertices = true if center_face0 has a valence 2 vertex and we need the hash table
    {
      const MYON_SubDEdgePtr* edges = center_face0->m_edge4;
      MYON__UINT_PTR edge_ptr;
      const MYON_SubDEdge* edge;
      const MYON_SubDVertex* vertex;
      const MYON_SubDFace* vertex_face;
      unsigned int fei;
      edge = center_face0->Edge(N - 1);
      if (nullptr == edge)
        break;
      bool bEdgeIsHardCrease[2] = { false, edge->IsHardCrease() };
      for (fei = 0; fei < N; fei++, edges++)
      {
        if (4 == fei)
        {
          edges = center_face0->m_edgex;
          if (nullptr == edges)
            break;
        }
        edge_ptr = edges->m_ptr;
        edge = MYON_SUBD_EDGE_POINTER(edge_ptr);
        if (nullptr == edge)
          break;
        bEdgeIsHardCrease[0] = bEdgeIsHardCrease[1];
        bEdgeIsHardCrease[1] = edge->IsHardCrease();
        vertex = edge->m_vertex[MYON_SUBD_EDGE_DIRECTION(edge_ptr)];
        if (nullptr == vertex)
          break;
        if (vertex->m_edge_count < 2)
          break;
        if (vertex->m_edge_count < vertex->m_face_count)
          break;
        S += vertex->m_edge_count;
        X += Internal_AtLeast4(vertex->m_edge_count);
        if ( bEdgeIsHardCrease[0] && bEdgeIsHardCrease[1] && vertex->IsCreaseOrCorner() )
        {
          // If this vertex has multiple sectors, the other sectors are isolated from center_face0 by hard creases.
          continue;
        }
        if (2 == vertex->m_edge_count)
        {
          // ring face has valence 2 vertex and the subdivision point for vertex_face
          // may be reference by 2 different edges from center_face0
          bValenceTwoVertices = true;
        }
        for (unsigned short vfi = 0; vfi < vertex->m_face_count; ++vfi)
        {
          vertex_face = vertex->m_faces[vfi];
          if (nullptr == vertex_face || center_face0 == vertex_face)
            continue;
          T += Internal_AtLeast4(vertex_face->m_edge_count);
        }
      }
      if (fei != N)
        break;
    }

    // NOTE: S >= 2*N
    v_capacity =  2*(S - N) + 1; // maximum possible and occurs when all face0 edges are distinct and smooth
    f_capacity =  S;  // maximum possible and occurs when all face0 edges are distinct and smooth

    // T = capacity required for vertex edge arrays on face subdivision vertices
    // 4*(S-N) = capacity required for vertex edge arrays on edge subdivision vertices
    // X = capacity required for vertex edge arrays on vertex subdivision vertices
    //
    a_capacity = 2*( X + T + 4 * (S - N) ); // Twice the number of edges from all subdivision vertices.

    return Internal_ReserveSubDWorkspace(
      v_capacity,
      f_capacity,
      a_capacity,
      (0U == center_face0->SubdivisionLevel()) || bValenceTwoVertices
    );
  }

  Destroy();
  if (nullptr == center_face0 )
    return true;

  return MYON_SUBD_RETURN_ERROR(false);
}

bool MYON_SubD_FixedSizeHeap::Internal_HashEnabled() const
{
  return (MYON_SubD_FixedSizeHeap::DisabledHashCount != m_h_count && m_h_capacity > 0);
}

unsigned int MYON_SubD_FixedSizeHeap::Internal_Hash(MYON_SubDComponentPtr component0)
{
  return Internal_HashEnabled() ? (((unsigned int)component0.Hash16FromTypeAndId()) % m_h_capacity) : 0U;
}

MYON_SubDVertex* MYON_SubD_FixedSizeHeap::Internal_HashFindVertex1(unsigned int hash, MYON_SubDComponentPtr component0)
{
  if (Internal_HashEnabled())
  {
    for (MYON_SubD_FixedSizeHeap_ComponentPairHashElement* e = m_hash_table[hash]; nullptr != e; e = e->m_next)
    {
      if (component0.m_ptr == e->m_pair.m_pair[0].m_ptr)
        return e->m_pair.m_pair[1].Vertex();
    }
  }
  return nullptr;
}

void MYON_SubD_FixedSizeHeap::Internal_HashAddPair(unsigned int hash, MYON_SubDComponentPtr component0, class MYON_SubDVertex* vertex1)
{
  if (Internal_HashEnabled())
  {
    if (vertex1->m_id == m_v_index)
    {
      MYON_SubD_FixedSizeHeap_ComponentPairHashElement* e = &m_hash_elements[vertex1->m_id - 1];
      e->m_pair.m_pair[0] = component0;
      e->m_pair.m_pair[1] = MYON_SubDComponentPtr::Create(vertex1);
      e->m_next = m_hash_table[hash];
      m_hash_table[hash] = e;
      ++m_h_count;
    }
    else
    {
      MYON_SUBD_ERROR("unexpected has table state");
    }
  }
}

MYON_SubDVertex* MYON_SubD_FixedSizeHeap::AllocateVertex(
  const double vertexP[3],
  unsigned int edge_capacity
  )
{
  if (nullptr == m_v || m_v_index >= m_v_capacity)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int face_capacity = edge_capacity;

  if (edge_capacity + face_capacity + m_p_index > m_p_capacity )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON__UINT_PTR* a = nullptr;
  if (0 != edge_capacity || 0 != face_capacity)
  {
    if ( edge_capacity > 0xFFFFu)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    if ( face_capacity > 0xFFFFu)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    a = AllocatePtrArray(edge_capacity + face_capacity, true);
    if (nullptr == a)
      return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  MYON_SubDVertex* v = m_v + m_v_index;
  memset(v, 0, sizeof(*v));
  if (m_v_index > 0)
  {
    // code in MYON_SubDFaceNeighborhood.Subdivide() relies on
    // m_next_vertex being set this way.
    m_v[m_v_index - 1].m_next_vertex = v;
    v->m_prev_vertex = &m_v[m_v_index - 1];
  }
  v->m_id = ++m_v_index;

  if (nullptr != vertexP)
  {
    v->m_P[0] = vertexP[0];
    v->m_P[1] = vertexP[1];
    v->m_P[2] = vertexP[2];
  }

  if (edge_capacity > 0)
  {
    v->m_edge_capacity = (unsigned short)edge_capacity;
    v->m_edges = (MYON_SubDEdgePtr*)a;
    a += edge_capacity;
  }
  if (face_capacity > 0)
  {
    v->m_face_capacity = (unsigned short)face_capacity;
    v->m_faces = (const MYON_SubDFace**)a;
  }
  a = 0;

  return v;
}

MYON_SubDVertex* MYON_SubD_FixedSizeHeap::AllocateVertex(
  const MYON_SubDVertex* vertex0,
  unsigned int edge_capacity
  )
{
  if ( nullptr == vertex0)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  double subdP[3];
  if (false == vertex0->GetSubdivisionPoint(subdP))
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON_SubDVertex* v1 = AllocateVertex(subdP, edge_capacity);

  if (nullptr == v1)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  v1->SetSubdivisionLevel( vertex0->SubdivisionLevel() + 1 );

  v1->m_vertex_tag = vertex0->m_vertex_tag;

  if (vertex0->SurfacePointIsSet())
  {
    // copy any cached limit point from vertex0 to v1.
    MYON_SubDSectorSurfacePoint limit_point;
    if (vertex0->GetSurfacePoint(vertex0->m_faces[0], limit_point))
    {
      if (nullptr == limit_point.m_sector_face)
      {
        limit_point.m_next_sector_limit_point = (const MYON_SubDSectorSurfacePoint*)1;
        v1->SetSavedSurfacePoint(true, limit_point);
      }
    }
  }

  return v1;
}

MYON_SubDVertex* MYON_SubD_FixedSizeHeap::AllocateEdgeSubdivisionVertex(bool bUseFindOrAllocate, const MYON_SubDEdge* edge0)
{
  return bUseFindOrAllocate ? FindOrAllocateVertex(edge0) : AllocateVertex(edge0);
}

MYON_SubDVertex * MYON_SubD_FixedSizeHeap::FindOrAllocateVertex(const MYON_SubDEdge * edge0)
{
  if ( nullptr == edge0)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const MYON_SubDComponentPtr component0 = MYON_SubDComponentPtr::Create(edge0);
  const unsigned int hash = Internal_Hash(component0);
  MYON_SubDVertex* v1 = Internal_HashFindVertex1(hash, component0);

  if (nullptr != v1)
  {
    // found the previously allocated vertex
    if (((unsigned int)v1->m_edge_capacity) < 4)
    {
      MYON_SUBD_ERROR("edge capacity was too small when vertex was created.");
    }
    return v1;
  }

  v1 = AllocateVertex(edge0);
  if (nullptr == v1)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  Internal_HashAddPair(hash, component0, v1);

  return v1;
}

MYON_SubDVertex* MYON_SubD_FixedSizeHeap::AllocateVertex(
  const MYON_SubDEdge* edge0
  )
{
  if ( nullptr == edge0)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  double subdP[3];
  if (false == edge0->GetSubdivisionPoint(subdP))
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int edge_capacity = 4;
  MYON_SubDVertex* v1 = AllocateVertex(subdP, edge_capacity);
  if (nullptr == v1)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  v1->SetSubdivisionLevel( edge0->SubdivisionLevel() + 1 );

  if (MYON_SubDEdgeTag::Smooth == edge0->m_edge_tag || MYON_SubDEdgeTag::SmoothX == edge0->m_edge_tag)
    v1->m_vertex_tag = MYON_SubDVertexTag::Smooth;
  else if (MYON_SubDEdgeTag::Crease == edge0->m_edge_tag)
    v1->m_vertex_tag = MYON_SubDVertexTag::Crease;

  return v1;
}

MYON_SubDVertex * MYON_SubD_FixedSizeHeap::FindOrAllocateVertex(const MYON_SubDFace * face0)
{
  const unsigned int face0_edge_count = (nullptr != face0) ? ((unsigned int)face0->m_edge_count) : 0U;
  if (face0_edge_count < 3)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const MYON_SubDComponentPtr component0 = MYON_SubDComponentPtr::Create(face0);
  const unsigned int hash = Internal_Hash(component0);
  MYON_SubDVertex* v1 = Internal_HashFindVertex1(hash, component0);

  if (nullptr != v1)
  {
    // found the previously allocated vertex
    if (((unsigned int)v1->m_edge_capacity) < face0->m_edge_count)
    {
      MYON_SUBD_ERROR("edge capacity was too small when vertex was created.");
    }
    return v1;
  }

  double subdP[3];
  if (false == face0->GetSubdivisionPoint(subdP))
    return MYON_SUBD_RETURN_ERROR(nullptr);

  v1 = AllocateVertex(subdP, face0_edge_count );
  if (nullptr == v1)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  v1->SetSubdivisionLevel( face0->SubdivisionLevel() + 1 );
  v1->m_vertex_tag = MYON_SubDVertexTag::Smooth;

  Internal_HashAddPair(hash, component0, v1);

  return v1;
}


MYON_SubDVertex * MYON_SubD_FixedSizeHeap::AllocateSectorFaceVertex(const MYON_SubDFace * sector_face0)
{
  if (nullptr == sector_face0)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  double subdP[3];
  if (false == sector_face0->GetSubdivisionPoint(subdP))
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDVertex* v1 = AllocateVertex(subdP, 3 );
  if (nullptr == v1)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  v1->SetSubdivisionLevel( sector_face0->SubdivisionLevel() + 1 );
  v1->m_vertex_tag = MYON_SubDVertexTag::Smooth;
  return v1;
}

const MYON_SubDEdgePtr MYON_SubD_FixedSizeHeap::AllocateEdge(
  bool bUseFindOrAllocatEdge,
  MYON_SubDVertex* v0,
  double v0_sector_coefficient,
  MYON_SubDVertex* v1,
  double v1_sector_coefficient
)
{
  return bUseFindOrAllocatEdge 
    ? FindOrAllocateEdge(v0, v0_sector_coefficient, v1, v1_sector_coefficient) 
    : AllocateEdge(v0, v0_sector_coefficient, v1, v1_sector_coefficient);
}

const MYON_SubDEdgePtr MYON_SubD_FixedSizeHeap::FindOrAllocateEdge(
  MYON_SubDVertex * v0,
  double v0_sector_coefficient,
  MYON_SubDVertex * v1,
  double v1_sector_coefficient
)
{
  if ( nullptr == v0 || nullptr == v0->m_edges)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  if ( nullptr == v1 || nullptr == v1->m_edges)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  for (unsigned short v0ei = 0; v0ei < v0->m_edge_count; ++v0ei)
  {
    const MYON_SubDEdgePtr ep = v0->m_edges[v0ei];
    if (v0 == ep.RelativeVertex(0))
    {
      if (v1 == ep.RelativeVertex(1))
        return ep;
    }
    else if (v0 == ep.RelativeVertex(1))
    {
      if (v1 == ep.RelativeVertex(0))
        return ep.Reversed();
    }
    else
    {
      MYON_SUBD_RETURN_ERROR("Invalid MYON_SubDEdgePtr in vertex->m_edge[] array");
    }
  }

  return AllocateEdge(v0, v0_sector_coefficient, v1, v1_sector_coefficient);
}

const MYON_SubDEdgePtr MYON_SubD_FixedSizeHeap::AllocateEdge(
  MYON_SubDVertex* v0,
  double v0_sector_coefficient,
  MYON_SubDVertex* v1,
  double v1_sector_coefficient
  )
{
  if ( nullptr != v0 && nullptr == v0->m_edges)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  if ( nullptr != v1 && nullptr == v1->m_edges)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  if (nullptr == m_e || m_e_index >= m_e_capacity)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);

  bool bTaggedVertex[2];
  if (nullptr != v0)
  {
    if (nullptr == v0->m_edges || v0->m_edge_count >= v0->m_edge_capacity)
      return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
    if (MYON_SubDVertexTag::Smooth == v0->m_vertex_tag)
    {
      bTaggedVertex[0] = false;
      v0_sector_coefficient = MYON_SubDSectorType::IgnoredSectorCoefficient;
    }
    else
    {
      bTaggedVertex[0] = (MYON_SubDVertexTag::Unset != v0->m_vertex_tag);
    }
  }
  else
    bTaggedVertex[0] = false;

  if (nullptr != v1)
  {
    if (nullptr == v1->m_edges || v1->m_edge_count >= v1->m_edge_capacity)
      return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
    if (MYON_SubDVertexTag::Smooth == v1->m_vertex_tag)
    {
      bTaggedVertex[1] = false;
      v1_sector_coefficient = MYON_SubDSectorType::IgnoredSectorCoefficient;
    }
    else
    {
      bTaggedVertex[1] = (MYON_SubDVertexTag::Unset != v0->m_vertex_tag);
      if (bTaggedVertex[0] && bTaggedVertex[1])
      {
        // crease edge - no weights
        v0_sector_coefficient = MYON_SubDSectorType::IgnoredSectorCoefficient;
        v1_sector_coefficient = MYON_SubDSectorType::IgnoredSectorCoefficient;
      }
    }
  }
  else
    bTaggedVertex[1] = false;

  if ( false == MYON_SubDSectorType::IsValidSectorCoefficientValue(v0_sector_coefficient, true))
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);

  if ( false == MYON_SubDSectorType::IsValidSectorCoefficientValue(v1_sector_coefficient, true))
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);

  MYON_SubDEdge* e = m_e + m_e_index;
  memset(e, 0, sizeof(*e));
  if (m_e_index > 0)
  {
    // code in MYON_SubDFaceNeighborhood.Subdivide() relies on m_next_edge being set this way.
    m_e[m_e_index - 1].m_next_edge = e;
    e->m_prev_edge = &m_e[m_e_index - 1];
  }

  e->m_id = ++m_e_index;

  if (nullptr != v0)
  {
    e->m_vertex[0] = v0;
    v0->m_edges[v0->m_edge_count++] = MYON_SubDEdgePtr::Create(e,0);
    //v0->m_vertex_edge_order = MYON_SubD::VertexEdgeOrder::unset;
    e->SetSubdivisionLevel(v0->SubdivisionLevel());
  }

  if (nullptr != v1)
  {
    e->m_vertex[1] = v1;
    v1->m_edges[v1->m_edge_count++] = MYON_SubDEdgePtr::Create(e,1);
    //v1->m_vertex_edge_order = MYON_SubD::VertexEdgeOrder::unset;
    if ( e->SubdivisionLevel() < v1->SubdivisionLevel())
      e->SetSubdivisionLevel(v1->SubdivisionLevel());
  }

  e->m_sector_coefficient[0] = v0_sector_coefficient;
  e->m_sector_coefficient[1] = v1_sector_coefficient;
  e->m_edge_tag = (bTaggedVertex[0] && bTaggedVertex[1]) ? MYON_SubDEdgeTag::Crease : MYON_SubDEdgeTag::Smooth;

  return MYON_SubDEdgePtr::Create(e,0);
}

MYON_SubDFace* MYON_SubD_FixedSizeHeap::Internal_AllocateFace(
  unsigned int level_zero_face_id,
  unsigned int parent_face_id
  )
{
  if (nullptr == m_f || m_f_index >= m_f_capacity)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON_SubDFace* f = m_f + m_f_index;
  memset(f, 0, sizeof(*f));
  if (m_f_index > 0)
  {
    // code in MYON_SubDFaceNeighborhood.Subdivide() relies on
    // m_next_face being set this way.
    m_f[m_f_index-1].m_next_face = f;
    f->m_prev_face = &m_f[m_f_index-1];
  }

  f->m_id = ++m_f_index;
  f->ClearPerFaceColor();
  f->m_level_zero_face_id = (0 == level_zero_face_id) ? parent_face_id : level_zero_face_id;

  return f;
}


MYON_SubDFace* MYON_SubD_FixedSizeHeap::AllocateQuad(
  unsigned int zero_face_id,
  unsigned int parent_face_id,
  MYON_SubDEdgePtr e0,
  MYON_SubDEdgePtr e1,
  MYON_SubDEdgePtr e2,
  MYON_SubDEdgePtr e3
)
{
  const MYON_SubDEdgePtr eptrs[4] = { e0,e1,e2,e3 };
  return AllocateQuad(zero_face_id, parent_face_id, eptrs);
}

MYON_SubDFace* MYON_SubD_FixedSizeHeap::AllocateQuad(
  unsigned int zero_face_id,
  unsigned int parent_face_id,
  const MYON_SubDEdgePtr eptrs[4]
  )
{
  if (nullptr == eptrs)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDEdge* edges[4] = {
    MYON_SUBD_EDGE_POINTER(eptrs[0].m_ptr),
    MYON_SUBD_EDGE_POINTER(eptrs[1].m_ptr),
    MYON_SUBD_EDGE_POINTER(eptrs[2].m_ptr),
    MYON_SUBD_EDGE_POINTER(eptrs[3].m_ptr)};

  if (nullptr == edges[0] || edges[0]->m_face_count > 1)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[1] || edges[1]->m_face_count > 1)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[2] || edges[2]->m_face_count > 1)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edges[3] || edges[3]->m_face_count > 1)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON__UINT_PTR edgedirs[4] = {
    MYON_SUBD_EDGE_DIRECTION(eptrs[0].m_ptr),
    MYON_SUBD_EDGE_DIRECTION(eptrs[1].m_ptr),
    MYON_SUBD_EDGE_DIRECTION(eptrs[2].m_ptr),
    MYON_SUBD_EDGE_DIRECTION(eptrs[3].m_ptr)};

  MYON_SubDVertex* vertices[4] = {
    const_cast<MYON_SubDVertex*>(edges[0]->m_vertex[edgedirs[0]]),
    const_cast<MYON_SubDVertex*>(edges[1]->m_vertex[edgedirs[1]]),
    const_cast<MYON_SubDVertex*>(edges[2]->m_vertex[edgedirs[2]]),
    const_cast<MYON_SubDVertex*>(edges[3]->m_vertex[edgedirs[3]]) };

  if (nullptr == vertices[0] || nullptr == vertices[0]->m_faces || vertices[0]->m_face_count >= vertices[0]->m_face_capacity || vertices[0] != edges[3]->m_vertex[1-edgedirs[3]])
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[1] || nullptr == vertices[1]->m_faces || vertices[1]->m_face_count >= vertices[1]->m_face_capacity || vertices[1] != edges[0]->m_vertex[1-edgedirs[0]])
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[2] || nullptr == vertices[2]->m_faces || vertices[2]->m_face_count >= vertices[2]->m_face_capacity || vertices[2] != edges[1]->m_vertex[1-edgedirs[1]])
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == vertices[3] || nullptr == vertices[3]->m_faces || vertices[3]->m_face_count >= vertices[3]->m_face_capacity || vertices[3] != edges[2]->m_vertex[1-edgedirs[2]])
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDFace* f = Internal_AllocateFace(zero_face_id,parent_face_id);
  if (nullptr == f)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  f->m_edge_count = 4;
  f->m_edge4[0] = eptrs[0];
  f->m_edge4[1] = eptrs[1];
  f->m_edge4[2] = eptrs[2];
  f->m_edge4[3] = eptrs[3];

  edges[0]->m_face2[edges[0]->m_face_count++] = MYON_SubDFacePtr::Create(f,edgedirs[0]);
  edges[1]->m_face2[edges[1]->m_face_count++] = MYON_SubDFacePtr::Create(f,edgedirs[1]);
  edges[2]->m_face2[edges[2]->m_face_count++] = MYON_SubDFacePtr::Create(f,edgedirs[2]);
  edges[3]->m_face2[edges[3]->m_face_count++] = MYON_SubDFacePtr::Create(f,edgedirs[3]);

  vertices[0]->m_faces[vertices[0]->m_face_count++] = f;
  //vertices[0]->m_vertex_edge_order = MYON_SubD::VertexEdgeOrder::unset;
  vertices[1]->m_faces[vertices[1]->m_face_count++] = f;
  //vertices[1]->m_vertex_edge_order = MYON_SubD::VertexEdgeOrder::unset;
  vertices[2]->m_faces[vertices[2]->m_face_count++] = f;
  //vertices[2]->m_vertex_edge_order = MYON_SubD::VertexEdgeOrder::unset;
  vertices[3]->m_faces[vertices[3]->m_face_count++] = f;
  //vertices[3]->m_vertex_edge_order = MYON_SubD::VertexEdgeOrder::unset;

  f->SetSubdivisionLevel( edges[0]->SubdivisionLevel() );

  return f;
}

MYON__UINT_PTR* MYON_SubD_FixedSizeHeap::AllocatePtrArray(
  unsigned int capacity,
  bool bZeroMemory
  )
{
  if (0 == capacity)
    return nullptr;

  if (nullptr == m_p || capacity + m_p_index > m_p_capacity)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON__UINT_PTR* p = m_p + m_p_index;
  m_p_index += capacity;

  if (bZeroMemory)
  {
    MYON__UINT_PTR* p1 = p + capacity;
    while (p1 > p)
    {
      *(--p1) = 0;
    }
  }

  return p;
}

bool MYON_SubD_FixedSizeHeap::ReturnPtrArray(
  void* p,
  unsigned int capacity
  )
{
  if (nullptr != m_p && capacity <= m_p_index && p == m_p + (m_p_index - capacity))
  {
    m_p_index -= capacity;
    return true;
  }
  return MYON_SUBD_RETURN_ERROR(false);
}


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDHeap
//

size_t MYON_SubDHeap::m_offset_vertex_id = 0;
size_t MYON_SubDHeap::m_offset_edge_id = 0;
size_t MYON_SubDHeap::m_offset_face_id = 0;

MYON_SubDHeap::MYON_SubDHeap()
{
  m_fspv.Create(sizeof(class MYON_SubDVertex), 0, 0);
  m_fspe.Create(sizeof(class MYON_SubDEdge), 0, 0);
  m_fspf.Create(sizeof(class MYON_SubDFace), 0, 0);
  m_fsp5.Create(5 * sizeof(MYON__UINT_PTR), 0, 0);
  m_fsp9.Create(9 * sizeof(MYON__UINT_PTR), 0, 0);
  m_fsp17.Create(17 * sizeof(MYON__UINT_PTR), 0, 0);

  if (0 == MYON_SubDHeap::m_offset_vertex_id)
  {
    MYON_SubDVertex v;
    MYON_SubDHeap::m_offset_vertex_id = ((const char*)(&v.m_id)) - ((const char*)&v);
    MYON_SubDEdge e;
    MYON_SubDHeap::m_offset_edge_id = ((const char*)(&e.m_id)) - ((const char*)&e);
    MYON_SubDFace f;
    MYON_SubDHeap::m_offset_face_id = ((const char*)(&f.m_id)) - ((const char*)&f);
  }
}

MYON_SubDHeap::~MYON_SubDHeap()
{
  Destroy();
}

class MYON_SubDComponentBase* MYON_SubDHeap::Internal_AllocateComponentAndSetId(
  MYON_FixedSizePool& fspc,
  MYON_SubDComponentBase*& unused_list,
  unsigned int& max_id,
  unsigned int candidate_id
)
{
  // fspc is a m_fspv / m_fspe / m_fspf fixed size pool on an MYON_SubDHeap.
  // unused_list is the corresponding m_unused_vertex / m_unused_edge / m_unused_face list on that MYON_SubDHeap.
  

  // In order for m_fspv.ElementFromId(), m_fspe.ElementFromId() , m_fspf.ElementFromId() 
  // to work, it is critical that once a vertex/edge/face is allocated from m_fspv/mfspe/mfspf
  // the value of m_id never changes.  This is imporant because the value of m_id must persist
  // in binary archives in order for MYON_COMPONENT_INDEX values to persist in binary archives.

  MYON_SubDComponentBaseLink* c;
  if (candidate_id >3000000000U)
  {
    // Requests for a candidate_id value above 3 billion are ignored to insure
    // there is plenty of room for ids. 
    // It's almost certainly a bug if candidate_id > several millon or so.
    candidate_id = 0; 
  }

  if (nullptr != unused_list && candidate_id <= max_id)
  {
    MYON_SubDComponentBaseLink* prev = nullptr;

    if (candidate_id > 0 && candidate_id != unused_list->m_id)
    {
      // Caller wants a specific id. If it's found here, the context is probably
      // some editing code where the caller deleted the component and now wants it back
      // to preserve the id structure.

      for (prev = static_cast<MYON_SubDComponentBaseLink*>(unused_list); nullptr != prev; prev = const_cast<MYON_SubDComponentBaseLink*>(prev->m_next))
      {
        // If candidate_id is somewhere in the unused list after the first element, return it.
        if (nullptr != prev->m_next && candidate_id == prev->m_next->m_id)
          break;
      }
    }

    if (nullptr != prev)
    {
      // The candidate was found somewhere in the unused_list after the first element.
      c = const_cast<MYON_SubDComponentBaseLink*>(prev->m_next);
      prev->m_next = c->m_next;
    }
    else
    {
      // Return element at the head of the unusued list.
      c = static_cast<MYON_SubDComponentBaseLink*>(unused_list);
      unused_list = const_cast<MYON_SubDComponentBaseLink*>(c->m_next);
    }
    const unsigned int id = c->m_id;
    if (MYON_UNSET_UINT_INDEX == (&c->m_id)[1] && c->m_status.IsDeleted() )
    {
      // When a vertex/edge/face is put on the unused list, m_archive_id is set to MYON_UNSET_UINT_INDEX and m_status = MYON_ComponentStatus::Deleted.
      memset(c, 0, fspc.SizeofElement());
      c->m_id = id;
    }
    else
    {
      // Something is modifying returned elements. This is a serious bug.
      MYON_SubDIncrementErrorCount();
      memset(c, 0, fspc.SizeofElement());
      c->m_id = ++max_id;
    }
  }
  else
  {
    if (candidate_id > max_id)
    {
      // Caller wants a specific id. This is common when copying subds
      // and some of the components of the original subd were deleted.
      max_id = candidate_id;
    }
    else
    {
#if defined(MYON_DEBUG)
      // TEMPORARY ERROR CHECK added Feb 2020 to test new code. Can be removed in April 2020 or earlier if needed.
      // Ask Dale Lear if confused.
      if (0 != candidate_id)
      {
        MYON_SUBD_ERROR("Unable to assign candidate_id");
      }
#endif
      // otherwise assign the next id to this component.
      candidate_id = ++max_id;
    }

    // allocate a new vertex.
    c = (MYON_SubDComponentBaseLink*)fspc.AllocateElement();
    c->m_id = candidate_id;
  }
  return c;
}


class MYON_SubDVertex* MYON_SubDHeap::AllocateVertexAndSetId(unsigned int candidate_vertex_id)
{
  MYON_SubDComponentBase* unused_list = m_unused_vertex;
  MYON_SubDComponentBase* c = MYON_SubDHeap::Internal_AllocateComponentAndSetId(
    m_fspv,
    unused_list,
    m_max_vertex_id,
    candidate_vertex_id
  );
  m_unused_vertex = static_cast<MYON_SubDVertex*>(unused_list);
  return static_cast<MYON_SubDVertex*>(c);
}

void MYON_SubDHeap::ReturnVertex(class MYON_SubDVertex* v)
{
  if (nullptr != v)
  {
    ReturnVertexEdgeAndFaceArrays(v);
    (&v->m_id)[1] = MYON_UNSET_UINT_INDEX; // m_archive_id == MYON_UNSET_UINT_INDEX marks the fixed size pool element as unused
    v->m_status = MYON_ComponentStatus::Deleted;
    v->m_next_vertex = m_unused_vertex;
    m_unused_vertex = v;
    // It is critical that v->m_symmetry_set_next remains set
    // so deleted elements of symmetric SubDs can be found.

    // NO! // m_fspv.ReturnElement(v);
    // See comments in AllocateVertexAndSetId();
  }
}

class MYON_SubDEdge* MYON_SubDHeap::AllocateEdgeAndSetId(
  unsigned int candidate_edge_id
  )
{
  MYON_SubDComponentBase* unused_list = m_unused_edge;
  MYON_SubDComponentBase* c = MYON_SubDHeap::Internal_AllocateComponentAndSetId(
    m_fspe,
    unused_list,
    m_max_edge_id,
    candidate_edge_id
  );
  m_unused_edge = static_cast<MYON_SubDEdge*>(unused_list);
  return static_cast<MYON_SubDEdge*>(c);
}

void MYON_SubDHeap::ReturnEdge(class MYON_SubDEdge* e)
{
  if (nullptr != e)
  {
    if (nullptr != e->m_facex)
      ReturnArray(e->m_facex_capacity,(MYON__UINT_PTR*)e->m_facex);
    (&e->m_id)[1] = MYON_UNSET_UINT_INDEX; // m_archive_id == MYON_UNSET_UINT_INDEX marks the fixed size pool element as unused
    e->m_status = MYON_ComponentStatus::Deleted;
    e->m_next_edge = m_unused_edge;
    m_unused_edge = e;
    // It is critical that e->m_symmetry_set_next remains set
    // so deleted elements of symmetric SubDs can be found.

    // NO! // m_fspe.ReturnElement(e);
    // See comments in AllocateVertexAndSetId();
  }
}

class MYON_SubDFace* MYON_SubDHeap::AllocateFaceAndSetId(
  unsigned int candidate_face_id
)
{
  MYON_SubDComponentBase* unused_list = m_unused_face;
  MYON_SubDFace* f = static_cast<MYON_SubDFace*>( MYON_SubDHeap::Internal_AllocateComponentAndSetId(
    m_fspf,
    unused_list,
    m_max_face_id,
    candidate_face_id
  ) );
  m_unused_face = static_cast<MYON_SubDFace*>(unused_list);
  if (nullptr != f)
    f->ClearPerFaceColor();
  return f;
}

void MYON_SubDHeap::ReturnFace(class MYON_SubDFace* f)
{
  if (nullptr != f)
  {
    if (f->m_texture_points)
    {
      this->Return3dPointArray(f->m_texture_points);
      f->m_texture_points = nullptr;
      f->m_texture_status_bits = 0;
    }
    ReturnArray(f->m_edgex_capacity,(MYON__UINT_PTR*)f->m_edgex);
    (&f->m_id)[1] = MYON_UNSET_UINT_INDEX; // m_archive_id == MYON_UNSET_UINT_INDEX marks the fixed size pool element as unused
    f->m_status = MYON_ComponentStatus::Deleted;
    f->m_next_face = m_unused_face;
    m_unused_face = f;
    // It is critical that f->m_symmetry_set_next remains set
    // so deleted elements of symmetric SubDs can be found.

    // NO! // m_fspf.ReturnElement(f);
    // See comments in AllocateVertexAndSetId();
  }
}

void MYON_SubDHeap::Clear()
{
  class tagWSItem* p = m_ws;
  m_ws = 0;
  while (p)
  {
    class tagWSItem* next = p->m_next;
    onfree(p);
    p = next;
  }

  m_fspv.ReturnAll();
  m_fspe.ReturnAll();
  m_fspf.ReturnAll();

  m_fsp5.ReturnAll();
  m_fsp9.ReturnAll();
  m_fsp17.ReturnAll();

  m_full_fragment_display_density = 0;
  m_full_fragment_count_estimate = 0;
  m_part_fragment_count_estimate = 0;

  m_fsp_full_fragments.ReturnAll();
  m_fsp_part_fragments.ReturnAll();
  m_fsp_oddball_fragments.ReturnAll();
  m_fsp_limit_curves.ReturnAll();

  const size_t frag_size_count = sizeof(m_unused_fragments) / sizeof(m_unused_fragments[0]);
  for (size_t i = 0; i < frag_size_count; ++i)
    m_unused_fragments[i] = nullptr;

  m_unused_vertex = nullptr;
  m_unused_edge = nullptr;
  m_unused_face = nullptr;

  m_max_vertex_id = 0;
  m_max_edge_id = 0;
  m_max_face_id = 0;
}

void MYON_SubDHeap::Destroy()
{
  Clear();
  m_fspv.Destroy();
  m_fspe.Destroy();
  m_fspf.Destroy();
  m_fsp5.Destroy();
  m_fsp9.Destroy();
  m_fsp17.Destroy();
}

void MYON_SubDHeap::ClearArchiveId()
{
  MYON_FixedSizePoolIterator fit;
  fit.Create(&m_fspv);
  for (MYON_SubDVertex* v = (MYON_SubDVertex*)fit.FirstElement(); nullptr != v; v = (MYON_SubDVertex*)fit.NextElement())
  {
    if ( MYON_UNSET_UINT_INDEX != v->ArchiveId())
      v->SetArchiveId(0);
  }
  fit.Create(&m_fspe);
  for (MYON_SubDEdge* e = (MYON_SubDEdge*)fit.FirstElement(); nullptr != e; e = (MYON_SubDEdge*)fit.NextElement())
  {
    if ( MYON_UNSET_UINT_INDEX != e->ArchiveId())
      e->SetArchiveId(0);
  }
  fit.Create(&m_fspf);
  for (MYON_SubDFace* f = (MYON_SubDFace*)fit.FirstElement(); nullptr != f; f = (MYON_SubDFace*)fit.NextElement())
  {
    if ( MYON_UNSET_UINT_INDEX != f->ArchiveId())
      f->SetArchiveId(0);
  }
}

const class MYON_SubDVertex* MYON_SubDHeap::VertexFromId(
  unsigned int vertex_id
  ) const
{
  if ( 0 == vertex_id || MYON_UNSET_UINT_INDEX == vertex_id)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  const class MYON_SubDVertex* vertex = (const class MYON_SubDVertex*)m_fspv.ElementFromId(MYON_SubDHeap::m_offset_vertex_id,vertex_id);
  if ( nullptr == vertex || vertex_id != vertex->m_id)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( MYON_UNSET_UINT_INDEX == vertex->ArchiveId() )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  return vertex;
}

const class MYON_SubDEdge* MYON_SubDHeap::EdgeFromId(
  unsigned int edge_id
  ) const
{
  if ( 0 == edge_id || MYON_UNSET_UINT_INDEX == edge_id)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  const class MYON_SubDEdge* edge = (const class MYON_SubDEdge*)m_fspe.ElementFromId(MYON_SubDHeap::m_offset_edge_id,edge_id);
  if ( nullptr == edge || edge_id != edge->m_id)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( MYON_UNSET_UINT_INDEX == edge->ArchiveId() )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  return edge;
}

const class MYON_SubDFace* MYON_SubDHeap::FaceFromId(
  unsigned int face_id
  ) const
{
  if ( 0 == face_id || MYON_UNSET_UINT_INDEX == face_id)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  const class MYON_SubDFace* face = (const class MYON_SubDFace*)m_fspf.ElementFromId(MYON_SubDHeap::m_offset_face_id,face_id);
  if ( nullptr == face || face_id != face->m_id)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( MYON_UNSET_UINT_INDEX == face->ArchiveId() )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  return face;
}

static bool MYON_SubDHeapIsNotValid(bool bSilentError)
{
  MYON_SubDIncrementErrorCount();
  return bSilentError ? false : MYON_IsNotValid();
}

bool MYON_SubDHeap::IsValid(
  bool bSilentError,
  MYON_TextLog* text_log
) const
{
  if (false == m_fspv.ElementIdIsIncreasing(MYON_SubDHeap::m_offset_vertex_id))
  {
    if (nullptr != text_log)
      text_log->Print("m_fspv.ElementIdIsIncreasing() is false.");
    return MYON_SubDHeapIsNotValid(bSilentError);
  }

  if (false == m_fspe.ElementIdIsIncreasing(MYON_SubDHeap::m_offset_edge_id))
  {
    if (nullptr != text_log)
      text_log->Print("m_fspe.ElementIdIsIncreasing() is false.");
    return MYON_SubDHeapIsNotValid(bSilentError);
  }

  if (false == m_fspf.ElementIdIsIncreasing(MYON_SubDHeap::m_offset_face_id))
  {
    if (nullptr != text_log)
      text_log->Print("m_fspf.ElementIdIsIncreasing() is false.");
    return MYON_SubDHeapIsNotValid(bSilentError);
  }

  const unsigned max_fspv_max_id = m_fspv.MaximumElementId(MYON_SubDHeap::m_offset_vertex_id);
  if (m_max_vertex_id != max_fspv_max_id)
  {
    if (nullptr != text_log)
      text_log->Print("m_max_vertex_id = %u != %u = m_fspv.MaximumElementId()\n", m_max_vertex_id, max_fspv_max_id);
    return MYON_SubDHeapIsNotValid(bSilentError);
  }

  const unsigned max_fspe_max_id = m_fspe.MaximumElementId(MYON_SubDHeap::m_offset_edge_id);
  if (m_max_edge_id != max_fspe_max_id)
  {
    if (nullptr != text_log)
      text_log->Print("m_max_edge_id = %u != %u = m_fspe.MaximumElementId()\n", m_max_edge_id, max_fspe_max_id);
    return MYON_SubDHeapIsNotValid(bSilentError);
  }

  const unsigned max_fspf_max_id = m_fspf.MaximumElementId(MYON_SubDHeap::m_offset_face_id);
  if (m_max_face_id != max_fspf_max_id)
  {
    if (nullptr != text_log)
      text_log->Print("m_max_face_id = %u != %u = m_fspf.MaximumElementId()\n", m_max_face_id, max_fspf_max_id);
    return MYON_SubDHeapIsNotValid(bSilentError);
  }

  return true;
}

void MYON_SubDHeap::ResetIds()
{
  const unsigned int first_id = 1;
  const unsigned int next_vertex_id = m_fspv.ResetElementId(MYON_SubDHeap::m_offset_vertex_id,first_id);
  const unsigned int next_edge_id = m_fspe.ResetElementId(MYON_SubDHeap::m_offset_edge_id,first_id);
  const unsigned int next_face_id = m_fspf.ResetElementId(MYON_SubDHeap::m_offset_face_id,first_id);

  // m_max_..._id  =  maximum assigned id = m_next_..._id - 1
  m_max_vertex_id = (next_vertex_id > first_id) ? (next_vertex_id - 1U) : 0U;
  m_max_edge_id = (next_edge_id > first_id) ? (next_edge_id - 1U) : 0U;
  m_max_face_id = (next_face_id > first_id) ? (next_face_id - 1U) : 0U;
}

MYON_FixedSizePool* MYON_SubDHeap::Internal_ComponentFixedSizePool(
  MYON_SubDComponentPtr::Type component_type
)
{
  switch (component_type)
  {
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  case MYON_SubDComponentPtr::Type::Vertex:
    return &m_fspv;
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    return &m_fspe;
    break;
  case MYON_SubDComponentPtr::Type::Face:
    return &m_fspf;
    break;
  default:
    break;
  }
  return nullptr;
}

const MYON_FixedSizePool* MYON_SubDHeap::Internal_ComponentFixedSizePool(
  MYON_SubDComponentPtr::Type component_type
) const
{
  switch (component_type)
  {
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  case MYON_SubDComponentPtr::Type::Vertex:
    return &m_fspv;
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    return &m_fspe;
    break;
  case MYON_SubDComponentPtr::Type::Face:
    return &m_fspf;
    break;
  default:
    break;
  }
  return nullptr;
}


size_t MYON_SubDHeap::OversizedElementCapacity(size_t count)
{
  size_t capacity = 32 * (count / 32);
  if (count % 32 > 0 || 0 == count)
    capacity += 32;
  return capacity;
}

MYON__UINT_PTR* MYON_SubDHeap::AllocateOversizedElement(size_t* capacity)
{
  class tagWSItem* p;
  size_t actual_capacity = MYON_SubDHeap::OversizedElementCapacity(*capacity);
  size_t sz = (actual_capacity + 1)*sizeof(MYON__UINT_PTR);
  sz += sizeof(*p);
  p = (class tagWSItem*)onmalloc(sz);
  p->m_next = m_ws;
  if (p->m_next)
    p->m_next->m_prev = p;
  p->m_prev = 0;
  m_ws = p;
  MYON__UINT_PTR* a = (MYON__UINT_PTR*)(p + 1);
  *a++ = actual_capacity;
  *capacity = actual_capacity;
  return a;
}

void MYON_SubDHeap::ReturnOversizedElement(
  size_t capacity,
  MYON__UINT_PTR* a
  )
{
  if (0 != a && capacity > 0)
  {
    class tagWSItem* p = ((class tagWSItem*)(a - 1)) - 1;
    if (p == m_ws)
    {
      if (nullptr != p->m_next)
      {
        m_ws = p->m_next;
        p->m_next->m_prev = 0;
      }
      else
        m_ws = nullptr;
    }
    else
    {
      if (p->m_next)
        p->m_next->m_prev = p->m_prev;
      p->m_prev->m_next = p->m_next;
    }
    onfree(p);
  }
}

MYON__UINT_PTR* MYON_SubDHeap::ResizeArray(
  size_t current_count,
  size_t current_capacity,
  MYON__UINT_PTR* current_a,
  size_t* new_capacity
  )
{
  MYON__UINT_PTR capacity = MYON_SubDHeap::ArrayCapacity(current_capacity,current_a);
  if (capacity <= 0)
  {
    return (MYON__UINT_PTR*)AllocateArray(new_capacity);
  }

  if (*new_capacity <= 0)
  {
    ReturnArray(current_capacity,current_a);
    *new_capacity = 0;
    return nullptr;
  }

  if (*new_capacity <= capacity)
  {
    return current_a;
  }

  MYON__UINT_PTR* new_a = AllocateArray(new_capacity);
  MYON__UINT_PTR* a1 = new_a + current_count;
  while (new_a < a1)
  {
    *new_a++ = *current_a++;
  }
  ReturnArray(current_capacity,current_a - current_count);
  return (a1 - current_count);
}

bool MYON_SubDHeap::GrowVertexEdgeArray(
  MYON_SubDVertex* v,
  size_t capacity
  )
{
  if ( nullptr == v)
    return MYON_SUBD_RETURN_ERROR(false);
  if ( 0 == capacity )
    capacity = v->m_edge_count+1;
  if ( capacity <= v->m_edge_capacity)
    return true;
  MYON__UINT_PTR* a = ResizeArray(v->m_edge_count,v->m_edge_capacity,(MYON__UINT_PTR*)v->m_edges,&capacity);
  if ( nullptr == a )
  {
    v->m_edge_count = 0;
    v->m_edge_capacity = 0;
    v->m_edges = 0;
    return MYON_SUBD_RETURN_ERROR(false);
  }
  v->m_edges = (MYON_SubDEdgePtr*)a;
  v->m_edge_capacity = (unsigned short)capacity;
  return true;
}

bool MYON_SubDHeap::GrowVertexFaceArray(
  MYON_SubDVertex* v,
  size_t capacity
  )
{
  if ( nullptr == v)
    return MYON_SUBD_RETURN_ERROR(false);
  if ( 0 == capacity )
    capacity = v->m_face_count+1;
  if ( capacity <= v->m_face_capacity)
    return true;
  MYON__UINT_PTR* a = ResizeArray(v->m_face_count,v->m_face_capacity,(MYON__UINT_PTR*)v->m_faces,&capacity);
  if (nullptr == a)
  {
    v->m_face_count = 0;
    v->m_face_capacity = 0;
    v->m_faces = nullptr;
    return MYON_SUBD_RETURN_ERROR(false);
  }
  v->m_faces = (const MYON_SubDFace**)a;
  v->m_face_capacity = (unsigned short)capacity;
  return true;
}

bool MYON_SubDHeap::GrowEdgeFaceArray(
  MYON_SubDEdge* e,
  size_t capacity
  )
{
  if ( nullptr == e)
    return MYON_SUBD_RETURN_ERROR(false);
  if ( 0 == capacity )
    capacity = e->m_face_count+1;
  if ( capacity <= (size_t)(2 + e->m_facex_capacity))
    return true;
  size_t xcapacity = capacity-2;
  MYON__UINT_PTR* a = ResizeArray((e->m_face_count>2) ? (e->m_face_count-2) : 0,e->m_facex_capacity,(MYON__UINT_PTR*)e->m_facex,&xcapacity);
  if ( nullptr == a )
  {
    e->m_face_count = 0;
    e->m_facex_capacity = 0;
    e->m_facex = nullptr;
    return MYON_SUBD_RETURN_ERROR(false);
  }
  e->m_facex = (MYON_SubDFacePtr*)a;
  e->m_facex_capacity = (unsigned short)xcapacity;
  return true;
}

bool MYON_SubDHeap::GrowFaceEdgeArray(
  MYON_SubDFace* f,
  size_t capacity
  )
{
  if ( nullptr == f)
    return MYON_SUBD_RETURN_ERROR(false);

  // The capacity of f->m_texture_points[] must always be 4 + f->m_edgex_capacity
  const size_t texture_point_capacity0 = f->TexturePointsCapacity();

  if ( 0 == capacity )
    capacity = f->m_edge_count+1;
  if ( capacity <= (size_t)(4 + f->m_edgex_capacity))
    return true;
  size_t xcapacity = capacity-4;
  MYON__UINT_PTR* a = ResizeArray((f->m_edge_count>4) ? (f->m_edge_count-4) : 0,f->m_edgex_capacity,(MYON__UINT_PTR*)f->m_edgex,&xcapacity);
  if ( nullptr == a )
  {
    f->m_edge_count = 0;
    f->m_edgex_capacity = 0;
    f->m_edgex = nullptr;
    // also remove texture points
    f->m_texture_status_bits &= MYON_SubDFace::TextureStatusBits::NotTexturePointsBitsMask;
    f->m_texture_points = nullptr;
    return MYON_SUBD_RETURN_ERROR(false);
  }
  f->m_edgex = (MYON_SubDEdgePtr*)a;
  f->m_edgex_capacity = (unsigned short)xcapacity;
  if (texture_point_capacity0 > 0)
  {
    const size_t texture_point_capacity1 = 4 + xcapacity;
    if (texture_point_capacity0 < texture_point_capacity1)
    {
      MYON_3dPoint* texture_points0 = f->m_texture_points;
      MYON_3dPoint* texture_points1 = this->Allocate3dPointArray(texture_point_capacity1);
      for (size_t i = 0; i < texture_point_capacity0; ++i)
        texture_points1[i] = texture_points0[i];
      for (size_t i = texture_point_capacity0; i < texture_point_capacity1; ++i)
        texture_points1[i] = MYON_3dPoint::NanPoint;
      f->m_texture_points = texture_points1;
      this->Return3dPointArray(texture_points0);
    }
  }
  return true;
}

unsigned int MYON_SubDHeap::Managed3dPointArrayCapacity(class MYON_3dPoint* point_array)
{
  const unsigned int point_capacity =
    (nullptr != point_array)
    ? *((unsigned int*)(((const double*)point_array)-1))
    : 0;
  return (point_capacity >= 3 && point_capacity <= MYON_SubDFace::MaximumEdgeCount) ? point_capacity : 0;
}

MYON_3dPoint* MYON_SubDHeap::Allocate3dPointArray(size_t point_capacity)
{
  if (point_capacity <= 0 || point_capacity > MYON_SubDFace::MaximumEdgeCount)
    return nullptr;
#if defined(MYON_64BIT_RUNTIME)
  if (point_capacity < 5)
    point_capacity = 5; // maximize use of m_fsp17 chunks.
#endif
  const size_t a_capacity = 3 * point_capacity + 1;
  double* a = 
#if defined(MYON_64BIT_RUNTIME)
    // when sizeof(void*) = sizeof(double) we can use the fast fixed size pool for faces with 5 or fewer edges.
    ( a_capacity*sizeof(a[0]) <= m_fsp17.SizeofElement() ) 
    ? (double*)this->m_fsp17.AllocateDirtyElement()
    :
#endif
    a = (double*)onmalloc(a_capacity * sizeof(a[0]));

  if (nullptr != a)
  {
    *((unsigned int*)a) = ((unsigned int)point_capacity);
    ++a;
  }
  return ((MYON_3dPoint*)a);
}

void MYON_SubDHeap::Return3dPointArray(class MYON_3dPoint* point_array)
{
  const size_t point_capacity = MYON_SubDHeap::Managed3dPointArrayCapacity(point_array);
  if (0 == point_capacity)
  {
    MYON_SUBD_ERROR("point_array is not valid");
    return;
  }
  double* a = ((double*)point_array) - 1;
#if defined(MYON_64BIT_RUNTIME)
  // when sizeof(void*) = sizeof(double) we can use the fast fixed size pool for faces with 5 or fewer edges.
  const size_t a_capacity = 3 * point_capacity + 1;
  if ( a_capacity * sizeof(a[0]) <= m_fsp17.SizeofElement() )
    this->m_fsp17.ReturnElement(a);
  else
#endif
    onfree(a);

  return;
}

bool MYON_SubDHeap::GrowVertexEdgeArrayByOne(
  MYON_SubDVertex* v
  )
{
  return GrowVertexEdgeArray(v,0);
}

bool MYON_SubDHeap::GrowVertexFaceArrayByOne(
  MYON_SubDVertex* v
  )
{
  return GrowVertexFaceArray(v,0);
}

bool MYON_SubDHeap::GrowEdgeFaceArrayByOne(
  MYON_SubDEdge* e
  )
{
  return GrowEdgeFaceArray(e,0);
}

bool MYON_SubDHeap::GrowFaceEdgeArrayByOne(
  MYON_SubDFace* f
  )
{
  return GrowFaceEdgeArray(f,0);
}

bool MYON_SubDimple::GrowVertexEdgeArray(
  MYON_SubDVertex* v,
  size_t capacity
  )
{
  return m_heap.GrowVertexEdgeArray(v,capacity);
}

bool MYON_SubDimple::GrowVertexFaceArray(
  MYON_SubDVertex* v,
  size_t capacity
  )
{
  return m_heap.GrowVertexFaceArray(v,capacity);
}

bool MYON_SubDimple::GrowEdgeFaceArray(
  MYON_SubDEdge* e,
  size_t capacity
  )
{
  return m_heap.GrowEdgeFaceArray(e,capacity);
}

bool MYON_SubDimple::GrowFaceEdgeArray(
  MYON_SubDFace* f,
  size_t capacity
  )
{
  return m_heap.GrowFaceEdgeArray(f,capacity);
}

bool MYON_SubD::GrowVertexEdgeArray(
  MYON_SubDVertex* v,
  size_t capacity
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return MYON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowVertexEdgeArray(v,capacity);
}

bool MYON_SubD::GrowVertexFaceArray(
  MYON_SubDVertex* v,
  size_t capacity
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return MYON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowVertexFaceArray(v,capacity);
}

bool MYON_SubD::GrowEdgeFaceArray(
  MYON_SubDEdge* e,
  size_t capacity
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return MYON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowEdgeFaceArray(e,capacity);
}

bool MYON_SubD::GrowFaceEdgeArray(
  MYON_SubDFace* f,
  size_t capacity
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return MYON_SUBD_RETURN_ERROR(false);
  return subdimple->GrowFaceEdgeArray(f,capacity);
}

MYON__UINT_PTR MYON_SubDHeap::ArrayCapacity(
  size_t capacity,
  MYON__UINT_PTR* a
  )
{
#if defined(MYON_DEBUG)
  size_t acapacity = (nullptr == a) ? 0 : a[-1];
  if (capacity != acapacity)
  {
    MYON_SubDIncrementErrorCount();
  }
#endif
  return (nullptr == a) ? 0 : a[-1];
}

bool MYON_SubDHeap::ReturnVertexEdgeAndFaceArrays(
  MYON_SubDVertex* v
  )
{
  if ( nullptr == v )
    return MYON_SUBD_RETURN_ERROR(false);
  if (nullptr != v->m_edges || v->m_edge_capacity > 0 || v->m_edge_count > 0)
  {
    ReturnArray(v->m_edge_capacity,(MYON__UINT_PTR*)v->m_edges);
    v->m_edges = nullptr;
    v->m_edge_capacity = 0;
    v->m_edge_count = 0;
  }
  if (nullptr != v->m_faces || v->m_face_capacity > 0 || v->m_face_count > 0)
  {
    ReturnArray(v->m_face_capacity,(MYON__UINT_PTR*)v->m_faces);
    v->m_faces = nullptr;
    v->m_face_capacity = 0;
    v->m_face_count = 0;
  }
  return true;

}

bool MYON_SubDHeap::ReturnEdgeExtraArray(
  MYON_SubDEdge* e
  )
{
  if ( nullptr == e )
    return MYON_SUBD_RETURN_ERROR(false);
  if (nullptr != e->m_facex || e->m_facex_capacity > 0)
  {
    ReturnArray(e->m_facex_capacity,(MYON__UINT_PTR*)e->m_facex);
    e->m_facex = nullptr;
    e->m_facex_capacity = 0;
  }
  if (e->m_face_count > 2)
    e->m_face_count = 2;
  return true;
}

bool MYON_SubDHeap::ReturnFaceExtraArray(
  MYON_SubDFace* f
  )
{
  if ( nullptr == f )
    return MYON_SUBD_RETURN_ERROR(false);
  if (nullptr != f->m_edgex || f->m_edgex_capacity > 0)
  {
    ReturnArray(f->m_edgex_capacity,(MYON__UINT_PTR*)f->m_edgex);
    f->m_edgex = nullptr;
    f->m_edgex_capacity = 0;
  }
  if (f->m_edge_count > 4)
    f->m_edge_count = 4;
  return true;
}

MYON__UINT_PTR* MYON_SubDHeap::AllocateArray(size_t* capacity)
{
  MYON__UINT_PTR* a;
  size_t requested_capacity = *capacity;
  if (requested_capacity <= 0)
    return nullptr;

  if (requested_capacity <= 4)
  {
    a = (MYON__UINT_PTR*)m_fsp5.AllocateElement();
    *a++ = 4;
    *capacity = 4;
    return a;
  }

  if (requested_capacity <= 8)
  {
    a = (MYON__UINT_PTR*)m_fsp9.AllocateElement();
    *a++ = 8;
    *capacity = 8;
    return a;
  }

  if (requested_capacity <= 16)
  {
    a = (MYON__UINT_PTR*)m_fsp17.AllocateElement();
    *a++ = 16;
    *capacity = 16;
    return a;
  }

  return  AllocateOversizedElement(capacity);
}

void MYON_SubDHeap::ReturnArray(
  size_t capacity,
  MYON__UINT_PTR* a
  )
{
  if (nullptr != a && 0 == capacity)
  {
    MYON_SubDIncrementErrorCount();
  }

  switch (MYON_SubDHeap::ArrayCapacity(capacity,a))
  {
  case 0:
    break;
  case 4:
    m_fsp5.ReturnElement(a - 1);
    break;
  case 8:
    m_fsp9.ReturnElement(a - 1);
    break;
  case 16:
    m_fsp17.ReturnElement(a - 1);
    break;
  default:
    ReturnOversizedElement(capacity,a);
    break;
  }
  return;
}

bool MYON_SubDHeap::Internal_InitializeLimitCurvesPool()
{
  if (0 == m_fsp_limit_curves.SizeofElement())
  {
    unsigned subd_edge_count = 0;
    // count edge and face to get an estimate of haow many mesh fragments we need to managed.
    MYON_FixedSizePoolIterator fspeit(m_fspe);
    for (const void* p = fspeit.FirstElement(); nullptr != p; p = fspeit.NextElement())
    {
      const MYON_SubDEdge* e = (const MYON_SubDEdge*)p;
      if (false == e->IsActive())
        continue;
      ++subd_edge_count;
    }

    //const size_t sizeof_element = sizeof(MYON_SubDEdgeSurfaceCurve);

    Internal_InitializeMeshFragmentPool(
      sizeof(MYON_SubDEdgeSurfaceCurve),
      subd_edge_count,
      32,
      m_fsp_limit_curves
    );
  }
  return (m_fsp_limit_curves.SizeofElement() > 0);
}

bool MYON_SubDHeap::Internal_InitializeFragmentCountEstimates(
  unsigned subd_display_density
)
{
  if (0 == m_full_fragment_display_density)
  {
    m_full_fragment_display_density 
      = subd_display_density > 0 
      ? (subd_display_density <= MYON_SubDDisplayParameters::MaximumDensity ? subd_display_density : MYON_SubDDisplayParameters::MaximumDensity)
      : 1U
      ;

    // Count the number of active faces and fragments needed to mesh them
    unsigned subd_face_count = 0;
    unsigned full_frag_count = 0;
    unsigned part_frag_count = 0;

    MYON_FixedSizePoolIterator fspfit(m_fspf);
    for (const void* p = fspfit.FirstElement(); nullptr != p; p = fspfit.NextElement())
    {
      const MYON_SubDFace* f = (const MYON_SubDFace*)p;
      if (false == f->IsActive())
        continue;
      ++subd_face_count;
      if (4 == f->m_edge_count)
        ++full_frag_count; // one full fragment per quad
      else
        part_frag_count += f->m_edge_count; // n partial fragments per n-gon when n != 4
    }

    if (full_frag_count > 0 || part_frag_count > 0)
    {
      m_full_fragment_count_estimate = full_frag_count;
      m_part_fragment_count_estimate = part_frag_count;
    }
    else
    {
      m_full_fragment_count_estimate = 0;
      m_part_fragment_count_estimate = 0;
    }
  }

  return (m_full_fragment_display_density > 0 && m_full_fragment_display_density <= MYON_SubDDisplayParameters::MaximumDensity);
}

bool MYON_SubDHeap::Internal_InitializeMeshFragmentPool(
  size_t sizeof_element,
  size_t element_count_estimate,
  size_t min_fsp_2nd_block_element_count,
  MYON_FixedSizePool& fsp // fsp references either m_fsp_*_fragments or m_fsp_limit_curves.
)
{
  if (0 == fsp.SizeofElement() && sizeof_element > 0)
  {
    MYON_SleepLockGuard guard(fsp);
    fsp.CreateForExperts(sizeof_element, element_count_estimate, min_fsp_2nd_block_element_count);
    // check size again in case another thread beat this call
    if (0 == fsp.SizeofElement())
      fsp.CreateForExperts(sizeof_element, element_count_estimate, min_fsp_2nd_block_element_count);
  }
  return (fsp.SizeofElement() > 0);
}

MYON_SubDMeshFragment* MYON_SubDHeap::AllocateMeshFragment(
  unsigned subd_display_density,
  const MYON_SubDMeshFragment& src_fragment
)
{
  if (subd_display_density > MYON_SubDDisplayParameters::MaximumDensity)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  // When 4 == MYON_SubDDisplayParameters::DefaultDensity (setting used in February 2019)
  // quads get a single fragment with a 16x16 face grid
  // N-gons with N != 4 get N 8x8 grids.

  // density = density of src_fragment
  const unsigned int density = (src_fragment.m_face_fragment_count > 1)
    ? ((subd_display_density > 0) ? (subd_display_density -1) : MYON_UNSET_UINT_INDEX)
    : ((1==src_fragment.m_face_fragment_count) ? subd_display_density : MYON_UNSET_UINT_INDEX)
    ;
  if (MYON_UNSET_UINT_INDEX == density)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  const unsigned count = (unsigned)(sizeof(m_unused_fragments) / sizeof(m_unused_fragments[0]));
  if (density >= count)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const unsigned short side_seg_count = (unsigned short)MYON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(density);
  const unsigned short vertex_capacity = (side_seg_count + 1)*(side_seg_count + 1);
  if ( src_fragment.VertexCount() > 0 && src_fragment.VertexCount() < ((unsigned)vertex_capacity) )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (0 == m_full_fragment_display_density)
  {
    // Lazy initialization of m_full_fragment_display_density is done because
    // we don't know the display density when SubDs are being constructed.
    if (false == Internal_InitializeFragmentCountEstimates(subd_display_density))
      return MYON_SUBD_RETURN_ERROR(nullptr);
  }
  if (m_full_fragment_display_density <= 0 || m_full_fragment_display_density >= MYON_SubDDisplayParameters::MaximumDensity)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  // In all common situations, bUseFullFragmentFSP or bUsePartFragmentFSP is true.
  const bool bUseFullFragmentFSP = (density == this->m_full_fragment_display_density);
  const bool bUsePartFragmentFSP = (density+1 == this->m_full_fragment_display_density);

  MYON_FixedSizePool& fsp 
    = bUseFullFragmentFSP
    ? m_fsp_full_fragments
    : bUsePartFragmentFSP ? m_fsp_part_fragments : m_fsp_oddball_fragments;

  if (0 == fsp.SizeofElement())
  {
    // Lazy initialization of the fragment fixed size pools
    // is done so that we don't reserve pool memory that never gets used.

    const size_t sizeof_fragment
      = bUseFullFragmentFSP
      ? MYON_SubDHeap::g_sizeof_fragment[m_full_fragment_display_density]
      : (bUsePartFragmentFSP ? MYON_SubDHeap::g_sizeof_fragment[m_full_fragment_display_density - 1U] : MYON_SubDHeap::g_sizeof_fragment[MYON_SubDDisplayParameters::MaximumDensity])
      ;

    const size_t fragment_count_estimate
      = bUseFullFragmentFSP
      ? m_full_fragment_count_estimate
      : (bUsePartFragmentFSP ? m_part_fragment_count_estimate : ((unsigned)4U))
      ;

    const size_t min_fsp_2nd_block_element_count = (bUseFullFragmentFSP || bUsePartFragmentFSP) ? 32 : 1;

    if (false == this->Internal_InitializeMeshFragmentPool(
      sizeof_fragment,
      fragment_count_estimate,
      min_fsp_2nd_block_element_count,
      fsp
    ))
    {
      return MYON_SUBD_RETURN_ERROR(nullptr);
    }
  }

  MYON_SubDMeshFragment* fragment;
  {
    char* p = nullptr;
    char* p1 = nullptr;
    MYON_SleepLockGuard guard(fsp);
    if (nullptr == m_unused_fragments[density])
    {
      p = (char*)fsp.AllocateDirtyElement();
      if (nullptr == p)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      p1 = p + fsp.SizeofElement();
      m_unused_fragments[density] = (MYON_FixedSizePoolElement*)p;
      m_unused_fragments[density]->m_next = nullptr;
      const size_t sizeof_fragment = MYON_SubDHeap::g_sizeof_fragment[density];
      for (p += sizeof_fragment; p + sizeof_fragment <= p1; p += sizeof_fragment)
      {
        MYON_FixedSizePoolElement* ele = (MYON_FixedSizePoolElement*)p;
        ele->m_next = m_unused_fragments[density];
        m_unused_fragments[density] = ele;          
      }
    }
    fragment = (MYON_SubDMeshFragment*)m_unused_fragments[density];
    m_unused_fragments[density] = m_unused_fragments[density]->m_next;
  }

  *fragment = src_fragment;
  fragment->m_prev_fragment = nullptr;
  fragment->m_next_fragment = nullptr;

  // NOTE WELL:
  //   fragment and fragment array memory are from a single fixed size pool allocation.
  fragment->Internal_LayoutArrays(false, (double*)(fragment + 1), vertex_capacity);

  if (src_fragment.VertexCount() > 0)
    fragment->CopyFrom(src_fragment,density);

  return fragment;
}


MYON_SubDMeshFragment* MYON_SubDHeap::CopyMeshFragments(
  const MYON_SubDFace* source_face,
  unsigned destination_subd_display_density,
  const MYON_SubDFace* destination_face
)
{
  if (nullptr == source_face || nullptr == destination_face || nullptr != destination_face->m_mesh_fragments)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDMeshFragment* prev_dst_fragment = nullptr;
  for (const MYON_SubDMeshFragment* src_fragment = source_face->MeshFragments(); nullptr != src_fragment; src_fragment = src_fragment->m_next_fragment)
  {
    MYON_SubDMeshFragment* dst_fragment = this->AllocateMeshFragment(destination_subd_display_density ,*src_fragment);
    dst_fragment->m_face = destination_face;
    if (prev_dst_fragment)
      prev_dst_fragment->m_next_fragment = dst_fragment;
    else
    {
      destination_face->m_mesh_fragments = dst_fragment;
      destination_face->Internal_SetSavedSurfacePointFlag(true);
    }
    prev_dst_fragment = dst_fragment;
  }
  return destination_face->m_mesh_fragments;
}


bool MYON_SubDHeap::ReturnMeshFragment(MYON_SubDMeshFragment * fragment)
{
  if (nullptr == fragment)
    return false;

  const size_t count = sizeof(m_unused_fragments) / sizeof(m_unused_fragments[0]);
  size_t i;
  switch (fragment->VertexCapacity())
  {
  case 2 * 2: // 1x1 mesh quad fragment
    i = 0;
    break;
  case 3 * 3: // 2x2 mesh quad fragment
    i = 1;
    break;
  case 5 * 5: // 4x4 mesh quad fragment
    i = 2;
    break;
  case 9 * 9: // 8x8 mesh quad fragment
    i = 3;
    break;
  case 17 * 17: // 16x16 mesh quad fragment
    i = 4;
    break;
  case 33 * 33: // 32x32 mesh quad fragment
    i = 5;
    break;
  case 65 * 65: // 64x64 mesh quad fragment
    i = 6;
    break;
  default:
    i = count;
    break;
  }
  if (i >= count)
    return MYON_SUBD_RETURN_ERROR(false);

  MYON_FixedSizePool& fsp 
    = (i == m_full_fragment_display_density) ? m_fsp_full_fragments
    : (i+1 == m_full_fragment_display_density) ? m_fsp_part_fragments : m_fsp_oddball_fragments;

  MYON_FixedSizePoolElement* ele = (MYON_FixedSizePoolElement*)fragment;
  MYON_SleepLockGuard guard(fsp);
  ((unsigned int*)ele)[5] = 0; // zero m_vertex_count_etc and m_vertex_capacity_etc
  ele->m_next = m_unused_fragments[i];
  m_unused_fragments[i] = ele;

  return true;
}

bool MYON_SubDHeap::ReturnMeshFragments(const MYON_SubDFace * face)
{
  if (nullptr != face)
  {
    face->Internal_ClearSurfacePointFlag();
    MYON_SubDMeshFragment* fragment = face->m_mesh_fragments;
    face->m_mesh_fragments = nullptr;
    while (nullptr != fragment)
    {
      if (face != fragment->m_face)
        return MYON_SUBD_RETURN_ERROR(false);
      MYON_SubDMeshFragment* next_fragment = fragment->m_next_fragment;
      if (false == ReturnMeshFragment(fragment))
        return false;
      fragment = next_fragment;
    }
  }
  return true;
}

class MYON_SubDEdgeSurfaceCurve* MYON_SubDHeap::AllocateEdgeSurfaceCurve(
  unsigned int cv_capacity
)
{
  if (cv_capacity < 1 || cv_capacity > MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (0 == this->m_fsp_limit_curves.SizeofElement())
  {
    if( false == this->Internal_InitializeLimitCurvesPool())
      return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  MYON_SubDEdgeSurfaceCurve* limit_curve = nullptr;
  double* cvx = nullptr;
  {
    MYON_SleepLockGuard guard(m_fsp_limit_curves);
    limit_curve = (MYON_SubDEdgeSurfaceCurve*)m_fsp_limit_curves.AllocateDirtyElement();
    if (cv_capacity > MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity)
      cvx = (double*)m_fsp_limit_curves.AllocateDirtyElement();
  }

  if (nullptr != limit_curve)
  {
    memset(limit_curve, 0, sizeof(*limit_curve));
    limit_curve->m_cv_capacity = MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity;
    if (nullptr != cvx)
    {
      // increase capacity
      limit_curve->m_cv_capacity = MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity;
      limit_curve->m_cvx = cvx;
      double* p1 = cvx + 3 * (MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity - MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity);
      while (cvx < p1)
        *cvx++ = MYON_DBL_QNAN;
    }
  }

  return limit_curve;
}

MYON_SubDEdgeSurfaceCurve* MYON_SubDHeap::CopyEdgeSurfaceCurve(const MYON_SubDEdge* source_edge, const MYON_SubDEdge* desination_edge)
{
  if ( nullptr == desination_edge || source_edge == desination_edge)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  desination_edge->Internal_ClearSurfacePointFlag();
  if (source_edge->m_limit_curve == desination_edge->m_limit_curve)
    desination_edge->m_limit_curve = nullptr;
  else if (nullptr != desination_edge->m_limit_curve)
    desination_edge->m_limit_curve->m_cv_count = 0;

  if (nullptr == source_edge)
  {
    ReturnEdgeSurfaceCurve(desination_edge);
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  const MYON_SubDEdgeSurfaceCurve* source_curve = source_edge->Internal_SurfacePointFlag() ? source_edge->m_limit_curve : nullptr;
  const unsigned char cv_count = (nullptr != source_curve) ? source_curve->m_cv_count : 0;
  if (0 == cv_count)
  {
    source_edge->Internal_ClearSurfacePointFlag();
    ReturnEdgeSurfaceCurve(desination_edge);
    return nullptr;
  }

  if (cv_count < 2 || cv_count > MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity || (cv_count > MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity && nullptr == source_curve->m_cvx))
  {
    source_edge->Internal_ClearSurfacePointFlag();
    ReturnEdgeSurfaceCurve(desination_edge);
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  MYON_SubDEdgeSurfaceCurve* desination_curve = desination_edge->m_limit_curve;
  if (nullptr != desination_curve && desination_curve->m_cv_capacity < cv_count)
  {
    ReturnEdgeSurfaceCurve(desination_edge);
    desination_curve = nullptr;
  }
  if (nullptr == desination_curve)
  {
    desination_curve = this->AllocateEdgeSurfaceCurve(cv_count);
    if (nullptr == desination_curve)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    if (desination_curve->m_cv_capacity < cv_count)
    {
      ReturnEdgeSurfaceCurve(desination_curve);
      return MYON_SUBD_RETURN_ERROR(nullptr);
    }
  }
  const size_t sz5 = sizeof(desination_curve->m_cv5);
  memcpy(desination_curve->m_cv5, source_curve->m_cv5, sz5);
  if (cv_count > MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity && nullptr != desination_curve->m_cvx && nullptr != source_curve->m_cvx)
  {
    const size_t szx = ((size_t)(cv_count - MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity)) * 24;
    memcpy(desination_curve->m_cvx, source_curve->m_cvx, szx);
  }
  desination_curve->m_cv_count = cv_count;
  desination_edge->m_limit_curve = desination_curve;
  desination_edge->Internal_SetSavedSurfacePointFlag(true);
  return desination_curve;
}

bool MYON_SubDHeap::ReturnEdgeSurfaceCurve(
  MYON_SubDEdgeSurfaceCurve* limit_curve
)
{
  if (nullptr != limit_curve)
  {
    // zero cv_count and cv_capacity - to limit crashes caused by rogue references
    limit_curve->m_cv_count = 0;
    limit_curve->m_cv_capacity = 0;
    MYON_FixedSizePoolElement* ele0 = (MYON_FixedSizePoolElement*)limit_curve;
    MYON_FixedSizePoolElement* ele1 = (MYON_FixedSizePoolElement*)limit_curve->m_cvx;
    if (nullptr != ele1)
    {
      // zero cv_count and cv_capacity - to limit crashes caused by rogue references
      ((MYON_SubDEdgeSurfaceCurve*)ele1)->m_cv_count = 0;
      ((MYON_SubDEdgeSurfaceCurve*)ele1)->m_cv_capacity = 0;
    }
    MYON_SleepLockGuard guard(m_fsp_limit_curves);
    m_fsp_limit_curves.ReturnElement(ele0);
    if (nullptr != ele1)
      m_fsp_limit_curves.ReturnElement(ele1);
  }
  return true;
}

bool MYON_SubDHeap::ReturnEdgeSurfaceCurve(
  const class MYON_SubDEdge* edge
)
{
  bool rc = true;
  MYON_SubDEdgeSurfaceCurve* limit_curve = (nullptr != edge) ? edge->m_limit_curve : nullptr;
  if (nullptr != limit_curve)
  {
    edge->Internal_ClearSurfacePointFlag();
    edge->m_limit_curve = nullptr;
    rc = ReturnEdgeSurfaceCurve(limit_curve);
  }
  return rc;
}

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

bool MYON_SubDFaceRegionBreakpoint(
  unsigned int level0_face_id,
  const class MYON_SubDComponentRegionIndex& region_index
)
{
#if defined(MYON_DEBUG)
  if (
    8 != level0_face_id
    )
  {
    return false;
  }

  const unsigned short region_pattern[] = { 2, 2 }; // { 2, 2, 1 };
  const unsigned short region_pattern_count = (unsigned short)(sizeof(region_pattern) / sizeof(region_pattern[0]));

  if (region_index.m_subdivision_count < region_pattern_count)
    return false;

  for (unsigned short i = 0; i < region_pattern_count; i++)
  {
    if (region_index.m_index[i] != region_pattern[i])
      return false;
  }

  return true;// <- breakpoint here (or above)
#else
  return false;
#endif
}

bool MYON_SubDComponentRegionBreakpoint(const MYON_SubDComponentRegion* component_region)
{
#if defined(MYON_DEBUG)
  if (nullptr != component_region)
  {
    switch (component_region->m_level0_component.ComponentType())
    {
    case MYON_SubDComponentPtr::Type::Face:
      return MYON_SubDFaceRegionBreakpoint(component_region->m_level0_component_id, component_region->m_region_index);
      break;
    case MYON_SubDComponentPtr::Type::Edge:
      break;
    case MYON_SubDComponentPtr::Type::Vertex:
      break;
    default:
      break;
    }
  }
#endif
  return false;
}

const MYON_SubDComponentRegion MYON_SubDComponentRegion::Create(
    const class MYON_SubDFace* level0_face
)
{
  MYON_SubDComponentRegion r;
  r.m_level0_component = MYON_SubDComponentPtr::Create(level0_face);
  r.m_level0_component_id = (nullptr != level0_face ? level0_face->m_id : 0);
  return r;
}

const MYON_SubDComponentRegion MYON_SubDComponentRegion::Create(
  unsigned int component_id,
  MYON_SubDComponentPtr::Type component_type,
  bool bComponentMark
)
{
  MYON_SubDComponentRegion r;
  r.m_level0_component = MYON_SubDComponentPtr::CreateNull(component_type, bComponentMark);
  r.m_level0_component_id = component_id;
  return r;
}

const MYON_SubDComponentRegion MYON_SubDComponentRegion::CreateSubdivisionRegion(
  MYON_SubDComponentPtr::Type component_type,
  bool bComponentDirection,
  unsigned short subdivision_count,
  bool bAssignTransientId
)
{
  MYON_SubDComponentRegion r;
  r.m_region_index = MYON_SubDComponentRegionIndex::Unset;
  r.m_region_index.m_subdivision_count = subdivision_count;
  r.m_level0_component = MYON_SubDComponentPtr::CreateNull(component_type, bComponentDirection);

  if (bAssignTransientId)
  {
    r.m_level0_component_id = MYON_SubDComponentRegion::NewTransientId();
  }
  return r;
}

////MYON_SubDComponentRegion MYON_SubDComponentRegion::Reverse() const
////{
////  MYON_SubDComponentRegion r(*this);
////   r.m_level0_component = r.m_level0_component.ToggleMark();
////  if (r.m_subdivision_count > 0)
////  {
////    const int c = (int)(sizeof(m_region_index) / sizeof(m_region_index[0]));
////    int i = (int)(r.m_subdivision_count - 1);
////    int j = 0;
////    while (j < c && 0xFFFF == r.m_region_index[j])
////      j++;
////    for ( /*empty init*/; j < i && j < c; ++j,--i)
////    {
////      if (i < c)
////      {
////        unsigned short x = r.m_region_index[i];
////        r.m_region_index[i] = r.m_region_index[j];
////        r.m_region_index[j] = x;
////      }
////      else
////      {
////        r.m_region_index[j] = 0;
////      }
////    }
////  }  
////  return r;
////}
////
////MYON_SubDComponentRegion MYON_SubDComponentRegion::ReverseIfMarked() const
////{
////  return
////    0 != m_level0_component.ComponentMark()
////    ? Reverse()
////    : *this;
////}

int MYON_SubDComponentRegion::CompareTypeIdDirection(
  const MYON_SubDComponentRegion* lhs,
  const MYON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == rhs)
    return 1;
  if (nullptr == lhs)
    return -1;

  int rc = MYON_SubDComponentPtr::CompareType(&lhs->m_level0_component, &rhs->m_level0_component);
  if (0 != rc)
    return rc;

  if (lhs->m_level0_component_id < rhs->m_level0_component_id)
    return -1;
  if (lhs->m_level0_component_id > rhs->m_level0_component_id)
    return 1;

  rc = (0 != lhs->m_level0_component.ComponentDirection() ? (int)1 : (int)0) - (0 != lhs->m_level0_component.ComponentDirection() ? (int)1 : (int)0);
  if (0 != rc)
    return rc;

  return 0;
}

int MYON_SubDComponentRegionIndex::Compare(
  const MYON_SubDComponentRegionIndex* lhs,
  const MYON_SubDComponentRegionIndex* rhs
  )
{
  if (lhs == rhs)
    return 0;
  if (nullptr == rhs)
    return 1;
  if (nullptr == lhs)
    return -1;
  if (lhs->m_subdivision_count < rhs->m_subdivision_count)
    return -1;
  if (lhs->m_subdivision_count > rhs->m_subdivision_count)
    return 1;
  return MYON_SubDComponentRegionIndex::CompareMinimumSubregion(lhs, rhs);
}

int MYON_SubDComponentRegionIndex::CompareMinimumSubregion(
  const MYON_SubDComponentRegionIndex* lhs,
  const MYON_SubDComponentRegionIndex* rhs
  )
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  unsigned short subdivision_count0 = (lhs->m_subdivision_count < rhs->m_subdivision_count) ? lhs->m_subdivision_count : rhs->m_subdivision_count;
  if (subdivision_count0 > MYON_SubDComponentRegionIndex::IndexCapacity)
    subdivision_count0 = MYON_SubDComponentRegionIndex::IndexCapacity;
  for (unsigned short i = 0; i < subdivision_count0; i++)
  {
    if (lhs->m_index[i] < rhs->m_index[i])
      return -1;
    if (lhs->m_index[i] > rhs->m_index[i])
      return 1;
  }
  return 0;
}

int MYON_SubDComponentRegion::CompareTypeIdDirectionMinimumSubregion(
  const MYON_SubDComponentRegion* lhs,
  const MYON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;

  const int rc = MYON_SubDComponentRegion::CompareTypeIdDirection(lhs, rhs);
  if (0 != rc)
    return rc;

  return MYON_SubDComponentRegionIndex::CompareMinimumSubregion( &lhs->m_region_index, &rhs->m_region_index);
}

int MYON_SubDComponentRegion::CompareTypeIdDirectionSubregion(
  const MYON_SubDComponentRegion* lhs,
  const MYON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;

  int rc = MYON_SubDComponentRegion::CompareTypeIdDirection(lhs, rhs);
  if (0 == rc)
  {
    rc = MYON_SubDComponentRegionIndex::CompareMinimumSubregion(&lhs->m_region_index, &rhs->m_region_index);
    if (0 == rc)
    {
      if (lhs->m_region_index.m_subdivision_count < rhs->m_region_index.m_subdivision_count)
        rc = -1;
      else if (lhs->m_region_index.m_subdivision_count > rhs->m_region_index.m_subdivision_count)
        rc = 1;
    }
  }

  return rc;
}


int MYON_SubDComponentRegion::Compare(
  const MYON_SubDComponentRegion* lhs,
  const MYON_SubDComponentRegion* rhs
)
{
  if (lhs == rhs)
    return 0;

  const int rc = MYON_SubDComponentRegion::CompareTypeIdDirectionSubregion(lhs, rhs);
  if (0 != rc)
    return rc;

  if (lhs->m_level0_component.m_ptr < rhs->m_level0_component.m_ptr)
    return -1;
  if (lhs->m_level0_component.m_ptr > rhs->m_level0_component.m_ptr)
    return 1;

  return 0;
}


void MYON_SubDComponentRegion::SetLevel0Component(
  MYON_SubDComponentPtr component_ptr
)
{
  const class MYON_SubDComponentBase* component_base = component_ptr.ComponentBase();
  if (nullptr != component_base)
  {
    m_level0_component = component_ptr;
    m_level0_component_id = component_base->m_id;
  }
  else
  {
    m_level0_component = MYON_SubDComponentPtr::Null;
    m_level0_component_id = 0;
  }
  m_region_index = MYON_SubDComponentRegionIndex::Zero;
}

void MYON_SubDComponentRegion::SetLevel0Face(
  const MYON_SubDFace* face
  )
{
  SetLevel0Component(MYON_SubDComponentPtr::Create(face));
}

void MYON_SubDComponentRegion::SetLevel0EdgePtr(
  const MYON_SubDEdgePtr edge_ptr
)
{
  SetLevel0Component(MYON_SubDComponentPtr::Create(edge_ptr));
}

void MYON_SubDComponentRegion::SetLevel0Vertex(
  const MYON_SubDVertex* vertex
)
{
  SetLevel0Component(MYON_SubDComponentPtr::Create(vertex));
}

void MYON_SubDComponentRegionIndex::Push(
  unsigned int region_index
  )
{
  if ( region_index > 0xFFFFU )
    region_index = 0xFFFFU;
  if ( m_subdivision_count < MYON_SubDComponentRegionIndex::IndexCapacity )
    m_index[m_subdivision_count] = (unsigned short)region_index;
  ++m_subdivision_count;
}

void MYON_SubDComponentRegionIndex::Pop()
{
  if (m_subdivision_count > 0)
  {
    m_subdivision_count--;
    if ( m_subdivision_count < MYON_SubDComponentRegionIndex::IndexCapacity)
      m_index[m_subdivision_count] = 0;
  }
}


void MYON_SubDComponentRegion::PushAdjusted(
  unsigned int region_index
  )
{
  if (
    MYON_SubDComponentPtr::Type::Edge == m_level0_component.ComponentType()
    && 0 != m_level0_component.ComponentDirection()
    && region_index <= 1
    )
  {
    region_index = 1 - region_index;
  }
  PushAbsolute(region_index);
}

void MYON_SubDComponentRegion::PushAbsolute(
  unsigned int region_index
  )
{
  m_region_index.Push(region_index);
  MYON_SubDComponentRegionBreakpoint(this);
}

void MYON_SubDComponentRegion::Pop()
{
  m_region_index.Pop();
}

unsigned short MYON_SubDComponentRegion::SubdivisionCount() const
{
  return m_region_index.m_subdivision_count;
}


unsigned short MYON_SubDComponentRegionIndex::Index(
  unsigned short i
) const
{
  return
    (i < m_subdivision_count && i < MYON_SubDComponentRegionIndex::IndexCapacity)
    ? m_index[i]
    : 0xFFFF;
}

const MYON_SubDFace* MYON_SubDFaceRegion::Level0Face() const
{
  return this->m_face_region.m_level0_component.Face();
}

unsigned int MYON_SubDFaceRegion::CornerIndexFromVertexId(
  unsigned int vertex_id
) const
{
  unsigned corner_index = MYON_UNSET_UINT_INDEX;
  if (vertex_id > 0 && vertex_id < MYON_UNSET_UINT_INDEX)
  {
    for (unsigned i = 0; i < 4; ++i)
    {
      if (vertex_id == this->m_vertex_id[i])
      {
        if (MYON_UNSET_UINT_INDEX == corner_index)
          corner_index = i;
        else
          return MYON_UNSET_UINT_INDEX;
      }
    }
  }
  return corner_index;
}


void MYON_SubDFaceRegion::Push(unsigned int quadrant_index)
{
  m_face_region.PushAbsolute(quadrant_index);

  if (quadrant_index >= 0 && quadrant_index < 4)
  {
    m_edge_region[quadrant_index].PushAdjusted(0); // 1st half of this edge relative to face's orientation (adjusted to edge's orientation)
    m_edge_region[(quadrant_index + 1) % 4] = MYON_SubDComponentRegion::CreateSubdivisionRegion(MYON_SubDComponentPtr::Type::Edge, true, m_edge_region[quadrant_index].SubdivisionCount(), false);
    m_edge_region[(quadrant_index + 2) % 4] = MYON_SubDComponentRegion::CreateSubdivisionRegion(MYON_SubDComponentPtr::Type::Edge, false, m_edge_region[quadrant_index].SubdivisionCount(), false);
    m_edge_region[(quadrant_index + 3) % 4].PushAdjusted(1); // 2nd half of this edge relative to face's orientation (adjusted to edge's orientation)
  }

  const int surviving_vi
    = ((4 != m_level0_edge_count) && (1 == m_face_region.SubdivisionCount()))
    ? 2
    : quadrant_index;
  m_vertex_id[(surviving_vi+1)%4] = 0;
  m_vertex_id[(surviving_vi+2)%4] = 0;
  m_vertex_id[(surviving_vi+3)%4] = 0;
  m_sector_id[(surviving_vi + 1) % 4] = MYON_SubDSectorId::Zero;
  m_sector_id[(surviving_vi + 2) % 4] = MYON_SubDSectorId::Zero;
  m_sector_id[(surviving_vi + 3) % 4] = MYON_SubDSectorId::Zero;
}

bool MYON_SubDComponentRegion::IsEmptyRegion() const
{
  return
    MYON_SubDComponentPtr::Type::Unset == m_level0_component.ComponentType()
    && m_level0_component.IsNull()
    && 0 == m_level0_component_id
    && 0 == SubdivisionCount();
}

bool MYON_SubDFaceRegion::IsValid(
  bool bSilentError
) const
{
  if (m_face_region.IsEmptyRegion())
  {
    for (int ei = 0; ei < 4; ei++)
    {
      if (false == m_edge_region[ei].IsEmptyRegion())
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_face_region is empty and m_edge_region[] is not empty.");
        return false;
      }
    }
    for (int vi = 0; vi < 4; vi++)
    {
      if (0 != m_vertex_id[vi])
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_face_region is empty and m_vertex_id[] is not zero.");
        return false;
      }
      if (false == m_sector_id[vi].IsZero())
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_face_region is empty and m_sector_id[] is not zero.");
        return false;
      }
    }
    return true;
  }
  


  const MYON_SubDComponentPtr::Type face_type = m_face_region.m_level0_component.ComponentType();
  if (MYON_SubDComponentPtr::Type::Face != face_type)
  {
    if (false == bSilentError)
      MYON_SUBD_ERROR("Invalid m_face_region.");
    return false;
  }

  if (false == m_face_region.IsPersistentId())
  {
    if (false == bSilentError)
      MYON_SUBD_ERROR("m_face_region.IsPersistentId() is false");
    return false;
  }

  const MYON_SubDFace* face = m_face_region.m_level0_component.Face();
  if (nullptr == face )
  {
    if (false == bSilentError)
      MYON_SUBD_ERROR("m_face_region.m_level0_component.Face() is nullptr.");
    return false;
  }

  if (face->m_id != m_face_region.m_level0_component_id)
  {
    if (false == bSilentError)
      MYON_SUBD_ERROR("Unexpected value for m_face_region.m_level0_component_id");
    return false;
  }

  const unsigned int edge_count = (nullptr != face) ? face->EdgeCount() : 0;
  const bool bIsQuad = (4 == edge_count);
  if (false == bIsQuad)
  {
    if (0 == m_face_region.SubdivisionCount())
    {
      if (false == bSilentError)
        MYON_SUBD_ERROR("m_face_region is not a quad and 0 = m_subdivision_count.");
      return false;
    }
    if (((unsigned int)m_face_region.m_region_index.m_index[0]) >= edge_count)
    {
      if (false == bSilentError)
        MYON_SUBD_ERROR("Unexpected value in face_region.m_region_index[0].");
      return false;
    }
  }

  const unsigned short face_region_subdivision_count = m_face_region.SubdivisionCount();

  bool bPersistentVertex[4] = { bIsQuad, bIsQuad, true, bIsQuad };
  bool bPersistentEdge[4] = { bIsQuad, true, true, bIsQuad };
  for (unsigned short i = bIsQuad?0:1; i < face_region_subdivision_count && i < MYON_SubDComponentRegionIndex::IndexCapacity; i++)
  {
    const unsigned short r = m_face_region.m_region_index.m_index[i];
    if (r >= 4)
    {
      if (false == bSilentError)
        MYON_SUBD_ERROR("Unexpected value in face_region.m_region_index[].");
      return false;
    }
    bPersistentVertex[(r+1)%4] = false;
    bPersistentVertex[(r+2)%4] = false;
    bPersistentVertex[(r+3)%4] = false;
    bPersistentEdge[(r+1)%4] = false;
    bPersistentEdge[(r+2)%4] = false;
    if (false == bPersistentVertex[r] && false == bPersistentEdge[r] && false == bPersistentEdge[(r + 3) % 4] )
      break;
  }
   
  unsigned int fei[4] = { MYON_UNSET_UINT_INDEX,MYON_UNSET_UINT_INDEX,MYON_UNSET_UINT_INDEX,MYON_UNSET_UINT_INDEX };
  const MYON_SubDVertex* fv[4] = {};

  for (int ei = 0; ei < 4; ei++)
  {
    const bool bEmptyEdge = m_edge_region[ei].IsEmptyRegion();
    if (bEmptyEdge)
    {
      if ( bPersistentEdge[ei])
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("Unexpected empty edge in m_edge_region[].");
        return false;
      }
      continue;
    }

    const MYON_SubDComponentPtr::Type edge_type = m_edge_region[ei].m_level0_component.ComponentType();
    if (MYON_SubDComponentPtr::Type::Edge != edge_type)
    {
      if (false == bSilentError)
        MYON_SUBD_ERROR("Invalid m_face_region.");
      return false;
    }
    if ( m_edge_region[ei].SubdivisionCount() != m_face_region.SubdivisionCount() )
    {
      if (false == bSilentError)
        MYON_SUBD_ERROR("m_edge_region[].m_subdivision_count != m_face_region.m_subdivision_count.");
      return false;
    }

    if (bPersistentEdge[ei])
    {
      if (false == m_edge_region[ei].IsPersistentId())
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_edge_region[] missing a persistent edge id.");
        return false;
      }
      const MYON_SubDEdge* edge = m_edge_region[ei].m_level0_component.Edge();
      if (nullptr == edge)
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("Unexpected value for m_edge_region[].m_level0_component.Edge()");
        return false;
      }
      if (edge->m_id != m_edge_region[ei].m_level0_component_id)
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("Unexpected value for m_edge_region[].m_level0_component_id");
        return false;
      }
      fei[ei] = face->EdgeArrayIndex(edge);
      if (MYON_UNSET_UINT_INDEX == fei[ei])
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_edge_region[].m_level0_component.Edge() not in face.");
        return false;
      }
      fv[ei] = face->Vertex(ei);
      if ( nullptr == fv[ei] )
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_edge_region[].m_level0_component.Edge()->Vertex() is missing.");
        return false;
      }
    }
    else
    {
      if ( false == m_edge_region[ei].IsTransientId() )
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_edge_region[] missing a transient edge id.");
        return false;
      }
    }
  }

  for (unsigned int vi = 0; vi < 4; vi++)
  {
    if (bPersistentVertex[vi])
    {
      if (false == MYON_SubDComponentRegion::IsPersistentId(m_vertex_id[vi]))
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_vertex_id[] missing a persistent vertex id.");
        return false;
      } 

      if (face_region_subdivision_count <= 1)
      {
        unsigned int fvi = MYON_UNSET_UINT_INDEX;

        if (0 == face_region_subdivision_count)
          fvi = vi;
        else if (1 == face_region_subdivision_count)
          fvi = m_face_region.m_region_index.m_index[0];
        const MYON_SubDVertex* v = face->Vertex(fvi);
        if (nullptr == v)
        {
          if (false == bSilentError)
            MYON_SUBD_ERROR("face->Vertex() is nullptr.");
          return false;
        }
        if (v->m_id != m_vertex_id[vi])
        {
          if (false == bSilentError)
            MYON_SUBD_ERROR("m_vertex_id[] and face->Vertex()->m_id are different.");
          return false;
        }
      }

      const unsigned sector_vertex_id = m_sector_id[vi].VertexId();
      if (0 != sector_vertex_id)
      {
        if (sector_vertex_id != m_vertex_id[vi])
        {
          if (false == bSilentError)
            MYON_SUBD_ERROR("m_sector_id[].VertexId() is incorrect.");
          return false;
        }
        if (false == m_sector_id[vi].IsSet())
        {
          if (false == bSilentError)
            MYON_SUBD_ERROR("m_sector_id[] is missing face information.");
          return false;
        }
      }
      else
      {
        if (false == m_sector_id[vi].IsZero())
        {
          if (false == bSilentError)
            MYON_SUBD_ERROR("m_sector_id[] is missing vertex information.");
          return false;
        }
      }
    }
    else if ( 0 != m_vertex_id[vi] )
    {
      if (false == MYON_SubDComponentRegion::IsTransientId(m_vertex_id[vi]))
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("m_vertex_id[] missing a transient vertex id.");
        return false;
      }
      if (false == m_sector_id[vi].IsZero())
      {
        if (false == bSilentError)
          MYON_SUBD_ERROR("Transient vertex has an nonzero m_sector_id[].");
        return false;
      }
    }
  }

  return true;
}

bool MYON_SubDComponentRegion::IsTransientId() const
{
  return MYON_SubDComponentRegion::IsTransientId(m_level0_component_id);
}

bool MYON_SubDComponentRegion::IsPersistentId() const
{
  return MYON_SubDComponentRegion::IsPersistentId(m_level0_component_id);
}

static unsigned int Internal_TransientIdHelper(
  bool bReset
)
{  
  static std::atomic<unsigned int> src(0);

  if (bReset)
  {
    src = 0;
    return 0;
  }

  unsigned int transient_id = ++src;
  if (0 != (MYON_SubDComponentRegion::TransientIdBit & transient_id))
  {
    // This should be extremely rare. 
    // Calculations that use transient_id are time consuming, src should be small
    // after the first thread sets it back to zero.
    static MYON_SleepLock global_resource_lock;
    MYON_SleepLockGuard guard(global_resource_lock);
    if (0 != (MYON_SubDComponentRegion::TransientIdBit & src))
      src = 0;
    transient_id = ++src;
  }
  transient_id |= MYON_SubDComponentRegion::TransientIdBit;
  return transient_id;
}

void MYON_SubDComponentRegion::ResetTransientId()
{
  Internal_TransientIdHelper(true);
}

const unsigned int MYON_SubDComponentRegion::NewTransientId()
{
  return Internal_TransientIdHelper(false);
}

bool MYON_SubDComponentRegion::IsPersistentId(unsigned int id)
{
  return (0 != id) && (0 == (MYON_SubDComponentRegion::TransientIdBit & id));
}

unsigned int MYON_SubDComponentRegion::TransientId(unsigned int id)
{
  return (0 != (MYON_SubDComponentRegion::TransientIdBit & id)) ? ((~MYON_SubDComponentRegion::TransientIdBit) & id) : 0;
}


bool MYON_SubDComponentRegion::IsTransientId(unsigned int id)
{
  return 0 != (MYON_SubDComponentRegion::TransientIdBit & id) && 0 != ((~MYON_SubDComponentRegion::TransientIdBit) & id);
}

static wchar_t* Internal_AppendUnsigned(
  //wchar_t prefix1,
  //wchar_t prefix2,
  unsigned int i,
  wchar_t* s,
  wchar_t* s1
  )
{
  //if (0 != prefix1 && s < s1)
  //  *s++ = prefix1;
  //if (0 != prefix2 && s < s1)
  //  *s++ = prefix2;
  const bool bTransientId = (0 != (MYON_SubDComponentRegion::TransientIdBit & i) );
  if (bTransientId)
  {
    if (s < s1)
      *s++ = '<';
    i &= ~MYON_SubDComponentRegion::TransientIdBit;
  }
  wchar_t buffer[64];
  wchar_t* sdigit = buffer;
  wchar_t* sdigit1 = sdigit + (sizeof(buffer)/sizeof(buffer[0]));
  for ( *sdigit++ = 0; sdigit < sdigit1; sdigit++ )
  {
    *sdigit = (wchar_t)('0' + (i%10));
    i /= 10;
    if (0 == i)
    {
      while ( s < s1 && 0 != (*s = *sdigit--) )
        s++;
      break;
    }
  }
  if (bTransientId)
  {
    if (s < s1)
      *s++ = '>';
  }
  if (s <= s1)
    *s = 0;
  return s;
}

wchar_t* MYON_SubDComponentPtr::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  *s = 0;
  wchar_t* s1 = s + (s_capacity - 1);
  *s1 = 0;
  if (s < s1)
  {
    if (0 == m_ptr)
    {
      if (s + 7 < s1)
      {
        *s++ = 'N';
        *s++ = 'u';
        *s++ = 'l';
        *s++ = 'l';
        *s++ = 'P';
        *s++ = 't';
        *s++ = 'r';
      }
    }
    else
    {
      wchar_t c;
      switch (ComponentType())
      {
      case MYON_SubDComponentPtr::Type::Vertex:
        c = 'v';
        break;
      case MYON_SubDComponentPtr::Type::Edge:
        if ( s+2 < s1 )
          *s++ = (ComponentDirection()) ? '-' : '+';
        c = 'e';
        break;
      case MYON_SubDComponentPtr::Type::Face:
        c = 'f';
        break;
      case MYON_SubDComponentPtr::Type::Unset:
        c = 0;
        break;
      default:
        c = 0;
        break;
      }

      if (0 == c)
      {
        *s++ = '?';
      }
      else
      {
        *s++ = c;
        if (IsNull() && s + 6 < s1)
        {
          *s++ = '[';
          *s++ = 'n';
          *s++ = 'u';
          *s++ = 'l';
          *s++ = 'l';
          *s++ = ']';
        }
      }
    }
  }
  
  if (nullptr != s && s <= s1)
    *s = 0;
  return s;
};


const MYON_wString MYON_SubDComponentRegionIndex::ToString() const
{
  wchar_t buffer[32];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return MYON_wString(buffer);
  return MYON_wString::EmptyString;
}

wchar_t* MYON_SubDComponentRegionIndex::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  *s = 0;
  wchar_t* s1 = s + (s_capacity - 1);
  *s1 = 0;

  if (s < s1)
  {
    for (unsigned short i = 0; i < m_subdivision_count && nullptr != s && s < s1; i++)
    {
      if (s < s1)
        *s++ = '.';
      if (i >= MYON_SubDComponentRegionIndex::IndexCapacity)
      {
        // more subdivision levels that m_region_index[] can record.
        if (s < s1)
          *s++ = '_';
        break;
      }
      if (0xFFFF == m_index[i])
      {
        // This is component was added during a subdivision
        // and did not exist at level i.
        if (s < s1)
          *s++ = 'x';
      }
      else
      {
        // portion of component subdivided at level i
        s = Internal_AppendUnsigned(m_index[i], s, s1);
      }
    }
  }

  if (nullptr != s && s <= s1)
    *s = 0;

  return s;
}


const MYON_wString MYON_SubDComponentPtr::ToString() const
{
  wchar_t buffer[32];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return MYON_wString(buffer);
  return MYON_wString::EmptyString;
}


wchar_t* MYON_SubDComponentRegion::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  *s = 0;
  wchar_t* s1 = s + (s_capacity - 1);
  *s1 = 0;
  if (s < s1)
  {
    s = m_level0_component.ToString(s, s_capacity);
    if (nullptr != s && s < s1)
      s = Internal_AppendUnsigned(m_level0_component_id, s, s1);
  }

  if (nullptr != s && s < s1)
    s = m_region_index.ToString(s, 1 + (s1 - s));
  
  if (nullptr != s && s <= s1)
    *s = 0;

  return s;
}


const MYON_wString MYON_SubDComponentRegion::ToString() const
{
  wchar_t buffer[128];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return MYON_wString(buffer);
  return MYON_wString::EmptyString;
}

const MYON_wString MYON_SubDSectorId::ToString(bool bVerbose) const
{
  if (IsZero())
    return MYON_wString(bVerbose ? L"MYON_SubDSectorId::Zero" : L"Zero");
  if (m_sector_face_count > 0xFFFFU)
    return MYON_wString(bVerbose ? L"MYON_SubDSectorId::Invalid" : L"Invalid");
  wchar_t s_buffer[64];
  if (nullptr != ToString(s_buffer, sizeof(s_buffer) / sizeof(s_buffer[0])))
  {
    return bVerbose ? MYON_wString::FormatToString(L"MYON_SubDSectorId %ls", s_buffer) : MYON_wString(s_buffer);
  }
  return MYON_wString::EmptyString;
}

wchar_t* MYON_SubDSectorId::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  *s = 0;
  wchar_t* s1 = s + (s_capacity - 1);
  *s1 = 0;
  if (s < s1)
  {
    if (IsZero())
      *s++ = '0';
    else if (m_sector_face_count > 0xFFFFU)
      *s++ = 'X';
    else if ( s+6 < s1 )
    {
      *s++ = 'v';
      if (nullptr != s && s < s1)
      {
        s = Internal_AppendUnsigned(this->m_vertex_id, s, s1);
        if (nullptr != s && s + 5 < s1)
        {
          *s++ = '.';
          *s++ = 'f';
          s = Internal_AppendUnsigned(this->m_minimum_face_id, s, s1);
          if (nullptr != s && s + 2 < s1)
          {
            *s++ = 'x';
            s = Internal_AppendUnsigned(this->m_sector_face_count, s, s1);
          }
        }
      }
    }
  }

  if (nullptr != s && s <= s1)
    *s = 0;

  return s;
}


MYON__UINT32 MYON_SubDComponentRegionIndex::ToCompressedRegionIndex() const
{
  return MYON_SubDComponentRegionIndex::ToCompressedRegionIndex( m_subdivision_count, m_index);
}

const MYON_SubDComponentRegionIndex MYON_SubDComponentRegionIndex::FromCompressedRegionIndex(
  MYON__UINT32 compressed_region_index
)
{
  MYON_SubDComponentRegionIndex ri;
  MYON_SubDComponentRegionIndex::FromCompressedRegionIndex(compressed_region_index, &ri.m_subdivision_count, ri.m_index);
  return ri;
}

MYON__UINT32 MYON_SubDComponentRegionIndex::ToCompressedRegionIndex(
    unsigned short subdivision_count,
    const unsigned short* region_index
  )
{
  MYON__UINT32 rc 
    = (subdivision_count >= 255)
    ? 255
    : (MYON__UINT32)subdivision_count;
  rc <<= 24;
  if (nullptr != region_index && subdivision_count > 0)
  {
    MYON__UINT32 idx
      = (region_index[0] >= 255)
      ? 255
      : (MYON__UINT32)region_index[0];
    idx <<= 16;
    MYON__UINT32 shift = 14;
    for (unsigned short i = 1; i < subdivision_count && i < MYON_SubDComponentRegionIndex::IndexCapacity && shift <= 14; i++)
    {
      MYON__UINT32 bits = (MYON__UINT32)region_index[i];
      if (bits > 3)
        bits = 3;
      idx |= (bits << shift);
      shift -= 2;
    }
    rc |= idx;
  }
  return rc;
  //MYON__UINT32 shift = 27;
  //MYON__UINT32 rc = (MYON__UINT32)subdivision_count;
  //rc <<= shift;
  //
  //const unsigned short count
  //  = (subdivision_count <= MYON_SubDComponentRegion::region_index_capacity)
  //  ? subdivision_count
  //  : MYON_SubDComponentRegion::region_index_capacity;
  //for (unsigned short i = 0; i < count; i++)
  //{
  //  shift -= 3;
  //  MYON__UINT32 three_bits = (MYON__UINT32)(region_index[i] % 0x07);
  //  if (0 != three_bits)
  //    rc |= (three_bits << shift);
  //}
  //return rc;
}

void MYON_SubDComponentRegionIndex::FromCompressedRegionIndex(
  MYON__UINT32 compressed_region_index,
  unsigned short* subdivision_count,
  unsigned short* region_index
)
{
  const MYON__UINT32 count = (compressed_region_index >> 24);
  if (nullptr != subdivision_count)
    *subdivision_count = (unsigned short)count;
  if (nullptr != region_index)
  {
    region_index[0] = (unsigned short)((compressed_region_index & 0x00FF0000) >> 16);
    for (unsigned short i = 1; i < MYON_SubDComponentRegionIndex::IndexCapacity; i++)
    {
      region_index[i] = (unsigned short)((compressed_region_index & 0x0000C000) >> 14);
      compressed_region_index <<= 2;
    }
  }

  //MYON__UINT32 shift = 27;
  //MYON__UINT32 count = (region32 >> shift);
  //if (nullptr != subdivision_count)
  //  *subdivision_count = (unsigned short)count;
  //if (nullptr != region_index)
  //{
  //  for (unsigned short i = 0; i < MYON_SubDComponentRegion::region_index_capacity; i++)
  //    region_index[i] = 0;
  //  unsigned int idx = region32 << (32-shift);
  //  for (unsigned short i = 0; 0 != idx && i < MYON_SubDComponentRegion::region_index_capacity; i++)
  //  {
  //    MYON__UINT32 three_bits = (idx & 0xE0000000U);
  //    three_bits >>= 29;
  //    region_index[i] = (unsigned short)three_bits;
  //    idx <<= 3;      
  //  }
  //}

}

wchar_t* MYON_SubDFaceRegion::ToString(
  wchar_t* s,
  size_t s_capacity
) const
{
  if (s_capacity <= 0 || nullptr == s)
    return nullptr;

  wchar_t* s1 = s + s_capacity-1;
  *s1 = 0;

  s = m_face_region.ToString(s, s_capacity);
  if (nullptr != s && s+4 < s1)
  {
    for (unsigned int i = 0; i < 4 && nullptr != s && s + 4 < s1; i++)
    {
      *s++ = MYON_wString::Space;
      *s++ = (0 == i) ? '(' : ',';
      if ( MYON_SubDComponentPtr::Type::Edge == m_edge_region[i].m_level0_component.ComponentType() )
      {
        s = m_edge_region[i].ToString(s, s1 - s);
      }
      else
      {
        *s++ = 'e';
        *s++ = '?';
      }
    }
    if (nullptr != s && s < s1)
      *s++ = ')';
  }

  if (nullptr != s && s + 4 < s1)
  {
    for (unsigned int i = 0; i < 4 && nullptr != s && s + 4 < s1; i++)
    {
      *s++ = MYON_wString::Space;
      *s++ = (0 == i) ? '(' : ',';
      if (0 != m_vertex_id[i])
      {
        *s++ = 'v';
        s = Internal_AppendUnsigned(m_vertex_id[i], s, s1);
      }
      else
      {
        *s++ = '0';
      }
    }
    if (nullptr != s && s < s1)
      *s++ = ')';
  }

  if (
    false == m_sector_id[0].IsZero()
    ||
    false == m_sector_id[1].IsZero()
    ||
    false == m_sector_id[2].IsZero()
    ||
    false == m_sector_id[3].IsZero()
    )
  {
    if (nullptr != s && s + 4 < s1)
    {
      for (unsigned int i = 0; i < 4 && nullptr != s && s + 4 < s1; i++)
      {
        *s++ = MYON_wString::Space;
        *s++ = (0 == i) ? '(' : ',';
        s = m_sector_id[i].ToString(s, s1 - s);
      }
      if (nullptr != s && s < s1)
        *s++ = ')';
    }
  }

  if (nullptr != s && s <= s1)
    *s = 0;
  return s;
}

const MYON_wString MYON_SubDFaceRegion::ToString() const
{
  wchar_t buffer[256];
  if (nullptr != ToString(buffer, sizeof(buffer) / sizeof(buffer[0])))
    return MYON_wString(buffer);
  return MYON_wString::EmptyString;
}

static bool Internal_Seal3d(const double* src, double* dst, double tol )
{
#if 1
  // coded this way for debugging.
  // Release build optimization will inline this static and doubles will be in registers.
  const double d = (fabs(src[0] - dst[0]) + fabs(src[1] - dst[1]) + fabs(src[2] - dst[2]));
  if (d <= tol)
  {
    *dst++ = *src++;
    *dst++ = *src++;
    *dst = *src;
    return true;
  }
  return false;
#else
  // to see what happens if no "micro gap" sealing occurs.
  return true;
#endif
}


bool MYON_SubDMeshFragment::SealPoints(
  bool bTestNearEqual,
  const double* src,
  double* dst
)
{
  if (bTestNearEqual)
    return Internal_Seal3d( src, dst, 1.0e-8 );
  *dst++ = *src++;
  *dst++ = *src++;
  *dst = *src;
  return true;
}

bool MYON_SubDMeshFragment::SealNormals(
  bool bTestNearEqual,
  const double* src,
  double* dst
)
{
  if (bTestNearEqual)
    return Internal_Seal3d( src, dst, 1.0e-2 );
  *dst++ = *src++;
  *dst++ = *src++;
  *dst = *src;
  return true;
}

bool MYON_SubDMeshFragment::SealAdjacentSides(
  bool bTestNearEqual,
  bool bCopyNormals,
  const MYON_SubDMeshFragment& src_fragment,
  unsigned int i0,
  unsigned int i1,
  MYON_SubDMeshFragment& dst_fragment,
  unsigned int j0,
  unsigned int j1
)
{
  for (;;)
  {
    unsigned int m = 4 * src_fragment.m_grid.m_side_segment_count;
    if (i0 > m || i1 > m)
      break;
    m = 4 * dst_fragment.m_grid.m_side_segment_count;
    if (j0 > m || j1 > m)
      break;
    m = (i0 > i1) ? i0 - i1 : i1 - i0;
    if ( m != ((j0 > j1) ? j0 - j1 : j1 - j0))
      break;
    if (i0 > i1)
    {
      m = i0;
      i0 = i1;
      i1 = m;
      m = j0;
      j0 = j1;
      j1 = m;
    }
    const int delta_j = (j0 < j1) ? 1 : -1;

    const double* src;
    double* dst;

    unsigned int src_stride = (unsigned int)src_fragment.m_P_stride;
    unsigned int dst_stride = (unsigned int)dst_fragment.m_P_stride;
    int j = (int)j0;
    for (unsigned int i = i0; i <= i1; i++, j += delta_j)
    {
      src = &src_fragment.m_P[src_fragment.m_grid.m_S[i]*src_stride];
      dst = &dst_fragment.m_P[dst_fragment.m_grid.m_S[j]*dst_stride];
      if (false == MYON_SubDMeshFragment::SealPoints(bTestNearEqual,src,dst))
      {
        MYON_SUBD_ERROR("Point locations failed near equal test.");
        return false;
      }
    }

    if (bCopyNormals)
    {
      src_stride = (unsigned int)src_fragment.m_N_stride;
      dst_stride = (unsigned int)dst_fragment.m_N_stride;
      j = (int)j0;
      for (unsigned int i = i0; i <= i1; i++, j += delta_j)
      {
        src = &src_fragment.m_N[src_fragment.m_grid.m_S[i] * src_stride];
        dst = &dst_fragment.m_N[dst_fragment.m_grid.m_S[j] * dst_stride];
        if (false == MYON_SubDMeshFragment::SealNormals(bTestNearEqual,src,dst))
        {
          MYON_SUBD_ERROR("Normal locations failed near equal test.");
          return false;
        }
      }
    }
    return true;
  }

  MYON_SUBD_ERROR("Invalid input.");
  return false;
}

class VertexToDuplicate
{
public:
  // MYON_SubD information
  const MYON_SubDVertex* m_vertex = nullptr;
  const MYON_SubDFace* m_face = nullptr;
  unsigned int m_sector_id = 0; // all the dups for a specific vertex that are in the same sector get a unique nonzero sector id

  // MYON_Mesh information
  unsigned int m_mesh_V_index = 0;
  unsigned int m_mesh_F_index = 0;

  static int CompareVertexId(const class VertexToDuplicate* a, const class VertexToDuplicate*);
  static int CompareVertexIdAndFaceId(const class VertexToDuplicate* a, const class VertexToDuplicate*);
  static int CompareSectorIdAndFaceId(const class VertexToDuplicate* a, const class VertexToDuplicate*);

  static bool NeedsDuplicated(const MYON_SubDVertex* vertex);
};

int VertexToDuplicate::CompareVertexId(const class VertexToDuplicate* a, const class VertexToDuplicate* b)
{
  if ( a == b )
    return 0;
  if ( nullptr == a )
    return -1;
  if ( nullptr == b )
    return 1;

  unsigned int a_id = a->m_vertex ? a->m_vertex->m_id : 0;
  unsigned int b_id = b->m_vertex ? b->m_vertex->m_id : 0;
  if ( a_id < b_id )
    return -1;
  if ( a_id > b_id )
    return 1;

  return 0;
}

int VertexToDuplicate::CompareVertexIdAndFaceId(const class VertexToDuplicate* a, const class VertexToDuplicate* b)
{
  if ( a == b )
    return 0;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;
  
  unsigned int a_id = a->m_vertex ? a->m_vertex->m_id : 0;
  unsigned int b_id = b->m_vertex ? b->m_vertex->m_id : 0;
  if (a_id < b_id)
    return -1;
  if (a_id > b_id)
    return 1;

  a_id = a->m_face ? a->m_face->m_id : 0;
  b_id = b->m_face ? b->m_face->m_id : 0;
  if (a_id < b_id)
    return -1;
  if (a_id > b_id)
    return 1;

  return 0;
}

int VertexToDuplicate::CompareSectorIdAndFaceId(const class VertexToDuplicate* a, const class VertexToDuplicate* b)
{
  if (a == b)
    return 0;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;

  unsigned int a_id = a->m_sector_id;
  unsigned int b_id = b->m_sector_id;
  if (a_id < b_id)
    return -1;
  if (a_id > b_id)
    return 1;

  a_id = a->m_face ? a->m_face->m_id : 0;
  b_id = b->m_face ? b->m_face->m_id : 0;
  if (a_id < b_id)
    return -1;
  if (a_id > b_id)
    return 1;

  return 0;

}

bool VertexToDuplicate::NeedsDuplicated(
  const MYON_SubDVertex* vertex
  )
{
  if ( nullptr == vertex || vertex->m_face_count < 2 || vertex->m_edge_count < 2 || nullptr == vertex->m_edges || nullptr == vertex->m_faces)
    return false;
  if (vertex->IsSmooth())
    return false;
  const unsigned int edge_count = vertex->m_edge_count;
  for (unsigned int vei = 0; vei < edge_count; vei++)
  {
    const MYON_SubDEdge* edge = vertex->Edge(vei);
    if (nullptr != edge && false == edge->IsSmooth() && edge->m_face_count > 1)
      return true;
  }
  return false;
}

static bool Internal_UpdateMeshFaceVertexIndex(
  MYON_Mesh& mesh,
  unsigned mesh_F_index,
  unsigned int mesh_F_count,
  unsigned mesh_V_index0,
  unsigned mesh_V_index1
  )
{
  if (mesh_F_index < mesh_F_count && mesh_V_index0 < mesh_V_index1 )
  {
    unsigned int* fvi = (unsigned int*)(mesh.m_F[mesh_F_index].vi);
    if (fvi[0] == mesh_V_index0)
      fvi[0] = mesh_V_index1;
    if (fvi[1] == mesh_V_index0)
      fvi[1] = mesh_V_index1;
    if (fvi[2] == mesh_V_index0)
      fvi[2] = mesh_V_index1;
    if (fvi[3] == mesh_V_index0)
      fvi[3] = mesh_V_index1;
    return true;
  }
  MYON_SubDIncrementErrorCount();
  return false;
}


static VertexToDuplicate* Internal_FindMatchingVertexIdAndFaceId(const VertexToDuplicate* key, VertexToDuplicate* vertex_dups, unsigned int vertex_dups_count )
{
  return (VertexToDuplicate*)bsearch(key, vertex_dups, vertex_dups_count, sizeof(vertex_dups[0]), (int(*)(const void*, const void*))VertexToDuplicate::CompareVertexIdAndFaceId);
}

static bool Internal_DuplicateVertices(
  MYON_Mesh& mesh,
  MYON_3dPointArray& D,
  MYON_SimpleArray<VertexToDuplicate>& dups_array
  )
{
  const unsigned int mesh_F_count = mesh.m_F.UnsignedCount();
  const unsigned int mesh_D_count0 = D.UnsignedCount();

  const unsigned int dups_count = dups_array.UnsignedCount();
  if (dups_count <= 1)
    return true;

  unsigned int sector_id = 0;

  dups_array.QuickSortAndRemoveDuplicates(VertexToDuplicate::CompareVertexIdAndFaceId);

  MYON_SubDSectorIterator sit;
  VertexToDuplicate* dups = dups_array;
  VertexToDuplicate key;
  unsigned int i1 = 0;
  for (unsigned int i0 = i1; i0 < dups_count; i0 = i1)
  {
    key = dups[i0];
    if (nullptr == key.m_vertex)
    {
      MYON_SubDIncrementErrorCount();
      return false;
    }
    for (i1 = i0 + 1; i1 < dups_count; i1++)
    {
      int rc = VertexToDuplicate::CompareVertexId(&key,dups+i1);
      if (rc < 0)
        break;
      if ( 0 != rc
          || key.m_vertex != dups[i1].m_vertex 
          || key.m_mesh_V_index != dups[i1].m_mesh_V_index
          || key.m_mesh_V_index >= mesh_D_count0
          )
      {
        MYON_SubDIncrementErrorCount();
        return false;
      }
    }

    if ( i1 == i0+1)
      continue;

    const unsigned int mesh_V_index0 = key.m_mesh_V_index;
    const MYON_3dPoint P = D[mesh_V_index0];
    VertexToDuplicate* vertex_dups = dups+i0;
    const unsigned vertex_dups_count = i1 - i0;
    unsigned int sector_count = 0;
    // Set the sector id
    for (unsigned int k = 0; k < vertex_dups_count; ++k)
    {
      if (vertex_dups[k].m_sector_id > 0 )
        continue; // this was in a previously found sector.

      ++sector_id;
      ++sector_count;

      // When the priority is encoding creases, we need to divide the faces around this vertex
      // into sets that are in the same sector. Since a dart vertex has only 1 sector
      // but the crease edge at a dart needs to have duplicate vertices at both ends,
      // darts get special case handling.

      if (nullptr == sit.Initialize(vertex_dups[k].m_face, 0, key.m_vertex))
      {
        // not fatal, but don't bother with the vertices in vertex_dups[].
        MYON_SubDIncrementErrorCount();
        sector_count = 0;
        break;
      }
      if (nullptr == sit.IncrementToCrease(-1))
      {
        // not fatal, but don't bother with the vertices in vertex_dups[].
        MYON_SubDIncrementErrorCount();
        sector_count = 0;
        break;
      }

      if (0 == k && key.m_vertex->IsDart())
      {
        // darts have a single sector, but the dart vertex needs to be duplicated across the creased edge.
        const MYON_SubDEdge* edge = sit.CurrentEdge(0);
        if (nullptr == edge || false == edge->IsCrease() || 2 != edge->m_face_count)
        {
          // not fatal, but don't bother with the vertices in vertex_dups[].
          MYON_SubDIncrementErrorCount();
          sector_count = 0;
          break;
        }

        // we found the creased edge - duplicate the MYON_Mesh vertices at this edge.
        for (unsigned int efi = 0; efi < 2; efi++)
        {
          key.m_face = edge->Face(efi);
          VertexToDuplicate* vertex_dup = Internal_FindMatchingVertexIdAndFaceId(&key, vertex_dups, vertex_dups_count);
          if (nullptr == vertex_dup)
          {
            // not fatal, but don't bother with the vertices in vertex_dups[].
            MYON_SubDIncrementErrorCount();
            sector_count = 0;
            break;
          }
          // add a new vertex on either side of the dart's edge
          vertex_dup->m_mesh_V_index = D.UnsignedCount();
          D.Append(P);
          if ( false == Internal_UpdateMeshFaceVertexIndex(mesh, vertex_dup->m_mesh_F_index, mesh_F_count, mesh_V_index0, vertex_dup->m_mesh_V_index) )
          {
            // not fatal, but don't bother with the vertices in vertex_dups[].
            MYON_SubDIncrementErrorCount();
            sector_count = 0;
            break;
          }
        }
        for (k = 0; k < vertex_dups_count; ++k)
        {
          vertex_dups[k].m_sector_id = sector_id;
        }
        break;
      }

      // assign the sector id
      for (key.m_face = sit.CurrentFace(); nullptr != key.m_face; key.m_face = sit.NextFace(MYON_SubDSectorIterator::StopAt::AnyCrease))
      {
        VertexToDuplicate* vertex_dup = Internal_FindMatchingVertexIdAndFaceId(&key, vertex_dups, vertex_dups_count);
        if (nullptr == vertex_dup)
        {
          // not fatal, but don't bother with the vertices in vertex_dups[].
          MYON_SubDIncrementErrorCount();
          sector_count = 0;
          break;
        }
        vertex_dup->m_sector_id = sector_id;
      }
      if (0 == sector_count)
        break;
    }

    if (0 == sector_count)
      continue;

    if (sector_count > 1)
    {
      // sort vertex_dups[] by sector id 
      MYON_qsort(vertex_dups, vertex_dups_count, sizeof(vertex_dups[0]), (int(*)(const void*, const void*))VertexToDuplicate::CompareSectorIdAndFaceId);
    }
    
    unsigned int k1 = 0;
    for (unsigned int k0 = 0; k0 < vertex_dups_count; k0 = k1)
    {
      key = vertex_dups[k0];
      for (k1 = k0 + 1; k1 < vertex_dups_count; ++k1)
      {
        if (key.m_sector_id != vertex_dups[k1].m_sector_id)
          break;
      }
      if (k0 > 0)
      {
        // make a new vertex for this sector;
        key.m_mesh_V_index = D.UnsignedCount();
        D.Append(P);

        // update MYON_Mesh faces in this sector to use new vertex
        for (unsigned k = k0; k < k1; ++k)
          Internal_UpdateMeshFaceVertexIndex(mesh, vertex_dups[k].m_mesh_F_index, mesh_F_count, mesh_V_index0, key.m_mesh_V_index);
      }
    }
  }

  return true;
}

//static const MYON_2dPoint Internal_NgonFakeSurfaceParameter(
//  const MYON_SubDFace* face, 
//  const MYON_2dPoint face_pack_rect_corners[4], 
//  const MYON_2dPoint& center,
//  unsigned fvi
//)
//{
//  for (;;)
//  {
//    if (nullptr == face || face->m_edge_count < 5)
//      break;
//    const MYON_2dVector diag = face_pack_rect_corners[0] - center;
//    const double r = 0.5 * ((fabs(diag.y) < fabs(diag.x)) ? fabs(diag.y) : fabs(diag.x));
//    const double a = (((double)fvi) / ((double)face->m_edge_count))*MYON_2PI;
//    return MYON_2dPoint( center.x + r * cos(a), center.y + r*sin(a) );
//  }
//  return MYON_2dPoint::NanPoint;
//}

MYON_Mesh* MYON_SubD::GetControlNetMesh(
  MYON_Mesh* destination_mesh,
  MYON_SubDGetControlNetMeshPriority priority
) const
{
  if (destination_mesh)
    destination_mesh->Destroy();

  const MYON_SubDimple* subdimple = SubDimple();
  if (nullptr == subdimple)
    return nullptr; // SubD is empty - not an error

  const MYON_SubDLevel& level = ActiveLevel();
  if (level.IsEmpty())
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (level.m_vertex_count < 3)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (level.m_edge_count < 3)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( level.m_face_count < 1)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  std::unique_ptr< MYON_Mesh > up;
  MYON_Mesh* mesh = nullptr;
  if (nullptr != destination_mesh)
  {
    mesh = destination_mesh;
    *mesh = MYON_Mesh::Empty;
  }
  else
  {
    mesh = new MYON_Mesh();
  }

  bool bSuccess = false;
  switch (priority)
  {
  case MYON_SubDGetControlNetMeshPriority::Geometry:
    {
      unsigned int archive_id_partition[4] = {};
      bool bLevelLinkedListIncreasingId[3] = {};
      level.SetArchiveId(*subdimple, archive_id_partition, bLevelLinkedListIncreasingId);
      if (archive_id_partition[1] - archive_id_partition[0] == level.m_vertex_count)
      {
        // Have to use idit because subd editing (deleting and then adding) can leave the level's linked lists
        // with components in an order that is not increasing in id and it is critical that the next three for
        // loops iterate the level's components in order of increasing id.

        // must iterate vertices in order of increasing id
        MYON_SubDLevelComponentIdIterator vit_by_id;
        vit_by_id.Initialize(bLevelLinkedListIncreasingId[0], MYON_SubDComponentPtr::Type::Vertex, *subdimple, level);

        // must iterate vertices in order of increasing id
        MYON_SubDLevelComponentIdIterator fit_by_id;
        fit_by_id.Initialize(bLevelLinkedListIncreasingId[2], MYON_SubDComponentPtr::Type::Face, *subdimple, level);
        bSuccess = Internal_GetGeometryControlNetMesh(level, vit_by_id, fit_by_id, *mesh);
      }
    }
    break;

  case MYON_SubDGetControlNetMeshPriority::TextureCoordinates:
    bSuccess = Internal_GetTextureCoordinatesGeometryControlNetMesh(level, *mesh);
    break;
  }

  if (false == bSuccess)
  {
    if (mesh != destination_mesh)
      delete mesh;
    mesh = nullptr;
  }
  else
  {
    mesh->UpdateSinglePrecisionVertices();
    if (MYON_SubDGetControlNetMeshPriority::TextureCoordinates != priority)
    {
      mesh->ComputeFaceNormals();
      mesh->ComputeVertexNormals();
    }
    mesh->BoundingBox();
  }

  return mesh;
}


bool MYON_SubD::Internal_GetGeometryControlNetMesh(
  const MYON_SubDLevel& level,
  MYON_SubDLevelComponentIdIterator& vit_by_id,
  MYON_SubDLevelComponentIdIterator& fit_by_id,
  MYON_Mesh& mesh
) const
{
  VertexToDuplicate dup;
  MYON_SimpleArray<VertexToDuplicate> dups_array;

  const unsigned int subd_vertex_count = level.m_vertex_count;

  unsigned int mesh_ngon_count = 0;
  unsigned int mesh_face_count = 0;
  unsigned int max_ngon_Vcount = 0;
  for (const MYON_SubDFace* face = level.m_face[0]; nullptr != face; face = face->m_next_face)
  {
    if ( face->m_edge_count < 2 )
      continue;
    if (face->m_edge_count <= 4)
    {
      mesh_face_count++;
      continue;
    }
    mesh_ngon_count++;
    mesh_face_count += face->m_edge_count;
    if ( max_ngon_Vcount < face->m_edge_count )
      max_ngon_Vcount = face->m_edge_count;
  }

  if (subd_vertex_count < 3 || mesh_face_count < 1 )
    return MYON_SUBD_RETURN_ERROR(false);

  const size_t D_initial_capacity = subd_vertex_count + mesh_ngon_count;
  MYON_3dPointArray& D = mesh.DoublePrecisionVertices();
  D.Reserve(D_initial_capacity);
  D.SetCount(0);
  MYON_SimpleArray<bool> mesh_VertexNeedsDuplicated(D_initial_capacity);

  mesh.m_F.Reserve(mesh_face_count);
  mesh.m_F.SetCount(0);

  MYON_SimpleArray< MYON_2udex > ngon_spans(mesh_ngon_count);

  bool rc = false;
  for (;;)
  {
    // must iterate vertices in order of increasing id
    for (const MYON_SubDVertex* vertex = vit_by_id.FirstVertex(); nullptr != vertex; vertex = vit_by_id.NextVertex())
    {
      unsigned int vi = vertex->ArchiveId();
      if (vi < 1 || vi > subd_vertex_count)
        break;
      if (D.UnsignedCount()+1 != vi)
        break;
      D.AppendNew() = vertex->m_P;
      mesh_VertexNeedsDuplicated.AppendNew() = VertexToDuplicate::NeedsDuplicated(vertex);
    }

    if (D.UnsignedCount() != subd_vertex_count)
      break;

    ngon_spans.Reserve(mesh_ngon_count);
    unsigned int max_ngon_face_count = 0;
    mesh_face_count = 0;
    // must iterate faces in order of increasing id
    for (const MYON_SubDFace* face = fit_by_id.FirstFace(); nullptr != face; face = fit_by_id.NextFace())
    {
      MYON_MeshFace meshf = {};
      
      if (face->m_edge_count <= 4)
      {
        // SubD quad face or 3-gon face gets a single MYON_Mesh face
        if (face->m_edge_count < 3)
          continue;

        //const bool bQuad = 4 == face->m_edge_count;

        for (unsigned short fvi = 0; fvi < face->m_edge_count; fvi++)
        {
          const MYON_SubDVertex* vertex = face->Vertex(fvi);
          meshf.vi[fvi] = (int)((nullptr != vertex) ? vertex->ArchiveId() : 0U);
          if (meshf.vi[fvi] < 1 || meshf.vi[fvi] > (int)subd_vertex_count)
          {
            meshf.vi[0] = -1;
            break;
          }
          meshf.vi[fvi]--;
          if (mesh_VertexNeedsDuplicated[meshf.vi[fvi]])
          {
            dup.m_vertex = vertex;
            dup.m_face = face;
            dup.m_mesh_F_index = mesh.m_F.UnsignedCount();
            dup.m_mesh_V_index = meshf.vi[fvi];
            dups_array.Append(dup);
          }
        }
        if (-1 == meshf.vi[0] )
          continue;
        if ( 3 == face->m_edge_count)
          meshf.vi[3] = meshf.vi[2];
        mesh.m_F.Append(meshf);
        continue;
      }
      else // face->m_edge_count >= 5
      {
        // SubD n-gon face with n >= 5 gets n MYON_Mesh triangles grouped into
        MYON_3dPoint center_point;
        if (false == face->GetSubdivisionPoint( center_point))
          continue;

        MYON_2udex ngon_span = { mesh.m_F.UnsignedCount(), 0 };

        const unsigned int dup_count0 = dups_array.UnsignedCount();

        const unsigned int Dcount0 = D.UnsignedCount();
        const unsigned int Fcount0 = mesh.m_F.UnsignedCount();
        meshf.vi[2] = (int)Dcount0;
        meshf.vi[3] = meshf.vi[2];

        const MYON_SubDVertex* vertex = face->Vertex(0);
        meshf.vi[1] = (nullptr != vertex) ? vertex->ArchiveId() : 0;
        if (meshf.vi[1] < 1 || meshf.vi[1] > (int)subd_vertex_count)
          continue;
        meshf.vi[1]--;

        if (mesh_VertexNeedsDuplicated[meshf.vi[1]])
        {
          dup.m_vertex = vertex;
          dup.m_face = face;
          dup.m_mesh_F_index = mesh.m_F.UnsignedCount();
          dup.m_mesh_V_index = meshf.vi[1];
          dups_array.Append(dup);
        }

        mesh_VertexNeedsDuplicated.Append(false);
        D.Append(center_point);

        for (unsigned short fvi = 1; fvi <= face->m_edge_count; fvi++)
        {
          meshf.vi[0] = meshf.vi[1];
          vertex = face->Vertex(fvi % face->m_edge_count);
          meshf.vi[1] = (int)((nullptr != vertex) ? vertex->ArchiveId() : 0U);
          if (meshf.vi[1] < 1 || meshf.vi[1] > (int)subd_vertex_count)
          {
            meshf.vi[0] = -1;
            break;
          }
          meshf.vi[1]--;

          if (fvi < face->m_edge_count)
          {
            if (mesh_VertexNeedsDuplicated[meshf.vi[1]])
            {
              dup.m_vertex = vertex;
              dup.m_face = face;
              dup.m_mesh_F_index = mesh.m_F.UnsignedCount();
              dup.m_mesh_V_index = meshf.vi[1];
              dups_array.Append(dup);
            }
          }

          mesh.m_F.Append(meshf);
        }
        ngon_span.j = mesh.m_F.UnsignedCount();

        unsigned int ngon_face_count = ngon_span.j - ngon_span.i;
        if ( -1 == meshf.vi[0] || ngon_face_count != face->EdgeCount() )
        {
          D.SetCount(Dcount0);
          mesh.m_F.SetCount(Fcount0);
          dups_array.SetCount(dup_count0);
          continue;
        }
        ngon_span.j = mesh.m_F.UnsignedCount();
        if (ngon_face_count >= 3)
        {
          ngon_spans.Append(ngon_span);
          if ( ngon_face_count > max_ngon_face_count)
            max_ngon_face_count = ngon_face_count;
        }
      }
    }

    if (mesh.m_F.UnsignedCount() <= 0)
      break;

    rc = true;
    break;
  }

  level.ClearArchiveId();
  if (false == rc )
    return MYON_SUBD_RETURN_ERROR(false);

  if (D.UnsignedCount() < 3 || mesh.m_F.UnsignedCount() < 1)
    return MYON_SUBD_RETURN_ERROR(false);

  Internal_DuplicateVertices( mesh, D, dups_array);

  // group all mesh faces that came from the same level zero subd face into an ngon.
  if (ngon_spans.UnsignedCount() > 0 && max_ngon_Vcount >= 3)
  {
    MYON_SimpleArray< unsigned int> ngon_buffer;
    unsigned int* ngon_fi = ngon_buffer.Reserve(2*max_ngon_Vcount);
    unsigned int* ngon_vi = ngon_fi + max_ngon_Vcount;
    for (unsigned int ngon_dex = 0; ngon_dex < ngon_spans.UnsignedCount(); ngon_dex++ )
    {
      MYON_2udex ngon_span = ngon_spans[ngon_dex];
      unsigned int Fcount = ngon_span.j-ngon_span.i;
      if ( Fcount < 3)
        continue;

      ngon_fi[0] = ngon_span.i;
      ngon_fi[0] = (unsigned int)mesh.m_F[ngon_fi[0]].vi[0];

      unsigned int ngon_Vcount = 0;
      for (unsigned int i = ngon_span.i; i < ngon_span.j; i++)
      {
        ngon_fi[ngon_Vcount] = i;
        ngon_vi[ngon_Vcount] = (unsigned int)(mesh.m_F[i].vi[0]);
        ngon_Vcount++;
      }
      mesh.AddNgon(ngon_Vcount, ngon_vi, ngon_Vcount, ngon_fi );
    }
  }

  return true;
}

bool MYON_SubD::Internal_GetTextureCoordinatesGeometryControlNetMesh(
  const MYON_SubDLevel& level,
  MYON_Mesh& mesh
) const
{
  const bool bSetMeshT = 2 * this->TexturePointsAreSet() > this->FaceCount(); // more than half the faces have texture points.

  bool bSubdivide = false; // required if any SubD faces are not quads.
  unsigned mesh_4gon_count = 0;
  unsigned mesh_quad_count = 0;
  for (const MYON_SubDFace* f = level.m_face[0]; nullptr != f; f = f->m_next_face)
  {
    const unsigned n = f->EdgeCount();
    if (n < 3)
      continue;

    if (false == bSubdivide)
    {
      if (4 == n)
      {
        // subdivision is not required and we have another quad face
        ++mesh_quad_count;
        continue;
      }
      // first non-quad SubD face - switch to subdivided case
      bSubdivide = true;

      // each of the previously counted quad faces will generate 9 mesh vertices, 4 mesh faces, and a single MYON_MeshNgon.
      mesh_4gon_count = mesh_quad_count;
      mesh_quad_count *= 4;
    }

    // In the subdivided case, each SubD face is represented by n quads in the MYON_Mesh.
    if ( 4 == n)
      ++mesh_4gon_count;
    mesh_quad_count += n;
  }

  if (mesh_quad_count < 1)
    return MYON_SUBD_RETURN_ERROR(false);

  mesh.m_F.Reserve(mesh_quad_count);
  mesh.m_F.SetCount(0);
  mesh.m_FN.Reserve(mesh_quad_count);
  mesh.m_FN.SetCount(0);

  const unsigned mesh_vertex_count = 4 * (mesh_quad_count - 4* mesh_4gon_count) + 9 * mesh_4gon_count;
  MYON_3dPointArray& D = mesh.DoublePrecisionVertices();
  D.Reserve(mesh_vertex_count);
  D.SetCount(0);
  mesh.m_N.Reserve(mesh_vertex_count);
  mesh.m_N.SetCount(0);
  mesh.m_S.Reserve(mesh_vertex_count);
  mesh.m_S.SetCount(0);
  if (bSetMeshT)
    mesh.m_T.Reserve(mesh_vertex_count);
  mesh.m_T.SetCount(0);

  mesh.m_F.Reserve(mesh_quad_count);
  mesh.m_F.SetCount(0);
  mesh.m_FN.Reserve(mesh_quad_count);
  mesh.m_FN.SetCount(0);

  MYON_MeshFace mesh_f;
  MYON_2dPoint face_pack_rect_corners[4];
  MYON_3dPoint quadP[4];
  if (bSubdivide)
  {
    MYON_2dVector ngon_sub_pack_rect_size = MYON_2dVector::NanVector;
    MYON_2dVector ngon_sub_pack_rect_delta = MYON_2dVector::NanVector;
    MYON_2dPoint quadS[4] = { MYON_2dPoint::NanPoint, MYON_2dPoint::NanPoint, MYON_2dPoint::NanPoint, MYON_2dPoint::NanPoint };
    MYON_3dPoint quadT[4] = { MYON_3dPoint::NanPoint, MYON_3dPoint::NanPoint, MYON_3dPoint::NanPoint, MYON_3dPoint::NanPoint };
    MYON_3dPoint faceT[2] = { MYON_3dPoint ::NanPoint, MYON_3dPoint::NanPoint };


    MYON_SimpleArray<MYON_3dPoint> P(64);
    for (const MYON_SubDFace* f = level.m_face[0]; nullptr != f; f = f->m_next_face)
    {
      const unsigned n = f->m_edge_count;
      if (n < 3)
        continue;

      P.SetCount(0);
      for (unsigned i = 0; i < n; ++i)
      {
        const MYON_SubDVertex* v = f->Vertex(i);
        if (nullptr == v)
          break;
        const MYON_3dPoint C = v->ControlNetPoint();
        if (false == C.IsValid())
          break;
        P.Append(v->ControlNetPoint());
      }
      if (n != P.UnsignedCount())
        continue;

      quadP[0] = f->ControlNetCenterPoint();
      if (false == quadP[0].IsValid())
        continue;

      const MYON_3fVector N(f->ControlNetCenterNormal());

      f->GetFacePackRectCorners(false, face_pack_rect_corners);
      const MYON_2dVector face_pack_rect_size = f->PackRectSize();

      if (bSetMeshT)
      {
        faceT[0] = f->TexturePoint(n - 1);
        faceT[1] = f->TexturePoint(0);
        quadT[0] = f->TextureCenterPoint();
      }


      if (4 == n)
      {
        // An MYON_SubDFace quad becomes an MYON_Mesh ngon made from 4 MYON_Mesh quads.

        // center point
        mesh_f.vi[0] = D.UnsignedCount();
        D.Append(quadP[0]);
        mesh.m_N.Append(N);
        mesh.m_S.Append(f->PackRectOrigin() + 0.5 * f->PackRectSize());
        if (bSetMeshT)
          mesh.m_T.Append(MYON_2fPoint(quadT[0]));

        MYON_MeshNgon* four_gon = mesh.AllocateNgon(8,4);
        four_gon->m_vi[0] = D.UnsignedCount();
        four_gon->m_vi[1] = four_gon->m_vi[0] + 1;
        four_gon->m_vi[2] = four_gon->m_vi[0] + 2;
        four_gon->m_vi[3] = four_gon->m_vi[0] + 3;
        four_gon->m_vi[4] = four_gon->m_vi[0] + 4;
        four_gon->m_vi[5] = four_gon->m_vi[0] + 5;
        four_gon->m_vi[6] = four_gon->m_vi[0] + 6;
        four_gon->m_vi[7] = four_gon->m_vi[0] + 7;

        four_gon->m_fi[0] = mesh.m_F.UnsignedCount();
        four_gon->m_fi[1] = four_gon->m_fi[0] + 1;
        four_gon->m_fi[2] = four_gon->m_fi[0] + 2;
        four_gon->m_fi[3] = four_gon->m_fi[0] + 3;

        // add 8 boundary vertices
        for (unsigned i = 0; i < 4; ++i)
        {
          D.Append(P[i]);
          D.Append(MYON_3dPoint::Midpoint(P[i], P[(i + 1) % 4]));
          mesh.m_N.Append(N);
          mesh.m_N.Append(N);
          mesh.m_S.Append(face_pack_rect_corners[i]);
          mesh.m_S.Append(MYON_2dPoint::Midpoint(face_pack_rect_corners[i], face_pack_rect_corners[(i+1)%4]));
          if (bSetMeshT)
          {
            faceT[0] = faceT[1];
            faceT[1] = f->TexturePoint((i + 1) % 4);
            mesh.m_T.Append(MYON_2fPoint(faceT[0]));
            mesh.m_T.Append(MYON_2fPoint(MYON_3dPoint::Midpoint(faceT[0], faceT[1])));
          }
        }

        // add 4 MYON_Mesh quads that make up the MYON_SubDFace quad
        mesh_f.vi[3] = four_gon->m_vi[7];
        for (unsigned i = 0; i < 4; ++i)
        {
          mesh_f.vi[1] = mesh_f.vi[3];
          mesh_f.vi[2] = four_gon->m_vi[2 * i];
          mesh_f.vi[3] = mesh_f.vi[2] + 1;
          mesh.m_F.Append(mesh_f);
          mesh.m_FN.Append(N);
        }

        // add an MYON_MeshNgon that represents the MYON_SubDFace quad
        mesh.AddNgon(four_gon);
      }
      else
      {
        quadP[3] = MYON_3dPoint::Midpoint(P[n - 1], P[0]);
        if (bSetMeshT)
          quadT[3] = MYON_3dPoint::Midpoint(faceT[0], faceT[1]);


        const MYON_2udex ngon_grid_size
          = (n >= 5)
          ? MYON_SubDFace::GetNgonSubPackRectSizeAndDelta(n, face_pack_rect_size, ngon_sub_pack_rect_size, ngon_sub_pack_rect_delta)
          : MYON_2udex::Zero;

        // an MYON_Mesh n-gon is not possible because the fake packed surface parameters are not continuous across the MYON_SubD face.
        for (unsigned i = 0; i < n; ++i)
        {
          if (3 == n)
          {
            MYON_SubDMeshFragment::Get3gonFaceFragmentPackRectCorners(false, face_pack_rect_corners, i, false, quadS);
          }
          else
          {
            MYON_SubDMeshFragment::GetNgonFaceFragmentPackRectCorners(
              n,
              i,
              false,
              face_pack_rect_corners,
              face_pack_rect_size,
              ngon_grid_size,
              ngon_sub_pack_rect_size,
              ngon_sub_pack_rect_delta,
              quadS
            );
          }

          quadP[1] = quadP[3];
          quadP[2] = P[i];
          quadP[3] = MYON_3dPoint::Midpoint(P[i], P[(i + 1) % n]);
          if (bSetMeshT)
          {
            faceT[0] = faceT[1];
            faceT[1] = f->TexturePoint((i + 1) % n);
            quadT[1] = quadT[3];
            quadT[2] = faceT[0];
            quadT[3] = MYON_3dPoint::Midpoint(faceT[0], faceT[1]);
          }

          mesh_f.vi[0] = D.UnsignedCount();
          mesh_f.vi[1] = mesh_f.vi[0] + 1;
          mesh_f.vi[2] = mesh_f.vi[1] + 1;
          mesh_f.vi[3] = mesh_f.vi[2] + 1;

          for (unsigned j = 0; j < 4U; ++j)
          {
            D.Append(quadP[j]);
            mesh.m_N.Append(N);
            mesh.m_S.Append(quadS[j]);
            if (bSetMeshT)
              mesh.m_T.Append(MYON_2fPoint(quadT[j]));
          }
          mesh.m_F.Append(mesh_f);
          mesh.m_FN.Append(N);
        }
      }
    }
  }
  else
  {
    // All SubD faces are quads
    for (const MYON_SubDFace* f = level.m_face[0]; nullptr != f; f = f->m_next_face)
    {
      if (4 != f->m_edge_count)
        continue;
      quadP[3].x = MYON_DBL_QNAN;
      for (unsigned i = 0; i < 4; ++i)
      {
        const MYON_SubDVertex* v = f->Vertex(i);
        if (nullptr == v)
          break;
        quadP[i] = v->ControlNetPoint();
        if (false == quadP[i].IsValid())
          break;
      }
      if (false == quadP[3].IsValid())
        continue;
      const MYON_3fVector N(f->ControlNetCenterNormal());
      mesh_f.vi[0] = D.UnsignedCount();
      mesh_f.vi[1] = mesh_f.vi[0] + 1;
      mesh_f.vi[2] = mesh_f.vi[1] + 1;
      mesh_f.vi[3] = mesh_f.vi[2] + 1;
      mesh.m_F.Append(mesh_f);
      mesh.m_FN.Append(N);
      for (unsigned i = 0; i < 4U; ++i)
      {
        D.Append(quadP[i]);
        mesh.m_N.Append(N);
        mesh.m_S.Append(f->PackRectCorner(false, i));
        if (bSetMeshT)
          mesh.m_T.Append(MYON_2fPoint(f->TexturePoint(i)));
      }
    }
  }


  MYON_MappingTag mapping_tag = this->TextureMappingTag(false);

  if (mesh.m_S.UnsignedCount() != D.UnsignedCount())
  {
    mesh.m_S.Destroy();
    if (MYON_TextureMapping::TYPE::srfp_mapping == mapping_tag.m_mapping_type)
      mapping_tag = MYON_MappingTag::Unset;
  }
  else
  {
    // set fake surface mapping information
    mesh.m_srf_domain[0] = MYON_Interval::ZeroToOne;
    mesh.m_srf_domain[1] = MYON_Interval::ZeroToOne;
    mesh.m_srf_scale[0] = 0.0;
    mesh.m_srf_scale[1] = 0.0;
    mesh.m_packed_tex_domain[0] = MYON_Interval::ZeroToOne;
    mesh.m_packed_tex_domain[1] = MYON_Interval::ZeroToOne;
    mesh.m_packed_tex_rotate = false;

    if (
      false == bSetMeshT
      && (false == mapping_tag.IsSet() || mapping_tag.IsDefaultSurfaceParameterMapping())
      )
    {
      const int count = mesh.m_S.Count();
      mesh.m_T.Reserve(count);
      mesh.m_T.SetCount(0);
      for (int i = 0; i < count; ++i)
        mesh.m_T.Append(MYON_2fPoint(mesh.m_S[i]));
    }
  }

  if (bSetMeshT)
  {
    if (mesh.m_T.UnsignedCount() != D.UnsignedCount())
      mesh.m_T.Destroy();
  }
  mesh.m_Ttag = mapping_tag;

  return true;
}


void MYON_SubD::ClearEvaluationCache() const
{
  const MYON_SubDLevel* level = ActiveLevelConstPointer();

  if (nullptr != level)
  {
    const_cast<MYON_SubD*>(this)->ChangeGeometryContentSerialNumberForExperts(false);
    level->ClearEvaluationCache();
  }
}

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

unsigned int MYON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(MYON__UINT_PTR ptr)
{
  return (unsigned int)(ptr/(MYON_SUBD_COMPONENT_FLAGS_MASK+1));
}

const MYON_SubDComponentPtr MYON_SubDArchiveIdMap::FromVertex(
  const MYON_SubDVertex* vertex
  )
{
  MYON__UINT_PTR archive_id = (nullptr == vertex) ? 0 : vertex->ArchiveId();
  return MYON_SubDComponentPtr::Create((const MYON_SubDVertex*)(archive_id*(MYON_SUBD_COMPONENT_FLAGS_MASK+1)));
}

const MYON_SubDComponentPtr MYON_SubDArchiveIdMap::FromEdge(
  const MYON_SubDEdge* edge
  )
{
  MYON__UINT_PTR archive_id = (nullptr == edge) ? 0 : edge->ArchiveId();
  return MYON_SubDComponentPtr::Create((const MYON_SubDEdge*)(archive_id*(MYON_SUBD_COMPONENT_FLAGS_MASK+1)));
}

const MYON_SubDComponentPtr MYON_SubDArchiveIdMap::FromFace(
  const MYON_SubDFace* face
  )
{
  MYON__UINT_PTR archive_id = (nullptr == face) ? 0 : face->ArchiveId();
  return MYON_SubDComponentPtr::Create((const MYON_SubDFace*)(archive_id*(MYON_SUBD_COMPONENT_FLAGS_MASK+1)));
}

const MYON_SubDComponentPtr MYON_SubDArchiveIdMap::FromVertex(
  MYON_SubDVertexPtr vertex_ptr
  )
{
  MYON_SubDComponentPtr ptr = FromVertex(vertex_ptr.Vertex());
  ptr.m_ptr |= vertex_ptr.VertexDirection();
  return ptr;
}

const MYON_SubDComponentPtr MYON_SubDArchiveIdMap::FromEdge(
  MYON_SubDEdgePtr edge_ptr
  )
{
  MYON_SubDComponentPtr ptr = FromEdge(edge_ptr.Edge());
  ptr.m_ptr |= edge_ptr.EdgeDirection();
  return ptr;
}

const MYON_SubDComponentPtr MYON_SubDArchiveIdMap::FromFace(
  MYON_SubDFacePtr face_ptr
  )
{
  MYON_SubDComponentPtr ptr = FromFace(face_ptr.Face());
  ptr.m_ptr |= face_ptr.FaceDirection();
  return ptr;
}

const MYON_SubDComponentPtr MYON_SubDArchiveIdMap::FromSymmetrySetNext(
  MYON_SubDComponentPtr::Type component_type,
  const MYON_SubDComponentBase* component
)
{
  if (nullptr == component)
    return MYON_SubDComponentPtr::Null;

  MYON_SubDComponentPtr cptr;
  switch (component_type)
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    cptr = MYON_SubDArchiveIdMap::FromVertex(component->m_symmetry_set_next.Vertex());
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    cptr = MYON_SubDArchiveIdMap::FromEdge(component->m_symmetry_set_next.Edge());
    break;
  case MYON_SubDComponentPtr::Type::Face:
    cptr = MYON_SubDArchiveIdMap::FromFace(component->m_symmetry_set_next.Face());
    break;
  case MYON_SubDComponentPtr::Type::Unset:
  default:
    return MYON_SubDComponentPtr::Null;
    break;
  }
  return (1 == component->m_symmetry_set_next.ComponentDirection() && component->m_symmetry_set_next.IsNotNull()) ? cptr.SetComponentDirection() : cptr;
}

MYON_SubDVertex* MYON_SubDArchiveIdMap::CopyVertex(
  const MYON_SubDVertex* source_vertex,
  class MYON_SubDimple& subdimple
  )
{
  if ( nullptr == source_vertex )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON_SubDVertex* vertex = subdimple.AllocateVertex(
    source_vertex->m_id,
    source_vertex->m_vertex_tag,
    source_vertex->m_level,
    source_vertex->m_P,
    source_vertex->m_edge_count,
    source_vertex->m_face_count
    );
  if ( nullptr == vertex )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  
  const bool bCopyEdgeArray = true;
  const bool bCopyFaceArray = true;
  const bool bCopyLimitPointList = true;
  const bool bCopySymmetrySetNext = true;
  vertex->CopyFrom(source_vertex,bCopyEdgeArray,bCopyFaceArray,bCopyLimitPointList, bCopySymmetrySetNext);

  // convert vertex->m_edges[] pointers to archive_id values
  MYON_SubDComponentPtr ptr;
  for (unsigned int vei = 0; vei < vertex->m_edge_count; vei++)
  {
    ptr = MYON_SubDArchiveIdMap::FromEdge(vertex->m_edges[vei]);
    vertex->m_edges[vei].m_ptr = ptr.m_ptr;
  }

  // convert vertex->m_faces[] pointers to archive_id values
  for (unsigned int vfi = 0; vfi < vertex->m_face_count; vfi++)
  {
    ptr = MYON_SubDArchiveIdMap::FromFace(vertex->m_faces[vfi]);
    vertex->m_faces[vfi] = (const MYON_SubDFace*)ptr.m_ptr;
  }

  // convert vertex->m_limit_point[].m_sector_face pointers to archive_id values
  for (const MYON_SubDSectorSurfacePoint* p = &vertex->m_limit_point; nullptr != p; p = p->m_next_sector_limit_point)
  {
    ptr = MYON_SubDArchiveIdMap::FromFace(p->m_sector_face);
    const_cast<MYON_SubDSectorSurfacePoint*>(p)->m_sector_face = (const MYON_SubDFace*)ptr.m_ptr;
  }

  // convert symmetry set next pointers to archive_id values
  vertex->m_symmetry_set_next = MYON_SubDArchiveIdMap::FromSymmetrySetNext(MYON_SubDComponentPtr::Type::Vertex, vertex);
  
  return vertex;
}

MYON_SubDEdge* MYON_SubDArchiveIdMap::CopyEdge(
  const MYON_SubDEdge* source_edge,
  class MYON_SubDimple& subdimple
  )
{
  if ( nullptr == source_edge )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON_SubDEdge* edge = subdimple.AllocateEdge(
    source_edge->m_id,
    source_edge->m_edge_tag,
    source_edge->m_level,
    source_edge->m_face_count);
  if ( nullptr == edge )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  
  const bool bReverseEdge = false;
  const bool bCopyVertexArray = true;
  const bool bCopyEdgeArray = true;
  const bool bCopySymmetryLoop = true;
  edge->CopyFrom(source_edge,bReverseEdge,bCopyVertexArray,bCopyEdgeArray, bCopySymmetryLoop);

  // convert edge->m_vertex[] pointers to archive_id values
  MYON_SubDComponentPtr ptr;
  for (unsigned int evi = 0; evi < 2; evi++)
  {
    ptr = MYON_SubDArchiveIdMap::FromVertex(edge->m_vertex[evi]);
    edge->m_vertex[evi] = (const MYON_SubDVertex*)ptr.m_ptr;
  }

  // convert edge->m_faces[] pointers to archive_id values
  MYON_SubDFacePtr* fptr = edge->m_face2;
  for (unsigned int efi = 0; efi < edge->m_face_count; efi++, fptr++)
  {
    if (2 == efi)
      fptr = edge->m_facex;
    fptr->m_ptr = MYON_SubDArchiveIdMap::FromFace(*fptr).m_ptr;
  }

  // convert symmetry set next pointers to archive_id values
  edge->m_symmetry_set_next = MYON_SubDArchiveIdMap::FromSymmetrySetNext(MYON_SubDComponentPtr::Type::Edge, edge);

  return edge;
}

MYON_SubDFace* MYON_SubDArchiveIdMap::CopyFace(
  const MYON_SubDFace* source_face,
  class MYON_SubDimple& subdimple
  )
{
  if ( nullptr == source_face )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON_SubDFace* face = subdimple.AllocateFace( source_face->m_id, source_face->m_level,source_face->m_edge_count, source_face->TexturePointsAreSet());
  if ( nullptr == face )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  
  const bool bCopyEdgeArray = true;
  const bool bCopySymmetryLoop = true;
  face->CopyFrom(source_face,bCopyEdgeArray, bCopySymmetryLoop);

  // convert face->m_edges[] pointers to archive_id values
  MYON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned int fei = 0; fei < face->m_edge_count; fei++, eptr++)
  {
    if (4 == fei)
      eptr = face->m_edgex;
    eptr->m_ptr = MYON_SubDArchiveIdMap::FromEdge(*eptr).m_ptr;
  }

  // convert symmetry set next pointers to archive_id values
  face->m_symmetry_set_next = MYON_SubDArchiveIdMap::FromSymmetrySetNext(MYON_SubDComponentPtr::Type::Face, face);

  return face;
}
  
bool MYON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeVertexPtr(
  unsigned int vertex_count,
  size_t vertex_capacity,
  MYON_SubDVertex** vertex
  )
{
  if ( 0 == vertex_count )
    return true;
  if ( 0 == vertex_capacity || nullptr == vertex )
    return MYON_SUBD_RETURN_ERROR(false);
  if ( vertex_count > vertex_capacity )
    return MYON_SUBD_RETURN_ERROR(false);
  for (unsigned int i = 0; i < vertex_count; i++)
  {
    MYON__UINT_PTR vptr = (MYON__UINT_PTR)(vertex[i]);
    vertex[i] = nullptr;
    const unsigned int archive_id = MYON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(vptr);
    // future use // MYON__UINT_PTR flags = MYON_SUBD_COMPONENT_FLAGS(vptr);
    if (0 == archive_id || archive_id < m_archive_id_partition[0] || archive_id >= m_archive_id_partition[1])
    {
      MYON_ERROR("Invalid vertex archive id.");
      continue;
    }
    const MYON_SubDComponentPtr* eleptr = ComponentPtrFromArchiveId(archive_id);
    if (nullptr == eleptr)
    {
      MYON_ERROR("null element pointer.");
      continue;
    }
    MYON_SubDVertex* v = eleptr->Vertex();
    if (nullptr == v)
    {
      MYON_ERROR("null vertex pointer.");
      continue;
    }
    if (archive_id != v->ArchiveId())
    {
      MYON_ERROR("archive_id != v->ArchiveId().");
      continue;
    }
    vertex[i] = v;
  }
  return true;
}


bool MYON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeEdgePtr(
  unsigned int edge_count,
  size_t edgeN_capacity,
  MYON_SubDEdgePtr* edgeN,
  unsigned int edgeX_capacity,
  MYON_SubDEdgePtr* edgeX
  )
{
  if ( 0 == edge_count )
    return true;
  if ( 0 == edgeN_capacity || nullptr == edgeN )
    return MYON_SUBD_RETURN_ERROR(false);
  if ( 0 != edgeX_capacity && nullptr == edgeX )
    return MYON_SUBD_RETURN_ERROR(false);
  if ( edge_count > edgeN_capacity + edgeX_capacity )
    return MYON_SUBD_RETURN_ERROR(false);
  MYON_SubDEdgePtr* eptr = edgeN;
  for (unsigned int i = 0; i < edge_count; i++, eptr++)
  {
    if ( i == edgeN_capacity )
      eptr = edgeX;
    const unsigned int archive_id = MYON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(eptr->m_ptr);
    MYON__UINT_PTR flags = MYON_SUBD_COMPONENT_FLAGS(eptr->m_ptr);
    eptr->m_ptr = 0;
    if (0 == archive_id || archive_id < m_archive_id_partition[1] || archive_id >= m_archive_id_partition[2])
    {
      MYON_ERROR("Invalid edge archive id.");
      continue;
    }
    const MYON_SubDComponentPtr* eleptr = ComponentPtrFromArchiveId(archive_id);
    if (nullptr == eleptr)
    {
      MYON_ERROR("null element pointer.");
      continue;
    }
    MYON_SubDEdge* edge = eleptr->Edge();
    if (nullptr == edge)
    {
      MYON_ERROR("null edge pointer.");
      continue;
    }
    if (archive_id != edge->ArchiveId())
    {
      MYON_ERROR("archive_id != edge->ArchiveId().");
      continue;
    }
    *eptr = MYON_SubDEdgePtr::Create(edge,MYON_SUBD_COMPONENT_DIRECTION(flags));
  }
  return true;
}

bool MYON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeFacePtr(
  unsigned int face_count,
  size_t faceN_capacity,
  MYON_SubDFacePtr* faceN,
  unsigned int faceX_capacity,
  MYON_SubDFacePtr* faceX
  )
{
  if ( 0 == face_count )
    return true;
  if ( 0 == faceN_capacity || nullptr == faceN )
    return MYON_SUBD_RETURN_ERROR(false);
  if ( 0 != faceX_capacity && nullptr == faceX )
    return MYON_SUBD_RETURN_ERROR(false);
  if ( face_count > faceN_capacity + faceX_capacity )
    return MYON_SUBD_RETURN_ERROR(false);
  MYON_SubDFacePtr* fptr = faceN;
  for (unsigned int i = 0; i < face_count; i++, fptr++)
  {
    if ( i == faceN_capacity )
      fptr = faceX;
    const unsigned int archive_id = MYON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(fptr->m_ptr);
    MYON__UINT_PTR flags = MYON_SUBD_COMPONENT_FLAGS(fptr->m_ptr);
    fptr->m_ptr = 0;
    if (0 == archive_id || archive_id < m_archive_id_partition[2] || archive_id >= m_archive_id_partition[3])
    {
      MYON_ERROR("Invalid face archive id.");
      continue;
    }
    const MYON_SubDComponentPtr* eleptr = ComponentPtrFromArchiveId(archive_id);
    if (nullptr == eleptr)
    {
      MYON_ERROR("null element pointer.");
      continue;
    }
    MYON_SubDFace* face = eleptr->Face();
    if (nullptr == face)
    {
      MYON_ERROR("null face pointer.");
      continue;
    }
    if (archive_id != face->ArchiveId())
    {
      MYON_ERROR("archive_id != face->ArchiveId().");
      continue;
    }
    *fptr = MYON_SubDFacePtr::Create(face,MYON_SUBD_COMPONENT_DIRECTION(flags));
  }
  return true;
}

bool MYON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeSymmetrySetNextPtr(
  MYON_SubDComponentPtr::Type component_type,
  MYON_SubDComponentBase* component
)
{
  if (nullptr == component)
    return false;
  const MYON_SubDComponentPtr symmetry_set_next = component->m_symmetry_set_next; // symmetry_set_next stores archive id
  component->m_symmetry_set_next = MYON_SubDComponentPtr::Null;
  if (symmetry_set_next.IsNull())
    return true;
  if (component_type != symmetry_set_next.ComponentType())
    return MYON_SUBD_RETURN_ERROR(false);
  const unsigned archive_id = MYON_SubDArchiveIdMap::ArchiveIdFromComponentPtr(symmetry_set_next.m_ptr);
  if (archive_id < 1)
    return MYON_SUBD_RETURN_ERROR(false);
  unsigned parition0 = 0;
  switch (component_type)
  {
  case MYON_SubDComponentPtr::Type::Unset:
    return MYON_SUBD_RETURN_ERROR(false);
    break;
  case MYON_SubDComponentPtr::Type::Vertex:
    parition0 = 0;
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    parition0 = 1;
    break;
  case MYON_SubDComponentPtr::Type::Face:
    parition0 = 2;
    break;
  default:
    return MYON_SUBD_RETURN_ERROR(false);
    break;
  }
  if(archive_id < m_archive_id_partition[parition0] || archive_id >= m_archive_id_partition[parition0+1])
    return MYON_SUBD_RETURN_ERROR(false);
  const MYON_SubDComponentPtr* eleptr = ComponentPtrFromArchiveId(archive_id);
  if ( nullptr == eleptr || eleptr->ComponentType() != component_type)
    return MYON_SUBD_RETURN_ERROR(false);
  component->m_symmetry_set_next = (1 == symmetry_set_next.ComponentDirection()) ? eleptr->SetComponentDirection() : eleptr->ClearComponentDirection();
  return true;
}

void MYON_SubDArchiveIdMap::ValidateArrayCounts(
  unsigned short& array_count,
  size_t arrayN_capacity,
  const void* arrayN,
  unsigned short arrayX_capacity,
  const void* arrayX
  )
{
  if (arrayN_capacity >= 0xFFFFU)
  {
    MYON_ERROR("Invalid arrayN_capacity.");
    arrayN_capacity = 0xFFFFU;
  }

  unsigned short arrayN_cap = (unsigned short)arrayN_capacity;
  if ( array_count > arrayN_cap + arrayX_capacity 
    || (0 == arrayN_cap && 0 != arrayX_capacity)
    || (0 != arrayN_cap && nullptr == arrayN)
    || (0 != arrayX_capacity && nullptr == arrayX)
    )
  {
    MYON_ERROR("Invalid array counts.");
    if ( nullptr == arrayN )
      arrayN_cap = 0;
    if ( 0 == arrayN_cap || nullptr == arrayX )
      arrayX_capacity = 0;
    if (array_count > arrayN_cap + arrayX_capacity )
      array_count = arrayN_cap + arrayX_capacity;
  }
}

MYON_SubDArchiveIdMap::MYON_SubDArchiveIdMap()
{
  m_fsp.Create(sizeof(MYON_SubDComponentPtr),0,0);
  m_archive_id_partition[0] = 0;
  m_archive_id_partition[1] = 0;
  m_archive_id_partition[2] = 0;
  m_archive_id_partition[3] = 0;
}

bool MYON_SubDArchiveIdMap::Reset()
{
  m_fsp.ReturnAll();
  m_element_index = MYON_UNSET_UINT_INDEX;
  m_element_count = 0;
  m_archive_id_partition[0] = 0;
  m_archive_id_partition[1] = 0;
  m_archive_id_partition[2] = 0;
  m_archive_id_partition[3] = 0;
  return AddComponentPtr(MYON_SubDComponentPtr::Null,0);
}

unsigned int MYON_SubDArchiveIdMap::Count()
{
  return m_element_count;
}

const MYON_SubDComponentPtr* MYON_SubDArchiveIdMap::First()
{
  m_fsp_it.Create(&m_fsp);
  m_element_index = 0;
  const MYON_SubDComponentPtr* p = (const MYON_SubDComponentPtr*)m_fsp_it.FirstElement();
  if (nullptr != p)
    m_element_index++;
  return p;
}

const MYON_SubDComponentPtr* MYON_SubDArchiveIdMap::Next()
{
  if ( MYON_UNSET_UINT_INDEX == m_element_index)
    return First();
  const MYON_SubDComponentPtr* p = (const MYON_SubDComponentPtr*)m_fsp_it.NextElement();
  if (nullptr != p)
    m_element_index++;
  return p;
}

bool MYON_SubDArchiveIdMap::Add(const MYON_SubDVertex* vertex)
{
  return AddComponentPtr(MYON_SubDComponentPtr::Create(vertex,0),vertex ? vertex->ArchiveId() : 0);
}

bool MYON_SubDArchiveIdMap::Add(const MYON_SubDEdge* edge)
{
  return AddComponentPtr(MYON_SubDComponentPtr::Create(edge,0),edge ? edge->ArchiveId() : 0);
}

bool MYON_SubDArchiveIdMap::Add(const MYON_SubDFace* face)
{
  return AddComponentPtr(MYON_SubDComponentPtr::Create(face,0),face ? face->ArchiveId() : 0);
}


class MYON_SubDVertex* MYON_SubDArchiveIdMap::AddCopy(const class MYON_SubDVertex* source_vertex, class MYON_SubDimple& subdimple)
{
  MYON_SubDVertex* vertex = MYON_SubDArchiveIdMap::CopyVertex(source_vertex,subdimple);
  vertex->SetArchiveId(source_vertex->ArchiveId());
  Add(vertex);
  return vertex;
}

class MYON_SubDEdge* MYON_SubDArchiveIdMap::AddCopy(const class MYON_SubDEdge* source_edge, class MYON_SubDimple& subdimple)
{
  MYON_SubDEdge* edge = MYON_SubDArchiveIdMap::CopyEdge(source_edge,subdimple);
  edge->SetArchiveId(source_edge->ArchiveId());
  Add(edge);
  return edge;
}

class MYON_SubDFace* MYON_SubDArchiveIdMap::AddCopy(const class MYON_SubDFace* source_face, class MYON_SubDimple& subdimple)
{
  MYON_SubDFace* face = MYON_SubDArchiveIdMap::CopyFace(source_face,subdimple);
  face->SetArchiveId(source_face->ArchiveId());
  Add(face);
  return face;
}

const MYON_SubDComponentPtr* MYON_SubDArchiveIdMap::ComponentPtrFromArchiveId(
  unsigned int archive_id
  ) const
{
  return (const MYON_SubDComponentPtr*)m_fsp.Element(archive_id);
}


bool MYON_SubDArchiveIdMap::AddComponentPtr(MYON_SubDComponentPtr eptr, unsigned int archive_id)
{
  if (m_element_count != archive_id)
  {
    MYON_ERROR("Archive id is not valid and MYON_SubD::Read will fail.");
    return false;
  }
  MYON_SubDComponentPtr* p = (MYON_SubDComponentPtr*)m_fsp.AllocateElement();
  *p = eptr;

#if defined(MYON_DEBUG)
  if (0 != archive_id)
  {
    const MYON_SubDComponentPtr* p1 = (const MYON_SubDComponentPtr*)m_fsp.Element(archive_id);
    unsigned int archive_id1 = 0;
    if (p1 == p)
    {
      switch (p1->ComponentType())
      {
      case MYON_SubDComponentPtr::Type::Vertex:
        archive_id1 = p1->Vertex()->ArchiveId();
        break;
      case MYON_SubDComponentPtr::Type::Edge:
        archive_id1 = p1->Edge()->ArchiveId();
        break;
      case MYON_SubDComponentPtr::Type::Face:
        archive_id1 = p1->Face()->ArchiveId();
        break;
      default:
        MYON_ERROR("invalid element type");
        break;
      }
    }
    if (archive_id1 != archive_id)
    {
      // break here and then see what went wrong
      MYON_SubDIncrementErrorCount();
      m_fsp.Element(archive_id);
      m_fsp.Element(archive_id);
    }
  }
#endif

  m_element_count++;
  return true;
}

unsigned int MYON_SubDArchiveIdMap::ConvertArchiveIdsToRuntimePointers()
{
  // Convert archive_id references to runtime pointers.

  // The first element is MYON_SubDComponentPtr::Null.  This is done so the index of the elements
  // in element_list[] is equal to the element's archive_id.
  const MYON_SubDComponentPtr* element = First();
  if (nullptr == element || MYON_SubDComponentPtr::Type::Unset != element->ComponentType())
    return MYON_SUBD_RETURN_ERROR(0);
  
  element = Next();

  unsigned int archive_id;

  for (archive_id = m_archive_id_partition[0]; nullptr != element && archive_id < m_archive_id_partition[1]; archive_id++, element = Next())
  {
    MYON_SubDVertex* v = element->Vertex();
    if (nullptr == v)
      break;
    if (archive_id != v->ArchiveId())
      break;
    // convert MYON_SubDVertex.m_edges[]
    ConvertArchiveIdToRuntimeEdgePtr(v->m_edge_count,v->m_edge_capacity,v->m_edges,0,nullptr);
    // convert MYON_SubDVertex.m_faces[]
    ConvertArchiveIdToRuntimeFacePtr(v->m_face_count,v->m_face_capacity,(MYON_SubDFacePtr*)v->m_faces,0,nullptr);

    for (const MYON_SubDSectorSurfacePoint* p = &v->m_limit_point; nullptr != p; p = p->m_next_sector_limit_point)
    {
      if ( 0 != p->m_sector_face )
        ConvertArchiveIdToRuntimeFacePtr(1,1,(MYON_SubDFacePtr*)&p->m_sector_face,0,nullptr);
    }

    // convert v->m_symmetry_set_next to runtime pointer.
    ConvertArchiveIdToRuntimeSymmetrySetNextPtr(MYON_SubDComponentPtr::Type::Vertex, v);
  }
  if ( archive_id != m_archive_id_partition[1] )
    return MYON_SUBD_RETURN_ERROR(0);

  for (archive_id = m_archive_id_partition[1]; nullptr != element && archive_id < m_archive_id_partition[2]; archive_id++, element = Next())
  {
    MYON_SubDEdge* e = element->Edge();
    if (nullptr == e)
      break;
    if (archive_id != e->ArchiveId())
      break;
    // convert MYON_SubDEdge.m_vertex[2]
    ConvertArchiveIdToRuntimeVertexPtr(2,2,(MYON_SubDVertex**)(e->m_vertex));
    // convert MYON_SubDEdge.m_face2[] and MYON_SubDEdge.m_facex[]
    ConvertArchiveIdToRuntimeFacePtr(e->m_face_count,sizeof(e->m_face2)/sizeof(e->m_face2[0]),e->m_face2,e->m_facex_capacity,e->m_facex);

    // convert e->m_symmetry_set_next to runtime pointer.
    ConvertArchiveIdToRuntimeSymmetrySetNextPtr(MYON_SubDComponentPtr::Type::Edge, e);
  }
  if ( archive_id != m_archive_id_partition[2] )
    return MYON_SUBD_RETURN_ERROR(0);

  for (archive_id = m_archive_id_partition[2]; nullptr != element && archive_id < m_archive_id_partition[3]; archive_id++, element = Next())
  {
    MYON_SubDFace* f = element->Face();
    if (nullptr == f)
      break;
    if (archive_id != f->ArchiveId())
      break;
    // convert MYON_SubDFace.m_edge4[] and MYON_SubDFace.m_edgex[]
    MYON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeEdgePtr(f->m_edge_count,sizeof(f->m_edge4)/sizeof(f->m_edge4[0]),f->m_edge4,f->m_edgex_capacity,f->m_edgex);

    // convert f->m_symmetry_set_next to runtime pointer.
    ConvertArchiveIdToRuntimeSymmetrySetNextPtr(MYON_SubDComponentPtr::Type::Face, f);
  }
  if ( archive_id != m_archive_id_partition[3] )
    return MYON_SUBD_RETURN_ERROR(0);

  return archive_id;
}

void MYON_SubD::ShareContentsFrom(MYON_SubD& src_subd)
{
  if (this == &MYON_SubD::Empty || &src_subd == &MYON_SubD::Empty)
  {
    MYON_SubDIncrementErrorCount();
  }
  else if (m_subdimple_sp.get() != src_subd.m_subdimple_sp.get())
  {
    m_subdimple_sp = src_subd.m_subdimple_sp;
  }
}

void MYON_SubD::SwapContents(
  MYON_SubD& a,
  MYON_SubD& b
  )
{
  if (&a == &MYON_SubD::Empty || &b == &MYON_SubD::Empty)
  {
    MYON_SubDIncrementErrorCount();
  }
  else
  {
    std::swap(a.m_subdimple_sp, b.m_subdimple_sp);
  }
}


MYON_SubD::MYON_SubD(const MYON_SubD& src)
  : MYON_Geometry(src)
{
  this->CopyHelper(src);
}

MYON_SubD& MYON_SubD::operator=(const MYON_SubD& src)
{
  if ( this != &src )
  {
    this->Destroy();
    this->CopyHelper(src);
    // The next line copies user data
    MYON_Geometry::operator=(src);
  }
  return *this;
}

void MYON_SubD::CopyHelper(const MYON_SubD& src)
{
  MYON_SubDimple* subdimple = nullptr;
  const MYON_SubDimple* src_subdimple = src.SubDimple();
  if (nullptr != src_subdimple)
  {
    subdimple = new MYON_SubDimple(*src_subdimple);
  }
  m_subdimple_sp = std::shared_ptr<MYON_SubDimple>(subdimple);
  if (nullptr != subdimple)
    subdimple->SetManagedMeshSubDWeakPointers(m_subdimple_sp);
}

MYON__UINT64 MYON_SubDimple::GeometryContentSerialNumber() const
{
  return m_subd_geometry_content_serial_number;
}

MYON__UINT64 MYON_SubDimple::RenderContentSerialNumber() const
{
  return m_subd_render_content_serial_number;
}

MYON__UINT64 MYON_SubDimple::ComponentStatusSerialNumber() const
{
  return ActiveLevel().ComponentStatusSerialNumber();
}

const MYON_AggregateComponentStatusEx MYON_SubDimple::AggregateComponentStatus() const
{
  return ActiveLevel().AggregateComponentStatus();
}

MYON__UINT64 MYON_SubDimple::ChangeGeometryContentSerialNumber(
  bool bChangePreservesSymmetry
) const
{
  // For efficiency, must calculate bUpdateSymmetricObjectGeometry before changing m_subd_geometry_content_serial_number
  const bool bUpdateSymmetricObjectGeometry = bChangePreservesSymmetry && m_symmetry.SameSymmetricObjectGeometry(this);

  m_subd_geometry_content_serial_number = MYON_NextContentSerialNumber();
  m_subd_render_content_serial_number = m_subd_geometry_content_serial_number; // changing content automatically changes render content.

  if (m_symmetry.IsSet())
  {
    if (bUpdateSymmetricObjectGeometry)
      m_symmetry.SetSymmetricObject(this);
    else if (false == m_symmetry.SameSymmetricObjectTopology(this))
      m_symmetry.ClearSymmetricObject();
  }

  return m_subd_geometry_content_serial_number;
}

MYON__UINT64 MYON_SubDimple::ChangeRenderContentSerialNumber() const
{
  m_subd_render_content_serial_number = MYON_NextContentSerialNumber();
  return m_subd_render_content_serial_number;
}

void MYON_SubDimple::SetManagedMeshSubDWeakPointers(
  std::shared_ptr<class MYON_SubDimple>& subdimple_sp
  )
{
  // update weak ptrs on limit mesh
  const unsigned int level_count = m_levels.UnsignedCount();
  for (unsigned level_index = 0; level_index < level_count; level_index++)
  {
    MYON_SubDLevel* level = m_levels[level_index];
    if (nullptr == level)
      continue;
    MYON_SubDMeshImpl* surface_mesh_imple = level->m_surface_mesh.SubLimple();
    if (nullptr != surface_mesh_imple)
      surface_mesh_imple->SetSubDWeakPointer(level->m_face[0], subdimple_sp);
    MYON_SubDMeshImpl* control_net_mesh_imple = level->m_control_net_mesh.SubLimple();
    if (nullptr != control_net_mesh_imple)
      control_net_mesh_imple->SetSubDWeakPointer(level->m_face[0], subdimple_sp);
  }
}

void MYON_SubDMeshImpl::SetSubDWeakPointer(
  const MYON_SubDFace* subd_first_face,
  std::shared_ptr<class MYON_SubDimple>& subdimple_sp
)
{
  for (;;)
  {
    const MYON_SubDimple* subdimple = subdimple_sp.get();
    if (nullptr == subdimple)
      break;
    if (nullptr == subd_first_face)
      break;
    if (nullptr == m_first_fragment)
      break;
    if (m_first_fragment->m_face != subd_first_face)
      break;
    m_subdimple_wp = subdimple_sp;
    return;
  }

  // It's no longer clear that the subd used to create this limit mesh exist.
  ClearFragmentFacePointers(true);
}


MYON_SubDimple::MYON_SubDimple(const MYON_SubDimple& src)
  : RuntimeSerialNumber(++MYON_SubDimple::Internal_RuntimeSerialNumberGenerator)
{
  const bool bCopyComponentStatus = true;

  unsigned level_count = src.m_levels.UnsignedCount();
  for (/*empty init*/; level_count > 0; level_count--)
  {
    const MYON_SubDLevel* src_level = src.m_levels[level_count - 1];
    if (nullptr == src_level)
      continue;
    if (nullptr == src_level->m_vertex[0])
      continue;
    if (nullptr == src_level->m_edge[0])
      continue;
    // it's ok to have subd with just vertices and edges.
    //NO//if (nullptr == src_level->m_face[0])
    //NO//  continue;
    break;
  }

  MYON_SubDArchiveIdMap eptrlist;
  m_levels.Reserve(level_count);
  for (unsigned int level_index = 0; level_index < level_count; level_index++)
  {
    const MYON_SubDLevel* src_level = src.m_levels[level_index];
    MYON_SubDLevel* level = SubDLevel(level_index,true);
    if (nullptr == level)
      break;
    if (false == level->CopyHelper(src, *src_level, eptrlist, *this, bCopyComponentStatus))
      break;
    if ( src.m_active_level == src_level )
      m_active_level = level;
  }

  // this MYON_SubDimple is an exact copy of src, so they have identical geometry 
  // and render content.
  m_subd_geometry_content_serial_number = src.m_subd_geometry_content_serial_number;
  m_subd_render_content_serial_number = src.m_subd_render_content_serial_number;

  m_subd_appearance = src.m_subd_appearance;
  m_texture_coordinate_type = src.m_texture_coordinate_type;
  m_texture_mapping_tag = src.m_texture_mapping_tag;

  m_face_packing_id = src.m_face_packing_id;
  m_face_packing_topology_hash = src.m_face_packing_topology_hash;
  m_face_packing_topology_hash.m_subd_runtime_serial_number
    = (src.RuntimeSerialNumber == src.m_face_packing_topology_hash.SubDRuntimeSerialNumber())
    ? this->RuntimeSerialNumber
    : 0;
  if (0 == m_face_packing_topology_hash.m_subd_runtime_serial_number)
    m_face_packing_topology_hash.m_subd_geometry_content_serial_number = 0;

  m_symmetry = src.m_symmetry;

  if (
    src.RuntimeSerialNumber == src.m_subd_toplology_hash.SubDRuntimeSerialNumber()
    && m_subd_geometry_content_serial_number == src.m_subd_toplology_hash.SubDGeometryContentSerialNumber()
    )
  {
    // src.m_subd_toplology_hash is valid - copy it to this
    m_subd_toplology_hash = src.m_subd_toplology_hash;
    m_subd_toplology_hash.m_subd_runtime_serial_number = this->RuntimeSerialNumber;
  }

  if (
    src.RuntimeSerialNumber == src.m_subd_toplology_and_edge_creases_hash.SubDRuntimeSerialNumber()
    && m_subd_geometry_content_serial_number == src.m_subd_toplology_and_edge_creases_hash.SubDGeometryContentSerialNumber()
    )
  {
    // src.m_subd_toplology_and_edge_crease_hash is valid - copy it to this
    m_subd_toplology_and_edge_creases_hash = src.m_subd_toplology_and_edge_creases_hash;
    m_subd_toplology_and_edge_creases_hash.m_subd_runtime_serial_number = this->RuntimeSerialNumber;
  }

  if (
    src.RuntimeSerialNumber == src.m_subd_geometry_hash.SubDRuntimeSerialNumber()
    && m_subd_geometry_content_serial_number == src.m_subd_geometry_hash.SubDGeometryContentSerialNumber()
    )
  {
    // src.m_subd_geometry_hash is valid - copy it to this.
    m_subd_geometry_hash = src.m_subd_geometry_hash;
    m_subd_geometry_hash.m_subd_runtime_serial_number = this->RuntimeSerialNumber;
  }
}

bool MYON_SubDLevel::IsEmpty() const
{
  return (
    nullptr == m_vertex[0] 
    || 0U == m_vertex_count 
    || this == &MYON_SubDLevel::Empty 
    );
}

int MYON_SubDComponentBaseLink::CompareId(MYON_SubDComponentBaseLink const*const* lhs, MYON_SubDComponentBaseLink const*const* rhs)
{
  unsigned int lhs_id = (*lhs)->m_id;
  unsigned int rhs_id = (*rhs)->m_id;
  if (lhs_id < rhs_id)
    return -1;
  if (lhs_id > rhs_id)
    return 1;
  return 0;
}

void MYON_SubDLevelComponentIdIterator::Initialize(
  bool bLevelLinkedListIncreasingId,
  MYON_SubDComponentPtr::Type ctype,
  const MYON_SubDimple& subdimple,
  const MYON_SubDLevel& level
)
{
  m_bLevelLinkedListIncreasingId = false;
  m_ctype = ctype;
  m_level_index = (unsigned short)level.m_level_index;
  m_count = 0;
  m_prev_id = 0;
  m_first = nullptr;
  m_current = nullptr;

  switch (ctype)
  {
  case MYON_SubDComponentPtr::Type::Vertex:
      m_first = (const MYON_SubDComponentBaseLink*)level.m_vertex[0];
      break;
  case MYON_SubDComponentPtr::Type::Edge:
      m_first = (const MYON_SubDComponentBaseLink*)level.m_edge[0];
      break;
  case MYON_SubDComponentPtr::Type::Face:
      m_first = (const MYON_SubDComponentBaseLink*)level.m_face[0];
      break;
  default:
    m_first = nullptr;
    break;
  }

  if (nullptr == m_first)
    return;

  m_bLevelLinkedListIncreasingId = bLevelLinkedListIncreasingId;

  if (false == m_bLevelLinkedListIncreasingId)
  {
    subdimple.InitializeComponentIdIterator(ctype, m_cidit);
  }
}

const MYON_SubDVertex* MYON_SubDLevelComponentIdIterator::FirstVertex()
{
  return (MYON_SubDComponentPtr::Type::Vertex == m_ctype) ? (const MYON_SubDVertex*)InternalFirst() : nullptr;
}

const MYON_SubDVertex* MYON_SubDLevelComponentIdIterator::NextVertex()
{
  return (MYON_SubDComponentPtr::Type::Vertex == m_ctype) ? (const MYON_SubDVertex*)InternalNext() : nullptr;
}

const MYON_SubDEdge* MYON_SubDLevelComponentIdIterator::FirstEdge()
{
  return (MYON_SubDComponentPtr::Type::Edge == m_ctype) ? (const MYON_SubDEdge*)InternalFirst() : nullptr;
}

const MYON_SubDEdge* MYON_SubDLevelComponentIdIterator::NextEdge()
{
  return (MYON_SubDComponentPtr::Type::Edge == m_ctype) ? (const MYON_SubDEdge*)InternalNext() : nullptr;
}

const MYON_SubDFace* MYON_SubDLevelComponentIdIterator::FirstFace()
{
  return (MYON_SubDComponentPtr::Type::Face == m_ctype) ? (const MYON_SubDFace*)InternalFirst() : nullptr;
}

const MYON_SubDFace* MYON_SubDLevelComponentIdIterator::NextFace()
{
  return (MYON_SubDComponentPtr::Type::Face == m_ctype) ? (const MYON_SubDFace*)InternalNext() : nullptr;
}    

const MYON_SubDComponentBase* MYON_SubDLevelComponentIdIterator::InternalFirst()
{
  m_prev_id = 0;
  if (nullptr == m_first)
    return nullptr;
  m_current = nullptr;
  if (m_bLevelLinkedListIncreasingId)
  {
    m_current = m_first;
  }
  else
  {
    for (const MYON_SubDComponentBase* c = m_cidit.FirstComponent(); nullptr != c; c = m_cidit.NextComponent())
    {
      if (m_level_index != c->SubdivisionLevel())
        continue;
      m_current = (const MYON_SubDComponentBaseLink*)c;
      break;
    }
  }
  if (nullptr != m_current)
  {
    if (m_current->m_id <= m_prev_id)
    {
      // When this happens, searching by component id will be broken in the destination of the SubD copy.
      // It is a very serious bug, but we continue so something will get saved to the archive or be created in the copy.
      MYON_SUBD_ERROR("Iterator is not in order of increasing id.");
    }
    else
      m_prev_id = m_current->m_id;
    m_count = 1;
  }
  return m_current;
}


const MYON_SubDComponentBase* MYON_SubDLevelComponentIdIterator::InternalNext()
{
  if (nullptr == m_first || nullptr == m_current)
    return nullptr;
  if (m_bLevelLinkedListIncreasingId)
  {
    m_current = m_current->m_next;
  }
  else
  {
    m_current = nullptr;
    for (const MYON_SubDComponentBase* c = m_cidit.NextComponent(); nullptr != c; c = m_cidit.NextComponent())
    {
      if (m_level_index != c->SubdivisionLevel())
        continue;
      m_current = (const MYON_SubDComponentBaseLink*)c;
      break;
    }
  }
  if (nullptr != m_current)
  {
    if (m_current->m_id <= m_prev_id)
    {
      // When this happens, searching by component id will be broken in the destination of the SubD copy.
      // It is a very serious bug, but we continue so something will get saved to the archive or be created in the copy.
      MYON_SUBD_ERROR("Iterator is not in order of increasing id.");
    }
    else
      m_prev_id = m_current->m_id;
    ++m_count;
  }
  return m_current;
}



bool MYON_SubDLevel::CopyHelper(
  const class MYON_SubDimple& src_subdimple,
  const MYON_SubDLevel& src_level,
  class MYON_SubDArchiveIdMap& archive_id_map,
  class MYON_SubDimple& dest_subdimple,
  bool bCopyComponentStatus
  )
{
  bool rc = false;

  archive_id_map.Reset();

  m_surface_mesh.Clear();
  m_control_net_mesh.Clear();

  for (;;)
  {
    bool bLevelLinkedListIncreasingId[3] = {};
    if ( 0 == src_level.SetArchiveId(src_subdimple, archive_id_map.m_archive_id_partition, bLevelLinkedListIncreasingId) )
      break;

    unsigned int archive_id = 1;
    if ( archive_id != archive_id_map.m_archive_id_partition[0])
      break;

    // Have to use idit because subd editing (deleting and then adding) can leave the level's linked lists
    // with components in an order that is not increasing in id and it is critical that the next three for
    // loops iterate the level's components in order of increasing id.
    MYON_SubDLevelComponentIdIterator src_idit;

    // must iterate source vertices in order of increasing id
    src_idit.Initialize(bLevelLinkedListIncreasingId[0], MYON_SubDComponentPtr::Type::Vertex, src_subdimple, src_level);
    for (const MYON_SubDVertex* source_vertex = src_idit.FirstVertex(); nullptr != source_vertex; source_vertex = src_idit.NextVertex(), archive_id++)
    {
      if (archive_id != source_vertex->ArchiveId())
        break;
      MYON_SubDVertex* vertex = archive_id_map.AddCopy(source_vertex,dest_subdimple);
      if (nullptr == vertex )
        break;
      if (archive_id != vertex->ArchiveId())
        break;
      this->AddVertex(vertex);
      if ( bCopyComponentStatus )
        vertex->m_status = source_vertex->m_status;
    }
    if ( archive_id != archive_id_map.m_archive_id_partition[1])
      break;

    // must iterate source edges in order of increasing id
    src_idit.Initialize(bLevelLinkedListIncreasingId[1], MYON_SubDComponentPtr::Type::Edge, src_subdimple, src_level);
    for (const MYON_SubDEdge* source_edge = src_idit.FirstEdge(); nullptr != source_edge; source_edge = src_idit.NextEdge(), archive_id++)
    {
      if (archive_id != source_edge->ArchiveId())
        break;
      MYON_SubDEdge* edge = archive_id_map.AddCopy(source_edge,dest_subdimple);
      if (nullptr == edge )
        break;
      if (archive_id != edge->ArchiveId())
        break;
      this->AddEdge(edge);
      if ( bCopyComponentStatus )
        edge->m_status = source_edge->m_status;
    }
    if ( archive_id != archive_id_map.m_archive_id_partition[2])
      break;

    // must iterate source faces in order of increasing id
    src_idit.Initialize(bLevelLinkedListIncreasingId[2], MYON_SubDComponentPtr::Type::Face, src_subdimple, src_level);
    for (const MYON_SubDFace* source_face = src_idit.FirstFace(); nullptr != source_face; source_face = src_idit.NextFace(), archive_id++)
    {
      if (archive_id != source_face->ArchiveId())
        break;
      MYON_SubDFace* face = archive_id_map.AddCopy(source_face,dest_subdimple);
      if (nullptr == face )
        break;
      if (archive_id != face->ArchiveId())
        break;
      this->AddFace(face);
      if ( bCopyComponentStatus )
        face->m_status = source_face->m_status;
    }
    if ( archive_id != archive_id_map.m_archive_id_partition[3])
      break;

    if (0 == archive_id_map.ConvertArchiveIdsToRuntimePointers())
      break;

    for (int meshdex = 0; meshdex < 2; meshdex++)
    {
      MYON_SubDMesh* src_mesh = (0==meshdex) ? (&src_level.m_surface_mesh) : (&src_level.m_control_net_mesh);
      MYON_SubDMesh* this_mesh = (0 == meshdex) ? (&m_surface_mesh) : (&m_control_net_mesh);
      if (false == src_mesh->IsEmpty())
      {
        MYON_SubDMesh local_subd_mesh;
        local_subd_mesh.CopyFrom(*src_mesh);
        for (const MYON_SubDMeshFragment* fragment = local_subd_mesh.FirstFragment(); nullptr != fragment; fragment = fragment->m_next_fragment)
        {
          if (nullptr != fragment->m_face)
          {
            archive_id = fragment->m_face->ArchiveId();
            MYON_SubDComponentPtr cptr = MYON_SubDArchiveIdMap::FromFace(fragment->m_face);
            MYON_SubDFacePtr fptr = cptr.FacePtr();
            const_cast<MYON_SubDMeshFragment*>(fragment)->m_face = nullptr;
            if (0 != archive_id)
            {
              if (archive_id_map.ConvertArchiveIdToRuntimeFacePtr(1, 1, &fptr, 0, nullptr))
              {
                const_cast<MYON_SubDMeshFragment*>(fragment)->m_face = fptr.Face();
                if (nullptr != fragment->m_face)
                  continue;
              }
            }
          }
          local_subd_mesh = MYON_SubDMesh::Empty;
          break;
        }
        if (false == local_subd_mesh.IsEmpty())
          MYON_SubDMesh::Swap(*this_mesh, local_subd_mesh);
      }
    }

    this->ClearArchiveId();

    this->m_level_index = src_level.m_level_index;
    this->m_aggregates = src_level.m_aggregates;
    this->m_aggregates.MarkAllAsNotCurrent();

    rc = true;
    break;
  }

  archive_id_map.Reset();
  src_level.ClearArchiveId();

  if ( false == rc )
    return MYON_SUBD_RETURN_ERROR(false);

  return rc;
} // archive_id_map

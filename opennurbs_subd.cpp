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


MYON_SubDToBrepParameters::VertexProcess MYON_SubDToBrepParameters::VertexProcessFromUnsigned(
  unsigned int vertex_process_as_unsigned
)
{
  switch (vertex_process_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDToBrepParameters::VertexProcess::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDToBrepParameters::VertexProcess::LocalG1);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDToBrepParameters::VertexProcess::LocalG2);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDToBrepParameters::VertexProcess::LocalG1x);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDToBrepParameters::VertexProcess::LocalG1xx);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDToBrepParameters::VertexProcess::None);
}

const MYON_wString MYON_SubDToBrepParameters::VertexProcessToString(
  MYON_SubDToBrepParameters::VertexProcess vertex_process
)
{
  const wchar_t* s;
  switch (vertex_process)
  {
  case MYON_SubDToBrepParameters::VertexProcess::None:
    s = L"None";
    break;
  case MYON_SubDToBrepParameters::VertexProcess::LocalG1:
    s = L"G1";
    break;
  case MYON_SubDToBrepParameters::VertexProcess::LocalG2:
    s = L"G2";
    break;
  case MYON_SubDToBrepParameters::VertexProcess::LocalG1x:
    s = L"G1x";
    break;
  case MYON_SubDToBrepParameters::VertexProcess::LocalG1xx:
    s = L"G1xx";
    break;
  default:
    s = L"INVALID";
    break;
  }
  return MYON_wString(s);
}

MYON_SubDToBrepParameters::VertexProcess MYON_SubDToBrepParameters::ExtraordinaryVertexProcess() const
{
  return m_extraordinary_vertex_process;
}


int MYON_SubDToBrepParameters::Compare(
  const MYON_SubDToBrepParameters& lhs,
  const MYON_SubDToBrepParameters& rhs
)
{
  unsigned int a = lhs.m_bPackFaces ? 1 : 0;
  unsigned int b = rhs.m_bPackFaces ? 1 : 0;
  if (a < b)
    return -1;
  if (a > b)
    return 1;

  a = static_cast<unsigned char>(lhs.m_extraordinary_vertex_process);
  b = static_cast<unsigned char>(rhs.m_extraordinary_vertex_process);
  if (a < b)
    return -1;
  if (a > b)
    return 1;

  return 0;
}

int MYON_SubDToBrepParameters::CompareFromPointers(
  const MYON_SubDToBrepParameters* lhs,
  const MYON_SubDToBrepParameters* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  return MYON_SubDToBrepParameters::Compare(*lhs, *rhs);
}

bool operator==(const MYON_SubDToBrepParameters& lhs, const MYON_SubDToBrepParameters& rhs)
{
  return 0 == MYON_SubDToBrepParameters::Compare(lhs, rhs);
}

bool operator!=(const MYON_SubDToBrepParameters& lhs, const MYON_SubDToBrepParameters& rhs)
{
  return (0 != MYON_SubDToBrepParameters::Compare(lhs, rhs));
}

void MYON_SubDToBrepParameters::SetExtraordinaryVertexProcess(
  MYON_SubDToBrepParameters::VertexProcess ev_process
)
{
  m_extraordinary_vertex_process = ev_process;
}

bool MYON_SubDToBrepParameters::PackFaces() const
{
  return m_bPackFaces;
}

void MYON_SubDToBrepParameters::SetPackFaces(
  bool bPackFaces
)
{
  m_bPackFaces = bPackFaces ? true : false;
}

const MYON_wString MYON_SubDToBrepParameters::ToString( bool bVerbose ) const
{
  const MYON_wString exvtx(MYON_SubDToBrepParameters::VertexProcessToString(ExtraordinaryVertexProcess()));
  const MYON_wString faces(PackFaces() ? L"Packed" : L"Unpacked");

  const MYON_wString s = MYON_wString::FormatToString(L"Faces = %ls ExtraordinaryVertex = %ls",
    static_cast<const wchar_t*>(faces),
    static_cast<const wchar_t*>(exvtx)
    );

  return bVerbose ? (MYON_wString(L"MYON_SubDToBrepParameters: ") + s) : s;
}

bool MYON_SubDToBrepParameters::Read(MYON_BinaryArchive& archive)
{
  *this = MYON_SubDToBrepParameters::Default;
  int version = 0;
  if (false == archive.BeginRead3dmAnonymousChunk(&version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (version < 1)
      break;

    bool bPackFaces = this->PackFaces();
    if (false == archive.ReadBool(&bPackFaces))
      break;
    this->SetPackFaces(bPackFaces);

    unsigned u = static_cast<unsigned char>(this->ExtraordinaryVertexProcess());
    if (false == archive.ReadInt(&u))
      break;
    MYON_SubDToBrepParameters::VertexProcess exvtx = MYON_SubDToBrepParameters::VertexProcessFromUnsigned(u);
    this->SetExtraordinaryVertexProcess(exvtx);

    rc = true;
    break;
  }

  if (false == archive.EndRead3dmChunk())
    rc = false;
  return rc;
}

bool MYON_SubDToBrepParameters::Write(MYON_BinaryArchive& archive) const
{
  if (false == archive.BeginWrite3dmAnonymousChunk(1))
    return false;

  bool rc = false;
  for (;;)
  {
    if (false == archive.WriteBool(PackFaces()))
      break;
    const unsigned u = static_cast<unsigned char>(this->ExtraordinaryVertexProcess());
    if (false == archive.WriteInt(u))
      break;
    rc = true;
    break;
  }

  if (false == archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

MYON_SubDComponentPtr::Type MYON_SubDComponentPtr::ComponentPtrTypeFromUnsigned(
  unsigned int element_pointer_type_as_unsigned
  )
{
  switch (element_pointer_type_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDComponentPtr::Type::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDComponentPtr::Type::Vertex);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDComponentPtr::Type::Edge);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDComponentPtr::Type::Face);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDComponentPtr::Type::Unset);
}

MYON_SubDVertexTag MYON_SubD::VertexTagFromUnsigned(
  unsigned int vertex_tag_as_unsigned
  )
{
  switch (vertex_tag_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDVertexTag::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDVertexTag::Smooth);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDVertexTag::Crease);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDVertexTag::Corner);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDVertexTag::Dart);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDVertexTag::Unset);
}

const MYON_wString MYON_SubD::VertexTagToString(
  MYON_SubDVertexTag vertex_tag,
  bool bVerbose
)
{
  const wchar_t* tag_name;
  switch (vertex_tag)
  {
  case MYON_SubDVertexTag::Unset:
    tag_name = L"Unset";
    break;
  case MYON_SubDVertexTag::Smooth:
    tag_name = L"Smooth";
    break;
  case MYON_SubDVertexTag::Crease:
    tag_name = L"Crease";
    break;
  case MYON_SubDVertexTag::Corner:
    tag_name = L"Corner";
    break;
  case MYON_SubDVertexTag::Dart:
    tag_name = L"Dart";
    break;
  default:
    tag_name = L"invalid";
    break;
  }
  return bVerbose ? MYON_wString::FormatToString(L"MYON_SubDVertexTag::%ls", tag_name) : MYON_wString(tag_name);
}

const MYON_wString MYON_SubDVertex::ToString(
  bool bIncludeControlNetPoint,
  bool bIncludeTopology
) const
{
  const MYON_wString vtag = MYON_SubD::VertexTagToString(m_vertex_tag,false);
  MYON_wString v
    = bIncludeControlNetPoint
    ? MYON_wString::FormatToString(L"v%u %ls (%g,%g,%g)", m_id, static_cast<const wchar_t*>(vtag), m_P[0], m_P[1], m_P[2])
    : MYON_wString::FormatToString(L"v%u %ls", m_id, static_cast<const wchar_t*>(vtag))
    ;
  if (bIncludeTopology)
  {
    MYON_wString elist = MYON_wString::FormatToString(L" Edges[%u]", EdgeCount());
    if (nullptr != m_edges && m_edge_count > 0)
    {
      elist += L"={";
      for (unsigned short vei = 0; vei < m_edge_count; ++vei)
      {
        const MYON_SubDEdgePtr eptr = m_edges[vei];
        const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
        if (vei > 0)
          elist += L", ";
        if (nullptr == e)
          elist += L"Null";
        else
        {
          elist += (0 == MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr)) ? MYON_wString::FormatToString(L"+e%u", e->m_id) : MYON_wString::FormatToString(L"-e%u", e->m_id);
        }
      }
      elist += L"}";
    }
    v += elist;

    MYON_wString flist = MYON_wString::FormatToString(L" Faces[%u]", FaceCount());
    if (nullptr != m_faces && m_face_count > 0)
    {
      flist += L"={";
      for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
      {
        const MYON_SubDFace* f = m_faces[vfi];
        if (vfi > 0)
          flist += L", ";
        if (nullptr == f)
          flist += L"Null";
        else
        {
          flist += MYON_wString::FormatToString(L"f%u", f->m_id);
        }
      }
      flist += L"}";
    }
    v += flist;
  }
  return v;
}


bool MYON_SubD::VertexTagIsSet(
  MYON_SubDVertexTag vertex_tag
)
{
  return (
    MYON_SubDVertexTag::Smooth == vertex_tag
    || MYON_SubDVertexTag::Crease == vertex_tag
    || MYON_SubDVertexTag::Corner == vertex_tag
    || MYON_SubDVertexTag::Dart == vertex_tag
    );
}


MYON_SubDEdgeTag MYON_SubD::EdgeTagFromUnsigned(
  unsigned int edge_tag_as_unsigned
  )
{
  switch (edge_tag_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEdgeTag::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEdgeTag::Smooth);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEdgeTag::Crease);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEdgeTag::SmoothX);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgeTag::Unset);
}


const MYON_wString MYON_SubD::EdgeTagToString(
  MYON_SubDEdgeTag edge_tag,
  bool bVerbose
)
{
  const wchar_t* tag_name;
  switch (edge_tag)
  {
  case MYON_SubDEdgeTag::Unset:
    tag_name = L"Unset";
    break;
  case MYON_SubDEdgeTag::Smooth:
    tag_name = L"Smooth";
    break;
  case MYON_SubDEdgeTag::Crease:
    tag_name = L"Crease";
    break;
  case MYON_SubDEdgeTag::SmoothX:
    tag_name = L"SmoothX";
    break;
  default:
    tag_name = L"invalid";
    break;
  }
  return bVerbose ? MYON_wString::FormatToString(L"MYON_SubDEdgeTag::%ls", tag_name) : MYON_wString(tag_name);
}


bool MYON_SubD::EdgeTagIsSet(
  MYON_SubDEdgeTag edge_tag
)
{
  return (
    MYON_SubDEdgeTag::Smooth == edge_tag
    || MYON_SubDEdgeTag::Crease == edge_tag
    || MYON_SubDEdgeTag::SmoothX == edge_tag
    );
}

MYON_SubD::VertexFacetType MYON_SubD::VertexFacetTypeFromUnsigned(
  unsigned int vertex_facet_type_as_unsigned
  )
{
  switch (vertex_facet_type_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubD::VertexFacetType::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubD::VertexFacetType::Tri);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubD::VertexFacetType::Quad);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubD::VertexFacetType::Ngon);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubD::VertexFacetType::Mixed);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubD::VertexFacetType::Unset);
}

unsigned int MYON_SubDSectorType::SectorPointRingCountFromEdgeCount(
  MYON_SubDVertexTag vertex_tag,
  unsigned int sector_edge_count
  )
{
  if (sector_edge_count >= MYON_SubDSectorType::MinimumSectorEdgeCount(vertex_tag) && sector_edge_count <= MYON_SubDVertex::MaximumEdgeCount)
  {
    if (MYON_SubDVertexTag::Smooth == vertex_tag || MYON_SubDVertexTag::Dart == vertex_tag)
    {
      // interior vertex
      return (2 * sector_edge_count + 1);
    }

    if (MYON_SubDVertexTag::Crease == vertex_tag || MYON_SubDVertexTag::Corner == vertex_tag)
    {
      // boundary vertex
      return (2 * sector_edge_count);
    }
  }
  return MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubDSectorType::SectorPointRingCountFromFaceCount(
  MYON_SubDVertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  unsigned int sector_edge_count = MYON_SubDSectorType::SectorEdgeCountFromFaceCount(vertex_tag,sector_face_count);
  return (sector_edge_count > 0)
    ? MYON_SubDSectorType::SectorPointRingCountFromEdgeCount(vertex_tag,sector_edge_count)
    : MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubDSectorType::SectorFaceCountFromEdgeCount(
  MYON_SubDVertexTag vertex_tag,
  unsigned int sector_edge_count
  )
{
  if (sector_edge_count >= 2 && sector_edge_count <= MYON_SubDVertex::MaximumEdgeCount)
  {
    unsigned int sector_face_count
      = (MYON_SubDVertexTag::Crease == vertex_tag || MYON_SubDVertexTag::Corner == vertex_tag)
      ? sector_edge_count-1
      : sector_edge_count;
    return sector_face_count;
  }
  return MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubDSectorType::SectorEdgeCountFromFaceCount(
  MYON_SubDVertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  if (sector_face_count > 0 && sector_face_count <= MYON_SubDVertex::MaximumFaceCount)
  {
    unsigned int sector_edge_count
      = (MYON_SubDVertexTag::Crease == vertex_tag || MYON_SubDVertexTag::Corner == vertex_tag)
      ? sector_face_count+1
      : sector_face_count;
    return sector_edge_count;
  }
  return MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubDSectorType::MinimumSectorEdgeCount(
  MYON_SubDVertexTag vertex_tag
  )
{
  if (MYON_SubDVertexTag::Smooth == vertex_tag || MYON_SubDVertexTag::Dart == vertex_tag)
    return MYON_SubDSectorType::MinimumSectorFaceCount(vertex_tag);
  if (MYON_SubDVertexTag::Corner == vertex_tag || MYON_SubDVertexTag::Crease == vertex_tag)
    return MYON_SubDSectorType::MinimumSectorFaceCount(vertex_tag)+1;
  return MYON_UNSET_UINT_INDEX;
}

unsigned int MYON_SubDSectorType::MinimumSectorFaceCount(
  MYON_SubDVertexTag vertex_tag
  )
{
  unsigned int minimum_sector_face_count;
  switch (vertex_tag)
  {
  case MYON_SubDVertexTag::Unset:
    MYON_SUBD_ERROR("Unset tag.");
    minimum_sector_face_count = MYON_UNSET_UINT_INDEX;
    break;

  case MYON_SubDVertexTag::Smooth:
    // April 2019 Dale Lear - Catmull Clark Valence 2 Evaluation: Smooth
    // See comments in MYON_SubDSectorType::GetSurfaceEvaluationCoefficients()
    // for more details on how this case is handled.
    minimum_sector_face_count = 2; // 3 without special case handling
    break;

  case MYON_SubDVertexTag::Crease:
    // A "wire" crease can have zero faces - this is the minimum when faces exist
    minimum_sector_face_count = 1;
    break;

  case MYON_SubDVertexTag::Corner:
    // A "wire" corner can have zero faces - this is the minimum when faces exist
    minimum_sector_face_count = 1;
    break;

  case MYON_SubDVertexTag::Dart:
    // April 2019 Dale Lear - Catmull Clark Valence 2 Evaluation: Dart
    // See comments in MYON_SubDSectorType::GetSurfaceEvaluationCoefficients()
    // for more details on how this case is handled.
    minimum_sector_face_count = 2; // 3 without special case handling
    break;

  default:
    MYON_SUBD_ERROR("Invalid tag.");
    minimum_sector_face_count = MYON_UNSET_UINT_INDEX;
    break;
  }

  return minimum_sector_face_count;
}

bool MYON_SubD::IsValidSectorEdgeCount(
  MYON_SubDVertexTag vertex_tag,
  unsigned int sector_edge_count
  )
{
  return (sector_edge_count >= MYON_SubDSectorType::MinimumSectorEdgeCount(vertex_tag) && sector_edge_count <= MYON_SubDVertex::MaximumEdgeCount);
}

bool MYON_SubD::IsValidSectorFaceCount(
  MYON_SubDVertexTag vertex_tag,
  unsigned int sector_face_count
  )
{
  return (sector_face_count >= MYON_SubDSectorType::MinimumSectorFaceCount(vertex_tag) && sector_face_count <= MYON_SubDVertex::MaximumFaceCount);
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexPtr
//
//
bool MYON_SubDVertexPtr::IsNull() const
{
  return (nullptr == MYON_SUBD_VERTEX_POINTER(m_ptr));
}

bool MYON_SubDVertexPtr::IsNotNull() const
{
  return (nullptr != MYON_SUBD_VERTEX_POINTER(m_ptr));
}

unsigned int MYON_SubDVertexPtr::VertexId() const
{
  const MYON_SubDVertex* v = MYON_SUBD_VERTEX_POINTER(m_ptr);
  return (nullptr != v) ? v->m_id : 0U;
}

class MYON_SubDVertex* MYON_SubDVertexPtr::Vertex() const
{
  return MYON_SUBD_VERTEX_POINTER(m_ptr);
}

MYON__UINT_PTR MYON_SubDVertexPtr::VertexDirection() const
{
  return MYON_SUBD_VERTEX_DIRECTION(m_ptr);
}

void MYON_SubDVertexPtr::ClearSavedSubdivisionPoints() const
{
  const MYON_SubDVertex* v = MYON_SUBD_VERTEX_POINTER(m_ptr);
  if (nullptr != v)
    v->ClearSavedSubdivisionPoints();
}

void MYON_SubDVertexPtr::ClearSavedSubdivisionPoints(
  bool bClearNeighborhood
) const
{
  const MYON_SubDVertex* v = MYON_SUBD_VERTEX_POINTER(m_ptr);
  if (nullptr != v)
    v->ClearSavedSubdivisionPoints(bClearNeighborhood);
}

const MYON_ComponentStatus MYON_SubDVertexPtr::Status() const
{
  const MYON_SubDVertex* vertex = MYON_SUBD_VERTEX_POINTER(m_ptr);
  return (nullptr == vertex) ? MYON_ComponentStatus::NoneSet : vertex->m_status;
}

const MYON_SubDVertexPtr MYON_SubDVertexPtr::Create(
  const class MYON_SubDVertex* vertex
  )
{
  return MYON_SubDVertexPtr::Create(vertex,0);
}

const MYON_SubDVertexPtr MYON_SubDVertexPtr::Create(
  const class MYON_SubDVertex* vertex,
  MYON__UINT_PTR vertex_mark
  )
{
  MYON_SubDVertexPtr vptr = { (MYON__UINT_PTR)vertex | (vertex_mark & MYON_SUBD_COMPONENT_DIRECTION_MASK) };
  return vptr;
}

const MYON_SubDVertexPtr MYON_SubDVertexPtr::Create(
  const class MYON_SubDComponentPtr& vertex_element
  )
{
  return MYON_SubDVertexPtr::Create(vertex_element.Vertex(), vertex_element.ComponentDirection());
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgePtr
//


bool MYON_SubDEdgePtr::IsNull() const
{
  return (nullptr == MYON_SUBD_EDGE_POINTER(m_ptr));
}

bool MYON_SubDEdgePtr::IsNotNull() const
{
  return (nullptr != MYON_SUBD_EDGE_POINTER(m_ptr));
}

bool MYON_SubDEdgePtr::IsNotNullAndVerticesAreNotNull() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e && nullptr != e->m_vertex[0] && nullptr != e->m_vertex[1]);
}


class MYON_SubDEdge* MYON_SubDEdgePtr::Edge() const
{
  return MYON_SUBD_EDGE_POINTER(m_ptr);
}

unsigned int MYON_SubDEdgePtr::EdgeId() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->m_id : 0U;
}

unsigned int MYON_SubDEdgePtr::EdgeFaceCount() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? ((unsigned int)e->m_face_count) : 0U;
}

MYON_SubDEdgeTag MYON_SubDEdgePtr::EdgeTag() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->m_edge_tag : MYON_SubDEdgeTag::Unset;
}

bool MYON_SubDEdgePtr::EdgeIsSmooth() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsSmooth() : false;
}   

bool MYON_SubDEdgePtr::EdgeIsCrease() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsCrease() : false;
}   

bool MYON_SubDEdgePtr::EdgeIsHardCrease() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsHardCrease() : false;
}   

bool MYON_SubDEdgePtr::EdgeIsDartCrease() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsDartCrease() : false;
}

bool MYON_SubDEdgePtr::HasInteriorEdgeTopology(
  bool bRequireOppositeFaceDirections
) const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->HasInteriorEdgeTopology(bRequireOppositeFaceDirections) : false;
}


MYON__UINT_PTR MYON_SubDEdgePtr::EdgeDirection() const
{
  return MYON_SUBD_EDGE_DIRECTION(m_ptr);
}


const MYON_3dPoint MYON_SubDEdgePtr::SubdivisionPoint() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->SubdivisionPoint() : MYON_3dPoint::NanPoint;
}

void MYON_SubDEdgePtr::ClearSavedSubdivisionPoints() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  if (nullptr != e)
    e->ClearSavedSubdivisionPoints();
}

void MYON_SubDEdgePtr::ClearSavedSubdivisionPoints(
  bool bClearNeighborhood
) const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  if (nullptr != e)
    e->ClearSavedSubdivisionPoints(bClearNeighborhood);
}

const class MYON_SubDVertex* MYON_SubDEdgePtr::RelativeVertex(
  int relative_vertex_index
  ) const
{
  for (;;)
  {
    if (relative_vertex_index < 0 || relative_vertex_index>1)
      break;
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
    if (nullptr == edge)
      break;
    if (0 != MYON_SUBD_EDGE_DIRECTION(m_ptr))
      relative_vertex_index = 1 - relative_vertex_index;
    return edge->m_vertex[relative_vertex_index];
  }
  return nullptr;
}

const MYON_3dPoint MYON_SubDEdgePtr::RelativeVertexPoint(
  int relative_vertex_index,
  MYON_SubDComponentLocation point_location
) const
{
  const MYON_SubDVertex* v = this->RelativeVertex(relative_vertex_index);
  return (nullptr != v) ? v->Point(point_location) : MYON_3dPoint::NanPoint;
}


const MYON_3dPoint MYON_SubDEdgePtr::RelativeVertexSurfacePoint(
  int relative_vertex_index
) const
{
  const MYON_SubDVertex* v = this->RelativeVertex(relative_vertex_index);
  return (nullptr != v) ? v->SurfacePoint() : MYON_3dPoint::NanPoint;
}

unsigned MYON_SubDEdgePtr::RelativeVertexId(int relative_vertex_index) const
{
  const MYON_SubDVertex* v = this->RelativeVertex(relative_vertex_index);
  return (nullptr != v) ? v->m_id : 0U;
}

bool MYON_SubDEdgePtr::RelativeVertexMark(
  int relative_vertex_index,
  bool missing_vertex_return_value
) const
{
  const MYON_SubDVertex* v = this->RelativeVertex(relative_vertex_index);
  return (nullptr != v) ? v->Mark() : missing_vertex_return_value;
}

MYON__UINT8 MYON_SubDEdgePtr::RelativeVertexMarkBits(
  int relative_vertex_index,
  MYON__UINT8 missing_vertex_return_value
) const
{
  const MYON_SubDVertex* v = this->RelativeVertex(relative_vertex_index);
  return (nullptr != v) ? v->MarkBits() : missing_vertex_return_value;
}


const MYON_3dPoint MYON_SubDEdgePtr::RelativeControlNetPoint(
  int relative_vertex_index
) const
{
  const MYON_SubDVertex* v = RelativeVertex(relative_vertex_index);
  return (nullptr != v) ? v->ControlNetPoint() : MYON_3dPoint::NanPoint;
}

const MYON_Line MYON_SubDEdgePtr::RelativeControlNetLine() const
{
  return MYON_Line(RelativeControlNetPoint(0), RelativeControlNetPoint(1));
}

const MYON_3dVector MYON_SubDEdgePtr::RelativeControlNetDirection() const
{
  return RelativeControlNetLine().Direction();
}

double MYON_SubDEdgePtr::RelativeSectorCoefficient(
  int relative_vertex_index
) const
{
  for (;;)
  {
    if (relative_vertex_index < 0 || relative_vertex_index>1)
      break;
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
    if (nullptr == edge)
      break;
    if (0 != MYON_SUBD_EDGE_DIRECTION(m_ptr))
      relative_vertex_index = 1 - relative_vertex_index;
    return edge->m_sector_coefficient[relative_vertex_index];
  }
  return MYON_SubDSectorType::ErrorSectorCoefficient;
}

void  MYON_SubDEdgePtr::SetRelativeSectorCoefficientForExperts(
  int relative_vertex_index,
  double relative_sector_coefficient
) const
{
  for (;;)
  {
    if (relative_vertex_index < 0 || relative_vertex_index>1)
      break;
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
    if (nullptr == edge)
      break;
    if (0 != MYON_SUBD_EDGE_DIRECTION(m_ptr))
      relative_vertex_index = 1 - relative_vertex_index;
    edge->m_sector_coefficient[relative_vertex_index] = relative_sector_coefficient;
    break;
  }
}


const MYON_SubDEdgeSharpness MYON_SubDEdgePtr::RelativeSharpness() const
{
  const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
  if (nullptr != edge)
  {
    const MYON_SubDEdgeSharpness s = edge->Sharpness();
    return (0 == MYON_SUBD_EDGE_DIRECTION(m_ptr)) ? s : s.Reversed();
  }
  return MYON_SubDEdgeSharpness::Zero;
}

void MYON_SubDEdgePtr::SetRelativeSharpness(MYON_SubDEdgeSharpness relative_sharpness) const
{
  MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
  if (nullptr != edge)
    edge->SetSharpnessForExperts(
      (0 == MYON_SUBD_EDGE_DIRECTION(m_ptr))
      ? relative_sharpness 
      : relative_sharpness.Reversed()
    );
}

const MYON_3dVector MYON_SubDEdgePtr::RelativeDirection() const
{
  for (;;)
  {
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
    if (nullptr == edge)
      break;
    if (nullptr == edge->m_vertex[0] || nullptr == edge->m_vertex[1])
      break;
    const int i0 = (0 != MYON_SUBD_EDGE_DIRECTION(m_ptr)) ? 1 : 0;
    const MYON_3dPoint P0(edge->m_vertex[i0]->m_P);
    const MYON_3dPoint P1(edge->m_vertex[1-i0]->m_P);
    return (P1 - P0);
  }
  return MYON_3dVector::NanVector;
}

const class MYON_SubDFace* MYON_SubDEdgePtr::RelativeFace(
  int relative_face_index
) const
{
  if (relative_face_index < 0 || relative_face_index > 1)
    return nullptr; // invalid input
  
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  if (nullptr == e )
    return nullptr; // null input

  if (nullptr == e || e->m_face_count > 2)
    return nullptr; // nonmanifold edge

  const MYON_SubDFace* LR[2] = {};
  for (unsigned short efi = 0; efi < e->m_face_count; ++efi)
  {
    const MYON__UINT_PTR ptr = e->m_face2[efi].m_ptr;
    const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(ptr);
    if (nullptr == f)
      continue;
    if (0 == MYON_SUBD_FACE_DIRECTION(ptr))
    {
      if (nullptr != LR[0])
        return nullptr; // not an oriented manifold interior edge
      LR[0] = f;
    }
    else
    {
      if (nullptr != LR[1])
        return nullptr; // not an oriented manifold interior edge
      LR[1] = f;
    }
  }

  if (0 != MYON_SUBD_EDGE_DIRECTION(m_ptr))
    relative_face_index = 1 - relative_face_index;
  return LR[relative_face_index];
}

bool MYON_SubDEdgePtr::RelativeFaceMark(
  int relative_face_index,
  bool missing_face_return_value
) const
{
  const MYON_SubDFace* f = this->RelativeFace(relative_face_index);
  return (nullptr != f) ? f->Mark() : missing_face_return_value;
}

MYON__UINT8 MYON_SubDEdgePtr::RelativeFaceMarkBits(
  int relative_face_index,
  MYON__UINT8 missing_face_return_value
) const
{
  const MYON_SubDFace* f = this->RelativeFace(relative_face_index);
  return (nullptr != f) ? f->MarkBits() : missing_face_return_value;
}

const MYON_ComponentStatus MYON_SubDEdgePtr::Status() const
{
  const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr == edge) ? MYON_ComponentStatus::NoneSet : edge->m_status;
}

const MYON_SubDEdgePtr MYON_SubDEdgePtr::Reversed() const
{
  return MYON_SubDEdgePtr::Create(MYON_SUBD_EDGE_POINTER(m_ptr), 1 - (m_ptr & 1));
}

const MYON_SubDEdgePtr MYON_SubDEdgePtr::Create(
  const class MYON_SubDEdge* edge
  )
{
  MYON_SubDEdgePtr eptr = { (MYON__UINT_PTR)edge };
  return eptr;
}

const MYON_SubDEdgePtr MYON_SubDEdgePtr::Create(
  const class MYON_SubDEdge* edge,
  MYON__UINT_PTR direction
  )
{
  MYON_SubDEdgePtr eptr = { (MYON__UINT_PTR)edge | (direction & MYON_SUBD_COMPONENT_DIRECTION_MASK) };
  return eptr;
}

const MYON_SubDEdgePtr MYON_SubDEdgePtr::Create(
  const class MYON_SubDVertex* v0,
  const class MYON_SubDVertex* v1
)
{
  return MYON_SubDEdge::FromVertices(v0,v1);
}

const MYON_SubDEdgePtr MYON_SubDEdgePtr::Create(
  const class MYON_SubDComponentPtr& edge_element
  )
{
  return MYON_SubDEdgePtr::Create(edge_element.Edge(), edge_element.ComponentDirection());
}

const MYON_SubDEdgePtr MYON_SubDEdgePtr::CreateFromStartVertex(
  const class MYON_SubDEdge* edge,
  const MYON_SubDVertex* start_vertex
)
{
  for (;;)
  {
    if (nullptr == edge || nullptr == start_vertex)
      break;
    if (edge->m_vertex[0] == edge->m_vertex[1])
      break;
    
    MYON__UINT_PTR dir;
    if (start_vertex == edge->m_vertex[0])
      dir = 0;
    else if (start_vertex == edge->m_vertex[1])
      dir = 1;
    else
      break;
    return MYON_SubDEdgePtr::Create(edge, dir);
  }
  return MYON_SubDEdgePtr::Null;
}

const MYON_SubDEdgePtr MYON_SubDEdgePtr::CreateFromEndVertex(
  const class MYON_SubDEdge* edge,
  const MYON_SubDVertex* end_vertex
)
{
  return CreateFromStartVertex(edge,end_vertex).Reversed();
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFacePtr
//

bool MYON_SubDFacePtr::IsNull() const
{
  return (nullptr == MYON_SUBD_FACE_POINTER(m_ptr));
}

bool MYON_SubDFacePtr::IsNotNull() const
{
  return (nullptr != MYON_SUBD_FACE_POINTER(m_ptr));
}

MYON_SubDFace* MYON_SubDFacePtr::Face() const
{
  return MYON_SUBD_FACE_POINTER(m_ptr);
}

void MYON_SubDFacePtr::ClearSavedSubdivisionPoints() const
{
  const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(m_ptr);
  if (nullptr != f)
    f->ClearSavedSubdivisionPoints();
}

void MYON_SubDFacePtr::ClearSavedSubdivisionPoints(
  bool bClearNeighborhood
) const
{
  const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(m_ptr);
  if (nullptr != f)
    f->ClearSavedSubdivisionPoints(bClearNeighborhood);
}


unsigned int MYON_SubDFacePtr::FaceId() const
{
  const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr != f) ? f->m_id : 0U;
}

unsigned int MYON_SubDFacePtr::FaceEdgeCount() const
{
  const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr != f) ? ((unsigned int)f->m_edge_count) : 0U;
}

MYON__UINT_PTR MYON_SubDFacePtr::FaceDirection() const
{
  return MYON_SUBD_FACE_DIRECTION(m_ptr);
}

const MYON_ComponentStatus MYON_SubDFacePtr::Status() const
{
  const MYON_SubDFace* face = MYON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr == face) ? MYON_ComponentStatus::NoneSet : face->m_status;
}

const MYON_SubDFacePtr MYON_SubDFacePtr::Reversed() const
{
  return MYON_SubDFacePtr::Create(MYON_SUBD_FACE_POINTER(m_ptr), 1 - (m_ptr & 1));
}

const MYON_SubDFacePtr MYON_SubDFacePtr::Create(
  const class MYON_SubDFace* face,
  MYON__UINT_PTR direction
  )
{
  MYON_SubDFacePtr fptr = { (MYON__UINT_PTR)face | (direction & MYON_SUBD_COMPONENT_DIRECTION_MASK) };
  return fptr;
}

const MYON_SubDFacePtr MYON_SubDFacePtr::Create(
  const class MYON_SubDComponentPtr& face_element
  )
{
  return MYON_SubDFacePtr::Create(face_element.Face(), face_element.ComponentDirection());
}


int MYON_SubDFacePtr::Compare(
  const MYON_SubDFacePtr* lhs,
  const MYON_SubDFacePtr* rhs
)
{
  if ( nullptr == lhs )
    return 1;
  if ( nullptr == rhs )
    return -1;

  if (lhs->m_ptr < rhs->m_ptr)
    return -1;
  if (lhs->m_ptr > rhs->m_ptr)
    return 1;

  return 0;
}

int MYON_SubDFacePtr::CompareFacePointer(
  const MYON_SubDFacePtr* lhs,
  const MYON_SubDFacePtr* rhs
)
{
  if (lhs == rhs)
    return 0;
  if ( nullptr == lhs )
    return 1;
  if ( nullptr == rhs )
    return -1;

  const MYON__UINT_PTR lhs_ptr = (lhs->m_ptr & MYON_SUBD_COMPONENT_POINTER_MASK);
  const MYON__UINT_PTR rhs_ptr = (rhs->m_ptr & MYON_SUBD_COMPONENT_POINTER_MASK);
  if (lhs_ptr < rhs_ptr)
    return -1;
  if (lhs_ptr > rhs_ptr)
    return 1;
  return 0;
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentPtr
//

bool MYON_SubDComponentPtr::IsNull() const
{
  return nullptr == ComponentBase();
}

bool MYON_SubDComponentPtr::IsNotNull() const
{
  return nullptr != ComponentBase();
}

unsigned int MYON_SubDComponentPtr::ComponentId() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_id : 0U;
}

void MYON_SubDComponentPtr::ClearSavedSubdivisionPoints() const
{
  switch (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case MYON_SUBD_COMPONENT_TYPE_VERTEX:
  {
    const MYON_SubDVertex* v = Vertex();
    if (nullptr != v)
      v->ClearSavedSubdivisionPoints();
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_EDGE:
  {
    const MYON_SubDEdge* e = Edge();
    if (nullptr != e)
      e->ClearSavedSubdivisionPoints();
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_FACE:
  {
    const MYON_SubDFace* f = Face();
    if (nullptr != f)
      f->ClearSavedSubdivisionPoints();
  }
  break;
  }
}

void MYON_SubDComponentPtr::ClearSavedSubdivisionPoints(
  bool bClearNeighborhood
) const
{
  switch (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case MYON_SUBD_COMPONENT_TYPE_VERTEX:
  {
    const MYON_SubDVertex* v = Vertex();
    if (nullptr != v)
      v->ClearSavedSubdivisionPoints(bClearNeighborhood);
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_EDGE:
  {
    const MYON_SubDEdge* e = Edge();
    if (nullptr != e)
      e->ClearSavedSubdivisionPoints(bClearNeighborhood);
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_FACE:
  {
    const MYON_SubDFace* f = Face();
    if (nullptr != f)
      f->ClearSavedSubdivisionPoints(bClearNeighborhood);
  }
  break;
  }
}

const MYON_3dPoint MYON_SubDComponentPtr::ControlNetCenterPoint() const
{
  switch (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case MYON_SUBD_COMPONENT_TYPE_VERTEX:
  {
    const MYON_SubDVertex* v = Vertex();
    if (nullptr != v)
      return v->ControlNetPoint();
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_EDGE:
  {
    const MYON_SubDEdge* e = Edge();
    if (nullptr != e)
      return e->ControlNetCenterPoint();
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_FACE:
  {
    const MYON_SubDFace* f = Face();
    if (nullptr != f)
      return f->ControlNetCenterPoint();
  }
  break;
  }
  return MYON_3dPoint::NanPoint;
}

const MYON_BoundingBox MYON_SubDComponentPtr::ControlNetBoundingBox() const
{
  switch (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case MYON_SUBD_COMPONENT_TYPE_VERTEX:
  {
    const MYON_SubDVertex* v = Vertex();
    if (nullptr != v)
      return v->ControlNetBoundingBox();
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_EDGE:
  {
    const MYON_SubDEdge* e = Edge();
    if (nullptr != e)
      return e->ControlNetBoundingBox();
  }
  break;
  case MYON_SUBD_COMPONENT_TYPE_FACE:
  {
    const MYON_SubDFace* f = Face();
    if (nullptr != f)
      return f->ControlNetBoundingBox();
  }
  break;
  }
  return MYON_BoundingBox::NanBoundingBox;
}



MYON__UINT16 MYON_SubDComponentPtr::Hash16FromTypeAndId() const
{
  const MYON_SubDComponentBase* c = ComponentBase();
  return (0 != c)
    ? MYON_CRC16((MYON__UINT16)(MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr), sizeof(c->m_id), &(c->m_id))
    : ((MYON__UINT16)0U)
    ;
}

MYON__UINT32 MYON_SubDComponentPtr::Hash32FromPointer() const
{
  const MYON__UINT_PTR ptr = (MYON__UINT_PTR)ComponentBase();
  return MYON_CRC32((MYON__UINT32)(MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr), sizeof(ptr), &ptr);
}


MYON_SubDComponentPtr::Type MYON_SubDComponentPtr::ComponentType() const
{
  switch (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr)
  {
  case MYON_SUBD_COMPONENT_TYPE_VERTEX:
    return MYON_SubDComponentPtr::Type::Vertex;
  case MYON_SUBD_COMPONENT_TYPE_EDGE:
    return MYON_SubDComponentPtr::Type::Edge;
  case MYON_SUBD_COMPONENT_TYPE_FACE:
    return MYON_SubDComponentPtr::Type::Face;
  }
  return MYON_SubDComponentPtr::Type::Unset;
}


const MYON_ComponentStatus MYON_SubDComponentPtr::Status() const
{
  switch (ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = Vertex();
      if ( nullptr != vertex )
        return vertex->m_status;
    }
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* edge = Edge();
      if ( nullptr != edge )
        return edge->m_status;
    }
    break;
  case MYON_SubDComponentPtr::Type::Face:
    {
      const MYON_SubDFace* face = Face();
      if ( nullptr != face )
        return face->m_status;
    }
    break;
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  }
  return MYON_ComponentStatus::NoneSet;
}

unsigned int MYON_SubDComponentPtr::SetStatus(
  MYON_ComponentStatus status
  )
{
  switch (ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = Vertex();
      if (nullptr != vertex)
        return vertex->m_status.SetStatus(status);
    }
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* edge = Edge();
      if (nullptr != edge)
        return edge->m_status.SetStatus(status);
    }
    break;
  case MYON_SubDComponentPtr::Type::Face:
    {
      const MYON_SubDFace* face = Face();
      if (nullptr != face)
        return face->m_status.SetStatus(status);
    }
    break;
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  }
  return MYON_SUBD_RETURN_ERROR(0);
}


unsigned int MYON_SubDComponentPtr::SetStates(
  MYON_ComponentStatus states_to_set
  )
{
  switch (ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = Vertex();
      if (nullptr != vertex)
        return vertex->m_status.SetStates(states_to_set);
    }
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* edge = Edge();
      if (nullptr != edge)
        return edge->m_status.SetStates(states_to_set);
    }
    break;
  case MYON_SubDComponentPtr::Type::Face:
    {
      const MYON_SubDFace* face = Face();
      if (nullptr != face)
        return face->m_status.SetStates(states_to_set);
    }
    break;
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  }
  return MYON_SUBD_RETURN_ERROR(0);
}


unsigned int MYON_SubDComponentPtr::ClearStates(
  MYON_ComponentStatus states_to_clear
  )
{
  switch (ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = Vertex();
      if (nullptr != vertex)
        return vertex->m_status.ClearStates(states_to_clear);
    }
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* edge = Edge();
      if (nullptr != edge)
        return edge->m_status.ClearStates(states_to_clear);
    }
    break;
  case MYON_SubDComponentPtr::Type::Face:
    {
      const MYON_SubDFace* face = Face();
      if (nullptr != face)
        return face->m_status.ClearStates(states_to_clear);
    }
    break;
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  }
  return MYON_SUBD_RETURN_ERROR(0);
}

MYON_SubDComponentTest::MYON_SubDComponentTest(MYON__UINT_PTR ptr)
: m_ptr(ptr)
{}

MYON_SubDComponentTest::~MYON_SubDComponentTest()
{}

bool MYON_SubDComponentTest::Passes(const MYON_SubDComponentPtr cptr) const
{
  // Default implementation of a virtual function that is typically overridden
  return cptr.IsNotNull() && 0 != m_ptr;
}

bool MYON_SubDComponentTest::Passes(const class MYON_SubDVertex* v) const
{
  return Passes((nullptr != v) ? v->ComponentPtr() : MYON_SubDComponentPtr::Null);
}

bool MYON_SubDComponentTest::Passes(const class MYON_SubDEdge* e) const
{
  return Passes((nullptr != e) ? e->ComponentPtr() : MYON_SubDComponentPtr::Null);
}

bool MYON_SubDComponentTest::Passes(const MYON_SubDFace* f) const
{
  return Passes((nullptr != f) ? f->ComponentPtr() : MYON_SubDComponentPtr::Null);
}

MYON_SubDComponentId::MYON_SubDComponentId(MYON_SubDComponentPtr::Type component_type, unsigned int component_id)
  : m_id(component_id)
  , m_type(component_type)
{}

MYON_SubDComponentId::MYON_SubDComponentId(MYON_SubDComponentPtr cptr)
{
  const MYON_SubDComponentBase* b = cptr.ComponentBase();
  if (nullptr != b)
  {
    m_id = b->m_id;
    m_type = cptr.ComponentType();
  }
}

MYON_SubDComponentId::MYON_SubDComponentId(const class MYON_SubDVertex* v)
{
  if (nullptr != v)
  {
    m_id = v->m_id;
    m_type = MYON_SubDComponentPtr::Type::Vertex;
  }
}

MYON_SubDComponentId::MYON_SubDComponentId(const class MYON_SubDEdge* e)
{
  if (nullptr != e)
  {
    m_id = e->m_id;
    m_type = MYON_SubDComponentPtr::Type::Edge;
  }
}

MYON_SubDComponentId::MYON_SubDComponentId(const class MYON_SubDFace* f)
{
  if (nullptr != f)
  {
    m_id = f->m_id;
    m_type = MYON_SubDComponentPtr::Type::Face;
  }
}


int MYON_SubDComponentId::CompareTypeAndId(const MYON_SubDComponentId& lhs, const MYON_SubDComponentId& rhs)
{
  if (static_cast<unsigned char>(lhs.m_type) < static_cast<unsigned char>(rhs.m_type))
    return -1;
  if (static_cast<unsigned char>(lhs.m_type) > static_cast<unsigned char>(rhs.m_type))
    return 1;
  if (lhs.m_id < rhs.m_id)
    return -1;
  if (lhs.m_id > rhs.m_id)
    return 1;
  return 0;
}

int MYON_SubDComponentId::CompareTypeAndIdFromPointer(const MYON_SubDComponentId* lhs, const MYON_SubDComponentId* rhs)
{
  if (lhs == rhs)
    return 0;

  // nullptr sorts to end of list
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;

  if (static_cast<unsigned char>(lhs->m_type) < static_cast<unsigned char>(rhs->m_type))
    return -1;
  if (static_cast<unsigned char>(lhs->m_type) > static_cast<unsigned char>(rhs->m_type))
    return 1;
  if (lhs->m_id < rhs->m_id)
    return -1;
  if (lhs->m_id > rhs->m_id)
    return 1;
  return 0;
}

unsigned int MYON_SubDComponentId::ComponentId() const
{
  return m_id;
}

MYON_SubDComponentPtr::Type MYON_SubDComponentId::ComponentType() const
{
  return m_type;
}

bool MYON_SubDComponentId::IsSet() const
{
  return 0 != m_id && MYON_SubDComponentPtr::Type::Unset != m_type;
}

bool MYON_SubDComponentIdList::Passes(const MYON_SubDComponentPtr cptr) const
{
  return InList(cptr) ? m_bInListPassesResult : !m_bInListPassesResult;
}

void MYON_SubDComponentIdList::AddId(MYON_SubDComponentId cid)
{
  if (cid.IsSet())
  {
    m_bSorted = false;
    m_id_list.Append(cid);
  }
}

void MYON_SubDComponentIdList::AddId(MYON_SubDComponentPtr cptr)
{
  AddId(MYON_SubDComponentId(cptr));
}

void MYON_SubDComponentIdList::AddIdList(const MYON_SubDComponentIdList& list)
{
  const unsigned cnt{ list.UnsignedCount() };
  m_bSorted = false;
  for (unsigned i = 0; i < cnt; ++i)
  {
    m_id_list.Append(list[i]);
  }
}

unsigned int MYON_SubDComponentIdList::UnsignedCount() const
{
  return m_id_list.UnsignedCount();
}

MYON_SubDComponentId MYON_SubDComponentIdList::operator[](unsigned int i) const
{
  return m_id_list[i];
}

void MYON_SubDComponentIdList::SetInListPassesResult(bool bInListPassesResult)
{
  m_bInListPassesResult = bInListPassesResult ? true : false;
}

bool MYON_SubDComponentIdList::InListPassesResult() const
{
  return m_bInListPassesResult;
}

bool MYON_SubDComponentIdList::InList(MYON_SubDComponentId cid) const
{
  if (false == m_bSorted)
  {
    m_bSorted = true;
    m_id_list.QuickSortAndRemoveDuplicates(MYON_SubDComponentId::CompareTypeAndIdFromPointer);
  }
  const bool bInList = m_id_list.BinarySearch(&cid, MYON_SubDComponentId::CompareTypeAndIdFromPointer) >= 0;

  return bInList;
}

bool MYON_SubDComponentIdList::InList(MYON_SubDComponentPtr cptr) const
{
  return InList(MYON_SubDComponentId(cptr));
}


bool MYON_SubDComponentPtr::Mark() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool MYON_SubDComponentPtr::ClearMark() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool MYON_SubDComponentPtr::SetMark() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool MYON_SubDComponentPtr::SetMark(
  bool bMark
) const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}

MYON__UINT8 MYON_SubDComponentPtr::MarkBits() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.MarkBits() : 0U;
}

MYON__UINT8 MYON_SubDComponentPtr::SetMarkBits(MYON__UINT8 mark_bits) const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.SetMarkBits(mark_bits) : false;
}

MYON__UINT8 MYON_SubDComponentPtr::ClearMarkBits() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_status.SetMarkBits(0) : false;
}

unsigned int MYON_SubDComponentPtr::GroupId(unsigned int null_component_value) const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->m_group_id : null_component_value;
}

bool MYON_SubDComponentPtr::SetGroupId(
  unsigned int group_id
)
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  if (nullptr != c)
  {
    c->m_group_id = group_id;
    return true;
  }
  return false;
}


bool MYON_SubDVertexPtr::Mark() const
{
  const MYON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool MYON_SubDVertexPtr::ClearMark() const
{
  const MYON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool MYON_SubDVertexPtr::SetMark() const
{
  const MYON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool MYON_SubDVertexPtr::SetMark(
  bool bMark
) const
{
  const MYON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}

MYON__UINT8 MYON_SubDVertexPtr::MarkBits() const
{
  const MYON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.MarkBits() : 0U;
}

MYON__UINT8 MYON_SubDVertexPtr::SetMarkBits(MYON__UINT8 mark_bits) const
{
  const MYON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.SetMarkBits(mark_bits) : false;
}

MYON__UINT8 MYON_SubDVertexPtr::ClearMarkBits() const
{
  const MYON_SubDVertex* c = this->Vertex();
  return (nullptr != c) ? c->m_status.SetMarkBits(0) : false;
}

bool MYON_SubDEdgePtr::Mark() const
{
  const MYON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool MYON_SubDEdgePtr::ClearMark() const
{
  const MYON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool MYON_SubDEdgePtr::SetMark() const
{
  const MYON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool MYON_SubDEdgePtr::SetMark(
  bool bMark
) const
{
  const MYON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}

MYON__UINT8 MYON_SubDEdgePtr::MarkBits() const
{
  const MYON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.MarkBits() : 0U;
}

MYON__UINT8 MYON_SubDEdgePtr::SetMarkBits(MYON__UINT8 mark_bits) const
{
  const MYON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.SetMarkBits(mark_bits) : false;
}

MYON__UINT8 MYON_SubDEdgePtr::ClearMarkBits() const
{
  const MYON_SubDEdge* c = this->Edge();
  return (nullptr != c) ? c->m_status.SetMarkBits(0) : false;
}

bool MYON_SubDFacePtr::Mark() const
{
  const MYON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.RuntimeMark() : false;
}

bool MYON_SubDFacePtr::ClearMark() const
{
  const MYON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.ClearRuntimeMark() : false;
}

bool MYON_SubDFacePtr::SetMark() const
{
  const MYON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.SetRuntimeMark() : false;
}

bool MYON_SubDFacePtr::SetMark(
  bool bMark
) const
{
  const MYON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.SetRuntimeMark(bMark) : false;
}

MYON__UINT8 MYON_SubDFacePtr::MarkBits() const
{
  const MYON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.MarkBits() : 0U;
}

MYON__UINT8 MYON_SubDFacePtr::SetMarkBits(MYON__UINT8 mark_bits) const
{
  const MYON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.SetMarkBits(mark_bits) : false;
}

MYON__UINT8 MYON_SubDFacePtr::ClearMarkBits() const
{
  const MYON_SubDFace* c = this->Face();
  return (nullptr != c) ? c->m_status.SetMarkBits(0) : false;
}

MYON__UINT_PTR MYON_SubDComponentPtr::ComponentDirection() const
{
  return MYON_SUBD_COMPONENT_DIRECTION(m_ptr);
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::ClearComponentDirection() const
{
  MYON_SubDComponentPtr component_ptr = *this;
  component_ptr.m_ptr &= (MYON_SUBD_COMPONENT_POINTER_MASK | MYON_SUBD_COMPONENT_TYPE_MASK);
  return component_ptr;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::SetComponentDirection() const
{
  MYON_SubDComponentPtr component_ptr = *this;
  component_ptr.m_ptr |= MYON_SUBD_COMPONENT_DIRECTION_MASK;
  return component_ptr;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::SetComponentDirection(MYON__UINT_PTR dir) const
{
  MYON_SubDComponentPtr component_ptr = *this;
  if (0 == dir)
    component_ptr.m_ptr &= ~((MYON__UINT_PTR)MYON_SUBD_COMPONENT_DIRECTION_MASK);
  else if (1 == dir)
    component_ptr.m_ptr |= MYON_SUBD_COMPONENT_DIRECTION_MASK;
  else
    MYON_SUBD_ERROR("Invalid dir parameter");
  return component_ptr;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Reversed() const
{
  return (0 != (m_ptr & MYON_SUBD_COMPONENT_DIRECTION_MASK)) ? ClearComponentDirection() : SetComponentDirection();
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::CreateNull(
  MYON_SubDComponentPtr::Type component_type,
  MYON__UINT_PTR component_direction
)
{
  MYON_SubDComponentPtr component_ptr;
  switch (component_type)
  {
  case MYON_SubDComponentPtr::Type::Unset:
    component_ptr.m_ptr = 0;
    break;
  case MYON_SubDComponentPtr::Type::Vertex:
    component_ptr.m_ptr = MYON_SUBD_COMPONENT_TYPE_VERTEX;
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    component_ptr.m_ptr = MYON_SUBD_COMPONENT_TYPE_EDGE;
    break;
  case MYON_SubDComponentPtr::Type::Face:
    component_ptr.m_ptr = MYON_SUBD_COMPONENT_TYPE_FACE;
    break;
  default:
    component_ptr.m_ptr = 0;
    break;
  }
  if (1 == component_direction)
    component_ptr.m_ptr |= MYON_SUBD_COMPONENT_DIRECTION_MASK;
  return component_ptr;
}


class MYON_SubDComponentBase* MYON_SubDComponentPtr::ComponentBase() const
{
  switch ((MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
  {
  case MYON_SUBD_COMPONENT_TYPE_VERTEX:
  case MYON_SUBD_COMPONENT_TYPE_EDGE:
  case MYON_SUBD_COMPONENT_TYPE_FACE:
    // During archive id mapping, the returned value can be an archive id and not a true pointer.
    // This is in a controlled setting inside functions like MYON_SubDArchiveIdMap::ConvertArchiveIdToRuntimeSymmetrySetNextPtr().
    // All public level SDK code can safely assume the returned value is a true pointer.
    // It does mean that you cannot "validate" the value returned here
    // using some contraint on what you feel is a reasonable true pointer value.
    return ((class MYON_SubDComponentBase*)MYON_SUBD_COMPONENT_POINTER(m_ptr));
    break;
  }
  return nullptr;
}


class MYON_SubDComponentBase* MYON_SubDComponentPtr::ComponentBase(MYON_SubDComponentPtr::Type type_filter) const
{
  const MYON_SubDComponentPtr::Type ptr_type = static_cast<MYON_SubDComponentPtr::Type>((unsigned char)(MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr));
  switch (ptr_type)
  {
  case MYON_SubDComponentPtr::Type::Vertex:
  case MYON_SubDComponentPtr::Type::Edge:
  case MYON_SubDComponentPtr::Type::Face:
    return (ptr_type == type_filter || MYON_SubDComponentPtr::Type::Unset == type_filter) ? ((class MYON_SubDComponentBase*)MYON_SUBD_COMPONENT_POINTER(m_ptr)) : nullptr;
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  }
  return nullptr;
}

class MYON_SubDVertex* MYON_SubDComponentPtr::Vertex() const
{
  if (MYON_SUBD_COMPONENT_TYPE_VERTEX == (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
    return MYON_SUBD_VERTEX_POINTER(m_ptr);
  return nullptr;
}

class MYON_SubDEdge* MYON_SubDComponentPtr::Edge() const
{
  if (MYON_SUBD_COMPONENT_TYPE_EDGE == (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
    return MYON_SUBD_EDGE_POINTER(m_ptr);
  return nullptr;
}

class MYON_SubDFace* MYON_SubDComponentPtr::Face() const
{
  if (MYON_SUBD_COMPONENT_TYPE_FACE == (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr))
    return MYON_SUBD_FACE_POINTER(m_ptr);
  return nullptr;
}

const MYON_SubDVertexPtr MYON_SubDComponentPtr::VertexPtr() const
{
  MYON__UINT_PTR element_type = MYON_SUBD_COMPONENT_TYPE(m_ptr);
  if ( MYON_SUBD_COMPONENT_TYPE_VERTEX == element_type)
    return MYON_SubDVertexPtr::Create(Vertex(), ComponentDirection());
  return MYON_SubDVertexPtr::Null;
}

const MYON_SubDEdgePtr MYON_SubDComponentPtr::EdgePtr() const
{
  MYON__UINT_PTR element_type = MYON_SUBD_COMPONENT_TYPE(m_ptr);
  if ( MYON_SUBD_COMPONENT_TYPE_EDGE == element_type)
    return MYON_SubDEdgePtr::Create(Edge(), ComponentDirection());
  return MYON_SubDEdgePtr::Null;
}

const MYON_SubDFacePtr MYON_SubDComponentPtr::FacePtr() const
{
  MYON__UINT_PTR element_type = MYON_SUBD_COMPONENT_TYPE(m_ptr);
  if ( MYON_SUBD_COMPONENT_TYPE_FACE == element_type)
    return MYON_SubDFacePtr::Create(Face(), ComponentDirection());
  return MYON_SubDFacePtr::Null;
}

const bool MYON_SubDComponentPtr::IsVertex() const
{
  return (MYON_SUBD_COMPONENT_TYPE_VERTEX == (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr));
}

const bool MYON_SubDComponentPtr::IsEdge() const
{
  return (MYON_SUBD_COMPONENT_TYPE_EDGE == (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr));
}

const bool MYON_SubDComponentPtr::IsFace() const
{
  return (MYON_SUBD_COMPONENT_TYPE_FACE == (MYON_SUBD_COMPONENT_TYPE_MASK & m_ptr));
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  const class MYON_SubDVertex* vertex
  )
{
  if (nullptr != vertex)
  {
    MYON_SubDComponentPtr vptr = { (MYON__UINT_PTR)vertex | MYON_SUBD_COMPONENT_TYPE_VERTEX };
    return vptr;
  }
  return MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  const class MYON_SubDEdge* edge
  )
{
  if (nullptr != edge)
  {
    MYON_SubDComponentPtr eptr = { (MYON__UINT_PTR)edge | MYON_SUBD_COMPONENT_TYPE_EDGE };
    return eptr;
  }
  return MYON_SubDComponentPtr::Null;
}


const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  const class MYON_SubDFace* face
  )
{
  if (nullptr != face)
  {
    MYON_SubDComponentPtr fptr = { (MYON__UINT_PTR)face | MYON_SUBD_COMPONENT_TYPE_FACE };
    return fptr;
  }
  return MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  const class MYON_SubDVertex* vertex,
  MYON__UINT_PTR vertex_direction
  )
{
  if (nullptr != vertex)
  {
    MYON_SubDComponentPtr vptr = { (MYON__UINT_PTR)vertex | MYON_SUBD_COMPONENT_TYPE_VERTEX | (vertex_direction & MYON_SUBD_COMPONENT_DIRECTION_MASK) };
    return vptr;
  }
  return MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  const class MYON_SubDEdge* edge,
  MYON__UINT_PTR edge_direction
  )
{
  if (nullptr != edge)
  {    
    MYON_SubDComponentPtr eptr = { (MYON__UINT_PTR)edge | (MYON_SUBD_COMPONENT_TYPE_EDGE | (edge_direction & MYON_SUBD_COMPONENT_DIRECTION_MASK)) };
    return eptr;
  }
  return MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  const class MYON_SubDFace* face,
  MYON__UINT_PTR face_direction
  )
{
  if (nullptr != face)
  {
    MYON_SubDComponentPtr fptr = { (MYON__UINT_PTR)face | (MYON_SUBD_COMPONENT_TYPE_FACE | (face_direction & MYON_SUBD_COMPONENT_DIRECTION_MASK)) };
    return fptr;
  }
  return MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  MYON_SubDVertexPtr vertexptr
  )
{
  return Create(vertexptr.Vertex(), vertexptr.VertexDirection());
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  MYON_SubDEdgePtr edgeptr
  )
{
  return Create(edgeptr.Edge(), edgeptr.EdgeDirection());
}

const MYON_SubDComponentPtr MYON_SubDComponentPtr::Create(
  MYON_SubDFacePtr faceptr
  )
{
  return Create(faceptr.Face(), faceptr.FaceDirection());
}

int MYON_SubDComponentPtr::CompareComponentPtrType(
  MYON_SubDComponentPtr::Type a,
  MYON_SubDComponentPtr::Type b
  )
{
  if ( a == b )
    return 0;
  switch (a)
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    return -1;
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    return (MYON_SubDComponentPtr::Type::Vertex == b) ? 1 : -1;
    break;
  case MYON_SubDComponentPtr::Type::Face:
    return (MYON_SubDComponentPtr::Type::Vertex == b || MYON_SubDComponentPtr::Type::Edge == b) ? 1 : -1;
    break;
  default:
    break;
  }
  return (((unsigned char)a) < ((unsigned char)b)) ? -1 : 1;
}

int MYON_SubDComponentPtr::CompareType(
  const MYON_SubDComponentPtr* a,
  const MYON_SubDComponentPtr* b
  )
{
  if ( a == b )
    return 0;
  if ( nullptr == a )
    return 1;
  if ( nullptr == b )
    return -1;
  return MYON_SubDComponentPtr::CompareComponentPtrType(a->ComponentType(), b->ComponentType());
}


int MYON_SubDComponentPtr::CompareComponent(
  const MYON_SubDComponentPtr* a,
  const MYON_SubDComponentPtr* b
)
{
  if (a == b)
    return 0;
  const int rc = MYON_SubDComponentPtr::CompareComponentPtrType(a->ComponentType(), b->ComponentType());
  if (0 == rc)
  {
    const MYON__UINT_PTR x = a->m_ptr;
    const MYON__UINT_PTR y = b->m_ptr;
    if (x < y)
      return -1;
    if (x > y)
      return 1;
  }
  return rc;
}

int MYON_SubDComponentPtr::CompareComponentAndDirection(
  const MYON_SubDComponentPtr* a,
  const MYON_SubDComponentPtr* b
)
{
  if (a == b)
    return 0;
  const int rc = MYON_SubDComponentPtr::CompareComponent(a, b);
  if (0 == rc)
  {
    const MYON__UINT_PTR x = (a->m_ptr & MYON_SUBD_COMPONENT_POINTER_MASK);
    const MYON__UINT_PTR y = (b->m_ptr & MYON_SUBD_COMPONENT_POINTER_MASK);
    if (x < y)
      return -1;
    if (x > y)
      return 1;
  }
  return rc;
}

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::Create(
  MYON_SubDComponentPtr cptr,
  double x
)
{
  MYON_SubDComponentAndNumber cx;
  cx.m_cptr = cptr;
  cx.m_x = x;
  return cx;
}

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::Create(
  const MYON_SubDVertex* v,
  double x
)
{
  return MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Create(v), x);
}

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::Create(
  const MYON_SubDEdge* e,
  double x
)
{
  return MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Create(e), x);
}

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::Create(
  const MYON_SubDFace* f,
  double x
)
{
  return MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Create(f), x);
}

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::Create(
  const MYON_SubDVertexPtr vptr,
  double x
)
{
  return MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Create(vptr), x);
}

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::Create(
  const MYON_SubDEdgePtr eptr,
  double x
)
{
  return MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Create(eptr), x);
}

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::Create(
  const MYON_SubDFacePtr fptr,
  double x
)
{
  return MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Create(fptr), x);
}


const MYON_SubDComponentPtr MYON_SubDComponentAndNumber::Component() const
{
  return m_cptr;
}

void MYON_SubDComponentAndNumber::SetComponent(MYON_SubDComponentPtr cptr)
{
  m_cptr = cptr;
}

double MYON_SubDComponentAndNumber::Number() const
{
  return m_x;
}

void MYON_SubDComponentAndNumber::SetNumber(double x)
{
  m_x = x;
}

int MYON_SubDComponentAndNumber::CompareComponent(
  const MYON_SubDComponentAndNumber* a,
  const MYON_SubDComponentAndNumber* b
)
{
  if (a == b)
    return 0;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;
  return MYON_SubDComponentPtr::CompareComponent(&a->m_cptr, &b->m_cptr);
}

// 
int MYON_SubDComponentAndNumber::CompareComponentAndDirection(
  const MYON_SubDComponentAndNumber* a,
  const MYON_SubDComponentAndNumber* b
)
{
  if (a == b)
    return 0;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;
  return MYON_SubDComponentPtr::CompareComponentAndDirection(&a->m_cptr, &b->m_cptr);
}

int MYON_SubDComponentAndNumber::CompareNumber(
  const MYON_SubDComponentAndNumber* a,
  const MYON_SubDComponentAndNumber* b
)
{
  if (a == b)
    return 0;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;
  return MYON_CompareDouble(a->m_x, b->m_x);
}

int MYON_SubDComponentAndNumber::CompareComponentAndNumber(
  const MYON_SubDComponentAndNumber* a,
  const MYON_SubDComponentAndNumber* b
)
{
  const int rc = MYON_SubDComponentAndNumber::CompareComponent(a, b);
  return (0 != rc) ? rc : MYON_SubDComponentAndNumber::CompareNumber(a, b);
}

int MYON_SubDComponentAndNumber::CompareComponentAndDirectionAndNumber(
  const MYON_SubDComponentAndNumber* a,
  const MYON_SubDComponentAndNumber* b
)
{
  const int rc = MYON_SubDComponentAndNumber::CompareComponentAndDirection(a, b);
  return (0 != rc) ? rc : MYON_SubDComponentAndNumber::CompareNumber(a, b);
}



const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::Create(
  MYON_SubDComponentPtr cptr,
  MYON_3dPoint P
)
{
  MYON_SubDComponentAndPoint cx;
  cx.m_cptr = cptr;
  cx.m_P = P;
  return cx;
}

const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::Create(
  const MYON_SubDVertex* v,
  MYON_3dPoint P
)
{
  return MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Create(v), P);
}

const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::Create(
  const MYON_SubDEdge* e,
  MYON_3dPoint P
)
{
  return MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Create(e), P);
}

const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::Create(
  const MYON_SubDFace* f,
  MYON_3dPoint P
)
{
  return MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Create(f), P);
}

const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::Create(
  const MYON_SubDVertexPtr vptr,
  MYON_3dPoint P
)
{
  return MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Create(vptr), P);
}

const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::Create(
  const MYON_SubDEdgePtr eptr,
  MYON_3dPoint P
)
{
  return MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Create(eptr), P);
}

const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::Create(
  const MYON_SubDFacePtr fptr,
  MYON_3dPoint P
)
{
  return MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Create(fptr), P);
}


const MYON_SubDComponentPtr MYON_SubDComponentAndPoint::Component() const
{
  return m_cptr;
}

void MYON_SubDComponentAndPoint::SetComponent(MYON_SubDComponentPtr cptr)
{
  m_cptr = cptr;
}

const MYON_3dPoint MYON_SubDComponentAndPoint::Point() const
{
  return m_P;
}

void MYON_SubDComponentAndPoint::SetPoint(MYON_3dPoint P)
{
  m_P = P;
}

int MYON_SubDComponentAndPoint::CompareComponent(
  const MYON_SubDComponentAndPoint* lhs,
  const MYON_SubDComponentAndPoint* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return -1;
  if (nullptr == rhs)
    return 1;
  return MYON_SubDComponentPtr::CompareComponent(&lhs->m_cptr, &rhs->m_cptr);
}

// 
int MYON_SubDComponentAndPoint::CompareComponentAndDirection(
  const MYON_SubDComponentAndPoint* lhs,
  const MYON_SubDComponentAndPoint* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return -1;
  if (nullptr == rhs)
    return 1;
  return MYON_SubDComponentPtr::CompareComponentAndDirection(&lhs->m_cptr, &rhs->m_cptr);
}

int MYON_SubDComponentAndPoint::ComparePoint(
  const MYON_SubDComponentAndPoint* lhs,
  const MYON_SubDComponentAndPoint* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return -1;
  if (nullptr == rhs)
    return 1;
  return MYON_3dPoint::Compare(lhs->m_P, rhs->m_P);
}

int MYON_SubDComponentAndPoint::CompareComponentAndPoint(
  const MYON_SubDComponentAndPoint* lhs,
  const MYON_SubDComponentAndPoint* rhs
)
{
  const int rc = MYON_SubDComponentAndPoint::CompareComponent(lhs, rhs);
  return (0 != rc) ? rc : MYON_SubDComponentAndPoint::ComparePoint(lhs, rhs);
}

int MYON_SubDComponentAndPoint::CompareComponentAndDirectionAndPoint(
  const MYON_SubDComponentAndPoint* lhs,
  const MYON_SubDComponentAndPoint* rhs
)
{
  const int rc = MYON_SubDComponentAndPoint::CompareComponentAndDirection(lhs, rhs);
  return (0 != rc) ? rc : MYON_SubDComponentAndPoint::ComparePoint(lhs, rhs);
}


static const bool Internal_FirstIsPartOfSecond(
  const MYON_SubDComponentPoint& first,
  const MYON_SubDComponentPoint& second
)
{
  // returns true if first is topologically part of second (vertex on edge, vertex on face, or edge on face).
  const MYON_SubDComponentPtr::Type first_type = first.m_component_ptr.ComponentType();
  const MYON_SubDComponentPtr::Type second_type = second.m_component_ptr.ComponentType();
  if (first_type != second_type)
  {
    if (MYON_SubDComponentPtr::Type::Vertex == first_type)
    {
      const MYON_SubDVertex* v = first.m_component_ptr.Vertex();
      if (nullptr != v)
      {
        if (MYON_SubDComponentPtr::Type::Edge == second_type)
        {
          const MYON_SubDEdge* e = second.m_component_ptr.Edge();
          if (nullptr != e && (v == e->m_vertex[0] || v == e->m_vertex[1]))
            return true;
        }
        else if (MYON_SubDComponentPtr::Type::Face == second_type)
        {
          const MYON_SubDFace* f = second.m_component_ptr.Face();
          if (nullptr != f && f->VertexIndex(v) < MYON_UNSET_UINT_INDEX)
            return true;
        }
      }
    }
    else if (MYON_SubDComponentPtr::Type::Edge == first_type)
    {
      if (MYON_SubDComponentPtr::Type::Face == second_type)
      {
        const MYON_SubDEdge* e = first.m_component_ptr.Edge();
        const MYON_SubDFace* f = second.m_component_ptr.Face();
        if (nullptr != e && nullptr != f && e->FaceArrayIndex(f) < MYON_UNSET_UINT_INDEX)
          return true;
      }
    }
  }
  return false;
}

const MYON_SubDComponentPoint MYON_SubDComponentPoint::BestPickPoint(
  MYON_PickType pick_type,
  double vertex_depth_bias,
  double edge_depth_bias,
  const MYON_SubDComponentPoint& A,
  const MYON_SubDComponentPoint& B
)
{
  // If you're working on a bug where somebody isn't able to pick the SubD component
  // they thinked they clicked on, then you're in the right place.
  // Mikko has done lots of work in this area and be sure to retest RH-52172 and RH-52173 (fixed 3 May 2019).
  // Also, retest RH-59666 (fixed August 2020).
  //
  // This function will be a work in progress for many years.

  // unbiased_compare ignores SubD component types and relative topological relationships.
  // If unbiased_compare == 1, then A is "best" from this "unbiased" point of views.
  const int unbiased_compare = (MYON_PickPoint::Compare(A.m_pick_point, B.m_pick_point)>=0) ? 1 : -1;
  
  const MYON_SubDComponentPtr::Type A_component_type = A.m_component_ptr.ComponentType();
  const MYON_SubDComponentPtr::Type B_component_type = B.m_component_ptr.ComponentType();
  if (A_component_type == B_component_type)
  {
    // A and B are the same type of component
    return (unbiased_compare >= 0) ? A : B;
  }
  if (MYON_SubDComponentPtr::Type::Unset == A_component_type)
    return B;
  if (MYON_SubDComponentPtr::Type::Unset == B_component_type)
    return A;

  // If we get this far, then A and B are pick events on different types of SubD components.

  // type_bias = 1 if A is a vertex and B is an edge/face or A is an edge and B is a face.
  // type_bias = -1 if B is a vertex and A is an edge/face or B is an edge and A is a face.
  const int type_bias = (MYON_SubDComponentPtr::CompareComponentPtrType(A_component_type, B_component_type)) <= 0 ? 1 : -1;

  if (
    MYON_PickType::PointPick == pick_type
    && ((type_bias >= 0) ? Internal_FirstIsPartOfSecond(A, B) : Internal_FirstIsPartOfSecond(B, A))
    )
  {
    // A point pick is occuring and best is a vertex on an edge/face or best is an edge on a face.
    // Bias towards the vertex/edge.
    // Users can pick the middle of an edge/face if they want the "bigger" component.
    MYON_SubDComponentPoint best = (type_bias >= 0) ? A : B;
    best.m_pick_point.m_distance = MYON_Min(A.m_pick_point.m_distance, B.m_pick_point.m_distance);
    best.m_pick_point.m_depth = MYON_Max(A.m_pick_point.m_depth, B.m_pick_point.m_depth);
    return best;
  }

  MYON_PickPoint biased_A(A.m_pick_point);
  MYON_PickPoint biased_B(B.m_pick_point);

  // distance bias applied to point picks on vertices and edges
  // Events with smaller distances are "better" because they are visually are closer to the picked point.
  if (MYON_PickType::PointPick == pick_type)
  {
    // 18 Aug 2020 Dale Lear:
    //   The pick distance for a point pick is normalized so 1.0 = pick frustum radius. 
    //   Reasonable values for distance_tol are 0.25 to 1.0.
    //   0.5 fixes RH-52172
    //   1.0 seems to fix bugs like RH-59666 (0.5 is too small to fix RH-59666).
    const double distance_tol = 1.0;

    if ( type_bias > 0 && biased_A.m_distance > 0.0 && biased_A.m_distance <= distance_tol)
      biased_A.m_distance = 0.0; // consider pick A to be on the vertex/edge

    if ( type_bias < 0 && biased_B.m_distance > 0.0 && biased_B.m_distance <= distance_tol )
      biased_B.m_distance = 0.0; // consider pick B to be on the vertex/edge
  }

  // Apply a depth bias when we have a relative topological relationship between the components
  // (vertex on edge, vertex on face, or edge on face).
  // Events with larger depths are "better" because they are in front of smaller depths.
  double depth_bias = 0.0;
  const bool bHaveVertex = MYON_SubDComponentPtr::Type::Vertex == A_component_type || MYON_SubDComponentPtr::Type::Vertex == B_component_type;
  const bool bHaveEdge = MYON_SubDComponentPtr::Type::Edge == A_component_type || MYON_SubDComponentPtr::Type::Edge == B_component_type;
  const bool bHaveFace = MYON_SubDComponentPtr::Type::Face == A_component_type || MYON_SubDComponentPtr::Type::Face == B_component_type;
  if ( bHaveVertex && vertex_depth_bias > 0.0 && vertex_depth_bias < MYON_UNSET_POSITIVE_VALUE  )
  {
    // One event is a vertex, the other event is an edge or face, and a vertex bias was supplied.
    // Test to see if the vertex is on the edge or face.
    if (Internal_FirstIsPartOfSecond(A,B))
    {
      // B is an edge/face and A is a vertex on the edge/face
      depth_bias = vertex_depth_bias;
      biased_A.m_depth += depth_bias; // bias vertex closer
    }
    else if (Internal_FirstIsPartOfSecond(B, A))
    {
      // A is an edge/face and B is a vertex on the edge/face
      depth_bias = vertex_depth_bias;
      biased_B.m_depth += depth_bias; // bias vertex closer
    }
  }
  else if ( bHaveEdge && bHaveFace && edge_depth_bias > 0.0  && edge_depth_bias < MYON_UNSET_POSITIVE_VALUE )
  {
    // One event is an edge, the other event is a face, and an edge bias was supplied.
    // Test to see if the edge is on the face.
    if (Internal_FirstIsPartOfSecond(A, B))
    {
      // B is a face and A is an edge on the face
      depth_bias = edge_depth_bias;
      biased_A.m_depth += depth_bias; // bias edge closer
    }
    else if (Internal_FirstIsPartOfSecond(B, A))
    {
      // A is a face and B is an edge on the face
      depth_bias = edge_depth_bias;
      biased_B.m_depth += depth_bias; // bias edge closer
    }
  }

  // biased_compare adds a bias towards vertices and edges based on SubD component types and relative topological relationships.
  // If biased_compare >= 0, then A is "best" from this biased point of views.
  const int biased_compare = (MYON_PickPoint::Compare(biased_A, biased_B)>=0) ? 1 : -1;

  MYON_SubDComponentPoint best = (biased_compare >= 0) ? A : B;

  const MYON_SubDComponentPtr::Type best_type = best.m_component_ptr.ComponentType();
  if ( MYON_SubDComponentPtr::Type::Vertex == best_type || (MYON_SubDComponentPtr::Type::Edge == best_type && bHaveFace) )
  {
    // Either a vertex was better than an edge/face or an edge was better than a face.
    if (fabs(A.m_pick_point.m_depth - B.m_pick_point.m_depth) <= depth_bias)
    {
      // The two pick points were "close" in depth.
      // In these cases, strengthen "best" by using the "best" depth and distance values from A and B.
      best.m_pick_point.m_distance = MYON_Min(A.m_pick_point.m_distance, B.m_pick_point.m_distance);
      best.m_pick_point.m_depth = MYON_Max(A.m_pick_point.m_depth, B.m_pick_point.m_depth);
    }
  }

  return best;
}

int MYON_SubDComponentPoint::CompareComponentAndDirection(
  const MYON_SubDComponentPoint* a,
  const MYON_SubDComponentPoint* b
  )
{
  if ( a == b)
    return 0;  
  if ( nullptr == a)
    return 1; // nullptr > non-null pointer.
  if ( nullptr == b)
    return -1; // nullptr > non-null pointer.

  // 1st: compare component type
  // unset < vertex < edge < face
  MYON__UINT_PTR x = (MYON_SUBD_COMPONENT_TYPE_MASK & a->m_component_ptr.m_ptr);
  MYON__UINT_PTR y = (MYON_SUBD_COMPONENT_TYPE_MASK & b->m_component_ptr.m_ptr);
  if ( x < y )
    return -1;
  if ( x > y )
    return 1;

  // 2nd: compare component pointer
  x = (a->m_component_ptr.m_ptr & MYON_SUBD_COMPONENT_POINTER_MASK);
  y = (b->m_component_ptr.m_ptr & MYON_SUBD_COMPONENT_POINTER_MASK);
  if (x < y)
    return -1;
  if (x > y)
    return 1;

  // 3rd: compare component direction flag
  x = (a->m_component_ptr.m_ptr & MYON_SUBD_COMPONENT_DIRECTION_MASK);
  y = (b->m_component_ptr.m_ptr & MYON_SUBD_COMPONENT_DIRECTION_MASK);
  if (x < y)
    return -1;
  if (x > y)
    return 1;

  return 0;
}


const MYON_SubDComponentPtrPair MYON_SubDComponentPtrPair::Create(MYON_SubDComponentPtr first_ptr, MYON_SubDComponentPtr second_ptr)
{
  MYON_SubDComponentPtrPair p;
  p.m_pair[0] = first_ptr;
  p.m_pair[1] = second_ptr;
  return p;
}

int MYON_SubDComponentPtrPair::CompareComponent(
  const MYON_SubDComponentPtrPair* lhs,
  const MYON_SubDComponentPtrPair* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls sort to end.
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  int rc = MYON_SubDComponentPtr::CompareComponent(&lhs->m_pair[0], &rhs->m_pair[0]);
  if (0 == rc)
    rc = MYON_SubDComponentPtr::CompareComponent(&lhs->m_pair[1], &rhs->m_pair[1]);
  return rc;
}

int MYON_SubDComponentPtrPair::CompareComponentAndDirection(
  const MYON_SubDComponentPtrPair* lhs,
  const MYON_SubDComponentPtrPair* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls sort to end.
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  int rc = MYON_SubDComponentPtr::CompareComponentAndDirection(&lhs->m_pair[0], &rhs->m_pair[0]);
  if (0 == rc)
    rc = MYON_SubDComponentPtr::CompareComponentAndDirection(&lhs->m_pair[1], &rhs->m_pair[1]);
  return rc;
}


int MYON_SubDComponentPtrPair::CompareFirstPointer(
  const MYON_SubDComponentPtrPair* lhs,
  const MYON_SubDComponentPtrPair* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls sort to end.
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  const MYON__UINT_PTR lhs_ptr = (MYON_SUBD_COMPONENT_POINTER_MASK & lhs->m_pair[0].m_ptr);
  const MYON__UINT_PTR rhs_ptr = (MYON_SUBD_COMPONENT_POINTER_MASK & rhs->m_pair[0].m_ptr);
  if (lhs_ptr < rhs_ptr)
    return -1;
  if (lhs_ptr > rhs_ptr)
    return 1;
  return 0;
}

MYON_SubDComponentPtr::Type MYON_SubDComponentPtrPair::ComponentType() const
{
  const MYON_SubDComponentPtr::Type type = m_pair[0].ComponentType();
  return (type == m_pair[1].ComponentType()) ? type : MYON_SubDComponentPtr::Type::Unset;
}

const MYON_SubDComponentPtrPair MYON_SubDComponentPtrPair::SwapPair() const
{
  return MYON_SubDComponentPtrPair::Create(m_pair[1], m_pair[0]);
}

const MYON_SubDComponentPtrPair MYON_SubDComponentPtrPair::ReversedPair() const
{
  return MYON_SubDComponentPtrPair::Create(m_pair[0].Reversed(), m_pair[1].Reversed());
}

const MYON_SubDComponentPtr MYON_SubDComponentPtrPair::First() const
{
  return m_pair[0];
}

const MYON_SubDComponentPtr MYON_SubDComponentPtrPair::Second() const
{
  return m_pair[1];
}

bool MYON_SubDComponentPtrPair::FirstIsNull() const
{
  return (0 == (MYON_SUBD_COMPONENT_POINTER_MASK & m_pair[0].m_ptr));
}

bool MYON_SubDComponentPtrPair::SecondIsNull() const
{
  return (0 == (MYON_SUBD_COMPONENT_POINTER_MASK & m_pair[1].m_ptr));
}

bool MYON_SubDComponentPtrPair::BothAreNull() const
{
  return (0 == (MYON_SUBD_COMPONENT_POINTER_MASK & m_pair[0].m_ptr)) && 0 == (MYON_SUBD_COMPONENT_POINTER_MASK & m_pair[1].m_ptr);
}

bool MYON_SubDComponentPtrPair::FirstIsNotNull() const
{
  return m_pair[0].IsNotNull();
}

bool MYON_SubDComponentPtrPair::SecondIsNotNull() const
{
  return m_pair[1].IsNotNull();
}

bool MYON_SubDComponentPtrPair::BothAreNotNull() const
{
  return m_pair[0].IsNotNull() && m_pair[1].IsNotNull();
}


/////////////////////////////////////////////////////////////////////////
//
// MYON_SubD_ComponentIdTypeAndTag
//

const MYON_wString MYON_SubD_ComponentIdTypeAndTag::ToString() const
{
  switch (m_type)
  {
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  case MYON_SubDComponentPtr::Type::Vertex:
    return MYON_wString::FormatToString(L"Vertex id=%u tag=",m_id) + MYON_SubD::VertexTagToString(VertexTag(),false);
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    return MYON_wString::FormatToString(L"Edge id=%u tag=",m_id) + MYON_SubD::EdgeTagToString(EdgeTag(), false);
    break;
  case MYON_SubDComponentPtr::Type::Face:
    return MYON_wString::FormatToString(L"Face id=%u tag=%u", m_id, (unsigned)FaceTag());
    break;
  default:
    break;
  }
  return MYON_wString(L"Unset");
}

const MYON_SubD_ComponentIdTypeAndTag MYON_SubD_ComponentIdTypeAndTag::CreateFromVertex(const MYON_SubDVertex* v)
{
  MYON_SubD_ComponentIdTypeAndTag itt
    = (nullptr != v)
    ? MYON_SubD_ComponentIdTypeAndTag::CreateFromVertexId(v->m_id, v->m_vertex_tag)
    : MYON_SubD_ComponentIdTypeAndTag::Unset;
  if (itt.m_id > 0)
    itt.m_cptr = MYON_SubDComponentPtr::Create(v);
  return itt;
}

const MYON_SubD_ComponentIdTypeAndTag MYON_SubD_ComponentIdTypeAndTag::CreateFromVertexId(unsigned vertex_id, MYON_SubDVertexTag vtag)
{
  MYON_SubD_ComponentIdTypeAndTag itt;
  if (vertex_id > 0)
  {
    itt.m_id = vertex_id;
    itt.m_type = MYON_SubDComponentPtr::Type::Vertex;
    itt.m_tag = static_cast<unsigned char>(vtag);
  }
  return itt;
}

const MYON_SubD_ComponentIdTypeAndTag MYON_SubD_ComponentIdTypeAndTag::CreateFromEdge(const MYON_SubDEdge * e)
{
  MYON_SubD_ComponentIdTypeAndTag itt
    = (nullptr != e) 
    ? MYON_SubD_ComponentIdTypeAndTag::CreateFromEdgeId(e->m_id, e->m_edge_tag) 
    : MYON_SubD_ComponentIdTypeAndTag::Unset;
  if (itt.m_id > 0)
    itt.m_cptr = MYON_SubDComponentPtr::Create(e);
  return itt;
}


const MYON_SubD_ComponentIdTypeAndTag MYON_SubD_ComponentIdTypeAndTag::CreateFromEdgeId(unsigned edge_id, MYON_SubDEdgeTag etag)
{
  MYON_SubD_ComponentIdTypeAndTag itt;
  if (edge_id > 0)
  {
    itt.m_id = edge_id;
    itt.m_type = MYON_SubDComponentPtr::Type::Edge;
    itt.m_tag = static_cast<unsigned char>(MYON_SubDEdgeTag::SmoothX == etag ? MYON_SubDEdgeTag::Smooth : etag);
  }
  return itt;
}

const MYON_SubD_ComponentIdTypeAndTag MYON_SubD_ComponentIdTypeAndTag::CreateFromFace(const MYON_SubDFace* f, unsigned char ftag)
{
  MYON_SubD_ComponentIdTypeAndTag itt
    = (nullptr != f)
    ? MYON_SubD_ComponentIdTypeAndTag::CreateFromFaceId(f->m_id, ftag)
    : MYON_SubD_ComponentIdTypeAndTag::Unset;
  if (itt.m_id > 0)
    itt.m_cptr = MYON_SubDComponentPtr::Create(f);
  return itt;
}

const MYON_SubD_ComponentIdTypeAndTag MYON_SubD_ComponentIdTypeAndTag::CreateFromFaceId(unsigned face_id, unsigned char ftag)
{
  MYON_SubD_ComponentIdTypeAndTag itt;
  if (face_id > 0)
  {
    itt.m_id = face_id;
    itt.m_type = MYON_SubDComponentPtr::Type::Vertex;
    itt.m_tag = ftag;
  }
  return itt;
}

int MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId(const MYON_SubD_ComponentIdTypeAndTag * lhs, const MYON_SubD_ComponentIdTypeAndTag * rhs)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->m_type < rhs->m_type)
    return -1;
  if (lhs->m_type > rhs->m_type)
    return 1;
  if (lhs->m_id < rhs->m_id)
    return -1;
  if (lhs->m_id > rhs->m_id)
    return 1;
  return 0;
}

int MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndIdAndTag(const MYON_SubD_ComponentIdTypeAndTag* lhs, const MYON_SubD_ComponentIdTypeAndTag* rhs)
{
  int rc = MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId(lhs, rhs);
  if (0 != rc)
    return rc;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->m_tag < rhs->m_tag)
    return -1;
  if (lhs->m_tag > rhs->m_tag)
    return 1;
  return 0;
}

MYON_SubDVertexTag MYON_SubD_ComponentIdTypeAndTag::OriginalVertexTag(unsigned vertex_id, const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags)
{
  if (0 == vertex_id)
    return MYON_SubDVertexTag::Unset;
  const MYON_SubD_ComponentIdTypeAndTag itt = MYON_SubD_ComponentIdTypeAndTag::CreateFromVertexId(vertex_id,MYON_SubDVertexTag::Unset);
  const int i = sorted_tags.BinarySearch(&itt, MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId);
  return (i >= 0) ? sorted_tags[i].VertexTag() : MYON_SubDVertexTag::Unset;
}

MYON_SubDVertexTag MYON_SubD_ComponentIdTypeAndTag::OriginalVertexTag(const MYON_SubDVertex* v, const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags)
{
  if (nullptr == v)
    return MYON_SubDVertexTag::Unset;
  const MYON_SubD_ComponentIdTypeAndTag itt = MYON_SubD_ComponentIdTypeAndTag::CreateFromVertexId(v->m_id,MYON_SubDVertexTag::Unset);
  const int i = sorted_tags.BinarySearch(&itt, MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId);
  return (i >= 0) ? sorted_tags[i].VertexTag() : v->m_vertex_tag;
}

MYON_SubDEdgeTag MYON_SubD_ComponentIdTypeAndTag::OriginalEdgeTag(unsigned edge_id, const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags)
{
  if (0 == edge_id)
    return MYON_SubDEdgeTag::Unset;
  const MYON_SubD_ComponentIdTypeAndTag itt = MYON_SubD_ComponentIdTypeAndTag::CreateFromEdgeId(edge_id, MYON_SubDEdgeTag::Unset);
  const int i = sorted_tags.BinarySearch(&itt, MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId);
  return (i >= 0) ? sorted_tags[i].EdgeTag() : MYON_SubDEdgeTag::Unset;
}


MYON_SubDEdgeTag MYON_SubD_ComponentIdTypeAndTag::OriginalEdgeTag(const MYON_SubDEdge * e, const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>&sorted_tags)
{
  if (nullptr == e)
    return MYON_SubDEdgeTag::Unset;
  const MYON_SubD_ComponentIdTypeAndTag itt = MYON_SubD_ComponentIdTypeAndTag::CreateFromEdgeId(e->m_id,MYON_SubDEdgeTag::Unset);
  const int i = sorted_tags.BinarySearch(&itt, MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId);
  return (i >= 0) ? sorted_tags[i].EdgeTag() : e->m_edge_tag;
}

unsigned char MYON_SubD_ComponentIdTypeAndTag::OriginalFaceTag(unsigned face_id, const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags)
{
  if (0 == face_id)
    return 0;
  const MYON_SubD_ComponentIdTypeAndTag itt = MYON_SubD_ComponentIdTypeAndTag::CreateFromFaceId(face_id, 0);
  const int i = sorted_tags.BinarySearch(&itt, MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId);
  return (i >= 0) ? sorted_tags[i].FaceTag() : 0;
}


unsigned char MYON_SubD_ComponentIdTypeAndTag::OriginalFaceTag(const MYON_SubDFace* f, const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags)
{
  if (nullptr == f)
    return 0;
  const MYON_SubD_ComponentIdTypeAndTag itt = MYON_SubD_ComponentIdTypeAndTag::CreateFromFaceId(f->m_id,0);
  const int i = sorted_tags.BinarySearch(&itt, MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId);
  const unsigned char ftag = (i >= 0) ? sorted_tags[i].FaceTag() : 0;
  return ftag;
}

MYON_SubDComponentPtr::Type MYON_SubD_ComponentIdTypeAndTag::ComponentType() const
{
  return m_type;
}

unsigned MYON_SubD_ComponentIdTypeAndTag::VertexId() const
{
  return (MYON_SubDComponentPtr::Type::Vertex == m_type) ? m_id : 0;
}

unsigned MYON_SubD_ComponentIdTypeAndTag::EdgeId() const
{
  return (MYON_SubDComponentPtr::Type::Edge == m_type) ? m_id : 0;
}

unsigned MYON_SubD_ComponentIdTypeAndTag::FaceId() const
{
  return (MYON_SubDComponentPtr::Type::Face == m_type) ? m_id : 0;
}

MYON_SubDVertexTag MYON_SubD_ComponentIdTypeAndTag::VertexTag() const
{
  return (MYON_SubDComponentPtr::Type::Vertex == m_type) ? MYON_SubD::VertexTagFromUnsigned(m_tag) : MYON_SubDVertexTag::Unset;
}

MYON_SubDEdgeTag MYON_SubD_ComponentIdTypeAndTag::EdgeTag() const
{
  return (MYON_SubDComponentPtr::Type::Edge == m_type) ? MYON_SubD::EdgeTagFromUnsigned(m_tag) : MYON_SubDEdgeTag::Unset;
}

unsigned char MYON_SubD_ComponentIdTypeAndTag::FaceTag() const
{
  return (MYON_SubDComponentPtr::Type::Face == m_type) ? m_tag : 0;
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFromMeshParameters
//

// Convex corner options

MYON_SubDFromMeshParameters::ConvexCornerOption MYON_SubDFromMeshParameters::ConvexCornerOptionFromUnsigned(
  unsigned int convex_corner_option_as_unsigned
)
{
  switch (convex_corner_option_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::ConvexCornerOption::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::ConvexCornerOption::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::ConvexCornerOption::AtMeshCorner);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDFromMeshParameters::ConvexCornerOption::Unset);
}

void MYON_SubDFromMeshParameters::SetConvexCornerOption(
  MYON_SubDFromMeshParameters::ConvexCornerOption convex_corner_option
  )
{
  m_convex_corner_option = MYON_SubDFromMeshParameters::ConvexCornerOptionFromUnsigned((unsigned int)convex_corner_option);
}

MYON_SubDFromMeshParameters::ConvexCornerOption MYON_SubDFromMeshParameters::GetConvexCornerOption() const
{
  switch (m_convex_corner_option)
  {
  case MYON_SubDFromMeshParameters::ConvexCornerOption::Unset:
  case MYON_SubDFromMeshParameters::ConvexCornerOption::None:
    return m_convex_corner_option;

  case MYON_SubDFromMeshParameters::ConvexCornerOption::AtMeshCorner:
    if ( m_maximum_convex_corner_edge_count >= 2 
         && m_maximum_convex_corner_edge_count <= MYON_SubDVertex::MaximumEdgeCount
         && m_maximum_convex_corner_angle_radians >= 0.0
         && m_maximum_convex_corner_angle_radians < MYON_PI
         )
         return m_convex_corner_option;
    break;
  }

  return MYON_SubDFromMeshParameters::ConvexCornerOption::Unset;
}

void MYON_SubDFromMeshParameters::SetMaximumConvexCornerEdgeCount(
  unsigned int maximum_convex_corner_edge_count
  )
{
  if ( maximum_convex_corner_edge_count >= 2 && maximum_convex_corner_edge_count <= MYON_SubDVertex::MaximumEdgeCount)
    m_maximum_convex_corner_edge_count = (unsigned short)maximum_convex_corner_edge_count;
}

unsigned int MYON_SubDFromMeshParameters::MaximumConvexCornerEdgeCount() const
{
  return m_maximum_convex_corner_edge_count;
}

void MYON_SubDFromMeshParameters::SetMaximumConvexCornerAngleRadians(
  double maximum_convex_corner_angle_radians
  )
{
  if (maximum_convex_corner_angle_radians > 0.0 && maximum_convex_corner_angle_radians < MYON_PI)
    m_maximum_convex_corner_angle_radians = maximum_convex_corner_angle_radians;
}

double MYON_SubDFromMeshParameters::MaximumConvexCornerAngleRadians() const
{
  return m_maximum_convex_corner_angle_radians;
}

MYON_SubDFromMeshParameters::ConvexCornerOption MYON_SubDFromMeshParameters::CopyConvexCornerTest(
  MYON_SubDFromMeshParameters source_parameters
  )
{
  SetConvexCornerOption(source_parameters.GetConvexCornerOption());
  SetMaximumConvexCornerEdgeCount(source_parameters.MaximumConvexCornerEdgeCount());
  SetMaximumConvexCornerAngleRadians(source_parameters.MaximumConvexCornerAngleRadians());
  return GetConvexCornerOption();
}

// concave corner options

MYON_SubDFromMeshParameters::ConcaveCornerOption MYON_SubDFromMeshParameters::ConcaveCornerOptionFromUnsigned(
  unsigned int concave_corner_option_as_unsigned
)
{
  switch (concave_corner_option_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::ConcaveCornerOption::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::ConcaveCornerOption::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::ConcaveCornerOption::AtMeshCorner);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDFromMeshParameters::ConcaveCornerOption::Unset);
}

void MYON_SubDFromMeshParameters::SetConcaveCornerOption(
  MYON_SubDFromMeshParameters::ConcaveCornerOption concave_corner_option
)
{
  m_concave_corner_option = MYON_SubDFromMeshParameters::ConcaveCornerOptionFromUnsigned((unsigned int)concave_corner_option);
}

MYON_SubDFromMeshParameters::ConcaveCornerOption MYON_SubDFromMeshParameters::GetConcaveCornerOption() const
{
  switch (m_concave_corner_option)
  {
  case MYON_SubDFromMeshParameters::ConcaveCornerOption::Unset:
  case MYON_SubDFromMeshParameters::ConcaveCornerOption::None:
    return m_concave_corner_option;

  case MYON_SubDFromMeshParameters::ConcaveCornerOption::AtMeshCorner:
    if (m_minimum_concave_corner_edge_count >= 2
      && m_minimum_concave_corner_edge_count <= MYON_SubDVertex::MaximumEdgeCount
      && m_minimum_concave_corner_angle_radians > MYON_PI
      && m_minimum_concave_corner_angle_radians <= MYON_2PI
      )
      return m_concave_corner_option;
    break;
  }

  return MYON_SubDFromMeshParameters::ConcaveCornerOption::Unset;
}

void MYON_SubDFromMeshParameters::SetMinimumConcaveCornerEdgeCount(
  unsigned int minimum_concave_corner_edge_count
)
{
  if (minimum_concave_corner_edge_count >= 2 && minimum_concave_corner_edge_count <= MYON_SubDVertex::MaximumEdgeCount)
    m_minimum_concave_corner_edge_count = (unsigned short)minimum_concave_corner_edge_count;
}

unsigned int MYON_SubDFromMeshParameters::MinimumConcaveCornerEdgeCount() const
{
  return m_minimum_concave_corner_edge_count;
}

void MYON_SubDFromMeshParameters::SetMinimumConcaveCornerAngleRadians(
  double minimum_concave_corner_angle_radians
)
{
  if (minimum_concave_corner_angle_radians > MYON_PI && minimum_concave_corner_angle_radians <= MYON_2PI)
    m_minimum_concave_corner_angle_radians = minimum_concave_corner_angle_radians;
}

double MYON_SubDFromMeshParameters::MinimumConcaveCornerAngleRadians() const
{
  return m_minimum_concave_corner_angle_radians;
}

MYON_SubDFromMeshParameters::ConcaveCornerOption MYON_SubDFromMeshParameters::CopyConcaveCornerTest(
  MYON_SubDFromMeshParameters source_parameters
)
{
  SetConcaveCornerOption(source_parameters.GetConcaveCornerOption());
  SetMinimumConcaveCornerEdgeCount(source_parameters.MinimumConcaveCornerEdgeCount());
  SetMinimumConcaveCornerAngleRadians(source_parameters.MinimumConcaveCornerAngleRadians());
  return GetConcaveCornerOption();
}


bool MYON_SubDFromMeshParameters::InterpolateMeshVertices() const
{
  return m_bInterpolateMeshVertices;
}

void MYON_SubDFromMeshParameters::SetInterpolateMeshVertices(
  bool bInterpolateMeshVertices
)
{
  // Not supported in free opennurbs
  m_bInterpolateMeshVertices = false;
}

MYON_SubDFromMeshParameters::TextureCoordinatesOption MYON_SubDFromMeshParameters::TextureCoordinatesOptionFromUnsigned(
  unsigned int texture_coordinate_option_as_unsigned
)
{
  switch (texture_coordinate_option_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::TextureCoordinatesOption::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::TextureCoordinatesOption::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::TextureCoordinatesOption::Automatic);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::TextureCoordinatesOption::Packed);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::TextureCoordinatesOption::CopyMapping);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::TextureCoordinatesOption::CopyCoordinates);
  default:
    break;
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDFromMeshParameters::TextureCoordinatesOption::Unset);
}


void MYON_SubDFromMeshParameters::SetTextureCoordinatesOption(
  MYON_SubDFromMeshParameters::TextureCoordinatesOption texture_coorindates_option
)
{
  // use TextureCoordinatesOptionFromUnsigned to trap invalid input.
  m_texture_coordinates_option = MYON_SubDFromMeshParameters::TextureCoordinatesOptionFromUnsigned(static_cast<unsigned char>(texture_coorindates_option));
}

MYON_SubDFromMeshParameters::TextureCoordinatesOption MYON_SubDFromMeshParameters::GetTextureCoordinatesOption() const
{
  return m_texture_coordinates_option;
}

bool MYON_SubDFromMeshParameters::MergeColinearBoundaryEdges() const
{
  // clear bit means true, set bit means false
  return (0 == (MYON_SubDFromMeshParameters::MergeColinearBoundaryEdgesMask & m_merge_edges_bits));
}

void MYON_SubDFromMeshParameters::SetMergeColinearBoundaryEdges(
  bool bAllowColinearBoundaryEdges
)
{
  const unsigned char mask = MYON_SubDFromMeshParameters::MergeColinearBoundaryEdgesMask;
  if (false == bAllowColinearBoundaryEdges)
    m_merge_edges_bits |= mask; // set bit
  else
    m_merge_edges_bits &= ~mask; // clear bit
}

bool MYON_SubDFromMeshParameters::MergeColinearInteriorEdges() const
{
  // clear bit means true, set bit means false
  return (0 == (MYON_SubDFromMeshParameters::MergeColinearInteriorEdgesMask & m_merge_edges_bits));
}

void MYON_SubDFromMeshParameters::SetMergeColinearInteriorEdges(
  bool bAllowColinearInteriorEdges
)
{
  const unsigned char mask = MYON_SubDFromMeshParameters::MergeColinearInteriorEdgesMask;
  if (false == bAllowColinearInteriorEdges)
    m_merge_edges_bits |= mask; // set bit
  else
    m_merge_edges_bits &= ~mask; // clear bit
}


void MYON_SubDFromMeshParameters::SetInteriorCreaseOption(
  MYON_SubDFromMeshParameters::InteriorCreaseOption interior_crease_option
  )
{
  m_interior_crease_option = MYON_SubDFromMeshParameters::InteriorCreaseOptionFromUnsigned((unsigned int)interior_crease_option);
}

MYON_SubDFromMeshParameters::InteriorCreaseOption MYON_SubDFromMeshParameters::GetInteriorCreaseOption() const
{
  return m_interior_crease_option;
}

MYON_SubDFromMeshParameters::InteriorCreaseOption MYON_SubDFromMeshParameters::CopyInteriorCreaseTest(
  MYON_SubDFromMeshParameters source_parameters
  )
{
  SetInteriorCreaseOption(source_parameters.GetInteriorCreaseOption());
  return GetInteriorCreaseOption();
}

MYON_SubDFromMeshParameters::InteriorCreaseOption MYON_SubDFromMeshParameters::InteriorCreaseOptionFromUnsigned(
  unsigned int interior_crease_option_as_unsigned
  )
{
  switch (interior_crease_option_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::InteriorCreaseOption::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::InteriorCreaseOption::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDFromMeshParameters::InteriorCreaseOption::AtMeshDoubleEdge);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDFromMeshParameters::InteriorCreaseOption::Unset);
}


MYON_SubDFromSurfaceParameters::Methods MYON_SubDFromSurfaceParameters::Method() const
{
  return m_method;
}

void MYON_SubDFromSurfaceParameters::SetMethod(
  MYON_SubDFromSurfaceParameters::Methods method
  )
{
  m_method = method;
}

bool MYON_SubDFromSurfaceParameters::Corners() const
{
  return m_bCorners;
}

void MYON_SubDFromSurfaceParameters::SetCorners(
  bool bCorners
  )
{
  m_bCorners = bCorners ? true : false;
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertex
//

const MYON_SubDEdgePtr MYON_SubDVertex::EdgePtr(
  unsigned int i
) const
{
  return (i < m_edge_count) ? m_edges[i] : MYON_SubDEdgePtr::Null;
}


const class MYON_SubDEdge* MYON_SubDVertex::Edge(
  unsigned int i
  ) const
{
  return (i < m_edge_count) ? MYON_SUBD_EDGE_POINTER(m_edges[i].m_ptr) : nullptr;
}

MYON__UINT_PTR MYON_SubDVertex::EdgeDirection(
  unsigned int i
  ) const
{
  return (i < m_edge_count) ? MYON_SUBD_EDGE_DIRECTION(m_edges[i].m_ptr) : 0;
}

unsigned int MYON_SubDVertex::EdgeCount() const
{
  return m_edge_count;
}

unsigned int MYON_SubDVertex::EdgeCount(
  MYON_SubDEdgeTag edge_tag
  ) const
{
  if (nullptr != m_edges)
  {
    unsigned int matching_edge_count = 0;
    const unsigned int edge_count = m_edge_count;
    for (unsigned int vei = 0; vei < edge_count; vei++)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr != e && edge_tag == e->m_edge_tag)
        matching_edge_count++;
    }
    return matching_edge_count;
  }
  return 0;
}

unsigned int MYON_SubDVertex::EdgeArrayIndex(
  const MYON_SubDEdge* edge
  ) const
{
  if ( nullptr == edge )
    return MYON_UNSET_UINT_INDEX;
  const unsigned int edge_count = m_edge_count;
  if ( 0 == edge_count)
    return MYON_UNSET_UINT_INDEX;
  const MYON_SubDEdgePtr* eptr = m_edges;
  if ( nullptr == eptr)
    return MYON_SUBD_RETURN_ERROR(MYON_UNSET_UINT_INDEX);

  for ( unsigned int i = 0; i < edge_count; i++, eptr++)
  {
    if (edge == MYON_SUBD_EDGE_POINTER(eptr->m_ptr))
      return i;
  }

  return MYON_UNSET_UINT_INDEX;
}

unsigned int MYON_SubDVertex::ReplaceEdgeInArray(
  const MYON_SubDEdge* old_edge,
  const MYON_SubDEdgePtr new_edge
  )
{
  if (m_edges == nullptr || m_edge_count == 0)
    return MYON_SUBD_RETURN_ERROR(MYON_UNSET_UINT_INDEX);
  unsigned vei = old_edge != nullptr ? EdgeArrayIndex(old_edge) : MYON_UNSET_UINT_INDEX;
  if (MYON_UNSET_UINT_INDEX == vei || m_edges[vei].m_ptr == new_edge.m_ptr)
    return MYON_UNSET_UINT_INDEX;
  if (new_edge.IsNotNull())
  {
    m_edges[vei] = new_edge;
  }
  else
  {
    const unsigned c = (unsigned)(m_edge_count--);
    while (++vei < c)
      m_edges[vei - 1] = m_edges[vei];
  }
  return vei;
}

unsigned int MYON_SubDVertex::MarkedEdgeCount() const
{
  unsigned int mark_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr != e && e->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

bool MYON_SubDVertex::ClearEdgeMarks() const
{
  bool rc = true;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr == e)
      rc = false;
    else
      e->m_status.ClearRuntimeMark();
  }
  return rc;
}

unsigned int MYON_SubDVertex::MarkedFaceCount() const
{
  unsigned int mark_count = 0;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const MYON_SubDFace* f = m_faces[vfi];
    if (nullptr != f && f->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

bool MYON_SubDVertex::ClearFaceMarks() const
{
  bool rc = true;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const MYON_SubDFace* f = m_faces[vfi];
    if (nullptr == f)
      rc = false;
    else
      f->m_status.ClearRuntimeMark();
  }
  return rc;
}


unsigned int MYON_SubDVertex::MinimumFaceEdgeCount() const
{
  unsigned short min_count = 0xFFFFU;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const MYON_SubDFace* f = m_faces[vfi];
    if (nullptr != f && f->m_edge_count < min_count)
      min_count = f->m_edge_count;
  }
  return min_count < 0xFFFFU ? min_count : 0;
}

unsigned int MYON_SubDVertex::MaximumFaceEdgeCount() const
{
  unsigned short max_count = 0;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const MYON_SubDFace* f = m_faces[vfi];
    if (nullptr != f && f->m_edge_count < max_count)
      max_count = f->m_edge_count;
  }
  return max_count;
}


unsigned int MYON_SubDVertex::MinimumEdgeFaceCount() const
{
  unsigned short min_count = 0xFFFFU;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr != e && e->m_face_count < min_count)
      min_count = e->m_face_count;
  }
  return min_count < 0xFFFFU ? min_count : 0;
}

unsigned int MYON_SubDVertex::MaximumEdgeFaceCount() const
{
  unsigned short max_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr != e && e->m_face_count > max_count)
    max_count = e->m_face_count;
  }
  return max_count;
}

unsigned int MYON_SubDEdge::MarkedVertexCount() const
{
  unsigned int mark_count = 0;
  for (unsigned evi = 0; evi < 2; ++evi)
  {
    const MYON_SubDVertex* v = m_vertex[evi];
    if (nullptr != v && v->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

unsigned int MYON_SubDEdge::MarkedFaceCount() const
{
  unsigned int mark_count = 0;
  const MYON_SubDFacePtr* fptr = m_face2;
  for (unsigned short efi = 0; efi < m_face_count; ++efi, ++fptr)
  {
    if (2 == efi)
    {
      fptr = m_facex;
      if (nullptr == fptr)
        break;
    }
    const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(fptr->m_ptr);
    if (nullptr != f && f->m_status.RuntimeMark())
      ++mark_count;
  }
  return mark_count;
}

unsigned int MYON_SubDVertex::FaceCount() const
{
  return m_face_count;
}

const class MYON_SubDFace* MYON_SubDVertex::Face(
  unsigned int i
  ) const
{
  //return (i < m_face_count) ? MYON_SUBD_FACE_POINTER(m_faces[i].m_ptr) : nullptr;
  return (i < m_face_count) ? m_faces[i] : nullptr;
}


unsigned int MYON_SubDVertex::FaceArrayIndex(
  const MYON_SubDFace* face
  ) const
{
  if ( nullptr == face )
    return MYON_UNSET_UINT_INDEX;
  const unsigned int face_count = m_face_count;
  if ( 0 == face_count)
    return MYON_UNSET_UINT_INDEX;
  if ( nullptr == m_faces)
    return MYON_SUBD_RETURN_ERROR(MYON_UNSET_UINT_INDEX);

  for ( unsigned int i = 0; i < face_count; i++)
  {
    if (face == m_faces[i] )
      return i;
  }

  return MYON_UNSET_UINT_INDEX;
}

unsigned int MYON_SubDVertex::ReplaceFaceInArray(const MYON_SubDFace * old_face, const MYON_SubDFace * new_face)
{
  unsigned vfi = (nullptr != old_face && old_face != new_face) ? FaceArrayIndex(old_face) : MYON_UNSET_UINT_INDEX;
  if (MYON_UNSET_UINT_INDEX == vfi)
    return MYON_UNSET_UINT_INDEX;
  if (nullptr != new_face)
  {
    m_faces[vfi] = new_face;
  }
  else
  {
    const unsigned c = (unsigned)(m_face_count--);
    while (++vfi < c)
      m_faces[vfi - 1] = m_faces[vfi];
  }
  return vfi;
}

const MYON_3dPoint MYON_SubDVertex::ControlNetPoint() const
{
  return MYON_3dPoint(m_P);
}

bool MYON_SubDVertex::IsSmooth() const
{
  return (MYON_SubDVertexTag::Smooth == m_vertex_tag);
}

bool MYON_SubDVertex::IsCrease() const
{
  return (MYON_SubDVertexTag::Crease == m_vertex_tag);
}

bool MYON_SubDVertex::IsCorner() const
{
  return (MYON_SubDVertexTag::Corner == m_vertex_tag);
}

bool MYON_SubDVertex::IsDart() const
{
  return (MYON_SubDVertexTag::Dart == m_vertex_tag);
}

bool MYON_SubDVertex::IsCreaseOrCorner() const
{
  return (MYON_SubDVertexTag::Crease == m_vertex_tag || MYON_SubDVertexTag::Corner == m_vertex_tag);
}

bool MYON_SubDVertex::IsDartOrCreaseOrCorner() const
{
  return (
    MYON_SubDVertexTag::Dart == m_vertex_tag
    || MYON_SubDVertexTag::Crease == m_vertex_tag
    || MYON_SubDVertexTag::Corner == m_vertex_tag
    );
}


bool MYON_SubDVertex::IsDartOrCrease() const
{
  return (
    MYON_SubDVertexTag::Dart == m_vertex_tag
    || MYON_SubDVertexTag::Crease == m_vertex_tag
    );
}

bool MYON_SubDVertex::IsDartOrCorner() const
{
  return (
    MYON_SubDVertexTag::Dart == m_vertex_tag
    || MYON_SubDVertexTag::Corner == m_vertex_tag
    );
}


bool MYON_SubDVertex::IsSmoothOrDart() const
{
  return (MYON_SubDVertexTag::Smooth == m_vertex_tag || MYON_SubDVertexTag::Dart == m_vertex_tag);
}

bool MYON_SubDVertex::IsSmoothOrDartOrCrease() const
{
  return (MYON_SubDVertexTag::Smooth == m_vertex_tag || MYON_SubDVertexTag::Dart == m_vertex_tag || MYON_SubDVertexTag::Crease == m_vertex_tag);
}

bool MYON_SubDVertex::IsSmoothOrCrease() const
{
  return (MYON_SubDVertexTag::Smooth == m_vertex_tag || MYON_SubDVertexTag::Crease == m_vertex_tag);
}

bool MYON_SubDVertex::GetBoundaryVertexEdges(
  MYON_SubDEdgePtr* eptr0,
  MYON_SubDEdgePtr* eptr1
) const
{
  unsigned int vbi[2] = {};
  const bool rc = GetBoundaryVertexEdgeIndices(&vbi[0], &vbi[1]);
  if (rc)
  {
    if (nullptr != eptr0)
      *eptr0 = m_edges[vbi[0]];
    if (nullptr != eptr1)
      *eptr1 = m_edges[vbi[1]];
  }
  else
  {
    if (nullptr != eptr0)
      *eptr0 = MYON_SubDEdgePtr::Null;
    if (nullptr != eptr1)
      *eptr1 = MYON_SubDEdgePtr::Null;
  }
  return rc;
}

bool MYON_SubDVertex::GetBoundaryVertexEdgeIndices(
  unsigned* vei0,
  unsigned* vei1
) const
{
  unsigned int vbi_count = 0;
  unsigned int vbi[2] = {};
  for (unsigned short vei = 0; vei < m_edge_count; vei++)
  {
    const MYON_SubDEdge* e = m_edges[vei].Edge();
    if (1 == e->m_face_count)
    {
      if (vbi_count < 2)
        vbi[vbi_count++] = vei;
      else
      {
        vbi_count = 0;
        break;
      }
    }
  }
  if (2 != vbi_count)
    vbi[0] = vbi[1] = MYON_UNSET_UINT_INDEX;
  if (nullptr != vei0)
    *vei0 = vbi[0];
  if (nullptr != vei1)
    *vei1 = vbi[1];
  return (2 == vbi_count);
}

const MYON_SubDVertexEdgeProperties MYON_SubDVertex::EdgeProperties() const
{
  MYON_SubDVertexEdgeProperties ep;

  ep.m_edge_count = m_edge_count;
  ep.m_face_count = m_face_count;

  bool bFirstEdge = true;
  for (unsigned short vei = 0; vei < ep.m_edge_count; vei++)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr == e)
    {
      ep.m_null_edge_count++;
      continue;
    }

    if (e->IsCrease())
      ep.m_crease_edge_count++;
    else if (e->IsSmooth())
      ep.m_smooth_edge_count++;
    else
      ep.m_unset_edge_count++;

    const unsigned short edge_face_count = e->m_face_count;
    if (bFirstEdge)
    {
      bFirstEdge = false;
      ep.m_min_edge_face_count = edge_face_count;
      ep.m_max_edge_face_count = edge_face_count;
    }
    else if (edge_face_count < ep.m_min_edge_face_count)
      ep.m_min_edge_face_count = edge_face_count;
    else if (edge_face_count > ep.m_max_edge_face_count)
      ep.m_max_edge_face_count = edge_face_count;

    if (0 == edge_face_count)
      ep.m_wire_edge_count++;
    else if (1 == edge_face_count)
      ep.m_boundary_edge_count++;
    else if (2 == edge_face_count)
      ep.m_interior_edge_count++;
    else
      ep.m_nonmanifold_edge_count++;

  }
  return ep;
}


bool MYON_SubDEdge::IsCrease() const
{
  return (MYON_SubDEdgeTag::Crease == m_edge_tag) ? true : false;
}


bool MYON_SubDEdge::IsHardCrease() const
{
  return
    (
      MYON_SubDEdgeTag::Crease == m_edge_tag
      && nullptr != m_vertex[0]
      && nullptr != m_vertex[1]
      && m_vertex[0]->IsCreaseOrCorner()
      && m_vertex[1]->IsCreaseOrCorner()
      )
    ? true
    : false;
}

bool MYON_SubDEdge::IsDartCrease() const
{
  return
    (MYON_SubDEdgeTag::Crease == m_edge_tag && DartCount() > 0 )
    ? true
    : false;
}

unsigned int MYON_SubDEdge::DartCount() const
{
  unsigned int dart_count = 0;
  if (nullptr != m_vertex[0] && MYON_SubDVertexTag::Dart == m_vertex[0]->m_vertex_tag)
    dart_count++;
  if (nullptr != m_vertex[1] && MYON_SubDVertexTag::Dart == m_vertex[1]->m_vertex_tag)
    dart_count++;
  return dart_count;
}

bool MYON_SubDEdge::IsSmooth() const
{
  return (MYON_SubDEdgeTag::Smooth == m_edge_tag || MYON_SubDEdgeTag::SmoothX == m_edge_tag) ? true : false;
}

bool MYON_SubDEdge::IsSmoothNotX() const
{
  return (MYON_SubDEdgeTag::Smooth == m_edge_tag) ? true : false;
}

bool MYON_SubDEdge::IsSmoothX() const
{
  return (MYON_SubDEdgeTag::SmoothX == m_edge_tag) ? true : false;
}

const MYON_SubDEdgeSharpness MYON_SubDEdge::Sharpness() const
{
  return IsSmooth() ? m_sharpness : MYON_SubDEdgeSharpness::Zero;
}

double MYON_SubDEdge::EndSharpness(
  unsigned evi
) const
{
  return (IsSmooth() && evi >= 0 && evi <= 1) ? m_sharpness[evi] : 0.0;
}

double MYON_SubDEdge::EndSharpness(
  const MYON_SubDVertex* v
) const
{
  if (nullptr != v)
  {
    if (v == m_vertex[0])
      return EndSharpness(0U);
    if (v == m_vertex[1])
      return EndSharpness(1U);
  }
  return 0.0;
}

const MYON_SubDEdgeSharpness MYON_SubDEdge::SubdivideSharpness(
  unsigned evi,
  bool bReverse
) const
{
  const MYON_SubDEdgeSharpness s = Sharpness().Subdivided(evi);
  return bReverse ? s.Reversed() : s;
}

const MYON_SubDEdgeSharpness MYON_SubDEdge::SubdivideSharpness(
  const class MYON_SubDVertex* end_vertex,
  bool bReverse
) const
{
  if (nullptr != end_vertex)
  {
    if (end_vertex == this->m_vertex[0])
      return this->SubdivideSharpness(0U, bReverse);
    if (end_vertex == this->m_vertex[1])
      return this->SubdivideSharpness(1U, bReverse);
  }
  return MYON_SubDEdgeSharpness::Zero;
}

bool MYON_SubDEdge::ClearSharpnessForExperts()
{
  const bool bChanged = IsSmooth() && (m_sharpness != MYON_SubDEdgeSharpness::Zero);
  m_sharpness = MYON_SubDEdgeSharpness::Zero;
  return bChanged;
}

void MYON_SubDEdge::SetSharpnessForExperts(MYON_SubDEdgeSharpness sharpness)
{
  m_sharpness = IsSmooth() ? sharpness : MYON_SubDEdgeSharpness::Zero;
}


bool MYON_SubDEdgeSharpness::IsConstant() const
{
  return m_edge_sharpness[0] == m_edge_sharpness[1] && 0.0f <= m_edge_sharpness[0] && m_edge_sharpness[0] <= ((float)MYON_SubDEdgeSharpness::Maximum);
}

bool MYON_SubDEdgeSharpness::IsVariable() const
{
  return m_edge_sharpness[0] != m_edge_sharpness[1] && IsValid();
}

bool MYON_SubDEdgeSharpness::IsZero() const
{
  return (0.0f == m_edge_sharpness[0] && 0.0f == m_edge_sharpness[1]);
}

bool MYON_SubDEdgeSharpness::IsNotZero() const
{
  return (m_edge_sharpness[0] != 0.0f || m_edge_sharpness[1] != 0.0f) && IsValid();
}

bool MYON_SubDEdgeSharpness::IsValid() const
{
  return
    m_edge_sharpness[0] >= 0.0f
    && m_edge_sharpness[0] <= ((float)MYON_SubDEdgeSharpness::Maximum)
    && m_edge_sharpness[1] >= 0.0f
    && m_edge_sharpness[1] <= ((float)MYON_SubDEdgeSharpness::Maximum)
    ;
}

bool MYON_SubDEdgeSharpness::IsNotValid() const
{
  return (false == IsValid());
}

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::FromConstant(
  double sharpness
)
{
  MYON_SubDEdgeSharpness s;
  s.m_edge_sharpness[0] = (sharpness >= 0.0 && sharpness  <= MYON_SubDEdgeSharpness::Maximum) ? ((float)MYON_SubDEdgeSharpness::Sanitize(sharpness, 0.0)) : MYON_FLT_QNAN;
  s.m_edge_sharpness[1] = s.m_edge_sharpness[0];
  return s;
}

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::FromInterval(
  double sharpness0,
  double sharpness1
)
{
  if (sharpness0 >= 0.0 && sharpness0 <= MYON_SubDEdgeSharpness::Maximum && sharpness1 >= 0.0 && sharpness1 <= MYON_SubDEdgeSharpness::Maximum)
  {
    MYON_SubDEdgeSharpness s;
    s.m_edge_sharpness[0] = (float)MYON_SubDEdgeSharpness::Sanitize(sharpness0, 0.0);
    s.m_edge_sharpness[1] = (float)MYON_SubDEdgeSharpness::Sanitize(sharpness1, 0.0);
    return s;
  }
  return MYON_SubDEdgeSharpness::Nan;
}

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::FromInterval(const class MYON_Interval& sharpness_interval)
{
  return MYON_SubDEdgeSharpness::FromInterval(sharpness_interval[0], sharpness_interval[1]);
}

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::Union(
  const MYON_SubDEdgeSharpness& a,
  const MYON_SubDEdgeSharpness& b
)
{
  float s[4] = {};
  int count = 0;
  if (a.m_edge_sharpness[0] > 0.0f || a.m_edge_sharpness[1] > 0.0f)
  {
    s[0] = a.m_edge_sharpness[0];
    s[1] = a.m_edge_sharpness[1];
    count = 2;
  }
  if (b.m_edge_sharpness[0] > 0.0f || b.m_edge_sharpness[1] > 0.0f)
  {
    s[count] = b.m_edge_sharpness[0];
    s[count+1] = b.m_edge_sharpness[1];
    count += 2;
  }
  MYON_SubDEdgeSharpness u = MYON_SubDEdgeSharpness::FromConstant(s[0]);
  for ( int i = 1; i < count; ++i)
  { 
    float x = s[i];
    if (x < u.m_edge_sharpness[0])
      u.m_edge_sharpness[0] = x;
    else if (x > u.m_edge_sharpness[1])
      u.m_edge_sharpness[1] = x;
  }
  return u;
}

double MYON_SubDEdgeSharpness::Sanitize(
  double sharpness, 
  double invalid_input_result
)
{
  // When sharpness is withing MYON_SubDEdgeSharpness::Tolerance of an integer value,
  // snap to that integer value.

  if (false == (sharpness >= 0.0 && sharpness <= MYON_SubDEdgeSharpness::Maximum))
    return invalid_input_result; // when sharpness is nan, invalid_input_result is returned here. 

  // When sharpness is close to an integer value, snap to the integer value.
  // This results in cleaner looking limit surfaces and faster evaluation.
  const double f = floor(sharpness);
  if (f >= 0.0 && f <= MYON_SubDEdgeSharpness::Maximum)
  {
    if (sharpness - f <= MYON_SubDEdgeSharpness::Tolerance)
      sharpness = f;
    else if (f + 1.0 - sharpness <= MYON_SubDEdgeSharpness::Tolerance && f + 1.0 <= MYON_SubDEdgeSharpness::Maximum)
      sharpness = f + 1.0;
  }

  return sharpness;
}

double MYON_SubDEdgeSharpness::Sanitize(
  double sharpness
)
{
  // When edges are subdivided, the call MYON_SubDEdgeSharpness::Sanitize(current_sharpness-1.0).
  // These critical calls assume this function returns 0.0 when the input is not valid, for example
  // negative when 0 <= current_sharpness < 1.
  return MYON_SubDEdgeSharpness::Sanitize(sharpness, 0.0);
}

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::Subdivided(int end_index) const
{
  if (end_index >= 0 && end_index <= 1)
  {
    const double s[2] = { 
      MYON_SubDEdgeSharpness::Sanitize(((double)(this->m_edge_sharpness[0])) - 1.0),
      MYON_SubDEdgeSharpness::Sanitize(((double)(this->m_edge_sharpness[1])) - 1.0)
    };
    const double mids = (s[0] == s[1]) ? s[0] : MYON_SubDEdgeSharpness::Sanitize(0.5 * (s[0] + s[1]));
    return (0 == end_index) ? MYON_SubDEdgeSharpness::FromInterval(s[0], mids) : MYON_SubDEdgeSharpness::FromInterval(mids, s[1]);
  }
  return MYON_SubDEdgeSharpness::Zero;
}

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::Reversed() const
{
  MYON_SubDEdgeSharpness r;
  r.m_edge_sharpness[0] = this->m_edge_sharpness[1];
  r.m_edge_sharpness[1] = this->m_edge_sharpness[0];
  return r;
}

double MYON_SubDEdgeSharpness::operator[](int end_index) const
{
  return this->EndSharpness(end_index);
}

bool operator!=(const MYON_SubDEdgeSharpness& lhs, const MYON_SubDEdgeSharpness& rhs)
{
  const float* l = (const float*)(&lhs);
  const float* r = (const float*)(&rhs);
  return l[0] != r[0] || l[1] != r[1];
}

bool operator==(const MYON_SubDEdgeSharpness& lhs, const MYON_SubDEdgeSharpness& rhs)
{
  const float* l = (const float*)(&lhs);
  const float* r = (const float*)(&rhs);
  return l[0] == l[1] && r[0] == r[1];
}

double MYON_SubDEdgeSharpness::Average() const
{
  return
    (m_edge_sharpness[0] == m_edge_sharpness[1]) 
    ? ((double)m_edge_sharpness[0]) 
    : MYON_SubDEdgeSharpness::Sanitize(0.5 * ( ((double)m_edge_sharpness[0]) + ((double)m_edge_sharpness[1]) ));
}

double MYON_SubDEdgeSharpness::MinimumEndSharpness() const
{
  return (double)((m_edge_sharpness[0] <= m_edge_sharpness[1]) ? m_edge_sharpness[0] : m_edge_sharpness[1]);
}

double MYON_SubDEdgeSharpness::MaximumEndSharpness() const
{
  return (double)((m_edge_sharpness[0] >= m_edge_sharpness[1]) ? m_edge_sharpness[0] : m_edge_sharpness[1]);
}


double MYON_SubDEdgeSharpness::EndSharpness(int end_index) const
{
  return (end_index >= 0 && end_index <= 1) ? ((double)m_edge_sharpness[end_index]) : MYON_DBL_QNAN;
}

double MYON_SubDEdgeSharpness::VertexSharpness(
  MYON_SubDVertexTag vertex_tag,
  unsigned sharp_edge_end_count,
  double maximum_edge_end_sharpness
)
{
  // NOTE WELL:
  // For edge chains with a contant sharpness, this agrees with DKT SIGGRAPH 1998.
  // For edge chains with variable sharpness, having a sharpness interval on the
  // edge with end values matching at shared vertices gives nicer looking results
  // than assigning a constant sharpness to edges and Chaikin's subdivision rule
  // (averaging) at vertices.

  if (sharp_edge_end_count <= 0)
  {
    // no edges have nonzero end sharpness at this vertex
    return 0.0;
  }

  if (false == (maximum_edge_end_sharpness > 0.0))
  {
    // This happens when the sharp edges attached to the vertex
    // have variable sharpness with 0 at the vertex
    // and nonzero sharpness at the other ends.
    return 0.0;
  }

  if (MYON_SubDVertexTag::Corner == vertex_tag)
  {
    // No sharpness bias for corner vertex subdivision.
    // At a corner vertex, all subdivision points and the limit point are equal to the level 0 control net point.
    // Thus, applying a sharp bias doesn't modify the subdivision point or limit point.
    return 0.0;
  }

  unsigned crease_edge_count;
  if (MYON_SubDVertexTag::Smooth == vertex_tag)
    crease_edge_count = 0;
  else if (MYON_SubDVertexTag::Crease == vertex_tag)
    crease_edge_count = 2;
  else if (MYON_SubDVertexTag::Dart == vertex_tag)
    crease_edge_count = 1;
  else
  {
    MYON_SUBD_ERROR("vertex_tag parameter is invalid or unset.");
    return 0.0;
  }

  if (sharp_edge_end_count + crease_edge_count < 2U)
  {
    // 1 sharp edge and no creases
    // No sharp bias for the vertex subdivision point.
    return 0.0;
  }

  return maximum_edge_end_sharpness;
}

double MYON_SubDEdgeSharpness::SharpnessFromNormalizedValue(
  double normalized_slider_value
)
{
  return SharpnessFromSliderValue(
    MYON_Interval::ZeroToOne,
    normalized_slider_value,
    MYON_DBL_QNAN
  );
}

double MYON_SubDEdgeSharpness::SharpnessFromSliderValue(
  MYON_Interval slider_domain,
  double slider_value,
  double invalid_input_result
)
{
  if (slider_domain.IsInterval() && slider_value >= slider_domain.Min() && slider_value <= slider_domain.Max())
  {
    const double s = (0.0 == slider_domain[0] && 1.0 == slider_domain[1]) ? slider_value : slider_domain.NormalizedParameterAt(slider_value);
    if (s >= 0.0 && s <= 1.0)
    {
      const MYON_Interval ZeroToMaximumSharpness(0.0, MYON_SubDEdgeSharpness::Maximum);
      double sharpness = MYON_SubDEdgeSharpness::Sanitize(ZeroToMaximumSharpness.ParameterAt(s), invalid_input_result);
      if (sharpness >= 0.0 && sharpness <= MYON_SubDEdgeSharpness::Maximum)
        return sharpness;
    }
  }
  return invalid_input_result;
}



bool MYON_SubD::HasSharpEdges() const
{
  bool bHasSharpEdges = 0 != (MYON_ComponentAttributes::EdgeAttributes::InteriorSharp & this->AggregateEdgeAttributes());
  return bHasSharpEdges;

  //MYON_SubDEdgeIterator eit = this->EdgeIterator();
  //for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  //{
  //  if (e->IsSharp())
  //    return true;
  //}
  //return false;
}

unsigned int MYON_SubD::SharpEdgeCount(MYON_SubDEdgeSharpness& sharpness_range) const
{
  sharpness_range = MYON_SubDEdgeSharpness::Zero;
  unsigned int sharp_edge_count = 0;
  MYON_SubDEdgeIterator eit = this->EdgeIterator();
  for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    if (e->IsSharp())
    {
      sharpness_range = MYON_SubDEdgeSharpness::Union(sharpness_range, e->Sharpness());
      ++sharp_edge_count;      
    }
  }
  return sharp_edge_count;
}

unsigned int MYON_SubD::SharpEdgeCount() const
{
  MYON_SubDEdgeSharpness sharpness_range = MYON_SubDEdgeSharpness::Zero;
  return SharpEdgeCount(sharpness_range);
}

bool MYON_SubDVertex::IsSharp( bool bEndCheck ) const
{
  // This definition of vertex sharpness is from DeRose, Kass, Truong 1998
  // If there are exactly two sharp edges adjacent to the vertex, then
  // the vertex sharpness is the average of the edge sharpnesses.
  if (IsSmoothOrDartOrCrease() && nullptr != m_edges)
  {
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr != e && e->IsSharp() )
        return true;
    }
  }
  return false;
}

unsigned int MYON_SubD::ClearEdgeSharpness()
{
  unsigned int sharp_edge_count = 0;
  MYON_SubDEdgeIterator eit = this->EdgeIterator();
  for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    if (const_cast<MYON_SubDEdge*>(e)->ClearSharpnessForExperts())
    {
      e->ClearSavedSubdivisionPoints(true);
      ++sharp_edge_count;
    }
  }
  
  if (sharp_edge_count != 0)
    this->ChangeGeometryContentSerialNumberForExperts(true);

  return sharp_edge_count;
}


double MYON_SubDVertex::VertexSharpness() const
{
  MYON_3dPoint sharp_subdivision_point;
  return GetSharpSubdivisionPoint(sharp_subdivision_point);
}


double MYON_SubDVertex::GetSharpSubdivisionPoint(MYON_3dPoint& sharp_subdivision_point) const
{
  // Below "sharp edge" means an edge attached to this vertex that has a smooth tag
  // and a nonzero sharpness at the end attached to this vertex.
  // This definition of vertex sharpness is from DeRose, Kass, Truong 1998.
  // - If the vertex is smooth and adjacent to zero or one sharp edges, 
  //   then the ordindary vertex subdivision rule is used, 
  //   sharp_subdivision_point is set to NanPoint,
  //   and 0.0 is returned.
  // - If the vertex is adjacent to 3 or more sharp edges and crease edges, 
  //   then the corner vertex subdivision rule is used,
  //   sharp_subdivision_point = vertex control net point,
  //   and the maximum edge sharpness at the vertex is returned.
  // - If the vertex is adjacent to 2 sharp edges and crease edges, 
  //   then sharp_subdivision_point = crease vertex subdivision point, 
  //   and the average of the edge sharpnesses is returned.
  // 
  // The returned vertex sharpenss is awasy >= 0.
  // When the returned vertex sharpness is > 0, the vertex subdivision point
  // (1-s)*(ordinary subdivision point) + s*sharp_subdivision_point,
  // where s = min(returned vertex sharpness, 1).

  if (this->IsSmoothOrDartOrCrease() && nullptr != m_edges)
  {
    unsigned int sharp_edge_count = 0;
    unsigned other_v_count = 0;
    double max_end_sharpeness = 0.0;
    const MYON_SubDVertex* other_v[2] = {};

    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == e)
        continue;
      if (e->IsCrease())
      {
        if (other_v_count < 2)
          other_v[other_v_count] = e->OtherEndVertex(this);
        ++other_v_count;
      }
      else if ( e->IsSharp() )
      {
        // The edge is has a smooth tag and at leas one end has sharpness > 0.
        // For variable edge sharpness, s can be zero at an end.
        const double s = e->EndSharpness(this);
        if (s > 0.0)
        {
          if (other_v_count < 2)
            other_v[other_v_count] = e->OtherEndVertex(this);
          ++other_v_count;
          if (s > max_end_sharpeness)
            max_end_sharpeness = s;
          ++sharp_edge_count;
        }
      }
    }

    const double vertex_sharpness = MYON_SubDEdgeSharpness::VertexSharpness(m_vertex_tag, sharp_edge_count, max_end_sharpeness);
    if (vertex_sharpness > 0.0)
    {
      if (2 == other_v_count && nullptr != other_v[0] && nullptr != other_v[1])
      {
        // 2 creases and sharps - "crease" subdivision point
        sharp_subdivision_point = 0.125 * (6.0 * this->ControlNetPoint() + other_v[0]->ControlNetPoint() + other_v[1]->ControlNetPoint());
      }
      else
      {
        // 3 or more creases and sharps - "corner" subdivision point
        sharp_subdivision_point = this->ControlNetPoint();
      }
      return vertex_sharpness;
    }
  }

  sharp_subdivision_point = MYON_3dPoint::NanPoint;
  return 0.0;
}

unsigned int MYON_SubDVertex::SharpEdgeCount( bool bEndCheck ) const
{
  MYON_Interval sharpness_range;
  return SharpEdgeCount(bEndCheck, sharpness_range);
}

unsigned int MYON_SubDVertex::SharpEdgeCount(
  bool bEndCheck,
  MYON_Interval& sharpness_range
) const
{
  unsigned int sharp_edge_count = 0;
  double mins = 0.0;
  double maxs = 0.0;
  if (IsSmoothOrDartOrCrease() && nullptr != m_edges)
  {
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr != e ? e->IsSharp() : false)
      {
        const double s 
          = bEndCheck 
          ? e->EndSharpness(this) 
          : e->Sharpness().MaximumEndSharpness();
        if (s > 0.0)
        {
          if (0 == sharp_edge_count)
          {
            mins = s;
            maxs = s;
          }
          else if (s < mins)
            mins = s;
          else if (s > maxs)
            maxs = s;
          ++sharp_edge_count;
        }
      }
    }
  }
  sharpness_range.Set(mins, maxs);
  return sharp_edge_count;
}

bool MYON_SubDEdge::IsSharp() const
{
  return 
    (MYON_SubDEdgeTag::Smooth == m_edge_tag || MYON_SubDEdgeTag::SmoothX == m_edge_tag)
    && m_sharpness.IsNotZero();
}

bool MYON_SubDEdge::IsSmoothNotSharp() const
{
  return
    (MYON_SubDEdgeTag::Smooth == m_edge_tag || MYON_SubDEdgeTag::SmoothX == m_edge_tag)
    && (false == m_sharpness.IsNotZero());
}

bool MYON_SubDEdge::IsSmoothNotXNotSharp() const
{
  return MYON_SubDEdgeTag::Smooth == m_edge_tag && (false == m_sharpness.IsNotZero());
}

double MYON_SubDEdge::GetSharpSubdivisionPoint(MYON_3dPoint& sharp_subdivision_point) const
{
  if (IsSharp())
  {
    const double s = Sharpness().Average();
    sharp_subdivision_point = this->ControlNetCenterPoint();
    return s;
  }

  sharp_subdivision_point = MYON_3dPoint::NanPoint;
  return 0.0;
}

bool MYON_SubDVertex::IsSingleSectorVertex() const
{
  const bool bIsCreaseOrCorner = IsCreaseOrCorner();
  if (bIsCreaseOrCorner)
  {
    if (m_face_count < 1 || m_face_count + 1 != m_edge_count)
      return false;
  }
  else if (IsSmoothOrDart())
  {
    if (m_face_count < 2 || m_edge_count!= m_face_count)
      return false;
  }
  else
    return false;


  unsigned short boundary_crease_count = 0;
  unsigned short interior_crease_count = 0;
  unsigned short interior_smooth_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(this->m_edges[vei].m_ptr);
    if (nullptr == e)
      return false;

    if (e->IsSmooth())
    {
      if (2 == e->m_face_count)
      {
        ++interior_smooth_count;
        continue;
      }
    }
    else if (MYON_SubDEdgeTag::Crease == e->m_edge_tag)
    {
      if (2 == e->m_face_count)
      {
        ++interior_crease_count;
        if (MYON_SubDVertexTag::Dart == m_vertex_tag && 1 == interior_crease_count)
          continue;
      }
      else if (1 == e->m_face_count)
      {
        ++boundary_crease_count;
        if (bIsCreaseOrCorner && boundary_crease_count <= 2)
          continue;
      }
    }

    return false;
  }

  if (bIsCreaseOrCorner)
  {
    if (2 == boundary_crease_count && 2+interior_smooth_count == m_edge_count)
      return true;
  }
  else if (MYON_SubDVertexTag::Dart == m_vertex_tag)
  {
    if (1 == interior_crease_count && 1+interior_smooth_count == m_edge_count)
      return true;
  }
  else if (MYON_SubDVertexTag::Smooth == m_vertex_tag)
  {
    if (interior_smooth_count == m_edge_count)
      return true;
  }

  return false;
}

bool MYON_SubDVertex::IsManifoldBoundaryVertex() const
{
  return IsCreaseOrCorner() && IsSingleSectorVertex();
}


bool MYON_SubDVertex::HasInteriorVertexTopology() const
{
  if (m_edge_count >= 2 && m_edge_count == m_face_count && nullptr != m_edges && nullptr != m_faces)
  {
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == e || 2 != e->m_face_count)
        return false;
    }
    return true;
  }    
  return false;
}

bool MYON_SubDVertex::HasBoundaryVertexTopology() const
{
  if (m_edge_count >= 2 && m_edge_count == m_face_count+1 && nullptr != m_edges && nullptr != m_faces)
  {
    unsigned boundary_count = 0;
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == e || 0 == e->m_face_count || e->m_face_count > 2)
        return false;
      if (1 == e->m_face_count)
        ++boundary_count;
    }
    if (2 == boundary_count)
      return true;
  }    
  return false;
}

const MYON_SubDComponentPtrPair MYON_SubDVertex::BoundaryEdgePair() const
{
  MYON_SubDComponentPtrPair epair = MYON_SubDComponentPtrPair::Null;
  if (nullptr != m_edges && m_edge_count >= 2)
  {
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      MYON_SubDEdgePtr eptr = m_edges[vei];
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
      if (nullptr == e)
        continue;
      if (false == e->HasBoundaryEdgeTopology())
        continue;
      const MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
      if (this != e->m_vertex[edir])
      {
        // m_edges[vei] is corrupt ...
        MYON_SUBD_ERROR("m_edges[vei] has incorrect edge orientation flag.");
        if (this == e->m_vertex[1 - edir])
          eptr = eptr.Reversed(); // we can still return the requested information.
        else
          return MYON_SubDComponentPtrPair::Null;
      }
      if (epair.m_pair[0].IsNull())
        epair.m_pair[0] = MYON_SubDComponentPtr::Create(eptr);
      else if (epair.m_pair[1].IsNull())
        epair.m_pair[1] = MYON_SubDComponentPtr::Create(eptr);
      else
        return MYON_SubDComponentPtrPair::Null; // 3 or more boundary edges
    }
  }
  return (epair.m_pair[1].IsNotNull()) ? epair : MYON_SubDComponentPtrPair::Null;
}

const MYON_SubDComponentPtrPair MYON_SubDVertex::CreasedEdgePair(bool bInteriorEdgesOnly) const
{
  MYON_SubDComponentPtrPair creased_eptr_pair = MYON_SubDComponentPtrPair::Null;
  if (nullptr != m_edges && m_edge_count >= 2)
  {
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == e)
        continue;
      if (bInteriorEdgesOnly && false == e->HasInteriorEdgeTopology(false))
        continue;
      if (MYON_SubDEdgeTag::Crease == e->m_edge_tag)
      {
        if (e == MYON_SUBD_EDGE_POINTER(creased_eptr_pair.m_pair[0].m_ptr) || e == MYON_SUBD_EDGE_POINTER(creased_eptr_pair.m_pair[1].m_ptr))
        {
          MYON_SUBD_ERROR("Duplicate entries in m_edges[] list.");
          continue;
        }
        if (creased_eptr_pair.FirstIsNull())
          creased_eptr_pair.m_pair[0] = MYON_SubDComponentPtr::Create(m_edges[vei]);
        else if (creased_eptr_pair.SecondIsNull())
          creased_eptr_pair.m_pair[1] = MYON_SubDComponentPtr::Create(m_edges[vei]);
        else
          return MYON_SubDComponentPtrPair::Null; // 3 or more creases
      }
    }
  }
  return creased_eptr_pair.SecondIsNull() ? MYON_SubDComponentPtrPair::Null : creased_eptr_pair;
}

const MYON_SubDEdgePtr MYON_SubDVertex::CreasedEdge(bool bInteriorEdgesOnly) const
{
  MYON_SubDEdgePtr creased_eptr = MYON_SubDEdgePtr::Null;
  if (nullptr != m_edges)
  {
    for (unsigned short vei = 0; vei < m_edge_count; ++vei)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == e)
        continue;
      if (bInteriorEdgesOnly && false == e->HasInteriorEdgeTopology(false))
        continue;
      if (MYON_SubDEdgeTag::Crease == e->m_edge_tag)
      {
        if (creased_eptr.IsNull())
          creased_eptr = m_edges[vei];
        else
          return MYON_SubDEdgePtr::Null; // 2 or more creases.
      }
    }
  }
  return creased_eptr;
}

const unsigned int MYON_SubDVertex::CreasedEdgeCount() const
{
  return  CreasedEdgeCount(true, true, true, true);
}

const unsigned int MYON_SubDVertex::CreasedEdgeCount(
  bool bCountInteriorCreases, 
  bool bCountBoundaryCreases, 
  bool bCountNonmanifoldCreases,
  bool bCountWireCreases
) const
{
  unsigned creased_edge_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr == e)
      continue;
    if (MYON_SubDEdgeTag::Crease != e->m_edge_tag)
      continue;
    switch (e->m_face_count)
    {
    case 0:
      if (bCountWireCreases)
        ++creased_edge_count;
      break;
    case 1:
      if (bCountBoundaryCreases)
        ++creased_edge_count;
      break;
    case 2:
      if (bCountInteriorCreases)
        ++creased_edge_count;
      break;
    default:
      if (bCountNonmanifoldCreases)
        ++creased_edge_count;
      break;
    }
  }
  return creased_edge_count;
}

bool MYON_SubDVertexEdgeProperties::HasInteriorVertexTopology() const
{
  return 
    m_null_edge_count == 0
    && m_edge_count == m_face_count
    && m_boundary_edge_count == 0
    && m_interior_edge_count >= 2
    && m_nonmanifold_edge_count == 0
    ;
}

bool MYON_SubDVertexEdgeProperties::HasBoundaryVertexTopology() const
{
  return
    m_null_edge_count == 0
    && m_edge_count == m_face_count+1
    && m_boundary_edge_count == 2
    && m_nonmanifold_edge_count == 0
    ;
}

bool MYON_SubDVertexEdgeProperties::HasManifoldVertexTopology() const
{
  return HasInteriorVertexTopology() || HasBoundaryVertexTopology();
}

bool MYON_SubDVertexEdgeProperties::HasNonmanifoldVertexTopology() const
{
  return  
    (m_null_edge_count == 0)
    && (m_wire_edge_count > 0 || m_nonmanifold_edge_count > 0)
    ;
}

bool MYON_SubDVertex::IsStandard() const
{
  if (nullptr == m_edges)
    return false;

  if (nullptr == m_faces)
    return false;

  const unsigned int edge_count = m_edge_count;
  if (!MYON_SubD::IsValidSectorEdgeCount(m_vertex_tag,edge_count))
    return false;

  const unsigned int face_count = m_face_count;
  if (face_count != MYON_SubDSectorType::SectorFaceCountFromEdgeCount(m_vertex_tag, edge_count))
    return false;

  const unsigned short f_edge_count = 4;

  unsigned int crease_edge_face_count = MYON_UNSET_UINT_INDEX;
  bool bTaggedVertex = false;
  switch (m_vertex_tag)
  {
  case MYON_SubDVertexTag::Unset:
    return false;
    break;

  case MYON_SubDVertexTag::Smooth:
    if (edge_count != face_count)
      return false;
    break;

  case MYON_SubDVertexTag::Crease:
    if (edge_count != face_count+1)
      return false;
    crease_edge_face_count = 1;
    bTaggedVertex = true;
    break;

  case MYON_SubDVertexTag::Corner:
    if (edge_count != face_count+1)
      return false;
    crease_edge_face_count = 1;
    bTaggedVertex = true;
    break;

  case MYON_SubDVertexTag::Dart:
    if (edge_count != face_count)
      return false;
    crease_edge_face_count = 2;
    bTaggedVertex = true;
    break;

  default:
    return false;
    break;
  }
  

  for (unsigned int vfi = 0; vfi < face_count; vfi++)
  {
    const MYON_SubDFace* f = m_faces[vfi];
    if (nullptr == f)
      return false;
    if (f_edge_count != f->m_edge_count)
      return false;
  }

  unsigned int creased_edge_count = 0;
  double sector_coefficient = 0.0;
  for (unsigned int vei = 0; vei < edge_count; vei++)
  {
    const MYON_SubDEdge* e = m_edges[vei].Edge();
    if (nullptr == e)
      return false;

    unsigned int evi;
    if (this == e->m_vertex[0])
      evi = 0;
    else if (this == e->m_vertex[1])
      evi = 1;
    else
      return false;

    const MYON_SubDVertex* other_vertex = e->m_vertex[1-evi];

    if (nullptr == other_vertex)
      return false;

    if (MYON_SubDEdgeTag::Smooth == e->m_edge_tag)
    {
      if (2 != e->m_face_count)
        return false;

      if (bTaggedVertex && 0 == vei)
      {
        sector_coefficient = e->m_sector_coefficient[evi];
        if (!(0.0 <= sector_coefficient && sector_coefficient <= 1.0))
          return false;
      }
      
      if (!(sector_coefficient == e->m_sector_coefficient[evi]))
        return false;

      if (MYON_SubDVertexTag::Smooth == other_vertex->m_vertex_tag)
      {
        if ( !(0.0 == e->m_sector_coefficient[1-evi]) )
          return false;
      }
      else
      {
        if ( bTaggedVertex )
          return false;
        if ( !(0.5 == e->m_sector_coefficient[1-evi]) )
          return false;
      }
    }
    else if (MYON_SubDEdgeTag::Crease == e->m_edge_tag)
    {
      if (crease_edge_face_count != e->m_face_count)
        return false;
      creased_edge_count++;
      if (creased_edge_count > 2)
        return false;
      if (false == other_vertex->IsDartOrCreaseOrCorner())
        return false;
    }
    else
    {
      return false;
    }
  }

  switch (creased_edge_count)
  {
  case 0:
    if (false == IsSmooth())
      return false;
    break;
  case 1:
    if (false == IsDart())
      return false;
    break;
  case 2:
    if (false == IsCreaseOrCorner())
      return false;
    break;
  default:
    return false;
  }

  // The standard subdivison matrix will correctly calculate
  // the subdivision location for this vertex.
  return true;
}


unsigned int MYON_SubDEdge::EdgeAttributes() const
{
  unsigned int edge_topology_attributes = 0U;

  if (nullptr == m_vertex[0] || nullptr == m_vertex[1])
  {
    edge_topology_attributes |= MYON_ComponentAttributes::Damaged;
  }
  else
  {
    const double* P[2] = { m_vertex[0]->m_P, m_vertex[1]->m_P };
    if (
      fabs(P[0][0]) < MYON_UNSET_POSITIVE_VALUE
      && fabs(P[0][1]) < MYON_UNSET_POSITIVE_VALUE
      && fabs(P[0][2]) < MYON_UNSET_POSITIVE_VALUE
      && fabs(P[1][0]) < MYON_UNSET_POSITIVE_VALUE
      && fabs(P[1][1]) < MYON_UNSET_POSITIVE_VALUE
      && fabs(P[1][2]) < MYON_UNSET_POSITIVE_VALUE
      )
    {
      if (P[0][0] == P[1][0] && P[0][1] == P[1][1] && P[0][2] == P[1][2])
        edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::ZeroLength;
      else
        edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::NonzeroLength;
    }
    if (m_vertex[0] != m_vertex[1])
      edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::Open;
    else
      edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::Closed;
  }
  
  const MYON_SubDFace* f[2] = { MYON_SUBD_FACE_POINTER(m_face2[0].m_ptr),MYON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
  switch (m_face_count)
  {
  case 0:
    edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::Wire;
    break;

  case 1:
    edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::Boundary;
    if ( nullptr == f[0])
      edge_topology_attributes |= MYON_ComponentAttributes::Damaged;
    break;

  case 2:
    edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::Interior;

    if (IsSmooth())
    {
      edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorSmooth;
      if ( IsSharp() )
        edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorSharp;
    }
    else if (IsCrease())
      edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorCrease;

    if (nullptr != f[0] && nullptr != f[1])
    {
      if (MYON_SUBD_FACE_DIRECTION(m_face2[0].m_ptr) == MYON_SUBD_FACE_DIRECTION(m_face2[1].m_ptr))
        edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorNotOriented;
      else
        edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorOriented;
      if (f[0] != f[1])
      {
        edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorTwoFaced;
      }
      else
      {
        const unsigned int fecount = f[0]->EdgeCount();
        const unsigned int fei0 = f[0]->EdgeArrayIndex(this);
        if (fecount > 2 && fei0 < fecount)
        {
          for (unsigned int fei1 = fei0 + 1; fei1 < fecount; ++fei1)
          {
            if (this == f[0]->Edge(fei1))
            {
              if (f[0]->EdgeDirection(fei0) != f[0]->EdgeDirection(fei1))
              {
                if ( fei1+1 == fei0 || (0 == fei0 && fei1+1 == fecount))
                  edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorSlit;
                else
                  edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::InteriorSeam;
              }
              break;
            }
          }
        }
      }
    }
    else
    {
      edge_topology_attributes |= MYON_ComponentAttributes::Damaged;
    }
    break;

  default:
    edge_topology_attributes |= MYON_ComponentAttributes::EdgeAttributes::Nonmanifold;
    if ( nullptr == f[0] || nullptr == f[1] || nullptr == m_facex )
      edge_topology_attributes |= MYON_ComponentAttributes::Damaged;
    break;
  }

  return edge_topology_attributes;
}

static int Compare_MYON__UINT_PTR(const void* a, const void* b)
{
  MYON__UINT_PTR ai = *((const unsigned int*)a);
  MYON__UINT_PTR bi = *((const unsigned int*)b);
  if (ai < bi)
    return -1;
  if (ai > bi)
    return 1;
  return 0;
}

static int ComparePointerArrays(
  size_t count,
  const MYON__UINT_PTR* a,
  const MYON__UINT_PTR* b
  )
{
  if (count <= 0)
    return 0;
  if (nullptr == a)
    return ((nullptr == b) ? 0 : -1);
  if (nullptr == b)
    return 1;
  if (1 == count)
    return Compare_MYON__UINT_PTR(a, b);

  unsigned int stack_buffer[128];
  unsigned int* adex
    = (2 * count <= sizeof(stack_buffer) / sizeof(stack_buffer[0]))
    ? stack_buffer
    : new (std::nothrow) unsigned int[2 * count];  
  if (nullptr == adex)
    return 0;
  
  unsigned int* bdex = adex + count;
  MYON_Sort(MYON::sort_algorithm::quick_sort, adex, a, count, sizeof(a[0]), Compare_MYON__UINT_PTR);
  MYON_Sort(MYON::sort_algorithm::quick_sort, bdex, b, count, sizeof(b[0]), Compare_MYON__UINT_PTR);
  
  int rc = 0;
  for (unsigned int i = 0; 0 == rc && i < count; i++)
  {
    rc = Compare_MYON__UINT_PTR(a + adex[i], b + bdex[i]);
  }

  if (adex != stack_buffer)
    delete[] adex;

  return rc;
}

int MYON_SubDVertex::CompareUnorderedEdges(
  const MYON_SubDVertex* a,
  const MYON_SubDVertex* b
  )
{
  if (nullptr == a)
    return ((nullptr == b) ? 0 : -1);
  if (nullptr == b)
    return 1;

  if (a->m_edge_count < b->m_edge_count)
    return -1;
  if (a->m_edge_count > b->m_edge_count)
    return 1;
  return ComparePointerArrays(a->m_edge_count, (const MYON__UINT_PTR*)a->m_edges, (const MYON__UINT_PTR*)b->m_edges);
}

int MYON_SubDVertex::CompareUnorderedFaces(
  const MYON_SubDVertex* a,
  const MYON_SubDVertex* b
  )
{
  if (nullptr == a)
    return ((nullptr == b) ? 0 : -1);
  if (nullptr == b)
    return 1;

  if (a->m_face_count < b->m_face_count)
    return -1;
  if (a->m_face_count > b->m_face_count)
    return 1;
  return ComparePointerArrays(a->m_face_count, (const MYON__UINT_PTR*)a->m_faces, (const MYON__UINT_PTR*)b->m_faces);
}

int MYON_SubDVertex::CompareUnorderedEdgesAndFaces(
  const MYON_SubDVertex* a,
  const MYON_SubDVertex* b
  )
{
  int rc = CompareUnorderedEdges(a, b);
  if (0 == rc)
    rc = CompareUnorderedFaces(a, b);
  return rc;
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdge
//

void MYON_SubDComponentBase::CopyBaseFrom(
  const MYON_SubDComponentBase* src,
  bool bCopySymmetrySetNext
  )
{
  if ( nullptr == src )
    src = &MYON_SubDComponentBase::Unset;

  *this = *src;
  m_subd_point1 = nullptr;
  Internal_ClearSurfacePointFlag();
  if (bCopySymmetrySetNext)
    m_symmetry_set_next = src->m_symmetry_set_next;
}

void MYON_SubDEdge::CopyFrom(
  const MYON_SubDEdge* src,
  bool bReverseEdge,
  bool bCopyVertexArray,
  bool bCopyFaceArray,
  bool bCopySymmetrySetNext
)
{
  if (nullptr == src)
    src = &MYON_SubDEdge::Empty;

  CopyBaseFrom(src, bCopySymmetrySetNext);

  m_next_edge = nullptr;
  
  m_edge_tag = src->m_edge_tag;

  m_sharpness = src->m_sharpness;

  unsigned int end0 = bReverseEdge ? 1 : 0;

  if (bCopyVertexArray)
  {
    m_vertex[0] = src->m_vertex[end0];
    m_vertex[1] = src->m_vertex[1 - end0];
  }

  m_sector_coefficient[0] = src->m_sector_coefficient[end0];
  m_sector_coefficient[1] = src->m_sector_coefficient[1 - end0];

  if (bCopyFaceArray)
  {
    if (src->m_face_count > 0 && (src->m_face_count <= 2 || (nullptr != src->m_facex && nullptr != m_facex)))
    {
      m_face2[0] = src->m_face2[0];
      m_face2[1] = src->m_face2[1];
      unsigned int face_count = src->m_face_count;
      if (face_count > 2)
      {
        face_count -= 2;
        for (unsigned int efi = 0; efi < face_count; efi++)
          m_facex[efi] = src->m_facex[efi];
      }
      m_face_count = src->m_face_count;
    }
    else
      m_face_count = 0;
  }
}

unsigned int MYON_SubDEdge::TaggedEndIndex() const
{
  unsigned int tagged_end_index = 3;
  for (unsigned int evi = 0; evi < 2; evi++)
  {
    const MYON_SubDVertex* v = m_vertex[evi];
    if (nullptr == v || false == v->IsDartOrCreaseOrCorner())
      continue;
    tagged_end_index = (3 == tagged_end_index) ? evi : 2;
  }
  return tagged_end_index;
}

const MYON_SubDFacePtr MYON_SubDEdge::FacePtr(
  unsigned int i
  ) const
{
  return (i < 2) ? m_face2[i] : ((i < m_face_count) ? m_facex[i - 2] : MYON_SubDFacePtr::Null);
  //return (i < m_face_count) ? ((i < 2) ? m_face2[i] : m_facex[i - 2]) : MYON_SubDFacePtr::Null;
}

unsigned MYON_SubDEdge::VertexCount() const
{
  return 
    (nullptr != m_vertex[0]) 
    ? ((nullptr != m_vertex[1] && m_vertex[0] != m_vertex[1]) ? 2U : 1U)
    : ((nullptr != m_vertex[1] ? 1U : 0U))
    ;
}

unsigned int MYON_SubDEdge::VertexId(
  unsigned evi
) const
{
  const MYON_SubDVertex* v = Vertex(evi);
  return (nullptr != v) ? v->m_id : 0U;
}

const MYON_SubDVertex* MYON_SubDEdge::Vertex(
  unsigned evi
) const
{
  return (evi >= 0 && evi <= 1) ? m_vertex[evi] : nullptr;
}


unsigned int MYON_SubDEdge::FaceCount() const
{
  return m_face_count;
}

bool MYON_SubDEdge::HasBoundaryEdgeTopology() const
{
  for (;;)
  {
    if (1 != m_face_count)
      break;
    const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(m_face2[0].m_ptr);
    if (nullptr == f)
      break;
    const MYON_SubDEdgePtr feptr = f->EdgePtrFromEdge(this);
    if (this != MYON_SUBD_EDGE_POINTER(feptr.m_ptr))
    {
      MYON_SUBD_ERROR("m_face2[0] does not reference this edge.");
      break;
    }
    if (MYON_SUBD_FACE_DIRECTION(m_face2[0].m_ptr) != MYON_SUBD_FACE_DIRECTION(feptr.m_ptr))
    {
      MYON_SUBD_ERROR("m_face2[0] has inconsistent direction flags.");
      break;
    }
    if (nullptr == m_vertex[0] || nullptr == m_vertex[1] || m_vertex[0] == m_vertex[1])
    {
      MYON_SUBD_ERROR("m_vertex[] has null or invalid pointers.");
      break;
    }
    return true;
  }
  return false;
}

bool MYON_SubDEdge::HasInteriorEdgeTopology(bool bRequireOppositeFaceDirections) const
{
  for (;;)
  {
    if (2 != m_face_count)
      break;
    if (bRequireOppositeFaceDirections && MYON_SUBD_FACE_DIRECTION(m_face2[0].m_ptr) == MYON_SUBD_FACE_DIRECTION(m_face2[1].m_ptr))
      break;
    const MYON_SubDFace* f[2] = { MYON_SUBD_FACE_POINTER(m_face2[0].m_ptr), MYON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
    if ( nullptr == f[0] || nullptr == f[1] || f[0] == f[1])
      break;
    const MYON_SubDEdgePtr feptr[2] = { f[0]->EdgePtrFromEdge(this), f[1]->EdgePtrFromEdge(this) };
    if (this != MYON_SUBD_EDGE_POINTER(feptr[0].m_ptr))
    {
      MYON_SUBD_ERROR("m_face2[0] does not reference this edge.");
      break;
    }
    if (MYON_SUBD_FACE_DIRECTION(feptr[0].m_ptr) != MYON_SUBD_FACE_DIRECTION(m_face2[0].m_ptr))
    {
      MYON_SUBD_ERROR("m_face2[0] has inconsistent direction flags.");
      break;
    }
    if (this != MYON_SUBD_EDGE_POINTER(feptr[1].m_ptr))
    {
      MYON_SUBD_ERROR("m_face2[1] does not reference this edge.");
      break;
    }
    if (MYON_SUBD_FACE_DIRECTION(feptr[1].m_ptr) != MYON_SUBD_FACE_DIRECTION(m_face2[1].m_ptr))
    {
      MYON_SUBD_ERROR("m_face2[1] has inconsistent direction flags.");
      break;
    }
    if (nullptr == m_vertex[0] || nullptr == m_vertex[1] || m_vertex[0] == m_vertex[1])
    {
      MYON_SUBD_ERROR("m_vertex[] has null or invalid pointers.");
      break;
    }
    return true;
  }
  return false;
}

const class MYON_SubDFace* MYON_SubDEdge::Face(
  unsigned int i
  ) const
{
  return (i < 2) ? MYON_SUBD_FACE_POINTER(m_face2[i].m_ptr) : ((i < m_face_count) ? MYON_SUBD_FACE_POINTER(m_facex[i - 2].m_ptr) : nullptr);
}

MYON__UINT_PTR MYON_SubDEdge::FaceDirection(
  unsigned int i
  ) const
{
  return (i < 2) ? MYON_SUBD_FACE_DIRECTION(m_face2[i].m_ptr) : ((i < m_face_count) ? MYON_SUBD_FACE_DIRECTION(m_facex[i - 2].m_ptr) : 0);
}

const MYON_SubDFacePtr MYON_SubDEdge::FacePtrFromFace(
  const class MYON_SubDFace* f
) const
{
  if (nullptr != f)
  {
    const MYON_SubDFacePtr* fptr = m_face2;
    const unsigned int edge_face_count = m_face_count;
    for (unsigned int efi = 0; efi < edge_face_count; efi++, fptr++)
    {
      if (2 == efi)
      {
        fptr = m_facex;
        if (nullptr == fptr)
          break;
      }
      if (fptr->Face() == f)
        return *fptr;
    }
  }
  return MYON_SubDFacePtr::Null;
}


unsigned int MYON_SubDEdge::FaceArrayIndex(
  const MYON_SubDFace* f
  ) const
{
  if (nullptr == f)
    return MYON_UNSET_UINT_INDEX;
  const unsigned int face_count = m_face_count;
  if (face_count > 0)
  {
    if (f == MYON_SUBD_FACE_POINTER(m_face2[0].m_ptr))
      return 0;
    if (face_count > 1)
    {
      if (f == MYON_SUBD_FACE_POINTER(m_face2[1].m_ptr))
        return 1;
      if (face_count > 2 && nullptr != m_facex)
      {
        const MYON_SubDFacePtr* fptr = m_facex - 2;
        for (unsigned int efi = 2; efi < face_count; efi++)
        {
          if (f == MYON_SUBD_FACE_POINTER(fptr[efi].m_ptr))
            return efi;
        }
      }
    }
  }
  return MYON_UNSET_UINT_INDEX;
}

unsigned int MYON_SubDEdge::ReplaceFaceInArray(const MYON_SubDFace * old_face, const MYON_SubDFace * new_face)
{
  unsigned efi = (nullptr != old_face && old_face != new_face) ? FaceArrayIndex(old_face) : MYON_UNSET_UINT_INDEX;
  if (MYON_UNSET_UINT_INDEX == efi)
    return MYON_UNSET_UINT_INDEX;
  MYON_SubDFacePtr* fptr = (efi < 2) ? &m_face2[efi] : &m_facex[efi - 2];
  if (nullptr != new_face)
  {
    *fptr = MYON_SubDFacePtr::Create(new_face, fptr->FaceDirection());
  }
  else
  {    
    unsigned efi1 = efi + 1;
    MYON_SubDFacePtr* fptr1 = (efi1 < 2) ? &m_face2[efi1] : &m_facex[efi1 - 2];
    for (const unsigned c = (unsigned)(m_face_count--); efi1 < c; ++efi, ++efi1)
    {
      if (2 == efi)
        fptr = m_facex;
      else if (2 == efi1)
        fptr1 = m_facex;
      *fptr++ = *fptr1++;
    }
  }
  return efi;
}



unsigned int MYON_SubDEdge::VertexArrayIndex(const MYON_SubDVertex * v) const
{
  if (nullptr == v || m_vertex[0] == m_vertex[1])
    return MYON_UNSET_UINT_INDEX;
  if (v == m_vertex[0])
    return 0;
  if (v == m_vertex[1])
    return 1;
  return MYON_UNSET_UINT_INDEX;
}

const MYON_SubDFace* MYON_SubDEdgePtr::NeighborFace(
  const MYON_SubDFace* face,
  bool bStopAtCrease
) const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->NeighborFace(face, bStopAtCrease) : nullptr;
}

const MYON_SubDFace* MYON_SubDEdge::NeighborFace(
  const MYON_SubDFace* face,
    bool bStopAtCrease
  ) const
{
  if ( nullptr == face || 2 != m_face_count )
    return nullptr;
  // Do not stop at x tags
  if (bStopAtCrease && MYON_SubDEdgeTag::Crease == m_edge_tag)
    return nullptr;
  const MYON_SubDFace* f[2] = { MYON_SUBD_FACE_POINTER(m_face2[0].m_ptr), MYON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
  if (nullptr == f[0] || nullptr == f[1] )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (face == f[0])
  {
    if (face == f[1] )
      return MYON_SUBD_RETURN_ERROR(nullptr);
    return f[1];
  }
  if (face == f[1])
  {
    return f[0];
  }
  return MYON_SUBD_RETURN_ERROR(nullptr);
}

const MYON_SubDFacePtr MYON_SubDEdge::NeighborFacePtr(
  const MYON_SubDFace* face,
    bool bStopAtCrease
  ) const
{
  if ( nullptr == face || 2 != m_face_count )
    return MYON_SubDFacePtr::Null;
  // Do not stop at x tags
  if (bStopAtCrease && MYON_SubDEdgeTag::Crease == m_edge_tag)
    return MYON_SubDFacePtr::Null;
  const MYON_SubDFace* f[2] = { MYON_SUBD_FACE_POINTER(m_face2[0].m_ptr), MYON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
  if (nullptr == f[0] || nullptr == f[1] )
    return MYON_SUBD_RETURN_ERROR(MYON_SubDFacePtr::Null);
  if (face == f[0])
  {
    if (face == f[1] )
      return MYON_SUBD_RETURN_ERROR(MYON_SubDFacePtr::Null);
    return m_face2[1];
  }
  if (face == f[1])
  {
    return m_face2[0];
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDFacePtr::Null);
}

const MYON_SubDEdgePtr MYON_SubDEdge::AdjacentEdgePtr(
  unsigned int edge_vertex_index,
  unsigned int i
) const
{
  if ( edge_vertex_index >= 2 )
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  const MYON_SubDFacePtr fptr = FacePtr(i);
  const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(fptr.m_ptr);
  if (nullptr == f)
    return MYON_SubDEdgePtr::Null;
  const unsigned int fe_count = f->m_edge_count;
  if ( fe_count < 3 || fe_count > MYON_SubDFace::MaximumEdgeCount)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  const unsigned int fei = f->EdgeArrayIndex(this);
  if( fei >= fe_count)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  const MYON_SubDEdgePtr eptr = f->EdgePtr(fei);
  if ( this != MYON_SUBD_EDGE_POINTER(eptr.m_ptr))
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  const MYON__UINT_PTR dir = MYON_SUBD_FACE_DIRECTION(fptr.m_ptr);
  if (dir != MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr))
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  const unsigned int fei1
    = (dir == ((MYON__UINT_PTR)edge_vertex_index))
    ? ((fei + fe_count - 1) % fe_count)
    : ((fei + 1) % fe_count)
    ;
  return f->EdgePtr(fei1);
}

const MYON_SubDEdge* MYON_SubDEdge::AdjacentEdge(
  unsigned int edge_vertex_index,
  unsigned int i
  ) const
{
  return MYON_SUBD_EDGE_POINTER(AdjacentEdgePtr(edge_vertex_index, i).m_ptr);
}

const MYON_SubDVertex* MYON_SubDEdge::OtherEndVertex(
  const MYON_SubDVertex* vertex
  ) const
{
  if (nullptr != vertex)
  {
    if (m_vertex[0] == vertex)
    {
      if (m_vertex[1] != vertex )
        return m_vertex[1];
    }
    else if (m_vertex[1] == vertex )
      return m_vertex[0];
  }
  return nullptr;
}

const MYON_SubDEdgePtr MYON_SubDEdge::FromVertices(
  const MYON_SubDVertex* vertex0,
  const MYON_SubDVertex* vertex1
)
{
  if (nullptr != vertex0 && nullptr != vertex1 && vertex0 != vertex1 && nullptr != vertex0->m_edges)
  {
    for (unsigned short vei = 0; vei < vertex0->m_edge_count; vei++)
    {
      const MYON_SubDEdgePtr eptr = vertex0->m_edges[vei];
      if (vertex1 == eptr.RelativeVertex(1) && vertex0 == eptr.RelativeVertex(0))
        return eptr;
    }
  }
  return MYON_SubDEdgePtr::Null;
}

const MYON_SubDEdge* MYON_SubDEdge::FromVertices(
  const MYON_SubDVertex* vertex0,
  const MYON_SubDVertex* vertex1,
  bool bIgnoreOrientation
)
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(MYON_SubDEdge::FromVertices(vertex0, vertex1).m_ptr);
  if ( nullptr != e && false == bIgnoreOrientation && vertex0 != e->m_vertex[0] )
    e = nullptr;
  return e;
}

const MYON_SubDFacePtr MYON_SubDFace::FromVertices(const MYON_SimpleArray< const MYON_SubDVertex* >& vertex_list)
{
  return MYON_SubDFace::FromVertices(vertex_list.Array(), vertex_list.UnsignedCount());
}

const MYON_SubDFacePtr MYON_SubDFace::FromVertices(const MYON_SubDVertex* const* vertex_list, size_t vertex_count)
{
  if (nullptr == vertex_list || vertex_count < 3 || vertex_count > MYON_SubDFace::MaximumEdgeCount)
    return MYON_SubDFacePtr::Null;

  const MYON_SubDFace* candiates4[4];
  const MYON_SubDFace** candidates = nullptr;
  unsigned candidate_count = 0;
  const MYON_SubDFace* f = nullptr;

  const unsigned unsigned_vertex_count = (unsigned)vertex_count;
  if (unsigned_vertex_count < 3)
    return MYON_SubDFacePtr::Null;
  const MYON_SubDVertex* v[2] = { nullptr,vertex_list[0] };
  for (unsigned fei = 0; fei < unsigned_vertex_count; ++fei)
  {
    v[0] = v[1];
    v[1] = vertex_list[(fei + 1) % unsigned_vertex_count];
    const MYON_SubDEdge* e = MYON_SubDEdge::FromVertices(v[0], v[1]).Edge();
    if (nullptr == e || e->m_face_count <= 0)
      candidate_count = 0;
    else if (0 == fei)
    {
      candidates = (e->m_face_count <= 4) ? candiates4 : ((const MYON_SubDFace**)onmalloc(e->m_face_count * sizeof(candidates[0])));
      for (unsigned short efi = 0; efi < e->m_face_count; ++efi)
      {
        const MYON_SubDFace* ef = e->Face(efi);
        if (nullptr != ef && unsigned_vertex_count == ef->EdgeCount())
          candidates[candidate_count++] = ef;
      }
    }
    else
    {
      unsigned c = 0;
      for (unsigned i = 0; i < candidate_count; ++i)
      {
        if (e->FaceArrayIndex(candidates[i]) < MYON_UNSET_UINT_INDEX)
          candidates[c++] = candidates[i];
      }
      candidate_count = c;
    }

    if (0 == candidate_count)
      break;
    if (1 == candidate_count)
    {
      f = candidates[0];
      break;
    }
  }

  if (nullptr != candidates && candidates != candiates4)
    onfree(candidates);

  if (nullptr == f)
    return MYON_SubDFacePtr::Null;

  const unsigned fvi0 = f->VertexIndex(vertex_list[0]);
  if (fvi0 >= unsigned_vertex_count)
    return MYON_SubDFacePtr::Null;

  const MYON__UINT_PTR dir = (vertex_list[1] == f->Vertex((fvi0 + 1) % unsigned_vertex_count)) ? 0 : 1;
  if (0 == dir)
  {
    for (unsigned fvi = 2; fvi < unsigned_vertex_count; fvi++)
    {
      if (vertex_list[fvi] != f->Vertex((fvi0 + fvi) % unsigned_vertex_count))
        return MYON_SubDFacePtr::Null;
    }
  }
  else
  {
    for (unsigned fvi = 1; fvi < unsigned_vertex_count; fvi++)
    {
      if (vertex_list[fvi] != f->Vertex((fvi0 + unsigned_vertex_count - fvi) % unsigned_vertex_count))
        return MYON_SubDFacePtr::Null;
    }
  }
  return MYON_SubDFacePtr::Create(f, dir);
}

const MYON_3dPoint MYON_SubDEdge::ControlNetPoint( unsigned int i) const
{
  if (i >= 2 || nullptr == m_vertex[i])
    return MYON_3dPoint::NanPoint;
  return (MYON_3dPoint(m_vertex[i]->m_P));
}

const MYON_Line MYON_SubDEdge::ControlNetLine() const
{
  return MYON_Line(ControlNetPoint(0), ControlNetPoint(1));
}

const MYON_3dVector MYON_SubDEdge::ControlNetDirection() const
{
  if (nullptr == m_vertex[0] || nullptr == m_vertex[1])
    return MYON_3dVector::NanVector;
  const MYON_3dPoint P[2] = { MYON_3dPoint(m_vertex[0]->m_P) ,MYON_3dPoint(m_vertex[1]->m_P) };
  return (P[0].IsValid() && P[1].IsValid()) ? (P[1] - P[0]) : MYON_3dVector::NanVector;
}

const MYON_3dVector MYON_SubDEdge::ControlNetDirectionFrom(
  const MYON_SubDVertex* v
) const
{
  if (nullptr != v)
  {
    if (v == m_vertex[0] && nullptr != m_vertex[1])
      return ControlNetDirection();
    if (v == m_vertex[1] && nullptr != m_vertex[0])
      return -ControlNetDirection();
  }
  return MYON_3dVector::NanVector;
}


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFace
//



//bool MYON_SubDFace::IsOrdinary(
//  MYON_SubD::SubDType subdivision_type,
//  bool bTestFaces
//  ) const
//{
//  unsigned int ordinary_face_edge_count = 0;
//  if (MYON_SubD::SubDType::QuadCatmullClark == subdivision_type)
//    ordinary_face_edge_count = 4;
//  else if (MYON_SubD::SubDType::TriLoopWarren == subdivision_type)
//    ordinary_face_edge_count = 3;
//  else
//    return false;
//
//  if (ordinary_face_edge_count != m_edge_count)
//    return false;
//
//  for (unsigned int fei = 0; fei < ordinary_face_edge_count; fei++)
//  {
//    MYON__UINT_PTR eptr = m_edge4[fei].m_ptr;
//    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr);
//    if (nullptr == e || 2 != e->m_face_count || MYON_SubDEdgeTag::Smooth != e->m_edge_tag)
//      return false;
//    MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(eptr);
//    const MYON_SubDVertex* v = e->m_vertex[edir];
//    if (nullptr == v || false == v->IsOrdinary(subdivision_type,MYON_SubDVertexTag::Unset,bTestFaces))
//      return false;
//  }
//  return true;
//}

void MYON_SubDFace::CopyFrom(
  const MYON_SubDFace* src,
  bool bCopyEdgeArray,
  bool bCopySymmetrySetNext
  )
{
  if (nullptr == src)
    src = &MYON_SubDFace::Empty;

  CopyBaseFrom(src, bCopySymmetrySetNext);

  m_next_face = nullptr;

  m_material_channel_index = src->m_material_channel_index;
  m_per_face_color = src->m_per_face_color;

  m_level_zero_face_id = src->m_level_zero_face_id;

  if (bCopyEdgeArray)
  {
    if (src->m_edge_count > 0 && (src->m_edge_count <= 4 || (nullptr != src->m_edgex && nullptr != m_edgex)))
    {
      m_edge4[0] = src->m_edge4[0];
      m_edge4[1] = src->m_edge4[1];
      m_edge4[2] = src->m_edge4[2];
      m_edge4[3] = src->m_edge4[3];
      unsigned int edge_count = src->m_edge_count;
      if (edge_count > 4)
      {
        edge_count -= 4;
        for (unsigned int fei = 0; fei < edge_count; fei++)
          m_edgex[fei] = src->m_edgex[fei];
      }
      m_edge_count = src->m_edge_count;
    }
    else
      m_edge_count = 0;
  }

  // RH-56133 need to copy packed coordinate information.
  m_pack_id = src->m_pack_id;
  m_pack_rect_origin[0] = src->m_pack_rect_origin[0];
  m_pack_rect_origin[1] = src->m_pack_rect_origin[1];
  m_pack_rect_size[0] = src->m_pack_rect_size[0];
  m_pack_rect_size[1] = src->m_pack_rect_size[1];
  m_pack_status_bits = src->m_pack_status_bits;

  // and need to copy texture coordinates
  m_texture_status_bits = src->m_texture_status_bits;
  const unsigned this_texture_point_capacity = this->TexturePointsCapacity();
  const unsigned texture_point_count = (src->TexturePointsAreSet() && this_texture_point_capacity  >= this->EdgeCount()) ? this->EdgeCount() : 0;
  if (texture_point_count >= 3)
  {
    // copy texture points
    for (unsigned i = 0; i < texture_point_count; ++i)
      m_texture_points[i] = src->m_texture_points[i];
    const unsigned texture_point_capacity = this->TexturePointsCapacity();
    for (unsigned i = texture_point_count; i < texture_point_capacity; ++i)
      m_texture_points[i] = MYON_3dPoint::NanPoint;
    this->m_texture_status_bits |= MYON_SubDFace::TextureStatusBits::TexturePointsSet;
  }
  else
  {
    // whatever created the face failed to allocate texture point memory.
    this->m_texture_status_bits &= MYON_SubDFace::TextureStatusBits::NotTexturePointsBitsMask;
  }
}

const MYON_SubDEdgePtr MYON_SubDFace::EdgePtr(
  unsigned int i
  ) const
{
  return (i < 4) ? m_edge4[i] : ((i < m_edge_count) ? m_edgex[i-4] : MYON_SubDEdgePtr::Null);
}

unsigned int MYON_SubDFace::EdgeCount() const
{
  return m_edge_count;
}


bool MYON_SubDFace::HasEdges() const
{
  if (m_edge_count < 3 || m_edge_count > MYON_SubDFace::MaximumEdgeCount)
    return false;
  if (m_edge_count > 4 + m_edgex_capacity)
    return false;
  const MYON_SubDEdgePtr* eptr = m_edge4;
  const MYON_SubDVertex* v0 = nullptr;
  const MYON_SubDVertex* v1 = nullptr;
  const MYON_SubDVertex* ev[2];
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        return false;
      if (m_edge_count > 4 + m_edgex_capacity)
        return false;
    }
    const MYON__UINT_PTR ptr = eptr->m_ptr;
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(ptr);
    if (nullptr == e)
      return false;
    if (0 == e->m_face_count)
      return false;
    if (e->m_face_count > 2 + e->m_facex_capacity)
      return false;
    if (0 == MYON_SUBD_EDGE_DIRECTION(ptr))
    {
      ev[0] = e->m_vertex[0];
      ev[1] = e->m_vertex[1];
    }
    else
    {
      ev[0] = e->m_vertex[1];
      ev[1] = e->m_vertex[0];
    }
    if (nullptr == ev[0] || nullptr == ev[1] || ev[0] == ev[1])
      return false;
    if (nullptr == v0)
      v0 = ev[0];
    else if (v1 != ev[0])
      return false;
    v1 = ev[1];
    if (v1->m_edge_count < 2 || v1->m_edge_count > v1->m_edge_capacity)
      return false;
    if (v1->m_face_count < 1 || v1->m_face_count > v1->m_face_capacity)
      return false;
  }
  if ( v0 != v1)
    return false;

  return true;
}

unsigned int MYON_SubDFace::MarkedEdgeCount() const
{
  unsigned int marked_edge_count = 0;
  const MYON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != e && e->m_status.RuntimeMark())
      ++marked_edge_count;
  }
  return marked_edge_count;
}


unsigned int MYON_SubDFace::SetEdgeMarks(bool bMark) const
{
  bMark = bMark ? true : false; // so exact compare can be used
  unsigned int changed_mark_count = 0;
  const MYON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != e && bMark != e->m_status.RuntimeMark())
    {
      e->m_status.SetRuntimeMark(bMark);
      ++changed_mark_count;
    }
  }
  return changed_mark_count;
}

unsigned int MYON_SubDFace::GetEdgeArray(
  MYON_SimpleArray< MYON_SubDEdgePtr >& face_edge_array
) const
{
  face_edge_array.SetCount(0);
  const unsigned edge_count = m_edge_count;
  face_edge_array.Reserve(edge_count);
  face_edge_array.Append(edge_count <= 4 ? edge_count : 4U, m_edge4);
  if (edge_count > 4)
  {
    if ( nullptr != m_edgex )
     face_edge_array.Append(edge_count-4U, m_edgex);
    else
    {
      for (unsigned fei = 4; fei < edge_count; ++fei)
        face_edge_array.Append(MYON_SubDEdgePtr::Null);
    }
  }
  return edge_count;
}

unsigned int MYON_SubDFace::SetVertexMarks(bool bMark) const
{
  bMark = bMark ? true : false; // so exact compare can be used
  unsigned int changed_mark_count = 0;
  const MYON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const MYON_SubDVertex* v = eptr->RelativeVertex(0);
    if (nullptr != v && bMark != v->m_status.RuntimeMark())
    {
      v->m_status.SetRuntimeMark(bMark);
      ++changed_mark_count;
    }
  }
  return changed_mark_count;
}

unsigned int MYON_SubDFace::MarkedVertexCount() const
{
  unsigned int marked_vertex_count = 0;
  const MYON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned short fei = 0; fei < m_edge_count; ++fei, ++ eptr)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    const MYON_SubDVertex* v = eptr->RelativeVertex(0);
    if (nullptr != v && v->m_status.RuntimeMark())
      ++marked_vertex_count;
  }
  return marked_vertex_count;
}

bool MYON_SubDFace::EdgeMark(
  unsigned int i,
  bool bMissingEdgeReturnValue
) const
{
  const MYON_SubDEdge* e = Edge(i);
  return (nullptr != e) ? e->Mark() : bMissingEdgeReturnValue;
}

MYON__UINT8 MYON_SubDFace::EdgeMarkBits(
  unsigned int i,
  MYON__UINT8 missing_edge_markbits
) const
{
  const MYON_SubDEdge* e = Edge(i);
  return (nullptr != e) ? e->MarkBits() : missing_edge_markbits;
}

bool MYON_SubDFace::VertexMark(
  unsigned int i,
  bool bMissingVertexReturnValue
) const
{
  const MYON_SubDVertex* v = Vertex(i);
  return (nullptr != v) ? v->Mark() : bMissingVertexReturnValue;
}

MYON__UINT8 MYON_SubDFace::VertexMarkBits(
  unsigned int i,
  MYON__UINT8 missing_vertex_markbits
) const
{
  const MYON_SubDVertex* v = Vertex(i);
  return (nullptr != v) ? v->MarkBits() : missing_vertex_markbits;
}

const class MYON_SubDVertex* MYON_SubDFace::Vertex(
  unsigned int i
  ) const
{
  MYON_SubDEdge* e;
  const MYON__UINT_PTR edge_ptr = (i < 4) ? m_edge4[i].m_ptr : ((i < m_edge_count) ? m_edgex[i - 4].m_ptr : 0);
  return (nullptr != (e = MYON_SUBD_EDGE_POINTER(edge_ptr))) ? e->m_vertex[MYON_SUBD_EDGE_DIRECTION(edge_ptr)] : nullptr;
}

const MYON_3dPoint MYON_SubDFace::ControlNetPoint(unsigned int i) const
{
  const MYON_SubDEdge* e;
  const MYON__UINT_PTR edge_ptr = (i < 4) ? m_edge4[i].m_ptr : ((i < m_edge_count) ? m_edgex[i - 4].m_ptr : 0);
  const MYON_SubDVertex* v = (nullptr != (e = MYON_SUBD_EDGE_POINTER(edge_ptr))) ? e->m_vertex[MYON_SUBD_EDGE_DIRECTION(edge_ptr)] : nullptr;
  return (nullptr != v) ? MYON_3dPoint(v->m_P) : MYON_3dPoint::NanPoint;
}

const MYON_SubDVertex* MYON_SubDFace::QuadOppositeVertex(
  const MYON_SubDVertex* vertex
  ) const
{
  if ( nullptr == vertex )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( 4 != m_edge_count)
    return nullptr; // not an error
  
  MYON__UINT_PTR ptr0 = m_edge4[0].m_ptr;
  const MYON_SubDEdge* e0 = MYON_SUBD_EDGE_POINTER(ptr0);
  if ( nullptr == e0 )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  ptr0 = MYON_SUBD_EDGE_DIRECTION(ptr0);
  
  MYON__UINT_PTR ptr2 = m_edge4[2].m_ptr;
  const MYON_SubDEdge* e2 = MYON_SUBD_EDGE_POINTER(ptr2);
  if ( nullptr == e2 )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  ptr2 = MYON_SUBD_EDGE_DIRECTION(ptr2);

  if (vertex == e0->m_vertex[ptr0])
    return e2->m_vertex[ptr2];

  if (vertex == e0->m_vertex[1-ptr0])
    return e2->m_vertex[1-ptr2];

  if (vertex == e2->m_vertex[ptr2])
    return e0->m_vertex[ptr0];

  if (vertex == e2->m_vertex[1-ptr2])
    return e0->m_vertex[1-ptr0];

  return MYON_SUBD_RETURN_ERROR(nullptr);
}

const MYON_SubDEdge* MYON_SubDFace::QuadOppositeEdge(
  const MYON_SubDEdge* edge
  ) const
{
  if ( nullptr == edge )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( 4 != m_edge_count)
    return nullptr; // not an error
  
  for (unsigned int fei = 0; fei < 4; fei++)
  {
    const MYON_SubDEdge* e0 = MYON_SUBD_EDGE_POINTER(m_edge4[fei].m_ptr);
    if (nullptr == e0)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    if (e0 == edge)
    {
      e0 = MYON_SUBD_EDGE_POINTER(m_edge4[(fei + 2) % 4].m_ptr);
      if (nullptr == e0)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      return e0;
    }
  }

  return MYON_SUBD_RETURN_ERROR(nullptr);
}


const class MYON_SubDEdge* MYON_SubDFace::Edge(
  unsigned int i
  ) const
{
  return (i < 4) ? MYON_SUBD_EDGE_POINTER(m_edge4[i].m_ptr) : ((i < m_edge_count) ? MYON_SUBD_EDGE_POINTER(m_edgex[i - 4].m_ptr) : nullptr);
}

MYON__UINT_PTR MYON_SubDFace::EdgeDirection(
  unsigned int i
  ) const
{
  return (i < 4) ? MYON_SUBD_EDGE_DIRECTION(m_edge4[i].m_ptr) : ((i < m_edge_count) ? MYON_SUBD_EDGE_DIRECTION(m_edgex[i - 4].m_ptr) : 0);
}


const MYON_SubDEdgePtr MYON_SubDFace::EdgePtrFromEdge(
  const class MYON_SubDEdge* e
) const
{
  if (nullptr != e)
  {
    const MYON_SubDEdgePtr* eptr = m_edge4;
    const unsigned int face_edge_count = m_edge_count;
    for (unsigned int fei = 0; fei < face_edge_count; fei++, eptr++)
    {
      if (4 == fei)
      {
        eptr = m_edgex;
        if (nullptr == eptr)
          break;
      }
      if (e == MYON_SUBD_EDGE_POINTER(eptr->m_ptr))
        return *eptr;
    }
  }

  return MYON_SubDEdgePtr::Null;
}


unsigned int MYON_SubDFace::EdgeArrayIndex(
  const MYON_SubDEdge* e
  ) const
{
  if (nullptr != e)
  {
    const MYON_SubDEdgePtr* eptr = m_edge4;
    const unsigned int face_edge_count = m_edge_count;
    for (unsigned int fei = 0; fei < face_edge_count; fei++, eptr++)
    {
      if (4 == fei)
      {
        eptr = m_edgex;
        if (nullptr == eptr)
          break;
      }
      if (e == MYON_SUBD_EDGE_POINTER(eptr->m_ptr))
        return fei;
    }
  }

  return MYON_UNSET_UINT_INDEX;
}


const MYON_SubDEdge* MYON_SubDFace::PrevEdge(
  const MYON_SubDEdge* edge
  ) const
{
  unsigned int edge_index = EdgeArrayIndex(edge);
  if (MYON_UNSET_UINT_INDEX == edge_index)
    return nullptr;
  const unsigned int edge_count = m_edge_count;
  edge_index = (edge_index + (edge_count - 1)) % edge_count;
  return Edge(edge_index);
}

const MYON_SubDEdge* MYON_SubDFace::NextEdge(
  const MYON_SubDEdge* edge
  ) const
{
  unsigned int edge_index = EdgeArrayIndex(edge);
  if (MYON_UNSET_UINT_INDEX == edge_index)
    return nullptr;
  edge_index = (edge_index + 1) % ((unsigned int)m_edge_count);
  return Edge(edge_index);
}

unsigned int MYON_SubDFace::PrevEdgeArrayIndex(
  unsigned int edge_array_index
  ) const
{
  const unsigned int edge_count = m_edge_count;
  return (edge_array_index < edge_count) ? ((edge_array_index + edge_count - 1) % edge_count) : MYON_UNSET_UINT_INDEX;
}

unsigned int MYON_SubDFace::NextEdgeArrayIndex(
  unsigned int edge_array_index
  ) const
{
  const unsigned int edge_count = m_edge_count;
  return (edge_array_index < edge_count) ? ((edge_array_index +  1) % edge_count) : MYON_UNSET_UINT_INDEX;
}

bool MYON_SubDVertex::RemoveEdgeFromArray(const MYON_SubDEdge * e)
{
  if (nullptr == e || 0 == m_edge_count || nullptr == m_edges)
    return MYON_SUBD_RETURN_ERROR(false);
  unsigned short new_count = 0;
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    const MYON_SubDEdgePtr eptr = m_edges[vei];
    if (e == MYON_SUBD_EDGE_POINTER(eptr.m_ptr))
      continue;
    m_edges[new_count++] = eptr;
  }
  if (new_count == m_edge_count)
    return false;
  m_edge_count = new_count;
  return true;
}

bool MYON_SubDVertex::RemoveFaceFromArray(const MYON_SubDFace * f)
{
  if (nullptr == f || 0 == m_face_count || nullptr == m_faces)
    return MYON_SUBD_RETURN_ERROR(false);
  unsigned short new_count = 0;
  for (unsigned short vfi = 0; vfi < m_face_count; ++vfi)
  {
    const MYON_SubDFace* vf = m_faces[vfi];
    if (f == vf)
      continue;
    m_faces[new_count++] = vf;
  }
  if (new_count == m_face_count)
    return false;
  m_face_count = new_count;
  return true;
}

bool MYON_SubD::RemoveEdgeVertexConnection(
  MYON_SubDEdge* e,
  MYON_SubDVertex* v
)
{
  if (nullptr == e || nullptr == v)
    return false;
  if (v == e->m_vertex[0])
    e->m_vertex[0] = nullptr;
  if (v == e->m_vertex[1])
    e->m_vertex[1] = nullptr;
  return v->RemoveEdgeFromArray(e);
}

MYON_SubDVertex* MYON_SubD::RemoveEdgeVertexConnection(
  MYON_SubDEdge* e,
  unsigned evi
)
{
  if (nullptr == e)
    return nullptr;
  MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>((nullptr != e && evi >= 0 && evi <= 1) ? e->m_vertex[evi] : nullptr);
  return RemoveEdgeVertexConnection(e, v) ? v : nullptr;
}

MYON_SubDVertexTag MYON_SubDVertex::SuggestedVertexTag(
  bool bApplyInputTagBias,
  bool bReturnBestGuessWhenInvalid
) const
{
  unsigned wire_count = 0;
  unsigned boundary_count = 0;
  unsigned interior_count = 0;

  unsigned crease_count = 0;

  const unsigned edge_count = (nullptr != m_edges ? m_edge_count : 0U);
  if ( edge_count < 2)
    return MYON_SubDVertexTag::Corner;

  for (unsigned vei = 0; vei < edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr == e)
      continue;
    switch(e->m_face_count)
    {
    case 0:
      ++wire_count;
      ++crease_count;
      break;
    case 1:
      ++boundary_count;
      ++crease_count;
      break;
    case 2:
      ++interior_count;
      if (MYON_SubDEdgeTag::Crease == e->m_edge_tag)
        ++crease_count;
      break;
    default: // nonmanifold edge
      return MYON_SubDVertexTag::Corner;
      break;
    }
  }

  if (crease_count >= 3)
    return MYON_SubDVertexTag::Corner;

  if (wire_count > 0)
  {
    if (2 == wire_count && 0 == boundary_count && 0 == interior_count)
      return (bApplyInputTagBias && MYON_SubDVertexTag::Corner == m_vertex_tag) ? MYON_SubDVertexTag::Corner : MYON_SubDVertexTag::Crease;
    return MYON_SubDVertexTag::Corner;
  }

  MYON_SubDVertexTag best_guess_tag = MYON_SubDVertexTag::Unset;
  // crease_count >= 3 handled above
  switch (crease_count)
  {
  case 0:
    if (interior_count >= 2)
      return MYON_SubDVertexTag::Smooth;
    if (bReturnBestGuessWhenInvalid)
    {
      // can occure when there is a nullptr edge
      best_guess_tag = MYON_SubDVertexTag::Smooth;
    }
    break;
  case 1:
    if (0 == boundary_count && interior_count >= 2)
      return MYON_SubDVertexTag::Dart;
    if (bReturnBestGuessWhenInvalid)
    {
      // topology is far from valid and dart evaluation is very delicate. 
      // We need more boundary edges, but using corner will at least give a well defined vertex surface point.
      best_guess_tag = MYON_SubDVertexTag::Corner;
    }
    break;
  case 2:
    if(  (0 == boundary_count && interior_count >= 2) || (2 == boundary_count) )
      return (bApplyInputTagBias && MYON_SubDVertexTag::Corner == m_vertex_tag) ? MYON_SubDVertexTag::Corner : MYON_SubDVertexTag::Crease;
    if (bReturnBestGuessWhenInvalid)
    {
      // topology is far from valid and dart evaluation is very delicate. 
      // We need more boundary edges, but using corner will at least give a well defined vertex surface point.
      best_guess_tag = MYON_SubDVertexTag::Corner;
    }
    break;
  }

  return best_guess_tag;
}


bool MYON_SubDEdge::RemoveFaceFromArray(
  const MYON_SubDFace* f
  )
{
  unsigned int i;
  if (nullptr == f)
    return false;

  if (m_face_count <= 2)
  {
    for (i = 0; i < m_face_count; i++)
    {
      if (f == MYON_SUBD_FACE_POINTER(m_face2[i].m_ptr))
      {
        for (i++; i < m_face_count; i++)
          m_face2[i - 1] = m_face2[i];
        m_face_count--;
        return true;
      }
    }
  }
  else
  {
    for (i = 0; i < 2; i++)
    {
      if (f == MYON_SUBD_FACE_POINTER(m_face2[i].m_ptr))
      {
        for (i++; i < 2; i++)
          m_face2[i - 1] = m_face2[i];
        m_face2[1] = m_facex[0];
        for (i = 3; i < m_face_count; i++)
          m_facex[i - 3] = m_facex[i - 2];
        m_face_count--;
        return true;
      }
    }
    for (i = 2; i < m_face_count; i++)
    {
      if (f == MYON_SUBD_FACE_POINTER(m_facex[i - 2].m_ptr))
      {
        for (i++; i < m_face_count; i++)
          m_facex[i - 3] = m_facex[i - 2];
        m_face_count--;
        return true;
      }
    }
  }

  return false;
}

bool MYON_SubDEdge::AddFaceToArray(
  MYON_SubDFacePtr face_ptr
)
{
  if (m_face_count < 2)
    m_face2[m_face_count] = face_ptr;
  else if (nullptr != m_facex && m_face_count < 2 + m_facex_capacity)
    m_facex[m_face_count - 2] = face_ptr;
  else
  {
    // not enough room in m_facex.
    // If you really are trying to make a non-manifold subd, 
    // then use MYON_SubD::GrowEdgeFaceArray().
    return MYON_SUBD_RETURN_ERROR(false);
  }
  m_face_count++;
  return true;
}

bool MYON_SubDEdge::RemoveFaceFromArray(
  unsigned int i,
  MYON_SubDFacePtr& removed_face
  )
{
  removed_face = MYON_SubDFacePtr::Null;
  unsigned int count = m_face_count;
  if ( i >= count )
    return MYON_SUBD_RETURN_ERROR(false);
  if (i < 2)
  {
    removed_face = m_face2[i];
  }
  if (count > 2)
  {
    if ( nullptr == m_facex || m_facex_capacity + ((unsigned short)2) < m_face_count )
      return MYON_SUBD_RETURN_ERROR(false);
    if ( i >= 2 )
      removed_face = m_facex[i-2];
  }

  unsigned int j = i+1;

  while (j < 2 && j < count )
    m_face2[i++] = m_face2[j++];

  if (count > 2)
  {
    m_face2[1] = m_facex[0];
    i = 0;
    j = 1;
    count -= 2;
    while (j < count )
      m_facex[i++] = m_facex[j++];
  }

  m_face_count--;

  return true;

}

bool MYON_SubDFace::ReplaceEdgeInArray(
  unsigned int fei0,
  MYON_SubDEdge* edge_to_remove,
  MYON_SubDEdge* edge_to_insert
)
{
  const unsigned int face_edge_count = m_edge_count;
  MYON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned int fei = 0; fei < face_edge_count; fei++, eptr++)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    if (fei >= fei0 && edge_to_remove == eptr->Edge() )
    {
      const MYON__UINT_PTR edir = eptr->EdgeDirection();
      *eptr = MYON_SubDEdgePtr::Create(edge_to_insert,edir);
      return true;
    }
  }
  return false;
}

bool MYON_SubDFace::ReplaceEdgeInArray(
  unsigned int fei0,
  MYON_SubDEdge* edge_to_remove,
  MYON_SubDEdgePtr edgeptr_to_insert
)
{
  const unsigned int face_edge_count = m_edge_count;
  MYON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned int fei = 0; fei < face_edge_count; fei++, eptr++)
  {
    if (4 == fei)
    {
      eptr = m_edgex;
      if (nullptr == eptr)
        break;
    }
    if (fei >= fei0 && edge_to_remove == eptr->Edge() )
    {
      *eptr = edgeptr_to_insert;
      return true;
    }
  }
  return false;
}


bool MYON_SubDFace::RemoveEdgeFromArray(
  unsigned int i,
  MYON_SubDEdgePtr& removed_edge
  )
{
  removed_edge = MYON_SubDEdgePtr::Null;
  unsigned int count = m_edge_count;
  if ( i >= count )
    return MYON_SUBD_RETURN_ERROR(false);
  if (i < 4)
  {
    removed_edge = m_edge4[i];
  }
  if (count > 4)
  {
    if ( nullptr == m_edgex || m_edgex_capacity + ((unsigned short)4) < m_edge_count )
      return MYON_SUBD_RETURN_ERROR(false);
    if ( i >= 4 )
      removed_edge = m_edgex[i-4];
  }

  unsigned int j = i+1;

  while (j < count)
  {
    const MYON_SubDEdgePtr& edge_j = j < 4 ? m_edge4[j] : m_edgex[j - 4];
    if (i < 4)
      m_edge4[i] = edge_j;
    else
      m_edgex[i - 4] = edge_j;
    i++;
    j++;
  }

  m_edge_count--;

  return true;
}

bool MYON_SubDFace::RotateEdgeArray(
  unsigned int fei0
)
{
  if (0 == fei0)
    return true;

  const unsigned int edge_count = m_edge_count;
  if (edge_count < 2 || edge_count > MYON_SubDFace::MaximumEdgeCount || fei0 >= edge_count)
    return false;

  MYON_SubDEdgePtr stack_eptr[8];
  MYON_SubDEdgePtr* eptr 
    = (edge_count*sizeof(stack_eptr[0]) > sizeof(stack_eptr)) 
    ? ((MYON_SubDEdgePtr*)onmalloc(edge_count * sizeof(eptr[0])))
    : stack_eptr;
  if (nullptr == eptr)
    return false;

  MYON_SubDEdgePtr* feptr = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++)
  {
    if (4 == fei)
    {
      feptr = m_edgex;
      if (nullptr == feptr)
      {
        if ( eptr != stack_eptr )
          onfree(eptr);  
        return false;
      }
    }
    eptr[fei] = *feptr++;
  }

  feptr = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++)
  {
    if (4 == fei)
      feptr = m_edgex;
    *feptr++ = eptr[(fei + fei0) % edge_count];
  }
  if ( eptr != stack_eptr )
    onfree(eptr);

  return true;
}




bool MYON_SubDFace::RemoveEdgeFromArray(
  const MYON_SubDEdge* e
  )
{
  unsigned int i;
  if (nullptr == e)
    return false;

  if (m_edge_count <= 4)
  {
    for (i = 0; i < m_edge_count; i++)
    {
      if (e == MYON_SUBD_EDGE_POINTER(m_edge4[i].m_ptr))
      {
        for (i++; i < m_edge_count; i++)
          m_edge4[i - 1] = m_edge4[i];
        m_edge_count--;
        m_edge4[m_edge_count] = MYON_SubDEdgePtr::Null;
        return true;
      }
    }
  }
  else
  {
    for (i = 0; i < 4; i++)
    {
      if (e == MYON_SUBD_EDGE_POINTER(m_edge4[i].m_ptr))
      {
        for (i++; i < 4; i++)
          m_edge4[i - 1] = m_edge4[i];
        m_edge4[3] = m_edgex[0];
        for (i = 5; i < m_edge_count; i++)
          m_edgex[i - 5] = m_edgex[i - 4];
        m_edge_count--;
        m_edgex[m_edge_count-4] = MYON_SubDEdgePtr::Null;
        return true;
      }
    }
    for (i = 4; i < m_edge_count; i++)
    {
      if (e == MYON_SUBD_EDGE_POINTER(m_edgex[i - 4].m_ptr))
      {
        for (i++; i < m_edge_count; i++)
          m_edgex[i - 5] = m_edgex[i - 4];
        m_edge_count--;
        m_edgex[m_edge_count-4] = MYON_SubDEdgePtr::Null;
        return true;
      }
    }
  }

  return false;
}

unsigned int MYON_SubDFace::VertexIndex(
  const MYON_SubDVertex* vertex
  ) const
{
  if (nullptr == vertex)
    return MYON_UNSET_UINT_INDEX;

  const MYON_SubDEdgePtr* face_edges = m_edge4;
  const unsigned int edge_count = m_edge_count;

  for (unsigned int i = 0; i < edge_count; i += 2)
  {
    if (4 == i)
    {
      face_edges = m_edgex;
      if (nullptr == face_edges)
        break;
      face_edges -= 4;
    }
    MYON__UINT_PTR e_ptr = face_edges[i].m_ptr;
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr != edge)
    {
      if (vertex == edge->m_vertex[0])
        return (0 == MYON_SUBD_EDGE_DIRECTION(e_ptr)) ? i : ((i + 1) % edge_count);
      if (vertex == edge->m_vertex[1])
        return (0 == MYON_SUBD_EDGE_DIRECTION(e_ptr)) ? ((i + 1) % edge_count) : i;
    }
  }

  return MYON_UNSET_UINT_INDEX;
}

const MYON_SubDComponentPtrPair MYON_SubDFace::VertexEdgePair(
  const MYON_SubDVertex* vertex
) const
{
  return MYON_SubDFace::VertexEdgePair(VertexIndex(vertex));
}

const MYON_SubDComponentPtrPair MYON_SubDFace::VertexEdgePair(
  unsigned vertex_index
) const
{
  for (;;)
  {
    const unsigned edge_count = m_edge_count;
    if (edge_count < 3)
      break;
    if (vertex_index >= edge_count)
      break;
    if (edge_count > 4 && nullptr == m_edgex)
      break;
    const unsigned fei0 = (vertex_index + (edge_count - 1)) % edge_count;
    const MYON_SubDEdgePtr eptr[2] =
    {
      (fei0 < 4 ? m_edge4[fei0] : m_edgex[fei0 - 4]),
      (vertex_index < 4 ? m_edge4[vertex_index] : m_edgex[vertex_index - 4]),
    };
    const MYON_SubDVertex* v = eptr[0].RelativeVertex(1);
    if (nullptr == v || v != eptr[1].RelativeVertex(0))
      break;
    return MYON_SubDComponentPtrPair::Create( MYON_SubDComponentPtr::Create(eptr[0]), MYON_SubDComponentPtr::Create(eptr[1]) );
  }
  return MYON_SubDComponentPtrPair::Null;
}


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubD
//

MYON_OBJECT_IMPLEMENT(MYON_SubD,MYON_Geometry,"F09BA4D9-455B-42C3-BA3B-E6CCACEF853B");

MYON_SubD::MYON_SubD() MYON_NOEXCEPT
  : MYON_Geometry()
{}

MYON_SubD::~MYON_SubD()
{
  this->Destroy();
}


#if defined(MYON_HAS_RVALUEREF)
MYON_SubD::MYON_SubD( MYON_SubD&& src ) MYON_NOEXCEPT
  : MYON_Geometry(std::move(src))
  , m_subdimple_sp(std::move(src.m_subdimple_sp))
{}

MYON_SubD& MYON_SubD::operator=( MYON_SubD&& src )
{
  if ( this != &src )
  {
    this->Destroy();
    MYON_Geometry::operator=(std::move(src));
    m_subdimple_sp = std::move(src.m_subdimple_sp);
  }
  return *this;
}
#endif


MYON__UINT64 MYON_SubD::RuntimeSerialNumber() const
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->RuntimeSerialNumber : 0;
}

MYON__UINT64 MYON_SubD::GeometryContentSerialNumber() const
{
  const MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->GeometryContentSerialNumber() : 0;
}

const MYON_SubDHash MYON_SubD::SubDHash(
  MYON_SubDHashType hash_type,
  bool bForceUpdate
) const
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->SubDHash(hash_type, bForceUpdate) : MYON_SubDHash::Create(hash_type , *this);
}

const MYON_SHA1_Hash MYON_SubD::GeometryHash() const
{
  return this->SubDHash(MYON_SubDHashType::Geometry, false).SubDHash();
}

const MYON_SHA1_Hash MYON_SubD::TopologyHash() const
{
  return this->SubDHash(MYON_SubDHashType::Topology, false).SubDHash();
}

const MYON_SubDHash MYON_SubDimple::SubDHash(
  MYON_SubDHashType hash_type,
  bool bForceUpdate
) const
{
  const unsigned vertex_count = this->ActiveLevel().m_vertex_count;
  if (0 == vertex_count)
    return MYON_SubDHash::Empty;

  // m_subd_toplologyX_hash, m_subd_toplology_and_edge_crease_hash, and m_subd_geometry_hash
  // are mutable and use lazy evaluation to stay updated.
  // subd.GeometryContentSerialNumber() is used to detect stale values.
  MYON_SubDHash* h;
  switch (hash_type)
  {
  case MYON_SubDHashType::Topology:
    h = &this->m_subd_toplology_hash;
    break;
  case MYON_SubDHashType::TopologyAndEdgeCreases:
    h = &this->m_subd_toplology_and_edge_creases_hash;
    break;
  case MYON_SubDHashType::Geometry:
    h = &this->m_subd_geometry_hash;
    break;
  default:
    h = nullptr;
    break;
  }
  if ( nullptr == h)
    return MYON_SubDHash::Empty;


  const unsigned edge_count = this->ActiveLevel().m_edge_count;
  const unsigned face_count = this->ActiveLevel().m_face_count;
  const MYON__UINT64 rsn = this->RuntimeSerialNumber;
  const MYON__UINT64 gsn = this->GeometryContentSerialNumber();
  if (
    false == bForceUpdate
    && h->IsNotEmpty()
    && hash_type == h->HashType()
    && rsn > 0 && rsn == h->SubDRuntimeSerialNumber()
    && gsn > 0 && gsn == h->SubDGeometryContentSerialNumber()
    && vertex_count == h->VertexCount()
    && edge_count == h->EdgeCount()
    && face_count == h->FaceCount()
    )
  {
    // The chache hash values are up to date (or should be).
    // If h is out of date, something somewhere modified the SubD components and 
    // failed to change the GeometryContentSerialNumber(). 
    // All C++ SDK opennurbs code changes gsn after modifying SubD geometry (or it's a bug that should be fixed).
    // The unwashed masses can do just about anything and that's why the bForceUpdate parameter is supplied.
    return *h;
  }

  // update cached value
  *h = MYON_SubDHash::Create(hash_type,this);

  // return updated value
  return *h;
}

MYON__UINT64 MYON_SubD::RenderContentSerialNumber() const
{
  const MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->RenderContentSerialNumber() : 0;
}

MYON__UINT64 MYON_SubD::ComponentStatusSerialNumber() const
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ComponentStatusSerialNumber() : 0;
}

MYON__UINT64 MYON_SubD::ChangeGeometryContentSerialNumberForExperts(
  bool bChangePreservesSymmetry
)
{
  // changes both MYON_SubD::ContentSerialNumber and MYON_SubD::RenderContentSerialNumber().
  if (this == &MYON_SubD::Empty)
    return 0;
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ChangeGeometryContentSerialNumber(bChangePreservesSymmetry) : 0;
}


MYON__UINT64 MYON_SubD::ChangeRenderContentSerialNumber() const
{
  if (this == &MYON_SubD::Empty)
    return 0;
  const MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ChangeRenderContentSerialNumber() : 0;
}

MYON_SubDComponentLocation MYON_SubD::ToggleSubDAppearanceValue(MYON_SubDComponentLocation subd_appearance)
{
  if (MYON_SubDComponentLocation::Surface == subd_appearance)
    return MYON_SubDComponentLocation::ControlNet;
  if (MYON_SubDComponentLocation::ControlNet == subd_appearance)
    return MYON_SubDComponentLocation::Surface;
  return subd_appearance;
}

MYON_SubDComponentLocation MYON_SubDMeshFragmentIterator::SubDAppearance() const
{
  return 
    (MYON_SubDComponentLocation::Surface == m_subd_appearance_override || MYON_SubDComponentLocation::ControlNet == m_subd_appearance_override)
    ? m_subd_appearance_override
    : SubD().SubDAppearance();
}

void MYON_SubDMeshFragmentIterator::SetSubDAppearanceOverride(MYON_SubDComponentLocation subd_appearance_override)
{
  if (m_subd_appearance_override != subd_appearance_override )
    m_subd_appearance_override = subd_appearance_override;
}

MYON_SubDComponentLocation MYON_SubD::SubDAppearance() const
{
  const MYON_SubDimple* subdimple = this->SubDimple();
  return (nullptr != subdimple) ? subdimple->SubDAppearance() : MYON_SubD::DefaultSubDAppearance;
}

MYON_SubDComponentLocation MYON_SubDimple::SubDAppearance() const
{
  return m_subd_appearance;
}

void MYON_SubD::SetSubDAppearance(MYON_SubDComponentLocation subd_appearance) const
{
  if (
    subd_appearance != SubDAppearance()
    && (MYON_SubDComponentLocation::Surface == subd_appearance || MYON_SubDComponentLocation::ControlNet == subd_appearance)
    )
  {
    const MYON_SubDimple* subdimple = const_cast<MYON_SubD*>(this)->SubDimple(true);
    if (nullptr != subdimple)
      subdimple->SetSubDAppearance(subd_appearance);
  }
}

void MYON_SubDimple::SetSubDAppearance(MYON_SubDComponentLocation subd_appearance) const
{
  if (
    subd_appearance != m_subd_appearance
    && (MYON_SubDComponentLocation::Surface == subd_appearance || MYON_SubDComponentLocation::ControlNet == subd_appearance)
    )
  {
    m_subd_appearance = subd_appearance;
  }
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubD - MYON_Object overrides
//

//virtual 
void MYON_SubD::MemoryRelocate()
{
}

static bool MYON_SubDIsNotValid(bool bSilentError)
{
  MYON_SubDIncrementErrorCount();
  return bSilentError ? false : MYON_IsNotValid();
}

static bool EdgeSectorCoefficientIsSet(
  double edge_sector_coefficient
  )
{
  return (0.0 < edge_sector_coefficient && edge_sector_coefficient < 1.0);
}

static bool EdgeSectorCoefficientIsValid(
  double edge_vertex_coefficient,
  const MYON_SubDEdge* edge
)
{
  if (0.0 <= edge_vertex_coefficient && edge_vertex_coefficient < 1.0)
    return true;

  if (MYON_SubDSectorType::UnsetSectorCoefficient == edge_vertex_coefficient && nullptr != edge && 0 == edge->SubdivisionLevel())
    return true;

  return false;
}

static bool IsValidVertexEdgeLink(
  const MYON_SubDVertex* vertex,
  const MYON_SubDEdge* edge,
  MYON__UINT_PTR end_index,
  bool bSilentError
)
{
  if (nullptr == vertex || nullptr == edge)
    return MYON_SubDIsNotValid(bSilentError);

  if (end_index > 1)
    return MYON_SubDIsNotValid(bSilentError);

  if (edge->m_vertex[end_index] != vertex)
    return MYON_SubDIsNotValid(bSilentError);

  if (vertex->SubdivisionLevel() != edge->SubdivisionLevel())
    return MYON_SubDIsNotValid(bSilentError);

  if (false == EdgeSectorCoefficientIsValid(edge->m_sector_coefficient[end_index],edge))
    return MYON_SubDIsNotValid(bSilentError);

  if ( edge->IsSmooth() )
  {
    // edge->m_edge_tag is MYON_SubDEdgeTag::Smooth or MYON_SubDEdgeTag::SmoothX
    if (MYON_SubDVertexTag::Smooth == vertex->m_vertex_tag)
    {
      if (false == (0.0 == edge->m_sector_coefficient[end_index]))
        return MYON_SubDIsNotValid(bSilentError);
    }
    else
    {
      const unsigned int tagged_end_index = edge->TaggedEndIndex();
      if (MYON_SubDEdgeTag::SmoothX == edge->m_edge_tag)
      {
        if (2 != tagged_end_index)
          return MYON_SubDIsNotValid(bSilentError);
      }
      else
      {
        if (tagged_end_index != (unsigned int)end_index)
          return MYON_SubDIsNotValid(bSilentError);
      }
       
      MYON_SubDSectorType st = MYON_SubDSectorType::Create(edge,(unsigned int)end_index);
      if (!st.IsValid())
        return MYON_SubDIsNotValid(bSilentError);

      const double expected_sector_coefficient = st.SectorCoefficient();
      if (false == (expected_sector_coefficient == edge->m_sector_coefficient[end_index]))
        return MYON_SubDIsNotValid(bSilentError);

      if (false == EdgeSectorCoefficientIsSet(expected_sector_coefficient))
        return MYON_SubDIsNotValid(bSilentError);
    }
  }
  else if(MYON_SubDEdgeTag::Crease == edge->m_edge_tag)
  {
    // crease edge
    if (!(0.0 == edge->m_sector_coefficient[end_index]))
      return MYON_SubDIsNotValid(bSilentError);

    if (MYON_SubDVertexTag::Smooth == vertex->m_vertex_tag)
      return MYON_SubDIsNotValid(bSilentError);

    if (MYON_SubDVertexTag::Unset == vertex->m_vertex_tag)
      return MYON_SubDIsNotValid(bSilentError);
  }
  else
  {
    return MYON_SubDIsNotValid(bSilentError);
  }

  return true;
}

static bool IsValidVertexFaceLink(
  const MYON_SubDVertex* vertex,
  const MYON_SubDFace* face,
  unsigned int vertex_face_index,
  unsigned int face_vertex_index,
  bool bSilentError
  )
{
  if (nullptr == vertex || nullptr == face)
    return MYON_SubDIsNotValid(bSilentError);

  if (vertex->SubdivisionLevel() != face->SubdivisionLevel())
    return MYON_SubDIsNotValid(bSilentError);

  const unsigned int vertex_face_count = vertex->m_face_count;
  if (vertex_face_count <= 0)
    return MYON_SubDIsNotValid(bSilentError);
  if (nullptr == vertex->m_faces)
    return MYON_SubDIsNotValid(bSilentError);

  if (vertex_face_index >= vertex_face_count && MYON_UNSET_UINT_INDEX != vertex_face_index)
    return MYON_SubDIsNotValid(bSilentError);

  const unsigned int face_vertex_count = face->m_edge_count;
  if (face_vertex_count <= 0)
    return MYON_SubDIsNotValid(bSilentError);
  if (face_vertex_count > 4 && nullptr == face->m_edgex)
    return MYON_SubDIsNotValid(bSilentError);

  if (face_vertex_index >= face_vertex_count && MYON_UNSET_UINT_INDEX != face_vertex_index)
    return MYON_SubDIsNotValid(bSilentError);

  for (unsigned int i = 0; i < vertex_face_count; i++)
  {
    if (face == vertex->Face(i))
    {
      if (MYON_UNSET_UINT_INDEX == vertex_face_index)
        vertex_face_index = i;
      else if (i != vertex_face_index)
        return MYON_SubDIsNotValid(bSilentError);
    }
    else if (i == vertex_face_index)
    {
      return MYON_SubDIsNotValid(bSilentError);
    }
  }

  for (unsigned int i = 0; i < face_vertex_count; i++)
  {
    if (vertex == face->Vertex(i))
    {
      if (MYON_UNSET_UINT_INDEX == face_vertex_index)
        face_vertex_index = i;
      else if (i != face_vertex_index)
        return MYON_SubDIsNotValid(bSilentError);
    }
    else if (i == face_vertex_index)
    {
      return MYON_SubDIsNotValid(bSilentError);
    }
  }

  return true;
}


static bool IsValidEdgeFaceLink(
  const MYON_SubDEdge* edge,
  const MYON_SubDFace* face,
  unsigned int edge_face_index,
  unsigned int face_edge_index,
  bool bSilentError
  )
{
  if (nullptr == edge || nullptr == face)
    return MYON_SubDIsNotValid(bSilentError);

  if (edge->SubdivisionLevel() != face->SubdivisionLevel())
    return MYON_SubDIsNotValid(bSilentError);

  const unsigned int edge_face_count = edge->m_face_count;
  if (edge_face_count <= 0)
    return MYON_SubDIsNotValid(bSilentError);
  if (edge_face_count > 2 && nullptr == edge->m_facex)
    return MYON_SubDIsNotValid(bSilentError);

  if (edge_face_index >= edge_face_count && MYON_UNSET_UINT_INDEX != edge_face_index)
    return MYON_SubDIsNotValid(bSilentError);

  const unsigned int face_edge_count = face->m_edge_count;
  if (face_edge_count <= 0)
    return MYON_SubDIsNotValid(bSilentError);
  if (face_edge_count > 4 && nullptr == face->m_edgex)
    return MYON_SubDIsNotValid(bSilentError);

  if (face_edge_index >= face_edge_count && MYON_UNSET_UINT_INDEX != face_edge_index)
    return MYON_SubDIsNotValid(bSilentError);

  for (unsigned int i = 0; i < edge_face_count; i++)
  {
    if (face == edge->Face(i))
    {
      if (MYON_UNSET_UINT_INDEX == edge_face_index)
        edge_face_index = i;
      else if (i != edge_face_index)
        return MYON_SubDIsNotValid(bSilentError);
    }
    else if (i == edge_face_index)
    {
      return MYON_SubDIsNotValid(bSilentError);
    }
  }

  for (unsigned int i = 0; i < face_edge_count; i++)
  {
    if (edge == face->Edge(i))
    {
      if (MYON_UNSET_UINT_INDEX == face_edge_index)
        face_edge_index = i;
      else if (i != face_edge_index)
        return MYON_SubDIsNotValid(bSilentError);
    }
    else if (i == face_edge_index)
    {
      return MYON_SubDIsNotValid(bSilentError);
    }
  }

  return true;
}

class MYON_Internal_DamagedMarker
{
public:
  MYON_Internal_DamagedMarker() = default;
  ~MYON_Internal_DamagedMarker()
  {
    if (nullptr != m_subd_component)
      m_subd_component->m_status.SetDamagedState(true);
  }

private:
  MYON_Internal_DamagedMarker(const MYON_Internal_DamagedMarker&) = delete;
  MYON_Internal_DamagedMarker& operator=(const MYON_Internal_DamagedMarker&) = delete;

public:
  MYON_Internal_DamagedMarker(const MYON_SubDComponentBase* subd_component)
    : m_subd_component(subd_component)
  {}

  void ClearComponent()
  {
    m_subd_component = nullptr;
  }

private:
  const MYON_SubDComponentBase* m_subd_component = nullptr;
};

static bool IsValidSubDVertexTag(
  const MYON_SubDVertex* vertex,
  bool bSilentError
)
{
  if (nullptr == vertex)
    return true; // this error detected elsewhere.

  MYON_Internal_DamagedMarker dm(vertex);

  const unsigned short vertex_edge_count = vertex->m_edge_count;
  unsigned short crease_edge_count = 0;
  unsigned short smooth_edge_count = 0;
  for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
    if (nullptr == e)
      continue;
    if (e->IsSmooth())
      ++smooth_edge_count;
    else if (e->IsCrease())
      ++crease_edge_count;
  }
  const bool bValidEdgeTags = (vertex_edge_count == crease_edge_count + smooth_edge_count);

  switch (vertex->m_vertex_tag)
  {
  case MYON_SubDVertexTag::Unset:
    return MYON_SubDIsNotValid(bSilentError);
    break;

  case MYON_SubDVertexTag::Smooth:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if (
      0 != crease_edge_count
      || vertex_edge_count < 2
      || vertex_edge_count != smooth_edge_count
      || vertex_edge_count != vertex->m_face_count
      )
    {
      return MYON_SubDIsNotValid(bSilentError);
    }
    break;

  case MYON_SubDVertexTag::Crease:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if ( 2 != crease_edge_count )
    {
      return MYON_SubDIsNotValid(bSilentError);
    }
    break;

  case MYON_SubDVertexTag::Corner:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if (0 == crease_edge_count)
    {
      // currently, isolated vertices are not permitted - may change in the future
      return MYON_SubDIsNotValid(bSilentError);
    }
    if (1 == crease_edge_count)
    {
      // must be a single wire crease edge ending at this vertex 
      if ( 1 != vertex_edge_count || 0 != vertex->m_face_count)
        return MYON_SubDIsNotValid(bSilentError);

    }
    break;

  case MYON_SubDVertexTag::Dart:
    if (false == bValidEdgeTags)
      break; // invalid edge tags detected in IsValidSubDEdgeTag();

    if (
      1 != crease_edge_count
      || vertex_edge_count < 2
      || vertex_edge_count != smooth_edge_count + crease_edge_count
      || vertex_edge_count != vertex->m_face_count
      )
    {
      return MYON_SubDIsNotValid(bSilentError);
    }
    break;

  default:
    return MYON_SubDIsNotValid(bSilentError);
    break;
  }

  dm.ClearComponent();
  return true;
}

static bool IsValidSubDEdgeTag(
  const MYON_SubDEdge* edge,
  bool bSilentError
)
{
  if (nullptr == edge)
    return true; // this error detected elsewhere.

  //MYON_SubDVertexTag vtag[2] = { MYON_SubDVertexTag::Unset,MYON_SubDVertexTag::Unset };
  unsigned int smooth_vertex_count = 0;
  unsigned int crease_vertex_count = 0;
  unsigned int corner_vertex_count = 0;
  unsigned int dart_vertex_count = 0;

  for ( unsigned int evi = 0; evi < 2; evi++)
  {
    if (nullptr == edge->m_vertex[evi])
      return true; // topology errors detected elsewhere
    switch (edge->m_vertex[evi]->m_vertex_tag)
    {
    case MYON_SubDVertexTag::Smooth:
      ++smooth_vertex_count;
      break;
    case MYON_SubDVertexTag::Crease:
      ++crease_vertex_count;
      break;
    case MYON_SubDVertexTag::Corner:
      ++corner_vertex_count;
      break;
    case MYON_SubDVertexTag::Dart:
      ++dart_vertex_count;
      break;
    case MYON_SubDVertexTag::Unset:
      break;
    }
  };

  if (2 != smooth_vertex_count + crease_vertex_count + corner_vertex_count + dart_vertex_count)
    return true; // invalid vertex tags detected in IsValidSubDVertexTag();

  MYON_Internal_DamagedMarker dm(edge);

  //const unsigned short edge_face_count = edge->m_face_count;
  switch(edge->m_edge_tag)
  {

  case MYON_SubDEdgeTag::Unset:
    return MYON_SubDIsNotValid(bSilentError);
    break;

  case MYON_SubDEdgeTag::Smooth:
    if ( 2 != edge->m_face_count)
      return MYON_SubDIsNotValid(bSilentError);
    if ( smooth_vertex_count < 1)
      return MYON_SubDIsNotValid(bSilentError);
    break;

  case MYON_SubDEdgeTag::Crease:
    if ( 0 != smooth_vertex_count )
      return MYON_SubDIsNotValid(bSilentError);
    break;

  case MYON_SubDEdgeTag::SmoothX:
    if ( 2 != edge->m_face_count)
      return MYON_SubDIsNotValid(bSilentError);
    if ( 0 != smooth_vertex_count )
      return MYON_SubDIsNotValid(bSilentError);
    break;

  default:
    return MYON_SubDIsNotValid(bSilentError);
    break;
  }

  dm.ClearComponent();
  return true;
}


static bool IsValidSubDVertex(
  const MYON_SubDVertex* vertex,
  unsigned short level,
  unsigned int* vertex_id_range,
  unsigned short ordinary_valence_count,
  bool bSilentError
  )
{
  if (nullptr == vertex)
    return MYON_SubDIsNotValid(bSilentError);
  
  if (vertex->SubdivisionLevel() != level)
    return MYON_SubDIsNotValid(bSilentError);
  
  if (nullptr != vertex_id_range)
  {
    if (vertex->m_id < vertex_id_range[0])
      return MYON_SubDIsNotValid(bSilentError);
    if (vertex->m_id > vertex_id_range[1])
      return MYON_SubDIsNotValid(bSilentError);
  }

  MYON_Internal_DamagedMarker dm(vertex);

  if (vertex->m_edge_count < vertex->m_face_count)
  {
    if ( MYON_SubDVertexTag::Corner != vertex->m_vertex_tag || vertex->m_edge_count < 3 )
      return MYON_SubDIsNotValid(bSilentError);
  }

  if (vertex->m_edge_count > 0 && nullptr == vertex->m_edges)
    return MYON_SubDIsNotValid(bSilentError);

  if (vertex->m_face_count > 0 && nullptr == vertex->m_faces)
    return MYON_SubDIsNotValid(bSilentError);

  switch (vertex->m_vertex_tag)
  {
  case MYON_SubDVertexTag::Smooth: // interior vertex
    if (vertex->m_edge_count != vertex->m_face_count)
      return MYON_SubDIsNotValid(bSilentError);
    break;

  case MYON_SubDVertexTag::Crease:
    if ( vertex->m_edge_count < 2 )
      return MYON_SubDIsNotValid(bSilentError);
    break;

  case MYON_SubDVertexTag::Corner:
    if ( vertex->m_edge_count < 1 )
      return MYON_SubDIsNotValid(bSilentError);
    break;

  case MYON_SubDVertexTag::Dart: // interior vertex
    if (level > 0 && ordinary_valence_count != vertex->m_edge_count)
      return MYON_SubDIsNotValid(bSilentError);
    if (vertex->m_edge_count != vertex->m_face_count)
      return MYON_SubDIsNotValid(bSilentError);
    break;

  default:
    // invalid value for this enum
    return MYON_SubDIsNotValid(bSilentError);
    break;
  }

  unsigned int count = vertex->m_edge_count;
  for (unsigned int i = 0; i < count; i++)
  {
    const MYON_SubDEdge* edge = vertex->Edge(i);
    if (nullptr == edge)
      return MYON_SubDIsNotValid(bSilentError);
  }

  count = vertex->m_face_count;
  for (unsigned int i = 0; i < count; i++)
  {
    const MYON_SubDFace* face = vertex->Face(i);
    if (nullptr == face)
      return MYON_SubDIsNotValid(bSilentError);
  }

  dm.ClearComponent();
  return true;
}


static bool IsValidSubDEdge(
  const MYON_SubDEdge* edge,
  unsigned short level,
  unsigned int* edge_id_range,
  bool bSilentError
  )
{
  if (nullptr == edge)
    return MYON_SubDIsNotValid(bSilentError);

  if (edge->SubdivisionLevel() != level)
    return MYON_SubDIsNotValid(bSilentError);

  if (nullptr != edge_id_range)
  {
    if (edge->m_id < edge_id_range[0])
      return MYON_SubDIsNotValid(bSilentError);
    if (edge->m_id > edge_id_range[1])
      return MYON_SubDIsNotValid(bSilentError);
  }

  MYON_Internal_DamagedMarker dm(edge);

  const MYON_SubDVertex* edge_vertex[2] = {};
  for (unsigned int i = 0; i < 2; i++)
  {
    const MYON_SubDVertex* vertex = edge->Vertex(i);
    if (nullptr == vertex)
      return MYON_SubDIsNotValid(bSilentError);
    edge_vertex[i] = vertex;
  }
  if (edge_vertex[0] == edge_vertex[1])
    return MYON_SubDIsNotValid(bSilentError);

  if (edge->IsSmooth())
  {
    // m_edge_tag is MYON_SubDEdgeTag::Smooth or MYON_SubDEdgeTag::SmoothX
    if ( 2 != edge->m_face_count)
      return MYON_SubDIsNotValid(bSilentError);
  }
  else if (MYON_SubDEdgeTag::Crease != edge->m_edge_tag)
  {
    return MYON_SubDIsNotValid(bSilentError);
  }

  if (edge->m_face_count > 2 && nullptr == edge->m_facex)
    return MYON_SubDIsNotValid(bSilentError);

  dm.ClearComponent();

  return true;
}

static bool IsValidSubDFace(
  const MYON_SubDFace* face,
  unsigned short level,
  unsigned int* face_id_range,
  unsigned short ordinary_face_edge_count,
  bool bSilentError
  )
{
  if (nullptr == face)
    return MYON_SubDIsNotValid(bSilentError);

  if (face->SubdivisionLevel() != level)
    return MYON_SubDIsNotValid(bSilentError);

  if (nullptr != face_id_range)
  {
    if (face->m_id < face_id_range[0])
      return MYON_SubDIsNotValid(bSilentError);
    if (face->m_id > face_id_range[1])
      return MYON_SubDIsNotValid(bSilentError);
  }

  MYON_Internal_DamagedMarker dm(face);

  if (face->m_edge_count < 3)
    return MYON_SubDIsNotValid(bSilentError);

  if (face->m_edge_count > 4 && nullptr == face->m_edgex)
    return MYON_SubDIsNotValid(bSilentError);

  if (level > 0 && ordinary_face_edge_count != face->m_edge_count)
    return MYON_SubDIsNotValid(bSilentError);

  dm.ClearComponent();

  return true;
}

bool MYON_SubDimple::IsValidLevel(
  const MYON_SubD& subd,
  unsigned int level_index,
  bool bSilentError,
  MYON_TextLog* text_log
  ) const
{
  const unsigned int level_count = m_levels.UnsignedCount();
  if (level_index >= level_count || level_index >= 0xFFFF)
    return MYON_SubDIsNotValid(bSilentError);
  const MYON_SubDLevel* level = m_levels[level_index];
  if ( nullptr == level)
    return MYON_SubDIsNotValid(bSilentError);
  level->ClearComponentDamagedState();

  if ( level->m_level_index != level_index)
    return MYON_SubDIsNotValid(bSilentError);

  if (level_index <= 0)
  {
    if (level->m_vertex_count < 3)
      return MYON_SubDIsNotValid(bSilentError);
    if (level->m_edge_count < 3)
      return MYON_SubDIsNotValid(bSilentError);
    if (level->m_face_count < 1)
      return MYON_SubDIsNotValid(bSilentError);
  }
  else
  {
    const MYON_SubDLevel* previous_level = m_levels[level_index - 1];
    if (nullptr == previous_level)
      return MYON_SubDIsNotValid(bSilentError);
    if (level->m_vertex_count <= previous_level->m_vertex_count)
      return MYON_SubDIsNotValid(bSilentError);
    if (level->m_edge_count <= previous_level->m_edge_count)
      return MYON_SubDIsNotValid(bSilentError);
    if (level->m_face_count <= previous_level->m_face_count)
      return MYON_SubDIsNotValid(bSilentError);
  }

  if (nullptr == level->m_vertex[0])
    return MYON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_edge[0])
    return MYON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_face[0])
    return MYON_SubDIsNotValid(bSilentError);

  if (nullptr == level->m_vertex[1])
    return MYON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_edge[1])
    return MYON_SubDIsNotValid(bSilentError);
  if (nullptr == level->m_face[1])
    return MYON_SubDIsNotValid(bSilentError);


  const unsigned short expected_level = (unsigned short)level_index;
  unsigned int i;
  const MYON_SubDVertex* vertex;
  const MYON_SubDEdge* edge;
  const MYON_SubDFace* face;

  unsigned int v_id_range[2] = { MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX };
  unsigned int e_id_range[2] = { MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX };
  unsigned int f_id_range[2] = { MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX };

  unsigned int point_vertex_count = 0;
  unsigned int wire_edge_count = 0;

  // simple vertex validation
  if (level_index == subd.ActiveLevelIndex())
  {
    if (subd.FirstVertex() != level->m_vertex[0])
      return MYON_SubDIsNotValid(bSilentError);
    MYON_SubDVertexIterator vit = subd.VertexIterator();
    if (vit.FirstVertex() != level->m_vertex[0])
      return MYON_SubDIsNotValid(bSilentError);
    MYON_SubDVertexArray va = subd.VertexArray();
    if (va.VertexCount() != level->m_vertex_count)
      return MYON_SubDIsNotValid(bSilentError);
    if (va[0] != level->m_vertex[0])
      return MYON_SubDIsNotValid(bSilentError);
    if (va[level->m_vertex_count-1] != level->m_vertex[1])
      return MYON_SubDIsNotValid(bSilentError);
  }
  const MYON_SubDVertex* last_vertex = nullptr;
  for (i = 0, vertex = level->m_vertex[0]; i < level->m_vertex_count && nullptr != vertex; i++, vertex = vertex->m_next_vertex)
  {
    if (false == IsValidSubDVertex(vertex, expected_level, nullptr, level->m_ordinary_vertex_valence, bSilentError))
      return false;

    if (0 == i)
    {
      v_id_range[0] = v_id_range[1] = vertex->m_id;
    }
    else if (vertex->m_id < v_id_range[0])
      v_id_range[0] = vertex->m_id;
    else if (vertex->m_id > v_id_range[1])
      v_id_range[1] = vertex->m_id;

    if (0 == vertex->m_edge_count)
    {
      point_vertex_count++;
    }
    last_vertex = vertex;
  }
  if (level->m_vertex[1] != last_vertex)
    return MYON_SubDIsNotValid(bSilentError);


  if (i != level->m_vertex_count || nullptr != vertex)
    return MYON_SubDIsNotValid(bSilentError);
  if (1 + v_id_range[1] - v_id_range[0] < level->m_vertex_count)
    return MYON_SubDIsNotValid(bSilentError);
  if ( v_id_range[1] > MaximumVertexId() )
    return MYON_SubDIsNotValid(bSilentError);

  // currently, point vertices are not permitted
  if (point_vertex_count > 0)
    return MYON_SubDIsNotValid(bSilentError);

  // simple edge validation
  if (level_index == subd.ActiveLevelIndex())
  {
    if (subd.FirstEdge() != level->m_edge[0])
      return MYON_SubDIsNotValid(bSilentError);
    MYON_SubDEdgeIterator eit = subd.EdgeIterator();
    if (eit.FirstEdge() != level->m_edge[0])
      return MYON_SubDIsNotValid(bSilentError);
    MYON_SubDEdgeArray ea = subd.EdgeArray();
    if (ea.EdgeCount() != level->m_edge_count)
      return MYON_SubDIsNotValid(bSilentError);
    if (ea[0] != level->m_edge[0])
      return MYON_SubDIsNotValid(bSilentError);
    if (ea[level->m_edge_count-1] != level->m_edge[1])
      return MYON_SubDIsNotValid(bSilentError);
  }
  const MYON_SubDEdge* last_edge = nullptr;
  for (i = 0, edge = level->m_edge[0]; i < level->m_edge_count && nullptr != edge; i++, edge = edge->m_next_edge)
  {
    if (false == IsValidSubDEdge(edge, expected_level, nullptr, bSilentError))
      return false;
    if (0 == edge->m_face_count)
    {
      wire_edge_count++;
    }
    if (0 == i)
    {
      e_id_range[0] = e_id_range[1] = edge->m_id;
    }
    else if (edge->m_id < e_id_range[0])
      e_id_range[0] = edge->m_id;
    else if (edge->m_id > e_id_range[1])
      e_id_range[1] = edge->m_id;

    last_edge = edge;
  }
  if (i != level->m_edge_count || nullptr != edge)
    return MYON_SubDIsNotValid(bSilentError);
  if (1 + e_id_range[1] - e_id_range[0] < level->m_edge_count)
    return MYON_SubDIsNotValid(bSilentError);
  if (level->m_edge[1] != last_edge)
    return MYON_SubDIsNotValid(bSilentError);
  if ( e_id_range[1] > MaximumEdgeId() )
    return MYON_SubDIsNotValid(bSilentError);
   
  // As of NOvember 12, 2019 
  // Wire edges are permitted. THey exist in subds being edited.
  ////// currently, wire edges are not permitted
  ////if (wire_edge_count > 0)
  ////  return MYON_SubDIsNotValid(bSilentError);
   
  // simple face validation
  if (level_index == subd.ActiveLevelIndex())
  {
    if (subd.FirstFace() != level->m_face[0])
      return MYON_SubDIsNotValid(bSilentError);
    MYON_SubDFaceIterator fit = subd.FaceIterator();
    if (fit.FirstFace() != level->m_face[0])
      return MYON_SubDIsNotValid(bSilentError);
    MYON_SubDFaceArray fa = subd.FaceArray();
    if (fa.FaceCount() != level->m_face_count)
      return MYON_SubDIsNotValid(bSilentError);
    if (fa[0] != level->m_face[0])
      return MYON_SubDIsNotValid(bSilentError);
    if (fa[0] != level->m_face[0])
      return MYON_SubDIsNotValid(bSilentError);
  }
  const MYON_SubDFace* last_face = nullptr;
  for (i = 0, face = level->m_face[0]; i < level->m_face_count && nullptr != face; i++, face = face->m_next_face)
  {
    if (false == IsValidSubDFace(face, expected_level, nullptr, level->m_ordinary_face_edge_count, bSilentError))
      return false;

    if (0 == i)
    {
      f_id_range[0] = f_id_range[1] = face->m_id;
    }
    else if (face->m_id < f_id_range[0])
      f_id_range[0] = face->m_id;
    else if (face->m_id > f_id_range[1])
      f_id_range[1] = face->m_id;

    last_face = face;
  }
  if (i != level->m_face_count || nullptr != face)
    return MYON_SubDIsNotValid(bSilentError);
  if (1 + f_id_range[1] - f_id_range[0] < level->m_face_count)
    return MYON_SubDIsNotValid(bSilentError);  
  if (level->m_face[1] != last_face)
    return MYON_SubDIsNotValid(bSilentError);
  if ( f_id_range[1] > MaximumFaceId() )
    return MYON_SubDIsNotValid(bSilentError);


  // vertex topology validation
  for (vertex = level->m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
  {
    for (i = 0; i < vertex->m_edge_count; i++)
    {
      edge = vertex->Edge(i);
      if (false == IsValidSubDEdge(edge, expected_level, e_id_range, bSilentError))
        return false;
      if (false == IsValidVertexEdgeLink(vertex, edge, vertex->EdgeDirection(i), bSilentError))
        return false;
    }

    for (i = 0; i < vertex->m_face_count; i++)
    {
      face = vertex->Face(i);
      if (false == IsValidSubDFace(face, expected_level, f_id_range, level->m_ordinary_face_edge_count, bSilentError))
        return false;
      if (false == IsValidVertexFaceLink(vertex, face, i, MYON_UNSET_UINT_INDEX, bSilentError))
        return false;
    }
  }

  // edge topology validation
  for (edge = level->m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    for (i = 0; i < 2; i++)
    {
      vertex = edge->m_vertex[i];
      if (false == IsValidSubDVertex(vertex, expected_level, v_id_range, level->m_ordinary_vertex_valence, bSilentError))
        return false;
      if (false == IsValidVertexEdgeLink(vertex, edge, i, bSilentError))
        return false;
    }

    for (i = 0; i < edge->m_face_count; i++)
    {
      face = edge->Face(i);
      if (false == IsValidSubDFace(face, expected_level, f_id_range, level->m_ordinary_face_edge_count, bSilentError))
        return false;
      if (false == IsValidEdgeFaceLink(edge, face, i, MYON_UNSET_UINT_INDEX, bSilentError))
        return false;
    }
  }

  // face topology validation
  for (face = level->m_face[0]; nullptr != face; face = face->m_next_face)
  {
    for (i = 0; i < face->m_edge_count; i++)
    {
      edge = face->Edge(i);
      if (false == IsValidSubDEdge(edge, expected_level, e_id_range, bSilentError))
        return false;
      if (false == IsValidEdgeFaceLink(edge, face, MYON_UNSET_UINT_INDEX, i, bSilentError))
        return false;
    }

    for (i = 0; i < face->m_edge_count; i++)
    {
      vertex = face->Vertex(i);
      if (false == IsValidSubDVertex(vertex, expected_level, v_id_range, level->m_ordinary_vertex_valence, bSilentError))
        return false;
      if (false == IsValidVertexFaceLink(vertex, face, MYON_UNSET_UINT_INDEX, i, bSilentError))
        return false;
    }
  }


  // edge tag validation
  for (edge = level->m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    if (false == IsValidSubDEdgeTag(edge, bSilentError))
      return false;
  }

  // vertex tag validation
  for (vertex = level->m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
  {
    if (false == IsValidSubDVertexTag(vertex, bSilentError))
      return false;
  }

  // edge length validation
  for (edge = level->m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    const MYON_3dPoint P[2] = { edge->m_vertex[0]->ControlNetPoint(), edge->m_vertex[1]->ControlNetPoint() };
    if (false == (P[0] != P[1]))
    {
      edge->m_status.SetDamagedState(true);
      return MYON_SubDIsNotValid(bSilentError);
    }
  }

  return true;
}

bool MYON_SubDimple::IsValid(
  const MYON_SubD& subd,
  bool bSilentError,
  MYON_TextLog* text_log
  ) const
{
  if (false == m_heap.IsValid(bSilentError, text_log))
  {
    if (nullptr != text_log)
      text_log->Print("Component ids are not set correctly. m_heap.ResetId() will fix this but may break externally stored component references.\n");
    return MYON_SubDIsNotValid(bSilentError);
  }

  const unsigned int level_count = m_levels.UnsignedCount();
  if (level_count < 1)
  {
    return MYON_SubDIsNotValid(bSilentError);
  }

  for (unsigned int level_index = 0; level_index < level_count; level_index++)
  {
    if (false == IsValidLevel(subd, level_index, bSilentError, text_log))
      return false;
  }  

  return true;
}

//virtual
bool MYON_SubD::IsValid(MYON_TextLog* text_logx) const
{
  // If low bit of text_log pointer is 1, then MYON_Error is not called when the
  // knot vector is invalid.
  const MYON__INT_PTR lowbit = 1;
  const MYON__INT_PTR hightbits = ~lowbit;
  const bool bSilentError = (0 != (lowbit & ((MYON__INT_PTR)text_logx)));
  MYON_TextLog* text_log = (MYON_TextLog*)(((MYON__INT_PTR)text_logx) & hightbits);
  
  const MYON_SubDimple* subdimple = SubDimple();
  if (nullptr == subdimple)
    return MYON_SubDIsNotValid(bSilentError);

  return subdimple->IsValid(*this, bSilentError, text_log);
}

//virtual
void MYON_SubD::Dump(MYON_TextLog& text_log) const
{
  // At maximum verbosity, dump all vertices, edges, faces, else dump the first 16.
  const unsigned component_sample_count = text_log.LevelOfDetailIsAtLeast(MYON_TextLog::LevelOfDetail::Maximum) ? 0x7FFFFFFF : 16; 

  MYON_2udex id_range;
  id_range.i = component_sample_count;
  id_range.j = 0;
  DumpTopology(id_range,id_range,id_range,text_log);
}

unsigned int MYON_SubD::DumpTopology(MYON_TextLog & text_log) const
{
  return DumpTopology(MYON_2udex::Zero,MYON_2udex::Zero,MYON_2udex::Zero,text_log);
}

static const MYON_wString Internal_DescribeWaste(size_t waste, size_t total)
{
  if (waste <= 0 || total <= 0)
    return MYON_wString::ToMemorySize(0);

  double p = 100.0 * ((double)waste) / ((double)total);
  if (p != p)
    return MYON_wString::EmptyString;
  const double i = (p - floor(p) <= 0.5) ? floor(p) : ceil(p);
  double e = fabs(i - p);

  MYON_wString description = MYON_wString::EmptyString;
  const double negligible = 0.1;
  if (e < negligible)
  {
    if (0.0 == i)
      description = L" negligible";
    p = i;
  }
  if (description.IsEmpty())
    description = (i >= 10.0) ? MYON_wString::FormatToString(L"%g%% of total", i) : MYON_wString::FormatToString(L"%0.1f%% of total)", p);
  if (waste > 0)
    description += MYON_wString(L" (") + MYON_wString::ToMemorySize(waste) + MYON_wString(L")");
  return description;
}

unsigned int MYON_SubD::DumpTopology(
  MYON_2udex vertex_id_range,
  MYON_2udex edge_id_range,
  MYON_2udex face_id_range,
  MYON_TextLog& text_log
) const
{
  const class MYON_SubDimple* subdimple = SubDimple();
  if (nullptr == subdimple)
  {
    text_log.Print(L"SubD: Empty\n");
    return 0;
  }

  const unsigned int level_count = LevelCount();
  const unsigned int active_level_index = ActiveLevel().m_level_index;

  const bool bIsTextHash = text_log.IsTextHash();

  // TextHash ignores settings that don't depend on 3dm file content.
  const MYON__UINT64 runtime_sn = (bIsTextHash) ? 0 : RuntimeSerialNumber();
  const MYON__UINT64 geometry_content_sn = (bIsTextHash) ? 0 : this->GeometryContentSerialNumber();
  const MYON__UINT64 render_content_sn = (bIsTextHash) ? 0 : this->RenderContentSerialNumber();

  const unsigned subd_vertex_count = VertexCount();
  const unsigned subd_edge_count = EdgeCount();
  const unsigned subd_face_count = FaceCount();

  if (level_count > 1)
  {
    text_log.Print(L"SubD[%" PRIu64 "]: %u levels. Level %u is active (%u vertices, %u edges, %u faces).\n",
      runtime_sn,
      level_count,
      active_level_index,
      subd_vertex_count,
      subd_edge_count,
      subd_face_count
    );
  }
  else
  {
    text_log.Print(
      L"SubD[%" PRIu64 "]: %u vertices, %u edges, %u faces\n",
      runtime_sn,
      subd_vertex_count,
      subd_edge_count,
      subd_face_count
    );
  }

  MYON_SubDEdgeSharpness subd_sharpness_range = MYON_SubDEdgeSharpness::Zero;
  const unsigned int subd_sharp_edge_count = SharpEdgeCount(subd_sharpness_range);
  if (subd_sharp_edge_count > 0)
  {
    text_log.PushIndent();
    if ( subd_sharpness_range.IsConstant() )
    {
      if (1 == subd_sharp_edge_count)
        text_log.Print(
          L"1 sharp edge with end sharpness = %g.\n",
          subd_sharpness_range[0]
        );
      else
        text_log.Print(
          L"%u sharp edges with end sharpness = %g.\n",
          subd_sharp_edge_count,
          subd_sharpness_range[0]
        );
    }
    else
    {
      text_log.Print(
        L"%u sharp edges with end sharpnesses from %g to %g.\n",
        subd_sharp_edge_count,
        subd_sharpness_range[0],
        subd_sharpness_range[1]
        );

    }
    text_log.PopIndent();
  }

  const MYON_SubDHashType htype[] = {
    MYON_SubDHashType::Topology,
    MYON_SubDHashType::TopologyAndEdgeCreases,
    MYON_SubDHashType::Geometry
  };

  for (size_t i = 0; i < sizeof(htype) / sizeof(htype[0]); ++i)
  {
    const MYON_SubDHash h = this->SubDHash(htype[i], false);
    h.Dump(text_log);
  }

  text_log.Print(L"Texture coordinate settings:\n");
  {
    const MYON_SubDTextureCoordinateType subd_texture_coordinate_type = this->TextureCoordinateType();
    MYON_TextLogIndent indent1(text_log);
    const MYON_wString subd_texture_coordinate_type_as_string = MYON_SubD::TextureCoordinateTypeToString(this->TextureCoordinateType());
    text_log.Print(L"TextureCoordinateType() = %ls\n", static_cast<const wchar_t*>(subd_texture_coordinate_type_as_string));

    const bool bShowMappingTag
      = false == bIsTextHash
      || MYON_SubDTextureCoordinateType::FromMapping == subd_texture_coordinate_type
      ;

    if (bShowMappingTag)
    {
      const MYON_MappingTag mapping_tag = this->TextureMappingTag(true);

      const bool bSurfaceParameterMappingTag
        = (0 == MYON_MappingTag::CompareAll(MYON_MappingTag::SurfaceParameterMapping, mapping_tag))
        || (bIsTextHash && (MYON_TextureMapping::TYPE::srfp_mapping == mapping_tag.m_mapping_type || MYON_MappingTag::SurfaceParameterMapping.m_mapping_id == mapping_tag.m_mapping_id))
        ;

      const bool bUnsetMappingTag
        = ((false == bSurfaceParameterMappingTag) &&
          (0 == MYON_MappingTag::CompareAll(MYON_MappingTag::Unset, mapping_tag))) ||
          ((bIsTextHash && (MYON_TextureMapping::TYPE::no_mapping == mapping_tag.m_mapping_type || MYON_nil_uuid == mapping_tag.m_mapping_id)));

      // NOTE: the mapping tag is only applied when subd_texture_coordinate_type = FromMapping
      if (MYON_SubDTextureCoordinateType::FromMapping == subd_texture_coordinate_type && false == bUnsetMappingTag)
        text_log.Print(L"TextureMappingTag()");
      else
        text_log.Print(L"Inactive TextureMappingTag()");

      if (bUnsetMappingTag)
        text_log.Print(L" = MYON_MappingTag::Unset\n");
      else if (bSurfaceParameterMappingTag)
        text_log.Print(L" = MYON_MappingTag::SurfaceParameterMapping\n");
      else
      {
        text_log.Print(":\n");
        const MYON_TextLogIndent indent2(text_log);
        text_log.Print("m_mapping_type = ");
        switch (mapping_tag.m_mapping_type)
        {
        case  MYON_TextureMapping::TYPE::no_mapping:
          text_log.Print("none");
          break;
        case  MYON_TextureMapping::TYPE::srfp_mapping:
          text_log.Print("srfp");
          break;
        case  MYON_TextureMapping::TYPE::plane_mapping:
          text_log.Print("plane");
          break;
        case  MYON_TextureMapping::TYPE::cylinder_mapping:
          text_log.Print("cylinder");
          break;
        case  MYON_TextureMapping::TYPE::sphere_mapping:
          text_log.Print("sphere");
          break;
        case  MYON_TextureMapping::TYPE::box_mapping:
          text_log.Print("box");
          break;
        case  MYON_TextureMapping::TYPE::mesh_mapping_primitive:
          text_log.Print("mesh primitive");
          break;
        case  MYON_TextureMapping::TYPE::srf_mapping_primitive:
          text_log.Print("srf primitive");
          break;
        case  MYON_TextureMapping::TYPE::brep_mapping_primitive:
          text_log.Print("brep primitive");
          break;
        case  MYON_TextureMapping::TYPE::ocs_mapping:
          text_log.Print("ocs");
          break;
        case  MYON_TextureMapping::TYPE::false_colors:
          text_log.Print("false colors");
          break;
        }
        text_log.PrintNewLine();
        text_log.Print("m_mapping_id = ");
        text_log.Print(mapping_tag.m_mapping_id);
        if (mapping_tag.m_mapping_id == MYON_MappingTag::SurfaceParameterMapping.m_mapping_id)
          text_log.Print(" = MYON_MappingTag::SurfaceParameterMapping.m_mapping_id");
        text_log.PrintNewLine();
        text_log.Print("m_mapping_crc = %08x\n", mapping_tag.m_mapping_crc);
        text_log.Print("m_mesh_xform:\n");
        text_log.PushIndent();
        text_log.Print(mapping_tag.m_mesh_xform);
        text_log.PopIndent();
      }

    }

    const MYON_SHA1_Hash subd_texture_settings_hash = this->TextureSettingsHash();
    text_log.Print(L"TextureSettingsHash() = ");
    subd_texture_settings_hash.Dump(text_log);
    text_log.PrintNewLine();

    if (false == text_log.IsTextHash())
    {
      // runtime settings most recentltly used to set fragmant texture coordinates.
      const MYON_SHA1_Hash frament_texture_settings_hash = this->FragmentTextureCoordinatesTextureSettingsHash();
      text_log.Print(L"FragmentTextureCoordinatesTextureSettingsHash() = ");
      if (subd_texture_settings_hash == frament_texture_settings_hash)
        text_log.Print(L"TextureSettingsHash()");
      else
        frament_texture_settings_hash.Dump(text_log);
      text_log.PrintNewLine();
    }

    if (false == text_log.IsTextHash())
    {
      const MYON_SHA1_Hash subd_fragment_color_settings_hash = this->FragmentColorsSettingsHash();
      text_log.Print(L"FragmentColorsSettingsHash() = ");
      subd_fragment_color_settings_hash.Dump(text_log);
      text_log.PrintNewLine();
    }
  }

  bool bIncludeSymmetrySet = false;

  text_log.Print(L"Geometry content serial number = %" PRIu64 "\n", geometry_content_sn);
  text_log.Print(L"Render content serial number = %" PRIu64 "\n", render_content_sn);
  text_log.Print("Heap use:\n");
  {
    MYON_TextLogIndent indent1(text_log);
    size_t sizeof_subd = this->SizeOfAllElements();
    text_log.PrintString(MYON_wString(L"Total = ") + MYON_wString::ToMemorySize(sizeof_subd) + MYON_wString(L".\n"));
    const size_t sizeof_frags = this->SizeOfAllMeshFragments();
    text_log.PrintString(MYON_wString(L"Mesh fragments = ") + MYON_wString::ToMemorySize(sizeof_frags) + MYON_wString(L".\n"));
    const size_t sizeof_frags_waste = this->SizeOfUnusedMeshFragments();
    text_log.PrintString(MYON_wString(L"Reserved but ununsed = ")
      + Internal_DescribeWaste(sizeof_frags_waste,sizeof_subd)
      + MYON_wString(L".\n"));
  }

  text_log.Print(L"Levels:\n");

  MYON_SubDLevelIterator lit(subdimple->LevelIterator());

  const MYON_2udex empty_id_range(MYON_UNSET_UINT_INDEX, 0);

  MYON_SubDVertexIdIterator vidit(*this); 
  MYON_SubDEdgeIdIterator eidit(*this); 
  MYON_SubDFaceIdIterator fidit(*this);

  unsigned int error_count = 0;
  for (const MYON_SubDLevel* level = lit.First(); nullptr != level; level = lit.Next())
  {
    MYON_TextLogIndent indent1(text_log);
    if (nullptr == level)
      continue;
    MYON_2udex level_vertex_id_range
      = (0 != vertex_id_range.j || active_level_index == level->m_level_index)
      ? vertex_id_range
      : empty_id_range;
    MYON_2udex level_edge_id_range
      = (0 != edge_id_range.j || active_level_index == level->m_level_index)
      ? edge_id_range
      : empty_id_range;
    MYON_2udex level_face_id_range
      = (0 != face_id_range.j || active_level_index == level->m_level_index)
      ? face_id_range
      : empty_id_range;

    error_count += level->DumpTopology(
      *this,
      subdimple->MaximumVertexId(),
      subdimple->MaximumEdgeId(),
      subdimple->MaximumFaceId(),
      level_vertex_id_range,
      level_edge_id_range, 
      level_face_id_range,
      vidit,
      eidit,
      fidit,
      bIncludeSymmetrySet,
      text_log);
  }

  return error_count;
}

MYON_SubDHashType MYON_SubDHashTypeFromUnsigned(
  unsigned int subd_hash_type_as_unsigned
)
{
  switch (subd_hash_type_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDHashType::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDHashType::Topology);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDHashType::TopologyAndEdgeCreases);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDHashType::Geometry);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDHashType::Unset);
}

const MYON_wString MYON_SubDHashTypeToString(
  MYON_SubDHashType subd_hash_type,
  bool bVerbose
)
{
  const wchar_t* name;
  switch (subd_hash_type)
  {
  case MYON_SubDHashType::Unset:
    name = L"Unset";
    break;
  case MYON_SubDHashType::Topology:
    name = L"Topology";
    break;
  case MYON_SubDHashType::TopologyAndEdgeCreases:
    name = L"TopologyAndEdgeCreases";
    break;
  case MYON_SubDHashType::Geometry:
    name = L"Geometry";
    break;
  default:
    name = L"invalid";
    break;
  }

  return bVerbose ? (MYON_wString(L"MYON_SubDHashType::") + MYON_wString(name)) : MYON_wString(name);
}

MYON_SubDEndCapStyle MYON_SubDEndCapStyleFromUnsigned(
  unsigned int subd_cap_style_as_unsigned
)
{
  switch (subd_cap_style_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEndCapStyle::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEndCapStyle::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEndCapStyle::Triangles);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEndCapStyle::Quads);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_SubDEndCapStyle::Ngon);
  }
  return MYON_SUBD_RETURN_ERROR(MYON_SubDEndCapStyle::Unset);
}

const MYON_wString MYON_SubDEndCapStyleToString(
  MYON_SubDEndCapStyle subd_cap_style,
  bool bVerbose
)
{
  const wchar_t* s;
  switch (subd_cap_style)
  {
  case MYON_SubDEndCapStyle::Unset:
    s = L"Unset";
    break;
  case MYON_SubDEndCapStyle::None:
    s = L"None";
    break;
  case MYON_SubDEndCapStyle::Triangles:
    s = L"Triangles";
    break;
  case MYON_SubDEndCapStyle::Quads:
    s = L"Quads";
    break;
  case MYON_SubDEndCapStyle::Ngon:
    s = L"Ngon";
    break;
  default:
    s = L"invalid";
    break;
  }

  return MYON_wString(s);
}

static void Internal_AccumulateVertexHash(
  MYON_SHA1& sha1,
  MYON_SubDHashType hash_type,
  const MYON_SubDVertex* v
)
{
  sha1.AccumulateInteger32(v->m_id);
  if (MYON_SubDHashType::Geometry == hash_type)
  {
    sha1.AccumulateBytes(&v->m_vertex_tag, sizeof(v->m_vertex_tag));
    sha1.AccumulateDoubleArray(3, v->m_P);
  }
}

static const MYON_SHA1_Hash Internal_VertexHash(MYON_SubDHashType hash_type, const MYON_SubDVertex* first_vertex, unsigned int level_index, MYON_SubDVertexIdIterator& vidit)
{
  MYON_SHA1 sha1;
  if (MYON_SubDHashType::Unset != hash_type)
  {
    unsigned prev_id = 0;
    for (const MYON_SubDVertex* v = first_vertex; nullptr != v; v = v->m_next_vertex)
    {
      if (prev_id > v->m_id)
      {
        // must use slower vidit to get consistent results when complex editing juggles vertex order
        sha1.Reset();
        for (v = vidit.FirstVertex(); nullptr != v; v = vidit.NextVertex())
        {
          if ( level_index == v->SubdivisionLevel() && v->IsActive() )
            Internal_AccumulateVertexHash(sha1, hash_type, v);
        }
        break;
      }
      Internal_AccumulateVertexHash(sha1, hash_type, v);
      prev_id = v->m_id;
    }
  }
  return sha1.Hash();
}

const MYON_SHA1_Hash MYON_SubD::VertexHash(MYON_SubDHashType hash_type) const
{
  MYON_SubDVertexIdIterator vidit(*this);
  return Internal_VertexHash(hash_type, FirstVertex(), this->ActiveLevelIndex(), vidit);
}

const MYON_SHA1_Hash MYON_SubDimple::VertexHash(MYON_SubDHashType hash_type) const
{
  MYON_SubDVertexIdIterator vidit;
  this->InitializeVertexIdIterator(vidit);
  return Internal_VertexHash(hash_type, ActiveLevel().m_vertex[0], this->ActiveLevelIndex(), vidit);
}

static void Internal_AccumulateEdgeHash(
  MYON_SHA1& sha1,
  MYON_SubDHashType hash_type,
  const MYON_SubDEdge* e
)
{
  sha1.AccumulateInteger32(e->m_id);
  sha1.AccumulateInteger32(e->VertexId(0));
  sha1.AccumulateInteger32(e->VertexId(1));

  if (MYON_SubDHashType::TopologyAndEdgeCreases == hash_type || MYON_SubDHashType::Geometry == hash_type)
  {
    // Changing edge crease/smooth attributes often changes the regions used in face packing and exploding.
    const bool bIsCrease = e->IsCrease();
    sha1.AccumulateBool(bIsCrease);
    if (false == bIsCrease && MYON_SubDHashType::Geometry == hash_type && e->IsSharp())
    {
      const MYON_SubDEdgeSharpness s = e->Sharpness();
      const double a[2] = { s[0] ,s[1] };
      sha1.AccumulateDoubleArray(2, a);
    }
  }
}


static const MYON_SHA1_Hash Internal_EdgeHash(MYON_SubDHashType hash_type, const MYON_SubDEdge* first_edge, unsigned int level_index, MYON_SubDEdgeIdIterator& eidit)
{
  MYON_SHA1 sha1;
  if (MYON_SubDHashType::Unset != hash_type)
  {
    unsigned prev_id = 0;
    for (const MYON_SubDEdge* e = first_edge; nullptr != e; e = e->m_next_edge)
    {
      if (prev_id > e->m_id)
      {
        // must use slower eidit to get consistent results when complex editing juggles vertex order
        sha1.Reset();
        for (e = eidit.FirstEdge(); nullptr != e; e = eidit.NextEdge())
        {
          if (level_index == e->SubdivisionLevel() && e->IsActive())
            Internal_AccumulateEdgeHash(sha1, hash_type, e);
        }
        break;
      }
      Internal_AccumulateEdgeHash(sha1, hash_type, e);
      prev_id = e->m_id;
    }
  }
  return sha1.Hash();
}

const MYON_SHA1_Hash MYON_SubD::EdgeHash(MYON_SubDHashType hash_type) const
{
  MYON_SubDEdgeIdIterator eidit(*this);
  return Internal_EdgeHash(hash_type,FirstEdge(), this->ActiveLevelIndex(), eidit);
}


const MYON_SHA1_Hash MYON_SubDimple::EdgeHash(MYON_SubDHashType hash_type) const
{
  MYON_SubDEdgeIdIterator eidit;
  this->InitializeEdgeIdIterator(eidit);
  return Internal_EdgeHash(hash_type, ActiveLevel().m_edge[0], this->ActiveLevelIndex(), eidit);
}

static void Internal_AccumulateFaceHash(
  MYON_SHA1& sha1,
  MYON_SubDHashType hash_type,
  const MYON_SubDFace* f
)
{
  sha1.AccumulateInteger32(f->m_id);
  sha1.AccumulateInteger16(f->m_edge_count);
  const MYON_SubDEdgePtr* eptr = f->m_edge4;
  for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++eptr)
  {
    if (4 == fei)
    {
      eptr = f->m_edgex;
      if (nullptr == eptr)
        break;
    }
    sha1.AccumulateInteger32(eptr->EdgeId());
    if (0 != MYON_SUBD_EDGE_DIRECTION(eptr->m_ptr))
      sha1.AccumulateBool(true);
  }
}

static const MYON_SHA1_Hash Internal_FaceHash(MYON_SubDHashType hash_type, const MYON_SubDFace* first_face, unsigned int level_index, MYON_SubDFaceIdIterator& fidit)
{
  MYON_SHA1 sha1;
  if (MYON_SubDHashType::Unset != hash_type)
  {
    unsigned prev_id = 0;
    for (const MYON_SubDFace* f = first_face; nullptr != f; f = f->m_next_face)
    {
      if (prev_id > f->m_id)
      {
        // must use slower fidit to get consistent results when complex editing juggles vertex order
        sha1.Reset();
        for (f = fidit.FirstFace(); nullptr != f; f = fidit.NextFace())
        {
          if (level_index == f->SubdivisionLevel() && f->IsActive())
            Internal_AccumulateFaceHash(sha1, hash_type, f);
        }
        break;
      }
      Internal_AccumulateFaceHash(sha1, hash_type, f);
      prev_id = f->m_id;
    }
  }
  return sha1.Hash();
}

const MYON_SHA1_Hash MYON_SubD::FaceHash(MYON_SubDHashType hash_type) const
{
  MYON_SubDFaceIdIterator fidit(*this);
  return Internal_FaceHash(hash_type,FirstFace(), this->ActiveLevelIndex(), fidit);
}

const MYON_SHA1_Hash MYON_SubDimple::FaceHash(MYON_SubDHashType hash_type) const
{
  MYON_SubDFaceIdIterator fidit;
  this->InitializeFaceIdIterator(fidit);
  return Internal_FaceHash(hash_type, ActiveLevel().m_face[0], this->ActiveLevelIndex(), fidit);
}

static void Internal_AccumulateFragmentArrayHash(MYON_SHA1& sha1, size_t dim, const double* a, unsigned count, size_t stride)
{
  if (nullptr != a && count > 0 && dim > 0 && (0 == stride || stride >= dim))
  {
    sha1.AccumulateInteger32((unsigned int)dim);
    sha1.AccumulateInteger32(count);
    for (unsigned i = 0; i < count; ++i)
    {
      sha1.AccumulateDoubleArray(dim, a);
      a += stride;
    }
  }
}

const MYON_SubDHash MYON_SubDHash::Create(MYON_SubDHashType hash_type, const class MYON_SubD& subd)
{
  return MYON_SubDHash::Create(hash_type, subd.SubDimple());
}

const MYON_SubDHash MYON_SubDHash::Create(MYON_SubDHashType hash_type, const class MYON_SubDimple* subdimple)
{
  if (nullptr == subdimple)
    return MYON_SubDHash::Empty;

  const MYON_SubDLevel& active_level = subdimple->ActiveLevel();
  MYON_SubDHash h;
  h.m_hash_type = hash_type;
  h.m_vertex_count = active_level.m_vertex_count;
  h.m_edge_count = active_level.m_edge_count;
  h.m_face_count = active_level.m_face_count;
  h.m_subd_runtime_serial_number = subdimple->RuntimeSerialNumber;
  if (h.m_vertex_count > 0)
  {
    h.m_subd_geometry_content_serial_number = subdimple->GeometryContentSerialNumber();
    if (MYON_SubDHashType::Unset != hash_type)
    {
      h.m_vertex_hash = subdimple->VertexHash(hash_type);
      h.m_edge_hash = subdimple->EdgeHash(hash_type);
      h.m_face_hash = subdimple->FaceHash(hash_type);
    }
  }
  return h;
}

int MYON_SubDHash::Compare(const MYON_SubDHash& lhs, const MYON_SubDHash& rhs)
{
  if (lhs.m_vertex_count < rhs.m_vertex_count)
    return -1;
  if (lhs.m_vertex_count > rhs.m_vertex_count)
    return 1;

  if (lhs.m_edge_count < rhs.m_edge_count)
    return -1;
  if (lhs.m_edge_count > rhs.m_edge_count)
    return 1;

  if (lhs.m_face_count < rhs.m_face_count)
    return -1;
  if (lhs.m_face_count > rhs.m_face_count)
    return 1;

  int rc;

  rc = MYON_SHA1_Hash::Compare(lhs.m_vertex_hash, rhs.m_vertex_hash);
  if (0 != rc)
    return rc;

  rc = MYON_SHA1_Hash::Compare(lhs.m_edge_hash, rhs.m_edge_hash);
  if (0 != rc)
    return rc;

  return MYON_SHA1_Hash::Compare(lhs.m_face_hash, rhs.m_face_hash);
}

void MYON_SubDHash::Dump(MYON_TextLog& text_log) const
{
  if (text_log.IsTextHash())
    return; 

  bool bIsNotEmpty = IsNotEmpty();

  if (bIsNotEmpty)
  {
    switch (this->HashType())
    {
    case MYON_SubDHashType::Topology:
      text_log.Print("SubD toplogy hash:\n");
      break;
    case MYON_SubDHashType::TopologyAndEdgeCreases:
      text_log.Print("SubD toplogy and edge creases hash:\n");
      break;
    case MYON_SubDHashType::Geometry:
      text_log.Print("SubD geometry hash:\n");
      break;
    default:
      bIsNotEmpty = false;
      break;
    }
  }

  if (bIsNotEmpty)
  {
    const MYON_TextLogIndent indent1(text_log);
    const unsigned vcount = this->VertexCount();
    const unsigned ecount = this->EdgeCount();
    const unsigned fcount = this->FaceCount();
    const MYON_wString subdsha1 = this->SubDHash().ToStringEx(true);
    text_log.Print(L"SubD SHA1 = %ls\n", static_cast<const wchar_t*>(subdsha1));
    const MYON_TextLogIndent indent2(text_log);
    if (vcount > 0)
    {
      const MYON_wString vsha1 = this->VertexHash().ToStringEx(true);
      text_log.Print(L"%u vertices. SHA1 = %ls\n", vcount, static_cast<const wchar_t*>(vsha1));
    }
    else
      text_log.Print("No vertices.\n");

    if (ecount > 0)
    {
      const MYON_wString esha1 = this->EdgeHash().ToStringEx(true);
      text_log.Print(L"%u edges. SHA1 = %ls\n", ecount, static_cast<const wchar_t*>(esha1));
    }
    else
      text_log.Print("No edges.\n");

    if (fcount > 0)
    {
      const MYON_wString fsha1 = this->FaceHash().ToStringEx(true);
      text_log.Print(L"%u faces. SHA1 = %ls\n", fcount, static_cast<const wchar_t*>(fsha1));
    }
    else
      text_log.Print("No faces.\n");
  }
  else
    text_log.Print("SubD hash: Empty\n");
}

bool MYON_SubDHash::Write(class MYON_BinaryArchive& archive) const
{
  if (false == archive.BeginWrite3dmAnonymousChunk(1))
    return false;
  bool rc = false;
  for (;;)
  {
    const bool bIsEmpty = IsEmpty();
    if (false == archive.WriteBool(bIsEmpty))
      break;
    if (bIsEmpty)
    {
      rc = true;
      break;
    }
    // The SubD runtime serial number and geometry content serial numbers are runtime values.
    // When appropriate, calling contexts need to take appropriate steps when writing and 
    // set these after reading.
    const unsigned char hash_type_as_unsigned = static_cast<unsigned char>(m_hash_type);
    if (false == archive.WriteChar(hash_type_as_unsigned))
      break;
    if (false == archive.WriteInt(m_vertex_count))
      break;
    if (false == this->m_vertex_hash.Write(archive))
      break;
    if (false == archive.WriteInt(m_edge_count))
      break;
    if (false == this->m_edge_hash.Write(archive))
      break;
    if (false == archive.WriteInt(m_face_count))
      break;
    if (false == this->m_face_hash.Write(archive))
      break;
    rc = true;
    break;
  }
  if (false == archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_SubDHash::Read(class MYON_BinaryArchive& archive)
{
  *this = MYON_SubDHash::Empty;

  int chunk_version = 0;
  if (false == archive.BeginRead3dmAnonymousChunk(&chunk_version))
    return false;

  bool rc = false;
  for (;;)
  {
    bool bIsEmpty = true;
    if (false == archive.ReadBool(&bIsEmpty))
      break;
    if (bIsEmpty)
    {
      rc = true;
      break;
    }

    // The SubD runtime serial number and geometry content serial numbers are runtime values.
    // When appropriate, calling contexts need to set these after reading.
    unsigned char hash_type_as_unsigned = 0;
    if (false == archive.ReadChar(&hash_type_as_unsigned))
      break;
    m_hash_type = MYON_SubDHashTypeFromUnsigned(hash_type_as_unsigned);

    if (false == archive.ReadInt(&m_vertex_count))
      break;
    if (false == this->m_vertex_hash.Read(archive))
      break;
    if (false == archive.ReadInt(&m_edge_count))
      break;
    if (false == this->m_edge_hash.Read(archive))
      break;
    if (false == archive.ReadInt(&m_face_count))
      break;
    if (false == this->m_face_hash.Read(archive))
      break;
    rc = true;
    break;
  }

  if (false == archive.EndRead3dmChunk())
    rc = false;
  return rc;

}

MYON__UINT64 MYON_SubDHash::SubDRuntimeSerialNumber() const
{
  return this->m_subd_runtime_serial_number;
}

MYON__UINT64 MYON_SubDHash::SubDGeometryContentSerialNumber() const
{
  return this->m_subd_geometry_content_serial_number;
}

bool MYON_SubDHash::IsEmpty() const
{
  return MYON_SubDHashType::Unset == m_hash_type || 0 == m_vertex_count;
}

bool MYON_SubDHash::IsNotEmpty() const
{
  return MYON_SubDHashType::Unset != m_hash_type && m_vertex_count > 0;
}

MYON_SubDHashType MYON_SubDHash::HashType() const
{
  return m_hash_type;
}

unsigned int MYON_SubDHash::VertexCount() const
{
  return m_vertex_count;
}

unsigned int MYON_SubDHash::EdgeCount() const
{
  return m_edge_count;
}

unsigned int MYON_SubDHash::FaceCount() const
{
  return m_face_count;
}

const MYON_SHA1_Hash MYON_SubDHash::VertexHash() const
{
  return m_vertex_hash;
}

const MYON_SHA1_Hash MYON_SubDHash::EdgeHash() const
{
  return m_edge_hash;
}

const MYON_SHA1_Hash MYON_SubDHash::FaceHash() const
{
  return m_face_hash;
}

const MYON_SHA1_Hash MYON_SubDHash::SubDHash() const
{
  MYON_SHA1 sha1;
  if (m_vertex_count > 0)
    sha1.AccumulateInteger32(m_vertex_count);
  if (m_edge_count > 0)
    sha1.AccumulateInteger32(m_edge_count);
  if (m_face_count > 0)
    sha1.AccumulateInteger32(m_face_count);
  if (MYON_SubDHashType::Unset != m_hash_type)
  {
    if (m_vertex_count > 0)
      sha1.AccumulateSubHash(m_vertex_hash);
    if (m_edge_count > 0)
      sha1.AccumulateSubHash(m_edge_hash);
    if (m_face_count > 0)
      sha1.AccumulateSubHash(m_face_hash);
  }
  return sha1.Hash();
}

bool operator==(const MYON_SubDHash& lhs, const MYON_SubDHash& rhs)
{
  return 0 == MYON_SubDHash::Compare(lhs, rhs);
}

bool operator!=(const MYON_SubDHash& lhs, const MYON_SubDHash& rhs)
{
  return 0 != MYON_SubDHash::Compare(lhs, rhs);
}

static const MYON_SHA1_Hash Internal_PackRectHash(const MYON_SubDFace* first_face)
{
  MYON_SHA1 sha1;
  for (const MYON_SubDFace* f = first_face; nullptr != f; f = f->m_next_face)
  {
    if (false == f->PackRectIsSet())
      continue;
    sha1.AccumulateInteger32(f->m_id);
    sha1.AccumulateInteger16(f->m_edge_count);

    // The 4 f->TextureDomain...() values specify
    // the portion and orientation of texture space
    // this face uses.
    sha1.AccumulateInteger32(f->PackRectRotationDegrees());
    const MYON_2dPoint p = f->PackRectOrigin();
    sha1.Accumulate2dPoint(p);
    const MYON_2dVector v = f->PackRectSize();
    sha1.Accumulate2dVector(v);
  }
  return sha1.Hash();
}

static const MYON_SHA1_Hash Internal_TextureCoordinatesHash(const MYON_SubDFace* first_face)
{
  MYON_SHA1 sha1;
  bool bNotEmpty = false;
  for (const MYON_SubDFace* f = first_face; nullptr != f; f = f->m_next_face)
  {
    if (false == f->PackRectIsSet())
      continue;
    const MYON_SubDMeshFragment* first_frag = f->MeshFragments();
    if (nullptr == first_frag)
      continue;
    sha1.AccumulateInteger32(f->m_id);
    for (const MYON_SubDMeshFragment* frag = first_frag; nullptr != frag; frag = frag->NextFaceFragment(false))
    {
      const double* a = frag->TextureCoordinateArray(MYON_SubDComponentLocation::Surface);
      const unsigned count = frag->TextureCoordinateCount();
      if (nullptr != a && count > 0)
      {
        bNotEmpty = true;
        Internal_AccumulateFragmentArrayHash(sha1, 3, a, count, frag->TextureCoordinateArrayStride(MYON_SubDComponentLocation::Surface));
      }
    }
  }
  return bNotEmpty ? sha1.Hash() : MYON_SHA1_Hash::EmptyContentHash;
}

static const MYON_SHA1_Hash Internal_FragmentColorsHash(const MYON_SubDFace* first_face)
{
  MYON_SHA1 sha1;
  bool bNotEmpty = false;
  for (const MYON_SubDFace* f = first_face; nullptr != f; f = f->m_next_face)
  {
    const MYON_SubDMeshFragment* first_frag = f->MeshFragments();
    if (nullptr == first_frag)
      continue;
    bool bAccumulateId = true;
    for (const MYON_SubDMeshFragment* frag = first_frag; nullptr != frag; frag = frag->NextFaceFragment(false))
    {
      if (frag->ColorsExist())
      {
        const MYON_Color* a = frag->ColorArray(MYON_SubDComponentLocation::Surface);
        const unsigned count = frag->ColorArrayCount(MYON_SubDComponentLocation::Surface);
        if (nullptr != a && count > 0)
        {
          bNotEmpty = true;
          if (bAccumulateId)
          {
            sha1.AccumulateInteger32(f->m_id);
            bAccumulateId = false;
          }
          sha1.AccumulateInteger32Array(count, (const MYON__INT32*)a);
        }
      }
    }
  }
  return bNotEmpty ? sha1.Hash() : MYON_SHA1_Hash::EmptyContentHash;
}

static const MYON_SHA1_Hash Internal_FragmentCurvaturesHash(const MYON_SubDFace* first_face)
{
  MYON_SHA1 sha1;
  bool bNotEmpty = false;
  for (const MYON_SubDFace* f = first_face; nullptr != f; f = f->m_next_face)
  {
    const MYON_SubDMeshFragment* first_frag = f->MeshFragments();
    if (nullptr == first_frag)
      continue;
    bool bAccumulateId = true;
    for (const MYON_SubDMeshFragment* frag = first_frag; nullptr != frag; frag = frag->NextFaceFragment(false))
    {
      if (frag->CurvaturesExist())
      {
        const MYON_SurfaceCurvature* a = frag->CurvatureArray(MYON_SubDComponentLocation::Surface);
        const unsigned count = frag->CurvatureArrayCount(MYON_SubDComponentLocation::Surface);
        if (nullptr != a && count > 0)
        {
          bNotEmpty = true;
          if (bAccumulateId)
          {
            sha1.AccumulateInteger32(f->m_id);
            bAccumulateId = false;
          }
          Internal_AccumulateFragmentArrayHash(sha1, 2, (const double*)a, count, 2);
        }
      }
    }
  }
  return bNotEmpty ? sha1.Hash() : MYON_SHA1_Hash::EmptyContentHash;
}


unsigned int MYON_SubDLevel::DumpTopology(
  const MYON_SubD& parent_subd,
  const unsigned int validate_max_vertex_id,
  const unsigned int validate_max_edge_id,
  const unsigned int validate_max_face_id,
  MYON_2udex vertex_id_range,
  MYON_2udex edge_id_range,
  MYON_2udex face_id_range,
  MYON_SubDVertexIdIterator& vidit,
  MYON_SubDEdgeIdIterator& eidit,
  MYON_SubDFaceIdIterator& fidit,
  bool bIncludeSymmetrySet,
  MYON_TextLog& text_log
) const
{
  // NOTE WELL:
  //  This is a debugging tool.
  //  The code in this function needs to work when the topology information is corrupt.
  const unsigned short level_index = (unsigned short)m_level_index;

  const unsigned int vertex_max_dump_count 
    = (vertex_id_range.i > 0 && vertex_id_range.i != MYON_UNSET_UINT_INDEX && 0 == vertex_id_range.j)
    ? vertex_id_range.i 
    : 0;
  const unsigned int edge_max_dump_count 
    = (edge_id_range.i > 0 && edge_id_range.i != MYON_UNSET_UINT_INDEX && 0 == edge_id_range.j)
    ? edge_id_range.i
    : 0;
  const unsigned int face_max_dump_count 
    = (face_id_range.i > 0 && face_id_range.i != MYON_UNSET_UINT_INDEX && 0 == face_id_range.j)
    ? face_id_range.i 
    : 0;

  const bool bVertexIdTest = (vertex_max_dump_count > 0) || (vertex_id_range.i < vertex_id_range.j) || (MYON_UNSET_UINT_INDEX == (vertex_id_range.i));
  const bool bEdgeIdTest =  (edge_max_dump_count > 0) || (edge_id_range.i < edge_id_range.j) || (MYON_UNSET_UINT_INDEX == (edge_id_range.i));
  const bool bFaceIdTest =  (face_max_dump_count > 0) || (face_id_range.i < face_id_range.j) || (MYON_UNSET_UINT_INDEX == (face_id_range.i));

  const char error_code_point = '!';
  char prefix[16] = {};

  unsigned int vertex_error_count = 0;
  unsigned int edge_error_count = 0;
  unsigned int face_error_count = 0;

  text_log.Print(L"SubD level %u topology: %u vertices, %u edges", m_level_index, m_vertex_count, m_edge_count);


  unsigned int wire_edge_count = 0U;
  unsigned int boundary_edge_count = 0U;  
  unsigned int interior_edge_count = 0U;
  unsigned int nonmanifold_edge_count = 0U;
  for (const MYON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
  {
    if (0 == e->m_face_count)
      ++wire_edge_count;
    else if (1 == e->m_face_count)
      ++boundary_edge_count;
    else if (2 == e->m_face_count)
      ++interior_edge_count;
    else if (e->m_face_count >= 3)
      ++nonmanifold_edge_count;
  }

  if (wire_edge_count > 0U)
  {
    if (nonmanifold_edge_count > 0U)
    {
      if (boundary_edge_count > 0U && interior_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u interior, %u wire, %u nonmanifold)", boundary_edge_count, interior_edge_count, wire_edge_count, nonmanifold_edge_count);
      else if (boundary_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u wire, %u nonmanifold)", boundary_edge_count, wire_edge_count, nonmanifold_edge_count);
      else if (interior_edge_count > 0U)
        text_log.Print(L" (%u interior, %u wire, %u nonmanifold)", interior_edge_count, wire_edge_count, nonmanifold_edge_count);
      else
        text_log.Print(L" (%u wire, %u nonmanifold)", wire_edge_count, nonmanifold_edge_count);
    }
    else
    {
      if (boundary_edge_count > 0U && interior_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u interior, %u wire)", boundary_edge_count, interior_edge_count, wire_edge_count);
      else if (boundary_edge_count > 0U)
        text_log.Print(L" (%u boundary, %u wire)", boundary_edge_count, wire_edge_count);
      else if (interior_edge_count > 0U)
        text_log.Print(L" (%u interior, %u wire)", interior_edge_count, wire_edge_count);
      else
        text_log.Print(L" (%u wire)", wire_edge_count);
    }
  }
  else if (nonmanifold_edge_count > 0U)
  {
    if (boundary_edge_count > 0U && interior_edge_count > 0U)
      text_log.Print(L" (%u boundary, %u interior, %u nonmanifold)", boundary_edge_count, interior_edge_count, nonmanifold_edge_count);
    else if (boundary_edge_count > 0U)
      text_log.Print(L" (%u boundary, %u nonmanifold)", boundary_edge_count, nonmanifold_edge_count);
    else if (interior_edge_count > 0U)
      text_log.Print(L" (%u interior, %u nonmanifold)", interior_edge_count, nonmanifold_edge_count);
    else
      text_log.Print(L" (%u nonmanifold)", nonmanifold_edge_count);
  }
  else if (boundary_edge_count > 0U && interior_edge_count > 0U)
  {
    text_log.Print(L" (%u boundary, %u interior)", boundary_edge_count, interior_edge_count);
  }

  text_log.Print(L", ");

  unsigned int ngon_count[65] = {};
  unsigned int maxN = (unsigned int)(sizeof(ngon_count) / sizeof(ngon_count[0])) - 1;

  unsigned int face_count = 0;
  unsigned int uniformN = 0;
  for (const MYON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
  {
    if (face_count >= m_face_count && f->SubdivisionLevel() != level_index)
      break;
    face_count++;
    unsigned int N = f->EdgeCount();
    if (1 == face_count)
      uniformN = N;
    else if (N != uniformN)
      uniformN = 0;
    unsigned int j = (N < maxN) ? N : maxN;
    if (N < maxN)
      ngon_count[j]++;
  }

  if (face_count != m_face_count)
    face_error_count++;


  if (uniformN > 0 && face_count == m_face_count)
  {
    if (3 == uniformN)
      text_log.Print(L"%u triangles\n", m_face_count);
    else if (4 == uniformN)
      text_log.Print(L"%u quads\n", m_face_count);
    else
      text_log.Print(L"%u %u-gons\n", m_face_count, uniformN);
  }
  else
  {
    text_log.Print(L"%u faces\n", m_face_count );
    text_log.PushIndent();
    for (unsigned int N = 0; N <= maxN; N++)
    {
      if (0 == ngon_count[N])
        continue;
      if (3 == N)
        text_log.Print(L"%u triangles\n", ngon_count[N]);
      else if (4 == N)
        text_log.Print(L"%u quads\n", ngon_count[N]);
      else if (N < maxN)
        text_log.Print(L"%u %u-gons\n", ngon_count[N], N);
      else
        text_log.Print(L"%u N-gons\n", ngon_count[N]);
    }
    text_log.PopIndent();
  }

  if (IsEmpty())
    return 0;

  // The hash uniquely identifies the subd level topology and geometry.
  const MYON_SubDHashType hash_type = MYON_SubDHashType::Geometry;
  const MYON_SHA1_Hash vhash = Internal_VertexHash(hash_type, m_vertex[0], this->m_level_index, vidit);
  const MYON_SHA1_Hash ehash = Internal_EdgeHash(hash_type, m_edge[0], this->m_level_index, eidit);
  const MYON_SHA1_Hash fhash = Internal_FaceHash(hash_type, m_face[0], this->m_level_index, fidit);

  MYON_SHA1 level_sha1;
  level_sha1.AccumulateSubHash(vhash);
  level_sha1.AccumulateSubHash(ehash);
  level_sha1.AccumulateSubHash(fhash);
  const MYON_wString hashstr = level_sha1.Hash().ToString(true);
  text_log.Print(L"Level SubD geometry SHA1 = %ls\n", static_cast<const wchar_t*>(hashstr));

  const MYON_wString vhashstr = vhash.ToStringEx(true);
  const MYON_wString ehashstr = ehash.ToStringEx(true);
  const MYON_wString fhashstr = fhash.ToStringEx(true);
  text_log.Print(L"Vertices SHA1 = %ls\n", static_cast<const wchar_t*>(vhashstr));
  text_log.Print(L"Edges SHA1 = %ls\n", static_cast<const wchar_t*>(ehashstr));
  text_log.Print(L"Faces SHA1 = %ls\n", static_cast<const wchar_t*>(fhashstr));

  if (false == text_log.IsTextHash())
  {
    const MYON_SHA1_Hash prhash = Internal_PackRectHash(m_face[0]);
    const MYON_SHA1_Hash tchash = Internal_TextureCoordinatesHash(m_face[0]);
    const MYON_SHA1_Hash chash = Internal_FragmentColorsHash(m_face[0]);
    const MYON_SHA1_Hash khash = Internal_FragmentCurvaturesHash(m_face[0]);
    const MYON_wString prhashstr = prhash.ToStringEx(true);
    const MYON_wString tchashstr = tchash.ToStringEx(true);
    const MYON_wString chashstr = chash.ToStringEx(true);
    const MYON_wString khashstr = khash.ToStringEx(true);
    text_log.Print(L"Faces pack rectangles SHA1 = %ls\n", static_cast<const wchar_t*>(prhashstr));
    text_log.Print(L"Faces fragment texture coordinates SHA1 = %ls\n", static_cast<const wchar_t*>(tchashstr));
    text_log.Print(L"Faces fragment vertex colors SHA1 = %ls\n", static_cast<const wchar_t*>(chashstr));
    text_log.Print(L"Faces fragment vertex curvatures SHA1 = %ls\n", static_cast<const wchar_t*>(khashstr));
  }
  unsigned int damaged_vertex_count = 0;
  unsigned int damaged_edge_count = 0;
  unsigned int damaged_face_count = 0;
  enum : unsigned int
  {
    damaged_id_list_capacity = 8
  };
  for (;;)
  {
    unsigned int damaged_vertex_id[damaged_id_list_capacity] = {};
    unsigned int damaged_edge_id[damaged_id_list_capacity] = {};
    unsigned int damaged_face_id[damaged_id_list_capacity] = {};
    for (const MYON_SubDVertex* v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
    {
      if (false == v->m_status.IsDamaged())
        continue;
      if (damaged_vertex_count < damaged_id_list_capacity)
        damaged_vertex_id[damaged_vertex_count] = v->m_id;
      ++damaged_vertex_count;
    }
    for (const MYON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
    {
      if (false == e->m_status.IsDamaged())
        continue;
      if (damaged_edge_count < damaged_id_list_capacity)
        damaged_edge_id[damaged_edge_count] = e->m_id;
      ++damaged_edge_count;
    }
    for (const MYON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
    {
      if (false == f->m_status.IsDamaged())
        continue;
      if (damaged_face_count < damaged_id_list_capacity)
        damaged_face_id[damaged_face_count] = f->m_id;
      ++damaged_face_count;
    }

    if (0U == damaged_vertex_count && 0U == damaged_edge_count && 0U == damaged_face_count)
      break;
    text_log.Print("DAMAGED SubD level:\n");
    MYON_TextLogIndent indent1(text_log);
    if (damaged_vertex_count > 0)
    {
      text_log.Print(L"%u DAMAGED vertices: ", damaged_vertex_count);
      text_log.Print("v%u", damaged_vertex_id[0]);
      for (unsigned i = 1U; i < damaged_vertex_count; ++i)
        text_log.Print(", v%u", damaged_vertex_id[i]);
      if (damaged_vertex_count > damaged_id_list_capacity)
        text_log.Print(", ...");
      text_log.PrintNewLine();
    }
    if (damaged_edge_count > 0)
    {
      text_log.Print(L"%u DAMAGED edges: ", damaged_edge_count);
      text_log.Print("e%u", damaged_edge_id[0]);
      for (unsigned i = 1U; i < damaged_edge_count; ++i)
        text_log.Print(", e%u", damaged_edge_id[i]);
      if (damaged_edge_count > damaged_id_list_capacity)
        text_log.Print(", ...");
      text_log.PrintNewLine();
    }
    if (damaged_face_count > 0)
    {
      text_log.Print(L"%u DAMAGED faces: ", damaged_face_count);
      text_log.Print("f%u", damaged_face_id[0]);
      for (unsigned i = 1U; i < damaged_face_count; ++i)
        text_log.Print(", f%u", damaged_face_id[i]);
      if (damaged_face_count > damaged_id_list_capacity)
        text_log.Print(", ...");
      text_log.PrintNewLine();
    }
    break;
  }

  ///////////////////////////////////////////////////////////////////
  //
  // Vertex Topology
  // vN (x, y, z)
  //   vEdges[vertex_edge_count] = { +eA, -eB, ... }
  //   vFaces[vertex_edge_count] = { fP, fQ, fR, ... }
  //
  damaged_vertex_count = 0U;
  unsigned int vertex_count = 0;
  unsigned int vertex_dump_count = 0;
  MYON_2udex skipped_vertex_id = MYON_2udex::Zero;
  unsigned int max_vertex_id = 0;
  bool bSkippedPreviousComponent = false;
  for (const MYON_SubDVertex* v = m_vertex[0]; nullptr != v; v = v->m_next_vertex)
  {
    if (vertex_count >= m_vertex_count && v->SubdivisionLevel() != level_index)
      break;
    if (v->m_id > max_vertex_id)
      max_vertex_id = v->m_id;

    vertex_count++;
    const bool bIsDamaged = v->m_status.IsDamaged();
    if (bIsDamaged)
      ++damaged_vertex_count;

    if (bVertexIdTest && (false == bIsDamaged || damaged_vertex_count > damaged_id_list_capacity))
    {
      bool bSkip = true;
      for (;;)
      {
        if (MYON_UNSET_UINT_INDEX == vertex_id_range.i)
          break;
        if (vertex_max_dump_count > 0)
        {
          if (vertex_count > vertex_max_dump_count)
            break;
        }
        else
        {
          if (v->m_id < vertex_id_range.i || v->m_id >= vertex_id_range.j)
            break;
        }
        bSkip = false;
        break;
      }
      if (bSkip)
      {
        if (0 == skipped_vertex_id.j)
        {
          skipped_vertex_id.i = v->m_id;
          skipped_vertex_id.j = v->m_id;
        }
        else if (v->m_id < skipped_vertex_id.i)
          skipped_vertex_id.i = v->m_id;
        else if (v->m_id > skipped_vertex_id.j)
          skipped_vertex_id.j = v->m_id;
        bSkippedPreviousComponent = true;
        continue;
      }
    }

    if (0 == vertex_dump_count)
      text_log.Print(L"Vertices:\n");
    vertex_dump_count++;
    MYON_TextLogIndent vindent(text_log);

    const MYON_3dPoint P0(v->ControlNetPoint());

    const MYON_wString vtag = MYON_SubD::VertexTagToString(v->m_vertex_tag,false);

    if (bSkippedPreviousComponent)
    {
      text_log.Print(L"...\n");
      bSkippedPreviousComponent = false;
    }

    {
      MYON_String s = MYON_String::FormatToString("v%u: ",v->m_id);

      if (bIsDamaged)
        s += "(DAMAGED) ";

      s += MYON_String::FormatToString(
          "%ls (%g, %g, %g)",
          static_cast<const wchar_t*>(vtag),
          P0.x, P0.y, P0.z
        );
      if (v->m_group_id > 0)
        s += MYON_String::FormatToString(" group_id=%u", v->m_group_id);      
      text_log.PrintString(s);
      text_log.PrintNewLine();
    }

    text_log.PushIndent();

    MYON_3dPoint P1(MYON_3dPoint::NanPoint);
    if (v->GetSavedSubdivisionPoint(&P1.x) && P1.IsValid())
      text_log.Print( "v.SubdivisionPoint: (%g, %g, %g)\n", P1.x, P1.y, P1.z );

    MYON_3dPoint S(MYON_3dPoint::NanPoint);
    if (v->GetSavedSurfacePoint(&S.x) && S.IsValid())
      text_log.Print( "v.SurfacePoint: (%g, %g, %g)\n", S.x, S.y, S.z );

    const unsigned int vertex_edge_count = v->m_edge_count;
    text_log.Print("v.Edges[%u] = ", vertex_edge_count);
    prefix[0] = '{';
    prefix[1] = MYON_String::Space;
    prefix[2] = error_code_point;
    prefix[3] = 'e';
    prefix[4] = '%';
    prefix[5] = 'u';
    prefix[6] = 0;
    for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
    {
      if (1 == vei)
        prefix[0] = ',';
      prefix[2] = error_code_point;
      const MYON_SubDEdge* e = v->Edge(vei);
      if (nullptr != e)
      {
        if (v == e->m_vertex[0] && v != e->m_vertex[1])
          prefix[2] = '+';
        else if (v != e->m_vertex[0] && v == e->m_vertex[1])
          prefix[2] = '-';
        else
          vertex_error_count++;
        text_log.Print(prefix, e->m_id);
      }
      else
      {
        text_log.Print("%c %c", prefix[0], error_code_point);
        vertex_error_count++;
      }
    }
    text_log.Print(" }\n");

    const unsigned int vertex_face_count = v->m_face_count;
    text_log.Print("v.Faces[%u] = ", vertex_face_count);
    prefix[0] = '{';
    prefix[1] = MYON_String::Space;
    prefix[2] = 'f';
    prefix[3] = '%';
    prefix[4] = 'u';
    prefix[5] = 0;
    for (unsigned int vfi = 0; vfi < vertex_face_count; vfi++)
    {
      if (1 == vfi)
        prefix[0] = ',';
      const MYON_SubDFace* f = v->Face(vfi);
      if (nullptr != f)
        text_log.Print(prefix, f->m_id);
      else
      {
        text_log.Print("%c %c", prefix[0], error_code_point);
        vertex_error_count++;
      }
    }
    text_log.Print(" }\n");
    text_log.PopIndent();
  }

  text_log.PushIndent();
  if (vertex_dump_count > 0 && vertex_dump_count < vertex_count)
  {
    text_log.Print(
      L"... %u additional vertices (v%u to v%u).\n", 
      vertex_count-vertex_dump_count,
      skipped_vertex_id.i,
      skipped_vertex_id.j
    );
  }
  text_log.Print("Maximum vertex id = %u.  ",max_vertex_id);
  if (validate_max_vertex_id >= max_vertex_id)
    text_log.Print("Next id = %u.\n", validate_max_vertex_id + 1);
  else
    text_log.Print("ERROR Next id = %u.\n", validate_max_vertex_id + 1);
  text_log.PopIndent();

  ///////////////////////////////////////////////////////////////////
  //
  // Edge Topology
  // eN (+vA, -vB)
  //   eFaces[edge_face_count] = { fP, fQ, fR, ... }
  //
  damaged_edge_count = 0U;
  wire_edge_count = 0U;
  nonmanifold_edge_count = 0U;
  unsigned int edge_count = 0;
  unsigned int edge_dump_count = 0;
  MYON_2udex skipped_edge_id = MYON_2udex::Zero;
  unsigned int max_edge_id = 0;
  bSkippedPreviousComponent = false;
  for (const MYON_SubDEdge* e = m_edge[0]; nullptr != e; e = e->m_next_edge)
  {
    if (edge_count >= m_edge_count && e->SubdivisionLevel() != level_index)
      break;
    if (e->m_id > max_edge_id)
      max_edge_id = e->m_id;
    edge_count++;

    const bool bIsDamaged = e->m_status.IsDamaged();
    if (bIsDamaged)
      ++damaged_edge_count;
    const bool bIsWireEdge = (0U == e->m_face_count);
    if (bIsWireEdge)
      ++wire_edge_count;
    const bool bIsNonmanifoldEdge = (e->m_face_count >= 3U);
    if (bIsNonmanifoldEdge)
      ++nonmanifold_edge_count;

    if (
      bEdgeIdTest 
      && (false == bIsDamaged || damaged_edge_count > damaged_id_list_capacity)
      && (false == bIsWireEdge || wire_edge_count > damaged_id_list_capacity)
      && (false == bIsNonmanifoldEdge || nonmanifold_edge_count > damaged_id_list_capacity)
      )
    {
      bool bSkip = true;
      for (;;)
      {
        if (MYON_UNSET_UINT_INDEX == edge_id_range.i)
          break;
        if (edge_max_dump_count > 0)
        {
          if (edge_count > edge_max_dump_count)
            break;
        }
        else
        {
          if (e->m_id < edge_id_range.i || e->m_id >= edge_id_range.j)
            break;
        }
        bSkip = false;
        break;
      }
      if (bSkip)
      {
        if (0 == skipped_edge_id.j)
        {
          skipped_edge_id.i = e->m_id;
          skipped_edge_id.j = e->m_id;
        }
        else if (e->m_id < skipped_edge_id.i)
          skipped_edge_id.i = e->m_id;
        else if (e->m_id > skipped_edge_id.j)
          skipped_edge_id.j = e->m_id;
        bSkippedPreviousComponent = true;
        continue;
      }
    }

    if (0 == edge_dump_count)
      text_log.Print(L"Edges:\n");
    edge_dump_count++;
    MYON_TextLogIndent eindent(text_log);

    const MYON_wString etag = MYON_SubD::EdgeTagToString(e->m_edge_tag,false);

    if (bSkippedPreviousComponent)
    {
      text_log.Print(L"...\n");
      bSkippedPreviousComponent = false;
    }

    {
      MYON_String s = MYON_String::FormatToString("e%u: ", e->m_id);
      if (bIsDamaged)
        s += "(DAMAGED) ";

      if (bIsWireEdge)
      {
        s += MYON_String::FormatToString(
          "wire %ls ",
          static_cast<const wchar_t*>(etag)
        );
      }
      else if (bIsNonmanifoldEdge)
      {
        s += MYON_String::FormatToString(
          "nonmanifold %ls ",
          static_cast<const wchar_t*>(etag)
        );
      }
      else
      {
        s += MYON_String::FormatToString(
          "%ls ",
          static_cast<const wchar_t*>(etag)
        );
      }

      s += "( ";
      prefix[0] = 'v';
      prefix[1] = '%';
      prefix[2] = 'u';
      prefix[3] = 0;
      for (unsigned int evi = 0; evi < 2; evi++)
      {
        if (1 == evi)
          s += " to ";
        const MYON_SubDVertex* v = e->m_vertex[evi];
        if (nullptr != v)
          s += MYON_String::FormatToString(prefix, v->m_id);
        else
        {
          s += error_code_point;
          edge_error_count++;
        }
      }
      s += " )";

      if (e->m_group_id > 0)
        s += MYON_String::FormatToString(" group_id=%u", e->m_group_id);

      text_log.PrintString(s);
      text_log.PrintNewLine();
    }

    text_log.PushIndent();

    MYON_3dPoint P1(MYON_3dPoint::NanPoint);
    if (e->GetSavedSubdivisionPoint(&P1.x) && P1.IsValid())
      text_log.Print( "e.SubdivisionPoint: (%g, %g, %g)\n", P1.x, P1.y, P1.z );

    const unsigned int edge_face_count = e->m_face_count;
    text_log.Print("e.Faces[%u] = ", edge_face_count);
    prefix[0] = '{';
    prefix[1] = MYON_String::Space;
    prefix[2] = error_code_point;
    prefix[3] = 'f';
    prefix[4] = '%';
    prefix[5] = 'u';
    prefix[6] = 0;
    for (unsigned int efi = 0; efi < edge_face_count; efi++)
    {
      if (1 == efi)
        prefix[0] = ',';
      prefix[2] = error_code_point;
      MYON_SubDFacePtr fptr = e->FacePtr(efi);
      const MYON_SubDFace* f = fptr.Face();
      const MYON__UINT_PTR edge_fdir = fptr.FaceDirection();
      if (nullptr != f)
      {
        MYON_SubDEdgePtr eptr = f->EdgePtrFromEdge(e);
        if (eptr.Edge() == e && eptr.EdgeDirection() == edge_fdir)
          prefix[2] = ((0 == edge_fdir) ? '+' : '-');
        else
          edge_error_count++;
        text_log.Print(prefix, f->m_id);
      }
      else
      {
        text_log.Print("%c %c", prefix[0], error_code_point);
        edge_error_count++;
      }
    }
    text_log.Print(" }\n");


    text_log.PopIndent();
  }

  text_log.PushIndent();
  if (edge_dump_count > 0 && edge_dump_count < edge_count)
  {
    text_log.Print(L"... %u additional edges (e%u to e%u).\n",
      edge_count - edge_dump_count,
      skipped_edge_id.i,
      skipped_edge_id.j
    );
  }
  text_log.Print("Maximum edge id = %u.  ",max_edge_id);
  if (validate_max_edge_id >= max_edge_id)
    text_log.Print("Next id = %u.\n", validate_max_edge_id + 1);
  else
    text_log.Print("ERROR Next id = %u.\n", validate_max_edge_id + 1);
  text_log.PopIndent();


  ///////////////////////////////////////////////////////////////////
  //
  // Face Topology
  // fN
  //   fEdges[face_edge_count] = { +eA, -eB, +eC, ...}
  //   fVertices[face_edge_count] = { vP, vQ, vR, ... }
  //
  damaged_face_count = 0U;
  face_count = 0;
  unsigned int face_dump_count = 0;
  MYON_2udex skipped_face_id = MYON_2udex::Zero;
  unsigned int max_face_id = 0;
  bSkippedPreviousComponent = false;
  for (const MYON_SubDFace* f = m_face[0]; nullptr != f; f = f->m_next_face)
  {
    if (face_count >= m_face_count && f->SubdivisionLevel() != level_index)
      break;
    if (f->m_id > max_face_id)
      max_face_id = f->m_id;
    face_count++;

    const bool bIsDamaged = f->m_status.IsDamaged();
    if (bIsDamaged)
      ++damaged_face_count;

    if (bFaceIdTest && (false == bIsDamaged || damaged_face_count > damaged_id_list_capacity))
    {
      bool bSkip = true;
      for (;;)
      {
        if (MYON_UNSET_UINT_INDEX == face_id_range.i)
          break;
        if (face_max_dump_count > 0)
        {
          if (face_count > face_max_dump_count)
            break;
        }
        else
        {
          if (f->m_id < face_id_range.i || f->m_id >= face_id_range.j)
            break;
        }
        bSkip = false;
        break;
      }
      if (bSkip)
      {
        if (0 == skipped_face_id.j)
        {
          skipped_face_id.i = f->m_id;
          skipped_face_id.j = f->m_id;
        }
        else if (f->m_id < skipped_face_id.i)
          skipped_face_id.i = f->m_id;
        else if (f->m_id > skipped_face_id.j)
          skipped_face_id.j = f->m_id;
        bSkippedPreviousComponent = true;
        continue;
      }
    }

    if (0 == face_dump_count)
      text_log.Print(L"Faces:\n");
    face_dump_count++;
    MYON_TextLogIndent eindent(text_log);

    if (bSkippedPreviousComponent)
    {
      text_log.Print(L"...\n");
      bSkippedPreviousComponent = false;
    }
    if (bIsDamaged)
    {
      text_log.Print(
        "f%u (DAMAGED):\n",
        f->m_id
      );
    }
    else
    {
      MYON_String s = MYON_String::FormatToString("f%u:", f->m_id);

      if (f->m_group_id > 0)
        s += MYON_String::FormatToString(" group_id=%u", f->m_group_id);
      text_log.PrintString(s);
      text_log.PrintNewLine();
    }

    text_log.PushIndent();

    MYON_3dPoint P1(MYON_3dPoint::NanPoint);
    if (f->GetSavedSubdivisionPoint(&P1.x) && P1.IsValid())
      text_log.Print( "f.SubdivisionPoint: (%g, %g, %g)\n", P1.x, P1.y, P1.z );


    const unsigned int face_edge_count = f->m_edge_count;
    text_log.Print("f.Edges[%u] = ", face_edge_count);
    prefix[0] = '{';
    prefix[1] = MYON_String::Space;
    prefix[2] = error_code_point;
    prefix[3] = 'e';
    prefix[4] = '%';
    prefix[5] = 'u';
    prefix[6] = 0;
    for (unsigned int fei = 0; fei < face_edge_count; fei++)
    {
      if (1 == fei)
        prefix[0] = ',';
      prefix[2] = error_code_point;
      const MYON_SubDEdgePtr eptr = f->EdgePtr(fei);
      const MYON_SubDEdge* e = eptr.Edge();
      const MYON__UINT_PTR face_edir = eptr.EdgeDirection();
      if (nullptr != e)
      {
        MYON_SubDFacePtr fptr = e->FacePtrFromFace(f);
        if (fptr.Face() == f && fptr.FaceDirection() == face_edir)
          prefix[2] = ((0 == face_edir) ? '+' : '-');
        else
          face_error_count++;
        text_log.Print(prefix, e->m_id);
      }
      else
      {
        text_log.Print("%c %c", prefix[0], error_code_point);
        face_error_count++;
      }
    }
    text_log.Print(" }\n");

    text_log.Print("f.Vertices[%u] = ", face_edge_count);
    prefix[0] = '{';
    prefix[1] = MYON_String::Space;
    prefix[2] = 'v';
    prefix[3] = '%';
    prefix[4] = 'u';
    prefix[5] = 0;
    for (unsigned int fvi = 0; fvi < face_edge_count; fvi++)
    {
      if (1 == fvi)
        prefix[0] = ',';
      const MYON_SubDVertex* v = f->Vertex(fvi);
      if (nullptr != v)
        text_log.Print(prefix, v->m_id);
      else
      {
        text_log.Print("%c %c", prefix[0], error_code_point);
        face_error_count++;
      }
    }
    text_log.Print(" }\n");


    if (f->TexturePointsAreSet())
    {
      text_log.Print("f.TexturePoints[%u] = {", face_edge_count);
      prefix[0] = MYON_String::Space;
      prefix[1] = 0;
      for (unsigned int fei = 0; fei < face_edge_count; fei++)
      {
        if (1 == fei)
        {
          prefix[0] = ',';
          prefix[1] = MYON_String::Space;
          prefix[2] = 0;
        }
        const MYON_3dPoint tp = f->TexturePoint(fei);
        text_log.Print("%s(%g,%g,%g)", prefix, tp.x, tp.y, tp.z);
      }
      text_log.Print(" }\n");
    }



    bool bNeedComma = false;

    const MYON_Color per_face_color = f->PerFaceColor();
    if (MYON_Color::UnsetColor != per_face_color)
    {
      if (bNeedComma)
        text_log.Print(", ");
      else
        text_log.Print("Per face");
      bNeedComma = true;
      text_log.Print("face color=(");
      text_log.PrintColor(per_face_color);
      text_log.Print(")");
    }

    const int per_face_material_channel_index = f->MaterialChannelIndex();
    if (0 != per_face_material_channel_index)
    {
      if (bNeedComma)
        text_log.Print(", ");
      else
        text_log.Print("Per face");
      bNeedComma = true;
      text_log.Print(" material channel index=%d", per_face_material_channel_index);
    }
    if (bNeedComma)
      text_log.PrintNewLine();

    if (f->PackRectIsSet())
    {
      bNeedComma = true;
      const bool bGridOrder = true;
      MYON_2dPoint corners[4] = {
        f->PackRectCorner(bGridOrder,0),
        f->PackRectCorner(bGridOrder,1),
        f->PackRectCorner(bGridOrder,2),
        f->PackRectCorner(bGridOrder,3)
      };
      text_log.Print("f.PackId = %u Pack rectangle corners: (%g,%g), (%g,%g), (%g,%g), (%g,%g)",
        f->PackId(),
        corners[0].x, corners[0].y,
        corners[1].x, corners[1].y,
        corners[2].x, corners[2].y,
        corners[3].x, corners[3].y
      );
    }
    else
    {
      text_log.Print("Pack rectangle is not set.");
    }
    text_log.PrintNewLine();

    if (false == text_log.IsTextHash())
    {
      const MYON_SubDMeshFragment* first_frag = f->MeshFragments();
      if (nullptr != first_frag)
      {
        text_log.Print(L"Fragments:\n");
        const MYON_TextLogIndent indent(text_log);
        for (const MYON_SubDMeshFragment* frags = first_frag; nullptr != frags; frags = frags->NextFaceFragment(false))
          frags->Dump(text_log);
      }
    }

    text_log.PopIndent();
  }

  text_log.PushIndent();
  if (face_dump_count > 0 && face_dump_count < face_count)
  {
    text_log.Print(
      L"... %u additional faces (f%u to f%u).\n",
      face_count - face_dump_count,
      skipped_face_id.i,
      skipped_face_id.j
    );
  }
  text_log.Print("Maximum face id = %u.  ",max_face_id);
  if (validate_max_face_id >= max_face_id)
    text_log.Print("Next id = %u.\n", validate_max_face_id + 1);
  else
    text_log.Print("ERROR Next id = %u.\n", validate_max_face_id + 1);
  text_log.PopIndent();

 
  const unsigned int topology_error_count
    = vertex_error_count
    + edge_error_count
    + face_error_count;

  text_log.PushIndent();
  if (0 == topology_error_count)
  {
    text_log.Print("No topology inconsistencies.\n");
  }
  else
  {
    text_log.Print(
      "ERRORS: %u vertex, %u edge, %u face topology inconsistencies marked with \"%c\".\n", 
      vertex_error_count,
      edge_error_count,
      face_error_count, 
      error_code_point
    );
  }
  text_log.PopIndent();

  return topology_error_count;
}

static void Internal_DumpFragmentArray(MYON_TextLog& text_log, const wchar_t* description, size_t dim, const double* a, unsigned count, size_t stride)
{
  if (nullptr != a && count > 0 && dim > 0 && stride >= dim)
  {
    text_log.Print(L"%ls = ", description);
    if (4 == count)
    {
      text_log.Print("{");
      for (unsigned i = 0; i < count; ++i)
      {
        if (0 != i)
          text_log.Print(L",");
        text_log.Print(L"(%g", a[0]);
        for (unsigned j = 1; j < dim; ++j)
          text_log.Print(L",%g", a[j]);
        text_log.Print(L")");
        a += stride;
      }
      text_log.Print("}");
    }
    else
    {
      for (size_t i = 0; i < dim; ++i)
        text_log.Print(L"%g,", a[i]);
      MYON_SHA1 sha1;
      Internal_AccumulateFragmentArrayHash(sha1, dim, a, count, stride);
      const MYON_wString s = sha1.Hash().ToString(true);
      text_log.Print(L" ... SHA1 hash=%ls", static_cast<const wchar_t*>(s));
    }
  }
  else
  {
    text_log.Print(L"%ls: Not set.",description);
  }
  text_log.PrintNewLine();
}

void MYON_SubDMeshFragment::Dump(MYON_TextLog& text_log) const
{
  const unsigned count = VertexCount();
  text_log.Print("MYON_SubDMeshFragment: vertex count = %u", count);
  const unsigned n = m_grid.SideSegmentCount();
  const unsigned grid_point_count = m_grid.GridPointCount();
  if (count > 0)
  {
    if (n > 0 && count == grid_point_count)
    {
      text_log.Print(
        L", %u x %u grid\n",
        n, n
      );

      MYON_TextLogIndent indent1(text_log);
      text_log.Print(
        L"bounding box (%g to %g X %g to %g X %g to %g)\n",
        m_surface_bbox.m_min.x, m_surface_bbox.m_max.x,
        m_surface_bbox.m_min.y, m_surface_bbox.m_max.y,
        m_surface_bbox.m_min.z, m_surface_bbox.m_max.z
      );

      text_log.Print(
        L"pack rect (%g,%g),(%g,%g),(%g,%g),(%g,%g) \n",
        m_pack_rect[0][0], m_pack_rect[0][1],
        m_pack_rect[1][0], m_pack_rect[1][1],
        m_pack_rect[2][0], m_pack_rect[2][1],
        m_pack_rect[3][0], m_pack_rect[3][1]
      );

      for (int pass = 0; pass < 2; ++pass)
      {
        const MYON_SubDComponentLocation cl = (0 == pass) ? MYON_SubDComponentLocation::ControlNet : MYON_SubDComponentLocation::Surface;
        if (MYON_SubDComponentLocation::ControlNet == cl)
          text_log.Print("Corners:\n");
        else if (MYON_SubDComponentLocation::Surface == cl)
          text_log.Print("Surface:\n");
        MYON_TextLogIndent ident2(text_log);
        Internal_DumpFragmentArray(
          text_log, L"points", 3,
          PointArray(cl),
          PointArrayCount(cl),
          PointArrayStride(cl));
        Internal_DumpFragmentArray(
          text_log, L"normals", 3,
          NormalArray(cl),
          NormalArrayCount(cl),
          NormalArrayStride(cl));
        Internal_DumpFragmentArray(
          text_log, L"texture coordinates", 3, // tcs are uvw
          TextureCoordinateArray(cl),
          TextureCoordinateArrayCount(cl),
          TextureCoordinateArrayStride(cl));
        Internal_DumpFragmentArray(
          text_log, L"curvatures", 2, // 2 principal
          (const double*)(CurvatureArray(cl)),
          CurvatureArrayCount(cl),
          sizeof(MYON_SurfaceCurvature)/sizeof(double)
        );
      }
    }
    else
    {
      text_log.Print(L". Invalid fragment.\n");
    }
  }
  else
  {
    text_log.PrintNewLine();
  }
}



//virtual
unsigned int MYON_SubD::SizeOf() const
{
  size_t sz = MYON_Geometry::SizeOf();
  sz += sizeof(*this) - sizeof(MYON_Geometry);
  const MYON_SubDimple* subdimple = SubDimple();
  if ( subdimple )
    sz += subdimple->SizeOf();
  return (unsigned int)sz;
}

size_t MYON_SubD::SizeOfAllElements() const
{
  const MYON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->SizeOfAllElements() : 0;
}

size_t MYON_SubD::SizeOfActiveElements() const
{
  const MYON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->SizeOfActiveElements() : 0;
}

size_t MYON_SubD::SizeOfUnusedElements() const
{
  const MYON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->SizeOfUnusedElements() : 0;
}

size_t MYON_SubD::SizeOfAllMeshFragments() const
{
  const MYON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->SizeOfAllMeshFragments() : 0;
}

size_t MYON_SubD::SizeOfActiveMeshFragments() const
{
  const MYON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->SizeOfActiveMeshFragments() : 0;
}

size_t MYON_SubD::SizeOfUnusedMeshFragments() const
{
  const MYON_SubDimple* subdimple = SubDimple();
  return (nullptr != subdimple) ? subdimple->SizeOfUnusedMeshFragments() : 0;
}

//virtual
MYON__UINT32 MYON_SubD::DataCRC(MYON__UINT32 current_remainder) const
{
  return 0;
}

//virtual
MYON::object_type MYON_SubD::ObjectType() const
{
  return MYON::subd_object;
}

//virtual
void MYON_SubD::DestroyRuntimeCache( bool bDelete )
{
  const MYON_SubDimple* dimple = SubDimple();
  if (nullptr != dimple)
  {
    unsigned int level_count = dimple->LevelCount();
    for (unsigned int level_index = 0; level_index < level_count; level_index++)
    {
      const MYON_SubDLevel* level = dimple->SubDLevel(level_index);
      if (level)
      {
        level->ClearEvaluationCache();
        level->MarkAggregateComponentStatusAsNotCurrent();
      }
    }
    dimple->ChangeGeometryContentSerialNumber(false);
  }
  return;
}

//virtual 
int MYON_SubD::Dimension() const
{
  return 3;
}

//virtual
bool MYON_SubD::GetBBox(
        double* boxmin,
        double* boxmax,
        bool bGrowBox
        ) const
{ 
  int j;
  for ( j = 0; j < 3 && bGrowBox; j++ )
  {
    if ( !MYON_IsValid(boxmin[j]) || !MYON_IsValid(boxmax[j]) || boxmin[j] > boxmax[j])
      bGrowBox = false;
  }

  MYON_BoundingBox bbox = MYON_BoundingBox::EmptyBoundingBox;
  bool rc = false;

  // GetBBox must always returns the control net box - it contains both the surface and the control net.
  bbox = ActiveLevel().ControlNetBoundingBox();
  rc = bbox.IsValid();
  if (rc)
  {
    if (bGrowBox)
    {
      if (bbox.m_min.x < boxmin[0]) boxmin[0] = bbox.m_min.x;
      if (bbox.m_max.x > boxmax[0]) boxmax[0] = bbox.m_max.x;
      if (bbox.m_min.y < boxmin[1]) boxmin[1] = bbox.m_min.y;
      if (bbox.m_max.y > boxmax[1]) boxmax[1] = bbox.m_max.y;
      if (bbox.m_min.z < boxmin[2]) boxmin[2] = bbox.m_min.z;
      if (bbox.m_max.z > boxmax[2]) boxmax[2] = bbox.m_max.z;
    }
    else
    {
      boxmin[0] = bbox.m_min.x; boxmin[1] = bbox.m_min.y; boxmin[2] = bbox.m_min.z;
      boxmax[0] = bbox.m_max.x; boxmax[1] = bbox.m_max.y; boxmax[2] = bbox.m_max.z;
    }
  }

  return (rc || bGrowBox);
}

//virtual
void MYON_SubD::ClearBoundingBox()
{
  // For MYON_SubD, MYON_SubD::ClearBoundingBox() and MYON_SubD::DestroyRuntimeCache(true)

  MYON_SubD::DestroyRuntimeCache(true);
}

//virtual
bool MYON_SubD::Transform( 
        const MYON_Xform& xform
        )
{
  if ( this == &MYON_SubD::Empty)
    return true;  // transform applied to empty subd is true on purpose

  // userdata transformation, etc.
  if (!this->MYON_Geometry::Transform(xform))
    return false;

  MYON_SubDimple* subdimple = SubDimple(false);
  if ( 0 == subdimple )
    return true;  // transform applied to empty subd is true on purpose

  return subdimple->Transform(xform);
}

//virtual
bool MYON_SubD::IsDeformable() const
{
  return true;
}

//virtual
bool MYON_SubD::MakeDeformable()
{
  return true;
}

//virtual
bool MYON_SubD::SwapCoordinates(
      int i,
      int j
      )
{
  return false;
}

//virtual
bool MYON_SubD::HasBrepForm() const
{
  return false;
}

//virtual
MYON_Brep* MYON_SubD::BrepForm( MYON_Brep* destination_brep) const
{
  return nullptr;
}

//virtual
MYON_COMPONENT_INDEX MYON_SubD::ComponentIndex() const
{
  return MYON_Geometry::ComponentIndex();
}

//virtual
bool MYON_SubD::EvaluatePoint( const class MYON_ObjRef& objref, MYON_3dPoint& P ) const
{
  return false;
}

//////////////////////////////////////////////////////////////
//
//
//

class MYON_SubDHeap* MYON_SubD::Internal_Heap() const
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? &subdimple->Heap() : nullptr;
}

bool MYON_SubD::InSubD(const class MYON_SubDVertex* vertex) const
{
  return InSubD(MYON_SubDComponentPtr::Create(vertex));
}

bool MYON_SubD::InSubD(const class MYON_SubDEdge* edge) const
{
  return InSubD(MYON_SubDComponentPtr::Create(edge));
}

bool MYON_SubD::InSubD(const class MYON_SubDFace* face) const
{
  return InSubD(MYON_SubDComponentPtr::Create(face));
}

bool MYON_SubD::InSubD(MYON_SubDComponentPtr cptr) const
{
  const MYON_SubDHeap* h = this->Internal_Heap();
  return (nullptr != h) ? h->InHeap(cptr) : false;
}

const MYON_SubDComponentPtr MYON_SubD::InSubD(const MYON_SubDComponentBase* b) const
{
  const MYON_SubDHeap* h = this->Internal_Heap();
  return (nullptr != h) ? h->InHeap(b) : MYON_SubDComponentPtr::Null;
}


bool MYON_SubDHeap::InHeap(MYON_SubDComponentPtr cptr) const
{
  const MYON_FixedSizePool* fsp = this->Internal_ComponentFixedSizePool(cptr.ComponentType());
  return (nullptr != fsp) ? fsp->InPool(cptr.ComponentBase()) : false;
}

const MYON_SubDComponentPtr MYON_SubDHeap::InHeap(const class MYON_SubDComponentBase* b) const
{
  if (nullptr != b)
  {
    MYON_SubDComponentPtr::Type t[3] = {
      MYON_SubDComponentPtr::Type::Vertex,
      MYON_SubDComponentPtr::Type::Edge,
      MYON_SubDComponentPtr::Type::Face
    };
    for (int i = 0; i < 3; ++i)
    {
      const MYON_FixedSizePool* fsp = this->Internal_ComponentFixedSizePool(t[i]);
      if (nullptr != fsp && fsp->InPool(b))
      {
        switch (t[i])
        {
        case MYON_SubDComponentPtr::Type::Vertex:
          return MYON_SubDComponentPtr::Create((const MYON_SubDVertex*)b);
          break;
        case MYON_SubDComponentPtr::Type::Edge:
          return MYON_SubDComponentPtr::Create((const MYON_SubDEdge*)b);
          break;
        case MYON_SubDComponentPtr::Type::Face:
          return MYON_SubDComponentPtr::Create((const MYON_SubDFace*)b);
          break;
        case MYON_SubDComponentPtr::Type::Unset:
          break;
        }
      }
    }
  }
  return MYON_SubDComponentPtr::Null;
}

const class MYON_SubDLevel& MYON_SubD::ActiveLevel() const
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ActiveLevel() : MYON_SubDLevel::Empty;
}

class MYON_SubDLevel const * MYON_SubD::ActiveLevelConstPointer() const
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ActiveLevelPointer() : nullptr;
}

class MYON_SubDLevel* MYON_SubD::ActiveLevelPointer()
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  return (nullptr != subdimple) ? subdimple->ActiveLevelPointer() : nullptr;
}


MYON_SubDimple* MYON_SubD::SubDimple(bool bCreateIfNeeded)
{
  MYON_SubDimple* subdimple = m_subdimple_sp.get();
  if (nullptr == subdimple && bCreateIfNeeded)
  {
    subdimple = new MYON_SubDimple();
    m_subdimple_sp = std::shared_ptr<MYON_SubDimple>(subdimple);
  }
  return subdimple;
}

const class MYON_SubDimple* MYON_SubD::SubDimple() const
{
  return m_subdimple_sp.get();
}

unsigned int MYON_SubD::SubDimpleUseCount() const
{
  return (unsigned int)m_subdimple_sp.use_count();
}

void MYON_SubD::ShareDimple(const MYON_SubD& other_subd)
{
  if (m_subdimple_sp.get() != other_subd.m_subdimple_sp.get())
  {
    m_subdimple_sp.reset();
    m_subdimple_sp = other_subd.m_subdimple_sp;
  }
}

void MYON_SubD::ShareDimple(const class MYON_SubDMeshImpl& subd_limple)
{
  std::shared_ptr<MYON_SubDimple> limple_sp(subd_limple.m_subdimple_wp.lock());

  const MYON_SubDimple* subd_imple = m_subdimple_sp.get();

  if (nullptr == limple_sp.get())
  {
    // weak pointer is nullptr, meaning there are no known references to the
    // subd used to create this limit mesh.
    const_cast<MYON_SubDMeshImpl&>(subd_limple).ClearFragmentFacePointers(true);
  }

  if (subd_imple != limple_sp.get())
  {
    m_subdimple_sp.reset();
    m_subdimple_sp = limple_sp;
  }
}

void MYON_SubD::SwapDimple(class MYON_SubDMeshImpl& subd_limple )
{
  std::shared_ptr <MYON_SubDimple> limple_sp(subd_limple.m_subdimple_wp.lock());
  if (m_subdimple_sp.get() != limple_sp.get())
  {
    m_subdimple_sp.swap(limple_sp);
    subd_limple.m_subdimple_wp = limple_sp;
  }
}


void MYON_SubD::SwapDimple(MYON_SubD& other_subd)
{
  if (this != &other_subd)
  {
    m_subdimple_sp.swap(other_subd.m_subdimple_sp);
  }
}

void MYON_SubD::Clear()
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if ( subdimple )
    subdimple->Clear();
}

unsigned int MYON_SubD::ClearHigherSubdivisionLevels(
  unsigned int max_level_index
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  return
    (nullptr != subdimple)
    ? subdimple->ClearHigherSubdivisionLevels(max_level_index)
    : 0U;
}

unsigned int MYON_SubD::ClearLowerSubdivisionLevels(
  unsigned int min_level_index
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  return
    (nullptr != subdimple)
    ? subdimple->ClearLowerSubdivisionLevels(min_level_index)
    : 0U;
}

unsigned MYON_SubD::ClearInactiveLevels()
{
  MYON_SubDimple* subdimple = SubDimple(false);
  return
    (nullptr != subdimple)
    ? subdimple->ClearInactiveLevels()
    : 0U;
}

void MYON_SubD::Destroy()
{
  m_subdimple_sp.reset();
}

class MYON_SubDVertex* MYON_SubD::AddVertex(
  const double* P
)
{
  return AddVertex(MYON_SubDVertexTag::Unset, P);
}

class MYON_SubDVertex* MYON_SubD::AddVertex(
  MYON_SubDVertexTag vertex_tag,
  const double* P
  )
{
  MYON_SubDimple* subdimple = SubDimple(true);
  if ( 0 == subdimple )
    return 0;
  const unsigned int level_index = subdimple->ActiveLevelIndex();
  class MYON_SubDVertex* v = subdimple->AllocateVertex(vertex_tag, level_index, P); // 0 = level
  subdimple->AddVertexToLevel(v);
  return v;
}

MYON_SubDVertex* MYON_SubD::AddVertexForExperts(
  unsigned int candidate_vertex_id,
  MYON_SubDVertexTag vertex_tag,
  const double* P,
  unsigned int initial_edge_capacity,
  unsigned int initial_face_capacity
)
{
  MYON_SubDimple* subdimple = SubDimple(true);
  if (0 == subdimple)
    return 0;
  MYON_SubDVertex* v = subdimple->AllocateVertex( candidate_vertex_id, vertex_tag, subdimple->ActiveLevelIndex(), P, initial_edge_capacity, initial_face_capacity);
  subdimple->AddVertexToLevel(v);
  return v;
}

bool MYON_SubD::ReturnVertexForExperts(
  MYON_SubDVertex* v
)
{
  if (nullptr == v)
    return false;

  if (this->InSubD(v) && v->IsActive() && 0 == v->m_edge_count && 0 == v->m_face_count )
  {
    MYON_SubDimple* subdimple = SubDimple(false);
    if (nullptr != subdimple)
    {
      subdimple->ReturnVertex(v);
      return true;
    }
  }

  // Caller is not an expert but a crash has been prevented.
  return MYON_SUBD_RETURN_ERROR(false);
}

class MYON_SubDEdge* MYON_SubDimple::AddEdge(
  MYON_SubDEdgeTag edge_tag,
  MYON_SubDVertex* v0,
  double v0_sector_coefficient,
  MYON_SubDVertex* v1,
  double v1_sector_coefficient
)
{
  return AddEdge(
    0U,
    edge_tag,
    v0,
    v0_sector_coefficient,
    v1,
    v1_sector_coefficient,
    0U
  );
}

class MYON_SubDEdge* MYON_SubDimple::AddEdge(
  unsigned int candidate_edge_id,
  MYON_SubDEdgeTag edge_tag,
  MYON_SubDVertex* v0,
  double v0_sector_coefficient,
  MYON_SubDVertex* v1,
  double v1_sector_coefficient,
  unsigned initial_face_capacity
)
{
  if (false == MYON_SubDSectorType::IsValidSectorCoefficientValue(v0_sector_coefficient, true))
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (false == MYON_SubDSectorType::IsValidSectorCoefficientValue(v1_sector_coefficient, true))
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (nullptr != v0 && nullptr != v1 && v0->SubdivisionLevel() != v1->SubdivisionLevel())
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const bool bEdgeTagSet = MYON_SubD::EdgeTagIsSet(edge_tag);

  if (bEdgeTagSet
    && MYON_SubDSectorType::IgnoredSectorCoefficient != v0_sector_coefficient
    && MYON_SubDSectorType::UnsetSectorCoefficient != v0_sector_coefficient
    && nullptr != v0
    && MYON_SubDVertexTag::Smooth == v0->m_vertex_tag
    )
  {
    // minimizes checking when building subds because constant crease coefficients can be passed in
    v0_sector_coefficient = MYON_SubDSectorType::IgnoredSectorCoefficient;
  }

  if (bEdgeTagSet
    && MYON_SubDSectorType::IgnoredSectorCoefficient != v1_sector_coefficient
    && MYON_SubDSectorType::UnsetSectorCoefficient != v1_sector_coefficient
    && nullptr != v1
    && MYON_SubDVertexTag::Smooth == v1->m_vertex_tag
    )
  {
    // minimizes checking when building subds because constant crease coefficients can be passed in
    v1_sector_coefficient = MYON_SubDSectorType::IgnoredSectorCoefficient;
  }

  class MYON_SubDEdge* e = AllocateEdge(candidate_edge_id, edge_tag, 0, 0);
  if (nullptr == e)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (nullptr != v0)
    e->SetSubdivisionLevel(v0->SubdivisionLevel());
  else if (nullptr != v1)
    e->SetSubdivisionLevel(v1->SubdivisionLevel());
  else if (ActiveLevelIndex() < MYON_UNSET_UINT_INDEX)
    e->SetSubdivisionLevel(ActiveLevelIndex());

  for (unsigned int i = 0; i < 2; i++)
  {
    MYON_SubDVertex* v = (i ? v1 : v0);
    double vertex_coefficient = (i ? v1_sector_coefficient : v0_sector_coefficient);
    e->m_vertex[i] = v;
    e->m_sector_coefficient[i] = vertex_coefficient;
    if (nullptr != v)
    {
      if (false == m_heap.GrowVertexEdgeArrayByOne(v))
      {
        v->m_status.SetDamagedState(true);
        ReturnEdge(e);
        return MYON_SUBD_RETURN_ERROR(nullptr);
      }
      v->m_edges[v->m_edge_count++] = MYON_SubDEdgePtr::Create(e, i);
    }
  }

  if ( nullptr == AddEdgeToLevel(e) )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (initial_face_capacity > 2)
  {
    m_heap.GrowEdgeFaceArray(e, initial_face_capacity);
  }

  return e;
}

MYON_SubDEdgeTag MYON_SubD::EdgeTagFromContext(
  unsigned int edge_face_count,
  const MYON_SubDVertex* v0,
  const MYON_SubDVertex* v1
)
{
  return
    (nullptr != v0 && nullptr != v1)
    ? MYON_SubD::EdgeTagFromContext(edge_face_count, v0->m_vertex_tag, v1->m_vertex_tag)
    : MYON_SubDEdgeTag::Unset;
}

MYON_SubDEdgeTag MYON_SubD::EdgeTagFromContext(
  unsigned int edge_face_count,
  const MYON_SubDVertexTag v0_tag,
  const MYON_SubDVertexTag v1_tag
)
{
  MYON_SubDEdgeTag edge_tag = MYON_SubDEdgeTag::Unset;

  for(;;)
  {
    if (edge_face_count > 0x7FFFU)
      break;

    if (1 == edge_face_count || edge_face_count >= 3 )
    {
      edge_tag = MYON_SubDEdgeTag::Crease;
      break;
    }

    const bool bSmooth0 = MYON_SubDVertexTag::Smooth == v0_tag;
    const bool bSmooth1 = MYON_SubDVertexTag::Smooth == v1_tag;

    if ( bSmooth0 || bSmooth1 )
    {
      if ( 2 == edge_face_count && bSmooth0 && bSmooth1)
        edge_tag = MYON_SubDEdgeTag::Smooth;
      break;
    }

    if ( MYON_SubD::VertexTagIsSet(v0_tag) && MYON_SubD::VertexTagIsSet(v1_tag) )
    {
      if (2 == edge_face_count)
        edge_tag = MYON_SubDEdgeTag::SmoothX;
      break;
    }

    break;
  }

  return edge_tag;
}

const MYON_SubDVertex* MYON_SubD::FindVertex(
  const double* control_net_point,
  double distance_tolerance
) const
{
  if (nullptr == control_net_point )
    return nullptr;
  const MYON_3dPoint P(control_net_point);
  if ( false == P.IsValid() )
    return nullptr;
  if ( false == (0.0 <= distance_tolerance))
    return nullptr;

  const MYON_SubDVertex* best_v = nullptr;
  MYON_SubDVertexIterator vit(*this);
  for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
  {
    const double d = P.DistanceTo(v->ControlNetPoint());
    if (0.0 == d)
      return v;
    if (d < distance_tolerance && (nullptr == best_v || d <= distance_tolerance) )
    {
      distance_tolerance = d;
      best_v = v;
    }
  }
  return best_v;
}

const MYON_SubDVertex* MYON_SubD::FindOrAddVertex(
  const double* control_net_point,
  double distance_tolerance
)
{
  if (nullptr == control_net_point)
    return nullptr;
  const MYON_3dPoint P(control_net_point);
  if (false == P.IsValid())
    return nullptr;
  if (false == (0.0 <= distance_tolerance))
    return nullptr;
  const MYON_SubDVertex* v = FindVertex(&P.x, distance_tolerance);
  return (nullptr != v) ? v : AddVertex(&P.x);
}

const MYON_SubDEdgePtr MYON_SubD::FindEdge(
  const class MYON_SubDVertex* v0,
  const class MYON_SubDVertex* v1
) const
{
  return MYON_SubDEdge::FromVertices(v0, v1);
}

const MYON_SubDEdgePtr MYON_SubD::FindOrAddEdge(
  class MYON_SubDVertex* v0,
  class MYON_SubDVertex* v1
)
{
  MYON_SubDEdgePtr eptr = MYON_SubDEdge::FromVertices(v0, v1);
  if (nullptr == eptr.Edge())
    eptr = MYON_SubDEdgePtr::Create(AddEdge(v0, v1), 0);
  return eptr;
}

class MYON_SubDEdge* MYON_SubD::AddEdge(
  MYON_SubDVertex* v0,
  MYON_SubDVertex* v1
)
{
  return MYON_SubD::AddEdge(MYON_SubDEdgeTag::Unset, v0, v1);
}

class MYON_SubDEdge* MYON_SubD::AddEdge(
  MYON_SubDEdgeTag edge_tag,
  MYON_SubDVertex* v0,
  MYON_SubDVertex* v1
  )
{
  // NO automatic edge tag setting - causes more problems than it helps.
  // Users can call MYON_SubD::EdgeTagFromContext() if they want to
  // preset the edge tag based on context.

  return AddEdgeWithSectorCoefficients(
    edge_tag,
    v0,
    MYON_SubDSectorType::UnsetSectorCoefficient,
    v1,
    MYON_SubDSectorType::UnsetSectorCoefficient
    );
}


class MYON_SubDEdge* MYON_SubD::AddEdge(
  MYON_SubDEdgeTag edge_tag,
  MYON_SubDVertex* v0,
  MYON_SubDVertex* v1,
  MYON_SubDEdgeSharpness sharpness
)
{
  // NO automatic edge tag setting - causes more problems than it helps.
  // Users can call MYON_SubD::EdgeTagFromContext() if they want to
  // preset the edge tag based on context.

  class MYON_SubDEdge* e = AddEdgeWithSectorCoefficients(
    edge_tag,
    v0,
    MYON_SubDSectorType::UnsetSectorCoefficient,
    v1,
    MYON_SubDSectorType::UnsetSectorCoefficient
  );
  if (nullptr != e && e->IsSmooth())
    e->SetSharpnessForExperts(sharpness);
  return e;
}


MYON_SubDEdge* MYON_SubD::AddEdgeWithSectorCoefficients(
  MYON_SubDEdgeTag edge_tag,
  class MYON_SubDVertex* v0,
  double v0_sector_coefficient,
  class MYON_SubDVertex* v1,
  double v1_sector_coefficient
)
{
  MYON_SubDimple* subdimple = SubDimple(true);
  if (nullptr != subdimple)
    return subdimple->AddEdge(edge_tag, v0, v0_sector_coefficient, v1, v1_sector_coefficient);
  return MYON_SUBD_RETURN_ERROR(nullptr);
}

class MYON_SubDEdge* MYON_SubD::AddEdgeForExperts(
  unsigned int candidate_edge_id,
  MYON_SubDEdgeTag edge_tag,
  class MYON_SubDVertex* v0,
  double v0_sector_coefficient,
  class MYON_SubDVertex* v1,
  double v1_sector_coefficient,
  unsigned int initial_face_capacity
)
{
  MYON_SubDimple* subdimple = SubDimple(true);
  if (nullptr != subdimple)
    return subdimple->AddEdge(candidate_edge_id, edge_tag, v0, v0_sector_coefficient, v1, v1_sector_coefficient, initial_face_capacity);
  return MYON_SUBD_RETURN_ERROR(nullptr);
}

bool MYON_SubD::ReturnEdgeForExperts(
  MYON_SubDEdge* e
)
{
  if (nullptr == e)
    return false;

  if (this->InSubD(e) && e->IsActive() && 0 == e->m_face_count && nullptr == e->m_vertex[0] && nullptr == e->m_vertex[1])
  {
    MYON_SubDimple* subdimple = SubDimple(false);
    if (nullptr != subdimple)
    {
      subdimple->ReturnEdge(e);
      return true;
    }
  }

  // Caller is not an expert but a crash has been prevented.
  return MYON_SUBD_RETURN_ERROR(false);
}


class MYON_SubDFace* MYON_SubDimple::AddFace(
  unsigned int edge_count,
  const MYON_SubDEdgePtr* edge
)
{
  return AddFace(0U, edge_count, edge);
}

class MYON_SubDFace* MYON_SubDimple::AddFace(
  unsigned int candidate_face_id,
  unsigned int edge_count,
  const MYON_SubDEdgePtr* edge
  )
{
  if ( edge_count > 0 && nullptr == edge)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  unsigned f_level = 0;
  bool bHaveLevel = false;
  for (unsigned int i = 0; i < edge_count; i++)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(edge[i].m_ptr);
    if (nullptr == e)
      continue;
    if (bHaveLevel)
    {
      if (e->SubdivisionLevel() != f_level)
        return MYON_SUBD_RETURN_ERROR(nullptr);
    }
    else
    {
      f_level = e->SubdivisionLevel();
      bHaveLevel = true;
    }
  }

  MYON_SubDFace* f = AllocateFace( candidate_face_id, 0, 0);
  if ( nullptr == f)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  f->SetSubdivisionLevel(f_level);

  if (edge_count > 0)
  {
    if (edge_count > 4)
    {
      if (false == m_heap.GrowFaceEdgeArray(f,edge_count))
      {
        ReturnFace(f);
        return MYON_SUBD_RETURN_ERROR(nullptr);
      }
    }

    MYON_SubDEdgePtr* f_edge = f->m_edge4;
    for (unsigned int i = 0; i < edge_count; i++)
    {
      if (4 == i)
        f_edge = f->m_edgex - 4;

      f_edge[i] = edge[i];
      MYON__UINT_PTR eptr = edge[i].m_ptr;
      MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr);
      if ( nullptr == e)
        continue;

      eptr = MYON_SUBD_EDGE_DIRECTION(eptr);

      MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>(e->m_vertex[eptr]);
      if (false == m_heap.GrowVertexFaceArrayByOne(v))
      {
        v->m_status.SetDamagedState(true);
        ReturnFace(f);
        return MYON_SUBD_RETURN_ERROR(nullptr);
      }
      v->m_faces[v->m_face_count++] = f;
      //if (1 == v->m_face_count)
      //{
      //  if (4 == f->m_edge_count)
      //    v->m_vertex_facet_type = MYON_SubD::VertexFacetType::Quad;
      //  else if (3 == f->m_edge_count)
      //    v->m_vertex_facet_type = MYON_SubD::VertexFacetType::Tri;
      //  else if ( f->m_edge_count > 4)
      //    v->m_vertex_facet_type = MYON_SubD::VertexFacetType::Ngon;
      //}
      //else
      //{
      //  const MYON_SubDFace* f0 = v->m_faces[0];
      //  if (nullptr == f0 || f0->m_edge_count != f->m_edge_count)
      //    v->m_vertex_facet_type = MYON_SubD::VertexFacetType::Mixed;
      //}
      //v->m_vertex_edge_order = MYON_SubD::VertexEdgeOrder::unset;

      MYON_SubDFacePtr* e_faces;
      if (e->m_face_count < 2)
      {
        e_faces = e->m_face2;
      }
      else
      {
        // Dale Lear, April 3, 2019 RH-49843 - we support non-manifold SubD objects now.
        //if (2 == e->m_face_count)
        //{
        //  // Getting this error in a valid situation means it is time
        //  // to support non-manifold subdivision objects.
        //  MYON_SubDIncrementErrorCount();
        //  MYON_WARNING("creating non-manifold subdivision object");
        //}
        if (false == m_heap.GrowEdgeFaceArrayByOne(e))
        {
          e->m_status.SetDamagedState(true);
          continue;
        }
        e_faces = e->m_facex - 2;
      }
      e_faces[e->m_face_count++] = MYON_SubDFacePtr::Create(f, eptr);
    }
    f->m_edge_count = (unsigned short)edge_count;
  }

  if ( nullptr == AddFaceToLevel(f))
    return MYON_SUBD_RETURN_ERROR(nullptr);

  return f;
}

unsigned int MYON_SubDimple::AllocateFaceTexturePoints(const MYON_SubDFace* face) const
{
  return const_cast<MYON_SubDimple*>(this)->m_heap.AllocateFaceTexturePoints(face);
}

void MYON_SubDimple::ReturnFaceTexturePoints(const MYON_SubDFace* face) const
{
  const_cast<MYON_SubDimple*>(this)->m_heap.ReturnFaceTexturePoints(face);
}


unsigned int MYON_SubDHeap::AllocateFaceTexturePoints(const MYON_SubDFace* face)
{
  if (nullptr == face)
    return false;
  unsigned texture_point_capacity = face->TexturePointsCapacity();
  if (texture_point_capacity < 3)
  {
    face->ClearTexturePoints();
    face->m_texture_points = this->Allocate3dPointArray(4 + face->m_edgex_capacity);
    texture_point_capacity = face->TexturePointsCapacity();
  }
  return texture_point_capacity;
}

void MYON_SubDHeap::ReturnFaceTexturePoints(const MYON_SubDFace* face)
{
  if (nullptr != face)
  {
    face->ClearTexturePoints();
    MYON_3dPoint* a = face->m_texture_points;
    face->m_texture_points = nullptr;
    this->Return3dPointArray(a);
  }
}

bool MYON_SubDEdge::UpdateEdgeSectorCoefficientsForExperts(bool bUnsetEdgeSectorCoefficientsOnly) const
{
  const double  input_sector_coefficient[2] = { m_sector_coefficient[0], m_sector_coefficient[1] };
  if (bUnsetEdgeSectorCoefficientsOnly)
  {
    if (input_sector_coefficient[0] >= 0.0 && input_sector_coefficient[0] <= 1.0
      && input_sector_coefficient[1] >= 0.0 && input_sector_coefficient[1] <= 1.0
      )
      return false;
  }
  m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;
  m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
  if (MYON_SubDEdgeTag::Smooth == m_edge_tag || MYON_SubDEdgeTag::SmoothX == m_edge_tag)
  {
    const unsigned int tagged_end_index = TaggedEndIndex();
    if (tagged_end_index < 2)
    {
      m_sector_coefficient[tagged_end_index] = MYON_SubDSectorType::Create( this, tagged_end_index).SectorCoefficient();
    }
    else if (2 == tagged_end_index)
    {
      if (MYON_SubDEdgeTag::Smooth == m_edge_tag && 2 == m_face_count )
        const_cast<MYON_SubDEdge*>(this)->m_edge_tag = MYON_SubDEdgeTag::SmoothX;

      if (MYON_SubDEdgeTag::Smooth == m_edge_tag)
        const_cast<MYON_SubDEdge*>(this)->m_edge_tag = MYON_SubDEdgeTag::Crease;
      else if (MYON_SubDEdgeTag::SmoothX == m_edge_tag)
      {
        m_sector_coefficient[0] = MYON_SubDSectorType::Create( this, 0).SectorCoefficient();
        m_sector_coefficient[1] = MYON_SubDSectorType::Create( this, 1).SectorCoefficient();
      }
    }
  }

  const bool bNoChanges 
    =  input_sector_coefficient[0] == m_sector_coefficient[0]
    && input_sector_coefficient[1] == m_sector_coefficient[1];

  return (false == bNoChanges);
}

unsigned int MYON_SubDLevel::UpdateEdgeSectorCoefficients(
  bool bUnsetEdgeSectorCoefficientsOnly
  ) const
{
  unsigned int changed_edge_count = 0;
  for (const MYON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    if (const_cast<MYON_SubDEdge*>(edge)->UpdateEdgeSectorCoefficientsForExperts(bUnsetEdgeSectorCoefficientsOnly))
      ++changed_edge_count;
  }
  return changed_edge_count;
}

class MYON_SubDFace* MYON_SubD::AddTriangleFace(
  class MYON_SubDEdge* edge0,
  class MYON_SubDEdge* edge1,
  class MYON_SubDEdge* edge2
)
{
  MYON_SubDEdge* edges[3] = { edge0,edge1,edge2 };
  return AddFace(edges, 3);
}

class MYON_SubDFace* MYON_SubD::AddQuadFace(
  class MYON_SubDEdge* edge0,
  class MYON_SubDEdge* edge1,
  class MYON_SubDEdge* edge2,
  class MYON_SubDEdge* edge3
  )
{
  MYON_SubDEdge* edges[4] = { edge0,edge1,edge2,edge3 };
  return AddFace(edges, 4);
}

class MYON_SubDFace* MYON_SubD::AddTriangleFace(
  MYON_SubDEdgePtr edge0,
  MYON_SubDEdgePtr edge1,
  MYON_SubDEdgePtr edge2
)
{
  MYON_SubDEdgePtr eptr3[3] = { edge0,edge1,edge2 };
  return AddFace(eptr3, 3);
}

class MYON_SubDFace* MYON_SubD::AddQuadFace(
  MYON_SubDEdgePtr edge0,
  MYON_SubDEdgePtr edge1,
  MYON_SubDEdgePtr edge2,
  MYON_SubDEdgePtr edge3
  )
{
  MYON_SubDEdgePtr eptr4[4] = { edge0,edge1,edge2,edge3 };
  return AddFace(eptr4, 4);
}

class MYON_SubDFace* MYON_SubD::AddFace(
  const MYON_SimpleArray<MYON_SubDEdge*>& edges
)
{
  return AddFace(edges.Array(), edges.UnsignedCount());
}

class MYON_SubDFace* MYON_SubD::AddFace(
  const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
)
{
  return AddFace(edges.Array(), edges.UnsignedCount());
}

class MYON_SubDFace* MYON_SubD::AddFace(
  MYON_SubDEdge *const* edge,
  unsigned int edge_count
  )
{
  if (edge_count < 3 || nullptr == edge)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge[0] || nullptr == edge[0]->m_vertex[0] || nullptr == edge[0]->m_vertex[1] || edge[0]->m_vertex[0] == edge[0]->m_vertex[1])
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( edge[0] == edge[edge_count-1] )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON_SubDEdgePtr* eptr = (MYON_SubDEdgePtr*)onmalloc(edge_count * sizeof(*eptr));
  if (nullptr == eptr)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  eptr[0] = MYON_SubDEdgePtr::Create(
    edge[0],
    (edge[0]->m_vertex[0] == edge[1]->m_vertex[0] || edge[0]->m_vertex[0] == edge[1]->m_vertex[1]) ? 1 : 0
  );
  eptr[edge_count - 1] = MYON_SubDEdgePtr::Null;
  for (unsigned int fei = 1; fei < edge_count; ++fei)
  {
    if (nullptr == edge[fei] || nullptr == edge[fei]->m_vertex[0] || nullptr == edge[fei]->m_vertex[1] || edge[fei]->m_vertex[0] == edge[fei]->m_vertex[1])
      break;
    if (edge[fei - 1] == edge[fei])
      break;
    const MYON_SubDVertex* v = eptr[fei - 1].RelativeVertex(1);
    if (nullptr == v)
      break;
    eptr[fei] = MYON_SubDEdgePtr::Create(edge[fei], v == edge[fei]->m_vertex[0] ? 0 : 1);
    if (v != eptr[fei].RelativeVertex(0))
      break;
  }

  MYON_SubDFace* face
    = (eptr[edge_count - 1].IsNotNull() && eptr[0].RelativeVertex(0) == eptr[edge_count - 1].RelativeVertex(1))
    ? AddFace(eptr, edge_count)
    : nullptr;
  onfree(eptr);
  if (nullptr == face)
  {
    MYON_SUBD_ERROR("Invalid input edge[] array");
  }
  return face;
}

class MYON_SubDFace* MYON_SubD::AddFace(
  const MYON_SubDEdgePtr* edge,
  unsigned int edge_count
  )
{
  MYON_SubDimple* subdimple = SubDimple(true);
  return (nullptr != subdimple) ? subdimple->AddFace(edge_count, edge) : nullptr;
}


class MYON_SubDFace* MYON_SubD::AddFaceForExperts(
  unsigned int candiate_face_id,
  const MYON_SubDEdgePtr* edge,
  unsigned int edge_count
  )
{
  MYON_SubDimple* subdimple = SubDimple(true);
  return (nullptr != subdimple) ? subdimple->AddFace(candiate_face_id, edge_count, edge) : nullptr;
}

bool MYON_SubD::ReturnFaceForExperts(
  MYON_SubDFace* f
)
{
  if (nullptr == f)
    return false;

  if (this->InSubD(f) && f->IsActive() && 0 == f->m_edge_count)
  {
    MYON_SubDimple* subdimple = SubDimple(false);
    if (nullptr != subdimple)
    {
      subdimple->ReturnFace(f);
      return true;
    }
  }

  // Caller is not an expert but a crash has been prevented.
  return MYON_SUBD_RETURN_ERROR(false); 
}

bool MYON_SubD::AddFaceTexturePoints(
  const class MYON_SubDFace* face,
  const class MYON_3dPoint* texture_points,
  size_t texture_points_count
) const
{
  if (nullptr == face)
    return false;
  face->ClearTexturePoints();
  const unsigned int face_edge_count = face->EdgeCount();
  if (nullptr != texture_points && texture_points_count >= face_edge_count && face_edge_count >= 3)
  {
    const MYON_SubDimple* subdimple = SubDimple();
    if (nullptr != subdimple)
    {
      const unsigned capacity = subdimple->AllocateFaceTexturePoints(face);
      if (capacity >= face_edge_count)
      {
        for (unsigned i = 0; i < face_edge_count; ++i)
          face->SetTexturePoint(i, texture_points[i]);
      }
    }
  }
  return face->TexturePointsAreSet();
}

unsigned int MYON_SubD::AllocateFaceTexturePoints(
  const class MYON_SubDFace* face
) const
{
  if (nullptr == face)
    return false;
  const unsigned int face_edge_count = face->EdgeCount();
  if (face_edge_count >= 3)
  {
    const MYON_SubDimple* subdimple = SubDimple();
    if (nullptr != subdimple)
      return subdimple->AllocateFaceTexturePoints(face);
  }
  face->ClearTexturePoints();
  return 0;
}

unsigned int MYON_SubDFace::TexturePointsCapacity() const
{
  return MYON_SubDHeap::Managed3dPointArrayCapacity(this->m_texture_points);
}

bool MYON_SubDFace::TexturePointsAreSet() const
{
  return
    0 != (this->m_texture_status_bits & MYON_SubDFace::TextureStatusBits::TexturePointsSet)
    && TexturePointsCapacity() >= EdgeCount()
    && EdgeCount() >= 3
    ;
}

bool MYON_SubDFace::SetTexturePoint(
  unsigned i,
  MYON_3dPoint texture_point
) const
{
  const unsigned texture_point_capacity = this->TexturePointsCapacity();
  if (i < texture_point_capacity)
  {
    this->m_texture_points[i] = texture_point;
    this->m_texture_status_bits |= MYON_SubDFace::TextureStatusBits::TexturePointsSet;
    return true;
  }
  return false;
}

void MYON_SubDFace::ClearTexturePoints() const
{
  this->m_texture_status_bits &= MYON_SubDFace::TextureStatusBits::NotTexturePointsBitsMask;
}

const MYON_3dPoint MYON_SubDFace::TexturePoint(
  unsigned int i
) const
{
  const unsigned edge_count = this->EdgeCount();
  return (i < edge_count&& TexturePointsAreSet()) ? this->m_texture_points[i] : MYON_3dPoint::NanPoint;
}

const MYON_3dPoint MYON_SubDFace::TextureCenterPoint() const
{
  const unsigned edge_count = this->EdgeCount();
  if (edge_count >= 3 && TexturePointsAreSet())
  {
    MYON_3dPoint c = MYON_3dPoint::Origin;
    for (unsigned i = 0; i < edge_count; ++i)
      c += m_texture_points[i];
    const double d = edge_count;
    c.x /= d;
    c.y /= d;
    c.z /= d;
    return c;
  }
  return MYON_3dPoint::NanPoint;
}


bool MYON_SubD::AddFaceEdgeConnection(
  MYON_SubDFace* face,
  unsigned int i,
  MYON_SubDEdge* edge,
  MYON__UINT_PTR edge_direction
  )
{
  return AddFaceEdgeConnection(face, i, MYON_SubDEdgePtr::Create(edge, edge_direction));
}

bool MYON_SubD::AddFaceEdgeConnection(
  MYON_SubDFace* face,
  unsigned int i,
  MYON_SubDEdgePtr eptr
)
{
  return AddFaceEdgeConnection(face, i, eptr, false, false);
}

bool MYON_SubD::AddFaceEdgeConnection(MYON_SubDFace* face, unsigned int i, MYON_SubDEdgePtr eptr, bool bAddbAddFaceToRelativeVertex0, bool bAddbAddFaceToRelativeVertex1)
{
  if (nullptr == face && i >= MYON_SubDFace::MaximumEdgeCount)
  {
    return MYON_SUBD_RETURN_ERROR(false);
  }

  unsigned int face_edge_count = (unsigned int)face->m_edge_count + 1U;
  if ( face_edge_count <= i )
    face_edge_count = i+1;

  MYON_SubDEdge* edge = eptr.Edge();
  if (nullptr != edge)
  {
    if (edge->m_face_count >= edge->m_facex_capacity + (unsigned short)2)
    {
      if (false == GrowEdgeFaceArray(edge, 0))
        return MYON_SUBD_RETURN_ERROR(false);
    }

    MYON_SubDFacePtr fptr = MYON_SubDFacePtr::Create(face,eptr.EdgeDirection());

    unsigned short efi = edge->m_face_count;
    if ( efi < 2 )
      edge->m_face2[efi] = fptr;
    else
    {
      if ( nullptr == edge->m_facex )
        return MYON_SUBD_RETURN_ERROR(false);
      edge->m_facex[efi - 2] = fptr;
    }
    edge->m_face_count++;
  }

  if (face_edge_count > ((unsigned int)face->m_edgex_capacity) + 4U)
  {
    if (false == GrowFaceEdgeArray(face,face_edge_count))
      return MYON_SUBD_RETURN_ERROR(false);
  }

  if (i >= ((unsigned int)face->m_edge_count))
  {
    unsigned int j = face->m_edge_count;
    for (/*empty init*/;j < 4; j++)
      face->m_edge4[j] = MYON_SubDEdgePtr::Null;
    for (/*empty init*/;j < i; j++)
      face->m_edgex[j-4] = MYON_SubDEdgePtr::Null;
  }
  else
  {
    for (unsigned int j = face_edge_count - 1; j > i; j--)
    {
      if (j > 4)
        face->m_edgex[j - 4] = face->m_edgex[j - 5];
      else if (4 == j)
        face->m_edgex[0] = face->m_edge4[3];
      else
        face->m_edge4[j] = face->m_edge4[j - 1];
    }
  }

  if ( i < 4 )
    face->m_edge4[i] = eptr;
  else 
    face->m_edgex[i-4] = eptr;
  face->m_edge_count = (unsigned short)face_edge_count;

  for (unsigned evi = 0; evi < 2; ++evi)
  {
    MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>((0 == evi ? bAddbAddFaceToRelativeVertex0 : bAddbAddFaceToRelativeVertex1) ? eptr.RelativeVertex(evi) : nullptr);
    if (nullptr != v)
    {
      if ( false == this->GrowVertexFaceArray(v, v->m_face_count + 1))
        return MYON_SUBD_RETURN_ERROR(false);
      v->m_faces[v->m_face_count++] = face;
    }
  }

  return true;
}


bool MYON_SubD::SetFaceBoundary(
  MYON_SubDFace* face,
  const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
)
{
  return SetFaceBoundary(face, edges.Array(), edges.UnsignedCount());
}

bool MYON_SubD::SetFaceBoundary(
  MYON_SubDFace* face,
  const MYON_SubDEdgePtr* edges,
  size_t edge_count
)
{
  // Do a little validation to prevent disasters. 
  if (nullptr == face)
    return MYON_SUBD_RETURN_ERROR(false);
  if (0 != face->m_edge_count)
    return MYON_SUBD_RETURN_ERROR(false);
  if (nullptr == edges || edge_count < 3 || edge_count > ((size_t)MYON_SubDFace::MaximumEdgeCount))
    return MYON_SUBD_RETURN_ERROR(false);
  const MYON_SubDVertex* v1 = edges[edge_count - 1].RelativeVertex(1);
  if ( nullptr == v1)
    return MYON_SUBD_RETURN_ERROR(false);
  for (size_t fei = 0; fei < edge_count; ++fei)
  {
    const MYON_SubDVertex* v0 = edges[fei].RelativeVertex(0);
    if ( v0 != v1)
      return MYON_SUBD_RETURN_ERROR(false);
    v1 = edges[fei].RelativeVertex(1);
    if ( nullptr == v1 || v0 == v1)
      return MYON_SUBD_RETURN_ERROR(false);
  }

  // set face-edge pointers and add face to vertex face arrays
  if (false == this->GrowFaceEdgeArray(face, edge_count))
    return MYON_SUBD_RETURN_ERROR(false);
  for (size_t fei = 0; fei < edge_count; ++fei)
  {
    MYON_SubDEdgePtr eptr = edges[fei];
    if (false == this->AddFaceEdgeConnection(face, (unsigned)fei, eptr, true, false))
      return MYON_SUBD_RETURN_ERROR(false);
  }

  return true;
}


bool MYON_SubD::RemoveFaceEdgeConnection(
  MYON_SubDFace* face,
  MYON_SubDEdge* edge
  )
{
  MYON_SubDEdgePtr removed_edge;
  return RemoveFaceEdgeConnection(face, face->EdgeArrayIndex(edge), removed_edge);
}

bool MYON_SubD::RemoveFaceEdgeConnection(
  MYON_SubDFace* face,
  unsigned int i
  )
{
  MYON_SubDEdgePtr removed_edge;
  return RemoveFaceEdgeConnection(face, i, removed_edge);
}

bool MYON_SubD::RemoveFaceEdgeConnection(
  MYON_SubDFace* face,
  unsigned int i,
  MYON_SubDEdgePtr& removed_edge
  )
{
  removed_edge = MYON_SubDEdgePtr::Null;
  if ( nullptr == face && i >= (unsigned int)face->m_edge_count )
  {
    return MYON_SUBD_RETURN_ERROR(false);
  }

  if ( false == face->RemoveEdgeFromArray(i,removed_edge) )
    return MYON_SUBD_RETURN_ERROR(false);

  MYON_SubDEdge* edge = removed_edge.Edge();
  if (nullptr != edge)
  {
    if (false == edge->RemoveFaceFromArray(face))
      return MYON_SUBD_RETURN_ERROR(false);
  }

  return true;
}

bool MYON_SubD::RemoveFaceConnections(
  MYON_SubDFace* face
)
{
  if ( nullptr == face )
  {
    return MYON_SUBD_RETURN_ERROR(false);
  }
  if (face->m_edge_count > 0)
  {
    MYON_SubDEdgePtr removed_edge;
    for (unsigned short fei = face->m_edge_count; fei > 0; --fei)
    {
      removed_edge = MYON_SubDEdgePtr::Null;
      if (false == face->RemoveEdgeFromArray(fei - 1, removed_edge))
        return MYON_SUBD_RETURN_ERROR(false);
      MYON_SubDEdge* edge = removed_edge.Edge();
      if (nullptr != edge)
      {
        if (false == edge->RemoveFaceFromArray(face))
          return MYON_SUBD_RETURN_ERROR(false);

        for (int evi = 0; evi < 2; ++evi)
        {
          MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>(edge->m_vertex[evi]);
          if (nullptr != v)
          {
            for (unsigned short vfi = 0; vfi < v->m_face_count; ++vfi)
            {
              if (face == v->m_faces[vfi])
              {
                for (++vfi; vfi < v->m_face_count; ++vfi)
                  v->m_faces[vfi - 1] = v->m_faces[vfi];
                v->m_face_count--;
                break;
              }
            }
          }
        }

      }
    }
    face->m_edge_count = 0;
  }

  return true;
}



static bool MYON_SubDFace_GetSubdivisionPointError(
  const class MYON_SubDFace* face,
  double subdivision_point[3],
  bool bDamagedState
  )
{
  if (nullptr == subdivision_point)
    return MYON_SUBD_RETURN_ERROR(false); // caller passed a null pointer - face is not necessarily damaged

  // make sure returned point is not used by a caller who doesn't bother to check return codes.
  subdivision_point[0] = MYON_DBL_QNAN;
  subdivision_point[1] = MYON_DBL_QNAN;
  subdivision_point[2] = MYON_DBL_QNAN;

  if (nullptr == face) 
    return MYON_SUBD_RETURN_ERROR(false);

  // face is damaged in some way - mark it
  face->m_status.SetDamagedState(bDamagedState);

  return MYON_SUBD_RETURN_ERROR(false);
}

const MYON_3dPoint MYON_SubDFace::SubdivisionPoint() const
{
  MYON_3dPoint S;
  return (GetSubdivisionPoint(&S.x) && S.IsValid()) ? S : MYON_3dPoint::NanPoint;
}

bool MYON_SubDFace::GetSubdivisionPoint(
  double subdivision_point[3]
) const
{
  if (nullptr == subdivision_point)
    return MYON_SubDFace_GetSubdivisionPointError(this, subdivision_point, false);

  if (GetSavedSubdivisionPoint(subdivision_point))
    return true;
    
  if (EvaluateCatmullClarkSubdivisionPoint(subdivision_point))
  {
    SetSavedSubdivisionPoint(subdivision_point);
    return true;
  }

  return false;
}

bool MYON_SubDFace::EvaluateCatmullClarkSubdivisionPoint(double subdivision_point[3]) const
{
  if (nullptr == subdivision_point)
    return MYON_SubDFace_GetSubdivisionPointError(this,subdivision_point,false);

  const unsigned int count = m_edge_count;
  if (count < 3)
    return MYON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);

  const class MYON_SubDEdgePtr* edge_ptr = m_edge4;

  MYON__UINT_PTR e_ptr;
  const MYON_SubDEdge* e;
  MYON__UINT_PTR edir;
  const double* vertexP[4];

  // Use faster code for the case when the face is a quad.
  // Since this is a Catmull-Clark subdivision scheme, this
  // case is the most common by far and code that gives quads
  // special treatment will run noticably faster.
  e_ptr = edge_ptr[0].m_ptr;
  e = MYON_SUBD_EDGE_POINTER(e_ptr);
  if ( nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] )
    return MYON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
  edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
  vertexP[0] = e->m_vertex[edir]->m_P;
  vertexP[1] = e->m_vertex[1 - edir]->m_P;

  e_ptr = edge_ptr[2].m_ptr;
  e = MYON_SUBD_EDGE_POINTER(e_ptr);
  if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
    return MYON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
  edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
  vertexP[2] = e->m_vertex[edir]->m_P;
  vertexP[3] = e->m_vertex[1 - edir]->m_P;

  if (4 == count)
  {
    // most common case in quad subdivision schemes
    subdivision_point[0] = (vertexP[0][0] + vertexP[1][0] + vertexP[2][0] + vertexP[3][0])*0.25;
    subdivision_point[1] = (vertexP[0][1] + vertexP[1][1] + vertexP[2][1] + vertexP[3][1])*0.25;
    subdivision_point[2] = (vertexP[0][2] + vertexP[1][2] + vertexP[2][2] + vertexP[3][2])*0.25;
    return true;
  }

  if (3 == count)
  {
    // 2nd most common case in quad subdivision schemes
    subdivision_point[0] = (vertexP[0][0] + vertexP[1][0] + vertexP[2][0]) / 3.0;
    subdivision_point[1] = (vertexP[0][1] + vertexP[1][1] + vertexP[2][1]) / 3.0;
    subdivision_point[2] = (vertexP[0][2] + vertexP[1][2] + vertexP[2][2]) / 3.0;
    return true;
  }

  // count > 4
  double faceP[3]
    = {
    (vertexP[0][0] + vertexP[1][0] + vertexP[2][0] + vertexP[3][0]),
    (vertexP[0][1] + vertexP[1][1] + vertexP[2][1] + vertexP[3][1]),
    (vertexP[0][2] + vertexP[1][2] + vertexP[2][2] + vertexP[3][2]) 
    };

  if (nullptr == m_edgex)
  {
    return MYON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
  }

  edge_ptr = m_edgex - 4; // -4 because index i begins at 4
  unsigned int i;
  for (i = 4; i + 1 < count; i += 2)
  {
    e_ptr = edge_ptr[i].m_ptr;
    e = MYON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return MYON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
    edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
    vertexP[0] = e->m_vertex[edir]->m_P;
    vertexP[1] = e->m_vertex[1 - edir]->m_P;

    faceP[0] += vertexP[0][0];
    faceP[1] += vertexP[0][1];
    faceP[2] += vertexP[0][2];

    faceP[0] += vertexP[1][0];
    faceP[1] += vertexP[1][1];
    faceP[2] += vertexP[1][2];
  }

  if (i < count)
  {
    // odd number of edges and vertices
    e_ptr = edge_ptr[count - 1].m_ptr;
    e = MYON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return MYON_SubDFace_GetSubdivisionPointError(this, subdivision_point, true);
    edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
    vertexP[0] = e->m_vertex[edir]->m_P;
    faceP[0] += vertexP[0][0];
    faceP[1] += vertexP[0][1];
    faceP[2] += vertexP[0][2];
  }

  const double n = count;

  subdivision_point[0] = faceP[0] / n;
  subdivision_point[1] = faceP[1] / n;
  subdivision_point[2] = faceP[2] / n;
  return true;
}

int MYON_SubDComponentBase::CompareId(
  const MYON_SubDComponentBase* lhs,
  const MYON_SubDComponentBase* rhs
)
{
  if (lhs == rhs)
    return 0;
  // nulls at end of list
  if (nullptr == rhs)
    return -1;
  if (nullptr == lhs)
    return 1;
  if (lhs->m_id < rhs->m_id)
    return -1;
  if (lhs->m_id > rhs->m_id)
    return 1;
  return 0;
}

void MYON_SubDComponentBase::ClearSavedSubdivisionPoint() const
{
  Internal_ClearSubdivisionPointAndSurfacePointFlags();
}

bool MYON_SubDComponentBase::SetSavedSubdivisionPoint(
  const double subdivision_point[3]
  ) const
{
  if (nullptr == subdivision_point)
  {
    Internal_ClearSubdivisionPointAndSurfacePointFlags();
    return true;
  }
  
  if ( MYON_IsValid(subdivision_point[0])
    && MYON_IsValid(subdivision_point[1])
    && MYON_IsValid(subdivision_point[2])
    )
  {
    m_saved_subd_point1[0] = subdivision_point[0];
    m_saved_subd_point1[1] = subdivision_point[1];
    m_saved_subd_point1[2] = subdivision_point[2];
    m_saved_points_flags |= MYON_SUBD_CACHE_POINT_FLAG_BIT;
    return true;
  }

  Internal_ClearSubdivisionPointAndSurfacePointFlags();
  return MYON_SUBD_RETURN_ERROR(false);
}

bool MYON_SubDComponentBase::GetSavedSubdivisionPoint(
  double subdivision_point[3]
  ) const
{
  if ( 0 == (MYON_SUBD_CACHE_POINT_FLAG_BIT & m_saved_points_flags) )
    return false;

  if (nullptr != subdivision_point)
  {
    subdivision_point[0] = m_saved_subd_point1[0];
    subdivision_point[1] = m_saved_subd_point1[1];
    subdivision_point[2] = m_saved_subd_point1[2];
  }

  return true;
}

const MYON_3dPoint MYON_SubDComponentBase::SavedSubdivisionPoint() const
{
  MYON_3dPoint p(MYON_3dPoint::NanPoint);
  return GetSavedSubdivisionPoint(&p.x) ? p : MYON_3dPoint::NanPoint;
}

unsigned const MYON_SubDComponentBase::SubdivisionLevel() const
{
  return (unsigned)m_level;
}

void MYON_SubDComponentBase::SetSubdivisionLevel(unsigned level)
{
  if (level <= 255U)
    m_level = ((unsigned char)level);
}

const MYON_ComponentStatus MYON_SubDComponentBase::Status() const
{
  return m_status;
}

bool MYON_SubDComponentBase::IsActive() const
{
  return (m_id > 0 && m_archive_id != MYON_UNSET_UINT_INDEX);
}


bool MYON_SubDComponentBase::IsSymmetrySetPrimaryMotif() const
{
  return 1 == this->m_symmetry_set_next.ComponentDirection();
}

bool MYON_SubDComponentBase::InSymmetrySet() const
{
  return this->m_symmetry_set_next.IsNotNull();
}

bool MYON_SubDComponentPtr::IsSymmetrySetPrimaryMotif() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->IsSymmetrySetPrimaryMotif() : false;
}

bool MYON_SubDComponentPtr::InSymmetrySet() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->InSymmetrySet() : false;
}

bool MYON_SubDComponentBase::Mark() const
{
  return m_status.RuntimeMark();
}

bool MYON_SubDComponentBase::ClearMark() const
{
 return m_status.ClearRuntimeMark();
}

bool MYON_SubDComponentBase::SetMark() const
{
  return m_status.SetRuntimeMark();
}

bool MYON_SubDComponentBase::SetMark(
  bool bMark
) const
{
  return m_status.SetRuntimeMark(bMark);
}


MYON__UINT8 MYON_SubDComponentBase::MarkBits() const
{
  return m_status.MarkBits();
}

MYON__UINT8 MYON_SubDComponentBase::SetMarkBits(
  MYON__UINT8 mark_bits
) const
{
  return m_status.SetMarkBits(mark_bits);
}

MYON__UINT8 MYON_SubDComponentBase::ClearMarkBits() const
{
  return m_status.SetMarkBits(0);
}

bool MYON_SubDComponentPtr::IsActive() const
{
  const MYON_SubDComponentBase* c = this->ComponentBase();
  return (nullptr != c) ? c->IsActive() : false;
}

bool MYON_SubDVertexPtr::IsActive() const
{
  const MYON_SubDVertex* v = MYON_SUBD_VERTEX_POINTER(m_ptr);
  return (nullptr != v) ? v->IsActive() : false;
}

bool MYON_SubDEdgePtr::IsActive() const
{
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_ptr);
  return (nullptr != e) ? e->IsActive() : false;
}

bool MYON_SubDFacePtr::IsActive() const
{
  const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(m_ptr);
  return (nullptr != f) ? f->IsActive() : false;
}


const MYON_ComponentStatus MYON_SubDVertex::NeighborhoodStatusLogicalOr(
  bool bIncludeEdges,
  bool bIncludeFaces
) const
{
  MYON_ComponentStatus s(m_status);
  if (bIncludeEdges && nullptr != m_edges)
  {
    for (unsigned short vei = 0; vei < m_edge_count; vei++)
    {
      const MYON_SubDEdge* e = m_edges[vei].Edge();
      if (nullptr != e)
        s = MYON_ComponentStatus::LogicalOr(s, e->m_status);
    }
  }
  if (bIncludeFaces && nullptr != m_faces)
  {
    for (unsigned short vfi = 0; vfi < m_face_count; vfi++)
    {
      const MYON_SubDFace* f = m_faces[vfi];
      if (nullptr != f)
        s = MYON_ComponentStatus::LogicalOr(s, f->m_status);
    }
  }
  return s;
}
  
const MYON_ComponentStatus MYON_SubDEdge::NeighborhoodStatusLogicalOr(
  bool bIncludeVertices,
  bool bIncludeFaces
) const
{
  MYON_ComponentStatus s(m_status);
  if (bIncludeVertices)
  {
    for (unsigned int evi = 0; evi < 2; evi++)
    {
      const MYON_SubDVertex* v = m_vertex[evi];
      if (nullptr != v)
        s = MYON_ComponentStatus::LogicalOr(s, v->m_status);
    }
  }
  if (bIncludeFaces)
  {
    const MYON_SubDFacePtr* fptr = m_face2;
    for (unsigned short vfi = 0; vfi < m_face_count; vfi++)
    {
      const MYON_SubDFace* f = fptr->Face();
      if (nullptr != f)
        s = MYON_ComponentStatus::LogicalOr(s, f->m_status);
      if (1 == vfi)
      {
        fptr = m_facex;
        if (nullptr == fptr)
          break;
      }
      else
        fptr++;
    }
  }
  return s;
}

const MYON_ComponentStatus MYON_SubDFace::NeighborhoodStatusLogicalOr(bool bIncludeVertices, bool bIncludeEdges) const
{
  MYON_ComponentStatus s(m_status);
  if (bIncludeVertices || bIncludeEdges)
  {
    const MYON_SubDEdgePtr* eptr = m_edge4;
    for (unsigned int fei = 0; fei < m_edge_count; fei++)
    {
      if (4 == fei)
      {
        eptr = m_edgex;
        if (nullptr == eptr)
          break;
      }
      const MYON_SubDEdge* e = eptr->Edge();
      if (nullptr != e)
      {
        if (bIncludeEdges)
        {
          s = MYON_ComponentStatus::LogicalOr(s, e->m_status);
        }
        if (bIncludeVertices)
        {
          const MYON_SubDVertex* v = e->m_vertex[(0!=eptr->EdgeDirection())?1:0];
          if (nullptr != v)
            s = MYON_ComponentStatus::LogicalOr(s, v->m_status);
        }
      }
      eptr++;
    }
  }
  return s;
}

static void Internal_ClearVertexNeighborhoodCache(const MYON_SubDVertex* vertex)
{
  // Clear cached values for this vertex every component touching this vertex.
  vertex->ClearSavedSubdivisionPoints();
  for (unsigned short vei = 0; vei < vertex->m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
    if (nullptr != e)
      e->ClearSavedSubdivisionPoints();
  }
  for (unsigned short vfi = 0; vfi < vertex->m_face_count; ++vfi)
  {
    const MYON_SubDFace* f = vertex->m_faces[vfi];
    if (nullptr != f)
      f->ClearSavedSubdivisionPoints();
  }
}

static void Internal_ClearFaceNeighborhoodCache(const MYON_SubDFace* face)
{
  // Clear cached values for every component associated with this face.
  face->ClearSavedSubdivisionPoints();
  const MYON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned int efi = 0; efi < face->m_edge_count; ++efi, ++eptr)
  {
    if (4 == efi)
    {
      eptr = face->m_edgex;
      if ( nullptr == eptr || face->m_edgex_capacity < face->m_edge_count - 4)
      {
        MYON_SUBD_ERROR("Invalid face edge count or edgex information.");
        break;
      }
    }
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != edge)
    {
      edge->ClearSavedSubdivisionPoints();
      edge->UnsetSectorCoefficientsForExperts();
      for (unsigned int evi = 0; evi < 2; evi++)
      {
        const MYON_SubDVertex* vertex = edge->m_vertex[evi];
        if (nullptr == vertex)
          continue;
        Internal_ClearVertexNeighborhoodCache(vertex);
      }
    }
  }
}

void MYON_SubDVertex::VertexModifiedNofification() const
{
  // NOTE WELL:
  //  This function is called by MYON_SubDEdge::EdgeModifiedNotification() and MYON_SubDFace::FaceModifiedNotification().

  ClearSavedSubdivisionPoints();

  if (nullptr != m_edges)
  {
    for (unsigned short vei = 0; vei < m_edge_count; vei++)
    {
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
      if (nullptr == edge)
        continue;
      edge->ClearSavedSubdivisionPoints();
      edge->UnsetSectorCoefficientsForExperts();
      const MYON_SubDVertex* v1 = edge->m_vertex[1-MYON_SUBD_EDGE_DIRECTION(m_edges[vei].m_ptr)];
      if (nullptr != v1)
        v1->ClearSavedSubdivisionPoints();
    }

    if (nullptr != m_faces)
    {
      // This is needed to clear cached information in the Catmull-Clark 
      // ring that is not immediately adjacent to this vertex but whose values
      // this vertex affects.
      for (unsigned short vfi = 0; vfi < m_face_count; vfi++)
      {
        const MYON_SubDFace* face = m_faces[vfi];
        if (nullptr != face)
          Internal_ClearFaceNeighborhoodCache(face);
      }
    }
  }
}

void MYON_SubDEdge::EdgeModifiedNofification() const
{
  // NOTE WELL:
  //  This function is called by MYON_SubDFace::FaceModifiedNotification().

  ClearSavedSubdivisionPoints();
  UnsetSectorCoefficientsForExperts();
  for (unsigned int evi = 0; evi < 2; evi++)
  {
    if (nullptr != m_vertex[evi])
      m_vertex[evi]->VertexModifiedNofification();
  }
}

void MYON_SubDEdge::UnsetSectorCoefficientsForExperts() const
{
  const_cast<MYON_SubDEdge*>(this)->m_sector_coefficient[0] = MYON_SubDSectorType::UnsetSectorCoefficient;
  const_cast<MYON_SubDEdge*>(this)->m_sector_coefficient[1] = MYON_SubDSectorType::UnsetSectorCoefficient;
}

void MYON_SubDVertex::UnsetSectorCoefficientsForExperts(unsigned int relative_edge_end_dex) const
{
  for (unsigned short vei = 0; vei < m_edge_count; ++vei)
  {
    MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_edges[vei].m_ptr);
    if (nullptr == e)
      continue;
    MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(m_edges[vei].m_ptr);
    const unsigned evi
      = (relative_edge_end_dex < 2)
      ? ((0 == edir ? false : true) == (0 == relative_edge_end_dex ? false : true) ? 0U : 1U)
      : 2U;
    if ( evi < 2)
      e->m_sector_coefficient[evi] = MYON_SubDSectorType::UnsetSectorCoefficient;
    else
    {
      e->m_sector_coefficient[0] = MYON_SubDSectorType::UnsetSectorCoefficient;
      e->m_sector_coefficient[1] = MYON_SubDSectorType::UnsetSectorCoefficient;
    }
  }
}

void MYON_SubDFace::FaceModifiedNofification() const
{
  ClearSavedSubdivisionPoints();

  const MYON_SubDEdgePtr* eptr = m_edge4;
  for (unsigned int efi = 0; efi < m_edge_count; efi++)
  {
    if (4 == efi)
    {
      eptr = m_edgex;
      if ( nullptr == eptr)
        break;
    }
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr != edge)
      edge->EdgeModifiedNofification();
    eptr++;
  }
}

unsigned int MYON_SubDFace::PackId() const
{
  return m_pack_id;
}

void MYON_SubDFace::SetPackIdForExperts(
  unsigned int pack_id
)
{
  m_pack_id = pack_id;
}

bool MYON_SubDFace::IsValidPackRect(
  MYON_2dPoint pack_rect_origin,
  MYON_2dVector pack_rect_size,
  int packing_rotation_degrees
)
{
  const double fuzzy_1 = 1.0 + MYON_SQRT_EPSILON;
  bool rc 
    =  0.0 <= pack_rect_origin.x && pack_rect_origin.x < 1.0
    && 0.0 <= pack_rect_origin.y && pack_rect_origin.y < 1.0
    && 0.0 < pack_rect_size.x && (pack_rect_origin.x + pack_rect_size.x) <= fuzzy_1
    && 0.0 < pack_rect_size.y && (pack_rect_origin.y + pack_rect_size.y) <= fuzzy_1
    && 0 == packing_rotation_degrees % 90
    ;
  return rc;
}

bool MYON_SubDFace::SetPackRectForExperts(MYON_2dPoint pack_rect_origin, MYON_2dVector pack_rect_size, int packing_rotation_degrees)
{
  const bool bValidPackRectangle = MYON_SubDFace::IsValidPackRect(pack_rect_origin, pack_rect_size, packing_rotation_degrees);
  if (bValidPackRectangle)
  {
    m_pack_rect_origin[0] = pack_rect_origin.x;
    m_pack_rect_origin[1] = pack_rect_origin.y;
    m_pack_rect_size[0] = pack_rect_size.x;
    m_pack_rect_size[1] = pack_rect_size.y;

    MYON_SubDFace::PackStatusBits packing_rotation = MYON_SubDFace::PackStatusBits::PackingRotate0;
    switch (((packing_rotation_degrees % 360) + 360) % 360)
    {
    case 90:
      packing_rotation = MYON_SubDFace::PackStatusBits::PackingRotate90;
      break;
    case 180:
      packing_rotation = MYON_SubDFace::PackStatusBits::PackingRotate180;
      break;
    case 270:
      packing_rotation = MYON_SubDFace::PackStatusBits::PackingRotate270;
      break;
    }
    m_pack_status_bits = MYON_SubDFace::PackStatusBits::PackRectSet;
    m_pack_status_bits |= packing_rotation;
  }
  else
  {
    MYON_SUBD_ERROR("Invalid pack rect input");
    ClearPackRect();
  }
  return bValidPackRectangle;
}

void MYON_SubDFace::ClearPackRect()
{
  m_pack_rect_origin[0] = MYON_DBL_QNAN;
  m_pack_rect_origin[1] = MYON_DBL_QNAN;
  m_pack_rect_size[0] = MYON_DBL_QNAN;
  m_pack_rect_size[1] = MYON_DBL_QNAN;
  m_pack_status_bits = 0;
}

void MYON_SubDFace::ClearPackId()
{
  m_pack_id = 0;
  m_pack_rect_origin[0] = MYON_DBL_QNAN;
  m_pack_rect_origin[1] = MYON_DBL_QNAN;
  m_pack_rect_size[0] = MYON_DBL_QNAN;
  m_pack_rect_size[1] = MYON_DBL_QNAN;
  m_pack_status_bits = 0;
}

bool MYON_SubDFace::PackRectIsSet() const
{
  return 0 != (m_pack_status_bits & MYON_SubDFace::PackStatusBits::PackRectSet);
}

const MYON_2dPoint MYON_SubDFace::PackRectOrigin() const
{
  return MYON_2dPoint(m_pack_rect_origin);
}

const MYON_2dVector MYON_SubDFace::PackRectSize() const
{
  return MYON_2dVector(m_pack_rect_size);
}

unsigned int MYON_SubDFace::PackRectRotationDegrees() const
{
  if (0 == (m_pack_status_bits & MYON_SubDFace::PackStatusBits::PackRectSet))
    return 0;
  unsigned int packing_rotation_degrees = 0;
  switch (m_pack_status_bits & MYON_SubDFace::PackStatusBits::PackingRotateMask)
  {
  case MYON_SubDFace::PackStatusBits::PackingRotate90:
    packing_rotation_degrees = 90;
    break;
  case MYON_SubDFace::PackStatusBits::PackingRotate180:
    packing_rotation_degrees = 180;
    break;
  case MYON_SubDFace::PackStatusBits::PackingRotate270:
    packing_rotation_degrees = 270;
    break;
  }
  return packing_rotation_degrees;
}

double MYON_SubDFace::PackRectRotationRadians() const
{
  if (0 == (m_pack_status_bits & MYON_SubDFace::PackStatusBits::PackRectSet))
    return MYON_DBL_QNAN;
  double x = 0.0;
  switch (m_pack_status_bits & MYON_SubDFace::PackStatusBits::PackingRotateMask)
  {
  case MYON_SubDFace::PackStatusBits::PackingRotate90:
    x = 1.0;
    break;
  case MYON_SubDFace::PackStatusBits::PackingRotate180:
    x = 2.0;
    break;
  case MYON_SubDFace::PackStatusBits::PackingRotate270:
    x = 3.0;
    break;
  }
  return x * 0.5 * MYON_PI;
}

const MYON_2dPoint MYON_SubDFace::PackRectCorner(bool bGridOrder, int corner_index) const
{
  if (0 == (m_pack_status_bits & MYON_SubDFace::PackStatusBits::PackRectSet))
    return MYON_2dPoint::NanPoint;

  corner_index = ((corner_index % 4) + 4) % 4;
  // now corner_index = 0,1,2 or 3.

  if (bGridOrder)
  {
    if (2 == corner_index)
      corner_index = 3;
    else if (3 == corner_index)
      corner_index = 2;
  }
  // now corner index is a counter-clockwise corner index

  int packrot_dex = 0;
  switch (m_pack_status_bits & MYON_SubDFace::PackStatusBits::PackingRotateMask)
  {
  case MYON_SubDFace::PackStatusBits::PackingRotate90:
    packrot_dex = 3;
    break;
  case MYON_SubDFace::PackStatusBits::PackingRotate180:
    packrot_dex = 2;
    break;
  case MYON_SubDFace::PackStatusBits::PackingRotate270:
    packrot_dex = 1;
    break;
  }

  corner_index = (corner_index + packrot_dex) % 4;
  // now the packing rotation is taken into account.

  MYON_2dPoint corner = PackRectOrigin();
  const MYON_2dVector delta = PackRectSize();
  switch (corner_index)
  {
  case 1:
    corner.x += delta.x;
    break;
  case 2:
    corner.x += delta.x;
    corner.y += delta.y;
    break;
  case 3:
    corner.y += delta.y;
    break;
  }

  return corner;
}

bool MYON_SubDFace::GetFacePackRectCorners(bool bGridOrder, MYON_2dPoint face_pack_rect_corners[4]) const
{
  if (nullptr != face_pack_rect_corners)
  {
    if (this->PackRectIsSet())
    {
      face_pack_rect_corners[0] = PackRectCorner(bGridOrder, 0);
      face_pack_rect_corners[1] = PackRectCorner(bGridOrder, 1);
      face_pack_rect_corners[2] = PackRectCorner(bGridOrder, 2);
      face_pack_rect_corners[3] = PackRectCorner(bGridOrder, 3);
      return true;
    }
    face_pack_rect_corners[0] = MYON_2dPoint::NanPoint;
    face_pack_rect_corners[1] = MYON_2dPoint::NanPoint;
    face_pack_rect_corners[2] = MYON_2dPoint::NanPoint;
    face_pack_rect_corners[3] = MYON_2dPoint::NanPoint;
  }
  return false;
}

void MYON_SubDComponentBase::Internal_ClearSubdivisionPointAndSurfacePointFlags() const
{
  MYON_SUBD_CACHE_CLEAR_POINT_FLAG(m_saved_points_flags);
  MYON_SUBD_CACHE_CLEAR_LIMITLOC_FLAG(m_saved_points_flags);
}

bool MYON_SubDComponentBase::Internal_SubdivisionPointFlag() const
{
  return (0 != MYON_SUBD_CACHE_POINT_FLAG(m_saved_points_flags));
}

void MYON_SubDComponentBase::Internal_ClearSubdivisionPointFlag() const
{
  MYON_SUBD_CACHE_CLEAR_POINT_FLAG(m_saved_points_flags);
}

bool MYON_SubDComponentBase::Internal_SurfacePointFlag() const
{
  return (0 != MYON_SUBD_CACHE_LIMITLOC_FLAG(m_saved_points_flags));
}

void MYON_SubDComponentBase::Internal_ClearSurfacePointFlag() const
{
  MYON_SUBD_CACHE_CLEAR_LIMITLOC_FLAG(m_saved_points_flags);
}

bool MYON_SubDComponentBase::SavedSubdivisionPointIsSet() const
{
  return 
    (0 != MYON_SUBD_CACHE_POINT_FLAG(m_saved_points_flags))
    ? (MYON_IS_VALID(m_saved_subd_point1[0]) && MYON_IS_VALID(m_saved_subd_point1[1]) && MYON_IS_VALID(m_saved_subd_point1[2]))
    : false;
}

bool MYON_SubDEdge::EdgeSurfaceCurveIsSet() const
{
  return false;
}

bool MYON_SubDComponentBase::SubdivisionDisplacementIsNonzero() const
{
  // deprecated - never used.
  return false;
}

bool MYON_SubDComponentBase::SetSubdivisionDisplacement(const double*)
{
  // deprecated - never used.
  return false;
}

void MYON_SubDComponentBase::ClearSubdivisionDisplacement() const
{
  // deprecated - never used.
}

bool MYON_SubDComponentBase::GetSubdivisionDisplacement(double*) const
{
  // deprecated - never used.
  return false;
}

const MYON_3dVector MYON_SubDComponentBase::SubdivisionDisplacement() const
{
  // deprecated - never used.
  return MYON_3dVector::ZeroVector;
}

void MYON_SubDComponentBase::Internal_SetSavedSurfacePointFlag(bool bSavedSurfacePointFlag) const
{
  if (bSavedSurfacePointFlag)
    m_saved_points_flags |= MYON_SUBD_CACHE_LIMITLOC_FLAG_BIT;
  else
    Internal_ClearSurfacePointFlag();
}


void MYON_SubDComponentBase::Internal_SetModified1Flag() const
{
  m_saved_points_flags |= MYON_SubDComponentBase::ModifiedFlags::Modified1Bit;
}

void MYON_SubDComponentBase::Internal_SetModified2Flag() const
{
  m_saved_points_flags |= MYON_SubDComponentBase::ModifiedFlags::Modified1Bit;
}

void MYON_SubDComponentBase::Internal_ClearModifiedFlags() const
{
  m_saved_points_flags &= ~MYON_SubDComponentBase::ModifiedFlags::ModifiedFlagsMask;
}

bool MYON_SubDComponentBase::Internal_Modified1IsSet() const
{
  return (0 != (m_saved_points_flags & MYON_SubDComponentBase::ModifiedFlags::Modified1Bit));
}

bool MYON_SubDComponentBase::Internal_Modified1or2IsSet() const
{
  return (0 != (m_saved_points_flags & MYON_SubDComponentBase::ModifiedFlags::ModifiedFlagsMask));
}

bool MYON_SubDFace::ReverseEdgeList()
{
  const unsigned int edge_count = m_edge_count;
  if ( 0 == edge_count)
    return true;
  if (edge_count > 4 && nullptr == m_edgex)
  {
    return MYON_SUBD_RETURN_ERROR(false);
  }

  MYON_SubDEdgePtr buffer[16];
  MYON_SubDEdgePtr* reversed_eptrs;
  if ( edge_count <= sizeof(buffer)/sizeof(buffer[0]) )
    reversed_eptrs = buffer;
  else
  {
    reversed_eptrs = new(std::nothrow) MYON_SubDEdgePtr[edge_count];
    if ( nullptr == reversed_eptrs)
      return MYON_SUBD_RETURN_ERROR(false);
  }

  MYON_SubDEdgePtr* face_eptrs = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++, face_eptrs++)
  {
    if (4 == fei)
      face_eptrs = m_edgex;

    MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(face_eptrs->m_ptr);
    if ( nullptr == e)
      continue;
    MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(face_eptrs->m_ptr);
    reversed_eptrs[edge_count-1-fei] = MYON_SubDEdgePtr::Create(e,1-edir);

    MYON_SubDFacePtr* edges_fptrs = e->m_face2;
    const unsigned int face_count = e->m_face_count;
    for (unsigned int efi = 0; efi < face_count; efi++, edges_fptrs++)
    {
      if (2 == efi)
      {
        edges_fptrs = e->m_facex;
        if ( nullptr == edges_fptrs)
          break;
      }
      if ( this != MYON_SUBD_FACE_POINTER(edges_fptrs->m_ptr) )
        continue;
      *edges_fptrs = MYON_SubDFacePtr::Create(this,1-MYON_SUBD_FACE_DIRECTION(edges_fptrs->m_ptr));
      break;
    } 
  }

  face_eptrs = m_edge4;
  for (unsigned int fei = 0; fei < edge_count; fei++)
  {
    if (4 == fei)
      face_eptrs = m_edgex;
    *face_eptrs++ = reversed_eptrs[fei];
  }

  if ( reversed_eptrs != buffer )
    delete[] reversed_eptrs;

  return true;
}

static bool MYON_SubDEdge_GetSubdivisionPointError(
  const class MYON_SubDEdge* edge,
  double edge_point[3],
  const double* edgeP[2],
  bool bDamagedState
  )
{
  if (nullptr == edge || nullptr == edge_point)
    return false; // caller passed a null pointer - edge is not necessarily damaged

  MYON_SubDIncrementErrorCount();
  edge->m_status.SetDamagedState(bDamagedState);

  if (nullptr != edgeP && nullptr != edgeP[0] && nullptr != edgeP[1])
  {
    const double edgePsum[3] = { edgeP[0][0] + edgeP[1][0], edgeP[0][1] + edgeP[1][1], edgeP[0][2] + edgeP[1][2] };
    edge_point[0] = 0.5*edgePsum[0];
    edge_point[1] = 0.5*edgePsum[1];
    edge_point[2] = 0.5*edgePsum[2];
  }
  return true;
}

unsigned int MYON_SubDEdge::GetFacePointSum(
  const MYON_SubDFace* face,
  const MYON_SubDEdge* edge,
  double* facePsum
  )
{
  const MYON_SubDEdge* e;
  MYON__UINT_PTR e_ptr, edir;
  const double* vertexP[2];

  if (nullptr == face)
    return 0;
  
  const unsigned int n = face->m_edge_count;

  if (3 == n)
  {
    if (edge == MYON_SUBD_EDGE_POINTER(face->m_edge4[0].m_ptr))
      e_ptr = face->m_edge4[1].m_ptr;
    else if (edge == MYON_SUBD_EDGE_POINTER(face->m_edge4[1].m_ptr))
      e_ptr = face->m_edge4[2].m_ptr;
    else if (edge == MYON_SUBD_EDGE_POINTER(face->m_edge4[2].m_ptr))
      e_ptr = face->m_edge4[0].m_ptr;
    else
      return 0;
      e = MYON_SUBD_EDGE_POINTER(e_ptr);
      if (nullptr == e)
        return 0;
      if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
        return 0;
      edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
      if (edge->m_vertex[0] != e->m_vertex[edir] && edge->m_vertex[1] != e->m_vertex[edir])
        return 0;
      vertexP[0] = e->m_vertex[1 - edir]->m_P;
      facePsum[0] = vertexP[0][0];
      facePsum[1] = vertexP[0][1];
      facePsum[2] = vertexP[0][2];

      return n;
  }

  if (4 == n)
  {
    if (edge == MYON_SUBD_EDGE_POINTER(face->m_edge4[0].m_ptr))
      e_ptr = face->m_edge4[2].m_ptr;
    else if (edge == MYON_SUBD_EDGE_POINTER(face->m_edge4[1].m_ptr))
      e_ptr = face->m_edge4[3].m_ptr;
    else if (edge == MYON_SUBD_EDGE_POINTER(face->m_edge4[2].m_ptr))
      e_ptr = face->m_edge4[0].m_ptr;
    else if (edge == MYON_SUBD_EDGE_POINTER(face->m_edge4[3].m_ptr))
      e_ptr = face->m_edge4[1].m_ptr;
    else
      return 0;
    e = MYON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e)
      return 0;
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return 0;
    edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
    vertexP[0] = e->m_vertex[edir]->m_P;
    vertexP[1] = e->m_vertex[1 - edir]->m_P;
    facePsum[0] = vertexP[0][0] + vertexP[1][0];
    facePsum[1] = vertexP[0][1] + vertexP[1][1];
    facePsum[2] = vertexP[0][2] + vertexP[1][2];

    return n;
  }

  if (n < 3)
    return 0;

  const MYON_SubDEdgePtr* edgeptr = face->m_edge4;
  const MYON_SubDVertex* edge_vertex[2] = { edge->m_vertex[0], edge->m_vertex[1] };
  facePsum[0] = 0.0;
  facePsum[1] = 0.0;
  facePsum[2] = 0.0;
  for (unsigned i = 0; i < n; i++)
  {
    if (4 == i)
      edgeptr = face->m_edgex - 4;
    e = MYON_SUBD_EDGE_POINTER(edgeptr[i].m_ptr);
    if (nullptr == e)
      return 0;
    if (edge == e)
      continue;
    edir = MYON_SUBD_EDGE_DIRECTION(edgeptr[i].m_ptr);
    const MYON_SubDVertex* e_vertex[2] = { e->m_vertex[edir], e->m_vertex[1 - edir] };
    if (nullptr == e_vertex[0] || nullptr == e_vertex[1])
      return 0;
    if (edge_vertex[0] != e_vertex[0] && edge_vertex[1] != e_vertex[0])
    {
      vertexP[0] = e_vertex[0]->m_P;
      facePsum[0] += vertexP[0][0];
      facePsum[1] += vertexP[0][1];
      facePsum[2] += vertexP[0][2];
    }
    if (i + 1 < n)
    {
      // start of next edge = end of this edge
      if (edge_vertex[0] != e_vertex[1] && edge_vertex[1] != e_vertex[1])
      {
        vertexP[0] = e_vertex[1]->m_P;
        facePsum[0] += vertexP[0][0];
        facePsum[1] += vertexP[0][1];
        facePsum[2] += vertexP[0][2];
      }
      i++;
      if (4 == i && n > 4)
        edgeptr = face->m_edgex - 4;
    }
  }

  return n;
}

const MYON_3dPoint MYON_SubDEdge::SubdivisionPoint() const
{
  MYON_3dPoint S;
  return (GetSubdivisionPoint(&S.x) && S.IsValid()) ? S : MYON_3dPoint::NanPoint;
}

bool MYON_SubDEdge::GetSubdivisionPoint(
  double subdivision_point[3]
) const
{
  if (nullptr == subdivision_point)
    return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  if (GetSavedSubdivisionPoint(subdivision_point))
    return true;

  if (EvaluateCatmullClarkSubdivisionPoint(subdivision_point))
  {
    SetSavedSubdivisionPoint(subdivision_point);
    return true;
  }

  return false;
}

const MYON_3dPoint MYON_SubDEdge::ControlNetCenterPoint() const
{
  return 0.5*(ControlNetPoint(0) + ControlNetPoint(1));
}

const MYON_3dVector MYON_SubDEdge::ControlNetCenterNormal(
  unsigned int edge_face_index
) const
{
  const MYON_SubDFace* face = Face(edge_face_index);
  return (nullptr != face) ? face->ControlNetCenterNormal() : MYON_3dVector::NanVector;
}

bool MYON_SubDEdge::EvaluateCatmullClarkSubdivisionPoint(double subdivision_point[3]) const
{
  if (nullptr == subdivision_point)
    return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  const MYON_SubDVertex* edge_vertex[2] = { m_vertex[0], m_vertex[1] };
  if (nullptr == edge_vertex[0] || nullptr == edge_vertex[1])
    return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, nullptr, true);

  const double* edgeP[2] = { edge_vertex[0]->m_P, edge_vertex[1]->m_P };
  const double edgePsum[3] = { edgeP[0][0] + edgeP[1][0], edgeP[0][1] + edgeP[1][1], edgeP[0][2] + edgeP[1][2] };

  if ( IsSmooth() )
  {
    // All smooth edges (Smooth and SmoothX tags) must have exactly two neighboring faces.
    // Both ends of a level 0 SmoothX edge can be non-smooth vertices but the SmoothX tag
    // indicates it is to be subdivided as a smooth edge.
    // All other smooth edge must have at least one edn attached to a smooth vertex.

    if (2 != m_face_count)
      return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);

    const MYON_SubDFace* faces[2] = { MYON_SUBD_FACE_POINTER(m_face2[0].m_ptr), MYON_SUBD_FACE_POINTER(m_face2[1].m_ptr) };
    if (nullptr == faces[0] || nullptr == faces[1])
      return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);

    MYON_3dPoint sharp_subdivision_point;
    const double edge_sharpness = this->GetSharpSubdivisionPoint(sharp_subdivision_point);
    if (edge_sharpness >= 1.0)
    {
      // sharp crease vertex with sharpness >= 1 at current level
      subdivision_point[0] = sharp_subdivision_point.x;
      subdivision_point[1] = sharp_subdivision_point.y;
      subdivision_point[2] = sharp_subdivision_point.z;
      return true;
    }

    // for each neighbor face, sum the vertex locations that are not on this edge
    double facePsum[2][3];
    const unsigned int face_edge_count[2]
      = { MYON_SubDEdge::GetFacePointSum(faces[0], this, facePsum[0]),
      MYON_SubDEdge::GetFacePointSum(faces[1], this, facePsum[1])
    };
    if (0 == face_edge_count[0] || 0 == face_edge_count[1])
      return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);

    const unsigned int tagged_end
      = (MYON_SubDVertexTag::Smooth != edge_vertex[0]->m_vertex_tag)
      ? 0
      : ((MYON_SubDVertexTag::Smooth != edge_vertex[1]->m_vertex_tag) ? 1 : MYON_UNSET_UINT_INDEX);
    double EP[3];
    if (
      MYON_UNSET_UINT_INDEX == tagged_end
      || 0.5 == m_sector_coefficient[tagged_end]
      || (MYON_SubDEdgeTag::SmoothX == m_edge_tag)
      )
    {
      // ignore edge coefficients
      EP[0] = 0.375 * edgePsum[0];
      EP[1] = 0.375 * edgePsum[1];
      EP[2] = 0.375 * edgePsum[2];
    }
    else if (MYON_SubDVertexTag::Smooth == edge_vertex[1 - tagged_end]->m_vertex_tag
      && m_sector_coefficient[tagged_end] > 0.0
      && m_sector_coefficient[tagged_end] < 1.0
      )
    {
      double w[2];
      w[tagged_end] = m_sector_coefficient[tagged_end];
      w[1 - tagged_end] = 1.0 - w[tagged_end];
      EP[0] = 0.75 * (w[0] * edgeP[0][0] + w[1] * edgeP[1][0]);
      EP[1] = 0.75 * (w[0] * edgeP[0][1] + w[1] * edgeP[1][1]);
      EP[2] = 0.75 * (w[0] * edgeP[0][2] + w[1] * edgeP[1][2]);
    }
    else
    {
      // error:
      //   Both ends of a smooth vertex are tagged
      //   or coefficients are incorrectly set
      //   or ...
      return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);
    }

    if (4 == face_edge_count[0] && 4 == face_edge_count[1])
    {
      // common case when both neighboring faces are quads
      subdivision_point[0] = EP[0] + 0.0625*(facePsum[0][0] + facePsum[1][0]);
      subdivision_point[1] = EP[1] + 0.0625*(facePsum[0][1] + facePsum[1][1]);
      subdivision_point[2] = EP[2] + 0.0625*(facePsum[0][2] + facePsum[1][2]);
      if (edge_sharpness > 0.0)
      {
        // sharp crease vertex with 0 < sharpness < 1 at the current level
        const double a = 1.0 - edge_sharpness;
        subdivision_point[0] = a * subdivision_point[0] + edge_sharpness * sharp_subdivision_point.x;
        subdivision_point[1] = a * subdivision_point[1] + edge_sharpness * sharp_subdivision_point.y;
        subdivision_point[2] = a * subdivision_point[2] + edge_sharpness * sharp_subdivision_point.z;
      }
      return true;
    }
    
    if (3 == face_edge_count[0] && 3 == face_edge_count[1])
    {
      // common case when both neighboring faces are triangles

      //// bug in evaluation prior to Nov 11, 2019
      ////subdivision_point[0] = EP[0] + 0.125*(facePsum[0][0] + facePsum[1][0]);
      ////subdivision_point[1] = EP[1] + 0.125*(facePsum[0][1] + facePsum[1][1]);
      ////subdivision_point[2] = EP[2] + 0.125*(facePsum[0][2] + facePsum[1][2]);
      subdivision_point[0] = EP[0] + (0.5*edgePsum[0] + facePsum[0][0] + facePsum[1][0]) / 12.0;
      subdivision_point[1] = EP[1] + (0.5*edgePsum[1] + facePsum[0][1] + facePsum[1][1]) / 12.0;
      subdivision_point[2] = EP[2] + (0.5*edgePsum[2] + facePsum[0][2] + facePsum[1][2]) / 12.0;
      if (edge_sharpness > 0.0)
      {
        // sharp crease vertex with 0 < sharpness < 1 at the current level
        const double a = 1.0 - edge_sharpness;
        subdivision_point[0] = a * subdivision_point[0] + edge_sharpness * sharp_subdivision_point.x;
        subdivision_point[1] = a * subdivision_point[1] + edge_sharpness * sharp_subdivision_point.y;
        subdivision_point[2] = a * subdivision_point[2] + edge_sharpness * sharp_subdivision_point.z;
      }
      return true;
    }

    // general formula works for all cases including face_edge_count[0] != face_count[2]
      //// bug in evaluation prior to Nov 11, 2019
    ////const double f0 = 0.125 / ((double)(face_edge_count[0] - 2));
    ////const double f1 = 0.125 / ((double)(face_edge_count[1] - 2));
    ////subdivision_point[0] = EP[0] + f0 * facePsum[0][0] + f1 * facePsum[1][0];
    ////subdivision_point[1] = EP[1] + f0 * facePsum[0][1] + f1 * facePsum[1][1];
    ////subdivision_point[2] = EP[2] + f0 * facePsum[0][2] + f1 * facePsum[1][2];

    const double f0 = (double)(face_edge_count[0] * 4U);
    const double f1 = (double)(face_edge_count[1] * 4U);
    const double x = 1.0 / f0 + 1.0 / f1 - 0.125;
    subdivision_point[0] = EP[0] + x * edgePsum[0] + facePsum[0][0] / f0 + facePsum[1][0] / f1;
    subdivision_point[1] = EP[1] + x * edgePsum[1] + facePsum[0][1] / f0 + facePsum[1][1] / f1;
    subdivision_point[2] = EP[2] + x * edgePsum[2] + facePsum[0][2] / f0 + facePsum[1][2] / f1;
    if (edge_sharpness > 0.0)
    {
      // sharp crease vertex with 0 < sharpness < 1 at the current level
      const double a = 1.0 - edge_sharpness;
      subdivision_point[0] = a * subdivision_point[0] + edge_sharpness * sharp_subdivision_point.x;
      subdivision_point[1] = a * subdivision_point[1] + edge_sharpness * sharp_subdivision_point.x;
      subdivision_point[2] = a * subdivision_point[2] + edge_sharpness * sharp_subdivision_point.x;
    }
    return true;
  }

  if ( IsCrease() )
  {
    subdivision_point[0] = 0.5*edgePsum[0];
    subdivision_point[1] = 0.5*edgePsum[1];
    subdivision_point[2] = 0.5*edgePsum[2];
    return true;
  }

  // invalid edge->m_edge_tag
  return MYON_SubDEdge_GetSubdivisionPointError(this, subdivision_point, edgeP, true);
}











static unsigned int GetSectorBoundaryEdgesError()
{
  return MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubDEdge::GetSectorBoundaryEdges(
  unsigned int edge_vertex_index,
  MYON_SubDEdgePtr* edge_ptr0,
  MYON_SubDEdgePtr* edge_ptr1
  ) const
{
  if (nullptr != edge_ptr0)
    *edge_ptr0 = MYON_SubDEdgePtr::Null;
  if (nullptr != edge_ptr1)
    *edge_ptr1 = MYON_SubDEdgePtr::Null;

  const unsigned int edge_face_count = m_face_count;
  if (edge_face_count <= 0 || edge_face_count > 2)
    return GetSectorBoundaryEdgesError();

  if (2 == edge_face_count && MYON_SubDEdgeTag::Crease == m_edge_tag)
    return GetSectorBoundaryEdgesError();

  if (0 != edge_vertex_index && 1 != edge_vertex_index)
    return GetSectorBoundaryEdgesError();

  const MYON_SubDVertex* vertex = m_vertex[edge_vertex_index];
  if (nullptr == vertex || vertex->m_face_count <= 0)
    return GetSectorBoundaryEdgesError();

  const unsigned int vertex_face_count = vertex->m_face_count;
  unsigned int sector_face_count = 0;
  MYON_SubDEdgePtr sector_boundary[2] = {};
  for (unsigned int edge_face_index = 0; edge_face_index < edge_face_count; edge_face_index++)
  {
    const MYON_SubDEdge* edge0 = this;
    unsigned int edge0_end_index = edge_vertex_index;
    unsigned int edge0_face_index = edge_face_index;
    MYON_SubDFacePtr face_ptr = edge0->m_face2[edge0_face_index];
    while (sector_face_count < vertex_face_count)
    {
      const MYON_SubDFace* face = MYON_SUBD_FACE_POINTER(face_ptr.m_ptr);
      if (0 == face)
        return GetSectorBoundaryEdgesError();
      MYON__UINT_PTR face_dir = MYON_SUBD_FACE_DIRECTION(face_ptr.m_ptr);

      sector_face_count++;

      unsigned int face_edge0_index = face->EdgeArrayIndex(edge0);
      if (MYON_UNSET_UINT_INDEX == face_edge0_index)
        return 0;

      unsigned int face_edge1_index
        = face_edge0_index;
      face_edge1_index +=
        (1 == (edge0_end_index + face_dir))
        ? 1
        : (face->m_edge_count - 1);
      face_edge1_index %= face->m_edge_count;

      MYON_SubDEdgePtr edge1_ptr = face->EdgePtr(face_edge1_index);
      const MYON_SubDEdge* edge1 = MYON_SUBD_EDGE_POINTER(edge1_ptr.m_ptr);
      if (nullptr == edge1)
        return GetSectorBoundaryEdgesError();

      unsigned int edge1_end_index = (0 == face_dir) ? (1 - edge0_end_index) : edge0_end_index;
      if (1 == MYON_SUBD_EDGE_DIRECTION(edge1_ptr.m_ptr))
        edge1_end_index = 1 - edge1_end_index;
      if (vertex != edge1->m_vertex[edge1_end_index])
        return GetSectorBoundaryEdgesError();

      if ( edge1->IsSmooth() && 2 == edge1->m_face_count )
      {
        const MYON_SubDFace* edge1_faces[2] = { MYON_SUBD_FACE_POINTER(edge1->m_face2[0].m_ptr), MYON_SUBD_FACE_POINTER(edge1->m_face2[1].m_ptr) };
        unsigned int edge1_face_index = (face == edge1_faces[0] ? 1 : 0);
        if (nullptr == edge1_faces[edge1_face_index] || face == edge1_faces[edge1_face_index])
          return GetSectorBoundaryEdgesError();
        face_ptr = edge1->m_face2[edge1_face_index];
        edge0 = edge1;
        edge0_face_index = edge1_face_index;
        edge0_end_index = edge1_end_index;
        continue;
      }

      sector_boundary[edge_face_index] = MYON_SubDEdgePtr::Create(edge1, edge1_end_index);
      break;
    }
  }
  
  if (sector_face_count <= 0 || sector_boundary[0].IsNull())
    return GetSectorBoundaryEdgesError();
  if (1 == edge_face_count)
    sector_boundary[1] = MYON_SubDEdgePtr::Create(this, edge_vertex_index);
  else if (sector_boundary[1].IsNull())
    return GetSectorBoundaryEdgesError();

  if (nullptr != edge_ptr0)
    *edge_ptr0 = sector_boundary[0];
  if (nullptr != edge_ptr1)
    *edge_ptr1 = sector_boundary[1];

  return sector_face_count;
}

class MYON_ScratchBuffer
{
public:
  MYON_ScratchBuffer(
    size_t sizeof_buffer,
    void* stack_buffer,
    size_t sizeof_stack_buffer
    )
    : m_buffer(nullptr)
    , m_heap_buffer(nullptr)
  {
    m_buffer
      = (sizeof_buffer > sizeof_stack_buffer || nullptr == stack_buffer)
      ? stack_buffer
      : (m_heap_buffer = new (std::nothrow) double[1 + sizeof_buffer / sizeof(double)]);
  }

  void* Buffer()
  {
    return m_buffer;
  }

  ~MYON_ScratchBuffer()
  {
    if (nullptr != m_heap_buffer)
    {
      double* p = m_heap_buffer;
      m_heap_buffer = nullptr;
      delete[] p;
    }
  }

private:
  void* m_buffer;
  double* m_heap_buffer;

private:
  // prohibit use - no implementation
  MYON_ScratchBuffer(const MYON_ScratchBuffer&);
  MYON_ScratchBuffer& operator-(const MYON_ScratchBuffer&);
};

class FACE_AND_FACE_POINT
{
public:
  const MYON_SubDFace* m_face;
  double m_faceP[3];
  static int CompareFacePointer(const void* a, const void* b);
};

int FACE_AND_FACE_POINT::CompareFacePointer(const void* a, const void* b)
{
  MYON__UINT_PTR fa = (MYON__UINT_PTR)(((const FACE_AND_FACE_POINT*)a)->m_face);
  MYON__UINT_PTR fb = (MYON__UINT_PTR)(((const FACE_AND_FACE_POINT*)b)->m_face);
  if (fa < fb)
    return -1;
  if (fa > fb)
    return 1;
  return 0;
}

const MYON_3dPoint MYON_SubDSectorSurfacePoint::Point() const
{
  return MYON_3dPoint(m_limitP);
}

const MYON_3dVector MYON_SubDSectorSurfacePoint::Normal() const
{
  return MYON_3dPoint(m_limitN);
}

const MYON_3dVector MYON_SubDSectorSurfacePoint::Tangent(int tangent_index) const
{
  if (0 == tangent_index)
    return MYON_3dPoint(m_limitT1);
  if (1 == tangent_index)
    return MYON_3dPoint(m_limitT2);
  return MYON_3dVector::ZeroVector;
}


bool MYON_SubDSectorSurfacePoint::IsUnset() const
{
  return (m_limitP[0] == MYON_UNSET_VALUE);
}

bool MYON_SubDSectorSurfacePoint::IsNan() const
{
  return !(m_limitP[0] == m_limitP[0]);
}

bool MYON_SubDSectorSurfacePoint::IsZero() const
{
  const double* p = m_limitP;
  const double* p1 = p+12;
  while (p < p1)
  {
    if (!(0.0 == *p++))
      return false;
  }
  return true;
}

bool MYON_SubDSectorSurfacePoint::IsSet(
  bool bUndefinedNormalIsPossible
) const
{
  double x, y;
  const double* p = m_limitP;
  const double* p1 = p+3;
  while (p < p1)
  {
    x = *p++;
    if (MYON_UNSET_VALUE == x || !(x == x))
      return false;
  }

  p = m_limitT1;
  p1 = p + 6;
  while (p < p1)
  {
    const double* p2 = p + 3;
    y = 0.0;
    while (p < p2)
    {
      x = *p++;
      if (MYON_UNSET_VALUE == x || !(x == x))
        return false;
      if (0.0 != x)
        y = x;
    }
    if (false == bUndefinedNormalIsPossible)
    {
      if (!(y != 0.0))
        return false;
    }

    p = m_limitN;
    p1 = p + 3;
    y = 0.0;
    while (p < p1)
    {
      x = *p++;
      if (MYON_UNSET_VALUE == x || !(x == x))
        return false;
      y += x * x;
    }
    if (false == bUndefinedNormalIsPossible)
    {
      if (!(fabs(y - 1.0) <= 1e-4))
        return false;
    }
  }

  return true;
}

void MYON_SubDVertex::CopyFrom(
  const MYON_SubDVertex* src,
  bool bCopyEdgeArray,
  bool bCopyFaceArray,
  bool bCopyLimitPointList,
  bool bCopySymmetrySetNext
  )
{
  if (nullptr == src)
    src = &MYON_SubDVertex::Empty;

  ClearSavedSubdivisionPoints();
  CopyBaseFrom(src, bCopySymmetrySetNext);

  m_vertex_tag = src->m_vertex_tag;

  m_P[0] = src->m_P[0];
  m_P[1] = src->m_P[1];
  m_P[2] = src->m_P[2];

  if (bCopyLimitPointList)
  {
    if ( src->SurfacePointIsSet() )
    {
      for (const MYON_SubDSectorSurfacePoint* p = &src->m_limit_point; nullptr != p; p = p->m_next_sector_limit_point)
      {
        MYON_SubDSectorSurfacePoint limit_point = *p;
        limit_point.m_next_sector_limit_point = (MYON_SubDSectorSurfacePoint*)1; // disable checks
        SetSavedSurfacePoint( true, limit_point);
      }
    }
  }

  if (bCopyEdgeArray)
  {
    if (src->m_edge_count > 0 && nullptr != src->m_edges && nullptr != m_edges && src->m_edge_count <= m_edge_capacity)
    {
      m_edge_count = src->m_edge_count;
      const unsigned int edge_count = src->m_edge_count;
      for (unsigned int vei = 0; vei < edge_count; vei++)
        m_edges[vei] = src->m_edges[vei];
    }
    else
      m_edge_count = 0;
  }

  if (bCopyFaceArray)
  {
    if (src->m_face_count > 0 && nullptr != src->m_faces && nullptr != m_faces && src->m_face_count <= m_face_capacity)
    {
      m_face_count = src->m_face_count;
      const unsigned int face_count = src->m_face_count;
      for (unsigned int vfi = 0; vfi < face_count; vfi++)
        m_faces[vfi] = src->m_faces[vfi];
    }
    else
      m_face_count = 0;
  }
}


static bool MYON_SubDVertex_GetSubdivisionPointError(
  const class MYON_SubDVertex* vertex,
  double vertex_point[3],
  const double* vertexP,
  bool bDamagedState
  )
{
  if (nullptr == vertex || nullptr == vertex_point)
    return false; // caller passed a null pointer - vertex is not necessarily damaged

  MYON_SubDIncrementErrorCount();
  vertex->m_status.SetDamagedState(bDamagedState);

  vertex->ClearSavedSubdivisionPoints();

  if (nullptr != vertexP)
  {
    vertex_point[0] = vertexP[0];
    vertex_point[1] = vertexP[1];
    vertex_point[2] = vertexP[2];
  }

  return true;
}

bool MYON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(
  const class MYON_SubDVertex* vertex,
  double vertex_point[3]
)
{
  if (nullptr == vertex_point)
  {
    MYON_SUBD_ERROR("input vertex_point is nullptr.");
    return false;
  }

  vertex_point[0] = MYON_DBL_QNAN;
  vertex_point[1] = MYON_DBL_QNAN;
  vertex_point[2] = MYON_DBL_QNAN;

  if (nullptr == vertex)
  {
    MYON_SUBD_ERROR("input vertex is nullptr.");
    return false;
  }

  const unsigned int n = vertex->m_face_count;
  if (nullptr == vertex->m_faces
    || nullptr == vertex->m_edges
    || vertex->m_face_count != vertex->m_edge_count
    || n < MYON_SubDSectorType::MinimumSectorFaceCount(MYON_SubDVertexTag::Smooth)
    )
  {
    MYON_SUBD_ERROR("input vertex is not valid.");
    return false;
  }

  const double* vertexP = vertex->m_P;

  MYON_3dPoint sharp_subdivision_point;
  const double vertex_sharpness = vertex->GetSharpSubdivisionPoint(sharp_subdivision_point);
  if (vertex_sharpness >= 1.0)
  {
    vertex_point[0] = sharp_subdivision_point.x;
    vertex_point[1] = sharp_subdivision_point.y;
    vertex_point[2] = sharp_subdivision_point.z;
    return true;
  }

  // It is critical to use the centroids of the neighboring faces
  // in this step because the number of edges in each face's
  // boundary may not be constant.
  double facePsum[3] = {};
  const MYON_SubDFace*const* vertex_faces = vertex->m_faces;
  for (unsigned int i = 0; i < n; i++)
  {
    const MYON_SubDFace* face = vertex_faces[i];
    if (nullptr != face)
    {
      double faceC[3];
      if (face->GetSubdivisionPoint( faceC))
      {
        facePsum[0] += faceC[0];
        facePsum[1] += faceC[1];
        facePsum[2] += faceC[2];
        continue;
      }
    }
    // treat missing or damaged face as infinitesimally small
    facePsum[0] += vertexP[0];
    facePsum[1] += vertexP[1];
    facePsum[2] += vertexP[2];
  }

  double edgePsum[3] = {};
  class MYON_SubDEdgePtr* edges = vertex->m_edges;
  for (unsigned int i = 0; i < n; i++)
  {
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(edges[i].m_ptr);
    if (nullptr != edge)
    {
      const MYON_SubDVertex* edge_vertex = edge->OtherEndVertex(vertex);
      if (nullptr != edge_vertex)
      {
        const double* edgeP = edge_vertex->m_P;
        edgePsum[0] += edgeP[0];
        edgePsum[1] += edgeP[1];
        edgePsum[2] += edgeP[2];
        continue;
      }
    }
    // treat missing or damaged edge as infinitesimally small
    edgePsum[0] += vertexP[0];
    edgePsum[1] += vertexP[1];
    edgePsum[2] += vertexP[2];
  }

  const double v_weight = 1.0 - 2.0 / ((double)n);
  const double ef_weight = 1.0 / ((double)(n*n));
  vertex_point[0] = v_weight*vertexP[0] + ef_weight*(edgePsum[0] + facePsum[0]);
  vertex_point[1] = v_weight*vertexP[1] + ef_weight*(edgePsum[1] + facePsum[1]);
  vertex_point[2] = v_weight*vertexP[2] + ef_weight*(edgePsum[2] + facePsum[2]);

  if (vertex_sharpness > 0.0)
  {
    // 0 < vertex_sharpness < 1
    const double a = 1.0 - vertex_sharpness;
    vertex_point[0] = a * vertex_point[0] + vertex_sharpness * sharp_subdivision_point.x;
    vertex_point[1] = a * vertex_point[1] + vertex_sharpness * sharp_subdivision_point.y;
    vertex_point[2] = a * vertex_point[2] + vertex_sharpness * sharp_subdivision_point.z;
    return true;
  }

  return true;
}

bool MYON_SubDVertex::Internal_GetCatmullClarkSubdivisionPoint(
  const class MYON_SubDVertex* vertex,
  double vertex_point[3]
  )
{
  // This function is used to convert an arbitrary control polygon into the
  // "level 1" quad subD.  It cannot use the faster sub-D formulas because
  // a face can have an arbitrary number of edges.
  if (nullptr == vertex || nullptr == vertex_point)
    return MYON_SubDVertex_GetSubdivisionPointError(vertex,vertex_point,nullptr,false);

  const double* vertexP = vertex->m_P;

  const unsigned int n = (nullptr != vertex->m_edges ? vertex->m_edge_count : 0);
  if (MYON_SubDVertexTag::Smooth == vertex->m_vertex_tag || MYON_SubDVertexTag::Dart == vertex->m_vertex_tag)
  {
    const unsigned int minimum_n = MYON_SubDSectorType::MinimumSectorEdgeCount(vertex->m_vertex_tag);
    if (n < minimum_n || n != vertex->m_face_count || nullptr == vertex->m_faces)
      return MYON_SubDVertex_GetSubdivisionPointError(vertex, vertex_point, vertexP, true);

    double facePsum[3] = {};
    const MYON_SubDFace*const* vertex_faces = vertex->m_faces;

    const MYON_SubDFace* face = vertex_faces[0];
    if (nullptr == face)
      return MYON_SubDVertex_GetSubdivisionPointError(vertex, vertex_point, vertexP, true);

    ////// for debugging code below, uncomment this line
    ////// and look for differences in results.
    ////return GetGeneralQuadSubdivisionPoint(vertex, vertex_point);

    const unsigned int k = (nullptr == face) ? 0U : face->m_edge_count;
    if (4 == k)
    {
      // possibly (probably?) every face is a quad
      double sum[3];
      for (unsigned int i = 0; i < n; i++)
      {
        const MYON_SubDFace* vface = vertex_faces[i];
        const unsigned int face_n = MYON_SubDVertex::Internal_GetFacePointSum(vface, vertex, sum);
        if (4 != face_n)
        {
          // The first face is a quadrangle and this face is not a quadrangle.
          //
          // It is critical to use the centroids of the neighboring faces
          // for this vertex subdivision point because the number of edges
          // in each face's boundary is not constant.
          return MYON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(vertex, vertex_point);
        }
        facePsum[0] += sum[0];
        facePsum[1] += sum[1];
        facePsum[2] += sum[2];
      }
    }
    else if (3 == k)
    {
      // possibly (probably?) every face is a triangle
      for (unsigned int i = 0; i < n; i++)
      {
        const MYON_SubDFace* vface = vertex_faces[i];
        if (k != ((nullptr == vface) ? 0U : vface->m_edge_count))
        {
          // The first face is a triangle and this face is not a triangle.
          //
          // It is critical to use the centroids of the neighboring faces
          // for this vertex subdivision point because the number of edges
          // in each face's boundary is not constant.
          return MYON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(vertex, vertex_point);
        }
      }
    }
    else
    {
      // The first face has 5 or more edges.  
      // It is likely this is the initial subdivision being applied
      // to the level zero SubD control polygon.
      //
      // It may be critical to use the centroids of the neighboring faces
      // for this vertex subdivision point because the number of edges
      // in each face's boundary may not constant.  In any case, this
      // situation is not common and typically happens only on the
      // first subdivision step.
      return MYON_SubDVertex::Internal_GetGeneralQuadSubdivisionPoint(vertex, vertex_point);
    }

    MYON_3dPoint sharp_subdivision_point = MYON_3dPoint::NanPoint;
    const double vertex_sharpness = vertex->GetSharpSubdivisionPoint(sharp_subdivision_point);
    if (vertex_sharpness >= 1.0)
    {
      // use corner subdivision point
      vertex_point[0] = sharp_subdivision_point.x;
      vertex_point[1] = sharp_subdivision_point.y;
      vertex_point[2] = sharp_subdivision_point.z;
      return true;
    }

    double edgePsum[3] = {};
    class MYON_SubDEdgePtr* edges = vertex->m_edges;
    for (unsigned int i = 0; i < n; i++)
    {
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(edges[i].m_ptr);
      if (nullptr != edge)
      {
        const MYON_SubDVertex* edge_vertex = edge->OtherEndVertex(vertex);
        if (nullptr != edge_vertex)
        {
          const double* edgeP = edge_vertex->m_P;
          edgePsum[0] += edgeP[0];
          edgePsum[1] += edgeP[1];
          edgePsum[2] += edgeP[2];
          continue;
        }
      }
      // treat missing or damaged edge as infinitesimally small
      edgePsum[0] += vertexP[0];
      edgePsum[1] += vertexP[1];
      edgePsum[2] += vertexP[2];
    }

    if (4 == k)
    {
      // all faces were quads
      const double v_weight = 1.0 - 1.75 / ((double)n);
      const double e_weight = 1.5 / ((double)(n*n));
      const double f_weight = 0.25 / ((double)(n*n));
      vertex_point[0] = v_weight*vertexP[0] + e_weight*edgePsum[0] + f_weight*facePsum[0];
      vertex_point[1] = v_weight*vertexP[1] + e_weight*edgePsum[1] + f_weight*facePsum[1];
      vertex_point[2] = v_weight*vertexP[2] + e_weight*edgePsum[2] + f_weight*facePsum[2];
    }
    else
    {
      // all faces were triangles
      const double v_weight = 1.0 - 5.0 / ((double)(3 * n));
      const double e_weight = 5.0 / ((double)(3*n*n));
      vertex_point[0] = v_weight*vertexP[0] + e_weight*edgePsum[0];
      vertex_point[1] = v_weight*vertexP[1] + e_weight*edgePsum[1];
      vertex_point[2] = v_weight*vertexP[2] + e_weight*edgePsum[2];
    }

    if (vertex_sharpness > 0.0)
    {
      // 0 < vertex_sharpness < 1
      const double a = 1.0 - vertex_sharpness;
      vertex_point[0] = a * vertex_point[0] + vertex_sharpness * sharp_subdivision_point.x;
      vertex_point[1] = a * vertex_point[1] + vertex_sharpness * sharp_subdivision_point.y;
      vertex_point[2] = a * vertex_point[2] + vertex_sharpness * sharp_subdivision_point.z;
      return true;
    }

    return true;
  }

  // vertex->m_vertex_tag is damaged
  return MYON_SubDVertex_GetSubdivisionPointError(vertex, vertex_point, vertexP, true);
}


const MYON_3dPoint MYON_SubDVertex::SubdivisionPoint() const
{
  MYON_3dPoint S;
  return (GetSubdivisionPoint(&S.x) && S.IsValid()) ? S : MYON_3dPoint::NanPoint;
}

bool MYON_SubDVertex::GetSubdivisionPoint(
  double subdivision_point[3]
) const
{
  if (nullptr == subdivision_point)
    return MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  if (GetSavedSubdivisionPoint(subdivision_point))
    return true;

  if (EvaluateCatmullClarkSubdivisionPoint(subdivision_point))
  {
    SetSavedSubdivisionPoint(subdivision_point);
    return true;
  }

  return false;
}

bool MYON_SubDVertex::EvaluateCatmullClarkSubdivisionPoint(double subdivision_point[3]) const
{
  // This function is used to convert an arbitrary control polygon into the
  // "level 1" subD.  It cannot use the faster sub-D formulas because
  // a face can have an arbitrary number of edges.
  if (nullptr == subdivision_point )
    return MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, nullptr, false);

  const double* vertexP = m_P;

  const unsigned int n = (nullptr != m_edges ? m_edge_count : 0);
  if (n < 2)
    return MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);

  if (MYON_SubDVertexTag::Smooth == m_vertex_tag || MYON_SubDVertexTag::Dart == m_vertex_tag)
  {
    return MYON_SubDVertex::Internal_GetCatmullClarkSubdivisionPoint(this, subdivision_point);
  }

  if (MYON_SubDVertexTag::Crease == m_vertex_tag)
  {
    class MYON_SubDEdgePtr* edges = m_edges;
    const MYON_SubDVertex* edge0_vertex = nullptr;
    const MYON_SubDVertex* edge1_vertex = nullptr;
    bool bSharpEdges = false;

    for (unsigned int i = 0; i < n; i++)
    {
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(edges[i].m_ptr);
      if (nullptr == edge)
      {
        MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
        continue;
      }

      if (MYON_SubDEdgeTag::Crease != edge->m_edge_tag)
      {
        if (false == bSharpEdges)
          bSharpEdges = edge->IsSharp();
        continue;
      }

      const MYON_SubDVertex* edge_vertex = edge->OtherEndVertex(this);

      if (nullptr == edge_vertex)
      {
        MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
        continue;
      }

      if (nullptr == edge0_vertex)
      {
        edge0_vertex = edge_vertex;
        continue;
      }

      if (edge0_vertex == edge_vertex)
      {
        MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
        continue;
      }

      if (nullptr == edge1_vertex)
      {
        edge1_vertex = edge_vertex;
        continue;
      }

      if (edge1_vertex == edge_vertex)
      {
        MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
        continue;
      }

      // 3 or more creased edges attached to this crease vertex.
      MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
      subdivision_point[0] = vertexP[0];
      subdivision_point[1] = vertexP[1];
      subdivision_point[2] = vertexP[2];
      return true;
    }

    if (nullptr != edge0_vertex && nullptr != edge1_vertex)
    {
      // We found the two crease edges that share this crease vertex.
      MYON_3dPoint sharp_subdivision_point = MYON_3dPoint::NanPoint;
      const double vertex_sharpness 
        = bSharpEdges
        ? this->GetSharpSubdivisionPoint(sharp_subdivision_point)
        : 0.0;
      if (vertex_sharpness >= 1.0)
      {
        // 2 creases and a sharp edge with sharpness >= 1
        // This modification is not part of the DKT1998 Pixar algorithm.
        // It is required to get sharp edges to evenly end at
        // ordinary boundaries. It does break crease edge evaluation relying
        // only on boundary settings, but at this time (Dale Lear Nov 2022),
        // I'm guessing it is the best option out of 2 nonideal choices.
        subdivision_point[0] = sharp_subdivision_point.x;
        subdivision_point[1] = sharp_subdivision_point.y;
        subdivision_point[2] = sharp_subdivision_point.z;
        return true;
      }

      // (The parenthesis around the edgeP sum is to insure this result 
      // is independent of the order of the edges.)
      vertexP = m_P;

      const double* edgeP[2] = { edge0_vertex->m_P, edge1_vertex->m_P };
      subdivision_point[0] = (vertexP[0] * 6.0 + (edgeP[0][0] + edgeP[1][0])) * 0.125;
      subdivision_point[1] = (vertexP[1] * 6.0 + (edgeP[0][1] + edgeP[1][1])) * 0.125;
      subdivision_point[2] = (vertexP[2] * 6.0 + (edgeP[0][2] + edgeP[1][2])) * 0.125;
      if (vertex_sharpness > 0.0)
      {
        // 2 creases and a sharp edge with 0 < sharpness < 1
        // 
        // This modification is not part of the DKT1998 Pixar algorithm.
        // It is required to get sharp edges to evenly end at ordinary boundaries. 
        // It does break crease edge evaluation relying
        // only on boundary settings, but at this time (Dale Lear Nov 2022),
        // I'm guessing it is the best option out of 2 nonideal choices.
        //
        const double a = 1.0 - vertex_sharpness;
        subdivision_point[0] = a * subdivision_point[0] + vertex_sharpness * sharp_subdivision_point.x;
        subdivision_point[1] = a * subdivision_point[1] + vertex_sharpness * sharp_subdivision_point.y;
        subdivision_point[2] = a * subdivision_point[2] + vertex_sharpness * sharp_subdivision_point.z;
        return true;
      }
      return true;
    }

    return MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
  }

  if (MYON_SubDVertexTag::Corner == m_vertex_tag)
  {
    vertexP = m_P;
    subdivision_point[0] = vertexP[0];
    subdivision_point[1] = vertexP[1];
    subdivision_point[2] = vertexP[2];
    return true;
  }

  // vertex is damaged
  return MYON_SubDVertex_GetSubdivisionPointError(this, subdivision_point, vertexP, true);
}

unsigned int MYON_SubDVertex::Internal_GetFacePointSum(
  const MYON_SubDFace* face,
  const MYON_SubDVertex* vertex,
  double* facePsum
  )
{
  const MYON_SubDEdge* e;
  MYON__UINT_PTR e_ptr, edir;
  const double* faceP;

  if (nullptr == face)
    return 0;

  const unsigned int n = face->m_edge_count;

  facePsum[0] = 0.0;
  facePsum[1] = 0.0;
  facePsum[2] = 0.0;

  if (3 == n)
  {
    return n;
  }

  if (4 == n)
  {
    for (unsigned int i = 0; i < 4; i++)
    {
      e_ptr = face->m_edge4[i].m_ptr;
      e = MYON_SUBD_EDGE_POINTER(e_ptr);
      if (nullptr != e && (vertex == e->m_vertex[0] || vertex == e->m_vertex[1]))
      {
        edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
        e_ptr = face->m_edge4[(i + ((vertex == e->m_vertex[edir]) ? 2 : 3)) % 4].m_ptr;
        e = MYON_SUBD_EDGE_POINTER(e_ptr);
        edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
        if (nullptr == e || nullptr == e->m_vertex[edir])
          return 0;
        faceP = e->m_vertex[edir]->m_P;
        facePsum[0] = faceP[0];
        facePsum[1] = faceP[1];
        facePsum[2] = faceP[2];
        return n;
      }
    }
    return 0;
  }

  if (n <= 4 || nullptr == face->m_edgex)
    return 0;

  e_ptr = face->m_edgex[n-5].m_ptr;
  e = MYON_SUBD_EDGE_POINTER(e_ptr);
  if (nullptr == e)
    return 0;
  edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
  unsigned int skipped_edge_count = (vertex == e->m_vertex[edir]) ? 1 : 0;
  unsigned int facePcount = 0;
  const MYON_SubDEdgePtr* edge_ptrs = face->m_edge4;
  for (unsigned int i = skipped_edge_count; i < n; i++)
  {
    if (4 == i)
      edge_ptrs = face->m_edgex - 4;
    e_ptr = edge_ptrs[i].m_ptr;
    e = MYON_SUBD_EDGE_POINTER(e_ptr);
    if (nullptr == e)
      return 0;
    edir = MYON_SUBD_EDGE_DIRECTION(e_ptr);
    if (vertex == e->m_vertex[0] || vertex == e->m_vertex[1])
    {
      skipped_edge_count++;
      if (skipped_edge_count > 2)
      {
        facePsum[0] = 0.0;
        facePsum[1] = 0.0;
        facePsum[2] = 0.0;
        return 0;
      }
      if (vertex == e->m_vertex[edir])
      {
        i++;
        if (4 == i)
          edge_ptrs = face->m_edgex - 4;
      }
      continue;
    }
    faceP = e->m_vertex[edir]->m_P;
    facePsum[0] += faceP[0];
    facePsum[1] += faceP[1];
    facePsum[2] += faceP[2];
    facePcount++;
  }

  if (n == facePcount + 3)
    return n;

  facePsum[0] = 0.0;
  facePsum[1] = 0.0;
  facePsum[2] = 0.0;
  return 0;
}

bool MYON_SubDimple::LocalSubdivide(
  MYON_SubDFace const*const* face_list,
  size_t face_list_count
)
{
  if (nullptr == face_list || face_list_count < 1 || m_levels.UnsignedCount() < 1)
    return false;

  unsigned int level0_index = MYON_UNSET_UINT_INDEX;
  for (size_t i = 0; i < face_list_count; ++i)
  {
    const MYON_SubDFace* f = face_list[i];
    if (nullptr == f || f->m_edge_count < 3 || f->SubdivisionLevel() >= m_levels.UnsignedCount())
      continue;
    level0_index = f->SubdivisionLevel();
    break;
  }
  
  if (level0_index >= m_levels.UnsignedCount() || nullptr == m_levels[level0_index])
    return MYON_SUBD_RETURN_ERROR(false);

  ClearHigherSubdivisionLevels(level0_index + 1);
  if (level0_index + 1 != m_levels.UnsignedCount())
    return MYON_SUBD_RETURN_ERROR(false);

  m_active_level = m_levels[level0_index];
  if ( nullptr == m_active_level || 0 == m_active_level->m_face_count)
    return MYON_SUBD_RETURN_ERROR(false);

  MYON_SubDLevel& level0 = *m_levels[level0_index];
  m_active_level = &level0;

  level0.ClearRuntimeMarks(true, true, true);

  unsigned face_count = 0;
  unsigned edge_count = 0;
  unsigned vertex_count = 0;
  MYON_3dPoint P;
  for (size_t i = 0; i < face_list_count; ++i)
  {
    const MYON_SubDFace* f = face_list[i];
    if (nullptr == f || f->m_edge_count < 3 || level0_index != f->SubdivisionLevel() )
      continue;
    if (f->m_status.RuntimeMark())
      continue;
    f->m_status.SetRuntimeMark();
    if ( false == f->GetSubdivisionPoint(P))
      return MYON_SUBD_RETURN_ERROR(false);
    const MYON_SubDEdgePtr* eptr = f->m_edge4;
    for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++eptr)
    {
      if (4 == fei)
      {
        eptr = f->m_edgex;
        if (nullptr == eptr)
          return MYON_SUBD_RETURN_ERROR(false);
      }
      MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
      if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
        return MYON_SUBD_RETURN_ERROR(false);
      if (e->m_status.RuntimeMark())
        continue;
      e->UpdateEdgeSectorCoefficientsForExperts(true);
      e->m_status.SetRuntimeMark();
      if (false == e->GetSubdivisionPoint(P))
        return MYON_SUBD_RETURN_ERROR(false);
      for (unsigned evi = 0; evi < 2; ++evi)
      {
        if (e->m_vertex[evi]->m_status.RuntimeMark())
          continue;
        e->m_vertex[evi]->m_status.SetRuntimeMark();
        if ( false == e->m_vertex[evi]->GetSubdivisionPoint(P) )
          return MYON_SUBD_RETURN_ERROR(false);
        ++vertex_count;
      }
      ++edge_count;
    }
    ++face_count;
  }
  if (face_count < 1 || edge_count < 3 || vertex_count < 3)
    return false;
  if (face_count >= level0.m_face_count)
    return GlobalSubdivide();

  // Get face subdivision points
  MYON_SimpleArray<MYON_SubDFace*> faces(face_count);
  MYON_SimpleArray<MYON_3dPoint> face_points(face_count);

  // this subd is being modifed.
  ChangeGeometryContentSerialNumber( false);

  for (const MYON_SubDFace* f0 = level0.m_face[0]; nullptr != f0; f0 = f0->m_next_face)
  {
    if (false == f0->m_status.RuntimeMark())
      continue;
    faces.Append(const_cast<MYON_SubDFace*>(f0));
    P = f0->SubdivisionPoint();
    if ( false == P.IsValid())
      return MYON_SUBD_RETURN_ERROR(false);
    face_points.Append(P);
  }
  if (face_count != face_points.UnsignedCount())
    return MYON_SUBD_RETURN_ERROR(false);

  // Get edge subdivision points
  MYON_SimpleArray<MYON_3dPoint> edge_points(edge_count);
  MYON_SimpleArray<MYON_SubDEdge*> edges(edge_count);
  for (const MYON_SubDEdge* e0 = level0.m_edge[0]; nullptr != e0; e0 = e0->m_next_edge)
  {
    if (false == e0->m_status.RuntimeMark())
      continue;
    P = (e0->IsSmooth() && 2 == e0->m_face_count && 2 == e0->MarkedFaceCount()) ? e0->SubdivisionPoint() : e0->ControlNetCenterPoint();
    edge_points.Append(P);
    edges.Append(const_cast<MYON_SubDEdge*>(e0));
  }
  if ( edge_count != edge_points.UnsignedCount())
    return MYON_SUBD_RETURN_ERROR(false);

  // Set vertex points
  for (const MYON_SubDVertex* v0 = level0.m_vertex[0]; nullptr != v0; v0 = v0->m_next_vertex)
  {
    if (false == v0->m_status.RuntimeMark() || v0->m_edge_count < 2 || ((unsigned)v0->m_edge_count) != v0->MarkedEdgeCount())
      continue;
    P = v0->SubdivisionPoint();
    MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>(v0);
    v->m_P[0] = P.x;
    v->m_P[1] = P.y;
    v->m_P[2] = P.z;
  }

  // split edges
  for (unsigned edex = 0; edex < edge_count; ++edex)
  {
    MYON_SubDEdge* e = edges[edex];

    const MYON_SubDEdgeSharpness e_sharpness = e->Sharpness();

    e->EdgeModifiedNofification();
    const MYON_SubDEdge* new_edge = SplitEdge(e, edge_points[edex]);
    if (nullptr == new_edge)
      return MYON_SUBD_RETURN_ERROR(false);
    new_edge->m_status.ClearRuntimeMark();
    e->m_status.SetRuntimeMark();

    // LOCAL subdivide is more of an add control points to an existing level
    // as opposed to a global subdivision level operation. It will be common
    // for the input to include some but not all sharp edges.
    // Dale Lear's best guess on Nov 9, 2022 is that
    // reusing the orginal sharpness is the best way to maximize overall
    // user happiness. This makes some sense because the level of the edges
    // is not changing in a LOCAL subdivide. 
    // It is a near certainty that some users will not like this in some cases. 
    const_cast<MYON_SubDEdge*>(e)->SetSharpnessForExperts(MYON_SubDEdgeSharpness::FromInterval(e_sharpness[0], e_sharpness.Average()));
    const_cast<MYON_SubDEdge*>(new_edge)->SetSharpnessForExperts(MYON_SubDEdgeSharpness::FromInterval(e_sharpness.Average(), e_sharpness[1]));
  }

  MYON_SimpleArray<MYON_SubDEdgePtr> fbdry(32);
  MYON_SimpleArray<MYON_SubDEdge*> radial_edges(32);
  for (unsigned fdex = 0; fdex < face_count; ++fdex)
  {
    MYON_SubDFace* f = faces[fdex];
    f->FaceModifiedNofification();
    P = face_points[fdex];
    fbdry.SetCount(0);
    const unsigned e_count = f->GetEdgeArray(fbdry);
    if (e_count < 6 || 0 != e_count %2)
      return MYON_SUBD_RETURN_ERROR(false);

    // Get edges[] with edge[0] = marked edges.
    const MYON_SubDEdgePtr* eptrs = fbdry.Array();
    if (nullptr != eptrs[0].RelativeVertex(0) && false == eptrs[0].RelativeVertex(0)->Mark() )
    {
      fbdry.Append(eptrs[0]);
      eptrs = fbdry.Array() + 1;
    }

    // save face status and candidate_face
    unsigned int candidate_face_id = f->m_id;
    const MYON_ComponentStatus fstatus = f->m_status;

    for (unsigned fei = 0; fei < e_count; ++fei)
    {
      MYON_SubDEdge* e = eptrs[fei].Edge();
      if ( nullptr == e)
        return MYON_SUBD_RETURN_ERROR(false);
      const MYON_SubDVertex* ev[2] = { eptrs[fei].RelativeVertex(0), eptrs[fei].RelativeVertex(1) };
      if ( nullptr == ev[0] || nullptr == ev[1] || ev[0] == ev[1])
        return MYON_SUBD_RETURN_ERROR(false);
      if (0 == fei % 2)
      {
        if (false == ev[0]->Mark())
          return MYON_SUBD_RETURN_ERROR(false);
        if (ev[1]->Mark())
          return MYON_SUBD_RETURN_ERROR(false);
      }
      else
      {
        if (ev[0]->Mark())
          return MYON_SUBD_RETURN_ERROR(false);
        if (false == ev[1]->Mark())
          return MYON_SUBD_RETURN_ERROR(false);
      }
    }

    // remove face that will be subdivided;
    for (unsigned short fei = 0; fei < f->m_edge_count; ++fei)
    {
      eptrs[fei].Edge()->RemoveFaceFromArray(f);
      const_cast<MYON_SubDVertex*>(eptrs[fei].RelativeVertex(0))->RemoveFaceFromArray(f);
    }
    ReturnFace(f);
    
    MYON_SubDVertex* center = AllocateVertex(MYON_SubDVertexTag::Smooth, level0_index, &P.x);
    AddVertexToLevel(center);

    radial_edges.SetCount(0);
    radial_edges.Reserve(e_count /2);
    for (unsigned fei = 0; fei < e_count; fei += 2)
    {
      MYON_SubDEdge* r = AddEdge(MYON_SubDEdgeTag::Smooth, center, MYON_SubDSectorType::UnsetSectorCoefficient, const_cast<MYON_SubDVertex*>(eptrs[fei].RelativeVertex(1)), MYON_SubDSectorType::UnsetSectorCoefficient);
      radial_edges.Append(r);
    }

    MYON_SubDEdge* r[2] = { nullptr,radial_edges[e_count / 2 - 1] };
    for (unsigned fei = 0; fei < e_count; fei += 2)
    {
      r[0] = r[1];
      r[1] = radial_edges[fei / 2];
      const MYON_SubDEdgePtr qbdry[4] = {
        MYON_SubDEdgePtr::Create(r[0],0),
        eptrs[(fei + e_count - 1) % e_count],
        eptrs[fei],
        MYON_SubDEdgePtr::Create(r[1],1) 
      };
      MYON_SubDFace* q = AddFace(candidate_face_id, 4, qbdry);
      candidate_face_id = 0;
      q->m_status = fstatus;
    }

  }

  level0.ClearRuntimeMarks(true, true, false);
  level0.ClearBoundingBox();
  level0.ClearEvaluationCache();
  level0.ClearEdgeFlags();

  if (nullptr != m_active_level)
    m_active_level->UpdateAllTagsAndSectorCoefficients(true);
  
  return true; 
}

unsigned int MYON_SubDimple::GlobalSubdivide()
{
  if (m_levels.UnsignedCount() <= 0)
    return MYON_SUBD_RETURN_ERROR(0U);
  const unsigned int level0_index = m_levels.UnsignedCount()-1;

  if ( nullptr == m_levels[level0_index])
    return MYON_SUBD_RETURN_ERROR(0U);

  MYON_SubDLevel& level0 = *m_levels[level0_index];
  if (level0.IsEmpty())
    return MYON_SUBD_RETURN_ERROR(0U);
  if ( level0.m_edge_count <= 0U )
    return MYON_SUBD_RETURN_ERROR(0U);

  level0.UpdateEdgeSectorCoefficients(true);

  const unsigned int level1_index = level0_index+1;
  
  MYON_SubDLevel* level1 = SubDLevel(level1_index,true);
  if ( nullptr == level1 )
    return MYON_SUBD_RETURN_ERROR(0);

  double P[3];
  MYON_SubDVertex* v;

  // If the object is currently symmetric, a global subdivision will not break symmetry
  const bool bChangePreservesSymmetry = true;
  this->ChangeGeometryContentSerialNumber(bChangePreservesSymmetry);

  // Add face points
  unsigned int max_pack_id = 0U;
  for (const MYON_SubDFace* f0 = level0.m_face[0]; nullptr != f0; f0 = f0->m_next_face)
  {
    if (f0->PackId() > max_pack_id) max_pack_id = f0->PackId();
    if (false == f0->GetSubdivisionPoint(P))
      continue;
    if (nullptr == f0->m_subd_point1)
    {
      const_cast<MYON_SubDFace*>(f0)->m_subd_point1 = v = AllocateVertex(MYON_SubDVertexTag::Smooth, level1_index, P);
      AddVertexToLevel(v);
    }
    else
    {
      v = const_cast<MYON_SubDVertex*>(f0->m_subd_point1);
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }
  }

  // Add edge points
  for (const MYON_SubDEdge* e0 = level0.m_edge[0]; nullptr != e0; e0 = e0->m_next_edge)
  {
    if (false == e0->GetSubdivisionPoint(P))
      continue;
    // (the subdivision point of an edge tagged as MYON_SubDEdgeTag::SmoothX is a smooth vertex.)
    const MYON_SubDVertexTag vertex_tag
      = MYON_SubDEdgeTag::Crease == e0->m_edge_tag
      ? MYON_SubDVertexTag::Crease
      : MYON_SubDVertexTag::Smooth;
    if (nullptr == e0->m_subd_point1)
    {
      const_cast<MYON_SubDEdge*>(e0)->m_subd_point1 = v = AllocateVertex(vertex_tag, level1_index, P );
      AddVertexToLevel(v);
    }
    else
    {
      v = const_cast<MYON_SubDVertex*>(e0->m_subd_point1);
      v->m_vertex_tag = vertex_tag;
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }
  }

  // Add vertex points
  for (const MYON_SubDVertex* v0 = level0.m_vertex[0]; nullptr != v0; v0 = v0->m_next_vertex)
  {
    if (false == v0->GetSubdivisionPoint(P))
      continue;
    if (nullptr == v0->m_subd_point1)
    {
      const_cast<MYON_SubDVertex*>(v0)->m_subd_point1 = v = AllocateVertex(v0->m_vertex_tag, level1_index, P);
      AddVertexToLevel(v);
    }
    else
    {
      v = const_cast<MYON_SubDVertex*>(v0->m_subd_point1);
      v->m_vertex_tag = v0->m_vertex_tag;
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }    
  }

  // subdivide edges
  for (const MYON_SubDEdge* e0 = level0.m_edge[0]; nullptr != e0; e0 = e0->m_next_edge)
  {
    if (nullptr == e0->m_subd_point1)
      continue;
    MYON_SubDVertex* end_vertex[2] = { const_cast<MYON_SubDVertex*>(e0->m_vertex[0]->m_subd_point1), const_cast<MYON_SubDVertex*>(e0->m_vertex[1]->m_subd_point1) };
    MYON_SubDVertex* mid_vertex = const_cast<MYON_SubDVertex*>(e0->m_subd_point1);
    double w[2] = { e0->m_sector_coefficient[0], e0->m_sector_coefficient[1] };
    MYON_SubDEdgeTag edge_tag = e0->m_edge_tag;
    if (MYON_SubDEdgeTag::SmoothX == edge_tag && 2 == e0->m_face_count)
    {
      if ( nullptr != mid_vertex && MYON_SubDVertexTag::Smooth == mid_vertex->m_vertex_tag )
        edge_tag = MYON_SubDEdgeTag::Smooth;
    }

    class MYON_SubDEdge* e10 = AddEdge(edge_tag, end_vertex[0], w[0], mid_vertex, 0.0);
    class MYON_SubDEdge* e11 = AddEdge(edge_tag, mid_vertex, 0.0, end_vertex[1], w[1]);
    if ( e0->IsSharp() )
    {
      const MYON_SubDEdgeSharpness e0_sharpness = e0->Sharpness();
      if (e0_sharpness.IsNotZero())
      {
        if (nullptr != e10)
          e10->SetSharpnessForExperts(e0_sharpness.Subdivided(0));
        if (nullptr != e11)
          e11->SetSharpnessForExperts(e0_sharpness.Subdivided(1));
      }
    }
  }

  for (const MYON_SubDFace* f0 = level0.m_face[0]; nullptr != f0; f0 = f0->m_next_face)
  {
    Internal_GlobalQuadSubdivideFace(f0, max_pack_id);
  }

  return level1_index;
}

unsigned int MYON_SubDimple::Internal_GlobalQuadSubdivideFace(
  const MYON_SubDFace* f0,
  unsigned max_pack_id
  )
{
  // This is a private member function.  
  // The caller insures f0 != nullptr.

  const unsigned int f0_edge_count = f0->m_edge_count;
  if (f0_edge_count < 3)
    return 0;

  const int material_channel_index = f0->MaterialChannelIndex();
  const MYON_Color per_face_color = f0->PerFaceColor();
  const unsigned int level_zero_face_id = (0 == f0->SubdivisionLevel()) ? f0->m_id : f0->m_level_zero_face_id;

  // 2022-06-30, Pierre, RH-69170. Try to keep face packs when globally subdividing
  unsigned int pack_id = f0->PackId();
  const bool pack_rect{ f0->PackRectIsSet() };

  // We should never have to make a new pack: if the previous packing was valid, all pack_ids were set.
  // If max_pack_id > 0 and one of the faces has pack_id == 0, the packing was invalid
  const bool make_new_pack = max_pack_id > 0 && pack_id == 0 && !pack_rect && f0_edge_count == 4;
  const bool set_pack_id = max_pack_id > 0 && ((pack_id > 0 && pack_rect) || make_new_pack);
  const bool make_ngons_pack = max_pack_id > 0 && pack_id > 0 && pack_rect && f0_edge_count != 4;
  unsigned int ngons_quadrant_size[4]{};

  MYON_2dPoint pack_rect_origin{ 0., 0. };
  MYON_2dVector pack_rect_size{ .5, .5 };
  unsigned int packing_rotation_degrees{ 0U };
  unsigned int rot_dex{ 0U };
  MYON_2dVector pack_rect_offsets[4]{
    MYON_2dVector{0., 0.}, MYON_2dVector{.5, 0.}, MYON_2dVector{.5, .5}, MYON_2dVector{0., .5}
  };
  if (set_pack_id)
  {
    if (make_new_pack)
    {
      pack_id = max_pack_id + 1U; 
    }
    else
    {
      packing_rotation_degrees = f0->PackRectRotationDegrees();
      rot_dex = (((packing_rotation_degrees / 90) % 4) + 4) % 4;
      pack_rect_size = f0->PackRectSize() / 2;
      pack_rect_origin = f0->PackRectCorner(false, rot_dex);
      pack_rect_offsets[1].x = pack_rect_size.x;
      pack_rect_offsets[2].x = pack_rect_size.x;
      pack_rect_offsets[2].y = pack_rect_size.y;
      pack_rect_offsets[3].y = pack_rect_size.y;
    }
    if (make_ngons_pack)
    {
      // TODO: Use MYON_SubDFace::GetNgonSubPackRectSizeAndDelta() instead?
      pack_id = max_pack_id;  // Will be incremented later
      const unsigned int k = f0_edge_count / 4;
      const unsigned int r = f0_edge_count % 4;
      ngons_quadrant_size[0] = k + (r > 0 ? 1 : 0);
      ngons_quadrant_size[1] = k + (r == 3 ? 1 : 0);
      ngons_quadrant_size[2] = k + (r > 1 ? 1 : 0);
      ngons_quadrant_size[3] = k;
    }
  }

  if (nullptr == f0->m_subd_point1)
  {
    // add face centroid
    double faceC[3];
    if (false == f0->GetSubdivisionPoint( faceC))
      return 0;
    f0->SetSavedSubdivisionPoint(faceC);
    unsigned int level1_index = f0->SubdivisionLevel() + 1;
    MYON_SubDVertex* v = AllocateVertex(MYON_SubDVertexTag::Smooth, level1_index, faceC );
    AddVertexToLevel(v);
    const_cast<MYON_SubDFace*>(f0)->m_subd_point1 = v;
  }


  MYON_SubDEdge* E0[2];
  MYON__UINT_PTR E0dir[2];
  MYON_SubDEdge* E1[4];
  MYON__UINT_PTR E1dir[4];
  MYON_SubDEdgePtr f1edges[4];
  MYON__UINT_PTR e_ptr;
  MYON_SubDEdge* FirstE1(nullptr);
  double w;

  e_ptr = f0->EdgePtr(f0_edge_count - 1).m_ptr;
  E0[1] = MYON_SUBD_EDGE_POINTER(e_ptr);
  E0dir[1] = MYON_SUBD_EDGE_DIRECTION(e_ptr);
  E1[2] = nullptr;

  unsigned int f1_count = 0;

  const double w_2facesector = MYON_SubDSectorType::CreaseSectorCoefficient(2);

  for (unsigned int i = 0; i < f0_edge_count; i++)
  {
    E0[0] = E0[1];
    E0dir[0] = E0dir[1];
    e_ptr = f0->EdgePtr(i).m_ptr;
    E0[1] = MYON_SUBD_EDGE_POINTER(e_ptr);
    E0dir[1] = MYON_SUBD_EDGE_DIRECTION(e_ptr);

    if (nullptr == E0[0] || nullptr == E0[1])
      continue;
    if (nullptr == E0[0]->m_subd_point1 || nullptr == E0[1]->m_subd_point1)
      continue;

    e_ptr = E0[0]->m_subd_point1->m_edges[0 == E0dir[0] ? 1 : 0].m_ptr;
    E1[0] = MYON_SUBD_EDGE_POINTER(e_ptr);
    E1dir[0] = E0dir[0];

    e_ptr = E0[1]->m_subd_point1->m_edges[0 == E0dir[1] ? 0 : 1].m_ptr;
    E1[1] = MYON_SUBD_EDGE_POINTER(e_ptr);
    E1dir[1] = E0dir[1];

    E1[3] = E1[2];
    if (nullptr == E1[3])
    {
      //  The value of E0[0]->m_subd_point1->m_vertex_tag should be either 
      //  MYON_SubDVertexTag::Smooth or MYON_SubDVertexTag::Crease. In the
      //  case when it's value is "crease", the resulting edge end coefficient
      //  will be 0.5 because the edge has two adjacent faces and "theta"
      //  will be pi/2.  
      //  The resulting quad edge coefficient is 0.5 = 1/2 + 1/3*cos(pi/2).
      w = (MYON_SubDVertexTag::Crease == E0[0]->m_subd_point1->m_vertex_tag) ? w_2facesector : 0.0;
      E1[3] = AddEdge(MYON_SubDEdgeTag::Smooth, const_cast<MYON_SubDVertex*>(f0->m_subd_point1), 0.0, const_cast<MYON_SubDVertex*>(E0[0]->m_subd_point1), w);
      if (nullptr == FirstE1)
        FirstE1 = E1[3];
    }
    E1dir[3] = 0;

    if (i + 1 < f0_edge_count || nullptr == FirstE1)
    {
      //  The value of E0[0]->m_subd_point1->m_vertex_tag should be either 
      //  MYON_SubDVertexTag::Smooth or MYON_SubDVertexTag::Crease. In the
      //  case when it's value is "crease", the resulting edge end coefficient
      //  will be zero because the edge has two adjacent faces and "theta"
      //  will be pi/2.  The resulting edge coefficient is 0.5.
      w = (MYON_SubDVertexTag::Crease == E0[1]->m_subd_point1->m_vertex_tag) ? w_2facesector : 0.0;
      E1[2] = AddEdge(MYON_SubDEdgeTag::Smooth, const_cast<MYON_SubDVertex*>(f0->m_subd_point1), 0.0, const_cast<MYON_SubDVertex*>(E0[1]->m_subd_point1), w);
    }
    else
    {
      E1[2] = FirstE1;
    }
    E1dir[2] = 1;

    f1edges[0] = MYON_SubDEdgePtr::Create(E1[0], E1dir[0]);
    f1edges[1] = MYON_SubDEdgePtr::Create(E1[1], E1dir[1]);
    f1edges[2] = MYON_SubDEdgePtr::Create(E1[2], E1dir[2]);
    f1edges[3] = MYON_SubDEdgePtr::Create(E1[3], E1dir[3]);

    MYON_SubDFace* f1 = AddFace(4, f1edges);
    if (nullptr != f1)
    {
      f1->SetMaterialChannelIndex(material_channel_index);
      f1->SetPerFaceColor(per_face_color);
      f1->m_level_zero_face_id = level_zero_face_id;
      f1_count++;

      if (set_pack_id)
      {
        if (make_ngons_pack)
        {
          // TODO: Use MYON_SubDFace::GetNgonSubPackRectSizeAndDelta() instead?
          ++pack_id;
          const unsigned int quadrant = (i * 4) / f0_edge_count;
          const unsigned int prevsizes = (
            quadrant > 0 ? ngons_quadrant_size[0] : 0
            + quadrant > 1 ? ngons_quadrant_size[1] : 0
            + quadrant > 2 ? ngons_quadrant_size[2] : 0);
          const unsigned int subidx = i - prevsizes;
          const MYON_2dVector size{
            quadrant % 2 == 0 ? (pack_rect_size.x / ngons_quadrant_size[quadrant]) : pack_rect_size.x,
            quadrant % 2 == 1 ? (pack_rect_size.y / ngons_quadrant_size[quadrant]) : pack_rect_size.y };
          const MYON_2dVector offset{
            pack_rect_offsets[(quadrant + 4 - rot_dex) % 4]
            + (quadrant % 2 == 0 ? MYON_2dVector{ size.x * subidx, 0. } : MYON_2dVector{ size.y * subidx, 0. }) };
          f1->SetPackRectForExperts(
            pack_rect_origin + offset,
            size, packing_rotation_degrees - quadrant * 90U + 90U);
        }
        else
        {
          f1->SetPackRectForExperts(
            pack_rect_origin + pack_rect_offsets[(i + 4 - rot_dex) % 4],
            pack_rect_size, packing_rotation_degrees - i * 90U + 90U);
        }
        f1->SetPackIdForExperts(pack_id);
        if (pack_id > max_pack_id) max_pack_id = pack_id;
      }
    }
  }

  // return number of new faces
  return f1_count;
}

unsigned int MYON_SubD::GlobalSubdivideQuadCount() const
{
  return GlobalSubdivideQuadCount(1);
}

unsigned int MYON_SubD::GlobalSubdivideQuadCount(unsigned int subdivision_count) const
{
  if (subdivision_count >= 16)
    return MYON_UINT_MAX;
  unsigned int subdivide_quad_count = 0;
  if (subdivision_count >= 1)
  {
    unsigned int level1_quad_count = 0;
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
      level1_quad_count += f->EdgeCount();
    const unsigned pow4 = 1U << (2U*subdivision_count); // pow4 = 4^subdivision_count
    subdivide_quad_count = (subdivide_quad_count > MYON_UINT_MAX / pow4) ? MYON_UINT_MAX  : (level1_quad_count * pow4);
  }
  return subdivide_quad_count;
}

bool MYON_SubD::GlobalSubdivide()
{
  return GlobalSubdivide(1U);
}

bool MYON_SubD::GlobalSubdivide(
  unsigned int count
)
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if (nullptr == subdimple)
    return MYON_SUBD_RETURN_ERROR(false);
  return subdimple->GlobalSubdivide(count);
}

bool MYON_SubD::LocalSubdivide(
  const MYON_SimpleArray<MYON_COMPONENT_INDEX>& face_list
)
{
  const int count = face_list.Count();
  MYON_SimpleArray< const MYON_SubDFace* > ptr_list(count);
  for (int i = 0; i < count; ++i)
  {
    const MYON_COMPONENT_INDEX ci = face_list[i];
    if (MYON_COMPONENT_INDEX::TYPE::subd_face != ci.m_type)
      continue;
    if (ci.m_index <= 0)
      continue;
    const MYON_SubDFace* f = this->FaceFromId(ci.m_index);
    if (nullptr == f)
      continue;
    ptr_list.Append(f);
  }
  const bool rc = LocalSubdivide(ptr_list);
  return rc;
}

bool MYON_SubD::LocalSubdivide(const MYON_SimpleArray< const MYON_SubDFace* >& face_list)
{
  return LocalSubdivide(face_list.Array(), face_list.UnsignedCount());
}

bool MYON_SubD::LocalSubdivide( MYON_SubDFace const*const* face_list, size_t face_count)
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if (nullptr == subdimple)
    return MYON_SUBD_RETURN_ERROR(false);
  return subdimple->LocalSubdivide(face_list, face_count);
}

bool MYON_SubDimple::GlobalSubdivide(
  unsigned int count
  )
{
  if (m_levels.UnsignedCount() < 1)
    return MYON_SUBD_RETURN_ERROR(false);
  if (nullptr == m_active_level)
  {
    m_active_level = m_levels[m_levels.UnsignedCount() - 1];
    if (nullptr == m_active_level)
      return MYON_SUBD_RETURN_ERROR(false);
  }

  const unsigned level0_index = m_active_level->m_level_index;
  if (level0_index >= m_levels.UnsignedCount() || nullptr == m_levels[level0_index])
    return MYON_SUBD_RETURN_ERROR(false);

  if (count <= 0)
    return MYON_SUBD_RETURN_ERROR(false);

  if (level0_index + count > MYON_SubD::maximum_subd_level)
    return MYON_SUBD_RETURN_ERROR(false);

  ClearHigherSubdivisionLevels(level0_index + 1);
  if (level0_index + 1 != m_levels.UnsignedCount() )
    return MYON_SUBD_RETURN_ERROR(false);

  m_active_level = m_levels[level0_index];
 
  for (unsigned int i = level0_index +1; i <= level0_index +count; i++)
  {
    unsigned int rc = GlobalSubdivide();
    if (i != rc)
      return MYON_SUBD_RETURN_ERROR(false);
    m_active_level = m_levels[i];
  }
  return true;
}

MYON_SubDEdgePtr MYON_SubDimple::MergeConsecutiveEdges(
  MYON_SubDEdgePtr eptr0,
  MYON_SubDEdgePtr eptr1
  )
{
  if ( false == MYON_SubD::EdgesAreConsecutive(eptr0,eptr1) )
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);

  MYON_SubDEdge* e[2] = { MYON_SUBD_EDGE_POINTER(eptr0.m_ptr), MYON_SUBD_EDGE_POINTER(eptr1.m_ptr) };
  MYON__UINT_PTR edir[2] = { MYON_SUBD_EDGE_DIRECTION(eptr0.m_ptr), MYON_SUBD_EDGE_DIRECTION(eptr1.m_ptr) };
  const MYON_SubDVertex* end_v[2] = {e[0]->m_vertex[edir[0]], e[1]->m_vertex[1 - edir[1]]};
  if (nullptr == end_v[0] || nullptr == end_v[1] || end_v[0] == end_v[1] )
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);

  MYON_SubDEdgeTag merged_edge_tag 
    = (e[0]->IsSmooth() || e[1]->IsSmooth())
    ? MYON_SubDEdgeTag::Smooth
    : MYON_SubDEdgeTag::Crease;

  for (unsigned int j = 0; j < e[1]->m_face_count; j++)
  {
    MYON_SubDFace* f = const_cast<MYON_SubDFace*>(e[1]->Face(j));
    if (nullptr == f)
      continue;
    f->RemoveEdgeFromArray(e[1]);
  }
  
  // remove middle vertex
  MYON_SubDVertex* middle_v = const_cast< MYON_SubDVertex* >(e[1]->m_vertex[edir[1]]);
  if (nullptr != middle_v && middle_v != end_v[0] && middle_v != end_v[1] )
  {
    if (middle_v->m_edge_count > 0 && nullptr != middle_v->m_edges)
    {
      unsigned int vei0 = middle_v->EdgeArrayIndex(e[0]);
      unsigned int vei1 = middle_v->EdgeArrayIndex(e[1]);      
      unsigned int vei_count = middle_v->m_edge_count;
      middle_v->m_edge_count = 0;
      for (unsigned int vei = 0; vei < vei_count; vei++)
      {
        if ( vei == vei0 || vei == vei1 )
          continue;

        // happens when middle_v is a multiple sector corner, non-manifold, or other rare cases
        if (vei > middle_v->m_edge_count)
          middle_v->m_edges[middle_v->m_edge_count] = middle_v->m_edges[vei];
        middle_v->m_edge_count++;
      }
    }
    if (0 == middle_v->m_edge_count || nullptr == middle_v->m_edges)
    {
      ReturnVertex(middle_v);
      middle_v = nullptr;
    }
  }

  e[0]->m_vertex[1-edir[0]] = nullptr;
  e[1]->m_vertex[edir[1]] = nullptr;
  e[1]->m_vertex[1-edir[1]] = nullptr;

  for (unsigned int i = 0; i < end_v[1]->m_edge_count; i++)
  {
    if (e[1] == MYON_SUBD_EDGE_POINTER(end_v[1]->m_edges[i].m_ptr))
    {
      const_cast< MYON_SubDVertex* >(end_v[1])->m_edges[i] = MYON_SubDEdgePtr::Create(e[0], 1 - edir[0]);
      e[0]->m_vertex[1 - edir[0]] = end_v[1];
      break;
    }
  }

  e[0]->m_sector_coefficient[1 - edir[0]] = e[1]->m_sector_coefficient[1 - edir[1]];

  const bool bTagged[2] = { end_v[0]->IsCreaseOrCorner(), end_v[1]->IsCreaseOrCorner() };
  if (MYON_SubDEdgeTag::Smooth == merged_edge_tag || false == bTagged[0] || false == bTagged[1])
  {
    e[0]->m_edge_tag
      = (bTagged[0] && bTagged[1])
      ? MYON_SubDEdgeTag::SmoothX
      : MYON_SubDEdgeTag::Smooth;
    if ( false == bTagged[0])
      e[0]->m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;
    else if (!(e[0]->m_sector_coefficient[0] > 0.0 && e[0]->m_sector_coefficient[0] < 1.0))
      e[0]->m_sector_coefficient[0] = MYON_SubDSectorType::UnsetSectorCoefficient;
    if ( false == bTagged[1])
      e[0]->m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
    else if (!(e[0]->m_sector_coefficient[1] > 0.0 && e[0]->m_sector_coefficient[1] < 1.0))
      e[0]->m_sector_coefficient[1] = MYON_SubDSectorType::UnsetSectorCoefficient;
  }
  else
  {
    e[0]->m_edge_tag = MYON_SubDEdgeTag::Crease;
    e[0]->m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;
    e[0]->m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
  }

  ReturnEdge(e[1]);

  return eptr0;
}

MYON_SubDEdgePtr MYON_SubD::MergeConsecutiveEdges(
  MYON_SubDEdgePtr eptr0,
  MYON_SubDEdgePtr eptr1
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  return (nullptr != subdimple) ? subdimple->MergeConsecutiveEdges(eptr0, eptr1) : MYON_SubDEdgePtr::Null;
}

static bool Internal_EdgesAreConsecutive(
  const MYON_SubDEdgePtr eptr[2],
  bool bTestColinearity,
  double distance_tolerance,
  double maximum_aspect,
  double sin_angle_tolerance
  )
{
  const MYON_SubDEdge* e[2] = { MYON_SUBD_EDGE_POINTER(eptr[0].m_ptr), MYON_SUBD_EDGE_POINTER(eptr[1].m_ptr) };
  MYON__UINT_PTR edir[2] = { MYON_SUBD_EDGE_DIRECTION(eptr[0].m_ptr), MYON_SUBD_EDGE_DIRECTION(eptr[1].m_ptr) };
  if (
    nullptr == e[0] || nullptr == e[1] 
    || e[0] == e[1]
    || edir[0] > 1 || edir[1] > 1
    || e[0]->m_face_count != e[1]->m_face_count
    //|| e[0]->m_edge_tag != e[1]->m_edge_tag
    )
  {
    return false;
  }

  if ( nullptr == e[1]->m_vertex[0] || nullptr == e[1]->m_vertex[1] )
  {
    return false;
  }

  // v[0] = start
  // v[1] = end
  // v[2] = middle (will be removed)
  const MYON_SubDVertex* v[4] = { e[0]->m_vertex[edir[0]], e[1]->m_vertex[1 - edir[1]], e[0]->m_vertex[1 - edir[0]], e[1]->m_vertex[edir[1]] };

  if (nullptr == v[0] || nullptr == v[1] || nullptr == v[2] || v[0] == v[1] || v[2] != v[3])
  {
    return false;
  }

  if (bTestColinearity)
  {
    if (MYON_UNSET_UINT_INDEX == v[2]->EdgeArrayIndex(e[0]) || MYON_UNSET_UINT_INDEX == v[2]->EdgeArrayIndex(e[1]))
      bTestColinearity = false;
  }

  // edges must have the same faces
  switch (e[0]->m_face_count)
  {
  case 0:
    break;

  case 1:
    if (MYON_SUBD_FACE_POINTER(e[0]->m_face2[0].m_ptr) == MYON_SUBD_FACE_POINTER(e[1]->m_face2[0].m_ptr))
      break;

    return false;

  case 2:
    if (MYON_SUBD_FACE_POINTER(e[0]->m_face2[0].m_ptr) == MYON_SUBD_FACE_POINTER(e[1]->m_face2[0].m_ptr)
      && MYON_SUBD_FACE_POINTER(e[0]->m_face2[1].m_ptr) == MYON_SUBD_FACE_POINTER(e[1]->m_face2[1].m_ptr))
      break;

    if (MYON_SUBD_FACE_POINTER(e[0]->m_face2[0].m_ptr) == MYON_SUBD_FACE_POINTER(e[1]->m_face2[1].m_ptr)
      && MYON_SUBD_FACE_POINTER(e[0]->m_face2[1].m_ptr) == MYON_SUBD_FACE_POINTER(e[1]->m_face2[0].m_ptr))
      break;

    return false;

  default:
    // non-manifold edge
    {
      unsigned int j, k;
      for (j = 0; j < e[0]->m_face_count; j++)
      {
        const MYON_SubDFace* f = e[0]->Face(j);
        for (k = 0; k < e[1]->m_face_count; k++)
        {
          if (f == e[1]->Face(k))
            break;
        }
        if (k < e[1]->m_face_count)
          continue;
        break;
      }
      if (j != e[0]->m_face_count)
        return false;
    }
    break;
  }

  if (bTestColinearity)
  {
    const MYON_3dPoint* P[3] = { (const MYON_3dPoint*)v[0]->m_P, (const MYON_3dPoint*)v[2]->m_P, (const MYON_3dPoint*)v[1]->m_P };

    MYON_3dVector D(*P[2] - *P[0]);
    const double d = D.Length();
    if (!(d > 0.0))
      return false;
    const MYON_3dVector V(*P[1] - *P[0]);
    const double t = (V*D) / (d*d);
    if (!(t > MYON_EPSILON && t < 1.0 - MYON_EPSILON))
      return false;

    const MYON_3dPoint M = (1.0 - t)*(*P[0]) + t*(*P[2]);
    const double h = P[1]->DistanceTo(M);

    if (0.0 == h)
      return true;

    if (!(h > 0.0))
      return false;

    const double miniscule_distance_tolerance = MYON_ZERO_TOLERANCE;
    if (h <= miniscule_distance_tolerance && !(distance_tolerance >= 0.0 && distance_tolerance < miniscule_distance_tolerance))
    {
      // skip parameter tests for miniscule h.
      return true;
    }

    const double miniscule_maximum_aspect = 1e-4;
    if (h <= miniscule_maximum_aspect * d && !(maximum_aspect >= 0.0 && maximum_aspect < miniscule_maximum_aspect))
    {
      // skip parameter tests for miniscule h/d.
      return true;
    }

    if (distance_tolerance >= 0.0 && !(h <= distance_tolerance))
      return false; // failed distance to chord test

    if (maximum_aspect >= 0.0 && !(h <= maximum_aspect * d))
      return false; // failed maximum aspect test

    if (sin_angle_tolerance >= 0.0 && sin_angle_tolerance < 1.0 && !(MYON_CrossProduct(V, (*P[1] - *P[2])).Length() <= sin_angle_tolerance))
      return false; // failed minimum angle test
  }
  return true;
}

bool MYON_SubD::EdgesAreConsecutive(
  MYON_SubDEdgePtr eptr0,
  MYON_SubDEdgePtr eptr1
  )
{
  MYON_SubDEdgePtr eptr[2] = { eptr0,eptr1 };
  return Internal_EdgesAreConsecutive(eptr,false,MYON_DBL_QNAN,MYON_DBL_QNAN,MYON_DBL_QNAN);
}

static bool Internal_EdgesPassTypeFilter(
  const MYON_SubDEdgePtr eptr[2],
  bool bMergeBoundaryEdges,
  bool bMergeInteriorCreaseEdges,
  bool bMergeInteriorSmoothEdges
)
{
  for (unsigned i = 0; i < 2; ++i)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr[i].m_ptr);
    if (nullptr == e || e->m_face_count < 1)
      return false;
    if (1 == e->m_face_count)
    {
      if (false == bMergeBoundaryEdges)
        return false;
    }
    else if (e->m_face_count >= 2)
    {
      if (false == (e->IsSmooth() ? bMergeInteriorSmoothEdges : bMergeInteriorCreaseEdges))
        return false;
    }
    else
      return false;
  };
  return true;
}

unsigned int MYON_SubDimple::MergeColinearEdges(
  bool bMergeBoundaryEdges,
  bool bMergeInteriorCreaseEdges,
  bool bMergeInteriorSmoothEdges,
  double distance_tolerance,
  double maximum_aspect,
  double sin_angle_tolerance
  )
{
  if (1 != m_levels.UnsignedCount())
    return false;

  unsigned int removed_edge_count = 0;

  for (const MYON_SubDFace* f = ActiveLevel().m_face[0]; nullptr != f; f = f->m_next_face)
  {
    unsigned int edge_count = f->m_edge_count;
    if (edge_count < 3)
      continue;
    
    // First - find a pair of edges that should not be merged.
    MYON_SubDEdgePtr eptr[2] = { MYON_SubDEdgePtr::Null, f->EdgePtr(edge_count - 1) };
    unsigned int fei0 = 0;
    while (fei0 < edge_count)
    {
      eptr[0] = eptr[1];
      eptr[1] = f->EdgePtr(fei0);
      if (false == Internal_EdgesAreConsecutive(eptr, true, distance_tolerance, maximum_aspect, sin_angle_tolerance))
        break;
      if (false == Internal_EdgesPassTypeFilter(eptr, bMergeBoundaryEdges, bMergeInteriorCreaseEdges, bMergeInteriorSmoothEdges))
        break;
      ++fei0;
    }
    if (fei0 >= edge_count)
    {
      // face is degenerate or corrupt.
      continue;
    }
    if (0 != fei0)
    {
      if (false == (const_cast<MYON_SubDFace*>(f)->RotateEdgeArray(fei0)))
      {
        // face is degenerate or corrupt.
        continue;
      }
    }

    // At this point, we know the last edge and the first edge should not me merged.
    eptr[1] = f->EdgePtr(0U);
    unsigned int fei = 1;
    while ( fei < edge_count)
    {
      eptr[0] = eptr[1];
      eptr[1] = f->EdgePtr(fei);
      if (
        Internal_EdgesAreConsecutive(eptr, true, distance_tolerance, maximum_aspect, sin_angle_tolerance)
        && Internal_EdgesPassTypeFilter(eptr, bMergeBoundaryEdges, bMergeInteriorCreaseEdges, bMergeInteriorSmoothEdges)
        )
      {
        // merge edges f->Edge(fei-1) and f->Edge(fei) into f->Edge(fei-1).
        if (eptr[0].m_ptr != MergeConsecutiveEdges(eptr[0], eptr[1]).m_ptr)
        {
          MYON_SUBD_ERROR("Bug in consecutive edge merging.");
          break;
        }
        ++removed_edge_count;
        --edge_count;
        eptr[1] = eptr[0];
      }
      else
      {
        ++fei;
      }
    }
  }

  return removed_edge_count;
}

unsigned int MYON_SubD::MergeColinearEdges(
  bool bMergeBoundaryEdges,
  bool bMergeInteriorCreaseEdges,
  bool bMergeInteriorSmoothEdges,
  double distance_tolerance,
  double maximum_aspect,
  double sin_angle_tolerance
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  return 
    (nullptr != subdimple) 
    ? subdimple->MergeColinearEdges( bMergeBoundaryEdges, bMergeInteriorCreaseEdges, bMergeInteriorSmoothEdges, distance_tolerance, maximum_aspect, sin_angle_tolerance) 
    : 0;
}

unsigned int MYON_SubD::LevelCount() const
{
  const MYON_SubDimple* subdimple = SubDimple();
  return (0 != subdimple ? subdimple->LevelCount() : 0);
}


unsigned int MYON_SubD::ActiveLevelIndex() const
{
  return ActiveLevel().m_level_index;
}

bool MYON_SubD::IsEmpty() const
{
  return (nullptr == SubDimple());
}

bool MYON_SubD::IsNotEmpty() const
{
  return (nullptr != SubDimple());
}

/////////////////////////////////////////////////////////
//
// Element (Vertex, Edge, Face) access
//
const MYON_COMPONENT_INDEX MYON_SubDComponentPtr::ComponentIndex() const
{
  switch (ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = MYON_SUBD_VERTEX_POINTER(m_ptr);
      if ( nullptr != vertex)
        return MYON_COMPONENT_INDEX(MYON_COMPONENT_INDEX::TYPE::subd_vertex,vertex->m_id);
    }
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_ptr);
      if ( nullptr != edge)
        return MYON_COMPONENT_INDEX(MYON_COMPONENT_INDEX::TYPE::subd_edge,edge->m_id);
    }
    break;
  case MYON_SubDComponentPtr::Type::Face:
    {
      const MYON_SubDFace* face = MYON_SUBD_FACE_POINTER(m_ptr);
      if ( nullptr != face)
        return MYON_COMPONENT_INDEX(MYON_COMPONENT_INDEX::TYPE::subd_face,face->m_id);
    }
    break;
  default:
    if (IsNull() )
      return MYON_COMPONENT_INDEX::UnsetComponentIndex;
    break;
  }
  return MYON_SUBD_RETURN_ERROR(MYON_COMPONENT_INDEX::UnsetComponentIndex);
}

const MYON_SubDComponentPtr MYON_SubD::ComponentPtrFromComponentIndex(
  MYON_COMPONENT_INDEX component_index
  ) const
{
  if (0 != component_index.m_index && -1 != component_index.m_index)
  {
    switch (component_index.m_type)
    {
    case MYON_COMPONENT_INDEX::TYPE::subd_vertex:
      return MYON_SubDComponentPtr::Create(VertexFromId(component_index.m_index));
    case MYON_COMPONENT_INDEX::TYPE::subd_edge:
      return MYON_SubDComponentPtr::Create(EdgeFromId(component_index.m_index));
    case MYON_COMPONENT_INDEX::TYPE::subd_face:
      return MYON_SubDComponentPtr::Create(FaceFromId(component_index.m_index));
    default:
      break;
    }
  }
  else if (MYON_COMPONENT_INDEX::TYPE::invalid_type == component_index.m_type)
  {
    return MYON_SubDComponentPtr::Null;
  }

  return MYON_SUBD_RETURN_ERROR(MYON_SubDComponentPtr::Null);
}

/////////////////////////////////////////////////////////
//
// Vertex access
//

unsigned int MYON_SubD::VertexCount() const
{
  return ActiveLevel().m_vertex_count;
}

const MYON_SubDVertex* MYON_SubD::FirstVertex() const
{
  return ActiveLevel().m_vertex[0];
}

const MYON_SubDVertex* MYON_SubD::LastVertex() const
{
  return ActiveLevel().m_vertex[1];
}

MYON_SubDVertexIterator MYON_SubD::VertexIterator() const
{
  return MYON_SubDVertexIterator(*this);
}

MYON_SubDVertexIterator MYON_SubDRef::VertexIterator() const
{
  return MYON_SubDVertexIterator(*this);
}

MYON_SubDVertexArray MYON_SubD::VertexArray() const
{
  return MYON_SubDVertexArray(*this);
}

const class MYON_SubDVertex* MYON_SubD::VertexFromId(
  unsigned int vertex_id
  ) const
{
  for (;;)
  {
    if (0 == vertex_id || MYON_UNSET_UINT_INDEX == vertex_id )
      break;
    const MYON_SubDimple* subdimple = SubDimple();
    if (nullptr == subdimple)
      break;
    return subdimple->VertexFromId(vertex_id);
  }
  return nullptr;
}

const class MYON_SubDVertex* MYON_SubD::VertexFromComponentIndex(
  MYON_COMPONENT_INDEX component_index
) const
{
  return (MYON_COMPONENT_INDEX::TYPE::subd_vertex == component_index.m_type) ? VertexFromId(component_index.m_index) : nullptr;
}

const MYON_COMPONENT_INDEX MYON_SubDVertex::ComponentIndex() const
{
  return MYON_COMPONENT_INDEX(MYON_COMPONENT_INDEX::TYPE::subd_vertex,m_id);
}

const MYON_SubDComponentPtr MYON_SubDVertex::ComponentPtr() const
{
  return MYON_SubDComponentPtr::Create(this);
}

/////////////////////////////////////////////////////////
//
// Edge access
//

unsigned int MYON_SubD::EdgeCount() const
{
  return ActiveLevel().m_edge_count;
}

const MYON_SubDEdge* MYON_SubD::FirstEdge() const
{
  return ActiveLevel().m_edge[0];
}

const MYON_SubDEdge* MYON_SubD::LastEdge() const
{
  return ActiveLevel().m_edge[1];
}

MYON_SubDEdgeIterator MYON_SubD::EdgeIterator() const
{
  return MYON_SubDEdgeIterator(*this);
}

MYON_SubDEdgeIterator MYON_SubDRef::EdgeIterator() const
{
  return MYON_SubDEdgeIterator(*this);
}

MYON_SubDEdgeArray MYON_SubD::EdgeArray() const
{
  return MYON_SubDEdgeArray(*this);
}

const class MYON_SubDEdge* MYON_SubD::EdgeFromId(
  unsigned int edge_id
  ) const
{
  for (;;)
  {
    if (0 == edge_id || MYON_UNSET_UINT_INDEX == edge_id )
      break;
    const MYON_SubDimple* subdimple = SubDimple();
    if (nullptr == subdimple)
      break;
    return subdimple->EdgeFromId(edge_id);
  }
  return nullptr;
}

const class MYON_SubDEdge* MYON_SubD::EdgeFromComponentIndex(
  MYON_COMPONENT_INDEX component_index
) const
{
  return (MYON_COMPONENT_INDEX::TYPE::subd_edge == component_index.m_type) ? EdgeFromId(component_index.m_index) : nullptr;
}

const MYON_COMPONENT_INDEX MYON_SubDEdge::ComponentIndex() const
{
  return MYON_COMPONENT_INDEX(MYON_COMPONENT_INDEX::TYPE::subd_edge,m_id);
}

const MYON_SubDComponentPtr MYON_SubDEdge::ComponentPtr() const
{
  return MYON_SubDComponentPtr::Create(this);
}

/////////////////////////////////////////////////////////
//
// Face access
//

unsigned int MYON_SubD::FaceCount() const
{
  return ActiveLevel().m_face_count;
}

const MYON_SubDFace* MYON_SubD::FirstFace() const
{
  return ActiveLevel().m_face[0];
}

const MYON_SubDFace* MYON_SubD::LastFace() const
{
  return ActiveLevel().m_face[1];
}

MYON_SubDFaceIterator MYON_SubD::FaceIterator() const
{
  return MYON_SubDFaceIterator(*this);
}

MYON_SubDFaceIterator MYON_SubDRef::FaceIterator() const
{
  return MYON_SubDFaceIterator(*this);
}


MYON_SubDFaceArray MYON_SubD::FaceArray() const
{
  return MYON_SubDFaceArray(*this);
}

const class MYON_SubDFace* MYON_SubD::FaceFromId(
  unsigned int face_id
  ) const
{
  for (;;)
  {
    if (0 == face_id || MYON_UNSET_UINT_INDEX == face_id )
      break;
    const MYON_SubDimple* subdimple = SubDimple();
    if (nullptr == subdimple)
      break;
    return subdimple->FaceFromId(face_id);
  }
  return nullptr;
}

const class MYON_SubDFace* MYON_SubD::FaceFromComponentIndex(
  MYON_COMPONENT_INDEX component_index
) const
{
  return (MYON_COMPONENT_INDEX::TYPE::subd_face == component_index.m_type) ? FaceFromId(component_index.m_index) : nullptr;
}

const MYON_COMPONENT_INDEX MYON_SubDFace::ComponentIndex() const
{
  return MYON_COMPONENT_INDEX(MYON_COMPONENT_INDEX::TYPE::subd_face,m_id);
}

const MYON_SubDComponentPtr MYON_SubDFace::ComponentPtr() const
{
  return MYON_SubDComponentPtr::Create(this);
}

/////////////////////////////////////////////////////////
//
// MYON_SubD properties
//

bool MYON_SubD::IsOriented() const
{
  for (const MYON_SubDEdge* edge = FirstEdge(); nullptr != edge; edge = edge->m_next_edge)
  {
    if ( 2 != edge->m_face_count )
      continue;
    if (nullptr == MYON_SUBD_FACE_POINTER(edge->m_face2[0].m_ptr) || nullptr == MYON_SUBD_FACE_POINTER(edge->m_face2[1].m_ptr) )
      continue;
    if ( MYON_SUBD_FACE_DIRECTION(edge->m_face2[0].m_ptr) == MYON_SUBD_FACE_DIRECTION(edge->m_face2[1].m_ptr) )
      return false;
  }
  return true;
}

// reverses the orientation of all facets
bool MYON_SubD::ReverseOrientation() const
{

  // Limit point normals and limit surface mesh fragments will need to be recalculated.
  // DestroyRuntimeCache() will clear all this information.
  const_cast<MYON_SubD*>(this)->DestroyRuntimeCache(true);


  for (const MYON_SubDFace* face = FirstFace(); nullptr != face; face = face->m_next_face)
  {
    const_cast<MYON_SubDFace*>(face)->ReverseEdgeList();
  }
  return true;
}

// Attempts to orient all facet to match the first facet.

static unsigned int OrientFaceNeighbors(
  unsigned int recursion_level,
  const MYON_SubDFace** face_list,
  unsigned int id0,
  const MYON_SubDFace* face
  )
{
  MYON_SubDFace* next_set[4];
  const unsigned int next_set_capacity = (unsigned int)(sizeof(next_set) / sizeof(next_set[0]));
  unsigned int next_set_count = 0;

  const unsigned int edge_count = face->m_edge_count;
  const MYON_SubDEdgePtr* face_eptr = face->m_edge4;
  const MYON_SubDEdge* e;
  MYON_SubDFace* neighbor_face;

  if (nullptr != face_list[face->m_id - id0])
  {
    // search for an oriented neighbor
    neighbor_face = nullptr;
    for (unsigned int fei = 0; fei < edge_count; fei++, face_eptr++)
    {
      if (4 == fei)
      {
        face_eptr = face->m_edgex;
        if (nullptr == face_eptr)
          break;
      }
      e = MYON_SUBD_EDGE_POINTER(face_eptr->m_ptr);
      if (nullptr == e || 2 != e->m_face_count)
        continue;
      neighbor_face = MYON_SUBD_FACE_POINTER(e->m_face2[0].m_ptr);
      if (face == neighbor_face)
        neighbor_face = MYON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr);
      else if (face != MYON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr))
        continue;
      if (nullptr == neighbor_face)
        continue;

      if (nullptr == face_list[neighbor_face->m_id - id0])
        return OrientFaceNeighbors(recursion_level,face_list,id0,neighbor_face);
    }

    // nothing near face is oriented. 
    return 0;
  }

  unsigned int orient_count = 0;
  for (unsigned int fei = 0; fei < edge_count; fei++, face_eptr++)
  {
    if (4 == fei)
    {
      face_eptr = face->m_edgex;
      if ( nullptr == face_eptr)
        break;
    }
    e = MYON_SUBD_EDGE_POINTER(face_eptr->m_ptr);
    if (nullptr == e || 2 != e->m_face_count)
      continue;
    neighbor_face = MYON_SUBD_FACE_POINTER(e->m_face2[0].m_ptr);
    if (face == neighbor_face)
      neighbor_face = MYON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr);
    else if (face != MYON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr))
      continue;
    if (nullptr == neighbor_face)
      continue;

    if (nullptr == face_list[neighbor_face->m_id - id0])
      continue;

    if (MYON_SUBD_FACE_DIRECTION(e->m_face2[0].m_ptr) == MYON_SUBD_FACE_DIRECTION(e->m_face2[1].m_ptr))
      neighbor_face->ReverseEdgeList();
    face_list[neighbor_face->m_id - id0] = nullptr;
    orient_count++;

    if (recursion_level < 12)
    {
      if (next_set_count >= next_set_capacity)
      {
        for (unsigned i = 0; i < next_set_capacity; i++)
          orient_count += OrientFaceNeighbors(recursion_level + 1, face_list, id0, next_set[i]);
        next_set_count = 0;
      }
      next_set[next_set_count++] = neighbor_face;
    }
  }

  for ( unsigned i = 0; i < next_set_count; i++)
    orient_count += OrientFaceNeighbors(recursion_level+1,face_list,id0,next_set[i]);

  return orient_count;
}

bool MYON_SubD::Orient() const
{
  const MYON_SubDFace* first_face = FirstFace();
  if ( nullptr == first_face || nullptr == first_face->m_next_face)
    return true;

  unsigned int nonzero_face_count = 0;
  MYON_SimpleArray< const MYON_SubDFace* > faces_array(FaceCount());
  unsigned int face_id0 = first_face->m_id;
  unsigned int face_id1 = first_face->m_id;
  for (const MYON_SubDFace* face = FirstFace(); nullptr != face; face = face->m_next_face)
  {
    faces_array.Append(face);
    if (face->m_id > face_id1)
      face_id1 = face->m_id;
    else if (face->m_id < face_id0)
      face_id0 = face->m_id;
    nonzero_face_count++;
  }

  const unsigned int face_count = faces_array.UnsignedCount();
  if (face_count <= 1)
    return true;

  const MYON_SubDFace** faces = faces_array.Array();
  if (face_id1 - face_id0 >= faces_array.UnsignedCount())
  {
    faces_array.Reserve(face_id1 - face_id0 + 1);
    faces_array.SetCount(face_id1 - face_id0 + 1);
    faces_array.Zero();
    // Update faces pointer after reallocating faces_array
    faces = faces_array.Array();
    for (const MYON_SubDFace* face = FirstFace(); nullptr != face; face = face->m_next_face)
    {
      faces[face->m_id-face_id0] = face;
    }
  }

  unsigned int orient_count = 0;
  unsigned int connected_region_count = 0;
  bool bSearchForNewComponent = true;
  unsigned int first_face_index = 0;

  for (;;)
  {
    unsigned int orient_count0 = orient_count;
    while (first_face_index < face_count && nullptr == faces[first_face_index])
      first_face_index++;

    if ( first_face_index >= face_count)
      break;

    for (unsigned int i = first_face_index; i < face_count && orient_count < nonzero_face_count; i++)
    {
      const MYON_SubDFace* face = faces[i];
      if (nullptr == face)
        continue;
      if (bSearchForNewComponent)
      {
        // first face in new connected component
        orient_count++;
        connected_region_count++;
        faces[i] = nullptr;
        bSearchForNewComponent = false;
        first_face_index = i+1;
      }
      orient_count += OrientFaceNeighbors(0, faces, face_id0, face);
    }
    
    if ( orient_count >= nonzero_face_count)
      break;

    if (orient_count0 >= orient_count)
    {
      if (bSearchForNewComponent)
        break;
      bSearchForNewComponent = true;
    }
  }

  return (connected_region_count > 0 && orient_count > 0);
}

const MYON_SubDVertex * MYON_SubD::ReplaceFaceWithTriangleFan(MYON_SubDFace * face, MYON_3dPoint fan_center_point, bool bMarkFaces)
{
  const unsigned edge_count = face->m_edge_count;
  if (edge_count < 3)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDimple* subdimple = SubDimple(false);
  if (nullptr == subdimple)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  // validate and get centroid (which may not be needed).
  MYON_3dPoint P = MYON_3dPoint::Origin;
  MYON_SimpleArray<MYON_SubDEdgePtr> edges(edge_count);
  const MYON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    if (4U == i)
    {
      eptr = face->m_edgex;
      if (nullptr == eptr)
        return MYON_SUBD_RETURN_ERROR(nullptr);
    }
    const MYON_SubDVertex* v = eptr->RelativeVertex(0);
    if (nullptr == v)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    P += v->ControlNetPoint();
    edges.Append(*eptr);
  }

  if (fan_center_point.IsValid())
    P = fan_center_point;
  else
    P /= ((double)edge_count);

  MYON_SubDVertex* v0 = AddVertex(MYON_SubDVertexTag::Smooth, P);
  if (nullptr == v0)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    if (nullptr == AddEdge(MYON_SubDEdgeTag::Smooth, v0, const_cast<MYON_SubDVertex*>(edges[i].RelativeVertex(0))))
    {
      MYON_SubDComponentPtr cptr = MYON_SubDComponentPtr::Create(v0);
      DeleteComponents(&cptr, 1, false);
      return MYON_SUBD_RETURN_ERROR(nullptr);
    }
  }

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    if (i < 4)
      face->m_edge4[i] = MYON_SubDEdgePtr::Null;
    else
      face->m_edgex[i - 4] = MYON_SubDEdgePtr::Null;
    MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(edges[i].m_ptr);
    e->RemoveFaceFromArray(face);
    const_cast<MYON_SubDVertex*>(edges[i].RelativeVertex(0))->RemoveFaceFromArray(face);
  }

  subdimple->ReturnFace(face);
  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    AddTriangleFace(v0->m_edges[i], edges[i], v0->m_edges[(i + 1) % edge_count].Reversed());
  }

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
  {
    MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(edges[i].m_ptr);
    e->EdgeModifiedNofification();
    e->UpdateEdgeSectorCoefficientsForExperts(false);
  }

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
    MYON_SUBD_EDGE_POINTER(edges[i].m_ptr)->UpdateEdgeSectorCoefficientsForExperts(false);

  for (unsigned i = 0; i < edge_count; ++i, ++eptr)
    MYON_SUBD_EDGE_POINTER(v0->m_edges[i].m_ptr)->UpdateEdgeSectorCoefficientsForExperts(false);

  return v0;
}



const MYON_SubDEdge* MYON_SubDimple::SplitEdge(
  MYON_SubDEdge* edge,
  MYON_3dPoint vertex_location
  )
{
  if ( nullptr == edge )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge->m_vertex[0] || nullptr == edge->m_vertex[1] )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge->m_vertex[0]->m_edges || edge->m_vertex[0]->m_edge_count <= 0 || edge->m_vertex[0]->m_edge_capacity < edge->m_vertex[0]->m_edge_count )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (nullptr == edge->m_vertex[1]->m_edges || edge->m_vertex[1]->m_edge_count <= 0 || edge->m_vertex[1]->m_edge_capacity < edge->m_vertex[1]->m_edge_count )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if ( vertex_location.IsUnsetOrNan() )
  {
    MYON_Line L;
    L.from = MYON_3dPoint(edge->m_vertex[0]->m_P);
    L.to = MYON_3dPoint(edge->m_vertex[1]->m_P);
    vertex_location = L.PointAt(0.5);
  }
  if ( false == vertex_location.IsValid() )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if ( vertex_location == MYON_3dPoint(edge->m_vertex[0]->m_P) || vertex_location == MYON_3dPoint(edge->m_vertex[1]->m_P) )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDEdgeTag edge_tag = edge->m_edge_tag;
  MYON_SubDVertexTag vertex_tag;
  switch (edge->m_edge_tag)
  {
  case MYON_SubDEdgeTag::Smooth:
    vertex_tag = MYON_SubDVertexTag::Smooth;
    break;
  case MYON_SubDEdgeTag::Crease:
    vertex_tag = MYON_SubDVertexTag::Crease;
    break;
  case MYON_SubDEdgeTag::SmoothX:
    vertex_tag = MYON_SubDVertexTag::Smooth;
    edge_tag = MYON_SubDEdgeTag::Smooth;
    break;
  default:
    return MYON_SUBD_RETURN_ERROR(nullptr);
    break;
  }

  MYON_SubDVertex* end_vertex[2] = { const_cast<MYON_SubDVertex*>(edge->m_vertex[0]), const_cast<MYON_SubDVertex*>(edge->m_vertex[1]) };

  MYON_SubDVertex* new_vertex = nullptr;
  MYON_SubDEdge* new_edge = nullptr;  
  for (;;)
  {
    new_vertex = AllocateVertex( 0U, vertex_tag, edge->SubdivisionLevel(), static_cast<const double*>(vertex_location), 2, edge->m_face_count);
    if (nullptr == new_vertex)
     break;

    new_edge = AllocateEdge( 0U, edge_tag, edge->SubdivisionLevel(), edge->m_face_count);
    if (nullptr == new_edge)
      break;

    // change end_vertex[1] edge reference from edge to new_edge
    bool bConnectedNewEdgeToEndVertex = false;
    const MYON_SubDEdgePtr old_edge_reference = MYON_SubDEdgePtr::Create(edge,1);
    for (unsigned short vei = 0; vei < end_vertex[1]->m_edge_count; vei++)
    {
      if (old_edge_reference.m_ptr == end_vertex[1]->m_edges[vei].m_ptr)
      {
        // change end_vertex[1]->m_edges[vei] reference
        // from "edge" to "new_edge".
        bConnectedNewEdgeToEndVertex = true;
        end_vertex[1]->m_edges[vei] = MYON_SubDEdgePtr::Create(new_edge,1);
        break;
      }
    }

    if (false == bConnectedNewEdgeToEndVertex)
    {
      // end_vertex[1]->m_edges[] does not reference edge
      break;
    }

    // finish setting new_vertex and end_vertex[] <-> new_edge connections
    new_edge->m_vertex[0] = new_vertex;
    new_edge->m_vertex[1] = end_vertex[1];
    new_vertex->m_edges[new_vertex->m_edge_count++] = MYON_SubDEdgePtr::Create(new_edge,0);

    // finish setting new_vertex <-> input edge and connections
    edge->m_edge_tag = edge_tag; // changes "X" to "Smooth" if required
    edge->m_vertex[1] = new_vertex;
    new_vertex->m_edges[new_vertex->m_edge_count++] = MYON_SubDEdgePtr::Create(edge,1);
       
    // add new_vertex and new_edge <-> edge->m_face[] connections.
    const MYON_SubDFacePtr* fptr0 = edge->m_face2;
    MYON_SubDFacePtr* fptr1 = new_edge->m_face2;
    for (unsigned short efi = 0; efi < edge->m_face_count; efi++)
    {
      if (2 == efi)
      {
        fptr0 = edge->m_facex;
        fptr1 = new_edge->m_facex;
      }

      MYON_SubDFace* face = MYON_SUBD_FACE_POINTER(fptr0->m_ptr);

      if (nullptr != face)
      {
        face->FaceModifiedNofification();

        // add new_vertex reference to face
        new_vertex->m_faces[new_vertex->m_face_count++] = face;

        // insert new_edge into face->m_edge[] list after "edge"
        if (GrowFaceEdgeArray(face, face->m_edge_count + 1))
        {
          if ( face->m_edge_count < 4 )
            face->m_edge4[face->m_edge_count] = MYON_SubDEdgePtr::Null;
          else
            face->m_edgex[face->m_edge_count-4] = MYON_SubDEdgePtr::Null;
          face->m_edge_count++;
          MYON_SubDEdgePtr* face_edge = face->m_edge4;
          for (unsigned short fei = 0; fei < face->m_edge_count; fei++)
          {
            if (4 == fei)
              face_edge = face->m_edgex;
            if (edge == MYON_SUBD_EDGE_POINTER(face_edge->m_ptr))
            {
              MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(face_edge->m_ptr);
              MYON_SubDEdgePtr eptr = MYON_SubDEdgePtr::Create(new_edge,edir);
              if (0 == edir)
              {
                fei++;
                face_edge++;
              }
              for (/*empty init*/; fei < face->m_edge_count; fei++)
              {
                if (4 == fei)
                  face_edge = face->m_edgex;
                const MYON_SubDEdgePtr tmp = *face_edge;
                *face_edge = eptr;
                eptr = tmp;
                face_edge++;
              }
              break;
            }
            face_edge++;
          }
        }
      }

      *fptr1++ = *fptr0++;
      new_edge->m_face_count++;
    }

    // original ending vertex
    new_edge->m_sector_coefficient[1] = edge->m_sector_coefficient[1];

    // Either edge was a crease, new_edge is a crease, and sector coefficients do not apply
    // or edge was X or Smooth, edge is smooth, new_edge is smooth, new_vertex is smooth,
    // and the sector coefficients at this vertex do not apply.
    edge->m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
    new_edge->m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;

    AddVertexToLevel(new_vertex);
    AddEdgeToLevel(new_edge);

    end_vertex[0]->VertexModifiedNofification();
    end_vertex[1]->VertexModifiedNofification();

    // TODO
    //   Delete any levels greater than this level.
    return new_edge;
  }

  if ( nullptr != new_vertex)
    ReturnVertex(new_vertex);
  if ( nullptr != new_edge)
    ReturnEdge(new_edge);
  return MYON_SUBD_RETURN_ERROR(nullptr);
}

const MYON_SubDEdge* MYON_SubDimple::SplitFace(
  MYON_SubDFace* face,
  unsigned int fvi0,
  unsigned int fvi1
  )
{
  if ( nullptr == face || fvi0 == fvi1)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int edge_count = face->m_edge_count;
  if (edge_count < 4 || (edge_count > 4 && nullptr == face->m_edgex))
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (fvi0 >= edge_count || fvi1 >= edge_count)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if ((fvi0 + 1) % edge_count == fvi1 || (fvi1 + 1) % edge_count == fvi0)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (face->SubdivisionLevel() >= m_levels.UnsignedCount())
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON_SubDLevel* level = m_levels[face->SubdivisionLevel()];
  if ( nullptr == level)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDVertex* v[2] = { const_cast<MYON_SubDVertex*>(face->Vertex(fvi0)), const_cast<MYON_SubDVertex*>(face->Vertex(fvi1)) };

  if (nullptr == v[0] || v[0]->m_face_count <= 0 || nullptr == v[0]->m_faces || v[0]->m_edge_count < 2 || nullptr == v[0]->m_edges )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (nullptr == v[1] || v[1]->m_face_count <= 0 || nullptr == v[1]->m_faces || v[1]->m_edge_count < 2 || nullptr == v[1]->m_edges)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  if (v[0] == v[1])
    return MYON_SUBD_RETURN_ERROR(nullptr);

  unsigned new_edge_count[2];
  if (fvi0 < fvi1)
  {
    new_edge_count[1] = fvi1 - fvi0 + 1;
    new_edge_count[0] = (edge_count + 2) - new_edge_count[1];
  }
  else
  {
    new_edge_count[0] = fvi0 - fvi1 + 1;
    new_edge_count[1] = (edge_count + 2) - new_edge_count[0];
  }
  // make sure each side is at least a triangle and no overflows occured
  if (new_edge_count[0] < 3 || new_edge_count[0] >= edge_count)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (new_edge_count[1] < 3 || new_edge_count[1] >= edge_count)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (new_edge_count[0] + new_edge_count[1] != edge_count+2 )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  
  // make sure face topology is valid
  MYON_SimpleArray< MYON_SubDEdgePtr > edges(edge_count);
  MYON_SubDEdgePtr* eptr = face->m_edge4;
  edges.SetCount(edge_count);
  for (unsigned int fei = 0; fei < edge_count; ++fei, ++eptr)
  {
    if (4 == fei)
      eptr = face->m_edgex;
    const MYON_SubDEdge* face_e = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
    if (nullptr == face_e)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    if (MYON_UNSET_UINT_INDEX == face_e->FaceArrayIndex(face) )
      return MYON_SUBD_RETURN_ERROR(nullptr);
    const MYON_SubDVertex* face_v = face_e->m_vertex[MYON_SUBD_EDGE_DIRECTION(eptr->m_ptr)];
    if ( nullptr == face_v)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    if (MYON_UNSET_UINT_INDEX == face_v->FaceArrayIndex(face) )
      return MYON_SUBD_RETURN_ERROR(nullptr);
    edges[fei] = *eptr;
  }

  // create diagonal edge
  MYON_SubDEdge* new_e = nullptr;
  MYON_SubDFace* new_f = nullptr;
  for (;;)
  {
    if (false == m_heap.GrowVertexFaceArrayByOne(v[0]))
      break;
    if (false == m_heap.GrowVertexFaceArrayByOne(v[1]))
      break;

    new_f = AllocateFace();
    if (nullptr == new_f)
      break;
    new_f->SetSubdivisionLevel( face->SubdivisionLevel() );
    AddFaceToLevel(new_f);
    if (new_edge_count[1] > 4)
    {
      if (false == m_heap.GrowFaceEdgeArray(new_f, new_edge_count[1]))
        break;
    }

    new_e = AddEdge(
      ((v[0]->IsSmooth() || v[1]->IsSmooth()) ? MYON_SubDEdgeTag::Smooth : MYON_SubDEdgeTag::SmoothX),
      v[0], MYON_SubDSectorType::UnsetSectorCoefficient, 
      v[1], MYON_SubDSectorType::UnsetSectorCoefficient);
    if (nullptr == new_e)
      break;

    face->FaceModifiedNofification();

    v[0]->m_faces[v[0]->m_face_count++] = new_f;
    v[1]->m_faces[v[1]->m_face_count++] = new_f;

    new_e->m_face2[0] = MYON_SubDFacePtr::Create(face, 0);
    new_e->m_face2[1] = MYON_SubDFacePtr::Create(new_f, 1);
    new_e->m_face_count = 2;
    const MYON_SubDEdgePtr new_eptr = MYON_SubDEdgePtr::Create(new_e);

    eptr = face->m_edge4;
    for (unsigned int fei = 0; fei < edge_count; ++fei, ++eptr)
    {
      if (4 == fei)
        eptr = face->m_edgex;
      *eptr = MYON_SubDEdgePtr::Null;
    }
    face->m_edge_count = 0;
    if (new_edge_count[0] <= 4 && nullptr != face->m_edgex)
      m_heap.ReturnFaceExtraArray(face);

    // update old face
    face->m_edge4[0] = new_eptr;
    eptr = &(face->m_edge4[1]);
    for (unsigned fei = 1; fei < new_edge_count[0]; ++fei, ++eptr)
    {
      if (4 == fei)
        eptr = face->m_edgex;
      *eptr = edges[(fvi1 + fei - 1) % edge_count];
    }
    face->m_edge_count = (unsigned short)new_edge_count[0];

    // initialize new_f
    new_f->m_edge4[0] = new_eptr.Reversed();
    eptr = &(new_f->m_edge4[1]);
    for (unsigned fei = 1; fei < new_edge_count[1]; ++fei, ++eptr)
    {
      if (4 == fei)
        eptr = new_f->m_edgex;
      *eptr = edges[(fvi0 + fei - 1) % edge_count];

      // change edge's face reference from old face to new_f.
      MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
      e->ReplaceFaceInArray(face, new_f);

      MYON_SubDVertex* vtx = const_cast<MYON_SubDVertex*>(eptr->RelativeVertex(0));
      if (nullptr != vtx && v[0] != vtx && v[1] != vtx)
        vtx->ReplaceFaceInArray(face, new_f);
    }
    new_f->m_edge_count = (unsigned short)new_edge_count[1];
    
    // update sector coefficients because they depend on the number of edges
    for (unsigned int vi = 0; vi < 2; vi++)
    {
      for (unsigned short evi = 0; evi < v[vi]->m_edge_count; ++evi)
      {
        MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(v[vi]->m_edges[evi].m_ptr);
        if (nullptr != edge)
          edge->UpdateEdgeSectorCoefficientsForExperts(true);
      }
    }

    // Debugging code
    const MYON_SubDVertex* v0[6] = {
      face->m_edge4[0].RelativeVertex(0),face->m_edge4[0].RelativeVertex(1),
      face->m_edge4[1].RelativeVertex(0),face->m_edge4[1].RelativeVertex(1),
      face->EdgePtr(face->m_edge_count-1).RelativeVertex(0),face->EdgePtr(face->m_edge_count - 1).RelativeVertex(1),
    };
    const MYON_SubDEdge* e0[3] = {
      face->m_edge4[0].Edge(),
      face->m_edge4[1].Edge(),
      face->EdgePtr(face->m_edge_count - 1).Edge()
    };
    const MYON_SubDVertex* v1[6] = {
      new_f->m_edge4[0].RelativeVertex(0),new_f->m_edge4[0].RelativeVertex(1),
      new_f->m_edge4[1].RelativeVertex(0),new_f->m_edge4[1].RelativeVertex(1),
      new_f->EdgePtr(new_f->m_edge_count - 1).RelativeVertex(0),new_f->EdgePtr(new_f->m_edge_count - 1).RelativeVertex(1),
    };
    const MYON_SubDEdge* e1[3] = {
      new_f->m_edge4[0].Edge(),
      new_f->m_edge4[1].Edge(),
      new_f->EdgePtr(new_f->m_edge_count - 1).Edge()
    };
    if (nullptr == v0[5] && nullptr == v1[5] && nullptr == e0[2] && nullptr == e1[2])
      return nullptr;



    return new_e;
  }

  if ( nullptr != new_f  )
   ReturnFace(new_f);

  if (nullptr != new_e)
  {
    v[0]->m_edge_count--;
    v[0]->m_edges[v[0]->m_edge_count] = MYON_SubDEdgePtr::Null;
    v[1]->m_edge_count--;
    v[1]->m_edges[v[1]->m_edge_count] = MYON_SubDEdgePtr::Null;
    new_e->m_vertex[0] = nullptr;
    new_e->m_vertex[1] = nullptr;
    new_e->m_face_count = 0;
    ReturnEdge(new_e);
  }

  return MYON_SUBD_RETURN_ERROR(nullptr);
}



const MYON_SubDEdge* MYON_SubD::SplitEdge(
  MYON_SubDEdge* edge,
  MYON_3dPoint vertex_location
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  return subdimple->SplitEdge(edge,vertex_location);
}

const MYON_SubDEdgePtr MYON_SubD::SplitEdge(
  MYON_SubDEdgePtr eptr,
  MYON_3dPoint vertex_location,
  unsigned new_edge_end
)
{
  if ( 0 != new_edge_end && 1 != new_edge_end)
    return MYON_SubDEdgePtr::Null;
  MYON_SubDEdge* e = eptr.Edge();
  if (nullptr == e)
    return MYON_SubDEdgePtr::Null;
  const MYON_SubDVertex* v[3] = { e->m_vertex[0],nullptr,e->m_vertex[1] };
  MYON_SubDEdge* e1 = const_cast<MYON_SubDEdge*>(this->SplitEdge(e, vertex_location));
  if ( nullptr == e1)
    return MYON_SubDEdgePtr::Null;

  const MYON__UINT_PTR eptr_dir = eptr.EdgeDirection();
  const MYON_SubDEdgePtr e1ptr = MYON_SubDEdgePtr::Create(e1, eptr_dir);

  if (v[0] != e->m_vertex[0] || v[2] != e1->m_vertex[1])
  {
    MYON_SUBD_ERROR("Error in SplitEdge(MYON_SubDEdge*,...).");
    return e1ptr;
  }

  v[1] = e->m_vertex[1];
  if ( nullptr == v[1] || v[1] != e1->m_vertex[0] || 2 != v[1]->m_edge_count)
  {
    MYON_SUBD_ERROR("Error in SplitEdge(MYON_SubDEdge*,...).");
    return e1ptr;
  }

  if (e->m_face_count != e1->m_face_count)
  {
    MYON_SUBD_ERROR("Error in SplitEdge(MYON_SubDEdge*,...).");
    return e1ptr;
  }

  const unsigned v0edex = v[0]->EdgeArrayIndex(e);
  const unsigned v2edex = v[2]->EdgeArrayIndex(e1);
  if (v0edex >= v[0]->EdgeCount() || v2edex >= v[2]->EdgeCount())
  {
    MYON_SUBD_ERROR("Error in SplitEdge(MYON_SubDEdge*,...).");
    return e1ptr;
  }

  if (0 == eptr_dir && 1 == new_edge_end)
  {
    return e1ptr;
  }

  if (1 == eptr_dir && 0 == new_edge_end)
  {
    return e1ptr;
  }

  // swap e and e1 topology connections to put the "new edge" where the caller wants it.

  for (unsigned short efi = 0; efi < e->m_face_count; ++efi)
  {
    MYON_SubDFace* f = const_cast<MYON_SubDFace*>(e->Face(efi));
    if (nullptr == f)
      continue;
    const unsigned edge_count = f->EdgeCount();
    const unsigned fei = f->EdgeArrayIndex(e);
    const unsigned fe1i = f->EdgeArrayIndex(e1);
    if (fei >= edge_count || fe1i >= edge_count)
      continue;

    const MYON_SubDEdgePtr feptr = MYON_SubDEdgePtr::Create(e1, f->EdgePtr(fei).EdgeDirection()); // e1 is correct - we are swapping
    if (fei < 4)
      f->m_edge4[fei] = feptr;
    else
      f->m_edgex[fei - 4] = feptr;

    const MYON_SubDEdgePtr fe1ptr = MYON_SubDEdgePtr::Create(e, f->EdgePtr(fe1i).EdgeDirection()); // e is correct - we are swapping
    if (fe1i < 4)
      f->m_edge4[fe1i] = fe1ptr;
    else
      f->m_edgex[fe1i - 4] = fe1ptr;
  }

  const MYON_SubDEdgePtr new_v0eptr = MYON_SubDEdgePtr::Create(e1, v[0]->EdgePtr(v0edex).EdgeDirection());
  const MYON_SubDEdgePtr new_v2eptr = MYON_SubDEdgePtr::Create(e, v[2]->EdgePtr(v2edex).EdgeDirection());
  v[0]->m_edges[v0edex] = new_v0eptr;
  v[1]->m_edges[0] = new_v0eptr.Reversed();
  v[1]->m_edges[1] = new_v2eptr.Reversed();
  v[2]->m_edges[v2edex] = new_v2eptr;
  e1->m_vertex[0] = v[0];
  e1->m_vertex[1] = v[1];
  e->m_vertex[0] = v[1];
  e->m_vertex[1] = v[2];


  return e1ptr;
}

const MYON_SubDEdge* MYON_SubDimple::SpinEdge(MYON_SubDEdge* edge,
                                          bool spin_clockwise) {
  if (edge == nullptr || !edge->HasInteriorEdgeTopology(true) ||
      edge->SubdivisionLevel() >= m_levels.UnsignedCount() ||
      m_levels.UnsignedCount() < 1) {
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  unsigned int level0_index{edge->SubdivisionLevel()};

  if (m_levels[level0_index] == nullptr) return MYON_SUBD_RETURN_ERROR(nullptr);

  ClearHigherSubdivisionLevels(level0_index + 1);
  if (level0_index + 1 != m_levels.UnsignedCount()) {
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  m_active_level = m_levels[level0_index];
  if (m_active_level == nullptr || m_active_level->m_face_count == 0) {
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  MYON_SubDVertex* vertices[2]{const_cast<MYON_SubDVertex*>(edge->Vertex(0)),
                             const_cast<MYON_SubDVertex*>(edge->Vertex(1))};

  const bool swap_faces{(edge->FaceDirection(0) == 1) != spin_clockwise};
  MYON_SubDFace* faces[2]{
      const_cast<MYON_SubDFace*>(swap_faces ? edge->Face(1) : edge->Face(0)),
      const_cast<MYON_SubDFace*>(swap_faces ? edge->Face(0) : edge->Face(1))};

  const unsigned face_edge_indices[2]{faces[0]->EdgeArrayIndex(edge),
                                      faces[1]->EdgeArrayIndex(edge)};
  if (face_edge_indices[0] == MYON_UNSET_UINT_INDEX ||
      face_edge_indices[1] == MYON_UNSET_UINT_INDEX) {
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }
  MYON__UINT_PTR new_directions[2]{faces[0]->EdgeDirection(face_edge_indices[0]),
                                 faces[1]->EdgeDirection(face_edge_indices[1])};

  const MYON_SubDEdgePtr adjacent_edges[2]{
      spin_clockwise
          ? faces[1]->VertexEdgePair(vertices[0]).First().EdgePtr()
          : faces[1]->VertexEdgePair(vertices[0]).Second().EdgePtr(),
      spin_clockwise
          ? faces[0]->VertexEdgePair(vertices[1]).First().EdgePtr()
          : faces[0]->VertexEdgePair(vertices[1]).Second().EdgePtr()};

  MYON_SubDVertex* new_vertices[2]{
      const_cast<MYON_SubDVertex*>(
          adjacent_edges[0].RelativeVertex(spin_clockwise ? 0 : 1)),
      const_cast<MYON_SubDVertex*>(
          adjacent_edges[1].RelativeVertex(spin_clockwise ? 0 : 1))};
  if (new_vertices[0] == nullptr || new_vertices[1] == nullptr) {
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  MYON_SubDEdge* new_edge{edge};
  // Also calls vertices[i].VertexNotifiedModification()
  new_edge->EdgeModifiedNofification();

  for (unsigned int i = 0; i < 2; i++) {
    MYON_SubDVertex* v = new_vertices[i];
    edge->m_vertex[i] = v;

    bool rc{true};
    rc = rc && vertices[i]->RemoveEdgeFromArray(edge);
    if (!m_heap.GrowVertexEdgeArrayByOne(v)) {
      v->m_status.SetDamagedState(true);
      return MYON_SUBD_RETURN_ERROR(nullptr);
    }
    v->m_edges[v->m_edge_count++] =
        MYON_SubDEdgePtr::Create(new_edge, (MYON__UINT_PTR)i);

    rc = rc && vertices[i]->RemoveFaceFromArray(faces[1 - i]);
    if (!m_heap.GrowVertexFaceArrayByOne(v)) {
      v->m_status.SetDamagedState(true);
      return MYON_SUBD_RETURN_ERROR(nullptr);
    }
    v->m_faces[v->m_face_count++] = faces[i];
    v->VertexModifiedNofification();

    rc = rc &&
         (adjacent_edges[i].Edge()->ReplaceFaceInArray(faces[1 - i], faces[i]) <
          adjacent_edges[i].EdgeFaceCount());

    rc = rc && faces[i]->ReplaceEdgeInArray(0, edge, adjacent_edges[i]);
    rc = rc && faces[i]->ReplaceEdgeInArray(
                   0, adjacent_edges[1 - i].Edge(),
                   MYON_SubDEdgePtr::Create(new_edge, new_directions[i]));
    faces[i]->FaceModifiedNofification();
    if (!rc) return MYON_SUBD_RETURN_ERROR(nullptr);
  }
  if (new_edge == nullptr) return MYON_SUBD_RETURN_ERROR(nullptr);

  if (nullptr != m_active_level) {
    m_active_level->UpdateAllTagsAndSectorCoefficients(true);
  }

  ChangeGeometryContentSerialNumber(false);

  return new_edge;
}

const MYON_SubDEdge* MYON_SubD::SpinEdge(MYON_SubDEdge* edge, bool spin_clockwise) {
  MYON_SubDimple* subdimple = SubDimple(false);
  if (nullptr == subdimple) return MYON_SUBD_RETURN_ERROR(nullptr);
  return subdimple->SpinEdge(edge, spin_clockwise);
}

const MYON_SubDEdgePtr MYON_SubD::SpinEdge(MYON_SubDEdgePtr eptr,
                                       bool spin_clockwise) {
  MYON_SubDEdge* edge = eptr.Edge();
  if (nullptr == edge) return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  const MYON_SubDEdge* new_edge{SpinEdge(edge, spin_clockwise)};
  if (nullptr == new_edge) return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  return MYON_SubDEdgePtr::Create(new_edge, eptr.EdgeDirection());
}

const MYON_SubDEdge * MYON_SubD::SplitFace(
  MYON_SubDFace * face, 
  const MYON_SubDVertex * v0,
  const MYON_SubDVertex * v1
)
{
  if (nullptr == face || nullptr == v0 || nullptr == v1 || v0 == v1)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  unsigned int fvi0 = MYON_UNSET_UINT_INDEX;
  unsigned int fvi1 = MYON_UNSET_UINT_INDEX;
  MYON_SubDEdgePtr* eptr = face->m_edge4;
  for (unsigned short fei = 0; fei < face->m_edge_count; ++fei, ++eptr)
  {
    if (4 == fei)
    {
      eptr = face->m_edgex;
      if (nullptr == eptr)
        return MYON_SUBD_RETURN_ERROR(nullptr);
    }
    const MYON_SubDVertex* v = eptr->RelativeVertex(0);
    if (v == v0)
    {
      if (MYON_UNSET_UINT_INDEX != fvi0)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      fvi0 = fei;
    }
    else if (v == v1)
    {
      if (MYON_UNSET_UINT_INDEX != fvi1)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      fvi1 = fei;
    }
  }
  return (MYON_UNSET_UINT_INDEX != fvi0 && MYON_UNSET_UINT_INDEX != fvi1)
    ? SplitFace(face, fvi0, fvi1)
    : MYON_SUBD_RETURN_ERROR(nullptr);
}

const MYON_SubDEdge* MYON_SubD::SplitFace(
  MYON_SubDFace* face,
  unsigned int fvi0,
  unsigned int fvi1
  )
{
  MYON_SubDimple* subdimple = SubDimple(false);
  if ( 0 == subdimple )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  return subdimple->SplitFace(face,fvi0,fvi1);
}

static void Internal_SplitFaceSwapFacePtr(
  MYON_SubDFace* f,
  MYON__UINT_PTR pairA[2],
  MYON__UINT_PTR pairB[2]
  )
{
  if (nullptr == f)
    return;

  if (pairA[0] == pairB[0] || pairA[1] == pairB[0] || pairA[0] == pairB[1] || pairA[1] == pairB[1])
  {
    MYON_SUBD_ERROR("pairA[] and pairB[] must be disjoint sets of values.");
    return;
  }

  MYON_SubDEdgePtr* feptr = f->m_edge4;
  for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++feptr)
  {
    if (4 == fei)
    {
      feptr = f->m_edgex;
      if (nullptr == feptr)
        break;
    }

    MYON_SubDEdge* fe = MYON_SUBD_EDGE_POINTER(feptr->m_ptr);
    if (nullptr == fe)
      continue;

    MYON_SubDFacePtr* efptr = fe->m_face2;
    for (unsigned short efi = 0; efi < fe->m_face_count; ++efi, ++efptr)
    {
      if (2 == efi)
      {
        efptr = fe->m_facex;
        if (nullptr == efptr)
          break;
      }
      const MYON__UINT_PTR x = (efptr->m_ptr & MYON_SUBD_COMPONENT_POINTER_MASK);
      const MYON__UINT_PTR d = (efptr->m_ptr & MYON_SUBD_COMPONENT_DIRECTION_MASK);
      if (x == pairA[0])
        efptr->m_ptr = (pairA[1] | d);
      else if (x == pairB[0])
        efptr->m_ptr = (pairB[1] | d);
    }


    MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>(feptr->RelativeVertex(0));
    if (nullptr != v && nullptr != v->m_faces)
    {
      for (unsigned short vfi = 0; vfi < v->m_face_count; ++vfi)
      {
        const MYON__UINT_PTR x = (MYON__UINT_PTR)v->m_faces[vfi];
        if (x == pairA[0])
          v->m_faces[vfi] = (MYON_SubDFace*)pairA[1];
        else if (x == pairB[0])
          v->m_faces[vfi] = (MYON_SubDFace*)pairB[1];
      }
    }
  }
}

static bool Internal_ValidateFaceTopology(
  const MYON_SubDFace* f
)
{
  if (nullptr == f)
    return false;

  if (f->m_edge_count < 3 || 0 == f->m_id)
    return MYON_SUBD_RETURN_ERROR(false);

  const MYON_SubDVertex* firstv = nullptr;
  const MYON_SubDVertex* ev[2] = {};
  bool rc = true;
  unsigned short fdex;
  const MYON_SubDEdgePtr* feptr = f->m_edge4;
  for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++feptr)
  {
    if (4 == fei)
    {
      feptr = f->m_edgex;
      if (nullptr == feptr)
      {
        MYON_SUBD_ERROR("face m_edge_count > 4 and m_edgex is nullptr.");
        rc = false;
        break;
      }
    }

    MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(feptr->m_ptr);
    if (nullptr == e || 0 == e->m_id)
    {
      MYON_SUBD_ERROR("null edge in face.");
      rc = false;
      continue;
    }

    if (e != f->Edge(fei))
    {
      MYON_SUBD_ERROR("face->Edge(fei) != e");
      rc = false;
    }

    for (unsigned short k = fei + 1; k < f->m_edge_count; ++k)
    {
      if (e == f->Edge(k))
      {
        MYON_SUBD_ERROR("face references e 2 or more times.");
        rc = false;
      }
    }

    const MYON__UINT_PTR fedir = MYON_SUBD_FACE_DIRECTION(feptr->m_ptr);

    if (0 == fei)
    {
      firstv = e->m_vertex[fedir];
      ev[1] = firstv;
    }

    if (ev[1] != e->m_vertex[fedir])
    {
      MYON_SUBD_ERROR("consecutive edge pointers do not have the same vertex at the end/start.");
      rc = false;
    }

    ev[0] = e->m_vertex[fedir];
    ev[1] = e->m_vertex[1-fedir];
    if (nullptr == ev[0] || nullptr == ev[1])
    {
      MYON_SUBD_ERROR("edge has null vertex pointers.");
      rc = false;
    }
    else if (ev[0] == ev[1])
    {
      MYON_SUBD_ERROR("edge begins and ends at the same vertex.");
      rc = false;
    }

    fdex = e->m_face_count;
    const MYON_SubDFacePtr* efptr = e->m_face2;
    for (unsigned short efi = 0; efi < e->m_face_count; ++efi, ++efptr)
    {
      if (2 == efi)
      {
        efptr = e->m_facex;
        if (nullptr == efptr)
        {
          MYON_SUBD_ERROR("null edge in face.");
          rc = false;
          break;
        }
      }

      const MYON_SubDFace* ef = MYON_SUBD_FACE_POINTER(efptr->m_ptr);
      if (f == ef)
      {
        if (fdex < e->m_face_count)
        {
          MYON_SUBD_ERROR("ef references face 2 or more times.");
          rc = false;
          break;
        }
        fdex = efi;
        MYON__UINT_PTR efdir = MYON_SUBD_FACE_DIRECTION(efptr->m_ptr);
        if (efdir != fedir)
        {
          MYON_SUBD_ERROR("face and edge have inconsistent direction settings.");
          rc = false;
          break;
        }
      }
    }
    if (fdex >= e->m_face_count)
    {
      MYON_SUBD_ERROR("The face references and edge that does not reference the face.");
      rc = false;
    }

    const MYON_SubDVertex* v = ev[0];
    if (nullptr != v )
    {
      if (v != f->Vertex(fei))
      {
        MYON_SUBD_ERROR("face->Vertex(fei) != v");
        rc = false;
      }

      fdex = v->m_face_count;
      for (unsigned short vfi = 0; vfi < v->m_face_count; ++vfi)
      {
        const MYON_SubDFace* vf = v->m_faces[vfi];
        if (f == vf)
        {
          if (fdex < v->m_face_count)
          {
            MYON_SUBD_ERROR("ef references face 2 or more times.");
            rc = false;
            break;
          }
          fdex = vfi;
        }
      }
      if (fdex >= v->m_face_count)
      {
        MYON_SUBD_ERROR("The face references a vertex that does not reference the face.");
        rc = false;
      }

      for (unsigned short k = fei + 1; k < f->m_edge_count; ++k)
      {
        if (v == f->Vertex(k))
        {
          MYON_SUBD_ERROR("face references v 2 or more times.");
          rc = false;
        }
      }
    }
  }

  if (firstv != ev[1])
  {
    MYON_SUBD_ERROR("The vertices at the end ofthe final edge and start of the initial edges are different.");
    rc = false;
  }

  return rc;
}

const MYON_SubDEdgePtr MYON_SubD::SplitFace(
  class MYON_SubDFace* face,
  unsigned int fvi0,
  unsigned int fvi1,
  unsigned new_face_side
)
{
  if (new_face_side != 0 && new_face_side != 1)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDEdgePtr::Null);
  face->ClearSavedSubdivisionPoints(true);
  MYON_SubDEdge* e = const_cast<MYON_SubDEdge*>(this->SplitFace(face, fvi0, fvi1));
  if (nullptr == e)
    return MYON_SubDEdgePtr::Null;

  const MYON_SubDEdgePtr eptr = MYON_SubDEdgePtr::Create(e, 0);

  MYON_SubDFace* f2[2] = { const_cast<MYON_SubDFace*>(eptr.RelativeFace(0)), const_cast<MYON_SubDFace*>(eptr.RelativeFace(1)) };
  if (nullptr == f2[0] || nullptr == f2[1] || f2[0] == f2[1])
    return MYON_SUBD_RETURN_ERROR(eptr);
  if (face != f2[0] && face != f2[1])
    return MYON_SUBD_RETURN_ERROR(eptr);

  if ( face == f2[new_face_side])
  {

    // swap faces

    // markA and markB need to be different, must be values no valid heap pointer will
    // ever have, and need to be chosen so that debug runtime pointer checking code will not
    // throw exceptions. Values like 1 and 2 would work just fine except debug runtime
    // pointer checking code is mindlessly diligent. the addresses of marking_faces[0]
    // and marking_faces[1] "tricks" the debug runtime pointer checking code and
    // are values no valid app heap will ever have (they point to stack
    // used by this call).
    const MYON_SubDFace marking_faces[2] = {};
    const MYON__UINT_PTR markA = (MYON__UINT_PTR)(&marking_faces[0]);
    const MYON__UINT_PTR markB = (MYON__UINT_PTR)(&marking_faces[1]);

    // The mark first, then change approach must be used because f2[0] and f2[1]
    // have multiple vertex and edge references and we must avoid swapping twice.

    // First change face references f2[0] to markA and f2[1] to markB.
    MYON__UINT_PTR pairA[2] = { (MYON__UINT_PTR)f2[0], markA };
    MYON__UINT_PTR pairB[2] = { (MYON__UINT_PTR)f2[1], markB };
    for (unsigned fdex = 0; fdex < 2; ++fdex)
      Internal_SplitFaceSwapFacePtr(f2[fdex], pairA, pairB);

    // ... then change markA to f2[1] and markB to f2[0].
    pairA[0] = markA;  pairA[1] = (MYON__UINT_PTR)f2[1];
    pairB[0] = markB;  pairB[1] = (MYON__UINT_PTR)f2[0];
    for (unsigned fdex = 0; fdex < 2; ++fdex)
      Internal_SplitFaceSwapFacePtr(f2[fdex], pairA, pairB);

    // At this point, all the vertex and edge references to f2[0] and f2[1] have been swapped.    

    // All that is left is to wwap edge references on f2[0] and f2[1].
    const unsigned short edge_count[2] = { f2[1]->m_edge_count, f2[0]->m_edge_count };
    const unsigned short edgex_capacity[2] = { f2[1]->m_edgex_capacity, f2[0]->m_edgex_capacity };
    MYON_SubDEdgePtr edge4[2][4] = {
      {f2[1]->m_edge4[0],f2[1]->m_edge4[1],f2[1]->m_edge4[2],f2[1]->m_edge4[3]},
      {f2[0]->m_edge4[0],f2[0]->m_edge4[1],f2[0]->m_edge4[2],f2[0]->m_edge4[3]}
    };
    MYON_SubDEdgePtr* edgex[2] = { f2[1]->m_edgex, f2[0]->m_edgex };
    for (unsigned i = 0; i < 2; ++i)
    {
      f2[i]->m_edge_count = edge_count[i];
      f2[i]->m_edgex_capacity = edgex_capacity[i];
      f2[i]->m_edge4[0] = edge4[i][0];
      f2[i]->m_edge4[1] = edge4[i][1];
      f2[i]->m_edge4[2] = edge4[i][2];
      f2[i]->m_edge4[3] = edge4[i][3];
      f2[i]->m_edgex = edgex[i];
    }

    //// The Internal_SplitFaceSwapFacePtr() calls above have switched the
    //// MYON_SubDFace pointer values in e->m_face2[]. Because the faces
    //// are switching sides, we need to swap the directions in e->m_face2[].
    //// 
    //e->m_face2[0] = e->m_face2[0].Reversed();
    //e->m_face2[1] = e->m_face2[1].Reversed();
  }


  if (2 == e->m_face_count && face == e->m_face2[new_face_side].Face())
  {
    // Adjust the e->m_face2[] so it jibes with the relative face values exactly.
    MYON_SubDFacePtr tmp = e->m_face2[0];
    e->m_face2[0] = e->m_face2[1];
    e->m_face2[1] = tmp;
  }

  if (false == Internal_ValidateFaceTopology(f2[0])
    || false == Internal_ValidateFaceTopology(f2[1])
    || f2[0] == f2[1]
    )
  {
    MYON_SUBD_ERROR("Invalid faces.");
  }


  return eptr; // new face is on the requested side
}

const MYON_SubDEdgePtr MYON_SubD::SplitFace(
  class MYON_SubDFace* face,
  const class MYON_SubDVertex* v0,
  const class MYON_SubDVertex* v1,
  unsigned new_face_side
)
{
  if (nullptr == face)
    return MYON_SubDEdgePtr::Null;
  return this->SplitFace(face, face->VertexIndex(v0), face->VertexIndex(v1), new_face_side);
}

void MYON_SubD::MarkAggregateComponentStatusAsNotCurrent() const
{
  const MYON_SubDLevel* level = ActiveLevelConstPointer();
  if ( level )
    level->MarkAggregateComponentStatusAsNotCurrent();
}

unsigned int MYON_SubDLevel::ClearStates(
  MYON_ComponentStatus states_to_clear
  ) const
{
  unsigned int rc = 0;
  m_aggregates.m_aggregate_status.ClearAggregateStatus(states_to_clear);
  for (const MYON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    rc += vertex->m_status.ClearStates(states_to_clear);
  for (const MYON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    rc += edge->m_status.ClearStates(states_to_clear);
  for (const MYON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    rc += face->m_status.ClearStates(states_to_clear);
  return rc;
}

unsigned int MYON_SubDLevel::ClearRuntimeMarks(
  bool bClearVertexMarks,
  bool bClearEdgeMarks,
  bool bClearFaceMarks
) const
{
  unsigned int rc = 0;
  if (bClearVertexMarks)
  {
    for (const MYON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      if (vertex->m_status.ClearRuntimeMark())
        ++rc;
    }
  }
  if (bClearEdgeMarks)
  {
    for (const MYON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    {
      if (edge->m_status.ClearRuntimeMark())
        ++rc;
    }
  }
  if (bClearFaceMarks)
  {
    for (const MYON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    {
      if (face->m_status.ClearRuntimeMark())
        ++rc;
    }
  }
  return rc;
}

unsigned int MYON_SubD::ClearComponentStates(
  MYON_ComponentStatus states_to_clear
  ) const
{
  const MYON_SubDLevel* level = ActiveLevelConstPointer();
  if ( level )
    return level->ClearStates(states_to_clear);
  return MYON_SUBD_RETURN_ERROR(0);
}

MYON_AggregateComponentStatus MYON_SubD::AggregateComponentStatus() const
{  
  return ActiveLevel().AggregateComponentStatus();
}

unsigned int MYON_SubDLevel::GetComponentsWithSetStates(
  MYON_ComponentStatus states_filter,
  bool bAllEqualStates,
  MYON_SimpleArray< MYON_SubDComponentPtr >& components_with_set_states
  ) const
{
  components_with_set_states.SetCount(0);
  if (states_filter.IsClear())
    return 0;

  const MYON_AggregateComponentStatusEx acs = AggregateComponentStatus();

  MYON_ComponentStatus as = acs.AggregateStatus();
  if (bAllEqualStates)
  {
    if ( false == as.AllEqualStates(states_filter, states_filter) )
      return 0;
  }
  else
  {
    if ( false == as.SomeEqualStates(states_filter, states_filter) )
      return 0;
  }

  unsigned int c = 0;
  if ( states_filter.IsSelected() && c < m_aggregates.m_aggregate_status.SelectedCount() )
    c = m_aggregates.m_aggregate_status.SelectedCount();
  if ( states_filter.IsHighlighted() && c < m_aggregates.m_aggregate_status.HighlightedCount() )
    c = m_aggregates.m_aggregate_status.HighlightedCount();
  if ( states_filter.IsHidden() && c < m_aggregates.m_aggregate_status.HiddenCount() )
    c = m_aggregates.m_aggregate_status.HiddenCount();
  if ( states_filter.IsLocked() && c < m_aggregates.m_aggregate_status.LockedCount() )
    c = m_aggregates.m_aggregate_status.LockedCount();
  if ( states_filter.IsDamaged() && c < m_aggregates.m_aggregate_status.DamagedCount() )
    c = m_aggregates.m_aggregate_status.DamagedCount();
  if ( states_filter.IsSelected() && c < m_aggregates.m_aggregate_status.SelectedCount() )
    c = m_aggregates.m_aggregate_status.SelectedCount();
  components_with_set_states.Reserve(c);

  if (bAllEqualStates)
  {
    for (const MYON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      if (vertex->m_status.AllEqualStates(states_filter, states_filter))
        components_with_set_states.Append(MYON_SubDComponentPtr::Create(vertex));
    }
    for (const MYON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    {
      if (edge->m_status.AllEqualStates(states_filter, states_filter))
        components_with_set_states.Append(MYON_SubDComponentPtr::Create(edge));
    }
    for (const MYON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    {
      if (face->m_status.AllEqualStates(states_filter, states_filter))
        components_with_set_states.Append(MYON_SubDComponentPtr::Create(face));
    }
  }
  else
  {
    for (const MYON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      if (vertex->m_status.SomeEqualStates(states_filter, states_filter))
        components_with_set_states.Append(MYON_SubDComponentPtr::Create(vertex));
    }
    for (const MYON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
    {
      if (edge->m_status.SomeEqualStates(states_filter, states_filter))
        components_with_set_states.Append(MYON_SubDComponentPtr::Create(edge));
    }
    for (const MYON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
    {
      if (face->m_status.SomeEqualStates(states_filter, states_filter))
        components_with_set_states.Append(MYON_SubDComponentPtr::Create(face));
    }
  }

  return components_with_set_states.UnsignedCount();
}


unsigned int MYON_SubD::GetComponentsWithSetStates(
  MYON_ComponentStatus states_filter,
  bool bAllEqualStates,
  MYON_SimpleArray< MYON_SubDComponentPtr >& components_with_set_states
  ) const
{
  return ActiveLevel().GetComponentsWithSetStates(
    states_filter,
    bAllEqualStates,
    components_with_set_states
    );
}


unsigned int MYON_SubD::GetComponentsWithSetStates(
  MYON_ComponentStatus states_filter,
  bool bAllEqualStates,
  MYON_SimpleArray< MYON_COMPONENT_INDEX >& components_with_set_states
  ) const
{
  components_with_set_states.SetCount(0);
  MYON_SimpleArray< MYON_SubDComponentPtr > cptr;
  GetComponentsWithSetStates(
    states_filter,
    bAllEqualStates,
    cptr
    );
  unsigned int count = cptr.UnsignedCount();
  if (count > 0)
  {
    components_with_set_states.Reserve(count);
    components_with_set_states.SetCount(count);
    const MYON_SubDComponentPtr* cp = cptr.Array();
    MYON_COMPONENT_INDEX* ci = components_with_set_states.Array();
    for ( const MYON_SubDComponentPtr* cp1 = cp+count; cp < cp1; cp++ )
      *ci++ = cp->ComponentIndex();
  }
  return count;
}

unsigned int MYON_SubDLevel::SetStates(
  MYON_SubDComponentPtr component_ptr,
  MYON_ComponentStatus states_to_set
  ) const
{
  if (0 != component_ptr.SetStates(states_to_set))
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
    return 1;
  }
  return 0;
}

unsigned int MYON_SubDLevel::ClearStates(
  MYON_SubDComponentPtr component_ptr,
  MYON_ComponentStatus states_to_clear
  ) const
{
  if (0 != component_ptr.ClearStates(states_to_clear))
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
    return 1;
  }
  return 0;
}

unsigned int MYON_SubDLevel::SetStatus(
  MYON_SubDComponentPtr component_ptr,
  MYON_ComponentStatus status_to_copy
  ) const
{
  if (0 != component_ptr.SetStatus(status_to_copy))
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
    return 1;
  }
  return 0;
}

unsigned int MYON_SubD::SetComponentStates(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus states_to_set
  ) const
{
  return SetComponentStates(ComponentPtrFromComponentIndex(component_index),states_to_set);
}

unsigned int MYON_SubD::SetComponentStates(
  MYON_SubDComponentPtr component_ptr,
  MYON_ComponentStatus states_to_set
  ) const
{
  const MYON_SubDLevel* level = ActiveLevelConstPointer();
  if ( nullptr != level )
    return level->SetStates(component_ptr,states_to_set);
  return MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubD::ClearComponentStates(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus states_to_clear
  ) const
{
  return ClearComponentStates(ComponentPtrFromComponentIndex(component_index),states_to_clear);
}

unsigned int MYON_SubD::ClearComponentStates(
  MYON_SubDComponentPtr component_ptr,
  MYON_ComponentStatus states_to_clear
  ) const
{
  const MYON_SubDLevel* level = ActiveLevelConstPointer();
  if ( nullptr != level )
    return level->ClearStates(component_ptr,states_to_clear);
  return MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubD::SetComponentStatus(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus status_to_copy
  ) const
{
  return ClearComponentStates(ComponentPtrFromComponentIndex(component_index),status_to_copy);
}

unsigned int MYON_SubD::SetComponentStatus(
  MYON_SubDComponentPtr component_ptr,
  MYON_ComponentStatus status_to_copy
  ) const
{
  const MYON_SubDLevel* level = ActiveLevelConstPointer();
  if ( nullptr != level )
    return level->SetStatus(component_ptr,status_to_copy);
  return MYON_SUBD_RETURN_ERROR(0);
}

void MYON_SubDLevel::ClearEvaluationCache() const
{
  ClearEdgeFlags();
  ClearBoundingBox();
  m_surface_mesh = MYON_SubDMesh::Empty;
  m_control_net_mesh = MYON_SubDMesh::Empty;
  m_aggregates.MarkAllAsNotCurrent();

  for (const MYON_SubDVertex* vertex = m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
  {
    vertex->ClearSavedSubdivisionPoints();
  }

  for (const MYON_SubDEdge* edge = m_edge[0]; nullptr != edge; edge = edge->m_next_edge)
  {
    edge->ClearSavedSubdivisionPoints();
    // NO // edge->UnsetSectorCoefficients(); 
    // Leave these set - they are not "cached" values and except for corner case below
    // the are independent vertex locations.
    if ( edge->IsSmooth() )
    {
      for (unsigned evi = 0; evi < 2; evi++)
      {
        if ( false == (edge->m_sector_coefficient[evi] > 0.0 && edge->m_sector_coefficient[evi] < 1.0) )
          continue;
        const MYON_SubDVertex* v = edge->m_vertex[evi];
        if (nullptr == v)
          continue;
        if (MYON_SubDVertexTag::Corner != v->m_vertex_tag)
          continue;
        // corner sector coefficients depend on the subtended angle of the sector's crease boundary.
        // All other sector coefficients are independent of vertex location.
        const_cast<MYON_SubDEdge* >(edge)->m_sector_coefficient[evi] = MYON_SubDSectorType::Create(edge, evi).SectorCoefficient();
      }
    }
  }

  for (const MYON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
  {
    face->ClearSavedSubdivisionPoints();
  }
}


bool MYON_SubD::CopyEvaluationCacheForExperts(const MYON_SubD& src)
{
  const MYON_SubDimple* src_subdimple = src.m_subdimple_sp.get();
  MYON_SubDimple* this_subdimple = m_subdimple_sp.get();
  return (nullptr != src_subdimple && nullptr != this_subdimple) ? this_subdimple->CopyEvaluationCacheForExperts(*src_subdimple) : false;
}

bool MYON_SubDimple::CopyEvaluationCacheForExperts(const MYON_SubDimple& src)
{
  const MYON_SubDLevel* src_level = src.ActiveLevelConstPointer();
  MYON_SubDLevel* this_level = this->ActiveLevelPointer();
  return (nullptr != src_level && nullptr != this_level) ? this_level->CopyEvaluationCacheForExperts(this->m_heap , *src_level, src.m_heap) : false;
}

bool MYON_SubDLevel::CopyEvaluationCacheForExperts( MYON_SubDHeap& this_heap, const MYON_SubDLevel& src, const MYON_SubDHeap& src_heap)
{
  // Validate conditions for coping the cached evaluation information
  if (
    this == &src
    || m_vertex_count != src.m_vertex_count
    || m_edge_count != src.m_edge_count
    || m_face_count != src.m_face_count
    )
    return MYON_SUBD_RETURN_ERROR(false);

  // The built in fragment cache always has adaptive MYON_SubDDisplayParameters::DefaultDensity
  unsigned subd_display_density = MYON_SubDDisplayParameters::AbsoluteDisplayDensityFromSubDFaceCount(MYON_SubDDisplayParameters::DefaultDensity,m_face_count);
  const unsigned this_level_index = this->m_level_index;
  const unsigned src_level_index = src.m_level_index;

  // It is critical to use the this_vit/src_vit iterators so we got through the vertices in id order.
  // When a copy of an editied subd is made, it is frequently the case that the vertex linked lists
  // are in different order. 
  MYON_SubDVertexIdIterator this_vit;
  MYON_SubDVertexIdIterator src_vit;
  this_heap.InitializeVertexIdIterator(this_vit);
  src_heap.InitializeVertexIdIterator(src_vit);

  MYON_SubDVertex* this_vertex;
  const MYON_SubDVertex* src_vertex;
  MYON_SubDEdgePtr this_veptr, src_veptr;
  MYON_SubDFace* this_face;
  const MYON_SubDFace* src_face;
  bool bCopyVertexCache = false;
  for (
    this_vertex = const_cast<MYON_SubDVertex*>(this_vit.FirstVertexOnLevel(this_level_index)), src_vertex = src_vit.FirstVertexOnLevel(src_level_index);
    nullptr != this_vertex && nullptr != src_vertex; 
    this_vertex = const_cast<MYON_SubDVertex*>(this_vit.NextVertexOnLevel(this_level_index)), src_vertex = src_vit.NextVertexOnLevel(src_level_index)
    )
  {
    if (this_vertex->m_id != src_vertex->m_id)
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_vertex->m_edge_count != src_vertex->m_edge_count)
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_vertex->m_face_count != src_vertex->m_face_count)
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_vertex->m_vertex_tag != src_vertex->m_vertex_tag)
      return MYON_SUBD_RETURN_ERROR(false);
    if (false == (this_vertex->ControlNetPoint() == src_vertex->ControlNetPoint()))
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_vertex->SavedSubdivisionPointIsSet() && false == (this_vertex->SubdivisionPoint() == src_vertex->SubdivisionPoint()))
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_vertex->SavedSubdivisionPointIsSet() && this_vertex->SurfacePointIsSet())
      continue;
    if (false == src_vertex->SavedSubdivisionPointIsSet())
      continue;
    bCopyVertexCache = true;
    for (unsigned short vei = 0; vei < this_vertex->m_edge_count; ++vei)
    {
      this_veptr = this_vertex->m_edges[vei];
      src_veptr = src_vertex->m_edges[vei];
      if ( this_veptr.EdgeId() != src_veptr.EdgeId())
        return MYON_SUBD_RETURN_ERROR(false);
      if (MYON_SUBD_EDGE_DIRECTION(this_veptr.m_ptr) != MYON_SUBD_EDGE_DIRECTION(src_veptr.m_ptr))
        return MYON_SUBD_RETURN_ERROR(false);
    }
    for (unsigned short vfi = 0; vfi < this_vertex->m_face_count; ++vfi)
    {
      this_face = const_cast<MYON_SubDFace*>(this_vertex->m_faces[vfi]);
      src_face = src_vertex->m_faces[vfi];
      if (nullptr == this_face || nullptr == src_face || this_face->m_id != src_face->m_id)
        return MYON_SUBD_RETURN_ERROR(false);
    }
  }
  if (nullptr != this_vertex || nullptr != src_vertex)
    return MYON_SUBD_RETURN_ERROR(false);

  // It is critical to use the this_eit/src_eit iterators so we got through the edges in id order.
  // When a copy of an editied subd is made, it is frequently the case that the edge linked lists
  // are in different order. 
  MYON_SubDEdgeIdIterator this_eit;
  MYON_SubDEdgeIdIterator src_eit;
  this_heap.InitializeEdgeIdIterator(this_eit);
  src_heap.InitializeEdgeIdIterator(src_eit);

  MYON_SubDEdge* this_edge;
  const MYON_SubDEdge* src_edge;
  const MYON_SubDFacePtr* this_fptr;
  const MYON_SubDFacePtr* src_fptr;
  bool bCopyEdgeCache = false;
  for (
    this_edge = const_cast<MYON_SubDEdge*>(this_eit.FirstEdgeOnLevel(this_level_index)), src_edge = src_eit.FirstEdgeOnLevel(src_level_index);
    nullptr != this_edge && nullptr != src_edge;
    this_edge = const_cast<MYON_SubDEdge*>(this_eit.NextEdgeOnLevel(this_level_index)), src_edge = src_eit.NextEdgeOnLevel(src_level_index)
    )
  {
    if (this_edge->m_id != src_edge->m_id)
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_edge->m_face_count != src_edge->m_face_count)
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_edge->m_edge_tag != src_edge->m_edge_tag)
      return MYON_SUBD_RETURN_ERROR(false);
    for (int evi = 0; evi < 2; ++evi)
    {
      if (nullptr == this_edge->m_vertex[evi] || nullptr == src_edge->m_vertex[evi])
        return MYON_SUBD_RETURN_ERROR(false);
      if (this_edge->m_vertex[evi]->m_id != src_edge->m_vertex[evi]->m_id)
        return MYON_SUBD_RETURN_ERROR(false);
    }
    if (this_edge->SavedSubdivisionPointIsSet() && false == (this_edge->SubdivisionPoint() == src_edge->SubdivisionPoint()))
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_edge->SavedSubdivisionPointIsSet() && this_edge->EdgeSurfaceCurveIsSet())
      continue;
    if (false == src_edge->SavedSubdivisionPointIsSet())
      continue;
    bCopyEdgeCache = true;
    this_fptr = this_edge->m_face2;
    src_fptr = src_edge->m_face2;
    for (unsigned short efi = 0; efi < this_edge->m_face_count; ++efi, ++this_fptr, ++src_fptr)
    {
      if (2 == efi)
      {
        this_fptr = this_edge->m_facex;
        src_fptr = src_edge->m_facex;
        if (nullptr == this_fptr || nullptr == src_fptr)
          return MYON_SUBD_RETURN_ERROR(false);
      }
      this_face = MYON_SUBD_FACE_POINTER(this_fptr->m_ptr);
      src_face = MYON_SUBD_FACE_POINTER(src_fptr->m_ptr);
      if (nullptr == this_face || nullptr == src_face || this_face->m_id != src_face->m_id )
        return MYON_SUBD_RETURN_ERROR(false);
      if ( this_face->m_edge_count != src_face->m_edge_count )
        return MYON_SUBD_RETURN_ERROR(false);
      if (MYON_SUBD_FACE_DIRECTION(this_fptr->m_ptr) != MYON_SUBD_FACE_DIRECTION(src_fptr->m_ptr))
        return MYON_SUBD_RETURN_ERROR(false);
    }
  }
  if (nullptr != this_edge || nullptr != src_edge)
    return MYON_SUBD_RETURN_ERROR(false);

  // It is critical to use the this_fit/src_fit iterators so we got through the faces in id order.
  // When a copy of an editied subd is made, it is frequently the case that the face linked lists
  // are in different order. 
  MYON_SubDFaceIdIterator this_fit;
  MYON_SubDFaceIdIterator src_fit;
  this_heap.InitializeFaceIdIterator(this_fit);
  src_heap.InitializeFaceIdIterator(src_fit);

  const MYON_SubDEdgePtr* this_eptr;
  const MYON_SubDEdgePtr* src_eptr;
  bool bCopyFaceCache = false;
  for (
    this_face = const_cast<MYON_SubDFace*>(this_fit.FirstFaceOnLevel(this_level_index)), src_face = src_fit.FirstFaceOnLevel(src_level_index);
    nullptr != this_face && nullptr != src_face;
    this_face = const_cast<MYON_SubDFace*>(this_fit.NextFaceOnLevel(this_level_index)), src_face = src_fit.NextFaceOnLevel(src_level_index)
    )
  {
    if (this_face->m_id != src_face->m_id)
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_face->m_edge_count != src_face->m_edge_count)
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_face->SavedSubdivisionPointIsSet() && false == (this_face->SubdivisionPoint() == src_face->SubdivisionPoint()) )
      return MYON_SUBD_RETURN_ERROR(false);
    if (this_face->SavedSubdivisionPointIsSet() && nullptr != this_face->MeshFragments())
      continue;
    if (false == src_face->SavedSubdivisionPointIsSet())
      continue;
    bCopyFaceCache = true;
    this_eptr = this_face->m_edge4;
    src_eptr = src_face->m_edge4;
    for (unsigned short fei = 0; fei < this_face->m_edge_count; ++fei, ++this_eptr, ++src_eptr)
    {
      if (4 == fei)
      {
        this_eptr = this_face->m_edgex;
        src_eptr = src_face->m_edgex;
        if(nullptr == this_eptr || nullptr == src_eptr)
          return MYON_SUBD_RETURN_ERROR(false);
      }
      if ( this_eptr->EdgeId() != src_eptr->EdgeId())
        return MYON_SUBD_RETURN_ERROR(false);
      if ( MYON_SUBD_EDGE_DIRECTION(this_eptr->m_ptr) != MYON_SUBD_EDGE_DIRECTION(src_eptr->m_ptr))
        return MYON_SUBD_RETURN_ERROR(false);
    }
  }
  if (nullptr != this_face || nullptr != src_face)
    return MYON_SUBD_RETURN_ERROR(false);

  if (false == bCopyVertexCache && false == bCopyEdgeCache && false == bCopyFaceCache)
    return false;

  // this and src subd have identical geometry - copy evluation cache

  double subdivision_point[3];
  if (bCopyVertexCache)
  {
    // It is critical to use the this_vit/src_vit iterators so we got through the vertices in id order.
    // When a copy of an editied subd is made, it is frequently the case that the vertex linked lists
    // are in different order. 
    MYON_SubDSectorSurfacePoint this_limit_point;
    for (
      this_vertex = const_cast<MYON_SubDVertex*>(this_vit.FirstVertexOnLevel(this_level_index)), src_vertex = src_vit.FirstVertexOnLevel(src_level_index);
      nullptr != this_vertex && nullptr != src_vertex;
      this_vertex = const_cast<MYON_SubDVertex*>(this_vit.NextVertexOnLevel(this_level_index)), src_vertex = src_vit.NextVertexOnLevel(src_level_index)
      )
    {
      if (false == src_vertex->GetSavedSubdivisionPoint(subdivision_point))
        continue;
      if (false == this_vertex->SavedSubdivisionPointIsSet())
        this_vertex->SetSavedSubdivisionPoint(subdivision_point);
     
      if (false == this_vertex->SurfacePointIsSet())
      {
        for (const MYON_SubDSectorSurfacePoint* src_limit_point = &src_vertex->SectorSurfacePointForExperts(); nullptr != src_limit_point; src_limit_point = src_limit_point->m_next_sector_limit_point)
        {
          this_limit_point = *src_limit_point;
          this_limit_point.m_next_sector_limit_point = (MYON_SubDSectorSurfacePoint*)1;
          this_limit_point.m_sector_face = nullptr;
          if (nullptr != src_limit_point->m_sector_face)
          {
            const unsigned vfi = src_vertex->FaceArrayIndex(src_limit_point->m_sector_face);
            if (vfi >= src_vertex->m_face_count)
              break;
            this_limit_point.m_sector_face = this_vertex->m_faces[vfi];
          }
          this_vertex->SetSavedSurfacePoint(true, this_limit_point);
        }
      }
    }
  }

  if (bCopyEdgeCache)
  {    
    // It is critical to use the this_eit/src_eit iterators so we got through the edges in id order.
    // When a copy of an editied subd is made, it is frequently the case that the edge linked lists
    // are in different order. 
    MYON_SimpleArray<MYON_3dPoint> edge_curve_cvs(MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity);
    for (
      this_edge = const_cast<MYON_SubDEdge*>(this_eit.FirstEdgeOnLevel(this_level_index)), src_edge = src_eit.FirstEdgeOnLevel(src_level_index);
      nullptr != this_edge && nullptr != src_edge;
      this_edge = const_cast<MYON_SubDEdge*>(this_eit.NextEdgeOnLevel(this_level_index)), src_edge = src_eit.NextEdgeOnLevel(src_level_index)
      )
    {
      if (false == src_edge->GetSavedSubdivisionPoint(subdivision_point))
        continue;
      if (false == this_edge->SavedSubdivisionPointIsSet())
        this_edge->SetSavedSubdivisionPoint(subdivision_point);
      if ( false == this_edge->EdgeSurfaceCurveIsSet() && src_edge->EdgeSurfaceCurveIsSet( ))
        this_heap.CopyEdgeSurfaceCurve(src_edge, this_edge);
    }
  }

  if (bCopyFaceCache)
  {
    // It is critical to use the this_fit/src_fit iterators so we got through the faces in id order.
    // When a copy of an editied subd is made, it is frequently the case that the face linked lists
    // are in different order. 
    for (
      this_face = const_cast<MYON_SubDFace*>(this_fit.FirstFaceOnLevel(this_level_index)), src_face = src_fit.FirstFaceOnLevel(src_level_index);
      nullptr != this_face && nullptr != src_face;
      this_face = const_cast<MYON_SubDFace*>(this_fit.NextFaceOnLevel(this_level_index)), src_face = src_fit.NextFaceOnLevel(src_level_index)
      )
    {
      if (false == src_face->GetSavedSubdivisionPoint(subdivision_point))
        continue;
      if ( false == this_face->SavedSubdivisionPointIsSet())
        this_face->SetSavedSubdivisionPoint(subdivision_point);
      if (nullptr == this_face->MeshFragments() && nullptr != src_face->MeshFragments())
        this_heap.CopyMeshFragments(src_face, subd_display_density, this_face);
    }
  }

  return true;
}

unsigned int MYON_SubD::ComponentPtrFromComponentIndex(
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  bool bIncludeVertices,
  bool bIncludeEdges,
  bool bIncludeFaces,
  MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list
) const
{
  if ( ci_count <= 0 )
    return 0;

  if (
    false == bIncludeVertices
    && false == bIncludeEdges
    && false == bIncludeFaces
    )
    return 0;

  if ( nullptr == ci_list )
    return MYON_SUBD_RETURN_ERROR(0);

  const unsigned int count0 = cptr_list.UnsignedCount();

  cptr_list.Reserve(count0 + ci_count);

  const bool bFilter
    = false == bIncludeVertices
    || false == bIncludeEdges
    || false == bIncludeFaces;



  for (size_t i = 0; i < ci_count; i++)
  {
    const MYON_COMPONENT_INDEX ci = ci_list[i];
    if (bFilter)
    {
      if (false == bIncludeVertices || MYON_COMPONENT_INDEX::TYPE::subd_vertex == ci.m_type)
        continue;
      if (false == bIncludeEdges || MYON_COMPONENT_INDEX::TYPE::subd_edge == ci.m_type)
        continue;
      if (false == bIncludeFaces || MYON_COMPONENT_INDEX::TYPE::subd_face == ci.m_type)
        continue;
    }
    MYON_SubDComponentPtr cptr = ComponentPtrFromComponentIndex(ci_list[i]);
    if (cptr.IsNull())
      continue;
    cptr_list.Append(cptr);
  }

  return (cptr_list.UnsignedCount() - count0);
}

unsigned int MYON_SubD::ComponentPtrFromComponentIndex(
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list
) const
{
  return ComponentPtrFromComponentIndex(ci_list, ci_count, true, true, true, cptr_list);
}



bool MYON_SubD::DeleteComponents(
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count
  )
{
  MYON_SimpleArray<MYON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return true; // nothing to delete

  return DeleteComponents(cptr_list.Array(),cptr_list.UnsignedCount(),false);
}

bool MYON_SubD::DeleteComponents(
  const MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list,
  bool bMarkDeletedFaceEdges
)
{
  return DeleteComponents(cptr_list.Array(), cptr_list.UnsignedCount(), bMarkDeletedFaceEdges);
}

bool MYON_SubD::DeleteComponents(
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bMarkDeletedFaceEdges
)
{
  const bool bDeleteIsolatedEdges = true;
  const bool bUpdateTagsAndCoefficients = true;
  return DeleteComponentsForExperts(cptr_list, cptr_count, bDeleteIsolatedEdges, bUpdateTagsAndCoefficients, bMarkDeletedFaceEdges);
}

bool MYON_SubD::DeleteComponentsForExperts(
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bDeleteIsolatedEdges,
  bool bUpdateTagsAndCoefficients,
  bool bMarkDeletedFaceEdges
)
{
  if (bMarkDeletedFaceEdges)
    ClearComponentMarks(false,true,false,nullptr);

  if ( cptr_count <= 0 )
    return true;

  if ( nullptr == cptr_list )
    return MYON_SUBD_RETURN_ERROR(false);

  MYON_SubDimple* subdimple = SubDimple(false);
  if ( nullptr == subdimple )
    return MYON_SUBD_RETURN_ERROR(false);

  const unsigned int level_count = subdimple->LevelCount();
  if (level_count <= 0)
    return MYON_SUBD_RETURN_ERROR(false);
  
  unsigned level_index = level_count;
  for (size_t i = 0; i < cptr_count; i++)
  {
    const MYON_SubDComponentBase* c = cptr_list[i].ComponentBase();
    if ( nullptr == c)
      continue;
    if ( c->SubdivisionLevel() < level_index )
      level_index = c->SubdivisionLevel();
  }
  if ( level_index == level_count )
    return MYON_SUBD_RETURN_ERROR(false);

  if ( false == subdimple->SetActiveLevel(level_index) )
    return MYON_SUBD_RETURN_ERROR(false);

  subdimple->ClearHigherSubdivisionLevels(level_index);
  const MYON_SubDLevel* level = subdimple->ActiveLevelPointer();
  if ( nullptr == level || level->m_level_index != level_index)
    return MYON_SUBD_RETURN_ERROR(false);

  // Make sure no components have a status = MYON_ComponentStatus::AllSet
  // because this uncommon status value is used to mark components that will be be deleted.
  MYON_SubDComponentIterator cit(*this);
  for (MYON_SubDComponentPtr cptr = cit.FirstComponent(); cptr.IsNotNull(); cptr = cit.NextComponent())
  {
    if ( MYON_ComponentStatus::AllSet == cptr.Status() )
      cptr.ClearStates(MYON_ComponentStatus::Damaged);
  }

  // Set the status of every compoent in cptr_list[] to MYON_ComponentStatus::AllSet.
  // If that component is a vertex, set the status of every edge and face that
  // touch the vertex to MYON_ComponentStatus::AllSet.
  // If that component is an edge, set the status of every face that
  // touches the edge to MYON_ComponentStatus::AllSet.
  for (size_t i = 0; i < cptr_count; i++)
  {
    MYON_SubDComponentPtr cptr = cptr_list[i];
    const MYON_SubDComponentBase* c = cptr.ComponentBase();
    if (nullptr == c)
      continue;
    if (c->SubdivisionLevel() != level_index)
      continue;
    c->m_status = MYON_ComponentStatus::AllSet;
    switch (cptr.ComponentType())
    {
    case MYON_SubDComponentPtr::Type::Vertex:
      {
        const MYON_SubDVertex* vertex = cptr.Vertex();
        if (nullptr == vertex)
          continue;
        for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
        {
          const MYON_SubDEdge* edge = vertex->Edge(vei);
          if (nullptr == edge)
            continue;
          edge->m_status = MYON_ComponentStatus::AllSet;
        }
        for (unsigned short vfi = 0; vfi < vertex->m_face_count; vfi++)
        {
          const MYON_SubDFace* face = vertex->Face(vfi);
          if (nullptr == face)
            continue;
          face->m_status = MYON_ComponentStatus::AllSet;
        }
      }
      break;

    case MYON_SubDComponentPtr::Type::Edge:
      {
        const MYON_SubDEdge* edge = cptr.Edge();
        if (nullptr == edge)
          continue;
        edge->m_status = MYON_ComponentStatus::AllSet;
        for (unsigned short efi = 0; efi < edge->m_face_count; efi++)
        {
          const MYON_SubDFace* face = edge->Face(efi);
          if (nullptr == face)
            continue;
          face->m_status = MYON_ComponentStatus::AllSet;
        }
      }
      break;

    case MYON_SubDComponentPtr::Type::Face:
      {
        const MYON_SubDFace* face = cptr.Face();
        if (nullptr == face)
          continue;
        face->m_status = MYON_ComponentStatus::AllSet;
      }
      break;
            
    case MYON_SubDComponentPtr::Type::Unset:
      break;
    }
  }

  // Minimum count of what will be deleted. (
  unsigned int deleted_vertex_count = 0;
  unsigned int deleted_edge_count = 0;
  unsigned int deleted_face_count = 0;
  for (MYON_SubDComponentPtr cptr = cit.FirstComponent(); cptr.IsNotNull(); cptr = cit.NextComponent())
  {
    if (MYON_ComponentStatus::AllSet == cptr.Status())
    {
      switch (cptr.ComponentType())
      {
      case MYON_SubDComponentPtr::Type::Vertex:
        deleted_vertex_count++;
        break;

      case MYON_SubDComponentPtr::Type::Edge:
        deleted_edge_count++;
        break;

      case MYON_SubDComponentPtr::Type::Face:
        deleted_face_count++;
        break;
      case MYON_SubDComponentPtr::Type::Unset:
        break;
      }
      continue;
    }
  }

  if ( 0 == deleted_vertex_count && 0 == deleted_edge_count && 0 ==  deleted_face_count )
    return false;

  const bool bDestroy
    = deleted_vertex_count >= level->m_vertex_count
    || deleted_edge_count >= level->m_edge_count
    || (deleted_face_count >= level->m_face_count && bDeleteIsolatedEdges)
    ;
  if (bDestroy)
  {
    Destroy();
    return true;
  }

  unsigned int deleted_component_count = subdimple->DeleteComponents(level_index,bDeleteIsolatedEdges,bUpdateTagsAndCoefficients,bMarkDeletedFaceEdges);

  if (0 == subdimple->LevelCount())
  {
    Destroy();
    return true;
  }

  if (deleted_component_count > 0 || level_index > 0)
  {
    // remove lower levels
    subdimple->ClearLowerSubdivisionLevels(level_index);
  }

  return (deleted_component_count > 0);
}

bool MYON_SubD::DeleteMarkedComponents(
  bool bDeleteMarkedComponents,
  MYON__UINT8 mark_bits,
  bool bMarkDeletedFaceEdges
)
{
  MYON_SimpleArray<MYON_SubDComponentPtr> cptr_list;
  GetMarkedComponents(bDeleteMarkedComponents, mark_bits, true, true, true, cptr_list);
  return DeleteComponents(
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    bMarkDeletedFaceEdges
  );
}

bool MYON_SubD::DeleteMarkedComponentsForExperts(
  bool bDeleteMarkedComponents,
  MYON__UINT8 mark_bits,
  bool bDeleteIsolatedEdges,
  bool bUpdateTagsAndCoefficients,
  bool bMarkDeletedFaceEdges
)
{
  MYON_SimpleArray<MYON_SubDComponentPtr> cptr_list;
  GetMarkedComponents(bDeleteMarkedComponents, mark_bits, true, true, true, cptr_list);
  return DeleteComponentsForExperts(
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    bDeleteIsolatedEdges,
    bUpdateTagsAndCoefficients,
    bMarkDeletedFaceEdges
  );
}

unsigned int MYON_SubDLevel::UpdateEdgeTags(
  bool bUnsetEdgeTagsOnly
  )
{
  // Update edge flags and sector coefficients.
  unsigned int edge_change_count = 0;
  MYON_SubDEdge* next_edge = m_edge[0];
  for (MYON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast<MYON_SubDEdge*>(edge->m_next_edge);

    if (2 != edge->m_face_count && edge->IsSmooth())
    {
      // Dale Lear - Added April 5, 2021 - don't tolerate obvious errors / oversights.
      edge->m_edge_tag = MYON_SubDEdgeTag::Unset;
    }

    const MYON_SubDEdgeTag edge_tag0 = edge->m_edge_tag;
    if (bUnsetEdgeTagsOnly && MYON_SubDEdgeTag::Unset != edge_tag0 )
    {
      continue;
    }

    if (nullptr == edge->m_vertex[0] || nullptr == edge->m_vertex[1])
    {
      MYON_SUBD_ERROR("nullptr edge->m_vertex[] values");
      continue;
    }

    const double edge_sector_coefficient0[2] = { edge->m_sector_coefficient[0], edge->m_sector_coefficient[1] };

    if (2 != edge->m_face_count)
    {
      edge->m_edge_tag = MYON_SubDEdgeTag::Crease;
      edge->m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;
      edge->m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
    }
    else
    {
      edge->m_sector_coefficient[0] = MYON_SubDSectorType::UnsetSectorCoefficient;
      edge->m_sector_coefficient[1] = MYON_SubDSectorType::UnsetSectorCoefficient;
      const bool bBothVertexTagsAreSet
        =  MYON_SubDVertexTag::Unset != edge->m_vertex[0]->m_vertex_tag
        && MYON_SubDVertexTag::Unset != edge->m_vertex[1]->m_vertex_tag
        ;
      const unsigned int tagged_end_index = edge->TaggedEndIndex();
      if (0 == tagged_end_index || 1 == tagged_end_index)
        edge->m_sector_coefficient[tagged_end_index] = MYON_SubDSectorType::IgnoredSectorCoefficient;

      switch (edge_tag0)
      {
      case MYON_SubDEdgeTag::Unset:
        if (2 == tagged_end_index)
        {
          edge->m_edge_tag = MYON_SubDEdgeTag::SmoothX;
        }
        else if ( bBothVertexTagsAreSet )
        {
          edge->m_edge_tag = MYON_SubDEdgeTag::Smooth;
          if (3 == tagged_end_index)
          {
            edge->m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;
            edge->m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
          }
        }
        break;

      case MYON_SubDEdgeTag::Smooth:
        if (2 == tagged_end_index)
        {
          edge->m_edge_tag = MYON_SubDEdgeTag::SmoothX;
        }
        else if (3 == tagged_end_index && bBothVertexTagsAreSet)
        {
          edge->m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;
          edge->m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
        }
        break;

      case MYON_SubDEdgeTag::Crease:
        edge->m_sector_coefficient[0] = MYON_SubDSectorType::IgnoredSectorCoefficient;
        edge->m_sector_coefficient[1] = MYON_SubDSectorType::IgnoredSectorCoefficient;
        break;

      case MYON_SubDEdgeTag::SmoothX:
        if ( 2 != tagged_end_index && bBothVertexTagsAreSet)
          edge->m_edge_tag = MYON_SubDEdgeTag::Smooth;
        break;

      default:
        break;
      }
    }

    if (!(edge_tag0 == edge->m_edge_tag
      && edge_sector_coefficient0[0] == edge->m_sector_coefficient[0]
      && edge_sector_coefficient0[1] == edge->m_sector_coefficient[1]))
      edge_change_count++;
  }

  return edge_change_count;
}


unsigned int MYON_SubDLevel::UpdateVertexTags(
  bool bUnsetVertexTagsOnly
  )
{
  // Update edge flags and sector coefficients.
  unsigned int vertex_change_count = 0;
 

  MYON_SubDVertex* next_vertex = m_vertex[0];
  for (MYON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
  {
    next_vertex = const_cast<MYON_SubDVertex*>(vertex->m_next_vertex);

    const MYON_SubDVertexTag vertex_tag0 = vertex->m_vertex_tag;
    if (bUnsetVertexTagsOnly && MYON_SubDVertexTag::Unset != vertex_tag0 )
    {
      continue;
    }

    const MYON_SubDVertexTag vertex_tag1 = vertex->SuggestedVertexTag(true, false);
    if (MYON_SubDVertexTag::Unset == vertex_tag1)
      continue;

    if ( vertex_tag0 != vertex_tag1)
    {
      vertex->m_vertex_tag = vertex_tag1;
      vertex_change_count++;
    }
  }

  return vertex_change_count;
}


unsigned int MYON_SubDLevel::UpdateAllTagsAndSectorCoefficients(
  bool bUnsetValuesOnly
)
{
  unsigned int change_count = 0;

  bool bUpdateEdges = true;
  bool bUpdateVertices = true;
  for ( unsigned int it_count = 0; it_count < 8; it_count++)
  {
    const unsigned int edge_change_count 
      = bUpdateEdges
      ? UpdateEdgeTags(bUnsetValuesOnly)
      : 0;
    bUpdateVertices = (edge_change_count > 0 || 0 == it_count);
    change_count += edge_change_count;

    const unsigned int vertex_change_count 
      = bUpdateVertices
      ? UpdateVertexTags(bUnsetValuesOnly)
      : 0;
    bUpdateEdges = (vertex_change_count > 0);    
    change_count += vertex_change_count;

    bUpdateVertices = false;
    if ( false == bUpdateEdges)
      break;
  }

  if (bUpdateVertices && bUpdateEdges)
  {
    MYON_SUBD_ERROR("Recursion limit exceeded.");
  }

  // Adjust edge tag smooth/X settings
  // This must be done before UpdateEdgeSectorCoefficients().
  // It is done between the heavy handed setting above so as not to disturb that delicate code.
  MYON_SubDEdge* next_edge = m_edge[0];
  for (MYON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast<MYON_SubDEdge*>(edge->m_next_edge);
    if (edge->IsSmooth())
    {
      const MYON_SubDEdgeTag etag = (2 == edge->TaggedEndIndex()) ? MYON_SubDEdgeTag::SmoothX : MYON_SubDEdgeTag::Smooth;
      if (etag != edge->m_edge_tag)
      {
        edge->m_edge_tag = etag;
        ++change_count;
      }
    }
  }

  change_count += UpdateEdgeSectorCoefficients(false);

  if (change_count > 0)
  {
    m_surface_mesh = MYON_SubDMesh::Empty;
    m_control_net_mesh = MYON_SubDMesh::Empty;
  }    

  return change_count;
}

unsigned int MYON_SubDLevel::ClearComponentDamagedState() const
{
  return ClearComponentDamagedState(true, true, true);
}

unsigned int MYON_SubDLevel::ClearComponentDamagedState(
  bool bClearVerticesDamagedState,
  bool bClearEdgesDamagedState,
  bool bClearFacesDamagedState
) const
{
  unsigned int change_count = 0;
  unsigned int i;
  if (bClearVerticesDamagedState)
  {
    i = 0;
    for (const MYON_SubDVertex* v = m_vertex[0]; nullptr != v && i++ < m_vertex_count; v = v->m_next_vertex)
      if (0 != v->m_status.SetDamagedState(false))
        ++change_count;
  }
  if (bClearEdgesDamagedState)
  {
    i = 0;
    for (const MYON_SubDEdge* e = m_edge[0]; nullptr != e && i++ < m_edge_count; e = e->m_next_edge)
      if (0 != e->m_status.SetDamagedState(false))
        ++change_count;
  }
  if (bClearFacesDamagedState)
  {
    i = 0;
    for (const MYON_SubDFace* f = m_face[0]; nullptr != f && i++ < m_face_count; f = f->m_next_face)
      if (0 != f->m_status.SetDamagedState(false))
        ++change_count;
  }
  return change_count;
}


unsigned int MYON_SubD::UpdateVertexTags(
    bool bUnsetVertexTagsOnly
    )
{
  MYON_SubDLevel* level = ActiveLevelPointer();
  if ( nullptr == level )
    return MYON_SUBD_RETURN_ERROR(0);

  return level->UpdateVertexTags(bUnsetVertexTagsOnly);
}

unsigned int MYON_SubD::UpdateEdgeTags(
  bool bUnsetEdgeTagsOnly
  )
{
  MYON_SubDLevel* level = ActiveLevelPointer();
  if ( nullptr == level )
    return MYON_SUBD_RETURN_ERROR(0);

  return level->UpdateEdgeTags(bUnsetEdgeTagsOnly);
}


unsigned int MYON_SubD::UpdateEdgeSectorCoefficients(
    bool bUnsetSectorCoefficientsOnly
    ) const
{
  const MYON_SubDLevel* level = ActiveLevelConstPointer();
  if ( nullptr == level )
    return MYON_SUBD_RETURN_ERROR(0);

  return level->UpdateEdgeSectorCoefficients(bUnsetSectorCoefficientsOnly);
}

void MYON_SubD::SubDModifiedNofification()
{
  // DestroyRuntimeCache() 
  // Clears
  //  Saved subdivision points.
  //  Saved limit surface information.
  //  Bounding boxes.
  //  
  DestroyRuntimeCache();

  // This is a heavy handed tag update.
  UpdateAllTagsAndSectorCoefficients(false);
}

unsigned int MYON_SubD::UpdateAllTagsAndSectorCoefficients(
  bool bUnsetValuesOnly
)
{
  MYON_SubDLevel* level = ActiveLevelPointer();
  if ( nullptr == level )
    return MYON_SUBD_RETURN_ERROR(0);

  return level->UpdateAllTagsAndSectorCoefficients(bUnsetValuesOnly);
}

unsigned int MYON_SubDimple::DeleteComponents(
  unsigned int level_index,
  bool bDeleteIsolatedEdges,
  bool bUpdateTagsAndCoefficients,
  bool bMarkDeletedFaceEdges
  )
{ 
  unsigned int deleted_component_count = 0;

  if (level_index >= m_levels.UnsignedCount())
    return MYON_SUBD_RETURN_ERROR(0);

  MYON_SubDLevel* level = m_levels[level_index];
  if (nullptr == level)
    return MYON_SUBD_RETURN_ERROR(0);

  if (bMarkDeletedFaceEdges)
    level->ClearRuntimeMarks(false,true,false);

  MYON_SubDFace* next_face = level->m_face[0];
  for (MYON_SubDFace* face = next_face; nullptr != face; face = next_face)
  {
    next_face = const_cast< MYON_SubDFace* >(face->m_next_face);
    bool bDelete = (MYON_ComponentStatus::AllSet == face->m_status || 0 == face->m_edge_count);
    if (false == bDelete)
    {
      const MYON_SubDEdgePtr* eptr = face->m_edge4;
      for (unsigned short fei = 0; fei < face->m_edge_count && false == bDelete; ++fei, ++eptr)
      {
        if (4 == fei)
        {
          eptr = face->m_edgex;
          if (nullptr == eptr)
            break;
        }
        const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
        if (nullptr == edge
          || nullptr == edge->m_vertex[0]
          || nullptr == edge->m_vertex[1]
          || MYON_ComponentStatus::AllSet == edge->m_status
          || MYON_ComponentStatus::AllSet == edge->m_vertex[0]->m_status
          || MYON_ComponentStatus::AllSet == edge->m_vertex[1]->m_status
          )
        {
          bDelete = true;
          if (nullptr != edge && MYON_ComponentStatus::AllSet != edge->m_status)
            edge->m_status = MYON_ComponentStatus::AllSet;
        }
      }
      if (false == bDelete)
        continue;
    }

    if (bMarkDeletedFaceEdges)
    {
      // Set runtime mark on face's boundary edges.
      const MYON_SubDEdgePtr* eptr = face->m_edge4;
      for (unsigned short fei = 0; fei < face->m_edge_count ; ++fei, ++eptr)
      {
        if (4 == fei)
        {
          eptr = face->m_edgex;
          if (nullptr == eptr)
            break;
        }
        const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
        if (nullptr != edge)
          edge->m_status.SetRuntimeMark();
      }
    }

    level->RemoveFace(face);
    m_heap.ReturnFace(face);
    deleted_component_count++;
  }

  MYON_ComponentStatus allsetcheck;
  MYON_SubDEdge* next_edge = level->m_edge[0];
  for (MYON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast< MYON_SubDEdge* >(edge->m_next_edge);
    allsetcheck = MYON_ComponentStatus::LogicalAnd(MYON_ComponentStatus::AllSet, edge->m_status);
    bool bDelete = (MYON_ComponentStatus::AllSet == allsetcheck || (bDeleteIsolatedEdges && 0 == edge->m_face_count) );
    if (false == bDelete)
    {
      for (unsigned short evi = 0; evi < 2 && false == bDelete; evi++)
      {
        if (nullptr == edge->m_vertex[0]
          || nullptr == edge->m_vertex[1]
          || MYON_ComponentStatus::AllSet == edge->m_vertex[0]->m_status
          || MYON_ComponentStatus::AllSet == edge->m_vertex[1]->m_status
          )
          bDelete = true;
      }
      if (false == bDelete)
        continue;
    }

    level->RemoveEdge(edge);
    m_heap.ReturnEdge(edge);
    deleted_component_count++;
  }

  MYON_SubDVertex* next_vertex = level->m_vertex[0];
  for (MYON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
  {
    next_vertex = const_cast<MYON_SubDVertex*>(vertex->m_next_vertex);
    allsetcheck = MYON_ComponentStatus::LogicalAnd(MYON_ComponentStatus::AllSet, vertex->m_status);
    bool bDelete = (MYON_ComponentStatus::AllSet == allsetcheck || (bDeleteIsolatedEdges && 0 == vertex->m_face_count) || 0 == vertex->m_edge_count );
    if ( false == bDelete )
      continue;

    level->RemoveVertex(vertex);
    m_heap.ReturnVertex(vertex);
    deleted_component_count++;
  }

  if ( 0 == deleted_component_count )
    return 0;

  // Remove edge references to deleted faces
  next_edge = level->m_edge[0];
  for (MYON_SubDEdge* edge = next_edge; nullptr != edge; edge = next_edge)
  {
    next_edge = const_cast<MYON_SubDEdge*>(edge->m_next_edge);
    MYON_SubDFacePtr* fptr0 = edge->m_face2;
    MYON_SubDFacePtr* fptr1 = edge->m_face2;
    const unsigned short edge_face_count = edge->m_face_count;
    edge->m_face_count = 0;
    for (unsigned short efi = 0; efi < edge_face_count; efi++, fptr0++)
    {
      if (2 == efi)
        fptr0 = edge->m_facex;
      const MYON_SubDFace* face = fptr0->Face();
      if (nullptr == face || MYON_UNSET_UINT_INDEX == face->ArchiveId())
        continue;
      *fptr1++ = *fptr0;
      edge->m_face_count++;
      if (2 == edge->m_face_count)
        fptr1 = edge->m_facex;
    }

    if (0 == edge->m_face_count && bDeleteIsolatedEdges)
    {
      level->RemoveEdge(edge);
      m_heap.ReturnEdge(edge);
      deleted_component_count++;
      continue;
    }
    
    if (edge->m_face_count <= 2 && nullptr != edge->m_facex)
      m_heap.ReturnEdgeExtraArray(edge);

    if (bUpdateTagsAndCoefficients)
    {
      if (edge->m_face_count != 2)
        edge->m_edge_tag = MYON_SubDEdgeTag::Crease;

      edge->m_sector_coefficient[0] = MYON_SubDSectorType::UnsetSectorCoefficient;
      edge->m_sector_coefficient[1] = MYON_SubDSectorType::UnsetSectorCoefficient;
    }
  }

  // Remove vertex references to deleted edges and faces
  next_vertex = level->m_vertex[0];
  for (MYON_SubDVertex* vertex = next_vertex; nullptr != vertex; vertex = next_vertex)
  {
    next_vertex = const_cast<MYON_SubDVertex*>(vertex->m_next_vertex);

    unsigned int count = vertex->m_edge_count;
    vertex->m_edge_count = 0;

    bool bInteriorVertex = true;
    unsigned int crease_count = 0;
    for (unsigned short vei = 0; vei < count; vei++)
    {
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
      if (nullptr == edge || MYON_UNSET_UINT_INDEX == edge->ArchiveId())
      {
        bInteriorVertex = false;
        continue;
      }
      if (edge->IsCrease())
        crease_count++;
      if (2 != edge->m_face_count)
        bInteriorVertex = false;
      vertex->m_edges[vertex->m_edge_count++] = vertex->m_edges[vei];
    }


    count = vertex->m_face_count;
    vertex->m_face_count = 0;
    for (unsigned short vfi = 0; vfi < count; vfi++)
    {
      const MYON_SubDFace* face = vertex->m_faces[vfi];
      if (nullptr == face || MYON_UNSET_UINT_INDEX == face->ArchiveId()) 
        continue;
      vertex->m_faces[vertex->m_face_count++] = vertex->m_faces[vfi];
    }

    if (0 == vertex->m_face_count && 0 == vertex->m_edge_count)
    {
      level->RemoveVertex(vertex);
      m_heap.ReturnVertex(vertex);
      deleted_component_count++;
    }
    else
    {
      if (1 == crease_count && 1 == vertex->m_edge_count && 0 == vertex->m_face_count)
        vertex->m_vertex_tag = MYON_SubDVertexTag::Corner;
      else if (crease_count > 2)
        vertex->m_vertex_tag = MYON_SubDVertexTag::Corner;
      else if (false == bInteriorVertex || crease_count > 1)
      {
        if (false == vertex->IsCreaseOrCorner())
          vertex->m_vertex_tag = MYON_SubDVertexTag::Crease;
      }
    }
  }

  if (0 == level->m_vertex_count || 0 == level->m_edge_count || (bDeleteIsolatedEdges && 0 == level->m_face_count))
  {
    Destroy();
  }
  else
  {
    // remove all information that is no longer valid
    level->MarkAggregateComponentStatusAsNotCurrent();
    level->ClearEvaluationCache();

    ClearHigherSubdivisionLevels(level_index);

    if (bUpdateTagsAndCoefficients)
    {
      // Update vertex tags, edge tags, and sector coefficients.
      level->UpdateAllTagsAndSectorCoefficients(false);
    }
  }

  ChangeGeometryContentSerialNumber(false);

  return deleted_component_count;
}



unsigned int MYON_SubD::ClearComponentMarks() const
{
  return ClearComponentMarks(true, true, true, nullptr);
}

unsigned int MYON_SubD::ClearVertexMarks() const
{
  return ClearComponentMarks(true, false, false, nullptr);
}

unsigned int MYON_SubD::ClearEdgeMarks() const
{
  return ClearComponentMarks(false, true, false, nullptr);
}

unsigned int MYON_SubD::ClearFaceMarks() const
{
  return ClearComponentMarks(false, false, true, nullptr);
}


unsigned int MYON_SubD::ClearComponentMarks(
  bool bClearVertexMarks,
  bool bClearEdgeMarks,
  bool bClearFaceMarks,
  MYON_SimpleArray< const MYON_SubDComponentBase* >* marked_component_list
) const
{
  unsigned int clear_count = 0;

  if (bClearVertexMarks)
  {
    MYON_SubDVertexIterator vit(*this);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (v->m_status.ClearRuntimeMark())
      {
        if (nullptr != marked_component_list)
          marked_component_list->Append(v);
        clear_count++;
      }
    }
  }

  if (bClearEdgeMarks)
  {
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (e->m_status.ClearRuntimeMark())
      {
        if (nullptr != marked_component_list)
          marked_component_list->Append(e);
        clear_count++;
      }
    }
  }

  if (bClearFaceMarks)
  {
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (f->m_status.ClearRuntimeMark())
      {
        if (nullptr != marked_component_list)
          marked_component_list->Append(f);
        clear_count++;
      }
    }
  }

  return clear_count;
}

unsigned int MYON_SubD::ClearGroupIds() const
{
  return ClearComponentGroupIds(true, true, true);
}

unsigned int MYON_SubD::ClearVertexGroupIds() const
{
  return ClearComponentGroupIds(true, false, false);
}

unsigned int MYON_SubD::ClearEdgeGroupIds() const
{
  return ClearComponentGroupIds(false, true, false);
}

unsigned int MYON_SubD::ClearFaceGroupIds() const
{
  return ClearComponentGroupIds(false, false, true);
}

unsigned int MYON_SubD::ClearComponentGroupIds(
  bool bClearVertexGroupIds,
  bool bClearEdgeGroupIds,
  bool bClearFaceGroupIds
) const
{
  unsigned int clear_count = 0;

  if (bClearVertexGroupIds)
  {
    MYON_SubDVertexIterator vit(*this);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (0 != v->m_group_id)
      {
        v->m_group_id = 0;
        clear_count++;
      }
    }
  }

  if (bClearEdgeGroupIds)
  {
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (0 != e->m_group_id)
      {
        e->m_group_id = 0;
        clear_count++;
      }
    }
  }

  if (bClearFaceGroupIds)
  {
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (0 != f->m_group_id)
      {
        f->m_group_id = 0;
        clear_count++;
      }
    }
  }

  return clear_count;
}


unsigned int MYON_SubD::ClearMarkBits() const
{
  return ClearComponentMarkBits(true, true, true);
}

unsigned int MYON_SubD::ClearVertexMarkBits() const
{
  return ClearComponentMarkBits(true, false, false);
}

unsigned int MYON_SubD::ClearEdgeMarkBits() const
{
  return ClearComponentMarkBits(false, true, false);
}

unsigned int MYON_SubD::ClearFaceMarkBits() const
{
  return ClearComponentMarkBits(false, false, true);
}

unsigned int MYON_SubD::ClearComponentMarkBits(
  bool bClearVertexMarkBits,
  bool bClearEdgeMarkBits,
  bool bClearFaceMarkBits
) const
{
  unsigned int clear_count = 0;

  if (bClearVertexMarkBits)
  {
    MYON_SubDVertexIterator vit(*this);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (0 != v->m_status.MarkBits())
      {
        v->m_status.SetMarkBits(0);
        clear_count++;
      }
    }
  }

  if (bClearEdgeMarkBits)
  {
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (0 != e->m_status.MarkBits())
      {
        e->m_status.SetMarkBits(0);
        clear_count++;
      }
    }
  }

  if (bClearFaceMarkBits)
  {
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (0 != f->m_status.MarkBits())
      {
        f->m_status.SetMarkBits(0);
        clear_count++;
      }
    }
  }

  return clear_count;
}

unsigned int MYON_SubD::UnselectComponents(
    bool bUnselectAllVertices,
    bool bUnselectAllEdges,
    bool bUnselectAllFaces
  ) const
{
  unsigned int unselected_count = 0;

  if (bUnselectAllVertices)
  {
    MYON_SubDVertexIterator vit(*this);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (v->m_status.SetSelectedState(false, false, false))
        ++unselected_count;
    }
  }

  if (bUnselectAllEdges)
  {
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (e->m_status.SetSelectedState(false, false, false))
        ++unselected_count;
    }
  }

  if (bUnselectAllFaces)
  {
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (f->m_status.SetSelectedState(false, false, false))
        ++unselected_count;
    }
  }

  return unselected_count;
}


unsigned int MYON_SubD::SetComponentMarks(
  bool bClearBeforeSet,
  const MYON_SimpleArray< const MYON_SubDComponentBase* >& marked_component_list
) const
{
  unsigned int set_count = 0;

  if (bClearBeforeSet)
    ClearComponentMarks(true, true, true, nullptr);

  const unsigned count = marked_component_list.Count();
  if (count <= 0)
    return 0;
  const MYON_SubDComponentBase*const* a = marked_component_list.Array();
  if (nullptr == a)
    return 0;

  for (const MYON_SubDComponentBase*const* a1 = a + count; a < a1; a++)
  {
    const MYON_SubDComponentBase* c = *a;
    if (nullptr == c)
      continue;
    if (c->m_status.SetRuntimeMark())
      set_count++;
  }

  return set_count;
}

unsigned int MYON_SubD::GetMarkedComponents(
  bool bAddMarkedComponents,
  MYON__UINT8 mark_bits,
  bool bIncludeVertices,
  bool bIncludeEdges,
  bool bIncludeFaces,
  MYON_SimpleArray< MYON_SubDComponentPtr >& component_list
) const
{
  bAddMarkedComponents = bAddMarkedComponents ? true : false; // so we can use == on boolean values
  unsigned int mark_count = 0;

  if (bIncludeVertices)
  {
    MYON_SubDVertexIterator vit(*this);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (bAddMarkedComponents == v->m_status.IsMarked(mark_bits))
      {
        component_list.Append(v->ComponentPtr());
        mark_count++;
      }
    }
  }

  if (bIncludeEdges)
  {
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (bAddMarkedComponents == e->m_status.IsMarked(mark_bits))
      {
        component_list.Append(e->ComponentPtr());
        mark_count++;
      }
    }
  }

  if (bIncludeFaces)
  {
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (bAddMarkedComponents == f->m_status.IsMarked(mark_bits))
      {
        component_list.Append(f->ComponentPtr());
        mark_count++;
      }
    }
  }

  return mark_count;
}

unsigned int MYON_SubD::GetMarkedComponents(
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
  MYON_SimpleArray< const MYON_SubDComponentBase* >& marked_component_list
) const
{
  unsigned int mark_count = 0;

  if (bIncludeVertices)
  {
    MYON_SubDVertexIterator vit(*this);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (v->m_status.RuntimeMark())
      {
        marked_component_list.Append(v);
        mark_count++;
      }
    }
  }

  if (bIncludeEdges)
  {
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (e->m_status.RuntimeMark())
      {
        marked_component_list.Append(e);
        mark_count++;
      }
    }
  }

  if (bIncludeFaces)
  {
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (f->m_status.RuntimeMark())
      {
        marked_component_list.Append(f);
        mark_count++;
      }
    }
  }

  return mark_count;
}

unsigned int MYON_SubD::GetComponentStatus(
  bool bGetVertexStatus,
  bool bGetEdgeStatus,
  bool bGetFaceStatus,
  bool bClearStatus,
  MYON_ComponentStatus status_mask,
  MYON_SimpleArray< const class MYON_SubDComponentBase* >& component_list,
  MYON_SimpleArray< MYON_ComponentStatus >& status_list
) const
{
  component_list.SetCount(0);
  status_list.SetCount(0);
  if ( MYON_ComponentStatus::NoneSet == status_mask )
    return 0;

  MYON_ComponentStatus s;

  if (bGetVertexStatus)
  {
    MYON_SubDVertexIterator vit(*this);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      s = MYON_ComponentStatus::LogicalAnd(status_mask, v->m_status);
      if (MYON_ComponentStatus::NoneSet == s)
        continue;
      component_list.Append(v);
      status_list.Append(s);
    }
  }

  if (bGetEdgeStatus)
  {
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      s = MYON_ComponentStatus::LogicalAnd(status_mask, e->m_status);
      if (MYON_ComponentStatus::NoneSet == s)
        continue;
      component_list.Append(e);
      status_list.Append(s);
    }
  }

  if (bGetFaceStatus)
  {
    MYON_SubDFaceIterator fit(*this);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      s = MYON_ComponentStatus::LogicalAnd(status_mask, f->m_status);
      if (MYON_ComponentStatus::NoneSet == s)
        continue;
      component_list.Append(f);
      status_list.Append(s);
    }
  }

  const unsigned int count = component_list.UnsignedCount();
  if (bClearStatus && count > 0)
  {
    const bool bRuntimeMark = status_mask.RuntimeMark();
    for (unsigned int i = 0; i < count; ++i)
    {
      const MYON_SubDComponentBase* c = component_list[i];
      if (nullptr == c)
        continue;
      c->m_status.ClearStates(status_mask);
      if (bRuntimeMark)
        c->m_status.ClearRuntimeMark();
    }
  }

  return count;
}

unsigned int MYON_SubD::SetComponentStatus(
  MYON_ComponentStatus status_mask,
  const MYON_SimpleArray< const class MYON_SubDComponentBase* >& component_list,
  const MYON_SimpleArray< MYON_ComponentStatus >& status_list
) const
{
  const unsigned int count = component_list.UnsignedCount();
  if (count < 1 || count != status_list.UnsignedCount())
    return 0;

  const bool bRuntimeMark = status_mask.RuntimeMark();

  for (unsigned int i = 0; i < count; ++i)
  {
    const MYON_SubDComponentBase* c = component_list[i];
    if (nullptr == c)
      continue;
    const MYON_ComponentStatus s = status_list[i];
    c->m_status.ClearStates(status_mask);
    c->m_status.SetStates(s);
    if (bRuntimeMark)
    {
      if ( s.RuntimeMark())
        c->m_status.SetRuntimeMark();
      else
        c->m_status.ClearRuntimeMark();
    }
  }

  return count;
}

MYON_SubDComponentMarksClearAndRestore::MYON_SubDComponentMarksClearAndRestore(
  const MYON_SubD& subd
)  
{
  m_subd.ShareDimple(subd);
  m_subd.ClearComponentMarks(true, true, true, &m_component_list);
}

MYON_SubDComponentMarksClearAndRestore::~MYON_SubDComponentMarksClearAndRestore()
{
  Restore(true);
}

const MYON_SimpleArray<const class MYON_SubDComponentBase*>& MYON_SubDComponentMarksClearAndRestore::ComponentList() const
{
  return m_component_list;
}

bool MYON_SubDComponentMarksClearAndRestore::Restore(
  bool bDisableFutureRestore )
{
  const bool rc = m_bRestore;
  if (rc)
  {
    if ( bDisableFutureRestore)
      m_bRestore = false;
    
    m_subd.ClearComponentMarks(true, true, true, nullptr);

//    if ( MYON_ComponentStatus::Marked == m_status_mask )
    {
      // RuntimeMark is the only bit being managed
      if (m_component_list.UnsignedCount() > 0)
        m_subd.SetComponentMarks(false, m_component_list);
    }
    //else if ( m_status_mask.IsNotClear() )
    //{
    //  // something fancier is going on

    //  // clear current settings
    //  MYON_SubDVertexIterator vit(m_subd);
    //  for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    //    v->m_status.ClearStates(m_status_mask);

    //  MYON_SubDEdgeIterator eit(m_subd);
    //  for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    //    e->m_status.ClearStates(m_status_mask);

    //  MYON_SubDFaceIterator fit(m_subd);
    //  for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    //    f->m_status.ClearStates(m_status_mask);

    //  // restore settings
    //  m_subd.SetComponentStatus(m_status_mask, m_component_list, m_status_list);
    //}

    if (bDisableFutureRestore)
    {
      m_component_list.Destroy();
      //m_status_list.Destroy();
    }
  }
  return rc;
}

void MYON_SubDComponentMarksClearAndRestore::DisableRestore()
{
  m_bRestore = false;
}

unsigned int MYON_SubD::TransformComponents(
  const MYON_Xform& xform,
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  MYON_SubDComponentLocation component_location
)
{
  if (
    false ==  xform.IsValidAndNotZeroAndNotIdentity()
    || ci_count <= 0
    || nullptr == ci_list
    )
    return 0;

  MYON_SimpleArray<MYON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return true; // nothing to delete

  return TransformComponents(xform,cptr_list.Array(),cptr_list.UnsignedCount(),component_location);
}



static unsigned int Internal_MarkStuffAndMaybeMoveVertices(
  const MYON_SubD& subd,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  const MYON_Xform* xform,
  MYON_SubDComponentLocation component_location,
  bool bExtrusionMarking,
  bool bExtrudeBoundaries,
  unsigned int& list_vertex_count,
  unsigned int& list_edge_count,
  unsigned int& list_face_count
)
{
  list_vertex_count = 0;
  list_edge_count = 0;
  list_face_count = 0;

  if (false == bExtrusionMarking)
    bExtrudeBoundaries = false;

  const bool bTransform 
    = false == bExtrusionMarking
    && nullptr != xform 
    && xform->IsValidAndNotZeroAndNotIdentity()
    ;

  if ((bTransform ? 1 : 0) == (bExtrusionMarking ? 1 : 0))
  {
    MYON_SUBD_ERROR("Invalid input.");
    return 0;
  }

  unsigned int marked_vertex_count = 0;
  //unsigned int potential_isolated_vertex_count = 0;
  unsigned int potential_isolated_edge_count = 0;

  MYON_SimpleArray<unsigned int> moved_vertices;

  if (bExtrusionMarking && 0 == cptr_count && nullptr == cptr_list)
  {
    // entire subd is being extruded
    MYON_SubDFaceIterator fit(subd);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      ++list_face_count;
      f->m_status.SetRuntimeMark();
      const unsigned int face_vertex_count = f->m_edge_count;
      for (unsigned int fvi = 0; fvi < face_vertex_count; ++fvi)
      {
        // used when extruding selected components
        const MYON_SubDEdge* e = f->Edge(fvi);
        if (nullptr != e && false == e->m_status.RuntimeMark())
          e->m_status.SetRuntimeMark();
        const MYON_SubDVertex* v = f->Vertex(fvi);
        if (nullptr != v && false == v->m_status.RuntimeMark())
        {
          v->m_status.SetRuntimeMark();
          ++marked_vertex_count;
        }
      }
    }
  }
  else
  {
    for (size_t i = 0; i < cptr_count; i++)
    {
      switch (cptr_list[i].ComponentType())
      {
      case MYON_SubDComponentPtr::Type::Vertex:
        {
          const MYON_SubDVertex* v = cptr_list[i].Vertex();
          if (nullptr == v)
            continue;
          ++list_vertex_count;
          if (v->m_status.RuntimeMark())
            continue;
          if (bTransform)
          {
            v->m_status.SetRuntimeMark();
            if (bTransform)
            {
              const_cast<MYON_SubDVertex*>(v)->Transform(false, *xform);
              moved_vertices.Append(v->m_id);
            }
            ++marked_vertex_count;
          }
        }
        break;

      case MYON_SubDComponentPtr::Type::Edge:
        {
          const MYON_SubDEdge* e = cptr_list[i].Edge();
          if (nullptr == e)
            continue;
          ++list_edge_count;
          if (e->Mark())
            continue;
          if (bTransform)
          {
            e->SetMark();
            for (unsigned int evi = 0; evi < 2; ++evi)
            {
              const MYON_SubDVertex* v = e->m_vertex[evi];
              if (nullptr != v && false == v->Mark())
              {
                v->SetMark();
                const_cast<MYON_SubDVertex*>(v)->Transform(false, *xform);
                moved_vertices.Append(v->m_id);
                ++marked_vertex_count;
              }
            }
          }
          else
          {
            if (bExtrudeBoundaries && 1 == e->m_face_count && nullptr != e->m_face2[0].Face())
            {
              // It's a boundary edge and we will mark it and its vertices later after we make sure
              // a face touching this edge isn't in the cptr_list[].
              ++potential_isolated_edge_count;
            }
            // otherwise ignore interior edges
          }
        }
        break;

      case MYON_SubDComponentPtr::Type::Face:
        {
          const MYON_SubDFace* f = cptr_list[i].Face();
          if (nullptr != f && false == f->Mark())
          {
            ++list_face_count;
            f->SetMark();
            const unsigned int face_vertex_count = f->m_edge_count;
            for (unsigned int fvi = 0; fvi < face_vertex_count; ++fvi)
            {
              if (bExtrusionMarking)
              {
                // used when extruding selected components
                const MYON_SubDEdge* e = f->Edge(fvi);
                if (nullptr != e && false == e->m_status.RuntimeMark())
                  e->m_status.SetRuntimeMark();
              }
              const MYON_SubDVertex* v = f->Vertex(fvi);
              if (nullptr != v && false == v->m_status.RuntimeMark())
              {
                v->m_status.SetRuntimeMark();
                if (bTransform)
                {
                  const_cast<MYON_SubDVertex*>(v)->Transform(false, *xform);
                  moved_vertices.Append(v->m_id);
                }
                ++marked_vertex_count;
              }
            }
          }
        }
        break;
              
      case MYON_SubDComponentPtr::Type::Unset:
        break;
      }
    }

    if (bExtrusionMarking && potential_isolated_edge_count > 0)
    {
      for (size_t i = 0; i < cptr_count; i++)
      {
        if (MYON_SubDComponentPtr::Type::Edge != cptr_list[i].ComponentType())
          continue;

        const MYON_SubDEdge* e = cptr_list[i].Edge();
        if (nullptr == e)
          continue;

        if (e->Mark())
          continue; // this edge us part of a boundary belonging to a face in  cptr_list[]

        if (1 == e->m_face_count && nullptr != e->m_face2[0].Face())
        {
          // this boundary edge was explicitly picked its attached face was not picked.
          // It will be extruded to a face.
          e->SetMark();
          for (unsigned int evi = 0; evi < 2; ++evi)
          {
            const MYON_SubDVertex* v = e->m_vertex[evi];
            if (nullptr != v && false == v->Mark())
            {
              v->SetMark();
              ++marked_vertex_count;
            }
          }
          if (0 == --potential_isolated_edge_count)
            break;
        }
      }
    }
  }

  const bool bSymmetryIsSet =
    false
    ;

  bool bChangePreservesSymmetry = false;


  if (bTransform)
  {
    if  ( bSymmetryIsSet || 3 * marked_vertex_count >= subd.VertexCount() )
    {
      subd.ClearEvaluationCache();
    }
    else
    {
      MYON_SubDVertexIterator vit(subd);
      for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
      {
        if (v->Mark())
          v->VertexModifiedNofification();
      }
      subd.UpdateEdgeSectorCoefficients(true);
    }
    const_cast<MYON_SubD&>(subd).ChangeGeometryContentSerialNumberForExperts(bChangePreservesSymmetry);
  }

  return marked_vertex_count;
}

static unsigned int Internal_MarkExtrudeComponents(
  const MYON_SubD& subd,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bExtrudeBoundaries,
  unsigned int& list_vertex_count,
  unsigned int& list_edge_count,
  unsigned int& list_face_count
)
{
  const bool bExtrusionMarking = true;
  const int marked_vertex_count = Internal_MarkStuffAndMaybeMoveVertices(
    subd,
    cptr_list,
    cptr_count,
    nullptr,
    MYON_SubDComponentLocation::Unset,
    bExtrusionMarking,
    bExtrudeBoundaries,
    list_vertex_count,
    list_edge_count,
    list_face_count
  );

  // It appears the best "hurestic" is to require the user to pick edges and faces.
  // isolated vertices will be ignored and no attempts to guess if a user wants to
  // extrude the boundary of a face (all its edges) or the face itself.

  // lots of delete "hurestic" code here :)

  return marked_vertex_count;
}

static unsigned int Internal_TransformComponents(
  const MYON_SubD& subd,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  const MYON_Xform& xform,
  MYON_SubDComponentLocation component_location
)
{
  // This version is used by SDK tools that simply transform components in cptr_list
  if (false == xform.IsValidAndNotZeroAndNotIdentity())
    return 0;

  const bool bExtrusionMarking = false;
  unsigned int list_vertex_count = 0;
  unsigned int list_edge_count = 0;
  unsigned int list_face_count = 0;

  return Internal_MarkStuffAndMaybeMoveVertices(
    subd,
    cptr_list,
    cptr_count,
    &xform,
    component_location,
    bExtrusionMarking, false,
    list_vertex_count,
    list_edge_count,
    list_face_count
  );
}

unsigned int MYON_SubD::TransformComponents(
  const MYON_Xform& xform,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  MYON_SubDComponentLocation component_location
)
{
  if ( 
    false == xform.IsValidAndNotZeroAndNotIdentity()
    || cptr_count <= 0
    || nullptr == cptr_list
    )
    return 0;

  MYON_SimpleArray<const MYON_SubDComponentBase*> marked_components;
  const bool bRestoreMarks = ClearComponentMarks(true, true, true, &marked_components) > 0;

  const unsigned int v_count = Internal_TransformComponents(*this, cptr_list, cptr_count, xform, component_location);

  if (bRestoreMarks)
    SetComponentMarks(true, marked_components);

  return (v_count > 0);
}


unsigned int MYON_SubD::ExtrudeComponents(
  const MYON_Xform& xform,
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count
)
{
  const bool bExtrudeBoundaries = true;
  const bool bPermitNonManifoldEdgeCreation = false;
  return ExtrudeComponents(xform, ci_list, ci_count, bExtrudeBoundaries, bPermitNonManifoldEdgeCreation);
}


unsigned int MYON_SubD::ExtrudeComponents(
  const MYON_Xform& xform,
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  bool bExtrudeBoundaries,
  bool bPermitNonManifoldEdgeCreation
)
{
  if ( 
    false == xform.IsValidAndNotZeroAndNotIdentity()
    || xform.IsIdentity()
    || ci_count <= 0
    || nullptr == ci_list
    )
    return 0;

  MYON_SimpleArray<MYON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return true; // nothing to extrude

  return ExtrudeComponents(
    xform,
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    bExtrudeBoundaries,
    bPermitNonManifoldEdgeCreation
    );
}

/*
MYON_Internal_ExtrudedVertex manages a vertex that is extruded into a "side" edge during the extrusion process.
*/
class MYON_Internal_ExtrudedVertex
{
public:
  MYON_Internal_ExtrudedVertex() = default;
  ~MYON_Internal_ExtrudedVertex() = default;
  MYON_Internal_ExtrudedVertex(const MYON_Internal_ExtrudedVertex&) = default;
  MYON_Internal_ExtrudedVertex& operator=(const MYON_Internal_ExtrudedVertex&) = default;

  static const MYON_Internal_ExtrudedVertex Unset;

  /////////////////////////////////////////////////////////////
  //
  // Up to 2 extruded edges that share this extruded vertex.
  // In rare cases there may be more. This information is used only
  // to speed up the setting of m_side_group_id.
  // 
  class MYON_Internal_ExtrudedEdge* m_extruded_edges[2] = {};
  unsigned char m_extruded_edges_count = 0; // 0, 1, 2, or 3. 3 means 3 or more

  /////////////////////////////////////////////////////////////
  //
  // State of the vertex before the extrusion was applied
  // 

  // tag the original vertex had before the extrusion was performed.
  MYON_SubDVertexTag m_initial_vertex_tag = MYON_SubDVertexTag::Unset;

  // in complicated cases when the initial vertex is a crease/corner, m_connecting_edge_tag may be set to crease.
  MYON_SubDEdgeTag m_connecting_edge_tag = MYON_SubDEdgeTag::Unset;

  // id of the original vertex - used to sort and search an array of MYON_Internal_ExtrudedVertex elements.
  unsigned int m_initial_vertex_id = 0;

  //// The face counts do not include the side faces created during the extrusion process.
  //unsigned short m_moved_face_count = 0;
  //unsigned short m_stationary_face_count = 0;

  //// Edge counts do not include the side edge "m_connecting_edge" that is created during the extrusion process.
  //// There are three sets of edges
  ////   Existing edges that will be extruded into side faces.
  ////   Existing edges that will be moved during the extrusion process.
  ////   Existing edges that will be stationary during the extrusion process.
  //unsigned short m_extruded_edge_count = 0; // these are edges in the new_sides[] array
  //unsigned short m_wire_edge_count = 0; // moved or stationary
  //unsigned short m_moved_crease_edge_count = 0;
  //unsigned short m_moved_smooth_edge_count = 0;
  //unsigned short m_stationary_crease_edge_count = 0;
  //unsigned short m_stationary_smooth_edge_count = 0;
  
  /////////////////////////////////////////////////////////////
  //
  // Additional information that will be created or modified during the extrusion.
  // 

  // Used to sort the sides into groups of edges that are connected
  unsigned m_side_group_id = 0;

  // This is the vertex in the original object. (This vertex is moved).
  MYON_SubDVertex* m_original_vertex = nullptr;

  // This vertex is new. It is a copy of the original vertex and remains at the original location.
  MYON_SubDVertex* m_copied_vertex = nullptr;

  // m_connecting_edge begins at m_copied_vertex and terminates at m_original_vertex;
  MYON_SubDEdge* m_connecting_edge = nullptr;
  

  bool SetFromInitialVertex(MYON_SubDVertex* initial_vertex)
  {
    *this = MYON_Internal_ExtrudedVertex::Unset;

    if ( nullptr == initial_vertex || 0 == initial_vertex->m_id )
      return false;

    // validate initial_vertex topology information so subsequent code assumptions are met.
    if (initial_vertex->m_edge_count <= 0)
      return false;
    if (initial_vertex->m_edge_count > initial_vertex->m_edge_capacity)
      return false;
    if (nullptr == initial_vertex->m_edges)
      return false;
    for (unsigned short vei = 0; vei < initial_vertex->m_edge_count; ++vei)
    {
      const MYON_SubDEdgePtr eptr = initial_vertex->m_edges[vei];
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
      if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
        return false;
      if (e->m_vertex[0]->m_id == e->m_vertex[1]->m_id)
        return false;
      if (initial_vertex != e->m_vertex[MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr)])
        return false;
    }

    if (initial_vertex->m_face_count > 0)
    {
      if (initial_vertex->m_face_count > initial_vertex->m_face_capacity)
        return false;
      if (nullptr == initial_vertex->m_faces)
        return false;
    }

    m_initial_vertex_tag = initial_vertex->m_vertex_tag;
    m_initial_vertex_id = initial_vertex->m_id;
    m_original_vertex = initial_vertex;

    return true;
  }

  bool SetConnectingEdgeTag();  

  bool ExtrudeVertex(MYON_SubD& subd, bool bIsInset, const MYON_Xform& xform)
  {
    if (nullptr != m_copied_vertex)
      return MYON_SUBD_RETURN_ERROR(false);

    if (nullptr == m_original_vertex)
      return MYON_SUBD_RETURN_ERROR(false);

    const MYON_3dPoint P = m_original_vertex->ControlNetPoint();
    for (;;)
    {
      // transform the original vertex
      m_original_vertex->m_vertex_tag = MYON_SubDVertexTag::Unset;

      if (false == bIsInset)
      {
        if (false == m_original_vertex->Transform(false, xform))
          break;
      }

      m_copied_vertex = subd.AddVertex(MYON_SubDVertexTag::Unset, P);
      if (nullptr == m_copied_vertex)
        break;

      const unsigned short face_count = m_original_vertex->m_face_count;
      if (face_count > 0 && false == subd.GrowVertexFaceArray(m_copied_vertex, face_count))
        break;

      if (false == subd.GrowVertexEdgeArray(m_copied_vertex, m_original_vertex->m_edge_count + 1))
        break;

      // edge from m_copied_vertex (stationary) to m_original_vertex (moved).
      m_connecting_edge = subd.AddEdge(m_connecting_edge_tag, m_copied_vertex, m_original_vertex);
      if (nullptr == m_connecting_edge)
        break;

      //if (bMarkedInteriorCrease && bUnmarkedInteriorCrease)
      //  m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::Crease;

      for (unsigned short vei = 0; vei < m_original_vertex->m_edge_count; ++vei)
      {
        MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_original_vertex->m_edges[vei].m_ptr);
        if (nullptr != e)
          e->UnsetSectorCoefficientsForExperts();
      }

      return true;
    }

    if (nullptr != m_copied_vertex)
    {
      MYON_SubDComponentPtr cptr = m_copied_vertex->ComponentPtr();
      subd.DeleteComponentsForExperts(&cptr, 1, false, false, false);
      m_copied_vertex = nullptr;
    }

    MYON_SUBD_ERROR("Unable to extrude m_original_vertex");
    m_original_vertex->m_vertex_tag = this->m_initial_vertex_tag;
    m_original_vertex->SetControlNetPoint(P,false);
    return false;
  }

  bool AttachUnmarkedFacesToCopiedVertex()
  {
    if (nullptr == m_copied_vertex)
      return MYON_SUBD_RETURN_ERROR(false);
    if (0 != m_copied_vertex->m_face_count)
      return MYON_SUBD_RETURN_ERROR(false);
    if (nullptr == m_original_vertex)
      return MYON_SUBD_RETURN_ERROR(false);
    if (0 == m_original_vertex->m_face_count)
      return true; // wire edge case

    const unsigned short face_count = m_original_vertex->m_face_count;

    if (face_count > m_original_vertex->m_face_capacity)
      return MYON_SUBD_RETURN_ERROR(false);
    if (face_count > m_copied_vertex->m_face_capacity)
      return MYON_SUBD_RETURN_ERROR(false);

    // marked faces remain attached to m_original_vertex
    // unmarked faces are attached to m_copied_vertex 
    m_original_vertex->m_face_count = 0;
    for (unsigned short vfi = 0; vfi < face_count; vfi++)
    {
      const MYON_SubDFace* f = m_original_vertex->m_faces[vfi];
      if (nullptr == f)
        continue;
      MYON_SubDVertex* v
        = (f->Mark())
        ? m_original_vertex
        : m_copied_vertex;
      v->m_faces[v->m_face_count] = f;
      v->m_face_count++;
    }
    for (unsigned short vfi = m_original_vertex->m_face_count; vfi < face_count; ++vfi)
      m_original_vertex->m_faces[vfi] = nullptr;

    return true;
  }


  void UndoAttachUnmarkedFacesToCopiedVertex()
  {
    // This is used to resort the subd to a somewhat valid state after a critical error occurs
    if (nullptr == m_copied_vertex)
      return;
    if (0 == m_copied_vertex->m_face_count)
      return;
    if (nullptr == m_original_vertex)
      return;
    // Move faces from m_copied_vertex back to m_original_vertex (which is where they started).
    for (unsigned short vfi = 0; vfi < m_copied_vertex->m_face_count; vfi++)
    {
      const MYON_SubDFace* f = m_copied_vertex->m_faces[vfi];
      if (nullptr == f)
        continue;
      m_copied_vertex->m_faces[vfi] = nullptr;
      if (m_original_vertex->m_face_count < m_original_vertex->m_face_capacity)
      {
        m_original_vertex->m_faces[m_original_vertex->m_face_count] = f;
        m_original_vertex->m_face_count++;
      }
    }
    m_copied_vertex->m_face_count = 0;
  }
   
  bool AddExtrudedEdgeReference(
    class MYON_Internal_ExtrudedEdge* extruded_edge,
    bool bSetExtrudedEdgeToo
  );  

  static int CompareInitialVertexId(
    const MYON_Internal_ExtrudedVertex* lhs,
    const MYON_Internal_ExtrudedVertex* rhs
  );

  void SetBothVertexTags(MYON_SubDVertexTag vertex_tag)
  {
    if (nullptr != m_original_vertex)
      m_original_vertex->m_vertex_tag = vertex_tag;
    if (nullptr != m_copied_vertex)
      m_copied_vertex->m_vertex_tag = vertex_tag;
    if (nullptr != m_connecting_edge)
    {
      switch (vertex_tag)
      {
      case MYON_SubDVertexTag::Unset:
        m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::Unset;
        break;
      case MYON_SubDVertexTag::Smooth:
        m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::Smooth;
        break;
      case MYON_SubDVertexTag::Crease:
        m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::SmoothX;
        break;
      case MYON_SubDVertexTag::Corner:
        m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::Crease;
        break;
      case MYON_SubDVertexTag::Dart:
        m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::SmoothX;
        break;
      default:
        m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::Unset;
        break;
      }
    }
  }

  bool IsValidTopology(bool bCheckCopies) const;

};

const MYON_Internal_ExtrudedVertex MYON_Internal_ExtrudedVertex::Unset;

/*
MYON_Internal_ExtrudedVertex manages an edge that is extruded into a "side" face extrusion process.
*/
class MYON_Internal_ExtrudedEdge
{
public:
  MYON_Internal_ExtrudedEdge() = default;
  ~MYON_Internal_ExtrudedEdge() = default;
  MYON_Internal_ExtrudedEdge(const MYON_Internal_ExtrudedEdge&) = default;
  MYON_Internal_ExtrudedEdge& operator=(const MYON_Internal_ExtrudedEdge&) = default;

  static const MYON_Internal_ExtrudedEdge Unset;

  // tag m_original_edge had before the extrusion was performed.
  MYON_SubDEdgeTag m_initial_edge_tag = MYON_SubDEdgeTag::Unset;

  unsigned int m_initial_vertex_id[2] = {};

  unsigned int m_initial_edge_face_count = 0;

  // Used to sort the sides into groups of edges that are connected
  unsigned m_side_group_id = 0;

  // This is the edge in the original object. (This edge is moved).
  MYON_SubDEdge* m_original_edge = nullptr;

  // This edge is new. It is a copy of the original edge and remains at the original location.
  MYON_SubDEdge* m_copied_edge = nullptr;

  // m_extruded_vertex[0]->m_connecting_edge begins at m_copied_edge->m_vertex[0] and terminates at m_original_edge->m_vertex[0]
  // m_extruded_vertex[1]->m_connecting_edge begins at m_copied_edge->m_vertex[1] and terminates at m_original_edge->m_vertex[1]
  MYON_Internal_ExtrudedVertex* m_extruded_vertex[2] = {};

  // This is the new "side" quad with boundary made from the 4 edges above.
  MYON_SubDFace* m_new_face = nullptr;

  static int CompareSideGroupId(const MYON_Internal_ExtrudedEdge* lhs, const MYON_Internal_ExtrudedEdge* rhs);

  bool SetFromInitialEdge(
    MYON_SubDEdge* initial_edge
  )
  {
    *this = MYON_Internal_ExtrudedEdge::Unset;

    if (nullptr == initial_edge)
      return false;
    if (nullptr == initial_edge->m_vertex[0] || nullptr == initial_edge->m_vertex[1] || initial_edge->m_vertex[0]->m_id == initial_edge->m_vertex[1]->m_id)
      return false;

    // validate edge / face topology information - corrupt information will cause great difficulties during the extrusion process
    const MYON_SubDFacePtr* fptr = initial_edge->m_face2;
    for (unsigned short efi = 0; efi < initial_edge->m_face_count; ++efi, ++fptr)
    {
      if (2 == efi)
      {
        fptr = initial_edge->m_facex;
        if (nullptr == fptr)
          return false; // corrupt edge / face topology information
        if (initial_edge->m_facex_capacity < initial_edge->m_face_count - 2)
          return false; // corrupt edge / face topology information
      }
      const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(fptr->m_ptr);
      if (nullptr == f || f->m_edge_count < 3)
        return false; // corrupt edge / face topology information

      bool bFoundInitialEdge = false;
      const MYON_SubDEdgePtr* eptr = f->m_edge4;
      for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++eptr)
      {
        if (4 == fei)
        {
          eptr = f->m_edgex;
          if (nullptr == eptr)
            return false; // corrupt edge / face topology information
          if (f->m_edgex_capacity < f->m_edge_count - 4)
            return false; // corrupt edge / face topology information
        }
        const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr->m_ptr);
        if (nullptr == e)
          return false;
        if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] || e->m_vertex[0]->m_id == e->m_vertex[1]->m_id)
          return false;
        if (e == initial_edge)
        {
          if (bFoundInitialEdge)
            return false;
          bFoundInitialEdge = true;
          if (MYON_SUBD_EDGE_DIRECTION(eptr->m_ptr) != MYON_SUBD_FACE_DIRECTION(fptr->m_ptr))
            return false;
        }
      }
      if (false == bFoundInitialEdge)
        return false;
    }

    m_initial_edge_tag = initial_edge->m_edge_tag;
    m_initial_vertex_id[0] = initial_edge->m_vertex[0]->m_id;
    m_initial_vertex_id[1] = initial_edge->m_vertex[1]->m_id;
    m_initial_edge_face_count = initial_edge->m_face_count;
    m_original_edge = initial_edge;

    return true;
  }

  MYON_SubDFace* ExtrudeFace(MYON_SubD& subd);

  const MYON_Internal_ExtrudedVertex* ExtrudedVertex(MYON__UINT_PTR evi) const
  {
    return (0 == evi || 1 == evi) ? m_extruded_vertex[evi] : nullptr;
  }

  /// Returns "side" edge created by extruding the reference vertex
  MYON_SubDEdge* ConnectingEdge(MYON__UINT_PTR evi) const
  {
    const MYON_Internal_ExtrudedVertex* extruded_vertex = ExtrudedVertex(evi);
    return (nullptr != extruded_vertex) ? extruded_vertex->m_connecting_edge : nullptr;
  }

  MYON_SubDEdge* CreateCopiedEdge(MYON_SubD& subd)
  {
    if (nullptr != m_copied_edge)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    MYON_SubDVertex* copied_vertex[2] = {};
    for (unsigned evi = 0; evi < 2; evi++)
    {
      if (nullptr == m_extruded_vertex[evi])
        return MYON_SUBD_RETURN_ERROR(nullptr);
      copied_vertex[evi] = m_extruded_vertex[evi]->m_copied_vertex;
      if (nullptr == copied_vertex[evi])
        return MYON_SUBD_RETURN_ERROR(nullptr);
    }
    m_copied_edge = subd.AddEdge(MYON_SubDEdgeTag::Unset, copied_vertex[0], copied_vertex[1]);
    if (nullptr == m_copied_edge)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    m_original_edge->m_edge_tag = MYON_SubDEdgeTag::Unset;
    return m_copied_edge;
  }

  /// returns true if an unset vertex pair had its id set which means recursive id setting should continue.
  bool SetSideGroupId(unsigned side_group_id)
  {
    if (m_side_group_id > 0)
      return false;
    bool rc = false;
    m_side_group_id = side_group_id;
    for (unsigned evi = 0; evi < 2; ++evi)
    {
      if (nullptr != m_extruded_vertex[evi] && 0 == m_extruded_vertex[evi]->m_side_group_id)
      {
        rc = true;
        m_extruded_vertex[evi]->m_side_group_id = side_group_id;
      }
    }
    return rc;
  }

  /// Returns true if an unset vertex pair had its id set which means recursive id setting should continue.
  bool SetSideGroupIdFromVertexPairs()
  {
    if (m_side_group_id > 0)
      return false;
    for (unsigned evi = 0; evi < 2; ++evi)
    {
      if (nullptr != m_extruded_vertex[evi] && m_extruded_vertex[evi]->m_side_group_id > 0)
        return SetSideGroupId(m_extruded_vertex[evi]->m_side_group_id);
    }
    return false;
  }

  bool InitialCrease() const
  {
    return (MYON_SubDEdgeTag::Crease == m_initial_edge_tag);
  }

  bool InitialCreaseWithCorner() const
  {
    if (MYON_SubDEdgeTag::Crease == m_initial_edge_tag)
    {
      for (unsigned evi = 0; evi < 2; ++evi)
      {
        if (nullptr != m_extruded_vertex[evi] && MYON_SubDVertexTag::Corner == m_extruded_vertex[evi]->m_initial_vertex_tag)
          return true;
      }
    }
    return false;
  }

  void SetBothEdgeTags(MYON_SubDEdgeTag edge_tag)
  {
    if (MYON_SubDEdgeTag::Crease == edge_tag)
    {
      if (nullptr != m_original_edge)
        m_original_edge->m_edge_tag = MYON_SubDEdgeTag::Crease;
      if (nullptr != m_copied_edge)
        m_copied_edge->m_edge_tag = MYON_SubDEdgeTag::Crease;
      for (unsigned evi = 0; evi < 2; ++evi)
      {
        if (nullptr != m_extruded_vertex[evi] && MYON_SubDEdgeTag::Unset == m_extruded_vertex[evi]->m_connecting_edge->m_edge_tag)
        {
          if (MYON_SubDVertexTag::Corner == m_extruded_vertex[evi]->m_initial_vertex_tag)
            m_extruded_vertex[evi]->SetBothVertexTags(MYON_SubDVertexTag::Corner);
          else if (nullptr != m_extruded_vertex[evi]->m_connecting_edge)
            m_extruded_vertex[evi]->m_connecting_edge->m_edge_tag = MYON_SubDEdgeTag::Smooth;
        }
      }
    }
  }

  bool IsValidTopology(
    bool bCheckCopies
  ) const;
};

static bool Internal_IsNotValidExtrudedTopology()
{
  return MYON_SUBD_RETURN_ERROR(false);
}

bool MYON_Internal_ExtrudedVertex::IsValidTopology(bool bCheckCopies) const
{
  if (nullptr == this->m_original_vertex)
    return Internal_IsNotValidExtrudedTopology();
  if (this->m_initial_vertex_id != this->m_original_vertex->m_id)
    return Internal_IsNotValidExtrudedTopology();
  if (this->m_extruded_edges_count == 0)
    return Internal_IsNotValidExtrudedTopology();
  if (bCheckCopies && nullptr == this->m_copied_vertex)
    return Internal_IsNotValidExtrudedTopology();
  for (unsigned vei = 0; vei < 2 && vei < this->m_extruded_edges_count; ++vei)
  {
    if (nullptr == this->m_extruded_edges[vei])
      return Internal_IsNotValidExtrudedTopology();
    const MYON_Internal_ExtrudedEdge& extruded_edge = *this->m_extruded_edges[vei];
    if (nullptr == extruded_edge.m_original_edge)
      return Internal_IsNotValidExtrudedTopology();
    const unsigned evi = (this->m_original_vertex == extruded_edge.m_original_edge->m_vertex[0]) ? 0 : 1;
    if (this->m_initial_vertex_id != extruded_edge.m_initial_vertex_id[evi])
      return Internal_IsNotValidExtrudedTopology();
    for (unsigned k = 0; k < (bCheckCopies ? 2U : 1U); ++k)
    {
      const MYON_SubDEdge* e = (0 == k) ? extruded_edge.m_original_edge : extruded_edge.m_copied_edge;
      if (nullptr == e)
        return Internal_IsNotValidExtrudedTopology();
      if (e->m_vertex[0] == e->m_vertex[1])
        return Internal_IsNotValidExtrudedTopology();
      const MYON_SubDVertex* v = (0 == k) ? this->m_original_vertex : this->m_copied_vertex;
      if (nullptr == v)
        return Internal_IsNotValidExtrudedTopology();
      if (e->m_vertex[evi] != v)
        return Internal_IsNotValidExtrudedTopology();
    }
    if (extruded_edge.m_side_group_id != this->m_side_group_id)
      return Internal_IsNotValidExtrudedTopology();
  }

  return true;
}

bool MYON_Internal_ExtrudedEdge::IsValidTopology(
  bool bCheckCopies
) const
{
  if (nullptr == m_original_edge)
    return Internal_IsNotValidExtrudedTopology();

  if (bCheckCopies && nullptr == m_copied_edge)
    return Internal_IsNotValidExtrudedTopology();

  for (unsigned evi = 0; evi < 2; ++evi)
  {
    if (nullptr == this->m_extruded_vertex[evi])
      return Internal_IsNotValidExtrudedTopology();
    const MYON_Internal_ExtrudedVertex& extruded_vertex = *this->m_extruded_vertex[evi];
    for (unsigned k = 0; k < (bCheckCopies ? 2U : 1U); ++k)
    {
      const MYON_SubDEdge* e = (0 == k) ? this->m_original_edge : this->m_copied_edge;
      if (nullptr == e)
        return Internal_IsNotValidExtrudedTopology();
      const MYON_SubDVertex* v = (0 == k) ? extruded_vertex.m_original_vertex : extruded_vertex.m_copied_vertex;
      if (nullptr == v)
        return Internal_IsNotValidExtrudedTopology();
      if (e->m_vertex[evi] != v)
        return Internal_IsNotValidExtrudedTopology();
      if (v->EdgeArrayIndex(e) >= v->m_edge_count)
        return Internal_IsNotValidExtrudedTopology();
    }
    if (extruded_vertex.m_initial_vertex_id != extruded_vertex.m_original_vertex->m_id)
      return Internal_IsNotValidExtrudedTopology();
    if (extruded_vertex.m_initial_vertex_id != this->m_initial_vertex_id[evi])
      return Internal_IsNotValidExtrudedTopology();
    if (this->m_side_group_id != extruded_vertex.m_side_group_id)
      return Internal_IsNotValidExtrudedTopology();
  }
  return true;
}

bool MYON_Internal_ExtrudedVertex::AddExtrudedEdgeReference(
  class MYON_Internal_ExtrudedEdge* extruded_edge,
  bool bSetExtrudedEdgeToo
)
{
  if (nullptr == m_original_vertex || 0 == m_initial_vertex_id || m_initial_vertex_id != m_original_vertex->m_id)
    return MYON_SUBD_RETURN_ERROR(false);

  if (nullptr == extruded_edge || nullptr == extruded_edge->m_original_edge )
    return MYON_SUBD_RETURN_ERROR(false);

  unsigned int evi;

  for (evi = 0; evi < 2; ++evi)
  {
    if (m_original_vertex == extruded_edge->m_original_edge->m_vertex[evi] && m_initial_vertex_id == extruded_edge->m_initial_vertex_id[evi])
      break;
  }
  if (evi > 1)
  {
    // edge and vertex are not attached
    return MYON_SUBD_RETURN_ERROR(false);
  }

  if ( nullptr != extruded_edge->m_extruded_vertex[evi] && this != extruded_edge->m_extruded_vertex[evi])
    return MYON_SUBD_RETURN_ERROR(false);

  switch (m_extruded_edges_count)
  {
  case 0:
    m_extruded_edges[0] = extruded_edge;
    m_extruded_edges_count = 1;
    break;

  case 1:
    if (m_extruded_edges[0] != extruded_edge)
    {
      m_extruded_edges[1] = extruded_edge;
      m_extruded_edges_count = 2;
    }
    break;

  case 2:
    if (m_extruded_edges[0] != extruded_edge && m_extruded_edges[1] != extruded_edge)
      m_extruded_edges_count = 3; // rare case with 3 or more extruded edges sharing an extruded vertex
    break;

  case 3:
    // do nothing this is vertex will be extruded into a non-manifold "side" connecting edge
    break;

  default:
    // invalid value of m_extruded_edges_count
    return MYON_SUBD_RETURN_ERROR(false);
    break;
  }

  if (bSetExtrudedEdgeToo && nullptr == extruded_edge->m_extruded_vertex[evi])
    extruded_edge->m_extruded_vertex[evi] = this;

  return true;
}

bool MYON_Internal_ExtrudedVertex::SetConnectingEdgeTag()
{
  if (nullptr == m_original_vertex || 0 == m_initial_vertex_id)
    return MYON_SUBD_RETURN_ERROR(false); // to early

  if (nullptr != m_copied_vertex || nullptr != m_connecting_edge)
    return MYON_SUBD_RETURN_ERROR(false); // to late

  if (m_initial_vertex_id != m_original_vertex->m_id || m_initial_vertex_tag != m_original_vertex->m_vertex_tag || MYON_SubDEdgeTag::Unset != m_connecting_edge_tag)
    return MYON_SUBD_RETURN_ERROR(false); // corrupt information

  if (m_extruded_edges_count != 2)
  {
    // simple case
    m_connecting_edge_tag = MYON_SubDEdgeTag::Crease;
    return true;
  }

  //if (MYON_SubDVertexTag::Corner == m_initial_vertex_tag)
  //{
  //  // simple case
  //  m_connecting_edge_tag = MYON_SubDEdgeTag::Crease;
  //  return true;
  //}

  if (MYON_SubDVertexTag::Crease != m_initial_vertex_tag && MYON_SubDVertexTag::Corner != m_initial_vertex_tag)
    return true; // leaving m_connecting_edge_tag unset for anything else works best


  bool bMovedCrease = false;
  bool bStationaryCrease = false;

  const MYON_SubDEdge* extruded_edge[2] = {};
  for (unsigned vei = 0; vei < 2; ++vei)
  {
    extruded_edge[vei] = (nullptr != m_extruded_edges[vei]) ? m_extruded_edges[vei]->m_original_edge : nullptr;
    if (nullptr == extruded_edge[vei])
      return MYON_SUBD_RETURN_ERROR(false); // corrupt information
    if (1 == extruded_edge[vei]->m_face_count)
    {
      if (extruded_edge[vei]->MarkedFaceCount() > 0)
        bStationaryCrease = true; // the boundary (crease) will not move but the attached face does move
      else
      {
        // the boundary (crease) will move but the attached face does not move
        if (MYON_SubDVertexTag::Corner == m_initial_vertex_tag)
        {
          m_connecting_edge_tag = MYON_SubDEdgeTag::Crease;
          return true;
        }
        bMovedCrease = true; 
      }
    }
  }

  // determine vertex topology and edge demographics
  for (unsigned short vei = 0; vei < m_original_vertex->m_edge_count; ++vei)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(m_original_vertex->m_edges[vei].m_ptr);
    if (nullptr == e || false == e->IsCrease() || e == extruded_edge[0] || e == extruded_edge[1])
      continue;
    if (e->MarkedFaceCount() > 0)
      bMovedCrease = true;
    else
      bStationaryCrease = true;
  }

  // used the collected vertex topology and edge demographics to determine if connecting_edge_tag should be Crease
  if (bMovedCrease && bStationaryCrease)
    m_connecting_edge_tag = MYON_SubDEdgeTag::Crease;

  return true;
}


int MYON_Internal_ExtrudedEdge::CompareSideGroupId(const MYON_Internal_ExtrudedEdge* lhs, const MYON_Internal_ExtrudedEdge* rhs)
{
  const unsigned lhs_side_group_id = lhs->m_side_group_id;
  const unsigned rhs_side_group_id = rhs->m_side_group_id;
  if (lhs_side_group_id < rhs_side_group_id)
    return -1;
  if (lhs_side_group_id > rhs_side_group_id)
    return 1;
  return 0;
}

const MYON_Internal_ExtrudedEdge MYON_Internal_ExtrudedEdge::Unset;

int MYON_Internal_ExtrudedVertex::CompareInitialVertexId(
  const MYON_Internal_ExtrudedVertex* lhs,
  const MYON_Internal_ExtrudedVertex* rhs
)
{
  const unsigned int lhs_id = lhs->m_initial_vertex_id;
  const unsigned int rhs_id = rhs->m_initial_vertex_id;
  if (lhs_id < rhs_id)
    return -1;
  if (lhs_id > rhs_id)
    return 1;
  return 0;
}

static void Internal_SetEdgeVertices(
  MYON_SubD& subd,
  MYON_Internal_ExtrudedVertex& extruded_vertex
  )
{
  // marked edges use the marked vertex.
  MYON_SubDVertex* marked_vertex = extruded_vertex.m_original_vertex;
  MYON_SubDVertex* unmarked_vertex =  extruded_vertex.m_copied_vertex;
  const unsigned int vertex_edge_count = marked_vertex->EdgeCount();
  unsigned int marked_edge_count = 0;
  unsigned int unmarked_edge_count = 0;
  unsigned int new_edge_count = 0;
  for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
  {
    MYON_SubDEdgePtr eptr = marked_vertex->m_edges[vei];
    const MYON_SubDEdge* e = eptr.Edge();
    if (nullptr == e)
      continue;
    if (extruded_vertex.m_connecting_edge == e)
      new_edge_count++;
    else if (e->m_status.RuntimeMark())
      marked_edge_count++;
    else
      unmarked_edge_count++;
  }

  if (unmarked_edge_count <= 0)
    return;

  unmarked_edge_count += unmarked_vertex->m_edge_count;

  if ( unmarked_vertex->m_edge_capacity < (unmarked_edge_count+new_edge_count) )
    subd.GrowVertexEdgeArray(unmarked_vertex, unmarked_edge_count);    

  marked_vertex->m_edge_count = 0;
  for (unsigned int vei = 0; vei < vertex_edge_count; vei++)
  {
    MYON_SubDEdgePtr eptr = marked_vertex->m_edges[vei];
    MYON_SubDEdge* e = eptr.Edge();
    if (nullptr == e)
      continue;
    if (extruded_vertex.m_connecting_edge == e || e->m_status.RuntimeMark())
    {
      marked_vertex->m_edges[marked_vertex->m_edge_count] = eptr;
      marked_vertex->m_edge_count++;
    }
    else
    {
      if (e->m_vertex[0] == marked_vertex)
        e->m_vertex[0] = unmarked_vertex;
      else if (e->m_vertex[1] == marked_vertex)
        e->m_vertex[1] = unmarked_vertex;
      unmarked_vertex->m_edges[unmarked_vertex->m_edge_count] = eptr;
      unmarked_vertex->m_edge_count++;
    }
  }
}

MYON_SubDFace* MYON_Internal_ExtrudedEdge::ExtrudeFace(
  MYON_SubD& subd
)
{
  // All components that will be moved have the runtime mark set.
  // All other components have a clear runtime mark.
  // The original_edge will be moved.
  // The new_edge will not be moved.
  // new edge and original edge go the same direction.
  // new_side_edges[2] run from new to original edges.
  
  // change edges of unmarked faces to use the new edge
  MYON__UINT_PTR edir = 0;
  MYON_SubDEdge* marked_edge = this->m_original_edge; // will be moved
  MYON_SubDEdge* unmarked_edge = this->m_copied_edge; // a copy of the original edge at the original location
  unsigned int marked_edge_face_count0 = marked_edge->m_face_count;
  MYON_SubDFacePtr* marked_edge_fptr1 = marked_edge->m_face2;
  const MYON_SubDFacePtr* marked_edge_fptr0 = marked_edge_fptr1;
  unsigned int marked_edge_face_count1 = 0;
  subd.GrowEdgeFaceArray(unmarked_edge, marked_edge_face_count0);
  for (unsigned int efi = 0; efi < marked_edge_face_count0; efi++, marked_edge_fptr0++)
  {
    if (2 == efi)
      marked_edge_fptr0 = marked_edge->m_facex;

    if (2 == marked_edge_face_count1)
      marked_edge_fptr1 = marked_edge->m_facex;

    MYON_SubDFace* f = marked_edge_fptr0->Face();
    if (nullptr == f)
    {
      MYON_SUBD_ERROR("null face pointer");
      continue;
    }

    if (f->m_status.RuntimeMark())
    {
      edir = marked_edge_fptr0->FaceDirection();
      marked_edge_face_count1++;
      *marked_edge_fptr1 = *marked_edge_fptr0;
      marked_edge_fptr1++;
      continue; // this face will be moved and keeps edge e1
    }

    // f is unmarked.
    // change referenced edge from marked_edge to unmarked_edge.
    f->ReplaceEdgeInArray(0, marked_edge, unmarked_edge);

    unmarked_edge->AddFaceToArray(*marked_edge_fptr0);
  }

  // When marked_edge is a manifold edge, face_count goes from 2 to 1.
  marked_edge->m_face_count = static_cast<unsigned short>(marked_edge_face_count1);

  MYON_SubDEdge* side0 = this->ConnectingEdge(edir);
  MYON_SubDEdge* side1 = this->ConnectingEdge(1 - edir);
  MYON_SubDEdgePtr new_face_eptr[4];
  new_face_eptr[0] = MYON_SubDEdgePtr::Create(this->m_original_edge, 1-edir);
  new_face_eptr[1] = MYON_SubDEdgePtr::Create(side0, 1);
  new_face_eptr[2] = MYON_SubDEdgePtr::Create(this->m_copied_edge, edir);
  new_face_eptr[3] = MYON_SubDEdgePtr::Create(side1, 0);
  
  this->m_new_face = subd.AddFace(new_face_eptr, 4);

  if (nullptr != this->m_new_face)
  {
    // When isolated edges are extruded, we need to flip the face.
    // In all other cases, we don't.
    bool bFlip = false;
    bool bFlipSet = false;
    for (unsigned int fei = 0; fei < 4; fei++)
    {
      const MYON_SubDEdgePtr eptr = this->m_new_face->m_edge4[fei];
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
      if (nullptr == e || e->m_face_count > 2)
      {
        bFlipSet = false;
        break;
      }
      if (2 != e->m_face_count)
        continue;
      const MYON__UINT_PTR fedir = MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
      const MYON_SubDFacePtr fptr[2] = { e->m_face2[0], e->m_face2[1] };
      const MYON_SubDFace* f[2] = { MYON_SUBD_FACE_POINTER(fptr[0].m_ptr), MYON_SUBD_FACE_POINTER(fptr[1].m_ptr) };
      if (nullptr == f[0] || nullptr == f[1] || f[0] == f[1])
      {
        bFlipSet = false;
        break;
      }
      if (this->m_new_face != f[0] && this->m_new_face != f[1] )
      {
        bFlipSet = false;
        break;
      }
      const MYON__UINT_PTR fdir[2] = { MYON_SUBD_FACE_DIRECTION(fptr[0].m_ptr), MYON_SUBD_FACE_DIRECTION(fptr[1].m_ptr) };
      if (fedir != fdir[(f[0] == this->m_new_face) ? 0 : 1])
      {
        bFlipSet = false;
        break;
      }
      const bool bSameDir = (fdir[0] == fdir[1]) ? true : false;
      if (false == bFlipSet)
      {
        bFlipSet = true;
        bFlip = bSameDir;
      }
      else if (bSameDir != bFlip)
      {
        bFlipSet = false;
        break;
      }
    }
    if (bFlip)
      this->m_new_face->ReverseEdgeList();
  }

  return this->m_new_face;
}

unsigned int MYON_SubD::ExtrudeComponents(
  const MYON_Xform& xform,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count
)
{
  const bool bExtrudeBoundaries = true;
  const bool bPermitNonManifoldEdgeCreation = false;
  return ExtrudeComponents(xform, cptr_list, cptr_count, bExtrudeBoundaries, bPermitNonManifoldEdgeCreation);
}

unsigned int MYON_SubD::ExtrudeComponents(
  const MYON_Xform& xform,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bExtrudeBoundaries,
  bool bPermitNonManifoldEdgeCreation
)
{
  if (nullptr == cptr_list || cptr_count <= 0)
    return 0;
  return Internal_ExtrudeComponents(
    xform,
    cptr_list,
    cptr_count,
    bExtrudeBoundaries,
    bPermitNonManifoldEdgeCreation
    );
}

unsigned int MYON_SubD::Extrude(const MYON_Xform & xform)
{
  if (IsSolid())
    return 0;
  const bool bExtrudeBoundaries = true;
  const bool bPermitNonManifoldEdgeCreation = false;
  return Internal_ExtrudeComponents(
    xform,
    nullptr,
    0,
    bExtrudeBoundaries,
    bPermitNonManifoldEdgeCreation
    );
}

static bool Internal_SetSideGroupIds(MYON_SimpleArray<MYON_Internal_ExtrudedEdge>& new_sides)
{
  unsigned count = new_sides.UnsignedCount();
  if (count <= 0)
    return false;
  unsigned side_group_id = 1;
  for (unsigned int i = 0; i < count; ++i)
  {
    if (false == new_sides[i].SetSideGroupId(side_group_id))
      continue;

    // propogate side_group_id through all touching components
    unsigned j0 = i + 1;
    for (bool bContinue = true; bContinue; /*empty iterator*/)
    {      
      bContinue = false;
      while (j0 < count && new_sides[j0].m_side_group_id > 0)
        ++j0;
      while (count > j0 && new_sides[count - 1].m_side_group_id > 0)
        --count;
      for (unsigned j = j0; j < count; ++j)
      {
        if (new_sides[j].SetSideGroupIdFromVertexPairs())
          bContinue = true;
      }
    }


    ++side_group_id;
  }

  if (side_group_id <= 1)
    return false;

  if (side_group_id > 2)
    new_sides.QuickSort(MYON_Internal_ExtrudedEdge::CompareSideGroupId);

  return true;
}


static bool Internal_DebugValdateExtrudedTopology(
  bool bCheckCopies,
  const MYON_SimpleArray<MYON_Internal_ExtrudedEdge>& extruded_edges,
  const MYON_SimpleArray<MYON_Internal_ExtrudedVertex>& extruded_vertices
)
{
  const unsigned edge_count = extruded_edges.UnsignedCount();
  const unsigned vertex_count = extruded_vertices.UnsignedCount();

  for (unsigned ei = 0; ei < edge_count; ++ei)
  {
    const MYON_Internal_ExtrudedEdge& extruded_edge = extruded_edges[ei];
    if (false == extruded_edge.IsValidTopology(bCheckCopies))
      return false;
  }

  for (unsigned vi = 0; vi < vertex_count; ++vi)
  {
    const MYON_Internal_ExtrudedVertex& extruded_vertex = extruded_vertices[vi];
    if (false == extruded_vertex.IsValidTopology(bCheckCopies))
      return false;
  }

  if (vertex_count < edge_count)
    return Internal_IsNotValidExtrudedTopology();

  return true;
}

static bool Internal_IsInsetHack(
  const MYON_Xform& xform,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bExtrudeBoundaries,
  bool bPermitNonManifoldEdgeCreation
)
{
  if (nullptr == cptr_list || cptr_count <= 0 || false != bPermitNonManifoldEdgeCreation)
    return false;

  // inset xform is 0 except xform[0][2] = MYON_ZERO_TOLERANCE.
  for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
  {
    if (xform.m_xform[i][j] == ((0 == i && 2 == j) ? MYON_ZERO_TOLERANCE : 0.0) )
      continue;
    return false;
  }

  for (size_t i = 0; i < cptr_count; ++i)
  {
    if (nullptr == cptr_list[i].Face())
      return false;
  }

  return true;
}

unsigned int MYON_SubD::Internal_ExtrudeComponents(
  const MYON_Xform& xform_arg,
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  bool bExtrudeBoundaries,
  bool bPermitNonManifoldEdgeCreation
)
{
  const bool bIsInset = Internal_IsInsetHack(xform_arg, cptr_list, cptr_count, bExtrudeBoundaries, bPermitNonManifoldEdgeCreation);

  const bool bHaveCptrList = (cptr_count > 0 && nullptr != cptr_list);
  bool bExtrudeAll = false;
  if (false == bHaveCptrList && 0 == cptr_count)
  {
    bool bIsManifold = false;
    bool bIsOriented = false;
    bool bHasBoundary = false;
    int solid_orientation = 0;
    ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
    bExtrudeAll = bHasBoundary;
  }
  if (
    false == xform_arg.IsValidAndNotZeroAndNotIdentity()
    || (false == bHaveCptrList && false == bExtrudeAll)
    )
    return 0;
  
  const MYON_Xform& xform = bIsInset ? MYON_Xform::IdentityTransformation : xform_arg;

  MYON_SubDComponentMarksClearAndRestore mark_guard(*this);

  // Marks every face in cptr_list[].
  // Marks every edge attached to a marked face.
  // Marks every subd boundary edge in the cptr_list[].
  // Marks every vertex touching a marked edge or marked face.
  unsigned int list_vertex_count = 0;
  unsigned int list_edge_count = 0;
  unsigned int list_face_count = 0;
  const unsigned int marked_vertex_count = Internal_MarkExtrudeComponents(
    *this,
    cptr_list, cptr_count,
    bExtrudeBoundaries,
    list_vertex_count, list_edge_count, list_face_count
  );
  if (0 == marked_vertex_count)
    return 0;

  const bool bDestroyEvaluationCache = (4 * list_vertex_count >= VertexCount() || 4 * list_edge_count >= EdgeCount() || 4 * list_face_count >= FaceCount());

  // Set moved_faces[] = list of faces that will move.
  MYON_SimpleArray<const MYON_SubDFace*> moved_faces(list_face_count + 128);
  MYON_SubDFaceIterator fit(*this);
  for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
  {
    const unsigned int face_vertex_count = f->m_edge_count;
    if (face_vertex_count < 3 || false == f->m_status.RuntimeMark())
      continue;
    moved_faces.Append(f);
  }
  const unsigned int moved_face_count = moved_faces.UnsignedCount();

  MYON_SimpleArray<MYON_Internal_ExtrudedEdge> extruded_edges(64);
  MYON_SubDEdgeIterator eit(*this);
  for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    const bool bMarkedEdge = e->Mark();
    e->ClearMark();
    e->ClearMarkBits();
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      continue;

    const unsigned int edge_face_count = e->m_face_count;

    // marked wire edges and marked boundary edges get extruded whenever bExtrudeBoundaries is true.
    bool bExtrudeEdge = bMarkedEdge && (0 == edge_face_count || (1 == edge_face_count && bExtrudeBoundaries));

    bool bAttachedToMarkedFace = false;
    bool bAttachedToUnmarkedFace = false;
    for (unsigned int efi = 0; efi < edge_face_count; efi++)
    {
      const MYON_SubDFace* f = e->Face(efi);
      if (nullptr == f)
        continue;
      if (f->Mark())
        bAttachedToMarkedFace = true; // f is in the subset of faces that will be moved.
      else
        bAttachedToUnmarkedFace = true; // f is in the subset of faces that are stationary (will not be moved).

      if (bAttachedToMarkedFace && bAttachedToUnmarkedFace)
      {
        // e is on the boundary between the subset of moved faces and stationary faces
        // and will be extruded into a face.
        bExtrudeEdge = true;
        break;
      }
    }

    if (bExtrudeEdge)
    {
      MYON_Internal_ExtrudedEdge& extruded_edge = extruded_edges.AppendNew();
      if (false == extruded_edge.SetFromInitialEdge(const_cast<MYON_SubDEdge*>(e)))
      {
        MYON_SUBD_ERROR("null e or vertex pointers");
        return 0;
      }

      // Marking e here indicates e is an extruded edge.
      // MYON_Internal_ExtrudedVertex.SetFromInitialVertex() assumes marked edges are extruded edges.
      e->SetMark();
    }
  }

  const unsigned int extruded_edge_count = extruded_edges.UnsignedCount();
  
  if (0 == extruded_edge_count)
  {
    // no new faces will be made
    if (moved_face_count > 0 && false == bIsInset)
    {
      // Every face is moving 
      Transform(xform);
      return FaceCount();
    }
    return 0;
  }

  // Mark edges on the boundary of the moved subset.
  // clear vertex  marks.
  this->ClearVertexMarks();
  this->ClearVertexMarkBits();

  // build a list extruded_vertices[] of vertices that will be extruded into "side" edges
  MYON_SimpleArray<MYON_Internal_ExtrudedVertex> extruded_vertices(extruded_edge_count + 8);
  for (unsigned int i = 0; i < extruded_edge_count; i++)
  {
    MYON_Internal_ExtrudedEdge& extruded_edge = extruded_edges[i];
    const MYON_SubDEdge* e = extruded_edge.m_original_edge;
    if (nullptr == e)
    {
      MYON_SUBD_ERROR("Should never be the case.");
      return 0;
    }
    for (unsigned int evi = 0; evi < 2; evi++)
    {
      MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>(e->m_vertex[evi]);
      if (v->Mark())
        continue; // this vertex has already been added to extruded_vertices[]

      // Mark this vertex to indicate it will be extruded into a "side" edge.
      v->SetMark();

      MYON_Internal_ExtrudedVertex& extruded_vertex = extruded_vertices.AppendNew();
      if (false == extruded_vertex.SetFromInitialVertex(v))
      {
        MYON_SUBD_ERROR("Should never be the case.");
        return 0;
      }

      // Partially setting the topology connections here reduces the number of binary searches by about half.
      // The scecond paramter is false because the sorting of extruded_vertices[] will change the memory
      // location where extruded_vertex is stored.
      if (false == extruded_vertex.AddExtrudedEdgeReference(&extruded_edge, false))
        return 0;
    }
  }

  // sort vertex pairs so they can be located by the original vertex id.
  extruded_vertices.QuickSort(MYON_Internal_ExtrudedVertex::CompareInitialVertexId);

  // After sorting but before searching, set extruded_edge.m_extruded_vertex[] pointer
  // for the extruded_edge used to generate the extruded_vertex in the for() loop
  // immediately before the sorting of extruded_vertices[].
  // This reduces the number of binary searches by about half.
  for (unsigned int i = 0; i < extruded_vertices.UnsignedCount(); i++)
  {
    MYON_Internal_ExtrudedVertex& extruded_vertex = extruded_vertices[i];
    if (
      extruded_vertex.m_initial_vertex_id > 0
      && 1 == extruded_vertex.m_extruded_edges_count
      && nullptr != extruded_vertex.m_extruded_edges[0]
      )
    {
      if (extruded_vertex.AddExtrudedEdgeReference(extruded_vertex.m_extruded_edges[0], true))
        continue;
    }

    MYON_SUBD_ERROR("bug introduced in code above since Feb 2020");
    return 0;
  }

  // Use the sorted extruded_vertices[] array and initial vertex ids to
  // finish set the topology connections between extruded_vertices[] and extruded_edges[]
  MYON_Internal_ExtrudedVertex bsearch_key;
  for (unsigned int i = 0; i < extruded_edges.UnsignedCount(); i++)
  {
    MYON_Internal_ExtrudedEdge& extruded_edge = extruded_edges[i];
    for (unsigned int evi = 0; evi < 2; evi++)
    {
      if (nullptr != extruded_edge.m_extruded_vertex[evi])
        continue; // this topology connection was set above and we can skip the binary search step

      // Use a binary search to find the element of extruded_vertices[] that corresponds to extruded_edge.m_original_edge->m_vertex[evi].
      bsearch_key.m_initial_vertex_id = extruded_edge.m_initial_vertex_id[evi];
      const int i0 = extruded_vertices.BinarySearch(&bsearch_key, MYON_Internal_ExtrudedVertex::CompareInitialVertexId);
      if (i0 < 0)
      {
        MYON_SUBD_ERROR("Should never be the case.");
        return 0;
      }
      if (false == extruded_vertices[i0].AddExtrudedEdgeReference(&extruded_edge, true))
        return 0;
    }
  }

  if (false == bPermitNonManifoldEdgeCreation)
  {
    for (unsigned int i = 0; i < extruded_vertices.UnsignedCount(); i++)
    {
      // 3 or more "side" faces will be attached to the edge made by extruding the vertex.
      // Thus, this edge will be a nonmanifold edge.
      if (extruded_vertices[i].m_extruded_edges_count >= 3)
        return 0;
    }
  }

  if (false == Internal_DebugValdateExtrudedTopology(false, extruded_edges, extruded_vertices))
    return 0;

  // Need to set some tag information that is used when we created the extruded edges and faces
  // before modifications are made to this subd.
  for (unsigned int i = 0; i < extruded_vertices.UnsignedCount(); ++i)
    extruded_vertices[i].SetConnectingEdgeTag();

  // Extrude vertices into side edges - creates new vertices and connecting edges.
  for (unsigned int i = 0; i < extruded_vertices.UnsignedCount(); ++i)
  {
    if (false == extruded_vertices[i].ExtrudeVertex(*this, bIsInset, xform))
      return 0;
  }

  // Create "side" face edge that will be opposite the original edge.
  for (unsigned int i = 0; i < extruded_edges.UnsignedCount(); ++i)
  {
    if (nullptr == extruded_edges[i].CreateCopiedEdge(*this))
      return 0;
  }

  if (false == Internal_DebugValdateExtrudedTopology(true, extruded_edges, extruded_vertices))
    return 0;

  // At this point moved_faces[] = set of marked faces.
  // Efficiency determines whither the face mark or the moved_faces[] array 
  // is used to decide if a face is moved or stationary.
  for (unsigned int i = 0; i < extruded_vertices.UnsignedCount(); ++i)
  {
    // Attach unmarked faces (stationary) to m_copied_vertex.
    // Leave the marked faces (transformed) attached to m_original_vertex.
    if (false == extruded_vertices[i].AttachUnmarkedFacesToCopiedVertex())
    {
      for (unsigned j = 0; j < i; ++j)
        extruded_vertices[i].UndoAttachUnmarkedFacesToCopiedVertex();
      return 0;
    }
  }

  // From this point on, we don't bail out, we just press on doing the best that can be done.


  // Mark everything a moved face touches
  // including interior edges and vertices.
  // Transform any vertices that are not already marked.
  // (The vertices in extruded_vertices[] are currently marked and have already been transformed.)
  for (unsigned int i = 0; i < moved_face_count; i++)
  {
    const MYON_SubDFace* f = moved_faces[i];
    if (false == bDestroyEvaluationCache)
      f->ClearSavedSubdivisionPoints();
    const unsigned int face_edge_count = f->m_edge_count;
    for (unsigned int fei = 0; fei < face_edge_count; ++fei)
    {
      const MYON_SubDEdge* e = f->Edge(fei);
      if (nullptr == e)
        continue;
      if (false == bDestroyEvaluationCache)
        e->ClearSavedSubdivisionPoints();
      e->SetMark();
      for (unsigned int evi = 0; evi < 2; evi++)
      {
        MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>(e->m_vertex[evi]);
        if (nullptr != v && false == v->Mark())
        {
          if (false == bIsInset)
            v->Transform(false, xform);
          v->SetMark();
          if (false == bDestroyEvaluationCache)
            v->VertexModifiedNofification();
        }
      }
    }
  }

  // For the original boundary vertrex, move unmarked edges to use the new vertex.
  for (unsigned int i = 0; i < extruded_vertices.UnsignedCount(); i++)
    Internal_SetEdgeVertices(*this, extruded_vertices[i]);

  // extrude edges into new "side" faces.
  for (unsigned int i = 0; i < extruded_edge_count; i++)
    extruded_edges[i].ExtrudeFace(*this);

  // remove cached subdivision calculations
  if (bDestroyEvaluationCache)
    ClearEvaluationCache();

  // Set tags that are clearly determined by the extrusion.
  if ( Internal_SetSideGroupIds(extruded_edges))
  {
    const unsigned count = extruded_edges.UnsignedCount();
    unsigned i1 = 0;
    for (unsigned i0 = 0; i0 < count; i0 = (i1 > i0) ? i1 : (i0 + 1))
    {
      const unsigned side_group_id = extruded_edges[i0].m_side_group_id;
      unsigned crease_count = 0;
      unsigned interior_crease_count = 0;
      unsigned corner_count = 0;
      for (i1 = i0; i1 < count && side_group_id == extruded_edges[i1].m_side_group_id; ++i1)
      {
        MYON_Internal_ExtrudedEdge& extruded_edge = extruded_edges[i1];
        if (nullptr != extruded_edge.m_original_edge && 2 != extruded_edge.m_original_edge->m_face_count)
        {
          extruded_edge.m_original_edge->m_edge_tag = MYON_SubDEdgeTag::Crease;
        }
        if (nullptr != extruded_edge.m_copied_edge)
        {
          if (
            2 != extruded_edge.m_copied_edge->m_face_count
            || 
            (2 == extruded_edge.m_initial_edge_face_count && MYON_SubDEdgeTag::Crease == extruded_edge.m_initial_edge_tag)
            )
            extruded_edge.m_copied_edge->m_edge_tag = MYON_SubDEdgeTag::Crease;
        }
        if (extruded_edge.InitialCrease())
        {
          ++crease_count;
          if (2 == extruded_edge.m_initial_edge_face_count)
            ++interior_crease_count;
          if (extruded_edge.InitialCreaseWithCorner())
          {
            ++corner_count;
            for (unsigned k = 0; k < 2; ++k)
            {
              MYON_SubDEdge* e = (0 == k) ? extruded_edge.m_original_edge : extruded_edge.m_copied_edge;
              if (2 != e->m_face_count)
              {
                e->m_edge_tag = MYON_SubDEdgeTag::Crease;
                for (unsigned evi = 0; evi < 2; ++evi)
                {
                  MYON_Internal_ExtrudedVertex* extruded_vertex = extruded_edge.m_extruded_vertex[evi];
                  if (MYON_SubDVertexTag::Corner == extruded_vertex->m_initial_vertex_tag)
                  {
                    MYON_SubDVertex* v = (0 == k) ? extruded_vertex->m_original_vertex : extruded_vertex->m_copied_vertex;
                    v->m_vertex_tag = MYON_SubDVertexTag::Corner;
                  }
                }
              }
            }
          }
        }
      }
      if (
        (crease_count == (i1 - i0))
        && 
        (corner_count > 0 || crease_count == interior_crease_count)
        )
      {
        for (unsigned i = i0; i < i1; ++i)
          extruded_edges[i].SetBothEdgeTags(MYON_SubDEdgeTag::Crease);
      }
    }
  }

  for (unsigned i = 0; i < extruded_vertices.UnsignedCount(); ++i)
  {
    MYON_Internal_ExtrudedVertex& pair = extruded_vertices[i];
    if (MYON_SubDVertexTag::Corner == pair.m_initial_vertex_tag)
    {
      for (unsigned vdex = 0; vdex < 2; ++vdex)
      {
        MYON_SubDVertex* v = (0 == vdex) ? pair.m_original_vertex : pair.m_copied_vertex;
        if (nullptr == v)
          continue;
        if (v->EdgeProperties().m_crease_edge_count >= 3)
          v->m_vertex_tag = MYON_SubDVertexTag::Corner;
      }
    }
  }

  if ( false == bDestroyEvaluationCache)
  {
    for (unsigned int i = 0; i < extruded_vertices.UnsignedCount(); ++i)
    {
      MYON_Internal_ExtrudedVertex& pair = extruded_vertices[i];
      if (nullptr != pair.m_original_vertex)
        pair.m_original_vertex->VertexModifiedNofification();
      if ( nullptr != pair.m_copied_vertex)
        pair.m_copied_vertex->VertexModifiedNofification();
    }
  }

  // Calculate unset vertex tags, unset edge tags, edge sector coefficients.
  if (false == bIsInset)
    this->UpdateAllTagsAndSectorCoefficients(true);

#if defined(MYON_DEBUG)
  if ( false == bIsInset)
    IsValid();
#endif

  this->ChangeGeometryContentSerialNumberForExperts(false);

  // number of moved faces and new faces created by extruding edges
  return moved_face_count + extruded_edge_count;
}

unsigned int MYON_SubD::SetVertexTags(
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  MYON_SubDVertexTag vertex_tag
)
{
  if (
    MYON_SubDVertexTag::Smooth != vertex_tag
    && MYON_SubDVertexTag::Crease != vertex_tag
    && MYON_SubDVertexTag::Corner != vertex_tag
    )
    return 0;

  if (
    ci_count <= 0
    || nullptr == ci_list
    || VertexCount() <= 0
    )
    return 0;

  MYON_SimpleArray<MYON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return 0; // nothing to change

  return SetVertexTags(
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    vertex_tag
    );
}

unsigned int MYON_SubD::SetVertexTags(
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  MYON_SubDVertexTag vertex_tag
)
{
  if (
    MYON_SubDVertexTag::Smooth != vertex_tag
    && MYON_SubDVertexTag::Crease != vertex_tag
    && MYON_SubDVertexTag::Corner != vertex_tag
    )
    return 0;

  if (cptr_count <= 0 || nullptr == cptr_list)
    return 0;

  MYON_SubDComponentMarksClearAndRestore mark_guard(*this);
  // be less generous with markbits.
  ClearComponentMarkBits(true, true, true);

  const MYON__UINT8 smooth_bias = 1;
  const MYON__UINT8 crease_bias = 2;
  

  const bool bNewVertexTagIsSmooth = (MYON_SubDVertexTag::Smooth == vertex_tag);

  // count and mark vertex candidates
  // mark edges that may need to have their tag changed
  unsigned int candidate_count = 0;
  for (size_t i = 0; i < cptr_count; i++)
  {
    MYON_SubDVertex* vertex = cptr_list[i].Vertex();
    if (nullptr == vertex)
      continue;
    if (vertex->m_vertex_tag == vertex_tag)
      continue;

    //const bool bRemoveCorner = MYON_SubDVertexTag::Corner == vertex->m_vertex_tag;

    if (MYON_SubDVertexTag::Corner != vertex_tag)
    {
      // new vertex_tag is Smooth or Crease
      if (nullptr == vertex->m_edges || vertex->m_edge_count < 2)
        continue;

      const MYON_SubDVertexEdgeProperties ep = vertex->EdgeProperties();

      if (ep.m_nonmanifold_edge_count > 0)
        continue; // nonmanifold edge

      if (ep.m_boundary_edge_count + ep.m_wire_edge_count > 2)
        continue;

      if (MYON_SubDVertexTag::Smooth == vertex_tag)
      {
        if (ep.m_interior_edge_count != vertex->m_edge_count)
          continue;
      }
      else if (MYON_SubDVertexTag::Crease == vertex_tag)
      {
        if (2 == ep.m_crease_edge_count)
        {
          // attempt change - further refinement may be needed here
        }
        else if (2 == ep.m_boundary_edge_count)
        {
          // attempt change - further refinement may be needed here
        }
        else if (2 == ep.m_wire_edge_count)
        {
          // attempt change - further refinement may be needed here
        }
        else
        {
          // dont' attempt change - further refinement may be needed here
          continue;
        }
      }
    }

    candidate_count++;
    vertex->m_status.SetRuntimeMark();
    if (nullptr != vertex->m_edges)
    {
      if (MYON_SubDVertexTag::Corner == vertex_tag)
      {
        const unsigned int crease_count = vertex->EdgeCount(MYON_SubDEdgeTag::Crease);
        if (crease_count >= 2)
          continue; // do not crease additional edges
      }

      for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
      {
        const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
        if (nullptr == edge)
          continue;
        if (bNewVertexTagIsSmooth)
        {
          // new vertex_tag is Smooth
          if (edge->IsSmoothNotX())
            continue;
        }
        else
        {
          // new vertex_tag is Crease or Corner
          if (edge->IsCrease())
            continue;
        }

        // This edge tag will need to be changed
        edge->m_status.SetRuntimeMark();
      }
    }
  }

  if (0 == candidate_count)
    return 0;

  bool bUpdateTags = (MYON_SubDVertexTag::Crease != vertex_tag);

  // This for loop is used when new vertex_tag is MYON_SubDVertexTag::Crease.
  for (int pass = 0; pass < 2 && false == bUpdateTags; pass++)
  {
    // More careful analysis is neeeded to accurately mark smooth edges that will become creases
    MYON_SubDEdgeIterator eit(*this);
    for (const MYON_SubDEdge* edge = eit.FirstEdge(); nullptr != edge; edge = eit.NextEdge())
    {
      if (false == edge->m_status.RuntimeMark())
        continue;
      
      edge->m_status.ClearRuntimeMark();
      if (false == edge->IsSmooth())
        continue;

      const MYON_SubDVertex* v[2] = { edge->m_vertex[0], edge->m_vertex[1] };
      if (nullptr == v[0] || nullptr == v[1])
        continue;

      const MYON_SubDVertexTag vtag[2] = {
        (v[0]->m_status.RuntimeMark() ? vertex_tag : v[0]->m_vertex_tag),
        (v[1]->m_status.RuntimeMark() ? vertex_tag : v[1]->m_vertex_tag)
      };

      // At least one of v[0] and v[1] had m_vertex_tag changed.
      const bool bGettingSmoother =
        (v[0]->IsCorner() && MYON_SubDVertexTag::Corner != vertex_tag)
        ||
        (v[1]->IsCorner() && MYON_SubDVertexTag::Corner != vertex_tag)
        ||
        (v[0]->IsDartOrCreaseOrCorner() && MYON_SubDVertexTag::Smooth == vertex_tag)
        ||
        (v[1]->IsDartOrCreaseOrCorner() && MYON_SubDVertexTag::Smooth == vertex_tag)
        ;

      MYON_SubDEdgeTag edge_tag;
      for (;;)
      {
        if (2 != edge->m_face_count)
        {
          edge_tag = MYON_SubDEdgeTag::Crease;
          break;
        }

        if (bGettingSmoother)
        {
          edge_tag = MYON_SubDEdgeTag::Smooth;
          break;
        }

        if (2 == v[0]->m_edge_count && (MYON_SubDVertexTag::Crease == vtag[0] || MYON_SubDVertexTag::Corner == vtag[0]))
        {
          edge_tag = MYON_SubDEdgeTag::Crease;
          break;
        }

        if (2 == v[1]->m_edge_count && (MYON_SubDVertexTag::Crease == vtag[1] || MYON_SubDVertexTag::Corner == vtag[1]))
        {
          edge_tag = MYON_SubDEdgeTag::Crease;
          break;
        }

        if (
          (MYON_SubDVertexTag::Crease == vtag[0] || MYON_SubDVertexTag::Corner == vtag[0] || MYON_SubDVertexTag::Dart == vtag[0])
          &&
          (MYON_SubDVertexTag::Crease == vtag[1] || MYON_SubDVertexTag::Corner == vtag[1] || MYON_SubDVertexTag::Dart == vtag[1])
          )
        {
          edge_tag = MYON_SubDEdgeTag::Crease;
          break;
        }

        edge_tag = MYON_SubDEdgeTag::Smooth;
        break;
      }

      if (MYON_SubDEdgeTag::Crease == edge_tag)
      {
        edge->m_status.SetRuntimeMark();
        edge->SetMarkBits(crease_bias);
      }
      else
      {
        edge->SetMarkBits(smooth_bias);
      }
    }

    // make sure new crease vertices will have the right number of creased edges
    bUpdateTags = true;
    for (size_t i = 0; i < cptr_count; i++)
    {
      MYON_SubDVertex* vertex = cptr_list[i].Vertex();
      if (nullptr == vertex)
        continue;
      if (false == vertex->m_status.RuntimeMark())
        continue;
      unsigned int crease_count = 0;
      unsigned int marked_count = 0;
      for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
      {
        const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
        if (nullptr == edge)
          continue;
        if (edge->IsCrease())
          ++crease_count;
        else if (edge->m_status.RuntimeMark())
          ++marked_count;
      }
      if (crease_count + marked_count <= 2 && (0 != crease_count || 0 != marked_count))
        continue;

      if (pass > 0)
        return 0;
      bUpdateTags = false;
      if (2 == crease_count)
      {
        for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
        {
          const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
          if (nullptr != edge)
            edge->m_status.ClearRuntimeMark();
        }
      }
      else
      {
        vertex->m_status.ClearRuntimeMark();
        candidate_count--;
      }
    }

    if (0 == candidate_count)
      return 0;
  }

  if (false == bUpdateTags)
    return 0;

  unsigned int changed_vertex_count = 0;
  for (size_t i = 0; i < cptr_count; i++)
  {
    MYON_SubDVertex* vertex = cptr_list[i].Vertex();
    if (nullptr == vertex)
      continue;
    if (false == vertex->m_status.RuntimeMark())
      continue;
    changed_vertex_count++;
    vertex->m_vertex_tag = vertex_tag;
    vertex->VertexModifiedNofification();
    if (MYON_SubDVertexTag::Corner == vertex_tag && vertex->EdgeCount(MYON_SubDEdgeTag::Crease) >= 2)
    {
      // do not crease additional edges
      continue; 
    }
    for (unsigned short vei = 0; vei < vertex->m_edge_count; vei++)
    {
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
      if (nullptr == edge)
        continue;
      if ( false == bNewVertexTagIsSmooth
        && edge->m_status.RuntimeMark() 
        && false == edge->IsCrease()
        )
      {
        const_cast<MYON_SubDEdge*>(edge)->m_edge_tag = MYON_SubDEdgeTag::Crease;
        edge->EdgeModifiedNofification();
      }
      edge->m_status.SetRuntimeMark();


      const MYON_SubDVertex* other_vertex = edge->OtherEndVertex(vertex);
      other_vertex->m_status.SetRuntimeMark();

      if ( false == bNewVertexTagIsSmooth
        && MYON_SubDEdgeTag::Crease == edge->m_edge_tag
        && other_vertex->IsSmooth()
        )
      {
        const_cast<MYON_SubDVertex*>(other_vertex)->m_vertex_tag = MYON_SubDVertexTag::Dart;
        other_vertex->VertexModifiedNofification();
      }
    }
  }

  if (0 == changed_vertex_count)
    return 0;

  MYON_SubDEdgeIterator eit(*this);
  for (const MYON_SubDEdge* edge = eit.FirstEdge(); nullptr != edge; edge = eit.NextEdge())
  {
    if (false == edge->m_status.RuntimeMark())
      continue;
    const MYON_SubDVertex* v[2] = { edge->m_vertex[0], edge->m_vertex[1] };
    if (nullptr == v[0] || nullptr == v[1])
      continue;

    MYON_SubDEdgeTag edge_tag;
    if (v[0]->IsDartOrCreaseOrCorner() && v[1]->IsDartOrCreaseOrCorner())
      edge_tag = (smooth_bias == edge->MarkBits()) ? MYON_SubDEdgeTag::SmoothX : MYON_SubDEdgeTag::Crease;
    else
      edge_tag = MYON_SubDEdgeTag::Smooth;

    if (edge->m_edge_tag == edge_tag)
      continue;
    const_cast<MYON_SubDEdge*>(edge)->m_edge_tag = edge_tag;
    edge->EdgeModifiedNofification();
  }

  MYON_SubDVertexIterator vit(*this);
  for (const MYON_SubDVertex* vertex = vit.FirstVertex(); nullptr != vertex; vertex = vit.NextVertex())
  {
    if (false == vertex->m_status.RuntimeMark())
      continue;
    const unsigned int crease_count = vertex->EdgeCount(MYON_SubDEdgeTag::Crease);
    MYON_SubDVertexTag vtag = vertex->m_vertex_tag;
    if (2 == crease_count)
    {
      if ( false == vertex->IsCreaseOrCorner() )
        vtag = MYON_SubDVertexTag::Crease;
    }
    else if (1 == crease_count)
      vtag = MYON_SubDVertexTag::Dart;
    else if (crease_count > 2)
      vtag = MYON_SubDVertexTag::Corner;
    else
      vtag = MYON_SubDVertexTag::Smooth;
    if (vertex->m_vertex_tag == vtag)
      continue;
    const_cast<MYON_SubDVertex*>(vertex)->m_vertex_tag = vtag;
    vertex->ClearSavedSubdivisionPoints();
  }

  ClearComponentMarkBits(true, true, true);
  ClearEvaluationCache();
  UpdateAllTagsAndSectorCoefficients(false);

  return changed_vertex_count;
}

unsigned int MYON_SubD::SetEdgeTags(
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count,
  MYON_SubDEdgeTag edge_tag
)
{
  if (MYON_SubDEdgeTag::Smooth != edge_tag && MYON_SubDEdgeTag::Crease != edge_tag)
    return 0;

  if (
    ci_count <= 0
    || nullptr == ci_list
    || EdgeCount() <= 0
    )
    return 0;

  MYON_SimpleArray<MYON_SubDComponentPtr> cptr_list;
  if (ComponentPtrFromComponentIndex(ci_list,ci_count,cptr_list) <= 0)
    return 0; // nothing to change

  return SetEdgeTags(
    cptr_list.Array(),
    cptr_list.UnsignedCount(),
    edge_tag
    );
}

unsigned int MYON_SubD::SetEdgeTags(
  const MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list,
  MYON_SubDEdgeTag edge_tag
)
{
  return this->SetEdgeTags(cptr_list.Array(), cptr_list.Count(), edge_tag);
}

unsigned int MYON_SubD::SetEdgeTags(
  const MYON_SubDComponentPtr* cptr_list,
  size_t cptr_count,
  MYON_SubDEdgeTag edge_tag
)
{
  if (MYON_SubDEdgeTag::Smooth != edge_tag && MYON_SubDEdgeTag::Crease != edge_tag)
    return 0;

  if (
    cptr_count <= 0
    || nullptr == cptr_list
    || EdgeCount() <= 0
    )
    return 0;

  unsigned int changed_edge_count = 0;

  const bool bChangeToSmooth = (MYON_SubDEdgeTag::Smooth == edge_tag) ? true : false;

  for (size_t i = 0; i < cptr_count; i++)
  {
    MYON_SubDEdge* edge = cptr_list[i].Edge();
    if (nullptr == edge)
      continue;
    if (bChangeToSmooth)
    {
      if ( edge->IsSmoothNotSharp())
        continue; // already smooth
      if (2 != edge->FaceCount())
        continue; // can't be smooth
    }
    else
    {
      if (edge->IsCrease())
        continue; // already a crease
    }

    edge->EdgeModifiedNofification();

    changed_edge_count++;
    edge->m_edge_tag = edge_tag;
    edge->ClearSharpnessForExperts();
    edge->UnsetSectorCoefficientsForExperts();
    for (int evi = 0; evi < 2; evi++)
    {
      MYON_SubDVertex* v = const_cast<MYON_SubDVertex*>(edge->m_vertex[evi]);
      if (nullptr == v)
        continue;
      v->m_vertex_tag = MYON_SubDVertexTag::Unset;
      v->ClearSavedSubdivisionPoints();
    }
  }

  if (0 == changed_edge_count)
    return 0;

  ClearEvaluationCache();

  MYON_SubDVertexIterator vit(*this);
  for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
  {
    if (MYON_SubDVertexTag::Unset != v->m_vertex_tag)
      continue;
    unsigned crease_count = 0;
    const unsigned vertex_edge_count = v->EdgeCount();
    for (unsigned vei = 0; vei < vertex_edge_count; vei++)
    {
      const MYON_SubDEdge* e = v->Edge(vei);
      if (nullptr == e)
        continue;
      if (e->IsCrease())
      {
        crease_count++;
        if (crease_count > 2)
          break;
      }
    }
    MYON_SubDVertexTag vertex_tag;
    switch (crease_count)
    {
    case 0:
      vertex_tag = MYON_SubDVertexTag::Smooth;
      break;
    case 1:
      vertex_tag = MYON_SubDVertexTag::Dart;
      break;
    case 2:
      vertex_tag = MYON_SubDVertexTag::Crease;
      break;
    default:
      vertex_tag = MYON_SubDVertexTag::Corner;
      break;
    }
    if (v->m_vertex_tag != vertex_tag)
    {
      const_cast<MYON_SubDVertex*>(v)->m_vertex_tag = vertex_tag;
      v->ClearSavedSubdivisionPoints();
    }
  }

  MYON_SubDEdgeIterator eit(*this);
  for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    MYON_SubDEdgeTag e_tag = e->m_edge_tag;
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      e_tag = MYON_SubDEdgeTag::Unset;
    else if (MYON_SubDEdgeTag::Smooth == e_tag
      && MYON_SubDVertexTag::Smooth != e->m_vertex[0]->m_vertex_tag
      && MYON_SubDVertexTag::Smooth != e->m_vertex[1]->m_vertex_tag)
      e_tag = MYON_SubDEdgeTag::Unset;
    if (e_tag != e->m_edge_tag)
    {
      const_cast<MYON_SubDEdge*>(e)->m_edge_tag = e_tag;
      e->UnsetSectorCoefficientsForExperts();
      e->ClearSavedSubdivisionPoints();
    }
  }

  UpdateAllTagsAndSectorCoefficients(false);
  return changed_edge_count;
}

unsigned int MYON_SubD::RemoveAllCreases()
{
  unsigned int changed_count = 0;
  MYON_SubDEdgeIterator eit(*this);
  for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    if ( (false == e->IsCrease() && false == e->IsSharp()) || 2 != e->m_face_count)
      continue;
    const_cast<MYON_SubDEdge*>(e)->ClearSharpnessForExperts();
    const_cast<MYON_SubDEdge*>(e)->m_edge_tag = MYON_SubDEdgeTag::Smooth;
    e->UnsetSectorCoefficientsForExperts();
    for (int evi = 0; evi < 2; evi++)
    {
      if (nullptr == e->m_vertex[evi])
        continue;
      const_cast<MYON_SubDVertex*>(e->m_vertex[evi])->m_vertex_tag = MYON_SubDVertexTag::Unset;
    }
    ++changed_count;
  }

  if (changed_count > 0)
  {
    this->DestroyRuntimeCache(true);
    this->UpdateAllTagsAndSectorCoefficients(true);
  }

  return changed_count;
}

const MYON_SubDEdgePtr MYON_SubDEdgeChain::EdgeChainNeighbor(MYON_SubDEdgePtr starting_edge, MYON_ChainDirection search_direction, MYON_SubD::ChainType chain_type)
{
  return MYON_SubDEdgeChain::EdgeChainNeighbor(
    starting_edge,
    search_direction,
    chain_type,
    false,
    MYON_ComponentStatus::NoneSet,
    MYON_ComponentStatus::NoneSet
  );
}

const MYON_SubDEdgePtr MYON_SubDEdgeChain::EdgeChainNeighbor(
    MYON_SubDEdgePtr starting_edge,
    MYON_ChainDirection search_direction,
    MYON_SubD::ChainType chain_type,
    bool bEnableStatusCheck,
    MYON_ComponentStatus status_pass,
    MYON_ComponentStatus status_fail
  )
{
  for (;;)
  {
    if (MYON_ChainDirection::Previous != search_direction && MYON_ChainDirection::Next != search_direction)
      break;
    const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(starting_edge.m_ptr);
    if (nullptr == edge)
      break;
    bool bReverse = (MYON_ChainDirection::Previous == search_direction);    
    if (0 != MYON_SUBD_EDGE_DIRECTION(starting_edge.m_ptr))
      bReverse = !bReverse;
    const MYON_SubDVertex* v = edge->m_vertex[bReverse ? 0 : 1];
    if (nullptr == v)
      break;
    if (v->m_edge_count <= 1 || nullptr == v->m_edges)
      break;
    
    const bool bIsSmooth = edge->IsSmooth();
    const bool bIsCrease = edge->IsCrease() || 2 != edge->m_face_count;
    if (bIsSmooth != (bIsCrease?false:true))
      break;

    const unsigned short vertex_edge_count 
      = ((MYON_SubD::ChainType::EqualEdgeTagAndOrdinary == chain_type || MYON_SubD::ChainType::EqualEdgeAndVertexTagAndOrdinary == chain_type)
        && (1 == edge->m_face_count || 2 == edge->m_face_count))
        ? (edge->m_face_count + 2)
        : ((unsigned short)0);

    if (vertex_edge_count > 0 && vertex_edge_count != v->m_edge_count)
      break;

    if (MYON_SubD::ChainType::EqualEdgeAndVertexTag == chain_type || MYON_SubD::ChainType::EqualEdgeAndVertexTagAndOrdinary == chain_type)
    {
      if (bIsSmooth)
      {
        // edge is smooth so vertex must be smooth
        if (MYON_SubDVertexTag::Smooth != v->m_vertex_tag)
          break;
      }
      else
      {
        // edge is crease so vertex must be crease
        if (MYON_SubDVertexTag::Crease != v->m_vertex_tag)
          break;
      }
    }

    // Look for a single neighbor with same crease/smooth property and same face count
    // This lets chains turn the right way when there are both creases and smooth
    // edges.
    const MYON_SubDEdge* nxt = nullptr;
    for (unsigned short vei = 0; vei < v->m_edge_count; vei++)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
      if (edge == e)
        continue;
      if (bIsSmooth != e->IsSmooth())
        continue;
      if (bIsCrease != e->IsCrease())
        continue;
      if (e->m_face_count != edge->m_face_count)
        continue;
      if (e->m_vertex[0] != v && e->m_vertex[1] != v)
        continue; // bogus edge
      if (nullptr == nxt)
      {
        nxt = e;
        continue;
      }
      // ambiguous options here
      nxt = nullptr;
      break;
    }
    const int nxt_connecting_vertex_index = (MYON_ChainDirection::Next == search_direction) ? 0 : 1;
    if (nullptr != nxt)
    {
      if (false == bEnableStatusCheck || MYON_ComponentStatus::StatusCheck(nxt->m_status, status_pass, status_fail))
        return MYON_SubDEdgePtr::Create(nxt, (v == nxt->m_vertex[nxt_connecting_vertex_index]) ? 0 : 1);
    }

    if (2 != edge->m_face_count || 4 != v->m_edge_count)
      break;
    const MYON_SubDFace* f[2] = { MYON_SUBD_FACE_POINTER(edge->m_face2[0].m_ptr),MYON_SUBD_FACE_POINTER(edge->m_face2[1].m_ptr) };
    if (nullptr == f[0] || nullptr == f[1] || f[0] == f[1])
      break;
    for (unsigned short vei = 0; vei < v->m_edge_count; vei++)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
      if (edge == e)
        continue;
      if (2 != e->m_face_count)
        continue;
      if (e->m_vertex[0] != v && e->m_vertex[1] != v)
        continue; // bogus edge
      const MYON_SubDFace* nxtf[2] = { MYON_SUBD_FACE_POINTER(e->m_face2[0].m_ptr),MYON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr) };
      if (nullptr == nxtf[0] || nullptr == nxtf[1] || nxtf[0] == nxtf[1])
        continue;
      if (f[0] == nxtf[0] || f[1] == nxtf[0])
        continue;
      if (f[0] == nxtf[1] || f[1] == nxtf[1])
        continue;
      if (nullptr == nxt)
      {
        nxt = e;
        continue;
      }
      // ambiguous options here
      nxt = nullptr;
      break;
    }
    if (nullptr != nxt)
    {
      if (bIsSmooth != nxt->IsSmooth())
      {
        // edge tag changed
        if ( 
          MYON_SubD::ChainType::EqualEdgeTag == chain_type 
          || MYON_SubD::ChainType::EqualEdgeAndVertexTag == chain_type
          || MYON_SubD::ChainType::EqualEdgeTagAndOrdinary == chain_type
          || MYON_SubD::ChainType::EqualEdgeAndVertexTagAndOrdinary == chain_type
          )
          break;
      }
      if (false == bEnableStatusCheck || MYON_ComponentStatus::StatusCheck(nxt->m_status, status_pass, status_fail))
        return MYON_SubDEdgePtr::Create(nxt, (v == nxt->m_vertex[nxt_connecting_vertex_index]) ? 0 : 1);
    }

    break;
  }

  return MYON_SubDEdgePtr::Null;
}

void MYON_UniqueTester::Block::DeleteBlock(Block* blk)
{
  if (nullptr != blk)
    onfree(blk);
}

MYON_UniqueTester::Block* MYON_UniqueTester::Block::NewBlock()
{
  size_t sz1 = sizeof(Block);
  while (0 != sz1 % 8)
    sz1++;
  size_t sz2 = MYON_UniqueTester::Block::BlockCapacity * sizeof(m_a[0]);
  void* p = onmalloc(sz1 + sz2);
  Block* blk = new (p) Block();
  blk->m_a = (MYON__UINT_PTR*)(((char*)(p)) + sz1);
  return blk;
}

int MYON_UniqueTester::Block::Compare(MYON__UINT_PTR* lhs, MYON__UINT_PTR* rhs)
{
  if (*lhs < *rhs)
    return -1;
  if (*lhs > *rhs)
    return 1;
  return 0;
}

bool MYON_UniqueTester::Block::InBlock(size_t sorted_count,MYON__UINT_PTR x) const
{
  if (nullptr != m_a && m_count > 0)
  {
    if ( sorted_count > 0 && x >= m_a[0] && x <= m_a[sorted_count - 1])
    {
      if (nullptr != bsearch(&x, m_a, sorted_count, sizeof(m_a[0]), (int(*)(const void*, const void*))MYON_UniqueTester::Block::Compare))
        return true;
    }
    if (sorted_count < m_count)
    {
      const MYON__UINT_PTR* p = m_a + sorted_count;
      const MYON__UINT_PTR* p1 = m_a + m_count;
      while (p < p1)
      {
        if (x == *p++)
          return true;
      }
    }
  }
  return false;
}

void MYON_UniqueTester::Block::SortBlock()
{
  MYON_qsort(m_a, m_count, sizeof(m_a[0]), (int(*)(const void*, const void*))MYON_UniqueTester::Block::Compare);
}

void MYON_UniqueTester::Internal_CopyFrom(
  const MYON_UniqueTester & src
)
{
  m_block_list = nullptr;
  m_sorted_count = 0;
 
  Block* first_blk = nullptr;

  for ( Block* src_blk = src.m_block_list; nullptr != src_blk; src_blk = src_blk->m_next)
  {
    Block* blk = Block::NewBlock();
    memcpy( blk->m_a, src_blk->m_a, src_blk->m_count * sizeof(blk->m_a[0]) );
    blk->m_count = src_blk->m_count;
    if (nullptr == first_blk)
    {
      first_blk = blk;
    }
    else
    {
      blk->m_next = m_block_list;
      m_block_list = blk;
    }
  }  

  if (nullptr != first_blk)
  {
    if (src.m_sorted_count != first_blk->m_count)
      first_blk->SortBlock();
    first_blk->m_next = m_block_list;
    m_block_list = first_blk;
    m_sorted_count = first_blk->m_count;
  }
}

void MYON_UniqueTester::Internal_Destroy()
{
  Block* nxt = m_block_list;
  m_block_list = nullptr;
  m_sorted_count = 0;
  for ( Block* blk = nxt; nullptr != blk; blk = nxt)
  {
    nxt = blk->m_next;
    Block::DeleteBlock(blk);
  }  
}

MYON_UniqueTester::~MYON_UniqueTester()
{
  Internal_Destroy();
}

MYON_UniqueTester::MYON_UniqueTester(const MYON_UniqueTester& src)
{
  Internal_CopyFrom(src);
}

MYON_UniqueTester& MYON_UniqueTester::operator=(const MYON_UniqueTester& src)
{
  if (this != &src)
  {
    Internal_Destroy();
    Internal_CopyFrom(src);
  }
  return *this;
}

bool MYON_UniqueTester::InList(MYON__UINT_PTR x) const
{
  size_t sorted_count = m_sorted_count;
  for ( const Block* blk = m_block_list; nullptr != blk; blk = blk->m_next)
  {
    if (blk->InBlock(sorted_count, x))
      return true;
    sorted_count = MYON_UniqueTester::Block::BlockCapacity;
  }  
  return false;
}

bool MYON_UniqueTester::AddToList(MYON__UINT_PTR x)
{
  if (nullptr != m_block_list && m_sorted_count + 50 == m_block_list->m_count)
  {
    m_block_list->SortBlock();
    m_sorted_count = m_block_list->m_count;
  }
  if (InList(x))
    return false;
  Internal_AddValue(x);
  return true;
}


void MYON_UniqueTester::ExpertAddNewToList(MYON__UINT_PTR x)
{
  Internal_AddValue(x);
}

void MYON_UniqueTester::Internal_AddValue(MYON__UINT_PTR x)
{
  if (nullptr == m_block_list || MYON_UniqueTester::Block::BlockCapacity == m_block_list->m_count)
  {
    if (nullptr != m_block_list && m_sorted_count < MYON_UniqueTester::Block::BlockCapacity)
      m_block_list->SortBlock();
    MYON_UniqueTester::Block* blk = MYON_UniqueTester::Block::NewBlock();
    blk->m_next = m_block_list;
    m_block_list = blk;
    m_sorted_count = 0;
  }
  m_block_list->m_a[m_block_list->m_count++] = x;
  if ( 1 == m_block_list->m_count
    || (m_sorted_count+1 == m_block_list->m_count && x > m_block_list->m_a[m_sorted_count-1])
    )
    ++m_sorted_count;
}

void MYON_UniqueTester::ClearList()
{
  Internal_Destroy();
}

unsigned int MYON_UniqueTester::Count() const
{
  size_t count = 0;
  for ( const Block* blk = m_block_list; nullptr != blk; blk = blk->m_next)
  {
    count += blk->m_count;
  }
  return (unsigned int)count;
}

bool MYON_SubDEdgeChain::GetSideComponents(
  unsigned relative_side,
  MYON_SimpleArray<MYON_SubDComponentPtr>& side_components
) const
{
  return MYON_SubDEdgeChain::GetSideComponents(EdgeChain(), relative_side, side_components);
}

bool MYON_SubDEdgeChain::GetSideComponents(
  const MYON_SimpleArray<MYON_SubDEdgePtr>& edge_chain,
  unsigned relative_side,
  MYON_SimpleArray<MYON_SubDComponentPtr>& side_components
)
{
  side_components.SetCount(0);
  if (relative_side > 1U)
    return false;

  const unsigned edge_count = edge_chain.UnsignedCount();
  if (edge_count < 1)
    return false;

  if (false == MYON_SubDEdgeChain::IsValidEdgeChain(edge_chain, true))
    return false; // bozo vaccine

  const bool bClosedLoop = edge_count >= 3 && edge_chain[0].RelativeVertex(0) == edge_chain[edge_count - 1].RelativeVertex(1);

  MYON_SubDSectorIterator sit;
  const MYON__INT_PTR sitdir = ((unsigned)1 - relative_side);

  MYON_SubDEdgePtr chain_eptr1 = MYON_SubDEdgePtr::Null;
  const MYON_SubDFace* f1 = nullptr;
  const MYON_SubDVertex* v = nullptr;
  if (false == bClosedLoop)
  {
    // first edge/face pair at the chain's initial vertex
    chain_eptr1 = edge_chain[0];
    f1 = chain_eptr1.RelativeFace(relative_side);
    if (nullptr != f1)
    {
      v = chain_eptr1.RelativeVertex(0);
      if (nullptr != v && v == sit.Initialize(f1, sitdir, v))
      {
        MYON_SubDEdgePtr e = sit.CurrentEdgePtr(0);
        if (e.IsNotNull())
        {
          side_components.Append(MYON_SubDComponentPtr::Create(e));
          side_components.Append(MYON_SubDComponentPtr::Create(f1));
        }
      }
    }
  }
  else
  {
    chain_eptr1 = edge_chain[edge_count-1];
    f1 = chain_eptr1.RelativeFace(relative_side);
  }

  // first_side_components_edge is needed for closed loops.
  MYON_SubDComponentPtr first_side_components_edge = MYON_SubDComponentPtr::Null;
  for (unsigned j = bClosedLoop ? 0 : 1; j < edge_count; ++j)
  {
    chain_eptr1 = edge_chain[j];
    v = chain_eptr1.RelativeVertex(0);
    const MYON_SubDFace* f0 = f1;
    f1 = chain_eptr1.RelativeFace(relative_side);
    if (nullptr != f0 && f0 == f1)
    {
      // side_components[] = ...f0,v,f1.
      unsigned n = side_components.UnsignedCount();
      if (0 == n || f0 != side_components[n-1].Face())
        side_components.Append(MYON_SubDComponentPtr::Create(f0));
      side_components.Append(MYON_SubDComponentPtr::Create(v));
      side_components.Append(MYON_SubDComponentPtr::Create(f1));
      continue;
    }

    // sit_dex < sit_max prevents possiblity of an infinite loop if the chain or subd is corrupt
    const unsigned sit_max = v->FaceCount();
    unsigned sit_dex = 1;

    const MYON_SubDFace* f = nullptr;
    for (;;)
    {
      if (nullptr == f0)
        break;

      if (nullptr == v || v != sit.Initialize(f0, sitdir, v))
        break;

      MYON_SubDEdgePtr eptr = sit.CurrentEdgePtr(1);
      if (eptr.IsNull())
        break;

      unsigned n = side_components.UnsignedCount();
      if (0 == n || f0 != side_components[n - 1].Face())
        side_components.Append(MYON_SubDComponentPtr::Create(f0));
      side_components.Append(MYON_SubDComponentPtr::Create(eptr));
      if (first_side_components_edge.IsNull())
        first_side_components_edge = MYON_SubDComponentPtr::Create(eptr); // needed for closed loops

      // The HasInteriorEdgeTopology() checks prevent getting garbage from
      // non-oriented SubDs. If people complain, they need to orient
      // their SubDs before asking "side" questions.
      // Some math nerd yokel is sure to stuff the boundary of
      // a mobius strip boundary into this function and then complain.
      // Too bad for them. [Dale Lear 2021]
      if (eptr.HasInteriorEdgeTopology(true))
      {
        for (f = sit.NextFace(MYON_SubDSectorIterator::StopAt::Boundary), sit_dex = 1; nullptr != f && sit_dex < sit_max; f = sit.NextFace(MYON_SubDSectorIterator::StopAt::Boundary), ++sit_dex)
        {
          if (f == f0)
            break;
          eptr = sit.CurrentEdgePtr(1);
          side_components.Append(MYON_SubDComponentPtr::Create(f));
          if (f == f1)
            break;
          side_components.Append(MYON_SubDComponentPtr::Create(eptr));
          if (false == eptr.HasInteriorEdgeTopology(true))
            break;
        }
      }
      break;
    }

    if (f == f1)
      continue;

    for (;;)
    {
      if (nullptr == f1)
        break;
      if (nullptr == v || v != sit.Initialize(f1, 1-sitdir, v))
        break;
      const MYON_SubDEdgePtr eptr1 = sit.CurrentEdgePtr(1);
      const unsigned count0 = side_components.UnsignedCount();
      if (eptr1.HasInteriorEdgeTopology(true))
      {
        const MYON_SubDFace* f2 = nullptr;
        for (f = sit.NextFace(MYON_SubDSectorIterator::StopAt::Boundary), sit_dex = 1; nullptr != f && sit_dex < sit_max; f = sit.NextFace(MYON_SubDSectorIterator::StopAt::Boundary), ++sit_dex)
        {
          if (f == f0 || f == f1)
          {
            f = nullptr;
            break;
          }
          f2 = f;
          if (false == sit.CurrentEdgePtr(1).HasInteriorEdgeTopology(true))
            break;
        }
        if (nullptr == f2)
          break;
        if (v != sit.Initialize(f2, sitdir, v))
          break;
        for (f = sit.CurrentFace(), sit_dex = 1; nullptr != f && sit_dex < sit_max; f = sit.NextFace(MYON_SubDSectorIterator::StopAt::Boundary), ++sit_dex)
        {
          MYON_SubDEdgePtr eptr = sit.CurrentEdgePtr(0);
          if (f == f0 || eptr.IsNull())
          {
            f = nullptr;
            break;
          }
          side_components.Append(MYON_SubDComponentPtr::Create(eptr));
          side_components.Append(MYON_SubDComponentPtr::Create(f));
          if (f == f1)
            break;
          eptr = sit.CurrentEdgePtr(1);
          if (false == eptr.HasInteriorEdgeTopology(true))
          {
            f = nullptr;
            break;
          }
        }
        if (f != f1)
        {
          side_components.SetCount(count0);
        }
      }
      if ( count0 == side_components.UnsignedCount() && eptr1.IsNotNull() )
      {
        side_components.Append(MYON_SubDComponentPtr::Create(eptr1));
        side_components.Append(MYON_SubDComponentPtr::Create(f1));
      }
      break;
    }
  }

  if (bClosedLoop)
  {
    const unsigned side_components_count = side_components.UnsignedCount();
    if (side_components_count >= 6
      && first_side_components_edge.IsEdge()
      && side_components[0].IsFace()
      && side_components[0].Face() == side_components[side_components_count-1].Face()
      )
    {
      // rearrange side_components[] so it begins with first_side_components_eptr.
      // This elimnates the complexity of having to begin/end a SubD fillet
      // at a ... f,v,f ... location.
      const MYON_SubDFace* f = side_components[0].Face();
      if (nullptr == f)
      {
        MYON_SUBD_ERROR("f should be non-null.");
      }
      else
      {
        // set j0 = current side_components[] index of the first edge
        unsigned j0 = side_components_count;
        for (unsigned j = 0; j < side_components_count; ++j)
        {
          if (first_side_components_edge.m_ptr == side_components[j].m_ptr)
          {
            j0 = j;
            break;
          }
        }
        if (j0 > 0 && j0 < side_components_count)
        {
          MYON_SimpleArray<MYON_SubDComponentPtr> tail;
          tail.Append((int)(j0-1), side_components.Array() + 1);
          for (unsigned k = 0; k + j0 < side_components_count; ++k)
            side_components[k] = side_components[j0 + k];
          side_components.SetCount(side_components_count - j0);
          side_components.Append(tail.Count(), tail.Array());
          side_components.Append(first_side_components_edge);
        }
      }
    }
  }
  else
  {
    // last edge at the chain's final vertex
    chain_eptr1 = edge_chain[edge_count - 1];
    f1 = chain_eptr1.RelativeFace(relative_side);
    if (nullptr != f1)
    {
      v = chain_eptr1.RelativeVertex(1);
      if (nullptr != v && v == sit.Initialize(f1, sitdir, v))
      {
        MYON_SubDEdgePtr eptr = sit.CurrentEdgePtr(1);
        if (eptr.IsNotNull())
        {
          unsigned n = side_components.UnsignedCount();
          if (0 == n || f1 != side_components[n - 1].Face())
            side_components.Append(MYON_SubDComponentPtr::Create(f1));
          side_components.Append(MYON_SubDComponentPtr::Create(eptr));
        }
      }
    }
  }

  return side_components.UnsignedCount() > 0;
}

const MYON_SimpleArray<MYON_SubDEdgePtr>& MYON_SubDEdgeChain::EdgeChain() const
{
  return m_edge_chain;
}

const MYON_SubD& MYON_SubDEdgeChain::SubD() const
{
  return m_subd_ref.SubD();
}

const MYON_SubDRef MYON_SubDEdgeChain::SubDRef() const
{
  return m_subd_ref;
}

bool MYON_SubDEdgeChain::InChain(
  const MYON_SubDEdgePtr edge_ptr
) const
{
  return InChain(MYON_SUBD_EDGE_POINTER(edge_ptr.m_ptr));
}

bool MYON_SubDEdgeChain::InChain(
  const MYON_SubDEdge* edge
) const
{
  return (nullptr == edge) ? false : m_unique_tester.InList((MYON__UINT_PTR)edge);
}

bool MYON_SubDEdgeChain::InChain(
  const MYON_SubDVertex* vertex
) const
{
  return (nullptr == vertex) ? false : m_unique_tester.InList((MYON__UINT_PTR)vertex);
}

bool MYON_SubDEdgeChain::IsClosedLoop() const
{
  const unsigned int count = m_edge_chain.UnsignedCount();
  return (count >= 3 && m_edge_chain[0].RelativeVertex(0) == m_edge_chain[count - 1].RelativeVertex(1));
}

bool MYON_SubDEdgeChain::IsConvexLoop(bool bStrictlyConvex) const
{
  if (false == IsClosedLoop())
    return false;

  const unsigned int count = m_edge_chain.UnsignedCount();
  MYON_SimpleArray<MYON_3dPoint> points(count);
  for (unsigned int i = 0; i < count; ++i)  
  {
    const MYON_SubDVertex* v = m_edge_chain[i].RelativeVertex(0);
    if (nullptr == v)
      return false;
    points.Append(v->ControlNetPoint());
  }
  if ( false == (points[0] != points[count - 1]) )
    return false;

  return MYON_IsConvexPolyline(points, bStrictlyConvex);
}


unsigned int MYON_SubDEdgeChain::BeginEdgeChain(
  MYON_UUID persistent_subd_id,
  MYON_SubDRef subd_ref,
  const MYON_SubDEdge* initial_edge
)
{
  return MYON_SubDEdgeChain::BeginEdgeChain(persistent_subd_id, subd_ref, MYON_SubDEdgePtr::Create(initial_edge, 0));
}

unsigned int MYON_SubDEdgeChain::BeginEdgeChain(
  MYON_UUID persistent_subd_id,
  MYON_SubDRef subd_ref,
  const MYON_SimpleArray<const MYON_SubDEdge* >& initial_edge_chain
)
{
  return BeginEdgeChain(persistent_subd_id, subd_ref, initial_edge_chain.Array(), initial_edge_chain.UnsignedCount());
}

unsigned int MYON_SubDEdgeChain::BeginEdgeChain(
  MYON_UUID persistent_subd_id,
  MYON_SubDRef subd_ref,
  const MYON_SubDEdge*const* initial_edge_chain,
  size_t edge_count
)
{
  ClearEdgeChain();

  if (
    edge_count <= 0 
    || subd_ref.SubD().IsEmpty() 
    || subd_ref.SubD().EdgeCount() < (unsigned int)edge_count
    )
    return 0;

  if ( 1 == edge_count)
    return MYON_SubDEdgeChain::BeginEdgeChain(persistent_subd_id, subd_ref, MYON_SubDEdgePtr::Create(initial_edge_chain[0], 0));

  const MYON_SubDEdge* e0 = initial_edge_chain[0];
  if (nullptr == e0 || nullptr == e0->m_vertex[0] || nullptr == e0->m_vertex[1] )
    return 0;
  const MYON_SubDEdge* e1 = initial_edge_chain[1];
  if (nullptr == e1 || nullptr == e1->m_vertex[0] || nullptr == e1->m_vertex[1] )
    return 0;


  MYON_SubDEdgePtr eptr = MYON_SubDEdgePtr::Create(e0, (e0->m_vertex[1] == e1->m_vertex[0] || e0->m_vertex[1] == e1->m_vertex[1]) ? 0 : 1);

  MYON_SimpleArray<MYON_SubDEdgePtr> eptr_chain(edge_count);
  eptr_chain.Append(eptr);
  const MYON_SubDVertex* v = eptr.RelativeVertex(1);
  for (unsigned int i = 1; i < edge_count; i++)
  {
    e1 = initial_edge_chain[i];
    if (nullptr == e1 || nullptr == e1->m_vertex[0] || nullptr == e1->m_vertex[1] )
      return 0;
    if (v != e1->m_vertex[0] && v != e1->m_vertex[1])
      return 0;
    eptr = MYON_SubDEdgePtr::Create(e1, (v == e1->m_vertex[0]) ? 0 : 1);
    eptr_chain.Append(eptr);
  }

  return MYON_SubDEdgeChain::BeginEdgeChain(persistent_subd_id, subd_ref,eptr_chain);
}

unsigned int MYON_SubDEdgeChain::BeginEdgeChain(
  MYON_UUID persistent_subd_id,
  MYON_SubDRef subd_ref,
  MYON_SubDEdgePtr eptr
)
{
  return MYON_SubDEdgeChain::BeginEdgeChain(persistent_subd_id, subd_ref, &eptr, 1);
}

unsigned int MYON_SubDEdgeChain::BeginEdgeChain(
  MYON_UUID persistent_subd_id,
  MYON_SubDRef subd_ref,
  const MYON_SimpleArray<MYON_SubDEdgePtr>& initial_edge_chain
)
{
  return MYON_SubDEdgeChain::BeginEdgeChain(persistent_subd_id, subd_ref, initial_edge_chain.Array(), initial_edge_chain.UnsignedCount() );
}

unsigned int MYON_SubDEdgeChain::BeginEdgeChain(
  MYON_UUID persistent_subd_id,
  MYON_SubDRef subd_ref,
  const MYON_SubDEdgePtr* initial_edge_chain,
  size_t edge_count
  )
{
  ClearEdgeChain();

  m_persistent_subd_id = persistent_subd_id;
  m_subd_ref = subd_ref;

  if (edge_count <= 0 || m_subd_ref.SubD().IsEmpty())
    return 0;

  if ( ((size_t)subd_ref.SubD().EdgeCount()) < edge_count )
    return 0;

  m_edge_chain.Reserve(edge_count + 128);
  const MYON_SubDVertex* first_vertex = nullptr;
  const MYON_SubDVertex* last_vertex = nullptr;
  for (size_t i = 0; i < edge_count; i++)
  {
    const MYON_SubDEdgePtr eptr = initial_edge_chain[i];
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
    if (nullptr == e)
      continue;
    if (m_unique_tester.InList((MYON__UINT_PTR)e))
      continue;
    const MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
    const MYON_SubDVertex* v[2] = { e->m_vertex[edir], e->m_vertex[1 - edir] };
    if (nullptr == v[0] || nullptr == v[1] || v[0] == v[1] )
      continue;
    if (nullptr == first_vertex)
    {
      first_vertex = v[0];
      last_vertex = v[1];
      m_unique_tester.ExpertAddNewToList((MYON__UINT_PTR)first_vertex);
      m_unique_tester.ExpertAddNewToList((MYON__UINT_PTR)last_vertex);
    }
    else 
    {
      if (last_vertex != v[0])
        continue;
      if (v[1] != first_vertex)
      {
        if (false == m_unique_tester.AddToList((MYON__UINT_PTR)v[1]))
          continue;
      }
    }
    m_edge_chain.Append(eptr);
    m_unique_tester.ExpertAddNewToList((MYON__UINT_PTR)e);
    last_vertex = v[1];
    if (last_vertex == first_vertex)
      break;
  };

  return m_edge_chain.UnsignedCount();
}

const MYON_UUID MYON_SubDEdgeChain::PersistentSubDId() const
{
  return m_persistent_subd_id;
}

bool MYON_SubDEdgeChain::HasPersistentEdgeIds() const
{
  const unsigned count = this->EdgeCount();
  return
    count > 0
    && count == m_persistent_edge_id.UnsignedCount()
    && count == m_persistent_edge_orientation.UnsignedCount()
    ;
}

bool MYON_SubDEdgeChain::HasRuntimeEdgePtrs() const
{
  const unsigned count = this->EdgeCount();
  return
    count > 0
    && count == this->m_edge_chain.UnsignedCount()
    && m_subd_ref.SubD().EdgeCount() > 0
    ;
}

bool MYON_SubDEdgeChain::SetPersistentEdgeIdsFromRuntimeEdgePtrs() const
{
  m_persistent_edge_id.SetCount(0);
  m_persistent_edge_orientation.SetCount(0); 
  const unsigned count = (m_subd_ref.SubD().EdgeCount() > 0) ? m_edge_chain.UnsignedCount() : 0;
  bool rc = count > 0;
  if (rc)
  {
    m_persistent_edge_id.Reserve(count);
    m_persistent_edge_orientation.Reserve(count);
    for (unsigned i = 0; i < count; ++i)
    {
      const MYON_SubDEdgePtr eptr = m_edge_chain[i];
      const unsigned edge_id = eptr.EdgeId();
      if (edge_id <= 0)
        break;
      if (false == eptr.IsActive())
        break;
      m_persistent_edge_id.Append(edge_id);
      m_persistent_edge_orientation.Append(0 == eptr.EdgeDirection() ? 0 : 1);
    }
    if (count != m_persistent_edge_id.UnsignedCount() || count != m_persistent_edge_orientation.UnsignedCount())
    {
      rc = false;
      m_persistent_edge_id.SetCount(0);
      m_persistent_edge_orientation.SetCount(0);
    }
  }
  return rc;
}

bool MYON_SubDEdgeChain::SetRuntimeEdgePtrsFromPersistentSubD(
  MYON_UUID persistent_subd_id,
  MYON_SubDRef persistent_subd_ref
)
{
  bool rc = true;
  m_edge_chain.SetCount(0);
  const unsigned count = m_persistent_edge_id.UnsignedCount();
  const MYON_SubD& subd = persistent_subd_ref.SubD();
  if (count > 0 && count == m_persistent_edge_orientation.UnsignedCount() && subd.EdgeCount() > 0)
  {
    rc = false;
    MYON_SimpleArray<MYON_SubDEdgePtr> local_edge_chain(count);
    for (unsigned i = 0; i < count; ++i)
    {
      const MYON_SubDEdge* e = subd.EdgeFromId(m_persistent_edge_id[i]);
      if (nullptr == e)
        break;
      const MYON_SubDEdgePtr eptr = MYON_SubDEdgePtr::Create(e, (1 == m_persistent_edge_orientation[i]) ? 1 : 0);
      local_edge_chain.Append(eptr);
    }
    if (count == local_edge_chain.UnsignedCount())
    {
      if (MYON_SubDEdgeChain::IsValidEdgeChain(local_edge_chain, false))
      {
        m_edge_chain = local_edge_chain;
        rc = true;
      }
    }
  }
  if (persistent_subd_ref.SubD().RuntimeSerialNumber() != m_subd_ref.SubD().RuntimeSerialNumber())
    m_subd_ref = persistent_subd_ref;
  if (MYON_UuidIsNotNil(persistent_subd_id) && 0 != MYON_UuidCompare(m_persistent_subd_id, persistent_subd_id))
    m_persistent_subd_id = persistent_subd_id;
  return rc;
}

bool MYON_SubDEdgeChain::Write(class MYON_BinaryArchive& archive) const
{
  // This write has to work if a read happened but the m_edge_chain[] was never set.
  if (m_edge_chain.UnsignedCount() > 0)
    SetPersistentEdgeIdsFromRuntimeEdgePtrs();
  const unsigned count = m_persistent_edge_id.UnsignedCount();

  if (false == archive.BeginWrite3dmAnonymousChunk(1))
    return false;

  bool rc = false;
  for (;;)
  {
    if (false == archive.WriteUuid(m_persistent_subd_id))
      break;
    if (false == archive.WriteInt(count))
      break;
    if (count > 0)
    {
      if (false == archive.WriteArray(m_persistent_edge_id))
        break;
      if (false == archive.WriteArray(m_persistent_edge_orientation))
        break;
    }

    rc = true;
    break;
  }

  if (false == archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_SubDEdgeChain::Read(class MYON_BinaryArchive& archive)
{
  *this = MYON_SubDEdgeChain::Empty;

  int chunk_version = 0;
  if (false == archive.BeginRead3dmAnonymousChunk(&chunk_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (chunk_version < 1)
      break;
    if (false == archive.ReadUuid(m_persistent_subd_id))
      break;
    unsigned int count = 0;
    if (false == archive.ReadInt(&count))
      break;
    if (count > 0)
    {
      if (false == archive.ReadArray(m_persistent_edge_id))
        break;
      if (false == archive.ReadArray(m_persistent_edge_orientation))
        break;
    }

    if (count != m_persistent_edge_id.UnsignedCount() || count != m_persistent_edge_orientation.UnsignedCount())
    {
      m_persistent_edge_id.SetCount(0);
      m_persistent_edge_orientation.SetCount(0);
    }

    rc = true;
    break;
  }

  if (false == archive.EndRead3dmChunk())
    rc = false;

  // At some point, the caller will need to find the subd for this edge chain and call SetRuntimeEdgePtrsFromPersistentSubD().

  return rc;
}

void MYON_SubDEdgeChain::Dump(class MYON_TextLog& text_log) const
{
  const unsigned edge_count = EdgeCount();
  const MYON__UINT64 subd_sn = m_subd_ref.SubD().EdgeCount() > 0 ? m_subd_ref.SubD().RuntimeSerialNumber() : 0;
  const bool bSubDIdIsNotNil = MYON_UuidIsNotNil(m_persistent_subd_id);
  if (edge_count > 0 && (0 != subd_sn || bSubDIdIsNotNil))
  {
    if (0 != subd_sn)
    {
      text_log.Print(L"SubD[%" PRIu64 "]", subd_sn);
      if (bSubDIdIsNotNil)
      {
        text_log.Print(L" persistent SubD id = ");
        text_log.Print(m_persistent_subd_id);
      }
    }
    else
    {
      text_log.Print(L"Persistent SubD id = ");
      text_log.Print(m_persistent_subd_id);
    }
    text_log.Print("%u edges.\n", edge_count);

    const bool bPrintEdgePtr = (subd_sn != 0 && edge_count == m_edge_chain.UnsignedCount());
    const bool bPrintPersistentEdgeId = 
      false == bPrintEdgePtr
      && edge_count == m_persistent_edge_id.UnsignedCount()
      && edge_count == m_persistent_edge_orientation.UnsignedCount();
    MYON_TextLogIndent indent1(text_log);
    if (bPrintEdgePtr || bPrintPersistentEdgeId)
    {
      const wchar_t plus_minus[3] = { '+', '-', '?' };
      for (unsigned i = 0; i < edge_count; ++i)
      {
        if (0 != i)
          text_log.Print(L", ");
        unsigned sign_dex = 2;
        unsigned edge_id = 0;
        if (bPrintEdgePtr)
        {
          const MYON_SubDEdgePtr eptr = m_edge_chain[i];
          sign_dex = 0 != eptr.EdgeDirection() ? 1 : 0;
          edge_id = eptr.EdgeId();
        }
        else if (bPrintPersistentEdgeId)
        {
          sign_dex = 0 != m_persistent_edge_orientation[i] ? 1 : 0;
          edge_id = m_persistent_edge_id[i];
        }
        text_log.Print(L"%lce%u", plus_minus[sign_dex], edge_id);
        if (i == 5 && edge_count > 15)
        {
          text_log.Print(L", ...");
          i = edge_count - 5;
        }
      }
    }
    else
    {
      text_log.Print("Corrupt edge list.");
    }
  }
  else if( 0 == edge_count)
  {
    text_log.Print("Empty SubD edge chain.");
  }
  else
  {
    text_log.Print("Corrupt SubD edge chain.");
  }
  text_log.PrintNewLine();

}

void MYON_SubDEdgeChain::ClearEdgeChain()
{
  m_edge_chain.SetCount(0);
  m_unique_tester.ClearList();
}

unsigned int MYON_SubDEdgeChain::EdgeCount() const
{
  const unsigned edge_count = m_edge_chain.UnsignedCount();
  if (edge_count > 0)
    return edge_count;
  const unsigned id_count = (MYON_UuidIsNotNil(this->m_persistent_subd_id)) ? m_persistent_edge_id.UnsignedCount() : 0;
  if (id_count > 0 && id_count == m_persistent_edge_orientation.UnsignedCount())
    return id_count;
  return 0;
}

void MYON_SubDEdgeChain::SetStatusCheck(
  bool bEnableStatusCheck,
  MYON_ComponentStatus status_check_pass,
  MYON_ComponentStatus status_check_fail
)
{
  m_bEnableStatusCheck = bEnableStatusCheck ? true : false;
  m_status_check_pass = status_check_pass;
  m_status_check_fail = status_check_fail;  
}

bool MYON_SubDEdgeChain::StatusCheckEnabled() const
{
  return m_bEnableStatusCheck;
}

void MYON_SubDEdgeChain::Reverse()
{
  MYON_SubDEdgeChain::ReverseEdgeChain(m_edge_chain);
}

const MYON_SubDEdgePtr MYON_SubDEdgeChain::FirstEdgePtr() const
{
  return m_edge_chain.UnsignedCount() > 0 ? m_edge_chain[0] : MYON_SubDEdgePtr::Null;
}
const MYON_SubDEdgePtr MYON_SubDEdgeChain::LastEdgePtr() const
{
  return m_edge_chain.UnsignedCount() > 0 ? *(m_edge_chain.Last()) : MYON_SubDEdgePtr::Null;
}

const MYON_SubDEdgePtr MYON_SubDEdgeChain::EdgePtr(int edge_index) const
{
  return (edge_index >= 0 && edge_index < m_edge_chain.Count()) ? m_edge_chain[edge_index] : MYON_SubDEdgePtr::Null;
}

const MYON_SubDEdge* MYON_SubDEdgeChain::FirstEdge() const
{
  return FirstEdgePtr().Edge();
}

const MYON_SubDEdge* MYON_SubDEdgeChain::LastEdge() const
{
  return LastEdgePtr().Edge();
}

const MYON_SubDEdge* MYON_SubDEdgeChain::Edge(int edge_index) const
{
  return EdgePtr(edge_index).Edge();
}

const MYON_SubDVertex* MYON_SubDEdgeChain::FirstVertex() const
{
  return Vertex(0);
}

const MYON_SubDVertex* MYON_SubDEdgeChain::LastVertex() const
{
  return Vertex(m_edge_chain.Count());
}

const MYON_SubDVertex* MYON_SubDEdgeChain::Vertex(int vertex_index) const
{
  const int edge_count = m_edge_chain.Count();
  if ( vertex_index >= 0 && vertex_index <= edge_count && edge_count > 0 )
  {
    return 
      (vertex_index == edge_count)
      ? m_edge_chain[edge_count - 1].RelativeVertex(1)
      : m_edge_chain[vertex_index].RelativeVertex(0);
  }
  return nullptr;
}

const MYON_3dPoint MYON_SubDEdgeChain::FirstControlNetPoint() const
{
  const MYON_SubDVertex* v = FirstVertex();
  return (nullptr != v) ? v->ControlNetPoint() : MYON_3dPoint::NanPoint;
}

const MYON_3dPoint MYON_SubDEdgeChain::LastControlNetPoint() const
{
  const MYON_SubDVertex* v = LastVertex();
  return (nullptr != v) ? v->ControlNetPoint() : MYON_3dPoint::NanPoint;
}

const MYON_3dPoint MYON_SubDEdgeChain::ControlNetPoint(int vertex_index) const
{
  const MYON_SubDVertex* v = Vertex(vertex_index);
  return (nullptr != v) ? v->ControlNetPoint() : MYON_3dPoint::NanPoint;
}



unsigned int MYON_SubDEdgeChain::AddOneNeighbor(
  MYON_ChainDirection direction,
  MYON_SubD::ChainType chain_type
)
{
  const unsigned int count0 = m_edge_chain.UnsignedCount();
  if (count0 <= 0 || IsClosedLoop() )
    return 0;

  MYON_SubDEdgePtr eptr;
  const MYON_SubDEdge* e;
  const MYON_SubDVertex* v;

  const MYON_SubDVertex* chain_ends[2] = { FirstVertex() ,LastVertex() };

  eptr
    = (MYON_ChainDirection::Previous != direction)
    ? MYON_SubDEdgeChain::EdgeChainNeighbor(LastEdgePtr(), MYON_ChainDirection::Next, chain_type, m_bEnableStatusCheck, m_status_check_pass, m_status_check_fail)
    : MYON_SubDEdgePtr::Null;
  e = eptr.Edge();
  v = eptr.RelativeVertex(0);
  if ( nullptr != v && v == chain_ends[1] && false == InChain(e) )
  {
    v = eptr.RelativeVertex(1);
    if (v == chain_ends[0] || m_unique_tester.AddToList((MYON__UINT_PTR)v))
    {
      m_unique_tester.ExpertAddNewToList((MYON__UINT_PTR)e);
      m_edge_chain.Append(eptr);
    }
  }

  eptr
    = (MYON_ChainDirection::Next != direction)
    ? MYON_SubDEdgeChain::EdgeChainNeighbor(FirstEdgePtr(), MYON_ChainDirection::Previous, chain_type, m_bEnableStatusCheck, m_status_check_pass, m_status_check_fail)
    : MYON_SubDEdgePtr::Null;
  e = eptr.Edge();
  v = eptr.RelativeVertex(1);
  if ( nullptr != v && v == chain_ends[0] && false == InChain(e) )
  {
    v = eptr.RelativeVertex(0);
    if (v == chain_ends[1] || m_unique_tester.AddToList((MYON__UINT_PTR)v))
    {
      m_unique_tester.ExpertAddNewToList((MYON__UINT_PTR)e);
      m_edge_chain.Insert(0, eptr);
    }
  }

  return m_edge_chain.UnsignedCount() - count0;
}

unsigned int MYON_SubDEdgeChain::AddAllNeighbors(
  MYON_ChainDirection direction,
  MYON_SubD::ChainType chain_type
)
{
  const unsigned int count0 = m_edge_chain.UnsignedCount();
  if (count0 <= 0 || IsClosedLoop())
    return 0;

  if (MYON_ChainDirection::Previous != direction)
    while (1 == AddOneNeighbor(MYON_ChainDirection::Next, chain_type)) {}      
  if (MYON_ChainDirection::Next != direction)
    while (1 == AddOneNeighbor(MYON_ChainDirection::Previous, chain_type)) {}      

  return m_edge_chain.UnsignedCount() - count0;
}

unsigned int MYON_SubDEdgeChain::AddEdge(
  const MYON_SubDEdge* edge
) 
{
  const unsigned int count0 = m_edge_chain.UnsignedCount();
  if (count0 <= 0)
    return 0;

  if (
    nullptr == edge 
    || nullptr == edge->m_vertex[0]
    || nullptr == edge->m_vertex[1] 
    || edge->m_vertex[0] == edge->m_vertex[1]
    )
    return 0;

  const MYON_SubDVertex* v[2] = { FirstVertex(),LastVertex() };
  if (v[0] == v[1])
    return 0;

  if ( m_bEnableStatusCheck && false == MYON_ComponentStatus::StatusCheck(edge->m_status, m_status_check_pass, m_status_check_fail))
    return 0;

  MYON_SubDEdgePtr eptr = MYON_SubDEdgePtr::Null;

  if (v[1] == edge->m_vertex[0])
    eptr = MYON_SubDEdgePtr::Create(edge, 0);
  else if (v[1] == edge->m_vertex[1])
    eptr = MYON_SubDEdgePtr::Create(edge, 1);
  else if (v[0] == edge->m_vertex[1])
    eptr = MYON_SubDEdgePtr::Create(edge, 0);
  else if (v[0] == edge->m_vertex[0])
    eptr = MYON_SubDEdgePtr::Create(edge, 1);
  else
    return 0;

  if (m_unique_tester.InList((MYON__UINT_PTR)edge))
    return 0;

  if (v[1] == eptr.RelativeVertex(0) )
  {
    if (v[0] == eptr.RelativeVertex(1) || m_unique_tester.AddToList((MYON__UINT_PTR)eptr.RelativeVertex(1)))
    {
      m_edge_chain.Append(eptr);
      m_unique_tester.ExpertAddNewToList((MYON__UINT_PTR)edge);
    }
  }
  else if (v[0] == eptr.RelativeVertex(1) )
  {
    if (v[1] == eptr.RelativeVertex(0) || m_unique_tester.AddToList((MYON__UINT_PTR)eptr.RelativeVertex(0)))
    {
      m_edge_chain.Insert(0, eptr);
      m_unique_tester.ExpertAddNewToList((MYON__UINT_PTR)edge);
    }
  }

  return m_edge_chain.UnsignedCount() - count0;
}

unsigned int MYON_SubDEdgeChain::RemoveEdges(
  const MYON_SubDEdge* first_edge,
  const MYON_SubDEdge* last_edge
) 
{
  unsigned int count0 = m_edge_chain.UnsignedCount();
  unsigned int i0 = 0;
  unsigned int i1 = count0;
  if (nullptr != first_edge)
  {
    while (i0 < count0 && first_edge != m_edge_chain[i0].Edge())
      ++i0;
  }
  if (nullptr != last_edge)
  {
    while (i1 > i0 && last_edge != m_edge_chain[i1 - 1].Edge())
      --i1;
  }

  const unsigned int count1 = i1 - i0;
  if (count1 >= count0)
    return 0;

  if (i0 > 0)
  {
    for (unsigned int i = i0; i < i1; i++)
      m_edge_chain[i - i0] = m_edge_chain[i];
  }

  m_edge_chain.SetCount(count1);
  m_unique_tester.ClearList();
  for (unsigned int i = 0; i < count1; i++)
  {
    m_unique_tester.AddToList((MYON__UINT_PTR)m_edge_chain[i].Edge());
    m_unique_tester.AddToList((MYON__UINT_PTR)m_edge_chain[i].RelativeVertex(0));
  }
  if ( FirstVertex() != LastVertex() )
    m_unique_tester.AddToList((MYON__UINT_PTR)LastVertex());

  return count0 - count1;
}


void MYON_SubDEdgeChain::ReverseEdgeChain(
  MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chain
)
{
  MYON_SubDEdgeChain::ReverseEdgeChain(edge_chain.Array(), edge_chain.UnsignedCount());
}

void MYON_SubDEdgeChain::ReverseEdgeChain(
  MYON_SubDEdgePtr* edge_chain,
  size_t edge_count
  )
{
  if (edge_count <= 0 || nullptr == edge_chain)
    return;
  MYON_SubDEdgePtr* p0 = edge_chain;
  MYON_SubDEdgePtr* p1 = p0 + (edge_count - 1);
  while ( p0 < p1)
  {
    MYON_SubDEdgePtr eptr = p0->Reversed();
    *p0 = p1->Reversed();
    *p1 = eptr;
    ++p0;
    --p1;
  }
  if (p0 == p1)
    *p0 = p0->Reversed();
}

bool MYON_SubDEdgeChain::IsValidEdgeChain(
  const MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chain,
  bool bCheckForDuplicateEdges
)
{
  return MYON_SubDEdgeChain::IsValidEdgeChain(edge_chain.Array(), edge_chain.UnsignedCount(), bCheckForDuplicateEdges);
}

bool MYON_SubDEdgeChain::IsValidEdgeChain(
  const MYON_SubDEdgePtr* edge_chain,
  size_t edge_count,
  bool bCheckForDuplicateEdges
)
{
  if (edge_count <= 0)
    return true;
  if (nullptr == edge_chain)
    return false;

  const MYON_SubDVertex* first_vertex = edge_chain->RelativeVertex(0);
  if (nullptr == first_vertex)
    return false;

  const MYON_SubDVertex* v = first_vertex;
  const MYON_SubDEdgePtr* p0 = edge_chain;
  const MYON_SubDEdgePtr* p1 = edge_chain+1;

  for (const MYON_SubDEdgePtr* p = p0; p < p1; ++p)
  {
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(p->m_ptr);
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      return false;
    MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(p->m_ptr);
    const MYON_SubDVertex* v0 = e->m_vertex[edir];
    const MYON_SubDVertex* v1 = e->m_vertex[1 - edir];
    if (v0 != v || nullptr == v1 || v0 == v1)
      return false;
    v = v1;
  }

  if (bCheckForDuplicateEdges)
  {
    const MYON_SubDVertex* last_vertex = v;
    MYON_UniqueTester tester;
    for (const MYON_SubDEdgePtr* p = p0; p < p1; ++p)
    {
      const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(p->m_ptr);
      if (false == tester.AddToList((MYON__UINT_PTR)e))
        return false; // duplicate edge
      if (false == tester.AddToList((MYON__UINT_PTR)e->m_vertex[MYON_SUBD_EDGE_DIRECTION(p->m_ptr)]))
        return false; // duplicate vertex
    }    
    if (first_vertex != last_vertex)
    {
      if (false == tester.AddToList((MYON__UINT_PTR)last_vertex))
        return false; // duplicate vertex
    }
  }

  return true;
}

class MYON_SubDMeshImpl* MYON_SubDMesh::SubLimple() const
{
  return m_impl_sp.get();
}

unsigned int MYON_SubDMesh::SubLimpleUseCount() const
{
  return (unsigned int)(m_impl_sp.use_count());
}


bool MYON_SubD::IsSolid() const
{
  bool bIsManifold = false;
  bool bIsOriented = false;
  bool bHasBoundary = false;
  int solid_orientation = 0;
  ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
  return (bIsManifold && bIsOriented && false == bHasBoundary);
}

int MYON_SubD::SolidOrientation() const
{
  bool bIsManifold = false;
  bool bIsOriented = false;
  bool bHasBoundary = false;
  int solid_orientation = 0;
  ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
  return solid_orientation;
}

bool MYON_SubD::IsManifold( bool& bIsOriented, bool& bHasBoundary ) const
{
  bool bIsManifold = false;
  bIsOriented = false;
  bHasBoundary = false;
  int solid_orientation = 0;
  ActiveLevel().GetTopologicalAttributes(bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
  return bIsManifold;
}

bool MYON_SubD::IsManifold() const
{
  bool bIsOriented = false;
  bool bHasBoundary = false;
  return IsManifold(bIsOriented, bHasBoundary);
}

#ifdef MYON_DEBUG
static bool Internal_UnrollChain(const MYON_SimpleArray<MYON_SubDEdgePtr>& chain) {
  const unsigned imax{chain.UnsignedCount()};
  MYON_SimpleArray<bool> reversed_dirs{(size_t)imax};
  MYON_SimpleArray<const MYON_SubDEdge*> edges{(size_t)imax};
  MYON_SimpleArray<const MYON_SubDVertex*> vertices{(size_t)2 * (size_t)imax};
  MYON_SimpleArray<unsigned> edge_ids{(size_t)imax};
  MYON_SimpleArray<unsigned> vertice_ids{(size_t)2 * (size_t)imax};
  const unsigned int max_id = 0xFFFFFFFFU;
  for (unsigned i = 0; i < imax; ++i) {
    MYON_SubDEdgePtr eptr = chain[i];
    const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
    if (nullptr == e) {
      reversed_dirs.Append(false);
      edges.Append(nullptr);
      vertices.Append(nullptr);
      vertices.Append(nullptr);
      edge_ids.Append(max_id);
      vertice_ids.Append(max_id);
      vertice_ids.Append(max_id);
      continue;
    }
    edges.Append(e);
    edge_ids.Append(e->m_id);
    MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
    reversed_dirs.Append(edir == 0 ? false : true);
    const MYON_SubDVertex* v[2] = {e->m_vertex[edir], e->m_vertex[1 - edir]};
    vertices.Append(v[0]);
    vertices.Append(v[1]);
    vertice_ids.Append(v[0] == nullptr ? max_id : v[0]->m_id);
    vertice_ids.Append(v[1] == nullptr ? max_id : v[1]->m_id);
  }
  return true;
}

static bool Internal_VerifyEdgeChain(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& chain, MYON_2udex& chaindex,
    unsigned* edge_count = nullptr) {
  bool valid_chain{true};
  unsigned valid_edge_count = 0U;
  const unsigned jmax{chain.UnsignedCount()};
  MYON_SubDEdgePtr eptr = chain[chaindex.i];
  const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
  if (nullptr == e) {
    if (edge_count != nullptr) *edge_count = 0;
    return false;
  }
  MYON__UINT_PTR edir = MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
  const MYON_SubDVertex* v[2] = {e->m_vertex[edir], e->m_vertex[1 - edir]};
  if (nullptr == v[0] || nullptr == v[1] || v[0] == v[1]) {
    if (edge_count != nullptr) *edge_count = 0;
    return false;
  }
  ++valid_edge_count;
  for (chaindex.j = chaindex.i + 1; chaindex.j < jmax; ++chaindex.j) {
    valid_chain = false;
    eptr = chain[chaindex.j];
    e = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
    if (nullptr == e) { 
      // End of chain marker
      valid_chain = true;
      if (chaindex.j < jmax) ++chaindex.j;
      break;
    }
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] ||
        e->m_vertex[0] == e->m_vertex[1]) {
      break;
    }
    edir = MYON_SUBD_EDGE_DIRECTION(eptr.m_ptr);
    if (v[1] != e->m_vertex[edir]) {
      if (edge_count == nullptr) valid_chain = true;
      break;
    }
    v[1] = e->m_vertex[1 - edir];
    ++valid_edge_count;
    valid_chain = true;
  }
  if (edge_count != nullptr && *edge_count != valid_edge_count) {
    *edge_count = valid_edge_count;
    return false;
  }
  return valid_edge_count > 0 && valid_chain;
}

static bool Internal_CountAndVerifyEdgeChains(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& sorted_edges,
    unsigned* chain_count = nullptr) {
  if (chain_count != nullptr && chain_count == 0) return false;
  unsigned valid_chain_count = 0U;
  bool valid_chains{false};
  const unsigned edge_count{sorted_edges.UnsignedCount()};
  MYON_2udex chaindex{0, edge_count};
  for (chaindex = {0, edge_count}; chaindex.i < edge_count;
       chaindex.i = (chaindex.j > chaindex.i) ? chaindex.j : (chaindex.i + 1)) {
    valid_chains = false;
    if (!Internal_VerifyEdgeChain(sorted_edges, chaindex)) break;
    ++valid_chain_count;
    valid_chains = true;
  }
  if (chain_count != nullptr &&
      (*chain_count != valid_chain_count || chaindex.j != edge_count)) {
    *chain_count = valid_chain_count;
    return false;
  }
  return valid_chain_count > 0 && valid_chains;
}
#endif

//////////////////////////
//
// Better edge sorter
//


static unsigned Internal_MuchImprovedSortEdgesIntoChains(
  const MYON__UINT_PTR* unsorted_edges,
  size_t unsorted_edges_count,
  MYON_SimpleArray<MYON_SubDEdgePtr>& sorted_edges
)
{
  // NOTE: sorted_edges and unsorted_edges should not point to the same array by this point
  sorted_edges.SetCount(0);
  sorted_edges.Reserve(unsorted_edges_count + 128U);

  if (unsorted_edges_count < 1 || nullptr == unsorted_edges)
    return 0;

  const unsigned unsorted_count = (unsigned)unsorted_edges_count;
  unsigned chain_count = 0;

  // Set MarkBits = 0 for every vertex and for every edge attached to a vertex in the edge chain.
  for (unsigned i = 0; i < unsorted_count; ++i)
  {
    const MYON_SubDEdge* e = (const MYON_SubDEdge*)(MYON_SUBD_COMPONENT_POINTER_MASK & unsorted_edges[i]);
    if (nullptr == e)
      continue;
    e->ClearMarkBits();
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      continue;
    if (e->m_vertex[0] == e->m_vertex[1])
      continue;
    for (unsigned evi = 0; evi < 2; ++evi)
    {
      const MYON_SubDVertex* v = e->m_vertex[evi];
      v->ClearMarkBits();
      for (unsigned short vei = 0; vei < v->m_edge_count; ++vei)
      {
        const MYON_SubDEdge* ve = MYON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
        if (nullptr != ve)
          ve->ClearMarkBits();
      }
    }
  }

  // Set e->MarkBits = 1 for every valid edge in the edge chain.
  for (unsigned i = 0; i < unsorted_count; ++i)
  {
    const MYON_SubDEdge* e = (const MYON_SubDEdge*)(MYON_SUBD_COMPONENT_POINTER_MASK & unsorted_edges[i]);
    if (nullptr == e)
      continue;
    if (nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1])
      continue;
    if (e->m_vertex[0] == e->m_vertex[1])
      continue;
    e->SetMarkBits(1);
  }

  // Set v->MarkBits() = number of unsorted_edges[] that have v as a vertex with 3 meaning 3 or more.
  for (unsigned i = 0; i < unsorted_count; ++i)
  {
    const MYON_SubDEdge* e = (const MYON_SubDEdge*)(MYON_SUBD_COMPONENT_POINTER_MASK & unsorted_edges[i]);
    if (nullptr == e)
      continue;
    if (0 == e->MarkBits())
      continue;
    for (unsigned evi = 0; evi < 2; ++evi)
    {
      const MYON_SubDVertex* v = e->m_vertex[evi];
      const MYON__UINT8 ecount = v->MarkBits();
      if (ecount < 3)
        v->SetMarkBits(ecount + 1);
    }
  }

  // Go through the unsorted edges and use the MarkBits values to quickly build the edge chains.
  MYON_SimpleArray<MYON_SubDEdgePtr> chain((int)unsorted_edges_count);

  for (unsigned i = 0; i < unsorted_edges_count; ++i)
  {
    const MYON_SubDEdge* seed_edge = (const MYON_SubDEdge*)(MYON_SUBD_COMPONENT_POINTER_MASK & unsorted_edges[i]);
    if (nullptr == seed_edge || 1 != seed_edge->MarkBits())
      continue; // seed_edge is invalid or already assigned to an edge chain

    // this edge pointer direcion will be reversed when the for loop evi = 1.
    chain.SetCount(0);
    chain.Append(MYON_SubDEdgePtr::Create(seed_edge, 1)); // dir=1 is correct. The direcion will be reversed in the for loop below when evi = 1.
    seed_edge->ClearMarkBits(); // seed_edge is now in a chain

    for (unsigned evi = 0; evi < 2; ++evi)
    {
      if (1 == evi)
      {
        MYON_SubDEdgeChain::ReverseEdgeChain(chain);
        if (chain[0].RelativeVertex(0) == chain[chain.UnsignedCount() - 1].RelativeVertex(1))
          break; // we found a closed loop
      }

      const MYON_SubDVertex* v = chain[chain.UnsignedCount() - 1].RelativeVertex(1);
      if (nullptr == v || 2 != v->MarkBits())
        continue; // edge chain cannot continue through v

      for (const MYON_SubDVertex* v1 = nullptr; nullptr != v && 2 == v->MarkBits(); v = v1)
      {
        v->ClearMarkBits(); // clearing v->MarkBits indicates this v has been used.
        v1 = nullptr;
        for (unsigned short vei = 0; vei < v->m_edge_count; ++vei)
        {
          const MYON_SubDEdge* ve = MYON_SUBD_EDGE_POINTER(v->m_edges[vei].m_ptr);
          if (nullptr == ve)
            continue;
          if (1 != ve->MarkBits())
            continue; // this ve was not in unsorted_edges[] or has already been assigned to a chain.
          if (v == ve->m_vertex[0])
          {
            ve->SetMarkBits(0); // MarkBits() = 0 indicates ve is now in a chain
            chain.Append(MYON_SubDEdgePtr::Create(ve, 0));
            v1 = ve->m_vertex[1];
          }
          else if (v == ve->m_vertex[1])
          {
            ve->SetMarkBits(0); // MarkBits() = 0 indicates ve is now in a chain
            chain.Append(MYON_SubDEdgePtr::Create(ve, 1));
            v1 = ve->m_vertex[0];
          }
          else
          {
            MYON_SUBD_ERROR("Corrupt edge/vertex topology.");
          }
        }
        v = v1;
      }
    }

#ifdef MYON_DEBUG
    unsigned temp_chain_edge_count{chain.UnsignedCount()};
    MYON_2udex temp_udex{0U, (unsigned)(chain.UnsignedCount() - 1)};
    if (!Internal_VerifyEdgeChain(chain, temp_udex, &temp_chain_edge_count)) {
      temp_chain_edge_count = 0;
    }
    Internal_UnrollChain(chain);
#endif
    sorted_edges.Append(chain.Count(), chain.Array());
    sorted_edges.Append(MYON_SubDEdgePtr::Null); // end of chain marker
    ++chain_count;
  }

  // clear all the mark bits that may still be set when the input is corrupt.
  for (size_t i = 0; i < unsorted_edges_count; ++i)
  {
    const MYON_SubDEdge* e = (const MYON_SubDEdge*)(MYON_SUBD_COMPONENT_POINTER_MASK & unsorted_edges[i]);
    if (nullptr == e)
      continue;
    e->ClearMarkBits();
    if (nullptr != e->m_vertex[0])
      e->m_vertex[0]->ClearMarkBits();
    if (nullptr != e->m_vertex[1])
      e->m_vertex[1]->ClearMarkBits();    
  }

#ifdef MYON_DEBUG
  Internal_UnrollChain(sorted_edges);
  if (!Internal_CountAndVerifyEdgeChains(sorted_edges, &chain_count)) {
    return chain_count;
  }
#endif
  return chain_count;
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SimpleArray< MYON_SubDEdgePtr >& unsorted_edges,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
)
{
  unsigned int chain_count = 0;
  if (unsorted_edges.Array() == sorted_edges.Array())
  {
    const MYON_SimpleArray< MYON_SubDEdgePtr > local_unsorted_edges(unsorted_edges);
    chain_count = Internal_MuchImprovedSortEdgesIntoChains(
      (const MYON__UINT_PTR * )local_unsorted_edges.Array(),
      local_unsorted_edges.UnsignedCount(),
      sorted_edges
    );
  }
  else
  {
    chain_count = Internal_MuchImprovedSortEdgesIntoChains(
      (const MYON__UINT_PTR*)unsorted_edges.Array(),
      unsorted_edges.UnsignedCount(),
      sorted_edges
    );
  }
  return chain_count;
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SimpleArray< const MYON_SubDEdge* >& unsorted_edges,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
)
{
  unsigned int chain_count = 0;
  if ((const void*)unsorted_edges.Array() == (const void*)sorted_edges.Array())
  {
    const MYON_SimpleArray< const MYON_SubDEdge* > local_unsorted_edges(unsorted_edges);
    chain_count = Internal_MuchImprovedSortEdgesIntoChains(
      (const MYON__UINT_PTR*)local_unsorted_edges.Array(),
      local_unsorted_edges.UnsignedCount(),
      sorted_edges
    );
  }
  else
  {
    chain_count = Internal_MuchImprovedSortEdgesIntoChains(
      (const MYON__UINT_PTR*)unsorted_edges.Array(),
      unsorted_edges.UnsignedCount(),
      sorted_edges
    );
  }
  return chain_count;
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SimpleArray< MYON_SubDComponentPtr >& unsorted_edges,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
)
{
  unsigned int chain_count = 0;
  if ((const void*)unsorted_edges.Array() == (const void*)sorted_edges.Array())
  {
    const MYON_SimpleArray< MYON_SubDComponentPtr > local_unsorted_edges(unsorted_edges);
    chain_count = Internal_MuchImprovedSortEdgesIntoChains(
      (const MYON__UINT_PTR*)local_unsorted_edges.Array(),
      local_unsorted_edges.UnsignedCount(),
      sorted_edges
    );
  }
  else
  {
    chain_count = Internal_MuchImprovedSortEdgesIntoChains(
      (const MYON__UINT_PTR*)unsorted_edges.Array(),
      unsorted_edges.UnsignedCount(),
      sorted_edges
    );
  }
  return chain_count;
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SubD& subd,
  const MYON_SimpleArray< MYON_COMPONENT_INDEX >& unsorted_edges,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
)
{
  const unsigned unsorted_count = unsorted_edges.UnsignedCount();
  MYON_SimpleArray< const MYON_SubDEdge* > local_unsorted_edges(unsorted_count);
  for (unsigned i = 0; i < unsorted_count; ++i)
  {
    const MYON_COMPONENT_INDEX ci = unsorted_edges[i];
    if (MYON_COMPONENT_INDEX::TYPE::subd_edge != ci.m_type)
      continue;
    if (0 == ci.m_index || -1 == ci.m_index)
      continue;
    const MYON_SubDEdge* e = subd.EdgeFromId((unsigned)ci.m_index);
    if (nullptr == e)
      continue;
    local_unsorted_edges.Append(e);
  }

  return Internal_MuchImprovedSortEdgesIntoChains(
    (const MYON__UINT_PTR*)local_unsorted_edges.Array(),
    unsorted_edges.UnsignedCount(),
    sorted_edges
  );
}

bool MYON_SubDEdgeChain::IsSingleEdgeChain(
  const MYON_SimpleArray<MYON_SubDEdgePtr>& edges,
  bool& bIsClosed,
  bool& bIsSorted
)
{
  bIsSorted = false;
  bIsClosed = false;

  const unsigned count = edges.UnsignedCount();
  if (count <= 1U)
  {
    const MYON_SubDEdge* e = (1U == count) ? edges[0].Edge() : nullptr;
    if (
      nullptr != e 
      && nullptr != e->m_vertex[0] 
      && nullptr != e->m_vertex[1] 
      && e->m_vertex[0] != e->m_vertex[1]
      )
    {
      // 1 valid edge
      bIsSorted = true;
      return true;
    }
    return false;
  }

  // save MarkBits() values on edges[] components so they can be restored after testing.
  // Internal_MuchImprovedSortEdgesIntoChains() uses MarkBits().
  union
  {
    MYON__UINT32 u32;
    MYON__UINT8 u8[4];
  } u;
  MYON_SimpleArray<MYON__UINT32> saved_markbits(count);
  for (unsigned i = 0; i < count; ++i)
  {
    u.u32 = 0;
    const MYON_SubDEdge* e = edges[i].Edge();
    if (nullptr != e)
    {
      u.u8[0] = e->MarkBits();
      if (e->m_vertex[0])
        u.u8[1] = e->m_vertex[0]->MarkBits();
      if (e->m_vertex[1])
        u.u8[2] = e->m_vertex[1]->MarkBits();
    }
    saved_markbits.Append(u.u32);
  }

  bool bIsSingleEdgeChain = false;
  for (;;)
  {
    MYON_SimpleArray<MYON_SubDEdgePtr> sorted_edges;
    const unsigned chain_count = Internal_MuchImprovedSortEdgesIntoChains(
      (const MYON__UINT_PTR*)edges.Array(),
      count,
      sorted_edges
    );

    if (1U != chain_count)
      break; // edges[] is not a contiguouse set of edges or it self intersects.
    if (count + 1U != sorted_edges.UnsignedCount())
      break; // edges[] contained null edges

    // determine edges[] sorts into a closed edge chain.
    if (count >= 3 && sorted_edges[0].RelativeVertex(0) == sorted_edges[count - 1].RelativeVertex(1))
      bIsClosed = true;

    // Determine edges[] is was already sorted.
    // Note that this test does not detect self interections and that's
    // why MYON_SubDEdgeChain::SortEdgesIntoEdgeChains() is called above.
    const MYON_SubDVertex* v0 = edges[0].RelativeVertex(0);
    if (nullptr != v0)
    {
      const MYON_SubDVertex* edges_ends[2] = { edges[0].RelativeVertex(0), edges[count - 1].RelativeVertex(1) };
      const MYON_SubDVertex* sorted_ends[2] = { sorted_edges[0].RelativeVertex(0), sorted_edges[count - 1].RelativeVertex(1) };
      if (bIsClosed)
      {
        if (edges_ends[0] == edges_ends[1] && sorted_ends[0] == sorted_ends[1])
          bIsSorted = true;
      }
      else
      {
        // sorted_eges[] may be reversed.
        if (edges_ends[0] == sorted_ends[0] && edges_ends[1] == sorted_ends[1])
          bIsSorted = true;
        else if (edges_ends[0] == sorted_ends[1] && edges_ends[1] == sorted_ends[0])
          bIsSorted = true;
      }

      for (unsigned i = 0; bIsSorted && i < count; ++i)
      {
        const MYON_SubDVertex* ev[2] = { edges[i].RelativeVertex(0), edges[i].RelativeVertex(1) };
        if (v0 != ev[0] || nullptr == ev[1] || ev[0] == ev[1])
        {
          bIsSorted = false;
          break;
        }
        v0 = ev[1];
      }
    }


    bIsSingleEdgeChain = true;
    break;
  }


  // restore MarkBits() values on edges[] components.
  for (unsigned i = 0; i < count; ++i)
  {
    u.u32 = saved_markbits[i];
    const MYON_SubDEdge* e = edges[i].Edge();
    if (nullptr != e)
    {
      e->SetMarkBits(u.u8[0]);
      if (e->m_vertex[0])
        e->m_vertex[0]->SetMarkBits(u.u8[1]);
      if (e->m_vertex[1])
        e->m_vertex[1]->SetMarkBits(u.u8[2]);
    }
  }

  return bIsSingleEdgeChain;
}

bool MYON_SubDEdgeChain::IsSingleEdgeChain(
  const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
)
{
  bool bIsClosed = false;
  bool bIsSorted = false;
  return MYON_SubDEdgeChain::IsSingleEdgeChain(edges, bIsClosed, bIsSorted);
}


class MYON_SubDEdgePtrLink
{
public:
  MYON_SubDEdgePtrLink() = default;
  ~MYON_SubDEdgePtrLink() = default;
  MYON_SubDEdgePtrLink(const MYON_SubDEdgePtrLink&) = default;
  MYON_SubDEdgePtrLink& operator=(const MYON_SubDEdgePtrLink&) = default;

public:
  static const MYON_SubDEdgePtrLink Empty;

public:
  MYON_SubDEdgePtr m_ep = MYON_SubDEdgePtr::Null;
  unsigned int m_index = 0;
  unsigned int m_nbr_index = 0;

  static int CompareVertex(
    const MYON_SubDEdgePtrLink* lhs,
    const MYON_SubDEdgePtrLink* rhs
  )
  {
    const MYON_SubDVertex* lhs_v = lhs->m_ep.RelativeVertex(0);
    const MYON_SubDVertex* rhs_v = rhs->m_ep.RelativeVertex(0);
    if (lhs_v < rhs_v)
      return -1;
    if (lhs_v > rhs_v)
      return 1;
    return 0;
  }

  static int CompareIndex(
    const MYON_SubDEdgePtrLink* lhs,
    const MYON_SubDEdgePtrLink* rhs
  )
  {
    const unsigned int lhs_i = lhs->m_index;
    const unsigned int rhs_i = rhs->m_index;
    if (lhs_i < rhs_i)
      return -1;
    if (lhs_i > rhs_i)
      return 1;
    return 0;
  }

  static void Resolve3OrMoreEdges(
    const unsigned int unset_nbr1_index,
    unsigned int count,
    const MYON_SubDVertex* v,
    MYON_SubDEdgePtrLink* links
  );  
};

const MYON_SubDEdgePtrLink MYON_SubDEdgePtrLink::Empty;

void MYON_SubDEdgePtrLink::Resolve3OrMoreEdges(
  const unsigned int unset_nbr1_index,
  unsigned int count,
  const MYON_SubDVertex* v,
  MYON_SubDEdgePtrLink* links
)
{
  // If the case can't be resolved by Resolve3OrMoreEdges(), 
  // then the vertex will not appear in the middle of a chain.

  if (count < 3 || nullptr == v || count != (unsigned int)v->m_edge_count)
    return;

  switch (count)
  {
  case 3:
    if (false == v->IsCrease() && false == v->IsDart())
      return;
    break;

  case 4:
    if (false == v->IsCrease() && false == v->IsSmooth())
      return;
    break;

  default:
    if (false == v->IsCrease())
      return;
    break;
  }

  const MYON_SubDEdge* link_edges[4] = {};
  const MYON_SubDEdge* vertex_edges[4] = {};
  unsigned int crease_edge_count = 0;
  unsigned int smooth_edge_count = 0;
  unsigned int smooth_edge_link_index[4] = {};
  unsigned int crease_edge_link_index[4] = {};
  for (unsigned int j = 0; j < count; j++)
  {
    const MYON_SubDEdge* e = links[j].m_ep.Edge();
    if (nullptr == e)
      return;
    const MYON_SubDEdge* ve = v->Edge(j);
    if (nullptr == ve)
      return;
    if (j < 4)
    {
      link_edges[j] = e;
      vertex_edges[j] = ve;
    }

    if (e->IsSmooth() && 2 == e->m_face_count)
    {
      if ( smooth_edge_count < 4)
        smooth_edge_link_index[smooth_edge_count] = j;
      ++smooth_edge_count;
    }
    else if (e->IsCrease())
    {
      if (crease_edge_count < 4)
        crease_edge_link_index[crease_edge_count] = j;
      ++crease_edge_count;
    }
    else
      return;
  }

  if ( 2 == crease_edge_count && v->IsCrease() )
  {
    // Link the two creased edges.
    // The vertex will be interior in a chain and the edges
    // will be next to each other.
    links[crease_edge_link_index[0]].m_nbr_index = links[crease_edge_link_index[1]].m_index;
    links[crease_edge_link_index[1]].m_nbr_index = links[crease_edge_link_index[0]].m_index;
    if (1 == smooth_edge_count)
    {
      // this edge will be at the end of a chain.
      links[smooth_edge_link_index[0]].m_nbr_index = unset_nbr1_index;
    }
  }

  if (2 == smooth_edge_count)
  {
    // Link the two smooth edges.
    // The vertex will be interior in a chain and the edges
    // will be next to each other.
    links[smooth_edge_link_index[0]].m_nbr_index = links[smooth_edge_link_index[1]].m_index;
    links[smooth_edge_link_index[1]].m_nbr_index = links[smooth_edge_link_index[0]].m_index;
    if (1 == crease_edge_count)
    {
      // this edge will be at the end of a chain
      links[crease_edge_link_index[0]].m_nbr_index = unset_nbr1_index;
    }
   }

  if (
    4 != count
    || 4 != smooth_edge_count
    || 0 != crease_edge_count
    || 4 != v->m_face_count
    || 4 != v->m_edge_count
    || false == v->IsSmooth()
    )
    return;

  // make sure vertex_edges[] and link_edges[] are the same list.
  unsigned int match_count = 0;
  for (unsigned int j = 0; j == match_count && j < count; j++)
  {
    for (unsigned int k = 0; k < count; k++)
    {
      if (vertex_edges[k] == link_edges[j])
      {
        vertex_edges[k] = nullptr;
        match_count++;
        break;
      }
    }
  }
  if (match_count != count)
    return;
  
  // vertex has 4 faces and 4 smooth edges. Link opposite edges.
  const MYON_SubDFace* edge_faces[4][2];
  for (unsigned int j = 0; j < 4; j++)
  {
    edge_faces[j][0] = link_edges[j]->Face(0);
    edge_faces[j][1] = link_edges[j]->Face(1);
    if (nullptr == edge_faces[j][0] || nullptr == edge_faces[j][1])
      return;
  }
  MYON_2udex pairs[2];
  unsigned int pair_count = 0;
  MYON_2udex pair;
  for (pair.i = 0; pair.i < 4; ++pair.i) for (pair.j = pair.i+1; pair.j < 4; ++pair.j)
  {
    if (
      edge_faces[pair.i][0] != edge_faces[pair.j][0]
      && edge_faces[pair.i][0] != edge_faces[pair.j][1]
      && edge_faces[pair.i][1] != edge_faces[pair.j][0]
      && edge_faces[pair.i][1] != edge_faces[pair.j][1]
      )
    {
      // the associated edges share no faces.
      if ( pair_count < 2)
        pairs[pair_count] = pair;
      if (++pair_count > 2)
        break;
    }
  }
  if (2 == pair_count)
  {
    links[pairs[0].i].m_nbr_index = links[pairs[0].j].m_index;
    links[pairs[0].j].m_nbr_index = links[pairs[0].i].m_index;
    links[pairs[1].i].m_nbr_index = links[pairs[1].j].m_index;
    links[pairs[1].j].m_nbr_index = links[pairs[1].i].m_index;
  }

  return;
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SubD& subd,
  const MYON_SimpleArray< MYON_COMPONENT_INDEX >& unsorted_edges,
  unsigned int minimum_chain_length,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
)
{
  const unsigned count = unsorted_edges.UnsignedCount();
  MYON_SimpleArray< const MYON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
  {
    const MYON_SubDEdge* e = subd.EdgeFromComponentIndex(unsorted_edges[i]);
    if (nullptr != e)
      a.Append(e);
  }
  return SortEdgesIntoEdgeChains(a, minimum_chain_length, sorted_edges);
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SimpleArray< MYON_SubDComponentPtr >& unsorted_edges,
  unsigned int minimum_chain_length,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
)
{
  const unsigned count = unsorted_edges.UnsignedCount();
  MYON_SimpleArray< const MYON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
    a.Append(unsorted_edges[i].Edge());
  return MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(a, minimum_chain_length, sorted_edges);
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SimpleArray< const MYON_SubDEdge* >& unsorted_edges,
  unsigned int minimum_chain_length,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges,
  const bool bIgnoreCorners
)
{
  const unsigned int unsorted_edge_count = unsorted_edges.Count();
  MYON_SimpleArray< MYON_SubDEdgePtr > unsorted_eptrs(unsorted_edge_count);
  for (unsigned i = 0; i < unsorted_edge_count; ++i)
  {
    const MYON_SubDEdge* e = unsorted_edges[i];
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] || e->m_vertex[0] == e->m_vertex[1])
      continue;
    MYON_SubDEdgePtr eptr = MYON_SubDEdgePtr::Create(e, 0);
    if (1 == e->m_face_count && 0 == MYON_SUBD_FACE_DIRECTION(e->m_face2[0].m_ptr))
      eptr = eptr.Reversed();
    unsorted_eptrs.Append(eptr);
  }
  return MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(unsorted_eptrs, minimum_chain_length, sorted_edges, bIgnoreCorners);
}

unsigned int MYON_SubDEdgeChain::SortEdgesIntoEdgeChains(
  const MYON_SimpleArray< MYON_SubDEdgePtr >& unsorted_edges,
  unsigned int minimum_chain_length,
  MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges,
  const bool bIgnoreCorners
)
{
  // NOTE:
  //  unsorted_edges[] and sorted_edges[] may reference the same array.
  ////const MYON_SubDEdge* ee[2] = {};
  ////const MYON_SubDVertex* vv[2] = {};

  const unsigned int unsorted_edge_count = unsorted_edges.Count();
  if (unsorted_edge_count <= 0)
  {
    sorted_edges.SetCount(0);
    return 0;
  }

  MYON_SimpleArray< MYON_SubDEdgePtrLink > links(2*unsorted_edge_count);
  const unsigned int unset_nbr1_index = 0xFFFFFFFEU;
  const unsigned int unset_nbrX_index = unset_nbr1_index+1;
  MYON_SubDEdgePtrLink epl;
  epl.m_nbr_index = unset_nbrX_index;
  for (unsigned int i = 0; i < unsorted_edge_count; i++)
  {
    MYON_SubDEdgePtr ep = unsorted_edges[i];
    const MYON_SubDEdge* e = ep.Edge();
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] || e->m_vertex[0] == e->m_vertex[1])
      continue;

    // unsorted_edges[i] has 2 links with m_index = 2*i and m_index = 2*+1.

    // links with even m_index have opposite orientation as unsorted_edges[].
    // links[2*i].m_ep.RelativeVertex(0) = unsorted_edges[i].RelativeVertex(1)
    epl.m_ep = ep.Reversed();
    links.Append(epl);
    ++epl.m_index;
    ////ee[0] = epl.m_ep.Edge();
    ////vv[1] = epl.m_ep.RelativeVertex(0);

    // links with odd m_index have same orientation as unsorted_edges[].
    // links[2*i+1].m_ep.RelativeVertex(0) = unsorted_edges[i].RelativeVertex(0)
    epl.m_ep = ep;
    links.Append(epl);
    ++epl.m_index;
    ////ee[1] = epl.m_ep.Edge();
    ////vv[0] = epl.m_ep.RelativeVertex(0);
    ////if (e != ee[0] || e != ee[1] || vv[0] == vv[1])
    ////  return false;
  }

  // NOTE:
  // unsorted_edges[] and sorted_edges[] may reference the same array.
  // At this point, I'm finished with unsorted_edges[] so it's ok to 
  // modify sorted_edges[] here.
  sorted_edges.SetCount(0);
  sorted_edges.Reserve(unsorted_edge_count);

  // sort links by MYON_SubDEdgePtr.RelativeVertex(0) and set m_nbr_index
  links.QuickSort(MYON_SubDEdgePtrLink::CompareVertex);
  // link_count = even number
  const unsigned int link_count = links.UnsignedCount();
  unsigned int i1 = link_count;
  for (unsigned int i0 = 0; i0 < link_count; i0 = i1)
  {
    MYON_SubDEdgePtrLink& epl0 = links[i0];
    const MYON_SubDVertex* v = epl0.m_ep.RelativeVertex(0);
    for (i1 = i0 + 1; i1 < link_count; ++i1)
    {
      if (0 != MYON_SubDEdgePtrLink::CompareVertex(&epl0, &links[i1]))
        break;
    }
    if (nullptr == v)
    {
      MYON_SUBD_ERROR("Bug in code that creates the links[] array.");
      continue;
    }
    if (!bIgnoreCorners)
    {
      if (v->IsCorner())
      {
        // These edges will be at the ends of chains.
        while (i0 < i1)
          links[i0++].m_nbr_index = unset_nbr1_index;
        continue;
      }
    }

    if (i0 + 1 == i1)
    {
      // The vertex is referenced by exactly 1 edge in unsorted_edges[]
      // This edge will appear in sorted_edges[] at the start or end of a chain.
      epl0.m_nbr_index = unset_nbr1_index;
      continue; 
    }
    if (i0 + 2 == i1)
    {
      // This vertex is referenced by exactly 2 edges in  unsorted_edges[].
      // The vertex will be in the interior of a chain and the edges will 
      // appear in sorted_edges[] next to each other in the same chain.
      MYON_SubDEdgePtrLink& epl1 = links[i0 + 1];
      epl0.m_nbr_index = epl1.m_index;
      epl1.m_nbr_index = epl0.m_index;
      continue;
    }

    // The vertex referenced by 3 or more edges in unsorted_edges[].
    // If the case cannot be resolved by Resolve3OrMoreEdges(), 
    // then this vertex will not be in the interior of a chain.
    MYON_SubDEdgePtrLink::Resolve3OrMoreEdges(
      unset_nbr1_index,
      i1 - i0,
      v,
      links.Array() + i0
    );
  }

  // Sort links[] by m_index valut to restore links[] to its original order.
  links.QuickSort(MYON_SubDEdgePtrLink::CompareIndex);
  MYON_SubDEdgePtrLink* links_array = links.Array();

  unsigned chain_count = 0;
  MYON_SimpleArray<MYON_SubDEdgePtr> chain(unsorted_edge_count);

  for (unsigned int i = 0; i < link_count; ++i)
  {
    // epl0 and epl1 are the links for edges[i/2]
    const MYON_SubDEdgePtrLink epl0 = links_array[i];
    links_array[i].m_ep = MYON_SubDEdgePtr::Null;
    const MYON_SubDEdgePtrLink epl1 = links_array[++i];
    links_array[i].m_ep = MYON_SubDEdgePtr::Null;

    if (nullptr == epl0.m_ep.Edge())
      continue; // this edge has already been inserted in sorted_edges[].

    chain.SetCount(0);

    // Add edges that come "before" edges[i/2] to chain[]
    epl = epl1;
    for (;;)
    {
      if (epl.m_nbr_index >= unset_nbr1_index)
        break;
      unsigned int j = epl.m_nbr_index;
      unsigned int j1 = (0 == (j % 2)) ? (j + 1) : (j - 1);
      // epl = "previous" link
      epl = links_array[j1];
      links_array[j].m_ep = MYON_SubDEdgePtr::Null;
      links_array[j1].m_ep = MYON_SubDEdgePtr::Null;
      if (nullptr == epl.m_ep.Edge())
        break;
      ////ee[0] = epl.m_ep.Edge();
      ////vv[0] = epl.m_ep.RelativeVertex(0);
      ////vv[1] = epl.m_ep.RelativeVertex(1);
      ////if (vv[0] == vv[1] || nullptr == ee[0])
      ////  return false;
      chain.Append(epl.m_ep);
    }

    const bool bClosedChain =
        chain.UnsignedCount() > 0 && (epl.m_index == epl1.m_index);
    const bool bFirstEdgeIsReversed = !bClosedChain &&
                                      unset_nbr1_index <= epl.m_nbr_index &&
                                      (0 == (epl.m_index % 2));
    bool bLastEdgeIsReversed{false};
    if (false == bClosedChain)
    {
      chain.Reverse();
    }

    ////ee[0] = epl1.m_ep.Edge();
    ////vv[0] = epl1.m_ep.RelativeVertex(0);
    ////vv[1] = epl1.m_ep.RelativeVertex(1);
    ////if (vv[0] == vv[1] || nullptr == ee[0])
    ////  return false;
    chain.Append(epl1.m_ep); // matches input edge orientation

    if (bClosedChain)
    {
      // put edges[i/2] at the start of the closed chain.
      chain.Reverse();
    }
    else
    {
      // Add edges that come "after" edges[i/2] to chain[]
      epl = epl0;
      for (;;)
      {
        if (epl.m_nbr_index >= unset_nbr1_index)
          break;
        unsigned int j = epl.m_nbr_index;
        unsigned int j1 = (0 == (j % 2)) ? (j + 1) : (j - 1);
        // epl = "next" link
        epl = links_array[j1];
        links_array[j].m_ep = MYON_SubDEdgePtr::Null;
        links_array[j1].m_ep = MYON_SubDEdgePtr::Null;
        if (nullptr == epl.m_ep.Edge())
          break;
        ////ee[0] = epl.m_ep.Edge();
        ////vv[0] = epl.m_ep.RelativeVertex(1);
        ////vv[1] = epl.m_ep.RelativeVertex(0);
        ////if (vv[0] == vv[1] || nullptr == ee[0])
        ////  return false;
        chain.Append(epl.m_ep.Reversed());
      }

      // No need to update bClosedChain here: if it was closed, then the circle
      // was found with the edges "before" edges[i/2]

      // First and last edge are reversed: reverse chain direction to keep first
      // edge direction the same as in unsorted_edges
      bLastEdgeIsReversed = epl.m_index % 2 == 0;
      if (bFirstEdgeIsReversed && bLastEdgeIsReversed) {
        ReverseEdgeChain(chain);
      }
    }

    const unsigned int chain_edge_count = chain.UnsignedCount();
    if (chain_edge_count > 0)
    {
      for (;;)
      {
        if (chain_edge_count < 3)
          break;
        const MYON_SubDVertex* c0 = chain[0].RelativeVertex(0);
        if (nullptr == c0)
          break;
        if (c0->IsCorner())
          break;
        const MYON_SubDVertex* c1 = chain[chain_edge_count-1].RelativeVertex(1);
        if (c0 != c1)
          break;
        const MYON_SubDEdge* e0 = chain[0].Edge();
        if (nullptr == e0)
          break;
        const MYON_SubDEdge* e1 = chain[chain_edge_count-1].Edge();
        if (nullptr == e1)
          break;
        const bool bSmooth = e0->IsSmooth();
        if (bSmooth != e1->IsSmooth())
          break;
        if (bSmooth && c0->IsCrease())
          break;
        // Check for an embedded crease vertex.
        for (unsigned int k = 1; k < chain_edge_count; ++k)
        {
          const MYON_SubDVertex* v = chain[k].RelativeVertex(0);
          if (nullptr == v)
            break;
          const MYON_SubDEdge* e = chain[k].Edge();
          if (nullptr == e)
            break;
          if (
            bSmooth != e->IsSmooth()
            || (bSmooth && v->IsCreaseOrCorner())
            )
          {
            // shift chain[] so it begins at chain[k];
            MYON_SimpleArray<MYON_SubDEdgePtr> tail;
            tail.Append(k, chain.Array());
            for (unsigned n = k; n < chain_edge_count; ++n)
              chain[n - k] = chain[n];
            chain.SetCount(chain_edge_count - k);
            chain.Append(tail.Count(), tail.Array());
            break;
          }
        }
        break;
      }
#ifdef MYON_DEBUG
      unsigned temp_chain_edge_count{chain_edge_count};
      MYON_2udex temp_udex{0U, (unsigned)(chain_edge_count - 1)};
      if (!Internal_VerifyEdgeChain(chain, temp_udex, &temp_chain_edge_count)) {
        temp_chain_edge_count = 0;
      }
      Internal_UnrollChain(chain);
#endif
      if (chain.UnsignedCount() >= minimum_chain_length) {
        const MYON_SubDVertex* c0{chain[0].RelativeVertex(0)};
        const MYON_SubDVertex* s1{sorted_edges.Last() == nullptr
                                    ? nullptr
                                    : sorted_edges.Last()->RelativeVertex(1)};
        if (c0 != nullptr && c0 == s1) {
          // If the first vertex of the chain is a corner, or a vertex with more
          // than 2 edges referencing it, we might have artificially split the
          // chain at that vertex and the last vertex of the previous chain is
          // the same as ours. Try a few tricks to put a gap between these two
          // vertices.
          const MYON_SubDVertex* c1{
              chain[chain_edge_count - 1].RelativeVertex(1)};
          const MYON_SubDVertex* s0{
              sorted_edges.First() == nullptr
                  ? nullptr
                  : sorted_edges.First()->RelativeVertex(0)};
          // By order of importance, try to:
          //   - Keep the orientation of the first edge the same as in unsorted_edges
          //   - Not use ReverseEdgeChain()
          //   - Append rather than Prepend to sorted_edges
          if (c1 != s0) {
            ++chain_count;
            sorted_edges.Prepend(chain.Count(), chain.Array());
          } else if (c1 != s1 && !bClosedChain &&
                     ((bFirstEdgeIsReversed && !bLastEdgeIsReversed) ||
                      (!bFirstEdgeIsReversed && bLastEdgeIsReversed))) {
            ReverseEdgeChain(chain);
            ++chain_count;
            sorted_edges.Append(chain.Count(), chain.Array());
          } else if (c0 != s0 && !bClosedChain &&
                     ((bFirstEdgeIsReversed && !bLastEdgeIsReversed) ||
                      (!bFirstEdgeIsReversed && bLastEdgeIsReversed))) {
            ReverseEdgeChain(chain);
            ++chain_count;
            sorted_edges.Prepend(chain.Count(), chain.Array());
          } else if (c1 != s1 && !bClosedChain) {
            ReverseEdgeChain(chain);
            ++chain_count;
            sorted_edges.Append(chain.Count(), chain.Array());
          } else if (c0 != s0 && !bClosedChain) {
            ReverseEdgeChain(chain);
            ++chain_count;
            sorted_edges.Prepend(chain.Count(), chain.Array());
          } else {
            // We tried, but nothing works easily enough: append the chain as
            // usual, this means the two chains are merged and chain_count does
            // not increase.
            sorted_edges.Append(chain.Count(), chain.Array());
          }
        } else {
          ++chain_count;
          sorted_edges.Append(chain.Count(), chain.Array());
        }
      }
    }

    if ( link_count == 2*sorted_edges.UnsignedCount() )
      break; // we've used all the links - no need to "skip over the rest".
  }
#ifdef MYON_DEBUG
  Internal_UnrollChain(sorted_edges);
  if (!Internal_CountAndVerifyEdgeChains(sorted_edges, &chain_count)) {
    return chain_count;
  }
#endif
  return chain_count;
}

const MYON_SHA1_Hash MYON_SubDEdgeChain::Hash() const
{
  MYON_SHA1 sha1;
  const unsigned edge_count = this->EdgeCount();
  for (unsigned i = 0; i < edge_count; ++i)
  {
    const MYON_SubDEdgePtr eptr = EdgePtr(i);
    sha1.AccumulateInteger32(eptr.EdgeId());
    sha1.AccumulateInteger32(eptr.RelativeVertexId(0));
    sha1.AccumulateInteger32(eptr.RelativeVertexId(1));
  }
  return sha1.Hash();
}

unsigned int MYON_SubDEdgeChain::OrientEdgesIntoEdgeChains(
  const MYON_SubD& subd,
  const MYON_SimpleArray< MYON_COMPONENT_INDEX >& edges,
  MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chain
)
{
  const unsigned count = edges.UnsignedCount();
  MYON_SimpleArray< const MYON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
    a.Append(subd.EdgeFromComponentIndex(edges[i]));
  return MYON_SubDEdgeChain::OrientEdgesIntoEdgeChains(a, edge_chain);
}

unsigned int MYON_SubDEdgeChain::OrientEdgesIntoEdgeChains(
  const MYON_SimpleArray< MYON_SubDComponentPtr >& edges,
  MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chain
)
{
  const unsigned count = edges.UnsignedCount();
  MYON_SimpleArray< const MYON_SubDEdge* > a(count);
  for (unsigned i = 0; i < count; ++i)
    a.Append(edges[i].Edge());
  return MYON_SubDEdgeChain::OrientEdgesIntoEdgeChains(a, edge_chain);
}

unsigned int MYON_SubDEdgeChain::OrientEdgesIntoEdgeChains(
  const MYON_SimpleArray< const MYON_SubDEdge* >& edges,
  MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chains
)
{
  const unsigned count = edges.UnsignedCount();
  edge_chains.SetCount(0);
  edge_chains.Reserve(count);
  unsigned int chain_count = 0;
  unsigned chain_length = 0;
  MYON_SubDEdgePtr* prev_eptr = nullptr;
  for (unsigned i = 0; i < count; ++i)
  {
    const MYON_SubDEdge* e = edges[i];
    if (nullptr == e || nullptr == e->m_vertex[0] || nullptr == e->m_vertex[1] || e->m_vertex[0] == e->m_vertex[1])
      continue;
    MYON_SubDEdgePtr& eptr = edge_chains.AppendNew();
    eptr = MYON_SubDEdgePtr::Create(e);
    if (nullptr != prev_eptr && prev_eptr->RelativeVertex(1) != eptr.RelativeVertex(0) )
    {
      const MYON_SubDVertex* prev_v[2] = { prev_eptr->RelativeVertex(0), prev_eptr->RelativeVertex(1) };
      const MYON_SubDVertex* v[2] = { eptr.RelativeVertex(0), eptr.RelativeVertex(1) };
      if (prev_v[1] == v[1])
        eptr = eptr.Reversed();
      else if (1 == chain_length)
      {
        if (prev_v[0] == v[0])
          *prev_eptr = prev_eptr->Reversed();
        else if (prev_v[0] == v[1])
        {
          *prev_eptr = prev_eptr->Reversed();
          eptr = eptr.Reversed();
        }
        else
          prev_eptr = nullptr;
      }
      else
        prev_eptr = nullptr;
    }

    if (nullptr == prev_eptr)
    {
      chain_count = 1;
      chain_length = 0;
    }
    prev_eptr = &eptr;
    ++chain_length;
  }
  return chain_count;
}



MYON_SubDComponentList::MYON_SubDComponentList(const MYON_SubDComponentList& src)
  : m_subd_runtime_serial_number(src.m_subd_runtime_serial_number)
  , m_subd_geometry_content_serial_number(src.m_subd_geometry_content_serial_number)
  , m_subd_render_content_serial_number(src.m_subd_render_content_serial_number)
  , m_subd_vertex_count(src.m_subd_vertex_count)
  , m_subd_edge_count(src.m_subd_edge_count)
  , m_subd_face_count(src.m_subd_face_count)
  , m_component_list(src.m_component_list)
{
  m_subd.ShareDimple(src.m_subd);
}

MYON_SubDComponentList& MYON_SubDComponentList::operator=(const MYON_SubDComponentList& src)
{
  if (this != &src)
  {
    m_subd_runtime_serial_number = src.m_subd_runtime_serial_number;
    m_subd_geometry_content_serial_number = src.m_subd_geometry_content_serial_number;
    m_subd_render_content_serial_number = src.m_subd_render_content_serial_number;
    m_subd_vertex_count = src.m_subd_vertex_count;
    m_subd_edge_count = src.m_subd_edge_count;
    m_subd_face_count = src.m_subd_face_count;
    m_component_list = src.m_component_list;
    m_subd.ShareDimple(src.m_subd);
  }
  return *this;
}

MYON__UINT64 MYON_SubDComponentList::SubDRuntimeSerialNumber() const
{
  return m_subd_runtime_serial_number;
}

MYON__UINT64 MYON_SubDComponentList::SubDGeometryContentSerialNumber() const
{
  return m_subd_geometry_content_serial_number;
}

MYON__UINT64 MYON_SubDComponentList::SubDRenderContentSerialNumber() const
{
  return m_subd_render_content_serial_number;
}

unsigned int MYON_SubDComponentList::Count() const
{
  return m_component_list.UnsignedCount();
}

const MYON_SubDComponentPtr MYON_SubDComponentList::operator[](int i) const
{
  return i >= 0 && i < m_component_list.Count() ? m_component_list[i] : MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentList::operator[](unsigned int i) const
{
  return i < m_component_list.UnsignedCount() ? m_component_list[i] : MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentList::operator[](MYON__INT64 i) const
{
  return i >= 0 && i < ((MYON__INT64)m_component_list.Count()) ? m_component_list[i] : MYON_SubDComponentPtr::Null;
}

const MYON_SubDComponentPtr MYON_SubDComponentList::operator[](MYON__UINT64 i) const
{
  return i < ((MYON__UINT64)m_component_list.UnsignedCount()) ? m_component_list[i] : MYON_SubDComponentPtr::Null;
}

#if defined(MYON_RUNTIME_APPLE)
const MYON_SubDComponentPtr MYON_SubDComponentList::operator[](size_t i) const
{
  return i >= 0 && i < m_component_list.Count() ? m_component_list[i] : MYON_SubDComponentPtr::Null;
}
#endif

const MYON_SimpleArray< MYON_SubDComponentPtr >& MYON_SubDComponentList::ComponentList() const
{
  return this->m_component_list;
}

const MYON_SubD& MYON_SubDComponentList::SubD() const
{
  return m_subd;
}

void MYON_SubDComponentList::UpdateContentSerialNumbers()
{
  m_subd_geometry_content_serial_number = m_subd.GeometryContentSerialNumber();
  m_subd_render_content_serial_number = m_subd.RenderContentSerialNumber();
}

unsigned int MYON_SubDComponentList::UpdateSubDForExperts(const MYON_SubD & subd, bool bUpdateDeletedComponents)
{
  const unsigned count0 = Count();
  if (subd.RuntimeSerialNumber() == m_subd.RuntimeSerialNumber())
    return count0; // the components in this list are in subd.

  // Use the component ids to update the list to reference components in subd.
  unsigned count1 = 0;
  for (unsigned i = 0; i < count0; ++i)
  {
    MYON_SubDComponentPtr cptr0 = m_component_list[i];
    const MYON_SubDComponentBase* c0 = cptr0.ComponentBase();
    if (nullptr == c0)
      continue;
    if (false == bUpdateDeletedComponents && false == c0->IsActive())
      continue;
    MYON_COMPONENT_INDEX ci = cptr0.ComponentIndex();
    if (0 == ci.m_index)
      continue;
    MYON_SubDComponentPtr cptr1 = subd.ComponentPtrFromComponentIndex(ci);
    if (cptr1.IsNull())
      continue;
    if (0 != cptr0.ComponentDirection())
      cptr1.SetComponentDirection();
    m_component_list[count1++] = cptr1;
  }
  m_component_list.SetCount(count1);
  m_subd.ShareDimple(subd);
  m_subd_runtime_serial_number = m_subd.RuntimeSerialNumber();
  m_subd_geometry_content_serial_number = m_subd.GeometryContentSerialNumber();
  m_subd_render_content_serial_number = m_subd.RenderContentSerialNumber();
  return Count();
}

unsigned MYON_SubDComponentList::CreateFromComponentList(const MYON_SubD& subd, const MYON_SimpleArray<MYON_COMPONENT_INDEX>& component_list)
{
  MYON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = component_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const MYON_COMPONENT_INDEX ci = component_list[i];
    if (MYON_COMPONENT_INDEX::TYPE::subd_vertex != ci.m_type)
      continue;
    const unsigned vertex_id = (unsigned)ci.m_index;
    const MYON_SubDVertex* v = subd.VertexFromId(vertex_id);
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, true, true, true, marked_count);
}

unsigned MYON_SubDComponentList::CreateFromComponentList(const MYON_SubD& subd, const MYON_SimpleArray<MYON_SubDComponentPtr>& component_list)
{
  MYON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = component_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const MYON_SubDComponentBase* c = component_list[i].ComponentBase();
    if (nullptr == c)
      continue;
    if (c->m_status.RuntimeMark())
      continue;
    c->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, true, true, true, marked_count);
}

unsigned MYON_SubDComponentList::CreateFromVertexIdList(const MYON_SubD& subd, const MYON_SimpleArray<unsigned>& vertex_id_list)
{
  MYON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = vertex_id_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const unsigned vertex_id = vertex_id_list[i];
    if (vertex_id <= 0 || vertex_id >= MYON_UNSET_UINT_INDEX)
      continue;
    const MYON_SubDVertex* v = subd.VertexFromId(vertex_id);
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, true, marked_count);
}


unsigned MYON_SubDComponentList::CreateFromVertexList(const MYON_SubD& subd, const MYON_SimpleArray<MYON_SubDVertexPtr>& vertex_list)
{
  MYON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = vertex_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const MYON_SubDVertex* v = vertex_list[i].Vertex();
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, true, marked_count);
}

unsigned MYON_SubDComponentList::CreateFromVertexList(const MYON_SubD& subd, const MYON_SimpleArray<const MYON_SubDVertex*>& vertex_list)
{
  MYON_SubDComponentMarksClearAndRestore saved_marks(subd);
  const unsigned count = vertex_list.UnsignedCount();
  unsigned marked_count = 0;
  for (unsigned i = 0; i < count; ++i)
  {
    const MYON_SubDVertex* v = vertex_list[i];
    if (nullptr == v)
      continue;
    if (v->m_status.RuntimeMark())
      continue;
    v->m_status.SetRuntimeMark();
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, true, marked_count);
}

unsigned MYON_SubDComponentList::CreateFromMarkedComponents(const MYON_SubD& subd, bool bComponentInListMark)
{
  unsigned marked_count = 0;
  MYON_SubDComponentIterator cit(subd);
  if (bComponentInListMark)
    bComponentInListMark = true; // avoid other byte values.
  for (MYON_SubDComponentPtr c = cit.FirstComponent(); c.IsNotNull(); c = cit.NextComponent())
  {
    if (bComponentInListMark != c.Mark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, true, true, true, bComponentInListMark, marked_count);
}

unsigned MYON_SubDComponentList::CreateFromMarkedVertices(const MYON_SubD& subd, bool bVertexInListMark)
{
  unsigned marked_count = 0;
  MYON_SubDVertexIterator vit(subd);
  if (bVertexInListMark)
    bVertexInListMark = true; // avoid other byte values.
  for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
  {
    if (bVertexInListMark != v->m_status.RuntimeMark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, true, false, false, bVertexInListMark, marked_count);
}

unsigned MYON_SubDComponentList::CreateFromMarkedEdges(const MYON_SubD& subd, bool bEdgeInListMark)
{
  unsigned marked_count = 0;
  MYON_SubDEdgeIterator eit(subd);
  if (bEdgeInListMark)
    bEdgeInListMark = true; // avoid other byte values.
  for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
  {
    if (bEdgeInListMark != e->m_status.RuntimeMark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, false, true, false, bEdgeInListMark, marked_count);
}

unsigned MYON_SubDComponentList::CreateFromMarkedFaces(const MYON_SubD& subd, bool bFaceInListMark)
{
  unsigned marked_count = 0;
  MYON_SubDFaceIterator fit(subd);
  if (bFaceInListMark)
    bFaceInListMark = true; // avoid other byte values.
  for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
  {
    if (bFaceInListMark != f->m_status.RuntimeMark())
      continue;
    ++marked_count;
  }
  return Internal_Create(subd, false, false, true, bFaceInListMark, marked_count);
}

unsigned MYON_SubDComponentList::Internal_Create(const MYON_SubD & subd, bool bAddVertices, bool bAddEdges, bool bAddFaces, bool bComponentInListMark, unsigned marked_component_count)
{
  Destroy();

  if (0 == marked_component_count)
    return 0;

  const unsigned face_count = bAddFaces ? subd.FaceCount() : 0U;
  const unsigned edge_count = bAddEdges ? subd.EdgeCount() : 0U;
  const unsigned vertex_count = bAddVertices ? subd.VertexCount() : 0U;
  if (0 == vertex_count && 0 == edge_count && 0 == face_count)
    return 0;

  if (marked_component_count > vertex_count + edge_count + face_count)
    return 0;

  bComponentInListMark = bComponentInListMark ? true : false;
  m_component_list.Reserve(marked_component_count);
  m_component_list.SetCount(0);
  if (vertex_count > 0)
  {
    MYON_SubDVertexIterator vit(subd);
    for (const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      if (bComponentInListMark != v->m_status.RuntimeMark())
        continue;
      m_component_list.Append(v->ComponentPtr());
    }
  }
  if (edge_count > 0)
  {
    MYON_SubDEdgeIterator eit(subd);
    for (const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      if (bComponentInListMark != e->m_status.RuntimeMark())
        continue;
      m_component_list.Append(e->ComponentPtr());
    }
  }
  if (face_count > 0)
  {
    MYON_SubDFaceIterator fit(subd);
    for (const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      if (bComponentInListMark != f->m_status.RuntimeMark())
        continue;
      m_component_list.Append(f->ComponentPtr());
    }
  }

  if (m_component_list.UnsignedCount() > 0)
  {
    m_subd.ShareDimple(subd);
    m_subd_runtime_serial_number = subd.RuntimeSerialNumber();
    m_subd_geometry_content_serial_number = subd.GeometryContentSerialNumber();
    m_subd_render_content_serial_number = subd.RenderContentSerialNumber();
  }
  return m_component_list.UnsignedCount();
}

unsigned int MYON_SubDComponentList::RemoveAllComponents()
{
  const unsigned count0 = Count();
  m_component_list.SetCount(0);
  return count0;
}

unsigned int MYON_SubDComponentList::RemoveAllVertices()
{
  return Internal_RemoveComponents(true, false, false);
}

unsigned int MYON_SubDComponentList::RemoveAllEdges()
{
  return Internal_RemoveComponents(false, true, false);
}

unsigned int MYON_SubDComponentList::RemoveAllFaces()
{
  return Internal_RemoveComponents(false, false, true);
}

unsigned MYON_SubDComponentList::Internal_RemoveComponents(
  bool bRemoveVertices,
  bool bRemoveEdges,
  bool bRemoveFaces
)
{
  unsigned int count0 = Count();
  if (bRemoveVertices || bRemoveEdges || bRemoveFaces)
  {
    unsigned count1 = 0;
    for (unsigned i = 0; i < count0; ++i)
    {
      const MYON_SubDComponentPtr cptr = m_component_list[i];
      bool bRemove = false;
      switch (cptr.ComponentType())
      {
      case MYON_SubDComponentPtr::Type::Vertex:
        bRemove = bRemoveVertices;
        break;
      case MYON_SubDComponentPtr::Type::Edge:
        bRemove = bRemoveEdges;
        break;
      case MYON_SubDComponentPtr::Type::Face:
        bRemove = bRemoveFaces;
        break;
      default:
        bRemove = true;
      }
      if (bRemove)
        continue;
      m_component_list[count1++] = cptr;
    }
    m_component_list.SetCount(count1);
  }
  return count0 - Count();
}


void MYON_SubDComponentList::Destroy()
{
  m_subd_runtime_serial_number = 0;
  m_subd_geometry_content_serial_number = 0;
  m_subd_render_content_serial_number = 0;
  m_component_list.SetCount(0);
  m_subd.ShareDimple(MYON_SubD::Empty);
}

const MYON_SubDComponentFilter MYON_SubDComponentFilter::Create(
  bool bAcceptVertices,
  bool bAcceptEdges,
  bool bAcceptFaces
)
{
  MYON_SubDComponentFilter f;
  if (false == bAcceptVertices)
    f.m_bRejectVertices = true;
  if (false == bAcceptEdges)
    f.m_bRejectEdges = true;
  if (false == bAcceptFaces)
    f.m_bRejectFaces = true;
  return f;
}

bool MYON_SubDComponentFilter::AcceptComponent(MYON_COMPONENT_INDEX component_index, const class MYON_Geometry* geometry) const
{
  if (false == component_index.IsSubDComponentIndex())
    return false;
  const MYON_SubDComponentRef* cref = MYON_SubDComponentRef::Cast(geometry);
  if (nullptr == cref)
    return false;
  const MYON_SubDComponentPtr cptr = cref->ComponentPtr();
  if (component_index.m_index != (int)cptr.ComponentId())
    return false;
  switch (component_index.m_type)
  {
  case MYON_COMPONENT_INDEX::TYPE::subd_vertex:
    if (MYON_SubDComponentPtr::Type::Vertex != cptr.ComponentType())
      return false;
    break;
  case MYON_COMPONENT_INDEX::TYPE::subd_edge:
    if (MYON_SubDComponentPtr::Type::Edge != cptr.ComponentType())
      return false;
    break;
  case MYON_COMPONENT_INDEX::TYPE::subd_face:
    if (MYON_SubDComponentPtr::Type::Face != cptr.ComponentType())
      return false;
    break;
  default: // 30 unhandled enum values
    break;
  }
  return AcceptComponent(cptr);
}

bool MYON_SubDComponentFilter::AcceptComponent(const class MYON_Geometry* geometry) const
{
  return AcceptComponent(MYON_SubDComponentRef::Cast(geometry));
}


bool MYON_SubDComponentFilter::AcceptComponent(const class MYON_SubDComponentRef* cref) const
{
  return (nullptr != cref) ? AcceptComponent(cref->ComponentPtr()) : false;
}

bool MYON_SubDComponentFilter::AcceptComponent(MYON_SubDComponentPtr cptr) const
{
  switch (cptr.ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    return AcceptVertex(cptr.Vertex());
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    return AcceptEdge(cptr.Edge());
    break;
  case MYON_SubDComponentPtr::Type::Face:
    return AcceptFace(cptr.Face());
    break;
  case MYON_SubDComponentPtr::Type::Unset:
    break;
  }
  return false;
}

bool MYON_SubDComponentFilter::AcceptVertex(MYON_SubDVertexPtr vptr) const
{
  return AcceptVertex(vptr.Vertex());
}

bool MYON_SubDComponentFilter::AcceptEdge(MYON_SubDEdgePtr eptr) const
{
  return AcceptEdge(eptr.Edge());
}


bool MYON_SubDComponentFilter::AcceptFace(MYON_SubDFacePtr fptr) const
{
  return AcceptFace(fptr.Face());
}

bool MYON_SubDComponentFilter::AcceptVertex(const MYON_SubDVertex * v) const
{
  if (m_bRejectVertices)
    return false;

  if (nullptr == v)
    return false;

  if (false == AcceptVertexTag(v->m_vertex_tag))
    return false;

  if (MYON_SubDComponentFilter::Topology::Unset != m_vertex_topology_filter)
  {
    // check boundary/interior/nonmanifold
    if (v->HasBoundaryVertexTopology())
    {
      if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Boundary)& static_cast<unsigned char>(m_vertex_topology_filter)))
        return false;
    }
    else if (v->HasInteriorVertexTopology())
    {
      if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Interior)& static_cast<unsigned char>(m_vertex_topology_filter)))
        return false;
    }
    else
    {
      if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Nonmanifold)& static_cast<unsigned char>(m_vertex_topology_filter)))
        return false;
    }
  }

  return true;
}

bool MYON_SubDComponentFilter::AcceptEdge(const MYON_SubDEdge * e) const
{
  if (m_bRejectEdges)
    return false;

  if (nullptr == e)
    return false;

  if (false == AcceptEdgeTag(e->m_edge_tag))
    return false;

  if (MYON_SubDComponentFilter::Topology::Unset != m_edge_topology_filter)
  {
    // check boundary/interior/nonmanifold
    if (1 == e->m_face_count)
    {
      if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Boundary)& static_cast<unsigned char>(m_edge_topology_filter)))
        return false;
    }
    else if (2 == e->m_face_count)
    {
      if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Interior)& static_cast<unsigned char>(m_edge_topology_filter)))
        return false;
    }
    else
    {
      if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Nonmanifold)& static_cast<unsigned char>(m_edge_topology_filter)))
        return false;
    }
  }

  return true;
}

bool MYON_SubDComponentFilter::AcceptFace(const MYON_SubDFace * f) const
{
  if (m_bRejectFaces)
    return false;

  if (nullptr == f)
    return false;

  if (m_maximum_face_edge_count > 0U)
  {
    const unsigned face_edge_count = f->m_edge_count;
    if (face_edge_count < m_minimum_face_edge_count || face_edge_count > m_maximum_face_edge_count)
      return false;
  }

  if (MYON_SubDComponentFilter::Topology::Unset != m_edge_topology_filter)
  {
    const MYON_SubDEdgePtr* eptr = f->m_edge4;
    for (unsigned short fei = 0; fei < f->m_edge_count; ++fei, ++eptr)
    {
      if (4 == fei)
      {
        eptr = f->m_edgex;
        if (nullptr == eptr)
          break;
      }
      const MYON_SubDEdge* e = eptr->Edge();
      if (nullptr == e)
        continue;
      if (1 == e->m_face_count)
      {
        if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Boundary)& static_cast<unsigned char>(m_edge_topology_filter)))
          return false;
      }
      else if (2 == e->m_face_count)
      {
        if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Interior)& static_cast<unsigned char>(m_edge_topology_filter)))
          return false;
      }
      else
      {
        if (0 == (static_cast<unsigned char>(MYON_SubDComponentFilter::Topology::Nonmanifold)& static_cast<unsigned char>(m_edge_topology_filter)))
          return false;
      }
    }
  }

  return true;
}

void MYON_SubDComponentFilter::SetAcceptVertices(bool bAcceptVertices)
{
  m_bRejectVertices = bAcceptVertices ? false : true;
}

bool MYON_SubDComponentFilter::AcceptVertices() const
{
  return false == m_bRejectVertices;
}

void MYON_SubDComponentFilter::SetAcceptEdges(bool bAcceptEdges)
{
  m_bRejectEdges = bAcceptEdges ? false : true;
}

bool MYON_SubDComponentFilter::AcceptEdges() const
{
  return false == m_bRejectEdges;
}

void MYON_SubDComponentFilter::SetAcceptFaces(bool bAcceptFaces)
{
  m_bRejectFaces = bAcceptFaces ? false : true;
}

bool MYON_SubDComponentFilter::AcceptFaces() const
{
  return false == m_bRejectFaces;
}

void MYON_SubDComponentFilter::SetVertexTopologyFilter(MYON_SubDComponentFilter::Topology vertex_topology_filter)
{
  m_vertex_topology_filter = vertex_topology_filter;
}

void MYON_SubDComponentFilter::ClearVertexTopologyFilter()
{
  m_vertex_topology_filter = MYON_SubDComponentFilter::Topology::Unset;
}

MYON_SubDComponentFilter::Topology MYON_SubDComponentFilter::VertexTopologyFilter() const
{
  return m_vertex_topology_filter;
}

void MYON_SubDComponentFilter::SetEdgeTopologyFilter(MYON_SubDComponentFilter::Topology edge_topology_filter)
{
  m_edge_topology_filter = edge_topology_filter;
}

MYON_SubDComponentFilter::Topology MYON_SubDComponentFilter::EdgeTopologyFilter() const
{
  return m_edge_topology_filter;
}

void MYON_SubDComponentFilter::ClearEdgeTopologyFilter()
{
  m_edge_topology_filter = MYON_SubDComponentFilter::Topology::Unset;
}

void MYON_SubDComponentFilter::SetFaceTopologyFilter(MYON_SubDComponentFilter::Topology face_topology_filter)
{
  m_face_topology_filter = face_topology_filter;
}

MYON_SubDComponentFilter::Topology MYON_SubDComponentFilter::FaceTopologyFilter() const
{
  return m_face_topology_filter;
}

void MYON_SubDComponentFilter::ClearFaceTopologyFilter()
{
  m_face_topology_filter = MYON_SubDComponentFilter::Topology::Unset;
}

bool MYON_SubDComponentFilter::AcceptVertexTag(MYON_SubDVertexTag vertex_tag) const
{
  if (MYON_SubDVertexTag::Unset == m_vertex_tag_filter[0])
    return true; // no tag filter

  for (size_t i = 0; i < sizeof(m_vertex_tag_filter) / sizeof(m_vertex_tag_filter[0]); ++i)
  {
    if (MYON_SubDVertexTag::Unset == m_vertex_tag_filter[i])
      break;
    if (m_vertex_tag_filter[i] != vertex_tag)
      continue;
    return true;
  }
  return false;
}

void MYON_SubDComponentFilter::AddAcceptedVertexTag(MYON_SubDVertexTag vertex_tag)
{
  for (size_t i = 0; i < sizeof(m_vertex_tag_filter) / sizeof(m_vertex_tag_filter[0]); ++i)
  {
    if (vertex_tag == m_vertex_tag_filter[i])
      break;
    if (MYON_SubDVertexTag::Unset == m_vertex_tag_filter[i])
    {
      m_vertex_tag_filter[i] = vertex_tag;
      break;
    }
  }
}

void MYON_SubDComponentFilter::ClearVertexTagFilter()
{
  for (size_t i = 0; i < sizeof(m_vertex_tag_filter) / sizeof(m_vertex_tag_filter[0]); ++i)
    m_vertex_tag_filter[i] = MYON_SubDVertexTag::Unset;
}


bool MYON_SubDComponentFilter::AcceptEdgeTag(MYON_SubDEdgeTag edge_tag) const
{
  if (MYON_SubDEdgeTag::Unset == m_edge_tag_filter[0])
    return true; // no tag filter

  for (size_t i = 0; i < sizeof(m_edge_tag_filter) / sizeof(m_edge_tag_filter[0]); ++i)
  {
    if (MYON_SubDEdgeTag::Unset == m_edge_tag_filter[i])
      break;
    if (m_edge_tag_filter[i] != edge_tag)
      continue;
    return true;
  }
  return false;
}

void MYON_SubDComponentFilter::AddAcceptedEdgeTag(MYON_SubDEdgeTag edge_tag)
{
  for (size_t i = 0; i < sizeof(m_edge_tag_filter) / sizeof(m_edge_tag_filter[0]); ++i)
  {
    if (edge_tag == m_edge_tag_filter[i])
      break;
    if (MYON_SubDEdgeTag::Unset == m_edge_tag_filter[i])
    {
      m_edge_tag_filter[i] = edge_tag;
      break;
    }
  }
}

void MYON_SubDComponentFilter::ClearEdgeTagFilter()
{
  for (size_t i = 0; i < sizeof(m_edge_tag_filter) / sizeof(m_edge_tag_filter[0]); ++i)
    m_edge_tag_filter[i] = MYON_SubDEdgeTag::Unset;
}

bool MYON_SubDComponentFilter::AcceptFaceEdgeCount(
  unsigned face_edge_count
) const
{
  return (m_maximum_face_edge_count >= 3U) ? (face_edge_count >= m_minimum_face_edge_count && face_edge_count <= m_maximum_face_edge_count) : false;
}

void MYON_SubDComponentFilter::SetFaceEdgeCountFilter(
  unsigned minimum_face_edge_count,
  unsigned maximum_face_edge_count
)
{
  if (minimum_face_edge_count <= maximum_face_edge_count && maximum_face_edge_count >= 3U)
  {
    m_minimum_face_edge_count = minimum_face_edge_count;
    m_maximum_face_edge_count = maximum_face_edge_count;
  }
}

void MYON_SubDComponentFilter::ClearFaceEdgeCountFilter()
{
  m_minimum_face_edge_count = 0U;
  m_maximum_face_edge_count = 0U;
}


double MYON_SubD::SurfacePointRadiusFromControlPointRadius(unsigned int polygon_count, double polgon_radius)
{
  for (;;)
  {
    if (polygon_count < 3)
      break;
    if (false == MYON_IsValid(polgon_radius))
      break;
    const double a = MYON_2PI / ((double)polygon_count);
    MYON_2dPoint cv[4] = {
      MYON_2dPoint(1,0),
      MYON_2dPoint(cos(a),sin(a)),
      MYON_2dPoint(cos(2 * a),sin(2 * a)),
      MYON_2dPoint(cos(3 * a),sin(3 * a))
    };
    double k[6] = { -2,-1,0,1,2,3 };
    MYON_NurbsCurve c;
    c.m_dim = 2;
    c.m_order = 4;
    c.m_cv_count = 4;
    c.m_cv = &cv[0].x;
    c.m_cv_stride = (int)(&cv[1].x - &cv[0].x);
    c.m_knot = k;
    const MYON_3dPoint p = c.PointAt(0.0);
    const double r = MYON_2dPoint(p.x, p.y).DistanceTo(MYON_2dPoint::Origin);
    if (r > 0.0)
      return polgon_radius * r;
    break;
  }
  return MYON_DBL_QNAN;
}


double MYON_SubD::ControlPointRadiusFromSurfacePointRadius(unsigned int polygon_count, double surface_radius)
{
  for (;;)
  {
    if (false == MYON_IsValid(surface_radius))
      break;
    const double r = MYON_SubD::SurfacePointRadiusFromControlPointRadius(polygon_count, 1.0);
    if (r > 0.0)
      return surface_radius / r;
    break;
  }
  return MYON_DBL_QNAN;
}

MYON_SubDFace* MYON_SubD::FindOrAddFace(
  MYON_SubDEdgeTag new_edge_tag,
  const MYON_SubDVertex* face_vertices[],
  size_t vertex_count
)
{
  if (nullptr == face_vertices)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (vertex_count < 3)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if (vertex_count > (size_t)MYON_SubDFace::MaximumEdgeCount)
    return MYON_SUBD_RETURN_ERROR(nullptr);


  // Mkae sure v[] has vertex_count unique non-null vertices.
  for (unsigned i = 0; i < vertex_count; ++i)
  {
    if (nullptr == face_vertices[i])
      return MYON_SUBD_RETURN_ERROR(nullptr);
    for (unsigned j = i + 1; j < vertex_count; ++j)
    {
      if (face_vertices[i] == face_vertices[j])
        return MYON_SUBD_RETURN_ERROR(nullptr);
    }
  }

  MYON_SimpleArray<MYON_SubDEdgePtr> eptrs(vertex_count);
  MYON_SimpleArray<const MYON_SubDFace*> faces(4);
  MYON_SimpleArray<const MYON_SubDFace*> faces_to_keep(4);
  const MYON_SubDVertex* ev[2] = { nullptr, face_vertices[0] };
  for (unsigned i = 0; i < vertex_count; ++i)
  {
    ev[0] = ev[1];
    ev[1] = face_vertices[(i + 1) % vertex_count];
    MYON_SubDEdgePtr eptr = MYON_SubDEdge::FromVertices(ev[0], ev[1]);
    if (eptr.IsNull())
    {
      // need to create this edge
      MYON_SubDEdge* e = this->AddEdge(new_edge_tag, const_cast<MYON_SubDVertex*>(ev[0]), const_cast<MYON_SubDVertex*>(ev[1]));
      if (nullptr == e)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      eptr = MYON_SubDEdgePtr::Create(e, 0);
      faces.SetCount(0);
    }
    else
    {
      const MYON_SubDEdge* e = eptr.Edge();
      if (nullptr == e)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      if (0 == e->m_face_count)
        faces.SetCount(0);
      else if (0 == i || faces.Count() > 0)
      {
        faces_to_keep.SetCount(0);
        const MYON_SubDFacePtr* fptr = e->m_face2;
        for (unsigned short efi = 0; efi < e->m_face_count; ++efi, ++fptr)
        {
          if (2 == efi)
          {
            fptr = e->m_facex;
            if (nullptr == fptr)
              break;
          }
          const MYON_SubDFace* f = MYON_SUBD_FACE_POINTER(fptr->m_ptr);
          if (nullptr == f)
            continue;
          if (0 == i)
            faces_to_keep.Append(f);
          else
          {
            for (unsigned j = 0; j < faces.UnsignedCount(); ++j)
            {
              if (f != faces[j])
                continue;
              // every edge so far is attached to f
              faces_to_keep.Append(f);
              break;
            }
          }
        }
        faces = faces_to_keep;
      }
    }
    eptrs.Append(eptr);
  }

  if (eptrs.UnsignedCount() != vertex_count)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  MYON_SubDFace* new_face = this->AddFace(eptrs);

  return new_face;
}

MYON_SubD* MYON_SubD::CreateCylinder(
  const MYON_Cylinder& cylinder,
  unsigned circumference_face_count,
  unsigned height_face_count,
  MYON_SubDEndCapStyle end_cap_style,
  MYON_SubDEdgeTag end_cap_edge_tag,
  MYON_SubDComponentLocation radius_location,
  MYON_SubD* destination_subd
)
{
  if (nullptr != destination_subd)
    *destination_subd = MYON_SubD::Empty;

  if (false == cylinder.IsValid())
  {
    MYON_SUBD_ERROR("Invalid cylinder parameter.");
    return nullptr;
  }

  if (circumference_face_count < 3)
  {
    MYON_SUBD_ERROR("Invalid circumference_face_count parameter.");
    return nullptr;
  }

  if (height_face_count < 1)
  {
    MYON_SUBD_ERROR("Invalid height_face_count parameter.");
    return nullptr;
  }

  const double r = cylinder.circle.Radius();
  if (false == (r > 0.0 && r < MYON_UNSET_POSITIVE_VALUE))
  {
    MYON_SUBD_ERROR("Invalid cylinder radius parameter.");
    return nullptr;
  }

  ////////////////////////////////////////////
  // Validate and sanitize end_cap_style parameter
  //
  switch (end_cap_style)
  {
  case MYON_SubDEndCapStyle::Unset:
    end_cap_style = MYON_SubDEndCapStyle::None;
    break;

  case MYON_SubDEndCapStyle::None:
    break;

  case MYON_SubDEndCapStyle::Triangles:
    if (circumference_face_count < 2)
      end_cap_style = MYON_SubDEndCapStyle::None;
    else if (circumference_face_count <= 3)
      end_cap_style = MYON_SubDEndCapStyle::Ngon; // single triangle
    break;

  case MYON_SubDEndCapStyle::Quads:
    if (circumference_face_count < 2)
      end_cap_style = MYON_SubDEndCapStyle::None;
    else if (circumference_face_count <= 4)
      end_cap_style = MYON_SubDEndCapStyle::Ngon; // single quad or single triangle
    else if (0 != (circumference_face_count%2))
      end_cap_style = MYON_SubDEndCapStyle::Triangles; // must have even number of sized for a multi-quad cap.
    break;

  case MYON_SubDEndCapStyle::Ngon:
    if (circumference_face_count < 2)
      end_cap_style = MYON_SubDEndCapStyle::None;
    break;

  default:
    end_cap_style = MYON_SubDEndCapStyle::None;
    break;
  }
  const bool bCapEnds = MYON_SubDEndCapStyle::None != end_cap_style;

  ///////////////////////////////////////////////
  // If cylinder in infinite, choose a height that makes the faces squarish.
  //
  const double height = cylinder.IsFinite() ? cylinder.Height() : (MYON_2PI*r/((double)circumference_face_count))*((double)height_face_count);
  if ( false == (MYON_IsValid(height) && 0.0 != height) )
  {
    MYON_SUBD_ERROR("Invalid cylinder or count parameters.");
    return nullptr;
  }

  /////////////////////////////////////////////
  // H = vector that translates a ring of vertices / edges from one circumference to the next.
  const MYON_3dVector H = (height / ((double)height_face_count)) * cylinder.Axis().UnitVector();

  /////////////////////////////////////////////
  // Adjust radius so result has surface and control net in the correct location.
  //
  MYON_Circle point_generator(cylinder.IsFinite() ? cylinder.CircleAt(cylinder.height[0]) : cylinder.circle);
  point_generator.radius = (MYON_SubDComponentLocation::Surface == radius_location) ? MYON_SubD::ControlPointRadiusFromSurfacePointRadius(circumference_face_count, r) : r;


  //////////////////////////////////////////////
  // circumference_points[] = ring of control point locations around the cylinder's base.
  //
  MYON_SimpleArray<MYON_3dPoint> circumference_points(circumference_face_count);
  for (unsigned i = 0; i < circumference_face_count; ++i)
  {
    const double a = MYON_Interval::ZeroToTwoPi.ParameterAt(((double)i) / ((double)circumference_face_count));
    const MYON_3dPoint P = point_generator.PointAt(a);
    circumference_points.Append(P);
  }

  /////////////////////////////////////////////
  // center[2] = cap centers (if needed).
  const MYON_3dPoint center[2] = { point_generator.Center(), point_generator.Center() + (((double)height_face_count) * H) };

  MYON_SubD* subd = (nullptr != destination_subd) ? destination_subd : new MYON_SubD();

  // v00 = 1st vertex in the previous ring of vertices / edges
  MYON_SubDVertex* v00 = nullptr;
  for (unsigned j = 0; j <= height_face_count; ++j)
  {
    // add a new ring of vertices / edges

    // v0 = 1st vertex in this ring of vertices / edges
    MYON_SubDVertex* v0 = nullptr;
    MYON_SubDVertex* ev[2] = {};

    const MYON_SubDVertexTag vtag
      = (false == bCapEnds || MYON_SubDEdgeTag::Crease == end_cap_edge_tag) && (0 == j || j == height_face_count)
      ? MYON_SubDVertexTag::Crease
      : MYON_SubDVertexTag::Smooth;
    const MYON_SubDEdgeTag etag = (MYON_SubDVertexTag::Crease == vtag) ? MYON_SubDEdgeTag::Crease : MYON_SubDEdgeTag::Smooth;


    for (unsigned i = 0; i < circumference_face_count; ++i)
    {
      const MYON_3dPoint P = circumference_points[i];
      circumference_points[i] = P + H; // move circumference_points[i] up to next ring

      ev[0] = ev[1];
      ev[1] = subd->AddVertex(vtag, P);
      if (0 == i)
        v0 = ev[1];
      else
        subd->AddEdge(etag, ev[0], ev[1]);
    }
    subd->AddEdge(etag, ev[1], v0);

    if (j > 0 && nullptr != v00 && nullptr != v0)
    {
      // add a new ring of faces

      const MYON_SubDVertex* fv[4] = {
        nullptr,
        v00, // 1st vertex in bottom face edge ring
        v0, // 1st vertex in top face edge ring
        nullptr
      };
      for (unsigned i = 0; i < circumference_face_count; ++i)
      {
        const bool bLastFaceInThisRing = (i+1 == circumference_face_count);
        // shift to next face's corners
        fv[0] = fv[1];
        fv[3] = fv[2];
        fv[1] = bLastFaceInThisRing ? v00 : fv[0]->m_next_vertex;
        fv[2] = bLastFaceInThisRing ? v0 : fv[3]->m_next_vertex;
        
        // make a new face
        subd->FindOrAddFace(MYON_SubDEdgeTag::Smooth, fv, 4);
      }
    }

    v00 = v0;
  }

  if (bCapEnds)
  {
    MYON_SimpleArray<MYON_SubDEdgePtr> bdry(circumference_face_count);
    const MYON_SubDVertex* last_wall_vertex = subd->LastVertex();
    const MYON_SubDEdge* last_wall_edge = subd->LastEdge();

    const bool bCapHasCenterVertex = MYON_SubDEndCapStyle::Triangles == end_cap_style || MYON_SubDEndCapStyle::Quads == end_cap_style;

    for (unsigned enddex = 0; enddex < 2; ++enddex)
    {
      bdry.SetCount(0);
      const MYON_SubDVertex* v0 = (0 == enddex) ? const_cast<MYON_SubDVertex*>(subd->FirstVertex()) : v00;
      const MYON_SubDVertex* ev[2] = { nullptr, v0 };
      for (unsigned i = 0; i < circumference_face_count; ++i)
      {
        ev[0] = ev[1];
        ev[1] = (i + 1 < circumference_face_count) ? const_cast<MYON_SubDVertex*>(ev[1]->m_next_vertex) : v0;
        const MYON_SubDEdgePtr eptr = MYON_SubDEdge::FromVertices(ev[0], ev[1]);
        if (eptr.IsNull())
          break;
        bdry.Append(eptr);
      }
      if (circumference_face_count != bdry.UnsignedCount())
        break;

      if (0 == enddex)
        MYON_SubDEdgeChain::ReverseEdgeChain(bdry);

      const MYON_SubDVertex* fv[4] = {};
      if (bCapHasCenterVertex)
      {
        // fv[0] = vertex at the center of the cap
        fv[0] = subd->AddVertex(MYON_SubDVertexTag::Smooth, center[enddex]);
        if (nullptr == fv[0])
          break;
      }

      switch (end_cap_style)
      {
      case MYON_SubDEndCapStyle::Unset:
        break;

      case MYON_SubDEndCapStyle::None:
        break;

      case MYON_SubDEndCapStyle::Triangles:
        if (nullptr != fv[0])
        {
          // radial triangles around the center vertex
          fv[2] = bdry[0].RelativeVertex(0);
          for (unsigned i = 0; i < circumference_face_count; ++i)
          {
            fv[1] = fv[2];
            fv[2] = bdry[i].RelativeVertex(1);
            subd->FindOrAddFace(MYON_SubDEdgeTag::Smooth, fv, 3);
          }
        }
        break;

      case MYON_SubDEndCapStyle::Quads:
        if (nullptr != fv[0])
        {
          // radial quads around the center vertex
          fv[3] = bdry[0].RelativeVertex(0);
          for (unsigned i = 0; i < circumference_face_count; i += 2)
          {
            fv[1] = fv[3];
            fv[2] = bdry[i].RelativeVertex(1);
            fv[3] = bdry[(i+1)% circumference_face_count].RelativeVertex(1);
            subd->FindOrAddFace(MYON_SubDEdgeTag::Smooth, fv, 4);
          }
        }
        break;

      case MYON_SubDEndCapStyle::Ngon:
        // cap = single n-gon
        subd->AddFace(bdry);
        break;
      default:
        break;
      }
    }

    if (bCapHasCenterVertex)
    {
      // vertices and edges added inside the caps are smooth.
      for (const MYON_SubDVertex* v = (nullptr != last_wall_vertex) ? last_wall_vertex->m_next_vertex : nullptr; nullptr != v; v = v->m_next_vertex)
        const_cast<MYON_SubDVertex*>(v)->m_vertex_tag = MYON_SubDVertexTag::Smooth;
      for (const MYON_SubDEdge* e = (nullptr != last_wall_edge) ? last_wall_edge->m_next_edge : nullptr; nullptr != e; e = e->m_next_edge)
        const_cast<MYON_SubDEdge*>(e)->m_edge_tag = MYON_SubDEdgeTag::Smooth;
    }
  }

  if (nullptr != subd)
    subd->UpdateAllTagsAndSectorCoefficients(true);

  return subd;
}

bool MYON_Symmetry::SetSymmetricObject(const MYON_SubD* subd) const
{
  const MYON_SubDimple* subdimple = (nullptr != subd) ? subd->SubDimple() : nullptr;
  return SetSymmetricObject(subdimple);
}

bool MYON_Symmetry::SetSymmetricObject(const MYON_SubDimple* subdimple) const
{
  bool rc;

  if (nullptr != subdimple && this->IsSet())
  {
    m_symmetric_object_content_serial_number = subdimple->GeometryContentSerialNumber();
    m_symmetric_object_topology_hash = subdimple->SubDHash(MYON_SubDHashType::Topology, false).SubDHash();
    m_symmetric_object_geometry_hash = subdimple->SubDHash(MYON_SubDHashType::Geometry, false).SubDHash();
    rc = true;
  }
  else
  {
    ClearSymmetricObject();
    rc = false;
  }

  return rc;
}

double MYON_SubDExpandEdgesParameters::ConstantOffset() const
{
  return this->m_constant_offset;
}

void MYON_SubDExpandEdgesParameters::SetConstantOffset(double offset)
{
  offset = MYON_SubDExpandEdgesParameters::CleanupOffset(offset);
  if (MYON_SubDExpandEdgesParameters::IsValidConstantOffset(offset))
    this->m_constant_offset = offset;
}


bool MYON_SubDExpandEdgesParameters::IsValidForHalfOffset(
  const MYON_SimpleArray<MYON_SubDEdgePtr>& edge_chain
)
{
  const unsigned count = edge_chain.UnsignedCount();
  for (unsigned i = 0; i < count; ++i)
  {
    if (false == edge_chain[i].HasInteriorEdgeTopology(true))
      return false;
  }
  return MYON_SubDEdgeChain::IsSingleEdgeChain(edge_chain);
}


bool MYON_SubDExpandEdgesParameters::IsValidForVariableOffset(
  const MYON_SimpleArray<MYON_SubDEdgePtr>& edge_chain
)
{
  bool bIsClosed = false;
  bool bIsSorted = false;
  const bool IsSingleEdgeChain = MYON_SubDEdgeChain::IsSingleEdgeChain(edge_chain, bIsClosed, bIsSorted);
  return IsSingleEdgeChain && false == bIsClosed;
}

const MYON_Interval MYON_SubDExpandEdgesParameters::VariableOffset() const
{
  return this->m_variable_offsets;
}

void MYON_SubDExpandEdgesParameters::SetVariableOffset(MYON_Interval variable_offsets)
{
  variable_offsets[0] = MYON_SubDExpandEdgesParameters::CleanupOffset(variable_offsets[0]);
  variable_offsets[1] = MYON_SubDExpandEdgesParameters::CleanupOffset(variable_offsets[1]);
  if (MYON_SubDExpandEdgesParameters::IsValidVariableOffset(variable_offsets))
  {
    this->m_variable_offsets = variable_offsets;
  }
  else
  {
    // invalid input
    ClearVariableOffset();
    if (MYON_SubDExpandEdgesParameters::IsValidConstantOffset(variable_offsets[0])
      && fabs(variable_offsets[0] - variable_offsets[1]) <= MYON_SubDExpandEdgesParameters::OffsetTolerance
      )
    {
      SetConstantOffset(variable_offsets[0]);
    }
  }
}

void MYON_SubDExpandEdgesParameters::ClearVariableOffset()
{
  this->m_variable_offsets = MYON_Interval::Nan;
}

bool MYON_SubDExpandEdgesParameters::IsVariableOffset() const
{
  return MYON_SubDExpandEdgesParameters::IsValidVariableOffset(this->m_variable_offsets);
}

bool MYON_SubDExpandEdgesParameters::IsValidConstantOffset(
  double constant_offset_candidate
)
{
  return constant_offset_candidate >= MYON_SubDExpandEdgesParameters::MinimumOffset && constant_offset_candidate <= MYON_SubDExpandEdgesParameters::MaximumOffset;
}

bool MYON_SubDExpandEdgesParameters::IsValidVariableOffset(
  MYON_Interval variable_offset_candidate
)
{
  for (int i = 0; i < 2; i++)
  {
    const double x[2] = { variable_offset_candidate[i], variable_offset_candidate[1 - i] };
    if (false == MYON_SubDExpandEdgesParameters::IsValidConstantOffset(x[0]))
    {
      if (0.0 == x[0])
        return (x[1] >= MYON_SubDExpandEdgesParameters::MinimumOffset && x[1] <= 1.0);
      if (1.0 == x[0])
        return (x[1] >= 0.0 && x[1] <= MYON_SubDExpandEdgesParameters::MaximumOffset);
      return false;
    }
  }
  return fabs(variable_offset_candidate[0] - variable_offset_candidate[1]) > MYON_SubDExpandEdgesParameters::OffsetTolerance;
}


MYON_SubDExpandEdgesParameters::Style MYON_SubDExpandEdgesParameters::FaceStyle() const
{
  return this->m_face_style;
}

void MYON_SubDExpandEdgesParameters::SetFaceStyle(MYON_SubDExpandEdgesParameters::Style face_style)
{
  this->m_face_style = face_style;
}

bool MYON_SubDExpandEdgesParameters::IsHalfFaceStyle() const
{
  return
    MYON_SubDExpandEdgesParameters::Style::HalfLeft == this->m_face_style
    || MYON_SubDExpandEdgesParameters::Style::HalfRight == this->m_face_style
    ;
}

const MYON_Color MYON_SubDExpandEdgesParameters::FaceColor() const
{
  return this->m_face_color;
}

void MYON_SubDExpandEdgesParameters::SetFaceColor(MYON_Color face_color)
{
  this->m_face_color = face_color;
}

const MYON_ComponentStatus MYON_SubDExpandEdgesParameters::FaceStatus() const
{
  return this->m_face_status;
}

void MYON_SubDExpandEdgesParameters::SetFaceStatus(MYON_ComponentStatus face_status)
{
  this->m_face_status = MYON_ComponentStatus::NoneSet;
  this->m_face_status.SetRuntimeMark(face_status.SetRuntimeMark());
  this->m_face_status.SetMarkBits(face_status.MarkBits());
  if (face_status.IsSelectedPersistent())
    this->m_face_status.SetSelectedState(MYON_ComponentState::SelectedPersistent, face_status.IsHighlighted());
  else if (face_status.IsSelected())
    this->m_face_status.SetSelectedState(MYON_ComponentState::Selected, face_status.IsHighlighted());
  else if (face_status.IsHighlighted())
    this->m_face_status.SetHighlightedState(true);
}

const MYON_SHA1_Hash MYON_SubDExpandEdgesParameters::Hash() const
{
  MYON_SHA1 sha1;
  sha1.AccumulateDouble(ConstantOffset());
  sha1.AccumulateDouble(VariableOffset().m_t[0]);
  sha1.AccumulateDouble(VariableOffset().m_t[1]);
  sha1.AccumulateInteger32((unsigned int)FaceColor());
  sha1.AccumulateBytes(&m_face_status, sizeof(m_face_status));
  sha1.AccumulateInteger32((unsigned int)FaceStyle());
  return sha1.Hash();
}

bool operator==(const MYON_SubDExpandEdgesParameters& lhs, const MYON_SubDExpandEdgesParameters& rhs)
{
  return lhs.Hash() == rhs.Hash();
}

bool operator!=(const MYON_SubDExpandEdgesParameters& lhs, const MYON_SubDExpandEdgesParameters& rhs)
{
  return lhs.Hash() != rhs.Hash();
}


double MYON_SubDExpandEdgesParameters::CleanupOffset(double x)
{

  const double r[] = {
    0.0,
    1.0,
    MYON_SubDExpandEdgesParameters::SmallOffset,
    MYON_SubDExpandEdgesParameters::MediumOffset,
    MYON_SubDExpandEdgesParameters::LargeOffset,
    MYON_SubDExpandEdgesParameters::MinimumOffset,
    MYON_SubDExpandEdgesParameters::MaximumOffset
  };

  const size_t c = sizeof(r) / sizeof(r[0]);
  for (size_t i = 0; i < c; ++i)
  {
    if (fabs(x - r[i]) <= MYON_SubDExpandEdgesParameters::OffsetTolerance)
      return r[i];
  }

  if (x > 0.0 && x < MYON_SubDExpandEdgesParameters::MinimumOffset)
    x = MYON_SubDExpandEdgesParameters::MinimumOffset;
  else if (x < 1.0 && x > MYON_SubDExpandEdgesParameters::MaximumOffset)
    x = MYON_SubDExpandEdgesParameters::MaximumOffset;

  if (x >= 0.0 && x <= 1.0)
    return x;

  return MYON_DBL_QNAN;
}




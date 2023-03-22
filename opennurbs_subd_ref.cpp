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

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDRef
//

MYON_SubDRef::MYON_SubDRef() MYON_NOEXCEPT
{}

MYON_SubDRef::~MYON_SubDRef()
{
  m_subd_sp.reset();
}

MYON_SubDRef::MYON_SubDRef(const MYON_SubDRef& src) MYON_NOEXCEPT
  : m_subd_sp(src.m_subd_sp)
{}

MYON_SubDRef& MYON_SubDRef::operator=(const MYON_SubDRef& src)
{
  if ( this != &src )
    m_subd_sp = src.m_subd_sp;
  return *this;
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDRef::MYON_SubDRef( MYON_SubDRef&& src) MYON_NOEXCEPT
  : m_subd_sp(std::move(src.m_subd_sp))
{}

// rvalue assignment operator
MYON_SubDRef& MYON_SubDRef::operator=(MYON_SubDRef&& src)
{
  m_subd_sp.reset();
  m_subd_sp = std::move(src.m_subd_sp);
  return *this;
}
#endif

const class MYON_SubD& MYON_SubDRef::SubD() const
{
  const MYON_SubD* subd = m_subd_sp.get();
  if ( nullptr == subd )
    subd = &MYON_SubD::Empty;
  return *subd;
}

unsigned int MYON_SubDRef::ReferenceCount() const
{
  return (unsigned int)m_subd_sp.use_count();
}

void MYON_SubDRef::Clear()
{
  m_subd_sp.reset();
}

class MYON_SubD& MYON_SubDRef::NewSubD()
{
  MYON_SubD* subd = new MYON_SubD();
  MYON_SubD* managed_subd = SetSubDForExperts(subd);
  return *managed_subd;
}

class MYON_SubD& MYON_SubDRef::CopySubD(
  const MYON_SubDRef& src
  )
{
  return CopySubD(src.SubD());
}

class MYON_SubD& MYON_SubDRef::CopySubD(
  const MYON_SubD& src
  )
{
  MYON_SubD* subd_copy = new MYON_SubD(src);
  MYON_SubD* managed_subd = SetSubDForExperts(subd_copy);
  return *managed_subd;
}

class MYON_SubD& MYON_SubDRef::UniqueSubD()
{
  const MYON_SubD& subd = SubD();
  if (m_subd_sp.use_count() > 1 )
    return CopySubD(subd);
  if (subd.m_subdimple_sp.use_count() > 1)
    return CopySubD(subd);
  return const_cast< MYON_SubD& >(subd);
}

class MYON_SubD* MYON_SubDRef::SetSubDForExperts(
  class MYON_SubD*& subd
  )
{
  Clear();
  MYON_SubD* managed_subd = ( subd == &MYON_SubD::Empty ) ? nullptr : subd;
  subd = nullptr;
  if (nullptr != managed_subd )
    m_subd_sp = std::shared_ptr<class MYON_SubD>(managed_subd);
  return managed_subd;
}

MYON_SubDRef::MYON_SubDRef(
  const MYON_SubD& subd
  )
{
  const MYON_SubDimple* subdimple = subd.SubDimple();
  if (nullptr != subdimple)
  {
    MYON_SubD* managed_subd = new MYON_SubD();
    managed_subd->ShareDimple(subd);
    this->SetSubDForExperts(managed_subd);
    if (nullptr != managed_subd)
      delete managed_subd;
  }
}

MYON_SubDRef MYON_SubDRef::CreateReferenceForExperts(
  const MYON_SubD& subd
  )
{
  MYON_SubDRef subd_ref(subd);
  return subd_ref;
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentRef
//

MYON_OBJECT_IMPLEMENT(MYON_SubDComponentRef,MYON_Geometry,"C221FC6D-36B7-47E8-90AA-AC8EC784E3DD");

MYON_SubDComponentRef::MYON_SubDComponentRef(const MYON_SubDComponentRef& src) MYON_NOEXCEPT
  : MYON_Geometry(src)
  , m_subd_ref(src.m_subd_ref)
  , m_component_ptr(src.m_component_ptr)
  , m_component_index(src.m_component_index)
  , m_component_location(src.m_component_location)
  , m_reference_id(src.m_reference_id)
{}

MYON_SubDComponentRef& MYON_SubDComponentRef::operator=(const MYON_SubDComponentRef& src)
{
  if (this != &src)
  {
    MYON_Geometry::operator=(src);
    m_subd_ref = src.m_subd_ref;
    m_component_ptr = src.m_component_ptr;
    m_component_index = src.m_component_index;
    m_component_location = src.m_component_location;
    m_reference_id = src.m_reference_id;
  }
  return *this;
}

#if defined(MYON_HAS_RVALUEREF)
MYON_SubDComponentRef::MYON_SubDComponentRef( MYON_SubDComponentRef&& src ) MYON_NOEXCEPT
  : MYON_Geometry(std::move(src))
  , m_subd_ref(std::move(src.m_subd_ref))
  , m_component_ptr(src.m_component_ptr)
  ,  m_component_index(src.m_component_index)
  , m_component_location(src.m_component_location)
  , m_reference_id(src.m_reference_id)
{}

MYON_SubDComponentRef& MYON_SubDComponentRef::operator=(MYON_SubDComponentRef&& src)
{
  if ( this != &src )
  {
    Clear();
    MYON_Geometry::operator=(std::move(src));
    m_subd_ref = std::move(src.m_subd_ref);
    m_component_ptr = src.m_component_ptr;
    m_component_index = src.m_component_index;
    m_component_location = src.m_component_location;
    m_reference_id = src.m_reference_id;
  }
  return *this;
}

#endif


int MYON_SubDComponentRef::Compare(const MYON_SubDComponentRef* lhs, const MYON_SubDComponentRef* rhs)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  const MYON__UINT64 lhs_sn = lhs->m_subd_ref.SubD().RuntimeSerialNumber();
  const MYON__UINT64 rhs_sn = rhs->m_subd_ref.SubD().RuntimeSerialNumber();
  if (lhs_sn < rhs_sn)
    return -1;
  if (lhs_sn > rhs_sn)
    return 1;
  return MYON_COMPONENT_INDEX::Compare(&lhs->m_component_index, &rhs->m_component_index);
}

int MYON_SubDComponentRef::Compare2(const MYON_SubDComponentRef* const* lhs, const MYON_SubDComponentRef* const* rhs)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  return MYON_SubDComponentRef::Compare(*lhs, *rhs);
}

const MYON_SubDComponentRef MYON_SubDComponentRef::Create(
  const MYON_SubDRef& subd_ref,
  MYON_COMPONENT_INDEX component_index,
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
  )
{
  MYON_SubDComponentPtr component_ptr = subd_ref.SubD().ComponentPtrFromComponentIndex(component_index);
  return MYON_SubDComponentRef::Create(subd_ref,component_ptr,component_location,reference_id);
}

const MYON_SubDComponentRef MYON_SubDComponentRef::Create(
  const MYON_SubDRef& subd_ref,
  MYON_SubDComponentPtr component_ptr,
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
  )
{
  MYON_SubDComponentRef component_ref;
  component_ref.m_subd_ref = subd_ref;
  component_ref.m_reference_id = reference_id;
  bool bValidInput = false;
  switch (component_ptr.ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = MYON_SUBD_VERTEX_POINTER(component_ptr.m_ptr);
      if (nullptr != vertex && vertex->m_id > 0 && vertex->m_id < MYON_UNSET_UINT_INDEX)
      {
        component_ref.m_component_ptr = component_ptr;
        component_ref.m_component_index.Set(MYON_COMPONENT_INDEX::TYPE::subd_vertex,(int)vertex->m_id);
        component_ref.m_component_location = component_location;
        bValidInput = true;
      }
    }
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(component_ptr.m_ptr);
      if (nullptr != edge && edge->m_id > 0 && edge->m_id < MYON_UNSET_UINT_INDEX)
      {
        component_ref.m_component_ptr = component_ptr;
        component_ref.m_component_index.Set(MYON_COMPONENT_INDEX::TYPE::subd_edge,(int)edge->m_id);
        component_ref.m_component_location = component_location;
        bValidInput = true;
      }
    }
    break;
  case MYON_SubDComponentPtr::Type::Face:
    {
      const MYON_SubDFace* face = MYON_SUBD_FACE_POINTER(component_ptr.m_ptr);
      if (nullptr != face && face->m_id > 0 && face->m_id < MYON_UNSET_UINT_INDEX)
      {
        component_ref.m_component_ptr = component_ptr;
        component_ref.m_component_index.Set(MYON_COMPONENT_INDEX::TYPE::subd_face,(int)face->m_id);
        component_ref.m_component_location = component_location;
        bValidInput = true;
      }
    }
    break;
  default:
    if ( component_ptr.IsNull() )
      bValidInput = true;
  }

  if (bValidInput)
  {
    return component_ref;
  }

  return MYON_SUBD_RETURN_ERROR(component_ref);
}

const MYON_SubDComponentRef MYON_SubDComponentRef::Create(
  const MYON_SubDRef& subd_ref,
  const class MYON_SubDVertex* vertex,
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
  )
{
  return MYON_SubDComponentRef::Create(subd_ref, MYON_SubDComponentPtr::Create(vertex),component_location,reference_id);
}

const MYON_SubDComponentRef MYON_SubDComponentRef::Create(
  const MYON_SubDRef& subd_ref,
  const class MYON_SubDEdge* edge,
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
  )
{
  return MYON_SubDComponentRef::Create(subd_ref, MYON_SubDComponentPtr::Create(edge),component_location,reference_id);
}

const MYON_SubDComponentRef MYON_SubDComponentRef::Create(
  const MYON_SubDRef& subd_ref,
  const class MYON_SubDFace* face,
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
  )
{
  return MYON_SubDComponentRef::Create(subd_ref, MYON_SubDComponentPtr::Create(face),component_location,reference_id);
}

void MYON_SubDComponentRef::Clear()
{
  MYON_Geometry::DestroyRuntimeCache();
  PurgeUserData();
  m_subd_ref.Clear();
  m_component_ptr = MYON_SubDComponentPtr::Null;
  m_component_index = MYON_COMPONENT_INDEX::UnsetComponentIndex;
  m_reference_id = 0;
}

MYON_SubDRef MYON_SubDComponentRef::SubDRef() const
{
  return m_subd_ref;
}

const class MYON_SubD& MYON_SubDComponentRef::SubD() const
{
  return m_subd_ref.SubD();
}

MYON_COMPONENT_INDEX MYON_SubDComponentRef::ComponentIndex() const
{
  return m_component_index;
}

MYON_SubDComponentPtr MYON_SubDComponentRef::ComponentPtr() const
{
  return m_component_ptr;
}

MYON_SubDComponentLocation MYON_SubDComponentRef::ComponentLocation() const
{
  return m_component_location;
}

const class MYON_SubDVertex* MYON_SubDComponentRef::Vertex() const
{
  return m_component_ptr.Vertex();
}

const class MYON_SubDEdge* MYON_SubDComponentRef::Edge() const
{
  return m_component_ptr.Edge();
}

const class MYON_SubDFace* MYON_SubDComponentRef::Face() const
{
  return m_component_ptr.Face();
}

MYON__UINT_PTR MYON_SubDComponentRef::ReferenceId() const
{
  return m_reference_id;
}

void MYON_SubDComponentRef::SetReferenceId(
  MYON__UINT_PTR reference_id
)
{
  m_reference_id = reference_id;
}

bool MYON_SubDComponentRef::IsValid(MYON_TextLog* text_log) const
{
  return (
    m_component_ptr.IsNotNull() 
    && (MYON_SubDComponentLocation::ControlNet == m_component_location || MYON_SubDComponentLocation::Surface == m_component_location)
    && false == SubD().IsEmpty()
    );
}

void MYON_SubDComponentRef::Dump(MYON_TextLog& text_log) const
{
  return;
}

unsigned int MYON_SubDComponentRef::SizeOf() const
{
  size_t sz = MYON_Geometry::SizeOf() + sizeof(*this) - sizeof(MYON_Geometry);
  return (unsigned int)sz;
}

MYON::object_type MYON_SubDComponentRef::ObjectType() const
{
  return MYON::object_type::subd_object;
}

// overrides of virtual MYON_Geometry functions
int MYON_SubDComponentRef::Dimension() const
{
  return 3;
}

bool MYON_SubDComponentRef::GetBBox(
  double* boxmin,
  double* boxmax,
  bool bGrowBox
  ) const
{
  if ( nullptr == boxmin || nullptr == boxmax )
    return false;

  MYON_BoundingBox bbox = MYON_BoundingBox::EmptyBoundingBox;

  switch (m_component_ptr.ComponentType())
  {
      case MYON_SubDComponentPtr::Type::Unset:
        break;
      
      case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = m_component_ptr.Vertex();
      if ( nullptr == vertex )
        break;
      switch (m_component_location)
      {
      case MYON_SubDComponentLocation::Surface:
        // public opennubs does not provide limit mesh tools.
      case MYON_SubDComponentLocation::ControlNet:
        bbox = vertex->ControlNetBoundingBox();
        break;
              
      case MYON_SubDComponentLocation::Unset:
        break;
      }
    }
    break;
  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* edge = m_component_ptr.Edge();
      if ( nullptr == edge )
        break;
      switch (m_component_location)
      {
      case MYON_SubDComponentLocation::Surface:
        // public opennubs does not provide limit mesh tools.
      case MYON_SubDComponentLocation::ControlNet:
        bbox = edge->ControlNetBoundingBox();
        break;
      case MYON_SubDComponentLocation::Unset:
        break;
      }
    }
    break;
  case MYON_SubDComponentPtr::Type::Face:
    {
      const MYON_SubDFace* face = m_component_ptr.Face();
      if ( nullptr == face )
        break;
      switch (m_component_location)
      {
      case MYON_SubDComponentLocation::Surface:
        // public opennubs does not provide limit mesh tools.
      case MYON_SubDComponentLocation::ControlNet:
        bbox = face->ControlNetBoundingBox();
        break;
      case MYON_SubDComponentLocation::Unset:
        break;
      }
    }
    break;
  }

  if (bGrowBox)
  {
    MYON_BoundingBox bbox1;
    bbox1.m_min = boxmin;
    bbox1.m_max = boxmax;
    if (bbox1.IsValid())
      bbox.Union(bbox1);
  }

  boxmin[0] = bbox.m_min.x;
  boxmin[1] = bbox.m_min.y;
  boxmin[2] = bbox.m_min.z;

  boxmax[0] = bbox.m_max.x;
  boxmax[1] = bbox.m_max.y;
  boxmax[2] = bbox.m_max.z;

  return bbox.IsValid();
}


void MYON_SubDComponentRefList::Internal_Destroy()
{
  for (unsigned int i = 0; i < m_list.UnsignedCount(); i++)
  {
    MYON_SubDComponentRef* p = m_list[i];
    m_list[i] = nullptr;
    if (nullptr != p)
      delete p;
  }
  m_list.SetCount(0);
  m_bIsClean = false;
}

void MYON_SubDComponentRefList::Internal_CopyFrom(const MYON_SubDComponentRefList& src)
{
  const unsigned int count = src.m_list.UnsignedCount();
  m_list.Reserve(count);
  m_list.SetCount(0);
  for (unsigned int i = 0; i < count; i++)
  {
    const MYON_SubDComponentRef* p = src.m_list[i];
    if (nullptr == p)
      m_list.Append(nullptr);
    else
      m_list.Append(new MYON_SubDComponentRef(*p));

    m_subd_count = src.m_subd_count;
    m_subd_vertex_smooth_count = src.m_subd_vertex_smooth_count;
    m_subd_vertex_dart_count = src.m_subd_vertex_dart_count;
    m_subd_vertex_crease_count = src.m_subd_vertex_crease_count;
    m_subd_vertex_corner_count = src.m_subd_vertex_corner_count;
    m_subd_edge_smooth_count = src.m_subd_edge_smooth_count;
    m_subd_edge_crease_count = src.m_subd_edge_crease_count;
    m_subd_face_count = src.m_subd_face_count;

    m_bIsClean = src.m_bIsClean;
  }
}

MYON_SubDComponentRefList::~MYON_SubDComponentRefList()
{
  Internal_Destroy();
}

MYON_SubDComponentRefList::MYON_SubDComponentRefList(const MYON_SubDComponentRefList& src)
{
  Internal_CopyFrom(src);
}

MYON_SubDComponentRefList& MYON_SubDComponentRefList::operator=(const MYON_SubDComponentRefList& src)
{
  if (this != &src)
  {
    Internal_Destroy();
    Internal_CopyFrom(src);
  }
  return *this;
}

bool MYON_SubDComponentRefList::Internal_UpdateCount(const MYON_SubDComponentRef& r, int i)
{
  if (r.SubD().IsEmpty())
    return false;

  bool rc = false;
  MYON_SubDComponentPtr cptr = r.ComponentPtr();
  switch (cptr.ComponentType())
  {
      case MYON_SubDComponentPtr::Type::Unset:
        break;
          
      case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* v = cptr.Vertex();
      if (nullptr == v)
        break;
      switch (v->m_vertex_tag)
      {
      case MYON_SubDVertexTag::Smooth:
        m_subd_vertex_smooth_count += i;
        rc = true;
        break;
      case MYON_SubDVertexTag::Crease:
        m_subd_vertex_crease_count += i;
        rc = true;
        break;
      case MYON_SubDVertexTag::Corner:
        m_subd_vertex_corner_count += i;
        rc = true;
        break;
      case MYON_SubDVertexTag::Dart:
        m_subd_vertex_dart_count += i;
        rc = true;
        break;
      case MYON_SubDVertexTag::Unset:
        break;
      }
    }
    break;

  case MYON_SubDComponentPtr::Type::Edge:
    {
      const MYON_SubDEdge* e = cptr.Edge();
      if (nullptr == e)
        break;
      switch (e->m_edge_tag)
      {
      case MYON_SubDEdgeTag::Smooth:
      case MYON_SubDEdgeTag::SmoothX:
        m_subd_edge_smooth_count += i;
        rc = true;
        break;
      case MYON_SubDEdgeTag::Crease:
        m_subd_edge_crease_count += i;
        rc = true;
        break;
      case MYON_SubDEdgeTag::Unset:
        break;
      }
    }
    break;

  case MYON_SubDComponentPtr::Type::Face:
    if (nullptr != cptr.Face())
    {
      m_subd_face_count += i;
      rc = true;
    }
    break;
  }

  return rc;
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::Append(
  const MYON_SubDRef & subd_ref,
  MYON_COMPONENT_INDEX ci, 
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
  )
{
  for (;;)
  {
    if (subd_ref.SubD().IsEmpty())
      break;
    if (false == ci.IsSubDComponentIndex())
      break;
    const MYON_SubDComponentRef r(MYON_SubDComponentRef::Create(subd_ref, ci, component_location, reference_id));
    return Append(&r);
  }
  return MYON_SubDComponentRef::Empty;
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::Append(
  const MYON_SubDRef & subd_ref, 
  MYON_SubDComponentPtr component_ptr, 
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
)
{
  for (;;)
  {
    if (subd_ref.SubD().IsEmpty())
      break;
    if (false == component_ptr.IsNull())
      break;
    const MYON_SubDComponentRef r(MYON_SubDComponentRef::Create(subd_ref, component_ptr, component_location,reference_id));
    return Append(&r);
  }
  return MYON_SubDComponentRef::Empty;
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::Append(const MYON_SubDComponentRef& src_ref)
{
  return Append(&src_ref);
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::Append(const MYON_SubDComponentRef* src_ref)
{
  for (;;)
  {
    if (nullptr == src_ref)
      break;
    if (src_ref->SubD().IsEmpty())
      break;
    if (false == Internal_UpdateCount(*src_ref,1))
      break;
    m_list.Append(new MYON_SubDComponentRef(*src_ref));
    m_bIsClean = false;
    return **(m_list.Last());
  }
  return MYON_SubDComponentRef::Empty;
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::AppendForExperts(MYON_SubDComponentRef*& ref)
{
  for (;;)
  {
    if (nullptr == ref)
      break;
    if (ref->SubD().IsEmpty())
      break;
    if (false == Internal_UpdateCount(*ref,1))
      break;
    m_list.Append(ref);
    m_bIsClean = false;
    return *ref;
  }
  return MYON_SubDComponentRef::Empty;
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::AppendForExperts(
  const MYON_SubD& subd,
  MYON_COMPONENT_INDEX ci,
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
)
{
  for (;;)
  {
    if (subd.IsEmpty())
      break;
    return Append(MYON_SubDRef::CreateReferenceForExperts(subd),ci,component_location,reference_id);
  }
  return MYON_SubDComponentRef::Empty;
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::AppendForExperts(
  const MYON_SubD& subd,
  MYON_SubDComponentPtr component_ptr,
  MYON_SubDComponentLocation component_location,
  MYON__UINT_PTR reference_id
)
{
  for (;;)
  {
    if (subd.IsEmpty())
      break;
    return Append(MYON_SubDRef::CreateReferenceForExperts(subd),component_ptr,component_location,reference_id);
  }
  return MYON_SubDComponentRef::Empty;
}


int MYON_SubDComponentRefList::Clean()
{
  if (m_bIsClean)
    return m_list.UnsignedCount();

  const unsigned dirty_count = m_list.UnsignedCount();

  ((MYON_SimpleArray< const MYON_SubDComponentRef* > *)(&m_list))->QuickSort(MYON_SubDComponentRef::Compare2);

  m_subd_count = 0;
  m_subd_vertex_smooth_count = 0;
  m_subd_vertex_dart_count = 0;
  m_subd_vertex_crease_count = 0;
  m_subd_vertex_corner_count = 0;
  m_subd_edge_smooth_count = 0;
  m_subd_edge_crease_count = 0;
  m_subd_face_count = 0;

  unsigned int clean_count = 0;
  const MYON_SubDComponentRef* prev_scr = nullptr;
  for (unsigned int i = 0; i < dirty_count; i++)
  {
    MYON_SubDComponentRef* scr = m_list[i];
    if (nullptr == scr)
      continue;
    if (
      0 == MYON_SubDComponentRef::Compare(prev_scr, scr)
      || false == Internal_UpdateCount(*scr,1)
      )
    {
      delete scr;
      continue;
    }
    if (nullptr == prev_scr || prev_scr->SubD().RuntimeSerialNumber() != scr->SubD().RuntimeSerialNumber())
      m_subd_count++;
    m_list[clean_count++] = scr;
    prev_scr = scr;
  }
  for (unsigned i = clean_count; i < dirty_count; i++)
    m_list[i] = nullptr;
  m_list.SetCount(clean_count);
  m_bIsClean = true;
  return clean_count;
}

int MYON_SubDComponentRefList::Count() const
{
  return m_list.Count();
}

void MYON_SubDComponentRefList::Remove(int i)
{
  MYON_SubDComponentRef* p = TransferForExperts(i);
  if (nullptr != p)
    delete p;
}

MYON_SubDComponentRef * MYON_SubDComponentRefList::TransferForExperts(int i)
{
  MYON_SubDComponentRef * p = (i >= 0 && i < m_list.Count()) ? m_list[i] : nullptr;
  if (p != nullptr)
  {
    Internal_UpdateCount(*p, -1);
    m_bIsClean = false;
  }
  return p;
}

const MYON_SubDComponentRef& MYON_SubDComponentRefList::operator[](int i) const
{
  const MYON_SubDComponentRef * p = (i >= 0 && i < m_list.Count()) ? m_list[i] : nullptr;
  return (nullptr == p) ? MYON_SubDComponentRef::Empty : *p;
}

int MYON_SubDComponentRefList::SubDCount() const
{
  return m_bIsClean ? m_subd_count : 0;
}

int MYON_SubDComponentRefList::VertexCount() const
{
  return
    m_subd_vertex_smooth_count
    + m_subd_vertex_dart_count
    + m_subd_vertex_crease_count
    + m_subd_vertex_corner_count;
}

int MYON_SubDComponentRefList::VertexCount(MYON_SubDVertexTag vertex_tag) const
{
  int c = 0;
  switch (vertex_tag)
  {
  case MYON_SubDVertexTag::Smooth:
    c = m_subd_vertex_smooth_count;
    break;
  case MYON_SubDVertexTag::Crease:
    c = m_subd_vertex_crease_count;
    break;
  case MYON_SubDVertexTag::Corner:
    c = m_subd_vertex_corner_count;
    break;
  case MYON_SubDVertexTag::Dart:
    c = m_subd_vertex_dart_count;
    break;
  case MYON_SubDVertexTag::Unset:
    break;
  }

  return c;
}

int MYON_SubDComponentRefList::EdgeCount() const
{
  return m_subd_edge_crease_count + m_subd_edge_smooth_count;
}

int MYON_SubDComponentRefList::EdgeCount(MYON_SubDEdgeTag edge_tag) const
{
  int c = 0;
  switch (edge_tag)
  {
  case MYON_SubDEdgeTag::Unset:
    break;
  case MYON_SubDEdgeTag::Smooth:
    c = m_subd_edge_smooth_count;
    break;
  case MYON_SubDEdgeTag::Crease:
    c = m_subd_edge_crease_count;
    break;
  case MYON_SubDEdgeTag::SmoothX:
    break;
  }
  return c;
}

int MYON_SubDComponentRefList::FaceCount() const
{
  return m_subd_face_count;
}

int MYON_SubDComponentRefList::ComponentCount() const
{
  return m_list.Count();
}

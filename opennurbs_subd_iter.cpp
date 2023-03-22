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
// MYON_SubDVertexArray
//
MYON_SubDVertexArray::MYON_SubDVertexArray(
  const class MYON_SubD& subd
  )
{
  m_subd.ShareDimple(subd);
  unsigned int vertex_count = subd.VertexCount();
  if (vertex_count > 0)
  {
    m_sp = subd.ActiveLevel().VertexArray();
    m_a = m_sp.get();
    if (nullptr != m_a)
      m_vertex_count = vertex_count;
  }
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDVertexArray::MYON_SubDVertexArray(MYON_SubDVertexArray&& src) MYON_NOEXCEPT
  : m_a(src.m_a)
  , m_vertex_count(src.m_vertex_count)
  , m_sp(std::move(src.m_sp))
{
  m_subd.SwapDimple(src.m_subd);
  src.m_subd.Destroy();
}

// The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
// which could throw exceptions.  See the implementation of
// MYON_Object::operator=(MYON_Object&&) for details.
MYON_SubDVertexArray& MYON_SubDVertexArray::operator=(MYON_SubDVertexArray&& src)
{
  if (this != &src)
  {
    m_a = src.m_a;
    m_subd.SwapDimple(src.m_subd);
    m_vertex_count = src.m_vertex_count;
    m_sp = std::move(src.m_sp);
    src.m_subd.Destroy();
  }
  return *this;
}
#endif


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeArray
//
MYON_SubDEdgeArray::MYON_SubDEdgeArray(
  const class MYON_SubD& subd
  )
{
  m_subd.ShareDimple(subd);
  unsigned int edge_count = subd.EdgeCount();
  if (edge_count > 0)
  {
    m_sp = subd.ActiveLevel().EdgeArray();
    m_a = m_sp.get();
    if (nullptr != m_a)
      m_edge_count = edge_count;
  }
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDEdgeArray::MYON_SubDEdgeArray(MYON_SubDEdgeArray&& src) MYON_NOEXCEPT
  : m_a(src.m_a)
  , m_edge_count(src.m_edge_count)
  , m_sp(std::move(src.m_sp))
{
  m_subd.SwapDimple(src.m_subd);
  src.m_subd.Destroy();
}

// The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
// which could throw exceptions.  See the implementation of
// MYON_Object::operator=(MYON_Object&&) for details.
MYON_SubDEdgeArray& MYON_SubDEdgeArray::operator=(MYON_SubDEdgeArray&& src)
{
  if (this != &src)
  {
    m_a = src.m_a;
    m_subd.SwapDimple(src.m_subd);
    m_edge_count = src.m_edge_count;
    m_sp = std::move(src.m_sp);
    src.m_subd.Destroy();
  }
  return *this;
}
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceArray
//
MYON_SubDFaceArray::MYON_SubDFaceArray(
  const class MYON_SubD& subd
  )
{
  m_subd.ShareDimple(subd);
  unsigned int face_count = subd.FaceCount();
  if (face_count > 0)
  {
    m_sp = subd.ActiveLevel().FaceArray();
    m_a = m_sp.get();
    if (nullptr != m_a)
      m_face_count = face_count;
  }
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDFaceArray::MYON_SubDFaceArray(MYON_SubDFaceArray&& src) MYON_NOEXCEPT
  : m_a(src.m_a)
  , m_face_count(src.m_face_count)
  , m_sp(std::move(src.m_sp))
{
  m_subd.SwapDimple(src.m_subd);
  src.m_subd.Destroy();
}

// The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
// which could throw exceptions.  See the implementation of
// MYON_Object::operator=(MYON_Object&&) for details.
MYON_SubDFaceArray& MYON_SubDFaceArray::operator=(MYON_SubDFaceArray&& src)
{
  if (this != &src)
  {
    m_a = src.m_a;
    m_subd.SwapDimple(src.m_subd);
    m_face_count = src.m_face_count;
    m_sp = std::move(src.m_sp);
    src.m_subd.Destroy();
  }
  return *this;
}
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentIdIterator
//

void MYON_SubDHeap::InitializeComponentIdIterator(
  MYON_SubDComponentPtr::Type ctype,
  class MYON_SubDComponentIdIterator& cidit
) const
{
  switch (ctype)
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    cidit.m_component_type = ctype;
    cidit.MYON_FixedSizePoolIterator::Create(&m_fspv);
    break;

  case MYON_SubDComponentPtr::Type::Edge:
    cidit.m_component_type = ctype;
    cidit.MYON_FixedSizePoolIterator::Create(&m_fspe);
    break;

  case MYON_SubDComponentPtr::Type::Face:
    cidit.m_component_type = ctype;
    cidit.MYON_FixedSizePoolIterator::Create(&m_fspf);
    break;

  default:
    MYON_SUBD_ERROR("Invalid ctype");
    break;
  }
}

void MYON_SubDimple::InitializeComponentIdIterator(
  MYON_SubDComponentPtr::Type ctype, 
  MYON_SubDComponentIdIterator & cidit
) const
{
  m_heap.InitializeComponentIdIterator(ctype, cidit);
}

const MYON_SubDComponentBase* MYON_SubDComponentIdIterator::FirstComponent()
{
  for (const MYON_SubDComponentBase* c = (const MYON_SubDComponentBase*)FirstElement(); nullptr != c; c = (const MYON_SubDComponentBase*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&c->m_id)[1])
      return c;
  }
  return nullptr;
}

const MYON_SubDComponentBase* MYON_SubDComponentIdIterator::NextComponent()
{
  for (const MYON_SubDComponentBase* c = (const MYON_SubDComponentBase*)NextElement(); nullptr != c; c = (const MYON_SubDComponentBase*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&c->m_id)[1])
      return c;
  }
  return nullptr;
}

const MYON_SubDComponentBase* MYON_SubDComponentIdIterator::CurrentComponent() const
{
  return (const MYON_SubDComponentBase*)CurrentElement();
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexIterator
//

void MYON_SubDHeap::InitializeVertexIdIterator(
  class MYON_SubDVertexIdIterator& vidit
) const
{
  vidit.MYON_FixedSizePoolIterator::Create(&m_fspv);
}

void MYON_SubDimple::InitializeVertexIdIterator(
  class MYON_SubDVertexIdIterator& vidit
) const
{
  m_heap.InitializeVertexIdIterator(vidit);
}

void MYON_SubDVertexIdIterator::Internal_Init()
{
  const MYON_SubDimple* subdimple = m_subd_ref.SubD().SubDimple();
  if (nullptr != subdimple)
    subdimple->InitializeVertexIdIterator(*this);
}

MYON_SubDVertexIdIterator::MYON_SubDVertexIdIterator(const MYON_SubDRef& subd_ref)
  : m_subd_ref(subd_ref)
{
  Internal_Init();
}

MYON_SubDVertexIdIterator::MYON_SubDVertexIdIterator(const MYON_SubD& subd)
  : m_subd_ref(MYON_SubDRef::CreateReferenceForExperts(subd))
{
  Internal_Init();
}

const MYON_SubDVertex* MYON_SubDVertexIdIterator::FirstVertex()
{
  for (const MYON_SubDVertex* v = (const MYON_SubDVertex*)FirstElement(); nullptr != v; v = (const MYON_SubDVertex*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&v->m_id)[1])
      return v;
  }
  return nullptr;
}

const MYON_SubDVertex* MYON_SubDVertexIdIterator::NextVertex()
{
  for (const MYON_SubDVertex* v = (const MYON_SubDVertex*)NextElement(); nullptr != v; v = (const MYON_SubDVertex*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&v->m_id)[1])
      return v;
  }
  return nullptr;
}


const MYON_SubDVertex* MYON_SubDVertexIdIterator::FirstVertexOnLevel(unsigned int level_index)
{
  for (const MYON_SubDVertex* v = (const MYON_SubDVertex*)FirstElement(); nullptr != v; v = (const MYON_SubDVertex*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&v->m_id)[1] && level_index == v->SubdivisionLevel())
      return v;
  }
  return nullptr;
}

const MYON_SubDVertex* MYON_SubDVertexIdIterator::NextVertexOnLevel(unsigned int level_index)
{
  for (const MYON_SubDVertex* v = (const MYON_SubDVertex*)NextElement(); nullptr != v; v = (const MYON_SubDVertex*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&v->m_id)[1] && level_index == v->SubdivisionLevel())
      return v;
  }
  return nullptr;
}

const MYON_SubDVertex* MYON_SubDVertexIdIterator::CurrentVertex() const
{
  return (const MYON_SubDVertex*)CurrentElement();
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeIterator
//

void MYON_SubDHeap::InitializeEdgeIdIterator(
  class MYON_SubDEdgeIdIterator& eidit
) const
{
  eidit.MYON_FixedSizePoolIterator::Create(&m_fspe);
}

void MYON_SubDimple::InitializeEdgeIdIterator(
  class MYON_SubDEdgeIdIterator& eidit
) const
{
  m_heap.InitializeEdgeIdIterator(eidit);
}

void MYON_SubDEdgeIdIterator::Internal_Init()
{
  const MYON_SubDimple* subdimple = m_subd_ref.SubD().SubDimple();
  if (nullptr != subdimple)
    subdimple->InitializeEdgeIdIterator(*this);
}

MYON_SubDEdgeIdIterator::MYON_SubDEdgeIdIterator(const MYON_SubDRef& subd_ref)
  : m_subd_ref(subd_ref)
{
  Internal_Init();
}

MYON_SubDEdgeIdIterator::MYON_SubDEdgeIdIterator(const MYON_SubD& subd)
  : m_subd_ref(MYON_SubDRef::CreateReferenceForExperts(subd))
{
  Internal_Init();
}

const MYON_SubDEdge* MYON_SubDEdgeIdIterator::FirstEdge()
{
  for (const MYON_SubDEdge* e = (const MYON_SubDEdge*)FirstElement(); nullptr != e; e = (const MYON_SubDEdge*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&e->m_id)[1])
      return e;
  }
  return nullptr;
}

const MYON_SubDEdge* MYON_SubDEdgeIdIterator::NextEdge()
{
  for (const MYON_SubDEdge* e = (const MYON_SubDEdge*)NextElement(); nullptr != e; e = (const MYON_SubDEdge*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&e->m_id)[1])
      return e;
  }
  return nullptr;
}

const MYON_SubDEdge* MYON_SubDEdgeIdIterator::FirstEdgeOnLevel(unsigned int level_index)
{
  for (const MYON_SubDEdge* e = (const MYON_SubDEdge*)FirstElement(); nullptr != e; e = (const MYON_SubDEdge*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&e->m_id)[1] && level_index == e->SubdivisionLevel())
      return e;
  }
  return nullptr;
}

const MYON_SubDEdge* MYON_SubDEdgeIdIterator::NextEdgeOnLevel(unsigned int level_index)
{
  for (const MYON_SubDEdge* e = (const MYON_SubDEdge*)NextElement(); nullptr != e; e = (const MYON_SubDEdge*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&e->m_id)[1] && level_index == e->SubdivisionLevel())
      return e;
  }
  return nullptr;
}

const MYON_SubDEdge* MYON_SubDEdgeIdIterator::CurrentEdge() const
{
  return (const MYON_SubDEdge*)CurrentElement();
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceIterator
//

void MYON_SubDHeap::InitializeFaceIdIterator(
  class MYON_SubDFaceIdIterator& fidit
) const
{
  fidit.MYON_FixedSizePoolIterator::Create(&m_fspf);
}

void MYON_SubDimple::InitializeFaceIdIterator(
  class MYON_SubDFaceIdIterator& fidit
) const
{
  m_heap.InitializeFaceIdIterator(fidit);
}

void MYON_SubDFaceIdIterator::Internal_Init()
{
  const MYON_SubDimple* subdimple = m_subd_ref.SubD().SubDimple();
  if (nullptr != subdimple)
    subdimple->InitializeFaceIdIterator(*this);
}

MYON_SubDFaceIdIterator::MYON_SubDFaceIdIterator(const MYON_SubDRef& subd_ref)
  : m_subd_ref(subd_ref)
{
  Internal_Init();
}

MYON_SubDFaceIdIterator::MYON_SubDFaceIdIterator(const MYON_SubD& subd)
  : m_subd_ref(MYON_SubDRef::CreateReferenceForExperts(subd))
{
  Internal_Init();
}

const MYON_SubDFace* MYON_SubDFaceIdIterator::FirstFace()
{
  for (const MYON_SubDFace* f = (const MYON_SubDFace*)FirstElement(); nullptr != f; f = (const MYON_SubDFace*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&f->m_id)[1])
      return f;
  }
  return nullptr;
}

const MYON_SubDFace* MYON_SubDFaceIdIterator::NextFace()
{
  for (const MYON_SubDFace* f = (const MYON_SubDFace*)NextElement(); nullptr != f; f = (const MYON_SubDFace*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&f->m_id)[1])
      return f;
  }
  return nullptr;
}

const MYON_SubDFace* MYON_SubDFaceIdIterator::FirstFaceOnLevel(unsigned int level_index)
{
  for (const MYON_SubDFace* f = (const MYON_SubDFace*)FirstElement(); nullptr != f; f = (const MYON_SubDFace*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&f->m_id)[1] && level_index == f->SubdivisionLevel())
      return f;
  }
  return nullptr;
}

const MYON_SubDFace* MYON_SubDFaceIdIterator::NextFaceOnLevel(unsigned int level_index)
{
  for (const MYON_SubDFace* f = (const MYON_SubDFace*)NextElement(); nullptr != f; f = (const MYON_SubDFace*)NextElement())
  {
    if (MYON_UNSET_UINT_INDEX != (&f->m_id)[1] && level_index == f->SubdivisionLevel())
      return f;
  }
  return nullptr;
}


const MYON_SubDFace* MYON_SubDFaceIdIterator::CurrentFace() const
{
  return (const MYON_SubDFace*)CurrentElement();
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexIterator
//

void MYON_SubDVertexIterator::Internal_Init(
  const MYON_SubDRef& subd_ref,
  unsigned int vertex_count,
  const MYON_SubDVertex* first,
  const MYON_SubDVertex* last,
  MYON_SubDComponentPtr component_ptr
)
{
  m_subd_ref = subd_ref;
  if (nullptr != first && nullptr != last && vertex_count > 0)
  {
    m_v_first = first;
    m_v_last = last;
    m_v_current = m_v_first;
    m_vertex_count = vertex_count;
    m_component_ptr
      = (MYON_SubDComponentPtr::Type::Unset == component_ptr.ComponentType())
      ? MYON_SubDComponentPtr::Null
      : component_ptr;
  }
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const class MYON_SubD& subd
  )
{
  const MYON_SubDLevel& level = subd.ActiveLevel();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    level.m_vertex_count,
    level.m_vertex[0],
    level.m_vertex[1],
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const class MYON_SubDRef& subd_ref
  )
{
  const MYON_SubDLevel& level = subd_ref.SubD().ActiveLevel();
  Internal_Init(
    subd_ref,
    level.m_vertex_count,
    level.m_vertex[0],
    level.m_vertex[1],
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const MYON_SubD& subd,
  const MYON_SubDVertex& vertex
  )
{
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    1,
    &vertex,
    &vertex,
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const MYON_SubDRef& subd_ref,
  const MYON_SubDVertex& vertex
  )
{
  Internal_Init(
    subd_ref,
    1,
    &vertex,
    &vertex,
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const class MYON_SubD& subd,
  const class MYON_SubDEdge& edge
  )
{
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    2,
    edge.Vertex(0),
    edge.Vertex(1),
    MYON_SubDComponentPtr::Create(&edge)
  );
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const class MYON_SubDRef& subd_ref,
  const class MYON_SubDEdge& edge
)
{
  Internal_Init(
    subd_ref,
    2,
    edge.Vertex(0),
    edge.Vertex(1),
    MYON_SubDComponentPtr::Create(&edge)
  );
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const class MYON_SubD& subd,
  const class MYON_SubDFace& face
)
{
  const unsigned int vertex_count = face.EdgeCount();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    vertex_count,
    face.Vertex(0),
    face.Vertex(vertex_count-1),
    MYON_SubDComponentPtr::Create(&face)    
  );
}

MYON_SubDVertexIterator::MYON_SubDVertexIterator(
  const class MYON_SubDRef& subd_ref,
  const class MYON_SubDFace& face
)
{
  const unsigned int vertex_count = face.EdgeCount();
  Internal_Init(
    subd_ref,
    vertex_count,
    face.Vertex(0),
    face.Vertex(vertex_count-1),
    MYON_SubDComponentPtr::Create(&face)    
  );
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDVertexIterator::MYON_SubDVertexIterator( MYON_SubDVertexIterator&& src) MYON_NOEXCEPT
  : m_subd_ref(std::move(src.m_subd_ref))
  , m_v_first(src.m_v_first)
  , m_v_last(src.m_v_last)
  , m_v_current(src.m_v_current)
  , m_vertex_index(src.m_vertex_index)
  , m_vertex_count(src.m_vertex_count)
  , m_component_ptr(src.m_component_ptr)
{}

// rvalue assignment operator
MYON_SubDVertexIterator& MYON_SubDVertexIterator::operator=(MYON_SubDVertexIterator&& src)
{
  m_subd_ref.Clear();
  m_subd_ref = std::move(src.m_subd_ref);
  m_v_first = src.m_v_first;
  m_v_last = src.m_v_last;
  m_v_current = src.m_v_current;
  m_vertex_index = src.m_vertex_index;
  m_vertex_count = src.m_vertex_count;
  m_component_ptr = src.m_component_ptr;
  return *this;
}
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeIterator
//
void MYON_SubDEdgeIterator::Internal_Init(
  const MYON_SubDRef& subd_ref,
  unsigned int edge_count,
  const MYON_SubDEdge* first,
  const MYON_SubDEdge* last,
  MYON_SubDComponentPtr component_ptr
)
{
  m_subd_ref = subd_ref;
  if (nullptr != first && nullptr != last && edge_count > 0)
  {
    m_e_first = first;
    m_e_last = last;
    m_e_current = m_e_first;
    m_edge_count = edge_count;
    m_component_ptr
      = (MYON_SubDComponentPtr::Type::Unset == component_ptr.ComponentType())
      ? MYON_SubDComponentPtr::Null
      : component_ptr;
  }
}

MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const class MYON_SubD& subd
  )
{
  const MYON_SubDLevel& level = subd.ActiveLevel();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    level.m_edge_count,
    level.m_edge[0],
    level.m_edge[1],
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const class MYON_SubDRef& subd_ref
  )
{
  const MYON_SubDLevel& level = subd_ref.SubD().ActiveLevel();
  Internal_Init(
    subd_ref,
    level.m_edge_count,
    level.m_edge[0],
    level.m_edge[1],
    MYON_SubDComponentPtr::Null
  );
}


MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const MYON_SubD& subd,
  const MYON_SubDEdge& edge
  )
{
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    1,
    &edge,
    &edge,
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const MYON_SubDRef& subd_ref,
  const MYON_SubDEdge& edge
  )
{
  Internal_Init(
    subd_ref,
    1,
    &edge,
    &edge,
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const class MYON_SubD& subd,
  const class MYON_SubDVertex& vertex
  )
{
  const unsigned int edge_count = vertex.EdgeCount();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    edge_count,
    vertex.Edge(0),
    vertex.Edge(edge_count - 1),
    MYON_SubDComponentPtr::Create(&vertex)
  );
}

MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const class MYON_SubDRef& subd_ref,
  const class MYON_SubDVertex& vertex
  )
{
  const unsigned int edge_count = vertex.EdgeCount();
  Internal_Init(
    subd_ref,
    edge_count,
    vertex.Edge(0),
    vertex.Edge(edge_count - 1),
    MYON_SubDComponentPtr::Create(&vertex)
  );
}

MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const class MYON_SubD& subd,
  const class MYON_SubDFace& face
  )
{
  const unsigned int edge_count = face.EdgeCount();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    edge_count,
    face.Edge(0),
    face.Edge(edge_count - 1),
    MYON_SubDComponentPtr::Create(&face)
  );
}

MYON_SubDEdgeIterator::MYON_SubDEdgeIterator(
  const class MYON_SubDRef& subd_ref,
  const class MYON_SubDFace& face
  )
  : m_subd_ref(subd_ref)
{
  const unsigned int edge_count = face.EdgeCount();
  Internal_Init(
    subd_ref,
    edge_count,
    face.Edge(0),
    face.Edge(edge_count - 1),
    MYON_SubDComponentPtr::Create(&face)
  );
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDEdgeIterator::MYON_SubDEdgeIterator( MYON_SubDEdgeIterator&& src) MYON_NOEXCEPT
  : m_subd_ref(std::move(src.m_subd_ref))
  , m_e_first(src.m_e_first)
  , m_e_last(src.m_e_last)
  , m_e_current(src.m_e_current)
  , m_edge_index(src.m_edge_index)
  , m_edge_count(src.m_edge_count)
  , m_component_ptr(src.m_component_ptr)
{}

// rvalue assignment operator
MYON_SubDEdgeIterator& MYON_SubDEdgeIterator::operator=(MYON_SubDEdgeIterator&& src)
{
  m_subd_ref.Clear();
  m_subd_ref = std::move(src.m_subd_ref);
  m_e_first = src.m_e_first;
  m_e_last = src.m_e_last;
  m_e_current = src.m_e_current;
  m_edge_index = src.m_edge_index;
  m_edge_count = src.m_edge_count;
  m_component_ptr = src.m_component_ptr;
  return *this;
}
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceIterator
//
void MYON_SubDFaceIterator::Internal_Init(
  const MYON_SubDRef& subd_ref,
  unsigned int face_count,
  const MYON_SubDFace* first,
  const MYON_SubDFace* last,
  MYON_SubDComponentPtr component_ptr
)
{
  m_subd_ref = subd_ref;
  if (nullptr != first && nullptr != last && face_count > 0)
  {
    m_face_first = first;
    m_face_last = last;
    m_face_current = m_face_first;
    m_face_count = face_count;
    m_component_ptr
      = (MYON_SubDComponentPtr::Type::Unset == component_ptr.ComponentType())
      ? MYON_SubDComponentPtr::Null
      : component_ptr;
  }
}

MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const MYON_SubD& subd
  )
{
  const MYON_SubDLevel& level = subd.ActiveLevel();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    level.m_face_count,
    level.m_face[0],
    level.m_face[1],
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const MYON_SubDRef& subd_ref
  )
{
  const MYON_SubDLevel& level = subd_ref.SubD().ActiveLevel();
  Internal_Init(
    subd_ref,
    level.m_face_count,
    level.m_face[0],
    level.m_face[1],
    MYON_SubDComponentPtr::Null
  );
}


MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const MYON_SubD& subd,
  const MYON_SubDFace& face
  )
{
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    1,
    &face,
    &face,
    MYON_SubDComponentPtr::Null
  );
}


MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const MYON_SubDRef& subd_ref,
  const MYON_SubDFace& face
  )
{
  Internal_Init(
    subd_ref,
    1,
    &face,
    &face,
    MYON_SubDComponentPtr::Null
  );
}

MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const class MYON_SubD& subd,
  const class MYON_SubDVertex& vertex
)
{
  const unsigned int face_count = vertex.FaceCount();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    face_count,
    vertex.Face(0),
    vertex.Face(face_count - 1),
    MYON_SubDComponentPtr::Create(&vertex)
  );
}

MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const class MYON_SubDRef& subd_ref,
  const class MYON_SubDVertex& vertex
)
{
  const unsigned int face_count = vertex.FaceCount();
  Internal_Init(
    subd_ref,
    face_count,
    vertex.Face(0),
    vertex.Face(face_count - 1),
    MYON_SubDComponentPtr::Create(&vertex)
  );
}

MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const class MYON_SubD& subd,
  const class MYON_SubDEdge& edge
)
{
  const unsigned int face_count = edge.FaceCount();
  Internal_Init(
    MYON_SubDRef::CreateReferenceForExperts(subd),
    face_count,
    edge.Face(0),
    edge.Face(face_count - 1),
    MYON_SubDComponentPtr::Create(&edge)
  );
}

MYON_SubDFaceIterator::MYON_SubDFaceIterator(
  const class MYON_SubDRef& subd_ref,
  const class MYON_SubDEdge& edge
)
{
  const unsigned int face_count = edge.FaceCount();
  Internal_Init(
    subd_ref,
    face_count,
    edge.Face(0),
    edge.Face(face_count - 1),
    MYON_SubDComponentPtr::Create(&edge)
  );
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDFaceIterator::MYON_SubDFaceIterator( MYON_SubDFaceIterator&& src) MYON_NOEXCEPT
  : m_subd_ref(std::move(src.m_subd_ref))
  , m_face_first(src.m_face_first)
  , m_face_last(src.m_face_last)
  , m_face_current(src.m_face_current)
  , m_face_index(src.m_face_index)
  , m_face_count(src.m_face_count)
  , m_component_ptr(src.m_component_ptr)
{}

// rvalue assignment operator
MYON_SubDFaceIterator& MYON_SubDFaceIterator::operator=(MYON_SubDFaceIterator&& src)
{
  m_subd_ref.Clear();
  m_subd_ref = std::move(src.m_subd_ref);
  m_face_first = src.m_face_first;
  m_face_last = src.m_face_last;
  m_face_current = src.m_face_current;
  m_face_index = src.m_face_index;
  m_face_count = src.m_face_count;
  m_component_ptr = src.m_component_ptr;
  return *this;
}
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceEdgeIterator
//

MYON_SubDFaceEdgeIterator::MYON_SubDFaceEdgeIterator()
  : m_face(nullptr)
  , m_edge_count(0)
  , m_edge_index0(0)
  , m_edge_index(0)
{}

MYON_SubDFaceEdgeIterator::MYON_SubDFaceEdgeIterator(
  const MYON_SubDFace* face
  )
  : m_face(nullptr)
  , m_edge_count(0)
  , m_edge_index0(0)
  , m_edge_index(0)
{
  Initialize(face, nullptr);
}

MYON_SubDFaceEdgeIterator::MYON_SubDFaceEdgeIterator(
  const MYON_SubDFace* face,
  const MYON_SubDEdge* first_edge
  )
  : m_face(nullptr)
  , m_edge_count(0)
  , m_edge_index0(0)
  , m_edge_index(0)
{
  Initialize(face, first_edge);
}

unsigned int MYON_SubDFaceEdgeIterator::Initialize(
  const MYON_SubDFace* face
  )
{
  return Initialize(face, nullptr);
}

unsigned int MYON_SubDFaceEdgeIterator::Initialize(
  const MYON_SubDFace* face,
  const MYON_SubDEdge* first_edge
  )
{
  m_face = face;
  m_edge_count = (nullptr != face) ? face->m_edge_count : 0;
  if (m_edge_count > 0 && nullptr != first_edge)
  {
    m_edge_index0 = face->EdgeArrayIndex(first_edge);
    if (MYON_UNSET_UINT_INDEX == m_edge_index0)
    {
      m_edge_count = 0;
      m_edge_index0 = 0;
    }
  }
  else
  {
    m_edge_index0 = 0;
  }
  m_edge_index = m_edge_index0;
  return m_edge_count;
}

unsigned int MYON_SubDFaceEdgeIterator::EdgeCount() const
{
  return m_edge_count;
}

const MYON_SubDEdge* MYON_SubDFaceEdgeIterator::FirstEdge()
{
  m_edge_index = m_edge_index0;
  return CurrentEdge();
}

const MYON_SubDEdge* MYON_SubDFaceEdgeIterator::CurrentEdge() const
{
  return (nullptr != m_face) ? m_face->Edge(m_edge_index) : nullptr;
}

const MYON_SubDEdge* MYON_SubDFaceEdgeIterator::NextEdge()
{
  if (m_edge_count > 0)
  {
    m_edge_index = (m_edge_index + 1) % m_edge_count;
    return CurrentEdge();
  }
  return nullptr;
}

const MYON_SubDEdge* MYON_SubDFaceEdgeIterator::PrevEdge()
{
  if (m_edge_count > 0)
  {
    m_edge_index = (m_edge_index + (m_edge_count - 1)) % m_edge_count;
    return CurrentEdge();
  }
  return nullptr;
}


const MYON_SubDEdge* MYON_SubDFaceEdgeIterator::NextEdge(bool bReturnNullAtFirstEdge)
{
  if (m_edge_count > 0)
  {
    const unsigned int next_edge_index = (m_edge_index + 1) % m_edge_count;
    if (bReturnNullAtFirstEdge && next_edge_index == m_edge_index0)
      return nullptr;
    m_edge_index = next_edge_index;
    return CurrentEdge();
  }
  return nullptr;
}

const MYON_SubDEdge* MYON_SubDFaceEdgeIterator::PrevEdge(bool bReturnNullAtFirstEdge)
{
  if (m_edge_count > 0)
  {
    const unsigned int prev_edge_index = (m_edge_index + (m_edge_count - 1)) % m_edge_count;
    if (bReturnNullAtFirstEdge && prev_edge_index == m_edge_index0)
      return nullptr;
    m_edge_index = prev_edge_index;
    return CurrentEdge();
  }
  return nullptr;
}

unsigned int MYON_SubDFaceEdgeIterator::FirstEdgeIndex() const
{
  return m_edge_index0;
}

unsigned int MYON_SubDFaceEdgeIterator::CurrentEdgeIndex() const
{
  return m_edge_index;
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDSectorId
//
const MYON_SubDSectorId MYON_SubDSectorId::Create(
  const MYON_SubDVertex* vertex,
  const MYON_SubDFace* face
)
{
  MYON_SubDSectorId sid;

  if (nullptr == face && nullptr != vertex && vertex->IsSingleSectorVertex())
    face = vertex->Face(0);

  if (nullptr == vertex || nullptr == face)
  {
    // Creation of MYON_SubDSectorId::Invalid in opennurbs_statics.cpp or 
    // just another invalid input.
    sid.m_vertex_id = 0;
    sid.m_minimum_face_id = 0;
    sid.m_sector_face_count = 0xFFFFFFFFU;
    return sid;
  }

  for (;;)
  {
    if (vertex->m_id <= 0 || vertex->m_id >= MYON_UNSET_UINT_INDEX)
      break;
    if (face->m_id <= 0 || face->m_id >= MYON_UNSET_UINT_INDEX)
      break;
    if (face->m_edge_count < 3)
      break;
    if (vertex->m_edge_count < 2)
      break;
    if (vertex->m_face_count < 1)
      break;
    const unsigned face_vi = face->VertexIndex(vertex);
    if (face_vi >= face->m_edge_count)
      break;

    sid.m_vertex_id = vertex->m_id;
    sid.m_minimum_face_id = face->m_id;
    const unsigned short n = vertex->m_face_count;
    unsigned short crease_face_count = 0;
    unsigned int expected_crease_count = 0;
    if (vertex->IsSmoothOrDart())
    {
      if (n < 2 || n != vertex->m_edge_count)
        break;
      const bool bIsDart = vertex->IsDart();
      crease_face_count = bIsDart ? 2 : 0;
      expected_crease_count = bIsDart ? 1 : 0;
    }
    else if ( vertex->IsCreaseOrCorner())
    {
      const MYON_SubDComponentPtrPair pair 
        = (n + 1 == vertex->m_edge_count && 2==vertex->CreasedEdgeCount(true,true,true,false))
          ? vertex->BoundaryEdgePair()
          : MYON_SubDComponentPtrPair::Null;
      if (pair.BothAreNotNull())
      {
        // we can avoid using a sector iterator
        const MYON_SubDEdge* e[2] = { pair.First().Edge(),pair.Second().Edge() };
        if (nullptr == e[0] || nullptr == e[1])
          break;
        if (1 != e[0]->m_face_count || 1 != e[1]->m_face_count)
          break;
        crease_face_count = 1;
        expected_crease_count = 2;
      }
      else
      {
        if (vertex->IsCrease())
        {
          if (n != vertex->m_edge_count)
            return MYON_SubDSectorId::Invalid;
        }

        // complicated case needs to use more time consuming sector iterator 
        MYON_SubDSectorIterator sit0;
        if ( vertex != sit0.Initialize(face, 0, face_vi))
          return MYON_SubDSectorId::Invalid;
        MYON_SubDSectorIterator sit1(sit0);
        // advance sit0 until we hit a crease;
        sid.m_sector_face_count = 1;
        for (const MYON_SubDFace* f = sit0.NextFace(MYON_SubDSectorIterator::StopAt::AnyCrease); nullptr != f && sid.m_sector_face_count <= vertex->m_face_count; f = sit0.NextFace(MYON_SubDSectorIterator::StopAt::AnyCrease))
        {
          ++sid.m_sector_face_count;
          if (f->m_id < sid.m_minimum_face_id)
            sid.m_minimum_face_id = f->m_id;
        }
        // advance sit1 until we hit a crease;
        for (const MYON_SubDFace* f = sit1.PrevFace(MYON_SubDSectorIterator::StopAt::AnyCrease); nullptr != f && sid.m_sector_face_count <= vertex->m_face_count; f = sit1.PrevFace(MYON_SubDSectorIterator::StopAt::AnyCrease))
        {
          ++sid.m_sector_face_count;
          if (f->m_id < sid.m_minimum_face_id)
            sid.m_minimum_face_id = f->m_id;
        }
        break;
      }
    }
    else
    {
      break;
    }

    if (0 == sid.m_sector_face_count)
    {
      // we can avoid expensive sector iterator.
      unsigned int crease_count = 0;
      const MYON_SubDFace* f;
      for (unsigned short vei = 0; vei < vertex->m_edge_count; ++vei)
      {
        const MYON_SubDEdge* e = MYON_SUBD_EDGE_POINTER(vertex->m_edges[vei].m_ptr);
        if (nullptr == e || 0 == e->m_face_count)
          return MYON_SubDSectorId::Invalid;
        f = MYON_SUBD_FACE_POINTER(e->m_face2[0].m_ptr);
        if (nullptr == f)
          return MYON_SubDSectorId::Invalid;
        if (f->m_id < sid.m_minimum_face_id)
          sid.m_minimum_face_id = f->m_id;
        if (MYON_SubDEdgeTag::Crease == e->m_edge_tag)
        {
          ++crease_count;
          if (crease_count > expected_crease_count)
            return MYON_SubDSectorId::Invalid;
          if (crease_face_count != e->m_face_count)
            return MYON_SubDSectorId::Invalid;
          if (1 == crease_face_count)
            continue;
        }
        if (2 != e->m_face_count)
          return MYON_SubDSectorId::Invalid;
        f = MYON_SUBD_FACE_POINTER(e->m_face2[1].m_ptr);
        if (nullptr == f)
          return MYON_SubDSectorId::Invalid;
        if (f->m_id < sid.m_minimum_face_id)
          sid.m_minimum_face_id = f->m_id;
      }
      if (crease_count == expected_crease_count)
        sid.m_sector_face_count = n;
    }
    break;
  }

  return (sid.IsSet() && sid.m_sector_face_count <= vertex->m_face_count) ? sid : MYON_SubDSectorId::Invalid;
}

const MYON_SubDSectorId MYON_SubDSectorId::CreateFromIds(
  unsigned int vertex_id,
  unsigned int minimum_face_id
)
{
  MYON_SubDSectorId sid;
  sid.m_vertex_id = vertex_id;
  sid.m_minimum_face_id = minimum_face_id;
  sid.m_sector_face_count = 0;
  return sid;
}

int MYON_SubDSectorId::Compare(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs)
{
  if (lhs.m_vertex_id < rhs.m_vertex_id)
    return -1;
  if (lhs.m_vertex_id > rhs.m_vertex_id)
    return 1;
  if (lhs.m_minimum_face_id < rhs.m_minimum_face_id)
    return -1;
  if (lhs.m_minimum_face_id > rhs.m_minimum_face_id)
    return 1;
  if (lhs.m_sector_face_count < rhs.m_sector_face_count)
    return -1;
  if (lhs.m_sector_face_count > rhs.m_sector_face_count)
    return 1;
  return 0;
}

int MYON_SubDSectorId::CompareFromPointers(const MYON_SubDSectorId* lhs, const MYON_SubDSectorId* rhs)
{
  if (nullptr == lhs || nullptr == rhs)
  {
    // sort nullptr to end
    if (nullptr != lhs)
      return -1;
    if (nullptr != rhs)
      return 1; 
    return 0;
  }
  return MYON_SubDSectorId::Compare(*lhs, *rhs);
}

const unsigned int MYON_SubDSectorId::VertexId() const
{
  return m_vertex_id;
}

const unsigned int MYON_SubDSectorId::MinimumFaceId() const
{
  return m_minimum_face_id;
}

const unsigned int MYON_SubDSectorId::SectorFaceCount() const
{
  return m_sector_face_count <= 0x10000U ? m_sector_face_count : 0U;
}

bool MYON_SubDSectorId::IsZero() const
{
  return 0 == m_vertex_id && 0 == m_minimum_face_id && 0 == m_sector_face_count;
}

bool MYON_SubDSectorId::IsSet() const
{
  // the maximum valid subd vertex face count is 0xFFFF.
  return m_vertex_id > 0 && m_minimum_face_id > 0 && m_sector_face_count > 0 && m_vertex_id < MYON_UNSET_UINT_INDEX&& m_minimum_face_id < MYON_UNSET_UINT_INDEX&& m_sector_face_count < 0x10000U;
}


bool operator==(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs)
{
  return 0 == MYON_SubDSectorId::Compare(lhs, rhs);
}

bool operator!=(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs)
{
  return 0 != MYON_SubDSectorId::Compare(lhs, rhs);
}

bool operator>(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs)
{
  return MYON_SubDSectorId::Compare(lhs, rhs) < 0;
}

bool operator<(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs)
{
  return MYON_SubDSectorId::Compare(lhs, rhs) > 0;
}

bool operator>=(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs)
{
  return MYON_SubDSectorId::Compare(lhs, rhs) >= 0;
}

bool operator<=(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs)
{
  return MYON_SubDSectorId::Compare(lhs, rhs) <= 0;
}


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDSectorIterator
//

static MYON_SubDSectorIterator MYON_SectorIterator_EmptyInitialize()
{  
  MYON_SubDSectorIterator empty_init;
  empty_init.Initialize();
  return empty_init;
}

const MYON_SubDSectorIterator MYON_SubDSectorIterator::Empty(MYON_SectorIterator_EmptyInitialize());

void MYON_SubDSectorIterator::Initialize()
{
  // apply in place construction to this
  new (this) MYON_SubDSectorIterator();
  m_current_eptr[0] = MYON_SubDEdgePtr::Null;
  m_current_eptr[1] = MYON_SubDEdgePtr::Null;
  m_current_fei[0] = 0;
  m_current_fei[1] = 0;
}

const MYON_SubDVertex* MYON_SubDSectorIterator::Initialize(
  const MYON_SubDVertex* center_vertex
  )
{
  Initialize();
  if (nullptr == center_vertex || 0 == center_vertex->m_face_count || nullptr == center_vertex->m_faces)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON__UINT_PTR initial_face_direction = 0;
  return Initialize(center_vertex->m_faces[0], initial_face_direction, center_vertex);
}

const MYON_SubDVertex* MYON_SubDSectorIterator::Initialize(
  const MYON_SubDFace* initial_face,
  MYON__UINT_PTR iterator_orientation,
  const MYON_SubDVertex* center_vertex
  )
{
  if (nullptr != center_vertex && nullptr != initial_face)
  {
    MYON__UINT_PTR eptr;
    const MYON_SubDEdge* edge;
    const unsigned int edge_count = initial_face->m_edge_count;
    const MYON_SubDEdgePtr* face_eptr = initial_face->m_edge4;
    for (unsigned int efi = 0; efi < edge_count; efi++, face_eptr++)
    {
      if (4 == efi)
      {
        face_eptr = initial_face->m_edgex;
        if (nullptr == face_eptr)
          break;
      }
      eptr = face_eptr->m_ptr;
      edge = MYON_SUBD_EDGE_POINTER(eptr);
      if (nullptr == edge)
        continue;
      eptr = MYON_SUBD_EDGE_DIRECTION(eptr);
      if (center_vertex == edge->m_vertex[eptr])
        return Initialize(initial_face, iterator_orientation, efi);
    }
  }

  Initialize();
  return MYON_SUBD_RETURN_ERROR(nullptr);
}

const MYON_SubDVertex* MYON_SubDSectorIterator::Initialize(
  const MYON_SubDFace* initial_face,
  MYON__UINT_PTR iterator_orientation,
  unsigned int face_vertex_index
  )
{
  Initialize();

  for (;;)
  {
    if (nullptr == initial_face)
      break;

    if (0 != iterator_orientation && 1 != iterator_orientation)
      break;

    const unsigned int face_edge_count = initial_face->m_edge_count;
    if (face_vertex_index >= face_edge_count)
      break;

    m_initial_face = initial_face;
    m_initial_fvi = face_vertex_index;

    //  initial_face->Edge(face_vertex_index) = edge "leaving" the center vertex
    MYON_SubDEdgePtr eptr;
    if (face_vertex_index < 4)
    {
      eptr = initial_face->m_edge4[face_vertex_index];
    }
    else
    {
      if (nullptr == initial_face->m_edgex)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      eptr = initial_face->m_edgex[face_vertex_index - 4];
    }

    const MYON_SubDEdge* edge0 = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
    if (nullptr == edge0)
      break;

    // m_current_eptr[0] = "prev" side edge
    m_current_fei[0] = face_vertex_index;
    m_current_eptr[0] = eptr;


    //  back up one vertex on the face
    face_vertex_index = (face_vertex_index+face_edge_count-1) % face_edge_count;

    //  initial_face->Edge(face_vertex_index) = edge "entering" the center vertex
    if (face_vertex_index < 4)
    {
      eptr = initial_face->m_edge4[face_vertex_index];
    }
    else
    {
      if (nullptr == initial_face->m_edgex)
        return MYON_SUBD_RETURN_ERROR(nullptr);
      eptr = initial_face->m_edgex[face_vertex_index - 4];
    }

    const MYON_SubDEdge* edge1 = MYON_SUBD_EDGE_POINTER(eptr.m_ptr);
    if (nullptr == edge1)
      break;

    // m_current_eptr[1] = "next" side edge
    m_current_fei[1] = face_vertex_index;
    m_current_eptr[1] = eptr.Reversed(); // (reversed because face's eptr ends at center vertex)

    const MYON_SubDVertex* center_vertex0 = edge0->m_vertex[MYON_SUBD_EDGE_DIRECTION(m_current_eptr[0].m_ptr)];
    const MYON_SubDVertex* center_vertex1 = edge1->m_vertex[MYON_SUBD_EDGE_DIRECTION(m_current_eptr[1].m_ptr)];
    
    if (center_vertex0 != center_vertex1)
    {
      if (nullptr == center_vertex0)
        m_center_vertex = center_vertex1;
      else if ( nullptr == center_vertex1)
        m_center_vertex = center_vertex0;
      else
        break;
    }
    else if (nullptr != center_vertex0)
      m_center_vertex = center_vertex0;
    else
      break;

    if (1 == iterator_orientation)
    {
      m_initial_face_dir = 1;
      unsigned int i = m_current_fei[0];
      m_current_fei[0] = m_current_fei[1];
      m_current_fei[1] = i;
      eptr = m_current_eptr[0];
      m_current_eptr[0] = m_current_eptr[1];
      m_current_eptr[1] = eptr;
    }
    m_current_face = m_initial_face;
    m_current_face_dir = m_initial_face_dir;
    m_current_fvi = m_initial_fvi;
    
#if defined(MYON_DEBUG)
    IsValid();
#endif

    return m_center_vertex;
  }

  Initialize();
  return MYON_SUBD_RETURN_ERROR(nullptr);
}

const MYON_SubDFace* MYON_SubDSectorIterator::FirstFace()
{
  if (nullptr == m_initial_face)
    Initialize();
  else
    Initialize(m_initial_face, m_initial_face_dir, m_initial_fvi);
  return m_initial_face;
}

bool MYON_SubDSectorIterator::IsValid() const
{
  if (nullptr == m_center_vertex && nullptr == m_initial_face)
    return true;

  if (nullptr == m_center_vertex || nullptr == m_initial_face)
    return MYON_SUBD_RETURN_ERROR(false);

  if ( m_center_vertex != m_initial_face->Vertex(m_initial_fvi) )
    return MYON_SUBD_RETURN_ERROR(false);

  if (nullptr == m_current_face)
    return true; // finished iterating

  if (m_center_vertex != m_current_face->Vertex(m_current_fvi))
    return MYON_SUBD_RETURN_ERROR(false);

  const MYON_SubDEdge* edge[2] = { m_current_eptr[0].Edge(), m_current_eptr[1].Edge() };
  if (nullptr == edge[0] || nullptr == edge[1])
    return MYON_SUBD_RETURN_ERROR(false);
  if (m_center_vertex != edge[0]->m_vertex[MYON_SUBD_EDGE_DIRECTION(m_current_eptr[0].m_ptr)])
    return MYON_SUBD_RETURN_ERROR(false);
  if (m_center_vertex != edge[1]->m_vertex[MYON_SUBD_EDGE_DIRECTION(m_current_eptr[1].m_ptr)])
    return MYON_SUBD_RETURN_ERROR(false);

  const MYON_SubDEdgePtr feptr[2] = { m_current_face->EdgePtr(m_current_fei[0]), m_current_face->EdgePtr(m_current_fei[1]) };
  
  if (edge[0] != MYON_SUBD_EDGE_POINTER(feptr[0].m_ptr))
    return MYON_SUBD_RETURN_ERROR(false);

  if (edge[1] != MYON_SUBD_EDGE_POINTER(feptr[1].m_ptr))
    return MYON_SUBD_RETURN_ERROR(false);

  return true;
}


const MYON_SubDVertex* MYON_SubDSectorIterator::CenterVertex() const
{
  return m_center_vertex;
}

const MYON_SubDFace* MYON_SubDSectorIterator::InitialFace() const
{
  return m_initial_face;
}

int MYON_SubDSectorIterator::CurrentRingIndex() const
{
  return m_current_ring_index;
}

const MYON_SubDFace* MYON_SubDSectorIterator::CurrentFace() const
{
  return m_current_face;
}

MYON_SubDFacePtr MYON_SubDSectorIterator::CurrentFacePtr() const
{
  return MYON_SubDFacePtr::Create(m_current_face,m_current_face_dir);
}

unsigned int MYON_SubDSectorIterator::InitialFaceCenterVertexIndex() const
{
  return m_initial_fvi;
}

unsigned int MYON_SubDSectorIterator::CurrentFaceCenterVertexIndex() const
{
  return m_current_fvi;
}


unsigned int MYON_SubDSectorIterator::CurrentFaceDirection() const
{
  return m_current_face_dir;
}

MYON_SubDEdgePtr MYON_SubDSectorIterator::CurrentEdgePtr(
  unsigned int face_side_index
  ) const
{
  return (face_side_index < 2) ? m_current_eptr[face_side_index] : MYON_SubDEdgePtr::Null;
}



const MYON_SubDEdge* MYON_SubDSectorIterator::CurrentEdge(
  unsigned int face_side_index
  ) const
{
  return (face_side_index < 2) ? MYON_SUBD_EDGE_POINTER(m_current_eptr[face_side_index].m_ptr) : nullptr;
}

MYON__UINT_PTR MYON_SubDSectorIterator::CurrentEdgeDirection(
  unsigned int face_side_index
  ) const
{
  return (face_side_index < 2) ? MYON_SUBD_EDGE_DIRECTION(m_current_eptr[face_side_index].m_ptr) : 0;
}

const MYON_SubDVertex* MYON_SubDSectorIterator::CurrentEdgeRingVertex(
  unsigned int face_side_index
  ) const
{
  if (face_side_index >= 2)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  MYON__UINT_PTR eptr = m_current_eptr[face_side_index].m_ptr;
  const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(eptr);
  if (nullptr == edge)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  const MYON_SubDVertex* edge_ring_vertex = edge->m_vertex[1 - MYON_SUBD_EDGE_DIRECTION(eptr)];
  if ( nullptr == edge_ring_vertex || edge_ring_vertex == m_center_vertex)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  return edge_ring_vertex;
}

const MYON_SubDFace* MYON_SubDSectorIterator::NextFace(
  MYON_SubDSectorIterator::StopAt stop_at
  )
{
  return IncrementFace(1, stop_at);
}

const MYON_SubDFace* MYON_SubDSectorIterator::PrevFace(
  MYON_SubDSectorIterator::StopAt stop_at
  )
{
  return IncrementFace(-1, stop_at);
}

const MYON_SubDFace* MYON_SubDSectorIterator::IncrementFace(
  int increment_direction,
  MYON_SubDSectorIterator::StopAt stop_at
  )
{
  if (nullptr == m_current_face)
  {
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  // increment_direction > 0 means counterclockwise / next
  // increment_direction <= 0 means clockwise / prev
  unsigned int side_index = (increment_direction > 0) ? 1 : 0; 
  MYON__UINT_PTR eptr = m_current_eptr[side_index].m_ptr;
  const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(eptr);
  if (nullptr == edge)
  {
    return MYON_SUBD_RETURN_ERROR(nullptr);
  }

  for (;;)
  {
    if (edge->m_face_count != 2)
    {
      // set stop_at to something not equal to Boundary
      stop_at = MYON_SubDSectorIterator::StopAt::AnyCrease;
      break;
    }

    if (MYON_SubDSectorIterator::StopAt::Boundary != stop_at)
    {
      if (MYON_SubDEdgeTag::Crease == edge->m_edge_tag)
      {
        if (MYON_SubDSectorIterator::StopAt::AnyCrease == stop_at)
          break;
        if (MYON_SubDSectorIterator::StopAt::HardCrease == stop_at && edge->IsHardCrease())
          break;
      }
      stop_at = MYON_SubDSectorIterator::StopAt::Boundary;
    }

    unsigned int efi;
    if (m_current_face == MYON_SUBD_FACE_POINTER(edge->m_face2[0].m_ptr))
    {
      efi = 0;
    }
    else if (m_current_face == MYON_SUBD_FACE_POINTER(edge->m_face2[1].m_ptr))
    {
      efi = 1;
    }
    else
      break;
    const MYON__UINT_PTR ef0dir = MYON_SUBD_FACE_DIRECTION(edge->m_face2[efi].m_ptr);
    efi = 1 - efi;
    const MYON__UINT_PTR ef1dir = MYON_SUBD_FACE_DIRECTION(edge->m_face2[efi].m_ptr);
    const MYON_SubDFace* face1 = MYON_SUBD_FACE_POINTER(edge->m_face2[efi].m_ptr);
    if (nullptr == face1)
      break;
    const unsigned int face1_edge_count = face1->m_edge_count;
    const MYON_SubDEdgePtr* face1_edges = face1->m_edge4;
    for (unsigned int fei = 0; fei < face1_edge_count; fei++, face1_edges++)
    {
      if (4 == fei)
      {
        if (nullptr == (face1_edges = face1->m_edgex))
          break;
      }

      if (edge != MYON_SUBD_EDGE_POINTER(face1_edges->m_ptr))
        continue;

      // At this point, face1->Edges(fei) is the edge I just hopped accross
      // to get from the old current face to face 1. Update current face
      // information and return.
      m_current_face = face1; 
      if (ef0dir == ef1dir)
        m_current_face_dir = 1 - m_current_face_dir;

      side_index = 1 - side_index;
      m_current_fei[side_index] = fei;
      if (m_center_vertex == edge->m_vertex[0])
        m_current_eptr[side_index] = MYON_SubDEdgePtr::Create(edge,0);
      else if (m_center_vertex == edge->m_vertex[1])
        m_current_eptr[side_index] = MYON_SubDEdgePtr::Create(edge,1);
      else
        break;

      if (side_index == m_current_face_dir)
      {
        m_current_fvi = fei;
        fei = (fei > 0) ? (fei - 1) : (face1_edge_count - 1);
      }
      else
      {
        fei = (fei + 1) % face1_edge_count;
        m_current_fvi = fei;
      }

      if (fei < 4)
        face1_edges = face1->m_edge4;
      else
      {
        if (nullptr == (face1_edges = face1->m_edgex))
          break;
        face1_edges -= 4;
      }
      side_index = 1 - side_index;
      m_current_fei[side_index] = fei;
      edge = MYON_SUBD_EDGE_POINTER(face1_edges[fei].m_ptr);
      if ( nullptr == edge)
        break;
      if (m_center_vertex == edge->m_vertex[0])
        m_current_eptr[side_index] = MYON_SubDEdgePtr::Create(edge,0);
      else if (m_center_vertex == edge->m_vertex[1])
        m_current_eptr[side_index] = MYON_SubDEdgePtr::Create(edge,1);
      else
        break;

      if (increment_direction > 0)
        m_current_ring_index++;
      else
        m_current_ring_index--;

#if defined(MYON_DEBUG)
      IsValid();
#endif

      return m_current_face;
    }

    break;
  }

  // termination
  m_current_face = nullptr;
  m_current_fvi = 0;
  m_current_fei[0] = 0;
  m_current_fei[1] = 0;
  
  if (MYON_SubDSectorIterator::StopAt::Boundary != stop_at)
  {
    // termination at a crease, nonmanifold edge, or edge with one face
    m_current_eptr[1 - side_index] = m_current_eptr[side_index];
    m_current_eptr[side_index] = MYON_SubDEdgePtr::Null;
    return nullptr;
  }

  // error condition
  m_current_eptr[0] = MYON_SubDEdgePtr::Null;
  m_current_eptr[0] = MYON_SubDEdgePtr::Null;

  return MYON_SUBD_RETURN_ERROR(nullptr);
}

const MYON_SubDFace* MYON_SubDSectorIterator::IncrementToCrease(
  int increment_direction
  )
{
  if (nullptr == m_center_vertex)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  const unsigned int N = m_center_vertex->m_edge_count;
  const unsigned int edge_side = increment_direction > 0 ? 1 : 0;

  MYON_SubDSectorIterator sit(*this);
  const MYON_SubDFace* face0 = sit.CurrentFace();
  if (nullptr == face0)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  // The for (unsigned int i = 0; i < N; i++) {} prevents infinite looping
  // if the topology is pointers contain an invalid cycle.
  for (unsigned int i = 0; i < N; i++)
  {
    const MYON_SubDEdge* edge = sit.CurrentEdge(edge_side);
    if (nullptr == edge)
      return MYON_SUBD_RETURN_ERROR(nullptr);

    if (edge->m_face_count != 2 || MYON_SubDEdgeTag::Crease == edge->m_edge_tag)
    {
      *this = sit;
      return CurrentFace();
    }

    const MYON_SubDFace* face = sit.IncrementFace(increment_direction,MYON_SubDSectorIterator::StopAt::AnyCrease);
    if (nullptr == face)
      return MYON_SUBD_RETURN_ERROR(nullptr);
    if ( face == face0 )
      return nullptr; // not an error - no crease and back where we started
  }

  return MYON_SUBD_RETURN_ERROR(nullptr);
}

bool MYON_SubDSectorIterator::InitializeToCurrentFace()
{
  const MYON_SubDFace* face = CurrentFace();
  if ( nullptr == face)
    return MYON_SUBD_RETURN_ERROR(false);
  MYON_SubDSectorIterator sit;
  if (nullptr == sit.Initialize(face, sit.CurrentFaceDirection(), sit.CurrentFaceCenterVertexIndex()))
    return MYON_SUBD_RETURN_ERROR(false);
  if (face != sit.CurrentFace())
    return MYON_SUBD_RETURN_ERROR(false);
  *this = sit;
  return true;
}



//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentIterator
//

MYON_SubDComponentIterator::MYON_SubDComponentIterator(
  const class MYON_SubD& subd
  )
{
  *this = MYON_SubDComponentIterator(MYON_SubDRef::CreateReferenceForExperts(subd));
}

MYON_SubDComponentIterator::MYON_SubDComponentIterator(
  const class MYON_SubDRef& subd_ref
  )
{
  m_subd_ref = subd_ref;
  const MYON_SubDLevel& level = subd_ref.SubD().ActiveLevel();
  const MYON_SubDFace* f_first = level.m_face[0];
  const MYON_SubDFace* f_last = level.m_face[1];
  if (f_first && f_last)
  {
    m_face_first = f_first;
    m_face_last = f_last;
    m_cptr_current = MYON_SubDComponentPtr::Create(m_face_first);
  }

  const MYON_SubDEdge* e_first = level.m_edge[0];
  const MYON_SubDEdge* e_last = level.m_edge[1];
  if (e_first && e_last)
  {
    m_edge_first = e_first;
    m_edge_last = e_last;
    m_cptr_current = MYON_SubDComponentPtr::Create(m_edge_first);
  }

  const MYON_SubDVertex* v_first = level.m_vertex[0];
  const MYON_SubDVertex* v_last = level.m_vertex[1];
  if (v_first && v_last)
  {
    m_vertex_first = v_first;
    m_vertex_last = v_last;
    m_cptr_current = MYON_SubDComponentPtr::Create(m_vertex_first);
  }
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDComponentIterator::MYON_SubDComponentIterator( MYON_SubDComponentIterator&& src) MYON_NOEXCEPT
  : m_subd_ref(std::move(src.m_subd_ref))
  , m_subd_level(src.m_subd_level)
  , m_vertex_first(src.m_vertex_first)
  , m_vertex_last(src.m_vertex_last)
  , m_edge_first(src.m_edge_first)
  , m_edge_last(src.m_edge_last)
  , m_face_first(src.m_face_first)
  , m_face_last(src.m_face_last)
  , m_cptr_current(src.m_cptr_current)
{}

// rvalue assignment operator
MYON_SubDComponentIterator& MYON_SubDComponentIterator::operator=(MYON_SubDComponentIterator&& src)
{
  m_subd_ref.Clear();
  m_subd_ref = std::move(src.m_subd_ref);
  m_subd_level = src.m_subd_level;
  m_vertex_first = src.m_vertex_first;
  m_vertex_last = src.m_vertex_last;
  m_edge_first = src.m_edge_first;
  m_edge_last = src.m_edge_last;
  m_face_first = src.m_face_first;
  m_face_last = src.m_face_last;
  m_cptr_current = src.m_cptr_current;
  return *this;
}
#endif

/*
Description:
  Set the iterator to the beginning of the component list.
Returns:
  First component in the list.
*/
const class MYON_SubDComponentPtr MYON_SubDComponentIterator::FirstComponent()
{
  if (nullptr != m_vertex_first)
    m_cptr_current = MYON_SubDComponentPtr::Create(m_vertex_first);
  else if (nullptr != m_edge_first)
    m_cptr_current = MYON_SubDComponentPtr::Create(m_edge_first);
  else if (nullptr != m_face_first)
    m_cptr_current = MYON_SubDComponentPtr::Create(m_face_first);
  else
    m_cptr_current = MYON_SubDComponentPtr::Null;
  return m_cptr_current;
}


const class MYON_SubDComponentPtr MYON_SubDComponentIterator::NextComponent()
{
  switch (m_cptr_current.ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    {
      const MYON_SubDVertex* vertex = MYON_SUBD_VERTEX_POINTER(m_cptr_current.m_ptr);
      if (nullptr != vertex && vertex != m_vertex_last && nullptr != vertex->m_next_vertex)
      {
        m_cptr_current = MYON_SubDComponentPtr::Create(vertex->m_next_vertex);
        return m_cptr_current;
      }
    }
    m_cptr_current = MYON_SubDComponentPtr::Null;
    // no break here

  case MYON_SubDComponentPtr::Type::Edge:
    {
      if (0 == m_cptr_current.m_ptr && nullptr != m_edge_first)
      {
        // switching from vertex to edge
        m_cptr_current = MYON_SubDComponentPtr::Create(m_edge_first);
        return m_cptr_current;
      }
      const MYON_SubDEdge* edge = MYON_SUBD_EDGE_POINTER(m_cptr_current.m_ptr);
      if (nullptr != edge && edge != m_edge_last && nullptr != edge->m_next_edge )
      {
        m_cptr_current = MYON_SubDComponentPtr::Create(edge->m_next_edge);
        return m_cptr_current;
      }
    }
    m_cptr_current = MYON_SubDComponentPtr::Null;
    // no break here

  case MYON_SubDComponentPtr::Type::Face:
    {
      if (0 == m_cptr_current.m_ptr && nullptr != m_face_first)
      {
        // switching from edge to face
        m_cptr_current = MYON_SubDComponentPtr::Create(m_face_first);
        return m_cptr_current;
      }
      const MYON_SubDFace* face = MYON_SUBD_FACE_POINTER(m_cptr_current.m_ptr);
      if (nullptr != face && face != m_face_last && nullptr != face->m_next_face)
      {
        m_cptr_current = MYON_SubDComponentPtr::Create(face->m_next_face);
        return m_cptr_current;
      }
    }
    m_cptr_current = MYON_SubDComponentPtr::Null;
    break;

  default:
    break;
  }
  
  return m_cptr_current;
}

const class MYON_SubDComponentPtr MYON_SubDComponentIterator::LastComponent()
{
  if (nullptr != m_face_last)
    m_cptr_current = MYON_SubDComponentPtr::Create(m_face_last);
  else if (nullptr != m_edge_last)
    m_cptr_current = MYON_SubDComponentPtr::Create(m_edge_last);
  else if (nullptr != m_vertex_last)
    m_cptr_current = MYON_SubDComponentPtr::Create(m_vertex_last);
  else
    m_cptr_current = MYON_SubDComponentPtr::Null;
  return m_cptr_current;
}

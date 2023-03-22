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

#if defined(OPENNURBS_INC_IN_PROGRESS)
///////////////////////////////////////////////////////////////////
//
// Including opennurbs.h must NEVER include this file.
//
///////////////////////////////////////////////////////////////////
#error Please read the comment above this line.
#endif

#if defined(MYON_COMPILING_OPENNURBS)
#if !defined(OPENNURBS_SUBD_UNSTABLE_CORE_AVAIALABLE)
#define OPENNURBS_SUBD_UNSTABLE_CORE_AVAIALABLE
#endif
#endif


#if !defined(OPENNURBS_SUBD_UNSTABLE_CORE_AVAIALABLE)
///////////////////////////////////////////////////////////////////
//
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
//
// The definitions in opennurbs_subd_data.h change randomly and unpredictably.
// Never include this file in any code you write, even as a hack.
// If you ignore this warning, your code will crash randomly and unpredictably
//
///////////////////////////////////////////////////////////////////
#error Please read the comment above this line.
#endif

#if !defined(OPENNURBS_SUBD_DATA_INC_)
#define OPENNURBS_SUBD_DATA_INC_

bool MYON_SubDFaceRegionBreakpoint(
  unsigned int level0_face_id,
  const class MYON_SubDComponentRegionIndex& region_index
);

bool MYON_SubDComponentRegionBreakpoint(
  const class MYON_SubDComponentRegion* component_region
);


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceNeighborhood
//
class MYON_CLASS MYON_SubDFaceNeighborhood
{
public:
  MYON_SubDFaceNeighborhood() = default;
  ~MYON_SubDFaceNeighborhood();

  // initial face
  const MYON_SubDFace* m_face0 = nullptr;

  unsigned char m_reserved1 = 0;
  
  // The center vertex will be a smooth vertex with valence = m_face0->m_edge_count.
  // The edges and faces are sorted radially and all faces are quads.
  //
  // In all other cases, m_center_vertex is null.
  const MYON_SubDVertex* m_center_vertex1 = nullptr;

  // m_face1[] is a list of the subdivision faces that subdivide the
  // original face. 
  
  // m_face1 is identical to m_center_vertex1->m_faces[]. 
  // The m_next_face pointers are set so
  // that m_face1[i]->m_next_face = m_face1[i+1].  Note that
  // m_face1[m_face1_count-1]->m_next_face may not be null.
  unsigned int m_face1_count = 0;
  const class MYON_SubDFace** m_face1 = nullptr;

  /*
  Description:
    Apply a single iteration of the subdivision algorithm to face
    and save the results in this MYON_SubDFaceNeighborhood.  If a vertex
    of the initial face is extraordinary, the limit point and normal 
    are calculated from face and saved on the subdivision.

    The resulting section of a subdivision surface is suitable for
    limit surface and limit mesh evaluation of the original face.
    The MYON_SubDFaceNeighborhood is designed to be used to calculate
    the limit surface and limit mesh for faces that have an extraordinary
    number of edges.
  */
  bool Subdivide(
    const MYON_SubDFace* face
    );

private:
  bool QuadSubdivideHelper(
    const MYON_SubDFace* face
    );

  bool ReserveCapacity(
    const MYON_SubDFace* face
    );

  MYON_SubD_FixedSizeHeap m_fsh;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDQuadNeighborhood
//

class MYON_CLASS MYON_SubDQuadNeighborhood
{
public:
  MYON_SubDQuadNeighborhood() = default;
  ~MYON_SubDQuadNeighborhood();
private:
  void Internal_Destroy(bool bReinitialize);

private:
  MYON_SubDQuadNeighborhood(const MYON_SubDQuadNeighborhood&) = delete;
  MYON_SubDQuadNeighborhood& operator=(const MYON_SubDQuadNeighborhood&) = delete;

public:
  // true if the limit surface of the center quad is a smooth bi-cubic surface with CVs
  // at the m_vertex_grid[][] locations.
  bool m_bIsCubicPatch = false;
  unsigned char m_initial_subdivision_level = 0; // Subdivsion level of the quad from the original SubD. (When the original SubD face is an n-gon, m_initial_subdivision_level=1)
  unsigned char m_current_subdivision_level = 0; // current subdivision level of contents

  // m_face_grid[1][1] is extraordinary and interpolation through the limit point
  // will eventually be required to get an approximate cubic patch or an exact
  // quad mesh vertex.
  unsigned char m_extraordinary_corner_vertex_count = 0;
  // m_bExtraordinaryCornerVertex[] = true if the corresponding corner vertex of 
  // the center quad is extraordinary.
  bool m_bExtraordinaryCornerVertex[4] = {};

  // m_bExactCubicPatchCorner[] = true if the sub 4x4 grid of subdivision points
  // for the corresponding quadrant can be used to create an exact quadrant cubic patch.
  unsigned char m_exact_quadrant_patch_count = 0;
  bool m_bExactQuadrantPatch[4] = {};

  // m_bBoundaryCrease[] = true if the corresponding m_center_edges[] is a crease
  // AND neither end vertex is a dart.
  unsigned char m_boundary_crease_count = 0;
  bool m_bBoundaryCrease[4] = {};

  // m_sharp_edge_count = number of sharp edges in the quad boundary and m_edge_grid[4][2]  
  unsigned char m_sharp_edge_count = 0;

private:
  unsigned char m_reserved[5];

public:
  const MYON_SubDVertex* m_vertex_grid[4][4] = {}; // vertex net m_quad_face corners = ([1][1], [2][1], [2][2], [1][2])

  const MYON_SubDEdge* m_edge_grid[4][2] = {};

  // m_face[][] is a 3x3 grid of faces.
  // Center face
  //   m_face[1][1] is the "central" quad face.
  // Side faces
  //   Side faces are null if the edge is a crease or has 1 == m_face_count.
  //   m_face[1][0] = the neighbor across m_center_edges[0]
  //   m_face[2][1] = the neighbor across m_center_edges[1]
  //   m_face[1][2] = the neighbor across m_center_edges[2]
  //   m_face[0][1] = the neighbor across m_center_edges[3]
  const MYON_SubDFace* m_face_grid[3][3] = {};

  // edges of center face;
  // m_center_edge[0] connects m_vertex_grid[1][1] and m_vertex_grid[2][1]
  // m_center_edge[1] connects m_vertex_grid[2][1] and m_vertex_grid[2][2]
  // m_center_edge[2] connects m_vertex_grid[2][2] and m_vertex_grid[1][2]
  // m_center_edge[3] connects m_vertex_grid[1][2] and m_vertex_grid[1][1]
  const MYON_SubDEdge* m_center_edges[4] = {}; 

  // If not null, the storage for the referenced subd information is from m_fsh.
  class MYON_SubD_FixedSizeHeap* m_fsh = nullptr;


  // level 1 subdivision control points
  double m_srf_cv1[5][5][3];

  /*
  Returns:
    The center quad face m_face[1][1].
  */
  const MYON_SubDFace* CenterQuad() const;

  const MYON_SubDVertex* CenterVertex(
    int vi
    ) const;

  static const MYON_2dex CenterVertexDex(
    int vi
    );

  /*
  Parameters:
    vertex_tag_filter - [in]
      If vertex_tag_filter is not MYON_SubDVertexTag::Unset, then the
      indexed vertex must have the specified tag.
    minimum_edge_count_filter - [in]
      The index vertex must have at least this many edges.
  Returns:
    0: m_vertex_grid[1][1] is the unique extraordinary center quad vertex
    1: m_vertex_grid[2][1] is the unique extraordinary center quad vertex
    2: m_vertex_grid[2][2] is the unique extraordinary center quad vertex
    3: m_vertex_grid[3][2] is the unique extraordinary center quad vertex
    MYON_UNSET_UINT_INDEX (otherwise)
  */
  unsigned int ExtraordinaryCenterVertexIndex(
    MYON_SubDVertexTag vertex_tag_filter,
    unsigned int minimum_edge_count_filter
  ) const;

  /*
  Parameters:
    fei - [in]
      center quad face edge index (0 to 3)
  Returns:
     A pointer to the neighbor face across m_face[1][1]->Edge(fei)
     This face will be null if the edge is a crease or has 2 != m_face_count
       m_face[0][1] = the neighbor across m_face[1][1]->Edge(0) or null if the Edge(0) is a crease.
       m_face[1][2] = the neighbor across m_face[1][1]->Edge(1) or null if the Edge(1) is a crease
       m_face[2][1] = the neighbor across m_face[1][1]->Edge(2) or null if the Edge(2) is a crease
       m_face[1][0] = the neighbor across m_face[1][1]->Edge(3) or null if the Edge(3) is a crease
  */
  const MYON_SubDFace* SideFace(
    unsigned int fei
    ) const;

  /*
  Parameters:
    fei - [in]
      center quad face vertex index (0 to 3)
  Returns:
     A pointer to the neighbor face opposite m_face[1][1]->Vertex(fvi).
     This face will be null if the vertex valence is not 4 or any of its edges are creases.
     m_face[0][0] = the neighbor diagonal from m_face[1][1]->Vertex(0)
     m_face[0][2] = the neighbor diagonal from m_face[1][1]->Vertex(1)
     m_face[2][2] = the neighbor diagonal from m_face[1][1]->Vertex(2)
     m_face[2][0] = the neighbor diagonal from m_face[1][1]->Vertex(3)
  */
  const MYON_SubDFace* CornerFace(
    unsigned int fvi
    ) const;

  bool Set(
    const MYON_SubDFace* center_quad_face
    );

  /*
  Description:
    Clear current settings so the MYON_SubDQuadNeighborhood can be reused.
  Parameters:
    subd_quad_nbd - [in/out]
      MYON_SubDQuadNeighborhood to clear.
    bRetainFixedSizeHeap - [in]
      The m_fsh heap is always reset. When bRetainFixedSizeHeap is true,
      m_fsh is not set to nullptr.
  */
  static void Clear(
    MYON_SubDQuadNeighborhood* subd_quad_nbd,
    bool bRetainFixedSizeHeap
    );

  bool IsValid() const;

  bool IsSet() const
  {
    return (nullptr != m_face_grid[1][1]);
  }

  /*
  Description:
    Get the limit surface for the entire quad
  */
  bool GetLimitSurfaceCV(
    //double srf_cv[4][4][3]
    double* srf_cv,
    unsigned int srf_cv_grid_size // 4 or 5
    ) const;

  /*
  Description:
    Get the limit sub surface exact patch for the specified corner.
  Returns:
    true when srf_cv are set to a the CVs for a bicubic uniform cubic NURBS bispan patch
  */
  bool GetLimitSubSurfaceSinglePatchCV(
    unsigned int fvi,
    double srf_cv[4][4][3]
    );


private:
  unsigned int SetLimitSubSurfaceExactCVs(
    bool bEnableApproximatePatch,
    unsigned int  quadrant_index // 0,1,2,3 sets quadrant, 4 sets all non extraordinary patches,
    );

private:
  /*
  Parameters:
    i - [in] (0 <= i < 5)      
    j - [in] (0 <= j < 5)
      srf_cv[i][j]
    unset_cv - [in]
      unset cv coordinate value
      (uses when false is returned)
    approximate_cv - [out]
      location for a cubic NURBS CV that will yield a smooth result but 
      approximate the true subdivision surface.      
  */
  bool Internal_GetApproximateCV(
    int i,
    int j,
    double unset_cv,
    double approximate_cv[3]
    ) const;

private:
  // After m_face_grid, m_vertex_grid, m_edge_grid and m_bBoundaryCrease[] are set,
  // this helper function sets the remaining information.
  void SetPatchStatus(
    const unsigned int fvi0
    );

  // SetPatchStatus() uses this helper to set m_bIsCubicPatch
  bool VertexGridIsExactCubicPatch(
    const MYON_2dex min_face_grid_dex,
    const MYON_2dex max_face_grid_dex,
    unsigned int boundary_corner_index
    ) const;

public:

  /*
  Description:
    Apply a single iteration of the built-in quad subdivision algorithm to
    this.
  */
  bool Subdivide(
    const unsigned int q0fvi,
    MYON_SubD_FixedSizeHeap& fsh,
    class MYON_SubDQuadNeighborhood* q1ft
    ) const;

  static MYON_2dex GridDex(
    unsigned int grid_size,
    unsigned int corner_index,
    unsigned int i,
    unsigned int j
    );

  static MYON_2dex DeltaDex(
    unsigned int corner_index,
    int delta_i,
    int delta_j
    );

  static bool GetSubdivisionPoint(
    const MYON_SubDVertex* vertex,
    double P1[3]
    );

  static bool GetLimitPoint(
    const MYON_SubDVertex* vertex,
    double limitP[3],
    double limitN[3]
    );

  static bool GetSubdivisionPoint(
    const MYON_SubDEdge* edge,
    double P1[3]
    );

  static bool GetSubdivisionPoint(
    const MYON_SubDFace* face,
    double P1[3]
    );
};

#if defined(MYON_COMPILING_OPENNURBS)

////////////////////////////////////////////////////////////////
//
//   Implementation of subdivision surface
//   
//   This implementation will change randomly and unpredictably.
//
////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDIncrementErrorCount() 
//
//  Appears in places where the code traps error conditions.
//  Putting a breakpoint on the line indicated below is an easy way
//  to have the debugger break on all error conditions and inspect
//  the first place something goes wrong in a complex calculation.
//
void MYON_SubDIncrementErrorCount(); // defined in opennurbs_subd.cpp
#define MYON_SUBD_ERROR(msg) (MYON_SubDIncrementErrorCount(),MYON_ERROR(msg))
#define MYON_SUBD_RETURN_ERROR(rc) (MYON_SubDIncrementErrorCount(),rc)
#define MYON_SUBD_RETURN_ERROR_MSG(msg,rc) (MYON_SubDIncrementErrorCount(),MYON_ERROR(msg),rc)

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexPtr, MYON_SubDEdgePtr, and MYON_SubDFacePtr are unsigned ints 
// that store a pointer to an MYON_SubDVertex, MYON_SubDEdge, or MYON_SubDFace
// along with a direction bit that is 0 or 1. The direction bit is used
// to indicate if the component is being referenced with its 
// natural orientation (0) or the reverse of its natural orientation (1).
//
// MYON_SubDComponentPtr is an unsigned int that stores a pointer to an 
// MYON_SubDVertex, MYON_SubDEdge, or MYON_SubDFace, the direction bit
// described above, and a type value used to indicate if the component is
// a vertex(2), edge(4), or face (6).
//
// This code assumes that the memory used to store MYON_SubDVertex, MYON_SubDEdge, 
// and MYON_SubDFace classes begins on an 8 bytes boundary (they contain doubles).
// If this assumption is false, you will get lots of crashes.
//
#define MYON_SUBD_COMPONENT_DIRECTION_MASK (0x1U)
#define MYON_SUBD_COMPONENT_TYPE_MASK (0x6U)
#define MYON_SUBD_COMPONENT_FLAGS_MASK (0x7U)
#if (8 == MYON_SIZEOF_POINTER)
#define MYON_SUBD_COMPONENT_POINTER_MASK (0xFFFFFFFFFFFFFFF8ULL)
#else
#define MYON_SUBD_COMPONENT_POINTER_MASK (0xFFFFFFF8U)
#endif

#define MYON_SUBD_COMPONENT_TYPE_VERTEX (0x2U)
#define MYON_SUBD_COMPONENT_TYPE_EDGE (0x4U)
#define MYON_SUBD_COMPONENT_TYPE_FACE (0x6U)

#define MYON_SUBD_COMPONENT_DIRECTION(p) ((p) & MYON_SUBD_COMPONENT_DIRECTION_MASK)
#define MYON_SUBD_COMPONENT_TYPE(p) ((p) & MYON_SUBD_COMPONENT_TYPE_MASK)
#define MYON_SUBD_COMPONENT_FLAGS(p) ((p) & MYON_SUBD_COMPONENT_FLAGS_MASK)
#define MYON_SUBD_COMPONENT_POINTER(p) ((void*)((p) & MYON_SUBD_COMPONENT_POINTER_MASK))

#define MYON_SUBD_VERTEX_DIRECTION(p) MYON_SUBD_COMPONENT_DIRECTION(p)
#define MYON_SUBD_EDGE_DIRECTION(p) MYON_SUBD_COMPONENT_DIRECTION(p)
#define MYON_SUBD_FACE_DIRECTION(p) MYON_SUBD_COMPONENT_DIRECTION(p)

#define MYON_SUBD_VERTEX_POINTER(p) ((class MYON_SubDVertex*)MYON_SUBD_COMPONENT_POINTER(p))
#define MYON_SUBD_EDGE_POINTER(p) ((class MYON_SubDEdge*)MYON_SUBD_COMPONENT_POINTER(p))
#define MYON_SUBD_FACE_POINTER(p) ((class MYON_SubDFace*)MYON_SUBD_COMPONENT_POINTER(p))

//////////////////////////////////////////////////////////////////////////
//
// m_saved_points_flags
//
#define MYON_SUBD_CACHE_POINT_FLAG_BIT MYON_SubDComponentBase::SavedPointsFlags::SubdivisionPointBit
#define MYON_SUBD_CACHE_LIMITLOC_FLAG_BIT MYON_SubDComponentBase::SavedPointsFlags::SurfacePointBit
#define MYON_SUBD_CACHE_FLAGS_MASK MYON_SubDComponentBase::SavedPointsFlags::CachedPointMask

#define MYON_SUBD_CACHE_FLAGS(cache_subd_flags) (MYON_SUBD_CACHE_FLAGS_MASK&(cache_subd_flags))
#define MYON_SUBD_CACHE_POINT_FLAG(cache_subd_flags) (MYON_SUBD_CACHE_POINT_FLAG_BIT&(cache_subd_flags))
#define MYON_SUBD_CACHE_LIMITLOC_FLAG(cache_subd_flags) (MYON_SUBD_CACHE_LIMITLOC_FLAG_BIT&(cache_subd_flags))

#define MYON_SUBD_CACHE_CLEAR_POINT_FLAG(cache_subd_flags) (cache_subd_flags &= (MYON_SUBD_CACHE_LIMITLOC_FLAG_BIT|MYON_SubDComponentBase::ModifiedFlags::ModifiedFlagsMask))
#define MYON_SUBD_CACHE_CLEAR_LIMITLOC_FLAG(cache_subd_flags) (cache_subd_flags &= (MYON_SUBD_CACHE_POINT_FLAG_BIT|MYON_SubDComponentBase::ModifiedFlags::ModifiedFlagsMask))


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDAggregates
//
class MYON_SubDAggregates
{
public:
  void UpdateBoundingBox(
    const MYON_SubDLevel* level
    );

  void UpdateAggregateEdgeAttributes(
    const MYON_SubDLevel* level
    );

  void UpdateAggregateComponentStatus(
    const MYON_SubDLevel* level
    );

  void UpdateTopologicalAttributes(
    const MYON_SubDLevel* level
  );

  /*
  Description:
    Get saved topological attributes.
  Parameters;
    bIsManifold - [out]
    bIsOriented - [out]
    bHasBoundary - [out]
    solid_orientation - [out]
      +2     subd is a solid but orientation cannot be computed
      +1     subd is a solid with outward facing normals
      -1     subd is a solid with inward facing normals
       0     subd is not a solid
  Returns:
    True if global topology attributes were set and retrieved.
    False if global topology attributes are not available and need to be updated.
  */
  bool GetTopologicalAttributes(
    bool& bIsManifold,
    bool& bIsOriented,
    bool& bHasBoundary,
    int& solid_orientation
  ) const;

  bool GetTopologicalAttributes(
    const MYON_SubDLevel* level,
    bool& bIsManifold,
    bool& bIsOriented,
    bool& bHasBoundary,
    int& solid_orientation
  );

  void ClearTopologicalAttributes()
  {
    m_topological_attributes = 0;
  }

  void MarkAllAsNotCurrent()
  {
    m_bDirtyEdgeAttributes = true;
    m_bDirtyBoundingBox = true;
    m_aggregate_status.MarkAsNotCurrent();
    ClearTopologicalAttributes();
  }

  bool m_bDirtyEdgeAttributes = false;
  bool m_bDirtyBoundingBox = false;
  MYON_AggregateComponentStatusEx m_aggregate_status = MYON_AggregateComponentStatusEx::Empty;
  unsigned int m_aggregate_edge_attributes = 0;

private:
  // If m_topological_attributes is nonzero, then the topogical attributes are set. 
  // Otherwise the topogical attributes are unknown and 
  // UpdateTopologicalAttributes() or GetTopologicalAttributes(level, ...)
  // must be called.
  unsigned int m_topological_attributes = 0;

public:
  //MYON_BoundingBox m_surface_bbox = MYON_BoundingBox::EmptyBoundingBox;
  MYON_BoundingBox m_controlnet_bbox = MYON_BoundingBox::EmptyBoundingBox;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentBaseLink
//
class MYON_SubDComponentBaseLink : public MYON_SubDComponentBase
{
public:
  const MYON_SubDComponentBaseLink* m_prev; // m_prev_vertex / m_prev_edge / m_prev_face
  const MYON_SubDComponentBaseLink* m_next; // m_next_vertex / m_next_edge / m_next_face

  static int CompareId(MYON_SubDComponentBaseLink const*const* lhs, MYON_SubDComponentBaseLink const*const* rhs);
private:
  // no instantiations permitted
  MYON_SubDComponentBaseLink() = delete;
  ~MYON_SubDComponentBaseLink() = delete;
  MYON_SubDComponentBaseLink(const MYON_SubDComponentBaseLink&) = delete;
  MYON_SubDComponentBaseLink& operator=(const MYON_SubDComponentBaseLink&) = delete;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDLevel
//

class MYON_SubDLevel
{
public:
  static const MYON_SubDLevel Empty;

  bool IsEmpty() const;

  MYON_SubDLevel()
  {
    m_vertex[0] = m_vertex[1] = nullptr;
    m_edge[0] = m_edge[1] = nullptr;
    m_face[0] = m_face[1] = nullptr;
  }


  unsigned int DumpTopology(
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
  ) const;

private:
  MYON_SubDLevel(const MYON_SubDLevel&) = delete;
  MYON_SubDLevel& operator=(const MYON_SubDLevel&) = delete;

public:
  unsigned int m_level_index = MYON_UNSET_UINT_INDEX;

  // temporary
  enum : unsigned char
  {
    m_ordinary_vertex_valence = 4
  };
  enum : unsigned char
  {
    m_ordinary_face_edge_count = 4
  };

private:
  unsigned int m_reserved1 = 0;

public:
  bool CopyHelper(
    const class MYON_SubDimple& src_subdimple,
    const MYON_SubDLevel& src_level, 
    class MYON_SubDArchiveIdMap& archive_id_map,
    class MYON_SubDimple& dest_subdimple,
    bool bCopyComponentStatus
    );

public:
  /*
  Returns:
    Number of changes to vertex tag, edge tag and edge sector coefficient values.
  */
  unsigned int UpdateEdgeTags(
    bool bUnsetEdgeTagsOnly
    );

  unsigned int UpdateVertexTags(
    bool bVertexEdgeTagsOnly
    );

  unsigned int UpdateAllTagsAndSectorCoefficients(
    bool bUnsetValuesOnly
    );

  unsigned int ClearComponentDamagedState() const;

  unsigned int ClearComponentDamagedState(
    bool bClearVerticesDamagedState,
    bool bClearEdgesDamagedState,
    bool bClearFacesDamagedState
  ) const;

private:
  void DestroyOnError()
  {
    MYON_SubDIncrementErrorCount();
    m_vertex_count = 0;
    m_edge_count = 0;
    m_face_count = 0;
    m_vertex_array_count = 0;
    m_edge_array_count = 0;
    m_face_array_count = 0;    m_vertex[0] = m_vertex[1] = nullptr;
    m_edge[0] = m_edge[1] = nullptr;
    m_face[0] = m_face[1] = nullptr;
    ResetVertexArray();
    ResetEdgeArray();
    ResetFaceArray();
  }

public:

  // m_vertex[2] = (head,tail) of linked list of vertices for this level
  class MYON_SubDVertex* m_vertex[2];

  // m_edge[2] = (head,tail) of linked list of edges for this level
  class MYON_SubDEdge* m_edge[2];

  // m_face[2] = (head,tail) of linked list of faces for this level
  class MYON_SubDFace* m_face[2];

  unsigned int m_vertex_count = 0;
  unsigned int m_edge_count = 0;
  unsigned int m_face_count = 0;
  unsigned int m_vertex_array_count = 0;
  unsigned int m_edge_array_count = 0;
  unsigned int m_face_array_count = 0;

  std::shared_ptr<const MYON_SubDVertex*> VertexArray() const;
  std::shared_ptr<const MYON_SubDEdge*> EdgeArray() const;
  std::shared_ptr<const MYON_SubDFace*> FaceArray() const;

  mutable MYON_SubDMesh m_surface_mesh;
  mutable MYON_SubDMesh m_control_net_mesh;

  /*
  Description:
    Sets the mutable m_archive_id value for every vertex, edge and face 
    in this level.
  Parameters:
    subdimple - [in]
      The subdimple is used to generate id iterators 
      from the vertex/edge,face heaps when the ids are not 
      strictly increasing in the linked lists on the level.
    archive_id_partition - [out]
      archive_id_partition[0] = 1 = first vertex archive_id
      archive_id_partition[1]-1 = last vertex archive_id
      archive_id_partition[1] = first edge archive_id
      archive_id_partition[2]-1 = last edge archive_id
      archive_id_partition[2] = first face archive_id
      archive_id_partition[3]-1 = last face archive_id
    bLevelLinkedListIncreasingId - [out]
      
      bLevelLinkedListIncreasingId[0] = true if the value
      of MYON_SubDVertex.m_id increases throughout the level's
      linked list beginning with m_vertex[0]. False otherwise
      in which case slower use of the subdimple.m_heap.m_fspv
      must be used to iterate over the level's vertices in
      increasing MYON_SubDVertex.m_id order.
      
      bLevelLinkedListIncreasingId[1] m_edge[0] linked list id order.

      bLevelLinkedListIncreasingId[2] m_face[0] linked list id order.
      
  Returns:
    The number of vertices, edges and faces on this level.
  */
  unsigned int SetArchiveId(
    const MYON_SubDimple& subdimple,
    unsigned int archive_id_partition[4],
    bool bLevelLinkedListIncreasingId[3]
    ) const;

  void ClearArchiveId() const;

  bool Read(
    MYON_BinaryArchive& archive,
    class MYON_SubDArchiveIdMap& element_list,
    MYON_SubD& subd
    );

  bool Write(
    const MYON_SubDimple& subdimple,
    MYON_BinaryArchive& archive
    ) const;

  /*
  Description:
    Apply a tranxfomration matrix to the entire level.
  Parameters:
    bGlobalTransformationIsIsometricOrUniformScale - [in]
      true if the transformation is isomectirc (rotation, translation),
      a dialoation (uniforma scale), or a compbination of those, and
      it will be applied to every component in the subdivision
      object.
      false otherwise.  
      If you don't know, pass false.
    xform - [in]
  */
  bool Transform(
    bool bGlobalTransformationIsIsometricOrDilation,
    const class MYON_Xform& xform
    );

  void ResetVertexArray()
  {
    if (m_vertex_array_count)
    {
      m_vertex_array_count = 0;
      m_vertex_array.reset();
    }
  }

  void ResetEdgeArray()
  {
    if (m_edge_array_count)
    {
      m_edge_array_count = 0;
      m_edge_array.reset();
    }
  }

  void ResetFaceArray()
  {
    if (m_face_array_count)
    {
      m_face_array_count = 0;
      m_face_array.reset();
    }
  }

  std::shared_ptr<const MYON_SubDVertex*> m_vertex_array;
  std::shared_ptr<const MYON_SubDEdge*> m_edge_array;
  std::shared_ptr<const MYON_SubDFace*> m_face_array;

  bool RemoveVertex(class MYON_SubDVertex* vertex)
  {
    m_aggregates.m_bDirtyBoundingBox = true;
    if (nullptr == vertex || vertex->SubdivisionLevel() != this->m_level_index )
      return MYON_SUBD_RETURN_ERROR(false);
    if ( 0 == m_vertex_count)
      return MYON_SUBD_RETURN_ERROR(false);
    MYON_SubDVertex* prev_vertex = const_cast<MYON_SubDVertex*>(vertex->m_prev_vertex);
    MYON_SubDVertex* next_vertex = const_cast<MYON_SubDVertex*>(vertex->m_next_vertex);
    vertex->m_prev_vertex = nullptr;
    vertex->m_next_vertex = nullptr;
    if (1 == m_vertex_count)
    {
      if (m_vertex[0] == vertex && m_vertex[1] == vertex && nullptr == prev_vertex && nullptr == next_vertex)
      {
        m_vertex[0] = nullptr;
        m_vertex[1] = nullptr;
      }
      else
      {
        // error - same action taken
        DestroyOnError();
        return false;
      }
    }
    else
    {
      if (m_vertex[0] == vertex)
      {
        if (m_vertex_count >= 2 && nullptr == prev_vertex && nullptr != next_vertex )
        {
          m_vertex[0] = next_vertex;
          next_vertex->m_prev_vertex = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else if (m_vertex[1] == vertex)
      {
        if (m_vertex_count >= 2 && nullptr == next_vertex && nullptr != prev_vertex)
        {
          m_vertex[1] = prev_vertex;
          prev_vertex->m_next_vertex = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else
      {
        if (m_vertex_count > 2 && nullptr != prev_vertex && nullptr != next_vertex)
        {
          prev_vertex->m_next_vertex = next_vertex;
          next_vertex->m_prev_vertex = prev_vertex;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
    }
    m_vertex_count--;
    ResetVertexArray();
    return true;
  }


  bool RemoveEdge(class MYON_SubDEdge* edge)
  {
    m_aggregates.m_bDirtyEdgeAttributes = true;
    if (nullptr == edge || edge->SubdivisionLevel() != this->m_level_index )
      return MYON_SUBD_RETURN_ERROR(false);
    if ( 0 == m_edge_count)
      return MYON_SUBD_RETURN_ERROR(false);
    MYON_SubDEdge* prev_edge = const_cast<MYON_SubDEdge*>(edge->m_prev_edge);
    MYON_SubDEdge* next_edge = const_cast<MYON_SubDEdge*>(edge->m_next_edge);
    edge->m_prev_edge = nullptr;
    edge->m_next_edge = nullptr;
    if (1 == m_edge_count)
    {
      if (m_edge[0] == edge && m_edge[1] == edge && nullptr == prev_edge && nullptr == next_edge)
      {
        m_edge[0] = nullptr;
        m_edge[1] = nullptr;
      }
      else
      {
        // error - same action taken
        DestroyOnError();
        return false;
      }
    }
    else
    {
      if (m_edge[0] == edge)
      {
        if (m_edge_count >= 2 && nullptr == prev_edge && nullptr != next_edge )
        {
          m_edge[0] = next_edge;
          next_edge->m_prev_edge = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else if (m_edge[1] == edge)
      {
        if (m_edge_count >= 2 && nullptr == next_edge && nullptr != prev_edge)
        {
          m_edge[1] = prev_edge;
          prev_edge->m_next_edge = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else
      {
        if (m_edge_count > 2 && nullptr != prev_edge && nullptr != next_edge)
        {
          prev_edge->m_next_edge = next_edge;
          next_edge->m_prev_edge = prev_edge;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
    }
    m_edge_count--;
    ResetEdgeArray();
    return true;
  }


  bool RemoveFace(class MYON_SubDFace* face)
  {
    if (nullptr == face || face->SubdivisionLevel() != this->m_level_index )
      return MYON_SUBD_RETURN_ERROR(false);
    if ( 0 == m_face_count)
      return MYON_SUBD_RETURN_ERROR(false);
    MYON_SubDFace* prev_face = const_cast<MYON_SubDFace*>(face->m_prev_face);
    MYON_SubDFace* next_face = const_cast<MYON_SubDFace*>(face->m_next_face);
    face->m_prev_face = nullptr;
    face->m_next_face = nullptr;
    if (1 == m_face_count)
    {
      if (m_face[0] == face && m_face[1] == face && nullptr == prev_face && nullptr == next_face)
      {
        m_face[0] = nullptr;
        m_face[1] = nullptr;
      }
      else
      {
        // error - same action taken
        DestroyOnError();
        return false;
      }
    }
    else
    {
      if (m_face[0] == face)
      {
        if (m_face_count >= 2 && nullptr == prev_face && nullptr != next_face )
        {
          m_face[0] = next_face;
          next_face->m_prev_face = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else if (m_face[1] == face)
      {
        if (m_face_count >= 2 && nullptr == next_face && nullptr != prev_face)
        {
          m_face[1] = prev_face;
          prev_face->m_next_face = nullptr;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
      else
      {
        if (m_face_count > 2 && nullptr != prev_face && nullptr != next_face)
        {
          prev_face->m_next_face = next_face;
          next_face->m_prev_face = prev_face;
        }
        else
        {
          DestroyOnError();
          return false;
        }
      }
    }
    m_face_count--;
    ResetFaceArray();
    return true;
  }

  const MYON_SubDVertex* AddVertex(class MYON_SubDVertex* vertex)
  {
    m_aggregates.m_bDirtyBoundingBox = true;
    if (nullptr == vertex)
      return nullptr;
    if (nullptr == m_vertex[1])
    {
      m_vertex[0] = vertex;
      vertex->m_prev_vertex = nullptr;
    }
    else
    {
      m_vertex[1]->m_next_vertex = vertex;
      vertex->m_prev_vertex = m_vertex[1];
    }
    m_vertex[1] = vertex;
    vertex->m_next_vertex = nullptr;
    m_vertex_count++;
    ResetVertexArray();
    return vertex;
  }

  const MYON_SubDEdge* AddEdge(class MYON_SubDEdge* edge)
  {
    m_aggregates.m_bDirtyEdgeAttributes = true;
    if (nullptr == edge)
      return nullptr;
    if (nullptr == m_edge[1])
    {
      m_edge[0] = edge;
      edge->m_prev_edge = nullptr;
    }
    else
    {
      m_edge[1]->m_next_edge = edge;
      edge->m_prev_edge = m_edge[1];
    }
    m_edge[1] = edge;
    edge->m_next_edge = nullptr;
    m_edge_count++;
    ResetEdgeArray();
    return edge;
  }

  const MYON_SubDFace* AddFace(class MYON_SubDFace* face)
  {
    if (nullptr == face)
      return nullptr;
    if (nullptr == m_face[1])
    {
      m_face[0] = face;
      face->m_prev_face = nullptr;
    }
    else
    {
      m_face[1]->m_next_face = face;
      face->m_prev_face = m_face[1];
    }
    m_face[1] = face;
    face->m_next_face = nullptr;
    m_face_count++;
    ResetFaceArray();
    return face;
  }

private:
  unsigned int m_reserved4 = 0;
public:

  unsigned int UpdateEdgeSectorCoefficients(
    bool bUnsetEdgeSectorCoefficientsOnly
    ) const;

  /*
  Description:
    If a state is set in the status parameter, it will be cleared
    from all components on the level.
  Returns:
    Number of components where a state setting changed.
  */
  unsigned int ClearStates(
    MYON_ComponentStatus states_to_clear
    ) const;

  /*
  Description:
    Clears all runtime marks for components on this level.
  Returns:
    Number of components where runtime mark was changed.
  */
  unsigned int ClearRuntimeMarks(
    bool bClearVertexMarks,
    bool bClearEdgeMarks,
    bool bClearFaceMarks
    ) const;

  unsigned int GetComponentsWithSetStates(
    MYON_ComponentStatus states_filter,
    bool bAllEqualStates,
    MYON_SimpleArray< MYON_SubDComponentPtr >& components_with_set_states
    ) const;

  unsigned int SetStates(
    MYON_SubDComponentPtr component_ptr,
    MYON_ComponentStatus states_to_set
    ) const;

  unsigned int ClearStates(
    MYON_SubDComponentPtr component_ptr,
    MYON_ComponentStatus states_to_clear
    ) const;

  unsigned int SetStatus(
    MYON_SubDComponentPtr component_ptr,
    MYON_ComponentStatus status_to_copy
    ) const;

  const MYON_AggregateComponentStatusEx AggregateComponentStatus() const;

  MYON__UINT64 ComponentStatusSerialNumber() const;

  void MarkAggregateComponentStatusAsNotCurrent() const
  {
    m_aggregates.m_aggregate_status.MarkAsNotCurrent();
  }

  void ClearBoundingBox() const
  {
    m_aggregates.m_bDirtyBoundingBox = true;
  }

  void ClearEvaluationCache() const;

  bool CopyEvaluationCacheForExperts(class MYON_SubDHeap& this_heap, const MYON_SubDLevel& src, const class MYON_SubDHeap& src_heap);
    
  void ClearTopologicalAttributes() const
  {
    m_aggregates.ClearTopologicalAttributes();
  }

  /*
  Description:
    Get saved topological attributes.
  Parameters;
    bIsManifold - [out]
    bIsOriented - [out]
    bHasBoundary - [out]
    solid_orientation - [out]
      +2     subd is a solid but orientation cannot be computed
      +1     subd is a solid with outward facing normals
      -1     subd is a solid with inward facing normals
       0     subd is not a solid
  Returns:
    True if global topology attributes were set and retrieved.
    False if global topology attributes are not available and need to be updated.
  */
  void GetTopologicalAttributes(
    bool& bIsManifold,
    bool& bIsOriented,
    bool& bHasBoundary,
    int& solid_orientation
  ) const
  {
    m_aggregates.GetTopologicalAttributes(this, bIsManifold, bIsOriented, bHasBoundary, solid_orientation);
  }

  MYON_BoundingBox ControlNetBoundingBox() const;

  void ClearEdgeFlags() const
  {
    m_aggregates.m_bDirtyEdgeAttributes = true;
  }

  unsigned int EdgeFlags() const;


private:
  mutable MYON_SubDAggregates m_aggregates;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDLevelIterator
//
class MYON_SubDLevelIterator
{
public:
  class MYON_SubDLevel* First()
  {
    for (m_level_index = 0; m_level_index < m_level_count; m_level_index++)
    {
      if (nullptr != m_levels[m_level_index])
        return m_levels[m_level_index];
    }
    return nullptr;    
  }

  class MYON_SubDLevel* Next()
  {
    for (m_level_index++; m_level_index < m_level_count; m_level_index++)
    {
      if (nullptr != m_levels[m_level_index])
        return m_levels[m_level_index];
    }
    return nullptr;    
  }

  unsigned int LevelIndex()
  {
    return m_level_index;
  }

  unsigned int LevelCount()
  {
    return m_level_count;
  }

  friend class MYON_SubDimple;
  MYON_SubDLevelIterator() = default;
  ~MYON_SubDLevelIterator() = default;
  MYON_SubDLevelIterator(const MYON_SubDLevelIterator&) = default;
  MYON_SubDLevelIterator& operator=(const MYON_SubDLevelIterator&) = default;

private:
  class MYON_SubDLevel** m_levels = nullptr;
  unsigned int m_level_count = 0;
  unsigned int m_level_index = 0;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDHeap
//

class MYON_SubDHeap
{
public:
  MYON_SubDHeap();
  ~MYON_SubDHeap();

private:
  static class MYON_SubDComponentBase* Internal_AllocateComponentAndSetId(
    MYON_FixedSizePool& fspc,
    MYON_SubDComponentBase*& unused_list,
    unsigned int& max_id,
    unsigned int candidate_id
  );

public:
  class MYON_SubDVertex* AllocateVertexAndSetId(unsigned int candidate_vertex_id);
  void ReturnVertex(class MYON_SubDVertex* v);

  class MYON_SubDEdge* AllocateEdgeAndSetId(unsigned int candidate_edge_id);
  void ReturnEdge(class MYON_SubDEdge* e);

  class MYON_SubDFace* AllocateFaceAndSetId(unsigned int candidate_face_id);
  void ReturnFace(class MYON_SubDFace* f);

  /*
  Description:
    Sets mutable m_archive_id values to 0.
  */
  void ClearArchiveId();

  const class MYON_SubDVertex* VertexFromId(
    unsigned int vertex_id
    ) const;

  const class MYON_SubDEdge* EdgeFromId(
    unsigned int edge_id
    ) const;

  const class MYON_SubDFace* FaceFromId(
    unsigned int face_id
    ) const;

  /*
  Returns:
    Maximum vertex id assigned to a vertex managed by this heap.
  Remarks:
    The next vertex that is created (not reused from m_unused_vertex) will
    be assigned id = MaximumVertexId() + 1;
  */
  unsigned int MaximumVertexId() const
  {
    return m_max_vertex_id;
  }

  /*
  Returns:
    Maximum edge id assigned to an edge managed by this heap.
  Remarks:
    The next edge that is created (not reused from m_unused_edge) will
    be assigned id = MaximumEdgeId() + 1;
  */
  unsigned int MaximumEdgeId() const
  {
    return m_max_edge_id;
  }

  /*
  Returns:
    Maximum face id assigned to a face managed by this heap.
  Remarks:
    The next face that is created (not reused from m_unused_face) will
    be assigned id = MaximumFaceId() + 1;
  */
  unsigned int MaximumFaceId() const
  {
    return m_max_face_id;
  }

public:
  /*
  Returns:
    True if this heap and the ids are valid.
  Remarks:
    If false is returned, ResetIds() generally will make the heap valid.
  */
  bool IsValid(
    bool bSilentError,
    MYON_TextLog* text_log
  ) const;

  /*
  Description:
     Resets all ids to begin with 1. 
     This breaks persistent id references, but will restore ability for the heap to work correctly.
  */
  void ResetIds();

private:
  /*
  Returns:
  Array of count MYON__UINT_PTR
  */
  MYON__UINT_PTR* AllocateArray(
    size_t* capacity
    );
  void ReturnArray(
    size_t capacity,
    MYON__UINT_PTR* a
    );
  static MYON__UINT_PTR ArrayCapacity(
    size_t capacity,
    MYON__UINT_PTR* a
    );
  MYON__UINT_PTR* ResizeArray(
    size_t current_count,
    size_t current_capacity,
    MYON__UINT_PTR* current_a,
    size_t* new_capacity
    );

public:
  bool GrowVertexEdgeArray(
    MYON_SubDVertex* v,
    size_t capacity
    );
  bool GrowVertexFaceArray(
    MYON_SubDVertex* v,
    size_t capacity
    );
  bool GrowEdgeFaceArray(
    MYON_SubDEdge* e,
    size_t capacity
    );
  bool GrowFaceEdgeArray(
    MYON_SubDFace* f,
    size_t capacity
    );

  bool GrowVertexEdgeArrayByOne(
    MYON_SubDVertex* v
    );
  bool GrowVertexFaceArrayByOne(
    MYON_SubDVertex* v
    );
  bool GrowEdgeFaceArrayByOne(
    MYON_SubDEdge* e
    );
  bool GrowFaceEdgeArrayByOne(
    MYON_SubDFace* f
    );

  bool ReturnVertexEdgeAndFaceArrays(
    MYON_SubDVertex* v
    );

  bool ReturnEdgeExtraArray(
    MYON_SubDEdge* e
    );

  bool ReturnFaceExtraArray(
    MYON_SubDFace* f
    );

  /*
  Description:
    Allocates fragments reference by MYON_SubDFace.m_limit_mesh_fragments
    for faces in this SubD. The fragments have to be 16x16 or 8x8.
  Parameters:
    src_fragment - [in]
      This fragment is copied to the returned fragment.
      The fragment grid must be 16x16 or 8x8.
  */
  class MYON_SubDMeshFragment* AllocateMeshFragment(
    unsigned subd_display_density,
    const class MYON_SubDMeshFragment& src_fragment
  );

  class MYON_SubDMeshFragment* CopyMeshFragments(
    const class MYON_SubDFace* source_face,
    unsigned destination_subd_display_density,
    const class MYON_SubDFace* destination_face
  );

  /*
  Parameters:
    fragment - [in]
      A pointer to a no longer used fragment that
      came from AllocateMeshFragment().
  */
  bool ReturnMeshFragment(
    class MYON_SubDMeshFragment* fragment
  );

  /*
  Parameters:
    face - [in]
      A face in this subd.
  */
  bool ReturnMeshFragments(
    const class MYON_SubDFace* face
  );


  /*
  Description:
    Allocates limit curves reference by MYON_SubDEdge.m_limit_curve
    for edges in this SubD.
  Parameters:
    cv_capacity - [in]
      desired cv_capacity <= MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity
  */
  class MYON_SubDEdgeSurfaceCurve* AllocateEdgeSurfaceCurve(
    unsigned int cv_capacity
  );

  class MYON_SubDEdgeSurfaceCurve* CopyEdgeSurfaceCurve(
    const class MYON_SubDEdge* source_edge,
    const class MYON_SubDEdge* desination_edge
  );

  /*
  Parameters:
    limit_curve - [in]
      A pointer to a no longer used limit_curve that
      came from AllocateEdgeSurfaceCurve().
  */
  bool ReturnEdgeSurfaceCurve(
    class MYON_SubDEdgeSurfaceCurve* limit_curve
  );

  /*
  Parameters:
    edge - [in]
      An edge in this subd.
  */
  bool ReturnEdgeSurfaceCurve(
    const class MYON_SubDEdge* edge
  );

  unsigned int AllocateFaceTexturePoints(
    const class MYON_SubDFace* face
  );

  void ReturnFaceTexturePoints(
    const class MYON_SubDFace* face
  );

  /*
  Description:
    Discard all contents of this MYON_SubDHeap.
  Remarks:
    More efficient than Destroy() if this MYON_SubDHeap will be reused soon.
  */
  void Clear();

  /*
  Description:
    Delete all contents release all memory used by this MYON_SubDHeap.
  */
  void Destroy();

  size_t SizeOf() const
  {
    size_t sz = sizeof(*this);

    sz += SizeOfAllPools();

    // For vertices with multiple sectors and multiple limit points,
    // additional limit points come from a global limit point pool.
    // In any given subd, this is a small minority of vertices and
    // this undercounting of memory is a tiny compared with the
    // pool sizes counted above.

    // For vertices with more than 17 faces or more than 17 edges,
    // the MYON_SubDVertex.m_faces[] and MYON_SubDVertex.m_edges[] arrays
    // are individually allocated. This is not common and the undercount
    // is tiny compared with the pool sizes counted above.

    // For non-manifold edges with more than 17 faces,
    // the MYON_SubDEdge.m_faces[] array is individually allocated. 
    // This is extremely uncommon and the undercount
    // is tiny compared with the pool sizes counted above.

    // For faces with more than 17 edges,
    // the MYON_SubDFace.m_edges[] array is individually allocated. 
    // This is uncommon and the undercount
    // is tiny compared with the pool sizes counted above.

    // Add this fudge factor typically more than accounts for 
    // the uncommon and minimal undercounts that are described above.
    sz += 8 * (m_fspv.ActiveElementCount() + m_fspe.ActiveElementCount() + m_fspf.ActiveElementCount());

    return sz;
  }

  size_t SizeOfAllPools() const
  {
    size_t sz = 0;
    sz += m_fspv.SizeOfPool();
    sz += m_fspe.SizeOfPool();
    sz += m_fspf.SizeOfPool();
    sz += m_fsp5.SizeOfPool();
    sz += m_fsp9.SizeOfPool();
    sz += m_fsp17.SizeOfPool();
    sz += m_fsp_full_fragments.SizeOfPool();
    sz += m_fsp_part_fragments.SizeOfPool();
    sz += m_fsp_oddball_fragments.SizeOfPool();
    sz += m_fsp_limit_curves.SizeOfPool();
    return sz;
  }

  size_t SizeOfUnusedPoolElements() const
  {
    size_t sz = 0;
    sz += m_fspv.SizeOfUnusedElements();
    sz += m_fspe.SizeOfUnusedElements();
    sz += m_fspf.SizeOfUnusedElements();
    sz += m_fsp5.SizeOfUnusedElements();
    sz += m_fsp9.SizeOfUnusedElements();
    sz += m_fsp17.SizeOfUnusedElements();
    sz += m_fsp_full_fragments.SizeOfUnusedElements();
    sz += m_fsp_part_fragments.SizeOfUnusedElements();
    sz += m_fsp_oddball_fragments.SizeOfUnusedElements();
    sz += m_fsp_limit_curves.SizeOfUnusedElements();
    return sz;
  }

  size_t SizeOfActivePoolElements() const
  {
    size_t sz = 0;
    sz += m_fspv.SizeOfActiveElements();
    sz += m_fspe.SizeOfActiveElements();
    sz += m_fspf.SizeOfActiveElements();
    sz += m_fsp5.SizeOfActiveElements();
    sz += m_fsp9.SizeOfActiveElements();
    sz += m_fsp17.SizeOfActiveElements();
    sz += m_fsp_full_fragments.SizeOfActiveElements();
    sz += m_fsp_part_fragments.SizeOfActiveElements();
    sz += m_fsp_oddball_fragments.SizeOfActiveElements();
    sz += m_fsp_limit_curves.SizeOfActiveElements();
    return sz;
  }

  /*
  Description:
    Tool for debugging mesh fragments memory use.
  Returns:
    Total operating system heap memory (in bytes) used by the mesh fragments pool.
  Remarks:
    SizeOfMeshFragmentPool() = SizeOfActiveMeshFragments() + SizeOfUnusedMeshFragments().
  */
  size_t SizeOfMeshFragmentsPool() const
  {
    return 
      m_fsp_full_fragments.SizeOfPool() 
      + m_fsp_part_fragments.SizeOfPool()
      + m_fsp_oddball_fragments.SizeOfPool()
      + m_fsp_limit_curves.SizeOfPool();
  }

  /*
  Description:
    Tool for debugging mesh fragments memory use.
  Returns:
    Operating system heap memory (in bytes) that are used by active mesh fragments.
  Remarks:
    SizeOfMeshFragmentPool() = SizeOfActiveMeshFragments() + SizeOfUnusedMeshFragments().
  */
  size_t SizeOfActiveMeshFragments() const
  {
    return SizeOfMeshFragmentsPool() - SizeOfUnusedMeshFragments();
  }

  /*
  Description:
    Tool for debugging mesh fragments memory use.
  Returns:
    Operating system heap memory (in bytes) that has been reserved for mesh fragments 
    but is not currently used by active mesh fragments.
  Remarks:
    SizeOfMeshFragmentPool() = SizeOfActiveMeshFragments() + SizeOfUnusedMeshFragments().
  */
  size_t SizeOfUnusedMeshFragments() const
  {
    size_t sz 
      = m_fsp_full_fragments.SizeOfUnusedElements()
      + m_fsp_part_fragments.SizeOfUnusedElements()
      + m_fsp_oddball_fragments.SizeOfUnusedElements()
      + m_fsp_limit_curves.SizeOfUnusedElements();

    // It has always been the case that count0 = count1 = MYON_SubDDisplayParameters::MaximumDensity + 1.
    // But a wrong answer is better than crashing if somebody incorrectly modifies MYON_SubDHeap 
    // in the far future.
    const size_t count0 = sizeof(m_unused_fragments) / sizeof(m_unused_fragments[0]);
    const size_t count1 = sizeof(MYON_SubDHeap::g_sizeof_fragment) / sizeof(MYON_SubDHeap::g_sizeof_fragment[0]);
    for (size_t i = 0; i < count0 && i < count1; ++i)
    {
      const size_t sizeof_fragment = MYON_SubDHeap::g_sizeof_fragment[i];
      // The memory for these fragments is managed by m_fsp_full_fragments or m_fsp_part_fragments.
      for (const MYON_FixedSizePoolElement* e = m_unused_fragments[i]; nullptr != e; e = e->m_next)
        sz += sizeof_fragment;
    }

    return sz;
  }


  bool InHeap(MYON_SubDComponentPtr cptr) const;

  const MYON_SubDComponentPtr InHeap(const class MYON_SubDComponentBase* b) const;

private:
  class tagWSItem
  {
  public:
    class tagWSItem* m_prev;
    class tagWSItem* m_next;
  };

  class tagWSItem* m_ws = nullptr;   // oversized arrays of MYON__UINT_PTRs

private:
  MYON_FixedSizePool m_fspv;  // element = MYON_SubDVertex
public:
  void InitializeVertexIdIterator(
    class MYON_SubDVertexIdIterator& vidit
  ) const;

private:
  MYON_FixedSizePool m_fspe;  // element = MYON_SubDEdge
public:
  void InitializeEdgeIdIterator(
    class MYON_SubDEdgeIdIterator& vidit
  ) const;

private:
  MYON_FixedSizePool m_fspf;  // element = MYON_SubDFace
public:
  void InitializeFaceIdIterator(
    class MYON_SubDFaceIdIterator& vidit
  ) const;

public:
  void InitializeComponentIdIterator(
    MYON_SubDComponentPtr::Type ctype,
    class MYON_SubDComponentIdIterator& cidit
  ) const;

private:
  MYON_FixedSizePool m_fsp5;  // element = capacity + array of 4 MYON__UINT_PTRs
  MYON_FixedSizePool m_fsp9;  // element = capacity + array of 8 MYON__UINT_PTRs
  MYON_FixedSizePool m_fsp17; // element = capacity + array of 16 MYON__UINT_PTRs

  // g_sizeof_fragment[i] = sizeof of a fragment NxN mesh quads where N = 2^i
  // g_sizeof_fragment[0] = sizeof of a fragment with 1x1 mesh quads. (quad fragment for density = 0)
  // g_sizeof_fragment[1] = sizeof of a fragment with 2x2 mesh quads. (quad fragment for density = 1)
  // g_sizeof_fragment[2] = sizeof of a fragment with 4x4 mesh quads. (quad fragment for density = 2)
  // g_sizeof_fragment[3] = sizeof of a fragment with 8x8 mesh quads. (quad fragment for density = 3)
  // g_sizeof_fragment[4] = sizeof of a fragment with 16x16 mesh quads. (quad fragment for density = 4)
  // ...
  static const size_t g_sizeof_fragment[MYON_SubDDisplayParameters::MaximumDensity + 1];


  /*
  Sets m_full_fragment_display_density, m_full_fragment_count_estimate, m_part_fragment_count_estimate.
  */
  bool Internal_InitializeFragmentCountEstimates(
    unsigned subd_display_density
  );

  // m_full_fragment_display_density = display density for a full fragment
  unsigned int m_full_fragment_display_density = 0;

  // m_full_fragment_count_estimate = an estimate of the total number of full fragments
  // needed. It's ok if we need additionally fragments later.
  unsigned int m_full_fragment_count_estimate = 0;

  // m_full_fragment_count_estimate = an estimate of the total number of full fragments
  // needed. It's ok if we need additionally fragments later.
  unsigned int m_part_fragment_count_estimate = 0;

  unsigned int m_reserved0 = 0;

  /*
  Parameters:
    fragment_count_estimate - [in]
      Estimate of the number of fragments that will come from this pool.
    sizeof_fragment - [in]
      number of bytes per most common fragment (MYON_SubDMeshFragment + vertex arrays)
    max_sizeof_fragment - [in]
      number of bytes per maximum possible sized fragment (MYON_SubDMeshFragment + vertex arrays)
      from fsp.
    fsp - [in/out]
      fixed size pool to initialize.     
  */
  static bool Internal_InitializeMeshFragmentPool(
    size_t sizeof_element,
    size_t element_count_estimate,
    size_t min_2nd_block_element_count,
    MYON_FixedSizePool& fsp // fsp references either m_fsp_*_fragments or m_fsp_limit_curves.
  );

  // MYON_SubDMeshFragments with density = m_full_fragment_display_density are allocated from m_fsp_full_fragments.
  MYON_FixedSizePool m_fsp_full_fragments;

  // MYON_SubDMeshFragments with density+1 = m_full_fragment_display_density are allocated from m_fsp_part_fragments.
  MYON_FixedSizePool m_fsp_part_fragments;

  // MYON_SubDMeshFragments with density < m_full_fragment_display_density-1 || density > m_full_fragment_display_density
  // are allocated from m_fsp_oddball_fragments. In all common cases, m_fsp_oddball_fragments is not used.
  MYON_FixedSizePool m_fsp_oddball_fragments;

  // m_unused_fragments[0] = unused 1x1 mesh quad fragments.
  // m_unused_fragments[1] = unused 2x2 mesh quad fragments.
  // m_unused_fragments[2] = unused 4x4 mesh quad fragments.
  // m_unused_fragments[3] = unused 8x8 mesh quad fragments.
  // m_unused_fragments[4] = unused 16x16 mesh quad fragments.
  // ...
  // The memory for these fragments is managed by m_fragment_pool.
  class MYON_FixedSizePoolElement* m_unused_fragments[MYON_SubDDisplayParameters::MaximumDensity + 1] = {};

  bool Internal_InitializeLimitCurvesPool();

  // Used to allocate edge curves (MYON_SubDEdgeSurfaceCurve class that MYON_SubDEdge.m_limit_curve points to
  // The memory for these curves is managed by m_limit_block_pool.
  MYON_FixedSizePool m_fsp_limit_curves;
  
  // list of vertices previously allocated from m_fspv and returned by ReturnVertex().
  // The memory for these vertices is managed by m_fspv.
  MYON_SubDVertex* m_unused_vertex = nullptr; 

  // list of edges previously allocated from m_fspv and returned by ReturnVertex().
  // The memory for these edges is managed by m_fspe.
  MYON_SubDEdge* m_unused_edge = nullptr;

  // list of faces previously allocated from m_fspv and returned by ReturnVertex().
  // The memory for these faces is managed by m_fspf.
  MYON_SubDFace* m_unused_face = nullptr;

  // Maximum vertex id assigned to a vertex in m_fspv.
  // If m_unused_vertex is not nullptr, some vertices are currently deleted.
  // Deleted vertices can be reused, but their id is never changed.
  unsigned int m_max_vertex_id = 0;

  // Maximum edge id assigned to an edge in m_fspe.
  // If m_unused_edge is not nullptr, some edges are currently deleted.
  // Deleted edges can be reused, but their id is never changed.
  unsigned int m_max_edge_id = 0;

  // Maximum face id assigned to a face in m_fspf.
  // If m_unused_face is not nullptr, some faces are currently deleted.
  // Deleted faces can be reused, but their id is never changed.
  unsigned int m_max_face_id = 0;

  unsigned int m_reserved2 = 0;

  MYON__UINT_PTR* AllocateOversizedElement(
    size_t* capacity
    );
  void  ReturnOversizedElement(
    size_t capacity,
    MYON__UINT_PTR* a
    );
  static size_t OversizedElementCapacity(
    size_t count
    );
  class MYON_3dPoint* Allocate3dPointArray(
    size_t point_capacity
  );
  void Return3dPointArray(
    class MYON_3dPoint* point_array
  );


private:
  MYON_FixedSizePool* Internal_ComponentFixedSizePool(
    MYON_SubDComponentPtr::Type component_type
  );
  const MYON_FixedSizePool* Internal_ComponentFixedSizePool(
    MYON_SubDComponentPtr::Type component_type
  ) const;

public:
  static unsigned int Managed3dPointArrayCapacity(class MYON_3dPoint* point_array);

private:
  static size_t m_offset_vertex_id;
  static size_t m_offset_edge_id;
  static size_t m_offset_face_id;
};


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDimple
//

class MYON_SubDimple
{
private:
  static std::atomic<MYON__UINT64> Internal_RuntimeSerialNumberGenerator;

public:
  /*
  Returns:
    A runtime serial number identifying this MYON_SubDimple instance.
  Remarks:
    MYON_SubD is a shared pointer to an implementation. As such, there can
    be multiple MYON_SubD instances that reference the same implementation.
    The runtime serial number uniquely identifies a particular instance
    of an implementation.
    The empty subd has runtime serial number = 0.
  */  
  const MYON__UINT64 RuntimeSerialNumber;
  
  /*
  Returns:
    A runtime serial number that is incremented every time a the active level,
    vertex location, vertex or edge flag, or subd topology is chaned.
  */
  MYON__UINT64 GeometryContentSerialNumber() const;

  const MYON_SubDHash SubDHash(
    MYON_SubDHashType hash_type,
    bool bForceUpdate
    ) const;

  const MYON_SHA1_Hash VertexHash(MYON_SubDHashType hash_type) const;

  const MYON_SHA1_Hash EdgeHash(MYON_SubDHashType hash_type) const;

  const MYON_SHA1_Hash FaceHash(MYON_SubDHashType hash_type) const;

  /*
  Returns:
    A runtime serial number that is incremented every time a component status
    (locked, hidden, selected, highlighted, ...) is changed.
  */
  MYON__UINT64 ComponentStatusSerialNumber() const;

  const MYON_AggregateComponentStatusEx AggregateComponentStatus() const;

  /*
  Description:
    Change the geometry content serial number to indicate something affecting
    the geometric shape of the subd has changed. This includes topologial changes,
    vertex and edge tag changes, and changes to vertex control net locations.
  Parameters:
    bChangePreservesSymmetry - [in]
      When in doubt, pass false.
      If the change preserves global symmtery, then pass true.
      (For example, a global subdivide preserves all types of symmetry.)
      Note well: 
        Transformations do not preserve symmetries that are
        set with respect to world coordinate systems.
  Returns:
    The new value of GeometryConentSerialNumber().
  Remarks:
    The value can change by any amount. 
    Changing the geometry content serial number automatically changes
    the render content serial number.
  */
  MYON__UINT64 ChangeGeometryContentSerialNumber(
    bool bChangePreservesSymmetry
  ) const;

  /*
  Description:
    The render content serial number changes whenever a change the might effect
    rendered appearance changes. This includes both geometry changes and
    changes that affect rendered appeance including changes to per face colors,
    per face materials, texture coordinates, and texture mappings.
  */
  MYON__UINT64 RenderContentSerialNumber() const;

  /*
  Description:
    Change the render content serial number to indicate something affecting
    only rendered appearance has changed. This includes changes to per face colors,
    per face materials, texture coordinates, and texture mappings.
  Remarks:
    Changing the geometry content serial number automatically changes
    the render content serial number. If you call ChangeGeometryContentSerialNumber(),
    there is no need to also call ChangeRenderContentSerialNumber().
  */
  MYON__UINT64 ChangeRenderContentSerialNumber() const;

private:
  mutable MYON__UINT64 m_subd_geometry_content_serial_number = 0;
  mutable MYON__UINT64 m_subd_render_content_serial_number = 0;

  mutable MYON_SubDHash m_subd_toplology_hash = MYON_SubDHash::Empty;
  mutable MYON_SubDHash m_subd_toplology_and_edge_creases_hash = MYON_SubDHash::Empty;
  mutable MYON_SubDHash m_subd_geometry_hash = MYON_SubDHash::Empty;

public:
  MYON_SubDimple();
  ~MYON_SubDimple();
  MYON_SubDimple(const MYON_SubDimple&);

  void SetManagedMeshSubDWeakPointers(
    std::shared_ptr<class MYON_SubDimple>& subdimple_sp
  );


  /*
  Description:
    Get the SubD appearance (surface or control net);
  Returns:
    MYON_SubDComponentLocation::Surface or MYON_SubDComponentLocation::ControlNet.
  */
  MYON_SubDComponentLocation SubDAppearance() const;

  /*
  Description:
    Set the SubD appearance (surface or control net) for a given context.
  Parameters:
    subd_appearance - [in]
      MYON_SubDComponentLocation::Surface or MYON_SubDComponentLocation::ControlNet.
  Remarks:
    This makes no changes to the information that defines the SubD.
  */
  void SetSubDAppearance(MYON_SubDComponentLocation subd_appearance) const;

  bool IsValid(
    const MYON_SubD& subd,
    bool bSilentError,
    MYON_TextLog*
    ) const;

  bool Write(
         MYON_BinaryArchive& archive
         ) const;

  bool Read(
         MYON_BinaryArchive& archive,
         class MYON_SubD& subd
         );

  bool Transform( 
    const MYON_Xform& xform
    );

  const class MYON_SubDVertex* VertexFromId(
    unsigned int vertex_id
    ) const
  {
    return m_heap.VertexFromId(vertex_id);
  }

  const class MYON_SubDEdge* EdgeFromId(
    unsigned int edge_id
    ) const
  {
    return m_heap.EdgeFromId(edge_id);
  }

  const class MYON_SubDFace* FaceFromId(
    unsigned int face_id
    ) const
  {
    return m_heap.FaceFromId(face_id);
  }

  MYON_SubDLevelIterator LevelIterator() const
  {
    MYON_SubDLevelIterator lit;
    lit.m_levels = (MYON_SubDLevel**)m_levels.Array();
    lit.m_level_count = m_levels.UnsignedCount();
    return lit;
  }

  unsigned int LevelCount() const
  {
    return m_levels.UnsignedCount();
  }

public:
  class MYON_SubDEdge* AddEdge(
    MYON_SubDEdgeTag edge_tag,
    MYON_SubDVertex* v0,
    double v0_sector_coefficient,
    MYON_SubDVertex* v1,
    double v1_sector_coefficient
    );

  class MYON_SubDEdge* AddEdge(
    unsigned int candidate_edge_id,
    MYON_SubDEdgeTag edge_tag,
    MYON_SubDVertex* v0,
    double v0_sector_coefficient,
    MYON_SubDVertex* v1,
    double v1_sector_coefficient,
    unsigned initial_face_capacity
  );

  /*
  Description:
    Split an edge.
    The input edge is modified to terminate at the input vertex.
    The new edge begins at the input vertex and ends at the final vertex
    of the original input edge.
  edge - [in]
    edge to split.
  vertex_location - [in]
    location of inserted vertex.
  Returns:
    A pointer to the new edge or nullptr if the input is not valid.
  */
  const MYON_SubDEdge* SplitEdge(
    MYON_SubDEdge* edge,
    MYON_3dPoint vertex_location
    );

  class MYON_SubDFace* AddFace(
    unsigned int edge_count,
    const MYON_SubDEdgePtr* edge
    );

  class MYON_SubDFace* AddFace(
    unsigned int candidate_face_id,
    unsigned int edge_count,
    const MYON_SubDEdgePtr* edge
  );
  
  /*
  Description:
    Spin an edge's endpoints around the boundary of its neighboring faces.
    In a counter-clockwise spin (looking at faces from their shared up orientation):
      The edge's start vertex is moved to the next vertex in the boundary
      of the face on the right-hand side of the edge.
      The edge's end vertex is moved to the next vertex in the boundary
      of the face on the left-hand side of the edge.
    Note that reversing the input edge does not change the result.
  Parameters:
    edge - [in]
      edge to spin.
    spin_clockwise - [in]
      false spins the edge counter-clockwise, true spins the edge clockwise
      in the adjacent faces.
  Returns:
    A pointer to the spun edge or nullptr if the input is not valid.
  */
  const MYON_SubDEdge* SpinEdge(
    MYON_SubDEdge* edge,
    bool spin_clockwise = false
  );

  /*
  Returns:
    Capacity of allocated face->m_texture_points[]
  */
  unsigned int AllocateFaceTexturePoints(
    const class MYON_SubDFace* face
  ) const;

  void ReturnFaceTexturePoints(
    const class MYON_SubDFace* face
  ) const;

  /*
  Description:
    Delete texture points from all faces.
  Returns:
    Number of faces that had texture points.
  */
  unsigned int ClearTexturePoints() const;


  /*
  Description:
    Split a face into two faces by inserting and edge connecting the
    specified vertices.
  Parameters:
    face - [in]
      A face with at least four edges.
    fvi0 - [in]
    fvi1 - [in]
      Indices of the inserted edge ends.
  Returns:
    A pointer to the inserted edge.
    The inserted edge runs from face->Vertex(fvi0) to face->Vertex(fvi1).
    MYON_SubDEdge.Face(0) is the original face and MYON_SubDEdge::Face(1) is 
    the added face.
    The first edge of the input face remains the first edge of face.  
    The inserted edge is the first edge of the added face.
  */
  const MYON_SubDEdge* SplitFace(
    MYON_SubDFace* face,
    unsigned int fvi0,
    unsigned int fvi1
    );

  class MYON_SubDVertex* AllocateVertex(
    MYON_SubDVertexTag vertex_tag,
    unsigned int level,
    const double* P
    )
  {
    return AllocateVertex( 0U, vertex_tag, level, P, 0, 0);
  }

  class MYON_SubDVertex* AllocateVertex(
    unsigned int candidate_id,
    MYON_SubDVertexTag vertex_tag,
    unsigned int level,
    const double* P,
    unsigned int edge_capacity,
    unsigned int face_capacity
    )
  {
    class MYON_SubDVertex* v = m_heap.AllocateVertexAndSetId( candidate_id);
    v->SetSubdivisionLevel(level);
    v->m_vertex_tag = vertex_tag;
    if (nullptr != P)
    {
      v->m_P[0] = P[0];
      v->m_P[1] = P[1];
      v->m_P[2] = P[2];
    }
    if (edge_capacity > 0 && edge_capacity < MYON_SubDVertex::MaximumEdgeCount )
      m_heap.GrowVertexEdgeArray(v,edge_capacity);
    if (face_capacity > 0 && face_capacity < MYON_SubDVertex::MaximumFaceCount )
      m_heap.GrowVertexFaceArray(v,face_capacity);
    return v;
  }

  const class MYON_SubDVertex* AddVertexToLevel(class MYON_SubDVertex* v)
  {
    class MYON_SubDLevel* subd_level = SubDLevel(v->SubdivisionLevel(),true);
    return (subd_level) ? subd_level->AddVertex(v) : nullptr;
  }

  void ReturnVertex(class MYON_SubDVertex* v)
  {
    if (nullptr != v && v->SubdivisionLevel() < m_levels.UnsignedCount())
    {
      MYON_SubDLevel* level = m_levels[v->SubdivisionLevel()];
      if (level)
        level->RemoveVertex(v);
    }
    v->ClearSavedSubdivisionPoints(); // return extras to pool
    m_heap.ReturnVertex(v);
  }

  MYON_SubDEdge* AllocateEdge(
    MYON_SubDEdgeTag edge_tag
  )
  {
    MYON_SubDEdge* e = m_heap.AllocateEdgeAndSetId(0U);
    e->m_edge_tag = edge_tag;
    return e;
  }

  MYON_SubDEdge* AllocateEdge(
    unsigned int candidate_edge_id,
    MYON_SubDEdgeTag edge_tag,
    unsigned int level,
    unsigned int face_capacity
    )
  {
    MYON_SubDEdge* e = m_heap.AllocateEdgeAndSetId( candidate_edge_id);
    e->m_edge_tag = edge_tag;
    e->SetSubdivisionLevel(level);
    if (face_capacity > 0 && face_capacity <= MYON_SubDEdge::MaximumFaceCount )
      m_heap.GrowEdgeFaceArray(e,face_capacity);
    return e;
  }

  const MYON_SubDEdge* AddEdgeToLevel(class MYON_SubDEdge* e)
  {
    MYON_SubDLevel* subd_level = SubDLevel(e->SubdivisionLevel(),true);
    return (subd_level) ? subd_level->AddEdge(e) : nullptr;
  }

  void ReturnEdge(class MYON_SubDEdge* e)
  {
    if (nullptr != e && e->SubdivisionLevel() < m_levels.UnsignedCount())
    {
      MYON_SubDLevel* level = m_levels[e->SubdivisionLevel()];
      if (level)
        level->RemoveEdge(e);
    }
    m_heap.ReturnEdge(e);
  }

  class MYON_SubDFace* AllocateFace()
  {
    class MYON_SubDFace* f = m_heap.AllocateFaceAndSetId( 0U);
    return f;
  }

  class MYON_SubDFace* AllocateFace(
    unsigned int candidate_face_id,
    unsigned int level,
    unsigned int edge_capacity
  )
  {
    return AllocateFace(candidate_face_id, level, edge_capacity, false);
  }

  class MYON_SubDFace* AllocateFace(
    unsigned int candidate_face_id,
    unsigned int level,
    unsigned int edge_capacity,
    bool bAllocateTexturePoints
  )
  {
    class MYON_SubDFace* f = m_heap.AllocateFaceAndSetId(candidate_face_id);
    if (nullptr != f)
    {
      f->SetSubdivisionLevel(level);
      if (edge_capacity > sizeof(f->m_edge4) / sizeof(f->m_edge4[0]) && edge_capacity <= MYON_SubDFace::MaximumEdgeCount)
        m_heap.GrowFaceEdgeArray(f, edge_capacity);
      if (bAllocateTexturePoints)
        m_heap.AllocateFaceTexturePoints(f);
    }
    return f;
  }

  const class MYON_SubDFace* AddFaceToLevel(class MYON_SubDFace* f)
  {
    class MYON_SubDLevel* subd_level = SubDLevel(f->SubdivisionLevel(),true);
    return (subd_level) ? subd_level->AddFace(f) : nullptr;
  }

  void ReturnFace(class MYON_SubDFace* f)
  {
    if (nullptr != f && f->SubdivisionLevel() < m_levels.UnsignedCount())
    {
      MYON_SubDLevel* level = m_levels[f->SubdivisionLevel()];
      if (level)
        level->RemoveFace(f);
    }
    m_heap.ReturnFace(f);
  }

  unsigned int DeleteComponents(
    unsigned int level_index,
    bool bDeleteIsolatedEdges,
    bool bUpdateTagsAndCoefficients,
    bool bMarkDeletedFaceEdges
    );

  /*
  Description:
    Discard all contents of this MYON_SubDimple.
  Remarks:
    More efficient than Destroy() if this MYON_SubDimple will be reused soon.
  */
  void Clear();

  /*
  Description:
    Removes every level above max_level_index
  Returns:
    Number of removed levels.
  */
  unsigned int ClearHigherSubdivisionLevels(
    unsigned int max_level_index
    );
  
  /*
  Description:
    Removes every level below min_level_index
  Returns:
    Number of removed levels.
  */
  unsigned int ClearLowerSubdivisionLevels(
    unsigned int min_level_index
    );

  /*
  Remove all levels except the active level.
  Returns:
    Number of removed levels.
  */
  unsigned int ClearInactiveLevels();

  void ClearLevelContents(
    MYON_SubDLevel* level
  );

  size_t SizeOf() const
  {
    size_t sz = sizeof(*this) + m_heap.SizeOf() - sizeof(m_heap);
    return sz;
  }

  size_t SizeOfAllElements() const
  {
    return m_heap.SizeOfAllPools();
  }

  size_t SizeOfActiveElements() const
  {
    return m_heap.SizeOfActivePoolElements();
  }

  size_t SizeOfUnusedElements() const
  {
    return m_heap.SizeOfUnusedPoolElements();
  }

  size_t SizeOfAllMeshFragments() const
  {
    return m_heap.SizeOfMeshFragmentsPool();
  }

  size_t SizeOfActiveMeshFragments() const
  {
    return m_heap.SizeOfActiveMeshFragments();
  }

  size_t SizeOfUnusedMeshFragments() const
  {
    return m_heap.SizeOfUnusedMeshFragments();
  }

  bool GlobalSubdivide(
    unsigned int count
    );

  bool LocalSubdivide(
    MYON_SubDFace const*const* face_list,
    size_t face_count
  );

  /*
  Description:
    Apply global subdivision to m_levels[].Last().
  */
  unsigned int GlobalSubdivide();

  unsigned int MergeColinearEdges(
    bool bMergeBoundaryEdges,
    bool bMergeInteriorCreaseEdges,
    bool bMergeInteriorSmoothEdges,
    double distance_tolerance,
    double maximum_aspect,
    double sin_angle_tolerance
    );

  MYON_SubDEdgePtr MergeConsecutiveEdges(
    MYON_SubDEdgePtr eptr0,
    MYON_SubDEdgePtr eptr1
    );

public:
  bool GrowVertexEdgeArray(
    MYON_SubDVertex* v,
    size_t capacity
    );
  bool GrowVertexFaceArray(
    MYON_SubDVertex* v,
    size_t capacity
    );
  bool GrowEdgeFaceArray(
    MYON_SubDEdge* e,
    size_t capacity
    );
  bool GrowFaceEdgeArray(
    MYON_SubDFace* f,
    size_t capacity
    );

  bool CopyEvaluationCacheForExperts(const MYON_SubDimple& src);

private:
  friend class MYON_Internal_SubDFaceMeshFragmentAccumulator;
  MYON_SubDHeap m_heap;

public:
  MYON_SubDHeap& Heap();

public:
  void InitializeVertexIdIterator(
    class MYON_SubDVertexIdIterator& vidit
  ) const;

  void InitializeEdgeIdIterator(
    class MYON_SubDEdgeIdIterator& eidit
  ) const;

  void InitializeFaceIdIterator(
    class MYON_SubDFaceIdIterator& fidit
  ) const;
  
  void InitializeComponentIdIterator(
    MYON_SubDComponentPtr::Type ctype,
    class MYON_SubDComponentIdIterator& cidit
  ) const;


  const MYON_MappingTag TextureMappingTag(bool bIgnoreTextureCoordinateType) const;
  void SetTextureMappingTag(const MYON_MappingTag& mapping_tag) const;

  enum MYON_SubDTextureCoordinateType TextureCoordinateType() const;
  void SetTextureCoordinateType(
    MYON_SubDTextureCoordinateType texture_coordinate_type
    ) const;

  const MYON_SHA1_Hash FragmentTextureCoordinatesTextureSettingsHash() const
  {
    return m_fragment_texture_settings_hash;
  }

  void Internal_SetFragmentTextureCoordinatesTextureSettingsHash(MYON_SHA1_Hash hash) const
  {
    m_fragment_texture_settings_hash = hash;
  }

  const MYON_SHA1_Hash FragmentColorsSettingsHash() const
  {
    return m_fragment_colors_settings_hash;
  }

  void Internal_SetFragmentColorsSettingsHash(MYON_SHA1_Hash hash) const
  {
    m_fragment_colors_settings_hash = hash;
  }

  const MYON_MappingTag FragmentColorsMappingTag() const;
  void SetFragmentColorsMappingTag(const MYON_MappingTag& mapping_tag) const;

private:
  mutable MYON_SubDComponentLocation m_subd_appearance = MYON_SubD::DefaultSubDAppearance;
  
  mutable MYON_SubDTextureCoordinateType m_texture_coordinate_type = MYON_SubDTextureCoordinateType::Unset;
  unsigned short m_reserved = 0;
  mutable MYON_MappingTag m_texture_mapping_tag;
  mutable MYON_MappingTag m_fragment_colors_mapping_tag;

  // hash of the settings used to create the current fragment texture coordinates
  mutable MYON_SHA1_Hash m_fragment_texture_settings_hash = MYON_SHA1_Hash::EmptyContentHash;

  // hash of the settings used to create the current fragment vertex colors
  mutable MYON_SHA1_Hash m_fragment_colors_settings_hash = MYON_SHA1_Hash::EmptyContentHash;

  MYON_SimpleArray< MYON_SubDLevel* > m_levels;
  MYON_SubDLevel* m_active_level = nullptr; // m_active_level = nullptr or m_active_level = m_levels[m_active_level->m_level_index].
  
public:
  unsigned int MaximumVertexId() const
  {
    return m_heap.MaximumVertexId();
  }
  unsigned int MaximumEdgeId() const
  {
    return m_heap.MaximumEdgeId();
  }
  unsigned int MaximumFaceId() const
  {
    return m_heap.MaximumFaceId();
  }

  //void IncreaseMaximumVertexId(
  //  unsigned new_maximum_vertex_id
  //)
  //{
  //  if (new_maximum_vertex_id > m_dimple_max_vertex_id && new_maximum_vertex_id + 1 < MYON_UNSET_UINT_INDEX)
  //    m_dimple_max_vertex_id = new_maximum_vertex_id;
  //}

  //void IncreaseMaximumEdgeId(
  //  unsigned new_maximum_edge_id
  //)
  //{
  //  if (new_maximum_edge_id > m_dimple_max_edge_id && new_maximum_edge_id + 1 < MYON_UNSET_UINT_INDEX)
  //    m_dimple_max_edge_id = new_maximum_edge_id;
  //}

  //void IncreaseMaximumFaceId(
  //  unsigned new_maximum_face_id
  //)
  //{
  //  if (new_maximum_face_id > m_dimple_max_face_id && new_maximum_face_id + 1 < MYON_UNSET_UINT_INDEX)
  //    m_dimple_max_face_id = new_maximum_face_id;
  //}

  /*
  Returns:
    Active level
  */
  const MYON_SubDLevel& ActiveLevel() const
  {
    return (nullptr != m_active_level) ? *m_active_level : MYON_SubDLevel::Empty;
  }

  const unsigned int ActiveLevelIndex() const
  {
    return (nullptr != m_active_level) ? m_active_level->m_level_index : 0;
  }

  MYON_SubDLevel* ActiveLevelPointer()
  {
    return m_active_level;
  }

  const MYON_SubDLevel* ActiveLevelConstPointer() const
  {
    return m_active_level;
  }

  bool SetActiveLevel(
    unsigned int level_index
    )
  {
    if (level_index < m_levels.UnsignedCount())
    {
      m_active_level = m_levels[level_index];
      return true;
    }
    return false;
  }
  
  class MYON_SubDLevel const * SubDLevel(
    unsigned int level_index
    ) const;

private:
  MYON_SubDLevel* ActiveLevel(
    bool bCreateIfNeeded
    );

  class MYON_SubDLevel* SubDLevel(
    unsigned int level_index,
    bool bCreateIfNeeded
    );

  /*
  Description:
    Delete all contents release all memory used by this MYON_SubDimple.
  */
  void Destroy();

  bool IsValidLevel(
    const MYON_SubD& subd,
    unsigned int level_index,
    bool bSilentError,
    MYON_TextLog* text_log
    ) const;

  /*
  Returns:
  Number of quads added. When all input is valid the
  returned value is >= 4 and equal to face->m_edge_count.
  */
  unsigned int Internal_GlobalQuadSubdivideFace(
    const MYON_SubDFace* face,
    unsigned max_pack_id
    );

public:
  const MYON_UUID FacePackingId() const;

  const MYON_SubDHash FacePackingTopologyHash() const;


  void SetFacePackingIdAndTopologyHash(
    MYON_UUID custom_packing_id,
    const MYON_SubDHash& current_topology_and_creases_hash
  );

  /*
  Description:
    Clear all face pack ids and related information.
  */
  void ClearFacePackIds();

  void SetFacePackingTopologyHashForExperts(const MYON_SubDHash& current_topology_and_creases_hash) const;

private:
  MYON_UUID m_face_packing_id = MYON_nil_uuid;
  mutable MYON_SubDHash m_face_packing_topology_hash = MYON_SubDHash::Empty;


private:
  MYON_Symmetry m_symmetry;

public:



 private:
   MYON_SubDimple& operator=(const MYON_SubDimple&) = delete;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentIdIterator
//
class MYON_SubDComponentIdIterator : private MYON_FixedSizePoolIterator
{
public:
  MYON_SubDComponentIdIterator() = default;
  ~MYON_SubDComponentIdIterator() = default;
  MYON_SubDComponentIdIterator(const MYON_SubDComponentIdIterator&) = default;
  MYON_SubDComponentIdIterator& operator=(const MYON_SubDComponentIdIterator&) = default;

public:
  /*
  Description:
    In general, you want to use a MYON_SubDVertexIterator to loop through SubD vertices.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every vertex on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The vertex with the smallest id.
  */
  const MYON_SubDComponentBase* FirstComponent();

  /*
  Description:
    In general, you want to use a MYON_SubDVertexIterator to loop through SubD vertices.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every vertex on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The vertex in order of increasing id.
  */
  const MYON_SubDComponentBase* NextComponent();

  /*
  Returns:
    The most recently returned vertex from a call to FirstVertex() or NextVertex().
  */
  const MYON_SubDComponentBase* CurrentComponent() const;

private:
  friend class MYON_SubDHeap;
  MYON_SubDComponentPtr::Type m_component_type = MYON_SubDComponentPtr::Type::Unset;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDLevelComponentIdIterator
//
class MYON_SubDLevelComponentIdIterator
{
public:
  MYON_SubDLevelComponentIdIterator() = default;
  ~MYON_SubDLevelComponentIdIterator() = default;
private:
  MYON_SubDLevelComponentIdIterator(const MYON_SubDLevelComponentIdIterator&) = delete;
  MYON_SubDLevelComponentIdIterator& operator=(const MYON_SubDLevelComponentIdIterator&) = delete;

public:
  void Initialize(
    bool bLevelLinkedListIncreasingId,
    MYON_SubDComponentPtr::Type ctype,
    const MYON_SubDimple& subdimple,
    const MYON_SubDLevel& level
  );  

  const MYON_SubDVertex* FirstVertex();
  const MYON_SubDVertex* NextVertex();

  const MYON_SubDEdge* FirstEdge();
  const MYON_SubDEdge* NextEdge();

  const MYON_SubDFace* FirstFace();
  const MYON_SubDFace* NextFace();

private:
  const MYON_SubDComponentBase* InternalFirst();
  
  const MYON_SubDComponentBase* InternalNext();

private:
  bool m_bLevelLinkedListIncreasingId = false;
  MYON_SubDComponentPtr::Type m_ctype = MYON_SubDComponentPtr::Type::Unset;
  unsigned short m_level_index = 0;
  const MYON_SubDComponentBaseLink* m_first = nullptr;
  const MYON_SubDComponentBaseLink* m_current = nullptr;
  unsigned int m_count = 0;
  unsigned int m_prev_id = 0;
  MYON_SubDComponentIdIterator m_cidit;
};




class MYON_SubDArchiveIdMap
{
  // used for file IO, copy construction, and operator= where the problem
  // of converting to/from pointers to archive_ids is necessary.
public:
  static unsigned int ArchiveIdFromComponentPtr(
    MYON__UINT_PTR ptr
    );

  bool ConvertArchiveIdToRuntimeVertexPtr(
    unsigned int vertex_count,
    size_t vertex_capacity,
    MYON_SubDVertex** vertex
    );

  bool ConvertArchiveIdToRuntimeEdgePtr(
    unsigned int edge_count,
    size_t edgeN_capacity,
    MYON_SubDEdgePtr* edgeN,
    unsigned int edgeX_capacity,
    MYON_SubDEdgePtr* edgeX
    );

  bool ConvertArchiveIdToRuntimeFacePtr(
    unsigned int face_count,
    size_t faceN_capacity,
    MYON_SubDFacePtr* faceN,
    unsigned int faceX_capacity,
    MYON_SubDFacePtr* faceX
    );

private:
  bool ConvertArchiveIdToRuntimeSymmetrySetNextPtr(
    MYON_SubDComponentPtr::Type component_type,
    MYON_SubDComponentBase* component
  );

public:
  static void ValidateArrayCounts(
    unsigned short& array_count,
    size_t arrayN_capacity,
    const void* arrayN,
    unsigned short arrayX_capacity,
    const void* arrayX
    );

public:
  static const MYON_SubDComponentPtr FromVertex(
    MYON_SubDVertexPtr vertex_ptr
    );

  static const MYON_SubDComponentPtr FromEdge(
    MYON_SubDEdgePtr edge_ptr
    );

  static const MYON_SubDComponentPtr FromFace(
    MYON_SubDFacePtr face_ptr
    );

  static const MYON_SubDComponentPtr FromVertex(
    const MYON_SubDVertex* vertex
    );

  static const MYON_SubDComponentPtr FromEdge(
    const MYON_SubDEdge* edge
    );

  static const MYON_SubDComponentPtr FromFace(
    const MYON_SubDFace* face
    );

private:
  static const MYON_SubDComponentPtr FromSymmetrySetNext(
    MYON_SubDComponentPtr::Type component_type,
    const MYON_SubDComponentBase* component
  );

public:
  static MYON_SubDVertex* CopyVertex(
    const MYON_SubDVertex* src,
    class MYON_SubDimple& subdimple
    );

  static MYON_SubDEdge* CopyEdge(
    const MYON_SubDEdge* src,
    class MYON_SubDimple& subdimple
    );

  static MYON_SubDFace* CopyFace(
    const MYON_SubDFace* src,
    class MYON_SubDimple& subdimple
    );

  MYON_SubDArchiveIdMap();

  bool Reset();

  unsigned int Count();

  const MYON_SubDComponentPtr* First();
  const MYON_SubDComponentPtr* Next();

  bool Add(const MYON_SubDVertex* vertex);
  bool Add(const MYON_SubDEdge* edge);
  bool Add(const MYON_SubDFace* face);

  class MYON_SubDVertex* AddCopy(const class MYON_SubDVertex* source_vertex, class MYON_SubDimple& subdimple);
  class MYON_SubDEdge* AddCopy(const class MYON_SubDEdge* source_edgeclass, class MYON_SubDimple& subdimple);
  class MYON_SubDFace* AddCopy(const class MYON_SubDFace* source_faceclass, class MYON_SubDimple& subdimple);

  const MYON_SubDComponentPtr* ComponentPtrFromArchiveId(
    unsigned int archive_id
    ) const;

  unsigned int m_archive_id_partition[4];

  unsigned int ConvertArchiveIdsToRuntimePointers();

public:
  static MYON_SubDComponentPtr& SymmetrySetNextForExperts(const MYON_SubDComponentBase&);

private:
  bool AddComponentPtr(MYON_SubDComponentPtr eptr, unsigned int archive_id);
  
  unsigned int m_element_index = MYON_UNSET_UINT_INDEX;
  unsigned int m_element_count = 0;
  MYON_FixedSizePool m_fsp;
  MYON_FixedSizePoolIterator m_fsp_it;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMesh
//

class /*DO NOT EXPORT*/MYON_SubDMeshImpl
{
public:
  MYON_SubDMeshImpl();
  ~MYON_SubDMeshImpl() = default;
  MYON_SubDMeshImpl(const MYON_SubDMeshImpl& src);

private:
  // no operator =
  MYON_SubDMeshImpl& operator=(const MYON_SubDMeshImpl&) = delete;

public:
  MYON__UINT64 ContentSerialNumber() const;
  MYON__UINT64 ChangeContentSerialNumber();

private:
  MYON__UINT64 m_mesh_content_serial_number = 0;
public:
  unsigned int m_absolute_subd_display_density = 0;
  unsigned int m_fragment_count = 0;
  unsigned int m_fragment_point_count = 0;
  MYON_SubDMeshFragment* m_first_fragment = nullptr;
  MYON_SubDMeshFragment* m_last_fragment = nullptr;
  
  bool ReserveCapacity(
    unsigned int subd_fragment_count,
    unsigned int absolute_subd_display_density
    );
  
  /*
  Description:
    MYON_SubDLimitMeshImpl_CallbackContext::FragmentCallbackFunction()
    uses CopyCallbackFragment() to make a copy of callback_fragment 
    delivered by MYON_SubD::GetMeshFragments().
  */
  MYON_SubDMeshFragment* CopyCallbackFragment(
    const MYON_SubDMeshFragment* callback_fragment
    );

  /*
  Description:
    MYON_SubDLimitMeshImpl_CallbackContext::FragmentCallbackFunction()
    uses AddFinishedFragment() to add finished fragments to this
    MYON_SubDMeshImpl's m_first_fragment ... m_list_fragment list.
  */
  bool AddFinishedFragment(
    MYON_SubDMeshFragment* fragment
    );

  /*
  After all fragments have been collected, it's necessary to seal
  the edges of the fragments along shared subd edges because the
  locations and normals are computed by evaluating different
  sides.
  */
  void SealEdges();

  const MYON_RTree& FragmentTree() const;

  void ClearTree();
  
  bool Transform( 
    const MYON_Xform& xform
    );


  bool GetTightBoundingBox(
    MYON_BoundingBox& bbox,
    bool bGrowBox,
    const MYON_Xform* xform
    ) const;

  MYON_BoundingBox m_bbox;

  // The weak pointer to the MYON_SubDimple is used to
  // check that the MYON_SubDimple managing the
  // MYON_SubDMeshFragment.m_face pointers is valid 
  // before those pointers are used.  This must be a weak
  // pointer and not a shared_ptr because limit meshes
  // are stored on MYON_SubDLevels that are members of
  // MYON_SubDimple.
  std::weak_ptr<class MYON_SubDimple> m_subdimple_wp;

  void SetSubDWeakPointer(
    const MYON_SubDFace* subd_first_face,
    std::shared_ptr<class MYON_SubDimple>& subdimple_sp
    );

  void ClearFragmentFacePointers(
    bool bResetSubDWeakPtr
    );

private:
  MYON_RTree* m_fragment_tree = nullptr;

private:
  // A fixed sized memory pool that allocates enough memory for a fragment and its points, normals, textures, and colors.
  // The fragments never get returned because the pool itself is destroyed in ~MYON_SubDMeshImpl().
  MYON_FixedSizePool m_fsp;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeSurfaceCurve
//

class /*DO NOT EXPORT*/ MYON_SubDEdgeSurfaceCurve
{
public:
  // Use CopyFrom() when proper management of m_cvx is required.
  // This class is used internally and never seen int developer SDK.
  static const MYON_SubDEdgeSurfaceCurve Unset; // all doubles are MYON_UNSET_VALUE, everything else is zero.
  static const MYON_SubDEdgeSurfaceCurve Nan;   // all doubles are MYON_DBL_QNAN, everything else is zero
  static const MYON_SubDEdgeSurfaceCurve Zero;  // all doubles are 0.0, everything else is zero

public:
  /*
  Returns:
    true if 4 <= m_cv_count <= 11 and all coordinates are valid doubles.
    false otherwise.
  */
  bool IsSet() const;

  bool Transform(
    const MYON_Xform& xform
    );

  void Clear();

  /*
  Parameters:
    cv_count - [in]
      0 or 4 <= cv_count <= CVCapacity().
    cvs - [in]
      cvs for a cubic uniform non-rational NURBS curve with unclamped knot vector
      (-2,-1,0,1,2,3,...,cv_count-1).
  Remarks:
    The knot vector is unclamped to permit efficient joining of adjacent edge
    curves into longer NURBS with simple interior knots. This occurs frequently.
  */
  bool SetCVs(
    int cv_count,
    const MYON_3dPoint* cvs
  );

  unsigned int CVCount() const;

  unsigned int CVCapacity() const;

  /*
  Parameters:
    cv_capacity - [in]
      maximum number of points the cvs[] array can contain.
    cvs - [out]
      cvs returned here
  Returns:
    number of set cvs.
    The cvs are for a cubic uniform non-rational unclamped NURBS curve 
    with unclamped knot vector (-2,-1,0,1,2,3,...,cv_count-1).
  Remarks:
    The knot vector is unclamped to permit efficient joining of adjacent edge
    curves into longer NURBS with simple interior knots. This occurs frequently.
  */
  unsigned int GetCVs(
    size_t cv_capacity,
    MYON_3dPoint* cvs
  ) const;
  
public:

  // NOTE WELL: 
  // It is required that:
  // 1) MaximumControlPointCapacity = 2*MinimumControlPointCapacity+1;
  // 2) MaximumControlPointCapacity >= 3 + 2^MYON_SubDEdgeSharpness::Maximum
  enum : unsigned char
  {
    MinimumControlPointCapacity = 9,
    MaximumControlPointCapacity = 19
  };

private:
  // It is critical that sizeof(MYON_SubDEdgeSurfaceCurve) >= (MaximumControlPointCapacity-MinimumControlPointCapacity)*3*sizeof(double).
  // The edge curve cache relies on this.
  // Do not remove m_reserved* fields.
  MYON__UINT64 m_reserved0 = 0; // overlaps with MYON_FixedSizePoolElement.m_next.
public:
  unsigned char m_cv_count = 0;
  unsigned char m_cv_capacity = MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity;
  unsigned short m_reserved2 = 0;
  unsigned int m_reserved3 = 0;
  double m_cv5[MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity][3]; // initial 4 or 5 cvs are here.
  // If m_cv_capacity > MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity and m_cvx != nullptr,
  // m_cvx points to an array of 3*(MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity-MYON_SubDEdgeSurfaceCurve::MinimumControlPointCapacity)
  // doubles and m_cv_capacity = MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity; 
  double* m_cvx = nullptr;
};

#endif // MYON_COMPILING_OPENNURBS)

#endif // OPENNURBS_SUBD_DATA_INC_


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

/////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMeshFragment
//

void MYON_SubDMeshFragment::Internal_Set3dPointArrayToNan(double* a, size_t a_count, size_t a_stride)
{
  if (nullptr == a || a_count <= 0 || a_stride < 3)
    return;
  double *a2 = a + a_count * a_stride;
  if (3 == a_stride)
  {
    while (a < a2)
    {
      *a++ = MYON_DBL_QNAN;
      *a++ = MYON_DBL_QNAN;
      *a++ = MYON_DBL_QNAN;
    }
  }
  else
  {
    a_stride -= 3;
    while (a < a2)
    {
      *a++ = MYON_DBL_QNAN;
      *a++ = MYON_DBL_QNAN;
      *a++ = MYON_DBL_QNAN;
      a += a_stride;
    }
  }
}


unsigned MYON_SubDMeshFragment::VertexCapacity() const
{
  return (m_vertex_capacity_etc & MYON_SubDMeshFragment::ValueMask);
}

bool MYON_SubDMeshFragment::Internal_ManagedArraysAreInterlaced()
{
  return MYON_SubDMeshFragment::ManagedDoublesPerVertex == MYON_SubDMeshFragment::Managed_3d_stride;
}

size_t MYON_SubDMeshFragment::Internal_Managed3dArrayOffset(size_t vertex_capacity)
{
  return (vertex_capacity > 0)
    ? (3 * (MYON_SubDMeshFragment::Internal_ManagedArraysAreInterlaced() ? 1 : vertex_capacity))
    : 0;
}

void MYON_SubDMeshFragment::Internal_LayoutArrays(
  bool bManagedArray,
  double* PNTC_array,
  size_t vertex_capacity
)
{
  this->SetVertexCount(0);

  if (nullptr == PNTC_array || vertex_capacity < 4 || vertex_capacity > ((size_t)MYON_SubDMeshFragment::MaximumVertexCount) )
  {
    PNTC_array = nullptr;
    vertex_capacity = 0;
    bManagedArray = false;
  }
  if (bManagedArray)
  {
    unsigned short etc = (m_vertex_capacity_etc & MYON_SubDMeshFragment::EtcMask);
    etc |= MYON_SubDMeshFragment::EtcManagedArraysBit;
    m_vertex_capacity_etc = (unsigned short)vertex_capacity;
    m_vertex_capacity_etc |= etc;
  }
  else
  {
    this->SetUnmanagedVertexCapacity(vertex_capacity);
  }

  const size_t offset_3d = (nullptr != PNTC_array) ? MYON_SubDMeshFragment::Internal_Managed3dArrayOffset(vertex_capacity) : 0;
  m_P = PNTC_array;
  m_P_stride = MYON_SubDMeshFragment::Managed_3d_stride;
  m_N = m_P + offset_3d;
  m_N_stride = MYON_SubDMeshFragment::Managed_3d_stride;
  m_T = m_N + offset_3d;
  m_T_stride = MYON_SubDMeshFragment::Managed_3d_stride;
  m_C = (MYON_Color*)(m_T + offset_3d);
  m_C_stride = MYON_SubDMeshFragment::Managed_color_stride;
}

bool MYON_SubDMeshFragment::ManagedArrays() const
{
  const size_t vertex_capacity = (MYON_SubDMeshFragment::ValueMask & m_vertex_capacity_etc);
  const size_t offset_3d = MYON_SubDMeshFragment::Internal_Managed3dArrayOffset(vertex_capacity);
  return (
    0 != (MYON_SubDMeshFragment::EtcManagedArraysBit & m_vertex_capacity_etc)
    && vertex_capacity > 0
    && MYON_SubDMeshFragment::Managed_3d_stride == m_P_stride
    && MYON_SubDMeshFragment::Managed_3d_stride == m_N_stride
    && MYON_SubDMeshFragment::Managed_3d_stride == m_T_stride
    && MYON_SubDMeshFragment::Managed_color_stride == m_C_stride
    && nullptr != m_P
    && m_N == m_P + offset_3d
    && m_T == m_N + offset_3d
    && m_C == (const MYON_Color*)(m_T + offset_3d)
    ) ? true : false;
}

bool MYON_SubDMeshFragment::DeleteManagedArrays()
{
  DeleteManagedCurvatureCapacity();
  if (ManagedArrays())
  {
    double* managed_array = m_P;
    m_vertex_count_etc &= MYON_SubDMeshFragment::EtcControlNetQuadBit;
    m_vertex_capacity_etc = 0;
    m_P = nullptr;
    m_N = nullptr;
    m_T = nullptr;
    m_C = nullptr;
    m_K = nullptr;
    m_P_stride = 0;
    m_N_stride = 0;
    m_T_stride = 0;
    m_C_stride = 0;
    if (nullptr != managed_array)
    {
      // Allocated in MYON_SubDMeshFragment::ReserveManagedVertexCapacity()
      delete[] managed_array;
    }
    return true;
  }
  return false;
}

bool MYON_SubDMeshFragment::UnmanagedArrays() const
{
  return
    (0 == (MYON_SubDMeshFragment::EtcManagedArraysBit & m_vertex_capacity_etc) && 0 != (m_vertex_capacity_etc & MYON_SubDMeshFragment::ValueMask) )
    ? true
    : false;
}

bool MYON_SubDMeshFragment::SetUnmanagedVertexCapacity(size_t vertex_capacity)
{
  if (vertex_capacity < 0 || vertex_capacity > (size_t)MYON_SubDMeshFragment::MaximumVertexCount)
    return MYON_SUBD_RETURN_ERROR(false);
  if (ManagedArrays())
  {
    // attempting to internally managed memory to externally managed memory
    return MYON_SUBD_RETURN_ERROR(false);
  }
  unsigned short etc = m_vertex_capacity_etc &= MYON_SubDMeshFragment::EtcMask;
  etc &= ~MYON_SubDMeshFragment::EtcManagedArraysBit;
  m_vertex_capacity_etc = ((unsigned short)vertex_capacity) | etc;
  return true;
}

bool MYON_SubDMeshFragment::ReserveManagedVertexCapacity(size_t vertex_capacity)
{
  if (vertex_capacity < 0 || vertex_capacity >(size_t)MYON_SubDMeshFragment::MaximumVertexCount)
    return MYON_SUBD_RETURN_ERROR(false);
  if (vertex_capacity > (size_t)(MYON_SubDMeshFragment::ValueMask))
    return MYON_SUBD_RETURN_ERROR(false); // too big
  if (UnmanagedArrays())
  {
    // attempting to convert externally managed memory to internally managed memory.
    return MYON_SUBD_RETURN_ERROR(false);
  }
  const size_t current_capacity = (size_t)(MYON_SubDMeshFragment::ValueMask & m_vertex_capacity_etc);
  if (ManagedArrays())
  {
    if (current_capacity >= vertex_capacity)
      return true;
    DeleteManagedArrays();
  }

  // p will be deleted in MYON_SubDMeshFragment::DeleteManagedArrays().
  double* managed_doubles = new(std::nothrow) double[MYON_SubDMeshFragment::ManagedDoublesPerVertex * vertex_capacity];
  Internal_LayoutArrays(true, managed_doubles, vertex_capacity);
  return (this->VertexCapacity() >= vertex_capacity);
}

bool MYON_SubDMeshFragment::ReserveManagedCurvatureCapacity() const
{
  const size_t current_capacity = (size_t)(MYON_SubDMeshFragment::ValueMask & m_vertex_capacity_etc);
  if (nullptr == m_K && current_capacity > 0)
  {
    m_K = new(std::nothrow) MYON_SurfaceCurvature[current_capacity];
    m_vertex_capacity_etc &= ~MYON_SubDMeshFragment::EtcCurvaturesExistBit; // m_K[] content is unset.
  }
  return (nullptr != m_K && current_capacity > 0);
}


void MYON_SubDMeshFragment::DeleteManagedCurvatureCapacity() const
{
  if (nullptr != m_K)
  {
    delete[] m_K;
    m_K = nullptr;
    m_vertex_capacity_etc &= ~MYON_SubDMeshFragment::EtcCurvaturesExistBit; // m_K[] content is unset.
  }
}

unsigned MYON_SubDMeshFragment::VertexCount() const
{
  return (m_vertex_count_etc & MYON_SubDMeshFragment::ValueMask);
}

bool MYON_SubDMeshFragment::SetVertexCount(size_t vertex_count)
{
  if (vertex_count < 0 || vertex_count > VertexCapacity())
    return MYON_SUBD_RETURN_ERROR(false);
  const unsigned short etc = m_vertex_count_etc & MYON_SubDMeshFragment::EtcMask;
  m_vertex_count_etc = ((unsigned short)vertex_count) | etc;
  return true;
}

unsigned int MYON_SubDMeshFragment::PointCount() const
{
  return (nullptr != m_P && m_P_stride >= 3) ? VertexCount() : 0U;
}

unsigned int MYON_SubDMeshFragment::NormalCount() const
{
  return (nullptr != m_N && m_N_stride >= 3) ? VertexCount() : 0U;
}

unsigned int MYON_SubDMeshFragment::CurvatureCount() const
{
  return (nullptr != m_K) ? VertexCount() : 0U;
}

unsigned int MYON_SubDMeshFragment::ColorCount() const
{
  return (nullptr != m_C && m_C_stride >= 1) ? VertexCount() : 0U;
}

unsigned int MYON_SubDMeshFragment::PointCapacity() const
{
  return (nullptr != m_P && m_P_stride >= 3) ? VertexCapacity() : 0U;
}

unsigned int MYON_SubDMeshFragment::NormalCapacity() const
{
  return (nullptr != m_N && m_N_stride >= 3) ? VertexCapacity() : 0U;
}


unsigned int MYON_SubDMeshFragment::CurvatureCapacity() const
{
  return (nullptr != m_K) ? VertexCapacity() : 0U;
}

unsigned int MYON_SubDMeshFragment::ColorCapacity() const
{
  return (nullptr != m_C && m_C_stride >= 1) ? VertexCapacity() : 0U;
}

const double* MYON_SubDMeshFragment::PointArray(MYON_SubDComponentLocation subd_appearance)const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? &m_ctrlnetP[0][0] : m_P;
}

size_t MYON_SubDMeshFragment::PointArrayStride(MYON_SubDComponentLocation subd_appearance)const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? 3 : m_P_stride;
}

unsigned MYON_SubDMeshFragment::PointArrayCount(MYON_SubDComponentLocation subd_appearance) const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? 4U : PointCount();
}

const double* MYON_SubDMeshFragment::NormalArray(MYON_SubDComponentLocation subd_appearance)const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? m_ctrlnetN : m_N;
}

size_t MYON_SubDMeshFragment::NormalArrayStride(MYON_SubDComponentLocation subd_appearance)const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? 0 : m_N_stride;
}

unsigned MYON_SubDMeshFragment::NormalArrayCount(MYON_SubDComponentLocation subd_appearance) const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? 4U : NormalCount();
}

const MYON_SurfaceCurvature* MYON_SubDMeshFragment::CurvatureArray(MYON_SubDComponentLocation subd_appearance)const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? &m_ctrlnetK[0] : m_K;
}

unsigned MYON_SubDMeshFragment::CurvatureArrayCount(MYON_SubDComponentLocation subd_appearance) const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? 4U : CurvatureCount();
}


const MYON_Color* MYON_SubDMeshFragment::ColorArray(MYON_SubDComponentLocation subd_appearance) const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? &m_ctrlnetC[0] : m_C;
}

size_t MYON_SubDMeshFragment::ColorArrayStride(MYON_SubDComponentLocation subd_appearance) const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? 1 : m_C_stride;
}


unsigned MYON_SubDMeshFragment::ColorArrayCount(MYON_SubDComponentLocation subd_appearance) const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? 4U : ColorCount();
}

bool MYON_SubDMeshFragment::ColorsExist() const
{
  return (nullptr != m_C && m_C_stride > 0 && 0 != (m_vertex_capacity_etc & MYON_SubDMeshFragment::EtcColorsExistBit));
}

void MYON_SubDMeshFragment::SetColorsExist(bool bColorsExist) const
{
  if (bColorsExist)
    m_vertex_capacity_etc |= MYON_SubDMeshFragment::EtcColorsExistBit;
  else
    m_vertex_capacity_etc &= ~MYON_SubDMeshFragment::EtcColorsExistBit;
}

bool MYON_SubDMeshFragment::CurvaturesExist() const
{
  return (nullptr != m_K && 0 != (m_vertex_capacity_etc & MYON_SubDMeshFragment::EtcCurvaturesExistBit));
}

void MYON_SubDMeshFragment::SetCurvaturesExist(bool bSetCurvaturesExist) const
{
  if (bSetCurvaturesExist)
    m_vertex_capacity_etc |= MYON_SubDMeshFragment::EtcCurvaturesExistBit;
  else
    m_vertex_capacity_etc &= ~MYON_SubDMeshFragment::EtcCurvaturesExistBit;
}

bool MYON_SubDMeshFragment::SetColors(MYON_Color color) const
{
  bool bColorsExist = false;
  for (;;)
  {
    m_ctrlnetC[0] = color;
    m_ctrlnetC[1] = color;
    m_ctrlnetC[2] = color;
    m_ctrlnetC[3] = color;
    const MYON_SubDComponentLocation subd_appearance = MYON_SubDComponentLocation::Surface;

    const unsigned count = PointArrayCount(subd_appearance);
    if (count <= 0)
      break;
    if (count != ColorArrayCount(subd_appearance))
      break;

    MYON_Color* C = const_cast<MYON_Color*>(ColorArray(subd_appearance));
    if (nullptr == C)
      break;

    bColorsExist = (MYON_Color::UnsetColor != color);
    for (const MYON_Color* C1 = C + count; C < C1; ++C)
      *C = color;
    break;
  }
  SetColorsExist(bColorsExist);
  return ColorsExist();
}

bool MYON_SubDMeshFragment::SetColorsFromCallback(
  const MYON_MappingTag& fragment_colors_mapping_tag,
  const MYON_SubD& subd,
  MYON__UINT_PTR callback_context,
  const MYON_Color(*color_callback)(
    MYON__UINT_PTR callback_context,
    const MYON_MappingTag& mapping_tag,
    const MYON_SubD& subd,
    MYON_SubDComponentPtr cptr,
    const MYON_3dPoint& P,
    const MYON_3dVector& N,
    const MYON_3dPoint& T,
    const MYON_SurfaceCurvature& K
    )
) const
{
  bool bColorsExist = false;
  for (;;)
  {
    m_ctrlnetC[0] = MYON_Color::UnsetColor;
    m_ctrlnetC[1] = MYON_Color::UnsetColor;
    m_ctrlnetC[2] = MYON_Color::UnsetColor;
    m_ctrlnetC[3] = MYON_Color::UnsetColor;

    if (nullptr == color_callback)
    {
      // removing vertex colors.
      break;
    }

    const MYON_SubDComponentLocation subd_appearance = MYON_SubDComponentLocation::Surface;
    const unsigned count = PointArrayCount(subd_appearance);
    if (count <= 0)
      break;
    if (count != ColorArrayCount(subd_appearance))
      break;
 
    const double* P = PointArray(subd_appearance);
    const size_t Pstride = PointArrayStride(subd_appearance);
    if (nullptr == P || Pstride < 3)
      break;

    MYON_Color* C = const_cast<MYON_Color*>(ColorArray(subd_appearance));
    if (nullptr == C)
      break;

    const double nan3[3] = { MYON_DBL_QNAN, MYON_DBL_QNAN, MYON_DBL_QNAN };

    const double* N = NormalArray(subd_appearance);
    const size_t Nstride = (nullptr != N) ? NormalArrayStride(subd_appearance) : 0;
    if (nullptr == N)
      N = nan3;

    const double* T = TextureCoordinateArray(subd_appearance);
    const size_t Tstride = (nullptr != T) ? TextureCoordinateArrayStride(subd_appearance) : 0;
    if (nullptr == T)
      T = nan3;

    const MYON_SurfaceCurvature* K = CurvatureArray(subd_appearance);
    const size_t Kstride = (nullptr != K) ? 1 : 0;
    if (nullptr == K)
      K = &MYON_SurfaceCurvature::Nan;

    const MYON_SubDComponentPtr cptr = MYON_SubDComponentPtr::Create(m_face);
    for (const double* P1 = P + Pstride * count; P < P1; P += Pstride, ++C)
    {
      // Never give the callback access to fragment array pointers.
      const MYON_3dPoint callbackP(P);
      const MYON_3dVector callbackN(N);
      const MYON_3dPoint callbackT(T);
      const MYON_SurfaceCurvature callbackK(*K);
      const MYON_Color callbackC = color_callback(callback_context, fragment_colors_mapping_tag, subd, cptr, callbackP, callbackN, callbackT, callbackK);
      if (callbackC != MYON_Color::UnsetColor)
        bColorsExist = true;
      *C = callbackC;
      N += Nstride;
      T += Tstride;
      K += Kstride;
    }
    break;
  }
  
  SetColorsExist(bColorsExist);
  if (bColorsExist)
  {
    m_ctrlnetC[0] = CornerColor(0);
    m_ctrlnetC[1] = CornerColor(1);
    m_ctrlnetC[2] = CornerColor(2);
    m_ctrlnetC[3] = CornerColor(3);
  }

  return ColorsExist();
}

bool MYON_SubD::SetFragmentColorsFromCallback(
  bool bLazySet,
  MYON_SHA1_Hash fragment_colors_settings_hash,
  MYON_MappingTag fragment_colors_mapping_tag,
  MYON__UINT_PTR callback_context,
  const MYON_Color(*color_callback)(
    MYON__UINT_PTR callback_context,
    const MYON_MappingTag& mapping_tag,
    const MYON_SubD& subd,
    MYON_SubDComponentPtr cptr, 
    const MYON_3dPoint& P, 
    const MYON_3dVector& N, 
    const MYON_3dPoint& T, 
    const MYON_SurfaceCurvature& K)
) const
{
  if (bLazySet && fragment_colors_settings_hash == FragmentColorsSettingsHash())
    return true;

  bool bFragmentVetexColorsSet = false;
  const MYON_SubDimple* subdimple = this->SubDimple();
  if (nullptr != subdimple)
  {
    MYON_SubDMeshFragmentIterator fragit(*this);
    for (const MYON_SubDMeshFragment* frag = fragit.FirstFragment(); nullptr != frag; frag = fragit.NextFragment())
    {
      const bool b = frag->SetColorsFromCallback(
        fragment_colors_mapping_tag,
        *this,
        callback_context,
        color_callback
      );
      if (b)
        bFragmentVetexColorsSet = true;
    }
    if (bFragmentVetexColorsSet)
    {
      subdimple->Internal_SetFragmentColorsSettingsHash(fragment_colors_settings_hash);
      SetFragmentColorsMappingTag(fragment_colors_mapping_tag);
      ChangeRenderContentSerialNumber();
    }
    else
    {
      subdimple->Internal_SetFragmentColorsSettingsHash(MYON_SHA1_Hash::EmptyContentHash);
      this->SetFragmentColorsMappingTag(MYON_MappingTag::Unset);
    }
  }

  return bFragmentVetexColorsSet;
}

void MYON_SubD::ClearFragmentColors(
  bool bClearFragmentColorsMappingTag
)
{
  if (bClearFragmentColorsMappingTag)
    SetFragmentColorsMappingTag(MYON_MappingTag::Unset);
  SetFragmentColorsFromCallback(false, MYON_SHA1_Hash::EmptyContentHash, MYON_MappingTag::Unset, 0, nullptr );
}

const MYON_SHA1_Hash MYON_SubD::FragmentColorsSettingsHash() const
{
  const MYON_SubDimple* subdimple = this->SubDimple();
  return (nullptr != subdimple) ? subdimple->FragmentColorsSettingsHash() : MYON_SHA1_Hash::EmptyContentHash;
}

void MYON_SubD::SetFragmentColorsMappingTag(const MYON_MappingTag& colors_mapping_tag) const
{
  const MYON_SubDimple* dimple = SubDimple();
  if (nullptr != dimple)
    dimple->SetFragmentColorsMappingTag(colors_mapping_tag);
}

const MYON_MappingTag MYON_SubD::FragmentColorsMappingTag() const
{
  const MYON_SubDimple* dimple = SubDimple();
  return (nullptr != dimple) ? dimple->FragmentColorsMappingTag() : MYON_MappingTag::Unset;
}

const MYON_3dPoint MYON_SubDMeshFragment::ControlNetQuadPoint(
  bool bGridOrder,
  unsigned point_index
) const
{
  if (point_index >= 4 || 0 == (MYON_SubDMeshFragment::EtcControlNetQuadBit & m_vertex_count_etc))
    return MYON_3dPoint::NanPoint;
  if (false == bGridOrder)
  {
    if (2 == point_index)
      point_index = 3;
    else if (3 == point_index)
      point_index = 2;
  }
  return MYON_3dPoint(m_ctrlnetP[point_index]);
}

const MYON_SubDMeshFragment MYON_SubDMeshFragment::ControlNetQuadFragmentForExperts() const
{
  MYON_SubDMeshFragment q(*this);
  for (;;)
  {
    if ((m_vertex_count_etc & MYON_SubDMeshFragment::ValueMask) < 4)
      break;
    if ( 0 == (m_vertex_count_etc & MYON_SubDMeshFragment::EtcControlNetQuadBit) )
      break;

    q.m_vertex_count_etc = 4;
    q.m_vertex_count_etc |= MYON_SubDMeshFragment::EtcControlNetQuadBit;
    q.m_vertex_count_etc |= (MYON_SubDMeshFragment::EtcTextureCoordinatesExistBit&m_vertex_count_etc);

    q.m_vertex_capacity_etc = 4;
    q.m_vertex_capacity_etc |= (MYON_SubDMeshFragment::EtcColorsExistBit & m_vertex_capacity_etc);
    q.m_vertex_capacity_etc |= (MYON_SubDMeshFragment::EtcCurvaturesExistBit & m_vertex_capacity_etc);

    // 4 quad corner points in grid order with stride = 3
    q.m_P = const_cast<double*>(&m_ctrlnetP[0][0]);
    q.m_P_stride = 3;

    // 4 identical normals with stride = 0
    q.m_N = const_cast<double*>(&m_ctrlnetN[0]);
    q.m_N_stride = 0;

    // 4 quad corner texture coordinates in grid order with stride = 3
    q.m_T = const_cast<double*>(&m_ctrlnetT[0][0]);
    q.m_T_stride = 3;

    q.m_C = const_cast<MYON_Color*>(&m_ctrlnetC[0]);
    q.m_C_stride = 1;

    q.m_K = const_cast<MYON_SurfaceCurvature*>(&m_ctrlnetK[0]);

    // The grid is a single quad
    q.m_grid = MYON_SubDMeshFragmentGrid::OneQuadGrid;

    // both bounding boxes are equal.
    q.m_surface_bbox = ControlNetQuadBoundingBox();
    return q;
  }

  memset(&q, 0, sizeof(q));
  return MYON_SUBD_RETURN_ERROR(q);
}

bool MYON_SubDMeshFragment::GetControlNetQuad(
  bool bGridOrder,
  MYON_3dPoint quad_points[4],
  MYON_3dVector& quad_normal
) const
{
  const bool rc = 0 != (MYON_SubDMeshFragment::EtcControlNetQuadBit & m_vertex_count_etc) ? true : false;
  if (nullptr != quad_points)
  {
    if (rc)
    {
      int i;
      quad_points[0] = MYON_3dPoint(m_ctrlnetP[0]);
      quad_points[1] = MYON_3dPoint(m_ctrlnetP[1]);
      i = bGridOrder ? 2 : 3;
      quad_points[i] = MYON_3dPoint(m_ctrlnetP[2]);
      i = bGridOrder ? 3 : 2;
      quad_points[i] = MYON_3dPoint(m_ctrlnetP[3]);
      quad_normal = MYON_3dVector(m_ctrlnetN);
    }
    else
    {
      for (int i = 0; i < 4; i++)
        quad_points[i] = MYON_3dPoint::NanPoint;
      quad_normal = MYON_3dVector::ZeroVector;
    }
  }
  return rc;
}

const MYON_SubDMeshFragmentGrid& MYON_SubDMeshFragment::Grid(MYON_SubDComponentLocation subd_appearance) const
{
  return (MYON_SubDComponentLocation::ControlNet == subd_appearance) ? MYON_SubDMeshFragmentGrid::OneQuadGrid : m_grid;
}

void MYON_SubDMeshFragment::SetControlNetQuad(bool bGridOrder, const MYON_3dPoint quad_points[4], MYON_3dVector quad_normal )
{
  if (nullptr != quad_points && quad_points[0].IsValid() && quad_points[1].IsValid() && quad_points[2].IsValid() && quad_points[3].IsValid() && quad_normal.IsNotZero())
  {
    int i;

    m_ctrlnetP[0][0] = quad_points[0].x;
    m_ctrlnetP[0][1] = quad_points[0].y;
    m_ctrlnetP[0][2] = quad_points[0].z;

    m_ctrlnetP[1][0] = quad_points[1].x;
    m_ctrlnetP[1][1] = quad_points[1].y;
    m_ctrlnetP[1][2] = quad_points[1].z;

    i = bGridOrder ? 2 : 3;
    m_ctrlnetP[i][0] = quad_points[2].x;
    m_ctrlnetP[i][1] = quad_points[2].y;
    m_ctrlnetP[i][2] = quad_points[2].z;

    i = bGridOrder ? 3 : 2;
    m_ctrlnetP[i][0] = quad_points[3].x;
    m_ctrlnetP[i][1] = quad_points[3].y;
    m_ctrlnetP[i][2] = quad_points[3].z;

    m_ctrlnetN[0] = quad_normal.x;
    m_ctrlnetN[1] = quad_normal.y;
    m_ctrlnetN[2] = quad_normal.z;
    m_vertex_count_etc |= MYON_SubDMeshFragment::EtcControlNetQuadBit;
  }
  else
  {
    UnsetControlNetQuad();
  }
}

void MYON_SubDMeshFragment::UnsetControlNetQuad()
{
  for (int i = 0; i < 4; ++i)
  {
    m_ctrlnetP[i][0] = m_ctrlnetP[i][1] = m_ctrlnetP[i][2] = MYON_DBL_QNAN;
  }
  m_ctrlnetN[0] = m_ctrlnetN[1] = m_ctrlnetN[2] = MYON_DBL_QNAN;
  m_vertex_count_etc &= ~MYON_SubDMeshFragment::EtcControlNetQuadBit;
}

const MYON_BoundingBox MYON_SubDMeshFragment::SurfaceBoundingBox() const
{
  return m_surface_bbox;
}

const MYON_BoundingBox MYON_SubDMeshFragment::ControlNetQuadBoundingBox() const
{
  MYON_3dPoint P[4];
  MYON_3dVector N;
  if (this->GetControlNetQuad(false, P, N))
  {
    MYON_BoundingBox bbox;
    bbox.Set(3, 0, 4, 3, &P[0].x, false);
    return bbox;
  }
  return MYON_BoundingBox::NanBoundingBox;
}

const MYON_BoundingBox MYON_SubDMeshFragment::BoundingBox(MYON_SubDComponentLocation subd_appearance) const
{
  return
    (MYON_SubDComponentLocation::ControlNet == subd_appearance)
    ? ControlNetQuadBoundingBox()
    : SurfaceBoundingBox();
}


unsigned int MYON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(
  unsigned int display_density
  )
{
  if (display_density <= MYON_SubDDisplayParameters::MaximumDensity)
    return (1 << display_density);

  return MYON_SUBD_RETURN_ERROR(0);
}

unsigned int MYON_SubDMeshFragment::DisplayDensityFromSideSegmentCount(
  unsigned int side_segment_count
  )
{
  unsigned int display_density;
  for (display_density = 0; display_density < MYON_SubDDisplayParameters::MaximumDensity; display_density++)
  {
    if ( 1U << display_density >= side_segment_count )
      break;
  }

  if ( 1U << display_density == side_segment_count )
    return display_density;

  return MYON_SUBD_RETURN_ERROR(display_density);
}

unsigned int MYON_SubDMeshFragment::PointCountFromDisplayDensity(
  unsigned int mesh_density
  )
{
  unsigned int count = MYON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  if ( 0 == count)
    return 0;
  return (count + 1)*(count+1);
}

unsigned int MYON_SubDMeshFragment::FaceCountFromDisplayDensity(
  unsigned int mesh_density
  )
{
  unsigned int count = MYON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  return count*count; // same for quads or tris
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_SubDManagedMeshFragment
//

MYON_SubDManagedMeshFragment::MYON_SubDManagedMeshFragment() MYON_NOEXCEPT
{
  memset(this, 0, sizeof(*this));
}

MYON_SubDManagedMeshFragment::~MYON_SubDManagedMeshFragment() MYON_NOEXCEPT
{
  Destroy();
}

MYON_SubDManagedMeshFragment::MYON_SubDManagedMeshFragment(const MYON_SubDManagedMeshFragment& src) MYON_NOEXCEPT
{
  Clear();
  CopyHelper(src);
}

MYON_SubDManagedMeshFragment& MYON_SubDManagedMeshFragment::operator=(const MYON_SubDManagedMeshFragment& src) MYON_NOEXCEPT
{
  if (this != &src)
  {
    Clear();
    CopyHelper(src);
  }
  return *this;
}

#if defined(MYON_HAS_RVALUEREF)
// rvalue copy constructor
MYON_SubDManagedMeshFragment::MYON_SubDManagedMeshFragment( MYON_SubDManagedMeshFragment&& src ) MYON_NOEXCEPT
{
  memcpy(this, &src, sizeof(*this));
  src.m_storage = nullptr;
  src.m_storage_capacity = 0;
}

// rvalue assignment operator
MYON_SubDManagedMeshFragment& MYON_SubDManagedMeshFragment::operator=( MYON_SubDManagedMeshFragment&& src ) MYON_NOEXCEPT
{
  if (this != &src)
  {
    Destroy();
    memcpy(this, &src, sizeof(*this));
    src.m_storage = nullptr;
    src.m_storage_capacity = 0;
  }
  return *this;
}

#endif

bool MYON_SubDManagedMeshFragment::ReserveCapacity(
  unsigned int display_density
) MYON_NOEXCEPT
{
  Clear();

  if ( display_density > 8)
    return MYON_SUBD_RETURN_ERROR(false);

  unsigned int fragment_side_count = MYON_SubDManagedMeshFragment::SideSegmentCountFromDisplayDensity(display_density);
  if ( 0 == fragment_side_count )
    return true;

  // Sanity check
  if ( fragment_side_count > 0xFFU )
    return MYON_SUBD_RETURN_ERROR(false);

  const unsigned int short_capacity_sanity_check = 0xFFFFU;

  const unsigned int V_capacity = MYON_SubDManagedMeshFragment::PointCountFromDisplayDensity(display_density);
  if (V_capacity >= short_capacity_sanity_check )
    return MYON_SUBD_RETURN_ERROR(false);

  const unsigned int F_capacity = MYON_SubDManagedMeshFragment::FaceCountFromDisplayDensity(display_density);
  if ( F_capacity >= short_capacity_sanity_check)
    return MYON_SUBD_RETURN_ERROR(false);

  //const size_t P_stride = 3;
  //const size_t N_stride = 3;
  //const size_t T_stride = 3;
  //const size_t C_stride = 1;

  // storage_capacity holds = 3 point coords + 3 normal coords + 3 texture point coords + an MYON_Color + unused 4 bytes.
  size_t storage_capacity = MYON_SubDMeshFragment::ManagedDoublesPerVertex * V_capacity;
  if (storage_capacity > m_storage_capacity || nullptr == m_storage)
  {
    if (m_storage_capacity > 0 && nullptr != m_storage)
    {
      delete[] m_storage;
      m_storage = nullptr;
    }

    m_storage = new(std::nothrow) double[storage_capacity];
    if (nullptr == m_storage)
      return MYON_SUBD_RETURN_ERROR(false);
    m_storage_capacity = storage_capacity;
  }

  UnsetControlNetQuad();
  // MYON_SubDManagedMeshFragment manages m_storage and m_P[]/m_N[]/m_T[]/m_C[] are not managed by MYON_SubDMeshFragment.
  // (MYON_SubDManagedMeshFragment is a legacy class that should not be used and this confusing situation is 
  // an artifact from early code.)
  Internal_LayoutArrays(false, m_storage, V_capacity);
  SetVertexCount(V_capacity);

  m_surface_bbox = MYON_BoundingBox::NanBoundingBox;

  m_grid = MYON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(display_density,0U);
  if ( nullptr == m_grid.m_F)
    return MYON_SUBD_RETURN_ERROR(false);

  return true;
}

void MYON_SubDManagedMeshFragment::Clear() MYON_NOEXCEPT
{
  memset(this, 0, sizeof(MYON_SubDMeshFragment));
}

void MYON_SubDManagedMeshFragment::Destroy() MYON_NOEXCEPT
{
  double* p = const_cast<double*>(m_P);
  memset(this, 0, sizeof(*this));
  if ( nullptr != p)
    delete[] p;
  this->DeleteManagedCurvatureCapacity();
}

void MYON_SubDManagedMeshFragment::CopyHelper(const MYON_SubDMeshFragment& src)
{
  if (this == &src)
    return;

  m_vertex_capacity_etc = 0;
  m_vertex_count_etc = 0;
  m_P = nullptr;
  m_P_stride = 0;
  m_N = nullptr;
  m_N_stride = 0;
  m_T = nullptr;
  m_T_stride = 0;
  m_C = nullptr;
  m_C_stride = 0;
  m_surface_bbox = MYON_BoundingBox::NanBoundingBox;
  const size_t V_count = src.VertexCount();
  if (0 == V_count)
    return;

  const unsigned P_count = src.PointCount();
  if (0 != P_count && V_count != P_count)
  {
    MYON_SUBD_ERROR("invalid counts");
    return;
  }

  const unsigned N_count = src.NormalCount();
  if (0 != N_count && V_count != N_count)
  {
    MYON_SUBD_ERROR("invalid counts");
    return;
  }

  const unsigned T_count = src.TextureCoordinateCount();
  if (0 != T_count && V_count != T_count)
  {
    MYON_SUBD_ERROR("invalid counts");
    return;
  }

  const unsigned C_count = src.ColorCount();
  if (0 != C_count && V_count != C_count)
  {
    MYON_SUBD_ERROR("invalid counts");
    return;
  }

  m_ctrlnetT[0][0] = src.m_ctrlnetT[0][0];
  m_ctrlnetT[0][1] = src.m_ctrlnetT[0][1];
  m_ctrlnetT[0][2] = src.m_ctrlnetT[0][2];
  m_ctrlnetT[1][0] = src.m_ctrlnetT[1][0];
  m_ctrlnetT[1][1] = src.m_ctrlnetT[1][1];
  m_ctrlnetT[1][2] = src.m_ctrlnetT[1][2];
  m_ctrlnetT[2][0] = src.m_ctrlnetT[2][0];
  m_ctrlnetT[2][1] = src.m_ctrlnetT[2][1];
  m_ctrlnetT[2][2] = src.m_ctrlnetT[2][2];
  m_ctrlnetT[3][0] = src.m_ctrlnetT[3][0];
  m_ctrlnetT[3][1] = src.m_ctrlnetT[3][1];
  m_ctrlnetT[3][2] = src.m_ctrlnetT[3][2];

  m_ctrlnetC[0] = src.m_ctrlnetC[0];
  m_ctrlnetC[1] = src.m_ctrlnetC[1];
  m_ctrlnetC[2] = src.m_ctrlnetC[2];
  m_ctrlnetC[3] = src.m_ctrlnetC[3];

  m_ctrlnetK[0] = src.m_ctrlnetK[0];
  m_ctrlnetK[1] = src.m_ctrlnetK[1];
  m_ctrlnetK[2] = src.m_ctrlnetK[2];
  m_ctrlnetK[3] = src.m_ctrlnetK[3];

  m_pack_rect[0][0] = src.m_pack_rect[0][0];
  m_pack_rect[0][1] = src.m_pack_rect[0][1];
  m_pack_rect[1][0] = src.m_pack_rect[1][0];
  m_pack_rect[1][1] = src.m_pack_rect[1][1];
  m_pack_rect[2][0] = src.m_pack_rect[2][0];
  m_pack_rect[2][1] = src.m_pack_rect[2][1];
  m_pack_rect[3][0] = src.m_pack_rect[3][0];
  m_pack_rect[3][1] = src.m_pack_rect[3][1];

  m_surface_bbox = src.m_surface_bbox;
  m_grid = src.m_grid;

  // 10 = 3 point coordinates + 3 normal coordinates + 3 texture coordinates + a double to provide double aligned color memory
  const size_t storage_capacity = 10U * V_count;
  m_storage = new(std::nothrow) double[storage_capacity];
  if (nullptr == m_storage)
  {
    MYON_SubDIncrementErrorCount();
    return;
  }
  m_storage_capacity = storage_capacity;
  SetUnmanagedVertexCapacity(V_count);
  SetVertexCount(V_count);

  double buffer3[3];
  const double* src_p;
  size_t src_p_stride;
  double* p;
  double* p1;

  m_P = m_storage;
  m_P_stride = 3;
  p = m_P;
  p1 = p + (m_P_stride*V_count);
  if ( P_count > 0 )
  {
    src_p = src.m_P;
    src_p_stride = src.m_P_stride;
  }
  else
  {
    buffer3[0] = buffer3[1] = buffer3[2] = MYON_DBL_QNAN;
    src_p = buffer3;
    src_p_stride = 0;
  }
  while (p < p1)
  {
    p[0] = src_p[0];
    p[1] = src_p[1];
    p[2] = src_p[2];
    p += m_P_stride;
    src_p += src_p_stride;
  }

  if (N_count > 0)
  {
    src_p = src.m_N;
    src_p_stride = src.m_N_stride;
  }
  else
  {
    buffer3[0] = buffer3[1] = buffer3[2] = 0.0;
    src_p = buffer3;
    src_p_stride = 0;
  }
  m_N = m_storage + (3* V_count);
  m_N_stride = 3;
  p = m_N;
  p1 = p + (m_N_stride*V_count);
  while (p < p1)
  {
    p[0] = src_p[0];
    p[1] = src_p[1];
    p[2] = src_p[2];
    p += m_N_stride;
    src_p += src_p_stride;
  }

  if (T_count > 0)
  {
    src_p = src.m_T;
    src_p_stride = src.m_T_stride;
  }
  else
  {
    buffer3[0] = buffer3[1] = buffer3[2] = MYON_DBL_QNAN;
    src_p = buffer3;
    src_p_stride = 0;
  }
  m_T = m_storage + (6*V_count);
  m_T_stride = 3;
  p = m_T;
  p1 = p + (m_T_stride*V_count);
  while (p < p1)
  {
    p[0] = src_p[0];
    p[1] = src_p[1];
    p[2] = src_p[2];
    p += m_T_stride;
    src_p += src_p_stride;
  }

  const MYON_Color* src_c;
  size_t src_c_stride;
  if (C_count > 0)
  {
    src_c = src.m_C;
    src_c_stride = src.m_C_stride;
  }
  else
  {
    src_c = &MYON_Color::UnsetColor;
    src_c_stride = 0;
  }
  m_C = (MYON_Color*)(m_storage + (9 * V_count));
  m_C_stride = 1;
  MYON_Color* c = m_C;
  MYON_Color* c1 = c + (m_C_stride * V_count);
  while (c < c1)
  {
    *c = *src_c;
    c += m_C_stride;
    src_c += src_c_stride;
  }

  // TODO - copy m_K as needed

  MYON_3dPoint quad_points[4];
  MYON_3dVector quad_normal;
  src.GetControlNetQuad(false, quad_points, quad_normal);
  SetControlNetQuad(false, quad_points, quad_normal);
}

MYON_SubDManagedMeshFragment MYON_SubDManagedMeshFragment::Create(const MYON_SubDMeshFragment& src) MYON_NOEXCEPT
{
  MYON_SubDManagedMeshFragment mf;
  mf.CopyHelper(src);
  return mf;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMeshFragmentGrid
//

unsigned int MYON_SubDMeshFragmentGrid::SetQuads(
  unsigned int side_segment_count,
  unsigned int level_of_detail,
  unsigned int* quads,
  size_t quad_capacity,
  size_t quad_stride,
  unsigned int* sides,
  size_t side_capacity,
  size_t side_stride
)
{
  if ( false == MYON_SubDMeshFragment::SideSegmentCountIsValid(side_segment_count) )
    return MYON_SUBD_RETURN_ERROR(0);

  if ( side_segment_count <= 1 )
    level_of_detail = 0;
  else if (level_of_detail > 0)
  {
    if (level_of_detail >= 32 || 1U << level_of_detail > side_segment_count)
    {
      level_of_detail = 1;
      while ( 2*level_of_detail < side_segment_count )
        level_of_detail*= 2;
    }
  }


  const unsigned int side_point_count = (side_segment_count+1);
  const unsigned int P_di = (1 << level_of_detail);
  const unsigned int P_dj = P_di*side_point_count;

  const unsigned int side_quad_count = side_segment_count / P_di;

  if (quad_capacity > 0 || nullptr != quads)
  {
    if ( quad_stride < 4 )
      return MYON_SUBD_RETURN_ERROR(0);
    if (side_quad_count*side_quad_count > quad_capacity)
      return MYON_SUBD_RETURN_ERROR(0);

    unsigned int* fvi = quads;
    for (unsigned int n = 0; n < side_quad_count; n++)
    {
      unsigned int vi0 = n*P_dj;
      unsigned int vi1 = vi0 + P_dj;
      for (const unsigned int* fvi1 = fvi + quad_stride*side_quad_count; fvi < fvi1; fvi += quad_stride)
      {
        fvi[0] = vi0;
        fvi[3] = vi1;
        vi0 += P_di;
        vi1 += P_di;
        fvi[1] = vi0;
        fvi[2] = vi1;
      }
    }
  }

  if (side_capacity > 0 || nullptr != sides)
  {
    if ( side_stride < 1 )
      return MYON_SUBD_RETURN_ERROR(0);
    if (side_capacity < 4*side_quad_count +1 )
      return MYON_SUBD_RETURN_ERROR(0);

    unsigned int vi = 0;

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi += P_di;
    }

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi += P_dj;
    }

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi -= P_di;
    }

    for (unsigned int* sides1 = sides + side_quad_count; sides < sides1; sides += side_stride)
    {
      *sides = vi;
      vi -= P_dj;
    }

    *sides = 0;
  }

  return side_quad_count*side_quad_count;
}

bool MYON_SubDMeshFragment::SideSegmentCountIsValid(
  unsigned int side_segment_count
  )
{
  if (side_segment_count > 0 && side_segment_count <= MYON_SubDMeshFragment::MaximumSideSegmentCount)
  {
    for (unsigned int n = 1; n <= side_segment_count; n *= 2)
    {
      if (n == side_segment_count)
        return true;
    }
  }

  return MYON_SUBD_RETURN_ERROR(false);
}

unsigned int MYON_SubDMeshFragment::SidePointCountFromSideCount(
  unsigned int side_segment_count
  )
{
  return MYON_SubDMeshFragment::SideSegmentCountIsValid(side_segment_count) ? (side_segment_count+1) : 0U;
}

unsigned int MYON_SubDMeshFragment::QuadGridPointCountFromSideCount(
  unsigned int side_segment_count
  )
{
  unsigned int side_point_count = MYON_SubDMeshFragment::SidePointCountFromSideCount(side_segment_count);
  return side_point_count*side_point_count;
}

unsigned int MYON_SubDMeshFragment::QuadGridQuadCountFromSideCount(
  unsigned int side_segment_count
  )
{
  return MYON_SubDMeshFragment::SideSegmentCountIsValid(side_segment_count) ? (side_segment_count*side_segment_count) : 0U;
}

const MYON_SubDFace* MYON_SubDMeshFragment::SubDFace() const
{
  return m_face;
}

bool MYON_SubDMeshFragment::IsFullFaceFragment() const
{
  return
    nullptr != m_face
    && 4 == m_face->m_edge_count
    && 1 == m_face_fragment_count
    && 0 == m_face_fragment_index
    && 0 == m_face_vertex_index[0]
    && 1 == m_face_vertex_index[1]
    && 2 == m_face_vertex_index[2]
    && 3 == m_face_vertex_index[3]
    && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S
    ;
}

bool MYON_SubDMeshFragment::IsFaceCornerFragment() const
{
  return MYON_UNSET_UINT_INDEX != FaceCornerIndex();
}

unsigned int MYON_SubDMeshFragment::FaceCornerIndex() const
{
  return (
    nullptr != m_face
    && (3 == m_face->m_edge_count || m_face->m_edge_count >= 5)
    && m_face->m_edge_count == m_face_fragment_count
    && m_face_fragment_index < m_face_fragment_count
    && m_face_vertex_index[2] < m_face->m_edge_count
    && m_face_vertex_index[0] > MYON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[1] > MYON_SubDFace::MaximumEdgeCount
    && m_face_vertex_index[3] > MYON_SubDFace::MaximumEdgeCount
    && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S
    )
    ? ((unsigned int)m_face_vertex_index[2])
    : MYON_UNSET_UINT_INDEX
    ;
}

const MYON_SubDMeshFragment* MYON_SubDMeshFragment::FirstFaceFragment() const
{
  if (IsFullFaceFragment())
    return this;
  if (false == IsFaceCornerFragment())
    return nullptr;

  const MYON_SubDMeshFragment* first = this;
  while (nullptr != first && first->m_face_fragment_index > 0)
    first = first->PreviousFaceFragment(false);
  if (nullptr == first)
    return nullptr;
  const MYON_SubDMeshFragment* last = first->NextFaceFragment(false);
  while (nullptr != last && last->m_face_fragment_index + 1 < last->m_face_fragment_count)
    last = last->NextFaceFragment(false);
  if (nullptr == last)
    return nullptr;
  return first;
}

const MYON_SubDMeshFragment* MYON_SubDMeshFragment::LastFaceFragment() const
{
  if (IsFullFaceFragment())
    return this;
  if (false == IsFaceCornerFragment())
    return nullptr;

  const MYON_SubDMeshFragment* last = this;
  while (nullptr != last && last->m_face_fragment_index + 1 < m_face_fragment_count)
    last = last->NextFaceFragment(false);
  if (nullptr == last)
    return nullptr;
  const MYON_SubDMeshFragment* first = last->PreviousFaceFragment(false);
  while (nullptr != first && first->m_face_fragment_index > 0)
    first = first->PreviousFaceFragment(false);
  if (nullptr == first)
    return nullptr;
  return last;
}

const MYON_SubDMeshFragment* MYON_SubDMeshFragment::PreviousFaceFragment(
  bool bLastFromFirst
) const
{
  if ( nullptr != m_face && m_face_fragment_index < m_face_fragment_count )
  {
    if (bLastFromFirst && 0 == m_face_fragment_index)
    {
      return LastFaceFragment();
    }
    else if (
      m_face_fragment_index > 0
      && nullptr != m_prev_fragment
      && m_face == m_prev_fragment->m_face
      && m_face_fragment_count == m_prev_fragment->m_face_fragment_count
      && m_face_fragment_index - 1 == m_prev_fragment->m_face_fragment_index
      )
    {
      return m_prev_fragment;
    }
  }
  return nullptr;
}

const MYON_SubDMeshFragment* MYON_SubDMeshFragment::NextFaceFragment(
  bool bFirstFromLast
) const
{
  if ( nullptr != m_face && m_face_fragment_index < m_face_fragment_count )
  {
    if (bFirstFromLast && m_face_fragment_index+1 == m_face_fragment_count)
    {
      return FirstFaceFragment();
    }
    else if (
      m_face_fragment_index + 1 < m_face_fragment_count
      && nullptr != m_next_fragment
      && m_face == m_next_fragment->m_face
      && m_face_fragment_count == m_next_fragment->m_face_fragment_count
      && m_face_fragment_index + 1 == m_next_fragment->m_face_fragment_index
      )
    {
      return m_next_fragment;
    }
  }
  return nullptr;
}

unsigned int MYON_SubDMeshFragment::FaceFragmentCount() const
{
  return m_face_fragment_count;
}

unsigned int MYON_SubDMeshFragment::GetFaceFragments(
  const MYON_SubDMeshFragment** fragments,
  size_t fragments_capacity
) const
{
  const unsigned int fragment_count = FaceFragmentCount();
  if (fragments_capacity < (size_t)(fragment_count > 0 ? fragment_count : 1U))
    return 0;
  const MYON_SubDMeshFragment* fragment = FirstFaceFragment();
  for (unsigned i = 0; i < fragment_count; ++i, fragment = fragment->m_next_fragment)
  {
    if (nullptr == fragment)
      return 0;
    fragments[i] = fragment;
  }
  return fragment_count;
}

unsigned int MYON_SubDMeshFragment::GetFaceFragments(
  MYON_SimpleArray<const MYON_SubDMeshFragment*>& fragments
) const
{
  unsigned int fragment_count = FaceFragmentCount();
  fragments.SetCount(0);
  fragments.Reserve(fragment_count);
  fragment_count = GetFaceFragments(fragments.Array(), fragment_count);
  fragments.SetCapacity(fragment_count);
  return fragment_count;
}

const MYON_3dPoint MYON_SubDMeshFragment::VertexPoint(
  MYON_2udex grid2dex
) const
{
  return VertexPoint(m_grid.PointIndexFromGrid2dex(grid2dex.i, grid2dex.j));
}

const MYON_3dPoint MYON_SubDMeshFragment::VertexPoint(
  unsigned grid2dex_i,
  unsigned grid2dex_j
) const
{
  return VertexPoint(m_grid.PointIndexFromGrid2dex(grid2dex_i, grid2dex_j));
}

const MYON_3dPoint MYON_SubDMeshFragment::VertexPoint(
  unsigned grid_point_index
) const
{
  if (grid_point_index >= (unsigned)PointCount())
    return MYON_3dPoint::NanPoint;
  return MYON_3dPoint(m_P + grid_point_index * m_P_stride);
}

const MYON_3dVector MYON_SubDMeshFragment::VertexNormal(
  MYON_2udex grid2dex
) const
{
  return VertexNormal(m_grid.PointIndexFromGrid2dex(grid2dex.i, grid2dex.j));
}

const MYON_3dVector MYON_SubDMeshFragment::VertexNormal(
  unsigned grid2dex_i,
  unsigned grid2dex_j
) const
{
  return VertexNormal(m_grid.PointIndexFromGrid2dex(grid2dex_i, grid2dex_j));
}

const MYON_3dVector MYON_SubDMeshFragment::VertexNormal(
  unsigned grid_point_index
) const
{
  return
    (grid_point_index >= NormalCount())
    ? MYON_3dVector::ZeroVector
    : MYON_3dVector(m_N + grid_point_index * m_N_stride)
    ;
}

const class MYON_SubDEdge* MYON_SubDMeshFragment::SubDEdge(
  unsigned int grid_side_index
  ) const
{
  return SubDEdgePtr(grid_side_index).Edge();
}

const class MYON_SubDEdgePtr MYON_SubDMeshFragment::SubDEdgePtr(
  unsigned int grid_side_index
  ) const
{
  const unsigned int grid_side_count = 4; // will be 3 for a tri sub-D
  if (nullptr != m_face &&  m_face->m_edge_count >= 3 && grid_side_index < grid_side_count)
  {
    unsigned short fei = m_face_vertex_index[grid_side_index];
    if (fei < m_face->m_edge_count)
      return m_face->EdgePtr(fei);
    fei = m_face_vertex_index[(grid_side_index+1) % grid_side_count];
    if (fei < m_face->m_edge_count)
      return m_face->EdgePtr((fei+m_face->m_edge_count-1) % m_face->m_edge_count);
  }
  return MYON_SubDEdgePtr::Null;
}

const class MYON_SubDVertex* MYON_SubDMeshFragment::SubDVertex(
  unsigned int grid_corner_index
  ) const
{
  if (nullptr != m_face && m_face->m_edge_count >= 3 && grid_corner_index < 4)
  {
    const unsigned short fei = m_face_vertex_index[grid_corner_index];
    if (fei < m_face->m_edge_count)
      return m_face->Vertex(fei);
  }
  return nullptr;
}

const MYON_3dPoint MYON_SubDMeshFragment::CornerPoint(
  unsigned int grid_corner_index
  ) const
{
  if ( grid_corner_index >= 4 || nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return MYON_3dPoint::NanPoint;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return MYON_3dPoint(m_P + (i*m_P_stride));
}

const MYON_Color MYON_SubDMeshFragment::CornerColor(
  unsigned int grid_corner_index
) const
{
  if (grid_corner_index >= 4 || nullptr == m_C || m_C_stride <= 0 || nullptr == m_grid.m_S)
    return MYON_Color::UnsetColor;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return m_C[i * m_C_stride];
}

const MYON_SurfaceCurvature MYON_SubDMeshFragment::CornerCurvature(unsigned int grid_corner_index) const
{
  if (grid_corner_index >= 4 || nullptr == m_K || nullptr == m_grid.m_S)
    return MYON_SurfaceCurvature::Nan;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return m_K[i];
}

const MYON_3dPoint MYON_SubDMeshFragment::TextureCoordinateCorner(
  unsigned int grid_corner_index
) const
{
  return
    (grid_corner_index < 4)
    ? MYON_3dPoint(m_ctrlnetT[grid_corner_index][0], m_ctrlnetT[grid_corner_index][1], m_ctrlnetT[grid_corner_index][2])
    : MYON_3dPoint::NanPoint;
}

const MYON_2dPoint MYON_SubDMeshFragment::PackRectCorner(
  unsigned int grid_corner_index
) const
{
  return 
    (grid_corner_index < 4)
    ? MYON_2dPoint(m_pack_rect[grid_corner_index][0], m_pack_rect[grid_corner_index][1])
    : MYON_2dPoint::NanPoint;
}

const MYON_2dPoint  MYON_SubDMeshFragment::PackRectCenter() const
{
  return MYON_2dPoint(
    0.25 * (m_pack_rect[0][0] + m_pack_rect[1][0] + m_pack_rect[2][0] + m_pack_rect[3][0]),
    0.25 * (m_pack_rect[0][1] + m_pack_rect[1][1] + m_pack_rect[2][1] + m_pack_rect[3][1])
  );
}


const MYON_3dVector MYON_SubDMeshFragment::CornerNormal(unsigned int grid_corner_index) const
{
  if ( grid_corner_index >= 4 || nullptr == m_N || m_N_stride <= 0 || nullptr == m_grid.m_S )
    return MYON_3dPoint::NanPoint;

  const unsigned int i = m_grid.m_S[grid_corner_index * m_grid.m_side_segment_count];

  return MYON_3dVector(m_N + (i*m_N_stride));
}

const MYON_3dPoint MYON_SubDMeshFragment::SidePoint(unsigned int grid_side_index) const
{
  if ( grid_side_index >= 4 || nullptr == m_P || m_P_stride <= 0 || nullptr == m_grid.m_S )
    return MYON_3dPoint::NanPoint;

  const unsigned int i = grid_side_index*m_grid.m_side_segment_count + m_grid.m_side_segment_count/2;

  return MYON_3dPoint(m_P + (i*m_P_stride));
}

const MYON_3dVector MYON_SubDMeshFragment::SideNormal(unsigned int grid_side_index) const
{
  if ( grid_side_index >= 4 || nullptr == m_N || m_N_stride <= 0 || nullptr == m_grid.m_S )
    return MYON_3dPoint::NanPoint;

  const unsigned int i = grid_side_index*m_grid.m_side_segment_count + m_grid.m_side_segment_count/2;

  return MYON_3dVector(m_N + (i*m_N_stride));
}

const MYON_3dPoint MYON_SubDMeshFragment::CenterPoint() const
{
  if ( nullptr == m_P || m_P_stride < 3 || m_grid.m_side_segment_count <= 0 || nullptr == m_grid.m_S )
    return MYON_3dPoint::NanPoint;

  if (1 == m_grid.m_side_segment_count)
  {
    return MYON_3dPoint(
      0.25*(m_P[0] + m_P[m_P_stride] + m_P[2 * m_P_stride] + m_P[3 * m_P_stride]),
      0.25*(m_P[1] + m_P[1+m_P_stride] + m_P[1 + 2 * m_P_stride] + m_P[1 + 3 * m_P_stride]),
      0.25*(m_P[2] + m_P[2+m_P_stride] + m_P[2 + 2 * m_P_stride] + m_P[2 + 3 * m_P_stride])
    );
  }

  // otherwise there is an actual point at the center of the grid.
  const unsigned int i = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;
  return MYON_3dPoint(m_P + (i*m_P_stride));
}

const MYON_3dVector MYON_SubDMeshFragment::CenterNormal() const
{
  if (nullptr == m_N || (m_N_stride != 0 && m_N_stride < 3) || m_grid.m_side_segment_count <= 0 || nullptr == m_grid.m_S)
    return MYON_3dVector::NanVector;

  if (1 == m_grid.m_side_segment_count)
  {
    const MYON_3dVector N(
      (m_N[0] + m_N[m_N_stride] + m_N[2 * m_N_stride] + m_N[3 * m_N_stride]),
      (m_N[1] + m_N[1 + m_N_stride] + m_N[1 + 2 * m_N_stride] + m_N[1 + 3 * m_N_stride]),
      (m_N[2] + m_N[2 + m_N_stride] + m_N[2 + 2 * m_N_stride] + m_N[2 + 3 * m_N_stride])
    );
    return N.UnitVector();
  }

  const unsigned int i = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;

  return MYON_3dVector(m_N + (i*m_N_stride));
}


const MYON_3dPoint MYON_SubDMeshFragment::CenterTextureCoordinate() const
{
  if (nullptr == m_T || (m_T_stride != 0 && m_T_stride < 3) || m_grid.m_side_segment_count <= 0 || nullptr == m_grid.m_S)
    return MYON_3dPoint::NanPoint;

  if (1 == m_grid.m_side_segment_count)
  {
    const MYON_3dPoint T(
      (m_T[0] + m_T[m_T_stride] + m_T[2 * m_T_stride] + m_T[3 * m_T_stride]),
      (m_T[1] + m_T[1 + m_T_stride] + m_T[1 + 2 * m_T_stride] + m_T[1 + 3 * m_T_stride]),
      (m_T[2] + m_T[2 + m_T_stride] + m_T[2 + 2 * m_T_stride] + m_T[2 + 3 * m_T_stride])
    );
    return T;
  }

  const unsigned int i = (m_grid.m_side_segment_count * (m_grid.m_side_segment_count + 2)) / 2;

  return MYON_3dPoint(m_T + (i * m_T_stride));
}

bool MYON_SubDMeshFragment::Internal_GetFrameHelper(
  unsigned int P_dex,
  unsigned int Q_dex,
  MYON_Plane& frame
) const
{
  const unsigned int P_count = PointCount();
  if ( P_dex < P_count
    && Q_dex < P_count
    && NormalCount() == PointCount()
    )
  {
    const MYON_3dPoint P(VertexPoint(P_dex));
    const MYON_3dVector Z(VertexNormal(P_dex));
    if (P.IsValid() && Z.IsNotZero())
    {
      const MYON_3dPoint Q(VertexPoint(Q_dex));
      const MYON_3dVector V = (Q - P).UnitVector();
      const MYON_3dVector X = (V - (Z*V)*Z).UnitVector();
      const MYON_3dVector Y = MYON_CrossProduct(Z, X).UnitVector();
      if ( X.IsUnitVector() && Y.IsUnitVector() 
        && fabs(X*Z) <= MYON_SQRT_EPSILON
        && fabs(Y*Z) <= MYON_SQRT_EPSILON
        )
      {
        frame.origin = P;
        frame.xaxis = X;
        frame.yaxis = Y;
        frame.zaxis = Z;
        frame.UpdateEquation();
      }
      else
      {
        frame = MYON_Plane(P, Z);
      }
      return true;
    }
  }
  return false;
}


const MYON_Plane MYON_SubDMeshFragment::CornerFrame(
  unsigned int grid_corner_index
) const
{
  if (grid_corner_index < 4 && m_grid.m_side_segment_count > 0 && nullptr != m_grid.m_S)
  {
    unsigned int S_dex = grid_corner_index * m_grid.m_side_segment_count;
    MYON_Plane corner_frame;
    if (Internal_GetFrameHelper(m_grid.m_S[S_dex], m_grid.m_S[S_dex + 1], corner_frame))
      return corner_frame;
  }

  return MYON_Plane::NanPlane;
}

const MYON_Plane MYON_SubDMeshFragment::SideFrame(unsigned int grid_side_index) const
{
  const unsigned int count = m_grid.m_side_segment_count;
  if (grid_side_index < 4U && count > 0 && nullptr != m_grid.m_S)
  {
    const unsigned int S_dex = grid_side_index * count + count / 2U;
    const unsigned int S_dex1 = (S_dex < (4U * count)) ? (S_dex + 1U) : (S_dex - 1U);
    MYON_Plane side_frame;
    if (Internal_GetFrameHelper(m_grid.m_S[S_dex], m_grid.m_S[S_dex1], side_frame))
      return side_frame;
  }

  return MYON_Plane::NanPlane;
}


const bool MYON_SubDMeshFragment::HasValidPointAndNormalGrid() const
{
  return
    nullptr != m_P
    && m_P_stride >= 3
    && nullptr != m_N
    && ( 0 == m_N_stride || m_N_stride >= 3)
    && m_grid.m_side_segment_count > 0
    && nullptr != m_grid.m_S
    ;
}

const MYON_Plane MYON_SubDMeshFragment::CenterFrame() const
{
  if (false == HasValidPointAndNormalGrid() )
    return MYON_Plane::NanPlane;

  if (1 == m_grid.m_side_segment_count)
  {
    const MYON_3dPoint P(
      0.25*(m_P[0] + m_P[m_P_stride] + m_P[2 * m_P_stride] + m_P[3 * m_P_stride]),
      0.25*(m_P[1] + m_P[1 + m_P_stride] + m_P[1 + 2 * m_P_stride] + m_P[1 + 3 * m_P_stride]),
      0.25*(m_P[2] + m_P[2 + m_P_stride] + m_P[2 + 2 * m_P_stride] + m_P[2 + 3 * m_P_stride])
    );
    if( false == (P.x == P.x))
      return MYON_Plane::NanPlane;
    MYON_3dVector N = MYON_3dVector(
      (m_N[0] + m_N[m_N_stride] + m_N[2 * m_N_stride] + m_N[3 * m_N_stride]),
      (m_N[1] + m_N[1 + m_N_stride] + m_N[1 + 2 * m_N_stride] + m_N[1 + 3 * m_N_stride]),
      (m_N[2] + m_N[2 + m_N_stride] + m_N[2 + 2 * m_N_stride] + m_N[2 + 3 * m_N_stride])
    ).UnitVector();
    if (false == N.IsUnitVector())
      N = MYON_3dVector(m_N).UnitVector();
    if ( N.IsUnitVector() )
    {
      MYON_Plane center_frame(P, N);
      const MYON_3dPoint Q(
        0.5*(m_P[m_P_stride] + m_P[3 * m_P_stride]),
        0.5*(m_P[1 + m_P_stride] + m_P[1 + 3 * m_P_stride]),
        0.5*(m_P[2 + m_P_stride] + m_P[2 + 3 * m_P_stride])
      );
      const MYON_3dVector V = (Q - P).UnitVector();
      const MYON_3dVector X = (V - (N*V)*V).UnitVector();
      if (X.IsUnitVector())
      {
        center_frame.xaxis = X;
        center_frame.yaxis = MYON_CrossProduct(N, X);
      }
      return center_frame;
    }
  }
  else
  {
    const unsigned int P_dex = (m_grid.m_side_segment_count*(m_grid.m_side_segment_count + 2)) / 2;
    MYON_Plane center_frame;
    if (Internal_GetFrameHelper(P_dex, P_dex + 1, center_frame))
      return center_frame;
  }

  return MYON_Plane::NanPlane;
}

const class MYON_SubDVertexPtr MYON_SubDMeshFragment::SubDVertexPtr(
  unsigned int grid_corner_index
  ) const
{
  return MYON_SubDVertexPtr::Create(SubDVertex(grid_corner_index));
}

MYON_ComponentStatus MYON_SubDMeshFragment::Status() const
{
  return (nullptr == m_face) ? MYON_ComponentStatus::NoneSet : m_face->m_status;
}


unsigned int MYON_SubDMeshFragmentGrid::LevelOfDetail() const
{
  // A better name for this value is Display Density Reduction.
  // Identical to MYON_SubDMeshFragmentGrid::DisplayDensityReduction().
  return m_F_level_of_detail;
}

unsigned int MYON_SubDMeshFragmentGrid::DisplayDensityReduction() const
{
  return m_F_level_of_detail;
}

unsigned int MYON_SubDMeshFragmentGrid::DisplayDensity() const
{
  unsigned int side_segment_count = SideSegmentCount();
  if (0 == side_segment_count)
    return MYON_UNSET_UINT_INDEX;
  unsigned int display_density = 0;
  while ((side_segment_count /= 2) > 0)
    display_density++;
  return display_density;
}

unsigned int MYON_SubDMeshFragmentGrid::SideSegmentCount() const
{
  return m_side_segment_count;
  //unsigned int side_segment_count = 1;
  //while( side_segment_count*side_segment_count < m_F_count)
  //  side_segment_count *= 2;
  //return (side_segment_count*side_segment_count == m_F_count) ? side_segment_count : 0;
}

unsigned int MYON_SubDMeshFragmentGrid::SidePointCount() const
{
  const unsigned int i = SideSegmentCount();
  return (i > 0U) ? (i + 1U) : 0U;
}

unsigned int MYON_SubDMeshFragmentGrid::GridFaceCount() const
{
  // TODO: Suport tri fragments
  unsigned int side_segment_count = SideSegmentCount();
  return side_segment_count * side_segment_count;
}


unsigned int MYON_SubDMeshFragmentGrid::GridPointCount() const
{
  // TODO: Suport tri fragments
  unsigned int side_segment_count = SideSegmentCount();
  return (side_segment_count > 0U) ? ((side_segment_count + 1U)*(side_segment_count + 1U)) : 0U;
}

unsigned int MYON_SubDMeshFragmentGrid::GridId() const
{
  for (;;)
  {
    if (nullptr != m_F)
      break;

    // m_F_count = 2^(2n)
    for (unsigned int id = 0; id <= 16; id += 2)
    {
      if ((1U << id) == m_F_count)
      {
        id /= 2; // id = "n"
        unsigned int lod = (m_F_level_of_detail > id) ? id : (unsigned int)m_F_level_of_detail;
        id = 32*id + 2*lod;
        return id;
      }
    }
  }

  return 0;
}

const MYON_2udex MYON_SubDMeshFragmentGrid::Grid2dexFromPointIndex(
  unsigned int grid_point_index
)  const
{
  for (;;)
  {
    // On a quad face, first grid points run from face->Vertex(0) to face->Vertex(1).
    // The "i" grid index increases from face->Vertex(0) to face->Vertex(1).
    // The "j" grid index increases from face->Vertex(0) to face->Vertex(3).
    const unsigned int side_segment_count = SideSegmentCount();
    if (0 == side_segment_count)
      break;
    const unsigned int grid_side_point_count = side_segment_count + 1;
    if (grid_point_index >= grid_side_point_count * grid_side_point_count)
      break;

    // CORRECT - do not change
    const MYON_2udex griddex(
      grid_point_index % grid_side_point_count,
      grid_point_index / grid_side_point_count
    );

    return griddex;
  }
  return MYON_SUBD_RETURN_ERROR(MYON_2udex::Unset);
}

unsigned int MYON_SubDMeshFragmentGrid::PointIndexFromGrid2dex(
  unsigned int i,
  unsigned int j
)  const
{
  for (;;)
  {
    const unsigned int side_segment_count = SideSegmentCount();
    if (0 == side_segment_count)
      break;
    const unsigned int grid_side_point_count = side_segment_count + 1;
    if (i >= grid_side_point_count && j >= grid_side_point_count)
      break;
    // BAD BUG Oct 2020 // return grid_side_point_count * i + j;
    // Fixed Oct 13, 2020
    return i + grid_side_point_count * j; // CORRECT - do not change
  }
  return MYON_UNSET_UINT_INDEX;
}


bool MYON_SubDMeshFragmentGrid::GetGridParameters(
  unsigned int grid_point_index,
  double grid_parameters[2]
  ) const
{
  for (;;)
  {
    const unsigned int side_segment_count = SideSegmentCount();
    if ( 0 == side_segment_count )
      break;
    const unsigned int grid_side_point_count = side_segment_count + 1;
    if (grid_point_index >= grid_side_point_count*grid_side_point_count)
      break;
    const MYON_2udex g2dex = Grid2dexFromPointIndex(grid_point_index);

    grid_parameters[0]
      = (g2dex.i < side_segment_count)
      ? (((double)g2dex.i) / ((double)side_segment_count))
      : 1.0;
    grid_parameters[1]
      = (g2dex.j < side_segment_count)
      ? (((double)g2dex.j) / ((double)side_segment_count))
      : 1.0;
    return true;
  }
  grid_parameters[0] = MYON_UNSET_VALUE;
  grid_parameters[1] = MYON_UNSET_VALUE;
  return false;
}


MYON_SubDMeshFragmentGrid MYON_SubDMeshFragmentGrid::QuadGridFromSideSegmentCount(
  unsigned int side_segment_count,
  unsigned int mesh_density_reduction
)
{
  const unsigned int display_density = MYON_SubDMeshFragment::DisplayDensityFromSideSegmentCount(side_segment_count);
  if ( side_segment_count != MYON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(display_density) )
    return MYON_SUBD_RETURN_ERROR(MYON_SubDMeshFragmentGrid::Empty);
  return MYON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(display_density, mesh_density_reduction);
}


MYON_SubDMeshFragmentGrid MYON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(
  unsigned int display_density,
  unsigned int mesh_density_reduction
  )
{
  static const MYON_SubDMeshFragmentGrid* grid_cache[9] = { 0 }; // side_segment_count = 1,2,4,8,16,32,64,128,256

  if ( ((size_t)display_density) >= sizeof(grid_cache)/sizeof(grid_cache[0]))
    return MYON_SUBD_RETURN_ERROR(MYON_SubDMeshFragmentGrid::Empty);

  const MYON_SubDMeshFragmentGrid* fragment_grid = grid_cache[display_density];
  if (nullptr != fragment_grid)
  {
    while ( fragment_grid->m_F_level_of_detail < mesh_density_reduction && nullptr != fragment_grid->m_next_level_of_detail)
      fragment_grid = fragment_grid->m_next_level_of_detail;
    return *fragment_grid;
  }

  // The code below is thread safe and constructs the MYON_SubDLimitMeshFragmentGrids
  // that are resources shared by all MYON_SubDLimitMeshFragments.

  static MYON_SleepLock lock;
  const bool bReturnLock = lock.GetLock(50,MYON_SleepLock::OneMinute,true);

  // try again in case cache was made while waiting
  fragment_grid = grid_cache[display_density];
  if (nullptr != fragment_grid)
  {
    lock.ReturnLock();
    while ( fragment_grid->m_F_level_of_detail < mesh_density_reduction && nullptr != fragment_grid->m_next_level_of_detail)
      fragment_grid = fragment_grid->m_next_level_of_detail;
    return *fragment_grid;
  }

  // The MYON_SubDMeshFragmentGrid classes created below are created one time as needed
  // and used millions of times after that. These are app workspace allocations
  // and not memory leaks. Once a grid exists, it is saved in grid_cache[] and returned
  // above the next zillion times it is required.
  MYON_MemoryAllocationTracking disable_tracking(false);

  // make the requested grid
  unsigned int quad_capacity = 0;
  unsigned int side_segment_capacity = 0;
  unsigned int grid_count = 0;
  unsigned int grid_cache_index0 = display_density;
  unsigned int grid_cache_index1 = display_density;
  if (display_density <= MYON_SubDDisplayParameters::DefaultDensity)
  {
    // make all the common small grids
    grid_cache_index0 = 0;
    grid_cache_index1 = MYON_SubDDisplayParameters::DefaultDensity;
  }
  for (unsigned int i = grid_cache_index0; i <= grid_cache_index1; i++)
  {
    // allocate all levels of detail for each segment side count
    unsigned int s1 = (1U << i);
    for (unsigned int s2 = s1; s2 > 0; s2 /= 2)
    {
      quad_capacity += MYON_SubDMeshFragment::QuadGridQuadCountFromSideCount(s2);
      side_segment_capacity += 4*s2 + 1;
      grid_count++;
    }
  }


  const unsigned int vdex_capacity = (quad_capacity*4 + side_segment_capacity);
  size_t sz1 = grid_count*sizeof(MYON_SubDMeshFragmentGrid);
  size_t sz2 = vdex_capacity*sizeof(unsigned int);
  if (0 != sz2 % sizeof(MYON_SubDMeshFragmentGrid))
    sz2 = (1 + sz2/sizeof(MYON_SubDMeshFragmentGrid))*sizeof(MYON_SubDMeshFragmentGrid);
  MYON_SubDMeshFragmentGrid* grids = new (std::nothrow) MYON_SubDMeshFragmentGrid[(sz1 + sz2)/sizeof(MYON_SubDMeshFragmentGrid)];

  MYON_SubDMeshFragmentGrid grid = MYON_SubDMeshFragmentGrid::Empty;
  grid.m_F_stride = 4;
  unsigned int* vdex0 = (unsigned int*)(grids + grid_count);
  unsigned int* vdex1 = vdex0 + vdex_capacity;
  unsigned int* vdex = vdex0;

  for (unsigned int i = grid_cache_index0; i <= grid_cache_index1; i++)
  {
    const unsigned int s1 = (1U << i);
    MYON_SubDMeshFragmentGrid* first_lod = grids;
    MYON_SubDMeshFragmentGrid* prev_lod = nullptr;
    grid.m_F_level_of_detail = 0;
    for (unsigned int s2 = s1; s2 > 0; s2 /= 2, grids++)
    {
      const unsigned int grid_F_capacity = MYON_SubDMeshFragment::QuadGridQuadCountFromSideCount(s2);
      const unsigned int grid_S_capacity = 4*s2 + 1;

      grid.m_side_segment_count = (unsigned char)s2;
      grid.m_F_count = (unsigned short)grid_F_capacity;
      grid.m_F = vdex;
      vdex += 4*grid_F_capacity;
      grid.m_S = vdex;
      vdex += grid_S_capacity;

      if (vdex > vdex1)
      {
        MYON_SubDIncrementErrorCount();
        break;
      }

      MYON_SubDMeshFragmentGrid::SetQuads(
        s1, // top level side_segment_count
        grid.m_F_level_of_detail,
        const_cast<unsigned int*>(grid.m_F),
        grid_F_capacity,
        grid.m_F_stride,
        const_cast<unsigned int*>(grid.m_S),
        grid_S_capacity,
        1U
        );

      *grids = grid;
      if ( nullptr != prev_lod )
      {
        grids->m_prev_level_of_detail = prev_lod;
        prev_lod->m_next_level_of_detail = grids;
      }
      prev_lod = grids;

      grid.m_F += (grid.m_F_count*grid.m_F_stride);
      grid.m_F_level_of_detail++;
    }

    // Do not initialize grid_cache[i] until entire linked list is ready to be used.
    // This way if the lock is stolen for some unforseen reason, we risk leaking memory
    // but we will not crash.
    grid_cache[i] = first_lod;
  }

  if ( bReturnLock )
    lock.ReturnLock();

  if (vdex != vdex1)
  {
    MYON_SubDIncrementErrorCount();
  }

  fragment_grid = grid_cache[display_density];
  if (nullptr != fragment_grid)
  {
    while ( fragment_grid->m_F_level_of_detail < mesh_density_reduction && nullptr != fragment_grid->m_next_level_of_detail)
      fragment_grid = fragment_grid->m_next_level_of_detail;
    return *fragment_grid;
  }

  return MYON_SUBD_RETURN_ERROR(MYON_SubDMeshFragmentGrid::Empty);
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMesh
//

void MYON_SubDMeshImpl::ClearFragmentFacePointers(
  bool bResetSubDWeakPtr
)
{
  // When the MYON_SubDimple that manages the faces referenced by
  // fragment->m_face is deleted, fragment->m_face must be set to zero.
  if (bResetSubDWeakPtr)
    m_subdimple_wp.reset();
  if (nullptr != m_first_fragment && nullptr != m_first_fragment->m_face)
  {
    for (auto fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
      fragment->m_face = nullptr;
  }
}
//
//bool MYON_SubDMeshImpl::ReserveCapacityx(
//  const class MYON_SubD& subd,
//  MYON_SubDDisplayParameters limit_mesh_parameters
//  )
//{
//  const unsigned int level_index = limit_mesh_parameters.m_level_index;
//
//  unsigned int subd_fragment_count = subd.MeshFragmentCount(level_index);
//  if ( subd_fragment_count < 1 )
//    return MYON_SUBD_RETURN_ERROR(false);
//
//  MYON_SubD::FacetType facet_type = MYON_SubD::FacetTypeFromSubDType(subd.LevelSubDType(level_index));
//
//  return ReserveCapacity(
//    subd_fragment_count,
//    facet_type,
//    limit_mesh_parameters.m_display_density,
//    level_index);
//}

bool MYON_SubDMeshImpl::ReserveCapacity(
  unsigned int subd_fragment_count,
  unsigned int absolute_subd_display_density
  )
{
  ClearTree();

  m_absolute_subd_display_density = 0;
  m_fragment_count = 0;
  m_fragment_point_count = 0;
  m_first_fragment = nullptr;

  if (absolute_subd_display_density > MYON_SubDDisplayParameters::MaximumDensity)
    return MYON_SUBD_RETURN_ERROR(false);

  unsigned int fragment_point_count = MYON_SubDMeshFragment::PointCountFromDisplayDensity(absolute_subd_display_density);
  if ( subd_fragment_count < 1 )
    return MYON_SUBD_RETURN_ERROR(false);

  // note sizeof(double) = 2*sizeof(MYON_Color) so we have
  // room for 3d poitns, 3d normals, 3d texture points, a color array
  // and a currently unused array of 32 bit elements.
  size_t sizeof_PNTC = MYON_SubDMeshFragment::ManagedDoublesPerVertex*fragment_point_count*sizeof(double);
  size_t sizeof_fragment = sizeof(MYON_SubDMeshFragment);
  if (0 != sizeof_fragment % sizeof(double))
  {
    sizeof_fragment = (1 + sizeof_fragment / sizeof(double))*sizeof(double);
  }

  if( false == m_fsp.Create(sizeof_fragment + sizeof_PNTC,subd_fragment_count,0))
    return MYON_SUBD_RETURN_ERROR(false);

  m_absolute_subd_display_density = absolute_subd_display_density;
  m_fragment_point_count = fragment_point_count;

  return true;
}

size_t MYON_SubDMeshFragment::SizeofFragment(
  unsigned int display_density,
  bool bCurvatureArray
)
{
  if (display_density > MYON_SubDDisplayParameters::MaximumDensity)
    return MYON_SUBD_RETURN_ERROR(0);

  const unsigned side_seg_count = MYON_SubDMeshFragment::SideSegmentCountFromDisplayDensity(display_density);
  if (side_seg_count < 1)
    return MYON_SUBD_RETURN_ERROR(0);

  const unsigned vertex_capacity = (side_seg_count + 1)*(side_seg_count + 1);
  size_t sz = sizeof(MYON_SubDMeshFragment);
  while (0 != (sz % sizeof(double)))
    ++sz;

  const size_t doubles_per_vertex
    = 10 // 6 = 3+3 = 3d points in m_P[] and 3d vectors in m_N[] and 3d points in m_T[] and a color in m_C[] (sizeof(m_C[0]) <= sizeof(double))
    + (size_t)((bCurvatureArray ? sizeof(MYON_SurfaceCurvature)/sizeof(double) : 0))
    ;
  const size_t sizeof_doubles = doubles_per_vertex * sizeof(double);
  sz += vertex_capacity * sizeof_doubles;
  return sz;
}

bool MYON_SubDMeshFragment::CopyFrom(
  const MYON_SubDMeshFragment& src_fragment
)
{
  unsigned int display_density = MYON_UNSET_UINT_INDEX;
  return MYON_SubDMeshFragment::CopyFrom(src_fragment, display_density);
}

bool MYON_SubDMeshFragment::CopyFrom(
  const MYON_SubDMeshFragment& src_fragment,
  unsigned int display_density
  )
{
  m_face = src_fragment.m_face;
  m_face_vertex_index[0] = src_fragment.m_face_vertex_index[0];
  m_face_vertex_index[1] = src_fragment.m_face_vertex_index[1];
  m_face_vertex_index[2] = src_fragment.m_face_vertex_index[2];
  m_face_vertex_index[3] = src_fragment.m_face_vertex_index[3];
  m_face_fragment_count = src_fragment.m_face_fragment_count;
  m_face_fragment_index = src_fragment.m_face_fragment_index;
  m_vertex_count_etc = 0;
  m_grid = MYON_SubDMeshFragmentGrid::Empty;
  m_surface_bbox = MYON_BoundingBox::NanBoundingBox;
  // Note well: Do not change capacity on this.
  SetVertexCount(0);

  MYON_3dPoint quad_points[4];
  MYON_3dVector quad_normal;
  src_fragment.GetControlNetQuad(false, quad_points, quad_normal);
  SetControlNetQuad(false, quad_points, quad_normal);

  m_ctrlnetT[0][0] = src_fragment.m_ctrlnetT[0][0];
  m_ctrlnetT[0][1] = src_fragment.m_ctrlnetT[0][1];
  m_ctrlnetT[0][2] = src_fragment.m_ctrlnetT[0][2];
  m_ctrlnetT[1][0] = src_fragment.m_ctrlnetT[1][0];
  m_ctrlnetT[1][1] = src_fragment.m_ctrlnetT[1][1];
  m_ctrlnetT[1][2] = src_fragment.m_ctrlnetT[1][2];
  m_ctrlnetT[2][0] = src_fragment.m_ctrlnetT[2][0];
  m_ctrlnetT[2][1] = src_fragment.m_ctrlnetT[2][1];
  m_ctrlnetT[2][2] = src_fragment.m_ctrlnetT[2][2];
  m_ctrlnetT[3][0] = src_fragment.m_ctrlnetT[3][0];
  m_ctrlnetT[3][1] = src_fragment.m_ctrlnetT[3][1];
  m_ctrlnetT[3][2] = src_fragment.m_ctrlnetT[3][2];

  m_ctrlnetC[0] = src_fragment.m_ctrlnetC[0];
  m_ctrlnetC[1] = src_fragment.m_ctrlnetC[1];
  m_ctrlnetC[2] = src_fragment.m_ctrlnetC[2];
  m_ctrlnetC[3] = src_fragment.m_ctrlnetC[3];

  m_ctrlnetK[0] = src_fragment.m_ctrlnetK[0];
  m_ctrlnetK[1] = src_fragment.m_ctrlnetK[1];
  m_ctrlnetK[2] = src_fragment.m_ctrlnetK[2];
  m_ctrlnetK[3] = src_fragment.m_ctrlnetK[3];

  m_pack_rect[0][0] = src_fragment.m_pack_rect[0][0];
  m_pack_rect[0][1] = src_fragment.m_pack_rect[0][1];
  m_pack_rect[1][0] = src_fragment.m_pack_rect[1][0];
  m_pack_rect[1][1] = src_fragment.m_pack_rect[1][1];
  m_pack_rect[2][0] = src_fragment.m_pack_rect[2][0];
  m_pack_rect[2][1] = src_fragment.m_pack_rect[2][1];
  m_pack_rect[3][0] = src_fragment.m_pack_rect[3][0];
  m_pack_rect[3][1] = src_fragment.m_pack_rect[3][1];

  if ( display_density > 8 && MYON_UNSET_UINT_INDEX != display_density)
    return MYON_SUBD_RETURN_ERROR(false);

  const unsigned src_V_count = src_fragment.VertexCount();
  if ( 0 == src_V_count || src_V_count != src_fragment.m_grid.GridPointCount() )
    return MYON_SUBD_RETURN_ERROR(false);

  if (src_V_count != src_fragment.PointCount())
    return MYON_SUBD_RETURN_ERROR(false);

  const MYON_SubDMeshFragmentGrid grid
    = (MYON_UNSET_UINT_INDEX == display_density)
    ? src_fragment.m_grid
    : MYON_SubDMeshFragmentGrid::QuadGridFromDisplayDensity(display_density,0);
  const unsigned V_count = grid.GridPointCount();
  if (VertexCapacity() < V_count)
  {
    if (false == this->ReserveManagedVertexCapacity(V_count))
      return MYON_SUBD_RETURN_ERROR(false);
  }
  if ( V_count > VertexCapacity() || V_count > src_fragment.VertexCount())
    return MYON_SUBD_RETURN_ERROR(false);

  m_grid = grid;

  double* p = m_P;
  size_t p_stride = m_P_stride;
  const double* src_p = src_fragment.m_P;
  size_t src_p_stride = src_fragment.m_P_stride;
  const double* src_p1 = src_p + src_p_stride * src_V_count;
  if (V_count == src_V_count)
  {
    // Most common case where srf_fragment and target have the same density - faster way to copy.

    // copy m_P[]
    while (src_p < src_p1)
    {
      p[0] = src_p[0];
      p[1] = src_p[1];
      p[2] = src_p[2];
      p += p_stride;
      src_p += src_p_stride;
    }

    if (V_count <= NormalCapacity() )
    {
      if (V_count == src_fragment.NormalCount())
      {
        // copy m_N[]
        p = m_N;
        p_stride = m_N_stride;
        src_p = src_fragment.m_N;
        src_p_stride = src_fragment.m_N_stride;
        src_p1 = src_p + src_p_stride * src_V_count;
        while (src_p < src_p1)
        {
          p[0] = src_p[0];
          p[1] = src_p[1];
          p[2] = src_p[2];
          p += p_stride;
          src_p += src_p_stride;
        }
      }
      else
      {
        MYON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_N, V_count, m_N_stride);
      }
    }

    if (V_count <= TextureCoordinateCapacity())
    {
      if (V_count == src_fragment.TextureCoordinateCount())
      {
        // copy m_T[]
        SetTextureCoordinatesExist(true);
        p = m_T;
        p_stride = m_T_stride;
        src_p = src_fragment.m_T;
        src_p_stride = src_fragment.m_T_stride;
        src_p1 = src_p + src_p_stride * src_V_count;
        while (src_p < src_p1)
        {
          p[0] = src_p[0];
          p[1] = src_p[1];
          p[2] = src_p[2];
          p += p_stride;
          src_p += src_p_stride;
        }
      }
      else
      {
        MYON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_T, V_count, m_T_stride);
      }
    }
  }
  else
  {
    // src_fragment is more dense than target. Copy a subset of the points.
    //
    // This code appears to be rarely used. It is not well tested.
    //
    const unsigned int s0 = grid.SideSegmentCount();
    const unsigned int s1 = src_fragment.m_grid.SideSegmentCount();
    unsigned int x = 1;
    while (x*s0 < s1)
      x *= 2;
    const unsigned int m = src_fragment.m_grid.SidePointCount();
    const unsigned int di = src_fragment.m_grid.PointIndexFromGrid2dex(x, 0);
    const unsigned int dj = src_fragment.m_grid.PointIndexFromGrid2dex(0, x);

    // copy from src_fragment.m_P[] to this->m_P[]
    size_t i_stride = di * src_fragment.m_P_stride;
    size_t j_stride = m * dj * src_fragment.m_P_stride;
    for (unsigned int j = 0; j < m; j += dj)
    {
      src_p = src_fragment.m_P + j * j_stride;
      for (unsigned int i = 0; i < m; i += di)
      {
        p[0] = src_p[0];
        p[1] = src_p[1];
        p[2] = src_p[2];
        p += p_stride;
        src_p += i_stride;
      }
    }

    if (V_count <= NormalCapacity())
    {
      // copy from src_fragment.m_N[] to this->m_N[]
      if (V_count <= src_fragment.NormalCount())
      {
        p = m_N;
        p_stride = m_N_stride;
        i_stride = di * src_fragment.m_N_stride;
        j_stride = m * dj * src_fragment.m_N_stride;
        for (unsigned int j = 0; j < m; j += dj)
        {
          src_p = src_fragment.m_N + j * j_stride;
          for (unsigned int i = 0; i < m; i += di)
          {
            p[0] = src_p[0];
            p[1] = src_p[1];
            p[2] = src_p[2];
            p += p_stride;
            src_p += i_stride;
          }
        }
      }
      else
      {
        MYON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_N, V_count, m_N_stride);
      }
    }

    if (V_count <= TextureCoordinateCapacity())
    {
      // copy from src_fragment.m_T[] to this->m_T[]
      if (V_count <= src_fragment.TextureCoordinateCount())
      {
        SetTextureCoordinatesExist(true);
        p = m_T;
        p_stride = m_T_stride;
        i_stride = di * src_fragment.m_T_stride;
        j_stride = m * dj * src_fragment.m_T_stride;
        for (unsigned int j = 0; j < m; j += dj)
        {
          src_p = src_fragment.m_T + j * j_stride;
          for (unsigned int i = 0; i < m; i += di)
          {
            p[0] = src_p[0];
            p[1] = src_p[1];
            p[2] = src_p[2];
            p += p_stride;
            src_p += i_stride;
          }
        }
      }
      else
      {
        MYON_SubDMeshFragment::Internal_Set3dPointArrayToNan(m_T, V_count, m_T_stride);
      }
    }

    if (V_count <= ColorCapacity())
    {
      // copy from src_fragment.m_C[] to this->m_C[]
      if (V_count <= src_fragment.ColorCount())
      {
        SetColorsExist(true);
        MYON_Color*c = m_C;
        const size_t c_stride = m_C_stride;
        i_stride = di * src_fragment.m_C_stride;
        j_stride = m * dj * src_fragment.m_C_stride;
        for (unsigned int j = 0; j < m; j += dj)
        {
          const MYON_Color* src_c = src_fragment.m_C + j * j_stride;
          for (unsigned int i = 0; i < m; i += di)
          {
            *c = *src_c;
            c += c_stride;
            src_c += i_stride;
          }
        }
      }
      else
      {
        MYON_Color* c = m_C;
        for (MYON_Color* c1 = c + V_count * m_C_stride; c < c1; c += m_C_stride)
          *c = MYON_Color::UnsetColor;
      }
    }
  }

  SetVertexCount(V_count);
  m_surface_bbox = src_fragment.m_surface_bbox;

  return true;
}

MYON_SubDMeshFragment* MYON_SubDMeshImpl::CopyCallbackFragment(
    const MYON_SubDMeshFragment* callback_fragment
    )
{
  if ( nullptr == callback_fragment)
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( 0 == callback_fragment->VertexCount() )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( callback_fragment->VertexCount() > m_fragment_point_count )
    return MYON_SUBD_RETURN_ERROR(nullptr);
  if ( 0 == callback_fragment->PointCount() || 0 == callback_fragment->NormalCount() )
    return MYON_SUBD_RETURN_ERROR(nullptr);

  // m_fsp.AllocateElement() allocates room for the fragment and the m_P[], m_N[], m_T[], and m_C[] arrays.
  MYON_SubDMeshFragment* fragment = (MYON_SubDMeshFragment*)m_fsp.AllocateElement();
  if ( nullptr == fragment)
    return MYON_SUBD_RETURN_ERROR(nullptr);

  // The memory for the arrays in this case is managed by m_fsp[].
  fragment->Internal_LayoutArrays(false, (double*)(fragment + 1), m_fragment_point_count);
  fragment->CopyFrom(*callback_fragment);

  ChangeContentSerialNumber();

  return fragment;
}

bool MYON_SubDMeshImpl::AddFinishedFragment(
  MYON_SubDMeshFragment* finished_fragment
  )
{
  if ( nullptr == finished_fragment)
    return MYON_SUBD_RETURN_ERROR(false);
  if ( 0 == finished_fragment->PointCount() || 0 == finished_fragment->NormalCount() )
    return MYON_SUBD_RETURN_ERROR(false);
  if ( finished_fragment->PointCount() > m_fragment_point_count )
    return MYON_SUBD_RETURN_ERROR(false);

  m_fragment_count++;

  if (nullptr == m_first_fragment)
  {
    m_first_fragment = finished_fragment;
    m_last_fragment = finished_fragment;
    m_bbox = finished_fragment->m_surface_bbox;
  }
  else
  {
    m_last_fragment->m_next_fragment = finished_fragment;
    finished_fragment->m_prev_fragment = m_last_fragment;
    m_last_fragment = finished_fragment;
    m_bbox.Union(finished_fragment->m_surface_bbox);
  }

  ChangeContentSerialNumber();

  return true;
}

class MYON_SubDLimitMeshSealEdgeInfo
{
public:
  MYON_SubDLimitMeshSealEdgeInfo() = default;
  ~MYON_SubDLimitMeshSealEdgeInfo() = default;
  MYON_SubDLimitMeshSealEdgeInfo(const MYON_SubDLimitMeshSealEdgeInfo&) = default;
  MYON_SubDLimitMeshSealEdgeInfo& operator=(const MYON_SubDLimitMeshSealEdgeInfo&) = default;

  static const MYON_SubDLimitMeshSealEdgeInfo Unset;

  enum Bits : unsigned char
  {
    // Set if edge orientation is opposite face boundary ccw orientation.
    EdgeDir = 0x01,

    // Set if the edge is smooth and normals should be sealed along with location.
    Smooth = 0x02,

    // The high bits are used for half edges so they will sort after full edges
    FirstHalf = 0x40,
    SecondHalf = 0x80,
    HalfMask = 0xC0,

    // Used to ignore edge dir
    IgnoreEdgeDirMask = 0xFE
  };

  bool SetEdge(
    unsigned int grid_side_dex
  )
  {
    for (;;)
    {
      if (nullptr == m_fragment || nullptr == m_fragment->m_face)
        break;

      m_face_edge_count = m_fragment->m_face->m_edge_count;
      if (m_face_edge_count < 3 || m_face_edge_count > MYON_SubDFace::MaximumEdgeCount)
        break; // bogus face

      const MYON_SubDEdgePtr eptr = m_fragment->SubDEdgePtr(grid_side_dex);
      const MYON_SubDEdge* edge = eptr.Edge();
      m_edge_id = (nullptr != edge && edge->m_face_count >= 2) ? edge->m_id : 0;
      if (0 == m_edge_id)
        break; // nothing to seal

      const bool bCompleteFragment = m_fragment->IsFullFaceFragment();
      const bool bPartialFragment
        = (false == bCompleteFragment)
        && m_fragment->m_face_vertex_index[0] > MYON_SubDFace::MaximumEdgeCount
        && m_fragment->m_face_vertex_index[1] > MYON_SubDFace::MaximumEdgeCount
        && m_fragment->m_face_vertex_index[2] < MYON_SubDFace::MaximumEdgeCount
        && m_fragment->m_face_vertex_index[3] > MYON_SubDFace::MaximumEdgeCount
        ;

      if (false == bCompleteFragment && false == bPartialFragment)
      {
        MYON_SUBD_ERROR("Unexpected m_face_vertex[] falues in partial fragment.");
        break;
      }

      m_bits = 0;
      const MYON__UINT_PTR edge_dir = eptr.EdgeDirection();
      if (bPartialFragment)
      {
        // The high bit is used for partial fragments so they will sort after full fragments.
        if ( 1 == grid_side_dex )
          m_bits |= (0==edge_dir) ? Bits::SecondHalf : Bits::FirstHalf;
        else if ( 2 == grid_side_dex )
          m_bits |= (0==edge_dir) ? Bits::FirstHalf : Bits::SecondHalf;
        else
        {
          // this is an inteior edge of a partial fragment and it
          // is always sealed with its neighbor when it is created.
          break;
        }
      }
      if (0 != edge_dir)
        m_bits |= Bits::EdgeDir;
      if (edge->IsSmooth())
        m_bits |= Bits::Smooth;

      m_grid_side_dex = (unsigned char)grid_side_dex; // 0,1,2, or 3

      return true;
    }


    m_edge_id = 0;
    m_bits = 0;
    m_grid_side_dex = 0;
    m_face_edge_count = 0;
    return false;
  }

  static bool Seal(
    const MYON_SubDLimitMeshSealEdgeInfo& src,
    const MYON_SubDLimitMeshSealEdgeInfo& dst
  );


  static int CompareEdgeIdBitsFaceId(
    const MYON_SubDLimitMeshSealEdgeInfo* lhs,
    const MYON_SubDLimitMeshSealEdgeInfo* rhs
  )
  {
    // sort by edge id
    if (lhs->m_edge_id < rhs->m_edge_id)
      return -1;
    if (lhs->m_edge_id > rhs->m_edge_id)
      return 1;

    // then sort by bits with full edges before half edges
    // Critical to ignore the EdgeDir bit because we need
    // the identical sections of edges to be sorted together.
    // When an edge has 2 N-gons (N != 4) attached, we need
    // the pairs for the first half and 2nd half sorted toether
    const unsigned char lhs_bits = (lhs->m_bits & MYON_SubDLimitMeshSealEdgeInfo::Bits::IgnoreEdgeDirMask);
    const unsigned char rhs_bits = (rhs->m_bits & MYON_SubDLimitMeshSealEdgeInfo::Bits::IgnoreEdgeDirMask);
    if (lhs_bits < rhs_bits)
      return -1;
    if (lhs_bits > rhs_bits)
      return 1;

    // then sort by face id
    unsigned int lhs_face_id = (nullptr != lhs->m_fragment->m_face) ? lhs->m_fragment->m_face->m_id : 0xFFFFFFFF;
    unsigned int rhs_face_id = (nullptr != rhs->m_fragment->m_face) ? rhs->m_fragment->m_face->m_id : 0xFFFFFFFF;
    if (0 == lhs_face_id)
      lhs_face_id = 0xFFFFFFFE;
    if (0 == rhs_face_id)
      rhs_face_id = 0xFFFFFFFE;
    if (lhs_face_id < rhs_face_id)
      return -1;
    if (lhs_face_id > rhs_face_id)
      return 1;

    return 0;
  }

public:
  unsigned int m_edge_id = 0;

  // m_bits is bitwise or of MYON_SubDLimitMeshSealEdgeInfo::Bits enum values
  unsigned char m_bits = 0;
  unsigned char m_grid_side_dex = 0; // 0,1,2,or 3
  unsigned short m_face_edge_count = 0;

  MYON_SubDMeshFragment* m_fragment = nullptr;
};

const MYON_SubDLimitMeshSealEdgeInfo MYON_SubDLimitMeshSealEdgeInfo::Unset;

bool MYON_SubDLimitMeshSealEdgeInfo::Seal(
  const MYON_SubDLimitMeshSealEdgeInfo& src,
  const MYON_SubDLimitMeshSealEdgeInfo& dst
)
{
  if (src.m_edge_id != dst.m_edge_id || 0 == src.m_edge_id)
    return false;
  if (nullptr == src.m_fragment || nullptr == dst.m_fragment)
    return false;
  const bool bSealNormals = (0 != (src.m_bits & MYON_SubDLimitMeshSealEdgeInfo::Bits::Smooth));
  const unsigned char src_half = (src.m_bits & MYON_SubDLimitMeshSealEdgeInfo::Bits::HalfMask);
  const unsigned char dst_half = (dst.m_bits & MYON_SubDLimitMeshSealEdgeInfo::Bits::HalfMask);
  unsigned int src_side_segment_count = src.m_fragment->m_grid.m_side_segment_count;
  unsigned int dst_side_segment_count = dst.m_fragment->m_grid.m_side_segment_count;
  unsigned int i0 = src.m_grid_side_dex*src_side_segment_count;
  unsigned int i1 = i0 + src_side_segment_count;

  ////unsigned int vid[2] = {};
  ////const MYON_SubDEdge* e = src.m_fragment->Edge(src.m_grid_side_dex);
  ////if (nullptr != e )
  ////{
  ////  const MYON_SubDVertex* v = src.m_fragment->Vertex(src.m_grid_side_dex);
  ////  if ( nullptr != v )
  ////    vid[0] = v->m_id;
  ////  v = src.m_fragment->Vertex((src.m_grid_side_dex+1)%4);
  ////  if ( nullptr !=v )
  ////    vid[1] = v->m_id;
  ////}

  // src_dir = 0 if SubD edge and fragment side have compatible natural orientations
  const unsigned char src_dir = (src.m_bits&MYON_SubDLimitMeshSealEdgeInfo::Bits::EdgeDir);

  if (src_half != dst_half || src_side_segment_count != dst_side_segment_count)
  {
    if (
      0 == src_half
      && 4 == src.m_face_edge_count
      && 4 != dst.m_face_edge_count
      && 2 * dst_side_segment_count == src_side_segment_count)
    {
      // The face for src_half is a quad and the face for dest_half is an N-gon with N != 3,
      // and src_fragment is a full sized fragment and dst_fragment is a half sized fragment.
      if (MYON_SubDLimitMeshSealEdgeInfo::Bits::FirstHalf == dst_half)
      {
        // only copy half of src_fragment side
        if (0 == src_dir)
        {
          i1 -= dst_side_segment_count; // copy first half of src_framgent side
          ////vid[1] = 0;
        }
        else
        {
          i0 += dst_side_segment_count; // copy 2nd half of src_framgent side
          ////vid[0] = 0;
        }
      }
      else if (MYON_SubDLimitMeshSealEdgeInfo::Bits::SecondHalf == dst_half)
      {
        // only copy half of src_fragment side
        if (0 == src_dir)
        {
          i0 += dst_side_segment_count; // copy 2nd half of src_framgent side
          ////vid[0] = 0;
        }
        else
        {
          i1 -= dst_side_segment_count; // copy first half of src_framgent side
          ////vid[1] = 0;
        }
      }
      else
      {
        // bug in this code or the code that sets the m_bits information.
        MYON_SUBD_ERROR("unexpected dst_half");
        return false;
      }
    }
    else
    {
      // Either the parent subd is invalid or information
      // set in the fragments, or the sorting in MYON_SubDMeshImpl::SealEdges()
      // is not valid (or some other bug).
      MYON_SUBD_ERROR("unexpected sealing fragment portions");
      return false;
    }
  }

  // seal this edge
  const bool bSameDirection = (src_dir == (dst.m_bits&MYON_SubDLimitMeshSealEdgeInfo::Bits::EdgeDir));
  const unsigned int j0 = dst.m_grid_side_dex*dst_side_segment_count + (bSameDirection?0:dst_side_segment_count);
  const unsigned int j1 = bSameDirection ? (j0+dst_side_segment_count) : (j0-dst_side_segment_count);
  MYON_SubDMeshFragment::SealAdjacentSides(
    true, // bTestNearEqual,
    bSealNormals,
    *src.m_fragment,
    i0,
    i1,
    *dst.m_fragment,
    j0,
    j1
  );
  return true;
}


void MYON_SubDMeshImpl::SealEdges()
{
  MYON_SimpleArray<MYON_SubDLimitMeshSealEdgeInfo> fe_list(m_fragment_count);
  MYON_SubDLimitMeshSealEdgeInfo fe;
  for (fe.m_fragment = m_first_fragment; nullptr != fe.m_fragment; fe.m_fragment = fe.m_fragment->m_next_fragment)
  {
    if (nullptr == fe.m_fragment->m_face)
      continue;
    for (unsigned int grid_side_dex = 0; grid_side_dex < 4; grid_side_dex++)
    {
      if ( fe.SetEdge(grid_side_dex) )
        fe_list.Append(fe);
    }
  }

  fe_list.QuickSort(MYON_SubDLimitMeshSealEdgeInfo::CompareEdgeIdBitsFaceId);
  const unsigned int fe_list_count = fe_list.UnsignedCount();
  unsigned int i0 = 0;
  while ( i0 < fe_list_count )
  {
    fe = fe_list[i0];
    const unsigned char src_half_mask = (fe.m_bits & MYON_SubDLimitMeshSealEdgeInfo::Bits::HalfMask);
    for ( i0++; i0 < fe_list_count && fe.m_edge_id == fe_list[i0].m_edge_id; i0++ )
    {
      if (0 != src_half_mask && 0 == (src_half_mask & fe_list[i0].m_bits))
        break; // necessary when all faces attached to an edge are not quads.
      MYON_SubDLimitMeshSealEdgeInfo::Seal(fe, fe_list[i0]);
    }
  }
}

MYON__UINT64 MYON_SubDMesh::ContentSerialNumber() const
{
  MYON_SubDMeshImpl* imple = SubLimple();
  return (nullptr != imple) ? imple->ContentSerialNumber() : 0;
}

MYON__UINT64 MYON_SubDMeshImpl::ContentSerialNumber() const
{
  return m_mesh_content_serial_number;
}

MYON__UINT64 MYON_SubDMeshImpl::ChangeContentSerialNumber()
{
  return (m_mesh_content_serial_number = MYON_NextContentSerialNumber());
}

MYON_SubDMeshImpl::MYON_SubDMeshImpl()
{
  ChangeContentSerialNumber();
}

MYON_SubDMeshImpl::MYON_SubDMeshImpl( const class MYON_SubDMeshImpl& src )
{
  ChangeContentSerialNumber();
  if ( nullptr != src.m_first_fragment && ReserveCapacity((unsigned int)src.m_fsp.ActiveElementCount(), src.m_absolute_subd_display_density) )
  {
    for (const MYON_SubDMeshFragment* src_fragment = src.m_first_fragment; nullptr != src_fragment; src_fragment = src_fragment->m_next_fragment)
    {
      MYON_SubDMeshFragment* fragment = CopyCallbackFragment(src_fragment);
      AddFinishedFragment(fragment);
    }
  }
}

void MYON_SubDMeshImpl::ClearTree()
{
  if (nullptr != m_fragment_tree)
  {
    delete m_fragment_tree;
    m_fragment_tree = nullptr;
  }
}

const MYON_RTree& MYON_SubDMeshImpl::FragmentTree() const
{
  if (nullptr != m_fragment_tree && nullptr != m_first_fragment)
  {
    MYON_RTree* fragment_tree = new MYON_RTree();
    for (const MYON_SubDMeshFragment* fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
    {
      if (fragment->PointCount() > 0 )
        fragment_tree->Insert(&(fragment->m_surface_bbox.m_min.x), &(fragment->m_surface_bbox.m_max.x), (void*)fragment);
    }
    const_cast< MYON_SubDMeshImpl* >(this)->m_fragment_tree = fragment_tree;
  }
  return (nullptr == m_fragment_tree ) ? MYON_RTree::Empty : *m_fragment_tree;
}

bool MYON_SubDMeshImpl::GetTightBoundingBox(
  MYON_BoundingBox& bbox,
  bool bGrowBox,
  const MYON_Xform* xform
  ) const
{
  MYON_BoundingBox local_bbox = MYON_BoundingBox::EmptyBoundingBox;
  if (nullptr != xform && xform->IsIdentity())
    xform = nullptr;

  if (nullptr == xform)
  {
    local_bbox = m_bbox;
  }
  else
  {
    for (const MYON_SubDMeshFragment* fragment = m_first_fragment; nullptr != fragment; fragment = fragment->m_next_fragment)
      MYON_GetPointListBoundingBox(3, 0, fragment->PointCount(), (int)fragment->m_P_stride, fragment->m_P, local_bbox, fragment != m_first_fragment, xform);
  }

  if (bGrowBox && bbox.IsValid())
    bbox.Union(local_bbox);
  else
    bbox = local_bbox;

  return true;
}

#if defined(MYON_HAS_RVALUEREF)
MYON_SubDMesh::MYON_SubDMesh( MYON_SubDMesh&& src) MYON_NOEXCEPT
  : m_impl_sp(std::move(src.m_impl_sp))
{}

MYON_SubDMesh& MYON_SubDMesh::operator=(MYON_SubDMesh&& src)
{
  if (this != &src)
  {
    m_impl_sp.reset();
    m_impl_sp = std::move(src.m_impl_sp);
  }
  return *this;
}
#endif

MYON_SubDDisplayParameters MYON_SubDMesh::DisplayParameters() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
  {
    return MYON_SubDDisplayParameters::CreateFromAbsoluteDisplayDensity(impl->m_absolute_subd_display_density);
  }
  return MYON_SubDDisplayParameters::Empty;
}

unsigned int MYON_SubDMesh::DisplayDensity() const
{
  return AbsoluteSubDDisplayDensity();
}

unsigned int MYON_SubDMesh::AbsoluteSubDDisplayDensity() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_absolute_subd_display_density
    : 0;
}

unsigned int MYON_SubDMesh::FragmentCount() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_fragment_count
    : 0;
}

const MYON_SubDMeshFragment* MYON_SubDMesh::FirstFragment() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr != impl)
    ? impl->m_first_fragment
    : nullptr;
}

const MYON_SubDMeshFragment* MYON_SubDMesh::FaceFragment(
  const class MYON_SubDFace* face
) const
{
  if (nullptr == face)
    return nullptr;
  for (const MYON_SubDMeshFragment* fragment = FirstFragment(); nullptr != fragment; fragment = fragment->m_next_fragment)
  {
    if (face == fragment->m_face)
      return fragment;
  }
  return nullptr;
}

bool MYON_SubDMesh::GetFaceCenterPointAndNormal(
  const class MYON_SubDFace* face,
  double* P,
  double* N
) const
{
  if (nullptr != P)
  {
    P[0] = MYON_DBL_QNAN;
    P[1] = MYON_DBL_QNAN;
    P[2] = MYON_DBL_QNAN;
  }
  if (nullptr != N)
  {
    N[0] = MYON_DBL_QNAN;
    N[1] = MYON_DBL_QNAN;
    N[2] = MYON_DBL_QNAN;
  }
  const MYON_SubDMeshFragment* fragment = FaceFragment(face);
  if (nullptr == fragment)
    return false;
  if (nullptr == fragment->m_P || nullptr == fragment->m_N)
    return false;
  const unsigned int n = fragment->m_grid.m_side_segment_count;
  const unsigned int P_dex = fragment->IsFullFaceFragment() ? (n*(n + 2) / 2) : 0;
  if (P_dex >= fragment->PointCount())
    return false;
  const double* fragment_P = fragment->m_P + (P_dex * fragment->m_P_stride);
  const double* fragment_N = fragment->m_N + (P_dex * fragment->m_N_stride);
  if (nullptr != P)
  {
    P[0] = fragment_P[0];
    P[1] = fragment_P[1];
    P[2] = fragment_P[2];
  }
  if (nullptr != N)
  {
    N[0] = fragment_N[0];
    N[1] = fragment_N[1];
    N[2] = fragment_N[2];
  }
  return true;
}


bool MYON_SubDMesh::GetEdgeCenterPointAndNormal(
  const class MYON_SubDEdge* edge,
  unsigned int edge_face_index,
  double* P,
  double* N
) const
{
  if (nullptr != P)
  {
    P[0] = MYON_DBL_QNAN;
    P[1] = MYON_DBL_QNAN;
    P[2] = MYON_DBL_QNAN;
  }
  if (nullptr != N)
  {
    N[0] = MYON_DBL_QNAN;
    N[1] = MYON_DBL_QNAN;
    N[2] = MYON_DBL_QNAN;
  }
  if (nullptr == edge)
    return false;
  const MYON_SubDFace* face = edge->Face(edge_face_index);
  if (nullptr == face)
    return false;
  const unsigned int fei = face->EdgeArrayIndex(edge);
  if (fei >= face->EdgeCount())
    return false;

  unsigned int P_dex = MYON_UNSET_UINT_INDEX;

  bool bIsPartialFragment = false;
  const MYON_SubDMeshFragment* fragment = nullptr;
  for (
    fragment =  FaceFragment(face);
    nullptr != fragment && fragment->m_face == face;
    fragment = bIsPartialFragment ? fragment->m_next_fragment : nullptr
    )
  {
    bIsPartialFragment = fragment->IsFaceCornerFragment();
    for (unsigned int grid_side_index = 0; grid_side_index < 4; grid_side_index++)
    {
      const MYON_SubDEdge* grid_side_edge = fragment->SubDEdge(grid_side_index);
      if (edge != grid_side_edge)
        continue;
      const unsigned int n = fragment->m_grid.m_side_segment_count;
      if (n <= 0 || nullptr == fragment->m_grid.m_S)
        break;
      if (bIsPartialFragment)
      {
        const MYON_SubDVertex* v = fragment->SubDVertex(grid_side_index);
        if (nullptr != v)
        {
          const unsigned int evi = (0 == face->EdgeDirection(fei)) ? 0U : 1U;
          if (v == edge->Vertex(evi))
            grid_side_index++;
          P_dex = fragment->m_grid.m_S[grid_side_index*n];
        }
      }
      else if (fragment->IsFullFaceFragment())
      {
        P_dex = fragment->m_grid.m_S[grid_side_index*n + n / 2];
      }
      break;
    }
    if (false == bIsPartialFragment || MYON_UNSET_UINT_INDEX != P_dex)
      break;
  }
  if (nullptr == fragment)
    return false;
  if (P_dex >= fragment->PointCount())
    return false;
  const double* fragment_P = fragment->m_P + (P_dex * fragment->m_P_stride);
  const double* fragment_N = fragment->m_N + (P_dex * fragment->m_N_stride);
  if (nullptr != P)
  {
    P[0] = fragment_P[0];
    P[1] = fragment_P[1];
    P[2] = fragment_P[2];
  }
  if (nullptr != N)
  {
    N[0] = fragment_N[0];
    N[1] = fragment_N[1];
    N[2] = fragment_N[2];
  }
  return true;
}

bool MYON_SubDMesh::Update(
  bool bShareUpdate
  )
{
  return false;
}

bool MYON_SubDMesh::IsEmpty() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  return (nullptr == impl || 0 == impl->m_fragment_count );
}

const MYON_RTree& MYON_SubDMesh::FragmentTree() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->FragmentTree();
  return MYON_RTree::Empty;
}

MYON_BoundingBox MYON_SubDMesh::BoundingBox() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->m_bbox;
  return MYON_BoundingBox::EmptyBoundingBox;
}

MYON_SubD MYON_SubDMesh::SubD() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  if ( nullptr == impl )
    return MYON_SubD::Empty;
  MYON_SubD subd;
  subd.ShareDimple(*impl);
  return subd;
}

MYON_SubDRef MYON_SubDMesh::SubDRef() const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  if ( nullptr == impl )
    return MYON_SubDRef::Empty;
  MYON_SubDRef subd_ref;
  MYON_SubD& subd = subd_ref.NewSubD();
  subd.ShareDimple(*impl);
  return subd_ref;
}

bool MYON_SubDMesh::GetTightBoundingBox(
  MYON_BoundingBox& bbox,
  bool bGrowBox,
  const MYON_Xform* xform
  ) const
{
  const MYON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    return impl->GetTightBoundingBox(bbox,bGrowBox,xform);
  return (bGrowBox && bbox.IsValid());
}

void MYON_SubDMesh::Clear()
{
  m_impl_sp.reset();
}

void MYON_SubDMesh::ClearTree()
{
  MYON_SubDMeshImpl* impl = m_impl_sp.get();
  if (nullptr != impl)
    impl->ClearTree();
}

MYON_SubDMesh& MYON_SubDMesh::CopyFrom(
  const MYON_SubDMesh& src
  )
{
  if (this != &src)
  {
    m_impl_sp.reset();
    const MYON_SubDMeshImpl* src_impl = src.m_impl_sp.get();
    if (nullptr != src_impl)
    {
      MYON_SubDMeshImpl* impl = new MYON_SubDMeshImpl(*src_impl);
      std::shared_ptr< MYON_SubDMeshImpl > new_impl_sp(impl);
      m_impl_sp.swap(new_impl_sp);
    }
  }
  return *this;
}

void MYON_SubDMesh::Swap(
  MYON_SubDMesh& a,
  MYON_SubDMesh& b
  )
{
  if (&a == &MYON_SubDMesh::Empty || &b == &MYON_SubDMesh::Empty)
  {
    MYON_SubDIncrementErrorCount();
  }
  else
  {
    std::swap(a.m_impl_sp, b.m_impl_sp);
  }
}

void MYON_SubDDisplayParameters::Dump(class MYON_TextLog& text_log) const
{
  const unsigned display_density = DisplayDensity(MYON_SubD::Empty);
  if (this->DisplayDensityIsAbsolute())
    text_log.Print(L"Absolute DisplayDensity = %u", display_density);
  else
    text_log.Print(L"Adaptive DisplayDensity = %u", display_density);

  switch (display_density)
  {
  case MYON_SubDDisplayParameters::UnsetDensity:
    text_log.Print(L" (UnsetDensity)");
    break;
  case MYON_SubDDisplayParameters::ExtraCoarseDensity:
    text_log.Print(L" (ExtraCoarse)");
    break;
  case MYON_SubDDisplayParameters::CoarseDensity:
    text_log.Print(L" (Coarse)");
    break;
  case MYON_SubDDisplayParameters::MediumDensity:
    text_log.Print(L" (Medium)");
    break;
  case MYON_SubDDisplayParameters::FineDensity:
    text_log.Print(L" (Fine)");
    break;
  case MYON_SubDDisplayParameters::ExtraFineDensity:
    text_log.Print(L" (ExtraFine)");
    break;
  case MYON_SubDDisplayParameters::MaximumDensity:
    text_log.Print(L" (Maximum)");
    break;
  }
  text_log.PrintNewLine();

  text_log.Print(L"MeshLocation = ");
  switch (MeshLocation())
  {
  case MYON_SubDComponentLocation::Unset:
    text_log.Print(L"Unset\n");
    break;
  case MYON_SubDComponentLocation::ControlNet:
    text_log.Print(L"ControlNet\n");
    break;
  case MYON_SubDComponentLocation::Surface:
    text_log.Print(L"Surface\n");
    break;
  default:
    text_log.Print(L"invalid (%u)\n",static_cast<unsigned char>(MeshLocation()));
    break;
  }
}

unsigned int MYON_SubDDisplayParameters::AbsoluteDisplayDensityFromSubDFaceCount(
  unsigned adaptive_subd_display_density,
  unsigned subd_face_count
)
{
  // The returned density must always be >= 2 so we can reliably calculate 
  // NURBS curve forms of SubD edge curves in that begin/end at an
  // extraordinary vertex. This is done in
  // MYON_SubDEdge::GetEdgeSurfaceCurveControlPoints().
  // When there is a better way to get NURBS approsimations of edges that end
  // at extraordinary vertices, we can reduce min_display_density to 1.
  // 
  // 2022-06-08. Pierre, RH-62025. I have no idea why this is set to 1 when
  // MYON_SubDEdge::GetEdgeSurfaceCurveControlPoints() has not changed.
  // Most of the SubD to NURBS code expects the display density to be >= 2.
  // However, setting it to 2 would force SubDs with >32k faces to be meshed
  // with a display density >= 2. Meshing and NURBSing parameters should not
  // be set in the same code!
  // I'm changing this to MYON_SubDDisplayParameters::MinimumAdaptiveDensity (== 1) to 
  // have some explanation for the value, and setting an absolute display density
  // of 2 in SubD to NURBS code.
  const unsigned min_display_density = MYON_SubDDisplayParameters::MinimumAdaptiveDensity; // adaptive cutoff

#if 1
  if (adaptive_subd_display_density <= min_display_density)
    // 2022-06-08. Pierre, RH-62025. This used to return adaptive_subd_display_density,
    // completely obviating the point of min_display_density.
    return min_display_density;
  if (adaptive_subd_display_density > MYON_SubDDisplayParameters::MaximumDensity)
    adaptive_subd_display_density = MYON_SubDDisplayParameters::DefaultDensity;

  const unsigned max_mesh_quad_count = MYON_SubDDisplayParameters::AdaptiveDisplayMeshQuadMaximum;
  unsigned absolute_display_density = adaptive_subd_display_density;
  unsigned mesh_quad_count = (1 << (2 * absolute_display_density)) * subd_face_count;
  while (absolute_display_density > min_display_density && mesh_quad_count > max_mesh_quad_count)
  {
    --absolute_display_density;
    mesh_quad_count /= 4;
  }
  return absolute_display_density;
#else

#if !defined(MYON_DEBUG)
  // This insures an accidental commit will never get merged.
#error NEVER COMMIT THIS CODE!
#endif

  // used to test contanst densities when debugging.
  return min_display_density;
  //return 3;
  //return MYON_SubDDisplayParameters::DefaultDensity;

#endif
}

unsigned int MYON_SubDDisplayParameters::AbsoluteDisplayDensityFromSubD(
  unsigned adaptive_subd_display_density,
  const MYON_SubD& subd
)
{
  // If this function changes, then a parallel change must be made in
  // MYON_SubDLevel::CopyEvaluationCacheForExperts(const MYON_SubDLevel& src, MYON_SubDHeap& this_heap)
  // so it uses the same automatic density value.
  unsigned int display_density = MYON_SubDDisplayParameters::AbsoluteDisplayDensityFromSubDFaceCount(adaptive_subd_display_density, subd.FaceCount());
  return display_density;
}

const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::CreateFromDisplayDensity(
  unsigned int adaptive_subd_display_density
)
{
  if (adaptive_subd_display_density > MYON_SubDDisplayParameters::MaximumDensity)
    return MYON_SUBD_RETURN_ERROR(MYON_SubDDisplayParameters::Default);

  MYON_SubDDisplayParameters limit_mesh_parameters;
  limit_mesh_parameters.m_display_density = (unsigned char)adaptive_subd_display_density;
  limit_mesh_parameters.m_bDisplayDensityIsAbsolute = false;
  return limit_mesh_parameters;
}

const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::CreateFromAbsoluteDisplayDensity(
  unsigned int absolute_subd_display_density
  )
{
  if (absolute_subd_display_density > MYON_SubDDisplayParameters::MaximumDensity)
  {
    MYON_SUBD_ERROR("absolute_subd_display_density parameter is too large.");
    absolute_subd_display_density = MYON_SubDDisplayParameters::DefaultDensity;
  }

  MYON_SubDDisplayParameters limit_mesh_parameters;
  limit_mesh_parameters.m_display_density = (unsigned char)absolute_subd_display_density;
  limit_mesh_parameters.m_bDisplayDensityIsAbsolute = true;
  return limit_mesh_parameters;
}

const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::CreateFromMeshDensity(
  double normalized_mesh_density
)
{
  normalized_mesh_density = MYON_MeshParameters::ClampMeshDensityValue(normalized_mesh_density);

  if (normalized_mesh_density >= 0.0 && normalized_mesh_density <= 1.0)
  {
    unsigned int subd_display_density;

    if (normalized_mesh_density <= MYON_ZERO_TOLERANCE)
      subd_display_density = MYON_SubDDisplayParameters::ExtraCoarseDensity;
    else if (normalized_mesh_density < 0.20)
      subd_display_density = MYON_SubDDisplayParameters::CoarseDensity;
    else if (normalized_mesh_density < 0.35)
      subd_display_density = MYON_SubDDisplayParameters::MediumDensity;
    else if (normalized_mesh_density <= 0.75)
      subd_display_density = MYON_SubDDisplayParameters::FineDensity;
    else if (normalized_mesh_density < 1.0 - MYON_ZERO_TOLERANCE)
      subd_display_density = MYON_SubDDisplayParameters::ExtraFineDensity;
    else if (normalized_mesh_density <= 1.0 + MYON_ZERO_TOLERANCE)
      subd_display_density = MYON_SubDDisplayParameters::ExtraFineDensity;
    else
    {
      MYON_ERROR("Bug in some if condition in this function.");
      subd_display_density = MYON_SubDDisplayParameters::DefaultDensity;
    }

    return MYON_SubDDisplayParameters::CreateFromDisplayDensity(subd_display_density);
  }

  MYON_ERROR("Invalid normalized_mesh_density parameter.");
  return MYON_SubDDisplayParameters::Default;
}

bool MYON_SubDDisplayParameters::DisplayDensityIsAdaptive() const
{
  return (false == m_bDisplayDensityIsAbsolute) ? true : false;
}

bool MYON_SubDDisplayParameters::DisplayDensityIsAbsolute() const
{
  return (true == m_bDisplayDensityIsAbsolute) ? true : false;
}

unsigned int MYON_SubDDisplayParameters::DisplayDensity() const
{
  // MYON_SubDDisplayParameters::DisplayDensity() is deprecated.
  // Use DisplayDensity(subd).
  return DisplayDensity(MYON_SubD::Empty); // Empty dispables reduction
}

unsigned int MYON_SubDDisplayParameters::DisplayDensity(const MYON_SubD& subd) const
{
  const unsigned display_density = this->m_display_density;
  return
    this->DisplayDensityIsAdaptive() ?
    MYON_SubDDisplayParameters::AbsoluteDisplayDensityFromSubD(display_density, subd)
    : display_density
    ;
}

void MYON_SubDDisplayParameters::SetDisplayDensity(unsigned int adaptive_display_density)
{
  SetAdaptiveDisplayDensity(adaptive_display_density);
}

void MYON_SubDDisplayParameters::SetAdaptiveDisplayDensity(unsigned int adaptive_display_density)
{
  if (adaptive_display_density > MYON_SubDDisplayParameters::MaximumDensity)
    adaptive_display_density = MYON_SubDDisplayParameters::MaximumDensity;
  m_display_density = (unsigned char)adaptive_display_density;
  m_bDisplayDensityIsAbsolute = false;
}

void MYON_SubDDisplayParameters::SetAbsoluteDisplayDensity(unsigned int absolute_display_density)
{
  if (absolute_display_density > MYON_SubDDisplayParameters::MaximumDensity)
    absolute_display_density = MYON_SubDDisplayParameters::MaximumDensity;
  m_display_density = (unsigned char)absolute_display_density;
  m_bDisplayDensityIsAbsolute = true;
}

MYON_SubDComponentLocation MYON_SubDDisplayParameters::MeshLocation() const
{
  return m_bControlNetMesh ? MYON_SubDComponentLocation::ControlNet : MYON_SubDComponentLocation::Surface;
}

void MYON_SubDDisplayParameters::SetMeshLocation(MYON_SubDComponentLocation mesh_location)
{
  m_bControlNetMesh = (MYON_SubDComponentLocation::ControlNet == mesh_location) ? true : false;
}

unsigned char MYON_SubDDisplayParameters::EncodeAsUnsignedChar() const
{
  const unsigned char max_display_density = ((unsigned char)MYON_SubDDisplayParameters::MaximumDensity);
  const unsigned char default_display_density = ((unsigned char)MYON_SubDDisplayParameters::DefaultDensity);
  unsigned char encoded_parameters;
  if (
    (default_display_density == m_display_density || m_display_density > max_display_density)
    && MYON_SubDDisplayParameters::Default.DisplayDensityIsAdaptive() == DisplayDensityIsAdaptive()
    && MYON_SubDDisplayParameters::Default.MeshLocation() == MeshLocation()
    )
  {
    // use defaults
    encoded_parameters = 0;
  }
  else
  {
    const unsigned char density
      = (m_display_density <= max_display_density)
      ? m_display_density
      : default_display_density
      ;

    const unsigned char density_as_char = (density & MYON_SubDDisplayParameters::subd_mesh_density_mask);

    const unsigned char location
      = (MYON_SubDComponentLocation::ControlNet == MeshLocation())
      ? MYON_SubDDisplayParameters::subd_mesh_location_bit
      : 0;

    const unsigned char absoute_density_as_char
      = m_bDisplayDensityIsAbsolute
      ? MYON_SubDDisplayParameters::subd_mesh_absolute_density_bit
      : 0;

    const unsigned char nondefault_settings = MYON_SubDDisplayParameters::subd_mesh_nondefault_bit;

    encoded_parameters
      = nondefault_settings
      | density_as_char
      | absoute_density_as_char
      | location
      ;
  }
  return encoded_parameters;
}

const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::DecodeFromUnsignedChar(
  unsigned char encoded_parameters
)
{
  MYON_SubDDisplayParameters p(MYON_SubDDisplayParameters::Default);

  if (0 != (MYON_SubDDisplayParameters::subd_mesh_nondefault_bit & encoded_parameters))
  {
    const unsigned char density_is_absolute = (MYON_SubDDisplayParameters::subd_mesh_absolute_density_bit & encoded_parameters);
    const unsigned char density_as_char = (MYON_SubDDisplayParameters::subd_mesh_density_mask & encoded_parameters);
    const unsigned char location_ctrl_net = (MYON_SubDDisplayParameters::subd_mesh_location_bit & encoded_parameters);
    p.m_bDisplayDensityIsAbsolute = (0 != density_is_absolute) ? true : false;
    p.m_display_density = density_as_char;
    if (0 != location_ctrl_net)
      p.SetMeshLocation(MYON_SubDComponentLocation::ControlNet);
  }

  return p;
}

MYON_SubDDisplayParameters::Context MYON_SubDDisplayParameters::ContextForExperts() const
{
  return m_context;
}

void MYON_SubDDisplayParameters::SetContextForExperts(MYON_SubDDisplayParameters::Context context)
{
  m_context = context;
}

MYON_Terminator * MYON_SubDDisplayParameters::Terminator() const
{
  return m_terminator;
}

void MYON_SubDDisplayParameters::SetTerminator(MYON_Terminator * terminator)
{
  m_terminator = terminator;
}

MYON_ProgressReporter * MYON_SubDDisplayParameters::ProgressReporter() const
{
  return m_progress_reporter;
}

const MYON_Interval MYON_SubDDisplayParameters::ProgressReporterInterval() const
{
  return m_progress_reporter_interval;
}

void MYON_SubDDisplayParameters::SetProgressReporter(MYON_ProgressReporter * progress_reporter, MYON_Interval progress_reporter_interval)
{
  m_progress_reporter = progress_reporter;
  m_progress_reporter_interval = progress_reporter_interval;
}

MYON_SubDMeshFragmentIterator::MYON_SubDMeshFragmentIterator(const class MYON_SubDMesh limit_mesh)
{
  m_limit_mesh = limit_mesh;
  m_subd.ShareDimple(m_limit_mesh.SubD());
}

MYON_SubDMeshFragmentIterator::MYON_SubDMeshFragmentIterator(MYON_SubDRef& subd_ref)
{
  m_subd.ShareDimple(subd_ref.SubD());
  m_fit = m_subd.FaceIterator();
  m_bFromFaceFragments = true;
}

MYON_SubDMeshFragmentIterator::MYON_SubDMeshFragmentIterator(const MYON_SubD& subd)
{
  m_subd.ShareDimple(subd);
  m_fit = m_subd.FaceIterator();
  m_bFromFaceFragments = true;
}

MYON_SubDMeshFragmentIterator::MYON_SubDMeshFragmentIterator(const MYON_SubDFaceIterator& fit)
{
  m_subd.ShareDimple(fit.SubD());
  m_fit = fit;
  m_bFromFaceFragments = true;
}

void MYON_SubDMeshFragmentIterator::Internal_CopyFrom(const MYON_SubDMeshFragmentIterator& src)
{
  m_limit_mesh = src.m_limit_mesh;
  m_subd.ShareDimple(src.m_subd); // <- reason default copy ctor and op= can't be used.
  m_fit = src.m_fit;
  m_current_fragment = src.m_current_fragment;
  m_bFromFaceFragments = src.m_bFromFaceFragments;
  m_bHaveCounts = src.m_bHaveCounts;
  m_maximum_mesh_density = src.m_maximum_mesh_density;
  m_full_size_fragment_count = src.m_full_size_fragment_count;
  m_half_size_fragment_count = src.m_half_size_fragment_count;
}

MYON_SubDMeshFragmentIterator::MYON_SubDMeshFragmentIterator(const MYON_SubDMeshFragmentIterator& src)
{
  Internal_CopyFrom(src);
}

MYON_SubDMeshFragmentIterator& MYON_SubDMeshFragmentIterator::operator=(const MYON_SubDMeshFragmentIterator& src)
{
  if (this != &src)
    Internal_CopyFrom(src);
  return *this;
}


const MYON_BoundingBox MYON_SubDMeshFragmentIterator::BoundingBox() const
{
  return
    (MYON_SubDComponentLocation::ControlNet == SubDAppearance())
    ? ControlNetQuadBoundingBox()
    : SurfaceBoundingBox();
}


const MYON_BoundingBox MYON_SubDMeshFragmentIterator::SurfaceBoundingBox() const
{
  MYON_BoundingBox bbox;
  if (m_bFromFaceFragments)
    m_subd.GetTightBoundingBox(bbox);
  else
    bbox = m_limit_mesh.BoundingBox();
  return bbox;
}

const MYON_BoundingBox MYON_SubDMeshFragmentIterator::ControlNetQuadBoundingBox() const
{
  MYON_BoundingBox bbox;
  if (m_bFromFaceFragments)
    bbox = m_subd.BoundingBox();
  else
  {
    MYON_SubDMeshFragmentIterator local_frit(*this);
    for (const MYON_SubDMeshFragment* fragment = local_frit.FirstFragment(); nullptr != fragment; fragment = local_frit.NextFragment())
    {
      bbox.Union(fragment->ControlNetQuadBoundingBox());
    }
  }
  return bbox;
}

bool MYON_SubDMeshFragmentIterator::IsEmpty() const
{
  const MYON_SubDMeshFragment* f0 = nullptr;
  if (m_bFromFaceFragments)
  {
    MYON_SubDFaceIterator local_fit(m_fit); // cannot modify fit
    for (const MYON_SubDFace* f = local_fit.FirstFace(); nullptr != f; f = local_fit.NextFace())
    {
      f0 = f->MeshFragments();
      if (nullptr != f0)
        break;
    }
  }
  else
  {
    f0 = m_limit_mesh.FirstFragment();
  }
  return (nullptr == f0);
}

const MYON_SubDMeshFragment* MYON_SubDMeshFragmentIterator::FirstFragment()
{
  if (m_bFromFaceFragments)
  {
    m_current_fragment = nullptr;
    for (const MYON_SubDFace* f = m_fit.FirstFace(); nullptr != f; f = m_fit.NextFace())
    {
      m_current_fragment = f->MeshFragments();
      if (nullptr != m_current_fragment)
        break;
    }
  }
  else
  {
    m_current_fragment = m_limit_mesh.FirstFragment();
  }
  return m_current_fragment;
}

const MYON_SubDMeshFragment* MYON_SubDMeshFragmentIterator::NextFragment()
{
  if (nullptr != m_current_fragment)
  {
    m_current_fragment = m_current_fragment->m_next_fragment;
    if (nullptr == m_current_fragment && m_bFromFaceFragments)
    {
      for (const MYON_SubDFace* f = m_fit.NextFace(); nullptr != f; f = m_fit.NextFace())
      {
        m_current_fragment = f->MeshFragments();
        if (nullptr != m_current_fragment)
          break;
      }
    }
  }
  return m_current_fragment;
}

const MYON_SubDMeshFragment* MYON_SubDMeshFragmentIterator::CurrentFragment()
{
  return m_current_fragment;
}

const MYON_SubD& MYON_SubDMeshFragmentIterator::SubD() const
{
  return m_subd;
}

unsigned int MYON_SubDMeshFragmentIterator::FragmentCount() const
{
  if ( false  == m_bHaveCounts )
  {
    // lazy evaluation of this value because it's frequently never used.
    MYON_SubDMeshFragmentIterator frit(*this);
    unsigned int side_count = 0;
    unsigned int full_size_fragment_count = 0;
    unsigned int half_size_fragment_count = 0;
    for (const MYON_SubDMeshFragment* fragment = frit.FirstFragment(); nullptr != fragment; fragment = frit.NextFragment())
    {
      if ( 1 == fragment->m_face_fragment_count )
      {
        ++full_size_fragment_count;
        if (0 == side_count)
          side_count = fragment->m_grid.SideSegmentCount();
      }
      else if (fragment->m_face_fragment_count > 1)
      {
        ++half_size_fragment_count;
        if (0 == side_count)
          side_count = 2*fragment->m_grid.SideSegmentCount();
      }
    }
    unsigned int mesh_density = 0;
    for (unsigned int i = 1; i < side_count; i *= 2)
      ++mesh_density;

    m_maximum_mesh_density = mesh_density;
    m_full_size_fragment_count = full_size_fragment_count;
    m_half_size_fragment_count = half_size_fragment_count;

    m_bHaveCounts = true;
  }
  return m_full_size_fragment_count + m_half_size_fragment_count;
}

unsigned int MYON_SubDMeshFragmentIterator::MaximumMeshDensity() const
{
  return FragmentCount() > 0 ? m_maximum_mesh_density : 0;
}

unsigned int MYON_SubDMeshFragmentIterator::GetFragmentCounts(
    unsigned int& full_size_fragment_count,
    unsigned int& half_size_fragment_count
  ) const
{
  unsigned int fragment_count = FragmentCount();

  if (fragment_count > 0)
  {
    full_size_fragment_count = m_full_size_fragment_count;
    half_size_fragment_count = m_half_size_fragment_count;
  }
  else
  {
    full_size_fragment_count = 0;
    half_size_fragment_count = 0;
  }
  return fragment_count;
}

unsigned int MYON_SubDMeshFragmentIterator::FullSizeFragmentCount() const
{
  return FragmentCount() > 0 ? m_full_size_fragment_count : 0;
}

unsigned int MYON_SubDMeshFragmentIterator::HalfSizeFragmentCount() const
{
  return FragmentCount() > 0 ? m_half_size_fragment_count : 0;
}


unsigned int MYON_SubDMeshFragmentIterator::MinimumMeshDensity() const
{
  return (MaximumMeshDensity() > 0 && HalfSizeFragmentCount() > 0) ? 1 : 0;
}

unsigned int MYON_SubDMeshFragmentIterator::ClampMeshDensity(unsigned int candidate_mesh_density) const
{
  if (candidate_mesh_density > MaximumMeshDensity())
    return MaximumMeshDensity();
  if (candidate_mesh_density < MinimumMeshDensity())
    return MinimumMeshDensity();
  return candidate_mesh_density;
}

unsigned int MYON_SubDMeshFragment::FullFragmentMeshQuadCountFromDensity(
  unsigned int mesh_density
)
{
  if (mesh_density > MYON_SubDDisplayParameters::MaximumDensity)
    return 0;
  const unsigned int grid_side_count = 1 << mesh_density;
  return grid_side_count * grid_side_count;
}

unsigned int MYON_SubDMeshFragment::HalfFragmentMeshQuadCountFromDensity(
  unsigned int mesh_density
)
{
  return
    (mesh_density >= 1 && mesh_density <= MYON_SubDDisplayParameters::MaximumDensity)
    ? MYON_SubDMeshFragment::FullFragmentMeshQuadCountFromDensity(mesh_density - 1)
    : 0;
}

unsigned int MYON_SubDMeshFragment::FullFragmentMeshPointCountFromDensity(
  unsigned int mesh_density
)
{
  if (mesh_density > MYON_SubDDisplayParameters::MaximumDensity)
    return 0;
  const unsigned int grid_side_count = 1 << mesh_density;
  return (grid_side_count+1) * (grid_side_count+1);
}

unsigned int MYON_SubDMeshFragment::HalfFragmentMeshPointCountFromDensity(
  unsigned int mesh_density
)
{
  return
    (mesh_density >= 1 && mesh_density <= MYON_SubDDisplayParameters::MaximumDensity)
    ? MYON_SubDMeshFragment::FullFragmentMeshPointCountFromDensity(mesh_density - 1)
    : 0;
}

unsigned int MYON_SubDMeshFragmentIterator::TotalQuadCount(
  unsigned int mesh_density
  ) const
{
  mesh_density = ClampMeshDensity(mesh_density);
  unsigned int full_size_fragment_count = 0;
  unsigned int half_size_fragment_count = 0;
  MYON_SubDMeshFragmentIterator::GetFragmentCounts(full_size_fragment_count, half_size_fragment_count);
  const unsigned int full_size_quad_count = MYON_SubDMeshFragment::FullFragmentMeshQuadCountFromDensity(mesh_density);
  const unsigned int half_size_quad_count = MYON_SubDMeshFragment::HalfFragmentMeshQuadCountFromDensity(mesh_density);
  return full_size_fragment_count * full_size_quad_count + half_size_fragment_count * half_size_quad_count;
}

unsigned int MYON_SubDMeshFragmentIterator::TotalPointCount(
  unsigned int mesh_density
  ) const
{
  mesh_density = ClampMeshDensity(mesh_density);
  unsigned int full_size_fragment_count = 0;
  unsigned int half_size_fragment_count = 0;
  MYON_SubDMeshFragmentIterator::GetFragmentCounts(full_size_fragment_count, half_size_fragment_count);
  // const unsigned int grid_side_count = 1 << mesh_density;
  const unsigned int full_size_point_count = MYON_SubDMeshFragment::FullFragmentMeshPointCountFromDensity(mesh_density);
  const unsigned int half_size_point_count = MYON_SubDMeshFragment::HalfFragmentMeshPointCountFromDensity(mesh_density);
  return full_size_fragment_count * full_size_point_count + half_size_fragment_count * half_size_point_count;
}

unsigned int MYON_SubDMeshFragmentIterator::MaximumDensityQuadCount() const
{
  return TotalQuadCount(MaximumMeshDensity());
}

unsigned int MYON_SubDMeshFragmentIterator::MaximumDensityPointCount() const
{
  return TotalPointCount(MaximumMeshDensity());
}

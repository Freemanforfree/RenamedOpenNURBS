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

///////////////////////////////////////////////////////////////////
//
// MYON_MeshFaceSide
// 

#pragma region MYON_MeshFaceSide implementation

static MYON_MeshFaceSide MYON_MeshFaceSide_UnsetInitializer()
{
  MYON_MeshFaceSide unset;
  memset(&unset,0,sizeof(unset));
  return unset;
}

const MYON_MeshFaceSide MYON_MeshFaceSide::Unset = MYON_MeshFaceSide_UnsetInitializer();


///////////////////////////////////////////////////////////////////
//
// MYON_MeshFaceSide - sort by vertex index
//

#define MYON_COMPILING_OPENNURBS_QSORT_FUNCTIONS
#define MYON_SORT_TEMPLATE_STATIC_FUNCTION
#define MYON_SORT_TEMPLATE_TYPE MYON_MeshFaceSide

#define MYON_SORT_TEMPLATE_COMPARE MYON__MeshFaceSide_compare_m_vi
static int MYON_SORT_TEMPLATE_COMPARE( 
        MYON_SORT_TEMPLATE_TYPE const * side1, 
        MYON_SORT_TEMPLATE_TYPE const * side2 
        )
{
  if ( side1->m_vi[0] < side2->m_vi[0] )
    return -1;
  if ( side1->m_vi[0] > side2->m_vi[0] )
    return 1;
  if ( side1->m_vi[1] < side2->m_vi[1] )
    return -1;
  if ( side1->m_vi[1] > side2->m_vi[1] )
    return 1;
  if ( side1->m_fi < side2->m_fi )
    return -1;
  if ( side1->m_fi > side2->m_fi )
    return 1;
  if ( side1->m_side < side2->m_side )
    return -1;
  if ( side1->m_side > side2->m_side )
    return 1;
  if ( side1->m_dir < side2->m_dir )
    return -1;
  if ( side1->m_dir > side2->m_dir )
    return 1;
  return 0;
}

#define MYON_QSORT_FNAME MYON__MeshFaceSide_qsort_m_vi
#include "opennurbs_qsort_template.h"

int MYON_MeshFaceSide::CompareVertexIndex(
    const MYON_MeshFaceSide* a,
    const MYON_MeshFaceSide* b
    )
{
  if ( 0 == a )
    a = &MYON_MeshFaceSide::Unset;
  if ( 0 == b )
    b = &MYON_MeshFaceSide::Unset;
  return MYON_SORT_TEMPLATE_COMPARE(a,b);
}

void MYON_MeshFaceSide::SortByVertexIndex(
  MYON_MeshFaceSide* face_sides,
  size_t face_sides_count
  )
{
  if ( face_sides_count >= 2 && 0 != face_sides )
    MYON_QSORT_FNAME( face_sides, face_sides_count );
}

#undef MYON_QSORT_FNAME
#undef MYON_SORT_TEMPLATE_COMPARE

///////////////////////////////////////////////////////////////////
//
// MYON_MeshFaceSide - sort by face index
//

#define MYON_SORT_TEMPLATE_HAVE_SHORT_SORT

#define MYON_SORT_TEMPLATE_COMPARE MYON__MeshFaceSide_compare_m_fi
static int MYON_SORT_TEMPLATE_COMPARE( 
        MYON_SORT_TEMPLATE_TYPE const * side1, 
        MYON_SORT_TEMPLATE_TYPE const * side2 
        )
{
  if ( side1->m_fi < side2->m_fi )
    return -1;
  if ( side1->m_fi > side2->m_fi )
    return 1;
  if ( side1->m_vi[0] < side2->m_vi[0] )
    return -1;
  if ( side1->m_vi[0] > side2->m_vi[0] )
    return 1;
  if ( side1->m_vi[1] < side2->m_vi[1] )
    return -1;
  if ( side1->m_vi[1] > side2->m_vi[1] )
    return 1;
  if ( side1->m_side < side2->m_side )
    return -1;
  if ( side1->m_side > side2->m_side )
    return 1;
  if ( side1->m_dir < side2->m_dir )
    return -1;
  if ( side1->m_dir > side2->m_dir )
    return 1;
  return 0;
}

#define MYON_QSORT_FNAME MYON__MeshFaceSide_qsort_m_fi
#include "opennurbs_qsort_template.h"

int MYON_MeshFaceSide::CompareFaceIndex(
    const MYON_MeshFaceSide* a,
    const MYON_MeshFaceSide* b
    )
{
  if ( 0 == a )
    a = &MYON_MeshFaceSide::Unset;
  if ( 0 == b )
    b = &MYON_MeshFaceSide::Unset;
  return MYON_SORT_TEMPLATE_COMPARE(a,b);
}

void MYON_MeshFaceSide::SortByFaceIndex(
  MYON_MeshFaceSide* face_sides,
  size_t face_sides_count
  )
{
  if ( face_sides_count >= 2 && 0 != face_sides )
    MYON_QSORT_FNAME( face_sides, face_sides_count );
}

///////////////////////////////////////////////////////////////////
//
// done using sort template for MYON_MeshFaceSide
//

#undef MYON_COMPILING_OPENNURBS_QSORT_FUNCTIONS
#undef MYON_SORT_TEMPLATE_STATIC_FUNCTION
#undef MYON_SORT_TEMPLATE_TYPE
#undef MYON_QSORT_FNAME
#undef MYON_SORT_TEMPLATE_HAVE_SHORT_SORT
#undef MYON_QSORT_SHORT_SORT_FNAME

//
// done using sort template for MYON_MeshFaceSide
//
///////////////////////////////////////////////////////////////////

#pragma endregion

//
// MYON_MeshFaceSide
// 
///////////////////////////////////////////////////////////////////

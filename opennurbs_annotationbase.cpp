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

#include "opennurbs_textiterator.h"
MYON_VIRTUAL_OBJECT_IMPLEMENT(MYON_Annotation, MYON_Geometry, "B5802E0C-5B16-43C9-BD43-A3E0AC18203B");

MYON::AnnotationType MYON::AnnotationTypeFromUnsigned( 
  unsigned int annotation_type_as_unsigned
)
{
  switch (annotation_type_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Aligned);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Angular);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Diameter);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Radius);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Rotated);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Ordinate);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::ArcLen);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::CenterMark);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Text);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Leader);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AnnotationType::Angular3pt);
  }
  MYON_ERROR("Invalid annotation_type_as_unsigned parameter");
  return MYON::AnnotationType::Unset;
}

MYON_Annotation::MYON_Annotation(MYON::AnnotationType annotation_type)
  : MYON_Geometry()
  , m_annotation_type(annotation_type)
{}

MYON::AnnotationType MYON_Annotation::Type() const
{
  return m_annotation_type;
}

void MYON_Annotation::Internal_CopyFrom(const MYON_Annotation& src)
{
  m_annotation_type = src.m_annotation_type;
  m_dimstyle_id = src.m_dimstyle_id;
  m_plane = src.m_plane;
  m_horizontal_direction = src.m_horizontal_direction;
  m_allow_text_scaling = src.m_allow_text_scaling;
  if (nullptr != src.m_text)
    m_text = new MYON_TextContent(*src.m_text);
  if (nullptr != src.m_override_dimstyle)
  {
    m_override_dimstyle = new MYON_DimStyle(*src.m_override_dimstyle);
  }
}

void MYON_Annotation::Internal_Destroy()
{
  ClearText();
  Internal_DeleteOverrideDimstyle();
}

MYON_Annotation::MYON_Annotation(const MYON_Annotation& src)
  : MYON_Geometry(src)
{
  Internal_CopyFrom(src);
}

MYON_Annotation::~MYON_Annotation()
{
  Internal_Destroy();
}

MYON_Annotation& MYON_Annotation::operator=(const MYON_Annotation& src)
{
  if (&src != this)
  {
    Internal_Destroy();
    Internal_CopyFrom(src);
  }
  return *this;
}

MYON::object_type MYON_Annotation::ObjectType() const
{
  return MYON::annotation_object;
}

bool MYON_BinaryArchive::Internal_Write3dmDimStyleOverrides(
  const MYON_Annotation& annotation,
  const MYON_DimStyle* dim_style_overrides
)
{
  // Chunk containing dimstyle overrides
  const int content_version = 1;
  if (false == BeginWrite3dmAnonymousChunk(content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    const MYON_UUID annotation_dimstyle_id = annotation.DimensionStyleId();

    const bool bHasOverrideDimStyle
      = nullptr != dim_style_overrides
      && MYON_nil_uuid != annotation_dimstyle_id
      && annotation_dimstyle_id == dim_style_overrides->ParentId()
      && annotation.IsOverrideDimStyleCandidate(dim_style_overrides,true)
      && (MYON_ModelComponent::Type::DimStyle == Manifest().ItemFromId(annotation_dimstyle_id).ComponentType()
        || MYON_DimStyle::SystemDimstyleFromId(annotation_dimstyle_id).Id() == annotation_dimstyle_id
        )
      ;

    if (!WriteBool(bHasOverrideDimStyle))
      break;

    if (bHasOverrideDimStyle)
    {
      // use archive.WriteObject() so user data will persist.
      if (!WriteObject(dim_style_overrides))
        break;
    }

    rc = true;
    break;
  }

  if (!EndWrite3dmChunk())
    rc = false;
  return rc;
}

static bool Internal_UpdateOverrideCandidateParentId(
  const MYON_BinaryArchive& archive,
  MYON_DimStyle* override_candidate
)
{
  for (;;)
  {
    if (
      false == archive.CheckForRemappedIds()
      && 0 == archive.ReferenceModelSerialNumber()
      && 0 == archive.InstanceDefinitionModelSerialNumber()
      )
    {
      return false; // common situation - no change reqired
    }


    if (nullptr == override_candidate)
      break;
    const MYON_UUID archive_parent_id = override_candidate->ParentId();
    if (MYON_nil_uuid == archive_parent_id)
      break;

    // We are reading a worksession reference model or reference style linked instance defintion.
    // The ids in the reference file may have had a collision with ids in the active model
    // and been changed.
    const MYON_ManifestMapItem parent_id_map_item = archive.ManifestMap().MapItemFromSourceId(archive_parent_id);
    if (MYON_ModelComponent::Type::DimStyle != parent_id_map_item.ComponentType())
      break;
    if (false == parent_id_map_item.SourceIsSet())
      break;
    if (false == parent_id_map_item.DestinationIsSet())
      break;
    if (parent_id_map_item.SourceId() != archive_parent_id)
      break;
    
    const MYON_UUID model_parent_id = parent_id_map_item.DestinationId();
    if (model_parent_id == MYON_nil_uuid)
      break;

    if (model_parent_id == archive_parent_id)
      return false; // common situation - no change reqired

    // We are reading a worksession reference model or reference style linked instance defintion.
    // The ids in the reference file may have had a collision with ids in the active model
    // and been changed.
    override_candidate->SetParentId(model_parent_id);
    return true; // changed parent id
  }

  return false; // unexpected but, no changes
}

bool MYON_BinaryArchive::Internal_Read3dmDimStyleOverrides(
  class MYON_Annotation& annotation,
  bool bFromDimStyleTable
)
{
  if (bFromDimStyleTable)
  {
    // V5 or old V6 file where override styles were kept in the dim style table
    for (;;)
    {
      if (false == m_bLegacyOverrideDimStylesInArchive)
        break;

      const MYON_UUID dim_style_id = annotation.DimensionStyleId();
      if (MYON_nil_uuid == dim_style_id)
        break;

      const unsigned count = this->m_archive_dim_style_table.UnsignedCount();
      if (count <= 0)
        break;

      for (unsigned int i = 0; i < count; i++)
      {
        const MYON_DimStyle* dim_style = m_archive_dim_style_table[i];
        if (nullptr == dim_style)
          continue;

        if (dim_style_id != dim_style->Id())
          continue;

        const MYON_UUID parent_id = dim_style->ParentId();
        
        if (MYON_nil_uuid == parent_id)
          continue;

        if (dim_style_id == parent_id)
          continue;

        if (MYON_ModelComponent::Type::DimStyle != Manifest().ItemFromId(parent_id).ComponentType())
          continue;

        const MYON_DimStyle* parent_ds = nullptr;
        for (unsigned int j = 0; j < count; j++)
        {
          const MYON_DimStyle* archive_ds = m_archive_dim_style_table[j];
          if (nullptr == archive_ds)
            continue;
          if (parent_id != archive_ds->Id())
            continue;
          parent_ds = archive_ds;
          break;
        }

        if (nullptr == parent_ds)
          break;
        if (parent_ds->ParentIdIsNotNil())
          break;

        annotation.SetDimensionStyleId(parent_id);

        MYON_DimStyle* override_candidate = new MYON_DimStyle(*dim_style);
        override_candidate->ClearId();
        override_candidate->ClearIndex();
        override_candidate->ClearName();
        Internal_UpdateOverrideCandidateParentId(*this, override_candidate);
        annotation.SetOverrideDimensionStyle(override_candidate);
        if (nullptr != override_candidate)
        {
          // If this error occurs frequently, then we will comment out the call to MYON_ERROR.
          MYON_ERROR("Legacy override dimstyle information discarded.");
          delete override_candidate;
          break;
        }
        // update override_candidate settings.
        annotation.DimensionStyle(*parent_ds);
        break;
      }
      break;
    }

    return true;
  }

  // Read information written by MYON_BinaryArchive::Internal_Write3dmDimStyleOverrides()
  int content_version = 0;
  if (false == BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    bool bHasOverrideDimStyle = false;
    if (!ReadBool(&bHasOverrideDimStyle))
      break;

    if (bHasOverrideDimStyle)
    {
      // use archive.ReadObject() so user data will persist.
      MYON_Object* ptr = nullptr;
      if (!ReadObject(&ptr))
        break;

      for (;;)
      {
        if (MYON_nil_uuid == annotation.DimensionStyleId())
          break;
        MYON_DimStyle* override_candidate = MYON_DimStyle::Cast(ptr);
        if (nullptr == override_candidate)
          break;
        Internal_UpdateOverrideCandidateParentId(*this, override_candidate);
        if (false == annotation.IsOverrideDimStyleCandidate(override_candidate,true))
          break;
        annotation.SetOverrideDimensionStyle(override_candidate);
        ptr = override_candidate; // if annotation is managing override_candidate, then it is nullptr here.
        break;
      }
      if (nullptr != ptr)
        delete ptr;
    }

    rc = true;
    break;
  }

  if (!EndRead3dmChunk())
    rc = false;
  return rc;

}


bool MYON_Annotation::Internal_WriteAnnotation(
  MYON_BinaryArchive& archive
  ) const
{
  // content_version = 2 - added override dimstyle to MYON_Annotation RH-37176
  // content_version = 3 -  added m_horizontal_direction
  // content_version = 4 -  added m_allow_text_scaling - Lowell
  const int content_version = 4;

  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;
  bool rc = false;
  for (;;)
  {
    const MYON_TextContent* text_to_write = (nullptr != m_text) ? m_text : &MYON_TextContent::Empty;

    if (false == text_to_write->Write(archive))
      break;

    if (!archive.WriteUuid(m_dimstyle_id))
      break;

    if (!archive.WritePlane(m_plane))
      break;

    // Added for content_version 1
    // Required to write/write linear dimensions and radial dimensions
    // because they have multiple possible values of m_annotation_type.
    unsigned int u = static_cast<unsigned char>(m_annotation_type);
    if (!archive.WriteInt(u))
      break;

    // Dale Lear 2016 Dec 12 https://mcneel.myjetbrains.com/youtrack/issue/RH-37176
    // content_version incremented to 2.
    if (!archive.Internal_Write3dmDimStyleOverrides(*this, m_override_dimstyle))
      break;

    // content_version = 3 ( 13 July, 2017 )
    if (!archive.WriteVector(m_horizontal_direction))
      break;

    // content_version = 4 ( 17 May, 2018 - Lowell)
    if (!archive.WriteBool(m_allow_text_scaling))
      break;

    rc = true;
    break;
  }
  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_Annotation::Internal_ReadAnnotation(
  MYON_BinaryArchive& archive
  )
{
  Internal_Destroy();
  m_dimstyle_id = MYON_nil_uuid;
  m_plane = MYON_Plane::World_xy;
  // do not change the value of m_annotation_type
    
  int content_version = -1;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (content_version < 0)
      break;
    
    MYON_TextContent* text = new MYON_TextContent;
    if (nullptr == text)
      break;
    if (false == text->Read(archive))
    {
      delete text;
      break;
    }
    m_text = text;

    if (!archive.Read3dmReferencedComponentId(MYON_ModelComponent::Type::DimStyle, &m_dimstyle_id))
      break;

    if (!archive.ReadPlane(m_plane))
      break;

    if (content_version <= 0)
    {
      rc = true;
      break;
    }

    unsigned int u = 0;
    if (!archive.ReadInt(&u))
      break;
    const MYON::AnnotationType annotation_type = MYON::AnnotationTypeFromUnsigned(u);
    if (annotation_type != m_annotation_type)
    {
      const MYON::AnnotationType annotation_type_pairs[3][2]
      {
        // MYON_DimLinear linear dimensions can have type = Aligned or Rotated
        {MYON::AnnotationType::Aligned,MYON::AnnotationType::Rotated},

        // MYON_DimRadial radial dimensions can have type = Diameter or Radius
        { MYON::AnnotationType::Diameter,MYON::AnnotationType::Radius },

        // MYON_DimAngular radial dimensions can have type = Angular or Angular3pt
        { MYON::AnnotationType::Angular,MYON::AnnotationType::Angular3pt },
      };
      for (int pair_dex = 0; pair_dex < 3 && annotation_type != m_annotation_type; pair_dex++)
      {
        for (int k = 0; k < 2; k++)
        {
          if (
            annotation_type_pairs[pair_dex][k] == annotation_type
            && annotation_type_pairs[pair_dex][1-k] == m_annotation_type
            )
          {
            m_annotation_type = annotation_type;
            break;
          }
        }            
      }
      if (annotation_type != m_annotation_type)
      {
        MYON_ERROR("Invalid annotation type.");
      }
    }

    // Dale Lear 2016 Dec 12 https://mcneel.myjetbrains.com/youtrack/issue/RH-37176
    // content_version incremented to 2.
    // optional override dimension style move from dimstyle table to annotaton object
    const bool bFromDimStyleTable = content_version <= 1;
    if (!archive.Internal_Read3dmDimStyleOverrides(*this, bFromDimStyleTable))
      break;

    if (content_version <= 2)
    {
      rc = true;
      break;
    }

    // content_version = 3 ( 13 July, 2017 )
    if (!archive.ReadVector(m_horizontal_direction))
      break;

    if (content_version <= 3)
    {
      rc = true;
      break;
    }

    // content_version = 4 ( 17 May, 2018 - Lowell)
    if (!archive.ReadBool(&m_allow_text_scaling))
      break;

    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_Annotation::IsValid(MYON_TextLog* text_log) const
{
  return (
    m_text->IsValid() 
    && m_plane.IsValid()
    );
}

bool MYON_Annotation::GetTextXform(
  const MYON_Xform* model_xform,
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  const MYON_Text* pText = MYON_Text::Cast(this);
  if (nullptr != pText)
    return pText->GetTextXform(model_xform, vp, dimstyle, dimscale, text_xform_out);

  const MYON_Leader* pLeader = MYON_Leader::Cast(this);
  if (nullptr != pLeader)
    return pLeader->GetTextXform(model_xform, vp, dimstyle, dimscale, text_xform_out);

  const MYON_DimLinear* pDimLinear = MYON_DimLinear::Cast(this);
  if (nullptr != pDimLinear)
    return pDimLinear->GetTextXform(model_xform, vp, dimstyle, dimscale, text_xform_out);

  const MYON_DimAngular* pDimAngular = MYON_DimAngular::Cast(this);
  if (nullptr != pDimAngular)
    return pDimAngular->GetTextXform(model_xform, vp, dimstyle, dimscale, text_xform_out);

  const MYON_DimRadial* pDimRadial = MYON_DimRadial::Cast(this);
  if (nullptr != pDimRadial)
    return pDimRadial->GetTextXform(model_xform, vp, dimstyle, dimscale, text_xform_out);

  const MYON_DimOrdinate* pDimOrdinate = MYON_DimOrdinate::Cast(this);
  if (nullptr != pDimOrdinate)
    return pDimOrdinate->GetTextXform(model_xform, vp, dimstyle, dimscale, text_xform_out);

  const MYON_Centermark* pCentermark = MYON_Centermark::Cast(this);
  if (nullptr != pCentermark)
    return pCentermark->GetTextXform(vp, dimstyle, dimscale, text_xform_out);

  MYON_ERROR("Annotation type not handled");
  return false;
}

void MYON_Annotation::SetPlane(const MYON_Plane& plane)
{
  m_plane = plane;
}

const MYON_Plane& MYON_Annotation::Plane() const
{
  return m_plane;
}

void MYON_Annotation::SetHorizontalDirection(MYON_2dVector horiz)
{
  if (horiz.Unitize())
    m_horizontal_direction = horiz;
}

const MYON_2dVector MYON_Annotation::HorizontalDirection() const
{
  return m_horizontal_direction;
}

MYON_3dVector MYON_Annotation::GetDefaultHorizontal(const MYON_Plane& plane)
{
  double dx = plane.zaxis * MYON_3dVector::XAxis;
  double dy = plane.zaxis * MYON_3dVector::YAxis;
  double dz = plane.zaxis * MYON_3dVector::ZAxis;

  MYON_3dVector h3d = MYON_3dVector::XAxis;
  if (fabs(dz) > fabs(dx) && fabs(dz) > fabs(dy))
    h3d = dz > 0.0 ? MYON_3dVector::XAxis : -MYON_3dVector::XAxis;
  else if (fabs(dx) > fabs(dy) && fabs(dx) > fabs(dz))
    h3d = dx > 0.0 ? MYON_3dVector::YAxis : -MYON_3dVector::YAxis;
  else if (fabs(dy) > fabs(dx) && fabs(dy) > fabs(dz))
    h3d = dy > 0.0 ? MYON_3dVector::XAxis : -MYON_3dVector::XAxis;

  MYON_2dVector h2d;
  MYON_3dPoint p = plane.origin + h3d;
  if (plane.ClosestPointTo(p, &h2d.x, &h2d.y) && h2d.Unitize())
  {
    p = plane.PointAt(h2d.x, h2d.y);
    h3d = p - plane.origin;
    if (h3d.Unitize())
      return h3d;
  }

  return MYON_3dVector::XAxis;
}


void MYON_Annotation::CalcTextFlip(
  const MYON_3dVector& text_xdir, const MYON_3dVector& text_ydir, const MYON_3dVector& text_zdir,
  const MYON_3dVector& view_xdir, const MYON_3dVector& view_ydir, const MYON_3dVector& view_zdir,
  const MYON_Xform* model_xform,
  const double fliptol,
  bool& flip_x,
  bool& flip_y)
{

  double XoX = text_xdir * view_xdir;
  double XoY = text_xdir * view_ydir;
  double YoX = text_ydir * view_xdir;
  double YoY = text_ydir * view_ydir;
  bool from_the_back = (view_zdir * text_zdir < 0.0);
  if (nullptr != model_xform && model_xform->Determinant() < 0.0)
    from_the_back = !from_the_back;

  double upsign = 1.0;

  // This part shifts text to the correct side of the dimension line
  if (fabs(XoX) > fabs(XoY)) // more horizontal
  {
    if (YoY > 0.0)
      upsign = 1.0;
    else
      upsign = -1.0;
  }
  else  // more vertical
  {
    if (from_the_back)
    {
      if (YoX < 0.0)
      {
        if (XoX < fliptol)
          upsign = 1.0;
        else
          upsign = -1.0;
      }
      else
      {
        if (XoX > -fliptol)
          upsign = -1.0;
        else
          upsign = 1.0;
      }
    }
    else
    {
      if (YoX > 0.0)
      {
        if (XoX > fliptol)
          upsign = 1.0;
        else
          upsign = -1.0;
      }
      else
      {
        if (XoX < -fliptol)
          upsign = -1.0;
        else
          upsign = 1.0;
      }
    }
  }
  flip_x = false;
  flip_y = false;
  if (from_the_back)
    upsign = -upsign;
  flip_x = upsign < 0.0;
  if (from_the_back)
    flip_y = !flip_x;
  else
    flip_y = flip_x;
}


const MYON_wString MYON_Annotation::PlainText() const
{
  if (nullptr == m_text)
    return MYON_wString::EmptyString;
  const MYON_TextRunArray* runs = m_text->TextRuns(true);
  if (nullptr != runs && 0 == runs->Count())
    BoundingBox();  // Side effect of building text runs
  return m_text->PlainText();
}

const MYON_wString MYON_Annotation::PlainTextWithFields() const
{
  if (nullptr == m_text)
    return MYON_wString::EmptyString;
  const MYON_TextRunArray* runs = m_text->TextRuns(true);
  if (nullptr != runs && 0 == runs->Count())
    BoundingBox();
  return m_text->PlainTextWithFields();
}

const MYON_wString  MYON_Annotation::PlainTextWithFields(MYON_SimpleArray<MYON_3dex>* runmap) const
{
  if (nullptr == m_text)
    return MYON_wString::EmptyString;
  const MYON_TextRunArray* runs = m_text->TextRuns(true);
  if (nullptr != runs && 0 == runs->Count())
    BoundingBox();
  return m_text->PlainTextWithFields(runmap);
}


const MYON_wString MYON_Annotation::RichText() const
{
  return (nullptr == m_text)
    ? MYON_wString::EmptyString
    : m_text->RichText();
}

MYON_UUID MYON_Annotation::DimensionStyleId() const
{
  return m_dimstyle_id;
}

void MYON_Annotation::SetDimensionStyleId(MYON_UUID dimstyle_id)
{
  const bool bKeepOverrides = false;
  SetDimensionStyleIdForExperts(dimstyle_id, bKeepOverrides);
}

void MYON_Annotation::SetDimensionStyleId(
  const class MYON_DimStyle& dim_style
)
{
  const MYON_UUID dim_style_id
    = dim_style.ParentIdIsNil()
    ? dim_style.Id()
    : dim_style.ParentId();

  MYON_DimStyle* override_dim_style
    = (MYON_nil_uuid != dim_style_id
      && dim_style_id != dim_style.Id()
      && MYON_Annotation::Internal_IsOverrideDimStyleCandidate(&dim_style, dim_style_id, true, false)
      )
    ? new MYON_DimStyle(dim_style)
    : nullptr;

  // If dim_style parameter was the override on this class,
  // the next line will delete it.
  SetDimensionStyleId(dim_style_id);

  SetOverrideDimensionStyle(override_dim_style);
}


void MYON_Annotation::SetDimensionStyleIdForExperts(
  MYON_UUID dimstyle_id,
  bool bKeepOverrides
)
{
  if (bKeepOverrides)
  {
    if (MYON_nil_uuid == dimstyle_id || nullptr == m_override_dimstyle)
      bKeepOverrides = false;
    else if (IsOverrideDimStyleCandidate(m_override_dimstyle,true))
      bKeepOverrides = m_override_dimstyle->SetParentId(dimstyle_id);
  }

  if (false == bKeepOverrides)
  {
    // This resets all overrides
    Internal_DeleteOverrideDimstyle();
  }

  m_dimstyle_id = dimstyle_id;
}

const MYON_DimStyle& MYON_Annotation::DimensionStyle(const MYON_DimStyle& parent_dimstyle) const
{
  return DimensionStyle(parent_dimstyle, false);
}

const MYON_DimStyle& MYON_Annotation::DimensionStyle(
  const MYON_DimStyle& parent_dimstyle,
  bool bForceOverrideUpdate
) const
{
  if (nullptr == m_override_dimstyle )
    return parent_dimstyle;

  if (false == m_override_dimstyle->HasOverrides())
  {
    Internal_DeleteOverrideDimstyle();
    return parent_dimstyle;
  }

  if (
    bForceOverrideUpdate
    || m_override_dimstyle->ParentId() != parent_dimstyle.Id() 
    || parent_dimstyle.ContentVersionNumber() != m_parent_dimstyle_content_version_number
    || m_override_dimstyle->ParentContentHash() != parent_dimstyle.ContentHash()
    )
  {
    // update m_override_dimstyle if parent content has changed.
    m_override_dimstyle->OverrideFields(*m_override_dimstyle, parent_dimstyle);
    if (false == m_override_dimstyle->HasOverrides())
    {
      // Updating overrides unsets all overrides that match parent values.
      // When this happens, the overrides are deleted here.
      Internal_DeleteOverrideDimstyle();
      return parent_dimstyle;
    }
    m_parent_dimstyle_content_version_number = parent_dimstyle.ContentVersionNumber();
  }

  return *m_override_dimstyle;
}

bool MYON_Annotation::IsOverrideStylePointer(
  const MYON_DimStyle* ptr
) const
{
  return (nullptr != ptr && ptr == m_override_dimstyle);
}

bool MYON_Annotation::AllowTextScaling() const
{
  // These functions are being added to continue the V5 behavior of
  // per-object text scaling. There is no user interface
  // in V6 or V7 that shows this setting or that allows a user
  // to change this setting.
  // AllowTextScaling() = false means the effective dimstyle value 
  // of DimScale() (model space scale factor) is ignored (treated as if it were 1).
  return m_allow_text_scaling;
}

void MYON_Annotation::SetAllowTextScaling(bool scale)
{
  // These functions are being added to continue the V5 behavior of
  // per-object text scaling. There is no user interface
  // in V6 or V7 that shows this setting or that allows a user
  // to change this setting.
  // AllowTextScaling() = false means the effective dimstyle value 
  // of DimScale() (model space scale factor) is ignored (treated as if it were 1).
  if (scale != m_allow_text_scaling)
  {
    m_allow_text_scaling = scale ? true : false;
    ClearBoundingBox();
  }
}

bool MYON_Annotation::IsOverrideDimStyleCandidate(
  const MYON_DimStyle* override_style_candidate,
  bool bRequireSetOverrides
) const
{
  const bool bIssueErrorsAndWarnings = false;
  return Internal_IsOverrideDimStyleCandidate(override_style_candidate, m_dimstyle_id, bRequireSetOverrides, bIssueErrorsAndWarnings);
}

bool MYON_DimStyle::IsOverrideDimStyleCandidate(
  MYON_UUID parent_id,
  bool bRequireSetOverrides,
  MYON_wString* error_description
) const
{
  if (bRequireSetOverrides)
  {
    if (false == this->HasOverrides())
    {
      if (error_description)
      {
        *error_description = "this->HasOverrides() is false.";
      }
      return false;
    }
  }

  if (this->IsDeleted())
  {
    if (error_description)
    {
      *error_description = "override style parameter is marked as deleted.";
    }
    return false;
  }

  const MYON_UUID candidate_parent_id = this->ParentId();
  if (MYON_nil_uuid == candidate_parent_id)
  {
    if (this->ParentIdIsLocked())
    {
      if (error_description)
      {  
        *error_description = "override style parent id is nil and locked.";
      }
      return false;
    }
  }
  else if (candidate_parent_id != parent_id)
  {
    if (error_description)
    {
      *error_description = "override style parent id is incorrectly set.";
    }
    return false;
  }
  
  if ( this->IdIsNotNil() )
  {
    if (error_description)
    {
      *error_description = "override style id is not nil.";
    }
    return false;
  }

  if ( this->NameIsNotEmpty() )
  {
    if (error_description)
    {
      *error_description = "override style name is not empty.";
    }
    return false;
  }
    
  if ( MYON_ModelComponent::Unset.Index() != this->Index() )
  {
    if (error_description)
    {
      *error_description = "override style index is not unset index.";
    }
    return false;
  }

  return true;  
}


bool MYON_Annotation::Internal_IsOverrideDimStyleCandidate(
  const MYON_DimStyle* override_style_candidate,
  MYON_UUID parent_id,
  bool bRequireSetOverrides,
  bool bIssueErrorsAndWarnings
)
{
  if (nullptr == override_style_candidate)
  {
    if (bIssueErrorsAndWarnings)
    {
      MYON_ERROR("override_style_candidate is nullptr.");
    }
    return false;
  }

  MYON_wString error_description;
  bool rc = override_style_candidate->IsOverrideDimStyleCandidate(
    parent_id,
    bRequireSetOverrides,
    (bIssueErrorsAndWarnings ? &error_description : nullptr)
  );
  if (false == rc && bIssueErrorsAndWarnings)
  {
    error_description.TrimLeftAndRight();
    MYON_String utf8_error_description = error_description;
    utf8_error_description.TrimLeftAndRight();
    if (utf8_error_description.IsEmpty())
    {
      MYON_ERROR("override_style_candidate is not a valid override candidate.");
    }
    else
    {
      MYON_ERROR(utf8_error_description);
    }
  }

  return true;  
}

void MYON_Annotation::ClearOverrideDimensionStyle()
{
  Internal_DeleteOverrideDimstyle();
}

/*
Description:
  If this->IsOverrideDimStyleCandidate(override_style_candidate,bRequireSetOverrides)
  is true, then a managed copy of override_style_candidate is set as an override.
Returns:
  True if an override is set.
*/
bool MYON_Annotation::SetOverrideDimensionStyle(
  const MYON_DimStyle* override_style_candidate,
  bool bRequireSetOverrides
)
{
  bool rc;

  if (nullptr == override_style_candidate)
  {
    ClearOverrideDimensionStyle();
    rc = true;
  }
  else if ( IsOverrideDimStyleCandidate(override_style_candidate, bRequireSetOverrides) )
  {
    MYON_DimStyle* managed_override_style = new MYON_DimStyle(*override_style_candidate);
    rc = SetOverrideDimensionStyle(managed_override_style);
    if (managed_override_style != m_override_dimstyle)
    {
      delete managed_override_style;
      rc = false;
    }
  }
  else
  {
    // invalid override candidate
    rc = false;
  }

  return rc;
}

bool MYON_Annotation::SetOverrideDimensionStyle(MYON_DimStyle*& override_style) const
{
  if (nullptr != m_override_dimstyle && m_override_dimstyle == override_style)
  {
    // The caller is very confused.
    // override_style is already managed by this MYON_Annotation instance.
    // Going further will cause a crash.
    // This is a serious error caused by a serious bug in the calling code.
    MYON_ERROR("m_override_dimstyle = override_style");
    override_style = nullptr;
    return false;
  }
  Internal_DeleteOverrideDimstyle();

  // handle simple cases of "clearing overrides" first.
  if (nullptr == override_style)
    return true;

  if (MYON_nil_uuid == m_dimstyle_id)
  {
    // MYON_ERROR automaically fills in function name and new lines when needed.
    //MYON_ERROR("SetOverrideDimensionStyle(): this->m_dimstyle_id must be non-nil to override a dimstyle.\n");
    MYON_ERROR("this->m_dimstyle_id must be non-nil to override a dimstyle.");
    return false;
  }

  // Do not require overrides to be set. Some workflows add dimstyle with no overrides set and then
  // modify that dimstyle.

  //if (false == override_style->HasOverrides())
  //{
  //  // skip detailed validation steps that don't matter.
  //  delete override_style;
  //  return true;
  //}

  const MYON_UUID candidate_parent_id
    = override_style->ParentIdIsNil()
    ? MYON_nil_uuid
    : m_dimstyle_id;
  const bool bRequireSetOverrides = false;
  const bool bIssueErrorsAndWarnings = true;
  if (false == MYON_Annotation::Internal_IsOverrideDimStyleCandidate(override_style, candidate_parent_id, bRequireSetOverrides, bIssueErrorsAndWarnings) )
  {
    // invalid override_style
    return false;
  }

  if (false == override_style->SetParentId(m_dimstyle_id) || m_dimstyle_id != override_style->ParentId())
  {
    MYON_ERROR("override_style->SetParentId(this->m_dimstyle_id) failed.");
    return false;
  }

  // override_style's must have unset id (= nil), unset index = (MYON_UNSET_INT_INDEX), unset name (empty)
  override_style->ClearId();
  override_style->LockId();

  override_style->ClearName();
  override_style->LockName();

  override_style->ClearIndex();
  override_style->LockIndex(); 

  m_override_dimstyle = override_style;
  override_style = nullptr;

  return true;
}

bool MYON_Annotation::HasDimensionStyleOverrides() const
{
  return (nullptr != m_override_dimstyle && m_override_dimstyle->HasOverrides());
}

void MYON_Annotation::Internal_DeleteOverrideDimstyle() const
{
  if (nullptr != m_override_dimstyle)
  {
    delete m_override_dimstyle;
    m_override_dimstyle = nullptr;
  }
  m_parent_dimstyle_content_version_number = 0;  
}

const MYON_TextContent* MYON_Annotation::Text() const
{
  return m_text;
}

MYON_TextContent* MYON_Annotation::Text()
{
  return m_text;
}

void MYON_Annotation::SetText(MYON_TextContent*& text) const
{
  if (text == m_text)
    return;
  ClearText();
  m_text = text;
  text = nullptr;
}

void MYON_Annotation::ClearText() const
{
  if (nullptr != m_text)
  {
    delete m_text;
    m_text = nullptr;
  }
}

double MYON_Annotation::TextRotationRadians() const
{
  double ang = 0.0;
  const MYON_TextContent* text = Text();
  if (nullptr != text)
  {
    ang = text->TextRotationRadians();
  }
  return ang;
}

void MYON_Annotation::SetTextRotationRadians(double rotation)
{
  MYON_TextContent* text = Text();
  if (nullptr != text)
  {
    text->SetTextRotationRadians(rotation);
  }
}

double MYON_Annotation::TextRotationDegrees() const
{
  double a = TextRotationRadians();
  return a * MYON_RADIANS_TO_DEGREES;
}

void MYON_Annotation::SetTextRotationDegrees(double rotation)
{
  double a = rotation * MYON_DEGREES_TO_RADIANS;
  SetTextRotationRadians(a);
}

MYON_SHA1_Hash MYON_Annotation::DimStyleTextPositionPropertiesHash() const
{
  const MYON_TextContent* text = Text();
  return
    (nullptr == text)
    ? MYON_TextContent::Empty.DimStyleTextPositionPropertiesHash()
    : text->DimStyleTextPositionPropertiesHash();
}

bool MYON_Annotation::EqualTextPositionProperties(
  const class MYON_DimStyle* dimstyle
) const
{
  const MYON_TextContent* text = Text();
  return
    (nullptr == text)
    ? (MYON_DimStyle::DimStyleOrDefault(dimstyle).TextPositionPropertiesHash() == DimStyleTextPositionPropertiesHash())
    : text->EqualTextPositionProperties( Type(), dimstyle );
}

static bool DimstyleHasMask(const MYON_DimStyle* dimstyle)
{
  return (nullptr == dimstyle || dimstyle->DrawTextMask() || dimstyle->MaskFrameType() != MYON_TextMask::MaskFrame::NoFrame);
}

const MYON_SHA1_Hash MYON_Annotation::Internal_GetBBox_InputHash(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  const MYON_2dPoint& text_point,
  unsigned int point_count,
  const MYON_2dPoint* points
) const
{
  MYON_SHA1 sha1;
  if (nullptr != vp)
    sha1.AccumulateSubHash(vp->ViewProjectionContentHash());
  if ( nullptr != dimstyle )
    sha1.AccumulateSubHash(dimstyle->ContentHash());
  sha1.AccumulateDouble(dimscale);
  
  sha1.Accumulate3dVector(m_plane.xaxis);
  sha1.Accumulate3dVector(m_plane.yaxis);
  sha1.Accumulate3dVector(m_plane.zaxis);
  sha1.AccumulateDoubleArray(4,&m_plane.plane_equation.x);

  if (nullptr != m_text)
  {
    // This return MYON_TextContent.m_mbox
    // which is a cached value
    const MYON_BoundingBox textbbox = m_text->BoundingBox();
    sha1.AccumulateBoundingBox(textbbox);
    if (textbbox.IsNotEmpty() && DimstyleHasMask(dimstyle))
    {
      sha1.AccumulateDouble(dimstyle->MaskBorder());
    }
  }

  sha1.Accumulate2dPoint(text_point);
  sha1.AccumulateUnsigned32(point_count);
  if (point_count > 0 && nullptr != points)
    sha1.AccumulateDoubleArray(2 * point_count, &points[0].x);

  return sha1.Hash();
}

bool MYON_Annotation::Internal_GetBBox_TextGlyphBox(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_BoundingBox& text_glyph_box
) const
{
  text_glyph_box = MYON_BoundingBox::UnsetBoundingBox;
  if (m_text)
  {
    MYON_Xform txf;
    // 20 June 2017 S. Baer (RH-39835)
    // GetTextXform can change cached information. Make sure this
    // is called before m_text->BoundingBox()
    // 23 June, 2020 - Lowell - Moved it above m_text->BoundingBox()
    bool b = GetTextXform(vp, dimstyle, dimscale, txf);

    text_glyph_box = m_text->BoundingBox();

    // if mask, grow 2d bbox
    if (text_glyph_box.IsNotEmpty() && DimstyleHasMask(dimstyle))
    {
      MYON_3dPoint bmin = text_glyph_box.Min();
      MYON_3dPoint bmax = text_glyph_box.Max();
      double d = dimstyle->MaskBorder();
      bmin.x -= d;
      bmin.y -= d;
      bmax.x += d;
      bmax.y += d;
      text_glyph_box.m_min = bmin;
      text_glyph_box.m_max = bmax;
    }

    if (b)
    {
      text_glyph_box.Transform(txf);
    }
  }
  return text_glyph_box.IsNotEmpty();
}

bool MYON_Annotation::Internal_GetBBox_Begin(
  const MYON_SHA1_Hash& hash,
  double* boxmin,
  double* boxmax,
  bool bGrow
) const
{
  MYON_BoundingBox bbox = MYON_BoundingBox::NanBoundingBox;
  if (nullptr != boxmin && nullptr != boxmax)
  {
    if (bGrow)
    {
      bbox.m_min = MYON_3dPoint(boxmin);
      bbox.m_max = MYON_3dPoint(boxmax);
      bGrow = bbox.IsNotEmpty();
    }
    if (false == bGrow)
      bbox = MYON_BoundingBox::EmptyBoundingBox;
    MYON_BoundingBox cached_bbox;
    const bool rc = m_bbox_cache.GetBoundingBox(hash, cached_bbox);
    if (rc)
    {
      if (bGrow)
        bbox.Union(cached_bbox);
      else
        bbox = cached_bbox;
    }
    boxmin[0] = bbox.m_min.x;
    boxmin[1] = bbox.m_min.y;
    boxmin[2] = bbox.m_min.z;
    boxmax[0] = bbox.m_max.x;
    boxmax[1] = bbox.m_max.y;
    boxmax[2] = bbox.m_max.z;
    return rc;
  }

  if (nullptr != boxmin)
  {
    boxmin[0] = boxmin[1] = boxmin[2] = MYON_DBL_QNAN;
  }
  if (nullptr != boxmax)
  {
    boxmax[0] = boxmax[1] = boxmax[2] = MYON_DBL_QNAN;
  }

  return false;
}

bool MYON_Annotation::Internal_GetBBox_End(
    const MYON_BoundingBox& bbox,
    const MYON_SHA1_Hash& hash,
    double* boxmin,
    double* boxmax,
    bool bGrow
) const
{
  if (bbox.IsNotEmpty())
  {
    m_bbox_cache.AddBoundingBox(bbox, hash);
  }

  MYON_BoundingBox box_out = bbox;
  if (bGrow && nullptr != boxmin && nullptr != boxmax )
  {
    MYON_BoundingBox box_in;
    box_in.m_min = MYON_3dPoint(boxmin);
    box_in.m_max = MYON_3dPoint(boxmax);
    if (box_in.IsNotEmpty())
      box_out.Union(box_in);
  }

  if (nullptr != boxmin)
  {
    boxmin[0] = box_out.m_min.x;
    boxmin[1] = box_out.m_min.y;
    boxmin[2] = box_out.m_min.z;
  }
  if (nullptr != boxmax)
  {
    boxmax[0] = box_out.m_max.x;
    boxmax[1] = box_out.m_max.y;
    boxmax[2] = box_out.m_max.z;
  }

  return box_out.IsNotEmpty();
}

const wchar_t* MYON_Annotation::RtfText() const
{
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return nullptr;
  return text->RtfText();
}

bool MYON_Annotation::ReplaceTextString(
  const wchar_t* RtfString,
  const MYON_DimStyle* dimstyle
)
{
  MYON_TextContent* text = Text();
  if (nullptr == text)
  {
    text = new MYON_TextContent;
    if (nullptr == text)
      return true;
    SetText(text);
    text = Text();
    if (nullptr == text)
      return true;
  }
  MYON_Dimension* dim = MYON_Dimension::Cast(this);
  if (nullptr != dim)
  {
    dim->SetUserText(RtfString);
    dim->ClearText();
    return true;
  }
  else
    return text->ReplaceTextString(RtfString, Type(), dimstyle);
}


bool MYON_Annotation::RunReplaceString(
  const MYON_DimStyle* dimstyle,
  const wchar_t* repl_str,
  int start_run_idx,
  int start_run_pos,
  int end_run_idx,
  int end_run_pos)
{
  MYON_TextContent* text_content = Text();
  if (nullptr == text_content)
    return false;
  bool rc = text_content->RunReplaceString(repl_str, start_run_idx, start_run_pos, end_run_idx, end_run_pos);

  text_content->ComposeText();
  
  text_content->RebuildRuns(Type(), dimstyle);

  
  return rc;
}


////double MYON_Annotation::Height() const
////{
////  //// Delete this function ASAP
////  //// TextHeight is on MYON_DimStyle
////  return MYON_DimStyle::Default.TextHeight();
////  //const MYON_TextContent* text = Text();
////  //if (nullptr == text)
////  //  return 1.0;
////  //return text->Height();
////}
////void MYON_Annotation::SetHeight(double height)
////{
////  //// Delete this function ASAP
////  //// TextHeight is on MYON_DimStyle
////  //MYON_TextContent* text = Text();
////  //if (nullptr != text)
////  //  text->SetHeight(height);
////}

void MYON_Annotation::GetAlignment(MYON::TextHorizontalAlignment& horz, MYON::TextVerticalAlignment& vert) const
{
  const MYON_TextContent* text = Text();
  if (nullptr == text)
  {
    horz = MYON::TextHorizontalAlignment::Left;
    vert = MYON::TextVerticalAlignment::Top;
  }
  else
  {
    text->GetAlignment(horz, vert);
  }
}

void MYON_Annotation::SetAlignment(MYON::TextHorizontalAlignment horz, MYON::TextVerticalAlignment vert)
{
  MYON_TextContent* text = Text();
  if (nullptr != text)
    text->SetAlignment(horz, vert);
}

double MYON_Annotation::FormattingRectangleWidth() const
{
  const MYON_TextContent* text = Text();
  if (nullptr != text)
    return text->FormattingRectangleWidth();
  return 1.0e300;
}

void MYON_Annotation::SetFormattingRectangleWidth(double width)
{
  MYON_TextContent* text = Text();
  if (nullptr != text)
    text->SetFormattingRectangleWidth(width);
}

bool MYON_Annotation::GetText3dCorners(MYON_3dPoint corners[4]) const
{
  if (nullptr == corners)
    return false;
  const MYON_TextContent* text_geom = Text();
  if (nullptr == text_geom)
    return false;
  return text_geom->Get3dCorners(corners);
}




//// ------------------ new overrides

bool MYON_Annotation::Internal_DimStyleDoubleChanged(
  const double current_value,
  double candidate_value
)
{
  const double tol = MYON_EPSILON * current_value;
  const double delta = fabs(current_value - candidate_value);
  return (delta > tol);
}


const MYON_DimStyle& MYON_Annotation::Internal_StyleForFieldQuery(
  const MYON_DimStyle* parent_style,
  MYON_DimStyle::field field_id
) const
{
  if (
    nullptr != m_override_dimstyle
    && m_override_dimstyle->IsFieldOverride(field_id)
    )
  {
    // There is an override style and this particular field value is overridden.
    return *m_override_dimstyle;
  }

  if (nullptr != parent_style)
  {
    // Even when there is an override style, it's contents may not be up to date
    // with the parent style. That's why we check m_override_dimstyle->IsFieldOverride(field_id)
    // above before returning m_override_dimstyle.
    return *parent_style;
  }

  // returning the m_override_dimstyle style here is done because 
  // there is some chance it will be up to date and its contents
  // are more applicable than the default settings.
  if (nullptr != m_override_dimstyle)
    return *m_override_dimstyle;

  // This is not a good situation. It means the developer could not get a parent
  // style from the context of the query.
  return MYON_DimStyle::Default;
}


MYON_DimStyle* MYON_Annotation::Internal_GetOverrideStyle(bool bCreateIfNull) const
{
  if (nullptr == m_override_dimstyle && bCreateIfNull)
  {
    MYON_DimStyle* style = new MYON_DimStyle;
    SetOverrideDimensionStyle(style);
  }
  return m_override_dimstyle;
}

bool MYON_Annotation::FieldIsOverridden(MYON_DimStyle::field field) const
{
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(false);
  if (nullptr == override_style)
    return false;
  return override_style->IsFieldOverride(field);
}

void MYON_Annotation::ClearFieldOverride(MYON_DimStyle::field field)
{
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(false);
  if (nullptr != override_style)
  {
    override_style->SetFieldOverride(field, false);
    if (!override_style->HasOverrides())
    {
      // Delete the override style if that was the last overridden field
      MYON_DimStyle* nullstyle = nullptr;
      SetOverrideDimensionStyle(nullstyle);
    }
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////

double MYON_Annotation::ExtensionLineExtension(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ExtensionLineExtension).ExtExtension();
}

void MYON_Annotation::SetExtensionLineExtension(const MYON_DimStyle* parent_style, double extension)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(extension, parent_style->ExtExtension());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetExtExtension(extension);
    override_style->SetFieldOverride(MYON_DimStyle::field::ExtensionLineExtension, bCreate);
  }
}

double MYON_Annotation::ExtensionLineOffset(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ExtensionLineOffset).ExtOffset();
}

void MYON_Annotation::SetExtensionLineOffset(const MYON_DimStyle* parent_style, double offset)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(offset, parent_style->ExtOffset());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetExtOffset(offset);
    override_style->SetFieldOverride(MYON_DimStyle::field::ExtensionLineOffset, bCreate);
  }
}

double MYON_Annotation::ArrowSize(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Arrowsize).ArrowSize();
}

void MYON_Annotation::SetArrowSize(const MYON_DimStyle* parent_style, double size)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(size, parent_style->ArrowSize());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArrowSize(size);
    override_style->SetFieldOverride(MYON_DimStyle::field::Arrowsize, bCreate);
  }
}

double MYON_Annotation::LeaderArrowSize(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderArrowsize).LeaderArrowSize();
}

void MYON_Annotation::SetLeaderArrowSize(const MYON_DimStyle* parent_style, double size)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(size, parent_style->LeaderArrowSize());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderArrowSize(size);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderArrowsize, bCreate);
  }
}

double MYON_Annotation::CenterMarkSize(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Centermark).CenterMark();
}

void MYON_Annotation::SetCenterMarkSize(const MYON_DimStyle* parent_style, double size)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(size, parent_style->CenterMark());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetCenterMark(size);
    override_style->SetFieldOverride(MYON_DimStyle::field::Centermark, bCreate);
  }
}

MYON_DimStyle::centermark_style MYON_Annotation::CenterMarkStyle(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::CentermarkStyle).CenterMarkStyle();
}

void MYON_Annotation::SetCenterMarkStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::centermark_style style)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (style != parent_style->CenterMarkStyle());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetCenterMarkStyle(style);
    override_style->SetFieldOverride(MYON_DimStyle::field::CentermarkStyle, bCreate);
  }
}

MYON_DimStyle::TextLocation MYON_Annotation::DimTextLocation(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DimTextLocation).DimTextLocation();
}

void MYON_Annotation::SetDimTextLocation(const MYON_DimStyle* parent_style, MYON_DimStyle::TextLocation text_location)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (text_location != parent_style->DimTextLocation());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimTextLocation(text_location);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimTextLocation, bCreate);
  }
}

MYON_DimStyle::TextLocation MYON_Annotation::DimRadialTextLocation(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DimRadialTextLocation).DimRadialTextLocation();
}

void MYON_Annotation::SetDimRadialTextLocation(const MYON_DimStyle* parent_style, MYON_DimStyle::TextLocation text_location)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (text_location != parent_style->DimRadialTextLocation());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimRadialTextLocation(text_location);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimRadialTextLocation, bCreate);
  }
}

MYON_DimStyle::angle_format MYON_Annotation::AngleFormat(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AngleFormat).AngleFormat();
}

void MYON_Annotation::SetAngleFormat(const MYON_DimStyle* parent_style, MYON_DimStyle::angle_format format)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (format != parent_style->AngleFormat());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAngleFormat(format);
    override_style->SetFieldOverride(MYON_DimStyle::field::AngleFormat, bCreate);
  }
}

int MYON_Annotation::LengthResolution(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LengthResolution).LengthResolution();
}

void MYON_Annotation::SetLengthResolution(const MYON_DimStyle* parent_style, int resolution)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (resolution != parent_style->LengthResolution());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLengthResolution(resolution);
    override_style->SetFieldOverride(MYON_DimStyle::field::LengthResolution, bCreate);
  }
}

int MYON_Annotation::AngleResolution(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AngleResolution).AngleResolution();
}

void MYON_Annotation::SetAngleResolution(const MYON_DimStyle* parent_style, int resolution)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (resolution != parent_style->AngleResolution());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAngleResolution(resolution);
    override_style->SetFieldOverride(MYON_DimStyle::field::AngleResolution, bCreate);
  }
}

double MYON_Annotation::TextGap(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::TextGap).TextGap();
}

void MYON_Annotation::SetTextGap(const MYON_DimStyle* parent_style, double gap)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(gap, parent_style->TextGap());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextGap(gap);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextGap, bCreate);
  }
}

double MYON_Annotation::TextHeight(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::TextHeight).TextHeight();
}

void MYON_Annotation::SetTextHeight(const MYON_DimStyle* parent_style, double height)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(height, parent_style->TextHeight());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextHeight(height);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextHeight, bCreate);
  }
}

double MYON_Annotation::LengthFactor(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LengthFactor).LengthFactor();
}

void MYON_Annotation::SetLengthFactor(const MYON_DimStyle* parent_style, double factor)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(factor, parent_style->LengthFactor());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLengthFactor(factor);
    override_style->SetFieldOverride(MYON_DimStyle::field::LengthFactor, bCreate);
  }
}

bool MYON_Annotation::Alternate(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Alternate).Alternate();
}

void MYON_Annotation::SetAlternate(const MYON_DimStyle* parent_style, bool alternate)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (alternate != parent_style->Alternate());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternate(alternate);
    override_style->SetFieldOverride(MYON_DimStyle::field::Alternate, bCreate);
  }
}

double MYON_Annotation::AlternateLengthFactor(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AlternateLengthFactor).AlternateLengthFactor();
}

void MYON_Annotation::SetAlternateLengthFactor(const MYON_DimStyle* parent_style, double factor)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(factor, parent_style->AlternateLengthFactor());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternateLengthFactor(factor);
    override_style->SetFieldOverride(MYON_DimStyle::field::AlternateLengthFactor, bCreate);
  }
}

int MYON_Annotation::AlternateLengthResolution(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AlternateLengthResolution).AlternateLengthResolution();
}

void MYON_Annotation::SetAlternateLengthResolution(const MYON_DimStyle* parent_style, int resolution)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (resolution != parent_style->AlternateLengthResolution());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternateLengthResolution(resolution);
    override_style->SetFieldOverride(MYON_DimStyle::field::AlternateLengthResolution, bCreate);
  }
}

const wchar_t* MYON_Annotation::Prefix(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Prefix).Prefix();
}

void MYON_Annotation::SetPrefix(const MYON_DimStyle* parent_style, const wchar_t* prefix)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = !MYON_wString::EqualOrdinal(prefix, parent_style->Prefix(), false);
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetPrefix(prefix);
    override_style->SetFieldOverride(MYON_DimStyle::field::Prefix, bCreate);
  }
}

const wchar_t* MYON_Annotation::Suffix(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Suffix).Suffix();
}

void MYON_Annotation::SetSuffix(const MYON_DimStyle* parent_style, const wchar_t* suffix)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = !MYON_wString::EqualOrdinal(suffix, parent_style->Suffix(), false);
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetSuffix(suffix);
    override_style->SetFieldOverride(MYON_DimStyle::field::Suffix, bCreate);
  }
}

const wchar_t* MYON_Annotation::AlternatePrefix(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AlternatePrefix).AlternatePrefix();
}

void MYON_Annotation::SetAlternatePrefix(const MYON_DimStyle* parent_style, const wchar_t* prefix)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = !MYON_wString::EqualOrdinal(prefix, parent_style->Prefix(), false);
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternatePrefix(prefix);
    override_style->SetFieldOverride(MYON_DimStyle::field::AlternatePrefix, bCreate);
  }
}

const wchar_t* MYON_Annotation::AlternateSuffix(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AlternateSuffix).AlternateSuffix();
}

void MYON_Annotation::SetAlternateSuffix(const MYON_DimStyle* parent_style, const wchar_t* suffix)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = !MYON_wString::EqualOrdinal(suffix, parent_style->Suffix(), false);
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternateSuffix(suffix);
    override_style->SetFieldOverride(MYON_DimStyle::field::AlternateSuffix, bCreate);
  }
}

bool MYON_Annotation::SuppressExtension1(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::SuppressExtension1).SuppressExtension1();
}

void MYON_Annotation::SetSuppressExtension1(const MYON_DimStyle* parent_style, bool suppress)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (suppress != parent_style->SuppressExtension1());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetSuppressExtension1(suppress);
    override_style->SetFieldOverride(MYON_DimStyle::field::SuppressExtension1, bCreate);
  }
}

bool MYON_Annotation::SuppressExtension2(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::SuppressExtension2).SuppressExtension2();
}

void MYON_Annotation::SetSuppressExtension2(const MYON_DimStyle* parent_style, bool suppress)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (suppress != parent_style->SuppressExtension2());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetSuppressExtension2(suppress);
    override_style->SetFieldOverride(MYON_DimStyle::field::SuppressExtension2, bCreate);
  }
}

double MYON_Annotation::DimExtension(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::DimensionLineExtension).DimExtension();
}

void MYON_Annotation::SetDimExtension(const MYON_DimStyle* parent_style, double extension)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(extension, parent_style->DimExtension());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimExtension(extension);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimensionLineExtension, bCreate);
  }
}

MYON_DimStyle::tolerance_format MYON_Annotation::ToleranceFormat(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ToleranceFormat).ToleranceFormat();
}

void MYON_Annotation::SetToleranceFormat(const MYON_DimStyle* parent_style, MYON_DimStyle::tolerance_format format)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (format != parent_style->ToleranceFormat());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetToleranceFormat(format);
    override_style->SetFieldOverride(MYON_DimStyle::field::ToleranceFormat, bCreate);
  }
}

int MYON_Annotation::ToleranceResolution(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ToleranceResolution).ToleranceResolution();
}

void MYON_Annotation::SetToleranceResolution(const MYON_DimStyle* parent_style, int resolution)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (resolution != parent_style->ToleranceResolution());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetToleranceResolution(resolution);
    override_style->SetFieldOverride(MYON_DimStyle::field::ToleranceResolution, bCreate);
  }
}

double MYON_Annotation::ToleranceUpperValue(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ToleranceUpperValue).ToleranceUpperValue();
}

void MYON_Annotation::SetToleranceUpperValue(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->ToleranceUpperValue());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetToleranceUpperValue(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ToleranceUpperValue, bCreate);
  }
}

double MYON_Annotation::ToleranceLowerValue(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ToleranceLowerValue).ToleranceLowerValue();
}

void MYON_Annotation::SetToleranceLowerValue(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->ToleranceLowerValue());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetToleranceLowerValue(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ToleranceLowerValue, bCreate);
  }
}

double MYON_Annotation::ToleranceHeightScale(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ToleranceHeightScale).ToleranceHeightScale();
}

void MYON_Annotation::SetToleranceHeightScale(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->ToleranceHeightScale());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetToleranceHeightScale(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ToleranceHeightScale, bCreate);
  }
}

double MYON_Annotation::BaselineSpacing(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::BaselineSpacing).BaselineSpacing();
}

void MYON_Annotation::SetBaselineSpacing(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->BaselineSpacing());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetBaselineSpacing(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::BaselineSpacing, bCreate);
  }
}

bool MYON_Annotation::DrawTextMask(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::DrawMask).DrawTextMask();
}

void MYON_Annotation::SetDrawTextMask(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->DrawTextMask());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDrawTextMask(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::DrawMask, bCreate);
  }
}

MYON_TextMask::MaskType MYON_Annotation::MaskFillType(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::MaskColorSource).MaskFillType();
}

void MYON_Annotation::SetMaskFillType(const MYON_DimStyle* parent_style, MYON_TextMask::MaskType value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->MaskFillType());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetMaskFillType(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::MaskColorSource, bCreate);
  }
}

MYON_TextMask::MaskFrame MYON_Annotation::MaskFrameType(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::MaskFrameType).MaskFrameType();
}

void MYON_Annotation::SetMaskFrameType(const MYON_DimStyle* parent_style, MYON_TextMask::MaskFrame value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->MaskFrameType());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetMaskFrameType(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::MaskFrameType, bCreate);
  }
}

MYON_Color MYON_Annotation::MaskColor(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::MaskColor).MaskColor();
}

void MYON_Annotation::SetMaskColor(const MYON_DimStyle* parent_style, MYON_Color value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->MaskColor());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetMaskColor(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::MaskColor, bCreate);
  }
}

double MYON_Annotation::MaskBorder(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::MaskBorder).MaskBorder();
}

void MYON_Annotation::SetMaskBorder(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->MaskBorder());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetMaskBorder(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::MaskBorder, bCreate);
  }
}

void MYON_Annotation::SetTextMask(const MYON_DimStyle* parent_style, const MYON_TextMask& mask)
{
  // In order for overrides to work correctly, each text mask property must be
  // set individually.
  const MYON_TextMask local_mask(mask);
  SetDrawTextMask(parent_style, local_mask.DrawTextMask());
  SetMaskColor(parent_style, local_mask.MaskColor());
  SetMaskFillType(parent_style, local_mask.MaskFillType());
  SetMaskBorder(parent_style, local_mask.MaskBorder());
  SetMaskFrameType(parent_style, local_mask.MaskFrameType());
}

double MYON_Annotation::FixedExtensionLength(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::FixedExtensionLength).FixedExtensionLen();
}

void MYON_Annotation::SetFixedExtensionLength(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->FixedExtensionLen());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetFixedExtensionLen(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::FixedExtensionLength, bCreate);
  }
}

bool MYON_Annotation::FixedExtensionLengthOn(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::FixedExtensionOn).FixedExtensionLenOn();
}

void MYON_Annotation::SetFixedExtensionLengthOn(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->FixedExtensionLenOn());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetFixedExtensionLenOn(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::FixedExtensionOn, bCreate);
  }
}

int MYON_Annotation::AlternateToleranceResolution(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AltToleranceResolution).AlternateToleranceResolution();
}

void MYON_Annotation::SetAlternateToleranceResolution(const MYON_DimStyle* parent_style, int value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->AlternateToleranceResolution());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternateToleranceResolution(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::AltToleranceResolution, bCreate);
  }
}

bool MYON_Annotation::SuppressArrow1(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::SuppressArrow1).SuppressArrow1();
}

void MYON_Annotation::SetSuppressArrow1(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->SuppressArrow1());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetSuppressArrow1(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::SuppressArrow1, bCreate);
  }
}

bool MYON_Annotation::SuppressArrow2(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::SuppressArrow2).SuppressArrow2();
}

void MYON_Annotation::SetSuppressArrow2(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->SuppressArrow2());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetSuppressArrow2(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::SuppressArrow2, bCreate);
  }
}

int MYON_Annotation::TextMoveLeader(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::TextmoveLeader).TextMoveLeader();
}

void MYON_Annotation::SetTextMoveLeader(const MYON_DimStyle* parent_style, int value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->TextMoveLeader());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextMoveLeader(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextmoveLeader, bCreate);
  }
}

int MYON_Annotation::ArcLengthSymbol(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ArclengthSymbol).ArcLengthSymbol();
}

void MYON_Annotation::SetArcLengthSymbol(const MYON_DimStyle* parent_style, int value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->ArcLengthSymbol());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArcLengthSymbol(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ArclengthSymbol, bCreate);
  }
}

MYON_DimStyle::stack_format MYON_Annotation::StackFractionFormat(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::StackFormat).StackFractionFormat();
}

void MYON_Annotation::SetStackFractionFormat(const MYON_DimStyle* parent_style, MYON_DimStyle::stack_format value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->StackFractionFormat());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetStackFractionFormat(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::StackFormat, bCreate);
  }
}

double MYON_Annotation::StackHeightScale(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::StackTextheightScale).StackHeightScale();
}

void MYON_Annotation::SetStackHeightScale(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->StackHeightScale());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetStackHeightScale(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::StackTextheightScale, bCreate);
  }
}

double MYON_Annotation::RoundOff(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Round).RoundOff();
}

void MYON_Annotation::SetRoundOff(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->RoundOff());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetRoundOff(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::Round, bCreate);
  }
}

double MYON_Annotation::AlternateRoundOff(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AltRound).AlternateRoundOff();
}

void MYON_Annotation::SetAlternateRoundOff(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->AlternateRoundOff());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternateRoundOff(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::AltRound, bCreate);
  }
}

double MYON_Annotation::AngleRoundOff(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AngularRound).AngleRoundOff();
}

void MYON_Annotation::SetAngleRoundOff(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->AngleRoundOff());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAngleRoundOff(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::AngularRound, bCreate);
  }
}

bool MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(
  MYON_DimStyle::suppress_zero zero_suppress,
  MYON_DimStyle::LengthDisplay length_display)
{
  bool rc = true;

  switch (length_display)
  {
  case MYON_DimStyle::LengthDisplay::ModelUnits:
    rc = true;
    break;

  case MYON_DimStyle::LengthDisplay::InchesFractional:
    if (
      MYON_DimStyle::suppress_zero::None != zero_suppress
      )
    {
      rc = false;
    }
    break;

  case MYON_DimStyle::LengthDisplay::FeetAndInches:
    if (
      MYON_DimStyle::suppress_zero::None != zero_suppress
      && MYON_DimStyle::suppress_zero::SuppressZeroFeet != zero_suppress
      && MYON_DimStyle::suppress_zero::SuppressZeroInches != zero_suppress
      && MYON_DimStyle::suppress_zero::SuppressZeroFeetAndZeroInches != zero_suppress
      )
    {
      rc = false;
    }
    break;

  default: // decimal format or any future format
    if (
      MYON_DimStyle::suppress_zero::None != zero_suppress
      && MYON_DimStyle::suppress_zero::SuppressLeading != zero_suppress
      && MYON_DimStyle::suppress_zero::SuppressTrailing != zero_suppress
      && MYON_DimStyle::suppress_zero::SuppressLeadingAndTrailing != zero_suppress
      )
    {
      rc = false;
    }
    break;
  }

  return rc;

}

MYON_DimStyle::suppress_zero MYON_Annotation::ZeroSuppress(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ZeroSuppress).ZeroSuppress();
}

 

void MYON_Annotation::SetZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->ZeroSuppress());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    if (false == override_style->IsFieldOverride(MYON_DimStyle::field::DimensionLengthDisplay))
    {
      // Make the DimensionLengthDisplay field on the override style match the parent style
      // but mark it not overridden so override_style->SetZeroSuppress(value) doesn't call an error
      MYON_DimStyle::LengthDisplay x = DimensionLengthDisplay(parent_style);
      if (x != override_style->DimensionLengthDisplay())
      {
        override_style->SetDimensionLengthDisplay(x);
        override_style->SetFieldOverride(MYON_DimStyle::field::DimensionLengthDisplay, false);
      }
    }

    if (MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(value, DimensionLengthDisplay(parent_style)))
    {
      override_style->SetZeroSuppress(value);
      override_style->SetFieldOverride(MYON_DimStyle::field::ZeroSuppress, bCreate);
    }
    else
    {
      MYON_ERROR("Attempting to set zero suppression to a value that doesn't match length display.");
    }
  }
}

MYON_DimStyle::suppress_zero MYON_Annotation::AlternateZeroSuppress(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AltZeroSuppress).AlternateZeroSuppress();
}

void MYON_Annotation::SetAlternateZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->AlternateZeroSuppress());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    if (false == override_style->IsFieldOverride(MYON_DimStyle::field::AlternateDimensionLengthDisplay))
    {
      // Make the DimensionLengthDisplay field on the override style match the parent style
      // but mark it not overridden so override_style->SetZeroSuppress(value) doesn't call an error
      MYON_DimStyle::LengthDisplay x = AlternateDimensionLengthDisplay(parent_style);
      if (x != override_style->AlternateDimensionLengthDisplay())
      {
        override_style->SetAlternateDimensionLengthDisplay(x);
        override_style->SetFieldOverride(MYON_DimStyle::field::AlternateDimensionLengthDisplay, false);
      }
    }

    if (MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(value, override_style->AlternateDimensionLengthDisplay()))
    {
      override_style->SetAlternateZeroSuppress(value);
      override_style->SetFieldOverride(MYON_DimStyle::field::AltZeroSuppress, bCreate);
    }
    else
    {
      MYON_ERROR("Attempting to set zero suppression to a value that doesn't match length display.");
    }
  }
}

MYON_DimStyle::suppress_zero MYON_Annotation::ToleranceZeroSuppress(const MYON_DimStyle* parent_style) const
{
  // OBSOLETE PROPERTY
  // The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
  return MYON_DimStyle::suppress_zero::None;
}

void MYON_Annotation::SetToleranceZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero value)
{
  // OBSOLETE PROPERTY
  // The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
}

MYON_DimStyle::suppress_zero MYON_Annotation::AngleZeroSuppress(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AngleZeroSuppress).AngleZeroSuppress();
}

void MYON_Annotation::SetAngleZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->AngleZeroSuppress());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAngleZeroSuppress(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::AngleZeroSuppress, bCreate);
  }
}

bool MYON_Annotation::AlternateBelow(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AltBelow).AlternateBelow();
}

void MYON_Annotation::SetAlternateBelow(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->AlternateBelow());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternateBelow(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::AltBelow, bCreate);
  }
}

MYON_Arrowhead::arrow_type MYON_Annotation::ArrowType1(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ArrowType1).ArrowType1();
}

void MYON_Annotation::SetArrowType1(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->ArrowType1());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArrowType1(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ArrowType1, bCreate);
  }
}

MYON_Arrowhead::arrow_type MYON_Annotation::ArrowType2(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ArrowType2).ArrowType2();
}

void MYON_Annotation::SetArrowType2(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->ArrowType2());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArrowType2(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ArrowType2, bCreate);
  }
}

void MYON_Annotation::SetArrowType1And2(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = ((value != parent_style->ArrowType1()) ||
                  (value != parent_style->ArrowType2()));
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArrowType1And2(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ArrowType1, bCreate);
    override_style->SetFieldOverride(MYON_DimStyle::field::ArrowType2, bCreate);
  }
}

MYON_Arrowhead::arrow_type MYON_Annotation::LeaderArrowType(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderArrowType).LeaderArrowType();
}

void MYON_Annotation::SetLeaderArrowType(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderArrowType());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderArrowType(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderArrowType, bCreate);
  }
}

MYON_UUID MYON_Annotation::ArrowBlockId1(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ArrowBlockId1).ArrowBlockId1();
}

void MYON_Annotation::SetArrowBlockId1(const MYON_DimStyle* parent_style, MYON_UUID value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->ArrowBlockId1());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArrowBlockId1(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ArrowBlockId1, bCreate);
  }
}

MYON_UUID MYON_Annotation::ArrowBlockId2(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::ArrowBlockId2).ArrowBlockId2();
}

void MYON_Annotation::SetArrowBlockId2(const MYON_DimStyle* parent_style, MYON_UUID value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->ArrowBlockId2());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArrowBlockId2(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::ArrowBlockId2, bCreate);
  }
}

MYON_UUID MYON_Annotation::LeaderArrowBlockId(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderArrowBlock).LeaderArrowBlockId();
}

void MYON_Annotation::SetLeaderArrowBlockId(const MYON_DimStyle* parent_style, MYON_UUID value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderArrowBlockId());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderArrowBlockId(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderArrowBlock, bCreate);
  }
}

MYON::TextVerticalAlignment MYON_Annotation::TextVerticalAlignment(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::TextVerticalAlignment).TextVerticalAlignment();
}

void MYON_Annotation::SetTextVerticalAlignment(const MYON_DimStyle* parent_style, MYON::TextVerticalAlignment value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->TextVerticalAlignment());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextVerticalAlignment(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextVerticalAlignment, bCreate);
  }
}

MYON::TextVerticalAlignment MYON_Annotation::LeaderTextVerticalAlignment(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderTextVerticalAlignment).LeaderTextVerticalAlignment();
}

void MYON_Annotation::SetLeaderTextVerticalAlignment(const MYON_DimStyle* parent_style, MYON::TextVerticalAlignment value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderTextVerticalAlignment());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderTextVerticalAlignment(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderTextVerticalAlignment, bCreate);
  }
}

MYON_DimStyle::ContentAngleStyle MYON_Annotation::LeaderContentAngleStyle(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderContentAngleStyle).LeaderContentAngleStyle();
}

void MYON_Annotation::SetLeaderContentAngleStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::ContentAngleStyle value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderContentAngleStyle());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderContentAngleStyle(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderContentAngleStyle, bCreate);
  }
}

MYON_DimStyle::leader_curve_type MYON_Annotation::LeaderCurveType(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderCurveType).LeaderCurveType();
}

void MYON_Annotation::SetLeaderCurveType(const MYON_DimStyle* parent_style, MYON_DimStyle::leader_curve_type value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderCurveType());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderCurveType(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderCurveType, bCreate);
  }
}

bool MYON_Annotation::LeaderHasLanding(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderHasLanding).LeaderHasLanding();
}

void MYON_Annotation::SetLeaderHasLanding(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderHasLanding());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderHasLanding(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderHasLanding, bCreate);
  }
}

double MYON_Annotation::LeaderLandingLength(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderLandingLength).LeaderLandingLength();
}

void MYON_Annotation::SetLeaderLandingLength(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->LeaderLandingLength());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderLandingLength(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderLandingLength, bCreate);
  }
}

double MYON_Annotation::LeaderContentAngleRadians(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderContentAngle).LeaderContentAngleRadians();
}

void MYON_Annotation::SetLeaderContentAngleRadians(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->LeaderContentAngleRadians());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderContentAngleRadians(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderContentAngle, bCreate);
  }
}

double MYON_Annotation::LeaderContentAngleDegrees(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderContentAngle).LeaderContentAngleDegrees();
}

void MYON_Annotation::SetLeaderContentAngleDegrees(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->LeaderContentAngleDegrees());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderContentAngleDegrees(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderContentAngle, bCreate);
  }
}

MYON::TextHorizontalAlignment MYON_Annotation::TextHorizontalAlignment(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::TextHorizontalAlignment).TextHorizontalAlignment();
}

void MYON_Annotation::SetTextHorizontalAlignment(const MYON_DimStyle* parent_style, MYON::TextHorizontalAlignment value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->TextHorizontalAlignment());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextHorizontalAlignment(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextHorizontalAlignment, bCreate);
  }
}

MYON::TextHorizontalAlignment MYON_Annotation::LeaderTextHorizontalAlignment(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::LeaderTextHorizontalAlignment).LeaderTextHorizontalAlignment();
}

void MYON_Annotation::SetLeaderTextHorizontalAlignment(const MYON_DimStyle* parent_style, MYON::TextHorizontalAlignment value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderTextHorizontalAlignment());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderTextHorizontalAlignment(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderTextHorizontalAlignment, bCreate);
  }
}

MYON_DimStyle::ContentAngleStyle MYON_Annotation::DimTextAngleStyle(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DimTextAngleStyle).DimTextAngleStyle();
}

void MYON_Annotation::SetDimTextAngleStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::ContentAngleStyle value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->DimTextAngleStyle());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimTextAngleStyle(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimTextAngleStyle, bCreate);
  }
}

MYON_DimStyle::ContentAngleStyle MYON_Annotation::DimRadialTextAngleStyle(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DimRadialTextAngleStyle).DimRadialTextAngleStyle();
}

void MYON_Annotation::SetDimRadialTextAngleStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::ContentAngleStyle value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->DimRadialTextAngleStyle());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimRadialTextAngleStyle(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimRadialTextAngleStyle, bCreate);
  }
}

MYON::TextOrientation MYON_Annotation::TextOrientation(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::TextOrientation).TextOrientation();
}

void MYON_Annotation::SetTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->TextOrientation());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextOrientation(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextOrientation, bCreate);
  }
}

MYON::TextOrientation MYON_Annotation::LeaderTextOrientation(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::LeaderTextOrientation).LeaderTextOrientation();
}

void MYON_Annotation::SetLeaderTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->LeaderTextOrientation());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetLeaderTextOrientation(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::LeaderTextOrientation, bCreate);
  }
}

MYON::TextOrientation MYON_Annotation::DimTextOrientation(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DimTextOrientation).DimTextOrientation();
}

void MYON_Annotation::SetDimTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->DimTextOrientation());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimTextOrientation(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimTextOrientation, bCreate);
  }
}

MYON::TextOrientation MYON_Annotation::DimRadialTextOrientation(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DimRadialTextOrientation).DimRadialTextOrientation();
}

void MYON_Annotation::SetDimRadialTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->DimRadialTextOrientation());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimRadialTextOrientation(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimRadialTextOrientation, bCreate);
  }
}

bool MYON_Annotation::TextUnderlined(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::TextUnderlined).TextUnderlined();
}

void MYON_Annotation::SetTextUnderlined(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->TextUnderlined());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextUnderlined(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextUnderlined, bCreate);
  }
}

bool MYON_Annotation::DrawForward(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DrawForward).DrawForward();
}

void MYON_Annotation::SetDrawForward(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->DrawForward());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDrawForward(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::DrawForward, bCreate);
  }
}

bool MYON_Annotation::SignedOrdinate(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::SignedOrdinate).SignedOrdinate();
}

void MYON_Annotation::SetSignedOrdinate(const MYON_DimStyle* parent_style, bool value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (value != parent_style->SignedOrdinate());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetSignedOrdinate(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::SignedOrdinate, bCreate);
  }
}

double MYON_Annotation::DimScale(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::DimensionScale).DimScale();
}

void MYON_Annotation::SetDimScale(const MYON_DimStyle* parent_style, double value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = Internal_DimStyleDoubleChanged(value, parent_style->DimScale());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimScale(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimensionScale, bCreate);
  }
}

wchar_t MYON_Annotation::DecimalSeparator(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DecimalSeparator).DecimalSeparator();
}

void MYON_Annotation::SetDecimalSeparator(const MYON_DimStyle* parent_style, wchar_t separator)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (separator != parent_style->DecimalSeparator());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDecimalSeparator(separator);
    override_style->SetFieldOverride(MYON_DimStyle::field::DecimalSeparator, bCreate);
  }
}

const class MYON_Font& MYON_Annotation::Font(const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Font).Font();
}

const class MYON_Font& MYON_Annotation::FontCharacteristics(const MYON_DimStyle* parent_style) const
{
  // FontCharacteristics() queries inforation that is set by calling MYON_DimStyle.SetFont()
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Font).FontCharacteristics();
}

const bool MYON_Annotation::FontSubstituted(const MYON_DimStyle* parent_style) const
{
  // FontSubstituted() queries inforation that is set by calling MYON_DimStyle.SetFont()
  return Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::Font).FontSubstituted();
}

void MYON_Annotation::SetFont(const MYON_DimStyle* parent_style, const class MYON_Font& value)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (0 != MYON_Font::CompareFontCharacteristics(value, parent_style->Font()));
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetFont(value);
    override_style->SetFieldOverride(MYON_DimStyle::field::Font, bCreate);
  }
}

MYON_DimStyle::LengthDisplay MYON_Annotation::DimensionLengthDisplay(const MYON_DimStyle* parent_style) const
{
  const MYON_DimStyle::LengthDisplay dim_length_display
    = Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::DimensionLengthDisplay).DimensionLengthDisplay();
  return dim_length_display;
}

MYON_DimStyle::LengthDisplay MYON_Annotation::AlternateDimensionLengthDisplay(const MYON_DimStyle* parent_style) const
{
  const MYON_DimStyle::LengthDisplay dim_length_display
    = Internal_StyleForFieldQuery(parent_style,MYON_DimStyle::field::AlternateDimensionLengthDisplay).AlternateDimensionLengthDisplay();
  return dim_length_display;
}

void MYON_Annotation::SetDimensionLengthDisplay(const MYON_DimStyle* parent_style, MYON_DimStyle::LengthDisplay length_display)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (length_display != parent_style->DimensionLengthDisplay());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetDimensionLengthDisplay(length_display);
    override_style->SetFieldOverride(MYON_DimStyle::field::DimensionLengthDisplay, bCreate);
  }
}

void MYON_Annotation::SetAlternateDimensionLengthDisplay(const MYON_DimStyle* parent_style, MYON_DimStyle::LengthDisplay length_display)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (length_display != parent_style->AlternateDimensionLengthDisplay());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetAlternateDimensionLengthDisplay(length_display);
    override_style->SetFieldOverride(MYON_DimStyle::field::AlternateDimensionLengthDisplay, bCreate);
  }
}

MYON::LengthUnitSystem MYON_Annotation::DimensionLengthDisplayUnit(
  const MYON_DimStyle* parent_style,
  unsigned int model_sn
) const
{
  if (MYON_UNSET_UINT_INDEX == model_sn && nullptr != parent_style)
    model_sn = parent_style->ModelSerialNumber();
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::DimensionLengthDisplay).DimensionLengthDisplayUnit(model_sn);
}

MYON::LengthUnitSystem MYON_Annotation::AlternateDimensionLengthDisplayUnit(
  const MYON_DimStyle* parent_style,
  unsigned int model_sn
) const
{
  if (MYON_UNSET_UINT_INDEX == model_sn && nullptr != parent_style)
    model_sn = parent_style->ModelSerialNumber();
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::AlternateDimensionLengthDisplay).DimensionLengthDisplayUnit(model_sn);
}
//--------------------------------

bool MYON_Annotation::ClearRtfFmt(const wchar_t* fmt_str_on, const wchar_t* fmt_str_off, MYON_wString& rtf_in)
{
  MYON_wString fmt_off = fmt_str_off;
  fmt_off += "{";
  rtf_in.Replace(fmt_off.Array(), L"{");
  fmt_off = fmt_str_off;
  fmt_off += "}";
  rtf_in.Replace(fmt_off.Array(), L"}");
  fmt_off = fmt_str_off;
  fmt_off += "\\";
  rtf_in.Replace(fmt_off.Array(), L"\\");
  fmt_off = fmt_str_off;
  fmt_off += " ";
  rtf_in.Replace(fmt_off.Array(), L"");

  MYON_wString fmt_on = fmt_str_on;
  fmt_on += "{";
  rtf_in.Replace(fmt_on.Array(), L"{");
  fmt_on = fmt_str_on;
  fmt_on += "}";
  rtf_in.Replace(fmt_on.Array(), L"}");
  fmt_on = fmt_str_on;
  fmt_on += "\\";
  rtf_in.Replace(fmt_on.Array(), L"\\");
  fmt_on = fmt_str_on;
  fmt_on += " ";
  rtf_in.Replace(fmt_on.Array(), L"");


  return true;
}

int MYON_Annotation::FindRtfTable(MYON_wString rtf_in, int startidx, const wchar_t* tablename)
{
  int count = rtf_in.Length();
  int idx = rtf_in.Find(tablename, startidx);
  if (-1 != idx)
  {
    int open = 0;
    for (int ci = idx; ci < count; ci++)
    {
      if (L'{' == rtf_in[ci])
        open++;
      else if (L'}' == rtf_in[ci])
        open--;
      if (0 == open)
      {
        idx = ci+1;
        break;
      }
    }
  }
  return idx;
}

bool MYON_Annotation::SetRtfFmt(MYON_wString& rtf_in, const wchar_t* fmt_str)
{
  MYON_wString rtf = rtf_in;

  int idx = rtf.Find(L"{\\rtf1");
  if (-1 == idx)
  {
    rtf_in.Format(L"{\\rtf1{%s %s}}", fmt_str, rtf.Array());
    return true;
  }
  int count = rtf.Length();
  if (-1 < idx && idx < count)
  {
    idx += 6;
    int nextidx = MYON_Annotation::FindRtfTable(rtf, idx, L"{\\fonttbl");
    if (-1 != nextidx)
      idx = nextidx;

    nextidx = MYON_Annotation::FindRtfTable(rtf, idx, L"{\\colortbl");
    if (-1 != nextidx)
      idx = nextidx;

    MYON_wString head = rtf.Left(idx);
    MYON_wString tail = rtf.Right(count - idx);
    if (rtf[idx + 1] != L'{')
    {
      rtf_in.Format(L"%s{%s{%s}}", head.Array(), fmt_str, tail.Array());
    }
    else
    {
      rtf_in.Format(L"%s{%s%s}", head.Array(), fmt_str, tail.Array());
    }
  }
  return true;
}

bool MYON_Annotation::SetAnnotationTextFormat(MYON_wString& rtf_in, const wchar_t* fmt_str_on, const wchar_t* fmt_str_off, bool set_on)
{
  bool rc = MYON_Annotation::ClearRtfFmt(fmt_str_on, fmt_str_off, rtf_in);
  if (set_on)
    rc = MYON_Annotation::SetRtfFmt(rtf_in, fmt_str_on);
  else
    rc = MYON_Annotation::SetRtfFmt(rtf_in, fmt_str_off);
  return rc;
}

bool MYON_Annotation::SetAnnotationBold(bool bold, const MYON_DimStyle* parent_style)
{
  MYON_Dimension* dim = MYON_Dimension::Cast(this);
  const wchar_t* textstring;
  if (nullptr == dim)
    textstring = RichText().Array();
  else
    textstring = dim->UserText();
  MYON_wString rtfstr(textstring);

  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  MYON_wString newrtf;
  if (!bold && parent_style->Font().IsBoldInQuartet())
  {
    newrtf = MYON_TextContext::FormatRtfString(rtfstr, parent_style, true, true, false, false, false, false, false, false, L"");
    newrtf.Replace(L"\\b", L"\\b0");
  }
  else
    newrtf = MYON_TextContext::FormatRtfString(rtfstr, parent_style, !bold, bold, false, false, false, false, false, false, L"");
  
  if (newrtf.IsNotEmpty())
  {
    if (nullptr != dim)
    {
      dim->SetUserText(newrtf);
    }
    else
    {
      MYON_TextContent* text = this->Text();
      MYON::AnnotationType type = this->Type();
      text->ReplaceTextString(newrtf, type, parent_style);
      SetText(text);
    }
    return true;
  }
  return false;
}

bool MYON_Annotation::SetAnnotationItalic(bool italic, const MYON_DimStyle* parent_style)
{
  MYON_Dimension* dim = MYON_Dimension::Cast(this);
  const wchar_t* textstring;
  if (nullptr == dim)
    textstring = RichText().Array();
  else
    textstring = dim->UserText();
  MYON_wString rtfstr(textstring);

  const MYON_wString newrtf = MYON_TextContext::FormatRtfString(rtfstr, parent_style, false, false, !italic, italic, false, false, false, false, L"");
  if (newrtf.IsNotEmpty())
  {
    if (nullptr != dim)
    {
      dim->SetUserText(newrtf);
    }
    else
    {
      MYON_TextContent* text = this->Text();
      MYON::AnnotationType type = this->Type();
      parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
      text->ReplaceTextString(newrtf, type, parent_style);
      SetText(text);
    }
    return true;
  }
  return false;
}

bool MYON_Annotation::SetAnnotationUnderline(bool underline, const MYON_DimStyle* parent_style)
{
  MYON_Dimension* dim = MYON_Dimension::Cast(this);
  const wchar_t* textstring;
  if (nullptr == dim)
    textstring = RichText().Array();
  else
    textstring = dim->UserText();
  MYON_wString rtfstr(textstring);

  const MYON_wString newrtf = MYON_TextContext::FormatRtfString(rtfstr, parent_style, false, false, false, false, !underline, underline, false, false, L"");
  if (newrtf.IsNotEmpty())
  {
    if (nullptr != dim)
    {
      dim->SetUserText(newrtf);
    }
    else
    {
      MYON_TextContent* text = this->Text();
      MYON::AnnotationType type = this->Type();
      parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
      text->ReplaceTextString(newrtf, type, parent_style);
      SetText(text);
    }
    return true;
  }
  return false;
}


bool MYON_Annotation::SetAnnotationFont(const MYON_Font* font, const MYON_DimStyle* parent_style)
{
  if (nullptr != font)
  {
    SetFont(parent_style, *font);

    const MYON_wString fontname = font->QuartetName();  //MYON_Font::RichTextFontName(font, true);
    bool bold = font->IsBoldInQuartet();
    bool italic = font->IsItalic();

    MYON_Dimension* dim = MYON_Dimension::Cast(this);
    const wchar_t* textstring;
    if (nullptr == dim)
      textstring = RichText().Array();
    else
      textstring = dim->UserText();
    MYON_wString rtfstr(textstring);

    const MYON_wString newrtf = MYON_TextContext::FormatRtfString(rtfstr, parent_style, false, bold, false, italic, false, false, false, true, fontname);
    if (newrtf.IsNotEmpty())
    {
      if (nullptr != dim)
      {
        dim->SetUserText(newrtf);
      }
      else
      {
        MYON_TextContent* text = this->Text();
        MYON::AnnotationType type = this->Type();
        parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
        text->ReplaceTextString(newrtf, type, parent_style);
        SetText(text);
      }
      return true;
    }
  }
  return false;
}

bool MYON_Annotation::SetAnnotationFacename(bool set_or_clear, const wchar_t* facename, const MYON_DimStyle* parent_style)
{
  MYON_Dimension* dim = MYON_Dimension::Cast(this);
  const wchar_t* textstring;
  if (nullptr == dim)
    textstring = RichText().Array();
  else
    textstring = dim->UserText();
  MYON_wString rtfstr(textstring);

  const MYON_wString newrtf = MYON_TextContext::FormatRtfString(rtfstr, parent_style, false, false, false, false, false, false, !set_or_clear, set_or_clear, facename);
  if(newrtf.IsNotEmpty())
  {
    if (nullptr != dim)
    {
      dim->SetUserText(newrtf);
    }
    else
    {
      MYON_TextContent* text = this->Text();
      MYON::AnnotationType type = this->Type();
      parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
      text->ReplaceTextString(newrtf, type, parent_style);
      SetText(text);
    }
    return true;
  }
  return false;
}

bool MYON_Annotation::FirstCharTextProperties(const wchar_t* rtfstr, bool& bold, bool& italic, bool& underline, MYON_wString& facename)
{
  bool rc = MYON_TextContext::RtfFirstCharProperties(rtfstr, bold, italic, underline, facename);
  return rc;
}

const MYON_Font* MYON_Annotation::FirstCharFont() const
{
  if(nullptr != Text())
    return Text()->FirstCharFont();
  return &MYON_Font::Default;
}

bool MYON_Annotation::IsAllBold() const
{
  return IsAllFormat(&MYON_Font::IsBold);
}

bool MYON_Annotation::IsAllItalic() const
{
  return IsAllFormat(&MYON_Font::IsItalic);
}

bool MYON_Annotation::IsAllUnderlined() const
{
  return IsAllFormat(&MYON_Font::IsUnderlined);
}

bool MYON_Annotation::IsAllFormat(bool (MYON_Font::*func)() const) const
{
  if (nullptr == func)
    return false;
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;

  const MYON_TextRunArray* runs = text->TextRuns(true);
  if (nullptr == runs)
    return false;

  for (int i = 0; i < runs->Count(); i++)
  {
    const MYON_TextRun* run = (*runs)[i];
    if (nullptr == run)
      continue;
    MYON_TextRun::RunType type = run->Type();
    if (MYON_TextRun::RunType::kText == type ||
      MYON_TextRun::RunType::kField == type ||
      MYON_TextRun::RunType::kFieldValue == type)
    {
      if (!(run->Font()->*func)())
        return false;
    }
  }
  return true;
}








































































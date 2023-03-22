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

#include "opennurbs_internal_defines.h"

MYON_VIRTUAL_OBJECT_IMPLEMENT(MYON_Dimension, MYON_Annotation, "EE6571FE-1596-4D5B-BD6D-7072B0643986");
MYON_OBJECT_IMPLEMENT(MYON_DimLinear, MYON_Dimension, "E550882B-F44D-4154-A1EF-6E50CBBBF543");
MYON_OBJECT_IMPLEMENT(MYON_DimAngular, MYON_Dimension, "D417786B-F6CD-4F12-9E1F-063F414DBEB6");
MYON_OBJECT_IMPLEMENT(MYON_DimRadial, MYON_Dimension, "FC749C2F-4C00-41FD-9840-26D94F047AD3");
MYON_OBJECT_IMPLEMENT(MYON_DimOrdinate, MYON_Dimension, "03124828-4C9B-4D28-9A82-664DDDE7A14F");
MYON_OBJECT_IMPLEMENT(MYON_Centermark, MYON_Dimension, "D46767BA-7E8F-4D9D-9A92-66050219A5B9");


MYON_Dimension::MYON_Dimension(MYON::AnnotationType annotation_type)
  : MYON_Annotation(annotation_type)
{}

MYON_Dimension::~MYON_Dimension()
{
  Internal_Destroy();
}

MYON_Dimension::MYON_Dimension( const MYON_Dimension& src )
  : MYON_Annotation(src)
{
  Internal_CopyFrom(src);
}

MYON_Dimension& MYON_Dimension::operator=(
  const MYON_Dimension& src
  )
{
  if (this != &src)
  {
    Internal_Destroy();
    MYON_Annotation::operator=(src);
    Internal_CopyFrom(src);
  }
  return *this;
}

void MYON_Dimension::Internal_Destroy()
{
  m_user_text.Destroy();
}

void MYON_Dimension::Internal_CopyFrom(const MYON_Dimension& src)
{
  // m_text_rotation NOT used in 6.0 - m_text_rotation = src.m_text_rotation;
  m_use_default_text_point = src.m_use_default_text_point;
  m_user_text_point = src.m_user_text_point;
  m_user_text = src.m_user_text;
  m_distance_scale = src.m_distance_scale;
  m_detail_measured = src.m_detail_measured;
  m_flip_arrow_1 = src.m_flip_arrow_1;
  m_flip_arrow_2 = src.m_flip_arrow_2;
}

bool MYON_Dimension::IsValid(MYON_TextLog* text_log) const
{
  return true;
}

MYON_2dPoint MYON_Dimension::TextPoint() const
{
  if (m_use_default_text_point)
    return DefaultTextPoint();
  else
    return m_user_text_point;
}

void MYON_Dimension::Set2dTextPoint(const MYON_2dPoint& textpoint)
{
  SetUseDefaultTextPoint(false);
  m_user_text_point = textpoint;
}

MYON_2dPoint MYON_Dimension::DefaultTextPoint() const
{
  return MYON_2dPoint::Origin;
}

bool MYON_Dimension::UseDefaultTextPoint() const
{
  return m_use_default_text_point;
}

void MYON_Dimension::SetUseDefaultTextPoint(bool usedefault)
{
  m_use_default_text_point = usedefault;
}

const wchar_t* MYON_Dimension::UserText() const
{
  return static_cast< const wchar_t* >(m_user_text);
}

void MYON_Dimension::SetUserText(const wchar_t* text)
{
  if (nullptr == text)
    return;

  if (m_user_text.CompareOrdinal(text, false))
  {
    if (0 == *text)
      m_user_text = L"<>";
    else
      m_user_text = text;
    m_plain_user_text.Empty();
  }
}

const wchar_t* MYON_Dimension::PlainUserText() const
{
  if (m_plain_user_text.IsEmpty())
  {
    MYON_TextContent tc;
    tc.Create(UserText(), Type(), &MYON_DimStyle::Default);
    m_plain_user_text = tc.PlainText();
  }
  return static_cast< const wchar_t* >(m_plain_user_text);
}


// Add to natural rotation
double MYON_Dimension::TextRotation() const
{
  // This V5 function should have been removed from the 6.0 SDK.
  // It returned some angle in radians and it doesn't do anything in V6. It was almost always zero.
  // Text rotation is handled completely differently in V5 an V6.
  return 0.0;
}

void MYON_Dimension::SetTextRotation(double ignored_rotation_radians)
{
  // This V5 function and m_text_rotation should have been removed from the 6.0 SDK.
  // Text rotation is handled completely differently in V5 an V6.
  // m_text_rotation = remainder(rotation_radians, (2.0 * MYON_PI));
  return;
}

bool MYON_Dimension::GetTextRect(MYON_3dPoint text_rect[4]) const
{
  const MYON_TextContent* text = Text();
  if (nullptr != text)
  {
    MYON_BoundingBox text_box;
    if (text->GetTightBoundingBox(text_box))
    {
      MYON_3dPoint text_point( TextPoint());
      text_rect[0].Set(text_box.m_min.x, text_box.m_min.y, 0.0);
      text_rect[1].Set(text_box.m_max.x, text_box.m_min.y, 0.0);
      text_rect[2].Set(text_box.m_max.x, text_box.m_max.y, 0.0);
      text_rect[3].Set(text_box.m_min.x, text_box.m_max.y, 0.0);
      return true;
    }
  }
  return false;
}

MYON_TextContent* MYON_Dimension::RebuildDimensionText(
  MYON::LengthUnitSystem units_in,
  const MYON_DimStyle* dimstyle,
  bool expandanglebrackets
) const
{
  if (Type() == MYON::AnnotationType::CenterMark)
    return nullptr;

  MYON_wString displaytext;

  if (expandanglebrackets)
  {
    if (!GetDistanceDisplayText(units_in, dimstyle, displaytext))
      return nullptr;
  }
  else
  {
    displaytext = displaytext + UserText();
    if (dimstyle->Prefix().IsNotEmpty() || dimstyle->Suffix().IsNotEmpty())
    {
      int ci = displaytext.Find(L"<>");
      if (ci > -1)
      {
        MYON_wString right;
        if (displaytext.Length() > ci + 2)
          right = displaytext.Right(displaytext.Length() - ci - 2);
        displaytext = displaytext.Left(ci);
        displaytext = displaytext + dimstyle->Prefix();
        displaytext = displaytext + L"<>";
        displaytext = displaytext + dimstyle->Suffix();
        displaytext = displaytext + right;
      }
    }
  }

  MYON_TextContent* newtext = new MYON_TextContent;
  if (nullptr != newtext)
  {
    bool wrapped = m_text ? m_text->TextIsWrapped() : false;
    double rect_width = m_text ? m_text->FormattingRectangleWidth() : 0.0;
    double rotation = m_text ? m_text->TextRotationRadians() : 0.0;
    if (newtext->Create(displaytext, Type(), dimstyle,wrapped, rect_width, rotation))
    {
#ifdef _DEBUG
      newtext->IsValid();
#endif
    }
  }
  return newtext;
}

bool MYON_Dimension::UpdateDimensionText(
  MYON::LengthUnitSystem units_in,
  const MYON_DimStyle* dimstyle
) const
{
  if (Type() == MYON::AnnotationType::CenterMark)
    return false;

  MYON_TextContent* newtext = RebuildDimensionText(units_in, dimstyle,  true);
  if (nullptr != newtext)
  {
    SetText(newtext);
    return true;
  }
  return false;
}

bool MYON_Dimension::GetDistanceDisplayText(
  MYON::LengthUnitSystem units_in,
  const MYON_DimStyle* dimstyle,
  MYON_wString& displaytext) const
{
  if (Type() == MYON::AnnotationType::CenterMark)
    return false;

  if (nullptr == dimstyle)
    return false;

  double measurement = Measurement();
  const wchar_t* user_text = UserText();
  MYON_TextContent::FormatDistanceMeasurement(measurement, units_in, dimstyle, user_text, displaytext);
  return true;
}



bool MYON_Dimension::ArrowIsFlipped(int i) const
{
  if (i == 0)
    return m_flip_arrow_1;
  else
    return m_flip_arrow_2;
}

void MYON_Dimension::FlipArrow(int i, bool flip) const
{
  if (i == 0)
    m_flip_arrow_1 = flip;
  else
    m_flip_arrow_2 = flip;
}

double MYON_Dimension::DistanceScale() const
{
  return m_distance_scale;
}

void MYON_Dimension::SetDistanceScale(double distance_scale) const
{
  m_distance_scale = distance_scale;
}

MYON_UUID MYON_Dimension::DetailMeasured() const
{
  return m_detail_measured;
}

void MYON_Dimension::SetDetailMeasured(MYON_UUID uuid)
{
  m_detail_measured = uuid;
}

MYON_Dimension::ForceArrow MYON_Dimension::ForceArrowPosition() const
{
  MYON_ERROR("Use MYON_Dimension::ArrowFit(const MYON_DimStyle* parent_style)");
  return MYON_Dimension::ForceArrow::Auto;
}

void MYON_Dimension::SetForceArrowPosition(MYON_Dimension::ForceArrow force)
{
  // 
  MYON_ERROR("Use MYON_Dimension::SetArrowFit(const MYON_DimStyle* parent_style,MYON_DimStyle::arrow_fit arrowfit)");
}

MYON_Dimension::ForceText MYON_Dimension::ForceTextPosition() const
{
  MYON_ERROR("Use MYON_Dimension::TextFit(const MYON_DimStyle* parent_style)");
  return MYON_Dimension::ForceText::Auto;
}

void MYON_Dimension::SetForceTextPosition(MYON_Dimension::ForceText force)
{
  MYON_ERROR("Use MYON_Dimension::SetTextFit(const MYON_DimStyle* parent_style,MYON_DimStyle::text_fit textfit)");
}
//--------------------------------

void MYON_Dimension::SetForceDimLine(
  const MYON_DimStyle* parent_style,
  bool force_dimline
)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (force_dimline != parent_style->ForceDimLine());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetForceDimLine(force_dimline);
    override_style->SetFieldOverride(MYON_DimStyle::field::ForceDimLine, bCreate);
  }
}

bool MYON_Dimension::ForceDimLine(
  const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::ForceDimLine).ForceDimLine();

}

void MYON_Dimension::SetTextFit(
  const MYON_DimStyle* parent_style,
  MYON_DimStyle::text_fit textfit)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (textfit != parent_style->TextFit());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetTextFit(textfit);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextFit, bCreate);
  }
}

MYON_DimStyle::text_fit MYON_Dimension::TextFit(
  const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::TextFit).TextFit();
}

void MYON_Dimension::SetArrowFit(
  const MYON_DimStyle* parent_style,
  MYON_DimStyle::arrow_fit arrowfit)
{
  parent_style = &MYON_DimStyle::DimStyleOrDefault(parent_style);
  bool bCreate = (arrowfit != parent_style->ArrowFit());
  MYON_DimStyle* override_style = Internal_GetOverrideStyle(bCreate);
  if (nullptr != override_style)
  {
    override_style->SetArrowFit(arrowfit);
    override_style->SetFieldOverride(MYON_DimStyle::field::TextFit, bCreate);
  }
}

MYON_DimStyle::arrow_fit MYON_Dimension::ArrowFit(
  const MYON_DimStyle* parent_style) const
{
  return Internal_StyleForFieldQuery(parent_style, MYON_DimStyle::field::ArrowFit).ArrowFit();
}


//----------------------------------------------------------
// Class MYON_DimLinear

MYON_DimLinear::MYON_DimLinear()
  : MYON_Dimension(MYON::AnnotationType::Rotated)
{}



MYON_Dimension::ForceArrow MYON_Dimension::ForceArrowFromUnsigned(
  unsigned int force_arrow_as_unsigned)
{
  switch (force_arrow_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceArrow::Auto);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceArrow::Inside);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceArrow::Outside);
  }
  MYON_ERROR("Invalid type_as_unsigned parameter.");
  return (MYON_Dimension::ForceArrow::Auto);
}

MYON_Dimension::ForceText MYON_Dimension::ForceTextFromUnsigned(
    unsigned int force_text_as_unsigned)
{
  switch (force_text_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceText::Auto);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceText::Inside);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceText::Right);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceText::Left);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceText::HintRight);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Dimension::ForceText::HintLeft);
  }
  MYON_ERROR("Invalid type_as_unsigned parameter.");
  return (MYON_Dimension::ForceText::Auto);
}

bool MYON_Dimension::Internal_WriteDimension(
  MYON_BinaryArchive& archive
) const
{
  // content_version = 1 added m_force_textpos
  const int content_version = 1;
  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!MYON_Annotation::Internal_WriteAnnotation(archive))
      break;

    const MYON_DimStyle& ds = archive.ArchiveCurrentDimStyle();


    if (!archive.WriteString(m_user_text))
      break;
    if (!archive.WriteDouble(0.0)) // OBSOLETE m_text_rotation
      break;
    if (!archive.WriteBool(m_use_default_text_point))
      break;
    if (!archive.WritePoint(m_user_text_point))
      break;
    if (!archive.WriteBool(m_flip_arrow_1))
      break;
    if (!archive.WriteBool(m_flip_arrow_2))
      break;
    const unsigned int legacy_arrow_fit = static_cast<unsigned int>(ArrowFit(&ds));
    if (!archive.WriteInt(legacy_arrow_fit))
      break;
    if (!archive.WriteUuid(m_detail_measured))
      break;
    if (!archive.WriteDouble(m_distance_scale))
      break;

    // content_version 1
    const unsigned int legacy_text_fit = static_cast<unsigned int>(TextFit(&ds));
    if (!archive.WriteInt(legacy_text_fit))
      break;

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_Dimension::Internal_ReadDimension(
  MYON_BinaryArchive& archive
)
{
  // This is a helper function called by liner and angular annotation classes.
  // "this" has already been set to default values before this function is called.
  int content_version = -1;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  unsigned int legacy_arrow_fit = 0;
  unsigned int legacy_text_fit = 0;

  bool rc = false;
  for (;;)
  {
    if (content_version < 0)
      break;
    if (!MYON_Annotation::Internal_ReadAnnotation(archive))
      break;

    if (!archive.ReadString(m_user_text))
      break;
    double obsolete_text_rotation = 0.0;
    if (!archive.ReadDouble(&obsolete_text_rotation))
      break;
    if (!archive.ReadBool(&m_use_default_text_point))
      break;
    if (!archive.ReadPoint(m_user_text_point))
      break;
    if (!archive.ReadBool(&m_flip_arrow_1))
      break;
    if (!archive.ReadBool(&m_flip_arrow_2))
      break;
    if (!archive.ReadInt(&legacy_arrow_fit))
      break;
    if (!archive.ReadUuid(m_detail_measured))
      break;
    if (!archive.ReadDouble(&m_distance_scale))
      break;
    // 24-Sep-2021 Dale Fugier, https://mcneel.myjetbrains.com/youtrack/issue/RH-65605
    //if (MYON_nil_uuid == m_detail_measured)
    //  m_distance_scale = 1.0;

    if (content_version <= 0)
    {
      rc = true;
      break;
    }

    // content_version 1
    if (!archive.ReadInt(&legacy_text_fit))
      break;

    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  const unsigned int version_v7_may_8_2019 = MYON_VersionNumberConstruct(7, 0, 2019, 5, 8, 0);
  if (rc && archive.ArchiveOpenNURBSVersion() < version_v7_may_8_2019 )
  {
    // may 2019 - "arrow fit" and "text fit" moved from member settings on MYON_Dimension
    // to settings on MYON_DimStyle.
    // The file being read is older than the change.
    const MYON_DimStyle::arrow_fit new_arrow_fit = MYON_DimStyle::ArrowFitFromUnsigned((unsigned int)legacy_arrow_fit);
    const MYON_DimStyle::text_fit new_text_fit = MYON_DimStyle::TextFitFromUnsigned((unsigned int)legacy_text_fit);
    const MYON_DimStyle& ds = archive.ArchiveCurrentDimStyle();
    bool bSetArrowFit = (new_arrow_fit != ArrowFit(&ds));
    if (bSetArrowFit)
      SetArrowFit(&ds, new_arrow_fit);
    bool bSetTextFit = (new_text_fit != TextFit(&ds));
    if (bSetTextFit)
      SetTextFit(&ds, new_text_fit);
  }

  return rc;
}


bool MYON_DimLinear::Write(
  MYON_BinaryArchive& archive
  ) const 
{
  const int content_version = 0;
  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!MYON_Dimension::Internal_WriteDimension(archive))
      break;
    if (!archive.WritePoint(m_def_pt_2))
      break;
    if (!archive.WritePoint(m_dimline_pt))
      break;
    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimLinear::Read(
  MYON_BinaryArchive& archive
  ) 
{
  *this = MYON_DimLinear::Empty;

  int content_version = -1;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (content_version < 0)
      break;
    if (!MYON_Dimension::Internal_ReadDimension(archive))
      break;
    if (!archive.ReadPoint(m_def_pt_2))
      break;
    if (!archive.ReadPoint(m_dimline_pt))
      break;
    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimLinear::Transform(const MYON_Xform& xform)
{
  bool rc = xform.IsIdentity();
  if (!rc)
  {
    rc = true;
    bool scaling = false;
    MYON_3dVector v = m_plane.xaxis;
    v.Transform(xform);
    if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
      scaling = true;
    else
    {
      v = m_plane.yaxis;
      v.Transform(xform);
      if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
        scaling = true;
      else
      {
        v = m_plane.zaxis;
        v.Transform(xform);
        if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
          scaling = true;
      }
    }

    if (rc)
    {
      if (scaling)
      {
        MYON_3dPoint defpt2_0(MYON_3dPoint::UnsetPoint);
        MYON_3dPoint dimlinept_0(MYON_3dPoint::UnsetPoint);
        MYON_3dPoint textpt_0(MYON_3dPoint::UnsetPoint);
        if (Get3dPoints(nullptr, &defpt2_0, nullptr, nullptr, &dimlinept_0, &textpt_0))
        {
          MYON_2dPoint defpt2(MYON_2dPoint::NanPoint), dimlinept(MYON_2dPoint::NanPoint), textpt(MYON_2dPoint::NanPoint);
          rc = m_plane.Transform(xform);
          defpt2_0.Transform(xform);
          dimlinept_0.Transform(xform);
          if (!UseDefaultTextPoint())
            textpt_0.Transform(xform);
          if (rc && !m_plane.ClosestPointTo(defpt2_0, &defpt2.x, &defpt2.y))
            rc = false;
          else if (rc && !m_plane.ClosestPointTo(dimlinept_0, &dimlinept.x, &dimlinept.y))
            rc = false;
          else if (rc && !UseDefaultTextPoint() && !m_plane.ClosestPointTo(textpt_0, &textpt.x, &textpt.y))
            rc = false;
          if (rc)
          {
            Set2dDefPoint2(defpt2);
            Set2dDimlinePoint(dimlinept);
            if (!UseDefaultTextPoint())
              Set2dTextPoint(textpt);
          }
        }
      }
      else
        rc = m_plane.Transform(xform);
    }
    if (rc)
      MYON_Geometry::Transform(xform);
  }
  return rc;
}

bool MYON_DimLinear::GetTextXform(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  return GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform_out);
}


bool MYON_DimLinear::GetTextXform(
  const MYON_Xform* model_xform,
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  MYON_3dVector view_x = nullptr == vp ? MYON_3dVector::XAxis : vp->CameraX();
  MYON_3dVector view_y = nullptr == vp ? MYON_3dVector::YAxis : vp->CameraY();
  MYON_3dVector view_z = nullptr == vp ? MYON_3dVector::ZAxis : vp->CameraZ();
  MYON::view_projection projection = vp ? vp->Projection() : MYON::view_projection::parallel_view;
  bool bDrawForward = dimstyle == nullptr ? false : dimstyle->DrawForward();
  return GetTextXform(model_xform, view_x, view_y, view_z, projection, bDrawForward, dimstyle, dimscale, text_xform_out);
}


bool MYON_DimLinear::GetTextXform(
  const MYON_Xform * model_xform,
  const MYON_3dVector view_x,
  const MYON_3dVector view_y,
  const MYON_3dVector view_z,
  MYON::view_projection projection,
  bool bDrawForward,
  const MYON_DimStyle * dimstyle,
  double dimscale,
  MYON_Xform & text_xform_out
) const
{
  bool rc = false;
  if (nullptr == dimstyle)
    return false;

  // This gets the display text that's already on the dimension
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;

  // See if the text needs remade because of some change in some property that
  // would change its appearance
  if (DimStyleTextPositionPropertiesHash() != dimstyle->TextPositionPropertiesHash())
  {
    MYON_wString rtfstr = text->RtfText();
    MYON::AnnotationType annotation_type = this->Type();
    bool wrapped = text->TextIsWrapped();
    double width = text->FormattingRectangleWidth();
    double rot = text->TextRotationRadians();
    const_cast<MYON_TextContent*>(text)->Create(rtfstr, annotation_type, dimstyle, wrapped, width, rot);
  }

  double text_width = 0.0;
  double text_height = 0.0;
  double text_gap = 0.0;
  double text_angle = 0.0; // in radians - deviation from horizontal ccw
  
  const MYON_DimStyle::TextLocation text_location = dimstyle->DimTextLocation();
  const MYON::TextOrientation text_orientation = dimstyle->DimTextOrientation();
  const MYON_DimStyle::ContentAngleStyle text_angle_style = dimstyle->DimTextAngleStyle();

  MYON_Xform text_to_dimplane(1.0);         // Text plane (world xy) to dimension plane rotation
  MYON_Xform dimplane_to_textpoint(1.0);        // Dimension plane to text point translation
  MYON_Xform text_rotation(1.0);          // Text rotation around text plane origin point

  // The amount past vertical where text flips to the other orientation
  const double fliptol = (projection == MYON::view_projection::perspective_view) ? cos(89.0 * MYON_DEGREES_TO_RADIANS) : cos(80.001 * MYON_DEGREES_TO_RADIANS);

  MYON_3dPoint text_center = MYON_3dPoint::Origin;
  // Text starts out approximately centered at origin
  MYON_3dPoint cp[4];
  if (!text->Get3dCorners(cp))
    return false;

  text_center = (cp[0] + cp[2]) / 2.0;
  text_width = (cp[1].x - cp[0].x) * dimscale;
  text_height = (cp[3].y - cp[0].y) * dimscale;

  text_gap = dimstyle->TextGap();
  if (dimstyle->MaskFrameType() != MYON_TextMask::MaskFrame::NoFrame)
    text_gap = dimstyle->TextMask().MaskBorder();
  text_gap *= dimscale;


  if (dimstyle->Alternate() && dimstyle->AlternateBelow())
    text_height = -2.0 * text_gap;

  text_xform_out = MYON_Xform::IdentityTransformation;
  text_to_dimplane.Rotation(MYON_Plane::World_xy, Plane());     // Rotate text from starting text plane to dimension plane
  bool draw_forward = dimstyle->DrawForward();

#pragma region ArrowAndTextFitting
  // See if arrows and text will all fit inside extension lines
  // or what has to be moved outside
  bool arrowflipped[2] = { false, false };
  
  MYON_DimStyle::arrow_fit arrow_fit = dimstyle->ArrowFit();
  if (MYON_DimStyle::arrow_fit::ArrowsOutside == arrow_fit)
    arrowflipped[0] = arrowflipped[1] = true;

  MYON_DimStyle::text_fit text_fit = dimstyle->TextFit();

  bool text_outside = false;
  double dist = Measurement();
  // Display scale for detail viewports when page space dimension measures model space
  if (MYON_nil_uuid != DetailMeasured())
  {
    double dist_scale = DistanceScale();
    if (dist_scale != 1.0 && dist_scale > 0.0)
      dist /= dist_scale;
  }

  // V6_Dimstyle Arrow1 & Arrow2
  double asz = dimstyle->ArrowSize() * dimscale;

  double total_text_width = (MYON_DimStyle::ContentAngleStyle::Horizontal == text_angle_style) ? text_height : text_width;

  if (text_fit == MYON_DimStyle::text_fit::TextLeft || text_fit == MYON_DimStyle::text_fit::TextRight)
  {
    total_text_width = 0.0;
    text_outside = true;
  }
  //else if (force_text == MYON_Dimension::ForceText::Inside)
  else if (text_fit == MYON_DimStyle::text_fit::TextInside)
  {
    total_text_width = 0.0;
    text_outside = false;
  }
  else if (0.0 < total_text_width)
    total_text_width += text_gap;

  static double arrow_width_factor = 1.1;
  double total_arrow_width = asz * arrow_width_factor * 2;
  if (MYON_DimStyle::arrow_fit::ArrowsOutside == arrow_fit)
    total_arrow_width = 0.0;

  if (total_arrow_width + total_text_width > dist)     // arrows + text dont fit
  {
    // Try to leave text inside and move arrows outside
    if (total_text_width > dist)   // text doesnt fit
    {
      // move text outside
      text_outside = true;
      if (total_arrow_width > dist && MYON_DimStyle::arrow_fit::Auto == arrow_fit)  // arrows dont fit either
      {
        arrowflipped[0] = true;
        arrowflipped[1] = true;
      }
    }
    //else if (ForceArrow::Auto == force_arrow)       // text fits
    else if (MYON_DimStyle::arrow_fit::Auto == arrow_fit)       // text fits
    {
      // flip arrows
      arrowflipped[0] = true;
      arrowflipped[1] = true;
    }
  }

  FlipArrow(0, arrowflipped[0]);
  FlipArrow(1, arrowflipped[1]);

  // This returns the midpoint of the dimension line in 2d coordinates
  MYON_2dPoint text_pt = TextPoint();
  if (text_outside && MYON_DimStyle::ContentAngleStyle::Horizontal != text_angle_style && UseDefaultTextPoint())
  {
    // move textpoint outside right arrow by 1/2 text width + 1-1/2 arrow width
    double x = (text_width * 0.5) + (text_gap * 3.0);
    if (text_fit == MYON_DimStyle::text_fit::TextLeft || text_fit == MYON_DimStyle::text_fit::TextHintLeft)
    {
      if (arrowflipped[0])
        x += (asz * arrow_width_factor);
      text_pt = ArrowPoint1().x < ArrowPoint2().x ? ArrowPoint1() : ArrowPoint2();
      text_pt.x -= x;
    }
    else  // right or auto
    {
      if (arrowflipped[1])
        x += (asz * arrow_width_factor);
      text_pt = ArrowPoint1().x < ArrowPoint2().x ? ArrowPoint2() : ArrowPoint1();
      text_pt.x += x;
    }
  }

#pragma endregion ArrowAndTextFitting

  MYON_3dVector dim_xaxis = Plane().xaxis;
  MYON_3dVector dim_yaxis = Plane().yaxis;
  MYON_3dVector dim_zaxis = Plane().zaxis;
  if (nullptr != model_xform && !model_xform->IsIdentity())
  {
    dim_xaxis.Transform(*model_xform);
    dim_yaxis.Transform(*model_xform);
    dim_zaxis.Transform(*model_xform);
  }

  MYON_3dVector view_xdir = view_x;
  MYON_3dVector view_ydir = view_y;
  MYON_3dVector view_zdir = view_z;

  // text is in dimension plane, not horizontal to the view
  MYON_2dVector h_dir = HorizontalDirection();
  MYON_3dVector text_xdir = dim_xaxis;
  MYON_3dVector text_ydir = dim_yaxis;
  MYON_3dVector text_zdir = dim_zaxis;
  if (MYON::TextOrientation::InPlane == text_orientation)
  {
    if (MYON_DimStyle::ContentAngleStyle::Rotated == text_angle_style)
    {
      // Rotation angle = 0 means the text is horizontal
      text_angle = 0.0; //TextRotation();
    }
    else if (MYON_DimStyle::ContentAngleStyle::Aligned == text_angle_style)
    {
      text_angle = 0.0;
    }

    if (MYON_DimStyle::ContentAngleStyle::Aligned != text_angle_style)
    {
      double h_angle = atan2(h_dir.y, h_dir.x);
      text_angle += h_angle;
      text_xdir.Rotate(h_angle, dim_zaxis);
      text_ydir.Rotate(h_angle, dim_zaxis);
    }
  }

  bool flip_x = false;                  
  bool flip_y = false;

  CalcTextFlip(
    text_xdir, text_ydir, text_zdir,
    view_xdir, view_ydir, view_zdir,
    model_xform,
    fliptol,
    flip_x,
    flip_y);

  if (MYON_DimStyle::TextLocation::AboveDimLine == text_location)
  {
    // Moves the text to AboveLine if that's the alignment mode
    double dy = flip_y ? -1.0 : 1.0;
    double d = (text_height * 0.5 + text_gap) * dy;
    text_pt.y += d;
  }


  MYON_3dPoint text_point_3d = Plane().PointAt(text_pt.x, text_pt.y);  // 3d text point
  dimplane_to_textpoint = MYON_Xform::TranslationTransformation(text_point_3d - Plane().origin);  // Move from dimplane origin to text point

  text_xform_out = MYON_Xform::DiagonalTransformation(dimscale, dimscale, dimscale);      // dimscale

  if (1.0e-2 < fabs(text_angle))  // There's a rotation angle change of more than 1/100 radian (~1/2 degree)
  {
    text_rotation.Rotation(text_angle, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);

    if (MYON::TextOrientation::InView != text_orientation)
      text_xform_out = text_rotation * text_xform_out;                  // text rotation
  }
  text_xform_out = text_to_dimplane * text_xform_out;                 // text plane to dim plane
  text_xform_out = dimplane_to_textpoint * text_xform_out;                // dimension plane to text point

  
  if (MYON::TextOrientation::InView == text_orientation)  // Draw dimension horizontal to view
  {
    if (nullptr != model_xform)
    {
      MYON_Xform xf(*model_xform);
      xf.Invert();
      view_xdir.Transform(xf);
      view_ydir.Transform(xf);
      view_zdir.Transform(xf);
    }

    MYON_Xform tp2sxf;        // Text point to view plane rotation
    tp2sxf.Rotation(text_point_3d, Plane().xaxis, Plane().yaxis, Plane().zaxis, text_point_3d, view_xdir, view_ydir, view_zdir);
    text_xform_out = tp2sxf * text_xform_out;
  }
  else if (draw_forward)
  {
    MYON_Xform mxf;  // Mirror xform for backwards text to adjust DrawForward
    if (flip_x)
    {
      mxf.Mirror(text_center, MYON_3dVector::XAxis);
      text_xform_out = text_xform_out * mxf;
    }
    if (flip_y)
    {
      mxf.Mirror(text_center, MYON_3dVector::YAxis);
      text_xform_out = text_xform_out * mxf;
    }
  }
  return rc;
}

bool MYON_DimLinear::GetBBox(double* bmin, double* bmax, bool grow) const
{
  const MYON_DimStyle* dimstyle = nullptr;
  return GetAnnotationBoundingBox(nullptr, dimstyle, 1.0, bmin, bmax, grow?true:false);
}


class StylePointer
{
public:
  ~StylePointer()
  {
    if (m_delete_style && nullptr != m_style)
    {
      delete m_style;
      m_style = nullptr;
      m_delete_style = false;
    }
  }
  const MYON_DimStyle* m_style = nullptr;
  bool m_delete_style = false;
};

bool MYON_DimLinear::GetAnnotationBoundingBox(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  double* boxmin,
  double* boxmax,
  bool bGrow
) const
{
  if (nullptr == dimstyle)
    dimstyle = &MYON_DimStyle::Default;

  const MYON_2dPoint hash_points[] = { 
    m_def_pt_2,
    m_dimline_pt
  };

  const MYON_SHA1_Hash hash = Internal_GetBBox_InputHash(
    vp, 
    dimstyle, 
    dimscale, 
    m_user_text_point,
    (unsigned int)(sizeof(hash_points)/sizeof(hash_points[0])),
    hash_points
  );

  if (Internal_GetBBox_Begin(hash, boxmin, boxmax, bGrow))
    return true;

  if (nullptr == boxmin || nullptr == boxmax)
    return false;

  MYON_Xform text_xform;
  GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform);

  MYON_BoundingBox dim_box;

  const MYON_TextContent* text = Text();
  MYON_3dPoint text_rect[4] = { MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin };

  if (nullptr != text && text->GetTightBoundingBox(dim_box))
  {
    text_rect[0].Set(dim_box.m_min.x, dim_box.m_min.y, 0.0);
    text_rect[1].Set(dim_box.m_max.x, dim_box.m_min.y, 0.0);
    text_rect[2].Set(dim_box.m_max.x, dim_box.m_max.y, 0.0);
    text_rect[3].Set(dim_box.m_min.x, dim_box.m_max.y, 0.0);
    for (int i = 0; i < 4; i++)
      text_rect[i].Transform(text_xform);  // Text + gap bounding rect

    for (int i = 0; i < 4; i++)
    {
      dim_box.Set(text_rect[i], 0 < i ? true : false);
    }
  }

  // Get non-text display geometry for the dimension
#define dimlinecount 4
  bool dimlines[dimlinecount] = { false, false, false, false };
  MYON_Line lines[dimlinecount];
  if (GetDisplayLines(vp, dimstyle, dimscale, text_rect, lines, dimlines, dimlinecount))
  {
    for (int i = 0; i < dimlinecount; i++)
    {
      if (dimlines[i])
      {
        dim_box.Set(lines[i].from, true);
        dim_box.Set(lines[i].to, true);
      }
    }
  }
#undef dimlinecount
  
  double points[12];
  if(Get3dPoints(
      (MYON_3dPoint*)(&points[0]),   // defpt1
      (MYON_3dPoint*)(&points[3]),   // defpt2
      (MYON_3dPoint*)(&points[6]),   // arrowpt1
      (MYON_3dPoint*)(&points[9]),   // arrowpt2
      nullptr,  // dimlinept
      nullptr)) // textpt
    dim_box.Set(3, 0, 4, 3, points, true);


  // Include arrows
  bool arrowflipped[2] = { ArrowIsFlipped(0), ArrowIsFlipped(1) };
  double scale = dimstyle->ArrowSize() * dimscale;

  for (int ai = 0; ai < 2; ai++)
  {
    if (0 == ai && dimstyle->SuppressArrow1())
      continue;
    if (1 == ai && dimstyle->SuppressArrow2())
      continue;

    MYON_Xform arrow_xform(1.0);
    GetArrowXform(ai, scale, arrowflipped[ai], false, arrow_xform);

    MYON_Arrowhead::arrow_type arrowtype = (0 == ai) ? dimstyle->ArrowType1() : dimstyle->ArrowType2();
    MYON_UUID arrow_block_id = (0 == ai) ? dimstyle->ArrowBlockId1() : dimstyle->ArrowBlockId2();
    MYON_Arrowhead::GetArrowheadBoundingBox(arrowtype, arrow_block_id, arrow_xform, dim_box, true);
  }

  return Internal_GetBBox_End(dim_box, hash, boxmin, boxmax, bGrow);
}

bool MYON_DimLinear::IsValidLinearDimensionType(
  MYON::AnnotationType annotation_type
)
{
  return (
    MYON::AnnotationType::Aligned == annotation_type
    || MYON::AnnotationType::Rotated == annotation_type
    );
}

bool MYON_DimLinear::SetLinearDimensionType(
  MYON::AnnotationType linear_dimension_type
)
{
  if ( !MYON_DimLinear::IsValidLinearDimensionType(linear_dimension_type) )
  {
    MYON_ERROR("Invalid linear_dimension_type parameter.");
    return false;
  }

  m_annotation_type = linear_dimension_type;

  return true;
}



// Returns unadjusted distance in entity plane coordinates
double MYON_DimLinear::Measurement() const
{
  if (!m_def_pt_2.IsValid())
    return 0.0;
  if (MYON_2dPoint::UnsetPoint == m_def_pt_2)
    return 0.0;
  double d = fabs(m_def_pt_2.x);
  if (DistanceScale() != 1.0)
    d *= DistanceScale();
  return d;
}

// Returns midpoint of dimline
MYON_2dPoint MYON_DimLinear::DefaultTextPoint() const
{
  return MYON_2dPoint(m_def_pt_2.x / 2.0, m_dimline_pt.y);
}

MYON_2dPoint MYON_DimLinear::DefPoint1() const
{
  return MYON_2dPoint(0.0, 0.0);
}

MYON_2dPoint MYON_DimLinear::DefPoint2() const
{
  return m_def_pt_2;
}

MYON_2dPoint MYON_DimLinear::DimlinePoint() const
{
  return m_dimline_pt;
}

void MYON_DimLinear::Set2dDimlinePoint(MYON_2dPoint pt)
{
  m_dimline_pt = pt;
}

void MYON_DimLinear::Set3dDimlinePoint(MYON_3dPoint pt)
{
  MYON_2dPoint p;
  if (m_plane.ClosestPointTo(pt, &p.x, &p.y))
    Set2dDimlinePoint(p);
}

MYON_2dPoint MYON_DimLinear::ArrowPoint1() const
{
  return MYON_2dPoint(0.0, m_dimline_pt.y);
}

MYON_2dPoint MYON_DimLinear::ArrowPoint2() const
{
  return MYON_2dPoint(m_def_pt_2.x, m_dimline_pt.y);
}


bool MYON_DimLinear::Get3dPoints(
  MYON_3dPoint* defpt1, MYON_3dPoint* defpt2,
  MYON_3dPoint* arrowpt1, MYON_3dPoint* arrowpt2,
  MYON_3dPoint* dimline, MYON_3dPoint* textpt) const
{
  bool rc = true;
  if (nullptr != defpt1)
    *defpt1 = m_plane.origin;

  if (nullptr != defpt2)
  {
    if (MYON_3dPoint::UnsetPoint != m_def_pt_2)
      *defpt2 = m_plane.PointAt(m_def_pt_2.x, m_def_pt_2.y);
    else
    {
      *defpt2 = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != dimline)
  {
    if (MYON_3dPoint::UnsetPoint != m_dimline_pt)
      *dimline = m_plane.PointAt(m_dimline_pt.x, m_dimline_pt.y);
    else
    {
      *dimline = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }
  
  if (nullptr != arrowpt1)
  {
    if (MYON_3dPoint::UnsetPoint != m_dimline_pt)
      *arrowpt1 = m_plane.PointAt(0.0, m_dimline_pt.y);
    else
    {
      *arrowpt1 = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != arrowpt2)
  {
    if (MYON_3dPoint::UnsetPoint != m_def_pt_2 && MYON_3dPoint::UnsetPoint != m_dimline_pt)
      *arrowpt2 = m_plane.PointAt(m_def_pt_2.x, m_dimline_pt.y);
    else
    {
      *arrowpt2 = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != textpt)
  {
    MYON_2dPoint textpt2d = MYON_2dPoint::UnsetPoint;
    if (m_use_default_text_point)
      textpt2d = DefaultTextPoint();
    else if (MYON_3dPoint::UnsetPoint != m_user_text_point)
        textpt2d = m_user_text_point;

    if (MYON_3dPoint::UnsetPoint != textpt2d)
      *textpt = m_plane.PointAt(textpt2d.x, textpt2d.y);
    else
    {
      *textpt = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }
  return rc;
}

MYON_DimLinear* MYON_DimLinear::CreateAligned(
  MYON_3dPoint extension_point0,
    MYON_3dPoint extension_point1,
    MYON_3dPoint dimension_line_point,
    MYON_3dVector plane_normal,
    MYON_UUID style_id,
    MYON_DimLinear* destination
)
{
  if (nullptr != destination)
    *destination = MYON_DimLinear::Empty;

  const MYON_Line ext_line(extension_point0, extension_point1);
  if (false == ext_line.IsValid())
    return nullptr;
  MYON_Plane plane;
  plane.xaxis = ext_line.Tangent();
  if (false == plane.xaxis.IsUnitVector())
    return nullptr;
  plane.zaxis = plane_normal;
  if (false == plane.zaxis.IsUnitVector() && false == plane.zaxis.Unitize())
    return nullptr;
  plane.yaxis = MYON_CrossProduct(plane.zaxis, plane.xaxis);
  if (false == plane.yaxis.IsUnitVector() && false == plane.yaxis.Unitize())
    return nullptr;
  plane.origin = extension_point0;
  plane.UpdateEquation();
  if (false == plane.IsValid())
  {
    plane.zaxis = MYON_CrossProduct(plane.xaxis, plane.yaxis);
    plane.zaxis.Unitize();
    plane.UpdateEquation();
    if (false == plane.IsValid())
      return nullptr;
  }

  MYON_Line dim_line = ext_line;
  double t = 0.5;
  if (dimension_line_point.IsValid())
  {
    MYON_3dPoint p = plane.ClosestPointTo(dimension_line_point);
    MYON_Line l(p, p + ext_line.Direction());
    MYON_Line l2(l.ClosestPointTo(extension_point0), l.ClosestPointTo(extension_point1));
    if (l2.IsValid() && fabs(1.0 - l2.Tangent()*plane.xaxis) <= 1e-4)
      dim_line = l2;
    double s = MYON_UNSET_VALUE;
    dim_line.ClosestPointTo(dimension_line_point, &s);
    if (s >= 0.0 && s <= 1.0)
      t = s;
    else if (s < 0.0)
      t = 0.0;
    else if (s > 1.0)
      t = 1.0;
  }

  MYON_DimLinear* dim_linear
    = (nullptr != destination)
    ? destination
    : new MYON_DimLinear();

  MYON_3dVector horizontal = MYON_Annotation::GetDefaultHorizontal(plane);
  if (false == dim_linear->Create(MYON::AnnotationType::Aligned, style_id, plane, horizontal, extension_point0, extension_point1, dim_line.PointAt(t)) )
  {
    if (nullptr != destination)
      *destination = MYON_DimLinear::Empty;
    else
      delete dim_linear;
    dim_linear = nullptr;
  }

  return dim_linear;
}

MYON_DimLinear* MYON_DimLinear::CreateRotated(
    MYON_3dPoint extension_point0,
    MYON_3dPoint extension_point1,
    MYON_Line dimension_line,
    MYON_3dVector plane_normal,
    MYON_UUID style_id,
    MYON_DimLinear* destination
)
{
  if (nullptr != destination)
    *destination = MYON_DimLinear::Empty;

  const MYON_Line ext_line(extension_point0, extension_point1);
  if (false == ext_line.IsValid())
    return nullptr;
  MYON_Plane plane;
  plane.xaxis = ext_line.Tangent();
  if (false == plane.xaxis.IsUnitVector())
    return nullptr;
  plane.zaxis = plane_normal;
  if (false == plane.zaxis.IsUnitVector() && false == plane.zaxis.Unitize())
    return nullptr;
  plane.yaxis = MYON_CrossProduct(plane.zaxis, plane.xaxis);
  if (false == plane.yaxis.IsUnitVector() && false == plane.yaxis.Unitize())
    return nullptr;
  plane.origin = extension_point0;
  plane.UpdateEquation();
  if (false == plane.IsValid())
  {
    plane.zaxis = MYON_CrossProduct(plane.xaxis, plane.yaxis);
    plane.zaxis.Unitize();
    plane.UpdateEquation();
    if (false == plane.IsValid())
      return nullptr;
  }

  MYON_Line dim_line = ext_line;
  double rot_angle = 0.0;
  if (dimension_line.IsValid())
  {
    MYON_Line l(plane.ClosestPointTo(dimension_line.from),plane.ClosestPointTo(dimension_line.to));
    if (l.IsValid())
    {
      MYON_Line l2(l.ClosestPointTo(extension_point0), l.ClosestPointTo(extension_point1));
      if (l2.IsValid() && fabs(l2.Tangent()*plane.zaxis) <= 1e-4)
      {
        dim_line = l2;
        double x = l2.Tangent()*plane.xaxis;
        double y = l2.Tangent()*plane.yaxis;
        double a = -atan2(y, x);
        if (a < -MYON_PI)
          a += 2.0*MYON_PI;
        else if (a > MYON_PI)
          a -= 2.0*MYON_PI;
        if (-MYON_PI <= a && a <= MYON_PI)
          rot_angle = a;
      }
    }
  }

  MYON_DimLinear* dim_linear
    = (nullptr != destination)
    ? destination
    : new MYON_DimLinear();

  MYON_3dVector horizontal = MYON_Annotation::GetDefaultHorizontal(plane);

  if (false == dim_linear->Create(MYON::AnnotationType::Rotated, style_id, plane, horizontal, extension_point0, extension_point1, dim_line.PointAt(0.5), rot_angle) )
  {
    if (nullptr != destination)
      *destination = MYON_DimLinear::Empty;
    else
      delete dim_linear;
    dim_linear = nullptr;
  }

  return dim_linear;
}

// Projects all points to the input plane
bool MYON_DimLinear::Create(
  MYON::AnnotationType dim_type,
  const MYON_UUID style_id,
  const MYON_Plane& plane,
  const MYON_3dVector& ref_horizontal,
  const MYON_3dPoint& def_pt_1,
  const MYON_3dPoint& def_pt_2,
  const MYON_3dPoint& dimline_pt,
  double rotation_in_plane
  )
{
  m_dimstyle_id = style_id;
  if (MYON_nil_uuid == m_dimstyle_id)
    return true;

  if ( !MYON_DimLinear::IsValidLinearDimensionType(dim_type) )
  {
    MYON_ERROR("Invalid dim_type parameter.");
    return false;
  }

  if (!plane.IsValid() || !def_pt_1.IsValid() || !def_pt_2.IsValid() || !dimline_pt.IsValid() || !MYON_IsValid(rotation_in_plane))
    return false;

  bool rc = SetLinearDimensionType(dim_type);

  m_plane = plane;

  if (0.0 != rotation_in_plane)
    rc = m_plane.Rotate(-rotation_in_plane, m_plane.zaxis);

  if (rc)
  {
    m_plane.origin = plane.ClosestPointTo(def_pt_1);
    rc = m_plane.ClosestPointTo(def_pt_2, &m_def_pt_2.x, &m_def_pt_2.y);
  }
  if (rc)
    rc = m_plane.ClosestPointTo(dimline_pt, &m_dimline_pt.x, &m_dimline_pt.y);

  if (rc)
  {
    MYON_2dVector horiz;
    MYON_3dPoint hp = m_plane.origin + ref_horizontal;
    rc = m_plane.ClosestPointTo(hp, &horiz.x, &horiz.y);
    if (rc)
      SetHorizontalDirection(horiz);
  }

  return rc;
}

void MYON_DimLinear::Set2dDefPoint1(MYON_2dPoint pt)
{
  // Move plane origin to pt
  MYON_3dPoint p = m_plane.PointAt(pt.x, pt.y);
  Set3dDefPoint1(p);
}

void MYON_DimLinear::Set2dDefPoint2(MYON_2dPoint pt)
{
  if ( MYON::AnnotationType::Aligned == Type() )
  {
    // Rotate around DefPoint1
    MYON_2dVector xdir = pt;
    if (!xdir.Unitize())
      return;
    double r = atan2(pt.y, pt.x);
    m_plane.Rotate(r, m_plane.zaxis);
    pt.Rotate(-r, MYON_2dPoint(0.0, 0.0));
  }
  m_def_pt_2 = pt;
}

void MYON_DimLinear::Set3dDefPoint1(MYON_3dPoint pt)
{
  MYON_2dPoint p;
  if (m_plane.ClosestPointTo(pt, &p.x, &p.y))
  {
    if (MYON::AnnotationType::Aligned == Type())
    {
      MYON_2dVector xdir = m_def_pt_2 - p;
      if (!xdir.Unitize())
        return;
      m_plane.origin = pt;
      m_plane.Rotate(xdir.y, xdir.x, m_plane.zaxis);
    }
    else //DimensionType::Rotated
    {
      m_plane.origin = pt;
      m_dimline_pt.x -= p.x;
      m_dimline_pt.y -= p.y;
      m_def_pt_2.x -= p.x;
      m_def_pt_2.y -= p.y;
    }
  }
}

void MYON_DimLinear::Set3dDefPoint2(MYON_3dPoint pt)
{
  if (MYON::AnnotationType::Aligned == Type())
  {
    MYON_2dPoint p;
    if (m_plane.ClosestPointTo(pt, &p.x, &p.y))
    {
      // Rotate around DefPoint1
      MYON_2dVector xdir = p;
      if (!xdir.Unitize())
        return;
      m_plane.Rotate(xdir.y, xdir.x, m_plane.zaxis);
    }
  }
  else
    m_plane.ClosestPointTo(pt, &m_def_pt_2.x, &m_def_pt_2.y);
}

static int ClipLineToTextRect(
  const MYON_Viewport* vp,
  const MYON_Line dimline,
  const MYON_3dPoint text_rect[4],
  MYON_Line linesegs[2])
{
  MYON_PlaneEquation cam_plane_eq;
  MYON_3dPoint cam_loc = MYON_3dPoint::Origin;
  MYON_3dVector cam_dir = MYON_3dVector::ZAxis;

  if (nullptr == vp)
  {
    cam_loc = MYON_3dPoint::Origin;
    MYON_3dVector xdir = text_rect[1] - text_rect[0]; xdir.Unitize();
    MYON_3dVector ydir = text_rect[3] - text_rect[0]; ydir.Unitize();
    cam_dir = MYON_CrossProduct(xdir, ydir);
    cam_dir = cam_dir * 100.0;
    cam_loc = cam_loc + cam_dir;
    cam_dir = cam_dir + cam_dir;
  }
  else
  {
    cam_loc = vp->CameraLocation();
    cam_dir = -vp->CameraDirection();
  }
  cam_plane_eq.Create(cam_loc, cam_dir);

  MYON_PlaneEquation frust_plane_eq[4];
  MYON_3dVector v0, v1;
  v0 = text_rect[3] - cam_loc;
  if (!v0.Unitize())
    return 0;

  bool bTextRectBackwards = false;
  {
    MYON_3dVector a = text_rect[2] - text_rect[0];
    MYON_3dVector b = text_rect[3] - text_rect[1];
    MYON_3dVector c = MYON_CrossProduct(a, b);
    if ((c * cam_dir) < 0.0)
      bTextRectBackwards = true;
  }

  for (int i = 0; i < 4; i++)
  {
    v1 = text_rect[i] - cam_loc;
    if (!v1.Unitize())
      return 0;
    // Makes normals facing out of frustum
    MYON_3dVector z = MYON_CrossProduct(v0, v1);
    if (bTextRectBackwards)
      z = -z;
    if (!frust_plane_eq[i].Create(cam_loc, z))
      return 0;
    v0 = v1;
  }

  double s[4];
  int intcount = 0;
  for (int i = 0; i < 4; i++)
  {
    double t;
    if (MYON_Intersect(dimline, frust_plane_eq[i], &t) && t >= 0.0 && t <= 1.0)
    {
      MYON_3dPoint p = dimline.PointAt(t);
      double d = cam_plane_eq.ValueAt(p);
      if (0.0 < d)
        continue;  // intersection behind camera

      bool inside = true;
      // Test if intersection is behind the other 3 planes
      // If not, it's outside the frustum
      for (int j = 1; j < 4; j++)
      {
        // eval other planes at p
        d = frust_plane_eq[(i + j) % 4].ValueAt(p);
        if (0.0 < d)  // intersection is outside frustum
        {
          inside = false;
          break;
        }
      }
      if (inside)
      {
        s[intcount] = t;
        intcount++;
      }
    }
  }

  bool end_1_inside = true;
  bool end_2_inside = true;

  if (0.0 < cam_plane_eq.ValueAt(dimline.from))
    end_1_inside = false;  // Dimline from point behind camera
  else for (int i = 0; i < 4; i++)
  {
    if (frust_plane_eq[i].ValueAt(dimline.from) > 0.0)
    {
      end_1_inside = false;
      break;
    }
  }

  if (0.0 < cam_plane_eq.ValueAt(dimline.to))
    end_2_inside = false;
  else for (int i = 0; i < 4; i++)
  {
    if (frust_plane_eq[i].ValueAt(dimline.to) > 0.0)
    {
      end_2_inside = false;
      break;
    }
  }

  if (end_1_inside && end_2_inside)
    return 0;

  if (0 == intcount)
  {
    // no intersection with frustum - return the whole line
    linesegs[0] = dimline;
    return 1;
  }

  double max_s = s[0];
  double min_s = s[0];
  for (int i = 1; i < intcount; i++)
  {
    if (s[i] > max_s)   max_s = s[i];
    if (s[i] < min_s)   min_s = s[i];
  }
  int segcount = 0;
  if (!end_1_inside && intcount > 0)
  {
    linesegs[segcount].from = dimline.from;
    linesegs[segcount].to = dimline.PointAt(min_s);
    segcount++;
  }
  if (!end_2_inside && intcount > 0)
  {
    linesegs[segcount].from = dimline.PointAt(max_s);
    linesegs[segcount].to = dimline.to;
    segcount++;
  }
  return segcount;
}

static int ClipArcToTextRect(
  const MYON_Viewport* vp,
  const MYON_Arc dimarc,
  const MYON_3dPoint text_rect[4],
  MYON_Arc arcsegs[2])
{
  if (nullptr == vp)
    return 0;

  MYON_3dPoint cam_loc = vp->CameraLocation();
  MYON_PlaneEquation cam_plane_eq;
  MYON_3dVector cam_dir = -vp->CameraDirection();
  cam_plane_eq.Create(cam_loc, cam_dir);

  MYON_Plane frust_plane[4];
  MYON_3dVector v0, v1;
  v0 = text_rect[3] - cam_loc;
  if (!v0.Unitize())
    return 0;

  for (int i = 0; i < 4; i++)
  {
    if (!frust_plane[i].CreateFromPoints(cam_loc, text_rect[i], text_rect[(i + 1) % 4]))
      return 0;
  }

  bool bTextRectBackwards = false;
  {
    MYON_3dVector a = text_rect[2] - text_rect[0];
    MYON_3dVector b = text_rect[3] - text_rect[1];
    MYON_3dVector c = MYON_CrossProduct(a, b);
    if ((c * cam_dir) < 0.0)
      bTextRectBackwards = true;
  }

  for (int i = 0; i < 4; i++)
  {
    v1 = text_rect[i] - cam_loc;
    if (!v1.Unitize())
      return 0;
    // Makes normals facing out of frustum
    MYON_3dVector z = MYON_CrossProduct(v0, v1);
    z.Unitize();
    if (bTextRectBackwards)
      z = -z;
    if(!frust_plane[i].CreateFromNormal(cam_loc, z))
      return 0;
    v0 = v1;
  }

  double s[8];
  int intcount = 0;
  for (int i = 0; i < 4; i++)
  {
    MYON_3dPoint p[2];
    int icount = MYON_Intersect(frust_plane[i], dimarc, p[0], p[1]);
    if (icount > 2)
      icount = 2;
    for (int ip = 0; ip < icount; ip++)
    {
      double d = cam_plane_eq.ValueAt(p[ip]);
      if (0.0 < d)
        continue;  // intersection behind camera

      bool inside = true;
      // Test if intersection is behind the other 3 planes
      // If not, it's outside the frustum
      for (int j = 1; j < 4; j++)
      {
        // eval other planes at p
        d = frust_plane[(i + j) % 4].plane_equation.ValueAt(p[ip]);
        if (0.0 < d)  // intersection is outside frustum
        {
          inside = false;
          break;
        }
      }
      if (inside)
      {
        dimarc.ClosestPointTo(p[ip], &s[intcount]);
        intcount++;
        if (intcount > 7)
          intcount = 7;
      }
    }
  }

  bool end_1_inside = true;
  bool end_2_inside = true;
  MYON_3dPoint end1 = dimarc.StartPoint();
  MYON_3dPoint end2 = dimarc.EndPoint();

  if (0.0 < cam_plane_eq.ValueAt(end1))
    end_1_inside = false;  // Dimline from point behind camera
  else for (int i = 0; i < 4; i++)
  {
    if (frust_plane[i].plane_equation.ValueAt(end1) > 0.0)
    {
      end_1_inside = false;
      break;
    }
  }

  if (0.0 < cam_plane_eq.ValueAt(end2))
    end_2_inside = false;
  else for (int i = 0; i < 4; i++)
  {
    if (frust_plane[i].plane_equation.ValueAt(end2) > 0.0)
    {
      end_2_inside = false;
      break;
    }
  }

  if (0 == intcount) // no intersection with frustum
  {
    if (end_1_inside && end_2_inside)
      return 0;
    else if (!end_1_inside && !end_2_inside)
    {
      arcsegs[0] = dimarc;
      return 1;
    }
  }

  double max_s = s[0];
  double min_s = s[0];
  for (int i = 1; i < intcount; i++)
  {
    if (s[i] > max_s)   max_s = s[i];
    if (s[i] < min_s)   min_s = s[i];
  }
  int segcount = 0;
  MYON_Interval domain = dimarc.Domain();
  if (!end_1_inside && intcount > 0)
  {
    arcsegs[segcount] = dimarc;
    arcsegs[segcount].Trim(MYON_Interval(domain[0], min_s));
    segcount++;
  }
  if (!end_2_inside && intcount > 0)
  {
    arcsegs[segcount] = dimarc;
    arcsegs[segcount].Trim(MYON_Interval(max_s, domain[1]));
    segcount++;
  }
  return segcount;
}


bool MYON_DimLinear::GetDisplayLines(
  const MYON_Viewport* vp,
  const MYON_DimStyle* style,
  double dimscale,
  MYON_3dPoint text_rect[4],
  MYON_Line lines[4],
  bool isline[4],
  int maxlines
) const
{
  if (maxlines != 4)
  {
    MYON_ERROR("Wrong linecount calling MYON_DimLinear::GetDisplayLines.\n");
    return false;
  }
  if (nullptr == style)
    return false;

  //MYON_BoundingBox text_box;
  //const MYON_TextContent* text = Text();
  //if (nullptr != text)
  //  text->GetTightBoundingBox(text_box);

  double eo = style->ExtOffset() * dimscale;
  double ee = style->ExtExtension() * dimscale;
  double el = style->FixedExtensionLen() * dimscale;
  if (!style->SuppressExtension1())
  {
    double o = eo;
    double e = ee;
    double l = el;
    if (m_dimline_pt.y < 0.0)
    {
      o = -o;
      e = -e;
      l = -l;
    }
    if (style->FixedExtensionLenOn())
      lines[0].from = m_plane.PointAt(0.0, m_dimline_pt.y - l);
    else
      lines[0].from = m_plane.PointAt(0.0, o);

    lines[0].to = m_plane.PointAt(0.0, m_dimline_pt.y + e);
    isline[0] = true;
  }
  else
    isline[0] = false;

  if (!style->SuppressExtension2())
  {
    double o = eo;
    double e = ee;
    double l = el;
    if (m_dimline_pt.y < m_def_pt_2.y)
    {
      o = -o;
      e = -e;
      l = -l;
    }
    if (style->FixedExtensionLenOn())
      lines[1].from = m_plane.PointAt(m_def_pt_2.x, m_dimline_pt.y - l);
    else
      lines[1].from = m_plane.PointAt(m_def_pt_2.x, m_def_pt_2.y + o);
    lines[1].to = m_plane.PointAt(m_def_pt_2.x, m_dimline_pt.y + e);
    isline[1] = true;
  }
  else
    isline[1] = false;

  const MYON_DimStyle::TextLocation text_location = style->DimTextLocation();
  const MYON::TextOrientation text_orientation = style->DimTextOrientation();
  const MYON_DimStyle::ContentAngleStyle text_angle_style = style->DimTextAngleStyle();


  double de[2] = { style->DimExtension() * dimscale, style->DimExtension() * dimscale };
  if (fabs(de[0]) < MYON_ZERO_TOLERANCE && ArrowIsFlipped(0))
    de[0] = style->ArrowSize() * dimscale * 1.5;
  if (fabs(de[1]) < MYON_ZERO_TOLERANCE && ArrowIsFlipped(1))
    de[1] = style->ArrowSize() * dimscale * 1.5;

  if (m_def_pt_2.x < 0.0)
  {
    double d = de[0];
    de[0] = -de[1];
    de[1] = -d;
  }

  lines[2].from = m_plane.PointAt(-de[0], m_dimline_pt.y);
  lines[2].to = m_plane.PointAt(m_def_pt_2.x + de[1], m_dimline_pt.y);
  isline[2] = true;
  isline[3] = false;

  if (/*UseDefaultTextPoint() &&*/ MYON_DimStyle::TextLocation::InDimLine != text_location)
  {
    if (m_use_default_text_point || fabs(m_user_text_point.y - m_dimline_pt.y) < style->TextGap() * dimscale * 0.75)
    {
      // If the dimline is under the text, and the text extends past the end of the dimline,
      // make the dim line as long as the text if the text is offset sideways from the 
      // extension lines.
      // If the text is within 3/4 * text gap of default vertical position, draw the extended line
      // If the text overlaps the extensions in both directions, it is centered and the
      // dimension line will hang out just a little each way, so don't do it in that case
      double t0, t1;
      lines[2].ClosestPointTo(text_rect[0], &t0);
      lines[2].ClosestPointTo(text_rect[1], &t1);
      if (fabs(t0 - t1) > 0.00001) // if text rect has some width
      {
        if (t0 > t1)
        {
          double t = t0; t0 = t1; t1 = t;
        }
        MYON_Line l = lines[2];
        if (t0 < 0.0 && t1 < 1.0)
          l.from = lines[2].PointAt(t0);
        if (t1 > 1.0 && t0 > 0.0)
          l.to = lines[2].PointAt(t1);
        lines[2] = l;
      }
    }
  }

  if (ArrowIsFlipped(0) && ArrowIsFlipped(1) && !style->ForceDimLine())
  {
    // Don't draw dimline between extensions if arrows are flipped
    lines[3].from = m_plane.PointAt(m_def_pt_2.x, m_dimline_pt.y);
    lines[3].to = lines[2].to;
    lines[2].to = m_plane.PointAt(0.0, m_dimline_pt.y);
    isline[3] = true;
  }
  else if (
    MYON_DimStyle::TextLocation::InDimLine == text_location
    || MYON::TextOrientation::InView == text_orientation
    || MYON_DimStyle::ContentAngleStyle::Aligned != text_angle_style
    ) // Means line has to be clipped around text
  {
    if (text_rect[0].DistanceTo(text_rect[2]) > MYON_SQRT_EPSILON)
    {
      MYON_Line dimline(lines[2]);
      MYON_Line line_segs[2];

      int seg_count = ClipLineToTextRect(vp, dimline, text_rect, line_segs);
      if (0 == seg_count)
        isline[2] = false;
      else
      {
        if (seg_count > 0)
          lines[2] = line_segs[0];
        if (seg_count > 1)
        {
          lines[3] = line_segs[1];
          isline[3] = true;
        }
      }
    }
  }
  return true;
}

void MYON_DimLinear::GetArrowXform(
  int which_end,
  double scale,
  bool arrowflipped,
  bool from_the_back,
  MYON_Xform& arrow_xform_out) const
{
  MYON_Xform xf, xfs, xfr;
  if (0 != which_end)
    which_end = 1;
  const MYON_Plane& plane = Plane();
  MYON_2dPoint ap = (which_end == 0) ? ArrowPoint1() : ArrowPoint2();
  xf.Rotation(MYON_xy_plane, plane);
  MYON_Xform xft = MYON_Xform::TranslationTransformation(ap.x, ap.y, 0.0);
  xf = xf * xft;
  double rotang = 0.0;
  bool flip = false;
  if (arrowflipped != (which_end == 0))
    flip = !flip;
  if (from_the_back)
    flip = !flip;
  MYON_2dVector v = DefPoint1() - DefPoint2();
  if (0.0 < (v * MYON_2dVector::XAxis))
    flip = !flip;
  if (flip)
    rotang += MYON_PI;
  rotang = fmod(rotang, (2.0 * MYON_PI));
  if (MYON_ZERO_TOLERANCE > fabs(rotang))
    rotang = 0.0;
  if (0.0 != rotang)
  {
    xfr.Rotation(rotang, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);
    xf = xf * xfr;
  }
  xfs = MYON_Xform::DiagonalTransformation(from_the_back ? -scale : scale, scale, scale);
  xf = xf * xfs;
  arrow_xform_out = xf;
}


//----------------------------------------------------------
// Class MYON_DimAngular

MYON_DimAngular::MYON_DimAngular()
  : MYON_Dimension(MYON::AnnotationType::Angular)
{}

bool MYON_DimAngular::IsValidAngularDimensionType(
  MYON::AnnotationType annotation_type
)
{
  return (
    MYON::AnnotationType::Angular == annotation_type ||
    MYON::AnnotationType::Angular3pt == annotation_type
    );
}

bool MYON_DimAngular::SetAngularDimensionType(
  MYON::AnnotationType angular_dimension_type
)
{
  if (!MYON_DimAngular::IsValidAngularDimensionType(angular_dimension_type))
  {
    MYON_ERROR("Invalid angular_dimension_type parameter.");
    return false;
  }

  m_annotation_type = angular_dimension_type;

  return true;
}




bool MYON_DimAngular::Write(
  MYON_BinaryArchive& archive
  ) const
{
  const int content_version = 0;
  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!MYON_Dimension::Internal_WriteDimension(archive))
      break;
    if (!archive.WriteVector(m_vec_1))
      break;
    if (!archive.WriteVector(m_vec_2))
      break;
    if (!archive.WriteDouble(m_ext_offset_1))
      break;
    if (!archive.WriteDouble(m_ext_offset_2))
      break;
    if (!archive.WritePoint(m_dimline_pt))
      break;
    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimAngular::Read(
  MYON_BinaryArchive& archive
  ) 
{
  *this = MYON_DimAngular::Empty;

  int content_version = -1;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (content_version < 0)
      break;
    if (!MYON_Dimension::Internal_ReadDimension(archive))
      break;
    if (!archive.ReadVector(m_vec_1))
      break;
    if (!archive.ReadVector(m_vec_2))
      break;
    if (!archive.ReadDouble(&m_ext_offset_1))
      break;
    if (!archive.ReadDouble(&m_ext_offset_2))
      break;
    if (!archive.ReadPoint(m_dimline_pt))
      break;
    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimAngular::Transform(const MYON_Xform& xform)
{
  bool rc = xform.IsIdentity();
  if (!rc)
  {
    rc = true;
    bool scaling = false;
    MYON_3dVector v = m_plane.xaxis;
    v.Transform(xform);
    if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
      scaling = true;
    else
    {
      v = m_plane.yaxis;
      v.Transform(xform);
      if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
        scaling = true;
      else
      {
        v = m_plane.zaxis;
        v.Transform(xform);
        if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
          scaling = true;
      }
    }

    if (scaling)
    {
      MYON_3dPoint extpt1(MYON_3dPoint::NanPoint);
      MYON_3dPoint extpt2(MYON_3dPoint::NanPoint);
      MYON_3dPoint arrowpt1(MYON_3dPoint::NanPoint);
      MYON_3dPoint arrowpt2(MYON_3dPoint::NanPoint);
      MYON_3dPoint dimlinept(MYON_3dPoint::NanPoint);
      MYON_3dPoint textpt(MYON_3dPoint::NanPoint);
      rc = Get3dPoints(nullptr, &extpt1, &extpt2, &arrowpt1, &arrowpt2, &dimlinept, &textpt);
      if (rc)
      {
        rc = m_plane.Transform(xform);
        extpt1.Transform(xform);
        extpt2.Transform(xform);
        arrowpt1.Transform(xform);
        arrowpt2.Transform(xform);
        dimlinept.Transform(xform);

        AdjustFromPoints(m_plane, extpt1, extpt2, arrowpt1, arrowpt2, dimlinept);
        if (!UseDefaultTextPoint())
        {
          textpt.Transform(xform);
          SetUserTextPoint(textpt);
        }
      }
    }
    else
      rc = m_plane.Transform(xform);

    if (rc)
      MYON_Geometry::Transform(xform);
  }
  return rc;
}

bool MYON_DimAngular::Create(
  const MYON_DimStyle* dim_style,
  MYON_Arc arc,
  double offset
)
{
  MYON_DimStyle local_dim_style;
  if (IsOverrideStylePointer(dim_style))
  {
    // make a local copy, because *this = MYON_DimAngular::Empty will delete dim_style.
    local_dim_style = *dim_style;
    dim_style = &local_dim_style;
  }
  *this = MYON_DimAngular::Empty;

  if (false == arc.IsValid())
    return false;

  const double arc_radius = arc.Radius();
  if (false == (arc_radius > 0.0))
    return false;

  if (nullptr == dim_style)
    dim_style = &MYON_DimStyle::Default;

  MYON_DimStyle* override_style = nullptr;
  MYON_UUID dimstyle_id = dim_style->ParentIdIsNotNil() ? dim_style->ParentId() : dim_style->Id();
  if (MYON_nil_uuid == dimstyle_id)
    dimstyle_id = MYON_DimStyle::Default.Id();

  if (dim_style->IsOverrideDimStyleCandidate(dim_style->ParentId(), true, nullptr))
  {
    override_style = new MYON_DimStyle(*dim_style);
    override_style->SetParentId(dimstyle_id);
  }

  *this = MYON_DimAngular::Empty;
  m_dimstyle_id = dimstyle_id;

  SetPlane(arc.Plane());
  if (nullptr != override_style)
    SetOverrideDimensionStyle(override_style);

  double dim_radius = arc_radius;
  if (MYON_IS_VALID(offset) && offset > -(1.0 - MYON_SQRT_EPSILON)*arc_radius)
    dim_radius += offset;

  SetAngularDimensionType(
    dim_radius != arc_radius
    ? MYON::AnnotationType::Angular
    : MYON::AnnotationType::Angular3pt
  );

  const MYON_Interval arc_angle = arc.DomainRadians();
  for (int i = 0; i < 2; i++)
  {
    MYON_2dVector& vec = (i == 0) ? m_vec_1 : m_vec_2;
    const double a = arc_angle[i];
    vec.x = cos(a);
    vec.y = sin(a);
  }
  double a = arc_angle.ParameterAt(1.0 / 3.0);
  m_dimline_pt.x = dim_radius*cos(a);
  m_dimline_pt.y = dim_radius*sin(a);
  m_ext_offset_1 = arc.Radius();
  m_ext_offset_2 = arc.Radius();

  return true;
}

bool MYON_DimAngular::Create(
  const MYON_DimStyle* dim_style,
  MYON_Line line1,
  MYON_3dPoint point_on_line1,
  MYON_Line line2,
  MYON_3dPoint point_on_line2,
  MYON_3dPoint point_on_angular_dimension_arc,
  bool bSetExtensionPoints
)
{
  MYON_DimStyle local_dim_style;
  if (IsOverrideStylePointer(dim_style))
  {
    // make a local copy, because *this = MYON_DimAngular::Empty will delete dim_style.
    local_dim_style = *dim_style;
    dim_style = &local_dim_style;
  }
  *this = MYON_DimAngular::Empty;

  if (false == point_on_angular_dimension_arc.IsValid())
    return false;

  MYON_3dVector dir1 = line1.Tangent();
  MYON_3dVector dir2 = line2.Tangent();
  if (false == dir1.IsUnitVector())
    return false;
  if (false == dir2.IsUnitVector())
    return false;

  MYON_3dPoint center = MYON_3dPoint::UnsetPoint;
  MYON_3dVector normal = MYON_3dVector::ZeroVector;
  bool bColinear = false;
  for (;;)
  {
    normal = MYON_CrossProduct(dir1, dir2);
    if ( false == normal.IsValid() )
      return false;
    if (false == normal.IsTiny() && normal.Unitize())
    {
      double t1 = MYON_UNSET_VALUE;
      double t2 = MYON_UNSET_VALUE;
      if (MYON_Intersect(line1, line2, &t1, &t2) && MYON_IsValid(t1) && MYON_IsValid(t2))
      {
        const MYON_3dPoint c1 = line1.PointAt(t1);
        if (false == c1.IsValid())
          return false;
        const MYON_3dPoint c2 = line2.PointAt(t2);
        if (false == c2.IsValid())
          return false;
        const double h1 = (point_on_angular_dimension_arc - c1)*normal;
        const double h2 = (point_on_angular_dimension_arc - c2)*normal;
        if (h1 <= h2)
        {
          center = c1 + h1*normal;
          if (center.IsValid())
            break;
          return false;
        }
        if (h2 < h1)
        {
          center = c2 + h2*normal;
          if (center.IsValid())
            break;
          return false;
        }
        // NANs
        return false;
      }
    }

    // if lines are colinear and point_on_arc is not on the line, we have a semi circle (180 degrees)
    normal = MYON_3dVector::ZeroVector;
    MYON_3dPoint q1 = line1.ClosestPointTo(point_on_angular_dimension_arc);
    MYON_3dPoint q2 = line2.ClosestPointTo(point_on_angular_dimension_arc);
    double d1 = point_on_angular_dimension_arc.DistanceTo(q1);
    double d2 = point_on_angular_dimension_arc.DistanceTo(q2);
    if (fabs(d1 - d2) <= MYON_SQRT_EPSILON*(d1 + d2))
    {
      // lines are colinear
      bColinear = true;
      if (d1 >= d2)
      {
        center = q1;
        normal = MYON_CrossProduct(dir1, point_on_angular_dimension_arc - center);
      }
      else
      {
        center = q2;
        normal = MYON_CrossProduct(dir2, point_on_angular_dimension_arc - center);
      }
      if (false == normal.IsTiny() &&  normal.Unitize() )
        break;
    }

    // lines are not colinear or point_on_arc is on the line.
    return false;
  }

  // project lines to arc's plane
  for (int i = 0; i < 2; i++)
  {
    MYON_3dVector& dir = (0 == i) ? dir1 : dir2;
    MYON_Line& line = (0 == i) ? line1 : line2;
    const double hfrom = (line.from - point_on_angular_dimension_arc)*normal;
    const double hto = (line.to - point_on_angular_dimension_arc)*normal;
    MYON_Line l1(line.from + hfrom*normal, line.to + hto*normal);
    const double h1from = (l1.from - point_on_angular_dimension_arc)*normal;
    const double h1to = (l1.to - point_on_angular_dimension_arc)*normal;
    if (fabs(h1from) < fabs(hfrom))
      line.from = l1.from;
    if (fabs(h1to) < fabs(hto))
      line.to = l1.to;
    if (false == line.IsValid())
      return false;
    dir = line.Tangent();
    if (false == dir.IsUnitVector())
      return false;
    if (!(fabs(dir*normal) < 1.0e-5))
      return false;
  }

  for (int i = 0; i < 2; i++)
  {
    MYON_3dVector& dir = (0 == i) ? dir1 : dir2;
    const MYON_Line& line = (0 == i) ? line1 : line2;
    MYON_3dPoint& point_on_line = (0 == i) ? point_on_line1 : point_on_line2;
    MYON_3dPoint p = line.PointAt(0.5);
    if (point_on_line.IsValid())
    {
      MYON_3dPoint q = line.ClosestPointTo(point_on_line);
      if (q.IsValid())
        p = q;
    }
    if ((p - center)*dir < 0.0)
    {
      dir = -dir;
      normal = -normal;
    }
  }

  MYON_Circle circle;
  if ( false == circle.plane.CreateFromNormal(center, normal) )
    return false;
  circle.plane.xaxis = dir1;
  circle.plane.yaxis = MYON_CrossProduct(normal, circle.plane.xaxis).UnitVector();
  if (false == circle.IsValid())
    return false;

  circle.radius = point_on_angular_dimension_arc.DistanceTo(circle.plane.origin);
  if (false == (circle.radius > 0.0) || false == circle.IsValid() )
    return false;



  MYON_3dPoint arc_end1 = MYON_3dPoint::UnsetPoint;
  MYON_3dPoint arc_end2 = MYON_3dPoint::UnsetPoint;
  for (int i = 0; i < 2; i++)
  {
    MYON_Line& line = (0 == i) ? line1 : line2;
    MYON_3dPoint& point_on_line = (0 == i) ? point_on_line1 : point_on_line2;
    MYON_3dPoint& arc_end = (0 == i) ? arc_end1 : arc_end2;
    double t0, t1;
    MYON_3dPoint p0, p1;
    if (2 != MYON_Intersect(line, circle, &t0, p0, &t1, p1))
      return false;

    double d0 = MYON_DBL_QNAN;
    double d1 = MYON_DBL_QNAN;
    if (point_on_line.IsValid())
    {
      double t;
      if (false == line.ClosestPointTo(point_on_line, &t))
        return false;
      d0 = fabs(t0 - t);
      d1 = fabs(t1 - t);
      MYON_3dPoint q = line.PointAt(t);
      if (point_on_line.DistanceTo(q) > MYON_SQRT_EPSILON*line.Length() )
        point_on_line = q;
    }
    else
    {
      d0 = fabs(t0 - 0.5);
      d1 = fabs(t1 - 0.5);
    }
    if (d0 <= d1)
      arc_end = p0;
    else if (d1 < d0)
      arc_end = p1;
    else
      return false; // NAN
  }

  circle.plane.xaxis = (arc_end1 - center).UnitVector();
  circle.plane.yaxis = MYON_CrossProduct(normal, circle.plane.xaxis).UnitVector();
  if (false == circle.IsValid())
    return false;
  
  MYON_2dVector a, b;
  if (false == circle.plane.ClosestPointTo(point_on_angular_dimension_arc, &a.x, &a.y))
    return false;
  if (false == circle.plane.ClosestPointTo(arc_end2, &b.x, &b.y))
    return false;
  double alpha = atan2(a.y, a.x);
  double beta = atan2(b.y, b.x);
  if (alpha < 0.0)
  {
    alpha += 2.0*MYON_PI;
    if (alpha < 0.0)
      alpha = 0.0;
  }
  if (beta < 0.0)
  {
    beta += 2.0*MYON_PI;
    if (beta < 0.0)
      beta = 0.0;
  }
  if (alpha > beta)
  {
    beta = 2.0*MYON_PI - beta;
    if (false == (beta > 0.0 && beta < 2.0*MYON_PI))
      return false;
    circle.plane.xaxis = (arc_end2 - center).UnitVector();
    circle.plane.yaxis = MYON_CrossProduct(normal, circle.plane.xaxis).UnitVector();
    if (false == circle.IsValid())
      return false;
  }

  for (;;)
  {
    MYON_Arc arc;
    if (false == arc.Create(circle, MYON_Interval(0.0, beta)) || false == arc.IsValid())
      return false;

    if (false == Create(dim_style, arc, MYON_UNSET_VALUE))
      break;
    
    SetAngularDimensionType(MYON::AnnotationType::Angular);

    if (bSetExtensionPoints)
    {
      for (int i = 0; i < 2; i++)
      {
        const MYON_3dPoint point_on_line = (i == 0) ? point_on_line1 : point_on_line2;
        if (false == point_on_line.IsValid())
          continue;

        const MYON_2dVector vec = (i == 0) ? m_vec_1 : m_vec_2;
        double& ext_offset = (i == 0) ? m_ext_offset_1 : m_ext_offset_2;
        MYON_2dVector e;
        if (circle.plane.ClosestPointTo(point_on_line, &e.x, &e.y))
        {
          double d = e*vec;
          if (d > 0.0 && fabs(d - arc.Radius()) > 0.001*arc.Radius())
            ext_offset = d;
        }
      }
    }
    return true;
  }

  *this = MYON_DimAngular::Empty;
  return false;
}

// Creates dimension with extension lines starting at plane origin (arc center)
bool MYON_DimAngular::Create(
  const MYON_UUID style_id,
  const MYON_Plane& plane,
  const MYON_3dVector& ref_horizontal,
  const MYON_3dPoint& center_pt,
  const MYON_3dPoint& extension_pt1,    // point on first extension line
  const MYON_3dPoint& extension_pt2,    // point on second extension line
  const MYON_3dPoint& dimline_pt        // point on dimension arc
)
{
  m_dimstyle_id = style_id;
  SetAngularDimensionType(MYON::AnnotationType::Angular3pt);
  //if (MYON_nil_uuid == m_dimstyle_id)
  //  return true;

  bool rc = AdjustFromPoints(plane, center_pt, extension_pt1, extension_pt2, dimline_pt);
  SetAngularDimensionType(MYON::AnnotationType::Angular3pt);
  SetHorizontalDirection( MYON_2dVector(ref_horizontal) );
  return rc;
}

bool MYON_DimAngular::Create(
  const MYON_UUID style_id,
  const MYON_Plane& plane,
  const MYON_3dVector& ref_horizontal,
  const MYON_3dPoint& extension_pt1,      // start of first extension line
  const MYON_3dPoint& extension_pt2,      // start of second extension line
  const MYON_3dPoint& direction_pt1,      // point on first extension vector
  const MYON_3dPoint& direction_pt2,      // point on second extension vector
  const MYON_3dPoint& dimline_pt          // point on dimension line
)
{
  m_dimstyle_id = style_id;
  //if (MYON_nil_uuid == m_dimstyle_id)
  //  return true;

  bool rc = AdjustFromPoints(plane, extension_pt1, extension_pt2, direction_pt1, direction_pt2, dimline_pt);
  SetAngularDimensionType(MYON::AnnotationType::Angular);
  SetHorizontalDirection( MYON_2dVector(ref_horizontal) );
  return rc;
}

static bool VectorAngle(MYON_2dVector v, double& angle)
{
  if (v.IsTiny())
    return false;
  v.Unitize();
  angle = atan2(v.y, v.x);
  while (angle < 0.0)
    angle += 2.0 * MYON_PI;
  while (angle >= 2.0 * MYON_PI)
    angle -= 2.0 * MYON_PI;
  return true;
}

bool MYON_DimAngular::FindAngleVertex(
  MYON_Line lines[2],
  MYON_3dPoint pickpoints[2],
  const MYON_Plane& plane,
  MYON_3dPoint& centerpoint_out)
{
  // Intersect lines to get centerpoint
  double a, b;
  if (MYON_IntersectLineLine(lines[0], lines[1], &a, &b, 0.01, false))
  {
    centerpoint_out = lines[0].PointAt(a);
    return true;
  }
  else
  {
    // If the lines don't intersect, project to getpoint cplane and intersect
    MYON_3dPoint from0, to0, from1, to1;
    from0 = plane.ClosestPointTo(lines[0].from);
    to0 = plane.ClosestPointTo(lines[0].to);
    from1 = plane.ClosestPointTo(lines[1].from);
    to1 = plane.ClosestPointTo(lines[1].to);
    MYON_Line l0(from0, to0);
    MYON_Line l1(from1, to1);
    if (MYON_IntersectLineLine(l0, l1, &a, &b, 0.01, false))
    {
      centerpoint_out = lines[0].PointAt(a);
      return true;
    }
    else  // no line intersection, check for collinear
    {
      MYON_3dVector v0 = l0.Direction();
      MYON_3dVector v1 = l1.Direction();
      int isp = v0.IsParallelTo(v1);
      if (isp != 0) // Lines are parallel
      {
        MYON_3dPoint pon = l0.ClosestPointTo(l1.from);
        if (pon.DistanceTo(l1.from) > MYON_ZERO_TOLERANCE) // lines aren't collinear
        {
          return false;
        }
        else
        {
          // use endpoints of lines if they meet at a point, otherwise, 1/2 way between pickpoints
          centerpoint_out = (pickpoints[0] + pickpoints[1]) / 2.0;
          if (l0.from.DistanceTo(l1.from) < MYON_ZERO_TOLERANCE || l0.from.DistanceTo(l1.to) < MYON_ZERO_TOLERANCE)
            centerpoint_out = l0.from;
          else if (l0.to.DistanceTo(l1.from) < MYON_ZERO_TOLERANCE || l0.to.DistanceTo(l1.to) < MYON_ZERO_TOLERANCE)
            centerpoint_out = l0.to;
          return true;
        }
      }
    }
  }
  return false;
}


bool MYON_DimAngular::AdjustFromPoints(
  const MYON_Plane& plane,
  const MYON_3dPoint& center_pt,
  const MYON_3dPoint& extension_pt1,
  const MYON_3dPoint& extension_pt2,
  const MYON_3dPoint& dimline_pt
  )
{
  if (center_pt.DistanceTo(dimline_pt) < MYON_ZERO_TOLERANCE)
    return false;

  MYON_2dPoint def1, def2, dimline;
  MYON_2dVector v;
  m_plane = plane;
  m_plane.origin = plane.ClosestPointTo(center_pt);
  if (!m_plane.ClosestPointTo(extension_pt1, &def1.x, &def1.y))
    return false;
  v = def1;
  if (v.Unitize())
  {
    m_plane.Rotate(v.y, v.x, plane.Normal());
    m_plane.ClosestPointTo(extension_pt1, &def1.x, &def1.y);
  }
  m_plane.ClosestPointTo(extension_pt2, &def2.x, &def2.y);
  m_plane.ClosestPointTo(dimline_pt, &dimline.x, &dimline.y);

  double a1 = MYON_DBL_QNAN;
  double a2 = MYON_DBL_QNAN;
  double ad = MYON_DBL_QNAN;
  if (VectorAngle(MYON_2dVector(def1), a1) && VectorAngle(MYON_2dVector(def2), a2) && VectorAngle(MYON_2dVector(dimline), ad))
  {
    if (ad > a2) // Swap the order of the points to get dimlinept between them
    {
      m_plane = plane;
      m_plane.origin = plane.ClosestPointTo(center_pt);
      if (!m_plane.ClosestPointTo(extension_pt2, &def1.x, &def1.y))
        return false;
      v = def1;
      if (v.Unitize())
      {
        m_plane.Rotate(v.y, v.x, plane.Normal());
        m_plane.ClosestPointTo(extension_pt2, &def1.x, &def1.y);
      }
      m_plane.ClosestPointTo(extension_pt1, &def2.x, &def2.y);
      m_plane.ClosestPointTo(dimline_pt, &dimline.x, &dimline.y);
    }
  }
  double offset1 = ((MYON_2dVector)def1).Length();
  if (offset1 > MYON_SQRT_EPSILON)
  {
    m_vec_1 = def1;
    m_vec_1.Unitize();
  }
  double offset2 = ((MYON_2dVector)def2).Length();
  if (offset2 > MYON_SQRT_EPSILON)
  {
    m_vec_2 = def2;
    m_vec_2.Unitize();
  }

  VectorAngle(m_vec_2, a2);
  MYON_2dVector vd(m_vec_1);
  vd.Rotate(a2 / 3.0);
  double r = ((MYON_2dVector)dimline).Length();
  m_dimline_pt = MYON_2dPoint(vd * r);
  m_ext_offset_1 = offset1;
  m_ext_offset_2 = offset2;

  ClearText();

  return true;
}

bool MYON_DimAngular::AdjustFromPoints(
  const MYON_Plane& plane,
  const MYON_3dPoint& extension_pt1,      // start of first extension line
  const MYON_3dPoint& extension_pt2,      // start of second extension line
  const MYON_3dPoint& direction_pt1,      // point on first extension vector
  const MYON_3dPoint& direction_pt2,      // point on second extension vector
  const MYON_3dPoint& dimline_pt          // point on dimension line
)
{
  if (!plane.IsValid() || !extension_pt1.IsValid() || !extension_pt2.IsValid() || !direction_pt1.IsValid() || !direction_pt2.IsValid() || !dimline_pt.IsValid())
    return false;

  MYON_3dPoint pe1on = plane.ClosestPointTo(extension_pt1);
  MYON_3dPoint pe2on = plane.ClosestPointTo(extension_pt2);
  MYON_3dPoint pd1on = plane.ClosestPointTo(direction_pt1);
  MYON_3dPoint pd2on = plane.ClosestPointTo(direction_pt2);

  if (pe1on.DistanceTo(pd1on) < MYON_ZERO_TOLERANCE || pe2on.DistanceTo(pd2on) < MYON_ZERO_TOLERANCE)
    return false;

  MYON_3dPoint center_point = MYON_3dPoint::Origin;

  MYON_Line lines[2] = { MYON_Line(pe1on, pd1on), MYON_Line(pe2on, pd2on) };
  MYON_3dPoint pickpoints[2] = { pe1on, pe2on };
  if (!MYON_DimAngular::FindAngleVertex(lines, pickpoints, plane, center_point))
    return false;

  MYON_3dPoint pd1 = pe1on;
  MYON_3dPoint pd2 = pe2on;
  if (center_point.DistanceTo(pe1on) < MYON_SQRT_EPSILON)
    pd1 = pd1on;
  if (center_point.DistanceTo(pe2on) < MYON_SQRT_EPSILON)
    pd2 = pd2on;

  if (AdjustFromPoints(plane, center_point, pd1, pd2, dimline_pt))
  {
    m_ext_offset_1 = center_point.DistanceTo(pe1on);
    m_ext_offset_2 = center_point.DistanceTo(pe2on);
    return true;
  }
  return false;
}

bool MYON_DimAngular::GetBBox(double* bmin, double* bmax, bool grow) const
{
  const MYON_DimStyle* dimstyle = nullptr;
  return GetAnnotationBoundingBox(nullptr, dimstyle, 1.0, bmin, bmax, grow?true:false);
}

bool MYON_DimAngular::GetAnnotationBoundingBox(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  double* boxmin,
  double* boxmax,
  bool bGrow
) const
{
  if (nullptr == dimstyle)
    dimstyle = &MYON_DimStyle::Default;

  const MYON_2dPoint hash_points[] = {
    MYON_2dPoint(m_vec_1),
    MYON_2dPoint(m_vec_2), 
    m_dimline_pt,
    MYON_2dPoint(m_ext_offset_1,m_ext_offset_2) 
  };

  const MYON_SHA1_Hash hash = Internal_GetBBox_InputHash(
    vp, 
    dimstyle, 
    dimscale, 
    m_user_text_point,
    (unsigned int)(sizeof(hash_points)/sizeof(hash_points[0])),
    hash_points
  );

  if (Internal_GetBBox_Begin(hash, boxmin, boxmax, bGrow))
    return true;

  if (nullptr == boxmin || nullptr == boxmax)
    return false;
  
  MYON_3dVector view_xdir = MYON_3dVector::XAxis;
  MYON_3dVector view_ydir = MYON_3dVector::YAxis;
  if (nullptr != vp)
  {
    view_xdir = vp->CameraX();
    view_ydir = vp->CameraY();
  }
  MYON_Xform text_xform;
  GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform);

  MYON_BoundingBox dim_box;

  const MYON_TextContent* text = Text();
  MYON_3dPoint text_rect[4] = { MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin };

  if (nullptr != text && text->GetTightBoundingBox(dim_box))
  {
    text_rect[0].Set(dim_box.m_min.x, dim_box.m_min.y, 0.0);
    text_rect[1].Set(dim_box.m_max.x, dim_box.m_min.y, 0.0);
    text_rect[2].Set(dim_box.m_max.x, dim_box.m_max.y, 0.0);
    text_rect[3].Set(dim_box.m_min.x, dim_box.m_max.y, 0.0);
    for (int i = 0; i < 4; i++)
      text_rect[i].Transform(text_xform);  // Text + gap bounding rect
  }

  dim_box.Destroy();
  for (int i = 0; i < 4; i++)
  {
    dim_box.Set(text_rect[i], 0 < i ? true : false);
  }


  bool dimlines[2] = { false, false };
  MYON_Line lines[2];
  bool dimarcs[2] = { false, false };
  MYON_Arc arcs[2];
  if (nullptr != dimstyle && GetDisplayLines(vp, dimstyle, dimscale, text_rect, lines, dimlines, arcs, dimarcs, 2, 2))
  {
    for (int i = 0; i < 2; i++)
    {
      if (dimlines[i])
      {
        dim_box.Set(lines[i].from, true);
        dim_box.Set(lines[i].to, true);
      }
    }
    for (int i = 0; i < 2; i++)
    {
      if (dimarcs[i])
      {
        arcs[i].GetTightBoundingBox(dim_box, true);
      }
    }
  }

  double points[21];
  if(Get3dPoints((MYON_3dPoint*)(&points[0]), (MYON_3dPoint*)(&points[3]), (MYON_3dPoint*)(&points[6]), (MYON_3dPoint*)(&points[9]),
                 (MYON_3dPoint*)(&points[12]), (MYON_3dPoint*)(&points[15]), (MYON_3dPoint*)(&points[18])))
    dim_box.Set(3, 0, 6, 3, points + 3, true);

  return Internal_GetBBox_End(dim_box, hash, boxmin, boxmax, bGrow);
}

bool MYON_DimAngular::GetTextXform(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  return GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform_out);
}

bool MYON_DimAngular::GetTextXform(
  const MYON_Xform* model_xform,
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  if (nullptr == dimstyle)
    return false;

  // This gets the display text that's already on the dimension
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;

  // See if the text needs remade because of some change in some property that
  // would change its appearance
  if (DimStyleTextPositionPropertiesHash() != dimstyle->TextPositionPropertiesHash())
  {
    MYON_wString rtfstr = text->RtfText();
    MYON::AnnotationType annotation_type = this->Type();
    bool wrapped = text->TextIsWrapped();
    double width = text->FormattingRectangleWidth();
    double rot = text->TextRotationRadians();
    const_cast<MYON_TextContent*>(text)->Create(rtfstr, annotation_type, dimstyle, wrapped, width, rot);
  }

  double text_width = 0.0;
  double text_height = 0.0;
  double text_gap = 0.0;
  double text_angle = 0.0;      // User set rotation - add to natural rotation
  double dim_text_angle_2d = 0.0;

  const MYON_DimStyle::TextLocation text_location = dimstyle->DimTextLocation();
  const MYON::TextOrientation text_orientation = dimstyle->DimTextOrientation();
  const MYON_DimStyle::ContentAngleStyle text_angle_style = dimstyle->DimTextAngleStyle();


  MYON_Xform t2dxf(1.0);         // Text plane to dimension plane rotation
  MYON_Xform d2tpxf(1.0);        // Dimension plane to text point translation
  MYON_Xform trxf(1.0);          // Text rotation around text plane origin point

  const MYON_Plane& textplane = MYON_xy_plane;
  const MYON_Plane& dimplane = Plane();
  MYON_3dPoint text_center = MYON_3dPoint::Origin;

  MYON_3dPoint cp[4];
  if (!text->Get3dCorners(cp))
    return false;

  text_center = (cp[0] + cp[2]) / 2.0;
  text_width = (cp[1].x - cp[0].x) * dimscale;
  text_height = (cp[3].y - cp[0].y) * dimscale;

  text_xform_out = MYON_Xform::IdentityTransformation;
  t2dxf.Rotation(textplane, dimplane);     // Rotate text from starting text plane (world xy) to dimension plane

  text_gap = dimstyle->TextGap();
  if (dimstyle->MaskFrameType() != MYON_TextMask::MaskFrame::NoFrame)
    text_gap = dimstyle->TextMask().MaskBorder();
  text_gap *= dimscale;

  bool draw_forward = dimstyle->DrawForward();

  MYON_2dPoint text_pt_2d = TextPoint();

  bool arrowflipped[2] = { false, false };
  bool text_outside = false;

  MYON_DimStyle::arrow_fit arrow_fit = dimstyle->ArrowFit();
  if (MYON_DimStyle::arrow_fit::ArrowsOutside == arrow_fit)
    arrowflipped[0] = arrowflipped[1] = true;

  MYON_DimStyle::text_fit text_fit = dimstyle->TextFit();

  // See if arrows and text will all fit inside extension lines
  // or what has to be moved outside
  double asz = dimstyle->ArrowSize() * dimscale;
  double dist = Radius() * Measurement();

  double total_text_width = text_width;
  if (text_fit != MYON_DimStyle::text_fit::Auto)
    total_text_width = 0.0;
  else if (0.0 < total_text_width)
    total_text_width += text_gap;

  if (text_fit != MYON_DimStyle::text_fit::Auto &&
      text_fit != MYON_DimStyle::text_fit::TextInside)
    text_outside = true;

  static double arrow_width_factor = 1.5;
  double total_arrow_width = asz * arrow_width_factor * 2;  // min arrow tail space is asz/2

  if (arrowflipped[0])
    total_arrow_width -= (asz * arrow_width_factor);
  if (arrowflipped[1])
    total_arrow_width -= (asz * arrow_width_factor);

  if (total_arrow_width + total_text_width > dist)     // arrows + text dont fit
  {
    if (total_text_width > dist)   // text doesnt fit
    {
      // move text outside
      text_outside = true;
      if (total_arrow_width > dist && MYON_DimStyle::arrow_fit::Auto == arrow_fit)  // arrows dont fit either
      {
        arrowflipped[0] = true;
        arrowflipped[1] = true;
      }
    }
    else if (MYON_DimStyle::arrow_fit::Auto == arrow_fit)      // text fits
    {
      // flip arrows
      arrowflipped[0] = true;
      arrowflipped[1] = true;
    }
  }

  // Dimension's MYON_TextContent display text is stored at wcs origin coords until it is drawn
  // text_xform positions text at the 3d point and rotation to draw
  if (fabs(text_pt_2d.x) < MYON_SQRT_EPSILON && fabs(text_pt_2d.y) < MYON_SQRT_EPSILON)
    text_pt_2d.Set(0.0, 0.0);

  if (text_outside && MYON_DimStyle::ContentAngleStyle::Horizontal != text_angle_style && UseDefaultTextPoint())
  {
    double radius = Radius();
    // move textpoint outside right arrow by 1/2 text width + 1-1/2 arrow width
    double x = text_width * 0.5 + text_gap;
    if (text_fit == MYON_DimStyle::text_fit::TextLeft)
    {
      //if (arrowflipped[0])
      x += 1.5 * asz * arrow_width_factor;
      text_pt_2d = ArrowPoint1();
      if (0.0 < radius)
      {
        double d_ang = x / radius;
        text_pt_2d.Rotate(-d_ang, MYON_2dPoint::Origin);
      }
    }
    else
    {
      //if (arrowflipped[1])
      x += asz * arrow_width_factor;
      text_pt_2d = ArrowPoint2();
      if (0.0 < radius)
      {
        double d_ang = x / radius;
        text_pt_2d.Rotate(d_ang, MYON_2dPoint::Origin);
      }
    }
  }

  FlipArrow(0, arrowflipped[0]);
  FlipArrow(1, arrowflipped[1]);

  if (MYON::TextOrientation::InPlane == text_orientation)
  {
    if (MYON_DimStyle::ContentAngleStyle::Rotated == text_angle_style)
    {
      // Rotation angle = 0 means the text is horizontal
      text_angle = 0.0; //TextRotation(); Was some gooffy 0 to 1 property from V5. Not degrees or radians
    }
    else if (MYON_DimStyle::ContentAngleStyle::Horizontal == text_angle_style)
    {
      text_angle = 0.0;
    }
    if (MYON_DimStyle::ContentAngleStyle::Aligned != text_angle_style)
    {
      MYON_2dVector h = HorizontalDirection();
      double h_angle = atan2(h.y, h.x);
      text_angle += h_angle;
    }
  }
  
  // Moves the text to AboveLine if that's the mode
  MYON_3dPoint text_point_3d = dimplane.PointAt(text_pt_2d.x, text_pt_2d.y);  // 3d text point
  MYON_3dVector text_up_dir = text_point_3d - dimplane.origin;
  text_up_dir.Unitize();
  MYON_3dVector text_right_dir = MYON_CrossProduct(text_up_dir, dimplane.zaxis);
  MYON_2dVector text_pt_dir_2d = text_pt_2d;  // From center toward text point
  text_pt_dir_2d.Unitize();
  if (MYON::TextOrientation::InPlane == text_orientation &&
    MYON_DimStyle::ContentAngleStyle::Horizontal != text_angle_style)
    dim_text_angle_2d = atan2(text_pt_dir_2d.y, text_pt_dir_2d.x) - (MYON_PI / 2.0);  // How the dimension would rotate it
  else
    dim_text_angle_2d = 0.0;

  MYON_Xform xfWorld2Cam;
  MYON_3dVector text_right_in_view = text_right_dir;
  MYON_3dVector text_z_in_view = dimplane.zaxis;
  if (nullptr != vp)
  {
    vp->GetXform(MYON::coordinate_system::world_cs, MYON::coordinate_system::camera_cs, xfWorld2Cam);
    text_right_in_view.Transform(xfWorld2Cam);
    text_z_in_view.Transform(xfWorld2Cam);
  }
  bool x_to_the_right = (text_right_in_view * MYON_3dVector::XAxis) > -MYON_SQRT_EPSILON;
  bool fromfront = text_z_in_view * MYON_3dVector::ZAxis > 0.0;

  if (MYON_DimStyle::TextLocation::AboveDimLine == text_location )
  {
    double d = text_height * 0.5 + text_gap;
    if (x_to_the_right != fromfront)
      d = -d;

    // Moves the text to AboveLine if that's the alignment mode
    text_pt_2d = text_pt_2d + (text_pt_dir_2d * d);
  }
  text_point_3d = dimplane.PointAt(text_pt_2d.x, text_pt_2d.y);
  d2tpxf = MYON_Xform::TranslationTransformation(text_point_3d - dimplane.origin);                // Move text from dimplane origin to text point

  if (1.0e-2 < fabs(text_angle + dim_text_angle_2d))  // There's a rotation angle change of more than 1/100 radian (~1/2 degree)
    trxf.Rotation(text_angle + dim_text_angle_2d, MYON_3dVector::ZAxis, MYON_3dPoint::Origin); // hopefully dim_text_angle_2d already adjusts to horizontal

  text_xform_out = MYON_Xform::DiagonalTransformation(dimscale, dimscale, dimscale);      // dimscale
  
  text_xform_out = trxf * text_xform_out;                  // text rotation
  text_xform_out = t2dxf * text_xform_out;                 // text plane to dim plane
  text_xform_out = d2tpxf * text_xform_out;                // dimension plane to text point

  MYON_3dVector view_xdir = MYON_3dVector::XAxis;
  MYON_3dVector view_ydir = MYON_3dVector::YAxis;
  MYON_3dVector view_zdir = MYON_3dVector::ZAxis;
  if (nullptr != vp)
  {
    view_xdir = vp->CameraX();
    view_ydir = vp->CameraY();
    view_zdir = vp->CameraZ();
  }
  MYON_3dVector dim_xdir = Plane().xaxis;
  MYON_3dVector dim_ydir = Plane().yaxis;
  if (nullptr != model_xform)
  {
    dim_xdir.Transform(*model_xform);
    dim_ydir.Transform(*model_xform);
  }

  if (MYON::TextOrientation::InView == text_orientation)  // Draw dimension horizontal to view
  {

    MYON_Xform tp2sxf;        // Text point to view plane rotation
    tp2sxf.Rotation(text_point_3d, dimplane.xaxis, dimplane.yaxis, dimplane.zaxis, text_point_3d, view_xdir, view_ydir, view_zdir);
    text_xform_out = tp2sxf * text_xform_out;
  }
  else if (draw_forward)
  {
    MYON_3dVector text_right_dir_local(1.0, 0.0, 0.0);
    text_right_dir_local.Transform(text_xform_out);
    if (nullptr != model_xform)
      text_right_dir_local.Transform(*model_xform);
    if (text_right_dir_local.Unitize())
    {
      MYON_3dVector zdir = MYON_CrossProduct(dim_xdir, dim_ydir);
      MYON_3dVector text_up_dir_local = MYON_CrossProduct(zdir, text_right_dir_local);
      bool fx = (0.0 > view_xdir * text_right_dir_local);
      bool fy = (0.0 > view_ydir * text_up_dir_local);

      MYON_Xform mxf;  // Mirror xform for backwards text to adjust DrawForward
      if (fx)
      {
        mxf.Mirror(text_center, MYON_3dVector::XAxis);
        text_xform_out = text_xform_out * mxf;
      }
      if (fy)
      {
        mxf.Mirror(text_center, MYON_3dVector::YAxis);
        text_xform_out = text_xform_out * mxf;
      }
    }
  }
  return true;
}


bool MYON_DimAngular::UpdateDimensionText(const MYON_DimStyle* dimstyle) const
{
  MYON_wString displaytext;

  if (!GetAngleDisplayText(dimstyle, displaytext))
    return false;

  MYON_TextContent* newtext = new MYON_TextContent;
  if (nullptr != newtext)
  {
    bool wrapped = m_text ? m_text->TextIsWrapped() : false;
    double rect_width = m_text ? m_text->FormattingRectangleWidth() : 0.0;
    double rotation = m_text ? m_text->TextRotationRadians() : 0.0;
    if (newtext->Create(displaytext, Type(), dimstyle,   wrapped, rect_width, rotation))
    {
#ifdef _DEBUG
      newtext->IsValid();
#endif
      SetText(newtext);
      return true;
    }
  }
  return false;
}

bool MYON_DimAngular::GetAngleDisplayText(
  const MYON_DimStyle* dimstyle,
  MYON_wString& displaytext) const
{
  if (nullptr == dimstyle)
    return false;

  double measurement = Measurement();
  const wchar_t* user_text = UserText();
  MYON_TextContent::FormatAngleMeasurement(measurement, dimstyle, user_text, displaytext);
  return true;
}




double MYON_DimAngular::Radius() const
{
  return ((MYON_2dVector)m_dimline_pt).Length();
}

bool MYON_DimAngular::GetAngles(double* start_ang, double* end_ang, double* mid_ang) const
{
  if (nullptr == start_ang || nullptr == end_ang)
    return false;
  //MYON_2dVector vm = m_dimline_pt;
  bool rc = false;
  {
    *start_ang = atan2(m_vec_1.y, m_vec_1.x);
    *end_ang = atan2(m_vec_2.y, m_vec_2.x);
    rc = true;
  }

  if (rc && nullptr != mid_ang)
  {
    MYON_2dVector vm = m_dimline_pt;
    if (vm.Unitize())
    {
      *mid_ang = atan2(vm.y, vm.x);
      rc = true;
    }
    else
      rc = false;
  }
  return rc;
}

// Returns angle between |defpt1 - centerpt| and |defpt2 - centerpt| in radians
double MYON_DimAngular::Measurement() const
{
  double a1 = 0.0, a2 = 0.0, am = 0.0;
  GetAngles(&a1, &a2, &am);
  double a = 0.0;
  if(MYON_ZERO_TOLERANCE > fabs(a1))
    a1 = 0.0;
  else
  {
    a2 -= a1;
    am -= a1;
    a1 = 0.0;
  }

  if(a2 < 0.0)
    a2 += 2.0 * MYON_PI;
  if(am < 0.0)
    am += 2.0 * MYON_PI;

  if(am > a1)
  {
    if(am < a2)
      a = a2 - a1;
    else
      a = a2;
  }
  return a;
}

// Returns midpoint of dimline
MYON_2dPoint MYON_DimAngular::DefaultTextPoint() const
{
  MYON_2dPoint tp(0.0, 0.0);
  double a1 = 0.0, a2 = 0.0, am = 0.0;
  if (GetAngles(&a1, &a2, &am))
  {
    if (a2 < 0.0)
      a2 += MYON_PI * 2.0;
    double a0 = a2 - a1;
    MYON_2dPoint c = CenterPoint();
    MYON_2dPoint d = DimlinePoint();
    double r = c.DistanceTo(d);
    double a = a0 * 0.5;
    double cosa0 = cos(a);
    double sina0 = sin(a);
    tp.x = r * cosa0;
    tp.y = r * sina0;
  }
  return tp;
}

MYON_2dPoint MYON_DimAngular::CenterPoint() const
{
  return MYON_2dPoint::Origin;
}

MYON_2dPoint MYON_DimAngular::DefPoint1() const
{
  return m_vec_1 * m_ext_offset_1;
}

MYON_2dPoint MYON_DimAngular::DefPoint2() const
{
  return m_vec_2 * m_ext_offset_2;
}

MYON_2dPoint MYON_DimAngular::DimlinePoint() const
{
  return m_dimline_pt;
}

MYON_2dPoint MYON_DimAngular::ArrowPoint1() const
{
    return m_vec_1  * Radius();
}

MYON_2dPoint MYON_DimAngular::ArrowPoint2() const
{
  return m_vec_2  * Radius();
}

MYON_2dPoint MYON_DimAngular::UserTextPoint() const
{
  return m_user_text_point;
}

void MYON_DimAngular::SetUserTextPoint(const MYON_3dPoint& point)
{
  MYON_2dPoint p;
  if (m_plane.ClosestPointTo(point, &p.x, &p.y))
    m_user_text_point = p;
}

MYON_2dVector MYON_DimAngular::ExtDir1() const
{
  return m_vec_1;
}

MYON_2dVector MYON_DimAngular::ExtDir2() const
{
  return m_vec_2;
}

void MYON_DimAngular::SetExtDir1(const MYON_2dVector& dir1)
{
  m_vec_1 = dir1;
}

void MYON_DimAngular::SetExtDir2(const MYON_2dVector& dir2)
{
  m_vec_2 = dir2;
}

void MYON_DimAngular::Set2dCenterPoint(MYON_2dPoint pt)
{
  // Move plane origin to pt
  MYON_2dVector v(-pt.x, -pt.y);
  MYON_3dPoint p = m_plane.PointAt(pt.x, pt.y);
  m_plane.origin = p;
  m_plane.UpdateEquation();
  m_dimline_pt = m_dimline_pt + v;
}

void MYON_DimAngular::Set2dDefPoint1(MYON_2dPoint pt)
{
  // Rotate plane to keep first line on x axis
  MYON_2dVector xdir = pt;
  double r = xdir.Length();
  if (!xdir.Unitize())
    return;

  if (fabs((xdir * MYON_2dVector::XAxis) - 1.0) > MYON_SQRT_EPSILON)
  {
    m_plane.Rotate(xdir.y, xdir.x, m_plane.zaxis);
    m_vec_2.Rotate(-xdir.y, xdir.x);
    m_dimline_pt.Rotate(-xdir.y, xdir.x, MYON_2dPoint::Origin);
  }
  m_ext_offset_1 = r;
}

void MYON_DimAngular::Set2dDefPoint2(MYON_2dPoint pt)
{
  MYON_2dVector xdir = pt;
  if (xdir.Unitize())
    m_vec_2 = xdir;
}

void MYON_DimAngular::Set2dDimlinePoint(MYON_2dPoint pt)
{
  m_dimline_pt = pt;
}

//void MYON_DimAngular::Set3dCenterPoint(MYON_3dPoint pt)
//{
//  m_plane.origin = pt;
//}
//
//void MYON_DimAngular::Set3dDefPoint1(MYON_3dPoint pt)
//{
//  MYON_3dVector x = pt - m_plane.origin;
//  MYON_3dVector z = m_plane.zaxis;
//  if (0.999 < x * z) // x & z nearly parallel
//    z = MYON_CrossProduct(x, m_plane.yaxis);
//  if (z.Unitize())
//  {
//    MYON_3dVector y = MYON_CrossProduct(z, x);
//    if (y.Unitize())
//      m_plane.CreateFromFrame(m_plane.origin, x, y);
//  }
//}
//
//void MYON_DimAngular::Set3dDefPoint2(MYON_3dPoint pt)
//{
//  MYON_3dVector y = pt - m_plane.origin;
//  MYON_3dVector x = m_plane.xaxis;
//  if (0.99998 > x * y) // x & v not parallel
//  {
//    MYON_3dVector z = MYON_CrossProduct(x, y);
//    if (z.Unitize())
//    {
//      y = MYON_CrossProduct(z, x);
//      if (y.Unitize())
//        m_plane.CreateFromFrame(m_plane.origin, x, y);
//    }
//  }
//  double u, v;
//  if (m_plane.ClosestPointTo(pt, &u, &v))
//    m_def_pt_2.Set(u, v);
//}
//
//void MYON_DimAngular::Set3dDimlinePoint(MYON_3dPoint pt)
//{
//  MYON_2dPoint p;
//  if (m_plane.ClosestPointTo(pt, &p.x, &p.y))
//    Set2dDimlinePoint(p);
//}

bool MYON_DimAngular::Get3dPoints(
  MYON_3dPoint* center,
  MYON_3dPoint* defpt1, MYON_3dPoint* defpt2,
  MYON_3dPoint* arrowpt1, MYON_3dPoint* arrowpt2,
  MYON_3dPoint* dimline, MYON_3dPoint* textpt) const
{
  bool rc = true;
  if (nullptr != center)
    *center = m_plane.origin;

  if (nullptr != defpt1)
  {
    MYON_2dPoint p1 = m_vec_1 * m_ext_offset_1;
    *defpt1 = m_plane.PointAt(p1.x, p1.y);
  }
  if (nullptr != defpt2)
  {
    MYON_2dPoint p2 = m_vec_2 * m_ext_offset_2;
    *defpt2 = m_plane.PointAt(p2.x, p2.y);
  }

  if (nullptr != dimline)
  {
    if(MYON_3dPoint::UnsetPoint != m_dimline_pt)
    *dimline = m_plane.PointAt(m_dimline_pt.x, m_dimline_pt.y);
    else
    {
      *dimline = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != arrowpt1)
  {
    MYON_2dPoint p = ArrowPoint1();
    if (MYON_2dPoint::UnsetPoint != p)
      *arrowpt1 = m_plane.PointAt(p.x, p.y);
    else
    {
      *arrowpt1 = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != arrowpt2)
  {
    MYON_2dPoint p = ArrowPoint2();
    if (MYON_2dPoint::UnsetPoint != p)
      *arrowpt2 = m_plane.PointAt(p.x, p.y);
    else
    {
      *arrowpt2 = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != textpt)
  {
    MYON_2dPoint textpt2d = MYON_2dPoint::UnsetPoint;
    if (m_use_default_text_point)
    {
      textpt2d = DefaultTextPoint();
    }
    else
    {
      if (MYON_3dPoint::UnsetPoint != m_user_text_point)
        textpt2d = m_user_text_point;
    }

    if (MYON_3dPoint::UnsetPoint != textpt2d)
      *textpt = m_plane.PointAt(textpt2d.x, textpt2d.y);
    else
    {
      *textpt = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }
  return rc;
}

bool MYON_DimAngular::GetDisplayLines(
  const MYON_Viewport* vp,
  const MYON_DimStyle* style,
  double dimscale,
  const MYON_3dPoint text_rect[4],
  MYON_Line lines[2],
  bool isline[2],
  MYON_Arc arcs[2],
  bool isarc[2],
  int maxlines,
  int maxarcs) const
{
  if (maxlines != 2 || maxarcs != 2)
  {
    MYON_ERROR("Wrong linecount calling MYON_DimAngular::GetDisplayLines.\n");
    return false;
  }
  if (nullptr == style)
    return false;

  isline[0] = isline[1] = isarc[0] = isarc[1] = false;
  double eo = style->ExtOffset() * dimscale;
  double ee = style->ExtExtension() * dimscale;
  double el = style->FixedExtensionLen() * dimscale;
  double radius = Radius();
  if (1.0e-8 > radius)
    return false;

  MYON_2dVector v0 = m_vec_1 * m_ext_offset_1;
  v0.Unitize();
  MYON_2dPoint arcpt_0 = m_vec_1 * radius;

  MYON_2dVector v1 = m_vec_2 * m_ext_offset_2;
  v1.Unitize();
  MYON_2dPoint arcpt_1 = m_vec_2 * radius;

  if (!style->SuppressExtension1())
  {
    double eo0 = eo;
    double ee0 = ee;
    double el0 = el;
    if (m_ext_offset_1 > radius)
    {
      eo0 = -eo0;
      ee0 = -ee0;
      el0 = -el0;
    }
    MYON_2dPoint lpt0, lpt1;
    if (style->FixedExtensionLenOn())
    {
      lpt1 = m_vec_1 * (radius + ee0);
      lpt0 = lpt1 + (-m_vec_1 * (el0 + ee0));
    }
    else
    {
      lpt0 = m_vec_1 * (m_ext_offset_1 + eo0);
      lpt1 = m_vec_1 * (radius + ee0);
    }
    lines[0].from = m_plane.PointAt(lpt0.x, lpt0.y);
    lines[0].to   = m_plane.PointAt(lpt1.x, lpt1.y);
    isline[0] = true;
  }

  if (!style->SuppressExtension2())
  {
    double eo1 = eo;
    double ee1 = ee;
    double el1 = el;
    if (m_ext_offset_2 > radius)
    {
      eo1 = -eo1;
      ee1 = -ee1;
      el1 = -el1;
    }
    MYON_2dPoint lpt0, lpt1;
    if (style->FixedExtensionLenOn())
    {
      lpt1 = m_vec_2 * (radius + ee1);
      lpt0 = lpt1 + (-m_vec_2 * (el1 + ee1));
    }
    else
    {
      lpt0 = m_vec_2 * (m_ext_offset_2 + eo1);
      lpt1 = m_vec_2 * (radius + ee1);
    }
    lines[1].from = m_plane.PointAt(lpt0.x, lpt0.y);
    lines[1].to   = m_plane.PointAt(lpt1.x, lpt1.y);
    isline[1] = true;
  }

  MYON_Circle c(MYON_xy_plane, radius);
  double a0 = 0.0, a1 = 0.0;
  if (c.ClosestPointTo(MYON_3dPoint(arcpt_0), &a0) &&
      c.ClosestPointTo(MYON_3dPoint(arcpt_1), &a1))
  {
    double dim_ext[2] = { style->DimExtension() * dimscale, style->DimExtension() * dimscale };
    if (fabs(dim_ext[0]) < MYON_ZERO_TOLERANCE && ArrowIsFlipped(0))
      dim_ext[0] = style->ArrowSize() * dimscale * 1.5;
    if (fabs(dim_ext[1]) < MYON_ZERO_TOLERANCE && ArrowIsFlipped(1))
      dim_ext[1] = style->ArrowSize() * dimscale * 1.5;

    double dim_ext_ang[2] = { 0.0, 0.0 };
    if (0.0 < dim_ext[0])
      dim_ext_ang[0] = dim_ext[0] / radius;
    if (0.0 < dim_ext[1])
      dim_ext_ang[1] = dim_ext[1] / radius;

    while (a0 + MYON_ZERO_TOLERANCE > MYON_PI * 2.0)
      a0 -= MYON_PI * 2.0;
    a0 -= dim_ext_ang[0];
    a1 += dim_ext_ang[1];
    if (arcs[0].Create(c, MYON_Interval(a0, a1)))
    {
      MYON_Xform xf;
      xf.Rotation(MYON_xy_plane, m_plane);
      arcs[0].Transform(xf);
      isarc[0] = true;
    }
  }

  // This part clips dimarc to textbox except when text is above line
  //MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode text_mode = style->TextAlignment();
  const MYON_DimStyle::TextLocation text_location = style->DimTextLocation();
  const MYON::TextOrientation text_orientation = style->DimTextOrientation();
  const MYON_DimStyle::ContentAngleStyle text_angle_style = style->DimTextAngleStyle();

  //if (MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine != text_mode)
  if (
    MYON_DimStyle::TextLocation::InDimLine == text_location
    || MYON::TextOrientation::InView == text_orientation
    || MYON_DimStyle::ContentAngleStyle::Aligned != text_angle_style
    )
  {
    if (text_rect[0].DistanceTo(text_rect[2]) > MYON_SQRT_EPSILON)
    {
      MYON_Arc dimarc(arcs[0]);
      MYON_Arc arc_segs[2];

      if (dimarc.IsValid())
      {
        int seg_count = ClipArcToTextRect(vp, dimarc, text_rect, arc_segs);
        if (0 == seg_count)
          isarc[0] = false;
        else
        {
          if (seg_count > 0)
            arcs[0] = arc_segs[0];
          if (seg_count > 1)
          {
            arcs[1] = arc_segs[1];
            isarc[1] = true;
          }
        }
      }
    }
  }

  return true;
}

void MYON_DimAngular::GetArrowXform(
  int which_end,
  double arrowlength,
  bool arrowflipped,
  bool from_the_back,
  MYON_Xform& arrow_xform_out) const
{
  MYON_Xform xf(1.0), xfp, xfs, xfr;
  if (0 != which_end)
    which_end = 1;
  MYON_2dPoint ap = (0 == which_end) ? ArrowPoint1() : ArrowPoint2();
  xfp.Rotation(MYON_xy_plane, Plane());
  MYON_Xform xft = MYON_Xform::TranslationTransformation(ap.x, ap.y, 0.0);
  double rotang = MYON_PI / 2.0;
  if (1 == which_end)
  {
    MYON_2dVector v = ap;
    v.Unitize();
    rotang = atan2(v.y, v.x);
    rotang += MYON_PI / 2.0;
  }
  if (from_the_back)
    rotang += MYON_PI;
  if (arrowflipped != (which_end == 0))
    rotang += MYON_PI;
  while (rotang >= (2.0 * MYON_PI))
    rotang -= (2.0 * MYON_PI);
  while (rotang < 0.0)
    rotang += (2.0 * MYON_PI);

  // little adjustment so dimension arc leaves arrow in the middle
  double f = (arrowlength * 0.5) / Radius();
  if (f > 1.0) f = 1.0;
  double adjust = asin(f);
  if (which_end == 1)
    adjust = -adjust;
  if (ArrowIsFlipped(which_end))
    adjust = -adjust;

  xfr.Rotation(rotang+adjust, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);
  xf = xft * xfr;
  xf = xfp * xf;
  xfs = MYON_Xform::DiagonalTransformation(from_the_back ? -arrowlength : arrowlength, arrowlength, arrowlength);
  arrow_xform_out = xf * xfs;
}

bool MYON_DimAngular::UpdateDimensionText(
  MYON::LengthUnitSystem units,
  const MYON_DimStyle* dimstyle) const
{
  return false;
}

bool MYON_DimAngular::GetDistanceDisplayText(
  MYON::LengthUnitSystem units,
  const MYON_DimStyle* dimstyle,
  MYON_wString& displaytext) const
{
  return false;
}


//----------------------------------------------------------
// Class MYON_DimRadial

MYON_DimRadial::MYON_DimRadial()
  : MYON_Dimension(MYON::AnnotationType::Radius)
{}

bool MYON_DimRadial::IsValidRadialDimensionType(
  MYON::AnnotationType annotation_type
)
{
  return (
    MYON::AnnotationType::Radius == annotation_type
    || MYON::AnnotationType::Diameter == annotation_type
    );
}

bool MYON_DimRadial::SetRadialDimensionType(
  MYON::AnnotationType radial_dimension_type
)
{
  if ( false == MYON_DimRadial::IsValidRadialDimensionType(radial_dimension_type) )
  {
    MYON_ERROR("Invalid radial_dimension_type parameter.");
    return false;
  }

  m_annotation_type = radial_dimension_type;
  MYON_wString usertext
    = (MYON::AnnotationType::Diameter == m_annotation_type)
    ? MYON_wString::DiameterSymbol
    : MYON_wString::RadiusSymbol;

  usertext += "<>";

  SetUserText(usertext);

  return true;
}




bool MYON_DimRadial::Write(
  MYON_BinaryArchive& archive
  ) const
{
  const int content_version = 0;
  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!MYON_Dimension::Internal_WriteDimension(archive))
      break;
    if (!archive.WritePoint(m_radius_pt))
      break;
    if (!archive.WritePoint(m_dimline_pt))
      break;
    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimRadial::Read(
  MYON_BinaryArchive& archive
  )
{
  *this = MYON_DimRadial::Empty;

  int content_version = -1;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (content_version < 0)
      break;
    if (!MYON_Dimension::Internal_ReadDimension(archive))
      break;
    if (!archive.ReadPoint(m_radius_pt))
      break;
    if (!archive.ReadPoint(m_dimline_pt))
      break;
    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimRadial::Transform(const MYON_Xform& xform)
{
  bool rc = xform.IsIdentity();
  if (!rc)
  {
    rc = true;
    bool scaling = false;
    MYON_3dVector v = m_plane.xaxis;
    v.Transform(xform);
    if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
      scaling = true;
    else
    {
      v = m_plane.yaxis;
      v.Transform(xform);
      if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
        scaling = true;
      else
      {
        v = m_plane.zaxis;
        v.Transform(xform);
        if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
          scaling = true;
      }
    }

    if (scaling)
    {
      MYON_3dPoint radius_pt_0(MYON_3dPoint::UnsetPoint);
      MYON_3dPoint dimline_pt_0(MYON_3dPoint::UnsetPoint);
      if (Get3dPoints(nullptr, &radius_pt_0, &dimline_pt_0, nullptr))
      {
        MYON_2dPoint radius_pt(MYON_2dPoint::NanPoint), dimline_pt(MYON_2dPoint::NanPoint);
        rc = m_plane.Transform(xform);
        radius_pt_0.Transform(xform);
        dimline_pt_0.Transform(xform);
        if (rc && !m_plane.ClosestPointTo(radius_pt_0, &radius_pt.x, &radius_pt.y))
          rc = false;
        else if (rc && !m_plane.ClosestPointTo(dimline_pt_0, &dimline_pt.x, &dimline_pt.y))
          rc = false;
        if (rc)
        {
          Set2dRadiusPoint(radius_pt);
          Set2dDimlinePoint(dimline_pt);
        }
      }
    }
    else
      rc = m_plane.Transform(xform);

    if (rc)
      MYON_Geometry::Transform(xform);
  }
  return rc;
}

bool MYON_DimRadial::Create(
  MYON::AnnotationType radial_dimension_type,
  const MYON_UUID style_id,
  const MYON_Plane& plane,
  const MYON_3dPoint& center_pt,
  const MYON_3dPoint& radius_pt,
  const MYON_3dPoint& dimline_pt)
{
  m_dimstyle_id = style_id;
  if (MYON_nil_uuid == m_dimstyle_id)
    return true; // .NET dialog hack

  if (false == MYON_DimRadial::IsValidRadialDimensionType(radial_dimension_type))
  {
    MYON_ERROR("Invalid radial_dimension_type parameter.");
    return false;
  }

  if (!plane.IsValid() || !center_pt.IsValid() || !center_pt.IsValid() || !radius_pt.IsValid() || !dimline_pt.IsValid())
    return false;

  bool rc = SetRadialDimensionType(radial_dimension_type);
  m_plane = plane;

  if (rc)
  {
    double x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
    MYON_3dPoint radius_pton;
    MYON_3dPoint dimline_pton;
    MYON_3dVector v1;
    MYON_3dVector v2;

    m_plane.origin = plane.ClosestPointTo(center_pt);
    rc = m_plane.ClosestPointTo(radius_pt, &x1, &y1);
    if (rc)
    {
      rc = m_plane.ClosestPointTo(dimline_pt, &x2, &y2);
      if (rc)
      {
        radius_pton = m_plane.PointAt(x1, y1);
        dimline_pton = m_plane.PointAt(x2, y2);
        v1 = radius_pton - m_plane.origin;
        v2 = dimline_pton - m_plane.origin;
        rc = v1.Unitize() && v2.Unitize();
      }
    }
    if (rc)
    {
      m_radius_pt.Set(x1, y1);
      m_dimline_pt.Set(x2, y2);
    }
  }
  return rc;
}

bool MYON_DimRadial::AdjustFromPoints(
  const MYON_Plane& plane,
  const MYON_3dPoint& center_pt,
  const MYON_3dPoint& radius_pt,
  const MYON_3dPoint& dimline_pt
  )
{
  MYON_2dPoint center_pt2d(MYON_2dPoint::Origin), radius_pt2d(MYON_2dPoint::Origin), dimline_pt2d(MYON_2dPoint::Origin);
  m_plane = plane;
  m_plane.origin = plane.ClosestPointTo(center_pt);
  //if (!plane.ClosestPointTo(center_pt, &c_pt.x, &c_pt.y))
  //  return false;
  if (!plane.ClosestPointTo(radius_pt, &radius_pt2d.x, &radius_pt2d.y))
    return false;
  if (!plane.ClosestPointTo(dimline_pt, &dimline_pt2d.x, &dimline_pt2d.y))
    return false;

  MYON_2dVector arrow_dir = radius_pt2d - center_pt2d;
  MYON_2dVector drag_dir = dimline_pt2d - center_pt2d;
  double drag_dist = drag_dir.Length();
  if (!arrow_dir.Unitize() || !drag_dir.Unitize())
    return false;

  if (fabs(center_pt2d.y - radius_pt2d.y) < MYON_SQRT_EPSILON)
  {
    // radial line is horizontal - skip intersecting with dimline
    dimline_pt2d = arrow_dir * drag_dist;
  }

  //m_plane = plane;
  m_radius_pt.Set(radius_pt2d.x, radius_pt2d.y);
  m_dimline_pt.Set(dimline_pt2d.x, dimline_pt2d.y);
  
  return true;
}

bool MYON_DimRadial::GetBBox(double* bmin, double* bmax, bool grow) const
{
  const MYON_DimStyle* dimstyle = nullptr;
  return GetAnnotationBoundingBox(nullptr, dimstyle, 1.0, bmin, bmax, grow ? true : false);
}

bool MYON_DimRadial::GetAnnotationBoundingBox(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  double* boxmin,
  double* boxmax,
  bool bGrow
) const
{
  if (nullptr == dimstyle)
    dimstyle = &MYON_DimStyle::Default;

  const MYON_2dPoint hash_points[] = {
    MYON_2dPoint(m_radius_pt),
    MYON_2dPoint(m_dimline_pt)
  };

  const MYON_SHA1_Hash hash = Internal_GetBBox_InputHash(
    vp, 
    dimstyle, 
    dimscale, 
    m_user_text_point,
    (unsigned int)(sizeof(hash_points)/sizeof(hash_points[0])),
    hash_points
  );

  if (Internal_GetBBox_Begin(hash, boxmin, boxmax, bGrow))
    return true;

  if (nullptr == boxmin || nullptr == boxmax)
    return false;

  MYON_Xform text_xform;
  GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform);

  MYON_BoundingBox dim_box;

  const MYON_TextContent* text = Text();
  MYON_3dPoint text_rect[4] = { MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin };

  dim_box.Destroy();

  if (nullptr != text && text->GetTightBoundingBox(dim_box))
  {
    text_rect[0].Set(dim_box.m_min.x, dim_box.m_min.y, 0.0);
    text_rect[1].Set(dim_box.m_max.x, dim_box.m_min.y, 0.0);
    text_rect[2].Set(dim_box.m_max.x, dim_box.m_max.y, 0.0);
    text_rect[3].Set(dim_box.m_min.x, dim_box.m_max.y, 0.0);
    for (int i = 0; i < 4; i++)
      text_rect[i].Transform(text_xform);  // Text + gap bounding rect

    for (int i = 0; i < 4; i++)
    {
      dim_box.Set(text_rect[i], 0 < i ? true : false);
    }
  }

#define dimlinecount 9
  bool dimlines[dimlinecount] = { false, false, false, false, false, false, false, false, false };
  MYON_Line lines[dimlinecount];
  if (GetDisplayLines(dimstyle, dimscale, text_rect, lines, dimlines, dimlinecount))
  {
    for (int i = 0; i < dimlinecount; i++)
    {
      if (dimlines[i])
      {
        dim_box.Set(lines[i].from, true);
        dim_box.Set(lines[i].to, true);
      }
    }
  }
#undef dimlinecount

  return Internal_GetBBox_End(dim_box, hash, boxmin, boxmax, bGrow);
}

bool MYON_DimRadial::GetTextXform(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  return GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform_out);
}

bool MYON_DimRadial::GetTextXform(
  const MYON_Xform* model_xform,
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;
  if (nullptr == dimstyle)
    return false;

  // See if the text needs remade because of some change in some property that
  // would change its appearance
  if (DimStyleTextPositionPropertiesHash() != dimstyle->TextPositionPropertiesHash())
  {
    MYON_wString rtfstr = text->RtfText();
    MYON::AnnotationType annotation_type = this->Type();
    bool wrapped = text->TextIsWrapped();
    double width = text->FormattingRectangleWidth();
    double rot = text->TextRotationRadians();
    const_cast<MYON_TextContent*>(text)->Create(rtfstr, annotation_type, dimstyle, wrapped, width, rot);
  }

  MYON_3dPoint text_center = MYON_3dPoint::Origin;

  MYON_3dPoint cp[4];
  if (!text->Get3dCorners(cp))
    return false;

  double text_width = 0.0;
  double text_height = 0.0;
  double line_height = 0.0;
  double text_gap = 0.0;
  double landing_length = 0.0;

  const MYON::TextOrientation text_orientation = dimstyle->DimRadialTextOrientation();
  const MYON_DimStyle::ContentAngleStyle text_alignment = dimstyle->DimRadialTextAngleStyle();
  const MYON::TextHorizontalAlignment halign = dimstyle->LeaderTextHorizontalAlignment();
  //const MYON::TextVerticalAlignment valign = dimstyle->LeaderTextVerticalAlignment();

  // Always move text to InLine if it's going to be horizontal and in the view plane
  const MYON_DimStyle::TextLocation text_location =   
    (MYON::TextOrientation::InView == text_orientation)
    ? MYON_DimStyle::TextLocation::InDimLine
    : dimstyle->DimRadialTextLocation();

  const MYON_Plane& textplane = MYON_xy_plane;
  bool draw_forward = dimstyle->DrawForward();

  MYON_Xform dimplane_xf(1.0);

  dimplane_xf.Rotation(textplane, Plane());     // Rotate text from world xy to dimension plane

  MYON_Xform textpt_xf(1.0);           // Dimension plane to text point translation
  MYON_Xform textrot_xf(1.0);          // Text rotation around text plane origin point
  MYON_Xform textscale_xf(1.0);
  
  text_center = (cp[0] * dimscale + cp[2] * dimscale) / 2.0;
  text_width = (cp[1].x - cp[0].x) * dimscale;
  text_height = (cp[3].y - cp[0].y) * dimscale;
  line_height = dimstyle->TextHeight() * dimscale;
  text_gap = dimstyle->TextGap();
  if (dimstyle->MaskFrameType() != MYON_TextMask::MaskFrame::NoFrame)
    text_gap = dimstyle->TextMask().MaskBorder();
  text_gap *= dimscale;

  landing_length = dimstyle->LeaderLandingLength() * dimscale;

  MYON_2dPoint dimline_pt = DimlinePoint();
  MYON_2dPoint radius_pt = RadiusPoint();
  MYON_2dPoint center_pt(0.0, 0.0);
  MYON_2dPoint kink_pt(MYON_2dPoint::UnsetPoint);
  MYON_2dVector radius_vector = radius_pt;
  if (!radius_vector.Unitize())
    return false;
  if (fabs(dimline_pt.x) < MYON_SQRT_EPSILON)
    dimline_pt.x = 0.0;
  if (fabs(dimline_pt.y) < MYON_SQRT_EPSILON)
    dimline_pt.y = 0.0;
  if (fabs(radius_pt.x) < MYON_SQRT_EPSILON)
    radius_pt.x = 0.0;
  if (fabs(radius_pt.y) < MYON_SQRT_EPSILON)
    radius_pt.y = 0.0;
  MYON_2dVector tail_dir(1.0, 0.0);

  kink_pt = KneePoint();

  MYON_3dVector dim_xdir = Plane().xaxis;
  MYON_3dVector dim_ydir = Plane().yaxis;
  if (nullptr != model_xform)
  {
    dim_xdir.Transform(*model_xform);
    dim_ydir.Transform(*model_xform);
  }

  MYON_3dVector view_x = MYON_3dVector::XAxis;
  MYON_3dVector view_y = MYON_3dVector::YAxis;
  MYON_3dVector view_z = MYON_3dVector::ZAxis;
  if (nullptr != vp)
  {
    view_x = vp->CameraX();
    view_y = vp->CameraY();
    view_z = vp->CameraZ();
  }

  // Text is horizontal in CPlane, not view
  if (MYON_DimStyle::ContentAngleStyle::Horizontal == text_alignment &&
      MYON_2dPoint::UnsetPoint != kink_pt)
  {
    if (fabs(dimline_pt.x - kink_pt.x) < MYON_SQRT_EPSILON)
    {
      // kink at dimlinept
      if (dimline_pt.x - radius_pt.x > -MYON_SQRT_EPSILON)
        tail_dir.Set(1.0, 0.0);
      else
        tail_dir.Set(-1.0, 0.0);
    }
    else
    {
      if (dimline_pt.x - kink_pt.x > -MYON_SQRT_EPSILON)
        tail_dir.Set(1.0, 0.0);
      else
        tail_dir.Set(-1.0, 0.0);
    }
  }
  // Text is aligned with last leader segment
  else if (MYON_DimStyle::ContentAngleStyle::Aligned == text_alignment) // && no kink point
  {
    double d = ((MYON_2dVector)dimline_pt).Length();
    if (((MYON_2dVector)dimline_pt) * ((MYON_2dVector)radius_pt) < 0.0)
      d = -d;  // text point is on the other side of center from arrow point
    dimline_pt = radius_vector * d;  // With no kink, adjust dimline point to line up with radius point
    tail_dir = dimline_pt - radius_pt;
    if (MYON_SQRT_EPSILON > tail_dir.Length() || !tail_dir.Unitize())
      tail_dir = radius_pt - center_pt;
  }
  if (dimline_pt.DistanceTo(DimlinePoint()) > MYON_SQRT_EPSILON)
    const_cast<MYON_DimRadial*>(this)->Set2dDimlinePoint(dimline_pt);

  if (!tail_dir.Unitize())
    return false;

  // Text position adjustment
  MYON_2dVector shift(0.0, 0.0);
  if (MYON_DimStyle::TextLocation::AboveDimLine == text_location)
    shift.y = text_gap;
  if (MYON_DimStyle::TextLocation::InDimLine == text_location)
    shift.y = -line_height/2.0;

  shift.x = text_gap;
  shift.x += landing_length;

  if (-MYON_SQRT_EPSILON > tail_dir.x)  // text to left
  {
    switch (halign)
    {
    default:
    case MYON::TextHorizontalAlignment::Left:
      shift.x += text_width;
      break;
    case MYON::TextHorizontalAlignment::Right:
      break;
    case MYON::TextHorizontalAlignment::Center:
      shift.x += text_width / 2.0;
      break;
    }
    shift.y = -shift.y;
  }
  else
  {
    switch (halign)
    {
    default:
    case MYON::TextHorizontalAlignment::Left:
      break;
    case MYON::TextHorizontalAlignment::Right:
      shift.x += text_width;
      break;
    case MYON::TextHorizontalAlignment::Center:
      shift.x += text_width / 2.0;
      break;
    }
  }

  if (dim_ydir * view_y < 0.0)
    shift.y = -shift.y;

  shift.Rotate(tail_dir.y, tail_dir.x);

  textpt_xf = MYON_Xform::TranslationTransformation( MYON_3dVector(dimline_pt + shift) );
  if (-MYON_SQRT_EPSILON > tail_dir.x)  // text to left
    textrot_xf.Rotation(-tail_dir.y, -tail_dir.x, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);
  else
    textrot_xf.Rotation(tail_dir.y, tail_dir.x, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);

  text_xform_out = MYON_Xform::DiagonalTransformation(dimscale, dimscale, dimscale);

  // Text is horizontal to view
  if (MYON::TextOrientation::InView != text_orientation)
    text_xform_out = textrot_xf * text_xform_out;
  text_xform_out = textpt_xf * text_xform_out;
  text_xform_out = dimplane_xf * text_xform_out;

  MYON_3dPoint text_point_3d = Plane().PointAt(dimline_pt.x + shift.x, dimline_pt.y + shift.y);

  // Text is horizontal to view
  if (MYON::TextOrientation::InView == text_orientation)
  {
    MYON_Xform tp2sxf;        // Text point to view plane rotation
    tp2sxf.Rotation(text_point_3d, Plane().xaxis, Plane().yaxis, Plane().zaxis, text_point_3d, view_x, view_y, view_z);
    text_xform_out = tp2sxf * text_xform_out;
  }
  else
    if (draw_forward)
  {
    // Check if the text is right-reading
    MYON_3dVector text_right_dir(1.0, 0.0, 0.0);
    text_right_dir.Transform(text_xform_out);
    if(nullptr != model_xform)
      text_right_dir.Transform(*model_xform);
    if (text_right_dir.Unitize())
    {
      MYON_3dVector zdir = MYON_CrossProduct(dim_xdir, dim_ydir);
      MYON_3dVector text_up_dir = MYON_CrossProduct(zdir, text_right_dir);
      bool fx = (0.0 > view_x * text_right_dir);
      bool fy = (0.0 > view_y * text_up_dir);
      if (fx || fy)
      {
        MYON_Xform mxf;  // Mirror xform for backwards text
        if (fx)
        {
          mxf.Mirror(text_center, textplane.xaxis);
          textpt_xf = textpt_xf * mxf;
        }
        if (fy)
        {
          mxf.Mirror(MYON_3dPoint::Origin, textplane.yaxis);
          textpt_xf = textpt_xf * mxf;
        }
        text_xform_out = MYON_Xform::DiagonalTransformation(dimscale, dimscale, dimscale);
        text_xform_out = textrot_xf * text_xform_out;
        text_xform_out = textpt_xf * text_xform_out;
        text_xform_out = dimplane_xf * text_xform_out;
      }
    }
  }
  return true;
}

double MYON_DimRadial::Measurement() const
{
  double l = 0.0;
  if (m_radius_pt.IsValid())
  {
    l = ((MYON_2dVector)m_radius_pt).Length();
    if ( MYON::AnnotationType::Diameter == Type() )
      l *= 2.0;
    if (DistanceScale() != 1.0)
      l *= DistanceScale();
  }
  return l;
}

MYON_2dPoint MYON_DimRadial::DefaultTextPoint() const
{
  return m_dimline_pt;
}

MYON_2dPoint MYON_DimRadial::CenterPoint() const
{
  return MYON_2dPoint::Origin;
}

MYON_2dPoint MYON_DimRadial::RadiusPoint() const
{
  return m_radius_pt;
}

MYON_2dPoint MYON_DimRadial::DimlinePoint() const
{
  return m_dimline_pt;
}

MYON_2dPoint MYON_DimRadial::KneePoint() const
{
  MYON_2dPoint kpt = MYON_2dPoint::UnsetPoint;
  if (MYON_2dPoint::UnsetPoint != m_radius_pt && MYON_2dPoint::UnsetPoint != m_dimline_pt)
  {
    kpt.Set(m_radius_pt.x, m_dimline_pt.y);

    if (MYON_SQRT_EPSILON > fabs(m_radius_pt.x) || 0.01 < fabs(m_radius_pt.y / m_radius_pt.x))      // radial isn't horizontal
    {
      double x = m_radius_pt.x;
      if (m_radius_pt.y != 0.0)
        x = m_radius_pt.x * m_dimline_pt.y / m_radius_pt.y;
      kpt.Set(x, m_dimline_pt.y);
    }
  }
  return kpt;
}

void MYON_DimRadial::Set2dCenterPoint(MYON_2dPoint pt)
{
  // Move plane origin to pt
  if (pt.IsValid())
  {
    MYON_2dVector v(-pt.x, -pt.y);
    m_plane.origin = m_plane.PointAt(pt.x, pt.y);
    m_radius_pt = m_radius_pt + v;
    m_dimline_pt = m_dimline_pt + v;
  }
}

void MYON_DimRadial::Set2dRadiusPoint(MYON_2dPoint pt)
{
  if (pt.IsValid())
  {
    m_radius_pt = pt;
  }
}

void MYON_DimRadial::Set2dDimlinePoint(MYON_2dPoint pt)
{
  if (pt.IsValid())
  {
    m_dimline_pt = pt;
  }
}

void MYON_DimRadial::Set3dCenterPoint(MYON_3dPoint pt)
{
  // This moves the whole dimension
  if (pt.IsValid())
  {
    m_plane.origin = pt;
  }
}

void MYON_DimRadial::Set3dRadiusPoint(MYON_3dPoint pt)
{
  if (pt.IsValid())
  {
    MYON_2dPoint p;
    if (m_plane.ClosestPointTo(pt, &p.x, &p.y))
      Set2dRadiusPoint(p);
  }
}

void MYON_DimRadial::Set3dDimlinePoint(MYON_3dPoint pt)
{
  if (pt.IsValid())
  {
    MYON_2dPoint p;
    if (m_plane.ClosestPointTo(pt, &p.x, &p.y))
      Set2dDimlinePoint(p);
  }
}

bool MYON_DimRadial::Get3dPoints(
  MYON_3dPoint* center_pt,
  MYON_3dPoint* radius_pt,
  MYON_3dPoint* dimline_pt,
  MYON_3dPoint* knee_pt) const
{
  bool rc = true;
  if (nullptr != center_pt)
    *center_pt = m_plane.origin;

  if (nullptr != radius_pt)
  {
    if (MYON_2dPoint::UnsetPoint != m_radius_pt)
      *radius_pt = m_plane.PointAt(m_radius_pt.x, m_radius_pt.y);
    else
    {
      *radius_pt = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != dimline_pt)
  {
    if (MYON_2dPoint::UnsetPoint != m_dimline_pt)
      *dimline_pt = m_plane.PointAt(m_dimline_pt.x, m_dimline_pt.y);
    else
    {
      *dimline_pt = MYON_3dPoint::UnsetPoint;
      rc = false;
    }
  }

  if (nullptr != knee_pt)
  {
    *knee_pt = MYON_3dPoint::UnsetPoint;
    MYON_2dPoint kpt = KneePoint();
    if (MYON_2dPoint::UnsetPoint != kpt)
      *knee_pt = m_plane.PointAt(kpt.x, kpt.y);
    else
      rc = false;
  }
  return rc;
}

bool MYON_DimRadial::GetDisplayLines(
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_3dPoint text_rect[4],
  MYON_Line lines[9],
  bool isline[9],
  int maxlines) const
{
  if (maxlines != 9)
    return false;
  if (nullptr == dimstyle)
    return false;

  isline[0] = isline[1] = isline[2] = isline[3] = 
    isline[4] = isline[5] = isline[6] = isline[7] = isline[8] = false;

  MYON_2dPoint center = CenterPoint();
  const MYON_Plane& plane = Plane();
  MYON_3dVector landing_dir(plane.xaxis);

  MYON_DimStyle::ContentAngleStyle alignment = dimstyle->DimRadialTextAngleStyle();

  MYON_2dPoint centerpt2d = CenterPoint();
  MYON_2dPoint radiuspt2d = RadiusPoint();
  MYON_2dPoint kneept2d = KneePoint();
  MYON_2dPoint dimlinept2d = DimlinePoint();
  MYON_2dVector taildir2d(1.0, 0.0);


  if (MYON_DimStyle::ContentAngleStyle::Horizontal == alignment &&
    MYON_2dPoint::UnsetPoint != kneept2d)
  {
    if (fabs(dimlinept2d.x - kneept2d.x) < MYON_SQRT_EPSILON)
    {
      // kink at dimlinept
      if (dimlinept2d.x - radiuspt2d.x > -MYON_SQRT_EPSILON)
        taildir2d.Set(1.0, 0.0);
      else
        taildir2d.Set(-1.0, 0.0);
    }
    else
    {
      if (dimlinept2d.x - kneept2d.x > -MYON_SQRT_EPSILON)
        taildir2d.Set(1.0, 0.0);
      else
        taildir2d.Set(-1.0, 0.0);
    }

    lines[0].from = plane.PointAt(radiuspt2d.x, radiuspt2d.y);
    lines[0].to = plane.PointAt(kneept2d.x, kneept2d.y);
    isline[0] = lines[0].Length() > MYON_SQRT_EPSILON;
    lines[1].from = plane.PointAt(kneept2d.x, kneept2d.y);
    lines[1].to = plane.PointAt(dimlinept2d.x, dimlinept2d.y);
    isline[1] = lines[1].Length() > MYON_SQRT_EPSILON;
  }
  else if (MYON_DimStyle::ContentAngleStyle::Aligned == alignment) // && no kink point
  {
    double d = ((MYON_2dVector)dimlinept2d).Length();
    MYON_2dVector rv = radiuspt2d;
    if (((MYON_2dVector)dimlinept2d) * ((MYON_2dVector)radiuspt2d) < 0.0)
      d = -d;
    if(rv.Unitize())
      dimlinept2d = rv * d;  // With no kink, adjust dimline point to line up with radius point
    taildir2d = dimlinept2d - radiuspt2d;
    if (MYON_SQRT_EPSILON > taildir2d.Length() || !taildir2d.Unitize())
      taildir2d = radiuspt2d - centerpt2d;
    lines[0].from = plane.PointAt(radiuspt2d.x, radiuspt2d.y);
    lines[0].to = plane.PointAt(dimlinept2d.x, dimlinept2d.y);
    isline[0] = lines[0].Length() > MYON_SQRT_EPSILON;
  }

  MYON_3dPoint p = plane.PointAt(taildir2d.x, taildir2d.y);
  landing_dir = p - plane.origin;

  if (landing_dir.Unitize())
  {
    double landinglength = dimstyle->LeaderLandingLength() * dimscale;
    //MYON_3dPoint kink_point = plane.PointAt(kneept2d.x, kneept2d.y);
    //MYON_3dPoint dimline_point = plane.PointAt(dimlinept2d.x, dimlinept2d.y);

    const MYON_DimStyle::TextLocation text_location = dimstyle->DimRadialTextLocation();

    if(MYON_DimStyle::TextLocation::AboveDimLine == text_location && MYON::TextOrientation::InView != dimstyle->DimRadialTextOrientation())
      landinglength += text_rect[1].DistanceTo(text_rect[0]);  // Add text width to draw under the text

    if (0.0 < landinglength)
    {
      lines[2].from = plane.PointAt(dimlinept2d.x, dimlinept2d.y);
      lines[2].to = lines[2].from + (landing_dir * landinglength);
      isline[2] = true;
    }
  }

  double centermarksize = dimstyle->CenterMark() * dimscale;
  MYON_DimStyle::centermark_style centermarkstyle = dimstyle->CenterMarkStyle();
  if (MYON_DimStyle::centermark_style::None != centermarkstyle && MYON_SQRT_EPSILON < centermarksize)
  {
    double radius = centerpt2d.DistanceTo(radiuspt2d);
    return MYON_Dimension::GetCentermarkDisplay(plane, center, centermarksize, radius, centermarkstyle, &lines[3], &isline[3], 6);
  }
  else
    return true;
}

void MYON_DimRadial::GetArrowXform(
  double scale,
  MYON_Xform& arrow_xform_out) const
{
  MYON_Xform arrow_xf, xfs, xfr;
  MYON_2dPoint ap = RadiusPoint();
  arrow_xf.Rotation(MYON_xy_plane, Plane());
  MYON_Xform xft = MYON_Xform::TranslationTransformation(ap.x, ap.y, 0.0);
  arrow_xf = arrow_xf * xft;
  double rotang = MYON_PI;

  double ra = 0.0;
  MYON_2dPoint rp = RadiusPoint();
  MYON_2dPoint kp = KneePoint();
  MYON_2dVector kv = kp - rp;
  if (((MYON_2dVector)rp).Unitize())
  {
    ra = atan2(rp.y, rp.x);
    if (kv.Unitize())
    {
      if (0.0 > kv * ((MYON_2dVector)rp))
        ra += MYON_PI;
    }
    rotang += ra;
  }
  double pi2 = MYON_PI * 2.0;
  while (pi2 <= rotang)
    rotang -= pi2;
  while (0 > rotang)
    rotang += pi2;

  if (MYON_ZERO_TOLERANCE > fabs(rotang))
    rotang = 0.0;
  if (0.0 != rotang)
  {
    xfr.Rotation(rotang, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);
    arrow_xf = arrow_xf * xfr;
  }
  xfs = MYON_Xform::DiagonalTransformation(scale, scale, scale);
  arrow_xform_out = arrow_xf * xfs;
}


//----------------------------------------------------------
// Class MYON_DimOrdinate

MYON_DimOrdinate::MYON_DimOrdinate()
  : MYON_Dimension(MYON::AnnotationType::Ordinate)
{}

MYON_DimOrdinate::MeasuredDirection MYON_DimOrdinate::MeasuredDirectionFromUnsigned(
  unsigned int measured_direction_as_unsigned
)
{
  switch (measured_direction_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimOrdinate::MeasuredDirection::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimOrdinate::MeasuredDirection::Xaxis);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimOrdinate::MeasuredDirection::Yaxis);
  }

  MYON_ERROR("Invalid measured_direction_as_unsigned value.");

  return MYON_DimOrdinate::Empty.m_direction;
}

bool MYON_DimOrdinate::Write(
  MYON_BinaryArchive& archive
  ) const
{
  const int content_version = 0;
  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!MYON_Dimension::Internal_WriteDimension(archive))
      break;
    const unsigned int u = static_cast<unsigned char>(m_direction);
    if (!archive.WriteInt(u))
      break;
    if (!archive.WritePoint(m_def_pt))
      break;
    if (!archive.WritePoint(m_ldr_pt))
      break;
    if (!archive.WriteDouble(m_kink_offset_1))
      break;
    if (!archive.WriteDouble(m_kink_offset_2))
      break;
    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimOrdinate::Read(
  MYON_BinaryArchive& archive
  )
{
  *this = MYON_DimOrdinate::Empty;

  int content_version = -1;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (content_version < 0)
      break;
    if (!MYON_Dimension::Internal_ReadDimension(archive))
      break;

    unsigned int u = static_cast<unsigned char>(m_direction);
    if (!archive.ReadInt(&u))
      break;
    m_direction = MYON_DimOrdinate::MeasuredDirectionFromUnsigned(u);

    if (!archive.ReadPoint(m_def_pt))
      break;
    if (!archive.ReadPoint(m_ldr_pt))
      break;
    if (!archive.ReadDouble(&m_kink_offset_1))
      break;
    if (!archive.ReadDouble(&m_kink_offset_2))
      break;
    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimOrdinate::Transform(const MYON_Xform& xform)
{
  bool rc = xform.IsIdentity();
  double xscale = 1.0, yscale = 1.0;
  if (!rc)
  {
    rc = true;
    bool scaling = false;
    MYON_3dVector v = m_plane.xaxis;
    v.Transform(xform);
    if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
    {
      scaling = true;
      xscale = v.Length();
    }
    else
    {
      v = m_plane.yaxis;
      v.Transform(xform);
      if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
      {
        scaling = true;
        yscale = v.Length();
      }
      else
      {
        v = m_plane.zaxis;
        v.Transform(xform);
        if (fabs(1.0 - v.Length()) > MYON_SQRT_EPSILON)
          scaling = true;
      }
    }

    if (scaling)
    {
      MYON_3dPoint base_pt(MYON_3dPoint::UnsetPoint);
      MYON_3dPoint def_pt(MYON_3dPoint::UnsetPoint);
      MYON_3dPoint ldr_pt(MYON_3dPoint::UnsetPoint);
      MYON_3dPoint kink_pt1(MYON_3dPoint::UnsetPoint);
      MYON_3dPoint kink_pt2(MYON_3dPoint::UnsetPoint);
      Get3dPoints(&base_pt, &def_pt, &ldr_pt, &kink_pt1, &kink_pt2);
      rc = m_plane.Transform(xform);
      def_pt.Transform(xform);
      ldr_pt.Transform(xform);
      MYON_2dPoint def_pt_2d(MYON_2dPoint::NanPoint), ldr_pt_2d(MYON_2dPoint::NanPoint);
      if (rc && !m_plane.ClosestPointTo(def_pt, &def_pt_2d.x, &def_pt_2d.y))
        rc = false;
      else if (rc && !m_plane.ClosestPointTo(ldr_pt, &ldr_pt_2d.x, &ldr_pt_2d.y))
        rc = false;
      if (rc)
      {
        if (MeasuredDirection::Xaxis == GetMeasuredDirection())
        {
          if (MYON_SQRT_EPSILON > fabs(def_pt_2d.x - ldr_pt_2d.x))
            ldr_pt_2d.x = def_pt_2d.x;
          else if (1.0 != yscale)
          {
            if (MYON_UNSET_VALUE != m_kink_offset_1)
              m_kink_offset_1 *= yscale;
            if (MYON_UNSET_VALUE != m_kink_offset_2)
              m_kink_offset_2 *= yscale;
          }
        }
        else if (MeasuredDirection::Yaxis == GetMeasuredDirection())
        {
          if (MYON_SQRT_EPSILON > fabs(def_pt_2d.y - ldr_pt_2d.y))
            ldr_pt_2d.y = def_pt_2d.y;
          else if (1.0 != xscale)
          {
            if (MYON_UNSET_VALUE != m_kink_offset_1)
              m_kink_offset_1 *= xscale;
            if (MYON_UNSET_VALUE != m_kink_offset_2)
              m_kink_offset_2 *= xscale;
          }
        }
        Set2dDefPt(def_pt_2d);
        Set2dLeaderPt(ldr_pt_2d);
      }
    }
    else
      rc = m_plane.Transform(xform);

    if (rc)
      MYON_Geometry::Transform(xform);
  }
  return rc;
}

bool MYON_DimOrdinate::GetBBox(double* boxmin, double* boxmax, bool grow) const  // overrides MYON_Geometry::GetBBox()
{
  const MYON_DimStyle* dimstyle = nullptr;
  return GetAnnotationBoundingBox(nullptr, dimstyle, 1.0, boxmin, boxmax, grow ? true : false);
}

bool MYON_DimOrdinate::GetAnnotationBoundingBox(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  double* boxmin,
  double* boxmax,
  bool bGrow
) const
{
  if (nullptr == dimstyle)
    dimstyle = &MYON_DimStyle::Default;

  MYON_2dPoint hash_points[] = {
    MYON_2dPoint(m_def_pt),
    MYON_2dPoint(m_ldr_pt), 
    MYON_2dPoint(m_kink_offset_1,m_kink_offset_2),
    MYON_2dPoint((double)static_cast<char>(m_direction),0.0)
  };

  const MYON_SHA1_Hash hash = Internal_GetBBox_InputHash(
    vp, 
    dimstyle, 
    dimscale, 
    m_user_text_point,
    (unsigned int)(sizeof(hash_points)/sizeof(hash_points[0])),
    hash_points
  );

  if (Internal_GetBBox_Begin(hash, boxmin, boxmax, bGrow))
    return true;

  if (nullptr == boxmin || nullptr == boxmax)
    return false;

  MYON_BoundingBox dbox;

  const MYON_TextContent* text = Text();
  if (nullptr != text)
  {
    dbox = text->TextContentBoundingBox();
    MYON_3dVector view_xdir = MYON_3dVector::XAxis;
    MYON_3dVector view_ydir = MYON_3dVector::YAxis;
    if (nullptr != vp)
    {
      view_xdir = vp->CameraX();
      view_ydir = vp->CameraY();
    }
    MYON_Xform textxform;
    GetTextXform(nullptr, vp, dimstyle, dimscale, textxform);
    dbox.Transform(textxform);
  }

  double points[12];
  const int defpt = 0, ldrpt = 3, kink1 = 6, kink2 = 9;
  Get3dPoints(nullptr, (MYON_3dPoint*)(&points[defpt]), (MYON_3dPoint*)(&points[ldrpt]), (MYON_3dPoint*)(&points[kink1]), (MYON_3dPoint*)(&points[kink2]));
  dbox.Set(3, 0, 4, 3, points, true);

  return Internal_GetBBox_End(dbox, hash, boxmin, boxmax, bGrow);
}

// Gets transform for dimension text from MYON_xy_plane to 3d display location
bool MYON_DimOrdinate::GetTextXform(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  return GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform_out);
}

bool MYON_DimOrdinate::GetTextXform(
  const MYON_Xform* model_xform,
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{

  // This gets the display text that's already on the dimension
  // If its not updated correctly for the view, the wrong size will be used
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;
  if (nullptr == dimstyle)
    return false;

  // See if the text needs remade because of some change in some property that
  // would change its appearance
  if (DimStyleTextPositionPropertiesHash() != dimstyle->TextPositionPropertiesHash())
  {
    MYON_wString rtfstr = text->RtfText();
    MYON::AnnotationType annotation_type = this->Type();
    bool wrapped = text->TextIsWrapped();
    double width = text->FormattingRectangleWidth();
    double rot = text->TextRotationRadians();
    const_cast<MYON_TextContent*>(text)->Create(rtfstr, annotation_type, dimstyle, wrapped, width, rot);
  }

  double text_width = 0.0;
  double text_height = 0.0;
  double text_gap = 0.0;
  double text_angle = 0.0;
  const MYON_Plane& textplane = MYON_xy_plane;
  bool draw_forward = dimstyle->DrawForward();

  const MYON_Plane& dimplane = Plane();
  MYON_3dPoint text_center = MYON_3dPoint::Origin;

  MYON_3dPoint cp[4];
  if (!text->Get3dCorners(cp))
    return false;

  text_center = (cp[0] + cp[2]) / 2.0;
  text_width = (cp[1].x - cp[0].x) * dimscale;
  text_height = (cp[3].y - cp[0].y) * dimscale;
  text_gap = dimstyle->TextGap() * dimscale;

  text_xform_out = MYON_Xform::IdentityTransformation;
  MYON_Xform dimplane_xf;
  dimplane_xf.Rotation(textplane, dimplane);     // Rotate text from world xy to dimension plane
  MYON_Xform textpt_xf(1.0);           // Dimension plane to text point translation 
  MYON_Xform textrot_xf(1.0);          // Text rotation around text plane origin point
  MYON_Xform textscale_xf(dimscale);
  MYON_Xform textshift_xf(1.0);

  MYON_2dPoint defpt = DefPt();
  MYON_2dPoint ldrpt = LeaderPt();

  MeasuredDirection direction = GetMeasuredDirection();
  if (MeasuredDirection::Unset == direction)
    direction = MeasuredDirection::Xaxis;

  MYON_2dVector shift(text_width / 2.0 + text_gap, 0.0);
  MYON_3dVector tail_dir;

  if (MeasuredDirection::Xaxis == direction) // Tail direction is vertical
  {
    tail_dir.Set(0.0, 1.0, 0.0);
    if (ldrpt.y < defpt.y)   // tail direction is down
    {
      tail_dir.y = -1.0;
      shift.x = -shift.x;
    }
    text_angle = MYON_PI / 2.0; // rotate 90 for vertical tail direction
  }
  else  // Measures y axis and tail direction is horizontal
  {
    tail_dir.Set(1.0, 0.0, 0.0);
    if (ldrpt.x < defpt.x)  // tail direction is left
    {
      tail_dir.x = -1.0;
      shift.x = -shift.x;
    }
  }

  MYON_Xform xfWorld2Cam;
  MYON_3dVector W2CX = dimplane.xaxis;
  MYON_3dVector W2CY = dimplane.yaxis;
  MYON_3dVector W2CZ = dimplane.zaxis;
  if (nullptr != vp)
  {
    vp->GetXform(MYON::coordinate_system::world_cs, MYON::coordinate_system::camera_cs, xfWorld2Cam);
    W2CX.Transform(xfWorld2Cam);
    W2CY.Transform(xfWorld2Cam);
    W2CZ.Transform(xfWorld2Cam);
    if (nullptr != model_xform)
    {
      W2CX.Transform(*model_xform);
      W2CY.Transform(*model_xform);
      W2CZ.Transform(*model_xform);
    }
  }
  bool xright = (W2CX * MYON_3dVector::XAxis) > -MYON_SQRT_EPSILON;
  bool yup = (W2CY * MYON_3dVector::YAxis) > -MYON_SQRT_EPSILON;
  
  textrot_xf.Rotation(text_angle, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);

  const MYON_DimStyle::TextLocation text_location = dimstyle->DimTextLocation();
  const MYON::TextOrientation text_orientation = dimstyle->DimTextOrientation();
  if (MYON_DimStyle::TextLocation::AboveDimLine == text_location)
  {
    shift.y = text_height / 2.0 + text_gap;
    if (!xright && direction == MeasuredDirection::Xaxis)
      shift.y = -shift.y;
    if (!yup && direction == MeasuredDirection::Yaxis)
      shift.y = -shift.y;
  }
  textshift_xf = MYON_Xform::TranslationTransformation(shift.x, shift.y, 0.0); // small shift around text point
  textpt_xf = MYON_Xform::TranslationTransformation(ldrpt.x, ldrpt.y, 0.0);    // base point to text point

  text_xform_out = MYON_Xform::DiagonalTransformation(dimscale, dimscale, dimscale);      // dimscale
  text_xform_out = textshift_xf * text_xform_out;          // dimension plane to text point
  text_xform_out = textrot_xf * text_xform_out;            // text rotation
  text_xform_out = textpt_xf * text_xform_out;             // dimension plane to text point
  text_xform_out = dimplane_xf * text_xform_out;           // text plane to dim plane

  MYON_3dVector view_xdir = MYON_3dVector::XAxis;
  MYON_3dVector view_ydir = MYON_3dVector::YAxis;
  MYON_3dVector view_zdir = MYON_3dVector::ZAxis;
  if (nullptr != vp)
  {
    view_xdir = vp->CameraX();
    view_ydir = vp->CameraY();
    view_zdir = vp->CameraZ();
  }

  if (MYON::TextOrientation::InView == text_orientation)  // Draw dimension horizontal to view
  {

    MYON_Xform tp2sxf;        // Text point to view plane rotation
    MYON_3dPoint text_point_3d = dimplane.PointAt(ldrpt.x, ldrpt.y);
    tp2sxf.Rotation(text_point_3d, dimplane.xaxis, dimplane.yaxis, dimplane.zaxis, text_point_3d, view_xdir, view_ydir, view_zdir);
    text_xform_out = tp2sxf * text_xform_out;
  }
  else if (draw_forward)
  {
    MYON_3dVector text_right_dir(1.0, 0.0, 0.0);
    text_right_dir.Transform(text_xform_out);
    if (nullptr != model_xform)
      text_right_dir.Transform(*model_xform);
    MYON_3dVector text_up_dir(0.0, 1.0, 0.0);
    text_up_dir.Transform(text_xform_out);
    if (nullptr != model_xform)
      text_up_dir.Transform(*model_xform);
    if (text_right_dir.Unitize() && text_up_dir.Unitize())
    {
      bool fx = false;
      bool fy = false;
      if (direction == MeasuredDirection::Xaxis)
      {
        fx = (view_ydir * text_right_dir) < 0.0;
        fy = (view_xdir * text_up_dir) > 0.0;
      }
      else
      {
        fx = (view_xdir * text_right_dir) < 0.0;
        fy = (view_ydir * text_up_dir) < 0.0;
      }

      MYON_Xform mxf;  // Mirror xform for backwards text to adjust DrawForward
      if (fx)
      {
        mxf.Mirror(text_center, MYON_3dVector::XAxis);
        text_xform_out = text_xform_out * mxf;
      }
      if (fy)
      {
        mxf.Mirror(text_center, MYON_3dVector::YAxis);
        text_xform_out = text_xform_out * mxf;
      }
    }
  }
  return true;
}

bool MYON_DimOrdinate::Create(
  const MYON_UUID style_id,
  const MYON_Plane& plane,
  MeasuredDirection direction,
  const MYON_3dPoint& basept,
  const MYON_3dPoint& defpt,
  const MYON_3dPoint& ldrpt,
  double kinkoffset1,
  double kinkoffset2)
{
  bool rc = true;
  m_dimstyle_id = style_id;
  if (MYON_nil_uuid == m_dimstyle_id)
    rc = true;
  if (!plane.IsValid() || !basept.IsValid() || !defpt.IsValid() || !ldrpt.IsValid())
    return false;

  m_plane = plane;

  MYON_2dPoint def_pton(MYON_2dPoint::NanPoint);
  MYON_2dPoint ldr_pton(MYON_2dPoint::NanPoint);

  m_plane.origin = plane.ClosestPointTo(basept);
  rc = m_plane.ClosestPointTo(defpt, &def_pton.x, &def_pton.y);
  if (rc)
  {
    rc = m_plane.ClosestPointTo(ldrpt, &ldr_pton.x, &ldr_pton.y);
    if (rc)
    {
      Set2dDefPt(def_pton);
      Set2dLeaderPt(ldr_pton);
      SetKinkOffset1(kinkoffset1);
      SetKinkOffset2(kinkoffset2);
    }
  }
  return rc;
}

bool MYON_DimOrdinate::AdjustFromPoints(
  const MYON_Plane& base_plane,
  MeasuredDirection direction,
  const MYON_3dPoint& basept,
  const MYON_3dPoint& defpt,
  const MYON_3dPoint& ldrpt,
  double kinkoffset1,
  double kinkoffset2)
{
  MYON_2dPoint base_pt(MYON_3dPoint::Origin), def_pt(MYON_3dPoint::Origin), ldr_pt(MYON_3dPoint::Origin);
  MYON_Plane plane = base_plane;
  plane.origin = basept;

  if (!plane.ClosestPointTo(defpt, &def_pt.x, &def_pt.y))
    return false;
  if (!plane.ClosestPointTo(ldrpt, &ldr_pt.x, &ldr_pt.y))
    return false;

  if (MeasuredDirection::Xaxis == GetMeasuredDirection())
  {
    if (MYON_SQRT_EPSILON > fabs(def_pt.x - ldr_pt.x))
      ldr_pt.x = def_pt.x;
  }
  else if (MeasuredDirection::Yaxis == GetMeasuredDirection())
  {
    if (MYON_SQRT_EPSILON > fabs(def_pt.y - ldr_pt.y))
      ldr_pt.y = def_pt.y;
  }
  SetPlane(plane);
  Set2dDefPt(def_pt);
  Set2dLeaderPt(ldr_pt);
  SetMeasuredDirection(direction);
  SetKinkOffset1(kinkoffset1);
  SetKinkOffset2(kinkoffset2);

  return true;
}

MYON_2dPoint MYON_DimOrdinate::DefPt() const
{
  return m_def_pt;
}

MYON_2dPoint MYON_DimOrdinate::LeaderPt() const
{
  return m_ldr_pt;
}

MYON_2dPoint MYON_DimOrdinate::KinkPt1() const
{
  MYON_2dPoint kink1(m_def_pt);
  MYON_2dPoint kink2(m_ldr_pt);
  CalcKinkPoints(m_def_pt, m_ldr_pt, MeasuredDirection(), 1.0, kink1, kink2);
  return kink1;
}

MYON_2dPoint MYON_DimOrdinate::KinkPt2() const
{
  MYON_2dPoint kink1(m_def_pt);
  MYON_2dPoint kink2(m_ldr_pt);
  CalcKinkPoints(m_def_pt, m_ldr_pt, MeasuredDirection(), 1.0, kink1, kink2);
  return kink2;
}
double MYON_DimOrdinate::KinkOffset1() const
{
  return m_kink_offset_1;
}

double MYON_DimOrdinate::KinkOffset2() const
{
  return m_kink_offset_2;
}

void MYON_DimOrdinate::Set2dDefPt(MYON_2dPoint pt)
{
  if (pt.IsValid())
    m_def_pt = pt;
}

void MYON_DimOrdinate::Set2dLeaderPt(MYON_2dPoint pt)
{
  if (pt.IsValid())
    m_ldr_pt = pt;
}

void MYON_DimOrdinate::SetKinkOffset1(double d)
{
  if (MYON_IsValid(d))
    m_kink_offset_1 = d;
}

void MYON_DimOrdinate::SetKinkOffset2(double d)
{
  if (MYON_IsValid(d))
    m_kink_offset_2 = d;
}

void MYON_DimOrdinate::Set3dBasePoint(MYON_3dPoint pt)
{
  MYON_2dVector p2;
  if (m_plane.ClosestPointTo(pt, &p2.x, &p2.y))
  {
    m_def_pt = m_def_pt - p2;
    m_ldr_pt = m_ldr_pt - p2;
    m_plane.origin = pt;
  }
}

void MYON_DimOrdinate::Set3dDefPt(MYON_3dPoint pt)
{
  double x, y;
  if (m_plane.ClosestPointTo(pt, &x, &y))
    m_def_pt.Set(x, y);
}

void MYON_DimOrdinate::Set3dLeaderPt(MYON_3dPoint pt)
{
  double x, y;
  if (m_plane.ClosestPointTo(pt, &x, &y))
    m_ldr_pt.Set(x, y);
}

MYON_3dPoint MYON_DimOrdinate::Get3dBasePoint() const
{
  return m_plane.origin;
}

MYON_3dPoint MYON_DimOrdinate::Get3dDefPt() const
{
  MYON_3dPoint p = m_plane.PointAt(m_def_pt.x, m_def_pt.y);
  return p;
}

MYON_3dPoint MYON_DimOrdinate::Get3dLeaderPt() const
{
  MYON_3dPoint p = m_plane.PointAt(m_ldr_pt.x, m_ldr_pt.y);
  return p;
}

MYON_3dPoint MYON_DimOrdinate::Get3dKinkPt1(double default_kink_offset) const
{
  MYON_3dPoint p(MYON_3dPoint::Origin);
  double ko1 = m_kink_offset_1;
  double ko2 = m_kink_offset_2;
  if (!MYON_IsValid(ko1))
    ko1 = default_kink_offset;
  if (!MYON_IsValid(ko2))
    ko2 = default_kink_offset;
  MeasuredDirection direction = MeasuredDirection();
  if ((MeasuredDirection::Xaxis == direction && m_ldr_pt.y < m_def_pt.y)
      ||
      (MeasuredDirection::Yaxis == direction && m_ldr_pt.x < m_def_pt.x))
  {
    ko1 = -ko1;
    ko2 = -ko2;
  }
  if (MeasuredDirection::Xaxis == direction)
    p = m_plane.PointAt(m_def_pt.x, m_ldr_pt.y - ko1 - ko2);
  else if (MeasuredDirection::Yaxis == direction)
    p = m_plane.PointAt(m_ldr_pt.x - ko1 - ko2, m_def_pt.y);
  return p;
}

MYON_3dPoint MYON_DimOrdinate::Get3dKinkPt2(double default_kink_offset) const
{
  MYON_3dPoint p(MYON_3dPoint::Origin);
  double ko1 = m_kink_offset_1;
  if (!MYON_IsValid(ko1))
    ko1 = default_kink_offset;
  MeasuredDirection direction = GetMeasuredDirection();
  if ((MeasuredDirection::Xaxis == direction && m_ldr_pt.y < m_def_pt.y)
      ||
      (MeasuredDirection::Yaxis == direction && m_ldr_pt.x < m_def_pt.x))
  {
    ko1 = -ko1;
  }
  if (MeasuredDirection::Xaxis == direction)
    p = m_plane.PointAt(m_ldr_pt.x, m_ldr_pt.y - ko1);
  else if (MeasuredDirection::Yaxis == direction)
    p = m_plane.PointAt(m_ldr_pt.x - ko1, m_ldr_pt.y);
  return p;
}

bool MYON_DimOrdinate::Get3dPoints(
  MYON_3dPoint* base,
  MYON_3dPoint* def1,
  MYON_3dPoint* def2,
  MYON_3dPoint* kink1,
  MYON_3dPoint* kink2,
  double default_kink_offset) const
{
  if (nullptr == base && nullptr == def1 && nullptr == def2 && nullptr == kink1 && nullptr == kink2)
    return false;
  if (nullptr != base)
    *base = m_plane.origin;
  if (nullptr != def1)
    *def1 = m_plane.PointAt(m_def_pt.x, m_def_pt.y);
  if (nullptr != def2)
    *def2 = m_plane.PointAt(m_ldr_pt.x, m_ldr_pt.y);
  if (nullptr != kink1 || nullptr != kink2)
  {
    double ko1 = m_kink_offset_1;
    double ko2 = m_kink_offset_2;
    if (!MYON_IsValid(ko1))
      ko1 = default_kink_offset;
    if (!MYON_IsValid(ko2))
      ko2 = default_kink_offset;
    MeasuredDirection direction = GetMeasuredDirection();
    if ((MeasuredDirection::Xaxis == direction && m_ldr_pt.y < m_def_pt.y)
        ||
        (MeasuredDirection::Yaxis == direction && m_ldr_pt.x < m_def_pt.x))
    {
      ko1 = -ko1;
      ko2 = -ko2;
    }
    if (nullptr != kink1)
    {
      if (MeasuredDirection::Xaxis == direction)
        *kink1 = m_plane.PointAt(m_def_pt.x, m_ldr_pt.y - ko1 - ko2);
      else if (MeasuredDirection::Yaxis == direction)
        *kink1 = m_plane.PointAt(m_ldr_pt.x - ko1 - ko2, m_def_pt.y);
    }
    if (nullptr != kink2)
    {
      if (MeasuredDirection::Xaxis == direction)
        *kink2 = m_plane.PointAt(m_ldr_pt.x, m_ldr_pt.y - ko1);
      else if (MeasuredDirection::Yaxis == direction)
        *kink2 = m_plane.PointAt(m_ldr_pt.x - ko1, m_ldr_pt.y);
    }
  }
  return true;  
}

bool MYON_DimOrdinate::GetDisplayLines(
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_3dPoint text_rect[4],
  MYON_Line lines[3],
  bool isline[3],
  int maxlines) const
{
  if (3 != maxlines)
  {
    MYON_ERROR("Wrong linecount calling MYON_DimOrdinate::GetDisplayLines.\n");
    return false;
  }
  MYON_3dPoint defpt, ldrpt, kink1, kink2, frompt;
  Get3dPoints(nullptr, &defpt, &ldrpt, &kink1, &kink2);
  frompt = defpt;
  double eodist = dimstyle->ExtOffset() * dimscale;
  MYON_3dVector eodir = kink1 - defpt;
  if (eodir.Unitize())
  {
    frompt = defpt + (eodir * eodist);
  }

  lines[0].from = frompt;
  lines[0].to = kink1;
  if (MYON_SQRT_EPSILON < lines[0].Length())
    isline[0] = true;
  else
    isline[0] = false;

  lines[1].from = kink1;
  lines[1].to = kink2;
  if (MYON_SQRT_EPSILON < lines[1].Length())
    isline[1] = true;
  else
    isline[1] = false;

  lines[2].from = kink2;
  lines[2].to = ldrpt;

  //MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode text_alignment_mode = dimstyle->TextAlignment();
  const MYON_DimStyle::TextLocation text_location = dimstyle->DimTextLocation();

  //if (MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine == text_alignment_mode)
  if (MYON_DimStyle::TextLocation::AboveDimLine == text_location )
  {
    MYON_2dPoint defpt2d = DefPt();
    MYON_2dPoint ldrpt2d = LeaderPt();

    if (MYON_DimOrdinate::MeasuredDirection::Xaxis == GetMeasuredDirection())
    {
      double text_width = fabs(text_rect[1].y - text_rect[0].y);
      if (ldrpt2d.y > defpt2d.y)
        lines[2].to = Plane().PointAt(ldrpt2d.x, ldrpt2d.y + text_width);
      else
        lines[2].to = Plane().PointAt(ldrpt2d.x, ldrpt2d.y - text_width);
    }
    else
    {
      double text_width = fabs(text_rect[1].x - text_rect[0].x);
      if (ldrpt2d.x > defpt2d.x)
        lines[2].to = Plane().PointAt(ldrpt2d.x + text_width, ldrpt2d.y);
      else
        lines[2].to = Plane().PointAt(ldrpt2d.x - text_width, ldrpt2d.y);
    }
  }
  if (MYON_SQRT_EPSILON < lines[2].Length())
    isline[2] = true;
  else
    isline[2] = false;
  return true;
}

bool MYON_DimOrdinate::CalcKinkPoints(
  MYON_2dPoint defpt,
  MYON_2dPoint ldrpt,
  MeasuredDirection direction,
  double default_kink_offset,
  MYON_2dPoint& kinkpt1_out,
  MYON_2dPoint& kinkpt2_out) const
{
  bool rc = false;
  if (MeasuredDirection::Unset == direction)
    direction = ImpliedDirection(defpt, ldrpt);
  if (MeasuredDirection::Unset == direction)
    return false;

  double offset1 = KinkOffset1();
  double offset2 = KinkOffset2();

  // if these haven't been set by dragging the offset points
  // default distance - 2 * textheight
  if (offset1 == MYON_UNSET_VALUE)
  {
    offset1 = default_kink_offset;
  }
  if (offset2 == MYON_UNSET_VALUE)
  {
    offset2 = default_kink_offset;
  }
  ((MYON_DimOrdinate*)this)->SetKinkOffset1(offset1);
  ((MYON_DimOrdinate*)this)->SetKinkOffset2(offset2);
  if (direction == MeasuredDirection::Xaxis)
  {
    if (defpt.y > ldrpt.y)
    {
      offset1 = -offset1;
      offset2 = -offset2;
    }
    kinkpt1_out.x = ldrpt.x;
    kinkpt1_out.y = ldrpt.y - offset1;
    kinkpt2_out.x = defpt.x;
    kinkpt2_out.y = ldrpt.y - offset1 - offset2;
    rc = true;
  }
  else if (direction == MeasuredDirection::Yaxis)
  {
    if (defpt.x > ldrpt.x)
    {
      offset1 = -offset1;
      offset2 = -offset2;
    }
    kinkpt1_out.y = ldrpt.y;
    kinkpt1_out.x = ldrpt.x - offset1;
    kinkpt2_out.y = defpt.y;
    kinkpt2_out.x = ldrpt.x - offset1 - offset2;
    rc = true;
  }
  return rc;
}

MYON_DimOrdinate::MeasuredDirection MYON_DimOrdinate::ImpliedDirection(
  MYON_2dPoint defpt,
  MYON_2dPoint ldrpt
  ) const
{
  MeasuredDirection direction = MeasuredDirection::Unset;
  if (fabs(ldrpt.x - defpt.x) <= fabs(ldrpt.y - defpt.y))
    direction = MeasuredDirection::Xaxis; // measures along x axis
  else
    direction = MeasuredDirection::Yaxis; // measures along y axis

  return direction;
}

MYON_DimOrdinate::MeasuredDirection MYON_DimOrdinate::GetMeasuredDirection() const
{
  if (MeasuredDirection::Unset == m_direction)
    return ImpliedDirection(m_def_pt, m_ldr_pt);
  else
    return m_direction;
}

void MYON_DimOrdinate::SetMeasuredDirection(MeasuredDirection direction)
{
  m_direction = direction;
}

double MYON_DimOrdinate::Measurement() const
{
  double m = 0.0;
  switch (GetMeasuredDirection())
  {
  case MeasuredDirection::Xaxis:
      m = m_def_pt.x;
    break;
  case MeasuredDirection::Yaxis:
      m = m_def_pt.y;
    break;
  case MeasuredDirection::Unset:
    break;
  }
  if (DistanceScale() != 1.0)
    m *= DistanceScale();

  return fabs(m);
}


//----------------------------------------------------------
// Class MYON_Centermark

MYON_Centermark::MYON_Centermark()
  : MYON_Dimension(MYON::AnnotationType::CenterMark)
{}

double MYON_Centermark::Measurement() const
{
  return 0.0;
}

bool MYON_Centermark::Write(
  MYON_BinaryArchive& archive
  ) const
{
  const int content_version = 0;
  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!MYON_Dimension::Internal_WriteDimension(archive))
      break;
    if (!archive.WriteDouble(m_radius))
      break;
    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_Centermark::Read(
  MYON_BinaryArchive& archive
  )
{
  *this = MYON_Centermark::Empty;

  int content_version = -1;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (content_version < 0)
      break;
    if (!MYON_Dimension::Internal_ReadDimension(archive))
      break;
    if (!archive.ReadDouble(&m_radius))
      break;
    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_Centermark::Transform(const MYON_Xform& xform)
{
  bool rc = xform.IsIdentity();
  if (!rc)
  {
    rc = m_plane.Transform(xform);
    if (rc)
      MYON_Geometry::Transform(xform);
  }
  return rc;
}

bool MYON_Centermark::GetTextXform(
  const MYON_Viewport*,
  const MYON_DimStyle*,
  double,
  MYON_Xform&
) const
{
  return true;
}

bool MYON_Centermark::Create(
  const MYON_UUID style_id,
  const MYON_Plane& plane,
  const MYON_3dPoint& center_pt,
  const double radius)
{
  bool rc = true;
  m_dimstyle_id = style_id;
  if (MYON_nil_uuid == m_dimstyle_id)
    rc = true;
  if (!plane.IsValid() || !center_pt.IsValid() || !center_pt.IsValid())
    return false;
  m_plane = plane;
  if (rc)
  {
    m_plane.origin = plane.ClosestPointTo(center_pt);
    m_radius = radius;
  }
  return rc;
}

bool MYON_Centermark::AdjustFromPoints(
  const MYON_Plane& plane,
  const MYON_3dPoint& center_pt
  )
{
  m_plane = plane;
  m_plane.origin = center_pt;
  return true;
}

bool MYON_Centermark::GetBBox(double* bmin, double* bmax, bool grow) const
{
  const MYON_DimStyle* dimstyle = nullptr;
  return GetAnnotationBoundingBox(nullptr, dimstyle, 1.0, bmin, bmax, grow ? true : false);
}

bool MYON_Centermark::GetAnnotationBoundingBox(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  double* boxmin,
  double* boxmax,
  bool bGrow
) const
{
  if (nullptr == dimstyle)
    dimstyle = &MYON_DimStyle::Default;

  const MYON_2dPoint hash_points[] = {
    MYON_2dPoint(m_radius,0.0)
  };

  const MYON_SHA1_Hash hash = Internal_GetBBox_InputHash(
    vp, 
    dimstyle, 
    dimscale, 
    m_user_text_point,
    (unsigned int)(sizeof(hash_points)/sizeof(hash_points[0])),
    hash_points
  );

  if (Internal_GetBBox_Begin(hash, boxmin, boxmax, bGrow))
    return true;

  if (nullptr == boxmin || nullptr == boxmax)
    return false;

  MYON_BoundingBox dbox;

#define dimlinecount 6
  MYON_Line lines[dimlinecount];
  bool isline[dimlinecount] = { false };
  if (GetDisplayLines(dimstyle, dimscale, lines, isline, dimlinecount))
  {
    for (int i = 0; i < dimlinecount; i++)
    {
      if (isline[i])
      {
        dbox.Set(lines[i].from, true);
        dbox.Set(lines[i].to, true);
      }
    }
  }
#undef dimlinecount

  return Internal_GetBBox_End(dbox, hash, boxmin, boxmax, bGrow);
}

MYON_2dPoint MYON_Centermark::CenterPoint() const
{
  return MYON_2dPoint::Origin;
}

void MYON_Centermark::Set2dCenterPoint(MYON_2dPoint pt)
{
  if (pt.IsValid())
    m_plane.origin = m_plane.PointAt(pt.x, pt.y);
}

void MYON_Centermark::Set3dCenterPoint(MYON_3dPoint pt)
{
  if (pt.IsValid())
    m_plane.origin = pt;
}

double MYON_Centermark::Radius() const
{
  return m_radius;
}

void MYON_Centermark::SetRadius(double radius)
{
  if (!(radius > MYON_UNSET_VALUE && radius < MYON_UNSET_POSITIVE_VALUE))
  {
    MYON_ERROR("Invalid radius parameter in MYON_Centermark::SetRadius().");
    return;
  }
  m_radius = radius;
}

bool MYON_Dimension::GetCentermarkSnapPoints(
  const MYON_Plane& plane,
  const MYON_2dPoint center,
  double marksize,
  double radius,
  MYON_DimStyle::centermark_style style,
  MYON_3dPoint points[13],
  bool ispoint[13])
{
  for (int i = 0; i < 13; i++)
    ispoint[i] = false;
  MYON_Line lines[6];
  bool isline[6] = { false,false,false,false,false,false };
  const int dimlinecount = 6;
  if (GetCentermarkDisplay(plane, center, marksize, radius, style, lines, isline, dimlinecount))
  {
    points[0] = plane.origin;
    ispoint[0] = true;
    for (int j = 0; j < dimlinecount; j++)
    {
      if (isline[j])
      {
        for (int i = 0; i < 2; i++)
        {
          int t = 1 + (2 * j) + i;
          points[t] = i ? lines[j].from : lines[j].to;
          ispoint[t] = true;
        }
      }
    }
    return true;
  }
  return false;
}

bool MYON_Dimension::GetCentermarkDisplay(
  const MYON_Plane& plane,
  const MYON_2dPoint center,
  double marksize,
  double radius,
  MYON_DimStyle::centermark_style style,
  MYON_Line lines[6],
  bool isline[6],
  int maxlines)
{
  if (MYON_DimStyle::centermark_style::None != style)
  {
    if (maxlines < 2)
    {
      MYON_ERROR("Wrong line count calling MYON_Dimension::GetCentermarkDisplay()\n");
      return false;
    }
    lines[0].from = plane.PointAt(center.x - marksize, center.y);
    lines[0].to   = plane.PointAt(center.x + marksize, center.y);
    lines[1].from = plane.PointAt(center.x, center.y - marksize);
    lines[1].to   = plane.PointAt(center.x, center.y + marksize);
    isline[0] = isline[1] = true;

    if (MYON_DimStyle::centermark_style::MarkAndLines == style)
    {
      if (maxlines != 6)
      {
        MYON_ERROR("Wrong line count calling MYON_Dimension::GetCentermarkDisplay()\n");
        return false;
      }

      lines[2].from = plane.PointAt(center.x + 2.0 * marksize, center.y);
      lines[2].to   = plane.PointAt(center.x + radius + marksize, center.y);
      lines[3].from = plane.PointAt(center.x, center.y + 2.0 * marksize);
      lines[3].to   = plane.PointAt(center.x, center.y + radius + marksize);
      lines[4].from = plane.PointAt(center.x - 2.0 * marksize, center.y);
      lines[4].to   = plane.PointAt(center.x - radius - marksize, center.y);
      lines[5].from = plane.PointAt(center.x, center.y - 2.0 * marksize);
      lines[5].to   = plane.PointAt(center.x, center.y - radius - marksize);
      isline[2] = isline[3] = isline[4] = isline[5] = true;
    }
  }
  return true;
}

bool MYON_Centermark::GetDisplayLines(
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Line lines[6],
  bool isline[6], 
  int maxlines) const
{
  if (maxlines != 6)
  {
    MYON_ERROR("Wrong linecount calling MYON_Centermark::GetDisplayLines.\n");
    return false;
  }
  if (nullptr == dimstyle)
    return false;
  isline[0] = isline[1] = isline[2] = isline[3] = isline[4] = isline[5] = false;

  if (MYON_DimStyle::centermark_style::None == dimstyle->CenterMarkStyle())
    return true;

  MYON_2dPoint center = CenterPoint();

  const MYON_Plane& plane = Plane();
  double centermarksize = dimstyle->CenterMark() * dimscale;
  double radius = Radius();
  MYON_DimStyle::centermark_style style = dimstyle->CenterMarkStyle();
  if (MYON_SQRT_EPSILON < centermarksize)
    return MYON_Dimension::GetCentermarkDisplay(plane, center, centermarksize, radius, style, lines, isline, 6);
  else
    return true;
}


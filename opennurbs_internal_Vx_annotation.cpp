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
////////////////////////////////////////////////////////////////

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_internal_V2_annotation.h"
#include "opennurbs_internal_V5_annotation.h"

/////////////////////////////////////////////////////////////////////////////
//
// This file contains code that translates current annotation object
// defintions to and from obsolete V2 and V5 annotation object definitions.
// This code is used to read and write old file formats.


/////////////////////////////////////////////////////////////////////////////
//
// text justification 
//
#pragma region

MYON::TextVerticalAlignment MYON::TextVerticalAlignmentFromV5Justification(
  unsigned int v5_justification_bits
)
{
  MYON::TextVerticalAlignment valign = MYON::TextVerticalAlignment::Top;

  const unsigned int v5_valign_mask
    = MYON_OBSOLETE_V5_TextObject::eTextJustification::tjBottom
    | MYON_OBSOLETE_V5_TextObject::eTextJustification::tjMiddle
    | MYON_OBSOLETE_V5_TextObject::eTextJustification::tjTop
    | MYON_OBSOLETE_V5_TextObject::eTextJustification::tjUndefined;

  if (MYON_OBSOLETE_V5_TextObject::eTextJustification::tjTop == (v5_justification_bits & v5_valign_mask))
    valign = MYON::TextVerticalAlignment::Top;
  else if (MYON_OBSOLETE_V5_TextObject::eTextJustification::tjMiddle == (v5_justification_bits & v5_valign_mask))
    valign = MYON::TextVerticalAlignment::Middle;
  else if (MYON_OBSOLETE_V5_TextObject::eTextJustification::tjBottom == (v5_justification_bits & v5_valign_mask))
    valign = MYON::TextVerticalAlignment::Bottom;

  return valign;
}

MYON::TextHorizontalAlignment MYON::TextHorizontalAlignmentFromV5Justification(
  unsigned int v5_justification_bits
)
{
  MYON::TextHorizontalAlignment halign = MYON::TextHorizontalAlignment::Left;

  const unsigned int v5_halign_mask
    = MYON_OBSOLETE_V5_TextObject::eTextJustification::tjLeft
    | MYON_OBSOLETE_V5_TextObject::eTextJustification::tjCenter
    | MYON_OBSOLETE_V5_TextObject::eTextJustification::tjRight;

  if (MYON_OBSOLETE_V5_TextObject::eTextJustification::tjCenter == (v5_justification_bits & v5_halign_mask))
    halign = MYON::TextHorizontalAlignment::Center;
  else if (MYON_OBSOLETE_V5_TextObject::eTextJustification::tjRight == (v5_justification_bits & v5_halign_mask))
    halign = MYON::TextHorizontalAlignment::Right;

  return halign;
}


#pragma endregion

/////////////////////////////////////////////////////////////////////////////
//
// annotation base class
//
#pragma region

MYON_Annotation* MYON_Annotation::CreateFromV2Annotation(
  const class MYON_OBSOLETE_V2_Annotation& V2_annotation,
  const class MYON_3dmAnnotationContext* annotation_context
)
{
  MYON_OBSOLETE_V5_Annotation* V5_annotation = MYON_OBSOLETE_V5_Annotation::CreateFromV2Annotation(V2_annotation, annotation_context);
  if (nullptr == V5_annotation)
    return nullptr;
  MYON_Annotation* V6_annotation = MYON_Annotation::CreateFromV5Annotation(*V5_annotation, annotation_context);
  delete V5_annotation;
  return V6_annotation;
}

void MYON_Annotation::Internal_SetDimStyleFromV5Annotation(
  const class MYON_OBSOLETE_V5_Annotation& V5_annotation,
  const class MYON_3dmAnnotationContext* annotation_context
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  const MYON_UUID parent_dim_style_id 
    = (nullptr == annotation_context)
    ? MYON_nil_uuid
    : annotation_context->ParentDimStyleId();
  SetDimensionStyleId( parent_dim_style_id );
  const MYON_DimStyle* override_candidate
    = (MYON_nil_uuid == parent_dim_style_id || nullptr == annotation_context || false == annotation_context->IsOverrideDimStyle())
    ? nullptr
    : &annotation_context->DimStyle();
  if (nullptr != override_candidate)
  {
    MYON_DimStyle* ds = new MYON_DimStyle(*override_candidate);
    ds->SetParentId(parent_dim_style_id);
    ds->ClearId();
    ds->ClearIndex();
    ds->ClearName();
    SetOverrideDimensionStyle(ds);
    if (nullptr != ds && ds != m_override_dimstyle)
      delete ds;
  }
}

MYON_Annotation* MYON_Annotation::CreateFromV5Annotation(
  const class MYON_OBSOLETE_V5_Annotation& V5_annotation,
  const class MYON_3dmAnnotationContext* annotation_context
)
{
  const MYON_OBSOLETE_V5_TextObject* V5_text_object = MYON_OBSOLETE_V5_TextObject::Cast(&V5_annotation);
  if (nullptr != V5_text_object)
    return MYON_Text::CreateFromV5TextObject(*V5_text_object, annotation_context, nullptr);

  const MYON_OBSOLETE_V5_Leader* V5_leader = MYON_OBSOLETE_V5_Leader::Cast(&V5_annotation);
  if ( nullptr != V5_leader )
    return MYON_Leader::CreateFromV5Leader(*V5_leader, annotation_context, nullptr);

  const MYON_OBSOLETE_V5_DimLinear* V5_dim_linear = MYON_OBSOLETE_V5_DimLinear::Cast(&V5_annotation);
  if ( nullptr != V5_dim_linear )
    return MYON_DimLinear::CreateFromV5DimLinear(*V5_dim_linear, annotation_context, nullptr);

  const MYON_OBSOLETE_V5_DimAngular* V5_dim_angle = MYON_OBSOLETE_V5_DimAngular::Cast(&V5_annotation);
  if ( nullptr != V5_dim_angle )
    return MYON_DimAngular::CreateFromV5DimAngular(*V5_dim_angle, annotation_context, nullptr);

  const MYON_OBSOLETE_V5_DimRadial* V5_dim_radial = MYON_OBSOLETE_V5_DimRadial::Cast(&V5_annotation);
  if ( nullptr != V5_dim_radial )
    return MYON_DimRadial::CreateFromV5DimRadial(*V5_dim_radial, annotation_context, nullptr);
  
  const MYON_OBSOLETE_V5_DimOrdinate* V5_dim_ordinate = MYON_OBSOLETE_V5_DimOrdinate::Cast(&V5_annotation);
  if ( nullptr != V5_dim_ordinate )
    return MYON_DimOrdinate::CreateFromV5DimOrdinate(*V5_dim_ordinate, annotation_context, nullptr);
  
  return nullptr;
}

void MYON_OBSOLETE_V5_Annotation::Internal_InitializeFromV2Annotation(
  const class MYON_OBSOLETE_V2_Annotation& V2_annotation,
  const class MYON_3dmAnnotationContext* annotation_context
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  //const MYON_3dmAnnotationSettings& annotation_settings = annotation_context->AnnotationSettings();

  SetTextFormula(nullptr);

  MYON_wString text
    = (V2_annotation.m_usertext.IsNotEmpty())
    ? V2_annotation.m_usertext
    : V2_annotation.m_defaulttext;
  text.TrimLeftAndRight();

  SetTextValue(text);

  // 30 Oct 2013 - Lowell - changed to copy the number of points in V2_annotation.PointCount() instead of always 5
  const int V2_point_count = V2_annotation.PointCount();
  const MYON_2dVector v
    = (V2_point_count > 0 && V2_annotation.m_points[0].IsValid() && MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAngular != V2_annotation.m_type)
    ? (MYON_2dPoint::Origin - V2_annotation.m_points[0])
    : MYON_2dVector::ZeroVector;

  m_points.SetCount(0);
  m_points.Reserve(V2_point_count);
  if (V2_point_count > 0)
    m_points.Append(MYON_2dPoint::Origin);
  for (int i = 1; i < V2_point_count; i++)
  {
    const MYON_2dPoint V2_point = V2_annotation.Point(i);
    MYON_2dPoint v5_point
      = V2_point.IsValid()
      ? V2_point + v
      : V2_point;
    SetPoint(i, v5_point);
  }

  m_plane = V2_annotation.Plane();
  m_plane.UpdateEquation();
  if (false == m_plane.IsValid())
    m_plane = MYON_Plane::World_xy;
  else if (v.IsNotZero())
  {
    m_plane.origin = m_plane.PointAt(-v.x, -v.y);
    m_plane.UpdateEquation();
  }

  m_textheight = 1.0;

  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode text_display_mode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal;
  bool bUserPositionedText = false;
  int min_point_count = -1;
  int max_point_count = -1;

  switch (V2_annotation.m_type)
  {
  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing:
    m_textdisplaymode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimLinear:
  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAligned:
    text_display_mode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine;
    min_point_count = MYON_OBSOLETE_V5_DimLinear::dim_pt_count;
    max_point_count = MYON_OBSOLETE_V5_DimLinear::dim_pt_count;
    break;
  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAngular:
    bUserPositionedText = V2_annotation.UserPositionedText() ? true : false;
    text_display_mode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine;
    min_point_count = 2;
    max_point_count = 3;
    break;
  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimDiameter:
    bUserPositionedText = V2_annotation.UserPositionedText() ? true : false;
    break;
  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimRadius:
    bUserPositionedText = V2_annotation.UserPositionedText() ? true : false;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtLeader:
    bUserPositionedText = false;
    text_display_mode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kInLine;
    min_point_count = 2;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtTextBlock:
    break;

  case MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimOrdinate:
    break;

  default:
    m_textdisplaymode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal;
    break;
  }

  m_textdisplaymode = text_display_mode;
  m_userpositionedtext = bUserPositionedText;

  if (max_point_count > 0 && max_point_count >= min_point_count && m_points.Count() > max_point_count)
    m_points.SetCount(max_point_count);
  else if (min_point_count >= 0 && m_points.Count() < min_point_count)
    m_points.SetCount(0);

  SetV5_3dmArchiveDimStyleIndex(annotation_context->V5_ArchiveDimStyleIndex());
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////
//
// text object
//
#pragma region

void MYON_Text::Internal_SetObsoleteV5TextObjectInformation(
  const class MYON_3dmAnnotationContext* annotation_context,
  const class MYON_OBSOLETE_V5_TextObject& V5_text_object
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();
  const MYON_DimStyle& dim_style = annotation_context->DimStyle();

  const MYON::LengthUnitSystem V5_model_space_object_unit_system = annotation_context->ModelLengthUnitSystem();

  const double obsolete_V5_text_object_height = V5_text_object.Height();
  const double obsolete_V5_model_space_text_scale
    = annotation_context->AnnotationSettings().Is_V5_AnnotationScalingEnabled()
    ? annotation_context->AnnotationSettings().WorldViewTextScale()
    : 1.0;

  // Set m_obsolete_V5_text_object_height
  for (;;)
  {
    if (!MYON_IsValid(obsolete_V5_text_object_height))
      break;
    if (!(obsolete_V5_text_object_height > 0.0))
      break;

    const double unit_scale = MYON::UnitScale(V5_model_space_object_unit_system, dim_style.UnitSystem());
    if (false == MYON_IsValidPositiveNumber(unit_scale))
      break;

    const double V6_object_text_height = unit_scale*obsolete_V5_text_object_height;
    if (!MYON_IsValid(V6_object_text_height))
      break;
    if (!(V6_object_text_height > 0.0))
      break;

    const double dim_style_text_height = dim_style.TextHeight();
    if (!MYON_IsValid(dim_style_text_height))
      break;
    if (!(dim_style_text_height > 0.0))
      break;

    if (fabs(V6_object_text_height - dim_style_text_height) <= dim_style_text_height*0.01)
      break;

    // override text height setting
    SetTextHeight(&parent_dim_style, V6_object_text_height);
    break;
  }

  SetAllowTextScaling(V5_text_object.m_annotative_scale);

  // Set m_obsolete_V5_model_space_text_scale
  for (;;)
  {
    if (!V5_text_object.m_annotative_scale)
      break;

    if (false == annotation_context->AnnotationSettings().Is_V5_AnnotationScalingEnabled())
      break;

    if (!MYON_IsValid(obsolete_V5_model_space_text_scale))
      break;
    if (!(obsolete_V5_model_space_text_scale > 0.0))
      break;

    const double dim_style_model_space_text_scale = dim_style.DimScale();
    if (!MYON_IsValid(dim_style_model_space_text_scale))
      break;
    if (!(dim_style_model_space_text_scale > 0.0))
      break;


    if (fabs(obsolete_V5_model_space_text_scale - dim_style_model_space_text_scale) <= dim_style_model_space_text_scale*0.01)
      break;

    // override dim scale setting
    SetDimScale(&parent_dim_style, obsolete_V5_model_space_text_scale);
    break;
  }

  return;
}

static bool RemoveV5Wrapping(const wchar_t* txt, const MYON_DimStyle& dimstyle, double& wrapwidth, MYON_wString& newtxt)
{
  if (nullptr == txt)
    return false;

  bool wrapped = false;
  int slen = (int)wcslen(txt);
  newtxt.ReserveArray(slen + 1);
  int linestart = 0;
  MYON_wString textline;
  wrapwidth = 0.0;
  for (int i = 0; i < slen; i++)
  {
    // throw away \n not after \r
    if (txt[i] == L'\n' && i > 0 && txt[i - 1] != L'\r')
    {
      // Make MYON_Text with one wrapped line to measure width
      textline = txt + linestart;
      textline.SetLength(i - linestart);
      if(i < slen-1)
        linestart = i + 1;
      MYON_Text* temptext = new MYON_Text();
      textline += L' ';
      temptext->Create(textline, &dimstyle, MYON_xy_plane);
      MYON_3dPoint pts[4];
      temptext->GetText3dCorners(pts);
      delete temptext;
      double w = pts[1].x - pts[0].x;
      if (w > wrapwidth)
        wrapwidth = w;

      wrapped = true;
      newtxt += L" ";
      continue;
    }
    newtxt += txt[i];
  }
  return wrapped;
}

MYON_Text* MYON_Text::CreateFromV5TextObject(
  const class MYON_OBSOLETE_V5_TextObject& V5_text_object,
  const class MYON_3dmAnnotationContext* annotation_context,
  MYON_Text* destination
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  const MYON_DimStyle& dim_style = annotation_context->DimStyle();

  const unsigned int just = V5_text_object.Justification();
  const MYON::TextHorizontalAlignment halign = MYON::TextHorizontalAlignmentFromV5Justification(just);
  const MYON::TextVerticalAlignment valign = MYON::TextVerticalAlignmentFromV5Justification(just);

  MYON_Plane plane = V5_text_object.Plane();
  const double v5height = V5_text_object.Height();
  MYON_wString txt = V5_text_object.TextFormula();
  if (txt.IsEmpty())
    txt = V5_text_object.TextValue();

  MYON_Text* V6_text_object = (nullptr != destination)
    ? destination
    : new MYON_Text();

  double wrapwidth = 0.0;
  MYON_wString newtxt;
  bool wraptext = RemoveV5Wrapping(txt, dim_style, wrapwidth, newtxt);

  newtxt.Replace(L"\\", L"\\\\");

  if (wraptext)
  {
    double dimscale = dim_style.DimScale();
    double dsht = dim_style.TextHeight();
    double widthscale = v5height * dimscale / dsht;
    double width = (wrapwidth + (v5height * 0.1)) * widthscale;
    V6_text_object->Create(newtxt, &dim_style, plane, true, width, 0.0);
  }
  else
    V6_text_object->Create(newtxt, &dim_style, plane);

    // These override settings currently get erased by 
  // MYON_Annotation::SetDimensionStyleId(MYON_UUID dimstyle_id)
  // which deletes any existing override style

  if (v5height > 0.0 && v5height != dim_style.TextHeight())
    V6_text_object->SetTextHeight(&dim_style, v5height);
  if (halign != dim_style.TextHorizontalAlignment())
    V6_text_object->SetTextHorizontalAlignment(&dim_style, halign);
  if (valign != dim_style.TextVerticalAlignment())
    V6_text_object->SetTextVerticalAlignment(&dim_style, valign);

  bool setmask = false;
  MYON_TextMask mask = dim_style.TextMask();
  bool drawmask = V5_text_object.DrawTextMask();
  if (drawmask != mask.DrawTextMask())
  {
    mask.SetDrawTextMask(drawmask);
    setmask = true;
  }
  if (drawmask)
  {
    double mb = V5_text_object.MaskOffsetFactor() * v5height;
    if (mb != mask.MaskBorder())
    {
      mask.SetMaskBorder(mb);
      setmask = true;
    }
    MYON_TextMask::MaskType mt = mask.MaskFillType();
    int mc = V5_text_object.MaskColorSource();
    if ((mt == MYON_TextMask::MaskType::BackgroundColor && mc != 0) ||
      (mt == MYON_TextMask::MaskType::MaskColor && mc != 1))
    {
      mask.SetMaskFillType(mc == 0 ? MYON_TextMask::MaskType::BackgroundColor : MYON_TextMask::MaskType::MaskColor);
      setmask = true;
    }
    if (V5_text_object.MaskColor() != mask.MaskColor())
    {
      mask.SetMaskColor(V5_text_object.MaskColor());
      setmask = true;
    }
  }
  if (setmask)
    V6_text_object->SetTextMask(&dim_style, mask);


  if (V5_text_object.m_annotative_scale && annotation_context->AnnotationSettingsAreSet())
  {
    if (false == annotation_context->AnnotationSettings().Is_V5_AnnotationScalingEnabled())
    {
      if (!(1.0 == dim_style.DimScale()))
      {
        V6_text_object->SetDimScale(&dim_style, 1.0);
      }
    }
    else
    {
      if (!(annotation_context->AnnotationSettings().WorldViewTextScale() == dim_style.DimScale()))
      {
        V6_text_object->SetDimScale(&dim_style, annotation_context->AnnotationSettings().WorldViewTextScale());
      }
    }
  }
  V6_text_object->Internal_SetObsoleteV5TextObjectInformation(
    annotation_context,
    V5_text_object
  );

  // updates any m_overrides to be current and updates content hash.
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();
  parent_dim_style.ContentHash();
  V6_text_object->DimensionStyle(parent_dim_style).ContentHash();

  return V6_text_object;
}

MYON_OBSOLETE_V5_TextObject* MYON_OBSOLETE_V5_TextObject::CreateFromV6TextObject(
  const class MYON_Text& V6_text_object,
  const class MYON_3dmAnnotationContext* annotation_context,
  MYON_OBSOLETE_V5_TextObject* destination
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;
  const MYON_TextContent* textgeom = V6_text_object.Text();
  if (nullptr == textgeom)
    return nullptr;

  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();

  MYON::TextHorizontalAlignment halign = MYON::TextHorizontalAlignment::Left;
  MYON::TextVerticalAlignment valign = MYON::TextVerticalAlignment::Top;
  V6_text_object.GetAlignment(halign, valign);
  unsigned int justification = 0;
  switch (halign)
  {
  case MYON::TextHorizontalAlignment::Left:
    justification |= MYON_OBSOLETE_V5_Annotation::eTextJustification::tjLeft;
    break;
  case MYON::TextHorizontalAlignment::Center:
    justification |= MYON_OBSOLETE_V5_Annotation::eTextJustification::tjCenter;
    break;
  case MYON::TextHorizontalAlignment::Right:
    justification |= MYON_OBSOLETE_V5_Annotation::eTextJustification::tjRight;
    break;
  default:
    break;
  }
  switch (valign)
  {
  case MYON::TextVerticalAlignment::Top:
    justification |= MYON_OBSOLETE_V5_Annotation::eTextJustification::tjTop;
    break;
  case MYON::TextVerticalAlignment::Middle:
    justification |= MYON_OBSOLETE_V5_Annotation::eTextJustification::tjMiddle;
    break;
  case MYON::TextVerticalAlignment::Bottom:
    justification |= MYON_OBSOLETE_V5_Annotation::eTextJustification::tjBottom;
    break;
  default:
    break;
  }

  MYON_OBSOLETE_V5_TextObject* V5_text_object
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_TextObject();

  V5_text_object->SetPlane(V6_text_object.Plane());

#ifdef RH41840
  const wchar_t* V6textfacename = V6_text_object.Font(&parent_dim_style).FontFaceName();
  const wchar_t* runfacename = nullptr;
  const MYON_TextContent* text = V6_text_object.Text();
  if (nullptr != text)
  {
    MYON_TextRunArray* runs = text->TextRuns(true);
    if (nullptr != runs)
    {
      int runcount = runs->Count();
      for (int i = 0; i < runcount; i++)
      {
        const wchar_t* run_facename = (*runs)[i]->Font()->FontFaceName();
        if (0 != wcscmp(V6textfacename, run_facename))
        {
          ;  // Some run on this text uses a font different than the style font
        }
      }
    }
  }
#endif

  if (textgeom->HasWrappedRuns())
  {
    // RH-64471 - WrappedPlainTextWithFields() adds multiple copies of the entire
    // text string for each wrapped run.
    //const MYON_wString plaintextFields = textgeom->WrappedPlainTextWithFields();
    //V5_text_object->SetTextFormula(plaintextFields);
    const MYON_wString plaintext = textgeom->WrappedPlainText();
    V5_text_object->SetTextValue(plaintext);
  }
  else
  {
    const MYON_wString plaintextFields = textgeom->PlainTextWithFields();
    V5_text_object->SetTextFormula(plaintextFields);
    const MYON_wString plaintext = textgeom->PlainText();
    V5_text_object->SetTextValue(plaintext);
  }

  V5_text_object->SetV5_3dmArchiveDimStyleIndex( annotation_context->V5_ArchiveDimStyleIndex() );
  V5_text_object->m_textheight = V6_text_object.TextHeight(&parent_dim_style);
  V5_text_object->SetAnnotativeScaling(true);
  V5_text_object->SetJustification(justification);

  V5_text_object->m_annotative_scale = V6_text_object.AllowTextScaling();

  if (MYON::active_space::model_space == annotation_context->ViewContext())
  {
    double v5_text_entity_scale = 1.0;
    if (V6_text_object.AllowTextScaling())
    {
      const MYON_3dmAnnotationSettings& annotation_settings = annotation_context->AnnotationSettings();
      if (annotation_settings.Is_V5_AnnotationScalingEnabled()
        && MYON_IsValid(annotation_settings.m_dimscale)
        && annotation_settings.m_dimscale > 0.0
        )
      {
        //v5_text_entity_scale = annotation_settings.m_dimscale;
        v5_text_entity_scale = annotation_settings.WorldViewTextScale();
      }

      if (annotation_settings.IsModelSpaceAnnotationScalingEnabled())
      {
        const double dim_style_scale = V6_text_object.DimScale(&parent_dim_style);
        if (MYON_IsValid(dim_style_scale) && dim_style_scale > 0.0 && v5_text_entity_scale > 0.0 && dim_style_scale != v5_text_entity_scale)
        {
          const double scale = dim_style_scale / v5_text_entity_scale;
          V5_text_object->m_textheight *= scale;
        }
      }
    }
  }

  for (;;)
  {
    if (nullptr == annotation_context)
      break;
    if (false == annotation_context->BinaryArchiveIsSet())
      break;
    const MYON_BinaryArchive* archive = annotation_context->BinaryArchive();
    if (nullptr == archive)
      break;
    if (archive->Archive3dmVersion() > 4)
      break;
    if (MYON::archive_mode::write3dm != archive->Mode())
      break;

    // TODO 
    // 16 Nov, 2011 - Lowell - Change text to bottom left justified for pre-v5 files rr94270
    //
    // TODO for saveas V4 when we can test it somehow
    //
    ////MYON_2dPoint ll, lr, ur, ul;
    ////MYON_3dPoint pt3;
    ////...->GetText2dCorners(ll, lr, ur, ul);
    ////double h = 0.0, v = 0.0;
    ////unsigned int tj = pTob->Justification();
    ////if(tj & MYON_OBSOLETE_V5_Annotation::tjLeft)
    ////  h = 0.0;
    ////else if(tj & MYON_OBSOLETE_V5_Annotation::tjCenter)
    ////  h = (ll.x - lr.x) / 2.0;
    ////else if(tj & MYON_OBSOLETE_V5_Annotation::tjRight)
    ////  h = ll.x - lr.x;
    ////if(tj & MYON_OBSOLETE_V5_Annotation::tjBottom)
    ////  v = 0.0;
    ////else if(tj & MYON_OBSOLETE_V5_Annotation::tjMiddle)
    ////  v = (ll.y - ul.y) / 2.0;
    ////else if(tj & MYON_OBSOLETE_V5_Annotation::tjTop)
    ////  v = ll.y - ul.y;

    ////pt3 = V5_text_object->Plane().PointAt(h, v);
    ////MYON_Plane plane = V5_text_object->Plane();
    ////plane.origin = pt3;
    ////plane.UpdateEquation();
    ////V5_text_object->SetPlane(plane);
    ////V5_text_object->SetJustification(MYON_OBSOLETE_V5_Annotation::tjBottomLeft);
    break;
  }          



  return V5_text_object;
}

MYON_OBSOLETE_V5_TextObject* MYON_OBSOLETE_V5_TextObject::CreateFromV2TextObject(
  const class MYON_OBSOLETE_V2_TextObject& V2_text_object,
  const class MYON_3dmAnnotationContext* annotation_context,
  MYON_OBSOLETE_V5_TextObject* destination
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  const MYON_3dmAnnotationSettings& annotation_settings = annotation_context->AnnotationSettings();
  const bool bAnnotationSettingsAreSet = annotation_context->AnnotationSettingsAreSet();

  const MYON_DimStyle dim_style = annotation_context->DimStyle();
  const bool bDimStyleIsSet = annotation_context->DimStyleIsSet();

  MYON_OBSOLETE_V5_TextObject* V5_text_object
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_TextObject();
  V5_text_object->Internal_InitializeFromV2Annotation(V2_text_object, annotation_context);
  V5_text_object->m_type = MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtTextBlock;

  const double dim_style_dim_scale
    = bDimStyleIsSet
    ? dim_style.DimScale()
    : 1.0;

  const double V2_dim_scale
    = (bAnnotationSettingsAreSet && MYON_IsValid(annotation_settings.m_dimscale) && annotation_settings.m_dimscale > 0.0)
    ? annotation_settings.m_dimscale
    : 1.0;

  //const double dim_scale
  //  = (dim_style_dim_scale != V2_dim_scale && dim_style_dim_scale > 0.0 && V2_dim_scale > 0.0)
  //  ? V2_dim_scale / dim_style_dim_scale
  //  : 1.0;

  const double model_view_text_scale
    = (bAnnotationSettingsAreSet && annotation_settings.Is_V5_AnnotationScalingEnabled() && annotation_settings.WorldViewTextScale() > 0.0)
    ? annotation_settings.WorldViewTextScale()
    : 1.0;

  const double V2_to_V5_text_scale
    = (model_view_text_scale > 0.0 && V2_dim_scale > 0.0 && dim_style_dim_scale > 0.0)
    ? model_view_text_scale*(V2_dim_scale / dim_style_dim_scale)
    : 1.0;


  V5_text_object->SetHeight(V2_text_object.Height()*V2_to_V5_text_scale);

  // Adjust from top-left justified to baseline-justified
  // 13 November 2006 Dale Lear
  //    The code that was here only worked for text entities
  //    in the x-y plane.
  //plane.origin.y += -1.11 * pT->Height() * plane.yaxis.y;
  //plane.origin.x += 1.11 * pT->Height() * plane.xaxis.y;

  // works for any plane
  MYON_Plane plane = V2_text_object.m_plane;
  plane.origin += -1.11 * (V2_text_object.Height()*V2_to_V5_text_scale) * plane.yaxis;
  plane.UpdateEquation();
  V5_text_object->SetPlane(plane);


  return V5_text_object;

}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////
//
// leader
//
#pragma region

MYON_Leader* MYON_Leader::CreateFromV5Leader(
  const class MYON_OBSOLETE_V5_Leader& V5_leader,
  const MYON_3dmAnnotationContext* annotation_context,
  MYON_Leader* destination
  )
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;
  const MYON_DimStyle& dim_style = annotation_context->DimStyle();

  const int pointcount = V5_leader.PointCount();

  MYON_3dPointArray points(pointcount);
  if (0 == pointcount)
    points.Append(MYON_3dPoint::Origin);
  else
  {
    for (int i = 0; i < pointcount; i++)
    {
      points.Append(V5_leader.Dim3dPoint(i));
    }
  }

  const MYON_Plane plane = V5_leader.Plane();
  MYON_wString ldrtext = V5_leader.TextFormula();
  if (ldrtext.IsEmpty())
    ldrtext = V5_leader.TextValue();

  MYON_Leader* V6_leader = (nullptr != destination)
    ? destination
    : new MYON_Leader;

  double wrapwidth = 0.0;
  MYON_wString newtxt;
  bool wraptext = RemoveV5Wrapping(ldrtext, dim_style, wrapwidth, newtxt);
  newtxt.Replace(L"\\", L"\\\\");

  V6_leader->Create(newtxt, &dim_style, points.Count(), points, plane, wraptext,  wrapwidth);

  V6_leader->Internal_SetDimStyleFromV5Annotation(V5_leader,annotation_context);
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();

  //MYON_2dVector textdir;
  //if (V5_leader.GetTextDirection(textdir))
  //{
  //  MYON_3dVector dir;
  //  dir.Set(textdir.x, textdir.y, 0.0);
  //  if (0.0 > dir * plane.xaxis)
  //  {
  //    // override
  //    V6_leader->SetLeaderTextHorizontalAlignment(&parent_dim_style, MYON::TextHorizontalAlignment::Right);
  //  }
  //}
  
  // MYON::TextHorizontalAlignment::Auto behaves like all V5 leaders did
  V6_leader->SetLeaderTextHorizontalAlignment(&parent_dim_style, MYON::TextHorizontalAlignment::Auto);


  // updates any m_overrides to be current and updates content hash.
  parent_dim_style.ContentHash();
  V6_leader->DimensionStyle(parent_dim_style).ContentHash();

  return V6_leader;
}

MYON_OBSOLETE_V5_Leader* MYON_OBSOLETE_V5_Leader::CreateFromV6Leader(
  const class MYON_Leader& V6_leader,
  const MYON_3dmAnnotationContext* annotation_context,
  MYON_OBSOLETE_V5_Leader* destination
  )
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;
  //const MYON_DimStyle& dim_style = annotation_context->DimStyle();
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();
  const MYON_DimStyle& dim_style = V6_leader.DimensionStyle(parent_dim_style);

  MYON::TextVerticalAlignment valign = V6_leader.TextVerticalAlignment(&parent_dim_style);
  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode mode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kInLine;
  if (MYON::TextVerticalAlignment::Bottom == valign)
    mode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine;

  int pointcount = V6_leader.PointCount();
  MYON_2dPointArray dimpoints(pointcount+1);
  for (int i = 0; i < pointcount; i++)
  {
    V6_leader.Point2d(i, dimpoints.AppendNew());
  }
  const double dimscale = V6_leader.DimScale(&parent_dim_style);
  const bool bLeaderHasLanding = V6_leader.LeaderHasLanding(&parent_dim_style);
  if (bLeaderHasLanding)
  {
    MYON_Line ll;
    V6_leader.LandingLine2d(&dim_style, dimscale, ll);
    dimpoints.AppendNew() = ll.to;
  }

  MYON_OBSOLETE_V5_Leader* V5_leader
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_Leader();

  V5_leader->SetTextDisplayMode(mode);
  V5_leader->SetPlane(V6_leader.Plane());
  V5_leader->SetPoints(dimpoints);

  const MYON_TextContent* textgeom = V6_leader.Text();
  if (nullptr != textgeom)
  {
    if (textgeom->HasWrappedRuns())
    {
      const MYON_wString plaintextFields = textgeom->WrappedPlainTextWithFields();
      V5_leader->SetTextFormula(plaintextFields);
      const MYON_wString plaintext = textgeom->WrappedPlainText();
      V5_leader->SetTextValue(plaintext);
    }
    else
    {
      const MYON_wString plaintextFields = textgeom->PlainTextWithFields();
      V5_leader->SetTextFormula(plaintextFields);
      const MYON_wString plaintext = textgeom->PlainText();
      V5_leader->SetTextValue(plaintext);
    }
  }

  V5_leader->m_textheight = V6_leader.TextHeight(&parent_dim_style);
  V5_leader->SetV5_3dmArchiveDimStyleIndex( annotation_context->V5_ArchiveDimStyleIndex() );

  return V5_leader;
}

MYON_OBSOLETE_V2_Leader* MYON_OBSOLETE_V2_Leader::CreateFromV5Leader(
  const class MYON_OBSOLETE_V5_Leader& V5_leader,
  const MYON_3dmAnnotationContext* annotation_context,
  MYON_OBSOLETE_V2_Leader* destination
)
{
  MYON_OBSOLETE_V2_Leader* V2_leader 
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V2_Leader();  

  V2_leader->Internal_InitializeFromV5Annotation(V5_leader,annotation_context);

  return V2_leader;
}


MYON_OBSOLETE_V5_Leader* MYON_OBSOLETE_V5_Leader::CreateFromV2Leader(
  const class MYON_OBSOLETE_V2_Leader& V2_leader,
  const MYON_3dmAnnotationContext* annotation_context,
  MYON_OBSOLETE_V5_Leader* destination
)
{
  MYON_OBSOLETE_V5_Leader* V5_leader 
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_Leader();    
  V5_leader->Internal_InitializeFromV2Annotation(V2_leader, annotation_context );
  V5_leader->m_type = MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtLeader;

  // sanity check
  for (int i = V5_leader->m_points.Count()-1; i >= 0; i-- )
  {
    if (false == V5_leader->m_points[i].IsValid())
    {
      if (0 == i)
        V5_leader->m_points[0] = MYON_2dPoint::Origin;
      else
        V5_leader->m_points.Remove(i);
    }
  }

  const double too_close_tol = MYON_SQRT_EPSILON;

  for (int i = V5_leader->m_points.Count() - 1; i >= 1; i--)
  {
    if (V5_leader->m_points[i].DistanceTo(V5_leader->m_points[i - 1]) <= too_close_tol)
    {
      if (1 == i )
      {
        if (V5_leader->m_points.Count() > 2)
          V5_leader->m_points.Remove(1);
      }
      else
        V5_leader->m_points.Remove(i - 1);
    }
  }

  return V5_leader;
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////
//
// linear dimension
//
#pragma region

MYON_OBSOLETE_V5_DimLinear* MYON_OBSOLETE_V5_DimLinear::CreateFromV6DimLinear(
    const class MYON_DimLinear& V6_dim_linear,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_OBSOLETE_V5_DimLinear* destination
  )
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();
  const MYON_DimStyle& xdim_style = V6_dim_linear.DimensionStyle(parent_dim_style);

  MYON_OBSOLETE_V5_DimLinear* V5_dim_linear 
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_DimLinear();

  if (MYON::AnnotationType::Aligned == V6_dim_linear.Type())
    V5_dim_linear->SetType(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAligned);
  else
    V5_dim_linear->SetType(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimLinear);

  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode mode = MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromV6DimStyle(xdim_style);
  V5_dim_linear->SetTextDisplayMode(mode);
  V5_dim_linear->SetPlane(V6_dim_linear.Plane());
  MYON_2dPointArray dimpoints(5);
  dimpoints.AppendNew() = V6_dim_linear.DefPoint1();
  dimpoints.AppendNew() = V6_dim_linear.ArrowPoint1();
  dimpoints.AppendNew() = V6_dim_linear.DefPoint2();
  dimpoints.AppendNew() = V6_dim_linear.ArrowPoint2();
  dimpoints.AppendNew() = V6_dim_linear.TextPoint();
  V5_dim_linear->SetPoints(dimpoints);
  V5_dim_linear->SetUserPositionedText(!V6_dim_linear.UseDefaultTextPoint());
  const MYON_wString plaintext = V6_dim_linear.PlainUserText();
  V5_dim_linear->SetTextValue(plaintext);

  V5_dim_linear->SetV5_3dmArchiveDimStyleIndex( annotation_context->V5_ArchiveDimStyleIndex() );
  V5_dim_linear->m_textheight = V6_dim_linear.TextHeight(&parent_dim_style);

  MYON_OBSOLETE_V5_DimExtra* extra = MYON_OBSOLETE_V5_DimExtra::DimensionExtension(V5_dim_linear, true);
  if (nullptr != extra)
  {
    extra->SetArrowPosition(0);
    extra->SetDetailMeasured(V6_dim_linear.DetailMeasured());    
    extra->SetDistanceScale(V6_dim_linear.DistanceScale());
  }

  return V5_dim_linear;
}

MYON_DimLinear* MYON_DimLinear::CreateFromV5DimLinear(
    const class MYON_OBSOLETE_V5_DimLinear& V5_dim_linear,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_DimLinear* destination
  )
{
  MYON::AnnotationType v6type;
  MYON_INTERNAL_OBSOLETE::V5_eAnnotationType dimtype = V5_dim_linear.Type();
  if (dimtype == MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAligned)
    v6type = MYON::AnnotationType::Aligned;
  else  if (dimtype == MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimLinear)
    v6type = MYON::AnnotationType::Rotated;
  else
    return nullptr;

  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  MYON_DimLinear* V6_dim_linear
    = (nullptr != destination)
    ? destination
    : new MYON_DimLinear();

  MYON_2dPoint arrow1 = V5_dim_linear.Point(MYON_OBSOLETE_V5_DimLinear::POINT_INDEX::arrow0_pt_index);
  MYON_2dPoint arrow2 = V5_dim_linear.Point(MYON_OBSOLETE_V5_DimLinear::POINT_INDEX::arrow1_pt_index);
  MYON_2dPoint defpt1 = V5_dim_linear.Point(MYON_OBSOLETE_V5_DimLinear::POINT_INDEX::ext0_pt_index);
  MYON_2dPoint defpt2 = V5_dim_linear.Point(MYON_OBSOLETE_V5_DimLinear::POINT_INDEX::ext1_pt_index);
  //MYON_2dPoint textpt = V5_dim_linear.Point(MYON_OBSOLETE_V5_DimLinear::POINT_INDEX::userpositionedtext_pt_index);
  MYON_Plane plane = V5_dim_linear.Plane();
  MYON_wString dimtext = V5_dim_linear.TextValue();

  MYON_3dPoint def1point = plane.PointAt(defpt1.x, defpt1.y);
  MYON_3dPoint def2point = plane.PointAt(defpt2.x, defpt2.y);
  MYON_3dPoint arrow1point = plane.PointAt(arrow1.x, arrow1.y);
  MYON_3dPoint arrow2point = plane.PointAt(arrow2.x, arrow2.y);
  MYON_3dPoint dimlinepoint = (arrow1point + arrow2point) / 2.0;
  //MYON_3dPoint textpoint = plane.PointAt(textpt.x, textpt.y);
  MYON_3dVector horizontal = MYON_3dVector::XAxis;

  const MYON_UUID dim_style_id = annotation_context->ParentDimStyleId();
  V6_dim_linear->Create(v6type, dim_style_id, plane, horizontal, def1point, def2point, dimlinepoint, 0.0);
  V6_dim_linear->Internal_SetDimStyleFromV5Annotation(V5_dim_linear,annotation_context);
  V6_dim_linear->Set2dTextPoint(V5_dim_linear.Point(MYON_OBSOLETE_V5_DimLinear::POINT_INDEX::userpositionedtext_pt_index));
  V6_dim_linear->SetUseDefaultTextPoint(!V5_dim_linear.UserPositionedText());
  MYON_wString usrtext = V5_dim_linear.TextFormula();
  usrtext.Replace(L"\\", L"\\\\");
  V6_dim_linear->SetUserText(usrtext.Array());

  const MYON_OBSOLETE_V5_DimExtra* extra = MYON_OBSOLETE_V5_DimExtra::DimensionExtension(const_cast<MYON_OBSOLETE_V5_DimLinear*>(&V5_dim_linear), false);
  if(nullptr != extra)
  {
    V6_dim_linear->SetDetailMeasured(extra->DetailMeasured());
    V6_dim_linear->SetDistanceScale(extra->DistanceScale());        
  }

  // updates any m_overrides to be current and updates content hash.
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();
  parent_dim_style.ContentHash();
  V6_dim_linear->DimensionStyle(parent_dim_style).ContentHash();

  return V6_dim_linear;
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////
//
// angular dimension
//
#pragma region

MYON_OBSOLETE_V5_DimAngular* MYON_OBSOLETE_V5_DimAngular::CreateFromV6DimAngular(
    const class MYON_DimAngular& V6_dim_angular,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_OBSOLETE_V5_DimAngular* destination
  )
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();

  MYON_OBSOLETE_V5_DimAngular* V5_dim_angle 
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_DimAngular();


  V5_dim_angle->SetType(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAngular);


  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode V5_mode = MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromV6DimStyle(V6_dim_angular.DimensionStyle(parent_dim_style));
  V5_dim_angle->SetTextDisplayMode(V5_mode);

  MYON_3dPoint center, def1, def2, ap1, ap2, dimline, textpt;
  MYON_3dVector normal = V6_dim_angular.Plane().Normal();

  V6_dim_angular.Get3dPoints(&center, &def1, &def2, &ap1, &ap2, &dimline, &textpt);
  V5_dim_angle->CreateFromPoints(center, def1, def2, dimline, normal);

  V5_dim_angle->SetUserPositionedText(!V6_dim_angular.UseDefaultTextPoint());
  if (!V6_dim_angular.UseDefaultTextPoint())
  {
    MYON_2dPoint tp2;
    V6_dim_angular.Plane().ClosestPointTo(textpt, &tp2.x, &tp2.y);
    V5_dim_angle->SetPoint(MYON_OBSOLETE_V5_DimAngular::userpositionedtext_pt_index, tp2);
  }
  MYON_wString usertext = V6_dim_angular.PlainUserText();
  usertext.Replace(L"\\", L"\\\\");
  V5_dim_angle->SetTextFormula(usertext);

  V5_dim_angle->m_textheight = V6_dim_angular.TextHeight(&parent_dim_style);
  V5_dim_angle->SetV5_3dmArchiveDimStyleIndex( annotation_context->V5_ArchiveDimStyleIndex() );

  return V5_dim_angle;
};

MYON_DimAngular* MYON_DimAngular::CreateFromV5DimAngular(
    const class MYON_OBSOLETE_V5_DimAngular& V5_dim_angle,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_DimAngular* destination
  )
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  const MYON_UUID dim_style_id = annotation_context->ParentDimStyleId();

  MYON_3dPoint extension1 = V5_dim_angle.Dim3dPoint(MYON_OBSOLETE_V5_DimAngular::POINT_INDEX::extension0_pt);
  MYON_3dPoint extension2 = V5_dim_angle.Dim3dPoint(MYON_OBSOLETE_V5_DimAngular::POINT_INDEX::extension1_pt);
  MYON_3dPoint arrow1 = V5_dim_angle.Dim3dPoint(MYON_OBSOLETE_V5_DimAngular::POINT_INDEX::arcstart_pt);
  MYON_3dPoint arrow2 = V5_dim_angle.Dim3dPoint(MYON_OBSOLETE_V5_DimAngular::POINT_INDEX::arcend_pt);
  MYON_3dPoint arc_center = V5_dim_angle.Dim3dPoint(MYON_OBSOLETE_V5_DimAngular::POINT_INDEX::arccenter_pt);
  MYON_3dPoint arc_point = V5_dim_angle.Dim3dPoint(MYON_OBSOLETE_V5_DimAngular::POINT_INDEX::arcmid_pt);
  MYON_2dPoint text_center = V5_dim_angle.Dim2dPoint(MYON_OBSOLETE_V5_DimAngular::POINT_INDEX::text_pivot_pt);
  MYON_3dVector horizontal = MYON_3dVector::XAxis;

  MYON_DimAngular* V6_dim_angle
    = (nullptr != destination)
    ? destination
    : new MYON_DimAngular();
  
  if (arrow1.DistanceTo(extension1) > MYON_SQRT_EPSILON && arrow2.DistanceTo(extension2) > MYON_SQRT_EPSILON)
    V6_dim_angle->Create(dim_style_id, V5_dim_angle.Plane(), horizontal, extension1, extension2, arrow1, arrow2, arc_point);
  else
    V6_dim_angle->Create(dim_style_id, V5_dim_angle.Plane(), horizontal, arc_center, arrow1, arrow2, arc_point);

  V6_dim_angle->Internal_SetDimStyleFromV5Annotation(V5_dim_angle, annotation_context);

  if (V5_dim_angle.UserPositionedText())
    V6_dim_angle->Set2dTextPoint(text_center);
  V6_dim_angle->SetUseDefaultTextPoint(!V5_dim_angle.UserPositionedText());
  MYON_wString usrtext = V5_dim_angle.TextFormula();
  usrtext.Replace(L"\\", L"\\\\");
  if (usrtext.Length() > 1 && usrtext[usrtext.Length()-1] == MYON_DegreeSymbol)
    usrtext.SetLength(usrtext.Length() - 1);
  V6_dim_angle->SetUserText(usrtext.Array());

  // updates any m_overrides to be current and updates content hash.
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();
  parent_dim_style.ContentHash();
  V6_dim_angle->DimensionStyle(parent_dim_style).ContentHash();

  return V6_dim_angle;
}

#pragma endregion


/////////////////////////////////////////////////////////////////////////////
//
// radial dimension
//
#pragma region
MYON_OBSOLETE_V5_DimRadial* MYON_OBSOLETE_V5_DimRadial::CreateFromV6DimRadial(
  const class MYON_DimRadial& V6_dim_radial,
  const class MYON_3dmAnnotationContext* annotation_context,
  MYON_OBSOLETE_V5_DimRadial* destination
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();

  MYON_OBSOLETE_V5_DimRadial* V5_dim_radial
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_DimRadial();

  if (MYON::AnnotationType::Radius == V6_dim_radial.Type())
    V5_dim_radial->SetType(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimRadius);
  else
    V5_dim_radial->SetType(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimDiameter);

  //MYON::TextVerticalAlignment valign = dimstyle->TextVerticalAlignment();
  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode mode = MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromV6DimStyle(V6_dim_radial.DimensionStyle(parent_dim_style));

  V5_dim_radial->SetTextDisplayMode(mode);
  V5_dim_radial->SetPlane(V6_dim_radial.Plane());
  MYON_2dPointArray dimpoints(4);
  dimpoints.AppendNew() = V6_dim_radial.CenterPoint();
  dimpoints.AppendNew() = V6_dim_radial.RadiusPoint();
  dimpoints.AppendNew() = V6_dim_radial.DimlinePoint();
  dimpoints.AppendNew() = V6_dim_radial.KneePoint();
  
  double ll = 0.0;
  const bool bLeaderHasLanding = V6_dim_radial.LeaderHasLanding(&parent_dim_style);
  const double dim_scale = V6_dim_radial.DimScale(&parent_dim_style);
  const double text_height = V6_dim_radial.TextHeight(&parent_dim_style);

  if( bLeaderHasLanding )
    ll = V6_dim_radial.LeaderLandingLength(&parent_dim_style) * dim_scale;
  else if (fabs(dimpoints[3].x - dimpoints[2].x) < MYON_ZERO_TOLERANCE)
    ll = text_height * dim_scale;

  if (dimpoints[3].x >= dimpoints[1].x)
    dimpoints[2].x += ll;
  else
    dimpoints[2].x -= ll;

  V5_dim_radial->SetPoints(dimpoints);
  V5_dim_radial->SetUserPositionedText(!V6_dim_radial.UseDefaultTextPoint());
  
  const MYON_wString plaintext = V6_dim_radial.PlainUserText();
  V5_dim_radial->SetTextValue(plaintext);
  
  V5_dim_radial->m_textheight = text_height;

  V5_dim_radial->SetV5_3dmArchiveDimStyleIndex(annotation_context->V5_ArchiveDimStyleIndex() );

  MYON_OBSOLETE_V5_DimExtra* extra = MYON_OBSOLETE_V5_DimExtra::DimensionExtension(V5_dim_radial, true);
  if (nullptr != extra)
  {
    extra->SetDetailMeasured(V6_dim_radial.DetailMeasured());
    extra->SetDistanceScale(V6_dim_radial.DistanceScale());
  }

  return V5_dim_radial;
};

MYON_DimRadial* MYON_DimRadial::CreateFromV5DimRadial(
    const class MYON_OBSOLETE_V5_DimRadial& V5_radial_dimension,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_DimRadial* destination
  )
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();

  MYON_DimRadial* V6_dim_radial
    = (nullptr != destination)
    ? destination
    : new MYON_DimRadial();

  MYON::AnnotationType radial_type = (MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimRadius == V5_radial_dimension.Type())
    ? MYON::AnnotationType::Radius
    : MYON::AnnotationType::Diameter;


  const MYON_Plane& plane = V5_radial_dimension.Plane();
  MYON_2dPoint center_pt = V5_radial_dimension.Point(MYON_OBSOLETE_V5_DimRadial::POINT_INDEX::center_pt_index);
  MYON_2dPoint arrow_pt = V5_radial_dimension.Point(MYON_OBSOLETE_V5_DimRadial::POINT_INDEX::arrow_pt_index);
  MYON_2dPoint tail_pt = V5_radial_dimension.Point(MYON_OBSOLETE_V5_DimRadial::POINT_INDEX::tail_pt_index);
  //MYON_2dPoint knee_pt = V5_radial_dimension.Point(MYON_OBSOLETE_V5_DimRadial::POINT_INDEX::knee_pt_index);
  const MYON_UUID styleid = parent_dim_style.Id();

  MYON_3dPoint center_point = plane.PointAt(center_pt.x, center_pt.y);
  MYON_3dPoint radius_point = plane.PointAt(arrow_pt.x, arrow_pt.y);
  //MYON_3dPoint knee_point = plane.PointAt(knee_pt.x, knee_pt.y);
  MYON_3dPoint dimline_point = plane.PointAt(tail_pt.x, tail_pt.y);

  V6_dim_radial->Create(radial_type, styleid, plane, center_point, radius_point, dimline_point);
  V6_dim_radial->Internal_SetDimStyleFromV5Annotation(V5_radial_dimension,annotation_context);


  V6_dim_radial->SetUseDefaultTextPoint(!V5_radial_dimension.UserPositionedText());
  MYON_wString usrtext = V5_radial_dimension.TextFormula();
  usrtext.Replace(L"\\", L"\\\\");
  V6_dim_radial->SetUserText(usrtext.Array());
  
  const MYON_OBSOLETE_V5_DimExtra* extra = MYON_OBSOLETE_V5_DimExtra::DimensionExtension(const_cast<MYON_OBSOLETE_V5_DimRadial*>(&V5_radial_dimension), false);
  if (nullptr != extra)
  {
    V6_dim_radial->SetDetailMeasured(extra->DetailMeasured());
    V6_dim_radial->SetDistanceScale(extra->DistanceScale());
  }

  // V5 radial dimensions had this behavior which is non-default in V6
  V6_dim_radial->SetDimTextLocation(&parent_dim_style, MYON_DimStyle::TextLocation::InDimLine);
  V6_dim_radial->SetLeaderContentAngleStyle(&parent_dim_style, MYON_DimStyle::ContentAngleStyle::Horizontal);

  // updates any m_overrides to be current and updates content hash.
  parent_dim_style.ContentHash();
  V6_dim_radial->DimensionStyle(parent_dim_style).ContentHash();

  return V6_dim_radial;
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////
//
// ordinate dimension
//
#pragma region

MYON_OBSOLETE_V5_DimOrdinate* MYON_OBSOLETE_V5_DimOrdinate::CreateFromV6DimOrdinate(
  const class MYON_DimOrdinate& V6_dim_ordinate,
  const class MYON_3dmAnnotationContext* annotation_context,
  MYON_OBSOLETE_V5_DimOrdinate* destination
  )
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();

  MYON_OBSOLETE_V5_DimOrdinate* V5_dim_ordinate 
    = (nullptr != destination)
    ? destination
    : new MYON_OBSOLETE_V5_DimOrdinate();

  MYON_OBSOLETE_V5_DimExtra* extra = MYON_OBSOLETE_V5_DimExtra::DimensionExtension(V5_dim_ordinate, true);
    V5_dim_ordinate->SetType(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimOrdinate);

  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode mode = MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromV6DimStyle(V6_dim_ordinate.DimensionStyle(parent_dim_style));
  V5_dim_ordinate->SetTextDisplayMode(mode);
  V5_dim_ordinate->SetPlane(V6_dim_ordinate.Plane());
  MYON_2dPointArray dimpoints(2);
  dimpoints.AppendNew() = V6_dim_ordinate.DefPt();
  dimpoints.AppendNew() = V6_dim_ordinate.LeaderPt();
  V5_dim_ordinate->SetPoints(dimpoints);
  V5_dim_ordinate->SetUserPositionedText(!V6_dim_ordinate.UseDefaultTextPoint());

  const MYON_wString plaintext = V6_dim_ordinate.PlainUserText();
  V5_dim_ordinate->SetTextValue(plaintext);

  V5_dim_ordinate->SetV5_3dmArchiveDimStyleIndex( annotation_context->V5_ArchiveDimStyleIndex() );
  V5_dim_ordinate->m_textheight = V6_dim_ordinate.TextHeight(&parent_dim_style);
  V5_dim_ordinate->SetKinkOffset(0, V6_dim_ordinate.KinkOffset1());
  V5_dim_ordinate->SetKinkOffset(1, V6_dim_ordinate.KinkOffset2());
  if (nullptr != extra)
  {
    extra->SetDetailMeasured(V6_dim_ordinate.DetailMeasured());
    extra->SetDistanceScale(V6_dim_ordinate.DistanceScale());
  }

  return V5_dim_ordinate;
};

MYON_DimOrdinate* MYON_DimOrdinate::CreateFromV5DimOrdinate(
  const class MYON_OBSOLETE_V5_DimOrdinate& v5_dim_ordinate,
  const class MYON_3dmAnnotationContext* annotation_context,
  MYON_DimOrdinate* destination
)
{
  if (nullptr == annotation_context)
    annotation_context = &MYON_3dmAnnotationContext::Default;

  MYON_DimOrdinate* V6_dim_ordinate 
    = (nullptr != destination)
    ? destination
    : new MYON_DimOrdinate();

  const MYON_UUID dim_style_id = annotation_context->ParentDimStyleId();

  if (annotation_context->DimStyleIsSet())
    V6_dim_ordinate->SetDimensionStyleId(dim_style_id);

  const MYON_Plane& plane = v5_dim_ordinate.Plane();
  MYON_3dPoint def_pt = v5_dim_ordinate.Dim3dPoint(MYON_OBSOLETE_V5_DimOrdinate::POINT_INDEX::definition_pt_index);
  MYON_3dPoint leader_pt = v5_dim_ordinate.Dim3dPoint(MYON_OBSOLETE_V5_DimOrdinate::POINT_INDEX::leader_end_pt_index);
  double koffset1 = v5_dim_ordinate.KinkOffset(0);
  double koffset2 = v5_dim_ordinate.KinkOffset(1);
  MYON_DimOrdinate::MeasuredDirection  direction = MYON_DimOrdinate::MeasuredDirection::Xaxis;
  switch (v5_dim_ordinate.Direction())
  {
  case -1:
    {
      const MYON_2dPoint& p0 = v5_dim_ordinate.m_points[0];
      const MYON_2dPoint& p1 = v5_dim_ordinate.m_points[1];
      if (fabs(p1.x - p0.x) <= fabs(p1.y - p0.y))
        direction = MYON_DimOrdinate::MeasuredDirection::Xaxis; // measures along x axis
      else
        direction = MYON_DimOrdinate::MeasuredDirection::Yaxis; // measures along y axis
    }
    break;
  case 0:
    direction = MYON_DimOrdinate::MeasuredDirection::Xaxis;
    break;
  case 1:
    direction = MYON_DimOrdinate::MeasuredDirection::Yaxis;
    break;
  }
  const MYON_DimStyle& parent_dim_style = annotation_context->ParentDimStyle();
  double k = parent_dim_style.TextHeight() * parent_dim_style.DimScale();
  if (koffset1 < k / 10.0)
    koffset1 = k;
  if (koffset2 < k / 10.0)
    koffset2 = k / 2.0;

  double dist = def_pt.DistanceTo(leader_pt);
  if (koffset1 > dist - 0.05)
    koffset1 = dist * 2.0 / 3.0;
  if (koffset2 > dist - koffset1)
    koffset2 = (dist - koffset1) / 2.0;

  V6_dim_ordinate->Create(dim_style_id, plane, direction, plane.origin, def_pt, leader_pt, koffset1, koffset2);
  V6_dim_ordinate->Internal_SetDimStyleFromV5Annotation(v5_dim_ordinate,annotation_context);
  V6_dim_ordinate->SetDimTextLocation(&parent_dim_style, MYON_DimStyle::TextLocation::InDimLine);
  
  const MYON_OBSOLETE_V5_DimExtra* extra = MYON_OBSOLETE_V5_DimExtra::DimensionExtension(const_cast<MYON_OBSOLETE_V5_DimOrdinate*>(&v5_dim_ordinate), false);
  if (nullptr != extra)
  {
    V6_dim_ordinate->SetDetailMeasured(extra->DetailMeasured());
    V6_dim_ordinate->SetDistanceScale(extra->DistanceScale());
  }

  MYON_wString usrtext = v5_dim_ordinate.TextFormula();
  usrtext.Replace(L"\\", L"\\\\");
  V6_dim_ordinate->SetUserText(usrtext.Array());
  
  // updates any m_overrides to be current and updates content hash.
  parent_dim_style.ContentHash();
  V6_dim_ordinate->DimensionStyle(parent_dim_style).ContentHash();

  return V6_dim_ordinate;
}

#pragma endregion

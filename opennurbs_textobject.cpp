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

MYON_OBJECT_IMPLEMENT(MYON_Text, MYON_Annotation, "57376349-62A9-4A16-B411-A46BCD544790");

//-----------------------------------------------------------------

MYON_Text::MYON_Text()
  : MYON_Annotation(MYON::AnnotationType::Text)
{}

MYON_Text::~MYON_Text()
{}

// Duplicates text string and runs
MYON_Text& MYON_Text::operator=(const MYON_Text& src)
{
  if (this != &src)
  {
    MYON_Annotation::operator=(src);
  }
  return *this;
}

MYON_Text::MYON_Text(const MYON_Text& src)
  : MYON_Annotation(src)
{}

bool MYON_Text::Create(
  const wchar_t* RtfString,
  const MYON_DimStyle* dimstyle,
  MYON_Plane plane,
  bool bWrapped,
  double rect_width,
  double text_rotation_radians
)
{
  // Parse string, create runs, find font, set plane & height
  SetPlane(plane);

  dimstyle = &MYON_DimStyle::DimStyleOrDefault(dimstyle);

  SetDimensionStyleId(*dimstyle);

  MYON_TextContent* text = nullptr;
  if (nullptr == RtfString || 0 == RtfString[0])
    RtfString = L"";
  if (nullptr != RtfString)
  {
    text = new MYON_TextContent;
    if (!text->Create( RtfString, Type(), dimstyle, bWrapped, rect_width, text_rotation_radians))
    {
      delete text;
      text = 0;
      return false;
    }
  }
  SetText(text);
  return true;
}

bool MYON_Text::Create(
  const wchar_t* RtfString,
  const MYON_DimStyle* dimstyle,
  MYON_Plane plane
)
{
  bool bWrapped = false;
  const double rect_width = MYON_TextContent::Empty.FormattingRectangleWidth();
  const double text_rotation_radians = MYON_TextContent::Empty.TextRotationRadians();
  return Create(
    RtfString,
    dimstyle,
    plane,
    bWrapped,
    rect_width,
    text_rotation_radians
  );
}

static bool MYON_V6_TextObject_IsNotIsValid()
{
  return false; // <- breakpoint here to detect invalid MYON_TextContent objects.
}

bool MYON_Text::IsValid(MYON_TextLog* text_log) const
{
  bool rc = true;
  if (!m_plane.IsValid())
    rc = MYON_V6_TextObject_IsNotIsValid();
  else if (MYON_nil_uuid == m_dimstyle_id)
    rc = MYON_V6_TextObject_IsNotIsValid();
  else if (nullptr == m_text || !m_text->IsValid())
    rc = MYON_V6_TextObject_IsNotIsValid();
  return rc;
}

// virtual
void MYON_Text::Dump(MYON_TextLog& text_log) const // for debugging
{}

bool MYON_Text::Write(
  MYON_BinaryArchive&  archive
) const
{
  const int content_version = 0;
  if (false == archive.BeginWrite3dmAnonymousChunk(content_version))
    return false;
  bool rc = false;
  for (;;)
  {
    if (false == MYON_Annotation::Internal_WriteAnnotation(archive))
      break;
    rc = true;
    break;
  }
  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_Text::Read(
  MYON_BinaryArchive&  archive
)
{
  *this = MYON_Text::Empty;

  int content_version = 0;
  if (false == archive.BeginRead3dmAnonymousChunk(&content_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (false == MYON_Annotation::Internal_ReadAnnotation(archive))
      break;
    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}

int MYON_Text::Dimension() const
{
  return 3;
}

// MYON_Geometry override
bool MYON_Text::GetBBox( // returns true if successful
  double* bbox_min,       // boxmin[dim]
  double* bbox_max,       // boxmax[dim]
  bool grow          // true means grow box
) const
{
  return GetAnnotationBoundingBox(nullptr, nullptr, 1.0, bbox_min, bbox_max, grow ? true : false);
}


// MYON_Annotation override
bool MYON_Text::GetAnnotationBoundingBox(
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

  const MYON_SHA1_Hash hash = Internal_GetBBox_InputHash(
    vp, 
    dimstyle, 
    dimscale, 
    MYON_2dPoint::Origin,
    0,
    nullptr
  );

  if (Internal_GetBBox_Begin(hash, boxmin, boxmax, bGrow))
    return true;

  if (nullptr == boxmin || nullptr == boxmax)
    return false;

  MYON_BoundingBox bbox;
  Internal_GetBBox_TextGlyphBox(
    vp,
    dimstyle,
    dimscale,
    bbox
  );

  // There is no other geometry on MYON_Text

  return Internal_GetBBox_End(bbox, hash, boxmin, boxmax, bGrow);
}

bool MYON_Text::Transform(const MYON_Xform& xform, const MYON_DimStyle* parent_dimstyle)
{
  MYON_3dVector Y = Plane().Yaxis();
  Y.Transform(xform);
  double scale = Y.Length();
  bool rc = Transform(xform);
  if (rc && MYON_ZERO_TOLERANCE < fabs(scale - 1.0))
  {
    double oldheight = TextHeight(parent_dimstyle);
    double newheight = oldheight * scale;
    SetTextHeight(parent_dimstyle, newheight);
    oldheight = MaskBorder(parent_dimstyle);
    newheight = oldheight * scale;
    SetMaskBorder(parent_dimstyle, newheight);
  }
  return rc;
}

bool MYON_Text::Transform(const MYON_Xform& xform)
{
  bool rc = MYON_Geometry::Transform(xform);
  if (rc)
    rc = m_plane.Transform(xform);
  if (rc)
  {
    MYON_TextContent* text = this->Text();
    if (nullptr != text && text->TextIsWrapped())
    {
      double w = text->FormattingRectangleWidth();
      MYON_3dVector x = m_plane.xaxis;
      if (x.Unitize())
      {
        double r = text->TextRotationRadians();
        x.Rotate(r, m_plane.zaxis);
        x.Transform(xform);
        w *= x.Length();
        text->SetFormattingRectangleWidth(w);
      }
    }
  }
  return rc;
}

// returns the base point and width grip using the current alignments
bool MYON_Text::GetGripPoints(MYON_2dPoint& base, MYON_2dPoint& width, double textscale) const
{
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;

  MYON_2dPoint p[4];
  if (!text->Get2dCorners(p))
    return false;

  MYON::TextHorizontalAlignment halign;
  MYON::TextVerticalAlignment valign;
  GetAlignment(halign, valign);
  base = MYON_2dPoint::Origin;
  width = (p[1] + p[2]) / 2.0;
  if (MYON::TextHorizontalAlignment::Right == halign)
    width = (p[0] + p[3]) / 2.0;

  width.x *= textscale;
  width.y *= textscale;
  double a = TextRotationRadians();
  width.Rotate(a, MYON_2dPoint::Origin);
  return true;
}

bool MYON_Text::GetTextXform(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
)const 
{

  return GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform_out);
}

bool MYON_Text::GetTextXform(
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

bool MYON_Text::GetTextXform(
  const MYON_Xform* model_xform,
  const MYON_3dVector view_x,
  const MYON_3dVector view_y,
  const MYON_3dVector view_z,
  MYON::view_projection projection,
  bool bDrawForward,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  if (nullptr == dimstyle)
    return false;
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;

  if (DimStyleTextPositionPropertiesHash() != dimstyle->TextPositionPropertiesHash())
  {
    MYON_wString rtfstr = text->RtfText();
    MYON_Plane objectplane = Plane();
    const_cast<MYON_TextContent*>(text)->Create(
      rtfstr, MYON::AnnotationType::Text, dimstyle, 
      text->TextIsWrapped(), text->FormattingRectangleWidth(), text->TextRotationRadians());
  }

  text_xform_out = MYON_Xform::IdentityTransformation;

  MYON_Xform textscale_xf(MYON_Xform::DiagonalTransformation(dimscale));
  MYON_Xform wcs2obj_xf(MYON_Xform::IdentityTransformation);           // WCS plane to leader plane rotation
  const MYON_Plane& textobjectplane = Plane();
  wcs2obj_xf.Rotation(MYON_Plane::World_xy, textobjectplane);    // Rotate text from starting text plane (wcs) to object plane
  MYON_Xform rotation_xf(MYON_Xform::IdentityTransformation);

  if ( MYON::TextOrientation::InView == dimstyle->TextOrientation() )  // Draw text horizontal and flat to the screen
  {
    MYON_Xform tp2sxf;        // Text point to view plane rotation
    MYON_3dPoint text_point_3d = Plane().origin;
    MYON_3dVector text_xdir = textobjectplane.xaxis;
    MYON_3dVector text_ydir = textobjectplane.yaxis;
    MYON_3dVector text_zdir = textobjectplane.zaxis;
    if (nullptr != model_xform)
    {
      text_xdir.Transform(*model_xform);
      text_ydir.Transform(*model_xform);
      text_zdir.Transform(*model_xform);
    }
    rotation_xf.Rotation(text_point_3d, text_xdir, text_ydir, text_zdir, text_point_3d, view_x, view_y, view_z);
    text_xform_out = wcs2obj_xf * textscale_xf;
    text_xform_out = rotation_xf * text_xform_out;
    return true;
  }
  else // MYON::TextOrientation::InPlane
  {
    double textrotation = TextRotationRadians();
    if (fabs(textrotation) > MYON_SQRT_EPSILON)
      rotation_xf.Rotation(textrotation, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);  // Text rotation
  
    //MYON_Xform textcenter_xf(MYON_Xform::IdentityTransformation);
    if (bDrawForward)
    {
      // Check if the text is right-reading by comparing
      // text plane x and y, rotated by text rotation angle,
      // to view right and up

      MYON_3dPoint text_corners[4] = { MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin };
      MYON_3dPoint text_center = MYON_3dPoint::Origin;
      if (text->Get3dCorners(text_corners))
      {
        text_center = (text_corners[0] + text_corners[2]) / 2.0;
        MYON_3dVector text_xdir = textobjectplane.xaxis;
        MYON_3dVector text_ydir = textobjectplane.yaxis;
        MYON_3dVector text_zdir = textobjectplane.zaxis;
        if (nullptr != model_xform)
        {
          text_xdir.Transform(*model_xform);
          text_ydir.Transform(*model_xform);
          text_zdir.Transform(*model_xform);
        }
        if (fabs(textrotation) > MYON_SQRT_EPSILON)
        {
          text_xdir.Rotate(textrotation, textobjectplane.zaxis);
          text_ydir.Rotate(textrotation, textobjectplane.zaxis);
        }

        bool flip_x = false;
        bool flip_y = false;

        const double fliptol = (projection == MYON::view_projection::perspective_view) ? 0.0 : cos(80.001 * MYON_DEGREES_TO_RADIANS);
        CalcTextFlip(
          text_xdir, text_ydir, text_zdir,
          view_x, view_y, view_z,
          model_xform,
          fliptol,
          flip_x,
          flip_y);

        MYON_Xform mxf;  // Mirror xform for backwards text to adjust DrawForward
        if (flip_x)
        {
          mxf.Mirror(text_center, MYON_3dVector::XAxis);
          textscale_xf = textscale_xf * mxf;
        }
        if (flip_y)
        {
          mxf.Mirror(text_center, MYON_3dVector::YAxis);
          textscale_xf = textscale_xf * mxf;
        }
      }
    }
    text_xform_out = textscale_xf;
    text_xform_out = rotation_xf * text_xform_out;
    //text_xform_out = textcenter_xf * text_xform_out;
    text_xform_out = wcs2obj_xf * text_xform_out;

    return true;
  }
}





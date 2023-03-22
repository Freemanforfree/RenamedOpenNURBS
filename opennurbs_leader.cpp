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

MYON_OBJECT_IMPLEMENT(MYON_Leader, MYON_Annotation, "945BF594-6FF9-4F5C-BFC0-B3AF528F29D2");

void MYON_Leader::Internal_Destroy()
{
  if (nullptr != m_curve)
  {
    delete m_curve;
    m_curve = nullptr;
  }
}

void MYON_Leader::Internal_CopyFrom(const MYON_Leader& src)
{
  if (nullptr != src.m_curve)
    m_curve = new MYON_NurbsCurve(*src.m_curve);
  m_points = src.m_points;
  m_text_point = src.m_text_point;
}


MYON_Leader::MYON_Leader()
  : MYON_Annotation(MYON::AnnotationType::Leader)
  , m_curve(nullptr)
  , m_text_point(MYON_2dPoint::UnsetPoint)
{
}

MYON_Leader::~MYON_Leader()
{
  Internal_Destroy();
}

MYON_Leader::MYON_Leader(const MYON_Leader& src)
  : MYON_Annotation(src)
{
  if (this != &src)
  {
    Internal_CopyFrom(src);
  }
}

MYON_Leader& MYON_Leader::operator=(const MYON_Leader& src)
{
  if (this != &src)
  {
    Internal_Destroy();
    MYON_Annotation::operator=(src);
    Internal_CopyFrom(src);
  }
  return *this;
}

bool MYON_Leader::IsValid(MYON_TextLog* log) const
{
  return true;
}

void MYON_Leader::Dump(MYON_TextLog& log) const
{
}

bool MYON_Leader::Write(MYON_BinaryArchive& archive) const
{
  const int chunk_version = 1;
  if (!archive.BeginWrite3dmAnonymousChunk(chunk_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (false == MYON_Annotation::Internal_WriteAnnotation(archive))
      break;

    if (!archive.WriteArray(m_points))
      break;

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_Leader::Read(MYON_BinaryArchive& archive)
{
  *this = MYON_Leader::Empty;

  int chunk_version = 0;
  if (!archive.BeginRead3dmAnonymousChunk(&chunk_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (chunk_version < 1)
      break;

    if (false == MYON_Annotation::Internal_ReadAnnotation(archive))
      break;

    if (!archive.ReadArray(m_points))
      break;

    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}


MYON::object_type MYON_Leader::ObjectType() const
{
  return MYON::annotation_object;
}

int MYON_Leader::Dimension() const
{
  return 3;
}

bool MYON_Leader::GetBBox( // returns true if successful
  double* bbox_min,       // boxmin[dim]
  double* bbox_max,       // boxmax[dim]
  bool grow          // true means grow box
  ) const
{
  // Even though this doesn't apply view info properly, it has a side-effect of creating runs
  // so that text query functions will work properly.  
  // I'm adding it back here to make Opennurbs MYONx model support for reading and not displaying
  // files work better wrt text queries
  return GetAnnotationBoundingBox(nullptr, nullptr, 1.0, bbox_min, bbox_max, grow ? true : false);
}

bool MYON_Leader::GetTextXform(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
)const
{
  return GetTextXform(nullptr, vp, dimstyle, dimscale, text_xform_out);
}

bool MYON_Leader::GetTextXform(
  const MYON_Xform* model_xform,
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Xform& text_xform_out
) const
{
  if (nullptr == dimstyle)
    return false;

  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return true;

  const MYON_TextMask::MaskFrame maskframe = dimstyle->MaskFrameType();
  const MYON_DimStyle::ContentAngleStyle textangle_style = dimstyle->LeaderContentAngleStyle();

  MYON_2dVector tail_dir = TailDirection(dimstyle);
  MYON_3dVector view_x = nullptr == vp ? MYON_3dVector::XAxis : vp->CameraX();
  MYON_3dVector view_y = nullptr == vp ? MYON_3dVector::YAxis : vp->CameraY();
  MYON_3dVector view_z = MYON_CrossProduct(view_x, view_y);
  MYON_Plane objectplane = Plane();
  MYON::TextHorizontalAlignment halign = dimstyle->LeaderTextHorizontalAlignment();
  MYON::TextVerticalAlignment valign = dimstyle->LeaderTextVerticalAlignment();
  if (MYON::TextHorizontalAlignment::Auto == halign)
  {
    double xdotx = objectplane.xaxis * view_x;
    if (tail_dir.x < -0.00001)
      xdotx = -xdotx;
    if (xdotx > -0.00001)
      halign = MYON::TextHorizontalAlignment::Left;
    else
      halign = MYON::TextHorizontalAlignment::Right;
  }
  MYON::TextHorizontalAlignment last_halign = text->RuntimeHorizontalAlignment();
  text->SetRuntimeHorizontalAlignment(halign);

  if (last_halign != halign)
  {
    const_cast<MYON_TextContent*>(text)->SetAlignment(halign, valign);
  }
  if (DimStyleTextPositionPropertiesHash() != dimstyle->TextPositionPropertiesHash())
  {
    MYON_wString rtfstr = text->RtfText();
    const_cast<MYON_TextContent*>(text)->Create(
      rtfstr, MYON::AnnotationType::Leader, dimstyle,
      text->TextIsWrapped(), text->FormattingRectangleWidth(), text->TextRotationRadians());

    const_cast<MYON_TextContent*>(text)->SetAlignment(halign, valign);
  }

  // Find center of scaled text
  double textblock_width = 0.0;
  double textblock_height = 0.0;
  double line_height = 1.0;

  MYON_2dPoint text_corners[4];
  MYON_2dPoint text_center;
  MYON_2dPoint text_pt(0.0, 0.0);
  if (text->Get2dCorners(text_corners)) // Gets unscaled 3d size
  {
    text_center = (text_corners[0] + text_corners[2]) / 2.0;
    textblock_width = (text_corners[1].x - text_corners[0].x);
    textblock_height = (text_corners[3].y - text_corners[0].y);
    line_height = dimstyle->TextHeight();

    MYON_2dVector text_shift;
    text_shift.x = 0.0;
    text_shift.y = 0.0;

    if (MYON_TextMask::MaskFrame::CapsuleFrame != maskframe)
    {
      // LeaderAttachStyle - Vertical alignment of text with leader text point
      MYON::TextVerticalAlignment attach = dimstyle->LeaderTextVerticalAlignment();
      switch (attach)
      {
      case MYON::TextVerticalAlignment::Top:
        text_shift.y = -textblock_height / 2.0;
        break;
      case MYON::TextVerticalAlignment::MiddleOfTop:
        text_shift.y = -(textblock_height / 2.0) + (line_height / 2.0);
        break;
      case MYON::TextVerticalAlignment::BottomOfTop:
        text_shift.y = -(textblock_height / 2.0) + line_height;
        break;
      case MYON::TextVerticalAlignment::Middle:
        text_shift.y = 0.0;
        break;
      case MYON::TextVerticalAlignment::MiddleOfBottom:
        text_shift.y = (textblock_height / 2.0) - (line_height / 2.0);
        break;
      case MYON::TextVerticalAlignment::Bottom:
        text_shift.y = textblock_height / 2.0;
        break;
      case MYON::TextVerticalAlignment::BottomOfBoundingBox:
        text_shift.y = (textblock_height / 2.0) + (dimstyle->TextGap());   /*(line_height / 10.0);*/
        break;
      }
    }

    // 2d Point at center of text but without vertical alignment shift
    MYON_2dPoint text_pt2(0.0, 0.0);
    if (0 < m_points.Count())
      text_pt2 = m_points[m_points.Count() - 1];

    double landing_length = 0.0;
    if (dimstyle->LeaderHasLanding())
      landing_length = dimstyle->LeaderLandingLength();
    double text_gap = dimstyle->TextGap();

    if (maskframe != MYON_TextMask::MaskFrame::NoFrame)
    {
      text_gap = dimstyle->TextMask().MaskBorder();
    }

    if (maskframe == MYON_TextMask::MaskFrame::CapsuleFrame)
    {
      double half_height = dimscale * (textblock_height * 0.5 + text_gap);
      double radius = MYON_2dVector(half_height, half_height).Length();
      if (landing_length > 0.0)
      {
        tail_dir = tail_dir.x < 0 ? MYON_2dVector(-1, 0) : MYON_2dVector(1, 0);
        text_pt2 = text_pt2 + (tail_dir * landing_length);
      }

      if (MYON_DimStyle::ContentAngleStyle::Aligned == textangle_style && landing_length == 0.0)
      {
        text_pt2 = text_pt2 + (tail_dir * (textblock_width * 0.5 + text_gap - radius * 0.5));
      }
      else if (fabs(tail_dir.x) > 0.1 && textblock_width > textblock_height)
      {
        if (tail_dir.x > 0)
        {
          text_pt2.x += (textblock_width * 0.5 + text_gap - radius * 0.5);
        }
        else
        {
          text_pt2.x -= (textblock_width * 0.5 + text_gap - radius * 0.5);
        }
      }
      text_pt2 = text_pt2 + (tail_dir * radius);
    }
    else
    {
      double x_offset = dimscale * (landing_length + text_gap + textblock_width / 2.0);

      text_pt2 = text_pt2 + (tail_dir * x_offset);
    }

    // Move from Origin to leader plane
    const MYON_Plane& leaderplane = Plane();

    MYON_2dVector text_dir(1.0, 0.0);     // Horizontal to cplane - MYON_DimStyle::ContentAngleStyle::Horizontal
    if (MYON_DimStyle::ContentAngleStyle::Aligned == textangle_style)
    {
      text_dir = tail_dir;
      if (text_dir.x < 0.0)
        text_dir = -text_dir;
    }
    else if (MYON_DimStyle::ContentAngleStyle::Rotated == textangle_style)
    {
      text_dir = tail_dir;  // Already has rotation included
    }
    if (!text_dir.Unitize())
      text_dir.Set(1.0, 0.0);

    MYON_Xform textscale_xf(MYON_Xform::DiagonalTransformation(dimscale));
    MYON_Xform textcenter_xf(MYON_Xform::IdentityTransformation);        // Centers text block at origin
    MYON_Xform textrotation_xf(MYON_Xform::IdentityTransformation);      // Text rotation around origin

    MYON_Xform world_to_leader_xf(MYON_Xform::IdentityTransformation);   // WCS plane to leader plane rotation
    world_to_leader_xf.Rotation(MYON_Plane::World_xy, leaderplane);    // Rotate text from starting text plane (wcs) to leader plane

    MYON_Xform leader_to_text_pt_xf(MYON_Xform::IdentityTransformation); // Leader plane to text point translation
    leader_to_text_pt_xf = MYON_Xform::TranslationTransformation(text_pt2);

    textrotation_xf.Rotation(text_dir.y, text_dir.x, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);

    textcenter_xf.m_xform[0][3] = -text_center.x;
    textcenter_xf.m_xform[1][3] = -text_center.y + text_shift.y;

    if (MYON::TextOrientation::InView == dimstyle->LeaderTextOrientation())
    {
      const MYON_Plane& ldrplane = Plane();
      MYON_3dPoint text_point_3d = Plane().PointAt(text_pt2.x, text_pt2.y);
      textrotation_xf.Rotation(text_point_3d, ldrplane.xaxis, ldrplane.yaxis, ldrplane.zaxis, text_point_3d, view_x, view_y, view_z);
      text_xform_out = textscale_xf * textcenter_xf;
      text_xform_out = leader_to_text_pt_xf * text_xform_out;
      text_xform_out = world_to_leader_xf * text_xform_out;
      text_xform_out = textrotation_xf * text_xform_out;
      return true;
    }
    else if (dimstyle->DrawForward())
    {
      if (dimstyle->DrawForward())
      {
        // Check if the text is right-reading by comparing
        // text plane x and y, rotated by text rotation angle,
        // to view right and up

        MYON_3dVector text_xdir = leaderplane.xaxis;
        MYON_3dVector text_ydir = leaderplane.yaxis;
        MYON_3dVector text_zdir = leaderplane.zaxis;
        if (text_zdir * view_z < 0.0)
        {
          MYON_Xform xfr = textrotation_xf.Inverse();
          text_xdir.Transform(xfr);
          text_ydir.Transform(xfr);
          text_zdir.Transform(xfr);
        }
        else
        {
          text_xdir.Transform(textrotation_xf);
          text_ydir.Transform(textrotation_xf);
          text_zdir.Transform(textrotation_xf);
        }

        if (nullptr != model_xform)
        {
          text_xdir.Transform(*model_xform);
          text_ydir.Transform(*model_xform);
          text_zdir.Transform(*model_xform);
        }

        bool flip_x = false;
        bool flip_y = false;
        const double fliptol = (nullptr != vp && vp->Projection() == MYON::view_projection::perspective_view) ? 0.0 : cos(88.0 * MYON_DEGREES_TO_RADIANS);
        CalcTextFlip(
          text_xdir, text_ydir, text_zdir,
          view_x, view_y, view_z,
          model_xform,
          fliptol,
          flip_x,
          flip_y);

        MYON_Xform mxf;
        if (flip_x)
        {
          mxf.Mirror(MYON_3dPoint::Origin, MYON_3dVector::XAxis);
          textscale_xf = textscale_xf * mxf;
        }
        if (flip_y)
        {
          mxf.Mirror(MYON_3dPoint::Origin, MYON_3dVector::YAxis);
          textscale_xf = textscale_xf * mxf;
        }
      }
    }

    text_xform_out = textscale_xf * textcenter_xf;
    text_xform_out = textrotation_xf * text_xform_out;
    text_xform_out = leader_to_text_pt_xf * text_xform_out;
    text_xform_out = world_to_leader_xf * text_xform_out;
  }
  return true;
}

// MYON_Annotation override
bool  MYON_Leader::GetAnnotationBoundingBox(
  const MYON_Viewport* vp,
  const MYON_DimStyle* dimstyle,
  double dimscale,
  double* boxmin,
  double* boxmax,
  bool bGrow) const
{
  if (nullptr == dimstyle)
    dimstyle = &MYON_DimStyle::Default;

  const MYON_SHA1_Hash hash = Internal_GetBBox_InputHash(
    vp, dimstyle, dimscale,
    m_text_point,
    m_points.UnsignedCount(),
    m_points.Array()
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

  const MYON_Curve* curve = Curve(dimstyle);
  if (nullptr != curve)
  {
    MYON_BoundingBox curve_box;
    curve->GetTightBoundingBox(curve_box);
    bbox.Union(curve_box);
  }

  return Internal_GetBBox_End(bbox, hash, boxmin, boxmax, bGrow);
}

bool MYON_Leader::Transform(const MYON_Xform& xform)
{
  bool rc = MYON_Geometry::Transform(xform);
  if (rc)
  {
    if (xform.IsTranslation())
      rc = m_plane.Transform(xform);
    else
    {
      int cnt = m_points.Count();
      MYON_3dPointArray pts(cnt+1);
      for (int i = 0; i < cnt; i++)
      {
        pts.AppendNew() = m_plane.PointAt(m_points[i].x, m_points[i].y);
        pts[i].Transform(xform);
      }
      m_text_point = MYON_3dPoint::UnsetPoint;
      rc = m_plane.Transform(xform);
      for (int i = 0; i < cnt; i++)
        m_plane.ClosestPointTo(pts[i], &m_points[i].x, &m_points[i].y);
    }
  }
  if (rc && nullptr != m_curve && !m_curve->Transform(xform))
  {
    delete m_curve;
    m_curve = nullptr;
  }
  return rc;
}

// returns the base point and width grip using the current alignments
bool MYON_Leader::GetTextGripPoints(
  MYON_2dPoint& base,
  MYON_2dPoint& width,
  const MYON_DimStyle* dimstyle,
  double dimscale) const
{
  const MYON_TextContent* text = Text();
  if (nullptr == text)
    return false;

  MYON_3dPoint q[4];
  if (!text->Get3dCorners(q))
    return false;

  MYON_2dVector taildir = TailDirection(dimstyle);

  MYON_3dPoint wp3 = (taildir.x < 0.0)
    ? (q[0] + q[3]) / 2.0
    : (q[1] + q[2]) / 2.0;

  MYON_3dPoint bp3 = (taildir.x < 0.0)
    ? (q[1] + q[2]) / 2.0
    : (q[0] + q[3]) / 2.0;

  MYON_Xform xform;
  GetTextXform(nullptr, dimstyle, dimscale, xform);
  bp3.Transform(xform);
  wp3.Transform(xform);
  Plane().ClosestPointTo(bp3, &base.x, &base.y);
  Plane().ClosestPointTo(wp3, &width.x, &width.y);

  return true;
}



//bool MYON_Leader::Explode(
//  const MYON_DimStyle* dimstyle,
//  MYON_SimpleArray<const MYON_Geometry*> object_parts) const
//{
//  bool rc = true;
//  if (nullptr == dimstyle)
//    dimstyle = &MYON_DimStyle::Default;
//  double text_scale = dimstyle->DimScale();
//
//  const MYON_TextContent* text = Text();
//  if (nullptr != text)
//  {
//    MYON_3dVector view_x(1.0, 0.0, 0.0);
//    MYON_3dVector view_y(0.0, 1.0, 0.0);
//    MYON_Xform text_xf(1.0);
//    GetTextXform(text_xf, dimstyle, text_scale, view_x, view_y, false);
//    MYON_Text* textobj = new MYON_Text;
//    if (nullptr != textobj)
//    {
//      // Explode to text object, not to curves
//      MYON_Plane plane(MYON_xy_plane);
//      plane.Transform(text_xf);
//      MYON::TextHorizontalAlignment halign = MYON::TextHorizontalAlignment::Left;
//      MYON::TextVerticalAlignment valign = MYON::TextVerticalAlignment::Top;
//      text->GetAlignment(halign, valign);
//      if (textobj->Create(text->RtfText(), dimstyle, plane, halign, valign, 0.0, 0.0))
//      {
//        object_parts.Append(textobj);
//      }
//      else
//      {
//        delete textobj;
//        textobj = nullptr;
//      }
//    }
//  }
//  const MYON_NurbsCurve* curve = Curve(dimstyle);
//  if (nullptr != curve)
//  {
//    MYON_NurbsCurve* curvecpy = curve->Duplicate();
//    if (nullptr != curvecpy)
//    {
//      object_parts.Append(curvecpy);
//      rc = true;
//    }
//  }
//
//  if (dimstyle->LeaderHasLanding())
//  {
//    MYON_Line line;
//    LandingLine3d(dimstyle, text_scale, line);
//    MYON_LineCurve* landing = new MYON_LineCurve(line);
//    if (nullptr != landing)
//    {
//      object_parts.Append(landing);
//    }
//  }
//  MYON_Arrowhead::arrow_type arrowtype = dimstyle->LeaderArrowType();
//  if (MYON_Arrowhead::arrow_type::NoArrow != arrowtype)
//  {
//    MYON_Xform arrow_xf(1.0);
//    MYON_2dVector uv(1.0, 0.0);
//
//    if (1 < PointCount())
//    {
//      MYON_2dPoint c;
//      Point2d(1, c);
//      uv.x = c.x; uv.y = c.y;
//    }
//
//    if (uv.Unitize())
//    {
//      double arrow_scale = dimstyle->LeaderArrowSize() * text_scale;
//      MYON_Xform xfr, xfs;
//      arrow_xf.Rotation(MYON_xy_plane, Plane());
//      xfs.Scale(arrow_scale, arrow_scale, arrow_scale);
//      xfr.Rotation(uv.y, uv.x, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);
//      arrow_xf = arrow_xf * xfs;
//      arrow_xf = arrow_xf * xfr;
//    }
//
//    //CRhinoAnnotationDrawing::ExplodeOnArrowhead(arrowtype, dimstyle->LeaderArrowBlockId(), arrow_xf, out_curves, out_objects);
//  }
//
//  return rc;
//
//}

bool MYON_Leader::Create(
  const wchar_t* leader_text,
  const MYON_DimStyle* dimstyle,
  int point_count,
  const MYON_3dPoint* points,
  const MYON_Plane& plane,
  bool bWrapped,
  double rect_width
  )
{
  if (point_count < 2)
    return false;
  InvalidateTextPoint();
  
  dimstyle = &MYON_DimStyle::DimStyleOrDefault(dimstyle);

  SetDimensionStyleId(*dimstyle);

  SetPlane(plane);
  SetPoints3d(point_count, points);

  MYON_TextContent* text = nullptr;
  if (nullptr != leader_text)
  {
    text = new MYON_TextContent;
    if (text->Create(leader_text, Type(), dimstyle, bWrapped, rect_width, 0.0))
      SetText(text);
    else
    {
      delete text;
      text = 0;
    }
  }

  return true;
}

void MYON_Leader::DeleteCurve() const
{
  if (nullptr != m_curve)
  delete m_curve;
  m_curve = nullptr;
}

const MYON_NurbsCurve* MYON_Leader::Curve(const MYON_DimStyle* dimstyle) const
{
  MYON_DimStyle::leader_curve_type curvetype = MYON_DimStyle::leader_curve_type::Polyline;
  if (nullptr != dimstyle)
    curvetype = dimstyle->LeaderCurveType();

  if (curvetype == MYON_DimStyle::leader_curve_type::None)
  {
    if (nullptr != m_curve)
    {
      delete m_curve;
      m_curve = nullptr;
      const_cast<MYON_Leader*>(this)->ClearBoundingBox();
    }
    return nullptr;
  }
  if (nullptr != m_curve &&
      ((curvetype == MYON_DimStyle::leader_curve_type::Polyline && m_curve->Order() != 2) ||
       (curvetype == MYON_DimStyle::leader_curve_type::Spline && m_curve->Order() == 2 && m_points.Count() > 2)))
  {
    delete m_curve;
    m_curve = nullptr;
  }

  if (nullptr == m_curve)
  {
    // Make a 3d curve to cache for display & picking
    if (m_points.Count() > 1)
    {
      int order;
      int pointcount = m_points.Count();
      MYON_3dPointArray points(pointcount);
      for (int i = 0; i < m_points.Count(); i++)
        points.AppendNew() = m_plane.PointAt(m_points[i].x, m_points[i].y);

      if (curvetype == MYON_DimStyle::leader_curve_type::Spline)
      {
        order = 4;
        if (points.Count() < 4)
          order = points.Count();
      }
      else
      {
        order = 2;
      }

      MYON_NurbsCurve* nc = new MYON_NurbsCurve(3, false, order, pointcount);
      if (0 != nc && 0 == m_curve)
      {
        double k = 0.0;
        int ki;
        for (ki = 0; ki < order - 1; ki++)
          nc->m_knot[ki] = k;
        for (int i = 0; i < pointcount - order + 1; i++)
        {
          k += points[i].DistanceTo(points[i + 1]);
          nc->m_knot[ki++] = k;
        }
        if (k > MYON_SQRT_EPSILON) // min curve length
        {
          for (int i = 0; i < order - 2; i++)
            nc->m_knot[ki++] = k;
          for (int i = 0; i < pointcount; i++)
            nc->SetCV(i, points[i]);
        }
        else
        {
          delete nc;
          nc = nullptr;
        }
        m_curve = nc;
      }
      const_cast<MYON_Leader*>(this)->ClearBoundingBox();
    }
  }
  return m_curve;
}

void MYON_Leader::SetPlane(MYON_Plane plane)
{
  if (Plane() == plane)
    return;
  MYON_Annotation::SetPlane(plane);
  if (nullptr != m_curve)
  {
    delete m_curve;
    m_curve = nullptr;
  }
  ClearBoundingBox();
}

bool MYON_Leader::LandingLine2d(
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Line& line) const
{
  if (nullptr == dimstyle)
    return false;
  int pointcount = PointCount();
  if (0 == pointcount)
    return false;
  MYON_2dVector taildir = TailDirection(dimstyle);
  double landing_length = dimstyle->LeaderLandingLength();
  if (landing_length < MYON_SQRT_EPSILON)
  {
    line.from = m_points[pointcount - 1];
    line.to = line.from;
  }

  if (dimstyle->LeaderContentAngleStyle() == MYON_DimStyle::ContentAngleStyle::Horizontal)
  {
    if (taildir.x < 0.0)
      taildir.x = -1.0;
    else
      taildir.x = 1.0;

    taildir.y = 0.0;
  }
  line.from = m_points[pointcount - 1];
  line.to = line.from + (landing_length * dimscale) * taildir;
  return true;
}

bool MYON_Leader::LandingLine3d(
  const MYON_DimStyle* dimstyle,
  double dimscale,
  MYON_Line& line) const
{
  MYON_Line line2d;
  if (!LandingLine2d(dimstyle, dimscale, line2d))
    return false;
  line.from = m_plane.PointAt(line2d.from.x, line2d.from.y);
  line.to = m_plane.PointAt(line2d.to.x, line2d.to.y);
  return true;
}

// Tests if the text is going to the right or left and
// Changes text alignment (justification) if necessary
// This really changes the TextRun offsets and destroys the Text bbox
// Only changes horizontal alignment
void MYON_Leader::UpdateTextAlignment(MYON_2dVector angle)
{
  return;
  //if (nullptr == m_text)
  //  return;
  //MYON::TextHorizontalAlignment ha;
  //
  //if (0.0 > LeaderContentAngleStyle().x)
  //  ha = MYON::TextHorizontalAlignment::Right;
  //else
  //  ha = MYON::TextHorizontalAlignment::Left;
  //
  //MYON::TextHorizontalAlignment current_h;
  //MYON::TextVerticalAlignment current_v;
  //m_text->GetAlignment(current_h, current_v);
  //if (ha != current_h)
  //{
  //  m_text->SetAlignment(ha, current_v);
  //  m_text->ClearBoundingBox();
  //}
}

MYON_2dVector MYON_Leader::TailDirection(const MYON_DimStyle* dimstyle) const
{
  MYON_2dVector dir = MYON_2dVector::XAxis;
  int pointcount = m_points.Count();
  if (2 <= pointcount)
  {
    dir = m_points[pointcount - 1] - m_points[pointcount - 2];    // This works for Aligned
    if (nullptr != dimstyle)
    {
      if (MYON_DimStyle::ContentAngleStyle::Horizontal == dimstyle->LeaderContentAngleStyle() &&
          MaskFrameType(dimstyle) != MYON_TextMask::MaskFrame::CapsuleFrame
        )
      {
        if (dir.x < 0.0)  // going to the left
          dir.Set(-1.0, 0.0);
        else
          dir.Set(1.0, 0.0);
      }
      else if (MYON_DimStyle::ContentAngleStyle::Rotated == dimstyle->LeaderContentAngleStyle())
      {
        double r = dimstyle->LeaderContentAngleRadians();
        if (fabs(r) > MYON_SQRT_EPSILON)
        {
          dir.x = cos(r);
          dir.y = sin(r);
        }
        else
        {
          dir.Set(1.0, 0.0);
        }
      }
    }
    dir.Unitize();
  }
  return dir;
}

void MYON_Leader::InvalidateTextPoint()
{
  m_text_point = MYON_3dPoint::UnsetPoint;
}

MYON_2dPointArray& MYON_Leader::Points2d()
{
  return m_points;
}

const MYON_2dPointArray& MYON_Leader::Points2d() const
{
  return m_points;
}

void MYON_Leader::SetPoints2d(int count, const MYON_2dPoint* points)
{
  m_points.Empty();
  m_points.Append(count, points);
  if (nullptr != m_curve)
  {
    delete m_curve;
    m_curve = nullptr;
  }
  InvalidateTextPoint();
}

void MYON_Leader::SetPoints3d(int count, const MYON_3dPoint* points)
{
  m_points.Empty();
  MYON_2dPoint uv;
  for (int i = 0; i < count; i++)
  {
    if(m_plane.ClosestPointTo(points[i], &uv.x, &uv.y))
      m_points.Append(uv);
  }
  if (nullptr != m_curve)
  {
    delete m_curve;
    m_curve = nullptr;
  }
  InvalidateTextPoint();
}

void MYON_Leader::InsertPoint2d(int atidx, MYON_2dPoint point)
{
  m_points.Insert(atidx, point);
  if (nullptr != m_curve)
  {
    delete m_curve;
    m_curve = nullptr;
  }
  InvalidateTextPoint();
}

void MYON_Leader::InsertPoint3d(int atidx, MYON_3dPoint point)
{
  MYON_2dPoint p2;
  if(m_plane.ClosestPointTo(point, &p2.x, &p2.y))
    m_points.Insert(atidx, p2);
}

void MYON_Leader::RemovePoint(int idx)
{
  m_points.Remove(idx);
  if (nullptr != m_curve)
  {
    delete m_curve;
    m_curve = nullptr;
  }
  InvalidateTextPoint();
}

void MYON_Leader::AppendPoint2d(MYON_2dPoint point)
{
  m_points.Append(point);
  if (nullptr != m_curve)
  {
    delete m_curve;
    m_curve = nullptr;
  }
  InvalidateTextPoint();
}

bool MYON_Leader::AppendPoint3d(MYON_3dPoint point)
{
  MYON_2dPoint uv;
  if (m_plane.ClosestPointTo(point, &uv.x, &uv.y))
  {
    m_points.Append(uv);
    if (nullptr != m_curve)
    {
      delete m_curve;
      m_curve = nullptr;
    }
    InvalidateTextPoint();
    return true;
  }
  return false;
}

bool MYON_Leader::SetPoint2d(int idx, MYON_2dPoint point)
{
  if (idx >= 0 && idx < m_points.Count())
  {
    m_points[idx] = point;
    if (nullptr != m_curve)
    {
      delete m_curve;
      m_curve = nullptr;
    }
    InvalidateTextPoint();
    return true;
  }
  return false;
}

bool MYON_Leader::SetPoint3d(int idx, MYON_3dPoint point)
{
  if (idx >= 0 && idx < m_points.Count())
  {
    MYON_2dPoint uv;
    if (m_plane.ClosestPointTo(point, &uv.x, &uv.y))
    {
      m_points[idx] = uv;
      if (nullptr != m_curve)
      {
        delete m_curve;
        m_curve = nullptr;
      }
      InvalidateTextPoint();
      return true;
    }
  }
  return false;
}

bool MYON_Leader::Point2d(int idx, MYON_2dPoint& point) const
{
  if (idx >= 0 && idx < m_points.Count())
  {
    point = m_points[idx];
    return true;
  }
  return false;
}

bool MYON_Leader::Point3d(int idx, MYON_3dPoint& point) const
{
  if (idx >= 0 && idx < m_points.Count())
  {
    point = m_plane.PointAt(m_points[idx].x, m_points[idx].y);
    return true;
  }
  return false;
}

MYON__UINT32 MYON_Leader::PointCount() const
{
  return m_points.Count();
}

bool MYON_Leader::GetTextPoint2d(
  const MYON_DimStyle* dimstyle,
  double leaderscale,
  MYON_2dPoint& point) const
{
  //if (m_text_point.IsUnsetPoint())
  {
    if (!const_cast<MYON_Leader*>(this)->UpdateTextPosition(dimstyle, leaderscale))
      return false;
  }
  point = m_text_point;
  return m_text_point.IsValid();
}

//bool MYON_Leader::GetTextPoint3d(MYON_3dPoint& point) const
//{
//  //if (m_text_point.IsUnsetPoint())
//  {
//    if (!const_cast<MYON_Leader*>(this)->UpdateTextPosition())
//      return false;
//  }
//  point = m_plane.PointAt(m_text_point.x, m_text_point.y);
//  return point.IsValid();
//}

// sets m_text_point and m_text_angle from m_points
// m_text_point is the point near the end of the leader tail
// where content attaches
bool MYON_Leader::UpdateTextPosition(
  const MYON_DimStyle* dimstyle,
  double leaderscale)
{
  if (nullptr == dimstyle)
    return false;
  MYON_2dVector text_dir;
  MYON_2dPoint attachpt(MYON_2dPoint::Origin);
  MYON_2dPoint from(MYON_2dPoint::Origin);
  MYON_Line ll;
  if (LandingLine2d(dimstyle, leaderscale, ll)) // from ponit is end of landing line
  {
    text_dir = ll.Direction();
    if (text_dir.Unitize())
      from = ll.to;
  }
  else // No landing line - from point is last leader point
  {
    text_dir = TailDirection(dimstyle);
    if (0 < m_points.Count())
      from = m_points[m_points.Count() - 1];
  }

  attachpt = from + (text_dir * dimstyle->TextGap() * leaderscale);
  if (attachpt.IsValid())
  {
    if (attachpt != m_text_point)
      ClearBoundingBox();
    m_text_point = attachpt;
    return true;
  }
  else
    return false;
}





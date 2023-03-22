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

// MYON_Leader class
#ifndef OPENNURBS_LEADER_H_INCLUDED
#define OPENNURBS_LEADER_H_INCLUDED


class MYON_CLASS MYON_Leader : public MYON_Annotation
{
  MYON_OBJECT_DECLARE(MYON_Leader);

public:
  MYON_Leader();
  ~MYON_Leader();

  MYON_Leader(const MYON_Leader& src);
  MYON_Leader& operator=(const MYON_Leader& src);

  static const MYON_Leader Empty;

private:
  void Internal_Destroy();
  void Internal_CopyFrom(const MYON_Leader& src);

public:

  /*
  Parameters:
    dimstyle - [in]
      If you want to specify text appearance or other custom properties ...
      MYON_DimStyle style = MYON_DimStyle::DimStyleFromProperties( doc->DimStyleContext().CurrentDimStyle(), ... );
      style.Set...(...);
      Then pass &style

  Remarks:
    Parses text string and makes runs
  */
  bool Create(
    const wchar_t* leader_text,
    const MYON_DimStyle* dimstyle,
    int point_count,
    const MYON_3dPoint* points,
    const MYON_Plane& plane,
    bool bWrapped,
    double rect_width
    );

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  void Dump(MYON_TextLog& log) const override;
  bool Write(MYON_BinaryArchive& file) const override;
  bool Read(MYON_BinaryArchive& file) override;
  MYON::object_type ObjectType() const override;

  /*
  Description:
    Create a V6 leader from a V5 leader. 
    The function is used when reading V5 files.
  Parameters:
    v5_leader -[in]
    dim_style - [in]
      Dimstyle referenced by v5_leader or nullptr if not available.
    destination - [in]
      If destination is not nullptr, then the V6 leader is constructed
      in destination. If destination is nullptr, then the new V6 leader
      is allocated with a call to new MYON_Leader().
  */
  static MYON_Leader* CreateFromV5Leader(
    const class MYON_OBSOLETE_V5_Leader& V5_leader,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_Leader* destination
  );
  
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  bool GetAnnotationBoundingBox(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    double* boxmin,
    double* boxmax,
    bool bGrow = false
  ) const override; // MYON_Annotation override

  bool Transform(const MYON_Xform& xform) override;

  bool GetTextGripPoints(
    MYON_2dPoint& base,
    MYON_2dPoint& width,
    const MYON_DimStyle* dimstyle,
    double textscale) const;

  //bool Explode(
  //  const MYON_DimStyle* dimstyle,
  //  MYON_SimpleArray<const MYON_Geometry*> object_parts) const;

  // Transforms text from natural position at origin to 
  // 3d location as it displays in the leader
  bool GetTextXform(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const override;

  bool GetTextXform(
    const MYON_Xform* model_xform,
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const;
  
  void UpdateTextAlignment(MYON_2dVector angle); // Sets text to right or left justified per leader direction

  const MYON_NurbsCurve* Curve(
    const MYON_DimStyle* dimstyle
    ) const; // cached curve for display and picking
  void DeleteCurve() const;

  void SetPlane(MYON_Plane plane);

  //// TailDirection is the tangent direction 
  //// of the end of the leader tail
  //// Returns 1,0 if there isn't a tangent
  MYON_2dVector TailDirection(const MYON_DimStyle* dimstyle) const;

  // These do nothing and return false if 
  // HasLanding is false
  // Otherwise, they return a line added to the 
  // tail of the leader in the direction of
  // LeaderContentAngleStyle()
  bool LandingLine2d(
    const MYON_DimStyle* style,
    double dimscale,
    MYON_Line& line) const;
  bool LandingLine3d(
    const MYON_DimStyle* style,
    double dimscale,
    MYON_Line& line) const;

  MYON__UINT32 PointCount() const;
  void SetPoints2d(int count, const MYON_2dPoint* points);
  void SetPoints3d(int count, const MYON_3dPoint* points);
  bool SetPoint2d(int idx, MYON_2dPoint point);
  bool SetPoint3d(int idx, MYON_3dPoint point);
  void InsertPoint2d(int atidx, MYON_2dPoint point);
  void InsertPoint3d(int atidx, MYON_3dPoint point);
  void AppendPoint2d(MYON_2dPoint point);
  bool AppendPoint3d(MYON_3dPoint point);
  void RemovePoint(int idx);
  bool Point2d(int idx, MYON_2dPoint& point) const;
  bool Point3d(int idx, MYON_3dPoint& point) const;

  bool GetTextPoint2d(
    const MYON_DimStyle* dimstyle, 
    double leaderscale,
    MYON_2dPoint& point) const;
  //bool GetTextPoint3d(MYON_3dPoint& point) const;
  MYON_2dPointArray& Points2d();
  const MYON_2dPointArray& Points2d() const;

  void InvalidateTextPoint();
  bool UpdateTextPosition(
    const MYON_DimStyle* dimstyle,
    double leaderscale);

private:
  MYON_2dPointArray         m_points;

  // runtime
  mutable MYON_NurbsCurve* m_curve = nullptr;            // Deleted by ~MYON_Leader()
  mutable MYON_2dPoint     m_text_point = MYON_2dPoint::UnsetPoint;
};



#endif


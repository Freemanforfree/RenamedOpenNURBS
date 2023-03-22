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

#if !defined(OPENNURBS_DIMENSION_INC_)
#define OPENNURBS_DIMENSION_INC_

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template MYON_ClassArray< class MYON_DimStyle >;
#endif

class MYON_CLASS MYON_Dimension : public MYON_Annotation
{
  MYON_OBJECT_DECLARE(MYON_Dimension);

public:
#pragma region RH_C_SHARED_ENUM [MYON_Dimension::ForceArrow] [Rhino.Geometry.Dimension.ForceArrow] [nested:int]
  /// <summary>
  /// OBSOLETE enum do not use.
  /// </summary>
  enum class ForceArrow : unsigned int
  {
    /// <summary> </summary>
    Auto = 0,
    /// <summary> </summary>
    Inside = 1,
    /// <summary> </summary>
    Outside = 2,
  };
#pragma endregion

  static MYON_Dimension::ForceArrow ForceArrowFromUnsigned(
    unsigned int force_arrow_as_unsigned);

#pragma region RH_C_SHARED_ENUM [MYON_Dimension::ForceText] [Rhino.Geometry.Dimension.ForceText] [nested:int]
  /// <summary>
  /// OBSOLETE enum do not use.
  /// </summary>
  enum class ForceText : unsigned int
  {
    /// <summary> </summary>
    Auto = 0,
    /// <summary> </summary>
    Inside = 1,
    /// <summary> </summary>
    Right = 2,
    /// <summary> </summary>
    Left = 3,
    /// <summary> </summary>
    HintRight = 4,
    /// <summary> </summary>
    HintLeft = 5,
  };
#pragma endregion

  static MYON_Dimension::ForceText ForceTextFromUnsigned(
    unsigned int force_text_as_unsigned);


protected:
  MYON_Dimension( MYON::AnnotationType annotation_type );
  ~MYON_Dimension();
  MYON_Dimension(const MYON_Dimension& src);
  MYON_Dimension& operator=(const MYON_Dimension& src);

private:
  MYON_Dimension() = delete;
  void Internal_Destroy();
  void Internal_CopyFrom(const MYON_Dimension& src);

public:
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  virtual MYON_2dPoint DefaultTextPoint() const;
  virtual bool UseDefaultTextPoint() const;
  virtual void SetUseDefaultTextPoint(bool usedefault);

  // Text center-midpoint in dimension plane
  MYON_2dPoint TextPoint() const;
  void Set2dTextPoint(const MYON_2dPoint& textpoint);

  const wchar_t* UserText() const;
  void SetUserText(const wchar_t* text);
  const wchar_t* PlainUserText() const;

  // Computes measurement value as a number
  virtual double Measurement() const = 0;

  // Add to natural rotation
  MYON_DEPRECATED_MSG("MYON_Dimension::TextRotation() is a mistake. Use MYON_Annotation::TextRotationRadians().")
  double TextRotation() const;

  MYON_DEPRECATED_MSG("MYON_Dimension::SetTextRotation() is a mistake. Use MYON_Annotation::SetTextRotationRadians().")
  void SetTextRotation(double rotation_radians);

  bool ArrowIsFlipped(int i) const;
  void FlipArrow(int i, bool flip) const;

  // If the dimension is a paper space object and the geometry being dimensioned is in 
  // model space, in a detail viewport, DetailMeasured() will have the UUID of the detail
  // that the dimension references.  Otherwise DetailMeasured() will be MYON_nil_uuid.
  MYON_UUID DetailMeasured() const;
  void SetDetailMeasured(MYON_UUID uuid);

  // If DetailMeasured() returns MYON_nil_uuid, DistanceScale() has no meaning
  // If the dimension is in page space and measures model space geometry,
  // DistanceScale() is the conversion from the model space distance being measured
  // to the paper space distance spanned by the dimension geometry.
  // When the zoom factor of the detail view changes, the distance scale will change
  double DistanceScale() const;
  void SetDistanceScale(double distance_scale) const;

  //virtual bool  GetBBox(
  //  const MYON_Viewport* vp,
  //  double dimscale,
  //  const MYON_DimStyle* dimstyle,
  //  double* boxmin,
  //  double* boxmax,
  //  bool bGrow = 0) const = 0;

  virtual bool GetTextRect(MYON_3dPoint text_rect[4]) const;

  // Remakes dimension text geometry object and sets it on the dimension
  virtual bool UpdateDimensionText(
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle
  ) const;

  // Makes text geometry for a dimension
  MYON_TextContent*  RebuildDimensionText(
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    bool expandanglebrackets  // replace <> with the formatted distance
  ) const;

  virtual bool GetDistanceDisplayText(
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    MYON_wString& displaytext) const;
  
  static bool GetCentermarkDisplay(
    const MYON_Plane& plane,
    const MYON_2dPoint center,
    double marksize,
    double radius,
    MYON_DimStyle::centermark_style style,
    MYON_Line lines[6],
    bool isline[6],
    int maxlines
    );

  static bool GetCentermarkSnapPoints(
    const MYON_Plane& plane,
    const MYON_2dPoint center,
    double marksize,
    double radius,
    MYON_DimStyle::centermark_style style,
    MYON_3dPoint points[13],
    bool ispoint[13]);

  // Obsolete
  MYON_DEPRECATED_MSG("MYON_Dimension::ArrowFit(const MYON_DimStyle* parent_style)")
  MYON_Dimension::ForceArrow ForceArrowPosition() const;

  MYON_DEPRECATED_MSG("MYON_Dimension::SetArrowFit(const MYON_DimStyle* parent_style,MYON_DimStyle::arrow_fit arrowfit)")
  void SetForceArrowPosition(ForceArrow force);

  MYON_DEPRECATED_MSG("MYON_Dimension::TextFit(const MYON_DimStyle* parent_style)")
  MYON_Dimension::ForceText ForceTextPosition() const;

  MYON_DEPRECATED_MSG("MYON_Dimension::SetTextFit(const MYON_DimStyle* parent_style,MYON_DimStyle::text_fit textfit)")
  void SetForceTextPosition(ForceText force);

  void SetForceDimLine(
    const MYON_DimStyle* parent_style,
    bool forcedimline
  );

  bool ForceDimLine(
    const MYON_DimStyle* parent_style) const;

  void SetTextFit(
    const MYON_DimStyle* parent_style,
    MYON_DimStyle::text_fit textfit);

  MYON_DimStyle::text_fit TextFit(
    const MYON_DimStyle* parent_style) const;

  void SetArrowFit(
    const MYON_DimStyle* parent_style,
    MYON_DimStyle::arrow_fit arrowfit);

  MYON_DimStyle::arrow_fit ArrowFit(
    const MYON_DimStyle* parent_style) const;



protected:
  MYON_wString            m_user_text = L"<>";            // If user overridden, or "<>" to use default
  double                m_reserved = 0.0;
  mutable MYON_wString    m_plain_user_text;

  bool                  m_use_default_text_point = true;
  MYON_2dPoint            m_user_text_point = MYON_2dPoint::UnsetPoint;   // Text point if default isn't used

  mutable bool          m_flip_arrow_1 = false;
  mutable bool          m_flip_arrow_2 = false;
  mutable bool          m_text_outside = false;
  unsigned int          m_reserved98 = 0;
  unsigned int          m_reserved99 = 0;


  // UUID of detail if dimension is in page space measuring model space geometry
  MYON_UUID               m_detail_measured = MYON_nil_uuid;
  // Conversion from model space size to paper space size if dimension is in page space measuring model space geometry
  mutable double        m_distance_scale = 1.0;

  bool Internal_WriteDimension(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const;

  bool Internal_ReadDimension(
    MYON_BinaryArchive&  // restore definition from binary archive
    );
};

class MYON_CLASS MYON_DimLinear : public MYON_Dimension
{
  MYON_OBJECT_DECLARE(MYON_DimLinear);

public:
  MYON_DimLinear();
  ~MYON_DimLinear() = default;
  MYON_DimLinear(const MYON_DimLinear& src) = default;
  MYON_DimLinear& operator=(const MYON_DimLinear& src) = default;

  static const MYON_DimLinear Empty;

  /*
  Description:
    Create a V6 linear dimension from a V5 linear dimension
    The function is used when reading V5 files.
  Parameters:
    V5_linear_dimension -[in]
    annotation_context - [in]
      Dimstyle and other information referenced by V5_linear_dimension or nullptr if not available.
    destination - [in]
      If destination is not nullptr, then the V6 linear dimension is constructed
      in destination. If destination is nullptr, then the new V6 linear dimension
      is allocated with a call to new MYON_DimLinear().
  */
  static MYON_DimLinear* CreateFromV5DimLinear(
    const class MYON_OBSOLETE_V5_DimLinear& V5_linear_dimension,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_DimLinear* destination
  );


  /*
  Parameters:
    annotation_type - [in]
      annotation type to test
  Returns:
    True if input parameter is one of the valid linear dimension types
    MYON::AnnotationType::Aligned or MYON::AnnotationType::Rotated.
  */
  static bool IsValidLinearDimensionType(
    MYON::AnnotationType annotation_type
  );

  /*
  Parameters:
    linear_dimension_type - [in]
      MYON::AnnotationType::Aligned or MYON::AnnotationType::Rotated.
  Returns:
    True if input parameter is valid and type is set.
  */
  bool SetLinearDimensionType(
    MYON::AnnotationType linear_dimension_type
  );

  bool Write(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const override;

  bool Read(
    MYON_BinaryArchive&  // restore definition from binary archive
    ) override;

  bool Transform(const MYON_Xform& xform) override;

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

  // Gets transform for dimension text from MYON_xy_plane to 3d display location
  bool GetTextXform(
    const MYON_Xform* model_xform,
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const;

  bool GetTextXform(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const override;

  bool Create(
    MYON::AnnotationType dim_type,
    const MYON_UUID style_id,
    const MYON_Plane& plane,
    const MYON_3dVector& ref_horizontal,
    const MYON_3dPoint& def_pt1,
    const MYON_3dPoint& def_pt2,
    const MYON_3dPoint& dimline_pt,
    double rotation_in_plane = 0.0
    );

  /*
  Description:
    Create an aligned linear dimension. The dimension line is
    parallel to the segment connecting the extension points.
  Parameters:
    extension_point0 - [in]
    extension_point1 - [in]
      locations of one of the points being dimensioned.
      The dimension line will be parallel to the segment
      connecting these points.
    dimension_line_point - [in]
      a point on the linear dimension line.
    plane_normal - [in]
      A vector perpindcular to the line between the extension points
      that defines the orientation of the dimension's plane.
    dim_style_id - [in]
    destination - [in]
      If nullptr, the returned MYON_DimLinear is allocated by operator new.
      Otherwise, the reuturned MYON_DimLinear is created in destination.
  */
  static MYON_DimLinear* CreateAligned(
    MYON_3dPoint extension_point0,
    MYON_3dPoint extension_point1,
    MYON_3dPoint dimension_line_point,
    MYON_3dVector plane_normal,
    MYON_UUID style_id,
    MYON_DimLinear* destination
  );

  /*
  Description:
    Create a rotated linear dimension to the document.
    The dimension line is explicitly specified.
  Parameters:
    extension_point0 - [in]
    extension_point1 - [in]
      locations of one of the points being dimensioned.
      The dimension line will be parallel to the segment
      connecting these points.
    dimension_line - [in]
      the dimension line. This is treated as an infinite
      line and the points are automatically calculated.
    plane_normal - [in]
      A vector perpindcular to the line between the extension points
      that defines the orientation of the dimension's plane. 
      The dimension line is projected to this plane.
    dim_style_id - [in]
    destination - [in]
      If nullptr, the returned MYON_DimLinear is allocated by operator new.
      Otherwise, the reuturned MYON_DimLinear is created in destination.
  */
  static MYON_DimLinear* CreateRotated(
    MYON_3dPoint extension_point0,
    MYON_3dPoint extension_point1,
    MYON_Line dimension_line,
    MYON_3dVector plane_normal,
    MYON_UUID style_id,
    MYON_DimLinear* destination
  );

  // virtual
  double Measurement() const override;
  MYON_2dPoint DefaultTextPoint() const override;

  // DefPoint1 is m_plane.origin
  // Meaasurement is between DefPoint1 and DefPoint2
  // parallel to the m_plane x-axis.
  MYON_2dPoint DefPoint1() const;
  MYON_2dPoint DefPoint2() const;
  MYON_2dPoint DimlinePoint() const;

  void Set2dDefPoint1(MYON_2dPoint pt);
  void Set2dDefPoint2(MYON_2dPoint pt);
  void Set2dDimlinePoint(MYON_2dPoint pt);

  void Set3dDefPoint1(MYON_3dPoint pt);
  void Set3dDefPoint2(MYON_3dPoint pt);
  void Set3dDimlinePoint(MYON_3dPoint pt);

  MYON_2dPoint ArrowPoint1() const;    // Calculated
  MYON_2dPoint ArrowPoint2() const;    // Calculated

  bool Get3dPoints(
    MYON_3dPoint* defpt1,
    MYON_3dPoint* defpt2,
    MYON_3dPoint* arrowpt1,
    MYON_3dPoint* arrowpt2,
    MYON_3dPoint* dimline,
    MYON_3dPoint* textpt) const;

  bool GetDisplayLines(
    const MYON_Viewport* vp,
    const MYON_DimStyle* style,
    double dimscale,
    MYON_3dPoint text_rect[4],
    MYON_Line lines[4],
    bool isline[4],
    int maxlines) const;

  void GetArrowXform(
    int which_end,
    double scale,
    bool arrowflipped,
    bool from_the_back,
    MYON_Xform& arrow_xform_out) const;

public:
	bool GetTextXform(
		const MYON_Xform* model_xform,
		const MYON_3dVector view_x,
		const MYON_3dVector view_y,
		const MYON_3dVector view_z,
		MYON::view_projection projection,
		bool bDrawForward,
		const MYON_DimStyle* dimstyle,
		double dimscale,
		MYON_Xform& text_xform_out
	) const;


protected:
  MYON_2dPoint m_def_pt_2 = MYON_2dPoint::UnsetPoint;
  MYON_2dPoint m_dimline_pt = MYON_2dPoint::UnsetPoint;
};

//---------------------------------------------------------------------

class MYON_CLASS MYON_DimAngular : public MYON_Dimension
{
  MYON_OBJECT_DECLARE(MYON_DimAngular);

public:
  MYON_DimAngular();
  ~MYON_DimAngular() = default;
  MYON_DimAngular(const MYON_DimAngular& src) = default;
  MYON_DimAngular& operator=(const MYON_DimAngular& src) = default;

  static const MYON_DimAngular Empty;

  /*
  Parameters:
  annotation_type - [in]
  annotation type to test
  Returns:
  True if input parameter is one of the valid linear dimension types
  MYON::AnnotationType::Angular or MYON::AnnotationType::Angular3pt.
  */
  static bool IsValidAngularDimensionType(
    MYON::AnnotationType annotation_type
  );

  /*
  Parameters:
  angular_dimension_type - [in]
  MYON::AnnotationType::Angular or MYON::AnnotationType::Angular3pt.
  Returns:
  True if input parameter is valid and type is set.
  */
  bool SetAngularDimensionType(
    MYON::AnnotationType angular_dimension_type
  );

  static MYON_DimAngular* CreateFromV5DimAngular(
    const class MYON_OBSOLETE_V5_DimAngular& V5_dim_angle,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_DimAngular* destination
  );

  bool Write(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const override;

  bool Read(
    MYON_BinaryArchive&  // restore definition from binary archive
    ) override;

  bool Transform(const MYON_Xform& xform) override;

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

// Gets transform for dimension text from MYON_xy_plane to 3d display location
  bool GetTextXform(
    const MYON_Xform* model_xform,
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const;

  bool GetTextXform(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const override;

  /*
  Parameters:
    dim_style - [in]
      Pass nullptr if a dim_style is not available.
    arc - [in]
      arc being dimensioned
    offset - [in]
      distance from the arc being dimensioned to the angular dimension arc.
      When offset > 0, the dimension is outside the arc's circle. 
      When offset < 0 and > - arc.Radius(), the dimension is inside the arc's circle. 
      In all other cases, the angular dimension arc is on the arc.
  Returns:
    True if successful.
    False if input is not valid. In this case  MYON_DimAngle::Empty settings are returned.
  */
  bool Create(
    const MYON_DimStyle* dim_style,
    MYON_Arc arc,
    double offset
  );

  /*
  Description:
    The angle between the lines is dimensioned. 
    
    If the lines intersect in a single point, that point is used as the center
    of the angular dimension arc. In this case, there are eight possible angles
    to dimension. The point_on_angular_dimension_arc and point_on_line parameters
    are used to select the correct angle to dimension. If a point_on_line parameter
    is not set, the corresponding line's midpoint is used.
    
    If the lines are colinear, the point on the line closest to 
    point_on_angular_dimension_arc is the center of the angular dimension arc.

  Parameters:
    dim_style - [in]
      Pass nullptr if a dim_style is not available.
    line1 - [in]
    point_on_line1 - [in]
      If point_on_line1 is specified, it indicates which semi-infinite portion of line1 to dimension.
      Otherwise the midpoint of lne1 as a segment is used.
      When in doubt, pass MYON_3dPoint::UnsetPoint.
    line2 - [in]
    point_on_line2 - [in]
      If point_on_line2 is specified, it indicates which semi-infinite portion of line2 to dimension.
      Otherwise the midpoint of line2 as a segment is used.
      When in doubt, pass MYON_3dPoint::UnsetPoint.
    point_on_angular_dimension_arc - [in]
      A point on the interior of the angular dimension arc. 
    bSetExtensionPoints - [in]
      If bSetExtensionPoints is true, and a point_on_line parameter is valid, that point
      is used as the extension point. Otherwise the angular dimension arc endpoint is used.
  Returns:
    True if successful.
    False if input is not valid. In this case  MYON_DimAngle::Empty settings are returned.
  */
  bool Create(
    const MYON_DimStyle* dim_style,
    MYON_Line line1,
    MYON_3dPoint point_on_line1,
    MYON_Line line2,
    MYON_3dPoint point_on_line2,
    MYON_3dPoint point_on_angular_dimension_arc,
    bool bSetExtensionPoints
  );

  bool Create(
    const MYON_UUID style_id,
    const MYON_Plane& plane,
    const MYON_3dVector& ref_horizontal,
    const MYON_3dPoint& center_pt,
    const MYON_3dPoint& extension_pt1,     // point on first extension vector
    const MYON_3dPoint& extension_pt2,     // point on second extension vector
    const MYON_3dPoint& dimline_pt         // point on dimension line
  );

  bool Create(
    const MYON_UUID style_id,
    const MYON_Plane& plane,
    const MYON_3dVector& ref_horizontal,
    const MYON_3dPoint& extension_pt1,     // start of first extension line
    const MYON_3dPoint& extension_pt2,     // start of second extension line
    const MYON_3dPoint& direction_pt1,     // point on first extension vector
    const MYON_3dPoint& direction_pt2,     // point on second extension vector
    const MYON_3dPoint& dimline_pt         // point on dimension line
  );

  bool AdjustFromPoints(
    const MYON_Plane& plane,
    const MYON_3dPoint& center_pt,          
    const MYON_3dPoint& extension_pt1,      // point on first extension vector
    const MYON_3dPoint& extension_pt2,      // point on second extension vector
    const MYON_3dPoint& dimline_pt          // point on dimension line
    );

  bool AdjustFromPoints(
    const MYON_Plane& plane,
    const MYON_3dPoint& extension_pt1,     // start of first extension line
    const MYON_3dPoint& extension_pt2,     // start of second extension line
    const MYON_3dPoint& direction_pt1,     // point on first extension vector
    const MYON_3dPoint& direction_pt2,     // point on second extension vector
    const MYON_3dPoint& dimline_pt         // point on dimension line
  );

  static bool FindAngleVertex(
    MYON_Line lines[2],
    MYON_3dPoint pickpoints[2],
    const MYON_Plane& plane,
    MYON_3dPoint& centerpoint_out);


  bool UpdateDimensionText(const MYON_DimStyle* dimstyle) const;

  bool GetAngleDisplayText(const MYON_DimStyle* dimstyle, MYON_wString& displaytext) const;

  // virtual
  double Measurement() const override;  // angle in radians
  MYON_2dPoint DefaultTextPoint() const override;
  bool GetAngles(double* start_ang, double* end_ang, double* mid_ang) const;
  double Radius() const;

  // CenterPoint is m_plane.origin
  // Measurement is angle between m_vec_1 & m_vec_2 in radians
  MYON_2dPoint CenterPoint() const;
  MYON_2dPoint DefPoint1() const;     // Start of first extension
  MYON_2dPoint DefPoint2() const;     // Start of second extension
  MYON_2dPoint DimlinePoint() const;  // Point on dimension arc
  MYON_2dPoint UserTextPoint() const; // Text point if user positioned
  MYON_2dVector ExtDir1() const;      // Direction of first extension
  MYON_2dVector ExtDir2() const;      // Direction of second extension
  void SetExtDir1(const MYON_2dVector& dir1);
  void SetExtDir2(const MYON_2dVector& dir2);
  
  void SetUserTextPoint(const MYON_3dPoint& point);

  void Set2dCenterPoint(MYON_2dPoint pt);   // Apex of angle
  void Set2dDefPoint1(MYON_2dPoint pt);     // Point where first extension starts
  void Set2dDefPoint2(MYON_2dPoint pt);     // Point where second extension starts
  void Set2dDimlinePoint(MYON_2dPoint pt);  // Point on dimension arc

  //void Set2dDefPoint1(MYON_2dPoint pt);     // Point where first extension starts
  //void Set2dDefPoint2(MYON_2dPoint pt);     // Point where second extension starts
  //void Set2dDimlinePoint(MYON_2dPoint pt);  // Point on dimension arc

  //void Set3dCenterPoint(MYON_3dPoint pt);
  //void Set3dDefPoint1(MYON_3dPoint pt);
  //void Set3dDefPoint2(MYON_3dPoint pt);
  //void Set3dDimlinePoint(MYON_3dPoint pt);

  MYON_2dPoint ArrowPoint1() const;    // Calculated - start of arc
  MYON_2dPoint ArrowPoint2() const;    // Calculated - end of arc

  bool Get3dPoints(
    MYON_3dPoint* center,
    MYON_3dPoint* defpt1,
    MYON_3dPoint* defpt2,
    MYON_3dPoint* arrowpt1,
    MYON_3dPoint* arrowpt2,
    MYON_3dPoint* dimline,
    MYON_3dPoint* textpt) const;

  bool GetDisplayLines(
    const MYON_Viewport* vp,
    const MYON_DimStyle* style,
    double dimscale,
    const MYON_3dPoint text_rect[4],
    MYON_Line lines[2],
    bool isline[2],
    MYON_Arc arcs[2],
    bool isarc[2],
    int maxlines,
    int maxarcs) const;

  void GetArrowXform(
    int which_end,
    double arrowlength,
    bool arrowflipped,
    bool from_the_back,
    MYON_Xform& arrow_xform_out) const;

  bool UpdateDimensionText(
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle) const override;

  bool GetDistanceDisplayText(
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    MYON_wString& displaytext) const override;

protected:
  // Center point is at plane origin (0,0)
  MYON_2dVector       m_vec_1 = MYON_2dVector::XAxis;         
  MYON_2dVector       m_vec_2 = MYON_2dVector::YAxis;
  double            m_ext_offset_1 = 0.0;  // distance along m_vec_1 to start extension line 1
  double            m_ext_offset_2 = 0.0;  // distance along m_vec_2 to start extension line 2
  MYON_2dPoint        m_dimline_pt = MYON_2dPoint(1.0, 1.0);  // point on interior of dimension arc
};

//---------------------------------------------------------------------

class MYON_CLASS MYON_DimRadial : public MYON_Dimension
{
  MYON_OBJECT_DECLARE(MYON_DimRadial);

public:
  MYON_DimRadial();
  ~MYON_DimRadial() = default;
  MYON_DimRadial(const MYON_DimRadial& src) = default;
  MYON_DimRadial& operator=(const MYON_DimRadial& src) = default;

  static const MYON_DimRadial Empty;


  /*
  Description:
    Create a V6 radial dimension from a V5 radial dimension
    The function is used when reading V5 files.
  Parameters:
    V5_radial_dimension -[in]
    annotation_context - [in]
      Dimstyle and other information referenced by V5_radial_dimension or nullptr if not available.
    destination - [in]
      If destination is not nullptr, then the V6 radial dimension is constructed
      in destination. If destination is nullptr, then the new V6 radial dimension
      is allocated with a call to new MYON_DimRadial().
  */
  static MYON_DimRadial* CreateFromV5DimRadial(
    const class MYON_OBSOLETE_V5_DimRadial& V5_radial_dimension,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_DimRadial* destination
  );
  
  /*
  Parameters:
    annotation_type - [in]
      annotation type to test
  Returns:
    True if input parameter is one of the valid radial dimension types
    MYON::AnnotationType::Radius or MYON::AnnotationType::Diameter.
  */
  static bool IsValidRadialDimensionType(
    MYON::AnnotationType annotation_type
  );

  /*
  Parameters:
    radial_dimension_type - [in]
      MYON::AnnotationType::Radius or MYON::AnnotationType::Diameter.
  Returns:
    True if input parameter is valid and type is set.
  */
  bool SetRadialDimensionType(
    MYON::AnnotationType radial_dimension_type
  );

  bool Write(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const override;

  bool Read(
    MYON_BinaryArchive&  // restore definition from binary archive
    ) override;

  bool Transform(const MYON_Xform& xform) override;

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

  // Gets transform for dimension text from MYON_xy_plane to 3d display location
  bool GetTextXform(
    const MYON_Xform* model_xform,
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const;

  bool GetTextXform(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const override;

  bool Create(
    MYON::AnnotationType type,
    const MYON_UUID style_id,
    const MYON_Plane& plane,
    const MYON_3dPoint& center_pt,
    const MYON_3dPoint& radius_pt,
    const MYON_3dPoint& dimline_pt
    );

  bool AdjustFromPoints(
    const MYON_Plane& plane,
    const MYON_3dPoint& center_pt,
    const MYON_3dPoint& radius_pt,
    const MYON_3dPoint& dimline_pt
    );

  double Measurement() const override;

  MYON_2dPoint DefaultTextPoint() const override;
  MYON_2dPoint CenterPoint() const;
  MYON_2dPoint RadiusPoint() const;      // Point on arc being measured
  MYON_2dPoint DimlinePoint() const;     // Endpoint of leader tail, not including landing
  MYON_2dPoint KneePoint() const;        // Point where leader tail bends

  void Set2dCenterPoint(MYON_2dPoint pt);
  void Set2dRadiusPoint(MYON_2dPoint pt);
  void Set2dDimlinePoint(MYON_2dPoint pt);

  void Set3dCenterPoint(MYON_3dPoint pt);
  void Set3dRadiusPoint(MYON_3dPoint pt);
  void Set3dDimlinePoint(MYON_3dPoint pt);

  bool Get3dPoints(
    MYON_3dPoint* center_pt,
    MYON_3dPoint* radius_pt,
    MYON_3dPoint* dimline_pt,
    MYON_3dPoint* knee_pt) const;

  bool GetDisplayLines(
    const MYON_DimStyle* style,
    double dimscale,
    MYON_3dPoint text_rect[4],
    MYON_Line lines[9],
    bool isline[9],
    int maxlines) const;

  void GetArrowXform(
    double scale,
    MYON_Xform& arrow_xform_out) const;

protected:
  MYON_2dPoint        m_radius_pt = MYON_2dPoint::UnsetPoint;
  MYON_2dPoint        m_dimline_pt = MYON_2dPoint::UnsetPoint;
};


//---------------------------------------------------------------------
//            + dimpt
//            |
//            |
//            |
//            + kinkpt2
//             \
//              \  kinkoffset2
//               \
//                + kinkpt1
//                |
//                |  kinkoffset1
//                |
//                +  ldrpt
//                1
//                2
//                3

class MYON_CLASS MYON_DimOrdinate : public MYON_Dimension
{
  MYON_OBJECT_DECLARE(MYON_DimOrdinate);

public:
  MYON_DimOrdinate();
  ~MYON_DimOrdinate() = default;
  MYON_DimOrdinate(const MYON_DimOrdinate& src) = default;
  MYON_DimOrdinate& operator=(const MYON_DimOrdinate& src) = default;

  static const MYON_DimOrdinate Empty;

#pragma region RH_C_SHARED_ENUM [MYON_DimOrdinate::MeasuredDirection] [Rhino.Geometry.OrdinateDimension.MeasuredDirection] [nested:byte]
  /// <summary>
  /// Ordinate dimension measures x or y direction
  /// </summary>
  enum class MeasuredDirection : unsigned char
  {
    /// <summary> </summary>
    Unset = 0,
    /// <summary> Measures horizontal distance </summary>
    Xaxis = 1,
    /// <summary> Measures vertical distance </summary>
    Yaxis = 2,
  };
#pragma endregion

  static MYON_DimOrdinate::MeasuredDirection MeasuredDirectionFromUnsigned(
    unsigned int measured_direction_as_unsigned
    );

  static MYON_DimOrdinate* CreateFromV5DimOrdinate(
    const class MYON_OBSOLETE_V5_DimOrdinate& V5_dim_ordinate,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_DimOrdinate* destination
  );

  bool Write(
    MYON_BinaryArchive& archive
    ) const override;

  bool Read(
    MYON_BinaryArchive& archive
    ) override;

  bool Transform(const MYON_Xform& xform) override;

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

  // Gets transform for dimension text from MYON_xy_plane to 3d display location
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

  bool Create(
    const MYON_UUID style_id,
    const MYON_Plane& plane,
    MeasuredDirection direction,
    const MYON_3dPoint& basept,
    const MYON_3dPoint& defpt,
    const MYON_3dPoint& ldrpt,
    double kinkoffset1,
    double kinkoffset2
    );

  bool AdjustFromPoints(
    const MYON_Plane& plane,
    MeasuredDirection direction,
    const MYON_3dPoint& basept,
    const MYON_3dPoint& defpt,
    const MYON_3dPoint& ldrpt,
    double kinkoffset1,
    double kinkoffset2
    );

  MYON_2dPoint DefPt() const;
  MYON_2dPoint LeaderPt() const;
  MYON_2dPoint KinkPt1() const;
  MYON_2dPoint KinkPt2() const;
  double KinkOffset1() const;
  double KinkOffset2() const;

  void Set2dDefPt(MYON_2dPoint pt);
  void Set2dLeaderPt(MYON_2dPoint pt);
  void SetKinkOffset1(double d);
  void SetKinkOffset2(double d);

  void Set3dBasePoint(MYON_3dPoint pt);
  void Set3dDefPt(MYON_3dPoint pt);
  void Set3dLeaderPt(MYON_3dPoint pt);

  MYON_3dPoint Get3dBasePoint() const;
  MYON_3dPoint Get3dDefPt() const;
  MYON_3dPoint Get3dLeaderPt() const;
  MYON_3dPoint Get3dKinkPt1(double default_kink_offset = 1.0) const;
  MYON_3dPoint Get3dKinkPt2(double default_kink_offset = 1.0) const;

  bool Get3dPoints(
    MYON_3dPoint* base_pt,
    MYON_3dPoint* def_pt,
    MYON_3dPoint* ldr_pt,
    MYON_3dPoint* kink_pt1,
    MYON_3dPoint* kink_pt2,
    double default_kink_offset = 1.0) const;

  bool GetDisplayLines(
    const MYON_DimStyle* style,
    double dimscale,
    MYON_3dPoint text_rect[4],
    MYON_Line lines[3],
    bool isline[3],
    int maxlines) const;

  bool CalcKinkPoints(
    MYON_2dPoint defpt,
    MYON_2dPoint ldrpt,
    MeasuredDirection direction,
    double default_kink_offset,
    MYON_2dPoint& kinkpt1_out,
    MYON_2dPoint& kinkpt2_out) const;

  MeasuredDirection ImpliedDirection(
    MYON_2dPoint defpt,
    MYON_2dPoint ldrpt
    ) const;

  MeasuredDirection GetMeasuredDirection() const;
  void SetMeasuredDirection(MeasuredDirection direction);

  double Measurement() const override;

protected:
  // Plane origin is base for measurements
  // Measurements are from plane origin to dimension point
  // in either x or y axis direction
  MeasuredDirection  m_direction = MeasuredDirection::Unset;

  MYON_2dPoint        m_def_pt = MYON_2dPoint::UnsetPoint;
  MYON_2dPoint        m_ldr_pt = MYON_2dPoint::UnsetPoint;

  double            m_kink_offset_1 = MYON_UNSET_VALUE; // measures from defpt1 toward defpt2 to kink1
  double            m_kink_offset_2 = MYON_UNSET_VALUE; // measures from kink1 toward defpt2 to kink2
};


//---------------------------------------------------------------------

class MYON_CLASS MYON_Centermark : public MYON_Dimension
{
  MYON_OBJECT_DECLARE(MYON_Centermark);

public:
  MYON_Centermark();
  ~MYON_Centermark() = default;
  MYON_Centermark(const MYON_Centermark& src) = default;
  MYON_Centermark& operator=(const MYON_Centermark& src) = default;

  static const MYON_Centermark Empty;

  bool Write(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const override;

  bool Read(
    MYON_BinaryArchive&  // restore definition from binary archive
    ) override;

  bool Transform(const MYON_Xform& xform) override;

  bool GetTextXform(
    const MYON_Viewport*,
    const MYON_DimStyle*,
    double,
    MYON_Xform&
  ) const override;

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

  bool Create(
    const MYON_UUID style_id,
    const MYON_Plane& plane,
    const MYON_3dPoint& center_pt,
    const double radius
    );

  bool AdjustFromPoints(
    const MYON_Plane& plane,
    const MYON_3dPoint& center_pt
    );

  double Measurement() const override;

  MYON_2dPoint CenterPoint() const;
  void Set2dCenterPoint(MYON_2dPoint pt);
  void Set3dCenterPoint(MYON_3dPoint pt);

  bool GetDisplayLines(
    const MYON_DimStyle* style,
    double dimscale,
    MYON_Line lines[6],
    bool isline[6],
    int maxlines) const;

  double Radius() const;         // radius of marked circle
  void SetRadius(double radius);

private:
  double m_radius = 0.0;
};




#endif


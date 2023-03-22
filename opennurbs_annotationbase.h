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

#if !defined(OPENNURBS_ANNOTATIONBASE_INC_)
#define OPENNURBS_ANNOTATIONBASE_INC_




class MYON_CLASS MYON_Annotation : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_Annotation);

protected:
  MYON_Annotation( MYON::AnnotationType annotation_type );
  MYON_Annotation( const MYON_Annotation& src);
  ~MYON_Annotation();
  MYON_Annotation& operator=(const MYON_Annotation& src);

public:
  static MYON_Annotation* CreateFromV2Annotation(
    const class MYON_OBSOLETE_V2_Annotation& V2_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );

public:
  static MYON_Annotation* CreateFromV5Annotation(
    const class MYON_OBSOLETE_V5_Annotation& V5_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );

protected:
  void Internal_SetDimStyleFromV5Annotation(
    const class MYON_OBSOLETE_V5_Annotation& V5_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );


private:
  MYON_Annotation() = delete;

private:
  void Internal_CopyFrom(const MYON_Annotation& src);
  void Internal_Destroy();

public:
  /*
  Returns:
    An MYON::AnnotationType value that indicates the
    type of the annotation.
  */
  MYON::AnnotationType Type() const;

  MYON::object_type ObjectType() const override;
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  virtual bool GetAnnotationBoundingBox(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    double* boxmin,
    double* boxmax,
    bool bGrow = false
  ) const = 0;

  /*
  Parameters:
    vp - [in]
      nullptr or viewport where annotation object is displayed
    dimstyle - [in]
      &this->DimensionStyle(const MYON_DimStyle& parent_dimstyle)
    bApplyDimStyleDimScale - [in]
      If true, dimsytyle->DimScale() is applied.
      If vp is a page view, bApplyDimStyleDimScale is generally false.
      If vp is a model view, bApplyDimStyleDimScale is generally 
      the value of a model property IsAnnotationScalingEnabled().
      from 
    bSingleStrokeFont - [in]
      True if text uses a single font that is a single stroke font and returned contours
      should be left open.
    text_contours - [out]
  */
  bool GetTextGlyphContours(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    bool bApplyDimStyleDimScale,
    bool bSingleStrokeFont,
    MYON_ClassArray< MYON_ClassArray< MYON_SimpleArray< MYON_Curve* > > >& text_contours
  ) const;

protected:

  /*
  Parameters:
    vp - [in]
      nullptr or viewport where annotation object is displayed
    dimstyle - [in]
      &this->DimensionStyle(const MYON_DimStyle& parent_dimstyle)
  */
  const MYON_SHA1_Hash Internal_GetBBox_InputHash(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    const MYON_2dPoint& text_point,
    unsigned int point_count,
    const MYON_2dPoint* points
  ) const;

  /*
  Parameters:
    vp - [in]
      nullptr or viewport where annotation object is displayed
    dimstyle - [in]
      &this->DimensionStyle(const MYON_DimStyle& parent_dimstyle)
  */
  bool Internal_GetBBox_TextGlyphBox(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_BoundingBox& text_glyph_box
  ) const;

  /*
  Returns:
    True if a cached bounding box was found
    and boxmin, boxmax are set.
  */
  bool Internal_GetBBox_Begin(
    const MYON_SHA1_Hash& hash,
    double* boxmin,
    double* boxmax,
    bool bGrow
  ) const;

  /*
  Returns:
    True if a boxmin, boxmax is a valid bounding box
  */
  bool Internal_GetBBox_End(
    const MYON_BoundingBox& bbox,
    const MYON_SHA1_Hash& hash,
    double* boxmin,
    double* boxmax,
    bool bGrow
  ) const;

public:
  virtual bool GetTextXform(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const = 0;

  bool GetTextXform(
    const MYON_Xform* model_xform,
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const;

  void SetPlane(const MYON_Plane& plane);
  const MYON_Plane& Plane() const;

  void SetHorizontalDirection(MYON_2dVector);
  const MYON_2dVector HorizontalDirection() const;

  // Returns a 2d vector to use as annotation horizontal
  // Use this function when you don't have a known horizontal direction
  static MYON_3dVector GetDefaultHorizontal(const MYON_Plane& plane);


  static void CalcTextFlip(
    const MYON_3dVector& text_xdir, const MYON_3dVector& text_ydir, const MYON_3dVector& text_zdir,
    const MYON_3dVector& view_xdir, const MYON_3dVector& view_ydir, const MYON_3dVector& view_zdir,
    const MYON_Xform* model_xform,
    const double flip_tol,
    bool& flip_x,
    bool& flip_y);

  /*
  Returns:
    Rich text that can contain rich text formatting instructions.
  */
  const MYON_wString RichText() const;

  /*
  Returns:
  Text information with rich text formatting insturctions removed.
  Fields are not evaluated.
  */
  const MYON_wString PlainText() const;

  /*
  Returns:
  Text information with rich text formatting insturctions removed.
  The result string from evaluating fields is included
  Field results may be cached from previous evaluation
  */
  const MYON_wString PlainTextWithFields() const;

  /*
  Finds the positions and lengths of substrings in the string returned by PlainTextWithFields()
    That string is the plain text (no rtf formatting) with field source unevaluated
    Each 3dex in the array is 
    i: run index, 
    j: position in the string where text from run[i] starts, 
    k: length of text from run[i]

  Returns the same string that PlainTextWithFields() returns
  */
  const MYON_wString PlainTextWithFields(MYON_SimpleArray<MYON_3dex>* runmap) const;

  // Return the id of the main (parent) dimstyle used by this object.
  // The style with this id should not be used directly if there is 
  // an override dimstyle present. 
  // Use this->DimensionStyle(parent_style) to get the effective
  // dimstyle for this object.
  MYON_UUID DimensionStyleId() const;

  // Sets the id of the main (parent) dimstyle used by this annotation object
  // Any override dimstyle on this object will be deleted when this is called,
  // resetting any style overrides.
  void SetDimensionStyleId(MYON_UUID dimstyle_id);

  /*
  Description:
    Set the id of the main (parent) dimstyle used by this annotation object
    and allow an expert user to control what happens to style override settings
    in cases where id collisions occur and ids need to be changed.
  Parameters:
    bKeepOverrides - [in]
      If you are not an expert oding something low level and complicated, then
      call SetDimensionStyleId(dimstyle_id) or pass bKeepOverrides = false.

      If bKeepOverrides is true and dimstyle_id is not nil and this object has 
      valid overrides, those overrides are retained. In all other cases, any 
      existing overrides are deleted.
  */
  void SetDimensionStyleIdForExperts(
    MYON_UUID dimstyle_id,
    bool bKeepOverrides
  );

  /*
  parameters:
    dim_style - [in]
      If dim_style.ParentId() is nil, then this function
      calls SetDimensionStyleId(dim_style.Id()) and returns.
      If dim_style.ParentId() is not nil, then this functions 
      calls SetDimensionStyleId(dim_style.ParentId()) and uses a copy
      of dim_style as the override dimstyle.
  */
  void SetDimensionStyleId(
    const class MYON_DimStyle& dim_style
    );

  // Get the proper dimension style, including overrides, to use for this
  // annotation object.
  // If there is an override in place, that dimstyle will be returned
  // If there is no override, the parent style passed in will be returned
  // If the content of the parent style has changed since the override was made,
  // the override style will be updated with the non-overriden values from  
  // the parent before returning.
  // If your annotation object has an override style and you call either of
  // these functions with a dimstyle other than the correct parent style
  // for this annotation, the override style will be removed.
  const MYON_DimStyle& DimensionStyle(const MYON_DimStyle& parent_dimstyle) const;
  const MYON_DimStyle& DimensionStyle(
    const MYON_DimStyle& parent_dimstyle,
    bool bForceOverrideUpdate
  ) const;

  // Apply a dimstyle with overrides set to this annotation object.
  //
  // Use MYON_Annotation::IsOverrideDimStyleCandidate() to determine if a non-nullptr
  // override_style is a valid to be used to set overrides.
  //
  // The override dimstyle memory will be managed and deleted by the annotation object and
  // must have been allocated using new. 
  // On return, if this function returns true, 
  // The dimstyle id of the annotation object must be set before this function is called.
  // Calling SetOverrideDimensionStyle(nullptr) will remove all overrides for this object.
  // override_dimstyle will be null.
  //
  // Returns:
  //   true if the override style was successfully set
  //   false if this->m_dimstyle_id is MYON_nil_uuid causing failure
  bool SetOverrideDimensionStyle(MYON_DimStyle*& override_style) const;

  /*
  Description:
    Removes any override dimension style that is present.
  */
  void ClearOverrideDimensionStyle();

  /*
  Description:
    If this->IsOverrideDimStyleCandidate(override_style_candidate,bRequireSetOverrides)
    is true, then a managed copy of override_style_candidate is set as an override.
  Returns:
    True if an override is set.
  */
  bool SetOverrideDimensionStyle(
    const MYON_DimStyle* override_style_candidate,
    bool bRequireSetOverrides
  );


  /*
  Description:
    A valid override dimstyle candidate has all of the following properties.
      override_style_candidate != nullptr.
      IsDeleted() = false;
      Id() = MYON_nil_uuid;
      Name() is empty.
      Index() = MYON_ModelComponent::Unset.Index()
      bRequireSetOverrides is false or HasOverrides() returns true.
  Parameters:
    override_style_candidate -[in]
      style candidate to evaluate.
    bRequireSetOverrides - [in]
      If bRequireSetOverrides is true, then override_style_candidate->HasOverrides() must be true for a valid candidate.
      If bRequireSetOverrides is flase, then override_style_candidate->HasOverrides() can have any value.
  Returns:
    True if override_style could be successfully used as the parameter 
    to SetOverrideDimensionStyle.
  */
  bool IsOverrideDimStyleCandidate(
    const MYON_DimStyle* override_style_candidate,
    bool bRequireSetOverrides
  ) const;


  /*
  Description:
    Conceptually, calling this function applies MYON_DimsStyle(scale) to the 
    dimstyle information used for this annotation. 
    
    When an annotation object is in **layout/page space**, this is
    the only way top get properties like TextHeight() to scale properly. 
    
    When an annotation object is in **model space** and 
    **model space scaling is enabled**,
    then calling this->SetDimScale(this->DimScale()*scale)
    will work as well.

  Parameters:
    parent_dimstyle - [in]
    scale - [in]
  */
  void ScaleOverrideDimstyle(
    const MYON_DimStyle* parent_dimstyle,
    double scale
  );

protected:
  static bool Internal_IsOverrideDimStyleCandidate(
    const MYON_DimStyle* override_style_candidate,
    MYON_UUID parent_id,
    bool bRequireSetOverrides,
    bool bIssueErrorsAndWarnings
  );


public:
  // Quickly check if this annotation object has style overrides applied.
  bool HasDimensionStyleOverrides() const;

  const MYON_TextContent* Text() const;
  MYON_TextContent* Text();
  void SetText(MYON_TextContent*& text) const;
  void ClearText() const;

  // return angle in radians between text plane and object plane
  virtual double TextRotationRadians() const;
  virtual void SetTextRotationRadians(double rotation);

  // return angle in degrees between text plane and object plane
  virtual double TextRotationDegrees() const;
  virtual void SetTextRotationDegrees(double rotation);

  //virtual bool Explode(
  //  const MYON_DimStyle* dimstyle,
  //  MYON_SimpleArray<const MYON_Geometry*> object_parts) const = 0;

  /*
  Returns:
     The value of MYON_DimStyle.TextPositionPropertiesHash() from the dimension style used
     to calculate the runtime text position (location, glyphs, and size).
  */
  MYON_SHA1_Hash DimStyleTextPositionPropertiesHash() const;

  /*
  Returns:
    True if this text position information used to create this text
    is identical to the text position parameters on dimstyle.
  */
  bool EqualTextPositionProperties(
    const class MYON_DimStyle* dimstyle
  ) const;

  const wchar_t* RtfText() const;

  bool ReplaceTextString(
    const wchar_t* RtfString,
    const MYON_DimStyle* dimstyle
  );

  bool RunReplaceString(
    const MYON_DimStyle* dimstyle,
    const wchar_t* str,
    int start_run_idx,
    int start_run_pos,
    int end_run_idx,
    int end_run_pos);


  // Deprecated - Use 
  // MYON::TextVerticalAlignment MYON_Annotation::TextVerticalAlignment(const MYON_DimStyle* parent_style) const;
  // void MYON_Annotation::SetTextVerticalAlignment(const MYON_DimStyle* parent_style, MYON::TextVerticalAlignment style);
  // MYON::TextVerticalAlignment MYON_Annotation::LeaderVerticalAlignment(const MYON_DimStyle* parent_style) const;
  // void MYON_Annotation::SetLeaderVerticalAlignment(const MYON_DimStyle* parent_style, MYON::TextVerticalAlignment style);
  void GetAlignment(MYON::TextHorizontalAlignment& horz, MYON::TextVerticalAlignment& vert) const;
  void SetAlignment(MYON::TextHorizontalAlignment horz, MYON::TextVerticalAlignment vert);

  // FormattingRectangleWidth is a width set by text wrapping. It's in model units
  double FormattingRectangleWidth() const;
  void SetFormattingRectangleWidth(double width);

  // Get corners of the whole text object
  // corners requires space for 4 points
  bool GetText3dCorners(MYON_3dPoint corners[4]) const;

  /*
  Parameters:
    ptr - [in]
      pointer to test
  Returns:
    True if ptr is not nullptr and points to the override style mangaged by this 
    instance.
  */
  bool IsOverrideStylePointer(
    const MYON_DimStyle* ptr
  ) const;

  // These functions are being added to continue the V5 behavior of
  // per-object text scaling. There is no user interface
  // in V6 or V7 that shows this setting or that allows a user
  // to change this setting.
  // AllowTextScaling() = false means the effective dimstyle value 
  // of DimScale() (model space scale factor) is ignored (treated as if it were 1).
  bool AllowTextScaling() const;
  void SetAllowTextScaling(bool scale);

protected:
  MYON::AnnotationType m_annotation_type = MYON::AnnotationType::Unset;
  bool m_allow_text_scaling = true;
  unsigned char m_reserved2 = 0;
  unsigned char m_reserved3 = 0;
  unsigned int m_reserved4 = 0;
  MYON_UUID m_dimstyle_id = MYON_DimStyle::Default.Id();
  MYON_Plane m_plane = MYON_Plane::World_xy; // plane origin used for alignment point
  MYON_2dVector m_horizontal_direction = MYON_2dVector::XAxis;  // direction used as horizontal to draw annotation, especially text
  mutable MYON_TextContent* m_text = nullptr;  // Deleted by ~MYON_Annotation()
private:
  // Pointer to an override dimstyle when style properties are overridden for this annotation object
  // If this pointer is null, use the style with id = m_dimstyle_id
  // Copy and delete this dimstyle (not this pointer) with the object.
  // This dimstyle should never be one held in a dimstyle table.  It is owned by this object
  mutable MYON_DimStyle* m_override_dimstyle = nullptr;
  mutable MYON__UINT64 m_parent_dimstyle_content_version_number = 0;
  void Internal_DeleteOverrideDimstyle() const;

  mutable MYON_BoundingBoxCache m_bbox_cache;

protected:
  bool Internal_WriteAnnotation(
    MYON_BinaryArchive& archive
    ) const;

  bool Internal_ReadAnnotation(
    MYON_BinaryArchive& archive
    );

private:
  MYON_DimStyle* Internal_GetOverrideStyle(bool bCreateIfNull) const;

  /*
  Description:
    Gets the appropriate MYON_DimStyle to query for a property value.
  Parameters:
    parent_style - [in]
      parent style pased to the MYON_Annotation query function
    field_id - [in]
      field being queried - this is used to select between using the override style or the parent style.
  */

  const MYON_DimStyle& Internal_StyleForFieldQuery(
    const MYON_DimStyle* parent_style,
    MYON_DimStyle::field field_id
  ) const;

private:
  static bool Internal_DimStyleDoubleChanged(
    const double current_value,
    double candidate_value
  );

public:
  void ClearFieldOverride(MYON_DimStyle::field field);
   
  bool FieldIsOverridden(MYON_DimStyle::field field) const;

  // These next several functions are to set overrides on individual annotation objects

  // Extension line extension
  double ExtensionLineExtension(const MYON_DimStyle* parent_style) const;
  void SetExtensionLineExtension(const MYON_DimStyle* parent_style, double d);

  // Extension line offset
  double ExtensionLineOffset(const MYON_DimStyle* parent_style) const;
  void SetExtensionLineOffset(const MYON_DimStyle* parent_style, double d);

  // Arrow size
  double ArrowSize(const MYON_DimStyle* parent_style) const;
  void SetArrowSize(const MYON_DimStyle* parent_style, double d);

  // Arrow size
  double LeaderArrowSize(const MYON_DimStyle* parent_style) const;
  void SetLeaderArrowSize(const MYON_DimStyle* parent_style, double d);

  // Centermark size
  double CenterMarkSize(const MYON_DimStyle* parent_style) const;
  void SetCenterMarkSize(const MYON_DimStyle* parent_style, double d);

  // Centermark style
  MYON_DimStyle::centermark_style CenterMarkStyle(const MYON_DimStyle* parent_style) const;
  void SetCenterMarkStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::centermark_style style);

  // The location of text relative to the dimension line in linear, angular, and ordinate dimensions.
  MYON_DimStyle::TextLocation DimTextLocation(const MYON_DimStyle* parent_style) const;
  void SetDimTextLocation(const MYON_DimStyle* parent_style, MYON_DimStyle::TextLocation dim_text_location);

  // The location of text relative to the dimension line in radial dimensions.
  MYON_DimStyle::TextLocation DimRadialTextLocation(const MYON_DimStyle* parent_style) const;
  void SetDimRadialTextLocation(const MYON_DimStyle* parent_style, MYON_DimStyle::TextLocation dim_text_location);

  // Angle units - Degrees, Degrees-Minutes-Seconds, Radians
  MYON_DimStyle::angle_format AngleFormat(const MYON_DimStyle* parent_style) const;
  void SetAngleFormat(const MYON_DimStyle* parent_style, MYON_DimStyle::angle_format format);

  // Display resolution for distance measurements
  int LengthResolution(const MYON_DimStyle* parent_style) const;
  void SetLengthResolution(const MYON_DimStyle* parent_style, int r);

  // Display resolution for angle measurements
  int AngleResolution(const MYON_DimStyle* parent_style) const;
  void SetAngleResolution(const MYON_DimStyle* parent_style, int r);

  // Distance from dimension lines to text
  double TextGap(const MYON_DimStyle* parent_style) const;
  void SetTextGap(const MYON_DimStyle* parent_style, double gap);

  // Height of dimension text
  double TextHeight(const MYON_DimStyle* parent_style) const;
  void SetTextHeight(const MYON_DimStyle* parent_style, double height);

  // Scale factor for displayed distances
  double LengthFactor(const MYON_DimStyle* parent_style) const;
  void SetLengthFactor(const MYON_DimStyle* parent_style, double);

  // Additional measurement display toggle
  bool Alternate(const MYON_DimStyle* parent_style) const;
  void SetAlternate(const MYON_DimStyle* parent_style, bool);

  // Distance scale factor for alternate display
  double AlternateLengthFactor(const MYON_DimStyle* parent_style) const;
  void SetAlternateLengthFactor(const MYON_DimStyle* parent_style, double);

  // Display resolution for alternate length measurements
  int AlternateLengthResolution(const MYON_DimStyle* parent_style) const;
  void SetAlternateLengthResolution(const MYON_DimStyle* parent_style, int);

  // Dimension prefix text
  const wchar_t* Prefix(const MYON_DimStyle* parent_style) const;
  void SetPrefix(const MYON_DimStyle* parent_style, const wchar_t*);

  // Dimension suffix text
  const wchar_t* Suffix(const MYON_DimStyle* parent_style) const;
  void SetSuffix(const MYON_DimStyle* parent_style, const wchar_t*);

  // Dimension alternate prefix text
  const wchar_t* AlternatePrefix(const MYON_DimStyle* parent_style) const;
  void SetAlternatePrefix(const MYON_DimStyle* parent_style, const wchar_t*);

  // Dimension alternate suffix text
  const wchar_t* AlternateSuffix(const MYON_DimStyle* parent_style) const;
  void SetAlternateSuffix(const MYON_DimStyle* parent_style, const wchar_t*);

  // Suppress first dimension extension line
  bool SuppressExtension1(const MYON_DimStyle* parent_style) const;
  void SetSuppressExtension1(const MYON_DimStyle* parent_style, bool b);

  // Suppress second dimension extension line
  bool SuppressExtension2(const MYON_DimStyle* parent_style) const;
  void SetSuppressExtension2(const MYON_DimStyle* parent_style, bool b);

  // Extension of dimension line past extension lines
  double DimExtension(const MYON_DimStyle* parent_style) const;
  void SetDimExtension(const MYON_DimStyle* parent_style, const double e);

  MYON_DimStyle::tolerance_format ToleranceFormat(const MYON_DimStyle* parent_style) const;
  void SetToleranceFormat(const MYON_DimStyle* parent_style, MYON_DimStyle::tolerance_format format);

  int  ToleranceResolution(const MYON_DimStyle* parent_style) const;
  void SetToleranceResolution(const MYON_DimStyle* parent_style, int resolution);

  double ToleranceUpperValue(const MYON_DimStyle* parent_style) const;
  void SetToleranceUpperValue(const MYON_DimStyle* parent_style, double upper_value);

  double ToleranceLowerValue(const MYON_DimStyle* parent_style) const;
  void SetToleranceLowerValue(const MYON_DimStyle* parent_style, double lower_value);

  double ToleranceHeightScale(const MYON_DimStyle* parent_style) const;
  void SetToleranceHeightScale(const MYON_DimStyle* parent_style, double scale);

  double BaselineSpacing(const MYON_DimStyle* parent_style) const;
  void SetBaselineSpacing(const MYON_DimStyle* parent_style, double spacing);

  // Determines whether or not to draw a Text Mask
  bool DrawTextMask(const MYON_DimStyle* parent_style) const;
  void SetDrawTextMask(const MYON_DimStyle* parent_style, bool bDraw);

  // Determines where to get the color to draw a Text Mask
  MYON_TextMask::MaskType  MaskFillType(const MYON_DimStyle* parent_style) const;
  void SetMaskFillType(const MYON_DimStyle* parent_style, MYON_TextMask::MaskType  source);

  // Determines whether to draw a frame around a text mask
  MYON_TextMask::MaskFrame  MaskFrameType(const MYON_DimStyle* parent_style) const;
  void SetMaskFrameType(const MYON_DimStyle* parent_style, MYON_TextMask::MaskFrame source);

  MYON_Color MaskColor(const MYON_DimStyle* parent_style) const;  // Only works right if MaskColorSource returns 1.
  void SetMaskColor(const MYON_DimStyle* parent_style, MYON_Color color);   // Does not return viewport background color

  // Offset for the border around text to the rectangle used to draw the mask
  // This number is the offset on each side of the tight rectangle around the
  // text characters to the mask rectangle.
  double MaskBorder(const MYON_DimStyle* parent_style) const;
  void SetMaskBorder(const MYON_DimStyle* parent_style, double offset);

  // The MYON_TextMask class contains the property values for
  //   DrawTextMask()
  //   MaskColor()
  //   MaskFillType()
  //   MaskBorder()
  // Use the 
  //   DrawTextMask()
  //   MaskColor()
  //   MaskFillType()
  //   MaskBorder()
  //   functions to query individual text mask properties.
  void SetTextMask(const MYON_DimStyle* parent_style, const MYON_TextMask& mask);
  
  double FixedExtensionLength(const MYON_DimStyle* parent_style) const;
  void SetFixedExtensionLength(const MYON_DimStyle* parent_style, double l);

  bool FixedExtensionLengthOn(const MYON_DimStyle* parent_style) const;
  void SetFixedExtensionLengthOn(const MYON_DimStyle* parent_style, bool on);

  int AlternateToleranceResolution(const MYON_DimStyle* parent_style) const;
  void SetAlternateToleranceResolution(const MYON_DimStyle* parent_style, int r);

  bool SuppressArrow1(const MYON_DimStyle* parent_style) const;
  void SetSuppressArrow1(const MYON_DimStyle* parent_style, bool s);

  bool SuppressArrow2(const MYON_DimStyle* parent_style) const;
  void SetSuppressArrow2(const MYON_DimStyle* parent_style, bool s);

  int TextMoveLeader(const MYON_DimStyle* parent_style) const;
  void SetTextMoveLeader(const MYON_DimStyle* parent_style, int m);

  int ArcLengthSymbol(const MYON_DimStyle* parent_style) const;
  void SetArcLengthSymbol(const MYON_DimStyle* parent_style, int m);

  MYON_DimStyle::stack_format StackFractionFormat(const MYON_DimStyle* parent_style) const;
  void SetStackFractionFormat(const MYON_DimStyle* parent_style, MYON_DimStyle::stack_format f);

  double StackHeightScale(const MYON_DimStyle* parent_style) const;
  void SetStackHeightScale(const MYON_DimStyle* parent_style, double f);

  double RoundOff(const MYON_DimStyle* parent_style) const;
  void SetRoundOff(const MYON_DimStyle* parent_style, double r);

  double AlternateRoundOff(const MYON_DimStyle* parent_style) const;
  void SetAlternateRoundOff(const MYON_DimStyle* parent_style, double r);

  double AngleRoundOff(const MYON_DimStyle* parent_style) const;
  void SetAngleRoundOff(const MYON_DimStyle* parent_style, double r);

  MYON_DimStyle::suppress_zero ZeroSuppress(const MYON_DimStyle* parent_style) const;
  void SetZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero s);

  MYON_DimStyle::suppress_zero AlternateZeroSuppress(const MYON_DimStyle* parent_style) const;
  void SetAlternateZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero s);

  // OBSOLETE - The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
  MYON_DimStyle::suppress_zero ToleranceZeroSuppress(const MYON_DimStyle* parent_style) const;

  // OBSOLETE - The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
  void SetToleranceZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero s);

  MYON_DimStyle::suppress_zero AngleZeroSuppress(const MYON_DimStyle* parent_style) const;
  void SetAngleZeroSuppress(const MYON_DimStyle* parent_style, MYON_DimStyle::suppress_zero s);

  bool AlternateBelow(const MYON_DimStyle* parent_style) const;
  void SetAlternateBelow(const MYON_DimStyle* parent_style, bool below);

  MYON_Arrowhead::arrow_type ArrowType1(const MYON_DimStyle* parent_style) const;
  void SetArrowType1(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type);

  MYON_Arrowhead::arrow_type ArrowType2(const MYON_DimStyle* parent_style) const;
  void SetArrowType2(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type);

  void SetArrowType1And2(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type);

  MYON_Arrowhead::arrow_type LeaderArrowType(const MYON_DimStyle* parent_style) const;
  void SetLeaderArrowType(const MYON_DimStyle* parent_style, MYON_Arrowhead::arrow_type);

  MYON_UUID ArrowBlockId1(const MYON_DimStyle* parent_style) const;
  void SetArrowBlockId1(const MYON_DimStyle* parent_style, MYON_UUID id);

  MYON_UUID ArrowBlockId2(const MYON_DimStyle* parent_style) const;
  void SetArrowBlockId2(const MYON_DimStyle* parent_style, MYON_UUID id);

  MYON_UUID LeaderArrowBlockId(const MYON_DimStyle* parent_style) const;
  void SetLeaderArrowBlockId(const MYON_DimStyle* parent_style, MYON_UUID id);

  MYON::TextVerticalAlignment TextVerticalAlignment(const MYON_DimStyle* parent_style) const;
  void SetTextVerticalAlignment(const MYON_DimStyle* parent_style, MYON::TextVerticalAlignment style);

  MYON::TextVerticalAlignment LeaderTextVerticalAlignment(const MYON_DimStyle* parent_style) const;
  void SetLeaderTextVerticalAlignment(const MYON_DimStyle* parent_style, MYON::TextVerticalAlignment style);

  MYON_DimStyle::ContentAngleStyle LeaderContentAngleStyle(const MYON_DimStyle* parent_style) const;
  void SetLeaderContentAngleStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::ContentAngleStyle style);

  MYON_DimStyle::leader_curve_type LeaderCurveType(const MYON_DimStyle* parent_style) const;
  void SetLeaderCurveType(const MYON_DimStyle* parent_style, MYON_DimStyle::leader_curve_type type);

  bool LeaderHasLanding(const MYON_DimStyle* parent_style) const;
  void SetLeaderHasLanding(const MYON_DimStyle* parent_style, bool landing);

  double LeaderLandingLength(const MYON_DimStyle* parent_style) const;
  void SetLeaderLandingLength(const MYON_DimStyle* parent_style, double length);

  double LeaderContentAngleRadians(const MYON_DimStyle* parent_style) const;
  void SetLeaderContentAngleRadians(const MYON_DimStyle* parent_style, double angle_radians);

  double LeaderContentAngleDegrees(const MYON_DimStyle* parent_style) const;
  void SetLeaderContentAngleDegrees(const MYON_DimStyle* parent_style, double angle_degrees);

  MYON_DimStyle::ContentAngleStyle DimTextAngleStyle(const MYON_DimStyle* parent_style) const;
  void SetDimTextAngleStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::ContentAngleStyle style);

  MYON_DimStyle::ContentAngleStyle DimRadialTextAngleStyle(const MYON_DimStyle* parent_style) const;
  void SetDimRadialTextAngleStyle(const MYON_DimStyle* parent_style, MYON_DimStyle::ContentAngleStyle style);

  MYON::TextHorizontalAlignment TextHorizontalAlignment(const MYON_DimStyle* parent_style) const;
  void SetTextHorizontalAlignment(const MYON_DimStyle* parent_style, MYON::TextHorizontalAlignment halign);

  MYON::TextHorizontalAlignment LeaderTextHorizontalAlignment(const MYON_DimStyle* parent_style) const;
  void SetLeaderTextHorizontalAlignment(const MYON_DimStyle* parent_style, MYON::TextHorizontalAlignment halign);

  MYON::TextOrientation TextOrientation(const MYON_DimStyle* parent_style) const;
  void SetTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation orientation);

  MYON::TextOrientation LeaderTextOrientation(const MYON_DimStyle* parent_style) const;
  void SetLeaderTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation orientation);

  MYON::TextOrientation DimTextOrientation(const MYON_DimStyle* parent_style) const;
  void SetDimTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation orientation);

  MYON::TextOrientation DimRadialTextOrientation(const MYON_DimStyle* parent_style) const;
  void SetDimRadialTextOrientation(const MYON_DimStyle* parent_style, MYON::TextOrientation orientation);

  bool DrawForward(const MYON_DimStyle* parent_style) const;
  void SetDrawForward(const MYON_DimStyle* parent_style, bool drawforward);

  bool TextUnderlined(const MYON_DimStyle* parent_style) const;
  void SetTextUnderlined(const MYON_DimStyle* parent_style, bool underlined);

  bool SignedOrdinate(const MYON_DimStyle* parent_style) const;
  void SetSignedOrdinate(const MYON_DimStyle* parent_style, bool allowsigned);

  double DimScale(const MYON_DimStyle* parent_style) const;
  void SetDimScale(const MYON_DimStyle* parent_style, double scale);

  wchar_t DecimalSeparator(const MYON_DimStyle* parent_style) const;
  void SetDecimalSeparator(const MYON_DimStyle* parent_style, wchar_t separator);

  MYON_DimStyle::LengthDisplay DimensionLengthDisplay(const MYON_DimStyle* parent_style) const;
  void SetDimensionLengthDisplay(const MYON_DimStyle* parent_style, MYON_DimStyle::LengthDisplay length_display);

  MYON_DimStyle::LengthDisplay AlternateDimensionLengthDisplay(const MYON_DimStyle* parent_style) const;
  void SetAlternateDimensionLengthDisplay(const MYON_DimStyle* parent_style, MYON_DimStyle::LengthDisplay length_display);

  /// <summary>
  /// Parameters:
  ///   model_sn - 0, a model serial number, or MYON_UNSET_UINT_INDEX to
  ///   use the dimstyle's ModelSerialNumber() value.
  /// Returns
  ///   Unit system for dimension length display.
  ///   If DimensionLengthDisplay() == MYON_DimStyle::LengthDisplay::ModelUnits
  ///   and model_sn > 0, then the value of MYON::LengthUnitSystemFromModelSerialNumber(model_sn)
  ///   is returned.
  ///   If DimensionLengthDisplay() == MYON_DimStyle::LengthDisplay::ModelUnits
  ///   and model_sn == 0, then MYON::LengthUnitSystem::None is returned.
  ///</summary>
  MYON::LengthUnitSystem DimensionLengthDisplayUnit(
    const MYON_DimStyle* parent_style,
    unsigned int model_sn
    ) const;

  /// <summary>
  /// Parameters:
  ///   model_sn - 0, a model serial number, or MYON_UNSET_UINT_INDEX to
  ///   use the dimstyle's ModelSerialNumber() value.
  /// Returns
  ///   Unit system for dimension length display.
  ///   If DimensionLengthDisplay() == MYON_DimStyle::LengthDisplay::ModelUnits
  ///   and model_sn > 0, then the value of MYON::LengthUnitSystemFromModelSerialNumber(model_sn)
  ///   is returned.
  ///   If DimensionLengthDisplay() == MYON_DimStyle::LengthDisplay::ModelUnits
  ///   and model_sn == 0, then MYON::LengthUnitSystem::None is returned.
  ///</summary>
  MYON::LengthUnitSystem AlternateDimensionLengthDisplayUnit(
    const MYON_DimStyle* parent_style,
    unsigned int model_sn
    ) const;

  /*
  Description:
  Set the font used to render text.
  Parameters:
  font_characteristics - [in]
  This parameter does not have to be a managed font.
  Remarks:
  If the parameter is a managed font (font_characteristics.IsManagedFont() is true),
  then the identical value is returned by MYON_DimStyle.Font().
  If the parameter is not a managed font (font_characteristics.IsManagedFont() is false),
  then the MYON_Font::GetManagedFont(font_characteristics) will be returned by
  MYON_DimStyle.Font().
  */
  void SetFont(const MYON_DimStyle* parent_style,  const class MYON_Font& font_characteristics);

  /*
  Returns:
  The managed font used to render text.
  */
  const class MYON_Font& Font(const MYON_DimStyle* parent_style) const;

  /*
  Returns:
  A copy of the font_characteristics information.
  Remarks:
  You probably want to use Font(). This function is only useful
  in isolated situations and is typically used to study font
  substitutions when a model moves between computers or platforms.
  */
  const class MYON_Font& FontCharacteristics(const MYON_DimStyle* parent_style) const;

  /*
  Returns:
  True if the font returned by Font() is a substitute
  for the font passed to SetFont().
  Remarks:
  Font substitution can occur when a model is moved between
  computers that have different fonts installed.
  */
  const bool FontSubstituted(const MYON_DimStyle* parent_style) const;

  bool SetAnnotationBold(bool bold, const MYON_DimStyle* dimstyle);
  bool SetAnnotationItalic(bool italic, const MYON_DimStyle* dimstyle);
  bool SetAnnotationUnderline(bool underline, const MYON_DimStyle* dimstyle);
  bool SetAnnotationFacename(bool set_or_clear, const wchar_t* facename, const MYON_DimStyle* parent_style);
  bool SetAnnotationFont(const MYON_Font* font, const MYON_DimStyle* parent_style);

  static bool SetAnnotationTextFormat(MYON_wString& rtf_in, const wchar_t* fmt_str_on, const wchar_t* fmt_str_off, bool set_on);

  static bool SetRtfFmt(MYON_wString& rtf_in, const wchar_t* fmt_str);
  static bool ClearRtfFmt(const wchar_t* fmt_str_on, const wchar_t* fmt_str_off, MYON_wString& rtf_in);
  static int FindRtfTable(MYON_wString rtf_in, int startidx, const wchar_t* tablename);

  static bool FirstCharTextProperties(const wchar_t* rtf_in, bool& bold, bool& italic, bool& underline, MYON_wString& facename);
  
  const MYON_Font* FirstCharFont() const;

private:
    bool IsAllFormat(bool (MYON_Font::*func)() const) const;

public:
  // true if all of the text is bold
  bool IsAllBold() const;

  // true if all of the text is italic
  bool IsAllItalic() const;

  // true if all of the text is underlined
  bool IsAllUnderlined() const;

  friend class MYON_Dimension;
};


/*
  A simple dot with text that doesn't rotate with the world axes
*/
class MYON_CLASS MYON_TextDot : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_TextDot);
public:
  static const wchar_t* DefaultFontFace; // Arial
  static const int DefaultHeightInPoints; // 14 points
  static const int MinimumHeightInPoints; // 3 points
  static const MYON_TextDot Unset;

  MYON_TextDot();
  ~MYON_TextDot();
  MYON_TextDot( const MYON_TextDot& ) = default;
  MYON_TextDot& operator=( const MYON_TextDot& ) = default;

  MYON_TextDot(
    MYON_3dPoint center_point,
    const wchar_t* primary_text,
    const wchar_t* secondary_text
    );

  static MYON_TextDot* CreateFromV2AnnotationTextDot(
    const class MYON_OBSOLETE_V2_TextDot& V2_text_dot,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_TextDot* destination
  );

  void EmergencyDestroy();

  //---------------------------
  // MYON_Object overrides

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  /*
    Description: Write data values to a text file for debugging
  */
  void Dump( MYON_TextLog& log) const override;

  /*
    Description: Writes the object to a file

    Returns:
      @untitled Table
      true     Success
      false    Failure
  */
  bool Write( MYON_BinaryArchive& ar) const override;

  /*
    Description: Reads the object from a file

    Returns:
      @untitled Table
      true     Success
      false    Failure
  */
  bool Read( MYON_BinaryArchive& ar) override;

  /*
    Returns: The Object Type of this object
  */
  MYON::object_type ObjectType() const override;

  //---------------------------
  // MYON_Geometry overrides

  /*
    Returns the geometric dimension of the object ( usually 3)
  */
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  /*
    Description:
      Transform the object by a 4x4 xform matrix
    Parameters:
      [in] xform  - An MYON_Xform with the transformation information
    Returns:
      true = Success
      false = Failure
    Remarks:
      The object has been transformed when the function returns
  */
  bool Transform( const MYON_Xform& xform) override;

  // virtual MYON_Geometry::IsDeformable() override
  bool IsDeformable() const override;

  // virtual MYON_Geometry::MakeDeformable() override
  bool MakeDeformable() override;


  MYON_3dPoint CenterPoint() const;
  void SetCenterPoint(
    MYON_3dPoint center_point
    );

  MYON_DEPRECATED_MSG("use CenterPoint")
  const MYON_3dPoint& Point() const;
  MYON_DEPRECATED_MSG("use SetCenterPoint")
  void SetPoint(const MYON_3dPoint& point);

  /*
  Returns:
    Text height in "points".
  Remarks:
    Default height = 14;
  */
  int HeightInPoints() const;
  void SetHeightInPoints(
    int height_in_points
    );

  /*
  Returns:
    Dot's primary text displayed in the model
    Typically a short and terse string.
    Default = empty string.
  Remarks:
    Additional information can be saved as secondary text.

    Never save the pointer value for future use.
    Save a copy in MYON_wString if the text is needed beyond the scope of
    the call to Text().
  */
  const wchar_t* PrimaryText() const;
  void SetPrimaryText( 
    const wchar_t* primary_dot_text
    );

  MYON_DEPRECATED_MSG("use PrimaryText")
  const wchar_t* TextString() const;
  MYON_DEPRECATED_MSG("use SetPrimaryText")
  void SetTextString(const wchar_t* string);
  /*
  Returns:
    Dot's secondary text displayed when a user interface event like cliking or hovering occurs.
    Typically longer and more detailed than the primary text.
    Default = empty string.
  Remarks:
    Never save the pointer value for future use.
    Save a copy in MYON_wString if the text is needed beyond the scope of
    the call to Text().
  */
  const wchar_t* SecondaryText() const;
  void SetSecondaryText(
    const wchar_t* secondary_dot_text
    );


  /*
  Returns:
    Primary text font face.
    Default = "Arial Bold";
  Remarks:
    Never save the pointer value for future use.
    Save a copy in MYON_wString if the text is needed beyond the scope of
    the call to FontFace().
  */
  const wchar_t* FontFace() const;
  void SetFontFace( 
    const wchar_t* font_face)
    ;
  
  /*
    Description:
      Get or Set whether the dot is drawn "On Top" of other geometry
    Parameters:
      [in] bTop  bool - It is or isn't on top
    Returns:
      @untitled table
      true - on top
      false - not on top
  */
  void SetAlwaysOnTop(
    bool bAlwaysOnTop
    );
  bool AlwaysOnTop() const;

  /*
    Description:
      Get or Set whether the dot is drawn with a transparent background
    Parameters:
      [in] bTransparent  bool - It is or isn't transparent
    Returns:
      @untitled table
      true - transparent
      false - not transparent
  */
  void SetTransparent(
    bool bTransparent
    );
  bool Transparent() const;

  /*
    Description:
      Get or Set whether the dot is drawn with Bold text
    Parameters:
      [in] bBold  bool - It is or isn't Bold
    Returns:
      @untitled table
      true - Bold
      false - not Bold
  */
  void SetBold(
    bool bBold
    );
  bool Bold() const;

  /*
    Description:
      Get or Set whether the dot is drawn with Italic text
    Parameters:
      [in] bItalic  bool - It is or isn't Italic
    Returns:
      @untitled table
      true - Italic
      false - not Italic
  */
  void SetItalic(
    bool bItalic
    );
  bool Italic() const;

private:
  // Location of the center of the text dot.
  MYON_3dPoint m_center_point = MYON_3dPoint::UnsetPoint;

private:
  MYON_wString m_primary_text; // default is empty

private:
  MYON_wString m_secondary_text; // default is empty

private:
  MYON_wString m_font_face;   // Empty means MYON_TextDot::DefaultFontFace

private:
  unsigned int m_display_bits = 0;

private:
  int m_height_in_points = MYON_TextDot::DefaultHeightInPoints;
};

#endif

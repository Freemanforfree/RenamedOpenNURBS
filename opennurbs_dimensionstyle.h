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

#if !defined(OPENNURBS_DIMENSIONSTYLE_INC_)
#define OPENNURBS_DIMENSIONSTYLE_INC_


class MYON_CLASS MYON_Arrowhead
{
public:
  MYON_Arrowhead() = default;
  ~MYON_Arrowhead() = default;
  MYON_Arrowhead(const MYON_Arrowhead&) = default;
  MYON_Arrowhead& operator=(const MYON_Arrowhead&) = default;

  bool operator==(const MYON_Arrowhead& other) const;
  bool operator!=(const MYON_Arrowhead& other) const;


#pragma region RH_C_SHARED_ENUM [MYON_Arrowhead::arrow_type] [Rhino.DocObjects.DimensionStyle.ArrowType] [nested:int]
  /// <summary>
  /// Defines enumerated values for arrowhead shapes.
  /// </summary>
  enum class arrow_type : unsigned int
  {
    /// <summary> </summary>
    None = 0,
    /// <summary> </summary>
    UserBlock = 1,
    /// <summary> </summary>
    SolidTriangle = 2,   // 2:1
    /// <summary> </summary>
    Dot = 3,
    /// <summary> </summary>
    Tick = 4,
    /// <summary> </summary>
    ShortTriangle = 5,   // 1:1
    /// <summary> </summary>
    OpenArrow = 6,
    /// <summary> </summary>
    Rectangle = 7,
    /// <summary> </summary>
    LongTriangle = 8,   // 4:1
    /// <summary> </summary>
    LongerTriangle = 9,   // 6:1
  };
#pragma endregion

  static MYON_Arrowhead::arrow_type ArrowTypeFromUnsigned(
    unsigned int type_as_unsigned
  );

  arrow_type ArrowheadType() const;
  void SetArrowheadType(arrow_type type);
  MYON_UUID ArrowBlockId() const;
  void SetArrowBlockId(MYON_UUID id);

  static MYON__UINT32 GetPoints(
    arrow_type type,
    const double*& points);

  static MYON__UINT32 GetPoints(
    arrow_type type,
    MYON_2dPointArray& points);

  static bool GetArrowheadBoundingBox(
    MYON_Arrowhead::arrow_type arrow_type,
    MYON_UUID arrow_block_id,
    MYON_Xform xform,
    MYON_BoundingBox& bbox,
    bool grow);

  static
    MYON_Arrowhead::arrow_type DefaultArrowType();

private:
  arrow_type       m_arrowhead_type = MYON_Arrowhead::arrow_type::SolidTriangle;
  MYON_UUID          m_arrow_block_id = MYON_nil_uuid;

};

class MYON_CLASS MYON_TextMask
{
public:

#pragma region RH_C_SHARED_ENUM [MYON_TextMask::MaskType] [Rhino.DocObjects.DimensionStyle.MaskType] [nested:byte]
  /// <summary>
  /// Text mask drawn with background color or explicit color
  /// </summary>
  enum class MaskType : unsigned char
  {
    /// <summary>
    /// Text mask drawn with background color
    /// </summary>
    BackgroundColor = 0,
    /// <summary>
    /// Text mask drawn with explicit color
    /// </summary>
    MaskColor = 1,
  };
#pragma endregion

  static MYON_TextMask::MaskType MaskTypeFromUnsigned(
    unsigned int mask_border_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON_TextMask::MaskFrame] [Rhino.DocObjects.DimensionStyle.MaskFrame] [nested:byte]
  /// <summary>
  /// Draw a frame stroke around the text mask area
  /// </summary>
  enum class MaskFrame : unsigned char
  {
    /// <summary> Text mask frame not drawn </summary>
    NoFrame = 0,
    /// <summary> Text mask frame outline rectangle drawn </summary>
    RectFrame = 1,
    /// <summary> Text mask frame outline capsule drawn </summary>
    CapsuleFrame = 2
  };
#pragma endregion

  static MYON_TextMask::MaskFrame MaskFrameFromUnsigned(
    unsigned int mask_frame_as_unsigned
  );

public:

  /*
  The default constructor content is idenical to MYON_TextMask::None.
  */
  MYON_TextMask() = default;
  ~MYON_TextMask() = default;
  MYON_TextMask(const MYON_TextMask& src) = default;
  MYON_TextMask& operator=(const MYON_TextMask& src) = default;

public:

  /*
  MYON_TextMask::None has no effect on text appearance.
  */
  static const MYON_TextMask None;

  /*
  Description:
    MYON_TextMask::Compare() compares content in a repeatable
    and well ordered way.
  Returns:
    0: lhs and rhs have identical content.
    <0: lhs content is less than rhs content
    >0: lhs content is greater than rhs content
  */
  static int Compare(
    const MYON_TextMask& lhs,
    const MYON_TextMask& rhs
    );

  // Specifies whether or not to draw a Text Mask
  bool DrawTextMask() const;
  void SetDrawTextMask(bool bDraw);

  // Determines where to get the color to draw a Text Mask
  // Can be background color or a specific color
  MYON_TextMask::MaskType  MaskFillType() const;
  void SetMaskFillType(MYON_TextMask::MaskType  source);
  
  // Determines whether or not to draw a rectangular frame around a text mask
  MYON_TextMask::MaskFrame MaskFrameType() const;
  void SetMaskFrameType(MYON_TextMask::MaskFrame frame);

  /*
  Returns:
    Mask color.
  Remarks:
    The mask color is applied only when MaskFillType() = MYON_TextMask::MaskType::MaskColor
  */
  MYON_Color MaskColor() const;

  void SetMaskColor(
    MYON_Color color
  );

  /*
  Returns:
    Width of border area around text to be masked. The default value is 0.0.
  */
  double MaskBorder() const;

  void SetMaskBorder(double offset);

  bool Write(
    MYON_BinaryArchive& archive
  ) const;

  bool Read(
    MYON_BinaryArchive& archive
  );

  /*
  Returns:
    A SHA1 of the values defining the text mask. 
    Two text masks have the same
    content if and only if they have identical content hash values.
  */
  const MYON_SHA1_Hash& ContentHash() const;

private:
  bool                   m_bDrawMask = false;
  MYON_TextMask::MaskType m_mask_type = MYON_TextMask::MaskType::BackgroundColor;
  MYON_TextMask::MaskFrame m_mask_frame = MYON_TextMask::MaskFrame::NoFrame;

  unsigned char m_reserved2 = 0;

  MYON_Color               m_mask_color = MYON_Color::White;
  double                 m_mask_border = 0.0;

  // At some point, the reserved fields may have the name changed and be
  // used to store additional information of how to draw the mask,
  // (feathered edges, rounded corners, etc.).
  unsigned int m_reserved3 = 0;
  mutable MYON_SHA1_Hash m_content_hash = MYON_SHA1_Hash::ZeroDigest;
};

bool operator==(
  const class MYON_TextMask& lhs,
  const class MYON_TextMask& rhs
  );

bool operator!=(
  const class MYON_TextMask& lhs,
  const class MYON_TextMask& rhs
  );


class MYON_CLASS MYON_DimStyle : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_DimStyle);
private:
  friend class MYON_V5x_DimStyle;

public:
  // Predefined default dimension styles always available
  static const MYON_DimStyle Unset;                         // index = MYON_UNSET_INT_INDEX, id = nil.
  static const MYON_DimStyle Default;                       // index = -1, unique and persistent id.
  static const MYON_DimStyle DefaultInchDecimal;            // index = -2, unique and persistent id.
  static const MYON_DimStyle DefaultInchFractional;         // index = -3, unique and persistent id.
  static const MYON_DimStyle DefaultFootInchArchitecture;   // index = -4, unique and persistent id.
  static const MYON_DimStyle DefaultMillimeterSmall;        // index = -5, unique and persistent id.
  static const MYON_DimStyle DefaultMillimeterLarge;        // index = -6, unique and persistent id.
  static const MYON_DimStyle DefaultMillimeterArchitecture; // index = -7, unique and persistent id.
  static const MYON_DimStyle DefaultFeetDecimal;            // index = -8, unique and persistent id.
  static const MYON_DimStyle DefaultFeetEngrave;            // index = -9, unique and persistent id.
  static const MYON_DimStyle DefaultMillimeterEngrave;      // index = -10, unique and persistent id.
  static const MYON_DimStyle DefaultModelUnitsDecimal;      // index = -11, unique and persistent id.
  static const MYON_DimStyle DefaultModelUnitsEngrave;      // index = -12, unique and persistent id.

public:
  /*
  Parameters:
    dimstyle - [in]
  Returns:
    If dimstyle not nullptr, then dimstyle is returned.
    Otherwise a non-null pointer to a persistent dimstyle is returned.
    A null pointer is never returned.
  Remarks:
    This function is used when a dimension style is required.
  */
  static const class MYON_DimStyle& DimStyleOrDefault(
    const class MYON_DimStyle* dimstyle
  );

  /*
  Parameters:
    id - [in]
  Returns:
    If the id is not nil and identifies one of the above system dimstyles, that
    dimstyle is returned. Otherwise, MYON_DimStyle::Unset is returned.
  */
  static const MYON_DimStyle& SystemDimstyleFromId(
    MYON_UUID id
  );

  /*
  Parameters:
    index - [in]
  Returns:
    If the id is not nil and identifies one of the above system dimstyles, that
    dimstyle is returned. Otherwise, MYON_DimStyle::Unset is returned.
  */
  static const MYON_DimStyle& SystemDimstyleFromIndex(
    int index
  );

  /*
  Parameters:
    name_hash - [in]
  Returns:
    If the id is not nil and identifies one of the above system dimstyles, that
    dimstyle is returned. Otherwise, MYON_DimStyle::Unset is returned.
  */
  static const MYON_DimStyle& SystemDimstyleFromName(
    const MYON_NameHash& name_hash
  );

  /*
  Parameters:
    name_hash - [in]
  Returns:
    If the id is not nil and identifies one of the above system dimstyles, that
    dimstyle is returned. Otherwise, MYON_DimStyle::Unset is returned.
  */
  static const MYON_DimStyle& SystemDimstyleFromContentHash(
    const MYON_SHA1_Hash& content_hash
  );

private:
  /*
  Parameters:
    system_dimstyle_list - [out]
  Returns:
    Number of system dimstyles.
  Remarks:
  MYON_DimStyle::Unset is not added system_dimstyle_list[].
  */
  static unsigned int Internal_GetSystemDimstyleList(
    MYON_SimpleArray<const MYON_DimStyle*>& system_dimstyle_list
  );

public:

  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if MYON_DimStyle::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If MYON_DimStyle::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned. 
  */
  static const MYON_DimStyle* FromModelComponentRef(
    const class MYON_ModelComponentReference& model_component_reference,
    const MYON_DimStyle* none_return_value
    );

  /*
  Description:
    Create a clean dimension style that has the specified font.
    With the exception of the name, the resulting dimension style 
    will have an unset MYON_ModelComponent properties (id, index, ...).
  Parameters:
    font_characteristics - [in]
      If nullptr, then &MYON_Font::Default is used.
    model_space_text_scale - [in]
      If model_space_text_scale is  > 0.0, then it is used to set
      the DimScale() value.
    dimstyle_settings - [in]
      Setting for non-font dimstyle properties. 
      If nullptr, then &MYON_DimStyle::Default is used.
    manifest - [in]
      If manifest is not nullptr, then it is used to generate
      a unique name.
    destination - [in]
      If destination is not nullptr, the result is stored here.
      Otherwise operator new is used to construct an MYON_DimStyle on the heap.
  */
  static MYON_DimStyle* CreateFromFont(
    const MYON_Font* font_characteristics,
    double model_space_text_scale,
    const MYON_DimStyle* dimstyle_settings,
    const class MYON_ComponentManifest* manifest,
    MYON_DimStyle* destination
  );

public:
  // Default constructor result is identical to MYON_DimStyle::Unset;
  MYON_DimStyle();

  ~MYON_DimStyle() = default;
  MYON_DimStyle(const MYON_DimStyle& src) = default;
  MYON_DimStyle& operator=(const MYON_DimStyle&) = default;

public:
  // Used when reading V5 and earlier archives
  MYON_DimStyle(
    MYON::LengthUnitSystem model_length_unit_system,
    const class MYON_V5x_DimStyle& src
  );

public:
  MYON_DimStyle(const MYON_3dmAnnotationSettings& src);

  /*
  Returns:
    True:
      "this" and src have identical names, dimension style appearance attributes,
      and identical attributes inherited from the same parent dimension style.
    MYON_ModelComponent settings other than Name() and ParentId() are
    not compared.
  Remaraks:
    A better name for this function would be EqualForAllPracticalPurposes().
  */
  bool CompareDimstyle(const MYON_DimStyle& src) const;

  /*
  Returns:
    True if this and src have identical dimension style appearance attributes
    and the same parent dimension style id.
    CompareFields() ignores Name, Index, Id() values.
    CompareFields() ignores differences in IsOverride(field_id) values.
  Remaraks:
    A better name for this function would be EqualAppearanceSettings().
  */
  bool CompareFields(const MYON_DimStyle& src) const;

private:
  /*
  Returns:
    True:
      If a.IsFieldOverride(field_id) == b.IsFieldOverride(field_id)
      for all MYON_DimStyle::field enum values.
  */
  static bool Internal_EqualOverrideParentFields(
    const MYON_DimStyle& a,
    const MYON_DimStyle& b
  );
public:

  //////////////////////////////////////////////////////////////////////
  //
  // MYON_Object overrides

  // virtual
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  // virtual
  void Dump(MYON_TextLog&) const override; // for debugging

  // virtual
  bool Write(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const override;

  // virtual
  bool Read(
    MYON_BinaryArchive&  // restore definition from binary archive
    ) override;

  //////////////////////////////////////////////////////////////////////
  // Interface

  void EmergencyDestroy();

  //////////////////////////////////////////////////////////////////////
  // Interface

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::LengthDisplay] [Rhino.DocObjects.DimensionStyle.LengthDisplay] [nested:int]
  /// <summary>
  /// Dimension display length unit system and style
  /// </summary>
  enum class LengthDisplay : unsigned int
  {
    /// <summary>
    /// Decimal current model units
    /// </summary>
    ModelUnits = 0,

    /// <summary>
    /// Decimal Millimeters
    /// </summary>
    Millmeters = 3,

    /// <summary>
    /// Decimal Centimeters
    /// </summary>
    Centimeters = 4,

    /// <summary>
    /// Decimal Meters
    /// </summary>
    Meters = 5,

    /// <summary>
    /// Decimal Kilometers
    /// </summary>
    Kilometers = 6,

    /// <summary>
    /// Decimal Inches
    /// </summary>
    InchesDecimal = 7,

    /// <summary>
    /// Fractional Inches ( 1.75 inches displays as 1-3/4 )
    /// </summary>
    InchesFractional = 1,

    /// <summary>
    /// Decimal Feet
    /// </summary>
    FeetDecimal = 8,

    /// <summary>
    /// Feet and Inches ( 14.75 inches displays as 1&apos;-2-3/4&quot; )
    /// </summary>
    FeetAndInches = 2,

    /// <summary>
    /// Decimal Miles
    /// </summary>
    Miles = 9
  };

#pragma endregion

  static MYON_DimStyle::LengthDisplay LengthDisplayFromUnsigned(
    unsigned int length_display_as_unsigned
  );

  /*
  Returns:
    true if length_display selects a decimal format.
    false if length_display is MYON_DimStyle::LengthDisplay::FeetAndInches 
    or MYON_DimStyle::LengthDisplay::InchesFractional.
  */
  static bool LengthDisplayIsDecimal(
    MYON_DimStyle::LengthDisplay dimension_length_display
  );

 static MYON::LengthUnitSystem LengthUnitSystemFromLengthDisplay(
    MYON_DimStyle::LengthDisplay dimension_length_display
  );


#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::tolerance_format] [Rhino.DocObjects.DimensionStyle.ToleranceDisplayFormat] [nested:byte]
  /// <summary>
  /// Style of tolerance display for dimensions
  /// </summary>
  enum class tolerance_format : unsigned char
  {
    /// <summary>
    /// No tolerance display
    /// </summary>
    None = 0,
    /// <summary>
    /// Symmetrical +/- tolerance
    /// </summary>
    Symmetrical = 1,
    /// <summary>
    /// Distance +tol, -tol
    /// </summary>
    Deviation = 2,
    /// <summary>
    /// Distance upper and lower limits
    /// </summary>
    Limits = 3,
  };
#pragma endregion

  static MYON_DimStyle::tolerance_format ToleranceFormatFromUnsigned(
    unsigned int format_as_unsigned
  );


#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::ContentAngleStyle] [Rhino.DocObjects.DimensionStyle.LeaderContentAngleStyle] [nested:byte]
  /// <summary>
  /// Angle for text or other leader or dimension content
  /// </summary>
  enum class ContentAngleStyle : unsigned char
  {
    /// <summary>
    /// Annotation text is horizontal in annotation object's plane
    /// </summary>
    Horizontal = 0,   
    /// <summary>
    /// Aligned with last leader direction or dimension line
    /// </summary>
    Aligned = 1,   
    /// <summary>
    /// Explicit angle
    /// </summary>
    Rotated = 2,
  };
#pragma endregion

  static MYON_DimStyle::ContentAngleStyle ContentAngleStyleFromUnsigned(
    unsigned int alignment_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::leader_curve_type] [Rhino.DocObjects.DimensionStyle.LeaderCurveStyle] [nested:byte]
  /// <summary>
  /// Type of leader curve
  /// </summary>
  enum class leader_curve_type : unsigned char
  {
    /// <summary>
    ///
    /// </summary>
    None = 0,
    /// <summary>
    ///
    /// </summary>
    Polyline = 1,
    /// <summary>
    ///
    /// </summary>
    Spline = 2
  };
#pragma endregion
  
  static MYON_DimStyle::leader_curve_type LeaderCurveTypeFromUnsigned(
    unsigned int type_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::field] [Rhino.DocObjects.DimensionStyle.Field] [nested:int]
  // Don't change these enum values. They are used in file reading and writing.
  /// <summary>
  /// Field identifiers used for file i/o and getting/setting values
  /// </summary>
  enum class field : unsigned int
  {
    /// <summary></summary>
    Unset                          = 0,

    /// <summary>Dimension style Name property. Cannot be inherited from parent.</summary>
    Name                           = 1,

    /// <summary>Dimension style runtime model component index property. Cannot be inherited from parent.</summary>
    Index                          = 2,

    /// <summary></summary>
    ExtensionLineExtension         = 3,
    /// <summary></summary>
    ExtensionLineOffset            = 4,
    /// <summary></summary>
    Arrowsize                      = 5,
    /// <summary></summary>
    LeaderArrowsize                = 6,
    /// <summary></summary>
    Centermark                     = 7,
    /// <summary></summary>
    TextGap                        = 8,
    /// <summary></summary>
    TextHeight                     = 9,
    /// <summary>Linear, angular, and ordinate dimension text location above/in/below</summary>
    DimTextLocation                = 10,
 
     //OBSOLETE_LengthFormat_                   = 11,
    /// <summary>Text mask frame</summary>
    MaskFrameType                  = 11,

    /// <summary></summary>
    LengthResolution               = 12,
    /// <summary></summary>
    AngleFormat                    = 13,
    /// <summary></summary>
    AngleResolution                = 14,
    /// <summary></summary>
    Font                           = 15,
    
    /// <summary>
    /// LengthFactor is a rarely used. It applies when a model is being
    /// drawn to a scale and the dimension length values should be
    /// reverse scaled. For example, if a model is drawn at 1/4 scale,
    /// a line 5 units long indicates the real world line is 20 units
    /// long. In this case setting LengthFactor to 4 would cause
    /// a linear dimension applied to that line to display a value of 20.
    ///</summary>
    LengthFactor                   = 16,

    /// <summary></summary>
    Alternate                      = 17,

    /// <summary>
    /// AlternateLengthFactor is a rarely used. See Length factor for
    /// a description of this property.
    ///</summary>
    AlternateLengthFactor          = 18,

    //OBSOLETE_AlternateLengthFormat_          = 19,

    /// <summary></summary>
    AlternateLengthResolution      = 20,
    /// <summary></summary>
    Prefix                         = 21,
    /// <summary></summary>
    Suffix                         = 22,
    /// <summary></summary>
    AlternatePrefix                = 23,
    /// <summary></summary>
    AlternateSuffix                = 24,
    /// <summary></summary>
    DimensionLineExtension         = 25,
    /// <summary></summary>
    SuppressExtension1             = 26,
    /// <summary></summary>
    SuppressExtension2             = 27,
    /// <summary></summary>
    ExtLineColorSource             = 28,
    /// <summary></summary>
    DimLineColorSource             = 29,
    /// <summary></summary>
    ArrowColorSource               = 30,
    /// <summary></summary>
    TextColorSource                = 31,
    /// <summary></summary>
    ExtLineColor                   = 32,
    /// <summary></summary>
    DimLineColor                   = 33,
    /// <summary></summary>
    ArrowColor                     = 34,
    /// <summary></summary>
    TextColor                      = 35,
    /// <summary></summary>
    ExtLinePlotColorSource         = 36,
    /// <summary></summary>
    DimLinePlotColorSource         = 37,
    /// <summary></summary>
    ArrowPlotColorSource           = 38,
    /// <summary></summary>
    TextPlotColorSource            = 39,
    /// <summary></summary>
    ExtLinePlotColor               = 40,
    /// <summary></summary>
    DimLinePlotColor               = 41,
    /// <summary></summary>
    ArrowPlotColor                 = 42,
    /// <summary></summary>
    TextPlotColor                  = 43,
    /// <summary></summary>
    ExtLinePlotWeightSource        = 44,
    /// <summary></summary>
    DimLinePlotWeightSource        = 45,
    /// <summary></summary>
    ExtLinePlotWeight_mm           = 46,
    /// <summary></summary>
    DimLinePlotWeight_mm           = 47,
    /// <summary></summary>
    ToleranceFormat                = 48,
    /// <summary></summary>
    ToleranceResolution            = 49,
    /// <summary></summary>
    ToleranceUpperValue            = 50,
    /// <summary></summary>
    ToleranceLowerValue            = 51,
    /// <summary></summary>
    AltToleranceResolution         = 52,
    /// <summary></summary>
    ToleranceHeightScale           = 53,
    /// <summary></summary>
    BaselineSpacing                = 54,
    /// <summary></summary>
    DrawMask                       = 55,
    /// <summary></summary>
    MaskColorSource                = 56,
    /// <summary></summary>
    MaskColor                      = 57,
    /// <summary></summary>
    MaskBorder                     = 58,
    /// <summary></summary>
    DimensionScale                 = 59,
    /// <summary></summary>
    DimscaleSource                 = 60,
    /// <summary></summary>
    FixedExtensionLength           = 61,
    /// <summary></summary>
    FixedExtensionOn               = 62,
    /// <summary></summary>
    TextRotation                   = 63,
    /// <summary></summary>
    SuppressArrow1                 = 64,
    /// <summary></summary>
    SuppressArrow2                 = 65,
    /// <summary></summary>
    TextmoveLeader                 = 66,
    /// <summary></summary>
    ArclengthSymbol                = 67,
    /// <summary></summary>
    StackTextheightScale           = 68,
    /// <summary></summary>
    StackFormat                    = 69,
    /// <summary></summary>
    AltRound                       = 70,
    /// <summary></summary>
    Round                          = 71,
    /// <summary></summary>
    AngularRound                   = 72,
    /// <summary></summary>
    AltZeroSuppress                = 73,

    //OBSOLETE ToleranceZeroSuppress          = 74,

    /// <summary></summary>
    AngleZeroSuppress              = 75,
    /// <summary></summary>
    ZeroSuppress                   = 76,
    /// <summary></summary>
    AltBelow                       = 77,
    /// <summary></summary>
    ArrowType1                     = 78,
    /// <summary></summary>
    ArrowType2                     = 79,
    /// <summary></summary>
    LeaderArrowType                = 80,
    /// <summary></summary>
    ArrowBlockId1                  = 81,
    /// <summary></summary>
    ArrowBlockId2                  = 82,
    /// <summary></summary>
    LeaderArrowBlock               = 83,
    /// <summary>Radial dimension text location above/in/below</summary>
    DimRadialTextLocation          = 84,
    /// <summary></summary>
    TextVerticalAlignment          = 85,
    /// <summary></summary>
    LeaderTextVerticalAlignment    = 86,
    /// <summary></summary>
    LeaderContentAngleStyle        = 87,
    /// <summary></summary>
    LeaderCurveType                = 88,
    /// <summary></summary>
    LeaderContentAngle             = 89,
    /// <summary></summary>
    LeaderHasLanding               = 90,
    /// <summary></summary>
    LeaderLandingLength            = 91,
    /// <summary></summary>
    MaskFlags                      = 92,
    /// <summary></summary>
    CentermarkStyle                = 93,
    /// <summary></summary>
    TextHorizontalAlignment        = 94,
    /// <summary></summary>
    LeaderTextHorizontalAlignment  = 95,
    /// <summary></summary>
    DrawForward                    = 96,
    /// <summary></summary>
    SignedOrdinate                 = 97,
    
    /// <summary>
    /// Unit system for dimension rendering sizes like  TextHeight, TextGap, ArrowSize, ExtOffset,
    /// and dozens of other properties that control the appearance and placement of the components
    /// used to render a dimension.
    ///</summary>
    UnitSystem                     = 98,

    /// <summary></summary>
    TextMask                       = 99,
    /// <summary></summary>
    TextOrientation                = 100,
    /// <summary></summary>        
    LeaderTextOrientation          = 101,
    /// <summary></summary>
    DimTextOrientation             = 102,
    /// <summary></summary>
    DimRadialTextOrientation       = 103,
    /// <summary></summary>
    DimTextAngleStyle              = 104,
    /// <summary></summary>
    DimRadialTextAngleStyle        = 105,
    /// <summary></summary>
    TextUnderlined                 = 106,

    //OBSOLETE_DimensionUnitSystem_            = 107,
    //OBSOLETE_AlternateDimensionUnitSystem_   = 108,

    /// <summary>
    /// Dimension length display. See MYON_DimStyle::DimensionLengthDisplay() for a descpription of this parameter.
    /// </summary>
    DimensionLengthDisplay = 109,

    /// <summary>
    /// Alternate dimension length display. See MYON_DimStyle::AlternateDimensionLengthDisplay() for a descpription of this parameter.
    /// </summary>
    AlternateDimensionLengthDisplay = 110,

    /// <summary>
    /// Force dimension line to draw when text is moved outside
    /// </summary>
    ForceDimLine = 111,

    /// <summary>
    /// Arrow position when arrows won't fit between extensions
    /// </summary>
    ArrowFit = 112,

    /// <summary>
    /// Text position when text won't fit between extensions
    /// </summary>
    TextFit = 113,

    /// <summary>
    /// Character to use for decimal separator in dimension text
    /// </summary>
    DecimalSeparator = 114,

    /// <summary>Every enum UINT value that identifies a valid dimension style property is less than the UINT value of Count.</summary>
    Count = 115
  };
  
#pragma endregion

  enum : unsigned int
  {
    // must be 1 + the maximum value of an MYON_DimStyle::field enum value.
    FieldCount = (unsigned int)field::Count
  };

  static MYON_DimStyle::field FieldFromUnsigned(
    unsigned int field_as_unsigned
  );
  

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::angle_format] [Rhino.DocObjects.DimensionStyle.AngleDisplayFormat] [nested:byte]
  /// <summary>
  /// Display format for angles
  /// </summary>
  enum class angle_format : unsigned char
  {
    /// <summary> Decimal Degrees </summary>
    DecimalDegrees   = 0,
    /// <summary> Degrees Minutes Seconds </summary>
    DegMinSec        = 1,
    /// <summary> Decimal Radians </summary>
    Radians          = 2,
    /// <summary> Decimal Gradians </summary>
    Grads            = 3
  };
#pragma endregion

  static MYON_DimStyle::angle_format AngleFormatFromUnsigned(
    unsigned int format_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::OBSOLETE_length_format] [Rhino.DocObjects.DimensionStyle.LengthDisplayFormat] [nested:byte]
  /// <summary>
  /// Obsolete format for length display - use MYON_DimStyle::DimensionLengthDisplay instead
  /// </summary>
  enum class OBSOLETE_length_format : unsigned char
  {
    /// <summary>Obsolete - use MYON_DimStyle::DimensionLengthDisplay::ModelUnits.</summary>
    Decimal       = 0,

    /// <summary>Obsolete - use MYON_DimStyle::DimensionLengthDisplay::InchesFractional</summary>
    Fractional    = 1,

    /// <summary>Obsolete - use MYON_DimStyle::DimensionLengthDisplay::FeetAndInches</summary>
    FeetInches = 2,

    /// <summary>Obsolete - use MYON_DimStyle::DimensionLengthDisplay::FeetAndInches enum.</summary>
    FeetDecimalInches = 3
  };
#pragma endregion


  static MYON_DimStyle::OBSOLETE_length_format OBSOLETE_LengthFormatFromUnsigned(
    unsigned int format_as_unsigned
  );

  /*
  Parameters:
    dimension_length_display - [in]
    model_serial_number - [in]
      0: Ignore model settings
      >0: dimstyle.ModelSerialNumber()
  */
  static MYON_DimStyle::OBSOLETE_length_format OBSOLETE_LengthFormatFromLengthDisplay(
    MYON_DimStyle::LengthDisplay dimension_length_display,
    unsigned int model_serial_number
  );

  static MYON_DimStyle::OBSOLETE_length_format OBSOLETE_LengthFormatFromLengthDisplay(
    MYON_DimStyle::LengthDisplay dimension_length_display,
    MYON::LengthUnitSystem model_unit_system
  );

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::TextLocation] [Rhino.DocObjects.DimensionStyle.TextLocation] [nested:byte]
  /// <summary>
  /// The location of text in linear, angular, radial, and ordinate dimensions.
  /// </summary>
  enum class TextLocation : unsigned char
  {
    /// <summary>Text is above dimension line.</summary>
    AboveDimLine = 0,
    /// <summary>Text is centered in dimension line.</summary>
    InDimLine = 1,
    /// <summary>Text is below dimension line.</summary>
    BelowDimLine = 2
  };
#pragma endregion

  static MYON_DimStyle::TextLocation TextLocationFromUnsigned(
    unsigned int dim_text_location_as_unsigned
    );  
  
  // convert MYON_DimStyle::OBSOLETE_length_format enum to MYON::OBSOLETE_DistanceDisplayMode enum
  static MYON::OBSOLETE_DistanceDisplayMode DistanceDisplayModeFromLengthFormat(
    MYON_DimStyle::OBSOLETE_length_format
    );

  // convert MYON::OBSOLETE_DistanceDisplayMode enum to MYON_DimStyle::OBSOLETE_length_format enum
  static MYON_DimStyle::OBSOLETE_length_format LengthFormatFromDistanceDisplayMode(
    MYON::OBSOLETE_DistanceDisplayMode
    );


#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::suppress_zero] [Rhino.DocObjects.DimensionStyle.ZeroSuppression] [nested:byte]
  /// <summary>
  /// Marks leading and trailing zeros for removal.
  /// </summary>
  enum class suppress_zero : unsigned char
  {
    /// <summary>No zero suppression.</summary>
    None                          = 0,
    /// <summary>Suppress leading zeros.</summary>
    SuppressLeading               = 1,
    /// <summary>Suppress trailing zeros.</summary>
    SuppressTrailing              = 2,
    /// <summary>Suppress leading and trailing zeros.</summary>
    SuppressLeadingAndTrailing    = 3,
    /// <summary>Suppress zero feet.</summary>
    SuppressZeroFeet              = 4,
    /// <summary>Suppress zero inches.</summary>
    SuppressZeroInches            = 8,
    /// <summary>Suppress zero feet and zero inches.</summary>
    SuppressZeroFeetAndZeroInches = 12
  };
#pragma endregion

  static MYON_DimStyle::suppress_zero ZeroSuppressFromUnsigned(
    unsigned int suppress_ero_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::stack_format] [Rhino.DocObjects.DimensionStyle.StackDisplayFormat] [nested:byte]
  /// <summary>
  /// Format of stacked fractions
  /// </summary>
  enum class stack_format : unsigned char
  {
    /// <summary> No stacking </summary>
    None               = 0,
    /// <summary> Stack with horizontal line </summary>
    StackHorizontal    = 1,
    /// <summary> Stack with angled line </summary>
    StackDiagonal      = 2,
  };
#pragma endregion

  static MYON_DimStyle::stack_format StackFormatFromUnsigned(
    unsigned int format_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::centermark_style] [Rhino.DocObjects.DimensionStyle.CenterMarkStyle] [nested:byte]
  /// <summary>
  /// Style for drawing centermark for Radial dimensions and Centermark objects
  /// </summary>
  enum class centermark_style : unsigned char
  {
    /// <summary>
    /// No centermark display
    /// </summary>
    None = 0,
    /// <summary>
    /// + mark only
    /// </summary>
    Mark = 1,
    /// <summary>
    /// + mark and lines to radius
    /// </summary>
    MarkAndLines = 2,
  };
#pragma endregion

  static MYON_DimStyle::centermark_style CentermarkStyleFromUnsigned(
    unsigned int centermark_as_unsigned
  );
   

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::arrow_fit] [Rhino.DocObjects.DimensionStyle.ArrowFit] [nested:byte]
  /// <summary>
  /// Arrow display position inside or outside extension lines
  /// </summary>
  enum class arrow_fit : unsigned char
  {
    /// <summary> Auto - Display when space permits </summary>
    Auto = 0,
    /// <summary> Force arrows inside extensions </summary>
    ArrowsInside = 1,
    /// <summary> Force arrows outside extensions </summary>
    ArrowsOutside = 2,
  };
#pragma endregion

  static MYON_DimStyle::arrow_fit ArrowFitFromUnsigned(
    unsigned int arrow_fit_as_unsigned
  );

#pragma region RH_C_SHARED_ENUM [MYON_DimStyle::text_fit] [Rhino.DocObjects.DimensionStyle.TextFit] [nested:byte]
  /// <summary>
  /// Text display position inside or outside extension lines
  /// </summary>
  enum class text_fit : unsigned char
  {
    /// <summary> Auto - Display inside when space permits </summary>
    Auto = 0,
    /// <summary> Force text inside extensions </summary>
    TextInside = 1,
    /// <summary> Force text outside to the right of extensions </summary>
    TextRight = 2,
    /// <summary> Force text outside to the left of extensions </summary>
    TextLeft = 3,
    /// <summary> Move text outside to the right of extensions when it won't fit inside </summary>
    TextHintRight = 4,
    /// <summary> Move text outside to the left of extensions when it won't fit inside  </summary>
    TextHintLeft = 5,
  };
#pragma endregion

  static MYON_DimStyle::text_fit TextFitFromUnsigned(
    unsigned int text_fit_as_unsigned
  );
     
  static MYON_DimStyle::LengthDisplay LengthDisplayFromUnitsAndFormat(
    MYON::LengthUnitSystem units, 
    MYON_DimStyle::OBSOLETE_length_format lengthformat
  );

  /// <summary>
  /// Dimension length units and format
  ///</summary>
  MYON_DimStyle::LengthDisplay DimensionLengthDisplay() const;

  /// <summary>
  /// Set dimension length units and format
  ///</summary>
  MYON_DimStyle::LengthDisplay AlternateDimensionLengthDisplay() const;

  /// <summary>
  /// Alternate dimension length units and format
  ///</summary>
  void SetDimensionLengthDisplay(MYON_DimStyle::LengthDisplay length_display);

  /// <summary>
  /// Set alternate dimension length units and format
  ///</summary>
  void SetAlternateDimensionLengthDisplay(MYON_DimStyle::LengthDisplay length_display);

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
    unsigned int model_sn
    ) const;


private:
  /*
  Returns:
    true if value was changed.
  */
  bool Internal_SetBoolMember(
    MYON_DimStyle::field field_id,
    bool value,
    bool& class_member
  );
  /*
  Returns:
    true if value was changed.
  */
  bool Internal_SetUnsignedCharMember(
    MYON_DimStyle::field field_id,
    unsigned char value,
    unsigned char& class_member
  );
  /*
  Returns:
    true if value was changed.
  */
  bool Internal_SetIntMember(
    MYON_DimStyle::field field_id,
    int value,
    int& class_member
  );
  /*
  Returns:
    true if value was changed.
  */
  bool Internal_SetColorMember(
    MYON_DimStyle::field field_id,
    MYON_Color value,
    MYON_Color& class_member
  );
  /*
  Returns:
    true if value was changed.
  */
  bool Internal_SetDoubleMember(
    MYON_DimStyle::field field_id,
    double value,
    double& class_member
  );
  /*
  Returns:
    true if value was changed.
  */
  bool Internal_SetIdMember(
    MYON_DimStyle::field field_id,
    MYON_UUID value,
    MYON_UUID& class_member
  );
  /*
  Returns:
    true if value was changed.
  */
  bool Internal_SetStringMember(
    MYON_DimStyle::field field_id,
    const wchar_t* value,
    MYON_wString& class_member
  );

  void Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field field_id);

public:  
  // Extension line extension
  double ExtExtension() const;
  void SetExtExtension(const double);

  // Extension line offset
  double ExtOffset() const;
  void SetExtOffset(const double);

  // Arrow size
  double ArrowSize() const;
  void SetArrowSize(const double);

  // Arrow size
  double LeaderArrowSize() const;
  void SetLeaderArrowSize(const double);

  // Centermark size
  double CenterMark() const;
  void SetCenterMark(const double);

  // Centermark style
  MYON_DimStyle::centermark_style CenterMarkStyle() const;
  void SetCenterMarkStyle(MYON_DimStyle::centermark_style style);

  // The location of text relative to the dimension line in linear, angular, and ordinate dimensions.
  MYON_DimStyle::TextLocation DimTextLocation() const;
  void SetDimTextLocation(MYON_DimStyle::TextLocation);

  // The location of text relative to the dimension line in radial dimensions.
  MYON_DimStyle::TextLocation DimRadialTextLocation() const;
  void SetDimRadialTextLocation(MYON_DimStyle::TextLocation);

  angle_format AngleFormat() const;
  void SetAngleFormat(angle_format format);

  // Display resolution for distance measurements
  int LengthResolution() const;
  void SetLengthResolution(int);

  // Display resolution for angle measurements
  int AngleResolution() const;
  void SetAngleResolution(int);

public:
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
  void SetFont(
    const class MYON_Font& font_characteristics
  );

  /*
  Returns:
    The managed font used to render text.
  */
  const class MYON_Font& Font() const;

  /*
  Returns:
    If a parent dimstyle is in play, this is the managed font used by the parent dimstyle.
    Otherwise, this is the font returned by Font().
  */
  const class MYON_Font& ParentDimStyleFont() const;

  /*
  Returns:
    A copy of the font_characteristics information.
  Remarks:
    You probably want to use Font(). This function is only useful
    in isolated situations and is typically used to study font
    substitutions when a model moves between computers or platforms.
  */
  const class MYON_Font& FontCharacteristics() const;

  /*
  Returns:
   True if the font returned by Font() is a substitute 
   for the font passed to SetFont(). 
  Remarks:
    Font substitution can occur when a model is moved between
    computers that have different fonts installed.
  */
  const bool FontSubstituted() const;

public:
  /*
  Description:
    Two dimension styles have identical text orientation, glyph content, 
    and size parameters if and only if the have identical values of 
    TextPositionPropertiesHash().
  Returns:
    A SHA-1 hash of the information that controls text position and size.
  Remarks:
    Independent of id, parent id, name, and index.
  */
  const class MYON_SHA1_Hash TextPositionPropertiesHash() const;

  /*
  Description:
    Two dimension styles have identical content if and only 
    if they have identical values of ContentHash().
  Returns:
    A SHA-1 hash of the information that controls annotation appearance.
  Remarks:
    Independent of id, parent id, name, and index.
  */
  const class MYON_SHA1_Hash& ContentHash() const;

private:
  void Internal_TextPositionPropertiesChange();

public:

  // Distance from dimension lines to text
  double TextGap() const;
  void SetTextGap(double gap);

  // Height of dimension text
  double TextHeight() const;
  void SetTextHeight(double height);

  /*
  Returns:
    Width of an em space (U+2003), also called a mutton, in the same units as TextHeight()
    using the current settings for TextHeight() and Font().
  */
  double TextWidthOfSpace() const;

  /*
  Returns:
    Width of an em space (U+2003), also called a mutton, in the same units as TextHeight()
    using the current settings for TextHeight() and Font().
  */
  double TextWidthOfEmSpace() const;

  /*
  Returns:
    Width of an en space (U+2002), also called a nut, in the same units as TextHeight()
    using the current settings for TextHeight() and Font().
  */
  double TextWidthOfEnSpace() const;

  /*
  Returns:
    Width of a figure space (U+2007) in the same units as TextHeight()
    using the current settings for TextHeight() and Font().
  Remarks:
    Typically close to the average with of a decimal digit (0123456789) and used
    to line up columns of numeric values.
  */
  double TextWidthOfFigureSpace() const;

  /*
  Returns:
    Width of an ideographic space (U+3000) in the same units as TextHeight()
    using the current settings for TextHeight() and Font().
  Remarks:
    The width of ideographic (CJK) characters.
  */
  double TextWidthOfIdeographicSpace() const;

  /*
  Returns:
    Width of a medium mathematical space (U+205F) in the same units as TextHeight()
    using the current settings for TextHeight() and Font().
  */
  double TextWidthOfMediumMathematicalSpace() const;

  /*
  Parameters:
    unicode_code_point - [in]
  Returns:
    The advande for a single code point glyph in the same units as TextHeight() using the current settings
    for TextHeight() and Font().
  Remarks:
    When sequences of code points are rendered, using the per glyph advance does not
    create the best looking text. Text rendering tools like DirectWrite that look at the 
    entire string adjust advance based on the locale, neighboring glyphs, and other contextual
    information.
  */
  double TextAdvanceOfCodePoint(
    unsigned unicode_code_point
  ) const;


  /// <summary>
  /// LengthFactor is a rarely used. It applies when a model is being
  /// drawn to a scale and the dimension length values should be
  /// reverse scaled. For example, if a model is drawn at 1/4 scale,
  /// a line 5 units long indicates the real world line is 20 units
  /// long. In this case setting LengthFactor to 4 would cause
  /// a linear dimension applied to that line to display a value of 20.
  ///</summary>
  double LengthFactor() const;
    
  /// <summary>
  /// LengthFactor is a rarely used. It applies when a model is being
  /// drawn to a scale and the dimension length values should be
  /// reverse scaled. For example, if a model is drawn at 1/4 scale,
  /// a line 5 units long indicates the real world line is 20 units
  /// long. In this case setting LengthFactor to 4 would cause
  /// a linear dimension applied to that line to display a value of 20.
  ///</summary>
  void SetLengthFactor(double);

  // Additional measurement display toggle
  bool Alternate() const;
  void SetAlternate(bool);

  // Distance scale factor for alternate display
  /// <summary>
  /// AlternateLengthFactor is a rarely used. See Length factor for
  /// a description of this property.
  ///</summary>
  double AlternateLengthFactor() const;

  /// <summary>
  /// AlternateLengthFactor is a rarely used. See Length factor for
  /// a description of this property.
  ///</summary>
  void SetAlternateLengthFactor(double);

  // Display resolution for alternate length measurements
  int AlternateLengthResolution() const;
  void SetAlternateLengthResolution(int);

  // Dimension prefix text
  const MYON_wString& Prefix() const;
  void SetPrefix(const wchar_t*);

  // Dimension suffix text
  const MYON_wString& Suffix() const;
  void SetSuffix(const wchar_t*);

  // Dimension alternate prefix text
  const MYON_wString& AlternatePrefix() const;
  void SetAlternatePrefix(const wchar_t*);

  // Dimension alternate suffix text
  const MYON_wString& AlternateSuffix() const;
  void SetAlternateSuffix(const wchar_t*);

  // Suppress first dimension extension line
  bool SuppressExtension1() const;
  void SetSuppressExtension1(bool);

  // Suppress second dimension extension line
  bool SuppressExtension2() const;
  void SetSuppressExtension2(bool);

  // Extension of dimension line past extension lines
  double DimExtension() const;
  void SetDimExtension(const double e);


  //// Colors of Text
  //MYON_Color TextColor() const;
  //void SetTextColor(MYON_Color color);
  //
  // Combines a field id and a field value
  // Dimensions will have an array of DimstyleField's to record 
  // dimension style overrides for individual dimensions
  class DimstyleField
  {
  public:
    DimstyleField()
      : m_next(nullptr)
      , m_field_id(MYON_DimStyle::field::Unset)
    {
      m_val.s_val = nullptr;
    }
    ~DimstyleField()
    {
      if (nullptr != m_next)
      {
        delete m_next;
        m_next = nullptr;
      }
      if (nullptr != m_val.s_val)
      {
        delete m_val.s_val;
        m_val.s_val = nullptr;
      }
    }

    DimstyleField* m_next;
    MYON_DimStyle::field m_field_id;
    union
    {
      bool b_val;
      int  i_val;
      unsigned char uc_val;
      double d_val;
      unsigned int c_val;
      const MYON_wString* s_val;
    }  m_val;
  };

  /*
  Parameters:
    field_id - [in]
  Returns:
    false: (default)
      The setting identified by field_id is inherited from the parent dimension style identified by ParentId().
    true:
      The setting identified by field_id is independent of any parent dimension style.
  */
  bool IsFieldOverride(MYON_DimStyle::field field_id) const;

  /*
  Parameters:
    field_id - [in]
    bOverrideParent - [in]
      false:
        The setting identified by field_id is inherited from the parent dimension style identified by ParentId().
      true:
        The setting identified by field_id is independent of any parent dimension style.
  */
  void SetFieldOverride(MYON_DimStyle::field field_id, bool bOverrideParent);

  /*
  Parameters:
    bOverrideParent - [in]
      true - if a field permits overriding, set it to true.
      false - set all field override values to false.
  */
  void SetFieldOverrideAll(bool bOverrideParent);

  /*
  Description:
    All dimension style settings identified the MYON_DimStyle::field enum, 
    except Name and Id, are inherited from the parent dimension style.
  */
  void ClearAllFieldOverrides();

  /*
  Returns:
    false: (default)
      Every setting identified by a MYON_DimStyle::field enum value, except name and id, 
      is inherited from the parent dimension style identified by ParentId().
    true:
      At least one setting identified by a MYON_DimStyle::field enum value is
      is independent of any parent dimension style.
  */
  bool HasOverrides() const;

  /*
  Returns:
    The number of DimStyle fields that are overridden.  Name, Id and Index are not counted.
  */
  MYON__UINT32 OverrideCount() const;

  /*
  Returns:
    The content hash of the parent dimstyle. If there is no parent dimstyle, then
    MYON_SHA1_Hash::EmptyContent is returned.
  */
  const MYON_SHA1_Hash& ParentContentHash() const;

  /*
  Description:
    Create a dimstyle from this that is configured to be customized for use
    in creating a new annotation object.
  Example:
        MYON_DimStyleContext = dim_style_context = ...;
        MYON_DimStyle my_dim_style = dim_style_context.CurrentDimStyle().CreateOverrideCandidate().
        // Customize my_dim_style
        my_dim_style.Set...(...);
  Returns:
    An MYON_DimStyle configured to be modified and used as an override dimstyle for annotation objects.
  */
  const MYON_DimStyle CreateOverrideCandidate() const;

  /*
  Description:
    Get an MYON_DimStyle with the specified properties.
  Parameters:
    parent_dim_style - [in]
      If you are getting ready to modify and existing annotation object,
      a good options for this parameter is the dimstyle returned by MYON_Annotation.DimStyle();
      If you are getting ready to create a new annotation object, then get
      an MYON_DimStyleContext class and pass MYON_DimStyleContext.CurrentDimStyle().
      In Rhino, use CRhinoDoc.DimStyleContext() to get an MYON_DimStyleContext.
      In an MYONX_Model, use MYONX_Model.DimStyleContext() to get an MYON_DimStyleContext.
      In other situations, you can pass on of the system dimstyles like
      MYON_DimStyle::DefaultMillimeterSmall or MYON_DimStyle::DefaultMillimeterArchitectural.
      The worst possible choices are MYON_DimStyle::Default or MYON_DimStyle::Unset.
    annotation_type - [in]
      MYON::AnnotationType::Unset if style will be used for multiple types of annotation
      or a specific type. For example, if you are making a text object, pass MYON::AnnotationType::Text; 
      if you are making a leader, pass MYON::AnnotationType::Leader, and so on.
    font - [in]
      nullptr for current default or specify the font you want.
      When in doubt, pass nullptr
    model_space_text_scale - [in]
      If > 0, then MYON_DimStyle.DimScale() is set, otherwise current default is used.
      When in doubt, pass MYON_UNSET_VALUE.
    text_height - [in]
    text_height_unit_system - [in]
      If text_height > 0, then MYON_DimStyle.TextHeight() is set, otherwise current default is used.
      When in doubt, pass MYON_UNSET_VALUE.
    valign - [in]
    halign - [in]
      valign and halign control placement  of text in text objects and leaders. 
      The value of the annotation_type parameter determines which objects use the
      valign and halign settings.
    text_orientation - [in]
    dim_text_location - [in]
      Controls placement of text in linear, angular, radial and ordinate dimensions.
      When in doubt, pass parent_dim_style.DimTextLocation().
  Returns:
    A dimstyle with the specified text properties. 
  Remarks:
    This is a useful tool for creating the dimension style parameters to
    CRhinoDoc.AddTextObject() and CRhinoDoc.AddLeaderObject().
  */
  static const MYON_DimStyle CreateFromProperties(
    const MYON_DimStyle& parent_dim_style,
    MYON::AnnotationType annotation_type,
    const MYON_Font* font,
    double model_space_text_scale,
    double text_height,
    MYON::LengthUnitSystem text_height_unit_system,
    MYON::TextVerticalAlignment valign,
    MYON::TextHorizontalAlignment halign
  );

  static const MYON_DimStyle CreateFromProperties(
    const MYON_DimStyle& parent_dim_style,
    MYON::AnnotationType annotation_type,
    const MYON_Font* font,
    double model_space_text_scale,
    double text_height,
    MYON::LengthUnitSystem text_height_unit_system,
    MYON::TextVerticalAlignment valign,
    MYON::TextHorizontalAlignment halign,
    MYON::TextOrientation orientation,
    MYON_DimStyle::TextLocation dim_text_location
  );

  static const MYON_DimStyle CreateFromProperties(
    const MYON_DimStyle& parent_dim_style,
    MYON::AnnotationType annotation_type,
    const MYON_Font* font,
    double model_space_text_scale,
    double text_height,
    MYON::LengthUnitSystem text_height_unit_system
  );

private:
  static void Internal_CreateFromProperties(
    const MYON_DimStyle& parent_dim_style,
    MYON::AnnotationType annotation_type,
    const MYON_Font* font,
    double model_space_text_scale,
    double text_height,
    MYON::LengthUnitSystem text_height_unit_system,
    bool bSetAlignment,
    MYON::TextVerticalAlignment valign,
    MYON::TextHorizontalAlignment halign,
    bool bSetOrientation,
    MYON::TextOrientation orientation,
    bool bSetLocation,
    MYON_DimStyle::TextLocation dim_text_location,
    MYON_DimStyle& destination
  );
public:

  /*

  */
  bool IsOverrideDimStyleCandidate(
    MYON_UUID parent_id,
    bool bRequireSetOverrides,
    MYON_wString* error_description = nullptr
  ) const;


  /*
  Description:
    For every dimension style property identified by a field_id MYON_DimStyle::field enum,
    except Name and Index, do the following:

    if ( source.IsFieldOverride(field_id) )
      copy corresponding value from source to this
    else
      copy corresponding value from parent to this

    Set this->ParentId() = parent.Id().
  Parameters:
    src - [in]
      It is permitted for src to be this.
    parent - [in]
      It is permitted for parent to be this.
  */
  void OverrideFields(
    const MYON_DimStyle& source, 
    const MYON_DimStyle& parent
    );

  /*
  Description:
    For every dimension style property identified by a field_id MYON_DimStyle::field enum,
    except Name and Index, if source and parent have different values, then
    set the field override for that property to true.
  Parameters:
    src - [in]
      It is permitted for src to be this.
    parent - [in]
      It is permitted for parent to be this.
  */
  void OverrideFieldsWithDifferentValues(
    const MYON_DimStyle& source, 
    const MYON_DimStyle& parent
    );

  /*
  Description:
    Set the parent dimension style id to parent.Id() and copies
    all inherited appearance properties from parent.
  Parameters:
    parent - [in]
      If this->IsFieldOverride(field_id) is false, then the dimension style
      property value corresponding to field_id is copied from parent to "this".      
  Remarks:
    Identical to calling this->OverrideFields(*this,parent).
  */
  void InheritFields(const MYON_DimStyle& parent);

  // Test if this dimstyle is the child of any other dimstyle
  bool IsChildDimstyle() const;

  /*
  Returns:
    True if parent_id is not nil and parent_id == this->ParentId().
  */
  bool IsChildOf(const MYON_UUID& parent_id) const;

  tolerance_format ToleranceFormat() const;
  int  ToleranceResolution() const;
  double ToleranceUpperValue() const;
  double ToleranceLowerValue() const;
  double ToleranceHeightScale() const;

  void SetToleranceFormat(MYON_DimStyle::tolerance_format format);
  void SetToleranceResolution(int resolution);
  void SetToleranceUpperValue(double upper_value);
  void SetToleranceLowerValue(double lower_value);
  void SetToleranceHeightScale(double scale);

  double BaselineSpacing() const;
  void SetBaselineSpacing(double spacing);

  // Determines whether or not to draw a Text Mask
  bool DrawTextMask() const;
  void SetDrawTextMask(bool bDraw);

  // Determines where to get the color to draw a Text Mask
  MYON_TextMask::MaskType  MaskFillType() const;
  void SetMaskFillType(MYON_TextMask::MaskType  source);

  // Determines whether to draw a frame around a Text Mask
  MYON_TextMask::MaskFrame  MaskFrameType() const;
  void SetMaskFrameType(MYON_TextMask::MaskFrame  source);

  MYON_Color MaskColor() const;  // Only works right if MaskColorSource returns 1.
  // Does not return viewport background color
  void SetMaskColor(MYON_Color color);

  // Offset for the border around text to the rectangle used to draw the mask
  // This number is the offset on each side of the tight rectangle around the
  // text characters to the mask rectangle.
  double MaskBorder() const;
  void SetMaskBorder(double offset);

  // The MYON_TextMask class contains the property values for
  //   DrawTextMask()
  //   MaskColor()
  //   MaskFillType()
  //   MaskBorder()
  // Use the 
  //   SetDrawTextMask()
  //   SetMaskColor()
  //   SetMaskFillType()
  //   SetMaskBorder()
  //   functions to modify text mask properties.
  const MYON_TextMask& TextMask() const;
  void SetTextMask(const MYON_TextMask& text_mask);

private:
  void Internal_SetTextMask(
    const MYON_TextMask& text_mask
  );
public:

  void Scale(double scale);

  // UUID of the dimstyle this was originally copied from
  // so Restore Defaults has some place to look
  void SetSourceDimstyle(MYON_UUID source_uuid);
  MYON_UUID SourceDimstyle() const;

  void SetExtensionLineColorSource(const MYON::object_color_source src);
  MYON::object_color_source ExtensionLineColorSource() const;
  void SetDimensionLineColorSource(const MYON::object_color_source src);
  MYON::object_color_source DimensionLineColorSource() const;
  void SetArrowColorSource(const MYON::object_color_source src);
  MYON::object_color_source ArrowColorSource() const;
  void SetTextColorSource(const MYON::object_color_source src);
  MYON::object_color_source TextColorSource() const;
  void SetExtensionLineColor(MYON_Color c);
  MYON_Color ExtensionLineColor() const;
  void SetDimensionLineColor(MYON_Color c);
  MYON_Color DimensionLineColor() const;
  void SetArrowColor(MYON_Color c);
  MYON_Color ArrowColor() const;
  void SetTextColor(MYON_Color c);
  MYON_Color TextColor() const;

  void SetExtensionLinePlotColorSource(const MYON::plot_color_source src);
  MYON::plot_color_source ExtensionLinePlotColorSource() const;
  void SetDimensionLinePlotColorSource(const MYON::plot_color_source src);
  MYON::plot_color_source DimensionLinePlotColorSource() const;
  void SetArrowPlotColorSource(const MYON::plot_color_source src);
  MYON::plot_color_source ArrowPlotColorSource() const;
  void SetTextPlotColorSource(const MYON::object_color_source src);
  MYON::object_color_source TextPlotColorSource() const;
  void SetExtensionLinePlotColor(MYON_Color c);
  MYON_Color ExtensionLinePlotColor() const;
  void SetDimensionLinePlotColor(MYON_Color c);
  MYON_Color DimensionLinePlotColor() const;
  void SetArrowPlotColor(MYON_Color c);
  MYON_Color ArrowPlotColor() const;
  void SetTextPlotColor(MYON_Color c);
  MYON_Color TextPlotColor() const;

  void SetExtensionLinePlotWeightSource(const MYON::plot_weight_source src);
  MYON::plot_weight_source ExtensionLinePlotWeightSource() const;
  void SetDimensionLinePlotWeightSource(const MYON::plot_weight_source src);
  MYON::plot_weight_source DimensionLinePlotWeightSource() const;
  void SetExtensionLinePlotWeight(double w);
  double ExtensionLinePlotWeight() const;
  void SetDimensionLinePlotWeight(double w);
  double DimensionLinePlotWeight() const;

  void SetFixedExtensionLen(double l);
  double FixedExtensionLen() const;
  void SetFixedExtensionLenOn(bool on);
  bool FixedExtensionLenOn() const;

  void SetTextRotation(double r);
  double TextRotation() const;

  void SetAlternateToleranceResolution(int r);
  int AlternateToleranceResolution() const;

  void SetSuppressArrow1(bool s);
  bool SuppressArrow1() const;
  void SetSuppressArrow2(bool s);
  bool SuppressArrow2() const;
  void SetTextMoveLeader(int m);

  int TextMoveLeader() const;
  void SetArcLengthSymbol(int m);
  int ArcLengthSymbol() const;

  void SetStackFractionFormat(MYON_DimStyle::stack_format f);
  MYON_DimStyle::stack_format StackFractionFormat() const;
  void SetStackHeightScale(double f);
  double StackHeightScale() const;

  void SetRoundOff(double r);
  double RoundOff() const;
  void SetAlternateRoundOff(double r);
  double AlternateRoundOff() const;
  void SetAngleRoundOff(double r);
  double AngleRoundOff() const;
  void SetZeroSuppress(MYON_DimStyle::suppress_zero s);
  MYON_DimStyle::suppress_zero ZeroSuppress() const;
  void SetAlternateZeroSuppress(MYON_DimStyle::suppress_zero s);
  MYON_DimStyle::suppress_zero AlternateZeroSuppress() const;

  // OBSOLETE - The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
  void SetToleranceZeroSuppress(MYON_DimStyle::suppress_zero s);

  // OBSOLETE - The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
  MYON_DimStyle::suppress_zero ToleranceZeroSuppress() const;

  void SetAngleZeroSuppress(MYON_DimStyle::suppress_zero s);
  MYON_DimStyle::suppress_zero AngleZeroSuppress() const;
  void SetAlternateBelow(bool below);

  /*
  Description:
    The valid choices for MYON_DimStyle::suppress_zero depend on
    the dimension length display.
  Parameters:
    zero_suppress - [in]
    length_display - [in]
  Returns:
    True if zero_suppression is a valid setting when
    DimensionLengthDiplay = dimension_length_display
  Remarks:
   LengthDisplay: Inch fractional – No zero suppression matches
   LengthDisplay : FeetAndInches – Zero suppress can be
                                              None, 
                                              Suppress zero feet,
                                              Suppress zero inches or
                                              Suppress zero feet and zero inches.
   LengthDisplay : ModelUnits or any Decimal mode – Zero suppress can be
                                              None,
                                              Suppress leading,
                                              Suppress trailing or
                                              Suppress leading and trailing.
  */
  static bool ZeroSuppressMatchesLengthDisplay(
    MYON_DimStyle::suppress_zero zero_suppress,
    MYON_DimStyle::LengthDisplay length_display);

  bool AlternateBelow() const;

  MYON_Arrowhead::arrow_type ArrowType1() const;
  void SetArrowType1(MYON_Arrowhead::arrow_type);
  MYON_Arrowhead::arrow_type ArrowType2() const;
  void SetArrowType2(MYON_Arrowhead::arrow_type);
  void SetArrowType1And2(MYON_Arrowhead::arrow_type);
  MYON_Arrowhead::arrow_type LeaderArrowType() const;
  void SetLeaderArrowType(MYON_Arrowhead::arrow_type);

  void SetArrowBlockId1(MYON_UUID id);
  MYON_UUID ArrowBlockId1() const;
  void SetArrowBlockId2(MYON_UUID id);
  MYON_UUID ArrowBlockId2() const;
  void SetLeaderArrowBlockId(MYON_UUID id);
  MYON_UUID LeaderArrowBlockId() const;

  MYON::TextVerticalAlignment TextVerticalAlignment() const;
  void SetTextVerticalAlignment(MYON::TextVerticalAlignment style);
  MYON::TextVerticalAlignment LeaderTextVerticalAlignment() const; // was attachstyle
  void SetLeaderTextVerticalAlignment(MYON::TextVerticalAlignment style);
  MYON_DimStyle::ContentAngleStyle LeaderContentAngleStyle() const; // was contentalignment
  void SetLeaderContentAngleStyle(MYON_DimStyle::ContentAngleStyle style);
  MYON_DimStyle::leader_curve_type LeaderCurveType() const;
  void SetLeaderCurveType(MYON_DimStyle::leader_curve_type type);
  bool LeaderHasLanding() const;
  void SetLeaderHasLanding(bool landing);
  double LeaderLandingLength() const;
  void SetLeaderLandingLength(double length);
  double LeaderContentAngleRadians() const;
  void SetLeaderContentAngleRadians(double angle_radians);
  double LeaderContentAngleDegrees() const;
  void SetLeaderContentAngleDegrees(double angle_degrees);
  MYON::TextHorizontalAlignment TextHorizontalAlignment() const;
  void SetTextHorizontalAlignment(MYON::TextHorizontalAlignment halign);
  MYON::TextHorizontalAlignment LeaderTextHorizontalAlignment() const;
  void SetLeaderTextHorizontalAlignment(MYON::TextHorizontalAlignment halign);
  bool DrawForward() const;
  void SetDrawForward(bool drawforward);
  bool SignedOrdinate() const;
  void SetSignedOrdinate(bool allowsigned);

  /// <summary>
  /// NOTE WELL: A dimstyle unit system was added in V6, but has never been fully used.
  /// The idea was this would make it easier to figure out what text height/ arrow size, 
  /// ... actually meant. Especially in situations where model space and page space have
  /// different unit systems, and in more complex cases like text in instance definitions
  /// and inserting annotation from models with mismatched unit systems.
  /// It is used internally to get some scales properly set and use in limited
  /// merging contexts.
  ///   
  /// From a user's perspective, in Rhino 6 and Rhino 7 MYON_DimStyle lengths like TextHeight(), ArrowSize(), ... 
  /// are with respect to the context the annotation resides in. For example, if TextHeight() = 3.5,
  /// model units = meters, page units = millimters, and DimScale() = 1, then 
  /// text created in model space will be 3.5 meters high and
  /// text created in page space will be 3.5 millimeters high.
  /// 
  /// Ideally, MYON_DimStyle::UnitSystem() would specify the text height units 
  /// and MYON_DimStyle::DimScale() cound be adjusted as model space extents require.
  /// Text in instance definitions would have a well defined height and references
  /// to those instance definitions would display predictably in both model space and page space.
  ///</summary>
  MYON::LengthUnitSystem UnitSystem() const;

  /// <summary>
  /// NOTE WELL: A dimstyle unit system was added in V6, but has never been fully used.
  /// The idea was this would make it easier to figure out what text height/ arrow size, 
  /// ... actually meant. Especially in situations where model space and page space have
  /// different unit systems, and in more complex cases like text in instance definitions
  /// and inserting annotation from models with mismatched unit systems.
  /// It is used internally to get some scales properly set and use in limited
  /// merging contexts.
  ///   
  /// From a user's perspective, in Rhino 6 and Rhino 7 MYON_DimStyle lengths like TextHeight(), ArrowSize(), ... 
  /// are with respect to the context the annotation resides in. For example, if TextHeight() = 3.5,
  /// model units = meters, page units = millimters, and DimScale() = 1, then 
  /// text created in model space will be 3.5 meters high and
  /// text created in page space will be 3.5 millimeters high.
  /// 
  /// Ideally, MYON_DimStyle::UnitSystem() would specify the text height units 
  /// and MYON_DimStyle::DimScale() cound be adjusted as model space extents require.
  /// Text in instance definitions would have a well defined height and references
  /// to those instance definitions would display predictably in both model space and page space.
  ///</summary>
  void SetUnitSystem(MYON::LengthUnitSystem us);

  /*
  Description:
    When a dimension style unit system is not set,
    this function examines the context the dimension style is
    being used in and sets the unit system.
    Ideally, both source_unit_system and destination_unit_system are page space units.
    Less ideally, both source_unit_system and destination_unit_system are model space units.
  Parameters:
    bUseName - [in]
      Consider the name when assigning a unit system.
      For example, a dimension style name "Millimters Small" would 
      be assigned a unit system of millimeters.
    source_unit_system - [in]
      unit system in the context where the dimension style originated.
    destination_unit_system - [in]
      unit system in the context where the dimension style will be used.
  */
  void SetUnitSystemFromContext(
    bool bUseName,
    MYON::LengthUnitSystem source_unit_system,
    MYON::LengthUnitSystem destination_unit_system
  );

  /*
  /// NOTE WELL: A dimstyle unit system was added in V6, but has never been fully used.
  /// The idea was this would make it easier to figure out what text height/ arrow size,
  /// ... actually meant. Especially in situations where model space and page space have
  /// different unit systems, and in more complex cases like text in instance definitions
  /// and inserting annotation from models with mismatched unit systems.
  /// It is used internally to get some scales properly set and use in limited
  /// merging contexts.
  ///
  /// From a user's perspective, in Rhino 6 and Rhino 7 MYON_DimStyle lengths like TextHeight(), ArrowSize(), ...
  /// are with respect to the context the annotation resides in. For example, if TextHeight() = 3.5,
  /// model units = meters, page units = millimters, and DimScale() = 1, then
  /// text created in model space will be 3.5 meters high and
  /// text created in page space will be 3.5 millimeters high.
  ///
  /// Ideally, MYON_DimStyle::UnitSystem() would specify the text height units
  /// and MYON_DimStyle::DimScale() cound be adjusted as model space extents require.
  /// Text in instance definitions would have a well defined height and references
  /// to those instance definitions would display predictably in both model space and page space.
  Returns:
    true if the unit system is set to an explicit valid length unit.
  */
  bool UnitSystemIsSet() const;

  const MYON_ScaleValue& ScaleValue() const;
  void SetDimScale(MYON_ScaleValue sv);
  void SetDimScale(double left_val, MYON::LengthUnitSystem left_us, double right_val, MYON::LengthUnitSystem right_us);
  
  double ScaleLeftLength_mm() const;
  double ScaleRightLength_mm() const;

  void SetDimScale(double scale);
  double DimScale() const;

  void SetDimScaleSource(int source);
  int DimScaleSource() const;          // 0: Global DimScale, 1: DimStyle DimScale

  void SetTextOrientation(MYON::TextOrientation);
  MYON::TextOrientation TextOrientation() const;

  void SetLeaderTextOrientation(MYON::TextOrientation);
  MYON::TextOrientation LeaderTextOrientation() const;

  void SetDimTextOrientation(MYON::TextOrientation);
  MYON::TextOrientation DimTextOrientation() const;

  void SetDimRadialTextOrientation(MYON::TextOrientation);
  MYON::TextOrientation DimRadialTextOrientation() const;

  MYON_DimStyle::ContentAngleStyle DimTextAngleStyle() const;
  void SetDimTextAngleStyle(MYON_DimStyle::ContentAngleStyle style);

  MYON_DimStyle::ContentAngleStyle DimRadialTextAngleStyle() const;
  void SetDimRadialTextAngleStyle(MYON_DimStyle::ContentAngleStyle style);

  bool TextUnderlined() const;
  void SetTextUnderlined(bool underlined);

  bool ForceDimLine() const;
  void SetForceDimLine(bool forcedimline);

  void SetArrowFit(MYON_DimStyle::arrow_fit arrowfit);
  MYON_DimStyle::arrow_fit ArrowFit() const;

  void SetTextFit(MYON_DimStyle::text_fit textfit);
  MYON_DimStyle::text_fit TextFit() const;

  void SetDecimalSeparator(wchar_t separator);
  wchar_t DecimalSeparator() const;

  //double ModelSize() const;
  //void SetModelSize(double size);
  //double PaperSize() const;
  //void SetPaperSize(double size);

  // For converting to and from V5 Dimstyles
  static int V5ArrowType(MYON_Arrowhead::arrow_type v6type);
  static int V5LengthFormat(MYON_DimStyle::OBSOLETE_length_format v6format);
  static int V5AngleFormat(MYON_DimStyle::angle_format v6format);
  static int V5ToleranceFormat(MYON_DimStyle::tolerance_format v6format);
  static int V5MaskColorSourceFromV6MaskType(MYON_TextMask::MaskType mask_type);
  static MYON_Arrowhead::arrow_type V6ArrowType(int v5type);
  static MYON_DimStyle::OBSOLETE_length_format V6LengthFormat(int v5format);
  static MYON_DimStyle::angle_format V6AngleFormat(int v5format);
  static MYON_DimStyle::tolerance_format V6ToleranceFormat(int v5format);
  static MYON_TextMask::MaskType V6MaskTypeFromV5MaskColorSource(int v5_mask_color_source);

private:
  double m_extextension                            = 0.5; // extension line extension
  double m_extoffset                               = 0.5;    // extension line offset
  double m_arrowsize                               = 1.0;  // length of an arrow - may mean different things to different arrows
  double m_leaderarrowsize                         = 1.0;  // length of an arrow for leader style dimensions
  double m_centermark                              = 0.5; // size of the + at circle centers
  MYON_DimStyle::centermark_style m_centermark_style = MYON_DimStyle::centermark_style::Mark; // Display style for centermarks
  double m_textgap                                 = 0.25;    // gap around the text for clipping dim line
  double m_textheight                              = 1.0; // model unit height of dimension text before applying dimscale

  //MYON::OBSOLETE_V5_TextDisplayMode m_REMOVE_ME_dimstyle_textalign  = MYON::OBSOLETE_V5_TextDisplayMode::kAboveLine;
  MYON_DimStyle::TextLocation m_dim_text_location    = MYON_DimStyle::TextLocation::AboveDimLine;
  MYON_DimStyle::TextLocation m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;

  MYON_DimStyle::angle_format m_angleformat          = MYON_DimStyle::angle_format::DecimalDegrees;
  int m_angleresolution = 2;    // for decimal degrees, digits past decimal

  /// <summary>
  /// The DimensionLengthDisplay is property controls the unit system and format
  /// of for display of lengths in dimensions. For more information, see the 
  /// descriptions of the MYON_DimStyle::LengthDisplay enum values.
  ///</summary>
  MYON_DimStyle::LengthDisplay m_dimension_length_display = MYON_DimStyle::LengthDisplay::ModelUnits;

  /// <summary>
  /// Alternate DimensionLengthDisplay property. 
  /// See the description of m_dimension_length_display for more information about this property.
  ///</summary>
  MYON_DimStyle::LengthDisplay m_alternate_dimension_length_display = MYON_DimStyle::LengthDisplay::ModelUnits;

  /// <summary>
  /// The LengthResolution property controls the precision of dimension length display.
  ///
  /// DECIMAL LENGTH DISPLAY: 
  ///   If m_dimension_length_display is any of the MYON_DimStyle::LengthDisplay decimal formats,
  ///   then m_lengthresolution is the number of digits after the decimal point.
  ///   For example, if m_lengthresolution is 2, then dimension length display will be n.ff
  ///   If m_lengthresolution=7,  then dimension length display will be n.fffffff.
  ///
  /// FRACTONAL LENGTH DISPLAY: 
  ///   If m_dimension_length_display is MYON_DimStyle::LengthDisplay::InchesFractional or
  ///   MYON_DimStyle::LengthDisplay::FeetAndInches, then fractional length display is used.
  ///   In this case any fractional part will be rouded to the closest multiple 
  ///   of 1/(2^m_alternate_lengthresolution). 
  ///   Examples: If fractional length display is used and m_lengthresolution=2, 
  //    then the possible fractions are 1/4, 1/2(=2/4), 3/4. 
  ///   If fractional length display is used and m_lengthresolution=7, 
  //    then any fractional part is rounded to the closest multipl of 1/128 (128=2^7).
  /// </summary>
  int m_lengthresolution = 2;

  /// <summary>
  /// Alternate LengthResolution property. 
  /// See the description of m_lengthresolution for more information about this property.
  ///</summary>
  int m_alternate_lengthresolution = 2;

  /// <summary>
  /// The LengthFactor is property a rarely used. It applies when a model is being
  /// drawn to a scale and the dimension length values should be
  /// reverse scaled. For example, if a model is drawn at 1/4 scale,
  /// a line 5 units long indicates the real world line is 20 units
  /// long. In this case setting LengthFactor to 4 would cause
  /// a linear dimension applied to that line to display a value of 20.
  /// Use the DimensionLengthDisplay property to control length unit system scaling.
  ///</summary>
  double m_lengthfactor = 1.0;  // (dimlfac) model units multiplier for length display

  /// <summary>
  /// Alternate LengthFactor property. 
  /// See the description of m_lengthfactor for more information about this property.
  ///</summary>
  double m_alternate_lengthfactor = 1.0; // (dimaltf) model units multiplier for alternate length display

  
private:
  // A copy of the font_characteristics passed to SetFont. 
  // This information is saved in 3dm archives.
  MYON_Font m_font_characteristics = MYON_Font::Default;

  // The managed font returned by MYON_Font::GetManagedFont(m_font).
  // This is the value returned by MYON_DimStyle.Font().
  const MYON_Font* m_managed_font = &MYON_Font::Default;

private:
  // all dim style content
  mutable MYON_SHA1_Hash m_content_hash = MYON_SHA1_Hash::EmptyContentHash;

  // text position properties content
  mutable MYON_SHA1_Hash m_text_position_properties_hash = MYON_SHA1_Hash::EmptyContentHash;

  mutable MYON_SHA1_Hash m_reserved_hash2 = MYON_SHA1_Hash::EmptyContentHash;

  // parent dim style content
  // All code should use ParentContentHash() to inspect this value.
  // Is is set by OverrideFields(). It may be cleared by a call to ParentContentHash().
  mutable MYON_SHA1_Hash m_parent_dim_style_content_hash = MYON_SHA1_Hash::EmptyContentHash;
  
private:
    

  bool m_bAlternate                                = false;      // (dimalt) display alternate dimension string (or not)
  
  bool m_bForceDimLine                             = true;                         // 4/30/2019
  MYON_DimStyle::arrow_fit m_ArrowFit                = MYON_DimStyle::arrow_fit::Auto; // 4/30/2019
  MYON_DimStyle::text_fit m_TextFit                  = MYON_DimStyle::text_fit::Auto;  // 4/30/2019
  wchar_t m_decimal_separator                      = MYON_wString::DecimalAsPeriod;

  MYON_wString m_prefix;              // string preceding dimension value string
  MYON_wString m_suffix;              // string following dimension value string
  MYON_wString m_alternate_prefix;    // string preceding alternate value string (Default = " [")
  MYON_wString m_alternate_suffix;    // string following alternate value string (Default = "]")

  double m_dimextension                            = 0.0;  // (dimdle) dimension line extension past the "tip" location

  bool   m_bSuppressExtension1                     = false;   // flag to not draw extension lines
  bool   m_bSuppressExtension2                     = false;   // flag to not draw extension lines
  bool   m_bReserved1 = false;
  bool   m_bReserved2 = false;

  // m_field_override_count
  //   number of MYON_DimStyle::field settings that are independent of the parent dimension style.
  //   (not inherited from)
  //   A value of 0 indicates every possible setting is inherited from the parent dimension style.
  //   A value > 0 indicates at least one setting is independent of the parent dimension style.
  MYON__UINT32 m_field_override_parent_count              = 0;

  // m_field_override_bitsN (Up to 128 true/false) values.
  //   Each MYON_DimStyle::field enum value > MYON_DimStyle::field::Unset and < MYON_DimStyle::field::Count
  //   has a corresponding bit in on of the m_field_override_bitsN values. 
  //   When the bit is clear (0), the corresponding override parent setting value = false and 
  //   that setting is inheritied from the parent dimension style. 0 is the default setting.
  //   When the bit is set (1), the corresponding override parent setting value = true and 
  //   that setting is independent of the parent dimension style.
  MYON__UINT32 m_field_override_parent_bits0 = 0;
  MYON__UINT32 m_field_override_parent_bits1 = 0;
  MYON__UINT32 m_field_override_parent_bits2 = 0;
  MYON__UINT32 m_field_override_parent_bits3 = 0;
  // Please do not replace this bitfield with an array of bools.
  // Using bools approach will change the size of this class when additional dimension style
  // settings are added and makes construction code more complicated.


  /*
  Parameters:
    mask - [out]
      0: field_id is not valid 
        (Unset, Name, Index, >= Count) 
        Name and Index settings cannot be inherited from the parent dimension style
      not 0:
        mask identifies the bit in the 
    mask - [out]
      0: field_id is not valid 
        (Unset, Name, Index, >= Count) 
        Name and Index settings cannot be inherited from the parent dimension style
      not 0:
        mask identifies the bit in the 
  Returns:
    nullptr:
      field_id is not valid 
        (Unset, Name, Index, >= Count) 
        Name and Index settings cannot be inherited from the parent dimension style.
    not nullptr:
      Address of the m_field_override_bitsN member that mask applies to.
  */
  MYON__UINT32* Internal_GetOverrideParentBit(MYON_DimStyle::field field_id, MYON__UINT32* mask) const;

  MYON_DimStyle::tolerance_format m_tolerance_format = MYON_DimStyle::tolerance_format::None;
  int    m_tolerance_resolution                    = 4;
  double m_tolerance_upper_value                   = 0.0;   // or both upper and lower in symmetrical style
  double m_tolerance_lower_value                   = 0.0;
  double m_tolerance_height_scale                  = 0.7;  // relative to the main dimension text

  double m_baseline_spacing                        = 3.0;

  MYON_TextMask m_text_mask                          = MYON_TextMask::None;

  // m_dimscale replaced by m_scale_value.RightToLeftScale()
  //double   m_dimscale                              = 1.0;
  int      m_dimscale_source                       = 0;

  // Uuid of source dimstyle to restore defaults
  MYON_UUID  m_source_dimstyle                       = MYON_nil_uuid;

  // Sub-object draw colors
  unsigned char   m_ext_line_color_source          = 0;
  unsigned char   m_dim_line_color_source          = 0;
  unsigned char   m_arrow_color_source             = 0;
  unsigned char   m_text_color_source              = 0;
  MYON_Color        m_ext_line_color                 = MYON_Color::Black;
  MYON_Color        m_dim_line_color                 = MYON_Color::Black;
  MYON_Color        m_arrow_color                    = MYON_Color::Black;
  MYON_Color        m_text_color                     = MYON_Color::Black;
  unsigned char   m_ext_line_plot_color_source     = 0;
  unsigned char   m_dim_line_plot_color_source     = 0;
  unsigned char   m_arrow_plot_color_source        = 0;
  unsigned char   m_text_plot_color_source         = 0;
  MYON_Color        m_ext_line_plot_color            = MYON_Color::Black;
  MYON_Color        m_dim_line_plot_color            = MYON_Color::Black;
  MYON_Color        m_arrow_plot_color               = MYON_Color::Black;
  MYON_Color        m_text_plot_color                = MYON_Color::Black;
  unsigned char   m_ext_line_plot_weight_source    = 0;
  unsigned char   m_dim_line_plot_weight_source    = 0;
  double          m_ext_line_plot_weight_mm        = 0.0;
  double          m_dim_line_plot_weight_mm        = 0.0;

  double          m_fixed_extension_len            = 1.0;        // Fixed extension line length if m_fixed_extension_len_on is true
  bool            m_fixed_extension_len_on         = false;     // true: use fixed_extension_len, false: don't use m_fixed_extension_len
    
  unsigned char   m_ReservedChar1                  = 0;
  unsigned short  m_ReservedShort1 = 0;
  unsigned int    m_ReservedInt1 = 0;

  double          m_text_rotation                  = 0.0;              // Dimension text rotation around text point (radians)
  int             m_alternate_tolerance_resolution = 4; // for decimal, digits past the decimal point, fractions: 1/2^n
  double          m_tol_textheight_fraction        = 0.6;    // fraction of main text height

  bool            m_suppress_arrow1                = false;            // false: dont suppress, true: suppress
  bool            m_suppress_arrow2                = false;            // false: dont suppress, true: suppress

  unsigned short  m_ReservedShort2                 = 0;

  int             m_textmove_leader                = 0;                // 0: move text anywhere, 1: add leader when moving text
  int             m_arclength_sym                  = 0;                // 0: symbol before dim text, 1: symbol above dim text, no symbol
  double          m_stack_textheight_fraction      = 0.7;              // fraction of main text height
  MYON_DimStyle::stack_format    m_stack_format      = MYON_DimStyle::stack_format::StackHorizontal;
  double          m_alt_round                      = 0.0;              // rounds to nearest specified value
  double          m_round                          = 0.0;
  double          m_angular_round                  = 0.0;

  MYON_DimStyle::suppress_zero   m_zero_suppress     = MYON_DimStyle::suppress_zero::None;
  MYON_DimStyle::suppress_zero   m_alt_zero_suppress = MYON_DimStyle::suppress_zero::None;

  MYON_DimStyle::suppress_zero   m_ang_zero_suppress = MYON_DimStyle::suppress_zero::None;

  bool            m_alt_below = false;                  // true: display alternate text below main text

  // false: display alternate text after main text
  MYON_Arrowhead::arrow_type m_arrow_type_1          = MYON_Arrowhead::arrow_type::SolidTriangle;      // Arrow types for MYON_Dimension derived dimensions
  MYON_Arrowhead::arrow_type m_arrow_type_2          = MYON_Arrowhead::arrow_type::SolidTriangle;
  MYON_Arrowhead::arrow_type m_leader_arrow_type     = MYON_Arrowhead::arrow_type::SolidTriangle;
  MYON_UUID         m_arrow_block_id_1               = MYON_nil_uuid;
  MYON_UUID         m_arrow_block_id_2               = MYON_nil_uuid;
  MYON_UUID         m_leader_arrow_block_id          = MYON_nil_uuid;

  // Text 
  MYON::TextVerticalAlignment  m_text_vertical_alignment    = MYON::TextVerticalAlignment::Top;
  MYON::TextHorizontalAlignment m_text_horizontal_alignment    = MYON::TextHorizontalAlignment::Left;

  // Leader
  MYON::TextVerticalAlignment  m_leader_text_vertical_alignment  = MYON::TextVerticalAlignment::Middle;
  MYON::TextHorizontalAlignment m_leader_text_horizontal_alignment  = MYON::TextHorizontalAlignment::Left;

  MYON_DimStyle::ContentAngleStyle   m_leader_content_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
  MYON_DimStyle::leader_curve_type   m_leader_curve_type          = MYON_DimStyle::leader_curve_type::Polyline;
  double              m_leader_content_angle       = 0.0;
  bool                m_leader_has_landing         = true;

  double              m_leader_landing_length      = 1.0;

  bool                m_draw_forward               = true;
  bool                m_signed_ordinate            = true;

  MYON_ScaleValue       m_scale_value                = MYON_ScaleValue::OneToOne;

  /// NOTE WELL: A dimstyle unit system was added in V6, but has never been fully used.
  /// The idea was this would make it easier to figure out what text height/ arrow size, 
  /// ... actually meant. Especially in situations where model space and page space have
  /// different unit systems, and in more complex cases like text in instance definitions
  /// and inserting annotation from models with mismatched unit systems.
  /// It is used internally to get some scales properly set and use in limited
  /// merging contexts.
  ///   
  /// From a user's perspective, in Rhino 6 and Rhino 7 MYON_DimStyle lengths like TextHeight(), ArrowSize(), ... 
  /// are with respect to the context the annotation resides in. For example, if TextHeight() = 3.5,
  /// model units = meters, page units = millimters, and DimScale() = 1, then 
  /// text created in model space will be 3.5 meters high and
  /// text created in page space will be 3.5 millimeters high.
  /// 
  /// Ideally, MYON_DimStyle::UnitSystem() would specify the text height units 
  /// and MYON_DimStyle::DimScale() cound be adjusted as model space extents require.
  /// Text in instance definitions would have a well defined height and references
  /// to those instance definitions would display predictably in both model space and page space.
  MYON::LengthUnitSystem m_dimstyle_unitsystem                    = MYON::LengthUnitSystem::None;

  MYON::TextOrientation m_text_orientation                        = MYON::TextOrientation::InPlane;
  MYON::TextOrientation m_leader_text_orientation                 = MYON::TextOrientation::InPlane;
  MYON::TextOrientation m_dim_text_orientation                    = MYON::TextOrientation::InPlane;
  MYON::TextOrientation m_dimradial_text_orientation              = MYON::TextOrientation::InPlane;

  MYON_DimStyle::ContentAngleStyle       m_dim_text_angle_style       = MYON_DimStyle::ContentAngleStyle::Aligned;
  MYON_DimStyle::ContentAngleStyle       m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;

  bool m_text_underlined = false; // extra/extended line under text block in leaders and radial dimensions

private:
  // The parent dimstyle's managed font.
  // Use the ParentDimStyleFont() member function to query this field.
  // 
  const MYON_Font* m_parent_dimstyle_managed_font = nullptr;

private:
  void Internal_ContentChange() const;
};


/*
Description:
  A general and portable interface to access a model's available dimension styles.
Remarks:
  The Rhino C++ SDK function CRhinoDoc.DimStyleContext() will return an MYON_DimStyleContext for the Rhino model.
  The MYONX_Model function MYONX_Model.DimStyleContext() will return an MYON_DimStyleContext for MYONX_Model model.
*/
class MYON_CLASS MYON_DimStyleContext
{
public:
  MYON_DimStyleContext() = default;
  virtual ~MYON_DimStyleContext();
  MYON_DimStyleContext(const MYON_DimStyleContext&) = default;
  MYON_DimStyleContext& operator=(const MYON_DimStyleContext&) = default;

public:
  virtual const MYON_DimStyle& CurrentDimStyle() const;

  virtual const MYON_DimStyle* DimStyleFromId(
    MYON_UUID id,
    const MYON_DimStyle* not_found_result = nullptr
  ) const;

  virtual const MYON_DimStyle* DimStyleFromName(
    const MYON_NameHash& name_hash,
    const MYON_DimStyle* not_found_result = nullptr
  ) const;

  virtual const MYON_DimStyle* DimStyleFromContentHash(
    const MYON_SHA1_Hash& content_hash,
    const MYON_DimStyle* not_found_result = nullptr
  ) const;

  virtual const MYON_DimStyle* DimStyleFromFont(
    const MYON_Font& font,
    double model_space_text_scale,
    double text_height,
    MYON::LengthUnitSystem text_height_unit_system,
    bool bReturnClosestMatch = true,
    const MYON_DimStyle* not_found_result = nullptr
  ) const;

  virtual bool AddDimStyle(
    const MYON_DimStyle& dim_style,
    bool bResolveNameAndIdConflicts
  );

  virtual bool ModifyDimStyle(
    MYON_UUID model_dim_style_id,
    const MYON_DimStyle& dim_style
  );

  virtual const MYON_DimStyle* FirstDimStyle(
    bool bIncludeSystemDimStyles = false,
    bool bIncludeDeletedDimStyles = false
  ) const;

  virtual const MYON_DimStyle* NextDimStyle(
    MYON_UUID id,
    bool bIncludeSystemDimStyles = false,
    bool bIncludeDeletedDimStyles = false
  ) const;

  virtual const MYON_DimStyle* PrevDimStyle(
    MYON_UUID id,
    bool bIncludeSystemDimStyles = false,
    bool bIncludeDeletedDimStyles = false
  ) const;

  virtual MYON::LengthUnitSystem ModelUnitSystem() const;
  
  virtual MYON__UINT64 ModelSerialNumber() const;

protected:
  mutable MYON::LengthUnitSystem m_unit_system = MYON::LengthUnitSystem::Millimeters;
  mutable MYON__UINT64 m_model_serial_number = 0;
};


#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_DimStyle*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ObjectArray<MYON_DimStyle>;
#endif

#endif

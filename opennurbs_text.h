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

#ifndef OPENNURBS_TEXT_H_INCLUDED
#define OPENNURBS_TEXT_H_INCLUDED

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template MYON_ClassArray< class MYON_Font >;
MYON_DLL_TEMPLATE template MYON_SimpleArray< class MYON_TextRun* >;
MYON_DLL_TEMPLATE template MYON_SimpleArray< class MYON_FontGlyph* >;
#endif

#define MYON_TEXT_BRACKET_FRACTION
class MYON_CLASS MYON_TextContent  : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_TextContent);

public:
  static const MYON_TextContent Empty;

public:

  MYON_TextContent()= default;
  ~MYON_TextContent();
 
  MYON_TextContent(const MYON_TextContent& src);
  MYON_TextContent& operator=(const MYON_TextContent& src);

private:
  void Internal_Destroy();
  void Internal_CopyFrom(
    const MYON_TextContent& src
    );

public:

  /*
  Returns:
    A hash of the information that determines the text content
    using wrapped text with evaluated fields.
  */
  MYON_SHA1_Hash TextContentHash() const;

  /*
  Parameters:
    bApplyWrapping - [in]
      true - hash wrapped text
      false - has unwrapped text
    bEvaluateFields - [in]
      true - hash text with fields evaluated
      false - hash text with fields unevaluated
  Returns:
    A hash of the information that determines the text content
    without evaluating the fields.
  */
  MYON_SHA1_Hash TextContentHash(
    bool bApplyWrapping,
    bool bEvaluateFields
  ) const;

public:

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  // Parses text string and makes runs
  bool Create(
    const wchar_t* RtfString,
    MYON::AnnotationType annotation_type, // used to select type specific dimstyle properties like text alignment settings
    const MYON_DimStyle* dimstyle,
    bool bWrapped,
    double rect_width,
    double text_rotation_radians
    );

  bool Create(
    const wchar_t* RtfString,
    MYON::AnnotationType annotation_type, // used to select type specific dimstyle properties like text alignment settings
    const MYON_DimStyle* dimstyle
    );

  bool ReplaceTextString(
    const wchar_t* RtfString,
    MYON::AnnotationType annotation_type, // used to select type specific dimstyle properties like text alignment settings
    const MYON_DimStyle* dimstyle
    );

  bool RebuildRuns(
    MYON::AnnotationType annotation_type,
    const MYON_DimStyle* dimstyle
  );

  bool RunReplaceString(
    const wchar_t* repl_str,
    int start_run_idx,
    int start_run_pos,
    int end_run_idx,
    int end_run_pos);


  /*
  Returns:
    The value of MYON_DimStyle.TextPositionPropertiesHash() of the MYON_DimStyle
    passed to Create(), ReplaceTextString(), or RebuildRuns().
  */
  MYON_SHA1_Hash DimStyleTextPositionPropertiesHash() const;

  /*
  Returns:
    True if this text position information used to create this text
    is identical to the text position parameters on dimstyle.
  */
  bool EqualTextPositionProperties(
    MYON::AnnotationType annotation_type,
    const class MYON_DimStyle* dimstyle
  ) const;

  bool GetGlyphContours(
    const MYON_Font* text_font,
    bool bSingleStrokeFont,
    double text_height,
    MYON_ClassArray< MYON_ClassArray< MYON_SimpleArray< MYON_Curve* > > >& text_contours
  ) const;

  bool GetGlyphContours(
    const MYON_Font* text_font,
    bool bSingleStrokeFont,
    const MYON_Xform& text_xform,
    MYON_ClassArray< MYON_ClassArray< MYON_SimpleArray< MYON_Curve* > > >& text_contours
  ) const;

private:
  static MYON::AnnotationType Internal_AlignmentAnnotationType(
    MYON::AnnotationType annotation_type
  );

private:

  bool Internal_SetText(
    const wchar_t* text,
    const MYON_DimStyle* dimstyle);
  /*
  Parameters:
    dim_style - [in]
      Parse and compose text using the parameters in this dimstyle.
      If nullptr, then MYON_DimStyle::Default is used as the dimstyle.
    bComposeAndUpdateRtf - [in]
      If true, then the parsed runs are passed to RtfComposer::Compose()
      to compose create an efficient and compressed rtf. Then
      MYON_TextContent.m_text is set to the efficient and compressed rtf
      returned from RtfComposer::Compose().

      When this->m_text might be a bloated result from a text editing
      control or a simple string like "Hello world" set during
      construction, then bComposeAndUpdateRtf should be true.

      When this->m_text has already been through RtfComposer::Compose()
      and you simply need to create the runs from m_text, then
      bComposeAndUpdateRtf should be false.  Reading binary archives
      is one case when bComposeAndUpdateRtf should be false.

      Internal_ParseRtf() sets the m_dimstyle_text_position_properties_hash member
      on MYON_TextContent
  */
 bool Internal_ParseRtf(
    const wchar_t* rtf_string,
    const MYON_DimStyle* dim_style,
    bool bComposeAndUpdateRtf
    );
  bool Internal_ParseRtfDefault(
    const wchar_t* rtf_string,
    bool bComposeAndUpdateRtf
    );

public:

  /*
  Returns:
    Rhino clean rich text that is currently saved on this instance.
    The text can contain text formatting instructions.
    Fields are not evaluated.
  */
  const MYON_wString RichText() const;

  /*
  Returns:
    Rich text suitable for initializing SDK controls on the current platform (Windows or Apple OS X).
    The returned string is always rich text (never plain text).
    The returned rich text is always generated directly from the runs.
    This text is typically different from the Rhino clean rich text returned by the RichText() command.
  */
  const MYON_wString PlatformRichTextFromRuns() const;

  
  /*
  Parameters:
    rich_text_style - [in]
      Type of rich text to return.
  Returns:
    The returned string is always rich text (never plain text).
    The returned rich text is always generated directly from the runs.
    This text is typically different from the Rhino clean rich text returned by the RichText() command.
  */
  const MYON_wString RichTextFromRuns(MYON::RichTextStyle rich_text_style) const;

  /*
  Returns:
  Plain text information with any rich text formatting instructions removed.
  The result string from evaluating fields is included
  Field results may be cached from previous evaluation
  */
  const MYON_wString PlainText() const;
  /*
  Same as PlainText() but separated wrapped run lines with '\n' for soft return
  and '\r''\n' for hard returns
  */
  const MYON_wString WrappedPlainText() const;

  /*
  Returns:
  Plain text information with any rich text formatting instructions removed.
  Fields are not evaluated
  */
  const MYON_wString PlainTextWithFields() const;

  /*
  Returns the same string as PlainTextWithFields() and fills in the runmap
    Each 3dex in the runmap array is 
    i: run index, 
    j: position in the string where text from run[i] starts, 
    k: length of text from run[i]
  */
  const MYON_wString PlainTextWithFields(MYON_SimpleArray<MYON_3dex>* runmap) const;

  /*
  Same as PlainTextWithFields() but separated wrapped run lines with '\n' for soft return
  and '\r''\n' for hard returns
  */
  const MYON_wString WrappedPlainTextWithFields() const;


private:
  void Internal_SetRunTextHeight(double height);

public:
  void GetAlignment(MYON::TextHorizontalAlignment& horz, MYON::TextVerticalAlignment& vert) const;
  void SetAlignment(MYON::TextHorizontalAlignment horz, MYON::TextVerticalAlignment vert);

  // FormattingRectangleWidth is a width set by text wrapping. It's in model units
  double FormattingRectangleWidth() const;
  void SetFormattingRectangleWidth(double width);
  // Rotation in radians around origin
  double TextRotationRadians() const;
  void SetTextRotationRadians(double rotation);
  // Rotation in degrees around origin
  double TextRotationDegrees() const;
  void SetTextRotationDegrees(double rotation);

  unsigned int EvaluationSerialNumber() const;
  void SetEvaluationSerialNumber(unsigned int sn) const;

  void RealignTextRuns(MYON::TextHorizontalAlignment new_h_align);

  // virtual
  void Dump( MYON_TextLog& ) const override; // for debugging

  int Dimension() const override;

  MYON::object_type ObjectType() const override;

  bool Write(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const override;

  bool Read(
    MYON_BinaryArchive&  // restore definition from binary archive
    ) override;

  const MYON_BoundingBox TextContentBoundingBox() const;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  void ClearBoundingBox() override;

  bool Transform(const MYON_Xform& xform) override;

  // Wrap text to a specified width in model space
  bool WrapText(double width) const;
  
  // True if text has wrapped runs, else false
  bool HasWrappedRuns() const;

  // True if flag to wrap is set
  bool TextIsWrapped() const;
  void SetTextIsWrapped(bool wrapped);

private:
  void Internal_DeleteWrappedRuns() const;

  const MYON_wString Internal_GetPlainText(bool evaluate_fields, bool wrapped) const;
  const MYON_wString Internal_GetPlainText(bool evaluate_fields, bool wrapped, MYON_SimpleArray<MYON_3dex>* runmap) const;


public:

  // Get the size of a 2d rectangle enclosing text
  bool Get2dSize(bool raw, double& width, double& height) const;

  // Get corners of the whole text object
  // corners requires space for 4 points
  bool Get2dCorners(MYON_2dPoint corners[4]) const;

  // Get corners of the whole text object
  // corners requires space for 4 points
  // Basic size of text, not including annotation scaling or 2d rotation
  // And rotated and translated in 3d to text's plane (not object's plane)
  bool Get3dCorners(MYON_3dPoint corners[4]) const;

  // Get corners of the whole text object
  // inflated by the border distance for mask drawing
  // corners requires space for 4 points
  bool Get3dMaskCorners(double border, MYON_3dPoint corners[4]) const;

  // Gets endpoints of a line under the text offset down by textgap
  bool Get3dUnderline(MYON_3dPoint ends[2], double scaled_gap) const;

  // Get corners of individual runs
  // corners requires space for 4 points
  bool GetRun3dCorners(const MYON_TextRun* run, MYON_3dPoint corners[4]) const;

  // returns the base point and with grip using the current alignments
  void GetGripPoints(MYON_2dPoint& base, MYON_2dPoint& width) const;

  MYON_Mesh* Get2dPickMesh() const;
  MYON_Mesh* Get3dPickMesh() const;
  MYON_Mesh* Get3dMaskPickMesh(double maskoffset) const;

  // Returns pointer to either m_runs, the basic parsed and evaluated text
  // or m_wrapped_runs which is the runs after text wrapping
  // m_wrapped_runs will be null unless the text has been wrapped
  // If raw is false and m_wrapped_runs is not null, m_wrapped_runs will be returned
  // If raw is true or m_wrapped_runs is null, m_runs will be returned  
  MYON_TextRunArray* TextRuns(bool bRaw) const;

  const wchar_t* RtfText() const;

  /*
  With runs in place, compose the text in the runs and
  fill in the Text's string that is returned by RtfText
  */
  bool ComposeText();

  ///*
  //Parameters:
  //  dimsytle - [in]
  //Returns:
  //  true if style was passed as dimstyle parameter to Create(), ReplaceTextString(),
  //  or RebuildRuns() and used to create the current text runs.
  //*/
  //bool IsCurrentDimStyle(
  //  const MYON_DimStyle* dimsytle
  //) const;

  //void SetCurrentDimStyle(const MYON_DimStyle* dimstyle) const;

  // Returns the default font for this text content.
  // Typically, that would be the font from the dimstyle
  const MYON_Font& DefaultFont() const;

  MYON::TextHorizontalAlignment RuntimeHorizontalAlignment() const;
  void SetRuntimeHorizontalAlignment(MYON::TextHorizontalAlignment halign) const;

private:
  // Data members
  //-----------------------
  MYON_wString                  m_text;          // Rtf laden string
  double                      m_rect_width = 1.0e300;  // formatting rectangle width in model units
  double                      m_rotation_radians = 0.0;   // radians rotation around origin
  double m_reserved_dbl = 0.0;
  MYON::TextHorizontalAlignment m_h_align = MYON::TextHorizontalAlignment::Left;    // Left, Center, Right
  MYON::TextVerticalAlignment   m_v_align = MYON::TextVerticalAlignment::Bottom;  // Top, Middle, Bottom
  
  // true when text is wrapped
  // Set by calling WrapText() or SetTextIsWrapped(true).
  // Query by calling TextIsWrapped().
  mutable bool                m_bWrapText = false;
  
  mutable MYON::TextHorizontalAlignment   m_runtime_halign = MYON::TextHorizontalAlignment::Auto;

  // m__runs and m__wrapped_runs are runtime information 
  // generated by parsing m_text and other information.
  mutable MYON_TextRunArray     m__runs;

  mutable MYON_TextRunArray*    m__wrapped_runs = nullptr;

  // display cache runtime value
  mutable unsigned int        m_run_evaluation_sn = 0; 

  // annotation type used to select dimstyle text alignment settings.
  mutable MYON::AnnotationType  m_annotation_type = MYON::AnnotationType::Unset;

  // dimstyle text position properties used to calculate the runs
  mutable MYON_SHA1_Hash m_dimstyle_text_position_properties_hash = MYON_SHA1_Hash::ZeroDigest;

  // hash of m_text, m_bWrapping, m_rect_width, m_rotation_radians, alignment
  mutable MYON_SHA1_Hash m_text_content_sub_hash = MYON_SHA1_Hash::ZeroDigest;

  MYON_SHA1_Hash Internal_TextContentSubHash() const;
  void Internal_ClearTextContentHash() const;

  // runtime bounding box
  // Value of TextContentHash() when m_text_content_bbox was set.
  mutable MYON_SHA1_Hash   m_text_content_bbox_hash = MYON_SHA1_Hash::ZeroDigest;
  mutable MYON_BoundingBox m_text_content_bbox = MYON_BoundingBox::EmptyBoundingBox;
  
  // Stores a pointer to the top level installed font for the entire textContent
  // May be overridden by font in individual runs
  // Dec. 12, 2019
  mutable const MYON_Font* m_default_font = &MYON_Font::Default;
  //MYON__INT_PTR m_reserved0 = (MYON__INT_PTR)0;


public:
  friend class MYON_Text;

  /*
  Description:
  Calculates the size, spacing and position of the runs in the MYON_TextContent
  Parameters:
  [in/out] MYON_TextContent& text  - Text to measure. Modified to store results
  [in] bool raw - if true, measure m_runs
  [in] bool wrapped - if true, measure m_wrapped_runs
  Returns:
  true = Success
  false = Failure
  Remarks:
  The runs in the text are modified to store the location info for positioning
  within the text object
  */
  static bool MeasureTextContent(MYON_TextContent* text, bool raw, bool wrapped);

  /*
  Description:
  Calculates the size, spacing and position of the runs in the MYON_TextRunArray
  Parameters:
  [in/out] MYON_TextRunArray* runs  - TextRuns to measure. Modified to store results
  [in] MYON::TextVerticalAlignment v_align - how to align the text
  [in] MYON::TextHorizontalAlignment h_align)
  Returns:
  true = Success
  false = Failure
  Remarks:
  The runs in the text are modified to store the location info for positioning
  within the text object
  */
  static bool MeasureTextRunArray(
    MYON_TextRunArray* runs,
    MYON::TextVerticalAlignment v_align,
    MYON::TextHorizontalAlignment h_align);

  /*
  Description:
    Calculates the size, spacing and position of the MYON_TextRun within an MYON_TextContent object
  Parameters:
    [in/out] MYON_TextRun& run  - Run to measure. Modified to store results
  Returns:
    true = Success
    false = Failure
  Remarks:
    The runs in the text are modified to store the location info for positioning
    within the text object
  */
  static bool MeasureTextRun(MYON_TextRun* run);
  static bool CreateStackedText(MYON_TextRun* run);
  static bool CreateStackedText(
    MYON_TextRun* run,
    int cpcount,
    const MYON__UINT32* cp,
    MYON__UINT32 stack_delimiter = L'/');
  
  /*
      Replaces runs[i] with stacked runs if any 
      "[[xx/xx]]" strings are found in wstr
      Returns the number of runs added to the array
  */
  static int FindAndStackFractions(MYON_TextRunArray* runs, int i, MYON_wString wstr);

  /*
  Description:
  Evaluates the field instructions in the run and puts the results
  in run->m_display_string.
  Parameters:
  [in/out] MYON_TextRun& run  - Run to evsluste. Modified to store results
  Returns:
  true = Success
  false = Failure
  Remarks:
  The runs in the text are modified to store the string result of evaluating any fields
  in the run m_string or m_codepoints
  */
  static bool EvaluateField(MYON_TextRun* run);

  /*
  Description:
    Returns the height in model units of the run, including text height and inter-line spacing
  */
  static double GetLinefeedHeight(MYON_TextRun& run);


  const MYON_Font* FirstCharFont() const;

  // Get the exact strings in the RTF font table and add them to font_table[]
  // Example
  // Input  rich_text = {\rtf1\deff0{\fonttbl{\f0 ArialMT;}{\f1 SegoeUI;}}\f0 \fs40{\f1 This is Segoe UI}}
  // returns font_table[] = { "ArialMT", "SegoeUI"}
  static bool GetRichTextFontTable(
    const MYON_wString rich_text,
    MYON_ClassArray< MYON_wString >& font_table
  );

  // Dimension text formatting
  static bool FormatDistance(
    double distance,
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    bool alternate,                     // Primary or alternate
    MYON_wString& formatted_string);      // Output

  static bool FormatTolerance(
    double distance,
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    bool alternate,                     // Primary or alternate
    MYON_wString& formatted_string);      // Output

  static bool FormatDistanceAndTolerance(
    double distance,
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    bool alternate,                     // Primary or alternate
    MYON_wString& formatted_string);      // Output

  static bool FormatDistanceMeasurement(
    double distance_in,
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    const wchar_t* user_text,           // Replace "<>" in user_text with formatted dimension
    MYON_wString& formatted_string);      // Output

  static bool FormatAngleMeasurement(
    double angle,
    const MYON_DimStyle* dimstyle,  // Angle format comes from dimstyle
    const wchar_t* user_text,
    MYON_wString& formatted_string);

  static bool FormatLength(
    double distance,
    MYON_DimStyle::LengthDisplay output_lengthdisplay,
    double round_off,
    int resolution,
    MYON_DimStyle::suppress_zero zero_suppress,
    bool bracket_fractions,
    wchar_t decimal_char,
    MYON_wString& output);

  static bool FormatAngleStringDMS(
    double angle_degrees,
    wchar_t decimal_char,
    MYON_wString& formatted_string);

  static bool FormatAngleStringDMS(
    double angle_degrees,
    wchar_t decimal_char,
    int resolution,
    MYON_wString& formatted_string);

  static bool FormatAngleStringDecimal(
    double angle_radians,
    int resolution,
    double roundoff,
    MYON_DimStyle::suppress_zero zero_suppression,
    wchar_t decimal_char,
    MYON_wString& formatted_string);

  // Area formatting
  static bool FormatArea(
    double area,
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    bool alternate,                     // Primary or alternate
    MYON_wString& formatted_string);      // Output

  // Volume formatting
  static bool FormatVolume(
    double volume,
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    bool alternate,                     // Primary or alternate
    MYON_wString& formatted_string);      // Output

  private:
  static bool FormatAreaOrVolume(
    double area_or_volume,
    bool format_area,
    MYON::LengthUnitSystem units_in,
    const MYON_DimStyle* dimstyle,
    bool alternate,                     // Primary or alternate
    MYON_wString& formatted_string);      // Output


};


#endif

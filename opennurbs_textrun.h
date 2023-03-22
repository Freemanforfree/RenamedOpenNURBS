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

#ifndef OPENNURBS_TEXTRUN_H_INCLUDED
#define OPENNURBS_TEXTRUN_H_INCLUDED

class MYON_CLASS MYON_StackedText
{
public:
  static const MYON_StackedText Empty;

public:
  MYON_StackedText() = default;
  ~MYON_StackedText();
  MYON_StackedText(const MYON_StackedText& src);

  // Sets m_parent_run = nullptr.
  // You must set m_parent_run after calling operator=().
  MYON_StackedText& operator=(const MYON_StackedText& src);

  class MYON_TextRun*  m_top_run = nullptr;
  class MYON_TextRun*  m_bottom_run = nullptr;
  const MYON_TextRun*  m_parent_run = nullptr;
  wchar_t            m_separator = (wchar_t)MYON_UnicodeCodePoint::MYON_Slash;

  enum class StackStyle : unsigned char
  {
    kUnset = 0,
    kHorizontalToScreen = 1,
    kSlanted    = 2,
  };

  static MYON_StackedText::StackStyle StackStyleFromUnsigned(
    unsigned int stack_style_as_unsigned
    );

private:
  friend class MYON_TextRun;

  //bool WriteNested(
  //  unsigned int nested_depth,
  //  MYON_BinaryArchive& archive
  //  ) const;

  //bool ReadNested(
  //  unsigned int nested_depth,
  //  MYON_BinaryArchive& archive
  //  );
};

// A range of text with all the same attributes
class MYON_CLASS MYON_TextRun
{
public:
  static const MYON_TextRun Empty;

  /*
  Description:
    MYON_TextRun::NewTextRun() gets a text run from an efficiently managed pool.
  Returns:
    A pointer to a text run. (never nullptr).
  */
  static MYON_TextRun* GetManagedTextRun();

  
  /*
  Description:
    MYON_TextRun::NewTextRun() gets a text run from an efficiently managed pool
    and copies src
  Returns:
    A pointer to a text run. (never nullptr).
  */
  static MYON_TextRun* GetManagedTextRun(
    const MYON_TextRun& src
    );

  /*
  Description:
    Return a managed MYON_TextRun.
  */
  static bool ReturnManagedTextRun(
    MYON_TextRun* managed_text_run
    );

  /*
  Returns:
    True if the memory for this MYON_TextRun is managed.
    It was created by calling MYON_TextRun::GetManagedTextRun(). If it is active,
    then is must be deleted by calling MYON_TextRun::ReturnManagedTextRun();
  */
  bool IsManagedTextRun() const;

  /*
  Returns:
    True if the memory for this MYON_TextRun is managed and the text run is active.
    It was created by calling MYON_TextRun::GetManagedTextRun() and should be
    deleted by calling MYON_TextRun::ReturnManagedTextRun();
  */
  bool IsActiveManagedTextRun() const;

public:
  MYON_TextRun() = default;
  ~MYON_TextRun();
  MYON_TextRun(const MYON_TextRun& src);
  MYON_TextRun& operator=(const MYON_TextRun& src);

private:
  MYON_TextRun(bool bManagedTextRun);
    
public:
  enum class RunType : unsigned char
  {
    kNone        = 0,
    kText        = 1,
    kNewline     = 2,
    kSoftreturn  = 3,
    kParagraph   = 4,
    kColumn      = 5,
    kField       = 6,
    kFieldValue  = 7,
    kFontdef     = 8,
    kHeader      = 9,
    kFonttbl     = 10,
    kColortbl    = 11,
  };

  static MYON_TextRun::RunType RunTypeFromUnsigned(
    unsigned int run_type_as_unsigned
    );

  enum class Stacked : unsigned char
  {
    kNone     = 0,
    kStacked  = 1,
    kTop      = 2,
    kBottom   = 3
  };

  static MYON_TextRun::Stacked StackedFromUnsigned(
    unsigned int stacked_as_unsigned
    );

  enum class RunDirection : unsigned char
  {
    kLtR      = 0,
    kRtL      = 1,
  };
  
  static MYON_TextRun::RunDirection RunDirectionFromUnsigned(
    unsigned int run_direction_as_unsigned
    );

public:

  MYON_SHA1_Hash TextRunContentHash() const;
  MYON_SHA1_Hash TextRunContentHash(
  bool bEvaluateFields
  ) const;


  void Init(
    const class MYON_Font* managed_font,
    double height,
    double stackscale,
    MYON_Color color,
    bool bold,
    bool italic,
    bool underlined,
    bool strikethrough,
    bool deletedisplay = true);

  bool IsText() const;
  bool IsNewline() const;
  bool IsColumn() const;
  bool IsValid() const;
  
  RunType Type() const;
  void SetType(MYON_TextRun::RunType);
  RunDirection FlowDirection() const;

  Stacked IsStacked() const;
  void SetStacked(Stacked stacked);
  void SetStackedOff();

  // Set or get the WCS model unit height of the text
  // not including any annotatition scaling
  double TextHeight() const;

  void SetTextHeight(double h);

  MYON_Color Color() const;
  void SetColor(MYON_Color color);

  void SetFont(const MYON_Font* font);
  const MYON_Font* Font() const;
  
  // bbox is stored as MYON_BoundingBox, but is always 2d. z=0
  const MYON_BoundingBox& BoundingBox() const;
  void SetBoundingBox(MYON_2dPoint pmin, MYON_2dPoint pmax);

  const MYON_2dVector& Offset() const;
  void SetOffset(MYON_2dVector offset);

  const MYON_2dVector& Advance() const;
  void SetAdvance(MYON_2dVector advance);

  // This returns the scale of m_height / HeightOfI.
  // It doesn't take into account anything about annotation scaling
  // This is the scale for converting MYON_TextRun bounding boxes and 
  // offsets to basic model units
  double HeightScale(const MYON_Font* font) const;

  void SetStackFractionHeight(double stackscale);
  double StackHeightFraction() const;
  static double DefaultStackFractionHeight();

  //bool Write(
  //  MYON_BinaryArchive&
  //  ) const;
  //bool Read(
  //  MYON_BinaryArchive&
  //  );

  void Get2dCorners(MYON_2dPoint corners[4]) const;

private:
  static
  void SetUnicodeString(MYON__UINT32*& dest, size_t count, const MYON__UINT32* cp);

public:
  void SetUnicodeString(size_t count, const MYON__UINT32* cp);
  static size_t CodepointCount(const MYON__UINT32* cp);

  // Returns string in m_display_string, which may be the result of parsing text fields
  void SetDisplayString(const wchar_t* str);

  // The display string is the TextString() with formulae evaluated.
  const wchar_t* DisplayString() const; 

  // Returns the string in m_text_string, which is a wchar_t version of the basic text for this run,
  // and may contain unevaluated field formulae
  const wchar_t* TextString() const;

  const MYON__UINT32* UnicodeString() const;

  bool GetGlyphContours(
    const MYON_Font* text_font,
    bool bSingleStrokeFont,
    const MYON_Xform& text_xform,
    MYON_ClassArray< MYON_ClassArray< MYON_SimpleArray< MYON_Curve* > > >& run_contours
  ) const;

  friend class MYON_TextBuilder;

  // Wrap text to a specified width in model space
  int WrapTextRun(
    int call_count,
    int start_char_offset,
    double width,
    double &y_offset,
    double& currentwidth,
    class MYON_TextRunArray& newruns) const;

  const MYON_Font*      m_managed_font = nullptr;   // Font used to draw, pick, or otherwise evaluate this
                                                  // MYON_TextRun.  This pointer is run-time only and must be 
                                                  // set and point to a valid MYON_Font for any font related 
                                                  // operations to work.

//  MYON_UUID              m_Font_id = MYON_nil_uuid;
private:
  MYON__UINT32*          m_codepoints = nullptr;     // Parsed text as UTF32
  
  // Set from m_codepoints when required.
  // Always use TextString() to get this value
  mutable MYON_wString   m_text_string = MYON_wString::EmptyString; 
                                                   // Parsed text as wchar_t*
                                                   // If this string is non-empty, it is valid and can be used 
                                                   // instead of m_codepoints
                                                   // This string may have unevaluated field definitions - %<field_name>%
  
  mutable MYON_wString   m_display_string = MYON_wString::EmptyString;  
                                                   // Text to display when this is drawn. 
                                                   // If this text has field definitions, m_display_string will have the 
                                                   // evaluation results to display
                                                   // This string may change often if there are fields to evaluate.

  mutable MYON_SHA1_Hash m_text_run_hash = MYON_SHA1_Hash::ZeroDigest;
  mutable MYON_SHA1_Hash m_text_run_display_hash = MYON_SHA1_Hash::ZeroDigest;

  MYON_TextRun::Stacked  m_text_stacked = MYON_TextRun::Stacked::kNone; // 0: Normal text, 1: Run is stacked container, 2: Run is top of stacked fraction, 3: Run is bottom of stacked fraction

private:
  void Internal_ContentChanged() const;
public:
  MYON_StackedText*      m_stacked_text = nullptr;    // pointers to runs for the top and bottom parts

  MYON_Color             m_color = MYON_Color::UnsetColor;
private:
  MYON_TextRun::RunType      m_run_type = MYON_TextRun::RunType::kNone;
  MYON_TextRun::RunDirection m_direction = MYON_TextRun::RunDirection::kLtR;

private:

  // This value of m_managed_status is not copied
  // 0: Not managed
  // 1: Managed by GetManagedTextRun() / ReturnManagedTextRun()
  const unsigned char m_managed_status = 0; 
  unsigned char m_active_status = 0; // 0: active, 1: inactive managed text_run

private:
  double               m_run_text_height = 1.0;             // (ECS) text height in model units or page units

public:
  MYON_2dVector          m_offset = MYON_2dVector::ZeroVector;  // (ECS) offset to lower left of bounding box from MYON_TextContent plane origin

  MYON_2dVector          m_advance = MYON_2dVector::ZeroVector; // (ECS) distance and direction from m_offset to start of next run

private:
  MYON_BoundingBox       m_bbox = MYON_BoundingBox::EmptyBoundingBox; // (ECS) 3d Bounding box oriented to MYON_TextContent object's plane (z == 0) with run offset already included

public:
  double               m_height_scale = -1.0;      // Font HeightOfI / text height - Converts from font units to model units or page units
  double               m_stackscale = 0.7;         // fraction for scaling textheight in stacked text

  // indent and margins are in model units or page units
                                                   // These apply to Paragraph type runs (m_type == rtParagraph)
  double               m_indent = 0.0;             // First line indentation for this paragraph
  double               m_left_margin = 0.0;        // left margin in formatting rect for this paragraph
  double               m_right_margin = 0.0;       // right margin in formatting rect for this paragraph

  int                  m_line_index = -1;          // line position in MYON_TextContent

private:
  MYON__UINT_PTR m_reserved=0;

private:
  friend class MYON_StackedText;
  //bool WriteNested(
  //  unsigned int nested_depth,
  //  MYON_BinaryArchive& archive
  //  ) const;
  //bool ReadNested(
  //  unsigned int nested_depth,
  //  MYON_BinaryArchive& archive
  //  );
private:
  void Internal_Destroy();
  void Internal_CopyFrom(const MYON_TextRun& src);
};


class MYON_CLASS MYON_TextRunArray : private MYON_SimpleArray< MYON_TextRun* >
{
  //MYON_TextRun*& AppendNew();
  //void Append(int, MYON_TextRun* const *);
  //void Append(MYON_TextRun* const);

public:
  static const MYON_TextRunArray EmptyArray;

  MYON_TextRunArray() = default;

  // Calls Destroy(true,true)
  ~MYON_TextRunArray();

  // Duplicate runs are managed text runs
  MYON_TextRunArray& operator=(const MYON_TextRunArray& src);

  // Duplicate runs are managed text runs
  MYON_TextRunArray(const MYON_TextRunArray& src);

public:

  /*
  Returns:
    A hash of the information that determines the text content with evaluated fields.
  */
  MYON_SHA1_Hash TextRunArrayContentHash() const;

  /*
  Parameters:
    bEvaluateFields - [in]
      true - hash text with fields evaluated
      false - hash text with fields unevaluated
  Returns:
    A hash of the information that determines the text content
    without evaluating the fields.
  */
  MYON_SHA1_Hash TextRunArrayContentHash(
    bool bEvaluateFields
  ) const;

  // Run must be a managed run or on the heap.
  // The destructor will return managed runs and delete unmanaged runs.
  void InsertRun(int i, MYON_TextRun*& run);

  void RemoveRun(int i);

  // Run must be a managed run or on the heap.
  // The destructor will return managed runs and delete unmanaged runs.
  void AppendRun(MYON_TextRun*& run);

  bool Get2dCorners(MYON_2dPoint corners[4]) const;

  const MYON_TextRun* operator[](int i) const;
  MYON_TextRun* operator[](int i);
  int Count() const;
  unsigned int UnsignedCount() const;
  MYON_TextRun*const* Array() const;

  /*
  Parameters:
    bReturnManagedRuns - [in]
      True: Managed runs will be returned.
      False: Caller must explicitly handle managed runs.
    bDeleteUnmanagedRuns - [in]
      True: Unmanaged runs are deleted.
      False: Caller must explicitly handle unmanaged runs.
  */
  void SetTextHeight(double height);

private:
  void Internal_Destroy();
  void Internal_CopyFrom(const MYON_TextRunArray& src);
};


#endif

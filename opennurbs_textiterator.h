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

#if !defined(OPENNURBS_TEXTITERATOR_INC_)
#define OPENNURBS_TEXTITERATOR_INC_

#define RTFFIRSTCHAR

typedef struct tagFontKey 
{
  int            rtf_font_index;
  const MYON_Font* managed_font;
} MYON_FontKey;

class MYON_FaceNameKey
{
public:
  int m_rtf_font_index = -1;

  // In reality, the RTF "face" name can be
  // a PostScript name, LOGFONT.lfFaceName, DWrite Family Name,
  // and probably more. It depends on what tool is used to create the RTF and we see
  // all three of the above names in Rhino parsing and the current Eto Text controls
  // generate RTF with all three of those "names" OS version.
  MYON_wString m_rtf_font_name;

  unsigned int m_codepage = 1252;
  unsigned int m_charset = 0;
};

// Converts wchar_t characters to Unicode codepoints 
class MYON_TextIterator
{
private:
  MYON_TextIterator() = delete;

public:
  MYON_TextIterator(const MYON_wString& str);
  MYON_TextIterator(const wchar_t* str, size_t length);

public:
  ~MYON_TextIterator() = default;
  MYON_TextIterator(const MYON_TextIterator&) = default;
  MYON_TextIterator& operator=(const MYON_TextIterator&) = default;

  /*
  Parameters:
    unicode_code_point - [out]
      current unicode code point returned here.
      0 = end of string
  Returns:
     true if returned unicode_code_point is not zero
  */
  bool PeekCodePoint(MYON__UINT32& unicode_code_point) const;

  /*
  Description:
    Gets the current unicode code point and calls Step() to advance the text iterator
    by one code point.
  Parameters:
    unicode_code_point - [out]
      current unicode code point returned here.
      0 = end of string
  Returns:
     true if returned unicode_code_point is not zero
  */
  bool ReadCodePoint(MYON__UINT32& unicode_code_point);


  bool        Back();  // move position back and return current codepoint after moving back

  // Get the next UNICODE code point encoded in m_text beginning at m_text[m_next_text_ci];
  // Save this code point in m_cur_codepoint.
  // Advance m_next_text_ci.
  bool        Step();

  bool AtBackslashTic() const;
  bool ReadCharValue(
    unsigned char& c
  );
private:

  const wchar_t*                    m_text = nullptr;
  size_t                            m_length = 0;
  size_t                            m_prev_text_ci = 0;   // previous offset in m_text wchar_t string
  size_t                            m_next_text_ci = 0;   // previous offset in m_text wchar_t string
  size_t                            m_cur_text_ci = 0;    // current offset in m_text wchar_t string
  MYON__UINT32                        m_prev_codepoint = 0; // previous UNICODE code point
  MYON__UINT32                        m_cur_codepoint = 0;  // UNICODE code point read by last call to Step()
  struct MYON_UnicodeErrorParameters  m_ue = MYON_UnicodeErrorParameters::MaskErrors;
};

class MYON_TextBuilder
{
public:
  MYON_TextBuilder();

  virtual ~MYON_TextBuilder();
  
  class TextProps
  {
  public:
    TextProps()
    {}
    TextProps(
      double height,
      double stackscale,
      MYON_Color color,
      MYON_DimStyle::stack_format stackformat,
      bool bold,
      bool italic,
      bool underlined,
      bool strikethrough,
      unsigned int charset)
      : m_height(height)
      , m_stackscale(stackscale)
      , m_color(color)
      , m_stackformat(stackformat)
      , m_bold(bold)
      , m_italic(italic)
      , m_underlined(underlined)
      , m_strikethrough(strikethrough)
      , m_codepage(1252)
    {}
    double Height() const
    {
      return m_height;
    }
    void SetHeight(double h)
    {
      if (h > 1e-8)
        m_height = h;
    }
    double StackScale() const
    {
      return m_stackscale;
    }
    void SetStackScale(double s)
    {
      if (0.0 < s && 10.0 > s)
        m_stackscale = s;
    }
    MYON_Color Color() const
    {
      return m_color;
    }
    void SetColor(MYON_Color c)
    {
      m_color = c;
    }
    MYON_DimStyle::stack_format StackFormat() const
    {
      return m_stackformat;
    }
    void SetStackFormat(MYON_DimStyle::stack_format s)
    {
      m_stackformat = s;
    }
    bool IsBold()
    {
      return m_bold;
    }
    void SetBold(bool bold)
    {
      m_bold = bold;
    }
    bool IsItalic()
    {
      return m_italic;
    }
    void SetItalic(bool italic)
    {
      m_italic = italic;
    }
    bool IsUnderlined()
    {
      return m_underlined;
    }
    void SetUnderlined(bool underlined)
    {
      m_underlined = underlined;
    }
    bool IsStrikethrough()
    {
      return m_strikethrough;
    }
    void SetStrikethrough(bool strikethrough)
    {
      m_strikethrough = strikethrough;
    }
    unsigned int CodePage()
    {
      return m_codepage;
    }
    void SetCodePage(unsigned int codepage)
    {
      m_codepage = codepage;
    }

    unsigned int CharSet()
    {
      return m_charset;
    }
    void SetCharSet(unsigned int charset, bool setcodepage)
    {
      m_charset = charset;
      if (setcodepage)
      {
        m_codepage = MYON_MapRTFcharsetToWindowsCodePage(charset, 1252);
      }
    }
    bool FormatPending()
    {
      return m_format_pending;
    }
    void SetFormatPending(bool b)
    {
      m_format_pending = b;
    }

  private:
    double                    m_height = 1.0;
    double                    m_stackscale = 0.7;
    MYON_Color                  m_color = MYON_Color::Black;
    MYON_DimStyle::stack_format m_stackformat = MYON_DimStyle::stack_format::StackHorizontal;
    bool                      m_bold = false;
    bool                      m_italic = false;
    bool                      m_underlined = false;
    bool                      m_strikethrough = false;
    unsigned int              m_codepage = 1252;
    unsigned int              m_charset = 0;   // Charset isn't really needed but is here to make debugging a little easier
    bool                      m_format_pending = false;
  };

  MYON_ClassArray< TextProps >  m_prop_stack;
  TextProps                   m_current_props;
  TextProps                   m_pending_props;

  // Rtf uses UTF-16 encoding and surrogate pairs need to be properly handled.
  // For example, the single UNICODE code point MYON_UnicodeCodePoint::Wastebasket U+1F5D1 (decimal 128465)
  // is in the RTF string as ...{\ltrch \u-10179?\u-8751?}...
  // The UNICODE code point U+1F5D1 is encoded as a UTF-16 surrogate pair is (0xD83D, 0xDDD1).
  //   \u-10179? -> unsigned short 0xD83D
  //   \u-8751?  -> unsigned short 0xDDD1
  enum : MYON__UINT16
  {
    m_UFT16_waiting_mark = 0xEEEE, // value must be > 0xE000 and uncommon unicode code point
    m_UFT16_unused_mark = 0xFFFF  // value must be > m_UFT16_waiting, <= 0xFFFF, and uncommon unicode code point
  };
  MYON__INT32 m_current_UTF16_buffer_count = 0;
  MYON__UINT16 m_current_UTF16_buffer[2];

  MYON_SimpleArray< MYON__UINT32 >       m_current_codepoints;
  MYON__INT32                          m_in_run;
  MYON__INT32                          m_level = 0;
  MYON__INT32                          m_font_table_level = 10000;
  MYON__INT32                          m_font_index = 0;
  MYON__INT32                          m_default_font_index = 0;
private:
  bool                               m_bReadingFontDefinition = false;
public:
  MYON_ClassArray< MYON_FaceNameKey >    m_facename_map;

  virtual void InitBuilder(const MYON_Font* default_font);
  virtual void FlushText(size_t count, MYON__UINT32* cp_array);
  virtual void GroupBegin();
  virtual void GroupEnd();
  virtual void RunBegin();
  virtual void RunEnd();
  virtual void FinishFontDef();
  virtual bool ReadingFontTable();
  virtual bool ReadingFontDefinition();
  virtual void SetReadingFontDefinition(bool b);


  virtual void BeginHeader();
  virtual void BeginFontTable();
  virtual void DefaultFont(const wchar_t* value);
  virtual void FontTag(const wchar_t* value);
  virtual void FontSize(const wchar_t* value);
  virtual void CharSet(const wchar_t* value);
  virtual void CodePage(const wchar_t* value);

  virtual void Newline();
  virtual void Paragraph();
  virtual void ParagraphDefaults();
  virtual void Section();
  virtual void Tab();

  virtual void Bold(const wchar_t* value);
  virtual void Italic(const wchar_t* value);
  virtual void UnderlineOn();
  virtual void UnderlineOff();
  virtual void Strikethrough(const wchar_t* value);
  
  virtual void Superscript();
  virtual void Subscript();
  virtual void NoSuperSub();

  virtual void BeginColorTable();
  virtual void ColorRed(const wchar_t* value);
  virtual void ColorGreen(const wchar_t* value);
  virtual void ColorBlue(const wchar_t* value);
  virtual void ColorForeground(const wchar_t* value);
  virtual void ColorBackground(const wchar_t* value);

  virtual void SetStackScale(const wchar_t* value);
  virtual void StackFraction(const wchar_t* value);
  virtual void StackEnd();

  virtual void TextField(const wchar_t* name);

  virtual void UniEmbeddedDest(const wchar_t* value);
  virtual void UniDest(const wchar_t* value);

  virtual void UniCpCount(const wchar_t* value);
  virtual void UniDecimal(const wchar_t* value);

  virtual void LQuote();
  virtual void RQuote();
  virtual void LDblQuote();
  virtual void RDblQuote();
  virtual void Bullet();
  virtual void EnDash();
  virtual void EmDash();

  virtual bool AppendCodePoint(MYON__UINT32 codept);
  virtual void FormatChange();

  MYON__UINT32* RunCodePoints(const MYON_TextRun& run);

  const MYON_wString FaceNameFromMap(int nval);
  unsigned int CodePageFromMap(int nval);
  unsigned int CharSetFromMap(int nval);

private:
  MYON_TextBuilder operator=(const MYON_TextBuilder& src);
};



class MYON_TextRunBuilder : public MYON_TextBuilder
{
public:
  MYON_TextRunBuilder(
    MYON_TextContent& text,
    MYON_TextRunArray& runs,
    const MYON_DimStyle* dimstyle,
    double height,
    MYON_Color color);

  virtual ~MYON_TextRunBuilder();

  MYON_SimpleArray< const MYON_Font* >   m_font_stack;

  const MYON_Font* CurrentFont() const
  {
    // Keep this as a simple one-liner that unconditionally returns m_private_current_font.
    return m_private_current_font;
  }

  void SetCurrentFont(const MYON_Font* font)
  {
    // Keep this as a simple one-liner that unconditionally sets m_private_current_font.
    m_private_current_font = font;
  }

private:
  // Please, in order to keep people debugging sane, always use 
  // CurrentFont() / SetCurrentFont() and never directly
  // set m_private_current_font.
  const MYON_Font*                     m_private_current_font = &MYON_Font::Default;

public:
  MYON_TextRun                         m_current_run;
  MYON_TextRunArray&                   m_runs;
  MYON_TextContent&                    m_text;

  void FinishCurrentRun();
  void AppendCurrentRun();

  void InitBuilder(const MYON_Font* default_font) override;
  void FlushText(size_t count, MYON__UINT32* cp_array) override;
  void GroupBegin() override;
  void GroupEnd() override;
  void RunBegin() override;
  void RunEnd() override;
  void FinishFontDef() override;
  bool AppendCodePoint(MYON__UINT32 codept) override;
  void FormatChange() override;

  void BeginHeader() override;
  void BeginFontTable() override;
  void DefaultFont(const wchar_t* value) override;
  void FontTag(const wchar_t* value) override;
  void FontSize(const wchar_t* value) override;

  void Newline() override;
  void Paragraph() override;
  void ParagraphDefaults() override;
  void Section() override;
  void Tab() override;

  void Bold(const wchar_t* value) override;
  void Italic(const wchar_t* value) override;
  void UnderlineOn() override;
  void UnderlineOff() override;
  void Strikethrough(const wchar_t* value) override;

  void Superscript() override;
  void Subscript() override;
  void NoSuperSub() override;

  void BeginColorTable() override;
  void ColorRed(const wchar_t* value) override;
  void ColorGreen(const wchar_t* value) override;
  void ColorBlue(const wchar_t* value) override;
  void ColorForeground(const wchar_t* value) override;
  void ColorBackground(const wchar_t* value) override;

  void SetStackScale(const wchar_t* value) override;
  void StackFraction(const wchar_t* value) override;
  void StackEnd() override;

  void TextField(const wchar_t* name) override;

  void UniEmbeddedDest(const wchar_t* value) override;
  void UniDest(const wchar_t* value) override;

private:
  MYON_TextRunBuilder operator=(const MYON_TextRunBuilder& src);
};

class MYON_RtfStringBuilder : public MYON_TextBuilder
{
public:
  MYON_RtfStringBuilder(
    const MYON_DimStyle* dimstyle,
    double height,
    MYON_Color color);
  
  virtual ~MYON_RtfStringBuilder();

  class TextRun
  {
  public: 
    TextRun() {}

    MYON_TextRun::RunType Type() const
    {
      return m_run_type;
    }
    void SetType(MYON_TextRun::RunType type)
    {
      m_run_type = type;
    }
    void InitRun()
    {
      m_run_type = MYON_TextRun::RunType::kNone;
      m_font_index = -1;
      m_text.Empty();
      m_bold = false;
      m_italic = false;
      m_underlined = false;
      m_strikethrough = false;
    }
    int FontIndex()
    {
      return m_font_index;
    }
    void SetFontIndex(int index) 
    {
      if(index >= -1)
        m_font_index = index;
    }

    bool IsBold() const
    {
      return m_bold;
    }
    bool IsItalic() const
    {
      return m_italic;
    }
    bool IsUnderlined() const
    {
      return m_underlined;
    }
    bool IsStrikeThrough() const
    {
      return m_strikethrough;
    }
    void SetBold(bool b)
    {
      m_bold = b;
    }
    void SetItalic(bool b)
    {
      m_italic = b;
    }
    void SetUnderlined(bool b)
    {
      m_underlined = b;
    }
    void SetStrikeThrough(bool b)
    {
      m_strikethrough = b;
    }

    void AddControl(const wchar_t* str)
    {
      m_text += str;
      size_t i = wcslen(str);
      if(str[i-1] == L' ' || str[i-1] == L'{' || str[i-1] == L'}')
        m_terminated = true;
      else
        m_terminated = false;
      m_has_content = true;
    }

    void AddText(const wchar_t* str)
    {
      if (!m_terminated)
        m_text += L' ';
      m_terminated = true;
      m_text += str;
      m_has_content = true;
    }

    void AddChar(const wchar_t ch)
    {
      if (!m_terminated)
        m_text += L' ';
      m_terminated = true;
      m_text += ch;
      m_has_content = true;
    }

    void SetTerminated(bool terminated)
    {
      m_terminated = terminated;
    }

    bool IsTerminated()
    {
      return m_terminated;
    }

    void EmptyText()
    {
      m_text = MYON_wString::EmptyString;
    }

    const MYON_wString& TextString()
    {
      return m_text;
    }

    const bool HasContent()
    {
      return m_has_content;
    }

    const void ClearHasContent()
    {
      m_has_content = false;
    }
    
  private:
    bool                   m_has_content = false;
    bool                   m_terminated = true;
    MYON_wString             m_text;
    bool                   m_bold = false;
    bool                   m_italic = false;
    bool                   m_underlined = false;
    bool                   m_strikethrough = false;
    int                    m_font_index = -1;
    MYON_TextRun::RunType    m_run_type = MYON_TextRun::RunType::kNone;
  };

private:        
  MYON_wString    m_string_out;
  bool          m_in_font_table = false;
  bool          m_in_color_table = false;

  bool          m_skip_color_tbl = false;
  bool          m_skip_bold = false;
  bool          m_skip_italic = false;
  bool          m_skip_underline = false;
  bool          m_skip_facename = false;

  bool          m_make_bold = false;
  bool          m_make_italic = false;
  bool          m_make_underline = false;
  bool          m_make_facename = false;

  MYON_wString    m_default_facename;
  MYON_wString    m_override_facename;

  bool          m_have_rtf = false;

public:

  TextRun                            m_current_run;
  MYON_ClassArray< TextRun >           m_run_stack;

  void InitStringBuilder(const MYON_DimStyle* default_style);
  const MYON_wString OutputString();
  void PushRun(TextRun& run);
  TextRun PopRun();
  
  //bool InFontTable();
  //void SetInFontTable(bool b);
  bool InColorTable();
  void SetInColorTable(bool b);

  void SetSkipColorTbl(bool b);
  void SetSkipBold(bool b);
  void SetSkipItalic(bool b);
  void SetSkipUnderline(bool b);
  void SetSkipFacename(bool b);

  bool SkipColorTbl();
  bool SkipBold();
  bool SkipItalic();
  bool SkipUnderline();
  bool SkipFacename();

  void SetMakeBold(bool b);
  void SetMakeItalic(bool b);
  void SetMakeUnderline(bool b);
  void SetMakeFacename(bool b);

  bool MakeBold();
  bool MakeItalic();
  bool MakeUnderline();
  bool MakeFacename();

  bool SkippingFacename();
  bool SettingFacename();

  void SetDefaultFacename(const wchar_t* facename);
  void SetOverrideFacename(const wchar_t* facename);

  // virtuals
  
  void GroupBegin() override;
  void GroupEnd() override;
  void RunBegin() override;
  void RunEnd() override;

  void BeginHeader() override;
  void BeginFontTable() override;
  void DefaultFont(const wchar_t* value) override;
  void FontTag(const wchar_t* value) override;
  void FontSize(const wchar_t* value) override;

  void Newline() override;
  void Paragraph() override;
  void ParagraphDefaults() override;
  void Section() override;
  void Tab() override;

  void Bold(const wchar_t* value) override;
  void Italic(const wchar_t* value) override;
  void UnderlineOn() override;
  void UnderlineOff() override;
  void Strikethrough(const wchar_t* value) override;

  void Superscript() override;
  void Subscript() override;
  void NoSuperSub() override;

  void BeginColorTable() override;
  void ColorRed(const wchar_t* value) override;
  void ColorGreen(const wchar_t* value) override;
  void ColorBlue(const wchar_t* value) override;
  void ColorForeground(const wchar_t* value) override;
  void ColorBackground(const wchar_t* value) override;

  void TextField(const wchar_t* name) override;

  bool AppendCodePoint(MYON__UINT32 codept) override;

  void UniEmbeddedDest(const wchar_t* value) override;
  void UniDecimal(const wchar_t* value) override;
  void UniDest(const wchar_t* value) override;

private:
  MYON_RtfStringBuilder operator=(const MYON_RtfStringBuilder& src);
};

#ifdef RTFFIRSTCHAR

class MYON_RtfFirstChar : public MYON_TextBuilder
{
public:
  MYON_RtfFirstChar(
    const MYON_DimStyle* dimstyle,
    double height,
    MYON_Color color);

  virtual ~MYON_RtfFirstChar();

  class TextRun
  {
  public:
    TextRun() {}

    MYON_TextRun::RunType Type() const
    {
      return m_run_type;
    }
    void SetType(MYON_TextRun::RunType type)
    {
      m_run_type = type;
    }
    void InitRun()
    {
      m_run_type = MYON_TextRun::RunType::kNone;
      m_font_index = -1;
      m_text.Empty();
      m_bold = false;
      m_italic = false;
      m_underlined = false;
      m_strikethrough = false;
    }
    int FontIndex()
    {
      return m_font_index;
    }
    void SetFontIndex(int index)
    {
      if (index >= -1)
        m_font_index = index;
    }

    bool IsBold() const
    {
      return m_bold;
    }
    bool IsItalic() const
    {
      return m_italic;
    }
    bool IsUnderlined() const
    {
      return m_underlined;
    }
    bool IsStrikeThrough() const
    {
      return m_strikethrough;
    }
    void SetBold(bool b)
    {
      m_bold = b;
    }
    void SetItalic(bool b)
    {
      m_italic = b;
    }
    void SetUnderlined(bool b)
    {
      m_underlined = b;
    }
    void SetStrikeThrough(bool b)
    {
      m_strikethrough = b;
    }

    void AddText(const wchar_t* str)
    {
      if (!m_terminated)
        m_text += L' ';
      m_terminated = true;
      m_text += str;
      m_has_content = true;
    }

    const MYON_wString& Text()
    {
      return m_text;
    }
    
  private:
    bool                   m_has_content = false;
    bool                   m_terminated = true;
    MYON_wString             m_text;
    bool                   m_bold = false;
    bool                   m_italic = false;
    bool                   m_underlined = false;
    bool                   m_strikethrough = false;
    int                    m_font_index = -1;
    MYON_TextRun::RunType    m_run_type = MYON_TextRun::RunType::kNone;
  };

private:
  bool          m_in_font_table = false;
  bool          m_in_color_table = false;

  bool          m_have_rtf = false;

public:

  TextRun                            m_current_run;
  MYON_ClassArray< TextRun >           m_run_stack;

  void InitStringBuilder(const MYON_DimStyle* default_style);
  const MYON_wString OutputString();
  void PushRun(TextRun& run);
  TextRun PopRun();

  //bool InFontTable();
  //void SetInFontTable(bool b);
  bool InColorTable();
  void SetInColorTable(bool b);
  
  // virtuals

  void GroupBegin() override;
  void GroupEnd() override;
  void RunBegin() override;
  void RunEnd() override;

  void BeginHeader() override;
  void BeginFontTable() override;
  void BeginColorTable() override;
  void TextField(const wchar_t* name) override;
  bool AppendCodePoint(MYON__UINT32 codept) override;

  void FontTag(const wchar_t* value) override;

  void Bold(const wchar_t* value) override;
  void Italic(const wchar_t* value) override;
  void UnderlineOn() override;
  void UnderlineOff() override;
  void Strikethrough(const wchar_t* value) override;

private:
  MYON_RtfFirstChar operator=(const MYON_RtfFirstChar& src);
};

#endif

class MYON_RtfParser
{
public:
  MYON_RtfParser(MYON_TextIterator& iter, MYON_TextBuilder& builder);
  bool Parse();

private:
  MYON__UINT32 Internal_ParseMBCSString(
    const MYON__UINT32 windows_code_page
    );

  MYON_TextIterator&   m_ti;

  MYON_TextBuilder&    m_builder;
  int                m_p_level = 0;
  bool               m_in_real_rtf = false;
  // suspend to close means ignore everything until the current group is closed
  // It's used to skip over commands and parameters in optional tags
  int                m_suspend_to_close = 0;

  bool FlushCurText(MYON_SimpleArray< MYON__UINT32 >& cp_array);
  bool ReadTag(bool optional);
  bool ReadOptionalTag();
  bool ProcessTag(const wchar_t* name, const wchar_t* value, bool optional);

  MYON_RtfParser operator=(const MYON_RtfParser& src);
};

class MYON_CLASS RtfComposer
{
public: 
  class RunInfo
  {
  public:
    MYON_TextRun::RunType m_run_type = MYON_TextRun::RunType::kNone;
    MYON_TextRun* m_text_run = nullptr;
    MYON_wString m_run_text;
    bool m_bold = false;
    bool m_italic = false;
    bool m_underline = false;
    bool m_strikeout = false;
    MYON_wString m_facename = L"Arial";
    int m_facename_key = -1;
  };

  static bool Compose(
    const MYON_TextContent* text,
    MYON_wString& rtf,
    bool bForceRtf);

  static const MYON_wString ComposeAppleRTF(
    const MYON_TextContent* text);

  static bool RecomposeRTF();
  static void SetRecomposeRTF(bool b);

  static bool ComposeFS();
  static void SetComposeFS(bool b);
  static int TextEditorFontSize();
  static void SetTextEditorFontSize(unsigned int size);


private:
  static bool m_bComposeRTF;
  static bool m_bComposeFS;
  static int m_TextEditorSize;

  RtfComposer();

  static unsigned int GetFacenameKey(const MYON_Font* font, MYON_ClassArray< MYON_wString >& fonttable);
  static unsigned int GetFacenameKey(const wchar_t* facename, MYON_ClassArray< MYON_wString >& fonttable);
  static unsigned int GetColorKey(MYON_Color color, MYON_SimpleArray< unsigned int >& colortable);
  static bool FormatTextHeight(double height, MYON_wString& str);
};


#endif 


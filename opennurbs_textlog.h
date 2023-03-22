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

#if !defined(MYON_TEXTLOG_INC_)
#define MYON_TEXTLOG_INC_

class MYON_CLASS MYON_TextLog
{
public:
  /*
  Description:
    Create a text log that dumps to the virtual function
    void MYON_TextLog::AppendText().
  */
  MYON_TextLog();

  /*
  Description:
    Create a text log that dumps to an ASCII file.
  Parameters:
    fp - [in] Pointer to an open ASCII text file.  The file
              pointer must remain valid as long as the text
              log is in use.
  */
  MYON_TextLog( FILE* fp); // dump to open ASCII text file

  /*
  Description:
    Create a text log that dumps to a string.
  Parameters:
    s - [in] String that must exist as long as
             the text log is in use.
  */
  MYON_TextLog( MYON_wString& s );


  //////////////////////////////////////////////////////////////
  //
  // Level of detail interface
  //

  /// <summary>
  /// MYON_TextLog::LevelOfDetail determines ow much detail is printed. Functions that have an MYON_TextLog
  /// parameter, like the Dump() functions, may use the level of detail to tailor their output.
  /// may use the level of detail to tailor their output.
  /// </summary>
  enum class LevelOfDetail : unsigned char
  {
    /// <summary>
    /// A brief summary or synopsis.
    /// </summary>
    Minimum = 0,

    /// <summary>
    /// The default level of detail.
    /// </summary>
    Medium = 1,

    /// <summary>
    /// A verbose description that may be so long it obscures the important points.
    /// </summary>
    Maximum = 2
  };

  static MYON_TextLog::LevelOfDetail LevelOfDetailFromUnsigned(
    unsigned int level_of_detail
  );

  /*
  Description:
    Set the level of detail to print.
  Parameters:
    level_of_detail - [in]
      (default = MYON_TextLog::LevelOfDetail::Medium)
  */
  void SetLevelOfDetail(MYON_TextLog::LevelOfDetail level_of_detail);

  /*
  Returns:
    Level of detail to print
    0 = minimum level of detail
    4 = maximum level of detail
  */
  MYON_TextLog::LevelOfDetail GetLevelOfDetail() const;

  /*
  Parameter:
    level_of_detail - [in]
  Returns:
    True if this text log's level of detail the same or more detailed than
    the amount specified by level_of_detail.
  */
  bool LevelOfDetailIsAtLeast(MYON_TextLog::LevelOfDetail level_of_detail);

  /*
  Description:
    MYON_TextLog::Null is a silent text log and can be used when no output
    is desired but an MYON_TextLog parameter is required.
  */
  static MYON_TextLog Null;

  /*
  Returns:
    True if this is MYON_TextLog::Null.
  */
  bool IsNull() const;

  /*
  Returns:
    True if the text is being used to calculate a geometric content SHA1 hash
    and non-geometry information that varies based on time, computer, or user 
    should not be printed.
  */
  bool IsTextHash() const;

public:
  virtual ~MYON_TextLog();

  void SetDoubleFormat( const char* ); // default is %g
  void GetDoubleFormat( MYON_String& ) const;

  void SetFloatFormat( const char* ); // default is %g
  void GetFloatFormat( MYON_String& ) const;

  void SetColorFormat(MYON_Color::TextFormat color_format);
  MYON_Color::TextFormat GetColorFormat();

  /*
  Description:
    Returns color format to the default MYON_Color::TextFormat::DecimalRGBa
  */
  void ClearColorFormat();

  void PushIndent();
  void PopIndent();
  int IndentSize() const; //  0: one tab per indent
                          // >0: number of spaces per indent
  void SetIndentSize(int);

  /*
  Returns:
    Current indentation count
  */
  int IndentCount();
  /*
  Description:
    Set indentation count.
  */
  void SetIndentCount(
    int indent_count
  );
  
  void PrintWrappedText( const char*, int = 60 );    // last arg is maximum line length
  void PrintWrappedText( const wchar_t*, int = 60 ); // last arg is maximum line length

  /*
  Description:
    Print a formatted ASCII string of up to 2000 characters.
  Parameters:
    format - [in] nullptr terminated format control string 
  Remarks:
    To print strings longer than 2000 characters, you must
    use MYON_TextLog::PrintString.
  See Also:
    MYON_TextLog::PrintString
  */
  void MYON_VARGS_FUNC_CDECL Print(const char* format, ...);

  /*
  Description:
    Print a formatted INICODE string of up to 2000 characters.
  Parameters:
    format - [in] nullptr terminated format control string 
  Remarks:
    To print strings longer than 2000 characters, you must
    use MYON_TextLog::PrintString.
  See Also:
    MYON_TextLog::PrintString
  */
  void MYON_VARGS_FUNC_CDECL Print(const wchar_t* format, ...);

  void Print( float );
  void Print( double );
  void Print( const MYON_2dPoint& );
  void Print( const MYON_3dPoint& );
  void Print( const MYON_4dPoint& );
  void Print( const MYON_2dVector& );
  void Print( const MYON_3dVector& );
  void Print( const MYON_Xform& );
  void Print( const MYON_UUID& );
  void Print( const MYON_COMPONENT_INDEX& );

  /*
  Description:
    Print an unformatted wide char string of any length.
  Parameters:
    string - [in]
  */
  void Print( const MYON_wString& string );

  /*
  Description:
    Print an unformatted UTF-8 string of any length.
  Parameters:
    string - [in]
  */
  void Print( const MYON_String& string );

  void Print( const MYON_3dPointArray&, const char* = nullptr );
  void Print( 
         const MYON_Matrix&, 
         const char* = nullptr, // optional preamble
         int = 0             // optional number precision
    );

  // printing utilities
  /*
  Description:
    Same as calling Print("\n");
  */
  void PrintNewLine();

  /*
  Description:
    Same as calling Print(" ");
  */
  void PrintSpace();

  /*
  Description:
    Same as calling Print("\t");
  */
  void PrintTab();

  /*
  Description:
    Print an unformatted UTF-8 encoded null terminated string.
  Parameters:
    s - [in] UTF-8 encoded null terminated string.
  */
  void PrintString( const char* s );

  void PrintString(MYON_String s);
  
  /*
  Description:
    Print an unformatted UTF-16 or UTF-32 encoded null terminated string.
  Parameters:
    s - [in] UTF-16 or UTF-32 encoded null terminated string.
  */
  void PrintString( const wchar_t* s );
  
  void PrintString(MYON_wString s);

  /*
  Description:
    Print color using the format MYON_Color::TextFormat::DecimalRGB.
  */
  void PrintRGB( const MYON_Color& color);

  /*
  Description:
    Print color using MYON_Color::ToText(this->GetColorFormat(),0,true,*this);
  */
  void PrintColor(const MYON_Color& color);

  /*
  Description:
    Prints the time in the t parameter as Universal Coordinated Time in English.
    weekday month date::hour::min::sec year
  Parameters:
    t - [in]
  */
  void PrintTime( 
    const struct tm& t 
  );

  /*
  Description:
    Prints the current Universal Coordinated Time returned by the gmtime() function in English.
    weekday month date::hour::min::sec year
  */
  void PrintCurrentTime();

  void PrintPointList( 
    int,               // dim
    bool,              // true for rational points
    int,               // count
    int,               // stride
    const double*,     // point[] array
    const char* = nullptr // optional preabmle
    );

  void PrintPointGrid( 
    int,               // dim
    bool,              // true for rational points
    int, int,          // point_count0, point_count1
    int, int,          // point_stride0, point_stride1
    const double*,     // point[] array
    const char* = nullptr // optional preabmle
    );
    
  void PrintKnotVector( 
    int,             // order
    int,             // cv_count
    const double*    // knot[] array
    );

  MYON_TextLog& operator<<( const char* );
  MYON_TextLog& operator<<( char );
  MYON_TextLog& operator<<( short );
  MYON_TextLog& operator<<( int );
  MYON_TextLog& operator<<( float );
  MYON_TextLog& operator<<( double );
  MYON_TextLog& operator<<( const MYON_2dPoint& );
  MYON_TextLog& operator<<( const MYON_3dPoint& );
  MYON_TextLog& operator<<( const MYON_4dPoint& );
  MYON_TextLog& operator<<( const MYON_2dVector& );
  MYON_TextLog& operator<<( const MYON_3dVector& );
  MYON_TextLog& operator<<( const MYON_Xform& );

protected:
  friend class MYON_TextHash;

  FILE* m_pFile = nullptr;
  MYON_wString* m_pString = nullptr;
  
  /*
  Description:
    If the MYON_TextLog(MYON_wString& wstr) constructor was used, the
    default appends s to wstr.  If the MYON_TextLog(FILE* fp) 
    constructor was used, the default calls fputs( fp, s).
    In all other cases, the default calls printf("%s",s).
  Parameters:
    s - [in];
  */
  virtual
  void AppendText(
        const char* s
        );

  /*
  Description:
    If the MYON_TextLog(MYON_wString& wstr) constructor was used, the
    default appends s to wstr.  In all other cases, the default 
    converts the string to an MYON_String and calls the ASCII
    version AppendText(const char*).
  Parameters:
    s - [in];
  */
  virtual
  void AppendText(
        const wchar_t* s
        );
                  
private:
  MYON_String m_indent;
  MYON_String m_double_format;
  MYON_String m_double2_format;
  MYON_String m_double3_format;
  MYON_String m_double4_format;
  MYON_String m_float_format;
  MYON_String m_float2_format;
  MYON_String m_float3_format;
  MYON_String m_float4_format;

  MYON_String m_line;

  int m_beginning_of_line = 1;

  // size of a single indentation
  int m_indent_size = 0;       // 0 use tabs, > 0 = number of spaces per indent level

  // Number of indentations at the start of a new line
  int m_indent_count = 0;

  const bool m_bNullTextLog = false;
  MYON_TextLog::LevelOfDetail m_level_of_detail = MYON_TextLog::LevelOfDetail::Medium;
  MYON_Color::TextFormat m_color_format = MYON_Color::TextFormat::DecimalRGBa;
  MYON__UINT8 m_reserved1 = 0;

private:
  MYON_TextLog( const MYON_TextLog& ) = delete;
  MYON_TextLog& operator=( const MYON_TextLog& ) = delete;
};

/*
Description:
  MYON_TextLogIndent is a class used with MYON_TextLog to
  push and pop indentation.
*/
class MYON_CLASS MYON_TextLogIndent
{
public:
    // The constructor calls text_log.PushIndent()
    // and the destructor calls text_log.PopIndent()
  MYON_TextLogIndent(
    class MYON_TextLog& text_log
    );

    // If bEnabled is true, the constructor calls text_log.PushIndent()
    // and the destructor calls text_log.PopIndent()
    MYON_TextLogIndent(
      class MYON_TextLog& text_log,
      bool bEnabled
    );

  ~MYON_TextLogIndent();

private:
  class MYON_TextLog& m_text_log;
  bool m_bEnabled;

  // prevent use of copy construction and operator=
  // (no implementations)
  MYON_TextLogIndent(const MYON_TextLogIndent&);
  MYON_TextLogIndent& operator=(const MYON_TextLogIndent&);
};


/*
Description:
  MYON_TextLogLevelOfDetail is a class used with MYON_TextLog to push and pop level of detail.
*/
class MYON_CLASS MYON_TextLogLevelOfDetail
{
public:

  // The constructor saves the current level of detail and then sets the level of detail to level_of_detail.
  MYON_TextLogLevelOfDetail(
    class MYON_TextLog& text_log,
    MYON_TextLog::LevelOfDetail level_of_detail
  );

  // The destructor restores the level ot detail the saved value.
  ~MYON_TextLogLevelOfDetail();

  /*
  Returns:
    Level of detail the text log had when the constructor was called.
  */
  MYON_TextLog::LevelOfDetail SavedLevelOfDetail() const;

private:
  MYON_TextLogLevelOfDetail() = delete;
  MYON_TextLogLevelOfDetail(const MYON_TextLogLevelOfDetail&) = delete;
  MYON_TextLogLevelOfDetail& operator=(const MYON_TextLogLevelOfDetail&) = delete;

private:
  class MYON_TextLog& m_text_log;
  const MYON_TextLog::LevelOfDetail m_saved_level_of_detail;
};

class MYON_CLASS MYON_TextHash : public MYON_TextLog
{
public:
  MYON_TextHash() = default;
  ~MYON_TextHash() = default;

private:
  MYON_TextHash(const MYON_TextHash&) = delete;
  MYON_TextHash& operator= (const MYON_TextHash&) = delete;

public:

  MYON_StringMapType StringMapType() const;

  const class MYON_Locale& StringMapLocale() const;

  void SetStringMap(
    const class MYON_Locale& locale,
    MYON_StringMapType map_type
  );

  void SetStringMap(
    MYON_StringMapOrdinalType map_type
  );

  /*
  Parameters:
    bEnableIdRemap - [in]
    if bEnableIdRemap is true, the sequences of
    code points that match the format
    XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    where X is a hexadecimal digit (0-9, a-f, or A-F)
    and the hyphen is literal.
    will be replaced with an id created by 
    MYON_NextNotUniqueId(). 
    This is used for comparing code that generates streams
    containing new uuids.
  */
  void SetIdRemap(
    bool bEnableIdRemap
  );

  /*
  Returns:
    True if id remap is available.
  */
  bool IdRemap() const;

  /*
  Description:
    In some testing situations, the output text log can be set 
    when it is necessary to see the text used to compute the 
    SHA-1 hash. The has can be calculate which no output text
    log.

  Parameters:
    output_text_log - [in]
      Destination text log for the mtext used to calculate the hash.
  */
  void SetOutputTextLog(
    MYON_TextLog* output_text_log
  );

  MYON_TextLog* OutputTextLog() const;

  /*
  Returns:
    Total number of bytes that have passed through this text log.
  */
  MYON__UINT64 ByteCount() const;

  /*
  Returns:
    SHA-1 hash value of the text sent to this text log.
  */
  MYON_SHA1_Hash Hash() const;

private:
  void AppendText(const char* s) override;
  void AppendText(const wchar_t* s) override;

private:
  bool m_bApplyStringMap = false;
  bool m_bApplyIdRemap = false;
  
  MYON_UUID m_remap_id = MYON_nil_uuid;
  MYON_UuidPairList m_remap_id_list;

  MYON_StringMapType m_string_map_type = MYON_StringMapType::Identity;
  MYON_StringMapOrdinalType m_string_map_ordinal_type = MYON_StringMapOrdinalType::Identity;
  MYON_Locale m_string_map_local = MYON_Locale::InvariantCulture;

  MYON_TextLog* m_output_text_log = nullptr;

  static const char* Internal_ParseId(
    const char* s,
    MYON_UUID* id
  );

  static bool Internal_IsHexDigit(char c);

  MYON_SHA1 m_sha1;
};


#endif

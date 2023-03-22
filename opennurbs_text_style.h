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


#if !defined(OPENNURBS_TEXT_STYLE_INC_)
#define OPENNURBS_TEXT_STYLE_INC_


class MYON_CLASS MYON_TextStyle : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_TextStyle);

public:
  static const MYON_TextStyle Unset;    // All values not set
  static const MYON_TextStyle Default;  // index = -1, font set
  static const MYON_TextStyle ByLayer;  // index = -2, no font
  static const MYON_TextStyle ByParent; // index = -3, no font

  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if MYON_TextStyle::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If MYON_TextStyle::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned. 
  */
  static const MYON_TextStyle* FromModelComponentRef(
    const class MYON_ModelComponentReference& model_component_reference,
    const MYON_TextStyle* none_return_value
    );

  MYON_TextStyle();
  ~MYON_TextStyle() = default;
  MYON_TextStyle(const MYON_TextStyle&);
  MYON_TextStyle& operator=(const MYON_TextStyle&);

private:
  void Internal_Copy(
    const MYON_TextStyle& src 
    );

public:
  /*
  Returns:
    The managed font for this text style.
    Returns MYON_Font::Default if the font is not set.
  */
  const MYON_Font& Font() const;

  const MYON_wString& FontDescription() const;

  const wchar_t* FontDescriptionAsPointer() const;

  const MYON_wString& AppleFontName() const;

  const wchar_t* AppleFontNameAsPointer() const;

  /*
  Parameters:
    font_characteristics - [in]
  */
  void SetFont(
    const MYON_Font* font_characteristics
    );
  void SetFont(
    const MYON_Font& font_characteristics
    );

  /*
  Parameters:
    font_description - [in]
      If font_description is an empty string, then SetFont( alternate_font ) 
      is called.
    alternate_font - [in]
      Font to use if the current runtime environment does not have a
      font with a matching font description.
      When alternate_font is nullptr, MYON_Font::Default will be used as
      the alternate font.
  */
  void SetFontFromDescription(
    const wchar_t* font_description,
    const wchar_t* apple_font_name,
    const MYON_Font* alternate_font
    );

  void ClearFont();

  bool FontIsSet() const;

  void LockFont();
  bool FontIsLocked() const;

  /*
  Description:
    Get a string with the text style name and font description.
  Parameters:
    sSeparator - [in]
      string that separates the text style name and the font description.
      For example, if sSeparator = " - ", then 
      name_and_font_description = "<text style name> - <font description>".
    name_and_font_description - [out]
  Returns:
    A pointer to the string in name_and_font_description.  This pointer is never
    nullptr.    
  */
  const wchar_t* GetNameAndFontDescription(
    const wchar_t* sSeparator,
    MYON_wString& name_and_font_description
    ) const;

  static bool EqualTextStyleFontAndName(
    const MYON_TextStyle& a,
    const MYON_TextStyle& b
    );

private:  
  // V5 files had a single string used for different purposes
  // on Windows and Apple platforms. These two strings are runtime
  // information.
  MYON_wString m_font_description;

  MYON_wString m_apple_font_name;

  // m_managed_font points to a managed MYON_Font and is the font returned by Font().
  const MYON_Font* m_managed_font = nullptr;
 
  enum : unsigned char {
    font_bit = 1
  };

  unsigned char m_is_set_bits = 0;
  unsigned char m_is_locked_bits = 0;

  unsigned char m_reserved1 = 0;
  unsigned int m_reserved2 = 0;
  MYON__UINT_PTR m_reserved_ptr_1 = 0;
  MYON__UINT_PTR m_reserved_ptr_2 = 0;
  double m_reserved_double_0 = 0.0;
  double m_reserved_double_1 = 0.0;

public:
  // overrides virtual MYON_Object::IsValid
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  // overrides virtual MYON_Object::IsValid
  void Dump( MYON_TextLog& ) const override;

  // On September 16, 2015 the "V5" MYON_Font was split into
  //   MYON_TextStyle (a document object) and
  //   MYON_Font (a current runtime resource)
  //   = MYON_VersionNumberConstruct(6, 0, 2015, 9, 16, 0)
  //   Versions of opennurbs before binary_archive_opennurbs_version
  //   serialized MYON_TextStyle information in the V5 MYON_Font format.
  static const unsigned int binary_archive_opennurbs_version;

  // overrides virtual MYON_Object::Write
  bool Write( MYON_BinaryArchive& ) const override;

  // overrides virtual MYON_Object::Read
  bool Read( MYON_BinaryArchive& ) override;

};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_TextStyle*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ObjectArray<MYON_TextStyle>;
#endif

#endif

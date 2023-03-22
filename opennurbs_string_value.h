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

#pragma once
#if !defined(OPENNURBS_STRING_VALUE_INC_)
#define OPENNURBS_STRING_VALUE_INC_

class MYON_CLASS MYON_LengthValue
{
public:
  MYON_LengthValue() = default;
  ~MYON_LengthValue() = default;
  MYON_LengthValue(const MYON_LengthValue&) = default;
  MYON_LengthValue& operator=(const MYON_LengthValue&) = default;

  static const MYON_LengthValue Unset;
  static const MYON_LengthValue Zero;

  bool IsUnset() const;
  bool IsSet() const;

  bool Write(
    class MYON_BinaryArchive& archive
  ) const;

  bool Read(
    class MYON_BinaryArchive& archive
  );

  /*
  Description:
    Create an MYON_LengthValue by parsing a string.
  Parameters:
    parse_settings - [in]
      Pass MYON_ParseSettings(context_length_unit_system,context_angle_unit_system,context_locale_id)
    string - [in]
      null terminated string to parse.
  Returns:
    If the string is valid, the exact length value is returned.
    If the string is not valid or parsing ends before the string's null terminator,
    the MYON_LengthValue::Unset is returned.
  Remarks:
    If the entire string is not parsed, that is an error condition.
    Use CreateFromSubString() to permit parsing a portion of the string.
  */
  static MYON_LengthValue CreateFromString(
    MYON_ParseSettings parse_settings,
    const wchar_t* string
  );

  /*
  Description:
    Create an MYON_LengthValue by parsing a string.
  Parameters:
    parse_settings - [in]
      Pass MYON_ParseSettings(context_length_unit_system,context_angle_unit_system,context_locale_id)
    string - [in]
      null terminated string to parse.
    string_count - [in]
      string[] and string_count specify the string to parse.
      If string_count >= 0, it specifies the maximum number of elements in string[]
      that may be parsed.  If string_count = -1, then the string must contain a 
      character that terminates length parsing.
    string_end - [out]
      If string_end is not nullptr, then *string_end points to the first 
      element in the string that was not parsed.
  Returns:
    If the string is valid, the exact length value is returned.
    If the string is not valid or parsing ends before the string's null terminator,
    the MYON_LengthValue::Unset is returned.
  Remarks:
    If the entire string is not parsed, that is an error condition.
    Use CreateFromSubString() to permit parsing a portion of the string.
  */
  static MYON_LengthValue CreateFromSubString(
    MYON_ParseSettings parse_settings,
    const wchar_t* string,
    int string_count,
    const wchar_t** string_end
  );

  /*
  Returns:
    A MYON_LengthValue with the same length value and unit system = None.
  */
  const MYON_LengthValue RemoveUnitSystem() const;

  /*
  Parameters:
    length_value - [in]
      New length.
  Returns:
    A MYON_LengthValue with the new length and other settings copied from this.
  */
  const MYON_LengthValue ChangeLength(
    double length_value
    ) const;

#pragma region RH_C_SHARED_ENUM [MYON_LengthValue::StringFormat] [Rhino.LengthValue.StringFormat] [nested:byte]
  /// <summary>
  /// Formatting to apply when creating a length value from a double.
  /// </summary>
  enum class StringFormat : unsigned char
  {
    ///<summary>Use exact decimal string.</summary>
    ExactDecimal = 0,

    ///<summary>If possible, use exact integer-fraction format (1.125 becomes 1-1/8).</summary>
    ExactProperFraction = 1,

     ///<summary>If possible, use exact fraction format (1.125 becomes 9/8).</summary>
    ExactImproperFraction = 2,

    ///<summary>The value may be adjusted slightly to improve clarity (1.124999... becomes 1.125).</summary>
    CleanDecimal = 3, 

    ///<summary>The value may be adjusted slightly to improve clarity (1.124999... becomes 1-1/8).</summary>
    CleanProperFraction = 4,

    ///<summary>The value may be adjusted slightly to improve clarity (1.124999... becomes 9/8).</summary>
    CleanImproperFraction = 5
  };
#pragma endregion

  static MYON_LengthValue::StringFormat LengthStringFormatFromUnsigned(
    unsigned int string_format_as_unsigned
  );

  /*
  Parameters:
    length_value - [in]
    length_unit_system - [in]
    bUseFractionsInString - [in]
      If bUseFractions is true and length_value can be represented as a common
      fraction, then the string form will contain a fraction rather than a decimal.
    locale_id - [in]
      locale id for the string length unit system
      0 will select MYON_Locale::CurrentCulture.
    string_format - [in]
      Determines the format of the string representation
  Returns:
    Length in the specified length unit system
  Remarks:
    If you don't like the automatically created string value, then 
    format the string yourself and use MYON_LengthValue::CreateFromString().
  */ 
  static MYON_LengthValue Create(
    double length_value,
    const class MYON_UnitSystem& length_unit_system,
    unsigned int locale_id,
    MYON_LengthValue::StringFormat string_format
  );

  /*
  Parameters:
    length_value - [in]
    length_unit_system - [in]
    bUseFractionsInString - [in]
      If bUseFractions is true and length_value can be represented as a common
      fraction, then the string form will contain a fraction rather than a decimal.
    locale_id - [in]
      locale id for the string length unit system
    bool 
  Returns:
    Length in the specified length unit system
  Remarks:
    If you don't like the automatically created string value, then 
    format the string yourself and use MYON_LengthValue::CreateFromString().
  */ 
  static MYON_LengthValue Create(
    double length_value,
    const MYON::LengthUnitSystem length_unit_system,
    unsigned int locale_id,
    MYON_LengthValue::StringFormat string_format
  );

  static MYON_LengthValue Create(
    double length_value,
    const class MYON_LengthUnitName& length_unit_system,
    MYON_LengthValue::StringFormat string_format
  );

  /*
  Parameters:
    context_unit_system - [in]
      length unit system for the returned value.
      Pass MYON_UnitSystem::None to ignore the length unit system and get the value save in this class. 
  Returns:
    Length in the specified length unit system
  */
  double Length(
    const class MYON_UnitSystem& context_unit_system
  ) const;

  /*
  Parameters:
    context_unit_system - [in]
      length unit system for the returned value.
      Pass MYON::LengthUnitSystem::None to ignore the length unit system and get the value save in this class. 
  Returns:
    Length in the specified length unit system
  */
  double Length(
    MYON::LengthUnitSystem context_unit_system
  ) const;

  /*
  Returns:
    Length unit system for this class.
  */
  const class MYON_UnitSystem& LengthUnitSystem() const;

  /*
  Returns:
    The length as a string.
  Remarks:
    If MYON_LengthValue::CreateFromString() or MYON_LengthValue::CreateFromSubString()
    were used to create this MYON_LengthValue, a copy of that string
    is returned.
  */
  const MYON_wString& LengthAsString() const;
  const wchar_t* LengthAsStringPointer() const;

  const MYON_ParseSettings LengthStringParseSettings() const;

  /*
  Returns:
    Format processing applied to input values.
  */
  MYON_LengthValue::StringFormat LengthStringFormat() const;

  /*
  Returns:
    Locale used to parse input strings and create unit names.
  */
  unsigned int ContextLocaleId() const;

  /*
  Returns:
    Angle unit system used to parse input strings.
  */
  MYON::AngleUnitSystem ContextAngleUnitSystem() const;

  const MYON_SHA1_Hash ContentHash() const;
  static int Compare(
    const MYON_LengthValue& lhs,
    const MYON_LengthValue& rhs
  );

private:
  // parsing context
  unsigned int m_context_locale_id = 0;
  MYON::AngleUnitSystem m_context_angle_unit_system = MYON::AngleUnitSystem::Unset;
  MYON_LengthValue::StringFormat m_string_format = MYON_LengthValue::StringFormat::ExactDecimal;

  MYON_UnitSystem m_length_unit_system = MYON_UnitSystem::Unset;
  double m_length = MYON_DBL_QNAN;

  MYON_wString m_length_as_string;
};


class MYON_CLASS MYON_AngleValue
{
public:
  MYON_AngleValue() = default;
  ~MYON_AngleValue() = default;
  MYON_AngleValue(const MYON_AngleValue&) = default;
  MYON_AngleValue& operator=(const MYON_AngleValue&) = default;

  bool Write(
    class MYON_BinaryArchive& archive
  ) const;

  bool Read(
    class MYON_BinaryArchive& archive
  );

  static const MYON_AngleValue Unset;
  static const MYON_AngleValue Zero;

  bool IsUnset() const;
  bool IsSet() const;

  /// <summary>
  /// MYON_AngleValue::StringFormat identifies the formatting to apply when creating 
  /// a length value from a double.
  /// </summary>
  enum class StringFormat : unsigned char
  {
    ///<summary>Use exact decimal string.</summary>
    ExactDecimal = 0,

    ///<summary>If possible, use exact fraction format (1.125 becomes 9/8).</summary>
    ExactFraction = 1,

    ///<summary>The value may be adjusted slightly to improve clarity (1.124999... becomes 1.125).</summary>
    CleanDecimal = 2, 

    ///<summary>The value may be adjusted slightly to improve clarity (1.124999... becomes 9/8).</summary>
    CleanFraction = 3
  };

  static MYON_AngleValue::StringFormat AngleStringFormatFromUnsigned(
    unsigned int string_format_as_unsigned
  );


  /*
  Description:
    Create an MYON_AngleValue by parsing a string.
  Parameters:
    parse_settings - [in]
      Pass MYON_ParseSettings(context_length_unit_system,context_angle_unit_system,context_locale_id)
    string - [in]
      null terminated string to parse.
  Returns:
    If the string is valid, the exact angle value is returned.
    If the string is not valid or parsing ends before the string's null terminator,
    the MYON_AngleValue::Unset is returned.
  Remarks:
    If the entire string is not parsed, that is an error condition.
    Use CreateFromSubString() to permit parsing a portion of the string.
  */
  static MYON_AngleValue CreateFromString(
    MYON_ParseSettings parse_settings,
    const wchar_t* string
  );
  
  /*
  Description:
    Create an MYON_AngleValue by parsing a string.
  Parameters:
    parse_settings - [in]
      Pass MYON_ParseSettings(context_length_unit_system,context_angle_unit_system,context_locale_id)
    string - [in]
      null terminated string to parse.
    string_count - [in]
      string[] and string_count specify the string to parse.
      If string_count >= 0, it specifies the maximum number of elements in string[]
      that may be parsed.  If string_count = -1, then the string must contain a 
      character that terminates angle parsing.
    string_end - [out]
      If string_end is not nullptr, then *string_end points to the first 
      element in the string that was not parsed.
  Returns:
    If the string is valid, the exact angle value is returned.
    If the string is not valid or parsing ends before the string's null terminator,
    the MYON_AngleValue::Unset is returned.
  Remarks:
    If the entire string is not parsed, that is an error condition.
    Use CreateFromSubString() to permit parsing a portion of the string.
  */
  static MYON_AngleValue CreateFromSubString(
    MYON_ParseSettings parse_settings,
    const wchar_t* string,
    int string_count,
    const wchar_t** string_end
  );

  /*
  Parameters:
    angle_value - [in]
    angle_unit_system - [in]
    bUseFractionsInString - [in]
      If bUseFractions is true and angle_value can be represented as a common
      fraction, then the string form will contain a fraction rather than a decimal.
    locale_id - [in]
      locale id for the string angle unit system
    bool 
  Returns:
    Angle in the specified angle unit system
  Remarks:
    If you don't like the automatically created string value, then 
    format the string yourself and use MYON_AngleValue::CreateFromString().
  */ 
  static MYON_AngleValue Create(
    double angle_value,
    MYON::AngleUnitSystem angle_unit_system,
    unsigned int locale_id,
    MYON_AngleValue::StringFormat string_format
  );

  static MYON_AngleValue Create(
    double angle_value,
    const class MYON_AngleUnitName& angle_unit_system,
    MYON_AngleValue::StringFormat string_format
  );

  /*
  Parameters:
    context_unit_system - [in]
      angle unit system for the returned value.
      Pass MYON::AngleUnitSystem::None to ignore the angle unit system and get the value save in this class. 
  Returns:
    Angle in the specified angle unit system
  */
  double Angle(
    MYON::AngleUnitSystem context_unit_system
  ) const;

  /*
  Returns:
    Angle unit system for this class.
  */
  MYON::AngleUnitSystem AngleUnitSystem() const;

  /*
  Returns:
    The angle as a string.
  Remarks:
    If MYON_AngleValue::CreateFromString() or MYON_AngleValue::CreateFromSubString()
    were used to create this MYON_AngleValue, a copy of that string
    is returned.
  */
  const MYON_wString& AngleAsString() const;
  const wchar_t* AngleAsStringPointer() const;

  const MYON_ParseSettings AngleStringParseSettings() const;

private:
  // parsing context
  unsigned int m_context_locale_id = 0;
  MYON::LengthUnitSystem m_context_length_unit_system = MYON::LengthUnitSystem::None;
  MYON_AngleValue::StringFormat m_string_format = MYON_AngleValue::StringFormat::ExactDecimal;
  
  MYON::AngleUnitSystem m_angle_unit_system = MYON::AngleUnitSystem::Unset;
  double m_angle = MYON_DBL_QNAN;

  MYON_wString m_angle_as_string;
};

class MYON_CLASS MYON_ScaleValue
{
public:
  MYON_ScaleValue() = default;
  ~MYON_ScaleValue() = default;
  MYON_ScaleValue(const MYON_ScaleValue&) = default;
  MYON_ScaleValue& operator=(const MYON_ScaleValue&) = default;

  static const MYON_ScaleValue Unset;
  static const MYON_ScaleValue OneToOne;

  bool IsUnset() const;
  bool IsSet() const;

  bool Write(
    class MYON_BinaryArchive& archive
  ) const;

  bool Read(
    class MYON_BinaryArchive& archive
  );

#pragma region RH_C_SHARED_ENUM [MYON_ScaleValue::ScaleStringFormat] [Rhino.ScaleValue.ScaleStringFormat] [nested:byte]
  /// <summary>
  /// Specifies preferred formats for automatically
  /// created string descriptions of a scale value.
  /// </summary>
  enum class ScaleStringFormat : unsigned char
  {
    /// <summary>
    /// No preference for automatically created string descriptions of a scale value.
    /// </summary>
    None = 0,

    /// <summary>
    /// Prefer the ratio format using a colon, like "1:4" or "4:1".
    /// </summary>
    RatioFormat = 1,

    /// <summary>
    /// Prefer the equation format using an equal sign, like "1 = 4" or "4 = 1".
    /// </summary>
    EquationFormat = 2,

    /// <summary>
    /// Prefer the fraction format using a slash, like "1/4" or "4/1".
    /// </summary>
    FractionFormat = 3,

    /// <summary>
    /// MYON_ScaleValue::ScaleStringFormat::Unset is used to indicate no preference is set.
    /// This condition is different from MYON_ScaleValue::ScaleStringFormat::None.
    /// </summary>
    Unset = 0xFF
  };
#pragma endregion

  static MYON_ScaleValue::ScaleStringFormat ScaleStringFormatFromUnsigned(
    unsigned int scale_string_format_as_unsigned
  );

  /*
  Description:
    Create an MYON_ScaleValue by parsing a string.
  Parameters:
    parse_settings - [in]
      Pass MYON_ParseSettings(context_length_unit_system,context_angle_unit_system,context_locale_id)
    string - [in]
      null terminated string to parse.
  Returns:
    If the string is valid, the exact scale value is returned.
    If the string is not valid or parsing ends before the string's null terminator,
    the MYON_ScaleValue::Unset is returned.
  Remarks:
    If the entire string is not parsed, that is an error condition.
    Use CreateFromSubString() to permit parsing a portion of the string.
  Examples:
    "1:4", "1=4", "1/4", "0.25"
    will set LeftToRightScale() = 4, RightToLeftScale() = 0.25

    "4:1", "4=1", "4/1", "4"
    will set LeftToRightScale() = 0.25, RightToLeftScale() = 4

    "100:1", "100=1", "1 meter = 1 centimeter"
    will set LeftToRightScale() = 0.01, RightToLeftScale() = 100

    "1:100", "1=100", "1 centimeter = 1 meter"
    will set LeftToRightScale() = 100, RightToLeftScale() = 0.01

    "12:1", "12=1", "12/1", "12", "1 foot = 1 inch"
    will set LeftToRightScale() = 0.08333..., RightToLeftScale() = 12

    "1:12", "1=12", "1/12", "1 inch = 1 foot"
    will set LeftToRightScale() = 12, RightToLeftScale() = 0.08333...

    "1:48", "1 = 48", "1/4 inch = 1 foot"
    will set LeftToRightScale() = 48, RightToLeftScale() = 0.0208333...
  */
  static MYON_ScaleValue CreateFromString(
    MYON_ParseSettings parse_settings,
    const wchar_t* string
  );

  /*
  Description:
    Create an MYON_ScaleValue by parsing a string.
  Parameters:
    parse_settings - [in]
      Pass MYON_ParseSettings(context_length_unit_system,context_angle_unit_system,context_locale_id)
    string - [in]
      null terminated string to parse.
    string_count - [in]
      string[] and string_count specify the string to parse.
      If string_count >= 0, it specifies the maximum number of elements in string[]
      that may be parsed.  If string_count = -1, then the string must contain a 
      character that terminates scale parsing.
    string_end - [out]
      If string_end is not nullptr, then *string_end points to the first 
      element in the string that was not parsed.
  Returns:
    If the string is valid, the exact scale value is returned.
    If the string is not valid or parsing ends before the string's null terminator,
    the MYON_ScaleValue::Unset is returned.
  Remarks:
    If the entire string is not parsed, that is an error condition.
    Use CreateFromSubString() to permit parsing a portion of the string.
  
  Examples:
    "1:4", "1=4", "1/4", "0.25"
    will set LeftToRightScale() = 4, RightToLeftScale() = 0.25

    "4:1", "4=1", "4/1", "4"
    will set LeftToRightScale() = 0.25, RightToLeftScale() = 4

    "100:1", "100=1", "1 meter = 1 centimeter"
    will set LeftToRightScale() = 0.01, RightToLeftScale() = 100

    "1:100", "1=100", "1 centimeter = 1 meter"
    will set LeftToRightScale() = 100, RightToLeftScale() = 0.01

    "12:1", "12=1", "12/1", "12", "1 foot = 1 inch"
    will set LeftToRightScale() = 0.08333..., RightToLeftScale() = 12

    "1:12", "1=12", "1/12", "1 inch = 1 foot"
    will set LeftToRightScale() = 12, RightToLeftScale() = 0.08333...

    "1:48", "1 = 48", "1/4 inch = 1 foot"
    will set LeftToRightScale() = 48, RightToLeftScale() = 0.0208333...
  */
  static MYON_ScaleValue CreateFromSubString(
    MYON_ParseSettings parse_settings,
    const wchar_t* string,
    int string_count,
    const wchar_t** string_end
  );

  /*
  Parameters:
    left_length - [in]
    right_length - [in]
  Returns:
    A scale value for converting a distance from source_length to
    destination_length.
  Remarks:
    If you don't like the automatically created string value, then 
    format the string yourself and use MYON_ScaleValue::CreateFromString().
  */ 
  static MYON_ScaleValue Create(
    const class MYON_LengthValue& left_side_length,
    const class MYON_LengthValue& right_side_length,
    MYON_ScaleValue::ScaleStringFormat string_format_preference
  );

  /*
  Returns:
    A dimensionless scale factor.
    The word "dimensionless" is critical. Differences in left and right
    side unit systems are accounted for in the returned value.
  Remarks:
    LeftToRightScale() = 1.0/RightToLeftScale()
  Examples:
    "1:4", "1=4", "1/4", "0.25"
    will set LeftToRightScale() = 4, RightToLeftScale() = 0.25

    "4:1", "4=1", "4/1", "4"
    will set LeftToRightScale() = 0.25, RightToLeftScale() = 4

    "100:1", "100=1", "1 meter = 1 centimeter"
    will set LeftToRightScale() = 0.01, RightToLeftScale() = 100

    "1:100", "1=100", "1 centimeter = 1 meter"
    will set LeftToRightScale() = 100, RightToLeftScale() = 0.01

    "12:1", "12=1", "12/1", "12", "1 foot = 1 inch"
    will set LeftToRightScale() = 0.08333..., RightToLeftScale() = 12

    "1:12", "1=12", "1/12", "1 inch = 1 foot"
    will set LeftToRightScale() = 12, RightToLeftScale() = 0.08333...

    "1:48", "1 = 48", "1/4 inch = 1 foot"
    will set LeftToRightScale() = 48, RightToLeftScale() = 0.0208333...
  */
  double LeftToRightScale() const;

  /*
  Returns:
    A dimensionless scale factor.
    The word "dimensionless" is critical. Differences in left and right
    side unit systems are accounted for in the returned value.
  Remarks:
    RightToLeftScale() = 1.0/LeftToRightScale()
  Examples:
    "1:4", "1=4", "1/4", "0.25"
    will set LeftToRightScale() = 4, RightToLeftScale() = 0.25

    "4:1", "4=1", "4/1", "4"
    will set LeftToRightScale() = 0.25, RightToLeftScale() = 4

    "100:1", "100=1", "1 meter = 1 centimeter"
    will set LeftToRightScale() = 0.01, RightToLeftScale() = 100

    "1:100", "1=100", "1 centimeter = 1 meter"
    will set LeftToRightScale() = 100, RightToLeftScale() = 0.01

    "12:1", "12=1", "12/1", "12", "1 foot = 1 inch"
    will set LeftToRightScale() = 0.08333..., RightToLeftScale() = 12

    "1:12", "1=12", "1/12", "1 inch = 1 foot"
    will set LeftToRightScale() = 12, RightToLeftScale() = 0.08333...

    "1:48", "1 = 48", "1/4 inch = 1 foot"
    will set LeftToRightScale() = 48, RightToLeftScale() = 0.0208333...
  */
  double RightToLeftScale() const;


  const class MYON_LengthValue& LeftLengthValue() const;
  const class MYON_LengthValue& RightLengthValue() const;

  /*
  Returns:
    The scale as a string.
  Remarks:
    If MYON_ScaleValue::CreateFromString() or MYON_ScaleValue::CreateFromSubString()
    were used to create this MYON_ScaleValue, a copy of that string is returned.
  */
  const MYON_wString& ScaleAsString() const;
  const wchar_t* ScaleAsStringPointer() const;

  const MYON_ParseSettings ScaleStringParseSettings() const;

  /*
  Description
    Exchange the left and right lengths.
  */
  void SwapLeftAndRight();

  const MYON_SHA1_Hash ContentHash() const;

  static int Compare(
    const MYON_ScaleValue& lhs,
    const MYON_ScaleValue& rhs
  );

private:
  double m_left_to_right_scale = MYON_DBL_QNAN;
  double m_right_to_left_scale = MYON_DBL_QNAN;

  // parsing context
  unsigned int m_context_locale_id = 0;
  MYON::LengthUnitSystem m_context_length_unit_system = MYON::LengthUnitSystem::None;
  MYON::AngleUnitSystem m_context_angle_unit_system = MYON::AngleUnitSystem::None;
  MYON_ScaleValue::ScaleStringFormat m_string_format_preference = MYON_ScaleValue::ScaleStringFormat::Unset;

  MYON_wString m_scale_as_string;

  MYON_LengthValue m_left_length = MYON_LengthValue::Unset;
  MYON_LengthValue m_right_length = MYON_LengthValue::Unset;
};

#endif


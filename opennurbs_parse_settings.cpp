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

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

void MYON_ParseSettings::Internal_DefaultCtor()
{
  size_t i;
  for ( i = 0; i < sizeof(m_true_default_bits)/sizeof(m_true_default_bits[0]); i++ )
    m_true_default_bits[i] = 0;
  for ( i = 0; i < sizeof(m_false_default_bits)/sizeof(m_false_default_bits[0]); i++ )
    m_false_default_bits[i] = 0;
}

MYON_ParseSettings::MYON_ParseSettings()
{
  Internal_DefaultCtor();
}

MYON_ParseSettings::MYON_ParseSettings(
  const class MYON_UnitSystem& context_length_unit_system,
  MYON::AngleUnitSystem context_angle_unit_system,
  unsigned int context_locale_id
)
{
  Internal_DefaultCtor();
  SetContextLengthUnitSystem(context_length_unit_system.UnitSystem());
  SetContextAngleUnitSystem(context_angle_unit_system);
  SetContextLocaleId(context_locale_id);
}

MYON_ParseSettings::MYON_ParseSettings(
  MYON::LengthUnitSystem context_length_unit_system,
  MYON::AngleUnitSystem context_angle_unit_system,
  unsigned int context_locale_id
)
{
  Internal_DefaultCtor();
  SetContextLengthUnitSystem(context_length_unit_system);
  SetContextAngleUnitSystem(context_angle_unit_system);
  SetContextLocaleId(context_locale_id);
}

void MYON_ParseSettings::SetAllToFalse()
{
  size_t i;
  *this = MYON_ParseSettings::DefaultSettings;
  for ( i = 0; i < sizeof(m_true_default_bits)/sizeof(m_true_default_bits[0]); i++ )
    m_true_default_bits[i] = 0xFFFFFFFF;
}

const int MYON_ParseSettings::Compare(
  const MYON_ParseSettings* a,
  const MYON_ParseSettings* b
  )
{
  size_t i;

  if ( 0 == a || 0 == b )
  {
    if ( 0 != b )
      return -1;
    if ( 0 != a )
      return 1;
    return 0;
  }
  
  for ( i = 0; i < sizeof(a->m_true_default_bits)/sizeof(a->m_true_default_bits[0]); i++ )
  {
    if ( a->m_true_default_bits[i] < b->m_true_default_bits[i] )
      return -1;
    if ( b->m_true_default_bits[i] < a->m_true_default_bits[i] )
      return 1;
  }
  
  for ( i = 0; i < sizeof(a->m_false_default_bits)/sizeof(a->m_false_default_bits[0]); i++ )
  {
    if ( a->m_false_default_bits[i] < b->m_false_default_bits[i] )
      return -1;
    if ( b->m_false_default_bits[i] < a->m_false_default_bits[i] )
      return 1;
  }

  if ( a->m_context_locale_id < b->m_context_locale_id )
    return -1;
  if ( b->m_context_locale_id < a->m_context_locale_id )
    return 1;

  if ( a->m_context_length_unit_system < b->m_context_length_unit_system )
    return -1;
  if ( b->m_context_length_unit_system < a->m_context_length_unit_system )
    return 1;

  if ( a->m_context_angle_unit_system < b->m_context_angle_unit_system )
    return -1;
  if ( b->m_context_angle_unit_system < a->m_context_angle_unit_system )
    return 1;

  return 0;
}

void MYON_ParseSettings::SetAllExpressionSettingsToFalse()
{
  // parsing of things that have multiple numbers like
  // feet and inches, arc degrees minutes seconds,
  // integer-fraction and rational numbers uses
  // this helper to prevent "fancy" stuff when parsing
  // the "additional" numbers.
  SetParseExplicitFormulaExpression(false);
  SetParseArithmeticExpression(false);
  SetParseMultiplication(false);
  SetParseDivision(false);  
  SetParseAddition(false);
  SetParseSubtraction(false);
  SetParseMathFunctions(false);
  SetParsePairedParentheses(false);
}

const MYON_ParseSettings MYON_ParseSettings::DefaultSettings;

static MYON_ParseSettings DefaultSettingsInRadiansHelper()
{
  MYON_ParseSettings default_settings_in_radians(MYON_ParseSettings::DefaultSettings);
  default_settings_in_radians.SetDefaultAngleUnitSystem(MYON::AngleUnitSystem::Radians);
  return default_settings_in_radians;
}

const MYON_ParseSettings MYON_ParseSettings::DefaultSettingsInRadians(DefaultSettingsInRadiansHelper());

static MYON_ParseSettings DefaultSettingsInDegreesHelper()
{
  MYON_ParseSettings default_settings_in_degrees(MYON_ParseSettings::DefaultSettings);
  default_settings_in_degrees.SetDefaultAngleUnitSystem(MYON::AngleUnitSystem::Degrees);
  return default_settings_in_degrees;
}

const MYON_ParseSettings MYON_ParseSettings::DefaultSettingsInDegrees(DefaultSettingsInDegreesHelper());

static MYON_ParseSettings FalseSettingsHelper()
{
  MYON_ParseSettings false_settings;
  false_settings.SetAllToFalse();
  return false_settings;
}

const MYON_ParseSettings MYON_ParseSettings::FalseSettings(FalseSettingsHelper());

static MYON_ParseSettings IntegerSettingsHelper()
{
  MYON_ParseSettings integer_settings(MYON_ParseSettings::FalseSettings);
  // do not enable parsing of leading white space here
  integer_settings.SetParseUnaryPlus(true);
  integer_settings.SetParseUnaryMinus(true);
  integer_settings.SetParseSignificandIntegerPart(true);
  return integer_settings;
}

const MYON_ParseSettings MYON_ParseSettings::IntegerNumberSettings(IntegerSettingsHelper());

static MYON_ParseSettings RationalNumberSettingsHelper()
{
  MYON_ParseSettings rational_number_settings(MYON_ParseSettings::IntegerNumberSettings);
  // do not enable parsing of leading white space here
  rational_number_settings.SetParseRationalNumber(true);
  return rational_number_settings;
}

const MYON_ParseSettings MYON_ParseSettings::RationalNumberSettings(RationalNumberSettingsHelper());

static MYON_ParseSettings DoubleNumberSettingsHelper()
{
  MYON_ParseSettings double_number_settings(MYON_ParseSettings::IntegerNumberSettings);
  // do not enable parsing of leading white space here
  double_number_settings.SetParseSignificandDecimalPoint(true);
  double_number_settings.SetParseSignificandFractionalPart(true);
  //double_number_settings.SetParseSignificandDigitSeparators(true);
  double_number_settings.SetParseScientificENotation(true);
  return double_number_settings;
}

const MYON_ParseSettings MYON_ParseSettings::DoubleNumberSettings(DoubleNumberSettingsHelper());

static MYON_ParseSettings RealNumberSettingsHelper()
{
  MYON_ParseSettings real_number_settings(MYON_ParseSettings::DoubleNumberSettings);
  // do not enable parsing of leading white space here
  real_number_settings.SetParsePi(true);
  return real_number_settings;
}

const MYON_ParseSettings MYON_ParseSettings::RealNumberSettings(RealNumberSettingsHelper());

MYON_ParseSettings& MYON_ParseSettings::operator|=(const MYON_ParseSettings& other)
{
  size_t i;

  for ( i = 0; i < sizeof(m_true_default_bits)/sizeof(m_true_default_bits[0]); i++ )
    m_true_default_bits[i] &= other.m_true_default_bits[i];

  for ( i = 0; i < sizeof(m_false_default_bits)/sizeof(m_false_default_bits[0]); i++ )
    m_false_default_bits[i] |= other.m_false_default_bits[i];

  if (0 == m_context_length_unit_system)
  {
    // "this" wins if it alread has a locale id.
    // The reason is that the |= operator is used to add
    // a property to "this" when its current
    // property has a "false" value.
    m_context_length_unit_system = other.m_context_length_unit_system;
  }

  if (0 == m_context_angle_unit_system)
  {
    // "this" wins if it alread has a locale id.
    // The reason is that the |= operator is used to add
    // a property to "this" when its current
    // property has a "false" value.
    m_context_angle_unit_system = other.m_context_angle_unit_system;
  }

  if (0 == m_context_locale_id)
  {
    // "this" wins if it alread has a locale id.
    // The reason is that the |= operator is used to add
    // a property to "this" when its current
    // property has a "false" value.
    m_context_locale_id = other.m_context_locale_id;
  }

  return *this;
}

MYON_ParseSettings& MYON_ParseSettings::operator&=(const MYON_ParseSettings& other)
{
  size_t i;

  for ( i = 0; i < sizeof(m_true_default_bits)/sizeof(m_true_default_bits[0]); i++ )
    m_true_default_bits[i] |= other.m_true_default_bits[i];
  
  for ( i = 0; i < sizeof(m_false_default_bits)/sizeof(m_false_default_bits[0]); i++ )
    m_false_default_bits[i] &= other.m_false_default_bits[i];
  
  if ( m_context_length_unit_system != other.m_context_length_unit_system )
  {
    m_context_length_unit_system = 0;
  }
  
  if ( m_context_angle_unit_system != other.m_context_angle_unit_system )
  {
    m_context_angle_unit_system = 0;
  }
  
  if ( m_context_locale_id != other.m_context_locale_id )
  {
    // If m_context_locale_id != other.m_context_locale_id 
    // identify the same language, then preserve the lauguage.
    // This is useful when parsing unit names, particularly in
    // English where en-US SI unit names end in "er" and many 
    // other en-* SI unit names end in "re".  Setting 
    // m_context_locale_id to "en-ZERO" means that both
    // "meter" and "metre" will get parsed as MYON::LengthUnitSystem::Meters.
    unsigned short this_language_id = (m_context_locale_id & 0x00FF);
    unsigned short other_language_id = (other.m_context_locale_id & 0x00FF);
    if ( this_language_id == other_language_id )
      m_context_locale_id = this_language_id;
    else
      m_context_locale_id = 0;
  }

  return *this;
}

bool operator==(
  const class MYON_ParseSettings& a,
  const class MYON_ParseSettings& b
  )
{
  return 0 == MYON_ParseSettings::Compare(&a,&b);
}

bool operator!=(
  const class MYON_ParseSettings& a,
  const class MYON_ParseSettings& b
  )
{
  return 0 != MYON_ParseSettings::Compare(&a,&b);
}

MYON_ParseSettings operator||(
  const class MYON_ParseSettings& a,
  const class MYON_ParseSettings& b
  )
{
  MYON_ParseSettings a_or_b(a);
  a_or_b |= b;
  return a_or_b;
}

MYON_ParseSettings operator&&(
  const class MYON_ParseSettings& a,
  const class MYON_ParseSettings& b
  )
{
  MYON_ParseSettings a_and_b(a);
  a_and_b &= b;
  return a_and_b;
}


#define MYON_ParseNumberSettingsBitTrueDefault(n)  (0 == (m_true_default_bits[n/32]  & (1<<((unsigned int)(n%32)))))
#define MYON_ParseNumberSettingsBitFalseDefault(n) (0 != (m_false_default_bits[n/32] & (1<<((unsigned int)(n%32)))))
#define MYON_SetParseNumberSettingsBitTrueDefault(e,b)  unsigned int n = e; unsigned int mask = (1<<(n%32)); if (b) m_true_default_bits[n/32]  &= ~mask; else m_true_default_bits[n/32]  |=  mask
#define MYON_SetParseNumberSettingsBitFalseDefault(e,b) unsigned int n = e; unsigned int mask = (1<<(n%32)); if (b) m_false_default_bits[n/32] |=  mask; else m_false_default_bits[n/32] &= ~mask

enum bitdex_true_default
{
  parse_leading_white_space,
  parse_arithmetic_expression,
  parse_explicit_formula_expression, // old style =(...) formula syntax
  parse_unary_minus,
  parse_unary_plus,
  parse_significand_integer_part,     // digits before the decimal point
  parse_significand_decimal_point,    // the decimal point
  parse_significand_fractional_part,  // digits after the decimal point
  parse_significand_digit_separators, // characters that delimit groups of digits
  parse_scientific_e_notation,
  parse_rational_number,
  parse_pi,
  parse_multiplication,
  parse_division,
  parse_addition,
  parse_subtraction,
  parse_math_functions,
  parse_paired_parentheses,
  parse_integer_dash_fraction,
  parse_feet_inches,
  parse_arc_degrees_minutes_seconds,
  parse_surveyors_notation,

  parse_full_stop_as_decimal_point,

  parse_hyphen_minus_as_number_dash,
  parse_whitespace_between_value_and_unit_system,
  parse_whitespace_between_feet_and_inches,

  // interior, leading, and trailing white space
  parse_horizontal_tab_as_whitespace,
  parse_space_as_whitespace,
  parse_nobreak_space_as_whitespace,
  parse_thin_space_as_whitespace,
  parse_nobreak_thin_space_as_whitespace,

  // leading white space
  parse_form_feed_as_leading_whitespace,
  parse_carriage_return_as_leading_whitespace,
  parse_line_feed_as_leading_whitespace,
  parse_vertical_tab_as_leading_whitespace,

  // trailing white space
  parse_form_feed_as_trailing_whitespace,
  parse_carriage_return_as_trailing_whitespace,
  parse_line_feed_as_trailing_whitespace,
  parse_vertical_tab_as_trailing_whitespace,

  // unary minus signs
  parse_hyphen_minus_as_unary_minus,
  parse_modifier_letter_minus_sign_as_unary_minus,
  parse_heavy_minus_sign_as_unary_minus,
  parse_small_hyphen_minus_as_unary_minus,
  parse_fullwidth_hyphen_minus_as_unary_minus,

  // unary plus signs
  parse_plus_as_unary_plus,
  parse_heavy_plus_as_unary_plus,
  parse_small_plus_as_unary_plus,
  parse_fullwidth_plus_as_unary_plus,

  // maximum 64 true default settings
};

enum bitdex_false_default
{
  parse_d_as_e_in_scientific_e_notation,

  parse_comma_as_decimal_point,

  parse_full_stop_as_digit_separator,
  parse_comma_as_digit_separator,
  parse_space_as_digit_separator,
  parse_thin_space_as_digit_separator,
  parse_nobreak_space_as_digit_separator,
  parse_nobreak_thin_space_as_digit_separator,

  parse_invalid_expression_error,
  parse_divide_by_zero_error,
  parse_overflow_error,

  parse_hyphen_as_number_dash,
  parse_nobreak_hyphen_as_number_dash,

  // maximum 64 false default settings
};

bool MYON_ParseSettings::ParseLeadingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_leading_white_space);
}

bool MYON_ParseSettings::ParseArithmeticExpression() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_arithmetic_expression);
}

bool MYON_ParseSettings::ParseMathFunctions() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_math_functions);
}

bool MYON_ParseSettings::ParseExplicitFormulaExpression() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_explicit_formula_expression);
}

bool MYON_ParseSettings::ParseUnaryMinus() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_unary_minus);
}

bool MYON_ParseSettings::ParseUnaryPlus() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_unary_plus);
}

bool MYON_ParseSettings::ParseSignificandIntegerPart() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_integer_part);
}

bool MYON_ParseSettings::ParseSignificandDecimalPoint() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_decimal_point);
}

bool MYON_ParseSettings::ParseSignificandFractionalPart() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_fractional_part);
}

bool MYON_ParseSettings::ParseSignificandDigitSeparators() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_digit_separators);
}

bool MYON_ParseSettings::ParseDAsExponentInScientificENotation() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_d_as_e_in_scientific_e_notation);
}

bool MYON_ParseSettings::ParseScientificENotation() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_scientific_e_notation);
}

bool MYON_ParseSettings::ParseFullStopAsDecimalPoint() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_full_stop_as_decimal_point);
}

bool MYON_ParseSettings::ParseCommaAsDecimalPoint() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_comma_as_decimal_point);
}

bool MYON_ParseSettings::ParseFullStopAsDigitSeparator() const
{
  if ( ParseFullStopAsDecimalPoint() )
    return false;
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_full_stop_as_digit_separator);
}

bool MYON_ParseSettings::ParseCommaAsDigitSeparator() const
{
  if ( ParseCommaAsDecimalPoint() )
    return false;
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_comma_as_digit_separator);
}

bool MYON_ParseSettings::ParseSpaceAsDigitSeparator() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_space_as_digit_separator);
}

bool MYON_ParseSettings::ParseThinSpaceAsDigitSeparator() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_thin_space_as_digit_separator);
}

bool MYON_ParseSettings::ParseNoBreakSpaceAsDigitSeparator() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_nobreak_space_as_digit_separator);
}

bool MYON_ParseSettings::ParseNoBreakThinSpaceAsDigitSeparator() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_nobreak_thin_space_as_digit_separator);
}

bool MYON_ParseSettings::ParseHyphenMinusAsNumberDash() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_hyphen_minus_as_number_dash);
}

bool MYON_ParseSettings::ParseHyphenAsNumberDash() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_hyphen_as_number_dash);
}

bool MYON_ParseSettings::ParseNoBreakHyphenAsNumberDash() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_nobreak_hyphen_as_number_dash);
}

bool MYON_ParseSettings::ParseRationalNumber() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_rational_number);
}

bool MYON_ParseSettings::ParsePi() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_pi);
}

bool MYON_ParseSettings::ParseMultiplication() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_multiplication);
}

bool MYON_ParseSettings::ParseDivision() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_division);
}


bool MYON_ParseSettings::ParseAddition() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_addition);
}

bool MYON_ParseSettings::ParseSubtraction() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_subtraction);
}

bool MYON_ParseSettings::ParsePairedParentheses() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_paired_parentheses);
}

bool MYON_ParseSettings::ParseIntegerDashFraction() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_integer_dash_fraction);
}

bool MYON_ParseSettings::ParseFeetInches() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_feet_inches);
}

bool MYON_ParseSettings::ParseArcDegreesMinutesSeconds() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_arc_degrees_minutes_seconds);
}

bool MYON_ParseSettings::ParseWhiteSpaceBetweenValueAndUnitSystem() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_whitespace_between_value_and_unit_system);
}

bool MYON_ParseSettings::ParseWhiteSpaceBetweenFeetAndInches() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_whitespace_between_feet_and_inches);
}

bool MYON_ParseSettings::ParseSurveyorsNotation() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_surveyors_notation);
}

unsigned int MYON_ParseSettings::ContextLocaleId() const
{
  return m_context_locale_id;
}

unsigned int MYON_ParseSettings::PreferedLocaleId() const
{
  return ContextLocaleId();
}

MYON::LengthUnitSystem MYON_ParseSettings::ContextLengthUnitSystem() const
{
  return MYON::LengthUnitSystemFromUnsigned(m_context_length_unit_system);
}

MYON::AngleUnitSystem MYON_ParseSettings::ContextAngleUnitSystem() const
{
  return
    (m_context_angle_unit_system > 0)
    ? MYON::AngleUnitSystemFromUnsigned(m_context_angle_unit_system)
    : MYON::AngleUnitSystem::Radians;
}

MYON::AngleUnitSystem MYON_ParseSettings::DefaultAngleUnitSystem() const
{
  return ContextAngleUnitSystem();
}

void MYON_ParseSettings::SetParseLeadingWhiteSpace(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_leading_white_space,bParse);
}

void MYON_ParseSettings::SetParseArithmeticExpression(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_arithmetic_expression,bParse);
}

void MYON_ParseSettings::SetParseMathFunctions(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_math_functions,bParse);
}

void MYON_ParseSettings::SetParseExplicitFormulaExpression(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_explicit_formula_expression,bParse);
}

void MYON_ParseSettings::SetParseUnaryMinus(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_unary_minus,bParse);
}

void MYON_ParseSettings::SetParseUnaryPlus(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_unary_plus,bParse);
}

void MYON_ParseSettings::SetParseSignificandIntegerPart(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_integer_part,bParse);
}

void MYON_ParseSettings::SetParseSignificandDecimalPoint(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_decimal_point,bParse);
}

void MYON_ParseSettings::SetParseSignificandFractionalPart(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_fractional_part,bParse);
}

void MYON_ParseSettings::SetParseSignificandDigitSeparators(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_significand_digit_separators,bParse);
}

void MYON_ParseSettings::SetParseDAsExponentInScientificENotation(bool bParse)
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_d_as_e_in_scientific_e_notation,bParse);
}

void MYON_ParseSettings::SetParseScientificENotation(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_scientific_e_notation,bParse);
}

void MYON_ParseSettings::SetParseFullStopAsDecimalPoint( bool bParse )
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_full_stop_as_decimal_point,bParse);
}

void MYON_ParseSettings::SetParseFullStopAsDigitSeparator( bool bParse )  
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_full_stop_as_digit_separator,bParse);
}

void MYON_ParseSettings::SetParseCommaAsDecimalPoint( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_comma_as_decimal_point,bParse);
}

void MYON_ParseSettings::SetParseCommaAsDigitSeparator( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_comma_as_digit_separator,bParse);
}

void MYON_ParseSettings::SetParseSpaceAsDigitSeparator( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_space_as_digit_separator,bParse);
}

void MYON_ParseSettings::SetParseThinSpaceAsDigitSeparator( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_thin_space_as_digit_separator,bParse);
}

void MYON_ParseSettings::SetParseNoBreakSpaceAsDigitSeparator( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_nobreak_space_as_digit_separator,bParse);
}

void MYON_ParseSettings::SetParseNoBreakThinSpaceAsDigitSeparator( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_nobreak_thin_space_as_digit_separator,bParse);
}

void MYON_ParseSettings::SetParseHyphenMinusAsNumberDash( bool bParse )
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_hyphen_minus_as_number_dash,bParse);
}

void MYON_ParseSettings::SetParseHyphenAsNumberDash( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_hyphen_as_number_dash,bParse);
}

void MYON_ParseSettings::SetParseNoBreakHyphenAsNumberDash( bool bParse )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_nobreak_hyphen_as_number_dash,bParse);
}


void MYON_ParseSettings::SetParseRationalNumber(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_rational_number,bParse);
}


void MYON_ParseSettings::SetParsePi(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_pi,bParse);
}

void MYON_ParseSettings::SetParseMultiplication(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_multiplication,bParse);
}

void MYON_ParseSettings::SetParseDivision(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_division,bParse);
}

void MYON_ParseSettings::SetParseAddition(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_addition,bParse);
}

void MYON_ParseSettings::SetParseSubtraction(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_subtraction,bParse);
}

void MYON_ParseSettings::SetParsePairedParentheses(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_paired_parentheses,bParse);
}

void MYON_ParseSettings::SetParseIntegerDashFraction(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_integer_dash_fraction,bParse);
}

void MYON_ParseSettings::SetParseFeetInches(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_feet_inches,bParse);
}

void MYON_ParseSettings::SetParseArcDegreesMinutesSeconds(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_arc_degrees_minutes_seconds,bParse);
}


void MYON_ParseSettings::SetParseWhiteSpaceBetweenValueAndUnitSystem(
  bool bParseWhiteSpaceBetweenValueAndUnitSystem
)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_whitespace_between_value_and_unit_system,bParseWhiteSpaceBetweenValueAndUnitSystem);
}


void MYON_ParseSettings::SetParseWhiteSpaceBetweenFeetAndInches(
  bool bParseWhiteSpaceBetweenFeetAndInches
)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_whitespace_between_feet_and_inches,bParseWhiteSpaceBetweenFeetAndInches);
}

void MYON_ParseSettings::SetParseSurveyorsNotation(bool bParse)
{
  MYON_SetParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_surveyors_notation,bParse);
}

void MYON_ParseSettings::SetContextLocaleId(
  unsigned int context_locale_id
  )
{
  m_context_locale_id = (context_locale_id <= 0xFFFF)
                       ? ((unsigned short)context_locale_id)
                       : 0;
}

void MYON_ParseSettings::SetPreferedLocaleId(
  unsigned int context_locale_id
  )
{
  SetContextLocaleId(context_locale_id);
}

void MYON_ParseSettings::SetContextLengthUnitSystem(
  MYON::LengthUnitSystem context_length_unit_system
  )
{
  for (;;)
  {
    if (MYON::LengthUnitSystem::None == context_length_unit_system || MYON::LengthUnitSystem::Unset == context_length_unit_system)
      break;
    const unsigned int length_unit_system_as_unsigned = static_cast<unsigned int>(context_length_unit_system);
    if (length_unit_system_as_unsigned > 0xFFU)
      break;
    if (context_length_unit_system != MYON::LengthUnitSystemFromUnsigned(length_unit_system_as_unsigned))
      break;
    m_context_length_unit_system = (MYON__UINT8)length_unit_system_as_unsigned;
    return;
  }
  m_context_length_unit_system = 0; // no length unit system
}

void MYON_ParseSettings::SetContextAngleUnitSystem(
  MYON::AngleUnitSystem context_angle_unit_system
  )
{
  for (;;)
  {
    if (MYON::AngleUnitSystem::None == context_angle_unit_system || MYON::AngleUnitSystem::Unset == context_angle_unit_system)
      break;
    const unsigned int angle_unit_system_as_unsigned = static_cast<unsigned int>(context_angle_unit_system);
    if (angle_unit_system_as_unsigned > 0xFFU)
      break;
    if (context_angle_unit_system != MYON::AngleUnitSystemFromUnsigned(angle_unit_system_as_unsigned))
      break;
    m_context_angle_unit_system = (MYON__UINT8)angle_unit_system_as_unsigned;
    return;
  }
  m_context_angle_unit_system = 0; // radians
}

void MYON_ParseSettings::SetDefaultAngleUnitSystem(
  MYON::AngleUnitSystem context_angle_unit_system
  )
{
  SetContextAngleUnitSystem(context_angle_unit_system);
}

bool MYON_ParseSettings::ParseSpaceAsWhiteSpace() const 
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_space_as_whitespace);
}

bool MYON_ParseSettings::ParseNoBreakSpaceAsWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_nobreak_space_as_whitespace);
}

bool MYON_ParseSettings::ParseHorizontalTabAsWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_horizontal_tab_as_whitespace);
}

bool MYON_ParseSettings::ParseThinSpaceAsWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_thin_space_as_whitespace);
}

bool MYON_ParseSettings::ParseNoBreakThinSpaceAsWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_nobreak_thin_space_as_whitespace);
}

bool MYON_ParseSettings::ParseLineFeedAsLeadingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_line_feed_as_leading_whitespace);
}

bool MYON_ParseSettings::ParseFormFeedAsLeadingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_form_feed_as_leading_whitespace);
}

bool MYON_ParseSettings::ParseCarriageReturnAsLeadingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_carriage_return_as_leading_whitespace);
}

bool MYON_ParseSettings::ParseVerticalTabAsLeadingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_vertical_tab_as_leading_whitespace);
}

bool MYON_ParseSettings::ParseLineFeedAsTrailingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_line_feed_as_trailing_whitespace);
}

bool MYON_ParseSettings::ParseFormFeedAsTrailingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_form_feed_as_trailing_whitespace);
}

bool MYON_ParseSettings::ParseCarriageReturnAsTrailingWhiteSpace() const 
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_carriage_return_as_trailing_whitespace);
}

bool MYON_ParseSettings::ParseVerticalTabAsTrailingWhiteSpace() const
{
  return MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_vertical_tab_as_trailing_whitespace);
}

void MYON_ParseSettings::SetParseSpaceAsWhiteSpace( bool bParseAsWhiteSpace ) 
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_space_as_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseNoBreakSpaceAsWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_nobreak_space_as_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseHorizontalTabAsWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_horizontal_tab_as_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseThinSpaceAsWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_thin_space_as_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseNoBreakThinSpaceAsWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_nobreak_thin_space_as_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseLineFeedAsLeadingWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_line_feed_as_leading_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseFormFeedAsLeadingWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_form_feed_as_leading_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseCarriageReturnAsLeadingWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_carriage_return_as_leading_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseVerticalTabAsLeadingWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_vertical_tab_as_leading_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseLineFeedAsTrailingWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_line_feed_as_trailing_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseFormFeedAsTrailingWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_form_feed_as_trailing_whitespace, bParseAsWhiteSpace );
}

void MYON_ParseSettings::SetParseCarriageReturnAsTrailingWhiteSpace( bool bParseAsWhiteSpace ) 
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_carriage_return_as_trailing_whitespace, bParseAsWhiteSpace );
}


void MYON_ParseSettings::SetParseVerticalTabAsTrailingWhiteSpace( bool bParseAsWhiteSpace )
{
  MYON_SetParseNumberSettingsBitTrueDefault( bitdex_true_default::parse_vertical_tab_as_trailing_whitespace, bParseAsWhiteSpace );
}

bool MYON_ParseSettings::IsLeadingWhiteSpace(MYON__UINT32 c) const
{
  if (IsInteriorWhiteSpace(c))
    return true;

  switch(c)
  {
  case 0x000A: // line feed
    if ( ParseLineFeedAsLeadingWhiteSpace() )
      return true;
    break;

  case 0x000B: // vertical tab
    if ( ParseVerticalTabAsLeadingWhiteSpace() )
      return true;
    break;

  case 0x000C: // form feed
    if ( ParseFormFeedAsLeadingWhiteSpace() )
      return true;
    break;

  case 0x000D: // carriage return
    if ( ParseCarriageReturnAsLeadingWhiteSpace() )
      return true;
    break;
  }

  return false;
}

bool MYON_ParseSettings::IsTrailingWhiteSpace(MYON__UINT32 c) const
{
  if (IsInteriorWhiteSpace(c))
    return true;

  switch(c)
  {
  case 0x000A: // line feed
    if ( ParseLineFeedAsTrailingWhiteSpace() )
      return true;
    break;

  case 0x000B: // vertical tab
    if ( ParseVerticalTabAsTrailingWhiteSpace() )
      return true;
    break;

  case 0x000C: // form feed
    if ( ParseFormFeedAsTrailingWhiteSpace() )
      return true;
    break;

  case 0x000D: // carriage return
    if ( ParseCarriageReturnAsTrailingWhiteSpace() )
      return true;
    break;
  }

  return false;}


bool MYON_ParseSettings::IsInteriorWhiteSpace(MYON__UINT32 c) const
{
  switch(c)
  {
  case 0x0009: // horizontal tab
    if ( ParseHorizontalTabAsWhiteSpace() )
      return true;
    break;

  case 0x0020: // space
    if ( ParseSpaceAsWhiteSpace() )
      return true;

  case 0x00A0: // no-break space
    return ParseNoBreakSpaceAsWhiteSpace();
    break;

  case 0x2008: // thin space
  case 0x2009: // thin space
  case 0x200A: // hair thin space
    return ParseThinSpaceAsWhiteSpace();
    break;

  case 0x202F: // narrow no-break
    return ParseNoBreakThinSpaceAsWhiteSpace();
    break;
  }

  return false;
}


bool MYON_ParseSettings::IsUnaryMinus(MYON__UINT32 c) const
{
  switch(c)
  {
  case 0x002D: // hyphen-minus
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_hyphen_minus_as_unary_minus) )
      return true;
    break;

  case 0x02D7: // modifier letter minus sign
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_modifier_letter_minus_sign_as_unary_minus) )
      return true;
    break;

  case 0x2796: // heavy minus sign
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_heavy_minus_sign_as_unary_minus) )
      return true;
    break;

  case 0xFE63: // small hyphen-minus
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_small_hyphen_minus_as_unary_minus) )
      return true;
    break;

  case 0xFF0D: // fullwidth hyphen-minus
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_fullwidth_hyphen_minus_as_unary_minus) )
      return true;
    break;
  }
  return false;
}

bool MYON_ParseSettings::IsUnaryPlus(MYON__UINT32 c) const
{
  switch(c)
  {
  case 0x002B: // plus
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_plus_as_unary_plus) )
      return true;
    break;

  case 0x2795: // heavy plus sign
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_heavy_plus_as_unary_plus) )
      return true;
    break;

  case 0xFE62: // small plus sign
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_small_plus_as_unary_plus) )
      return true;
    break;

  case 0xFE0B: // fullwidth plus sign
    if ( MYON_ParseNumberSettingsBitTrueDefault(bitdex_true_default::parse_fullwidth_plus_as_unary_plus) )
      return true;
    break;
  }
  return false;
}

char MYON_ParseSettings::IsDigit(MYON__UINT32 c) const
{
  return ( c >= '0' && c <= '9' ) ? ((char)c) : 0;
}

bool MYON_ParseSettings::IsDigitSeparator(MYON__UINT32 c) const
{
  if ( IsDecimalPoint(c) )
    return false; // prohibit the same symbol from being used as a decimal point

  switch(c)
  {
  case 0x002C: // comma
    return ParseCommaAsDigitSeparator(); // default = true
    break;

  case 0x002E: // full stop (period)
    return ParseFullStopAsDigitSeparator(); // default = false
    break;

  case 0x0020: // space
    return ParseSpaceAsDigitSeparator(); // default = true
    break;

  case 0x00A0: // no-break space
    return ParseNoBreakSpaceAsDigitSeparator();
    break;

  case 0x2008: // thin space
  case 0x2009: // thin space
  case 0x200A: // hair thin space
    return ParseThinSpaceAsDigitSeparator();
    break;

  case 0x202F: // narrow no-break
    return ParseNoBreakThinSpaceAsDigitSeparator();
    break;

  case 0x066C: // UNICODE ARABIC THOUSANDS SEPARATOR
    // Intended to be used with eastern arabic numerials, 
    // but its lexical function is always a thousands separator.
    return false;
    break;

  }

  return false;
}

bool MYON_ParseSettings::IsDecimalPoint(MYON__UINT32 c) const
{
  switch(c)
  {
  case 0x002C: // comma
    return ParseCommaAsDecimalPoint();
    break;

  case 0x002E: // full stop (period)
    return ParseFullStopAsDecimalPoint();
    break;

  case 0x2396: // UNICODE DECIMAL SEPARATOR KEY SYMBOL
    // Intended to be used as a symbol on keyboards,
    // but its lexical function is always a decimal point.
    return true;
    break;

  case 0x066B: // UNICODE ARABIC DECIMAL SEPARATOR
    // Intended to be used with eastern arabic numerials, 
    // but its lexical function is always a decimal point.
    return true;
    break;
  }
  return false;
}


bool MYON_ParseSettings::IsDecimalExponentSymbol(MYON__UINT32 c) const
{
  switch(c)
  {
  case 'e': // lower case e
  case 'E': // capital E
  case 0x23E8: // decimal exponent symbol
    return true;
    break;

  case 'D': // capital D
  case 'd': // lower case d
    return ParseDAsExponentInScientificENotation();
    break;
  }
  return false;
}

bool MYON_ParseSettings::IsGreekPi(MYON__UINT32 c) const
{
  switch(c)
  {
  case 0x03A:
  case 0x03C:
    return true;
    break;
  }

  return false;
}

bool MYON_ParseSettings::IsMultiplicationSymbol(MYON__UINT32 c) const
{
  switch(c)
  {
  case '*':
  case 0x00D7: // unicode multiply by symbol
    return true;
    break;
  }

  return false;
}


bool MYON_ParseSettings::IsDivisionSymbol(MYON__UINT32 c) const
{
  switch(c)
  {
  case '/':
  case 0x00F7: // unicode divide by symbol
    return true;
    break;
  }

  return false;
}


bool MYON_ParseSettings::IsAdditionSymbol(MYON__UINT32 c) const
{
  switch(c)
  {
  case 0x002B: // unicode plus by symbol
    return true;
    break;
  }

  return false;
}

bool MYON_ParseSettings::IsSubtractionSymbol(MYON__UINT32 c) const
{
  switch(c)
  {
  case '-': // unicode hyphen minus
  case 0x2212: // unicode minus sign
    return true;
    break;
  }

  return false;
}

bool MYON_ParseSettings::IsLeftParenthesisSymbol(MYON__UINT32 c) const
{
  return ('(' == c);
}

bool MYON_ParseSettings::IsRightParenthesisSymbol(MYON__UINT32 c) const
{
  return (')' == c);
}

bool MYON_ParseSettings::IsRationalNumberFractionBar(MYON__UINT32 c) const
{
  return ('/' == c);
}

bool MYON_ParseSettings::IsNumberDash(MYON__UINT32 c) const
{
  switch(c)
  {
  case 0x002D: // hyphen-minus '-' <- BAD CHOICE, but very common because it's the easiest to type
    return this->ParseHyphenMinusAsNumberDash();
    break;
  case 0x2010: // unicode hyphen
    return this->ParseHyphenAsNumberDash();
    break;
  case 0x2011: // unicode non-breaking hyphen
    return this->ParseNoBreakHyphenAsNumberDash();
    break;
  case 0x2012: // unicode figure dash <- GOOD CHOICE See http://en.wikipedia.org/wiki/Dash
    return true;
    break;

  // NOTES:
  //  THe en dash 0x2013 is not offered as an option because it
  //  is commonly used as a minus sign because its with is the
  //  same as a plus sign.
  //  The em dash 0x2014 is not offered as an option because its
  //  length makes it unlikely that it would be used to indicate
  //  the dash in 1-3/4.
  //  See http://en.wikipedia.org/wiki/Dash for more information.
  }

  return false;
}

bool MYON_ParseSettings::ParseError() const
{
  return    ParseDivideByZeroError() 
         || ParseOverflowError()
         || ParseInvalidExpressionError()
         ;
}

bool MYON_ParseSettings::ParseDivideByZeroError() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_divide_by_zero_error);
}

bool MYON_ParseSettings::ParseOverflowError() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_overflow_error);
}

bool MYON_ParseSettings::ParseInvalidExpressionError() const
{
  return MYON_ParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_invalid_expression_error);
}

void MYON_ParseSettings::SetParseDivideByZeroError(
  bool bParseDivideByZeroError
  )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_divide_by_zero_error,bParseDivideByZeroError);
}

void MYON_ParseSettings::SetParseOverflowError(
  bool bParseOverflowError
  )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_overflow_error,bParseOverflowError);
}

void MYON_ParseSettings::SetParseInvalidExpressionError(
  bool bParseInvalidExpressionError
  )
{
  MYON_SetParseNumberSettingsBitFalseDefault(bitdex_false_default::parse_invalid_expression_error,bParseInvalidExpressionError);
}


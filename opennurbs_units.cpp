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

#include "opennurbs_parse.h"

#define MYON_UNICODE_QUOTATION_MARK 0x0022
#define MYON_WCHAR_QUOTATION_MARK L"\""

#define MYON_UNICODE_APOSTROPHE 0x0027
#define MYON_WCHAR_APOSTROPHE L"\'"

#define MYON_UNICODE_LOW_LINE 0x005F

#define MYON_UNICODE_DEGREE_SYMBOL 0x00B0
#define MYON_WCHAR_DEGREE_SYMBOL L"\x00B0"

// The unicode masuline ordinal indicator (Spanish) 
// is often mistakenlyused as a degrees symbol.
#define MYON_UNICODE_MASCULINE_ORDINAL_INDICATOR 0x00BA
#define MYON_WCHAR_MASCULINE_ORDINAL_INDICATOR L"\x00BA"

#define MYON_UNICODE_LATIN_SMALL_LETTER_SHARP_S 0x00DF
#define MYON_WCHAR_LATIN_SMALL_LETTER_SHARP_S L"\x00DF"

#define MYON_UNICODE_LATIN_CAPITAL_LETTER_U_WITH_RING_ABOVE 0x016E
#define MYON_WCHAR_LATIN_CAPITAL_LETTER_U_WITH_RING_ABOVE L"\x016E"

#define MYON_UNICODE_LATIN_SMALL_LETTER_U_WITH_RING_ABOVE 0x016F
#define MYON_WCHAR_LATIN_SMALL_LETTER_U_WITH_RING_ABOVE L"\x016F"

#define MYON_UNICODE_LATIN_CAPITAL_LETTER_SHARP_S 0x1E9E

#define MYON_UNICODE_GREEK_CAPITAL_LETTER_TAU 0x03A4

#define MYON_UNICODE_GREEK_SMALL_LETTER_TAU 0x03C4
#define MYON_WCHAR_GREEK_SMALL_LETTER_TAU L"\x03C4"

#define MYON_RHINO_LOCALE_ID 1
#define MYON_INVALID_LOCALE_ID 2
#define MYON_CS_CZ_LOCALE_ID MYON_Locale::WindowsLCID::cs_CZ_LCID
#define MYON_DE_DE_LOCALE_ID MYON_Locale::WindowsLCID::de_DE_LCID
#define MYON_EN_CA_LOCALE_ID MYON_Locale::WindowsLCID::en_CA_LCID
#define MYON_EN_US_LOCALE_ID MYON_Locale::WindowsLCID::en_US_LCID
#define MYON_ES_ES_LOCALE_ID MYON_Locale::WindowsLCID::es_ES_LCID
#define MYON_FR_FR_LOCALE_ID MYON_Locale::WindowsLCID::fr_FR_LCID
#define MYON_IT_IT_LOCALE_ID MYON_Locale::WindowsLCID::it_IT_LCID
#define MYON_PL_PL_LOCALE_ID MYON_Locale::WindowsLCID::pl_PL_LCID
#define MYON_PT_PT_LOCALE_ID MYON_Locale::WindowsLCID::pt_PT_LCID

class MYON_UnitName
{
public:
  // The size and field alignment of MYON_UnitName must exactly match
  // that of MYON_LengthUnitName and MYON_AngleUnitName
  // Microsoft locale id // http://msdn.microsoft.com/en-us/library/ms912047(v=winembedded.10).aspx
  unsigned int m_locale_id; 

  // MYON::LengthUnitSystem or MYON::AngleUnitSystem enum value
  unsigned char m_unit_system;       

  bool m_bIsSingular;
  bool m_bIsPlural;

  const wchar_t* m_name;

};

class MYON_UnitNameEx : public MYON_UnitName
{
public:


  enum
  {
    m_utf32_name_capacity = 24
  };

  // Simplified name - used for speedy unit name searches.
  MYON__UINT32 m_utf32_name[MYON_UnitNameEx::m_utf32_name_capacity]; // null terminated utf-32 encoded lower case simple latin name string.
  unsigned int m_utf32_name_count; // number of nonzero wchar_t elements in m_name[] array

  /*
  Description:
    Parses m_name and sets the m_utf32_name[] and m_utf32_name_count
    fields.
  Returns:
    Number of elments of m_name that were parsed; 0 indicates failure.
  Remarks:
    m_name must contain a character that terminates unit system name parsing.
    This can be a null, digit, punctuation, aritmetic operator, or a 
    unicode code point <= 0x0020 (0x0020 = space = 32 decimal).
  */
  int SetSimplifiedName();

  static int Compare(const MYON_UnitNameEx*, const MYON_UnitNameEx*);
  static int Compare_m_utf32_name(const MYON_UnitNameEx*, const MYON_UnitNameEx*);
  static int Compare_m_name(const MYON_UnitNameEx*, const MYON_UnitNameEx*);
};

static MYON_UnitName si_length_units[] =
{
  //////////////////////////////////////////////////////////////
  //
  // SI length units universal
  //
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, true, L"mm" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, true, L"cm" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, true,      L"m" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, true,  L"km" },

  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, true, L"_mm" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, true, L"_cm" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, true,      L"_m" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, true,  L"_km" },

  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"_millimeters" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"_millimeter" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"_centimeters" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"_centimeter" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"_meters" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"_meter"},
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"_kilometers" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"_kilometer" },

  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"_millimetres" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"_millimetre" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"_centimetres" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"_centimetre" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"_metres" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"_metre" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"_kilometres" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"_kilometre" },


  //////////////////////////////////////////////////////////////
  //
  // SI length units cs-* Czech
  //
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"milimetr" MYON_WCHAR_LATIN_SMALL_LETTER_U_WITH_RING_ABOVE },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"milimetry" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"milimetr" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centimetr" MYON_WCHAR_LATIN_SMALL_LETTER_U_WITH_RING_ABOVE },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centimetry" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"centimetr" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"metr" MYON_WCHAR_LATIN_SMALL_LETTER_U_WITH_RING_ABOVE },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"metry" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"metr" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"kilometr" MYON_WCHAR_LATIN_SMALL_LETTER_U_WITH_RING_ABOVE },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"kilometry" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"kilometr" },

  //////////////////////////////////////////////////////////////
  //
  // SI length units de-* German
  //
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, true, L"Millimeter" },
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, true, L"Zentimeter" },
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, true,      L"Meter" },
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, true,  L"Kilometer" },
  // http://mcneel.myjetbrains.com/youtrack/issue/RH-34051
  // The words ending in n are not plural forms, they are inflected forms used in complete sentences.
  // They are included so information copied, possibly automatically, from documents into scripts will parse as expected.
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, true, L"Millimetern" },
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, true, L"Zentimetern" },
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, true,      L"Metern" },
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, true,  L"Kilometern" },
  // http://mcneel.myjetbrains.com/youtrack/issue/RH-34051
  // "zm" is not the way German speakers abbreviate zentimeter
  // NO // {MYON_DE_DE_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, true,  L"zm" },

  //////////////////////////////////////////////////////////////
  //
  // SI length units en-CA (metric Canadian English)
  //
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"millimetres" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"millimetre" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centimetres" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"centimetre" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"metres" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"metre" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"kilometres" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"kilometre" },

  //////////////////////////////////////////////////////////////
  //
  // SI length units en-US (metric American English)
  //
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"millimeters" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"millimeter" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centimeters" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"centimeter" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"meters" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"meter" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"kilometers" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"kilometer" },

  //////////////////////////////////////////////////////////////
  //
  // SI length units es-* Spanish
  //
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"mil\x00EDmetros" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"mil\x00EDmetro" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"cent\x00EDmetros" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"cent\x00EDmetro" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true, L"metros" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false, L"metro" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true, L"kil\x00F3metros" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false, L"kil\x00F3metro" },

  //////////////////////////////////////////////////////////////
  //
  // SI length units fr-* French
  //
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"millim\x00E8tres" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"millim\x00E8tre" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centim\x00E8tres" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"centim\x00E8tre" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"m\x00E8tres" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"m\x00E8tre" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"kilom\x00E8tres" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"kilom\x00E8tre" },

  //////////////////////////////////////////////////////////////
  //
  // SI length units it-* Italian
  //
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"millimetri" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"millimetro" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centimetri" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"centimetro" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"metri" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false ,     L"metro"},
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"chilometri" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"chilometro" },
  // it-* abbreviation for chilometri is "km"

  //////////////////////////////////////////////////////////////
  //
  // SI length units pl-* Polish
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"milimetr" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"milimetr\x00F3w" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"milimetry" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"centymetr" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centymetr\x00F3w" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"centymetry" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"metr" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"metr\x00F3w" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"metry" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"kilometr" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"kilometr\x00F3w" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"kilometry" },

  //////////////////////////////////////////////////////////////
  //
  // SI length units pt-* Portuguese
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), false, true, L"mil\x00EDmetros" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Millimeters), true, false, L"mil\x00EDmetro" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), false, true, L"cent\x00EDmetros" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Centimeters), true, false, L"cent\x00EDmetro" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), false, true,      L"metros" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Meters), true, false,      L"metro" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), false, true,  L"quil\x00F4metros" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, false,  L"quil\x00F4metro" },
  // "qm" uilometro is abbreviated in Brazil and Portugual
  // NO // {MYON_PT_PT_LOCALE_ID, static_cast<unsigned char>(MYON::LengthUnitSystem::Kilometers), true, true,   L"qm" },
};

static MYON_UnitName en_US_customary_length_units[] =
{
  //////////////////////////////////////////////////////////////
  //
  // United States customary units (Rhino - all locales)
  //
  //   All conversions to meters are exact.
  //   microinch =    2.54e-8 meters (1.0e-6 inches)
  //   mil       =    2.54e-5 meters (0.001 inches)
  //   inch      =    0.0254  meters (1/12 foot)
  //   foot      =    0.3048  meters (12 inches)
  //   yard      =    0.9144  meters (3 feet, 36 inches)
  //   mile      = 1609.344   meters (5280 feet)
  //
  // United States customary units are supported in en-US for two reasons.
  // 1) There is a internationally accepted definition of US customary
  //    lengths in terms of meters.  The international yard and pound
  //    agreement of July 1, 1959 defined one yard to be exactly 
  //    0.9144 meters which implies one inch is exactly 0.0254 meters.
  //    http://www.ngs.noaa.gov/PUBS_LIB/FedRegister/FRdoc59-5442.pdf
  // 2) These customary units are currently and widely used in the 
  //    United States. 
  //
  // United States customary units are not supported in any other
  // language or culture because there are no internationally 
  // accepted and commonly used definitions in terms of meters
  // that match the ones used in the United States and the units
  // are not commonly used in precise digital models.
  //
  // For an inlking of the peril of assuming a length unit in
  // Czech, French, Italian, Portuguese or Spanish that linguistically
  // translates to English "inch", "foot", or "mile" has the same physical
  // length as the "United States customary unit", see the references below.
  //   http://en.wikipedia.org/wiki/Foot_(unit)
  //   http://en.wikipedia.org/wiki/German_obsolete_units_of_measurement
  //   http://en.wikipedia.org/wiki/Spanish_customary_units
  //   http://en.wikipedia.org/wiki/Portuguese_customary_units
  //   http://en.wikipedia.org/wiki/Units_of_measurement_in_France
  //   http://www.convert-me.com/en/convert/length
  //   http://www.wordreference.com/czen/palec
  //   http://www.onlineunitconversion.com/braccio.Italy_to_miglio.html
  //
  // All conversions to meters and other US customary units are exact.
  //
  // 1 microinch = 2.54e-8 meters (1.0e-6 inches)
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Microinches), false, true, L"microinches" }, 
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Microinches), true, false, L"microinch" },
  // 1 mil = 2.54e-5 meters (0.001 inches)
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Mils), false, true, L"mils" },     
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Mils), true, false, L"mil" },
  // 1 inch = 0.0254 meters = 1/12 foot
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), false, true, L"inches" }, 
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), true, false, L"inch" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), false, true, L"ins" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), true, true, L"in" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), true, true, MYON_WCHAR_QUOTATION_MARK },
  // 1 foot = 0.3048 meters = 12 inches
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Feet), false, true, L"feet" },     
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Feet), true, false, L"foot" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Feet), true, true, L"ft" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Feet), true, true, MYON_WCHAR_APOSTROPHE },
  // 1 yard = 0.9144 meters = 3 feet = 36 inches
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), false, true, L"yards" },   
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), true, false, L"yard" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), false, true, L"yds" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), true, true, L"yd" },    
  // 1 US statute mile = 1609.34 meters = 5280 feet
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Miles), false, true, L"miles" }, 
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Miles), true, false, L"mile" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Miles), true, true, L"mi" },

  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Microinches), true, true, L"_microinches" }, 
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Microinches), true, true, L"_microinch" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Mils), false, true, L"_mils" },     
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Mils), true, false, L"_mil" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), false, true, L"_inches" }, 
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), true, false, L"_inch" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), false, true, L"_ins" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Inches), true, true, L"_in" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Feet), false, true, L"_feet" },     
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Feet), true, false, L"_foot" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Feet), true, true, L"_ft" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), false, true, L"_yards" },   
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), true, false, L"_yard" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), false, true, L"_yds" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Yards), true, true, L"_yd" },    
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Miles), false, true, L"_miles" }, 
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Miles), true, false, L"_mile" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::LengthUnitSystem::Miles), true, true, L"_mi" },
};

static MYON_UnitName angle_no_units[] =
{
  // These entries prevent parsing the strings unless an
  // entry for a locale explicitly inludes the string and
  // the parsing prefered local id matches exactly.
  // The purpose is to prevent incorrectly parsing strings
  // the define different unit systems in different 
  // locales.

  // Many strings that begin with "g" can mean degrees
  // in one locale and gradians in another locale.
  {MYON_INVALID_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::None), true, true,  L"g" },
  {MYON_INVALID_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::None), true, true,  L"gs" },
  {MYON_INVALID_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::None), true, true,  L"gc" },
  {MYON_INVALID_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::None), true, true,  L"gd" },
  {MYON_INVALID_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::None), true, true, L"grad" },
  {MYON_INVALID_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::None), true, true, L"grads" },

  // "s" could mean degrees in Czech or Polish and seconds in English.
  {MYON_INVALID_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::None), true, true,  L"s" },
};

static MYON_UnitName angle_radian_units[] =
{

  //////////////////////////////////////////////////////////////
  //
  // radian units universal
  //
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radians" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radian" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"rads" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"rad" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, true,  L"r" },

  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"_radians" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"_radian" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"_rads" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"_rad" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, true,  L"_r" },

  //////////////////////////////////////////////////////////////
  //
  // radian units cs-* Czech
  //
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radi\x00E1ny" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radi\x00E1n" },

  //////////////////////////////////////////////////////////////
  //
  // radian units en-* (radians English)
  //
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radians" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radian" },

  //////////////////////////////////////////////////////////////
  //
  // radian units de-* German
  //
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, true, L"Bogenma" MYON_WCHAR_LATIN_SMALL_LETTER_SHARP_S },

  //////////////////////////////////////////////////////////////
  //
  // radian units es-* Spanish
  //
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radianes" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radi\x00E1n" },

  //////////////////////////////////////////////////////////////
  //
  // radian units fr-* French
  //
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radians" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radian" },

  //////////////////////////////////////////////////////////////
  //
  // radian units it-* Italian
  //
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radianti" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radiante" },

  //////////////////////////////////////////////////////////////
  //
  // radian units pl-* Polish
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radiany" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radian" },

  //////////////////////////////////////////////////////////////
  //
  // radian units pt-* Portuguese
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), false, true, L"radians" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Radians), true, false, L"radian" },

};

static MYON_UnitName angle_degree_units[] =
{
  //////////////////////////////////////////////////////////////
  //
  // arc degree unit abbreviations (Rhino - all locales)
  //
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, MYON_WCHAR_DEGREE_SYMBOL },
  // The unicode masuline ordinal indicator (Spanish) 
  // is often mistakenlyused as a degrees symbol.
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, MYON_WCHAR_MASCULINE_ORDINAL_INDICATOR },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"degrees" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"degree" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"degs" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"deg" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"d" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"_degrees" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"_degree" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"_degs" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"_deg" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"_d" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units cs-* (Czech)
  //
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"stupn\x011B" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"stupe\x0148" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"stup" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units en-* (English)
  //
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"degrees" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"degree" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units de-* (German)
  //
  {MYON_DE_DE_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"Grad" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units es-* (Spanish)
  //
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"grados" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"grado" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"grads" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"grad" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units fr-* (French)
  //
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"degr\x00E9s" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"degr\x00E9" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units it-* (Italian)
  //
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"gradisessagesimali" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"gradosessagesimale" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"gradi" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"grado" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true,  L"gs" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units pl-* Polish
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"stopnie" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"stopie\x0144" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, true, L"stop" },

  //////////////////////////////////////////////////////////////
  //
  // arc degree units pt-* (Portuguese)
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), false, true, L"graus" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees), true, false, L"grau" },
};

static MYON_UnitName angle_minute_units[] =
{
  //////////////////////////////////////////////////////////////
  //
  // arc minute unit abbreviations (Rhino - all locales)
  //
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), true, true,  MYON_WCHAR_APOSTROPHE },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), false, true, L"mins" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), true, false, L"min" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), true, true,  L"m" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), false, true, L"_minutes" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), true, false, L"_minute" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), false, true, L"_mins" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), true, false, L"_min" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), true, true,  L"_m" },

  //////////////////////////////////////////////////////////////
  //
  // arc minute units cs-* (Czech)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc minute units en-* (English)
  //
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), false, true, L"minutes" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes), true, false, L"minute" },

  //////////////////////////////////////////////////////////////
  //
  // arc minute units de-* (German)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc minute units es-* (Spanish)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc minute units fr-* (French)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc minute units it-* (Italian)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc minute units pt-* (Portuguese)
};

static MYON_UnitName angle_second_units[] =
{
  //////////////////////////////////////////////////////////////
  //
  // arc second unit abbreviations (Rhino - all locales)
  //
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), true, true,  MYON_WCHAR_QUOTATION_MARK },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), false, true, L"seconds" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), true, false, L"second" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), false, true, L"secs" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), true, false, L"sec" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), false, true, L"_seconds" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), true, false, L"_second" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), false, true, L"_secs" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), true, false, L"_sec" },

  //////////////////////////////////////////////////////////////
  //
  // arc second units cs-* (Czech)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc second units en-* (English)
  //
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), false, true, L"seconds" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), true, false, L"second" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds), true, true,  L"s" },

  //////////////////////////////////////////////////////////////
  //
  // arc second units de-* (German)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc second units es-* (Spanish)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc second units fr-* (French)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc second units it-* (Italian)
  //

  //////////////////////////////////////////////////////////////
  //
  // arc second units pt-* (Portuguese)
};

static MYON_UnitName angle_turn_units[] =
{
  //////////////////////////////////////////////////////////////
  //
  // turn unit universal
  //
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), true, true,  MYON_WCHAR_GREEK_SMALL_LETTER_TAU }, // unicode small tau symbol
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), false, true, L"turns" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), true, false, L"turn" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), true, true,  L"t" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), true, true,  L"_t" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), false, true, L"_turns" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), true, false, L"_turn" },

  //////////////////////////////////////////////////////////////
  //
  // turn units cs-* Czech
  //

  //////////////////////////////////////////////////////////////
  //
  // turn units en-* English
  //
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), false, true, L"turns" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Turns), true, false, L"turn" },

  //////////////////////////////////////////////////////////////
  //
  // turn units de-* German
  //

  //////////////////////////////////////////////////////////////
  //
  // turn units es-* Spanish
  //

  //////////////////////////////////////////////////////////////
  //
  // turn units fr-* French
  //

  //////////////////////////////////////////////////////////////
  //
  // turn units it-* Italian
  //

  //////////////////////////////////////////////////////////////
  //
  // turn units pt-* Portuguese

};

static MYON_UnitName angle_gradian_units[] =
{
  //////////////////////////////////////////////////////////////
  //
  // gradian units universal
  //
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gons" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gon" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"_gons" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"_gon" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"_gradians" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"_gradian" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"_grads" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"_grad" },
  {MYON_RHINO_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, true,  L"_g" },

  //////////////////////////////////////////////////////////////
  //
  // gradian units cs-* Czech
  //
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gradi\x00E1ny" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"grad" },
  {MYON_CS_CZ_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, true,  L"g" },

  //////////////////////////////////////////////////////////////
  //
  // gradian units en-* English
  //
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gradians" },
  {MYON_EN_CA_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gradian" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gradians" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gradian" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"grads" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"grad" },
  {MYON_EN_US_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, true,  L"g" },

  //////////////////////////////////////////////////////////////
  //
  // gradian units de-* German
  //
  // German uses the universal "gon" which is in MYON_RHINO_LOCALE_ID

  //////////////////////////////////////////////////////////////
  //
  // gradian units es-* Spanish
  //
  // https://es.wikipedia.org/wiki/Grado_centesimal
  // http://blog.utp.edu.co/adriamec/files/2012/10/NTC1000.pdf (document page 8, PDF page 11)
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gradianes" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gradi\x00E1n" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gons" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gon" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gradoscent" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gradcent" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gonios" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gonio" },
  {MYON_ES_ES_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, true,  L"gd" },

  //////////////////////////////////////////////////////////////
  //
  // gradian units fr-* French
  //
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"grades" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"grade" },
  {MYON_FR_FR_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, true,  L"g" },

  //////////////////////////////////////////////////////////////
  //
  // gradian units it-* Italian
  //
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"gradicent" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"gradocent" },
  {MYON_IT_IT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, true,  L"gc" },

  //////////////////////////////////////////////////////////////
  //
  // gradian units pl-* Polish
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"grads" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"grad" },
  {MYON_PL_PL_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, true,  L"g" },

  //////////////////////////////////////////////////////////////
  //
  // gradian units pt-* Portuguese
  //
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), false, true, L"grads" },
  {MYON_PT_PT_LOCALE_ID, static_cast<unsigned int>(MYON::AngleUnitSystem::Gradians), true, false, L"grad" },
};

static unsigned int MapPreferedLocaleId(
  unsigned int prefered_locale_id
  )
{
  switch (prefered_locale_id)
  {
  case MYON_Locale::WindowsLCID::es_ES_tradnl_LCID:
    prefered_locale_id = MYON_Locale::WindowsLCID::es_ES_LCID;
    break;
  }

  return prefered_locale_id;
}

static int CompareNullPointers(const void* a, const void* b)
{
  if ( 0 == a || 0 == b )
  {
    if ( 0 != b )
      return -1;
    if ( 0 != a )
      return 1;
  }
  return 0;
}

int MYON_UnitNameEx::Compare(const MYON_UnitNameEx* a, const MYON_UnitNameEx* b)
{
  int  rc = Compare_m_utf32_name(a,b);
  if ( 0 == rc && 0 != a && 0 != b )
  {
    if ( a->m_locale_id < b->m_locale_id )
      rc = -1;
    else if ( a->m_locale_id > b->m_locale_id )
      rc = 1;
    else
      rc = Compare_m_name(a,b);
  }
  return rc;
}

int MYON_UnitNameEx::Compare_m_utf32_name(const MYON_UnitNameEx* a, const MYON_UnitNameEx* b)
{
  int rc = CompareNullPointers(a,b);
  if ( 0 != rc || 0 == a )
    return rc;

  if ( a->m_utf32_name_count < b->m_utf32_name_count )
    return -1;

  if ( a->m_utf32_name_count > b->m_utf32_name_count )
    return 1;

  MYON__UINT32 x, y;
  const unsigned int capacity = MYON_UnitNameEx::m_utf32_name_capacity;
  unsigned int i;

  for ( i = 0; i < capacity; i++ )
  {
    x = a->m_utf32_name[i];
    y = b->m_utf32_name[i];
    if ( x < y )
      return -1;
    if ( y < x )
      return 1;
    if ( 0 == x )
      break;
  }

  return 0;
}

int MYON_UnitNameEx::Compare_m_name(const MYON_UnitNameEx* a, const MYON_UnitNameEx* b)
{
  int rc = CompareNullPointers(a,b);
  if ( 0 != rc || 0 == a )
    return rc;

  rc = CompareNullPointers(a->m_name,b->m_name);
  if ( 0 != rc || 0 == a->m_name )
    return rc;

  wchar_t x, y;
  unsigned int i = 0;
  for(;;)
  {
    x = a->m_name[i];
    y = b->m_name[i];
    if ( x < y )
      return -1;
    if ( y < x )
      return 1;
    if ( 0 == x )
      break;
    i++;
  }
  return 0;
}

static MYON__UINT32 MYON_ToLower( MYON__UINT32 c )
{
  // This is a specialized and fast version that converts selected 
  // unicode capital latin letter code points to the corresponding
  // unicode latin small letter code points.  It is designed to be
  // used in code that has to quickly parse common unit names
  // in English, Spanish, French, German, Protuguese and Czech. 
  // If other languages need to be supported, this funcition
  // may need to be enhanced.  
  //
  // In many situations, "to lower" requires more context than a single
  // unicode code point.  If those cases need to be handled correctly,
  // then this function will need to be discarded.
  //
  // The reason this function exists is that there is not a robust
  // C library function that does "to lower" on wchar_t strings
  // that works on Microsoft's, Apple's, Google's or Gnu's
  // compilers.  There are solutions, but many depend on other
  // app state variables that define an app/machine locale.
  // This meeans the result is not "predictably consistent".
  if ( c < 'A' )
    return c;

  if ( c <= 'Z' )
    return c + 0x0020; // A to a, .... Z to z
 
  if ( c < 0x00C0 )
    return c;
  
 if ( c <= 0x00DE )
  {
    if ( 0x00D7 == c )
      return c; // multiplication sign
    return c + 0x0020;
  }
  
  if ( c < 0x0100 )
    return c;

  if ( c <= 0x0137 )
  {
    // add 1 if c is even
    c |= 0x0001;
    return c;
  }
  
  if ( c < 0x0139 )
    return c;  
  
  if ( c < 0x0148 )
  {
    if ( 0 != (c % 2) )
      c++;
    return c;
  }

  if ( c < 0x014A )
    return c;  

  if ( c < 0x0177 )
  {
    // add 1 if c is odd
    c |= 0x0001;
    return c;
  }

  if ( c < 0x0179 )
    return c;  

  if ( c < 0x017E )
  {
    if ( 0 != (c % 2) )
      c++;
    return c;
  }

  if ( c < MYON_UNICODE_LATIN_CAPITAL_LETTER_SHARP_S )
  {
    // NOTE:
    //  This skips some "obvious" to lower conversions, but none 
    //  of these conversions are needed for common unit 
    //  names in English, Spanish, French, German, Protuguese 
    //  or Czech.
    return c;
  }

  switch(c)
  {
  case MYON_UNICODE_GREEK_CAPITAL_LETTER_TAU: // symbol for angle "turns"
    c = MYON_UNICODE_GREEK_SMALL_LETTER_TAU;
    break;

  case MYON_UNICODE_LATIN_CAPITAL_LETTER_SHARP_S:
    c = MYON_UNICODE_LATIN_SMALL_LETTER_SHARP_S;
    break;
  }

  return c;
}

static MYON__UINT32 MYON_ToLatinAtoZ( MYON__UINT32 c )
{
  // This is a specialized and fast version that converts selected 
  // unicode code points whose glyph is a latin letter A to Z with a 
  // diacritical mark to the corresponding latin A to Z letter.  
  // It also converts sharp s (eszett) to latin letter S and 
  // greek tau to to latin letter T.
  //
  // This code is designed to be used efficently parse common unit 
  // names in English, Spanish, French, German, Protuguese and Czech.
  // If other languages need to be supported, this function will need
  // to be enhanced, redone, or removed.  The unit names being parsed
  // by this code are often input on devices, like English language 
  // keyboards, where it is not convenient for the user to type a 
  // letter or symbol in the language being used.
  //
  // Discarding diacritical marks is a risky approach.  Specifically,
  // this function cannot be used in any context where different 
  // relevant words will be mapped to the same "A to Z" version.

  if ( c < 0x00C0 )
    return c;

  if ( c < 0x0100 )
  {
    if ( c >= 0x00C0 && c <= 0x00C5 )
      c = 'A'; // capital letter A with grave, acute, circumflex, tilde, diaeresis, ring above,
    else if ( c >= 0x00C8 && c <= 0x00CB )
      c = 'E'; // capital letter E with grave, acute, circumflex, diaeresis,
    else if ( c >= 0x00CC && c <= 0x00CF )
      c = 'I'; // capital letter I with grave, acute, circumflex, diaeresis,
    else if ( c >= 0x00D2 && c <= 0x00D6 )
      c = 'O'; // capital letter O with grave, acute, circumflex, tilde, diaeresis,
    else if ( c >= 0x00D9 && c <= 0x00DC )
      c = 'U'; // capital letter U with grave, acute, circumflex, diaeresis,
    else if ( c >= 0x00E0 && c <= 0x00E5 )
      c = 'a'; // small letter a with grave, acute, circumflex, tilde, diaeresis, ring above,
    else if ( c == 00E7 )
      c = 'c'; // small letter c with dedilla
    else if ( c >= 0x00E8 && c <= 0x00EB )
      c = 'e'; // small letter e with grave, acute, circumflex, tilde, diaeresis, ring above,
    else if ( c >= 0x00EC && c <= 0x00EF )
      c = 'i'; // small letter i with grave, acute, circumflex, tilde, diaeresis, ring above,
    else if ( c == 0x00F1 )
      c = 'n'; // small letter n with tilde
    else if ( c >= 0x00F2 && c <= 0x00F6 )
      c = 'o'; // small letter o with grave, acute, circumflex, tilde, diaeresis,
    else if ( c == 0x00F8 )
      c = 'o'; // small letter o with stroke
    else if ( c >= 0x00F9 && c <= 0x00FC )
      c = 'u'; // small letter u with grave, acute, circumflex, tilde, diaeresis,
    else if ( c == 0x00FD )
      c = 'y'; // small letter y with acute
    else if ( c == 0x00FF )
      c = 'y'; // small letter y with diaeresis
    else
    {
      switch (c)
      {
      case 0x00D0: // capital letter Eth
        c = 'D';
        break;
      case 0x00D1: // capital letter N with tilde
        c = 'N';
        break;
      case 0x00D8: // capital letter O with stroke
        c = 'O';
        break;
      case MYON_UNICODE_LATIN_SMALL_LETTER_SHARP_S: // small sharp s (German eszet)
        c = 's';
        break;
      case 0x00C7: // capital letter C with cedilla
        c = 'C';
        break;
      case 0x00DD: // capital letter Y with acute
        c = 'Y';
        break;
      case 0x00E7: // small letter c with cedilla
        c = 'c';
        break;
      case 0x00F0: // small letter Eth
        c = 'd';
        break;
      }
    }
  }
  else if ( c < 0x0200 )
  {
    if ( c >= 0x0100 && c <= 0x0105 )
      c = ((0 != (1&c)) ? 'a' : 'A'); // odds -> small A
    else if ( c >= 0x0106 && c <= 0x010D )
      c = ((0 != (1&c)) ? 'c' : 'C'); // odds -> small C
    else if ( c >= 0x010E && c <= 0x0111 )
      c = ((0 != (1&c)) ? 'd' : 'D'); // odds -> small D
    else if ( c >= 0x0112 && c <= 0x011B )
      c = ((0 != (1&c)) ? 'e' : 'E'); // odds -> small E
    else if ( c >= 0x011C && c <= 0x0123 )
      c = ((0 != (1&c)) ? 'g' : 'G'); // odds -> small G
    else if ( c >= 0x0124 && c <= 0x0127 )
      c = ((0 != (1&c)) ? 'h' : 'H'); // odds -> small H
    else if ( c >= 0x0128 && c <= 0x0131 )
      c = ((0 != (1&c)) ? 'i' : 'I'); // odds -> small I
    else if ( c >= 0x0134 && c <= 0x0135 )
      c = ((0 != (1&c)) ? 'j' : 'J'); // odds -> small J
    else if ( c >= 0x0136 && c <= 0x0137 )
      c = ((0 != (1&c)) ? 'k' : 'K'); // odds -> small K
    else if ( c == 0x0138 )
      c = 'k'; // small kra
    else if ( c >= 0x0139 && c <= 0x0142 )
      c = ((0 != (1&c)) ? 'L' : 'l'); // odds -> capital L
    else if ( c >= 0x0143 && c <= 0x0148 )
      c = ((0 != (1&c)) ? 'N' : 'n'); // odds -> capital N
    else if ( c == 0x0149 )
      c = 'n'; //small n preceded by apostrophe
    else if ( c >= 0x014C && c <= 0x0151 )
      c = ((0 != (1&c)) ? 'o' : 'O'); // odds -> small O
    else if ( c >= 0x0154 && c <= 0x0159 )
      c = ((0 != (1&c)) ? 'r' : 'R'); // odds -> small R
    else if ( c >= 0x015A && c <= 0x0161 )
      c = ((0 != (1&c)) ? 's' : 'S'); // odds -> small S
    else if ( c >= 0x0162 && c <= 0x0167 )
      c = ((0 != (1&c)) ? 't' : 'T'); // odds -> small t
    else if ( c >= 0x0168 && c <= 0x0173 )
      c = ((0 != (1&c)) ? 'u' : 'U'); // odds -> small u
    else if ( c >= 0x0174 && c <= 0x0175 )
      c = ((0 != (1&c)) ? 'w' : 'W'); // odds -> small w
    else if ( c >= 0x0176 && c <= 0x0178 )
      c = ((0 != (1&c)) ? 'y' : 'Y'); // odds -> small y
    else if ( c >= 0x0179 && c <= 0x017E )
      c = ((0 != (1&c)) ? 'z' : 'Z'); // odds -> capital Z
  }
  else
  {
    switch (c)
    {
    case MYON_UNICODE_GREEK_CAPITAL_LETTER_TAU: // (angle turn units)
      c = 'T';
      break;
    case MYON_UNICODE_GREEK_SMALL_LETTER_TAU: // (angle turn units)
      c = 't';
      break;
    case MYON_UNICODE_LATIN_CAPITAL_LETTER_SHARP_S: // (German eszett)
      c = 'S'; 
    }
  }

  return c;
}

/*
Description:
  This function is designed to be used to map common unit names
  and abbreviations in Czech, English, French, German, Spanish and
  Portuguese to a lower case latin name that has unicode code points
  'a' to 'z' that can be used in code that parses input streams 
  that specify angles, lengths or point coordinates.

Parameters:
  name - [in]
    String to test to see if it begins with a common 
    unit name or abbreviation in Czech, English, French,
    German, Italian, Portuguese or Spanish.
  utf32_small_simple_name - [out]
    A buffer where a null termintated UTF-32 encoded 
    simplified small letter version of the input 
    unit name is returned.
  utf32_small_simple_name_capacity - [in]
    Number of elements that can be used in utf32_small_simple_name[]

Returns:
   0: name was not a unit recognized by this function.
  >0: number of elements of name[] that were parsed as a 
      unit name or abbreviation.

Remarks:
  This code is used to quickly parse optional embedded unit
  names, abreviations in input streams that specify angles,
  lengths, points in cartesian coordinates, and points in
  polar coordinates.  
  
  The unit systems in the strings may be different from the
  language the applications user interface is using or the unit
  names may be entered on a device, like and english language
  keyboard, that does not provide easy access to latin letters
  with diacritical marks.
  
  If it becomes necessary to correctly handle languages like 
  Russian, Greek, Arabic, Hebrew, Asian languages or anything
  else that does not use latin-esque letters with diacritical
  marks, then this code will be useless and must be discarded.
*/
static
unsigned int MYON_GetSmallSimpleUnitsName(
  const wchar_t* name,
  MYON__UINT32* utf32_small_simple_name,
  size_t utf32_small_simple_name_capacity
  )
{
  MYON_UnicodeErrorParameters e = {0};
  int decode_count;
  MYON__UINT32 c, lower_c, simple_c;

  if ( 0 == utf32_small_simple_name || utf32_small_simple_name_capacity <= 0 )
  {
    // failure
    return 0;
  }

  if ( utf32_small_simple_name_capacity > 1 )
  {
    // check for common single glyph symbols that are used
    // when specify length and angle units
    switch(name[0])
    {
    case MYON_UNICODE_QUOTATION_MARK:         // quote (inches, arc seconds)
    case MYON_UNICODE_APOSTROPHE:             // apostophe (feet, arc minutes)
    case MYON_UNICODE_DEGREE_SYMBOL:          // degree symbol (arc degrees)
    case MYON_UNICODE_GREEK_SMALL_LETTER_TAU: // small tau (turns)
      utf32_small_simple_name[0] = (MYON__UINT32)(name[0]);
      utf32_small_simple_name[1] = 0;
      return 1;  
    case MYON_UNICODE_GREEK_CAPITAL_LETTER_TAU: // capital tau (turns)
      // This is done so strings that have passed through 
      // a "toupper" converter will parse correctly.
      utf32_small_simple_name[0] = (MYON__UINT32)MYON_UNICODE_GREEK_SMALL_LETTER_TAU;
      utf32_small_simple_name[1] = 0;
      return 1;  
    case MYON_UNICODE_MASCULINE_ORDINAL_INDICATOR: 
      // The unicode masuline ordinal indicator (Spanish) 
      // is often mistakenlyused as a degrees symbol.
      utf32_small_simple_name[0] = MYON_UNICODE_DEGREE_SYMBOL;
      utf32_small_simple_name[1] = 0;
      return 1;  
    }
  }

  bool bLeadingUnderbar = false;

  utf32_small_simple_name[0] = 0;

  // return quickly when beginning code point cannot possibly
  // be a unit.

  if ( name[0] >= 0 && name[0] < 'A' )
  {
    return 0;
  }

  if ( name[0] > 'Z' && name[0] < 'a' )
  {
    // leading underbar is allowed
    if ( MYON_UNICODE_LOW_LINE != name[0] )
      return 0;
    if ( name[1] >= 0 && name[1] < 'A' )
      return 0;
    if ( name[1] > 'Z' && name[1] < 'a' )
      return 0;
    if ( name[1] > 'z' && name[1] < 0x80 )
      return 0;
    bLeadingUnderbar = true;
  }

  if ( name[0] > 'z' && name[0] < 0x80 )
  {
    return 0;
  }

  size_t count = 0;  
  int name_index = 0;
  while ( count < utf32_small_simple_name_capacity && 0 != name[name_index] )
  {
    e.m_error_status = 0;
    c = 0xFFFFFFFF;

    if ( name[name_index] <= 127 && name[name_index] >= 0 )
    {
      // The code point value for all UTF-* encodings map 
      // values from 0 to 127 (ASCII) to the same code point
      // value. These values are common and this special case
      // handling speeds up parsing.
      c = (MYON__UINT32)(name[name_index]);
      decode_count = 1;
    }
    else
    {
      switch(sizeof(wchar_t))
      {
      case 1: // assume name is UTF-8 encoded
        {
          int name_count = name_index+1;
          while( 0 != name[name_count] && 0 != (name[name_count] & 0x80) && name_count < name_index+6 )
            name_count++;
          decode_count = MYON_DecodeUTF8((const char*)(name+name_index),name_count-name_index,&e,&c);
        }
        break;

      case 2: // assume name is UTF-16 encoded
        decode_count = MYON_DecodeUTF16((const MYON__UINT16*)(name+name_index),2,&e,&c);
        break;

      case 4: // assume name is UTF-32 encoded
        c = (MYON__UINT32)(name[name_index]);
        decode_count = 1;
        break;

      default:
        // unsupported wchar_t size
        decode_count = 0;
        break;
      }
    }

    if ( decode_count < 1 || !MYON_IsValidUnicodeCodePoint(c) )
    {
      count = 0;
      break;
    }

    lower_c = MYON_ToLower(c);
    simple_c = MYON_ToLatinAtoZ(lower_c);
    if (    (simple_c <= 0x40)
         || (simple_c >= 0x5B && simple_c <= 0x60)
         || (simple_c >= 0x7B && simple_c <= 0xB4)
         || (simple_c >= 0xB6 && simple_c <= 0xBF)
         || (0xD7 == simple_c)
         || (0xF7 == simple_c)
       )
    {
      // This character terminates parsing unless it is a leading underbar.
      if ( count != 0 || !bLeadingUnderbar || MYON_UNICODE_LOW_LINE != simple_c )
        break;
    }

    name_index += decode_count;
    utf32_small_simple_name[count++] = simple_c;
  }

  if ( count >= utf32_small_simple_name_capacity )
    count = 0;

  utf32_small_simple_name[count] = 0;

  return ((count > 0 && name_index > 0) ? name_index : 0);
}

int MYON_UnitNameEx::SetSimplifiedName()
{
  const unsigned int capacity = MYON_UnitNameEx::m_utf32_name_capacity;
   m_utf32_name[capacity-1] = 0;
  const int name_index = MYON_GetSmallSimpleUnitsName(m_name,m_utf32_name,capacity);
  unsigned int count = 0;
  if (    name_index > 0
        && 0 != m_utf32_name[0]
        && 0 == m_utf32_name[capacity-1]
        )
  {
    while ( 0 != m_utf32_name[count] )
      count++;
  }
  if ( count > 0 && count < capacity
       && 0 == m_utf32_name[count]
       && 0 != m_utf32_name[count-1]
     )
  {
    m_utf32_name_count = count;
  }
  else
  {
    m_utf32_name_count = 0;
  }
  for ( count = m_utf32_name_count; count < capacity; count++ )
    m_utf32_name[count] = 0;
  return (count > 0 && name_index > 0) ? name_index : 0;
}

class MYON_UnitSystemNameCache
{
public:
  // m_unit_list[] has m_unit_list_count elements sorted
  // by MYON_UnitNameEx.m_utf32_name_count (shortest first)
  // and then by MYON_UnitNameEx.m_utf32_name
  const MYON_UnitNameEx* m_unit_list;
  unsigned int m_unit_list_count;
  MYON_2dex m_unit_index[MYON_UnitNameEx::m_utf32_name_capacity];
};

static bool GetUnitSystemNameCache(
  unsigned int (*GetUnitDictionary)(size_t,MYON_UnitName*,MYON_UnitNameEx*,bool),
  bool bIncludeInvalidLocaleIdNames,
  MYON_UnitSystemNameCache& cache
  )
{
  unsigned int i;

  const unsigned int utf32_name_capacity = MYON_UnitNameEx::m_utf32_name_capacity;
  cache.m_unit_list = 0;
  cache.m_unit_list_count = 0;

  for ( i = 0; i < utf32_name_capacity; i++ )
  {
    cache.m_unit_index[i].i = 0;
    cache.m_unit_index[i].j = 0;
  }

  // make a list
  unsigned int capacity = GetUnitDictionary(0,0,0,bIncludeInvalidLocaleIdNames);
  if ( capacity <= 0 )
    return false;

  // Dale Lear September 16, 2016
  // https://mcneel.myjetbrains.com/youtrack/issue/RH-28754
  // This allocation appears as a leak but it is not.
  // This pointer is allocated once when needed, saved as cache.m_unit_list,
  // and is used for the lifetime of the application.
  MYON_UnitNameEx* unit_list = new (std::nothrow) MYON_UnitNameEx[capacity];

  if ( nullptr == unit_list )
    return false;
  capacity = GetUnitDictionary(capacity,0,unit_list,bIncludeInvalidLocaleIdNames);
  if ( capacity <= 0 )
  {
    delete[] unit_list;
    return false;
  }

  // Sort the returned list
  MYON_qsort(
    unit_list,
    capacity,
    sizeof(unit_list[0]),
    (int (*)(const void*,const void*))MYON_UnitNameEx::Compare
    );

  // Cull duplicates and set m_name = 0;
  unit_list[0].m_name = 0;
  unsigned int count = 0;

  for ( i = count+1; i < capacity; i++ )
  {
    unit_list[i].m_name = 0;
    if (    0 == MYON_UnitNameEx::Compare_m_utf32_name(unit_list+count,unit_list+i) 
         && unit_list[count].m_locale_id == unit_list[i].m_locale_id
       )
    {
      continue;
    }
    count++;
    if (count < i )
    {
      unit_list[count] = unit_list[i];
    }
  }
  count++;

  // The array length_unit_list[] is sorted so the value of 
  // length_unit_list[].m_utf32_name_count increases.  
  // Use that fact to build an index based on the value of 
  // length_unit_list[].m_utf32_name_count so searches can
  // easily be restriced to the region of length_unit_list[]
  // where a possible match exists.
  for ( i = 0; i < count; i++ )
  {
    cache.m_unit_index[unit_list[i].m_utf32_name_count].i = (int)i;
    while(i+1 < count && unit_list[i].m_utf32_name_count == unit_list[i+1].m_utf32_name_count )
      i++;
    cache.m_unit_index[unit_list[i].m_utf32_name_count].j = (int)(i+1);
  }

  cache.m_unit_list = unit_list;

  if ( 0 != cache.m_unit_list )
    cache.m_unit_list_count = count;

  return (cache.m_unit_list_count > 0);
}

static unsigned char UnitSystemEnumValue(
  const MYON_UnitSystemNameCache& cache,
  unsigned int prefered_locale_id,
  const MYON_UnitNameEx* usname
  )
{
  prefered_locale_id = MapPreferedLocaleId(prefered_locale_id);

  if ( 0 == usname
       || usname->m_utf32_name_count <= 0
       || usname->m_utf32_name_count >= MYON_UnitNameEx::m_utf32_name_capacity
       || 0 == usname->m_utf32_name[0] 
       || 0 != usname->m_utf32_name[MYON_UnitNameEx::m_utf32_name_capacity-1] 
     )
  {
    return 0;
  }

  if ( 0 == cache.m_unit_list || cache.m_unit_list_count <= 0 )
  {
    return 0;
  }

  MYON_2dex range = cache.m_unit_index[usname->m_utf32_name_count];
  // All m_utf32_name entries in s_length_unit_list[range.i,...,range.j-1]
  // have the same length as usname->m_utf32_name[] and they are sorted by
  // m_utf32_name.  Do a binary search on m_utf32_name after checking
  // that the first character indicates a match may exist.
  if (    range.i < range.j 
       && usname->m_utf32_name[0] >= cache.m_unit_list[range.i].m_utf32_name[0]
       && usname->m_utf32_name[0] <= cache.m_unit_list[range.j-1].m_utf32_name[0]
     )
  {
    // binary search on m_utf32_name[] value
    do
    {
      int k = (range.i+range.j)/2;
      int rc = MYON_UnitNameEx::Compare_m_utf32_name(usname,cache.m_unit_list+k);
      if (rc < 0)
        range.j = k;
      else if (rc > 0)
        range.i = k+1;
      else
      {
        // string matches - make sure the locale id is valid
        if ( prefered_locale_id == cache.m_unit_list[k].m_locale_id )
          return cache.m_unit_list[k].m_unit_system;

        unsigned int common_angle_unit_system = cache.m_unit_list[k].m_unit_system;

        int k0 = k;
        while ( k0 > range.i
                && 0 == MYON_UnitNameEx::Compare_m_utf32_name(usname,cache.m_unit_list+(k0-1))
              )
        {
          k0--;
          if ( prefered_locale_id == cache.m_unit_list[k0].m_locale_id )
            return cache.m_unit_list[k0].m_unit_system;
          if ( common_angle_unit_system != cache.m_unit_list[k0].m_unit_system )
            common_angle_unit_system = 0;
        }

        int k1 = k+1;
        while ( k1 < range.j
                && 0 == MYON_UnitNameEx::Compare_m_utf32_name(usname,cache.m_unit_list+k1)
              )
        {
          if ( prefered_locale_id == cache.m_unit_list[k1].m_locale_id )
            return cache.m_unit_list[k1].m_unit_system;
          if ( common_angle_unit_system != cache.m_unit_list[k1].m_unit_system )
            common_angle_unit_system = 0;
          k1++;
        }

        // All matching strings specify the same unit system so
        // we don't care about locale id.
        if ( common_angle_unit_system != 0 )
        {
          return cache.m_unit_list[k].m_unit_system;
        }
        
        if ( k0+1 == k1 )
        {
          // There is a single entry for this string so
          // we don't care about locale id.
          return cache.m_unit_list[k].m_unit_system;
        }

        // If we get this far, there are 2 or more strings that
        // match usname and none of these exactly match the locale 
        // id.
        for ( k = k0; k < k1; k++ )
        {
          if ( MYON_RHINO_LOCALE_ID ==  cache.m_unit_list[k].m_locale_id )
            return cache.m_unit_list[k].m_unit_system; // "universal" string

          if ( MYON_INVALID_LOCALE_ID ==  cache.m_unit_list[k].m_locale_id )
            return 0; // this string requires an exact locale id match

          // Check for a language match; i.e., if cache.m_unit_list[] is
          // is for locale en-US and the prefered_locale_id is en-UK,
          // then use the en-US entry.  The "language" part of a locale id
          // is encoded in the low byte of the locale id.
          if ( (0xFF & prefered_locale_id) == (0xFF & cache.m_unit_list[k].m_locale_id) )
          {
            // the language part of the local matched
            return cache.m_unit_list[k].m_unit_system;
          }
        }

        return 0;
      }
    }
    while ( range.i < range.j );
  }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Length unit names
//

/*
  length_unit_system - [out]]
    If name is parsed as a length unit system name, then
    *length_unit_system is set to the corresponding length unit system.
    If name is not parsed, then *length_unit_system is set to 
    MYON::LengthUnitSystem::None.
*/
unsigned int GetLengthUnitList( 
  size_t length_unit_list_capacity,
  MYON_UnitName* length_unit_list, 
  MYON_UnitNameEx* length_unit_ex_list,
  bool bIncludeInvalidLocaleIdNames
  )
{
  size_t si_length_units_count = sizeof(si_length_units)/sizeof(si_length_units[0]);
  size_t en_US_customary_length_units_count = sizeof(en_US_customary_length_units)/sizeof(en_US_customary_length_units[0]);

  size_t capacity = si_length_units_count
                  + en_US_customary_length_units_count;

  if (    0 == length_unit_list_capacity
       && 0 == length_unit_list
       && 0 == length_unit_ex_list
     )
  {
    return (unsigned int)capacity;
  }

  if (   (0 == length_unit_list  && 0 == length_unit_ex_list)
       || length_unit_list_capacity < capacity 
      )
  {
    return 0;
  }


  size_t count = 0;
  MYON_UnitName unit_name;
  MYON_UnitNameEx x;
  const MYON_UnitName* input_list;
  size_t input_list_count;

  for ( size_t list_index = 0; list_index < 2; list_index++ )
  {
    switch(list_index)
    {
    case 0:
      input_list = si_length_units;
      input_list_count = si_length_units_count;
      break;
    case 1:
      input_list = en_US_customary_length_units;
      input_list_count = en_US_customary_length_units_count;
      break;
    default:
      input_list = 0;
      input_list_count = 0;
      break;
    }

    for ( size_t i = 0; i < input_list_count; i++ )
    {
      if ( 0 != length_unit_list )
      {
        length_unit_list[count++] = input_list[i];
      }
      else if ( 0 != length_unit_ex_list )
      {
        unit_name = input_list[i];
        x.m_locale_id   = unit_name.m_locale_id;
        x.m_unit_system = unit_name.m_unit_system;
        x.m_name        = unit_name.m_name;
        if ( x.SetSimplifiedName() > 0 
             && 0 != x.m_utf32_name_count > 0
             && 0 != x.m_utf32_name_count < MYON_UnitNameEx::m_utf32_name_capacity
             && 0 != x.m_utf32_name[0] 
             && 0 != x.m_utf32_name[x.m_utf32_name_count-1] 
             && 0 == x.m_utf32_name[x.m_utf32_name_count] 
             && 0 == x.m_utf32_name[MYON_UnitNameEx::m_utf32_name_capacity-1]
           )
        {
          length_unit_ex_list[count++] = x;
        }
        else
        {
          MYON_ERROR("Length unit list conatins invalid element.");
        }
      }
    }
  }

  return (unsigned int)count;
}

unsigned int MYON_LengthUnitName::GetLengthUnitNameList( 
  size_t length_unit_list_capacity,
  MYON_LengthUnitName* length_unit_list
  )
{
  return GetLengthUnitList(length_unit_list_capacity,(MYON_UnitName*)length_unit_list,0,false);
}

int MYON_ParseLengthUnitName(
  const wchar_t* str,
  int str_count,
  int prefered_locale_id,
  MYON::LengthUnitSystem* length_unit_system
)
{
  MYON_ParseSettings parse_settings;
  parse_settings.SetPreferedLocaleId(prefered_locale_id);
  return MYON_ParseLengthUnitName(str, str_count, parse_settings, length_unit_system);
}

int MYON_ParseLengthUnitName( 
  const wchar_t* str,
  int str_count,
  MYON_ParseSettings parse_settings,
  MYON::LengthUnitSystem* length_unit_system
  )
{
  unsigned int prefered_locale_id = MapPreferedLocaleId(parse_settings.PreferedLocaleId());

  static MYON_UnitSystemNameCache s_length_unit_cache = {0};

  if ( 0 != length_unit_system )
  {
    *length_unit_system = MYON::LengthUnitSystem::None;
  }

  MYON_UnitNameEx x;
  const unsigned int utf32_name_capacity = MYON_UnitNameEx::m_utf32_name_capacity;
  unsigned int count = 0;
  int str_index = 0;
  x.m_locale_id = 0;
  x.m_unit_system = static_cast<unsigned int>(MYON::LengthUnitSystem::None);
  x.m_name = str;
  x.m_utf32_name_count = 0;
  x.m_utf32_name[0] = 0;

  int whitespace_count = 0;

  for (;;)
  {
    if ( 0 == str_count || str_count < -1 )
      break;

    if ( nullptr == str )
      break;

    if (parse_settings.ParseLeadingWhiteSpace())
    {
      // skip over leading white space
      while ((-1 == str_count || whitespace_count < str_count) && parse_settings.IsLeadingWhiteSpace(str[str_index+whitespace_count]))
      {
        whitespace_count++;
      }
    }
    else if (parse_settings.ParseWhiteSpaceBetweenValueAndUnitSystem())
    {
      // skip over interior white space (needed to make MYON_LengthValue() parsing work correctly
      while ((-1 == str_count || whitespace_count < str_count) && parse_settings.IsInteriorWhiteSpace(str[str_index+whitespace_count]))
      {
        whitespace_count++;
      }
    }
    str += whitespace_count;
    if (-1 != str_count)
      str_count -= whitespace_count;

    if (    str[0] >= 0 
         && str[0] < 'A' 
         && MYON_UNICODE_QUOTATION_MARK != str[0] 
         && MYON_UNICODE_APOSTROPHE != str[0]
       )
    {
      break;
    }

    if ( 0 == s_length_unit_cache.m_unit_list )
    {
      if ( !GetUnitSystemNameCache(GetLengthUnitList,true,s_length_unit_cache) )
        break;
    }

    if (MYON_UNICODE_QUOTATION_MARK == str[0] )
    {
      // double quote = US inches abbreviation
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::LengthUnitSystem::Inches);
      x.m_utf32_name[0] = MYON_UNICODE_QUOTATION_MARK;
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    if (MYON_UNICODE_APOSTROPHE == str[0] )
    {
      // apostrophe = US feet abbreviation
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::LengthUnitSystem::Feet);
      x.m_utf32_name[0] = MYON_UNICODE_APOSTROPHE;
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    str_index = MYON_GetSmallSimpleUnitsName(str,x.m_utf32_name,utf32_name_capacity);
    if ( str_index <= 0 )
    {
      str_index = 0;
      break;
    }

    while (count < utf32_name_capacity && 0 != x.m_utf32_name[count] )
      count++;
    if ( count >= utf32_name_capacity )
    {
      str_index = 0;
      count = 0;
      break;
    }
    x.m_utf32_name_count = count;
    while (count < utf32_name_capacity)
      x.m_utf32_name[count++] = 0;

    x.m_unit_system = UnitSystemEnumValue(
      s_length_unit_cache,
      prefered_locale_id,
      &x);
    if ( static_cast<unsigned int>(MYON::LengthUnitSystem::None) == x.m_unit_system )
    {
      str_index = 0;
      count = 0;
    }
    break;
  }

  if ( str_index > 0 && 0 != length_unit_system )
    *length_unit_system = MYON::LengthUnitSystemFromUnsigned(x.m_unit_system);

  return (str_index > 0) ? (str_index+whitespace_count) : 0;
}



///////////////////////////////////////////////////////////////////////////////
//
// Angle unit names
//
static unsigned int GetAngleUnitList( 
  size_t angle_unit_list_capacity,
  MYON_UnitName* angle_unit_list, 
  MYON_UnitNameEx* angle_unit_ex_list,
  bool bIncludeInvalidLocaleIdNames
  )
{
  const size_t angle_radian_units_count = sizeof(angle_radian_units)/sizeof(angle_radian_units[0]);
  const size_t angle_degree_units_count = sizeof(angle_degree_units)/sizeof(angle_degree_units[0]);
  const size_t angle_minute_units_count = sizeof(angle_minute_units)/sizeof(angle_minute_units[0]);
  const size_t angle_second_units_count = sizeof(angle_second_units)/sizeof(angle_second_units[0]);
  const size_t angle_turn_units_count = sizeof(angle_turn_units)/sizeof(angle_turn_units[0]);
  const size_t angle_gradian_units_count = sizeof(angle_gradian_units)/sizeof(angle_gradian_units[0]);
  const size_t angle_no_units_count = sizeof(angle_no_units)/sizeof(angle_no_units[0]);

  const size_t capacity = angle_radian_units_count
                  + angle_degree_units_count
                  + angle_minute_units_count
                  + angle_second_units_count
                  + angle_turn_units_count
                  + angle_gradian_units_count
                  + (bIncludeInvalidLocaleIdNames ? angle_no_units_count : 0)
                  ;

  if ( 0 == angle_unit_list_capacity && 0 == angle_unit_list && 0 == angle_unit_ex_list )
    return (unsigned int)capacity;

  if (    angle_unit_list_capacity < capacity
       || (0 == angle_unit_list && 0 == angle_unit_ex_list)
     )
  {
    return 0;
  }

  if ( 0 != angle_unit_list && 0 != angle_unit_ex_list )
    return 0;

  size_t count = 0;
  MYON_UnitName unit_name;
  MYON_UnitNameEx x;
  const MYON_UnitName* input_list;
  size_t input_list_count;

  for ( size_t list_index = 0; list_index < 7; list_index++ )
  {
    switch(list_index)
    {
    case 0:
      input_list = angle_radian_units;
      input_list_count = angle_radian_units_count;
      break;
    case 1:
      input_list = angle_turn_units;
      input_list_count = angle_turn_units_count;
      break;
    case 2:
      input_list = angle_degree_units;
      input_list_count = angle_degree_units_count;
      break;
    case 3:
      input_list = angle_minute_units;
      input_list_count = angle_minute_units_count;
      break;
    case 4:
      input_list = angle_second_units;
      input_list_count = angle_second_units_count;
      break;
    case 5:
      input_list = angle_gradian_units;
      input_list_count = angle_gradian_units_count;
      break;
    case 6:
      if ( bIncludeInvalidLocaleIdNames )
      {
        input_list = angle_no_units;
        input_list_count = angle_no_units_count;
      }
      else
      {
        input_list = 0;
        input_list_count = 0;
      }
      break;
    default:
      input_list = 0;
      input_list_count = 0;
      break;
    }

    for ( size_t i = 0; i < input_list_count; i++ )
    {
      if ( 0 != angle_unit_list )
      {
        angle_unit_list[count++] = input_list[i];
      }
      else
      {
        unit_name = input_list[i];
        x.m_locale_id = unit_name.m_locale_id;
        x.m_unit_system = unit_name.m_unit_system;
        x.m_name = unit_name.m_name;
        if (    x.SetSimplifiedName() > 0 
             && 0 != x.m_utf32_name_count > 0
             && 0 != x.m_utf32_name_count < MYON_UnitNameEx::m_utf32_name_capacity
             && 0 != x.m_utf32_name[0] 
             && 0 != x.m_utf32_name[x.m_utf32_name_count-1] 
             && 0 == x.m_utf32_name[x.m_utf32_name_count] 
             && 0 == x.m_utf32_name[MYON_UnitNameEx::m_utf32_name_capacity-1]
           )
        {
          angle_unit_ex_list[count++] = x;
        }
        else
        {
          MYON_ERROR("Angle unit list conatins invalid element.");
        }
      }
    }
  }

  return (unsigned int)count;
}

int MYON_ParseAngleUnitName(
  const wchar_t* str,
  int str_count,
  int prefered_locale_id,
  MYON::AngleUnitSystem* angle_unit_system
)
{
  MYON_ParseSettings parse_settings;
  parse_settings.SetPreferedLocaleId(prefered_locale_id);
  parse_settings.SetParseLeadingWhiteSpace(false);
  return MYON_ParseAngleUnitName(str, str_count, parse_settings, angle_unit_system);
}

int MYON_ParseAngleUnitName( 
  const wchar_t* str,
  int str_count,
  MYON_ParseSettings parse_settings,
  MYON::AngleUnitSystem* angle_unit_system
  )
{
  static MYON_UnitSystemNameCache s_angle_unit_cache = {0};

  if ( 0 != angle_unit_system )
    *angle_unit_system = MYON::AngleUnitSystem::None;

  unsigned int prefered_locale_id = MapPreferedLocaleId(parse_settings.PreferedLocaleId());

  MYON_UnitNameEx x;
  const unsigned int utf32_name_capacity = MYON_UnitNameEx::m_utf32_name_capacity;
  unsigned int count = 0;
  int str_index = 0;
  x.m_locale_id = 0;
  x.m_unit_system = static_cast<unsigned int>(MYON::AngleUnitSystem::None);
  x.m_name = str;
  x.m_utf32_name_count = 0;
  x.m_utf32_name[0] = 0;
  
  int whitespace_count = 0;

  for (;;)
  {
    if ( 0 == str_count && str_count < -1 )
      break;

    if ( 0 == str )
      break;

    if (parse_settings.ParseLeadingWhiteSpace())
    {
      // skip over leading white space
      while ((-1 == str_count || whitespace_count < str_count) && parse_settings.IsLeadingWhiteSpace(str[str_index+whitespace_count]))
      {
        whitespace_count++;
      }
    }
    else if (parse_settings.ParseWhiteSpaceBetweenValueAndUnitSystem())
    {
      // skip over interior white space (needed to make MYON_AngleValue() parsing work correctly
      while ((-1 == str_count || whitespace_count < str_count) && parse_settings.IsInteriorWhiteSpace(str[str_index+whitespace_count]))
      {
        whitespace_count++;
      }
    }
    str += whitespace_count;
    if (-1 != str_count)
      str_count -= whitespace_count;

    if (    str[0] >= 0 
         && str[0] < 'A' 
         && MYON_UNICODE_QUOTATION_MARK != str[0] 
         && MYON_UNICODE_APOSTROPHE != str[0]
       )
    {
      break;
    }


    if ( 0 == s_angle_unit_cache.m_unit_list )
    {
      if ( !GetUnitSystemNameCache(GetAngleUnitList,true,s_angle_unit_cache) )
        break;
    }

    if (MYON_UNICODE_QUOTATION_MARK == str[0] )
    {
      // double quote = arc seconds
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::AngleUnitSystem::Seconds);
      x.m_utf32_name[0] = MYON_UNICODE_QUOTATION_MARK;
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    if ( MYON_UNICODE_APOSTROPHE == str[0] )
    {
      // apostrophe = arc minutes
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::AngleUnitSystem::Minutes);
      x.m_utf32_name[0] = MYON_UNICODE_APOSTROPHE;
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    if ( MYON_UNICODE_DEGREE_SYMBOL == str[0] )
    {
      // unicode degree symbol = arc degrees
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees);
      x.m_utf32_name[0] = MYON_UNICODE_DEGREE_SYMBOL;
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    if ( MYON_UNICODE_MASCULINE_ORDINAL_INDICATOR == str[0] )
    {
      // The unicode masuline ordinal indicator (Spanish) 
      // is often mistakenlyused as a degrees symbol.
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::AngleUnitSystem::Degrees);
      x.m_utf32_name[0] = MYON_UNICODE_DEGREE_SYMBOL; // correct - convert to degree symbol
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    if ( MYON_UNICODE_GREEK_SMALL_LETTER_TAU == str[0] )
    {
      // unicode greek tau = turns
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::AngleUnitSystem::Turns);
      x.m_utf32_name[0] = MYON_UNICODE_GREEK_SMALL_LETTER_TAU;
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    if ( MYON_UNICODE_GREEK_CAPITAL_LETTER_TAU == str[0] )
    {
      // unicode capital greek tau = turns
      str_index = 1;
      x.m_name = str;
      x.m_unit_system = static_cast<unsigned int>(MYON::AngleUnitSystem::Turns);
      x.m_utf32_name[0] = MYON_UNICODE_GREEK_SMALL_LETTER_TAU; // correct - convert to lower case
      x.m_utf32_name[1] = 0;
      x.m_utf32_name_count = 1;
      break;
    }

    str_index = MYON_GetSmallSimpleUnitsName(str,x.m_utf32_name,utf32_name_capacity);
    if ( str_index <= 0 )
    {
      str_index = 0;
      break;
    }

    while (count < utf32_name_capacity && 0 != x.m_utf32_name[count] )
      count++;
    if ( count >= utf32_name_capacity )
    {
      str_index = 0;
      count = 0;
      break;
    }
    x.m_utf32_name_count = count;
    while (count < utf32_name_capacity)
      x.m_utf32_name[count++] = 0;

    x.m_unit_system = UnitSystemEnumValue(
      s_angle_unit_cache,
      prefered_locale_id,
      &x
      );
    if ( static_cast<unsigned int>(MYON::AngleUnitSystem::None) == x.m_unit_system )
    {
      str_index = 0;
      count = 0;
    }
    break;
  }

  if ( str_index > 0 && angle_unit_system )
    *angle_unit_system = MYON::AngleUnitSystemFromUnsigned(x.m_unit_system);

  return (str_index > 0) ? (str_index+whitespace_count) : 0;
}

unsigned int MYON_AngleUnitName::GetAngleUnitNameList( 
  size_t angle_unit_list_capacity,
  MYON_AngleUnitName* angle_unit_list
  )
{
  return GetAngleUnitList(angle_unit_list_capacity,(MYON_UnitName*)angle_unit_list,0,false);
}

static const wchar_t* MYON_Internal_GetUnitsName(
  unsigned int locale_id,
  const unsigned int unit_system_as_unsigned,
  bool bPlural,
  size_t count,
  const MYON_UnitName* names
  )
{
  if (count <= 0 || nullptr == names)
    return nullptr;

  const unsigned int special_id_limit = 2;

  if (locale_id <= special_id_limit && locale_id != MYON_RHINO_LOCALE_ID)
  {
    if (0 == locale_id)
      locale_id = MYON_Locale::CurrentCulture.WindowsLCID();
    if ( locale_id <= special_id_limit && locale_id != MYON_RHINO_LOCALE_ID )
      locale_id = MYON_RHINO_LOCALE_ID;
  }
  else if (locale_id > special_id_limit && MYON_EN_US_LOCALE_ID != locale_id)
  {
    unsigned int prefered_locale_id = MapPreferedLocaleId(locale_id);
    if (locale_id != prefered_locale_id && prefered_locale_id > special_id_limit)
      locale_id = prefered_locale_id;
  }

  const wchar_t* singular_name = nullptr;
  const wchar_t* plural_name = nullptr;
  for (size_t i = 0; i < count; i++)
  {
    if (locale_id == names[i].m_locale_id && unit_system_as_unsigned == names[i].m_unit_system )
    {
      const wchar_t* name = names[i].m_name;
      if (nullptr != name && 0 != name[0])
      {
        if (nullptr == singular_name && names[i].m_bIsSingular)
        {
          if (!bPlural)
            return name;
          singular_name = name;
        }
        if (nullptr == singular_name && names[i].m_bIsPlural)
        {
          if (bPlural)
            return name;
          plural_name = name;
        }
      }
    }
  }

  if (nullptr != plural_name)
    return plural_name;
  if (nullptr != singular_name)
    return singular_name;

  if (MYON_RHINO_LOCALE_ID == locale_id)
    return nullptr; // nothing left to try
  
  if (locale_id > special_id_limit && MYON_EN_US_LOCALE_ID != locale_id)
  {
    // try English - US
    return MYON_Internal_GetUnitsName(MYON_EN_US_LOCALE_ID, unit_system_as_unsigned, bPlural, count, names);
  }

  // Try "rhino" locale
  return MYON_Internal_GetUnitsName(MYON_RHINO_LOCALE_ID, unit_system_as_unsigned, bPlural, count, names);
}


MYON_LengthUnitName MYON_LengthUnitName::Create(
  unsigned int locale_id,
  MYON::LengthUnitSystem length_unit_system,
  bool bPlural
)
{
  if (0 == locale_id)
    locale_id = MYON_Locale::CurrentCulture.WindowsLCID();

  size_t count = 0;
  const MYON_UnitName* names = nullptr;

  length_unit_system = MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(length_unit_system));

  switch (length_unit_system)
  {
  case MYON::LengthUnitSystem::None:
    break;

  case MYON::LengthUnitSystem::Angstroms:
  case MYON::LengthUnitSystem::Nanometers:
  case MYON::LengthUnitSystem::Microns:
  case MYON::LengthUnitSystem::Millimeters:
  case MYON::LengthUnitSystem::Centimeters:
  case MYON::LengthUnitSystem::Decimeters:
  case MYON::LengthUnitSystem::Meters:
  case MYON::LengthUnitSystem::Dekameters:
  case MYON::LengthUnitSystem::Hectometers:
  case MYON::LengthUnitSystem::Kilometers:
  case MYON::LengthUnitSystem::Megameters:
  case MYON::LengthUnitSystem::Gigameters:
    names = si_length_units;
    count = sizeof(si_length_units)/sizeof(si_length_units[0]);    
    break;

  case MYON::LengthUnitSystem::Microinches:
  case MYON::LengthUnitSystem::Mils:
  case MYON::LengthUnitSystem::Inches:
  case MYON::LengthUnitSystem::Feet:
  case MYON::LengthUnitSystem::Yards:
  case MYON::LengthUnitSystem::Miles:
    names = en_US_customary_length_units;
    count = sizeof(en_US_customary_length_units)/sizeof(en_US_customary_length_units[0]);    
    break;
    
  case MYON::LengthUnitSystem::PrinterPoints:
    break;
  case MYON::LengthUnitSystem::PrinterPicas:
    break;
  case MYON::LengthUnitSystem::NauticalMiles:
    break;
  case MYON::LengthUnitSystem::AstronomicalUnits:
    break;
  case MYON::LengthUnitSystem::LightYears:
    break;
  case MYON::LengthUnitSystem::Parsecs:
    break;
  case MYON::LengthUnitSystem::CustomUnits:
    break;
  case MYON::LengthUnitSystem::Unset:
    break;
  default:
    MYON_ERROR("Invalid length_unit_system parameter.");
    length_unit_system = MYON::LengthUnitSystem::Unset;
    break;
  }


  MYON_LengthUnitName length_unit_name;
  length_unit_name.m_locale_id = locale_id;
  length_unit_name.m_length_unit_system = length_unit_system;
  length_unit_name.m_bNameIsSingular = bPlural ? false : true;
  length_unit_name.m_bNameIsPlural = bPlural ? true : false;

  // attempt to get localized name
  length_unit_name.m_name = MYON_Internal_GetUnitsName(
    locale_id,
    static_cast<unsigned int>(length_unit_system),
    bPlural,
    count,
    names
  );

  if (nullptr == length_unit_name.m_name || 0 == length_unit_name.m_name[0])
  {
    switch (length_unit_system)
    {
    case MYON::LengthUnitSystem::None:
      break;

    case MYON::LengthUnitSystem::Angstroms:
      length_unit_name.m_name = bPlural ? L"angstroms" : L"angstrom";
      break;
    case MYON::LengthUnitSystem::Nanometers:
      length_unit_name.m_name = bPlural ? L"nanometers" : L"nanometer";
      break;
    case MYON::LengthUnitSystem::Microns:
      length_unit_name.m_name = bPlural ? L"microns" : L"micron";
      break;
    case MYON::LengthUnitSystem::Millimeters:
      length_unit_name.m_name = bPlural ? L"millimeters" : L"millimeter";
      break;
    case MYON::LengthUnitSystem::Centimeters:
      length_unit_name.m_name = bPlural ? L"centimeters" : L"centimeter";
      break;
    case MYON::LengthUnitSystem::Decimeters:
      length_unit_name.m_name = bPlural ? L"decimeters" : L"decimeter";
      break;
    case MYON::LengthUnitSystem::Meters:
      length_unit_name.m_name = bPlural ? L"meters" : L"meter";
      break;
    case MYON::LengthUnitSystem::Dekameters:
      length_unit_name.m_name = bPlural ? L"dekameters" : L"dekameter";
      break;
    case MYON::LengthUnitSystem::Hectometers:
      length_unit_name.m_name = bPlural ? L"hectometers" : L"hectometer";
      break;
    case MYON::LengthUnitSystem::Kilometers:
      length_unit_name.m_name = bPlural ? L"kilometers" : L"kilometer";
      break;
    case MYON::LengthUnitSystem::Megameters:
      length_unit_name.m_name = bPlural ? L"megameters" : L"megameter";
      break;
    case MYON::LengthUnitSystem::Gigameters:
      length_unit_name.m_name = bPlural ? L"gigameters" : L"gigameter";
      break;
    case MYON::LengthUnitSystem::Microinches:
      length_unit_name.m_name = bPlural ? L"microinches" : L"microinche";
      break;
    case MYON::LengthUnitSystem::Mils:
      length_unit_name.m_name = bPlural ? L"mils" : L"mil";
      break;
    case MYON::LengthUnitSystem::Inches:
      length_unit_name.m_name = bPlural ? L"inches" : L"inch";
      break;
    case MYON::LengthUnitSystem::Feet:
      length_unit_name.m_name = bPlural ? L"feet" : L"foot";
      break;
    case MYON::LengthUnitSystem::Yards:
      length_unit_name.m_name = bPlural ? L"yards" : L"yard";
      break;
    case MYON::LengthUnitSystem::Miles:
      length_unit_name.m_name = bPlural ? L"miles" : L"mile";
      break;
    case MYON::LengthUnitSystem::PrinterPoints:
      length_unit_name.m_name = bPlural ? L"points" : L"point";
      break;
    case MYON::LengthUnitSystem::PrinterPicas:
      length_unit_name.m_name = bPlural ? L"picas" : L"pica";
      break;
    case MYON::LengthUnitSystem::NauticalMiles:
      length_unit_name.m_name = bPlural ? L"nauticalmiles" : L"nauticalmile"; // no spaces!
      break;
    case MYON::LengthUnitSystem::AstronomicalUnits:
      length_unit_name.m_name = bPlural ? L"AUs" : L"AU"; // no spaces!
      break;
    case MYON::LengthUnitSystem::LightYears:
      length_unit_name.m_name = bPlural ? L"lightyears" : L"lightyear"; // no spaces!
      break;
    case MYON::LengthUnitSystem::Parsecs:
      length_unit_name.m_name = bPlural ? L"parsecs" : L"parsec";
      break;
    case MYON::LengthUnitSystem::CustomUnits:
      break;
    case MYON::LengthUnitSystem::Unset:
      break;
    }
  }



  return length_unit_name;
}

int MYON_LengthUnitName::Internal_Compare(
  unsigned int order_selector,
  const MYON_LengthUnitName& a,
  const MYON_LengthUnitName& b
)
{
  if (1 == order_selector)
  {
    // locale id is first
    if (a.m_locale_id < b.m_locale_id)
      return -1;
    if (a.m_locale_id > b.m_locale_id)
      return 1;
  }

  unsigned int i = static_cast<unsigned int>(a.m_length_unit_system);
  unsigned int j = static_cast<unsigned int>(b.m_length_unit_system);
    if (i < j)
    return -1;
  if (i > j)
    return 1;

  if (1 != order_selector)
  {
    // locale id is second
    if (a.m_locale_id < b.m_locale_id)
      return -1;
    if (a.m_locale_id > b.m_locale_id)
      return 1;
  }

  i =  MYON_wString::CompareOrdinal(a.m_name,b.m_name,false);
  if (i != 0)
  {
    // ignore case order is prefered
    j = MYON_wString::CompareOrdinal(a.m_name, b.m_name, true);
    return (0 != j) ? j : i;
  }

  i = a.m_bNameIsSingular ? 1 : 0;
  j = b.m_bNameIsSingular ? 1 : 0;
  if (i < j)
    return -1;
  if (i > j)
    return 1;

  i = a.m_bNameIsPlural ? 1 : 0;
  j = b.m_bNameIsPlural ? 1 : 0;
  if (i < j)
    return -1;
  if (i > j)
    return 1;

  return 0;
}

int MYON_LengthUnitName::CompareUnitSystemLocaleIdName(
  const MYON_LengthUnitName& a,
  const MYON_LengthUnitName& b
)
{
  return Internal_Compare(0, a, b);
}

int MYON_LengthUnitName::CompareLocaleIdUnitSystemName(
  const MYON_LengthUnitName& a,
  const MYON_LengthUnitName& b
)
{
  return Internal_Compare(1, a, b);
}


unsigned int MYON_LengthUnitName::LocaleId() const
{
  return m_locale_id;
}

MYON::LengthUnitSystem MYON_LengthUnitName::LengthUnit() const
{
  return m_length_unit_system;
}

bool MYON_LengthUnitName::LengthUnitAndNameAreSet() const
{
  return LengthUnitIsSet() && LengthUnitNameIsNotEmpty();
}

bool MYON_LengthUnitName::LengthUnitIsSet() const
{
  return (MYON::LengthUnitSystem::Unset != m_length_unit_system && MYON::LengthUnitSystem::None != m_length_unit_system);
}

bool MYON_LengthUnitName::LengthUnitIsSetOrNone() const
{
  return (MYON::LengthUnitSystem::Unset != m_length_unit_system);
}

const wchar_t* MYON_LengthUnitName::LengthUnitName() const
{
  return (nullptr == m_name) ? static_cast<const wchar_t*>(MYON_wString::EmptyString) : m_name;
}

bool MYON_LengthUnitName::LengthUnitNameIsSingular() const
{
  return m_bNameIsSingular;
}

bool MYON_LengthUnitName::LengthUnitNameIsPlural() const
{
  return m_bNameIsPlural;
}

bool MYON_LengthUnitName::LengthUnitNameIsEmpty() const
{
  return (nullptr == m_name || 0 == m_name[0]);
}

bool MYON_LengthUnitName::LengthUnitNameIsNotEmpty() const
{
  return (nullptr != m_name && 0 != m_name[0]);
}

bool operator==(
  const MYON_LengthUnitName& a,
  const MYON_LengthUnitName& b
  )
{
  return (0 == MYON_LengthUnitName::CompareUnitSystemLocaleIdName(a, b));
}

bool operator!=(
  const MYON_LengthUnitName& a,
  const MYON_LengthUnitName& b
  )
{
  return (0 != MYON_LengthUnitName::CompareUnitSystemLocaleIdName(a, b));
}

MYON_AngleUnitName MYON_AngleUnitName::Create(
  unsigned int locale_id,
  MYON::AngleUnitSystem angle_unit_system,
  bool bPlural
)
{
  angle_unit_system = MYON::AngleUnitSystemFromUnsigned(static_cast<unsigned int>(angle_unit_system));
  
  size_t count = 0;
  const MYON_UnitName* names = nullptr;

  switch (angle_unit_system)
  {
  case MYON::AngleUnitSystem::None:
    break;
  case MYON::AngleUnitSystem::Turns:
    names = angle_turn_units;
    count = sizeof(angle_turn_units)/sizeof(angle_turn_units[0]);
    break;
  case MYON::AngleUnitSystem::Radians:
    names = angle_radian_units;
    count = sizeof(angle_radian_units)/sizeof(angle_radian_units[0]);
    break;
  case MYON::AngleUnitSystem::Degrees:
    names = angle_degree_units;
    count = sizeof(angle_degree_units)/sizeof(angle_degree_units[0]);
    break;
  case MYON::AngleUnitSystem::Minutes:
    names = angle_minute_units;
    count = sizeof(angle_minute_units)/sizeof(angle_minute_units[0]);
    break;
  case MYON::AngleUnitSystem::Seconds:
    names = angle_second_units;
    count = sizeof(angle_second_units)/sizeof(angle_second_units[0]);
    break;
  case MYON::AngleUnitSystem::Gradians:
    names = angle_gradian_units;
    count = sizeof(angle_gradian_units)/sizeof(angle_gradian_units[0]);
    break;
  case MYON::AngleUnitSystem::Unset:
    break;
  default:
    MYON_ERROR("Invalid angle_unit_system parameter.");
    angle_unit_system = MYON::AngleUnitSystem::Unset;
    break;
  }

  MYON_AngleUnitName angle_unit_name;
  angle_unit_name.m_locale_id = locale_id;
  angle_unit_name.m_angle_unit_system = angle_unit_system;
  angle_unit_name.m_bNameIsSingular = bPlural ? false : true;
  angle_unit_name.m_bNameIsPlural = bPlural ? true : false;

  // attempt to get localized name
  angle_unit_name.m_name = MYON_Internal_GetUnitsName(
    locale_id,
    static_cast<unsigned int>(angle_unit_system),
    bPlural,
    count,
    names
  );

  if (nullptr == angle_unit_name.m_name || 0 == angle_unit_name.m_name[0])
  {
    switch (angle_unit_system)
    {
    case MYON::AngleUnitSystem::None:
      break;
    case MYON::AngleUnitSystem::Turns:
      angle_unit_name.m_name = bPlural ? L"turns" : L"turn";
      break;
    case MYON::AngleUnitSystem::Radians:
      angle_unit_name.m_name = bPlural ? L"radians" : L"radian";
      break;
    case MYON::AngleUnitSystem::Degrees:
      angle_unit_name.m_name = bPlural ? L"degrees" : L"degree";
      break;
    case MYON::AngleUnitSystem::Minutes:
      angle_unit_name.m_name = bPlural ? L"minutes" : L"minute";
      break;
    case MYON::AngleUnitSystem::Seconds:
      angle_unit_name.m_name = bPlural ? L"seconds" : L"second";
      break;
    case MYON::AngleUnitSystem::Gradians:
      angle_unit_name.m_name = bPlural ? L"gradians" : L"gradian";
      break;
    case MYON::AngleUnitSystem::Unset:
      break;
    }
  }
   
  return angle_unit_name;
}

int MYON_AngleUnitName::Internal_Compare(
  unsigned int order_selector,
  const MYON_AngleUnitName& a,
  const MYON_AngleUnitName& b
)
{
  if (1 == order_selector)
  {
    // locale id is first
    if (a.m_locale_id < b.m_locale_id)
      return -1;
    if (a.m_locale_id > b.m_locale_id)
      return 1;
  }

  unsigned int i = static_cast<unsigned int>(a.m_angle_unit_system);
  unsigned int j = static_cast<unsigned int>(b.m_angle_unit_system);
  if (i < j)
    return -1;
  if (i > j)
    return 1;

  if (1 != order_selector)
  {
    // localed id is second
    if (a.m_locale_id < b.m_locale_id)
      return -1;
    if (a.m_locale_id > b.m_locale_id)
      return 1;
  }

  i =  MYON_wString::CompareOrdinal(a.m_name,b.m_name,false);
  if (i != 0)
  {
    // ignore case order is prefered
    j = MYON_wString::CompareOrdinal(a.m_name, b.m_name, true);
    return (0 != j) ? j : i;
  }

  i = a.m_bNameIsSingular ? 1 : 0;
  j = b.m_bNameIsSingular ? 1 : 0;
  if (i < j)
    return -1;
  if (i > j)
    return 1;

  i = a.m_bNameIsPlural ? 1 : 0;
  j = b.m_bNameIsPlural ? 1 : 0;
  if (i < j)
    return -1;
  if (i > j)
    return 1;

  return 0;
}

int MYON_AngleUnitName::CompareUnitSystemLocaleIdName(
  const MYON_AngleUnitName& a,
  const MYON_AngleUnitName& b
)
{
  return Internal_Compare(0, a, b);
}

int MYON_AngleUnitName::CompareLocaleIdUnitSystemName(
  const MYON_AngleUnitName& a,
  const MYON_AngleUnitName& b
)
{
  return Internal_Compare(1, a, b);
}


unsigned int MYON_AngleUnitName::LocaleId() const
{
  return m_locale_id;
}

MYON::AngleUnitSystem MYON_AngleUnitName::AngleUnit() const
{
  return m_angle_unit_system;
}

bool MYON_AngleUnitName::AngleUnitAndNameAreSet() const
{
  return AngleUnitIsSet() && AngleUnitNameIsNotEmpty();
}

bool MYON_AngleUnitName::AngleUnitIsSet() const
{
  return (MYON::AngleUnitSystem::Unset != m_angle_unit_system && MYON::AngleUnitSystem::None != m_angle_unit_system);
}

bool MYON_AngleUnitName::AngleUnitIsSetOrNone() const
{
  return (MYON::AngleUnitSystem::Unset != m_angle_unit_system);
}

const wchar_t* MYON_AngleUnitName::AngleUnitName() const
{
  return (nullptr == m_name) ? static_cast<const wchar_t*>(MYON_wString::EmptyString) : m_name;
}

bool MYON_AngleUnitName::AngleUnitNameIsSingular() const
{
  return m_bNameIsSingular;
}

bool MYON_AngleUnitName::AngleUnitNameIsPlural() const
{
  return m_bNameIsPlural;
}

bool MYON_AngleUnitName::AngleUnitNameIsEmpty() const
{
  return (nullptr == m_name || 0 == m_name[0]);
}

bool MYON_AngleUnitName::AngleUnitNameIsNotEmpty() const
{
  return (nullptr != m_name && 0 != m_name[0]);
}

bool operator==(
  const MYON_AngleUnitName& a,
  const MYON_AngleUnitName& b
  )
{
  return (0 == MYON_AngleUnitName::CompareUnitSystemLocaleIdName(a, b));
}

bool operator!=(
  const MYON_AngleUnitName& a,
  const MYON_AngleUnitName& b
  )
{
  return (0 != MYON_AngleUnitName::CompareUnitSystemLocaleIdName(a, b));
}

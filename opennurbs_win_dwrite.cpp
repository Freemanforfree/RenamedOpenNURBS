//
// Copyright (c) 1993-2018 Robert McNeel & Associates. All rights reserved.
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

#if defined(MYON_OS_WINDOWS_GDI)

#include "opennurbs_internal_glyph.h"
#include "opennurbs_win_dwrite.h"

/////////////////////////////////////////////////////////////////////////////
//
// Global 
//   IDWriteFactory
//   IDWriteGdiInterop
//

class MYON_IDWrite
{
public:
  static IDWriteFactory* Factory();
  static IDWriteGdiInterop* GdiInterop();

private:
  MYON_IDWrite() = default;
  ~MYON_IDWrite() = default;
  MYON_IDWrite(const MYON_IDWrite&) = delete;
  MYON_IDWrite& operator=(const MYON_IDWrite&) = delete;

private:
  static IDWriteFactory* the_dwiteFactory;
  static IDWriteGdiInterop* the_dwriteGdiInterop;
};

IDWriteFactory* MYON_IDWrite::the_dwiteFactory = nullptr;
IDWriteGdiInterop* MYON_IDWrite::the_dwriteGdiInterop = nullptr;

IDWriteFactory* MYON_IDWrite::Factory()
{
  if (nullptr == MYON_IDWrite::the_dwiteFactory)
  {
    IDWriteFactory* dwiteFactory = nullptr;
    HRESULT hr = DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED,
      __uuidof(IDWriteFactory),
      reinterpret_cast<IUnknown**>(&dwiteFactory)
    );
    if (FAILED(hr) || nullptr == dwiteFactory)
    {
      MYON_ERROR("DWriteCreateFactory() failed.");
      return nullptr;
    }
    dwiteFactory->AddRef();
    MYON_IDWrite::the_dwiteFactory = dwiteFactory;
  }
  return MYON_IDWrite::the_dwiteFactory;
}

IDWriteGdiInterop* MYON_IDWrite::GdiInterop()
{
  if (nullptr == MYON_IDWrite::the_dwriteGdiInterop)
  {
    IDWriteFactory* dwiteFactory = MYON_IDWrite::Factory();
    if (nullptr == dwiteFactory)
      return nullptr;

    IDWriteGdiInterop* dwriteGdiInterop = nullptr;
    HRESULT hr = dwiteFactory->GetGdiInterop(&dwriteGdiInterop);
    if (FAILED(hr) || nullptr == dwriteGdiInterop)
    {
      MYON_ERROR("dwiteFactory->GetGdiInterop() failed.");
      return nullptr;
    }

    dwriteGdiInterop->AddRef();
    MYON_IDWrite::the_dwriteGdiInterop = dwriteGdiInterop;
  }
  return MYON_IDWrite::the_dwriteGdiInterop;
}

/////////////////////////////////////////////////////////////////////////////
//
// 
//

static bool Internal_GetLocalizeStrings(
  const MYON_wString preferedLocaleDirty,
  IDWriteLocalizedStrings* pIDWriteLocalizedStrings,
  MYON_wString& defaultLocaleString,
  MYON_wString& defaultLocale,
  MYON_wString& enLocaleString,
  MYON_wString& enLocale
)
{
  defaultLocaleString = MYON_wString::EmptyString;
  defaultLocale = MYON_wString::EmptyString;
  enLocaleString = MYON_wString::EmptyString;
  enLocale = MYON_wString::EmptyString;


  // get a clean preferedLocale with reliable storage for the string.
  MYON_wString preferedLocale(preferedLocaleDirty.Duplicate());
  preferedLocale.TrimLeftAndRight();
  if (MYON_wString::EqualOrdinal(preferedLocale, L"GetUserDefaultLocaleName", true))
  {
    // Get the default locale for this user.
    wchar_t userDefaultLocale[LOCALE_NAME_MAX_LENGTH + 1] = {};
    userDefaultLocale[LOCALE_NAME_MAX_LENGTH] = 0;
    if (0 == ::GetUserDefaultLocaleName(userDefaultLocale, LOCALE_NAME_MAX_LENGTH))
      userDefaultLocale[0];
    userDefaultLocale[LOCALE_NAME_MAX_LENGTH] = 0;
    preferedLocale = MYON_wString(userDefaultLocale);
    preferedLocale.TrimLeftAndRight();
  }

  const wchar_t* localeNames[] = 
  {
    static_cast<const wchar_t*>(preferedLocale),

    // The preferred English dialect should be listed next
    L"en-us", // United States

    // Other known English dialects are list below
    L"en-au", // Australia
    L"en-bz", // Belize
    L"en-ca", // Canada
    L"en-cb", // Caribbean
    L"en-gb", // Great Britain
    L"en-in", // India
    L"en-ie", // Ireland
    L"en-jm", // Jamaica
    L"en-nz", // New Zealand
    L"en-ph", // Philippines
    L"en-tt", // Trinidad
    L"en-za", // Southern Africa
  };

  const int localNamesCount = (int)(sizeof(localeNames)/sizeof(localeNames[0]));

  HRESULT hr;

  if (nullptr == pIDWriteLocalizedStrings)
    return MYON_wString::EmptyString;

  const UINT32 count = pIDWriteLocalizedStrings->GetCount();
  if (count < 1)
    return MYON_wString::EmptyString;

  UINT32 localeIndex = MYON_UNSET_UINT_INDEX;
  UINT32 enLocaleIndex = MYON_UNSET_UINT_INDEX;
  for (int i = 0; i < localNamesCount; i++)
  {
    const wchar_t* s = localeNames[i];
    if (nullptr == s || 0 == s[0])
      continue;
    BOOL exists = 0;
    UINT32 idx = MYON_UNSET_UINT_INDEX;
    hr = pIDWriteLocalizedStrings->FindLocaleName(s, &idx, &exists);
    if (FAILED(hr))
      continue;
    if (false == exists)
      continue;
    if (idx >= count)
      continue;
    if (i > 0)
    {
      enLocaleIndex = idx;
      enLocale = s;
    }
    else
    {
      // preferred locale
      localeIndex = idx;
      if (
        0 != s[0] && 0 != s[1] && 0 != s[2] 
        && MYON_wString::EqualOrdinal(L"en-", 3, s, 3, true)
        )
      {
        // preferred local is the preferred English dialect too.
        enLocaleIndex = idx;
        break;
      }
    }
  }
    
  for (int pass = 0; pass < 3; pass++)
  {
    if (pass > 1)
    {
      if (defaultLocaleString.IsNotEmpty() || enLocaleString.IsNotEmpty())
        break;
    }

    UINT32 i0, i1;
    if (0 == pass)
    {
      if (MYON_UNSET_UINT_INDEX == localeIndex)
        continue;
      i0 = localeIndex;
      i1 = i0 + 1;
    }
    else if (1 == pass)
    {

      if (MYON_UNSET_UINT_INDEX == enLocaleIndex || enLocaleIndex == localeIndex )
        continue;
      i0 = enLocaleIndex;
      i1 = i0 + 1;
    }
    else
    {
      if (defaultLocaleString.IsNotEmpty())
        break;
      i0 = 0;
      i1 = count;
    }

    for (UINT32 i = i0; i < i1; i++)
    {
      MYON_wString strStringValue;
      MYON_wString strLocaleName;
      if (i == localeIndex)
        strLocaleName = preferedLocale;
      else if (i == enLocaleIndex)
        strLocaleName = enLocale;
      for (int value = 0; value < (strLocaleName.IsEmpty()?2:1); value++)
      {
        const bool bStringValuePass = (0 == value);
        UINT32 slen = 0;
        hr = bStringValuePass
          ? pIDWriteLocalizedStrings->GetStringLength(i, &slen)
          : pIDWriteLocalizedStrings->GetLocaleNameLength(i, &slen)
          ;
        if (FAILED(hr))
          continue;
        if (slen < 1)
          continue;
        if (slen > 1024 * 1024)
          continue; // sanity check
        MYON_wString& s = bStringValuePass ? strStringValue : strLocaleName;
        s.ReserveArray(slen + 2);
        wchar_t* buffer = s.Array();
        buffer[0] = 0;
        buffer[slen] = 0;
        buffer[slen + 1] = 0;
        hr = bStringValuePass
          ? pIDWriteLocalizedStrings->GetString(i, buffer, slen + 1)
          : pIDWriteLocalizedStrings->GetLocaleName(i, buffer, slen + 1);
        if (SUCCEEDED(hr) && 0 != buffer[0] && 0 == buffer[slen + 1])
        {
          s.SetLength(MYON_wString::Length(buffer));
          s.TrimLeftAndRight();
        }
        else
          s = MYON_wString::EmptyString;
        if (bStringValuePass)
        {
          if (s.IsEmpty())
            break;
        }
      }

      if (strStringValue.IsEmpty())
        continue;
      
      if ( pass >= 2 && defaultLocaleString.IsEmpty() )
      {
        defaultLocaleString = strStringValue;
        defaultLocale = strLocaleName;
      }

      if (i == localeIndex)
      {
        defaultLocaleString = strStringValue;
        defaultLocale = strLocaleName;
      }

      if (i == enLocaleIndex)
      {
        enLocaleString = strStringValue;
        enLocale = strLocaleName;
      }

      if (
        defaultLocaleString.IsNotEmpty()
        && enLocaleString.IsNotEmpty()
        )
      {
        break;
      }     
    }
  }

  if (defaultLocaleString.IsEmpty())
  {
    defaultLocaleString = enLocaleString;
    defaultLocale = enLocale;
  }

  return defaultLocaleString.IsNotEmpty();
}


static bool Internal_GetLocalizeStrings(
  const wchar_t* preferedLocale,
  IDWriteLocalizedStrings* pIDWriteLocalizedStrings,
  MYON_wString& defaultLocaleString,
  MYON_wString& enLocaleString
)
{
  MYON_wString defaultLocale;
  MYON_wString enusLocale;
  return Internal_GetLocalizeStrings(
    preferedLocale,
    pIDWriteLocalizedStrings,
    defaultLocaleString, defaultLocale,
    enLocaleString, enusLocale
  );
}

static void Internal_PrintLocalizedNames(
  const MYON_wString stringDescription,
  const wchar_t* preferedLocale,
  IDWriteLocalizedStrings* pDWriteLocalizedStrings,
  MYON_TextLog& text_log
)
{
  MYON_ClassArray<MYON_wString> localizedStrings;
  MYON_wString preferedString;
  MYON_wString enusString;
  Internal_GetLocalizeStrings(preferedLocale, pDWriteLocalizedStrings, preferedString, enusString);
  text_log.Print(L"%ls: \"%ls\"\n", static_cast<const wchar_t*>(stringDescription), static_cast<const wchar_t*>(preferedString));
  if (enusString.IsNotEmpty() && enusString != preferedString)
  {
    text_log.PushIndent();
    text_log.Print(
      L"locale \"en-us\": \"%ls\"\n",
      static_cast<const wchar_t*>(enusString)
    );
    text_log.PopIndent();
  }
}

class Internal_DWriteFontInformation
{
public:
  Internal_DWriteFontInformation(
    DWRITE_INFORMATIONAL_STRING_ID id,
    const wchar_t* description
  )
    : m_id(id)
    , m_description(description)
  {}
  DWRITE_INFORMATIONAL_STRING_ID m_id = DWRITE_INFORMATIONAL_STRING_NONE;
  MYON_wString m_description;
};

static const MYON_wString Internal_DWRITE_FONT_WEIGHT_ToString(DWRITE_FONT_WEIGHT dwrite_weight)
{
  MYON_wString s;
  switch (dwrite_weight)
  {
  case DWRITE_FONT_WEIGHT_THIN: s = L"THIN"; break;// 100

    //case DWRITE_FONT_WEIGHT_EXTRA_LIGHT: s = L"";  break; // 200
    case DWRITE_FONT_WEIGHT_ULTRA_LIGHT: s = L"ULTRA_LIGHT"; break;//  200,

    case DWRITE_FONT_WEIGHT_LIGHT: s = L"LIGHT"; break; //  300,
    case DWRITE_FONT_WEIGHT_SEMI_LIGHT: s = L"SEMI_LIGHT"; break; //  350,

    case DWRITE_FONT_WEIGHT_NORMAL: s = L"NORMAL"; break; //  400,
    //case DWRITE_FONT_WEIGHT_REGULAR: s = L""; break; //  400,

    case DWRITE_FONT_WEIGHT_MEDIUM: s = L"MEDIUM"; break; //  500,

    //case DWRITE_FONT_WEIGHT_DEMI_BOLD: s = L""; break; //  600,
    case DWRITE_FONT_WEIGHT_SEMI_BOLD: s = L"SEMI_BOLD"; break; //  600,

    case DWRITE_FONT_WEIGHT_BOLD: s = L"BOLD"; break; //  700,

    //case DWRITE_FONT_WEIGHT_EXTRA_BOLD: s = L""; break; //  800,
    case DWRITE_FONT_WEIGHT_ULTRA_BOLD: s = L"ULTRA_BOLD"; break; //  800,

    //case DWRITE_FONT_WEIGHT_BLACK: s = L""; break; //  900,
    case DWRITE_FONT_WEIGHT_HEAVY: s = L"HEAVY"; break; //  900,

    //case DWRITE_FONT_WEIGHT_EXTRA_BLACK: s = L""; break; //  950,
    case DWRITE_FONT_WEIGHT_ULTRA_BLACK: s = L"ULTRA_BLACK"; break; //  950

    default:
      s = MYON_wString::FromNumber((unsigned int)dwrite_weight); break;
  };
  return s;
}

static const MYON_wString Internal_DWRITE_FONT_STRETCH_ToString(DWRITE_FONT_STRETCH dwrite_stretch)
{
  MYON_wString s;
  switch (dwrite_stretch)
  {
    case DWRITE_FONT_STRETCH_UNDEFINED: s = L"UNDEFINED"; break; // 0,
    case DWRITE_FONT_STRETCH_ULTRA_CONDENSED: s = L"ULTRA_CONDENSED"; break; // 1,
    case DWRITE_FONT_STRETCH_EXTRA_CONDENSED: s = L"EXTRA_CONDENSED"; break; // 2,
    case DWRITE_FONT_STRETCH_CONDENSED: s = L"CONDENSED"; break; // 3,
    case DWRITE_FONT_STRETCH_SEMI_CONDENSED: s = L"CONDENSED"; break; // 4,
    case DWRITE_FONT_STRETCH_NORMAL: s = L"NORMAL"; break; // 5,
    //case DWRITE_FONT_STRETCH_MEDIUM: s = L"MEDIUM"; break; // 5,
    case DWRITE_FONT_STRETCH_SEMI_EXPANDED: s = L"SEMI_EXPANDED"; break; // 6,
    case DWRITE_FONT_STRETCH_EXPANDED: s = L"EXPANDED"; break; // 7,
    case DWRITE_FONT_STRETCH_EXTRA_EXPANDED: s = L"EXTRA_EXPANDED"; break; // 8,
    case DWRITE_FONT_STRETCH_ULTRA_EXPANDED: s = L"ULTRA_EXPANDED"; break; // 9  }; break;
    default:
      s = MYON_wString::FromNumber((unsigned int)dwrite_stretch); break;
  }
  return s;
}

static const MYON_wString Internal_DWRITE_FONT_STYLE_ToString(DWRITE_FONT_STYLE dwrite_style)
{
  MYON_wString s;
  switch (dwrite_style)
  {
    case DWRITE_FONT_STYLE_NORMAL: s = L"NORMAL"; break;
    case DWRITE_FONT_STYLE_OBLIQUE: s = L"OBLIQUE"; break;
    case DWRITE_FONT_STYLE_ITALIC: s = L"ITALIC"; break;
    default:
      s = MYON_wString::FromNumber((unsigned int)dwrite_style); break;
  };

  return s;
}

void MYON_Font::DumpWindowsDWriteFont(
  IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale,
  MYON_TextLog& text_log
)
{
  if (nullptr == dwrite_font)
  {
    text_log.Print("IDWriteFont = nullptr\n");
    return;
  }

  const DWRITE_FONT_SIMULATIONS simulation = dwrite_font->GetSimulations();
  if (DWRITE_FONT_SIMULATIONS_NONE != simulation)
    text_log.Print("IDWriteFont SIMULATED:\n");
  else
    text_log.Print("IDWriteFont:\n");

  MYON_TextLogIndent indent1(text_log);

  HRESULT hr;

  // Family name
  Microsoft::WRL::ComPtr<IDWriteFontFamily> pIDWriteFontFamily = nullptr;
  hr = dwrite_font->GetFontFamily(&pIDWriteFontFamily);
  if ((SUCCEEDED(hr)) && nullptr != pIDWriteFontFamily)
  {
     Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> familyNames = nullptr;
    hr = pIDWriteFontFamily->GetFamilyNames(&familyNames);
    if (SUCCEEDED(hr))
    {
      Internal_PrintLocalizedNames(L"Family name", preferedLocale, familyNames.Get(), text_log);
    }
  }

  // Face name
  Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> faceNames = nullptr;
  hr = dwrite_font->GetFaceNames(&faceNames);
  if (SUCCEEDED(hr))
    Internal_PrintLocalizedNames(L"Face name", preferedLocale, faceNames.Get(), text_log);

  const DWRITE_FONT_WEIGHT dwrite_weight = dwrite_font->GetWeight();
  const MYON_wString weightString = Internal_DWRITE_FONT_WEIGHT_ToString(dwrite_weight);
  text_log.Print("Weight = %ls\n", static_cast<const wchar_t*>(weightString));
  
  const DWRITE_FONT_STYLE dwrite_style = dwrite_font->GetStyle();
  const MYON_wString styleString = Internal_DWRITE_FONT_STYLE_ToString(dwrite_style);
  text_log.Print("Style = %ls\n", static_cast<const wchar_t*>(styleString));
  
  const DWRITE_FONT_STRETCH dwrite_stretch = dwrite_font->GetStretch();
  const MYON_wString stretchString = Internal_DWRITE_FONT_STRETCH_ToString(dwrite_stretch);
  text_log.Print("Stretch = %ls\n", static_cast<const wchar_t*>(stretchString));
  
  const bool bIsSymbolFont = (dwrite_font->IsSymbolFont() ? true : false);
  text_log.Print("IsSymbolFont = %s.\n",(bIsSymbolFont?"true":"false"));

  if (DWRITE_FONT_SIMULATIONS_NONE != simulation)
  {
    const bool bBold = ((MYON__UINT32)DWRITE_FONT_SIMULATIONS_BOLD) & ((MYON__UINT32)simulation);
    const bool bOblique = ((MYON__UINT32)DWRITE_FONT_SIMULATIONS_OBLIQUE) & ((MYON__UINT32)simulation);
    text_log.Print("Simulations:");
    if (bBold)
      text_log.Print(" BOLD");
    if (bOblique)
      text_log.Print(" OBLIQUE");
    MYON__UINT32 s = 0;
    if (bBold)
      s |= (MYON__UINT32)DWRITE_FONT_SIMULATIONS_BOLD;
    if (bOblique)
      s |= (MYON__UINT32)DWRITE_FONT_SIMULATIONS_OBLIQUE;
    if ( s != (MYON__UINT32)simulation )
      text_log.Print(" other");
    text_log.PrintNewLine();
  }

  // Other names:
  Internal_DWriteFontInformation info_strings[] =
  {
    // field 4
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_FULL_NAME,L"DWrite full name"),

    // field 6
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_NAME,L"DWrite PostScript name"),

    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES,L"GDI family name"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_WIN32_SUBFAMILY_NAMES,L"GDI sub-family name"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_WEIGHT_STRETCH_STYLE_FAMILY_NAME,L"DWrite weight-stretch-style model family name"),

    // other fields - useful when trying to compare fonts / bugs from different computers
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_COPYRIGHT_NOTICE,L"DWrite field 0 copyright"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_VERSION_STRINGS,L"DWrite field 5 version"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_TRADEMARK,L"DWrite field 7 trademark"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_MANUFACTURER,L"DWrite field 8 manufacturer"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_DESIGNER,L"DWrite field 9 designer/foundary"),

    // DWRITE_INFORMATIONAL_STRING_DESCRIPTION gets field 10 from the ttf file.
    // Opennurbs searches the description saved in field 10 of the name table
    // for the strings "Engraving - single stroke" / "Engraving - double stroke" / "Engraving"
    // to identify fonts that are desgned for engraving (and which tend to render poorly when
    // used to dispaly text devices like screens, monitors, and printers).
    // The SLF (single line fonts) are examples of fonts that have Engraving in field 10.
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_DESCRIPTION,L"DWrite field 10 description"),

    // other fields - useful when trying to compare fonts / bugs from different computers
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_FONT_VENDOR_URL,L"DWrite field 11 vendor URL"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_DESIGNER_URL,L"DWrite field 12 designer URL"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_LICENSE_DESCRIPTION,L"DWrite field 13 license"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_LICENSE_INFO_URL,L"DWrite field 14 license URL"),
    Internal_DWriteFontInformation(DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_CID_NAME,L"Dwrite field 20 PostScript CID findfont name")
  };

  const size_t info_strings_count = sizeof(info_strings) / sizeof(info_strings[0]);
  for (size_t i = 0; i < info_strings_count; i++)
  {
    BOOL exists = false;
    Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> fontNames = nullptr;
    hr = dwrite_font->GetInformationalStrings(info_strings[i].m_id,&fontNames,&exists);
    if (SUCCEEDED(hr) && exists)
      Internal_PrintLocalizedNames(info_strings[i].m_description, preferedLocale, fontNames.Get(), text_log);
  }

  const MYON_wString postscriptName = MYON_Font::PostScriptNameFromWindowsDWriteFont(dwrite_font, preferedLocale);
  text_log.Print(L"MYON_Font PostScript name = \"%ls\"\n", static_cast<const wchar_t*>(postscriptName));

  // Opennurbs searches the description saved in field 10 of the name table
  // for the strings "Engraving - single stroke" / "Engraving - double stroke" / "Engraving"
  // to identify fonts that are desgned for engraving (and which tend to render poorly when
  // used to dispaly text devices like screens, monitors, and printers).
  // The SLF (single line fonts) are examples of fonts that have Engraving in field 10.
  const MYON_wString field_10_description = MYON_Font::Field_10_DescriptionFromWindowsDWriteFont(dwrite_font, preferedLocale);
  text_log.Print(L"MYON_Font field 10 description = \"%ls\"\n", static_cast<const wchar_t*>(field_10_description));
   
  bool bGotLogfont = false;
  for (;;)
  {
    IDWriteGdiInterop* dwriteGdiInterop = MYON_IDWrite::GdiInterop();
    if (nullptr == dwriteGdiInterop)
      break;

    BOOL isSystemFont = false;
    LOGFONTW logfont;
    memset(&logfont, 0, sizeof(logfont));
    hr = dwriteGdiInterop->ConvertFontToLOGFONT(dwrite_font, &logfont, &isSystemFont);
    if (FAILED(hr))
      break;
    text_log.Print("GDI interop IsSystemFont = %s\n", (isSystemFont ? "true" : "false"));
    text_log.Print("GDI interop ");
    MYON_Font::DumpLogfont(&logfont, text_log);
    bGotLogfont = true;
    break;
  }

  if (false == bGotLogfont)
  {
    text_log.Print("GDI interop LOGFONT - FAILED.\n");
  }
}

MYON_Font::Stretch MYON_Font::FontStretchFromDWriteStretch(
  unsigned int dwrite_stretch,
  MYON_Font::Stretch undefined_result
)
{
  MYON_Font::Stretch font_stretch;

  switch (dwrite_stretch)
  {
    case DWRITE_FONT_STRETCH_UNDEFINED:
      font_stretch = undefined_result; 
      break;

    case DWRITE_FONT_STRETCH_ULTRA_CONDENSED: 
      font_stretch = MYON_Font::Stretch::Ultracondensed;
      break;
    case DWRITE_FONT_STRETCH_EXTRA_CONDENSED: 
      font_stretch = MYON_Font::Stretch::Extracondensed; 
      break;
    case DWRITE_FONT_STRETCH_CONDENSED: 
      font_stretch = MYON_Font::Stretch::Condensed; 
      break;
    case DWRITE_FONT_STRETCH_SEMI_CONDENSED:
      font_stretch = MYON_Font::Stretch::Semicondensed;
      break;
    case DWRITE_FONT_STRETCH_MEDIUM: 
      font_stretch = MYON_Font::Stretch::Medium;
      break;
    case DWRITE_FONT_STRETCH_SEMI_EXPANDED: 
      font_stretch = MYON_Font::Stretch::Semiexpanded;
      break;
    case DWRITE_FONT_STRETCH_EXPANDED: 
      font_stretch = MYON_Font::Stretch::Expanded;
      break;
    case DWRITE_FONT_STRETCH_EXTRA_EXPANDED:
      font_stretch = MYON_Font::Stretch::Extraexpanded;
      break;
    case DWRITE_FONT_STRETCH_ULTRA_EXPANDED: 
      font_stretch = MYON_Font::Stretch::Ultraexpanded;
      break;

    default:
      font_stretch = undefined_result; 
      break;
  }

  return font_stretch;
}

const MYON_wString MYON_Font::PostScriptNameFromWindowsDWriteFont(
  IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  for (;;)
  {
    if (nullptr == dwrite_font)
      break;
    BOOL exists = false;
    Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> postscriptNames = nullptr;
    HRESULT hr = dwrite_font->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_NAME, &postscriptNames, &exists);
    if (FAILED(hr))
      break;
    MYON_wString loc_PostScriptName;
    MYON_wString en_PostScriptName;
    if (exists)
      Internal_GetLocalizeStrings(preferedLocale, postscriptNames.Get(), loc_PostScriptName, en_PostScriptName);

    // This was wrong.
    // Bahnschrift is an OpenType variable font and all faces of variable fonts
    // end up with the same PostScript name. In short, a PostScript name property
    // cannot be used as a "unique id" for a font face.

    ////if (MYON_wString::EqualOrdinal(L"Bahnschrift", postscriptName, true))
    ////{
    ////  // All faces of Bahnshrift (Win 10 en-us Feb 2018) return the same PostScriptName ( "Bahnschrift" ) A bug in the font file?
    ////  MYON_wString suffix;
    ////  const DWRITE_FONT_WEIGHT weight = dwrite_font->GetWeight();
    ////  switch (weight)
    ////  {
    ////  case DWRITE_FONT_WEIGHT_THIN: suffix += L"Thin"; break; //; // 100
    ////
    ////  //case DWRITE_FONT_WEIGHT_EXTRA_LIGHT: suffix += L"Bold"; break; //; // 200
    ////  case DWRITE_FONT_WEIGHT_ULTRA_LIGHT: suffix += L"UltraLight"; break; //; //  200,
    ////
    ////  case DWRITE_FONT_WEIGHT_LIGHT: suffix += L"Light"; break; //; //  300,
    ////  case DWRITE_FONT_WEIGHT_SEMI_LIGHT: suffix += L"Semilight"; break; //; //  350,
    ////
    ////  case DWRITE_FONT_WEIGHT_NORMAL: break;
    ////
    ////  case DWRITE_FONT_WEIGHT_MEDIUM: suffix += L"Medium"; break; //; //  500,
    ////
    ////  //case DWRITE_FONT_WEIGHT_DEMI_BOLD: suffix += L"Bold"; break; //; //  600,
    ////  case DWRITE_FONT_WEIGHT_SEMI_BOLD: suffix += L"Semibold"; break; //; //  600,
    ////
    ////  case DWRITE_FONT_WEIGHT_BOLD: suffix += L"Bold"; break; //; //  700,
    ////
    ////  //case DWRITE_FONT_WEIGHT_EXTRA_BOLD: suffix += L"Bold"; break; //; //  800,
    ////  case DWRITE_FONT_WEIGHT_ULTRA_BOLD: suffix += L"UltraBold"; break; //; //  800,
    ////
    ////  //case DWRITE_FONT_WEIGHT_BLACK: suffix += L"Bold"; break; //; //  900,
    ////  case DWRITE_FONT_WEIGHT_HEAVY: suffix += L"Heavy"; break; //; //  900,
    ////
    ////  //case DWRITE_FONT_WEIGHT_EXTRA_BLACK: suffix += L"Bold"; break; //; //  950,
    ////  case DWRITE_FONT_WEIGHT_ULTRA_BLACK: suffix += L"UltraBlack"; break; //; //  950
    ////
    ////  default:
    ////    break;
    ////  }
    ////
    ////  const DWRITE_FONT_STYLE style = dwrite_font->GetStyle();
    ////  switch (style)
    ////  {
    ////  case DWRITE_FONT_STYLE_NORMAL: break;
    ////  case DWRITE_FONT_STYLE_OBLIQUE: suffix += L"Oblique"; break;
    ////  case DWRITE_FONT_STYLE_ITALIC: suffix += L"Italic"; break;
    ////  default:
    ////    break;
    ////  }
    ////
    ////  if (suffix.IsNotEmpty())
    ////  {
    ////    postscriptName += MYON_wString::HyphenMinus;
    ////    postscriptName += suffix;
    ////  }
    ////}
    return loc_PostScriptName.IsNotEmpty() ? loc_PostScriptName : en_PostScriptName;
  }
  return MYON_wString::EmptyString;
}

static const MYON_wString Internal_InfoStringFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  enum DWRITE_INFORMATIONAL_STRING_ID info_string_id,
  const wchar_t* preferedLocale
)
{
  for (;;)
  {
    if (nullptr == dwrite_font)
      break;
    BOOL exists = false;
    Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> info_string = nullptr;
    HRESULT hr = dwrite_font->GetInformationalStrings(info_string_id, &info_string, &exists);
    if (FAILED(hr))
      break;
    MYON_wString loc_description;
    MYON_wString en_description;
    if (exists)
      Internal_GetLocalizeStrings(preferedLocale, info_string.Get(), loc_description, en_description);

    return loc_description.IsNotEmpty() ? loc_description : en_description;
  }
  return MYON_wString::EmptyString;
}

const MYON_wString MYON_Font::WeightStretchStyleModelFamilyNameFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_WEIGHT_STRETCH_STYLE_FAMILY_NAME, preferedLocale);
}

const MYON_wString MYON_Font::Field_0_CopyrightFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_COPYRIGHT_NOTICE, preferedLocale);
}

const MYON_wString MYON_Font::Field_5_VersionFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_VERSION_STRINGS, preferedLocale);
}

const MYON_wString MYON_Font::Field_7_TrademarkFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_TRADEMARK, preferedLocale);
}

const MYON_wString MYON_Font::Field_8_ManufacturerFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_MANUFACTURER, preferedLocale);
}

const MYON_wString MYON_Font::Field_9_DesignerFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_DESIGNER, preferedLocale);
}

// Returns the desription saved in field 10. 
// Opennurbs searches the description saved in field 10 of the name table
// for the strings "Engraving - single stroke" / "Engraving - double stroke" / "Engraving"
// to identify fonts that are desgned for engraving (and which tend to render poorly when
// used to dispaly text devices like screens, monitors, and printers).
// The SLF (single line fonts) are examples of fonts that have Engraving in field 10.
const MYON_wString MYON_Font::Field_10_DescriptionFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font,DWRITE_INFORMATIONAL_STRING_DESCRIPTION, preferedLocale);
}


const MYON_wString MYON_Font::Field_11_VendorURLFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_FONT_VENDOR_URL, preferedLocale);
}

const MYON_wString MYON_Font::Field_12_DesignerURLFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_DESIGNER_URL, preferedLocale);
}

const MYON_wString MYON_Font::Field_13_LicenseFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_LICENSE_DESCRIPTION, preferedLocale);
}

const MYON_wString MYON_Font::Field_14_LicenseURLFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_LICENSE_INFO_URL, preferedLocale);
}

const MYON_wString MYON_Font::Field_20_PostScriptCIDNameFromWindowsDWriteFont(
  struct IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  return Internal_InfoStringFromWindowsDWriteFont(dwrite_font, DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_CID_NAME, preferedLocale);
}

void MYON_WindowsDWriteFontInformation::Dump(MYON_TextLog& text_log) const
{
  text_log.Print("IDWriteFont:\n");

  const bool bTerse = text_log.LevelOfDetailIsAtLeast(MYON_TextLog::LevelOfDetail::Medium);
  const bool bChatty = text_log.LevelOfDetailIsAtLeast(MYON_TextLog::LevelOfDetail::Medium);
  const bool bVerbose = text_log.LevelOfDetailIsAtLeast(MYON_TextLog::LevelOfDetail::Maximum);

  MYON_TextLogIndent indent1(text_log);
  if ( bVerbose )
  {
    if (m_prefered_locale.IsNotEmpty())
      text_log.Print("Prefered locale = \"%ls\"\n", static_cast<const wchar_t*>(m_prefered_locale));
    if (FamilyName().IsNotEmpty())
    {
      if (FaceName().IsNotEmpty())
        text_log.Print("Iteration index = family[%d].font[%u]\n", m_family_index, m_family_font_index);
      else
        text_log.Print("Iteration index = family[%d]\n", m_family_index);
    }
  }

  MYON_wString family_name = m_loc_family_name;
  MYON_wString face_name = m_loc_face_name;
  MYON_wString postscript_name = m_loc_postscript_name;
  MYON_wString logfont_name = m_loc_gdi_family_name;
  if (false == bChatty)
  {
    if (family_name.IsEmpty())
      family_name = m_en_family_name;
    if (face_name.IsEmpty())
      face_name = m_en_face_name;
    if (postscript_name.IsEmpty())
      postscript_name = m_en_postscript_name;
    if (logfont_name.IsEmpty())
      logfont_name = m_en_gdi_family_name;
  }

  if (bChatty)
    text_log.Print(L"Localized Names:\n");
  else
    text_log.Print(L"Names:\n");
  
  text_log.PushIndent();

  text_log.Print(L"Family name = \"%ls\"\n", static_cast<const wchar_t*>(family_name));
  text_log.Print(L"Face name = \"%ls\"\n", static_cast<const wchar_t*>(face_name));
  text_log.Print(L"PostScript name = \"%ls\"\n", static_cast<const wchar_t*>(postscript_name));
  text_log.Print(L"GDI LOGFONT name = \"%ls\"\n", static_cast<const wchar_t*>(logfont_name));

  if ( bChatty )
  {
    text_log.Print(L"GDI subfamily name = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_gdi_subfamily_name));
    text_log.Print(L"Full name = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_full_name));
    if ( bVerbose )
    {
      text_log.Print(L"Direct Write Weight-Stretch-Style Model Name = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_weight_stretch_style_model_name));

      text_log.Print(L"Field 0 Copyright = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_0_copyright));
      text_log.Print(L"Field 5 Version = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_5_version));
      text_log.Print(L"Field 7 Trademark = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_7_trademark));
      text_log.Print(L"Field 8 Manufacturer = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_8_manufacturer));
      text_log.Print(L"Field 9 Designer = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_9_designer));
      text_log.Print(L"Field 10 Description = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_10_description));
      text_log.Print(L"Field 11 Vendor URL = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_11_vendor_URL));
      text_log.Print(L"Field 12 Designer_URL = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_12_designer_URL));
      text_log.Print(L"Field 13 License = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_13_license));
      text_log.Print(L"Field 14 License URL = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_14_license_URL));
      text_log.Print(L"Field 20 PostScript CID Name = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_20_postscript_cid));
    }
  }

  if ( false == bVerbose )
  {
    if (MYON_OutlineFigure::Type::Unset != MYON_OutlineFigure::FigureTypeFromField10Description(m_loc_field_10_description))
      text_log.Print(L"Field 10 Description = \"%ls\"\n", static_cast<const wchar_t*>(m_loc_field_10_description));
    else if (MYON_OutlineFigure::Type::Unset != MYON_OutlineFigure::FigureTypeFromField10Description(m_en_field_10_description))
      text_log.Print(L"Field 10 Description = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_10_description));
  }

  text_log.PopIndent();

  if ( bChatty )
  {
    text_log.Print(L"English Names:\n");
    MYON_TextLogIndent indent2(text_log);
    text_log.Print(L"Family name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_family_name));
    text_log.Print(L"Face name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_face_name));
    text_log.Print(L"PostScript name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_postscript_name));
    text_log.Print(L"GDI LOGFONT name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_gdi_family_name));
    text_log.Print(L"GDI subfamily name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_gdi_subfamily_name));
    text_log.Print(L"Full name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_full_name));

    if ( bVerbose )
    {
      text_log.Print(L"Direct Write Weight-Stretch-Style Model Name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_weight_stretch_style_model_name));

      text_log.Print(L"Field 0 Copyright = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_0_copyright));
      text_log.Print(L"Field 5 Version = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_5_version));
      text_log.Print(L"Field 7 Trademark = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_7_trademark));
      text_log.Print(L"Field 8 Manufacturer = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_8_manufacturer));
      text_log.Print(L"Field 9 Designer = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_9_designer));
      text_log.Print(L"Field 10 Description = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_10_description));
      text_log.Print(L"Field 11 Vendor URL = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_11_vendor_URL));
      text_log.Print(L"Field 12 Designer_URL = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_12_designer_URL));
      text_log.Print(L"Field 13 License = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_13_license));
      text_log.Print(L"Field 14 License URL = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_14_license_URL));
      text_log.Print(L"Field 20 PostScript CID Name = \"%ls\"\n", static_cast<const wchar_t*>(m_en_field_20_postscript_cid));
    }
  }

  if (bChatty)
  {
    if (m_bSimulatedBold || m_bSimulatedOblique || m_bSimulatedOther)
    {
      text_log.Print("Simuilations =");
      if (m_bSimulatedBold)
        text_log.Print(" BOLD");
      if (m_bSimulatedOblique)
        text_log.Print(" OBLIQUE");
      if (m_bSimulatedOther)
        text_log.Print(" other");
      text_log.PrintNewLine();
    }

    const DWRITE_FONT_WEIGHT dwrite_weight = (DWRITE_FONT_WEIGHT)m_weight;
    const MYON_wString weightString = Internal_DWRITE_FONT_WEIGHT_ToString(dwrite_weight);
    text_log.Print(L"Weight = %ls\n", static_cast<const wchar_t*>(weightString));

    const DWRITE_FONT_STYLE dwrite_style = (DWRITE_FONT_STYLE)m_style;
    const MYON_wString styleString = Internal_DWRITE_FONT_STYLE_ToString(dwrite_style);
    text_log.Print(L"Style = %ls\n", static_cast<const wchar_t*>(styleString));

    const DWRITE_FONT_STRETCH dwrite_stretch = (DWRITE_FONT_STRETCH)m_stretch;
    const MYON_wString stretchString = Internal_DWRITE_FONT_STRETCH_ToString(dwrite_stretch);
    text_log.Print(L"Stretch = %ls\n", static_cast<const wchar_t*>(stretchString));
  }

  if ( bVerbose )
  {
    text_log.Print("IsSymbolFont = %s.\n", (m_bIsSymbolFont ? "true" : "false"));
    if (0 != m_gdi_interop_logfont.lfFaceName[0])
    {
      text_log.Print("GDI interop IsSystemFont = %s\n", (m_gdi_interop_logfont_bIsSystemFont ? "true" : "false"));
      text_log.Print("GDI interop ");
      MYON_Font::DumpLogfont(&m_gdi_interop_logfont, text_log);
    }
  }
}

////////////////////////////////////////////////////////////////////////////
//
// DWRITE_FONT_METRICS to MYON_FontMetrics
// 

const MYON_FontMetrics MYON_FontMetrics::CreateFromDWriteFontMetrics(const struct DWRITE_FONT_METRICS* dwrite_font_metrics)
{
  if (nullptr == dwrite_font_metrics)
    return MYON_FontMetrics::Unset;

   MYON_FontMetrics fm;
  const int ascent = ((int)dwrite_font_metrics->ascent);

  // dwrite_font_metrics->descent is unsigned and hence >= 0
  const int signed_descent = -((int)dwrite_font_metrics->descent);

  // line_gap is signed and can be positive or negative.
  const int line_gap = ((int)dwrite_font_metrics->lineGap);

  const int line_space = ascent - signed_descent + line_gap;

  fm.SetHeights(
    dwrite_font_metrics->ascent,
    signed_descent, 
    dwrite_font_metrics->designUnitsPerEm,
    line_space
  );
  
  const int cap_height = ((int)dwrite_font_metrics->capHeight);
  fm.SetAscentOfCapital(cap_height);
  
  const int x_height = ((int)dwrite_font_metrics->xHeight);
  fm.SetAscentOfx(x_height);

  fm.SetStrikeout(
    (int)(dwrite_font_metrics->strikethroughPosition),
    (int)(dwrite_font_metrics->strikethroughThickness)
  );
  
  fm.SetUnderscore(
    (int)(dwrite_font_metrics->underlinePosition),
    (int)(dwrite_font_metrics->underlineThickness)
  );

  return fm;
}

const MYON_FontMetrics MYON_FontMetrics::CreateFromDWriteFont(struct IDWriteFont* dwrite_font)
{
  if (nullptr == dwrite_font)
    return MYON_FontMetrics::Unset;

  DWRITE_FONT_METRICS dwriteFontMetrics;
  memset(&dwriteFontMetrics, 0, sizeof(dwriteFontMetrics));
  dwrite_font->GetMetrics(&dwriteFontMetrics);
  return MYON_FontMetrics::CreateFromDWriteFontMetrics(&dwriteFontMetrics);
}


//////////////////////////////////////////////////////////////////////////
//
// Get list of fonts installed on this Windows device
//

static int Internal_CompareDWriteFontInformationSort0(
  const MYON_WindowsDWriteFontInformation* lhs,
  const MYON_WindowsDWriteFontInformation* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;

  const MYON_wString lhs_family = lhs->FamilyName();
  const MYON_wString rhs_family = rhs->FamilyName();
  int rc = MYON_wString::CompareOrdinal(lhs_family, rhs_family, true);
  if (0 != rc)
    return rc;

  const MYON_wString lhs_logfont = lhs->WindowsLogfontName();
  const MYON_wString rhs_logfont = rhs->WindowsLogfontName();
  rc = MYON_wString::CompareOrdinal(lhs_logfont, rhs_logfont, true);
  if (0 != rc)
    return rc;

  rc = ((int)lhs->m_weight) - ((int)rhs->m_weight);
  if (0 != rc)
    return rc;

  rc = ((int)lhs->m_stretch) - ((int)rhs->m_stretch);
  if (0 != rc)
    return rc;

  const int lhs_style
    = (lhs->m_bSimulatedOblique && DWRITE_FONT_STYLE_OBLIQUE == lhs->m_style)
    ? DWRITE_FONT_STYLE_ITALIC
    : lhs->m_style;
  const int rhs_style
    = (rhs->m_bSimulatedOblique && DWRITE_FONT_STYLE_OBLIQUE == rhs->m_style)
    ? DWRITE_FONT_STYLE_ITALIC
    : rhs->m_style;
  rc = lhs_style - rhs_style;
  if (0 != rc)
    return rc;

  return 0;
}

static bool Internal_DWriteFontIsSimulated(
  const MYON_WindowsDWriteFontInformation* f
)
{
  return (f->m_bSimulatedBold || f->m_bSimulatedOblique || f->m_bSimulatedOther);
}

static int Internal_CompareDWriteFontInformationSort1(
  const MYON_WindowsDWriteFontInformation* lhs,
  const MYON_WindowsDWriteFontInformation* rhs
)
{
  int rc = Internal_CompareDWriteFontInformationSort0(lhs, rhs);
  if (0 != rc)
    return rc;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;

  const int lhs_sim = Internal_DWriteFontIsSimulated(lhs) ? 1 : 0;
  const int rhs_sim = Internal_DWriteFontIsSimulated(rhs) ? 1 : 0;
  rc = (lhs_sim - rhs_sim);
  if (0 != rc)
    return rc;

  return 0;
}

//// Used to test ability to fallback to English file name when 
//// files from a different local are read.
//static const MYON_wString Internal_FakeLocaleTest(
//  const MYON_wString loc_str
//)
//{
//  if (loc_str.IsEmpty())
//    return MYON_wString::EmptyString;
//  const MYON_wString fake_loc_str = MYON_wString(L"FAKE") + loc_str;
//  return fake_loc_str;
//}
//
//static void Internal_FakeLocaleTest(
//  MYON_WindowsDWriteFontInformation& fi
//)
//{
//  fi.m_prefered_locale = L"fa-ke";
//  fi.m_loc_family_name = Internal_FakeLocaleTest(fi.m_loc_family_name);
//  fi.m_loc_face_name = Internal_FakeLocaleTest(fi.m_loc_face_name);
//  fi.m_loc_postscript_name = Internal_FakeLocaleTest(fi.m_loc_postscript_name);
//  fi.m_loc_gdi_family_name = Internal_FakeLocaleTest(fi.m_loc_gdi_family_name);
//  fi.m_loc_gdi_subfamily_name = Internal_FakeLocaleTest(fi.m_loc_gdi_subfamily_name);
//  MYON_wString fake_logfont = Internal_FakeLocaleTest(fi.m_gdi_interop_logfont.lfFaceName);
//  const size_t sz_lfFacename = sizeof(fi.m_gdi_interop_logfont.lfFaceName);
//  const size_t sz_fakeFaceName = fake_logfont.Length() * sizeof(wchar_t);
//  if (sz_fakeFaceName < sz_lfFacename)
//  {
//    wchar_t* a = fake_logfont.Array();
//    memset(fi.m_gdi_interop_logfont.lfFaceName, 0, sz_lfFacename);
//    memcpy(fi.m_gdi_interop_logfont.lfFaceName, a, sz_fakeFaceName);
//  }
//}

unsigned int MYON_Font::GetInstalledWindowsDWriteFonts(
  const wchar_t* preferedLocale,
  bool bIncludeSimulatedFontFaces,
  bool bKeepDWriteFont,
  MYON_SimpleArray<MYON_WindowsDWriteFontInformation>& dwrite_font_list
)
{
  dwrite_font_list.SetCount(0);

  const wchar_t* englishLocale = L"en-us";

  for (;;)
  {
    IDWriteFactory* dwriteFactory = MYON_IDWrite::Factory();
    if (nullptr == dwriteFactory)
      break;

    BOOL exists;
    HRESULT hr;

    // Get the system font collection.
    Microsoft::WRL::ComPtr<IDWriteFontCollection> dwriteFontCollection = nullptr;
    hr = dwriteFactory->GetSystemFontCollection(&dwriteFontCollection);
    if (FAILED(hr))
      break;
    if (nullptr == dwriteFontCollection || nullptr == dwriteFontCollection.Get())
      break;
    const UINT32 familyCount = dwriteFontCollection->GetFontFamilyCount();
    if (familyCount < 1)
      break;

    wchar_t userDefaultLocale[LOCALE_NAME_MAX_LENGTH + 1] = {};
    if (MYON_wString::EqualOrdinal(preferedLocale, L"GetUserDefaultLocaleName", true))
    {
      // Get the default locale for this user.
      userDefaultLocale[LOCALE_NAME_MAX_LENGTH] = 0;
      if (0 == ::GetUserDefaultLocaleName(userDefaultLocale, LOCALE_NAME_MAX_LENGTH))
        userDefaultLocale[0];
      userDefaultLocale[LOCALE_NAME_MAX_LENGTH] = 0;
      preferedLocale = userDefaultLocale;
    }

    IDWriteGdiInterop* dwriteGdiInterop = MYON_IDWrite::GdiInterop();
    if (nullptr == dwriteGdiInterop)
      break;

    dwrite_font_list.Reserve(5 * familyCount);

    for (UINT32 familyIndex = 0; familyIndex < familyCount; ++familyIndex)
    {
      Microsoft::WRL::ComPtr<IDWriteFontFamily> dwriteFontFamily = nullptr;
      hr = dwriteFontCollection->GetFontFamily(familyIndex, &dwriteFontFamily);
      if (FAILED(hr))
        continue;
      if (nullptr == dwriteFontFamily || nullptr == dwriteFontFamily.Get())
        continue;

      // Family name
      Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> familyNames = nullptr;
      hr = dwriteFontFamily->GetFamilyNames(&familyNames);
      MYON_wString loc_family_name;
      MYON_wString en_family_name;
      Internal_GetLocalizeStrings(preferedLocale, familyNames.Get(), loc_family_name, en_family_name);

      const UINT32 fontCount = dwriteFontFamily->GetFontCount();
      for (UINT32 fontIndex = 0; fontIndex < fontCount; fontIndex++)
      {
        Microsoft::WRL::ComPtr<IDWriteFont> dwriteFont = nullptr;
        hr = dwriteFontFamily->GetFont(fontIndex, &dwriteFont);
        if (FAILED(hr))
          continue;
        if (nullptr == dwriteFont || nullptr == dwriteFont.Get())
          continue;

        MYON_WindowsDWriteFontInformation fi;
        fi.m_prefered_locale = preferedLocale;
        memset(&fi.m_gdi_interop_logfont, 0, sizeof(fi.m_gdi_interop_logfont));

        const DWRITE_FONT_SIMULATIONS simulations = dwriteFont->GetSimulations();
        if (DWRITE_FONT_SIMULATIONS_NONE != simulations)
        {
          if (false == bIncludeSimulatedFontFaces)
            continue;
          DWRITE_FONT_SIMULATIONS s = DWRITE_FONT_SIMULATIONS_NONE;
          if (0 != (DWRITE_FONT_SIMULATIONS_BOLD & simulations))
          {
            s |= DWRITE_FONT_SIMULATIONS_BOLD;
            fi.m_bSimulatedBold = true;
          }
          if (0 != (DWRITE_FONT_SIMULATIONS_OBLIQUE & simulations))
          {
            s |= DWRITE_FONT_SIMULATIONS_OBLIQUE;
            fi.m_bSimulatedOblique = true;
          }
          if (s != simulations)
            fi.m_bSimulatedOther = true;
        }

        // Face name
        Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> faceNames = nullptr;
        hr = dwriteFont->GetFaceNames(&faceNames);
        if (SUCCEEDED(hr))
        {
          Internal_GetLocalizeStrings(preferedLocale, faceNames.Get(), fi.m_loc_face_name, fi.m_en_face_name);
        }

        fi.m_weight = dwriteFont->GetWeight();
        fi.m_style = dwriteFont->GetStyle();
        fi.m_stretch = dwriteFont->GetStretch();
        fi.m_bIsSymbolFont = (dwriteFont->IsSymbolFont() ? true : false);

        // Full name
        exists = false;
        Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> fullNames = nullptr;
        hr = dwriteFont->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_FULL_NAME, &fullNames, &exists);
        if (SUCCEEDED(hr) && exists)
        {
          Internal_GetLocalizeStrings(preferedLocale, fullNames.Get(), fi.m_loc_full_name, fi.m_en_full_name );
        }

        // PostScript name 
        fi.m_loc_postscript_name = MYON_Font::PostScriptNameFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_postscript_name = MYON_Font::PostScriptNameFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // GDI family name
        exists = false;
        Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> gdiFamilyNames = nullptr;
        hr = dwriteFont->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES, &gdiFamilyNames, &exists);
        if (SUCCEEDED(hr) && exists)
        {
          Internal_GetLocalizeStrings(preferedLocale, gdiFamilyNames.Get(), fi.m_loc_gdi_family_name, fi.m_en_gdi_family_name);
        }

        // GDI sub-family name
        exists = false;
        Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> gdiSubfamilyNames = nullptr;
        hr = dwriteFont->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_WIN32_SUBFAMILY_NAMES, &gdiSubfamilyNames, &exists);
        if (SUCCEEDED(hr) && exists)
        {
          Internal_GetLocalizeStrings(preferedLocale, gdiSubfamilyNames.Get(), fi.m_loc_gdi_subfamily_name, fi.m_en_gdi_subfamily_name);
        }

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_WEIGHT_STRETCH_STYLE_FAMILY_NAME, ... )
        fi.m_en_weight_stretch_style_model_name = MYON_Font::WeightStretchStyleModelFamilyNameFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_weight_stretch_style_model_name = MYON_Font::WeightStretchStyleModelFamilyNameFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_COPYRIGHT_NOTICE, ... )
        fi.m_loc_field_0_copyright = MYON_Font::Field_0_CopyrightFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_0_copyright = MYON_Font::Field_0_CopyrightFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_VERSION_STRINGS, ... )
        fi.m_loc_field_5_version = MYON_Font::Field_5_VersionFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_5_version = MYON_Font::Field_5_VersionFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_TRADEMARK, ... )
        fi.m_loc_field_7_trademark = MYON_Font::Field_7_TrademarkFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_7_trademark = MYON_Font::Field_7_TrademarkFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_MANUFACTURER, ... )
        fi.m_loc_field_8_manufacturer = MYON_Font::Field_8_ManufacturerFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_8_manufacturer = MYON_Font::Field_8_ManufacturerFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_DESIGNER, ... )
        fi.m_loc_field_9_designer = MYON_Font::Field_9_DesignerFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_9_designer = MYON_Font::Field_9_DesignerFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // Field 10 Description
        // Opennurbs searches the description saved in field 10 of the name table
        // for the strings "Engraving - single stroke" / "Engraving - double stroke" / "Engraving"
        // to identify fonts that are desgned for engraving (and which tend to render poorly when
        // used to dispaly text devices like screens, monitors, and printers).
        // The SLF (single line fonts) are examples of fonts that have Engraving in field 10.
        fi.m_loc_field_10_description = MYON_Font::Field_10_DescriptionFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_10_description = MYON_Font::Field_10_DescriptionFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        fi.m_loc_field_11_vendor_URL = MYON_Font::Field_11_VendorURLFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_11_vendor_URL = MYON_Font::Field_11_VendorURLFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_DESIGNER_URL, ... )
        fi.m_loc_field_12_designer_URL = MYON_Font::Field_12_DesignerURLFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_12_designer_URL = MYON_Font::Field_12_DesignerURLFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_LICENSE_DESCRIPTION, ... )
        fi.m_loc_field_13_license = MYON_Font::Field_13_LicenseFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_13_license = MYON_Font::Field_13_LicenseFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_LICENSE_INFO_URL, ... )
        fi.m_loc_field_14_license_URL = MYON_Font::Field_14_LicenseURLFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_14_license_URL = MYON_Font::Field_14_LicenseURLFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        // from IDWriteFont.GetInformationalStrings( DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_CID_NAME, ... )
        fi.m_loc_field_20_postscript_cid = MYON_Font::Field_20_PostScriptCIDNameFromWindowsDWriteFont(dwriteFont.Get(), preferedLocale);
        fi.m_en_field_20_postscript_cid = MYON_Font::Field_20_PostScriptCIDNameFromWindowsDWriteFont(dwriteFont.Get(), englishLocale);

        BOOL isSystemFont = false;
        LOGFONTW logfont;
        memset(&logfont, 0, sizeof(logfont));
        hr = dwriteGdiInterop->ConvertFontToLOGFONT(dwriteFont.Get(), &logfont, &isSystemFont);
        if (SUCCEEDED(hr) && 0 != logfont.lfFaceName[0])
        {
          logfont.lfHeight = 0;
          if (0 != logfont.lfItalic) logfont.lfItalic = 1;
          if (0 != logfont.lfUnderline) logfont.lfUnderline = 1;
          if (0 != logfont.lfStrikeOut) logfont.lfStrikeOut = 1;
          if (MYON_Font::WindowsConstants::logfont_symbol_charset != logfont.lfCharSet)
            logfont.lfCharSet = MYON_Font::WindowsConstants::logfont_default_charset;
          logfont.lfOutPrecision = MYON_Font::WindowsConstants::logfont_out_precis;
          logfont.lfClipPrecision = 0;
          logfont.lfQuality = MYON_Font::WindowsConstants::logfont_quality;
          logfont.lfPitchAndFamily = MYON_Font::WindowsConstants::logfont_pitch_and_family;
          fi.m_gdi_interop_logfont = logfont;
          fi.m_gdi_interop_logfont_bIsSystemFont = isSystemFont ? true : false;
        }

        if (0 == fi.m_gdi_interop_logfont.lfFaceName[0])
        {
          // During testing on Windows 10, this never occured.
          // ...
          // Dale Lear 7 Jan 2019.
          // It appears Turbo Tax 2018 installs 4 faces of Avenir LT with
          // empty family name and empty logfont name. It has non-empty PostScript names.
          //
          //   C:\PROGRAM FILES (X86)\TURBOTAX\DELUXE 2018\32BIT\LOCAL\FUEGO\HOST\AVENIR\
          //   Avenir LT 35 Light: AVENIRLT-LIGHT.TTF 
          //   Avenir LT 55 Roman: AVENIRLT-ROMAN.TTF 
          //   Avenir LT 65 Medium: AVENIRLT-MEDIUM.TTF 
          //   Avenir LT 85 Heavy: AVENIRLT-HEAVY.TTF 
          //
          // These fonts never appear in Windows Notepad, WordPad, or Word font UI. 
          // The Windows Setting font list leaves the name of this font blank as well. 
          // So I'm going to comment out the call to MYON_ERROR.
          //
          ////MYON_ERROR("Empty LOGFONT lfFaceName");
          continue;
        }

        fi.m_font_metrics = MYON_FontMetrics::CreateFromDWriteFont(dwriteFont.Get());

        for (;;)
        {
          IDWriteFont1* dwriteFont1 = nullptr;
          hr = dwriteFont.Get()->QueryInterface(__uuidof(IDWriteFont1), (void**)&dwriteFont1);
          if (FAILED(hr))
            break; // On some versions of Windows 7, IDWriteFont1 is not available
          if (nullptr == dwriteFont1)
            break;
          DWRITE_PANOSE dwritePANOSE;
          memset(&dwritePANOSE, 0, sizeof(dwritePANOSE));
          dwriteFont1->GetPanose(&dwritePANOSE);
          MYON_PANOSE1 panose1;
          fi.m_panose1.SetTenBytes(dwritePANOSE.values);
          dwriteFont1->Release();
          break;
        }




        if (MYON_OutlineFigure::Type::Unset == fi.m_outline_figure_type)
        {
          fi.m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromField10Description(fi.m_loc_field_10_description);
          if (MYON_OutlineFigure::Type::Unset == fi.m_outline_figure_type)
            fi.m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromField10Description(fi.m_loc_field_10_description);

          if (MYON_OutlineFigure::Type::Unset == fi.m_outline_figure_type)
            fi.m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(fi.m_loc_family_name);
          if (MYON_OutlineFigure::Type::Unset == fi.m_outline_figure_type
            && false == fi.m_en_family_name.EqualOrdinal(fi.m_loc_family_name, true)
            )
            fi.m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(fi.m_en_family_name);

          if (MYON_OutlineFigure::Type::Unset == fi.m_outline_figure_type)
            fi.m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(fi.m_loc_postscript_name);
          if (MYON_OutlineFigure::Type::Unset == fi.m_outline_figure_type
            && false == fi.m_en_postscript_name.EqualOrdinal(fi.m_loc_postscript_name, true)
            )
            fi.m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(fi.m_en_postscript_name);

          if (MYON_OutlineFigure::Type::Unset == fi.m_outline_figure_type)
            fi.m_outline_figure_type = MYON_OutlineFigure::Type::Unknown;
        }

        for (;;)
        {
          Microsoft::WRL::ComPtr<IDWriteFontFace> dwriteFontFace = nullptr;
          hr = dwriteFont->CreateFontFace(&dwriteFontFace);
          if (FAILED(hr))
            break;
          if (nullptr == dwriteFontFace || nullptr == dwriteFontFace.Get())
            break;

          UINT32 codepoints[] = {MYON_wString::Space, 'H', 'I', 'x'};
          UINT32 codepointCount = ((UINT32)sizeof(codepoints) / sizeof(codepoints[0]));
          UINT16 glyphIndices[sizeof(codepoints) / sizeof(codepoints[0])] = {};
          hr = dwriteFontFace.Get()->GetGlyphIndices(codepoints, codepointCount, glyphIndices);
          if (FAILED(hr))
            break;

          for (UINT32 k = 0; k < codepointCount; k++)
          {
            UINT16 glyphIndex = glyphIndices[k];
            if (0 == glyphIndex)
              continue;
            DWRITE_GLYPH_METRICS glyphMetric;
            memset(&glyphMetric, 0, sizeof(glyphMetric));
            BOOL isSideways = false;
            hr = dwriteFontFace.Get()->GetDesignGlyphMetrics(&glyphIndex, 1, &glyphMetric, isSideways);
            if (FAILED(hr))
              break;
            MYON_FontGlyph glyph_box;
            glyph_box.m_code_point = codepoints[k];
            glyph_box.m_font_glyph_index = glyphIndex;

            // In font design units
            glyph_box.m_font_unit_glyph_bbox = MYON_TextBox::CreateFromDWriteGlyphMetrics(&glyphMetric);

            // NOT applicable
            glyph_box.m_font_unit_glyph_bbox.m_max_basepoint.i = 0;
            glyph_box.m_font_unit_glyph_bbox.m_max_basepoint.j = 0;

            if (fi.m_font_metrics.UPM() > 0)
            {
              double scale = ((double)MYON_Font::AnnotationFontCellHeight) / ((double)fi.m_font_metrics.UPM());
              glyph_box.m_normalized_glyph_bbox = MYON_TextBox::Scale(glyph_box.m_font_unit_glyph_bbox, scale);
            }

            switch (codepoints[k])
            {
            case MYON_UnicodeCodePoint::MYON_Space:
              fi.m_Spacebox = glyph_box;
              break;
            case 'H':
              fi.m_Hbox = glyph_box;
              break;
            case 'I':
              fi.m_Ibox = glyph_box;
              break;
            case 'x':
              fi.m_xbox = glyph_box;
              break;
            default:
              break;
            }
          }

          break;
        }


        fi.m_family_index = familyIndex;
        fi.m_family_font_index = fontIndex;
        fi.m_loc_family_name = loc_family_name;
        fi.m_en_family_name = en_family_name;
        fi.m_prefered_locale = preferedLocale;

        if (bKeepDWriteFont)
        {
          fi.m_dwrite_font = dwriteFont.Detach();
        }

        //Internal_FakeLocaleTest(fi);
        dwrite_font_list.AppendNew() = fi;
      }
    }
    break;
  }

  // Removed redundant simulated fonts
  //
  // Example:
  //   KEEP Arial+Narrow Italic [normal-condensed-italic] (no simuliations)
  //   DISCARD: Arial+Narrow Obliqued [normal-condensed-italic(simulated)]
  //
  //   KEEP Arial+Narrow Bold Italic [bold-condensed-italic] (no simuliations)
  //   DISCARD: Arial+Narrow Bold Obliqued [(bold-condensed-italic(simulated)]
  //
  //   KEEP Arial+Black Italic [heavy-medium-italic(simulated)]
  //   because there is not a non-simulated version of Arial Black Italic
  //
  dwrite_font_list.QuickSort(Internal_CompareDWriteFontInformationSort1);
  const unsigned int count0 = dwrite_font_list.Count();
  MYON_WindowsDWriteFontInformation* a = dwrite_font_list.Array();
  MYON_WindowsDWriteFontInformation* a0 = nullptr;
  unsigned int count1 = 0;
  for ( unsigned int i = 0; i < count0; i++ )
  { 
    if ( Internal_DWriteFontIsSimulated(a + i) )
    {
      if ( 0 == Internal_CompareDWriteFontInformationSort0(a0, a + i) )
        continue; // simulation duplicates previous font
      if (a[i].m_bSimulatedBold)
      {
        // skip engraving fonts with simulated bold.
        MYON_OutlineFigure::Type figure_type = MYON_OutlineFigure::FigureTypeFromField10Description(a[i].m_loc_field_10_description);
        if (MYON_OutlineFigure::Type::Unset == figure_type )
          figure_type = MYON_OutlineFigure::FigureTypeFromField10Description(a[i].m_en_field_10_description);
        if (MYON_OutlineFigure::Type::Unset == figure_type)
          figure_type = MYON_OutlineFigure::FigureTypeFromFontName(a[i].FamilyName());
        if (MYON_OutlineFigure::Type::SingleStroke == figure_type)
          continue;
        if (MYON_OutlineFigure::Type::DoubleStroke == figure_type)
          continue;
      }
    }
    a0 = a + i;
    if ( count1 < i)
      a[count1] = a[i];
    count1++;
  }
  dwrite_font_list.SetCount(count1);

  return dwrite_font_list.UnsignedCount();
}

const MYON_TextBox MYON_TextBox::CreateFromDWriteGlyphMetrics(const struct DWRITE_GLYPH_METRICS* dwrite_glyph_metrics)
{
  if (nullptr == dwrite_glyph_metrics)
    return MYON_TextBox::Unset;

  MYON_TextBox glyph_box;
  // See image of Q
  // https://msdn.microsoft.com/en-us/library/system.windows.media.charactermetrics.blackboxheight(v=vs.85)?cs-save-lang=1&cs-lang=vb
  //const int blackBoxWidth = ((int)glyphMetric.advanceWidth) - ((int)glyphMetric.leftSideBearing) - ((int)glyphMetric.rightSideBearing);
  //const int blackBoxHeight = ((int)glyphMetric.advanceHeight) - ((int)glyphMetric.bottomSideBearing) - ((int)glyphMetric.topSideBearing);

  // In font design units
  glyph_box.m_advance.i = (int)(dwrite_glyph_metrics->advanceWidth);
  glyph_box.m_advance.j = (int)(dwrite_glyph_metrics->advanceHeight);

  glyph_box.m_bbmin.i = (int)(dwrite_glyph_metrics->leftSideBearing);
  glyph_box.m_bbmax.i = ((int)dwrite_glyph_metrics->advanceWidth) - ((int)dwrite_glyph_metrics->rightSideBearing);

  glyph_box.m_bbmin.j = ((int)dwrite_glyph_metrics->verticalOriginY) - ((int)dwrite_glyph_metrics->advanceHeight) + ((int)dwrite_glyph_metrics->bottomSideBearing);
  glyph_box.m_bbmax.j = ((int)dwrite_glyph_metrics->verticalOriginY) - ((int)dwrite_glyph_metrics->topSideBearing);

  return glyph_box;
}

///////////////////////////////////////////////////////////////////////
//
// Glyph outline 
//

class MYON_IDWriteGlyphOutlineAccumlator : public IDWriteGeometrySink
{
public:
  MYON_IDWriteGlyphOutlineAccumlator() = default;  
  virtual ~MYON_IDWriteGlyphOutlineAccumlator()
  {};

private:
  MYON_IDWriteGlyphOutlineAccumlator(const MYON_IDWriteGlyphOutlineAccumlator&) = delete;
  MYON_IDWriteGlyphOutlineAccumlator& operator=(const MYON_IDWriteGlyphOutlineAccumlator&) = delete;

public:
  MYON_OutlineAccumulator * m_accumulator = nullptr;

  D2D1_FILL_MODE m_fill_mode = D2D1_FILL_MODE::D2D1_FILL_MODE_ALTERNATE;
  D2D1_PATH_SEGMENT m_segment_flags = D2D1_PATH_SEGMENT::D2D1_PATH_SEGMENT_NONE;

private:
  long m_IDUnknown_reference_count = 0;

public:
  // IUnknown overrides
  unsigned long STDMETHODCALLTYPE AddRef() override
  { 
    return ++m_IDUnknown_reference_count;
  }

  unsigned long STDMETHODCALLTYPE Release() override
  {
    return --m_IDUnknown_reference_count;
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(
    IID const& riid, 
    void** ppvObject
  ) override
  {
    if (nullptr == ppvObject)
      return E_POINTER;

    if (
      __uuidof(IUnknown) == riid
      || __uuidof(IDWriteGeometrySink) == riid
      )
    {
      *ppvObject = this;
      return S_OK;
    }

    return E_FAIL;
  }

public:
  // IDWriteGeometrySink overrides
  void STDMETHODCALLTYPE AddBeziers(
    const D2D1_BEZIER_SEGMENT *beziers, 
    UINT32 beziersCount
  ) override
  {
    if (nullptr == m_accumulator)
      return;
    for (UINT32 i = 0; i < beziersCount; i++)
    {
      m_accumulator->AppendCubicBezier(
        *((const MYON_2fPoint*)&(beziers[i].point1)),
        *((const MYON_2fPoint*)&(beziers[i].point2)),
        *((const MYON_2fPoint*)&(beziers[i].point3))
      );
    }
  }

  void STDMETHODCALLTYPE AddLines(
    const D2D1_POINT_2F *points, 
    UINT32 pointsCount
  ) override
  {
    if (nullptr == m_accumulator)
      return;
    for (UINT32 i = 0; i < pointsCount; i++)
    {
      m_accumulator->AppendLine(*((const MYON_2fPoint*)&(points[i])));
    }
  }

  void STDMETHODCALLTYPE BeginFigure(
    D2D1_POINT_2F startPoint, 
    D2D1_FIGURE_BEGIN figureBegin
  ) override
  {
    if (nullptr == m_accumulator)
      return;

    MYON_OutlineFigurePoint::Type point_type 
      = MYON_OutlineFigurePoint::Type::BeginFigureUnknown;

    // Turns out the figureBegin parameter has no meaning when getting glyph outlines.
    // In tests of many fonts and glyphs, it is always 0.

    ////if (D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED == figureBegin)
    ////  point_type = MYON_OutlineFigurePoint::Type::BeginFigureFilled;
    ////else if (D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW == figureBegin)
    ////  point_type = MYON_OutlineFigurePoint::Type::BeginFigureHollow;
    ////else
    ////{
    ////  MYON_ERROR("Invalid D2D1_FIGURE_BEGIN figureBegin parameter");
    ////  m_accumulator->AbandonFigure();
    ////  return;
    ////}

    const MYON_2fPoint point(startPoint.x, startPoint.y);
    m_accumulator->BeginFigure( point_type, point );
  }

  HRESULT STDMETHODCALLTYPE Close() override
  {
    return S_OK;
  }

  void STDMETHODCALLTYPE EndFigure(D2D1_FIGURE_END figureEnd) override
  {
    if (nullptr == m_accumulator)
      return;

    MYON_OutlineFigurePoint::Type point_type;
    if ( D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN == figureEnd)
      point_type = MYON_OutlineFigurePoint::Type::EndFigureOpen;
    else if ( D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED == figureEnd)
      point_type = MYON_OutlineFigurePoint::Type::EndFigureClosed;
    else
    {
      MYON_ERROR("Invalid D2D1_FIGURE_END figureEnd parameter");
      m_accumulator->AbandonCurrentFigure();
      return;
    }

    m_accumulator->EndFigure(point_type);
  }

  void STDMETHODCALLTYPE SetFillMode(
    D2D1_FILL_MODE fillMode
  ) override
  {
    // From Microsoft Docs:
    //   The fill mode defaults to D2D1_FILL_MODE_ALTERNATE. 
    //   To set the fill mode, call SetFillMode before the first call to BeginFigure. 
    //   Not doing will put the geometry sink in an error state.
    //D2D1_FILL_MODE::D2D1_FILL_MODE_ALTERNATE = 0,
    //D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING = 1,
    if (m_fill_mode != fillMode)
    {
      m_fill_mode = fillMode;
    }
  }

  void STDMETHODCALLTYPE SetSegmentFlags(
    D2D1_PATH_SEGMENT vertexFlags
  ) override
  {
    // From Microsoft Docs:
    //   After this method is called, the specified segment flags are applied to each segment subsequently added to the sink. 
    //   The segment flags are applied to every additional segment until this method is called again and a different set of 
    //   segment flags is specified.
    //D2D1_PATH_SEGMENT::D2D1_PATH_SEGMENT_NONE = 0,
    //D2D1_PATH_SEGMENT::D2D1_PATH_SEGMENT_FORCE_UNSTROKED = 1,
    //D2D1_PATH_SEGMENT::D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN = 2,
    if (m_segment_flags != vertexFlags)
    {
      m_segment_flags = vertexFlags;
    }
  }
};

bool MYON_WindowsDWriteGetGlyphOutline(
  struct IDWriteFont* dwriteFont,
  unsigned int dwriteGlyphIndex,
  MYON_OutlineFigure::Type figure_type,
  class MYON_Outline& outline
)
{
  outline = MYON_Outline::Unset;

  if (nullptr == dwriteFont)
    return false;

  if ( dwriteGlyphIndex <= 0 || dwriteGlyphIndex > 0xFFFF )
    return false;

  DWRITE_FONT_METRICS dwriteFontMetrics;
  memset(&dwriteFontMetrics, 0, sizeof(dwriteFontMetrics));
  dwriteFont->GetMetrics(&dwriteFontMetrics);
  

  Microsoft::WRL::ComPtr<IDWriteFontFace> dwriteFontFace = nullptr;
  HRESULT hr = dwriteFont->CreateFontFace(&dwriteFontFace);
  if (FAILED(hr))
    return false;
  if (nullptr == dwriteFontFace || nullptr == dwriteFontFace.Get())
    return false;

  MYON_OutlineAccumulator accumulator;
  accumulator.BeginGlyphOutline(
    dwriteFontMetrics.designUnitsPerEm,
    figure_type,
    &outline
  );

  MYON_IDWriteGlyphOutlineAccumlator dwrite_accumulator;

  dwrite_accumulator.m_accumulator = &accumulator;

  const UINT32 glyphCount = 1;
  const BOOL isSideways = 0; // FALSE - We want the single glyph with respect to the English baseline
  const BOOL isRightToLeft = 0; // FALSE - We want the single glyph positioned for LTR rendering (RTL adjustments made elsewhere)
  const FLOAT emSize = dwriteFontMetrics.designUnitsPerEm; //96.0f; // in DIP units 1 DIP = 1/96 inch
  const UINT16 glyphIndices[2] = { (UINT16)dwriteGlyphIndex, 0 };

  dwriteFontFace->GetGlyphRunOutline(
    emSize,
    glyphIndices,
    nullptr, // OPTIONAL - FLOAT glyphAdvances[]
    nullptr, // OPTIONAL - DWRITE_GLYPH_OFFSET glyphOffsets[]
    glyphCount,
    isSideways,
    isRightToLeft, // FALSE - We want the single glyph positioned for LTR rendering (RTL adjustments made elsewhere)
    &dwrite_accumulator
    );

  const bool bNegatePointY = true;
  // NOTE WELL: 
  //  OpenType fonts can have glyphs with either orientation.
  //  We have to specify MYON_OutlineFigure::Orientation::Clockwise
  //  to get consistent results. MYON_OutlineFigure::Orientation::Clockwise
  //  is the default "TrueType" orientation. The Klavika OpenType font is
  //  an example where the outlines returned by DirectWrite are "backwards"
  //  from those returned by 
  accumulator.EndOutline(bNegatePointY, MYON_Outline::DefaultOuterOrientation);

  DWRITE_GLYPH_METRICS  glyphMetrics[2] = {};

  hr = dwriteFontFace->GetDesignGlyphMetrics(
    glyphIndices,
    glyphCount,
    glyphMetrics,
    isSideways
    );

  if (SUCCEEDED(hr))
  {
    outline.SetGlyphMetrics( MYON_TextBox::CreateFromDWriteGlyphMetrics(&glyphMetrics[0]) );
  }
  else
  {
    outline.SetGlyphMetrics( MYON_TextBox::Unset );
  }

  return true;
}

bool MYON_WindowsDWriteGetGlyphMetrics(
  struct IDWriteFont* dwriteFont,
  unsigned int dwriteGlyphIndex,
  class MYON_TextBox& glyph_metrics
)
{
  glyph_metrics = MYON_TextBox::Unset;

  if (nullptr == dwriteFont)
    return false;

  if ( dwriteGlyphIndex <= 0 || dwriteGlyphIndex > 0xFFFF )
    return false;

  Microsoft::WRL::ComPtr<IDWriteFontFace> dwriteFontFace = nullptr;
  HRESULT hr = dwriteFont->CreateFontFace(&dwriteFontFace);
  if (FAILED(hr))
    return false;
  if (nullptr == dwriteFontFace || nullptr == dwriteFontFace.Get())
    return false;

  const UINT32 glyphCount = 1;
  const BOOL isSideways = 0;
  const BOOL isRightToLeft = 0;
  const UINT16 glyphIndices[2] = { (UINT16)dwriteGlyphIndex, 0 };

  DWRITE_GLYPH_METRICS  glyphMetrics[2] = {};

  hr = dwriteFontFace->GetDesignGlyphMetrics(
    glyphIndices,
    glyphCount,
    glyphMetrics,
    isSideways
    );
  if (FAILED(hr))
    return false;

  glyph_metrics = MYON_TextBox::CreateFromDWriteGlyphMetrics(&glyphMetrics[0]);
  return true;
}

void MYON_WindowsDWriteGetFontMetrics(
  const MYON_Font* font,
  MYON_FontMetrics& font_metrics
)
{
  font_metrics = MYON_FontMetrics::Unset;

  for (;;)
  {
    if (nullptr == font)
      break;

    IDWriteFont* dwriteFont = font->WindowsDWriteFont();
    if (nullptr == dwriteFont)
      break;

    font_metrics = MYON_FontMetrics::CreateFromDWriteFont(dwriteFont);

    dwriteFont->Release();
    break;
  }

  return;
}

unsigned int MYON_WindowsDWriteGetGlyphMetrics(
  const MYON_FontGlyph* glyph,
  MYON_TextBox& glyph_metrics
)
{
  glyph_metrics = MYON_TextBox::Unset;

  if (nullptr == glyph)
    return 0;

  if (false == glyph->CodePointIsSet())
    return 0;

  const MYON_Font* font = glyph->Font();
  if (nullptr == font)
    return 0;

  IDWriteFont* dwriteFont = font->WindowsDWriteFont();
  if (nullptr == dwriteFont)
    return 0;

  unsigned int glyph_index = 0;
  for(;;)
  {
    glyph_index
      = glyph->FontGlyphIndexIsSet()
      ? glyph->FontGlyphIndex()
      : 0;
    if (glyph_index > 0)
      break;

    Microsoft::WRL::ComPtr<IDWriteFontFace> dwriteFontFace = nullptr;
    HRESULT hr = dwriteFont->CreateFontFace(&dwriteFontFace);
    if (FAILED(hr))
      break;
    if (nullptr == dwriteFontFace || nullptr == dwriteFontFace.Get())
      break;

    const UINT32 codepoints[] = {glyph->CodePoint(), 0 };
    const UINT32 codepointCount = 1;
    UINT16 glyphIndices[sizeof(codepoints) / sizeof(codepoints[0])] = {};
    hr = dwriteFontFace.Get()->GetGlyphIndices(codepoints, codepointCount, glyphIndices);
    if (FAILED(hr))
      break;

    glyph_index = glyphIndices[0];
    break;
  }

  const bool rc
    = (0 != glyph_index)
    ? MYON_WindowsDWriteGetGlyphMetrics(dwriteFont, glyph_index, glyph_metrics)
    : false;

  dwriteFont->Release();

  return rc ? glyph_index : 0;
}

bool MYON_WindowsDWriteGetGlyphOutline(
  const MYON_FontGlyph* glyph,
  MYON_OutlineFigure::Type figure_type,
  class MYON_Outline& outline
)
{
  outline = MYON_Outline::Unset;

  if (nullptr == glyph)
    return false;

  if (false == glyph->CodePointIsSet())
    return false;

  const unsigned int dwriteGlyphIndex = glyph->FontGlyphIndex();

  const MYON_Font* font = glyph->Font();
  if (nullptr == font)
    return false;

  IDWriteFont* dwriteFont = font->WindowsDWriteFont();
  if (nullptr == dwriteFont)
    return false;

  if (MYON_OutlineFigure::Type::Unset == figure_type)
  {
    MYON_OutlineFigure::Type font_figure_type = font->OutlineFigureType();
    if (MYON_OutlineFigure::Type::Unset != font_figure_type)
    {
      figure_type = font_figure_type;
    }
  }

  const bool rc = MYON_WindowsDWriteGetGlyphOutline(
    dwriteFont,
    dwriteGlyphIndex,
    figure_type,
    outline
  );

  dwriteFont->Release();

  return rc;
}

//////////////////////////////////////////////////////////////////////////
//
// MYON_Font <-> IDWriteFont
//

#define MYON_FONT_MODIFICATION_PERMITTED this->ModificationPermitted(OPENNURBS__FUNCTION__,__FILE__,__LINE__)

bool MYON_Font::SetFromWindowsDWriteFont(
  IDWriteFont* dwrite_font,
  const wchar_t* preferedLocale
)
{
  if (false == MYON_FONT_MODIFICATION_PERMITTED)
    return false;

  *this = MYON_Font::Unset;
  for (;;)
  {
    if (nullptr == dwrite_font)
      break;

    IDWriteGdiInterop* dwriteGdiInterop = MYON_IDWrite::GdiInterop();
    if (nullptr == dwriteGdiInterop)
      break;

    BOOL isSystemFont = false;
    LOGFONTW logfont;
    memset(&logfont, 0, sizeof(logfont));
    HRESULT hr = dwriteGdiInterop->ConvertFontToLOGFONT(dwrite_font, &logfont, &isSystemFont);
    if (FAILED(hr))
      break;
    if (0 == logfont.lfFaceName[0])
      break;
    if (SYMBOL_CHARSET != logfont.lfCharSet && DEFAULT_CHARSET != logfont.lfCharSet )
      logfont.lfCharSet = dwrite_font->IsSymbolFont() ? SYMBOL_CHARSET : DEFAULT_CHARSET;
    logfont.lfHeight = 0;
    if (false == SetFromWindowsLogFont(0, nullptr, logfont))
      break;

    // LOGFONT lfFaceName
    {
      BOOL exists = false;
      Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> gdiLogfontNames = nullptr;
      hr = dwrite_font->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES, &gdiLogfontNames, &exists);
      if (SUCCEEDED(hr) && exists && nullptr != gdiLogfontNames)
      {
        Internal_GetLocalizeStrings(preferedLocale, gdiLogfontNames.Get(), m_loc_windows_logfont_name, m_en_windows_logfont_name);
      }

      for(;;)
      {
        // For simulated fonts, the DWrite GDI family name is sometimes too long for a valid logfont name.
        if (
          m_loc_windows_logfont_name.IsNotEmpty() && m_loc_windows_logfont_name.Length() <= 32
          && m_en_windows_logfont_name.IsNotEmpty() && m_en_windows_logfont_name.Length() <= 32
          )
        {
          break;
        }
        if (0 == logfont.lfFaceName[0])
          break;
        // logfont_name[] = logfont lfFaceName we are cetain is null terminated.
        wchar_t logfont_name[1 + (sizeof(logfont.lfFaceName) / sizeof(logfont.lfFaceName[0]))];
        const size_t logfont_name_capacity = (sizeof(logfont_name)/sizeof(logfont_name[0])) - 1;
        memcpy(logfont_name, logfont.lfFaceName, logfont_name_capacity*sizeof(logfont_name[0]));
        logfont_name[logfont_name_capacity] = 0;
        if (m_en_windows_logfont_name.IsEmpty() || m_en_windows_logfont_name.Length() > 32)
          m_en_windows_logfont_name = logfont_name;
        if (m_loc_windows_logfont_name.IsEmpty() || m_loc_windows_logfont_name.Length() > 32)
          m_loc_windows_logfont_name = logfont_name;
        break;
      }     
    }

    MYON_wString name;
    // PostScript name
    name = MYON_Font::PostScriptNameFromWindowsDWriteFont(dwrite_font, preferedLocale);
    if ( name.IsNotEmpty() )
      m_loc_postscript_name = name;
    name = MYON_Font::PostScriptNameFromWindowsDWriteFont(dwrite_font, L"en-us");
    if ( name.IsNotEmpty() )
      m_en_postscript_name = name;

    // Family name
    for (;;)
    {
      Microsoft::WRL::ComPtr<IDWriteFontFamily> pIDWriteFontFamily = nullptr;
      hr = dwrite_font->GetFontFamily(&pIDWriteFontFamily);
      if ((SUCCEEDED(hr)) && nullptr != pIDWriteFontFamily && nullptr != pIDWriteFontFamily.Get())
      {
        Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> familyNames = nullptr;
        hr = pIDWriteFontFamily->GetFamilyNames(&familyNames);
        if (SUCCEEDED(hr))
        {
          Internal_GetLocalizeStrings(preferedLocale, familyNames.Get(), m_loc_family_name, m_en_family_name);
        }
      }

      if (m_loc_family_name.IsNotEmpty() && m_en_family_name.IsNotEmpty())
        break;

      BOOL exists = false;
      Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> preferedFamilyNames = nullptr;
      hr = dwrite_font->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_PREFERRED_FAMILY_NAMES, &preferedFamilyNames, &exists);
      if (SUCCEEDED(hr) && exists && nullptr != preferedFamilyNames)
      {
        MYON_wString locName;
        MYON_wString enName;
        Internal_GetLocalizeStrings(preferedLocale, preferedFamilyNames.Get(), locName, enName);
        if (m_loc_family_name.IsEmpty())
          m_loc_family_name = locName;
        if ( m_en_family_name.IsEmpty())
          m_en_family_name = enName;
        if (m_loc_family_name.IsNotEmpty() && m_en_family_name.IsNotEmpty())
          break;
      }

      break;
    }

    // Face name
    {
      Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> faceNames = nullptr;
      hr = dwrite_font->GetFaceNames(&faceNames);
      if (SUCCEEDED(hr) && nullptr != faceNames)
      {
        Internal_GetLocalizeStrings(preferedLocale, faceNames.Get(), m_loc_face_name, m_en_face_name);
      }
    }

    // Opennurbs searches the description saved in field 10 of the name table
    // for the strings "Engraving - single stroke" / "Engraving - double stroke" / "Engraving"
    // to identify fonts that are desgned for engraving (and which tend to render poorly when
    // used to dispaly text devices like screens, monitors, and printers).
    // The SLF (single line fonts) are examples of fonts that have Engraving in field 10.
    MYON_wString loc_field_10_description = MYON_Font::Field_10_DescriptionFromWindowsDWriteFont(dwrite_font, preferedLocale);
    MYON_wString en_field_10_description = MYON_Font::Field_10_DescriptionFromWindowsDWriteFont(dwrite_font, L"en-us");
    if (MYON_OutlineFigure::Type::Unset == m_outline_figure_type)
      m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromField10Description(loc_field_10_description);
    if (MYON_OutlineFigure::Type::Unset == m_outline_figure_type)
      m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromField10Description(en_field_10_description);

    /*
    Look for known engraving fonts by name
    */
    if (MYON_OutlineFigure::Type::Unset == m_outline_figure_type)
      m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(m_loc_postscript_name);
    if (MYON_OutlineFigure::Type::Unset == m_outline_figure_type
      && false == m_loc_postscript_name.EqualOrdinal(m_en_postscript_name, true)
      )
      m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(m_en_postscript_name);

    if (MYON_OutlineFigure::Type::Unset == m_outline_figure_type)
      m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(m_loc_family_name);
    if (MYON_OutlineFigure::Type::Unset == m_outline_figure_type
      && false == m_loc_family_name.EqualOrdinal(m_en_family_name, true)
      )
      m_outline_figure_type = MYON_OutlineFigure::FigureTypeFromFontName(m_en_family_name);

    // If it's still unset, we won't do any better in the future,
    // so set m_outline_figure_type to unknow.
    if (MYON_OutlineFigure::Type::Unset == m_outline_figure_type)
      m_outline_figure_type = MYON_OutlineFigure::Type::Unknown;

    m_font_origin = MYON_Font::Origin::WindowsFont;
    return true;
  }

  return false;
}

IDWriteFont* MYON_Font::WindowsDWriteFont() const
{
  for (;;)
  {
    IDWriteGdiInterop* dwriteGdiInterop = MYON_IDWrite::GdiInterop();
    if (nullptr == dwriteGdiInterop)
      break;

    LOGFONTW logfont = WindowsLogFont(0,nullptr);
    if (0 == logfont.lfFaceName[0])
      break;

    IDWriteFont* dwriteFont = nullptr;
    HRESULT hr = dwriteGdiInterop->CreateFontFromLOGFONT(&logfont, &dwriteFont);
    if (FAILED(hr))
      break;
    if (nullptr == dwriteFont)
      break;

    return dwriteFont;
  }

  return nullptr;
}

//////////////////////////////////////////////////////////////////////////
//
//
//

static bool Internal_GetDWriteFamilyName(
  const IDWriteFont* dwrite_font,
  const MYON_wString preferedLocaleDirty,
  MYON_wString& familyName,
  MYON_wString& locale
)
{
  familyName = MYON_wString::EmptyString;
  locale = MYON_wString::EmptyString;
  if (nullptr == dwrite_font)
    return false;

  // The name id are list in decreasing order of reliability.
  DWRITE_INFORMATIONAL_STRING_ID name_id[] =
  {
    /// <summary>
    /// Family name for the weight-stretch-style model.
    /// </summary>
    DWRITE_INFORMATIONAL_STRING_WEIGHT_STRETCH_STYLE_FAMILY_NAME,

    // Typographic family name preferred by the designer. This enables font designers to group more than four fonts in a single family without losing compatibility with
    // GDI. This name is typically only present if it differs from the GDI-compatible family name.
    DWRITE_INFORMATIONAL_STRING_TYPOGRAPHIC_FAMILY_NAMES,

    // GDI-compatible family name. Because GDI allows a maximum of four fonts per family, fonts in the same family may have different GDI-compatible family names
    // (e.g., "Arial", "Arial Narrow", "Arial Black").
    DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES,
  };

  const size_t name_id_count = sizeof(name_id) / sizeof(name_id[0]);

  MYON_wString preferedLocale(preferedLocaleDirty);
  preferedLocale.TrimLeftAndRight();
  if (preferedLocale.IsEmpty())
    preferedLocale = MYON_wString(L"GetUserDefaultLocaleName");

  for (size_t i = 0; i < name_id_count; ++i)
  {
    // iterate possible family names finding the best one to return.
    BOOL exists = false;
    Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> localizedFamilynames = nullptr;
    HRESULT hr = const_cast<IDWriteFont*>(dwrite_font)->GetInformationalStrings(name_id[i], &localizedFamilynames, &exists);
    if (FAILED(hr))
      break;

    IDWriteLocalizedStrings* familyNames = localizedFamilynames.Get();
    if (nullptr == familyNames)
      continue;

    MYON_wString defaultName;
    MYON_wString defaultLocale;
    MYON_wString enName;
    MYON_wString enLocale;
    if (exists)
    {
      if (Internal_GetLocalizeStrings(
        preferedLocale,
        familyNames,
        defaultName,
        defaultLocale,
        enName,
        enLocale
      ))
      {
        if (defaultName.IsNotEmpty() && defaultLocale.IsNotEmpty())
        {
          familyName = defaultName;
          locale = defaultLocale;
          break;
        }
      }
    }
  }

  return familyName.IsNotEmpty();
}

static IDWriteTextFormat* Internal_TextFormat(const IDWriteFont* dwriteFont)
{
  if (nullptr == dwriteFont)
    return nullptr;

  MYON_wString familyName;
  MYON_wString locale;
  const bool bHaveFamilyName = Internal_GetDWriteFamilyName(dwriteFont,MYON_wString::EmptyString,familyName,locale);
  if (false == bHaveFamilyName || familyName.IsEmpty() || locale.IsEmpty())
    return nullptr;

  IDWriteFactory* dwiteFactory = MYON_IDWrite::Factory();
  if (nullptr == dwiteFactory)
    return nullptr;

  DWRITE_FONT_WEIGHT weight = const_cast<IDWriteFont*>(dwriteFont)->GetWeight();
  DWRITE_FONT_STYLE style = const_cast<IDWriteFont*>(dwriteFont)->GetStyle();
  DWRITE_FONT_STRETCH stretch = const_cast<IDWriteFont*>(dwriteFont)->GetStretch();

  float logical_size_dip = 96.0; // 96 DIP ("device-independent pixel") = one inch

  IDWriteTextFormat* dwriteTextFormat = nullptr;

  HRESULT hr = dwiteFactory->CreateTextFormat(
    static_cast<const wchar_t*>(familyName),
    nullptr, // use System font collection
    weight, style, stretch,
    logical_size_dip,
    static_cast<const wchar_t*>(locale),
    &dwriteTextFormat
  );

  if (SUCCEEDED(hr))
    hr = dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

  // Center align (vertically) the text.
  if (SUCCEEDED(hr))
    hr = dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

  if (SUCCEEDED(hr))
    return dwriteTextFormat;

  return nullptr;
}


//static 
IDWriteTextLayout* Internal_TextLayout(
  MYON_wString textString,
  const MYON_Font* font,
  double height
)
{
  if (nullptr == font)
    return nullptr;

  if (textString.IsEmpty())
    return nullptr;

  const UINT32 textStringLength = (UINT32)textString.Length();

  // sanity check textStringLength
  if (textStringLength <= 0 || textStringLength > 1024*1024) 
    return nullptr;

  IDWriteFactory* dwiteFactory = MYON_IDWrite::Factory();
  if (nullptr == dwiteFactory)
    return nullptr;

  HRESULT hr = -1;
  IDWriteTextFormat* dwriteTextFormat = nullptr;
  IDWriteTextLayout* dwriteTextLayout = nullptr;

  for (;;)
  {
    // dwriteFont is managed by opennurbs
    const IDWriteFont* dwriteFont = font->WindowsDWriteFont();
    if (nullptr == dwriteFont)
      break;

    dwriteTextFormat = Internal_TextFormat(dwriteFont);
    if (nullptr == dwriteTextFormat)
      break;

    FLOAT maxWidthInPixels = 16384.0f; // The width of the layout box.
    FLOAT maxHeightInPixels = 128.0f; // The height of the layout box.

    hr = dwiteFactory->CreateTextLayout(
      static_cast<const wchar_t*>(textString),
      textStringLength,
      dwriteTextFormat,
      maxWidthInPixels,
      maxHeightInPixels,
      &dwriteTextLayout
    );
    if (nullptr == dwriteTextLayout)
      break;

    FLOAT fontSizeInDIPs = 96.0; // The font size in DIP units to be set for text in the range specified by textRange.

    const DWRITE_TEXT_RANGE textRange = { 0, textStringLength };
    
    if (SUCCEEDED(hr))
      hr = dwriteTextLayout->SetFontSize(fontSizeInDIPs, textRange);

    if (SUCCEEDED(hr) && font->IsUnderlined())
      hr = dwriteTextLayout->SetUnderline(TRUE, textRange);

    if (SUCCEEDED(hr) && font->IsStrikethrough())
      hr = dwriteTextLayout->SetStrikethrough(TRUE, textRange);

    break;
  }

  if (SUCCEEDED(hr))
    return dwriteTextLayout;

  // delete stuff here

  return nullptr;
}

#endif

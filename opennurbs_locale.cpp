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


static MYON_CRT_locale_t MYON_CRT_C_locale()
{
  static MYON_CRT_locale_t MYON_C_locale = 0;

  if (0 == MYON_C_locale)
  {
#if defined(MYON_RUNTIME_WIN)
    MYON_C_locale = _create_locale(LC_ALL, "C");
#elif defined(MYON_RUNTIME_APPLE)
    MYON_C_locale = _c_locale;
#elif defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
    MYON_C_locale = 0;
#else
    MYON_C_locale = _create_locale(category, locale);
#endif
  }
  
  return MYON_C_locale;
}

static MYON_CRT_locale_t MYON_CRT_create_locale_ALL( const char * locale )
{
  if ( nullptr == locale || 0 == locale[0] )
    return MYON_CRT_C_locale();

  if (
    ('C' == locale[0] || 'c' == locale[0])
    && 0 == locale[1]
    )
    return MYON_CRT_C_locale();

  if (
    ('P' == locale[0] || 'p' == locale[0])
    && ('O' == locale[1] || 'o' == locale[1])
    && ('S' == locale[2] || 's' == locale[2])
    && ('I' == locale[3] || 'i' == locale[3])
    && ('X' == locale[4] || 'x' == locale[4])
    && 0 == locale[5]
    )
    return MYON_CRT_C_locale();  
  
#if defined(MYON_RUNTIME_WIN)
  return _create_locale(LC_ALL, locale);
#elif defined(MYON_RUNTIME_APPLE)
  // Apple locale names are <language>_<REGION> (underbar)
  char language_subtag[32] = { 0 };
  const size_t language_subtag_capacity = sizeof(language_subtag)/sizeof(language_subtag[0]);
  char region_subtag[32] = { 0 };
  const size_t region_subtag_capacity = sizeof(region_subtag)/sizeof(region_subtag[0]);
  if ( false == MYON_Locale::ParseName(
    locale,-1,
    language_subtag, language_subtag_capacity,
    nullptr, 0,
    nullptr, 0,
    region_subtag, region_subtag_capacity,
    nullptr, 0
    ))
  {
    MYON_ERROR("locale_name is not valid");
    return MYON_CRT_C_locale();
  }
  char apple_name[sizeof(language_subtag) / sizeof(language_subtag[0]) + sizeof(region_subtag) / sizeof(region_subtag[0]) + 2] = { 0 };
  for (size_t i = 0; i < language_subtag_capacity; i++)
  {
    if (0 == (apple_name[i] = language_subtag[i]))
    {
      if (0 != region_subtag[0])
      {
        apple_name[i++] = '_'; // Apple needs an underbar here
        for (size_t j = 0; j < region_subtag_capacity; j++, i++)
        {
          if (0 == (apple_name[i] = region_subtag[j]))
            break;
        }
      }
      break;
    }
  }
  return newlocale(LC_ALL_MASK, apple_name, MYON_CRT_C_locale() );
#elif defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  return 0;
#else
  return _create_locale(category, locale);
#endif
}

MYON_Locale::MYON_Locale() MYON_NOEXCEPT
{
  memset(this, 0, sizeof(*this));
  m_numeric_locale = MYON_CRT_C_locale();
  m_string_coll_map_locale = MYON_CRT_C_locale();
}

MYON__UINT32 MYON_Locale::WindowsLCID() const
{
  return m_windows_lcid;
}

static char* LocaleStringBuilder(
  char c,
  const char* source,
  size_t source_capacity,
  char* dest,
  char* dest_end
  )
{
  if ( nullptr == dest || nullptr == dest_end || nullptr == source || source_capacity <= 0 )
    return nullptr;
  if ( 0 != source[source_capacity - 1])
    return nullptr;
  if (dest < dest_end)
  {
    if (0 == source[0])
    {
      *dest = 0;
      return dest;
    }
    if (c > 0 )
      *dest++ = c;
    while (dest < dest_end)
    {
      if (0 == (*dest = *source++))
        return dest;
      dest++;
    }
  }
  return nullptr;
}

#define MYON_LOCALE_SUBTAG(c,t) c,t,sizeof(t)/sizeof(t[0])

static char* LocaleStringBuilderDestEnd(
  char* buffer,
  size_t buffer_capacity
  )
{
  if (buffer_capacity > 0 && nullptr != buffer)
  {
    memset(buffer, 0, buffer_capacity*sizeof(buffer[0]));
    return buffer+buffer_capacity;
  }
  return nullptr;
}

static wchar_t* LocalWideStringBuider(
  const char* cbuffer,
  wchar_t* buffer,
  size_t buffer_capacity
  )
{
  if (buffer_capacity > 0 && nullptr != buffer)
  {
    memset(buffer, 0, buffer_capacity*sizeof(buffer[0]));
    if (nullptr != cbuffer)
    {
      wchar_t* dest = buffer;
      wchar_t* dest_end = dest + buffer_capacity;
      while (dest < dest_end)
      {
        if (0 == (*dest++ = (wchar_t)(*cbuffer++)))
          return buffer;
      }
      memset(buffer, 0, buffer_capacity*sizeof(buffer[0]));
    }
  }
  return nullptr;
}

const char* MYON_Locale::GetBCP47LanguageTag(
  char* buffer,
  size_t buffer_capacity
  ) const
{
  if ( nullptr == buffer || buffer_capacity <= 0 )
    return nullptr;

  char* dest = buffer;
  char* dest_end = LocaleStringBuilderDestEnd(buffer,buffer_capacity);
  const char* source = m_bcp47_language_tag;
  while (dest < dest_end)
  {
    if (0 == (*dest++ = *source++))
      return buffer;
  }
  memset(buffer, 0, buffer_capacity*sizeof(buffer[0]));
  return nullptr;
}

const wchar_t* MYON_Locale::GetBCP47LanguageTag(
  wchar_t* buffer,
  size_t buffer_capacity
  ) const
{
  char cbuffer[MYON_Locale::BUFFER_MAXIMUM_CAPACITY];
  return (LocalWideStringBuider(
    GetBCP47LanguageTag(cbuffer,sizeof(cbuffer)/sizeof(cbuffer[0])),
    buffer,
    buffer_capacity
    ));
}


const char* MYON_Locale::BCP47LanguageTag() const
{
  return m_bcp47_language_tag;
}

const char* MYON_Locale::GetWindowsLocaleName(
  char* buffer,
  size_t buffer_capacity
  ) const
{
  char* buffer_end = LocaleStringBuilderDestEnd(buffer,buffer_capacity);
  char* s = buffer;
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG(0,m_language_subtag),s,buffer_end);
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('-',m_script_subtag),s,buffer_end);
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('-',m_region_subtag),s,buffer_end);
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('_',m_windows_sortorder),s,buffer_end);
  return ( nullptr == s ) ? nullptr : buffer;
}

const wchar_t* MYON_Locale::GetWindowsLocaleName(
  wchar_t* buffer,
  size_t buffer_capacity
  ) const
{
  char cbuffer[MYON_Locale::BUFFER_MAXIMUM_CAPACITY];
  return (LocalWideStringBuider(
    GetWindowsLocaleName(cbuffer,sizeof(cbuffer)/sizeof(cbuffer[0])),
    buffer,
    buffer_capacity
    ));
}


const char* MYON_Locale::GetAppleLocaleName(
  char* buffer,
  size_t buffer_capacity
  ) const
{
  char* buffer_end = LocaleStringBuilderDestEnd(buffer,buffer_capacity);
  char* s = buffer;
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG(0,m_language_subtag),s,buffer_end);
  // Apple local names omit script


  // Apple local names use an underbar before <REGION>
  // s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('-',m_script_subtag),s,buffer_end);
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('_',m_region_subtag),s,buffer_end);
  return ( nullptr == s ) ? nullptr : buffer;
}

const wchar_t* MYON_Locale::GetAppleLocaleName(
  wchar_t* buffer,
  size_t buffer_capacity
  ) const
{
  char cbuffer[MYON_Locale::BUFFER_MAXIMUM_CAPACITY];
  return (LocalWideStringBuider(
    GetAppleLocaleName(cbuffer,sizeof(cbuffer)/sizeof(cbuffer[0])),
    buffer,
    buffer_capacity
    ));
}

const char* MYON_Locale::GetAppleLanguageName(
  char* buffer,
  size_t buffer_capacity
  ) const
{
  char* buffer_end = LocaleStringBuilderDestEnd(buffer,buffer_capacity);
  char* s = buffer;
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG(0,m_language_subtag),s,buffer_end);
  if ( MYON_String::EqualOrdinal("zh", 3, buffer, 3, true) || 0 != m_region_subtag[0] )
  {
    // Apple "language" names use "zh-Hans" for "zh-CN" and "zh-Hant" for "zh-TW"
    if ( 0 == m_script_subtag[0] )
    {
      const char* script_subtag = nullptr;
      if ( MYON_String::EqualOrdinal("CN", -1, m_region_subtag, -1, true))
        script_subtag = "Hans";
      else if (MYON_String::EqualOrdinal("TW", -1, m_region_subtag, -1, true))
        script_subtag = "Hant";
      if (0 != script_subtag)
      {
        s = LocaleStringBuilder('-', script_subtag, 5, s, buffer_end);
        return (nullptr == s) ? nullptr : buffer;
      }
    }
    s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('-', m_script_subtag), s, buffer_end);
  }
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('-', m_region_subtag), s, buffer_end);
  return ( nullptr == s ) ? nullptr : buffer;
}

const wchar_t* MYON_Locale::GetAppleLanguageName(
  wchar_t* buffer,
  size_t buffer_capacity
  ) const
{
  char cbuffer[MYON_Locale::BUFFER_MAXIMUM_CAPACITY];
  return (LocalWideStringBuider(
    GetAppleLanguageName(cbuffer,sizeof(cbuffer)/sizeof(cbuffer[0])),
    buffer,
    buffer_capacity
    ));
}


const char* MYON_Locale::LanguageCode() const
{
  return m_language_subtag;
}

const char* MYON_Locale::ScriptCode() const
{
  return m_script_subtag;
}

const char* MYON_Locale::RegionCode() const
{
  return m_region_subtag;
}

const char* MYON_Locale::WindowsSortOrder() const
{
  return m_windows_sortorder;
}


static bool ZeroWideBuffer(
  wchar_t*& buffer,
  size_t& buffer_capacity,
  size_t sizeof_buffer_element
  )
{
  bool rc 
    = (nullptr != buffer && buffer_capacity > 0 && sizeof_buffer_element > 0 )
    || 0 == buffer_capacity;
  if (nullptr == buffer || buffer_capacity <= 0 || sizeof_buffer_element <= 0 )
  {
    buffer = nullptr;
    buffer_capacity = 0;
  }
  else
  {
    memset(buffer,0,buffer_capacity*sizeof_buffer_element);
  }
  return rc;
}

static bool ZeroCharBuffer(
  char*& buffer,
  size_t& buffer_capacity,
  size_t sizeof_buffer_element
  )
{
  bool rc 
    = (nullptr != buffer && buffer_capacity > 0 && sizeof_buffer_element > 0 )
    || 0 == buffer_capacity;
  if (nullptr == buffer || buffer_capacity <= 0 || sizeof_buffer_element <= 0 )
  {
    buffer = nullptr;
    buffer_capacity = 0;
  }
  else
  {
    memset(buffer,0,buffer_capacity*sizeof_buffer_element);
  }
  return rc;
}

static bool IsAlpha(char c)
{
  return (c >= 'A' && c <= 'Z' ) || (c >= 'a' && c <= 'z' );
}

static bool IsDigit(char c)
{
  return (c >= '0' && c <= '1' );
}

static bool IsAlphaOrDigit(char c)
{
  return IsAlpha(c) || IsDigit(c);
}

static bool IsHyphen(char c)
{
  return ('-' == c);
}

static bool IsUnderbar(char c)
{
  return ('_' == c);
}

static char ToUpper(char c)
{
  return (c >= 'a' && c <= 'z') ? (c - ('a' - 'A')) : c;
}

static char ToLower(char c)
{
  return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

static bool IsHyphenOrUnderbar(char c)
{
  return IsHyphen(c) || IsUnderbar(c);
}

#if defined(MYON_RUNTIME_WIN)
static int ASCIIWideToChar(
  const wchar_t* wASCII,
  size_t wASCII_length,
  char* sASCII,
  size_t sASCII_capacity
  )
{
  for (;;)
  {
    if (nullptr == wASCII || wASCII_length <= 0)
      break;
    if (nullptr == sASCII || sASCII_capacity <= 0)
      break;
    if (sASCII_capacity < wASCII_length)
      break;

    size_t i;
    for (i = 0; i < wASCII_length; i++)
    {
      const wchar_t c = wASCII[i];
      if (c < 0 || c > 127)
        break;
      if (0 == c)
      {
        sASCII[i++] = 0;
        wASCII_length = i;
        break;
      }
      sASCII[i] = (char)c;
    }
    if ( i < wASCII_length  )
      break;

    for(/*empty init*/;i < sASCII_capacity; i++)
      sASCII[i] = 0;

    if ( 0 != sASCII[sASCII_capacity-1])
      break;
    return true;
  }

  if ( nullptr != sASCII && sASCII_capacity > 0)
    memset(sASCII,0,sASCII_capacity*sizeof(sASCII[0]));

  return false;
}
#endif

static bool ASCIICharToWide(
  const char* sASCII,
  size_t sASCII_length,
  wchar_t* wASCII,
  size_t wASCII_capacity
  )
{
  for (;;)
  {
    if (nullptr == sASCII || sASCII_length <= 0)
      break;
    if (nullptr == wASCII || wASCII_capacity <= 0)
      break;
    if (wASCII_capacity < sASCII_length)
      break;

    size_t i;
    for (i = 0; i < sASCII_length; i++)
    {
      const char c = sASCII[i];
      if (c < 0 || c > 127)
        break;
      if (0 == c)
      {
        wASCII[i++] = 0;
        sASCII_length = i;
        break;
      }

      wASCII[i] = (wchar_t)c;
    }
    if ( i < sASCII_length )
      break;

    for(/*empty init*/;i < wASCII_capacity; i++)
      wASCII[i] = 0;

    if ( 0 != wASCII[wASCII_capacity-1])
      break;
    return true;
  }

  if ( nullptr != wASCII && wASCII_capacity > 0)
    memset(wASCII,0,wASCII_capacity*sizeof(wASCII[0]));

  return false;
}

bool MYON_Locale::ParseName(
  const wchar_t* locale_name,
  int locale_name_element_count,
  wchar_t* language_code,
  size_t language_code_capacity,
  wchar_t* extlang_code,
  size_t extlang_code_capacity,
  wchar_t* script_code,
  size_t script_code_capacity,
  wchar_t* region_code,
  size_t region_code_capacity,
  wchar_t* sortorder,
  size_t sortorder_capacity
  )
{
  bool rc = true;

  if (false == ZeroWideBuffer(language_code, language_code_capacity, sizeof(language_code[0])))
    rc = false;
  if (false == ZeroWideBuffer(extlang_code, extlang_code_capacity, sizeof(extlang_code[0])))
    rc = false;
  if (false == ZeroWideBuffer(script_code, script_code_capacity, sizeof(script_code[0])))
    rc = false;
  if (false == ZeroWideBuffer(region_code, region_code_capacity, sizeof(region_code[0])))
    rc = false;
  if (false == ZeroWideBuffer(sortorder, sortorder_capacity, sizeof(sortorder[0])))
    rc = false;

  if (false == rc)
    return false;

  if ( locale_name_element_count < 0 )
    locale_name_element_count = MYON_wString::Length(locale_name);

  if ( 0 == locale_name_element_count )
    return true;

  if ( nullptr == locale_name || 0 == locale_name[0] )
    return false;

  if ( locale_name_element_count < 2 )
    return false;

  MYON_String buffer;
  buffer.ReserveArray(locale_name_element_count);
  buffer.SetLength(locale_name_element_count);
  char* sname = buffer.Array();
  for (int i = 0; i < locale_name_element_count; i++)
  {
    const wchar_t w = locale_name[i];
    if ( w > 127 )
      return false; // illegal symbol in language or locale name
    if (0 == w)
    {
      locale_name_element_count = i;
      break;
    }
    sname[i] = (char)w;
  }
  sname[locale_name_element_count] = 0;
  
  const size_t MYON_S_CAPACITY = 64;
  char slanguage_code[MYON_S_CAPACITY] = { 0 };
  char sextlang_code[MYON_S_CAPACITY] = { 0 };
  char sscript_code[MYON_S_CAPACITY] = { 0 };
  char sregion_code[MYON_S_CAPACITY] = { 0 };
  char ssortorder[MYON_S_CAPACITY] = { 0 };

  rc = MYON_Locale::ParseName(
    sname,
    locale_name_element_count,
    slanguage_code, MYON_S_CAPACITY,
    sextlang_code, MYON_S_CAPACITY,
    sscript_code, MYON_S_CAPACITY,
    sregion_code, MYON_S_CAPACITY,
    ssortorder, MYON_S_CAPACITY
    );

  if (!rc)
    return false;

  if ( !ASCIICharToWide(slanguage_code,MYON_S_CAPACITY,language_code,language_code_capacity) )
    rc = false;
  if ( !ASCIICharToWide(sscript_code,MYON_S_CAPACITY,script_code,script_code_capacity) )
    rc = false;
  if ( !ASCIICharToWide(sregion_code,MYON_S_CAPACITY,region_code,region_code_capacity) )
    rc = false;
  if ( !ASCIICharToWide(ssortorder,MYON_S_CAPACITY,sortorder,sortorder_capacity) )
    rc = false;

  return rc;
}

bool MYON_Locale::ParseName(
  const char* locale_name,
  int locale_name_element_count,
  char* language_code,
  size_t language_code_capacity,
  char* extlang_code,
  size_t extlang_code_capacity,
  char* script_code,
  size_t script_code_capacity,
  char* region_code,
  size_t region_code_capacity,
  char* sortorder,
  size_t sortorder_capacity
  )
{
  bool rc = true;

  if ( false == ZeroCharBuffer(language_code, language_code_capacity, sizeof(language_code[0])) )
    rc = false;
  if ( false == ZeroCharBuffer(extlang_code, extlang_code_capacity, sizeof(extlang_code[0])) )
    rc = false;
  if ( false == ZeroCharBuffer(script_code,   script_code_capacity,   sizeof(script_code[0])) )
    rc = false;
  if ( false == ZeroCharBuffer(region_code,   region_code_capacity,   sizeof(region_code[0])) )
    rc = false;
  if ( false == ZeroCharBuffer(sortorder,     sortorder_capacity,     sizeof(sortorder[0])) )
    rc = false;

  if (!rc)
    return false;

  if ( nullptr == locale_name || 0 == locale_name[0] || 0 == locale_name_element_count )
    return true;

  if ( locale_name_element_count < 0 )
    locale_name_element_count = MYON_String::Length(locale_name);

  if ( locale_name_element_count < 2 )
    return false;

  const char* locale_name_end = locale_name + locale_name_element_count;

  const char* s0 = locale_name;
  const char* s1 = s0;
  while( IsAlpha(*s1) )
    s1++;

  if ( s1-s0 < 2 )
    return false;

  for ( size_t i = 0; i < language_code_capacity && s0 < s1 ; i++)
    language_code[i] = ToLower(*s0++); // lower case for language code is a convention

  if ( 0 == language_code[0] || 0 == language_code[1] )
    return false;

  bool bScriptTest = true;
  bool bExtlangTest = true;
  bool bRegionTest = true;
  bool bSortOrderTest = true;

  for (int pass = 0; pass < 4 && s1 < locale_name_end; pass++)
  {
    char c0 = *s1;

    if (0 == c0)
      return true;

    if (false == IsHyphenOrUnderbar(c0))
      return false;

    s1++;
    s0 = s1;
    while (s1 < locale_name_end && IsAlphaOrDigit(*s1))
      s1++;

    if ( s1-s0 < 2 )
      return false;

    if (bExtlangTest)
    {
      bExtlangTest = false;
      if ( IsHyphen(c0)
        && 3 == s1 - s0
        && IsAlpha(s0[0])
        && IsAlpha(s0[1])
        && IsAlpha(s0[2])
        )
      {
        if ( extlang_code_capacity > 0 && extlang_code_capacity < (size_t)(s1-s0) )
          return false;
        if (extlang_code_capacity > 0)
        {
          for (size_t i = 0; i < extlang_code_capacity && s0 < s1; i++)
            extlang_code[i] = ToLower(*s0++); // lower case for extlang code is a convention
        }
        continue;
      }
    }

    if (bScriptTest)
    {
      bScriptTest = false;
      if ( IsHyphen(c0)
        && 4 == s1 - s0
        && IsAlpha(s0[0])
        && IsAlpha(s0[1])
        && IsAlpha(s0[2])
        && IsAlpha(s0[3])
        )
      {
        // ISO 15924 script code
        if ( script_code_capacity > 0 && script_code_capacity < (size_t)(s1-s0) )
          return false;
        if (script_code_capacity > 0)
        {
          // convention is for script codes is CAPITAL, small, small, small case
          script_code[0] = ToUpper(*s0++);
          for (size_t i = 1; i < script_code_capacity && s0 < s1; i++)
            script_code[i] = ToLower(*s0++);
        }
        continue;
      }
    }

    if (bRegionTest)
    {
      bRegionTest = false;
      // IsUnderbar(c0) is here to handle Apple OS X and iOS "locale id" names like "en_US" which use and underbar before region
      if (
        ( (IsHyphen(c0) || IsUnderbar(c0)) && 2 == s1 - s0 && IsAlpha(s0[0]) && IsAlpha(s0[1]) )  // ISO 3166 country/region identifier (2 alpha)
        || ( IsHyphen(c0) && 3 == s1 - s0 && IsDigit(s0[0]) && IsDigit(s0[1]) && IsDigit(s0[2]) ) // UN M.49 code (3 digits)
        )
      {
        if ( region_code_capacity > 0 && region_code_capacity <= (size_t)(s1-s0) )
          return false;
        for (size_t i = 0; i < region_code_capacity && s0 < s1; i++)
          region_code[i] = ToUpper(*s0++); // unppercase for regions is a convention
        continue;
      }
    }

    if (bSortOrderTest)
    {
      bSortOrderTest = false;
      if (IsUnderbar(c0))
      {
        // Windows sort order
        if (sortorder_capacity > 0 && sortorder_capacity <= (size_t)(s1 - s0))
          return false;
        for (size_t i = 0; i < sortorder_capacity && s0 < s1; i++)
          sortorder[i] = *s0++;
        continue;
      }
    }


    MYON_ERROR("Parser needs to be enhanced or input is not valid");
    return false;
  }

  return true;

}

MYON_Locale MYON_Locale::FromWindowsLCID(
  MYON__UINT32 windows_lcid
  )
{
  if (MYON_Locale::OrdinalLCID == windows_lcid
    || 1 == windows_lcid // "Rhino locale"
    || MYON_Locale::InvariantCultureLCID == windows_lcid
    )
  {
    // This code is called to initialize MYON_Locale::Ordinal and MYON_Locale::InvariantCulture.
    MYON_Locale locale;
    locale.m_windows_lcid = (MYON__UINT32)windows_lcid;
    return locale;
  }
  
#if defined(MYON_RUNTIME_WIN)

  wchar_t wide_locale_name[LOCALE_NAME_MAX_LENGTH + 1] = { 0 };
  const int locale_name_capacity = (int)(sizeof(wide_locale_name) / sizeof(wide_locale_name[0]) - 1);
  int local_name_length = ::LCIDToLocaleName(
    windows_lcid,
    wide_locale_name,
    locale_name_capacity,
    LOCALE_ALLOW_NEUTRAL_NAMES
    );

  if (0 == local_name_length)
  {
    MYON_ERROR("Windows ::LCIDToLocaleName() failed.");
    return MYON_Locale::Ordinal;
  }

  if (local_name_length >= MYON_Locale::BUFFER_MAXIMUM_CAPACITY)
  {
    MYON_ERROR("Windows locale name is too long.");
    return MYON_Locale::Ordinal;
  }

  wide_locale_name[locale_name_capacity] = 0;
  char locale_name[MYON_Locale::BUFFER_MAXIMUM_CAPACITY + 1] = { 0 };
  if (false == ASCIIWideToChar(wide_locale_name, local_name_length, locale_name, sizeof(locale_name) / sizeof(locale_name[0])) )
  {
    MYON_ERROR("Windows locale name contains invalid wchar_t element.");
    return MYON_Locale::Ordinal;
  }
  return MYON_Locale::FromWindowsLCIDAndName(windows_lcid,locale_name);

#else
  switch (windows_lcid)
  {
  case MYON_Locale::WindowsLCID::cs_CZ_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "cs-CZ" );
    break;
  case MYON_Locale::WindowsLCID::de_DE_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "de-DE" );
    break;
  case MYON_Locale::WindowsLCID::en_US_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "en-US" );
    break;
  case MYON_Locale::WindowsLCID::es_ES_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "es-ES" );
    break;
  case MYON_Locale::WindowsLCID::es_ES_tradnl_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "es-ES_tradnl" );
    break;
  case MYON_Locale::WindowsLCID::fr_FR_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "fr-FR" );
    break;
  case MYON_Locale::WindowsLCID::it_IT_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "it-IT" );
    break;
  case MYON_Locale::WindowsLCID::ja_JP_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "ja-JP" );
    break;
  case MYON_Locale::WindowsLCID::ko_KR_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "ko-KR" );
    break;
  case MYON_Locale::WindowsLCID::pl_PL_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "pl-PL" );
    break;
  case MYON_Locale::WindowsLCID::pt_PT_LCID:
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "pt-PT" );
    break;
  case MYON_Locale::WindowsLCID::zh_CN_LCID:
    // "Hans" script is implied by BCP 47 and should not be in the language tag
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "zh-CN" );
    break;
  case MYON_Locale::WindowsLCID::zh_TW_LCID:
    // "Hant" script is implied by BCP 47 and should not be in the language tag
    return MYON_Locale::FromWindowsLCIDAndName(windows_lcid, "zh-TW" );
    break;
  }

  MYON_ERROR("No case for this windows_lcid in the generic runtime code.");
  return MYON_Locale::Ordinal;

#endif
}

MYON_Locale MYON_Locale::FromWindowsLCIDAndName(
  MYON__UINT32 windows_lcid,
  const char* name
  )
{
  if ( windows_lcid == MYON_Locale::OrdinalLCID )
    return MYON_Locale::Ordinal;

  if ( windows_lcid == MYON_Locale::InvariantCultureLCID )
    return MYON_Locale::InvariantCulture;

  if (0 == name || 0 == name[0])
  {
    return MYON_Locale::InvariantCulture;
  }
  
  MYON_Locale locale;

  locale.m_windows_lcid = windows_lcid;

  if (false == MYON_Locale::ParseName(
    name,
    -1,
    locale.m_language_subtag, sizeof(locale.m_language_subtag) / sizeof(locale.m_language_subtag[0]),
    nullptr, 0,
    locale.m_script_subtag, sizeof(locale.m_script_subtag) / sizeof(locale.m_script_subtag[0]),
    locale.m_region_subtag, sizeof(locale.m_region_subtag) / sizeof(locale.m_region_subtag[0]),
    locale.m_windows_sortorder, sizeof(locale.m_windows_sortorder) / sizeof(locale.m_windows_sortorder[0])
    ))
  {
    MYON_ERROR("ParseLocaleName() failed.");
    return MYON_Locale::Ordinal;
  }

  if (0 == locale.m_language_subtag[0])
  {
    MYON_ERROR("ParseLocaleName() returned empty language name.");
    return MYON_Locale::Ordinal;
  }

  if (0 == locale.m_language_subtag[1])
  {
    MYON_ERROR("ParseLocaleName() returned invalid language name.");
    return MYON_Locale::Ordinal;
  }

  char* buffer_end = LocaleStringBuilderDestEnd(locale.m_bcp47_language_tag,sizeof(locale.m_bcp47_language_tag)/sizeof(locale.m_bcp47_language_tag[0]));
  char* s = locale.m_bcp47_language_tag;
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG(0,locale.m_language_subtag),s,buffer_end);
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('-',locale.m_script_subtag),s,buffer_end);
  s = LocaleStringBuilder(MYON_LOCALE_SUBTAG('-',locale.m_region_subtag),s,buffer_end);
  if (nullptr == s)
  {
    MYON_ERROR("Unable to create m_bcp47_language_tag.");
    return MYON_Locale::Ordinal;
  }
  
  locale.m_numeric_locale = MYON_CRT_C_locale();

  locale.m_string_coll_map_locale = MYON_CRT_create_locale_ALL( locale.m_bcp47_language_tag );
  if (0 == locale.m_string_coll_map_locale)
  {
    MYON_ERROR("MYON_CRT_create_locale(LC_ALL, locale.m_bcp47_language_tag) failed.");
    return MYON_Locale::Ordinal;
  }

  return locale;
}

MYON_Locale MYON_Locale::FromWindowsName(
    const char* locale_name
    )
{
  if (0 == locale_name || 0 == locale_name[0])
    return MYON_Locale::InvariantCulture;

  if ('C' == locale_name[0] && 0 == locale_name[1] )
    return MYON_Locale::Ordinal;

#if defined(MYON_RUNTIME_WIN)

  const MYON_wString buffer1(locale_name);
  const wchar_t* wlocale_name = static_cast< const wchar_t* >(buffer1);
  wchar_t buffer2[LOCALE_NAME_MAX_LENGTH] = { 0 };

  if (false == ::IsValidLocaleName(wlocale_name))
  {
    const int buffer2_capacity = (int)(sizeof(buffer2) / sizeof(buffer2[0]));
    if (0 != ::ResolveLocaleName(wlocale_name,buffer2,buffer2_capacity)
        && ::IsValidLocaleName(buffer2)
      )
    {
      wlocale_name = buffer2;
    }
    else
    {
      return MYON_Locale::InvariantCulture;
    }
  }

  LCID windows_lcid = ::LocaleNameToLCID(
    wlocale_name,
    LOCALE_ALLOW_NEUTRAL_NAMES
    );

  if (0 == windows_lcid)
  {
    MYON_ERROR("Windows ::LocaleNameToLCID() failed.");
    return MYON_Locale::Ordinal;
  }

  char slocale_name[MYON_Locale::BUFFER_MAXIMUM_CAPACITY] = { 0 };
  if (false == ASCIIWideToChar(wlocale_name, MYON_wString::Length(wlocale_name), slocale_name, sizeof(slocale_name) / sizeof(slocale_name[0])))
  {
    MYON_ERROR("locale_name contains invalid values.");
    return MYON_Locale::Ordinal;
  }

  return MYON_Locale::FromWindowsLCIDAndName(
    (MYON__UINT32)windows_lcid,
    slocale_name
    );

#else

  char language_subtag[32] = { 0 };
  char script_subtag[32] = { 0 };
  char region_subtag[32] = { 0 };
  char windows_sortorder[32] = { 0 };
  if (false == MYON_Locale::ParseName(
    locale_name,
    -1,
    language_subtag, sizeof(language_subtag) / sizeof(language_subtag[0]),
    nullptr, 0,
    script_subtag, sizeof(script_subtag) / sizeof(script_subtag[0]),
    region_subtag, sizeof(region_subtag) / sizeof(region_subtag[0]),
    windows_sortorder, sizeof(windows_sortorder) / sizeof(windows_sortorder[0])
    ))
  {
    MYON_ERROR("ParseLocaleName() failed.");
    return MYON_Locale::Ordinal;
  }

  if (0 == language_subtag[0])
  {
    MYON_ERROR("ParseLocaleName() returned empty language name.");
    return MYON_Locale::Ordinal;
  }

  if (0 == language_subtag[1])
  {
    MYON_ERROR("ParseLocaleName() returned invalid language name.");
    return MYON_Locale::Ordinal;
  }

  if ( MYON_String::EqualOrdinal("zh", -1, language_subtag, -1, true) )
  {
    // Rhino supports two Chinese locales.
    if (MYON_String::EqualOrdinal("Hans", -1, script_subtag, -1, true))
    {
      // Apple uses "zh-Hans" to mean BCP 47 "zh-CN"
      if ( 0 == region_subtag[0] || MYON_String::EqualOrdinal("CN", -1, region_subtag, -1, true) )
        MYON_Locale::FromWindowsLCIDAndName(MYON_Locale::zh_CN_LCID, "zh-CN" ); 
    }
    else if (MYON_String::EqualOrdinal("Hant", -1, script_subtag, -1, true))
    {
      // Apple uses "zh-Hant" to mean BCP 47 "zh-CN"
      if ( 0 == region_subtag[0] || MYON_String::EqualOrdinal("TW", -1, region_subtag, -1, true) )
        MYON_Locale::FromWindowsLCIDAndName(MYON_Locale::zh_TW_LCID, "zh-TW" ); 
    }
    else if ( MYON_String::EqualOrdinal("CN", -1, region_subtag, -1, true) )
      MYON_Locale::FromWindowsLCIDAndName(MYON_Locale::zh_CN_LCID, "zh-CN" ); 
    else if ( MYON_String::EqualOrdinal("TW", -1, region_subtag, -1, true) )
      MYON_Locale::FromWindowsLCIDAndName(MYON_Locale::zh_TW_LCID, "zh-TW" ); 
    else
      MYON_Locale::FromWindowsLCIDAndName(MYON_Locale::zh_TW_LCID, "zh-TW" ); 
  }
  else
  {
    // Rhino supports a single region and default script for these languages.
    MYON_String cleaned_loc = language_subtag;
    cleaned_loc += '-';
    cleaned_loc +=region_subtag;
    if ( 0 != windows_sortorder[0])
    {
      cleaned_loc += '_';
      cleaned_loc += windows_sortorder;
    }
    
#define LANG_CASE(lcid,lang) if (MYON_String::EqualOrdinal(lang, -1, cleaned_loc, -1, true)) return MYON_Locale::FromWindowsLCIDAndName(lcid, lang )
    LANG_CASE( MYON_Locale::cs_CZ_LCID, "cs-CZ" );
    LANG_CASE( MYON_Locale::de_DE_LCID, "de-DE" );
    LANG_CASE( MYON_Locale::en_US_LCID, "en-US" );
    LANG_CASE( MYON_Locale::es_ES_tradnl_LCID, "es-ES_tradnl" ); // _tradnl is Windows sort order
    LANG_CASE( MYON_Locale::es_ES_LCID, "es-ES" );
    LANG_CASE( MYON_Locale::fr_FR_LCID, "fr-FR" );
    LANG_CASE( MYON_Locale::it_IT_LCID, "it-IT" );
    LANG_CASE( MYON_Locale::ja_JP_LCID, "ja-JP" );
    LANG_CASE( MYON_Locale::ko_KR_LCID, "ko-KR" );
    LANG_CASE( MYON_Locale::pl_PL_LCID, "pl-PL" );
    LANG_CASE( MYON_Locale::pt_PT_LCID, "pt-PT" );
#undef LANG_CASE
    
    // Bail out clause to handle string like fr-US.
#define LANG_CASE_2(two_letter_lang_code,lcid,lang) if (MYON_String::EqualOrdinal(two_letter_lang_code, -1, language_subtag, -1, true)) return MYON_Locale::FromWindowsLCIDAndName(lcid, lang )
    LANG_CASE_2( "cs", MYON_Locale::cs_CZ_LCID, "cs-CZ" );
    LANG_CASE_2( "de", MYON_Locale::de_DE_LCID, "de-DE" );
    LANG_CASE_2( "en", MYON_Locale::en_US_LCID, "en-US" );
    LANG_CASE_2( "es", MYON_Locale::es_ES_LCID, "es-ES" );
    LANG_CASE_2( "fr", MYON_Locale::fr_FR_LCID, "fr-FR" );
    LANG_CASE_2( "it", MYON_Locale::it_IT_LCID, "it-IT" );
    LANG_CASE_2( "ja", MYON_Locale::ja_JP_LCID, "ja-JP" );
    LANG_CASE_2( "ko", MYON_Locale::ko_KR_LCID, "ko-KR" );
    LANG_CASE_2( "pl", MYON_Locale::pl_PL_LCID, "pl-PL" );
    LANG_CASE_2( "pt", MYON_Locale::pt_PT_LCID, "pt-PT" );
#undef LANG_CASE_2
  }

  MYON_ERROR("Unsupported language name.");
  return MYON_Locale::InvariantCulture;

#endif

}

MYON_Locale MYON_Locale::FromWindowsName(
  const wchar_t* locale_name
  )
{
  const MYON_String s_locale_name(locale_name);
  return MYON_Locale::FromWindowsName((const char*)s_locale_name);
}

MYON_Locale MYON_Locale::FromBCP47LanguageName(
  const char* language_name
  )
{
  return MYON_Locale::FromWindowsName(language_name);
}

MYON_Locale MYON_Locale::FromBCP47LanguageName(
  const wchar_t* language_name
  )
{
  return MYON_Locale::FromWindowsName(language_name);
}

MYON_Locale MYON_Locale::FromAppleName(
  const char* apple_name
  )
{
  // Apple "locale" names have underbars before <REGION>
  // Apple "language" names have hyphens before <REGION> or no <REGION> in the case of Chinese

  MYON_String buffer(apple_name);
  buffer.Replace('_', '-'); 
  apple_name = buffer;

  // According to https://en.wikipedia.org/wiki/Chinese_language, Chinese is a family of language
  // varieties, often mutually unintelligible.  Specifying both Script and REGION
  // (zh-Hans-CN or zh-Hant-TW) doesn't narrow things down nearly enough. 
  //
  // Basically we have to hope the string collate and mapping functions supplied by the OS and 
  // the translations supplied by our staff work well for our customers who select from the 
  // two types of "Chinese" Rhino offers.
  //
  if (MYON_String::EqualOrdinal("zh-Hans", -1, apple_name, -1, true) || MYON_String::EqualOrdinal("zh-CN", -1, apple_name, -1, true))
  {
    // Apple uses "zh-Hanx" for the "language" name and "zh_CN" for the "locale" name 
    // when identifying OS X string services that might be useful to people who live
    // in the Peoples Republic of China.
    return MYON_Locale::FromWindowsLCIDAndName(MYON_Locale::WindowsLCID::zh_CN_LCID, "zh-CN");
  }
  
  if (MYON_String::EqualOrdinal("zh-Hant", -1, apple_name, -1, true) || MYON_String::EqualOrdinal("zh-TW", -1, apple_name, -1, true))
  {
    // Apple uses "zh-Hant" for the "language" name and "zh_TW" for the "locale" name 
    // when identifying OS X string services that might be useful to people who live
    // in Tiawan.
    return MYON_Locale::FromWindowsLCIDAndName(MYON_Locale::WindowsLCID::zh_TW_LCID, "zh-TW");
  }

  return MYON_Locale::FromWindowsName( static_cast<const char*>(buffer) );
}

MYON_Locale MYON_Locale::FromAppleName(
  const wchar_t* apple_name
  )
{
  const MYON_String s(apple_name);
  return MYON_Locale::FromAppleName(s);
}

bool MYON_Locale::SetCurrentCulture(
  const MYON_Locale& current_culture_locale
  )
{
  MYON_Locale::m_CurrentCulture = current_culture_locale;

  char buffer[MYON_Locale::BUFFER_MAXIMUM_CAPACITY] = { 0 };
  size_t buffer_capacity = sizeof(buffer) / sizeof(buffer[0]);
  const char* s = nullptr;

  if (false == current_culture_locale.IsOrdinalOrInvariantCulture())
  {
#if defined(MYON_RUNTIME_WIN)
    // Windows sometimes appends _<sortorder>
    s = current_culture_locale.GetWindowsLocaleName(buffer,buffer_capacity);
#elif defined(MYON_RUNTIME_APPLE)
    // Apple puts an _ before <REGION>
    s = current_culture_locale.GetAppleLocaleName(buffer,buffer_capacity);
#else
    // The current "standard" ?
    s = current_culture_locale.GetBCP47LanguageTag(buffer, buffer_capacity);
#endif
  }

  // String collate and mapping functions should use the locale for the name.
  if ( nullptr == s || 0 == s[0] )
  {
    setlocale(LC_ALL,"C");
  }
  else
  {
    setlocale(LC_ALL,s);
  }

  // number parsing and formatting use a decimal point in all cases.
  setlocale(LC_NUMERIC,"C");

  return true;
}



MYON_Locale MYON_Locale::FromSubtags(
  const char* language_code,
  const char* script,
  const char* region_identifier
  )
{
  if (nullptr == language_code || 0 == language_code[0])
    return MYON_Locale::InvariantCulture;

  MYON_String language_name(language_code);
  language_name.MakeLowerOrdinal();

  if ( nullptr != script && (0 != script[0] || (0 != script[1] && 0 != script[2] && 0 != script[3] && 0 == script[4])) )
  {
    char Script[6];
    Script[0] = '-';
    Script[1] = MYON_String::MapCharacterOrdinal(MYON_StringMapOrdinalType::UpperOrdinal,script[0]);
    Script[2] = MYON_String::MapCharacterOrdinal(MYON_StringMapOrdinalType::LowerOrdinal,script[0]);
    Script[3] = MYON_String::MapCharacterOrdinal(MYON_StringMapOrdinalType::LowerOrdinal,script[0]);
    Script[4] = MYON_String::MapCharacterOrdinal(MYON_StringMapOrdinalType::LowerOrdinal,script[0]);
    Script[5] = 0;
    language_name += Script;
  }

  if (nullptr != region_identifier && 0 != region_identifier[0])
  {
    MYON_String REGION = '-';
    REGION += region_identifier;
    REGION.MakeUpperOrdinal();
    language_name += REGION;
  }

  return MYON_Locale::FromBCP47LanguageName(language_name);
}

MYON_Locale MYON_Locale::FromSubtags(
  const wchar_t* language_code,
  const wchar_t* script,
  const wchar_t* region_identifier
  )
{
  const MYON_String s_language_code(language_code);
  const MYON_String s_script(script);
  const MYON_String s_region_identifier(region_identifier);
  return MYON_Locale::FromSubtags(
    static_cast<const char*>(s_language_code),
    static_cast<const char*>(s_script),
    static_cast<const char*>(s_region_identifier)
    );
}

MYON_CRT_locale_t MYON_Locale::NumericLocalePtr() const
{
  return (MYON_CRT_locale_t)m_numeric_locale;
}

MYON_CRT_locale_t MYON_Locale::StringCollateAndMapLocalePtr() const
{
  return (MYON_CRT_locale_t)m_string_coll_map_locale;
}

bool MYON_Locale::IsInvariantCulture() const
{
  return 
    0x0027 == m_windows_lcid 
    && 0 != m_numeric_locale 
    && MYON_CRT_C_locale() == m_numeric_locale 
    && m_numeric_locale == m_string_coll_map_locale;
}

bool MYON_Locale::IsOrdinal() const
{
  return 
    0 == m_windows_lcid 
    && 0 != m_numeric_locale 
    && MYON_CRT_C_locale() == m_numeric_locale 
    && m_numeric_locale == m_string_coll_map_locale;
}

bool MYON_Locale::IsOrdinalOrInvariantCulture() const
{
  return IsOrdinal() || IsInvariantCulture();
}

class MYON_CRT_LOCALE
{
public:
  static bool Validate_sprintf()
  {
    // Test formatted printing
    char buffer[64] = { 0 };
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = sprintf(buffer, m_validation_print_format, m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
  }

  static bool Validate_sprintf_s()
  {
#if defined(MYON_COMPILER_CLANG)  || defined(MYON_COMPILER_GNU)
    // Test formatted printing
    char buffer[64] = { 0 };
    size_t buffer_capacity = (sizeof(buffer) / sizeof(buffer[0])) - 1;
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = snprintf(buffer, buffer_capacity, m_validation_print_format, m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#else
    // Test formatted printing
    char buffer[64] = { 0 };
    size_t buffer_capacity = (sizeof(buffer) / sizeof(buffer[0])) - 1;
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = sprintf_s(buffer, buffer_capacity, m_validation_print_format, m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#endif
  }

  static bool Validate_sprintf_l()
  {
#if defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
    // Test formatted printing
    char buffer[64] = { 0 };
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = sprintf(buffer, m_validation_print_format, m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#else
    // Test formatted printing
    char buffer[64] = { 0 };
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = sprintf_l(buffer, MYON_Locale::InvariantCulture.NumericLocalePtr(), m_validation_print_format, m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#endif
#else
    // Test formatted printing
    char buffer[64] = { 0 };
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = _sprintf_l(buffer, m_validation_print_format, MYON_Locale::InvariantCulture.NumericLocalePtr(), m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#endif
  }

  static bool Validate_sprintf_s_l()
  {
#if defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
    // Test formatted printing
    char buffer[64] = { 0 };
    size_t buffer_capacity = (sizeof(buffer) / sizeof(buffer[0])) - 1;
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = snprintf(buffer, buffer_capacity, m_validation_print_format, m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#else
    // Test formatted printing
    char buffer[64] = { 0 };
    size_t buffer_capacity = (sizeof(buffer) / sizeof(buffer[0])) - 1;
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = snprintf_l(buffer, buffer_capacity, MYON_Locale::InvariantCulture.NumericLocalePtr(), m_validation_print_format, m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#endif
#else
    // Test formatted printing
    char buffer[64] = { 0 };
    size_t buffer_capacity = (sizeof(buffer) / sizeof(buffer[0])) - 1;
    // Testing C-runtime - do not using MYON_String::FormatIntoBuffer
    int printf_rc = _sprintf_s_l(buffer, buffer_capacity, m_validation_print_format, MYON_Locale::InvariantCulture.NumericLocalePtr(), m_validation_value);
    return ValidateString(buffer, sizeof(buffer), printf_rc);
#endif
  }

  static bool Validate_sscanf()
  {
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = sscanf(m_validation_string, m_validation_scan_format, &a);
    return ValidateDouble(a, scanf_rc);
  }

  static bool Validate_sscanf_s()
  {
#if defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = sscanf(m_validation_string, m_validation_scan_format, &a);
    return ValidateDouble(a, scanf_rc);
#else
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = sscanf_s(m_validation_string, m_validation_scan_format, &a);
    return ValidateDouble(a, scanf_rc);
#endif
  }

  static bool Validate_sscanf_l()
  {
#if defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = sscanf(m_validation_string, m_validation_scan_format, &a);
    return ValidateDouble(a, scanf_rc);
#else
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = sscanf_l(m_validation_string, MYON_Locale::InvariantCulture.NumericLocalePtr(), m_validation_scan_format, &a);
    return ValidateDouble(a, scanf_rc);
#endif
#else
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = _sscanf_l(m_validation_string, m_validation_scan_format, MYON_Locale::InvariantCulture.NumericLocalePtr(), &a);
    return ValidateDouble(a, scanf_rc);
#endif
  }

  static bool Validate_sscanf_s_l()
  {
#if defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = sscanf(m_validation_string, m_validation_scan_format, &a);
    return ValidateDouble(a, scanf_rc);
#else
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = sscanf_l(m_validation_string, MYON_Locale::InvariantCulture.NumericLocalePtr(), m_validation_scan_format, &a);
    return ValidateDouble(a, scanf_rc);
#endif
#else
    // Test formatted scanning
    double a = MYON_UNSET_VALUE;
    // Testing C-runtime - do not using MYON_String::Scan
    int scanf_rc = _sscanf_s_l(m_validation_string, m_validation_scan_format, MYON_Locale::InvariantCulture.NumericLocalePtr(), &a);
    return ValidateDouble(a, scanf_rc);
#endif
  }

private:

  static bool ValidateString(
    const char* buffer,
    size_t sizeof_buffer,
    int printf_rc
    )
  {
    bool rc = false;

    if (0 == buffer
      || printf_rc <= 0
      || sizeof_buffer <= 0
      || 0 == m_validation_string
      || m_validation_length <= 0
      || m_validation_length >= sizeof_buffer
      || m_validation_length != (size_t)printf_rc
      )
    {
      rc = (0 == buffer && 0 == sizeof_buffer && 0 == m_validation_string && 0 == m_validation_length && 0 == printf_rc);
    }
    else
    {
      for (size_t i = 0; i < sizeof_buffer; i++)
      {
        if (i > m_validation_length)
          break;
        if (buffer[i] != m_validation_string[i])
          break;
        if (0 == m_validation_string[i])
        {
          rc = (i == m_validation_length);
          break;
        }
      }
    }

    return rc;
  }


  static bool ValidateDouble(
    double a,
    int scan_rc
    )
  {
    bool rc;

    if (1 == scan_rc && a == a && a == m_validation_value)
    {
      rc = true;
    }
    else
    {
      rc = false;
    }

    return rc;
  }

  static const double m_validation_value;
  static const char* m_validation_print_format;
  static const char* m_validation_scan_format;
  static const char* m_validation_string;
  static const size_t m_validation_length;
};

const double MYON_CRT_LOCALE::m_validation_value = 12345678901234.25;
const char* MYON_CRT_LOCALE::m_validation_print_format = "%.17lg";
const char* MYON_CRT_LOCALE::m_validation_scan_format = "%lg";
const char* MYON_CRT_LOCALE::m_validation_string = "12345678901234.25";
const size_t MYON_CRT_LOCALE::m_validation_length = 17;

bool MYON_Locale::PeriodIsCRuntimeDecimalPoint()
{
  // Test formatted printing

  // These tests use the locale MYON_Locale::InvariantCulture.LocalePtr()
  // and should always pass.
  if (!MYON_CRT_LOCALE::Validate_sprintf_l())
    return false;

  if (!MYON_CRT_LOCALE::Validate_sprintf_s_l())
    return false;

  // These tests use the C-runtime locale and will fail if
  // the locale does not use a period as the decimal separator.
  if (!MYON_CRT_LOCALE::Validate_sprintf())
    return false;

  if (!MYON_CRT_LOCALE::Validate_sprintf_s())
    return false;

  // Test formatted scanning

  // These tests use the locale MYON_Locale::InvariantCulture.LocalePtr()
  // and should always pass.
  if (!MYON_CRT_LOCALE::Validate_sscanf_l())
    return false;

  if (!MYON_CRT_LOCALE::Validate_sscanf_s_l())
    return false;

  // These tests use the C-runtime locale and will fail if
  // the locale does not use a period as the decimal separator.
  if (!MYON_CRT_LOCALE::Validate_sscanf())
    return false;

  if (!MYON_CRT_LOCALE::Validate_sscanf_s())
    return false;

  return true;

}

bool MYON_Locale::SetPeriodAsCRuntimeDecimalPoint()
{
  bool rc = MYON_Locale::PeriodIsCRuntimeDecimalPoint();
  if (false == rc)
  {
#if defined(MYON_COMPILER_MSC)
    // Microsoft's C compiler
    const int prev_type = _configthreadlocale(_DISABLE_PER_THREAD_LOCALE);

    const char* s = setlocale(LC_NUMERIC, "C");
    rc = (0 != s && 'C' == s[0] && 0 == s[1]);
    if (rc)
      rc = MYON_Locale::PeriodIsCRuntimeDecimalPoint();

    if (prev_type != _DISABLE_PER_THREAD_LOCALE && prev_type >= 0)
      _configthreadlocale(prev_type);

#elif defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
    // Apple's Clang compiler
    const char* s = setlocale(LC_NUMERIC, "C");
    rc = (0 != s && 'C' == s[0] && 0 == s[1]);
    if (rc)
      rc = MYON_Locale::PeriodIsCRuntimeDecimalPoint();
#else
    // another compiler
    const int prev_type = _configthreadlocale(_DISABLE_PER_THREAD_LOCALE);

    const char* s = setlocale(LC_NUMERIC, "C");
    rc = (0 != s && 'C' == s[0] && 0 == s[1]);
    if (rc)
      rc = MYON_Locale::PeriodIsCRuntimeDecimalPoint();

    if (prev_type != _DISABLE_PER_THREAD_LOCALE && prev_type >= 0)
      _configthreadlocale(prev_type);
#endif
  }
  return rc;
}

unsigned int MYON_Locale::EnforcePeriodAsCRuntimeDecimalPoint()
{
  if (true == MYON_Locale::PeriodIsCRuntimeDecimalPoint())
    return 1; // decimal point = period;

  if (false == MYON_Locale::SetPeriodAsCRuntimeDecimalPoint())
    return 0; // attempt to set decimal point = period failed
  
  if (false == MYON_Locale::PeriodIsCRuntimeDecimalPoint())
    return  0; // decimal point != period

  return 2; // decimal point = period
}

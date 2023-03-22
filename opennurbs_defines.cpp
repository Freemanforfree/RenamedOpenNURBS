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

#include "opennurbs_internal_defines.h"

// {EA2EFFD2-C9A9-4cb1-BE15-D2F46290F1A1}
//const MYON_UUID MYON_MaterialRef::material_from_layer = 
//{ 0xea2effd2, 0xc9a9, 0x4cb1, { 0xbe, 0x15, 0xd2, 0xf4, 0x62, 0x90, 0xf1, 0xa1 } };



// {86EDFDE4-8AAF-4bcd-AB7C-F7111978D7FE}
//const MYON_UUID MYON_MaterialRef::material_from_parent = 
//{ 0x86edfde4, 0x8aaf, 0x4bcd, { 0xab, 0x7c, 0xf7, 0x11, 0x19, 0x78, 0xd7, 0xfe } };


// on_stricmp() is a wrapper for case insensitive string compare
// and calls one of _stricmp(), stricmp(), or strcasecmp()
// depending on OS.
int on_stricmp(const char * s1, const char * s2)
{
#if defined(MYON_RUNTIME_WIN)
  //return stricmp(s1,s2);
  return _stricmp(s1,s2);
#else
  return strcasecmp(s1,s2);
#endif
}

// on_strupr() calls _strupr() or strupr() depending on OS
char* on_strupr(char* s)
{
#if defined(MYON_RUNTIME_WIN)
  return _strupr(s); // ANSI name
#else
  if (s) 
  {
    while (*s) 
    {
      *s = toupper(*s);
      s++;
    }
  }
  return s;
#endif
}

// on_strlwr() calls _strlwr() or strlwr() depending on OS
char* on_strlwr(char* s)
{
#if defined(MYON_RUNTIME_WIN)
  return _strlwr(s); // ANSI name
#else
  if (s) {
    while (*s)
    {
      *s = tolower(*s);
      s++;
    }
  }
  return s;
#endif
}

// on_strrev() calls _strrev() or strrev() depending on OS
char* on_strrev(char* s)
{
#if defined(MYON_RUNTIME_WIN)
  return _strrev(s); // ANSI name
#else
  int i, j;
  char c;
  for ( i = 0, j = ((int)strlen(s))-1; i < j; i++, j-- ) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
  return s;
#endif
}

#if defined(MYON_COMPILER_MSC)
// Disable the MSC /W4 unreachable code warning for the call to on__hack__wcsicmp()
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4702 )
#endif

int on_wcsicmp( const wchar_t* s1, const wchar_t* s2)
{
  // handle nullptr strings consistently and without crashing.
  return MYON_wString::Compare(
    s1,
    s2,
    MYON_Locale::InvariantCulture,
    true
    );
}

#if defined(MYON_COMPILER_MSC)
#pragma MYON_PRAGMA_WARNING_POP
#endif

wchar_t* on_wcsupr(wchar_t* s)
{
  const int length = MYON_wString::Length(s);
  if ( length < 0 )
    return nullptr;
  MYON_wString::MapStringOrdinal(MYON_StringMapOrdinalType::UpperOrdinal,s,length,s,length+1);
  return s;
}

// on_wcslwr() calls _wcslwr() or wcslwr() depending on OS
wchar_t* on_wcslwr(wchar_t* s)
{
  const int length = MYON_wString::Length(s);
  if ( length < 0 )
    return nullptr;
  MYON_wString::MapStringOrdinal(MYON_StringMapOrdinalType::LowerOrdinal,s,length,s,length+1);
  return s;
}


wchar_t* on_wcsrev(wchar_t* s)
{
  return MYON_wString::Reverse(s,-1);
}

int on_WideCharToMultiByte(
    const wchar_t* lpWideCharStr,
    int cchWideChar,
    char* lpMultiByteStr,
    int cchMultiByte
    )
{
  // 14 March 2011 Dale Lear
  //   It turns out that Windows WideCharToMultiByte does correctly
  //   convert UTF-16 to UTF-8 in Windows 7 when the code page 
  //   is CP_ACP and calls with CP_UTF8 sometimes fail to do
  //   any conversion.  So, I wrote MYON_ConvertWideCharToUTF8()
  //   and opennurbs will use MYON_ConvertWideCharToUTF8 to get 
  //   consistent results on all platforms.
  unsigned int error_status = 0;
  unsigned int error_mask = 0xFFFFFFFF;
  MYON__UINT32 error_code_point = 0xFFFD;
  const wchar_t* p1 = 0;
  int count = MYON_ConvertWideCharToUTF8(false,lpWideCharStr,cchWideChar,lpMultiByteStr,cchMultiByte,
                                       &error_status,error_mask,error_code_point,&p1);
  if ( 0 != error_status )
  {
    MYON_ERROR("Error converting UTF-16 encoded wchar_t string to UTF-8 encoded char string.");
  }
  return count;
}

int on_MultiByteToWideChar(
    const char* lpMultiByteStr,
    int cchMultiByte,
    wchar_t* lpWideCharStr,
    int cchWideChar
    )
{
  // 14 March 2011 Dale Lear
  //   It turns out that Windows WideCharToMultiByte does not correctly
  //   convert UTF-8 to UTF-16 in Windows 7 when the code page 
  //   is CP_ACP and calls with CP_UTF8 sometimes fail to do
  //   any conversion.  So, I wrote MYON_ConvertUTF8ToWideChar()
  //   and opennurbs will use MYON_ConvertUTF8ToWideChar to get 
  //   consistent results on all platforms.
  unsigned int error_status = 0;
  unsigned int error_mask = 0xFFFFFFFF;
  MYON__UINT32 error_code_point = 0xFFFD;
  const char* p1 = 0;
  int count = MYON_ConvertUTF8ToWideChar(false,lpMultiByteStr,cchMultiByte,lpWideCharStr,cchWideChar,
                                       &error_status,error_mask,error_code_point,&p1);
  if ( 0 != error_status )
  {
    MYON_ERROR("Error converting UTF-8 encoded char string to UTF-16 encoded wchar_t string.");
  }
  return count;
}

void on_splitpath(
  const char* path,
  const char** volume,
  const char** dir,
  const char** fname,
  const char** ext
  )
{
  // The "const char* path" parameter is a UTF-8 encoded string. 
  // Since the unicode code point values for the characters we 
  // are searching for ( '/' '\' '.' ':' A-Z a-z) are all > 0 
  // and < 128, we can simply check for an array element having
  // the character value and not have to worry about dealing
  // with UTF-8 continuation values (>= 128).

  const char slash1 = '/';
  const char slash2 = '\\'; // do this even with the os is unix because
                            // we might be parsing a file name saved
                            // in Windows.

  const char* f;
  const char* e;
  const char* s;
  const char* s1;

  if ( 0 != volume )
    *volume = 0;
  if ( 0 != dir )
    *dir = 0;
  if ( 0 != fname )
    *fname = 0;
  if ( 0 != ext )
    *ext = 0;

  if ( 0 != path && 0 != *path )
  {
    // deal with Windows' volume letter (even when the os is unix)
    if ( ':' == path[1] )
    {
      if ( (path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z') )
      {
        if ( volume )
          *volume = path;
        path += 2;
        if ( 0 == *path )
          return;
      }
    }
    else if (
      MYON_String::Backslash == path[0] 
      && MYON_String::Backslash == path[1]
      &&( (path[2] >= 'A' && path[2] <= 'Z') 
        || (path[2] >= 'a' && path[2] <= 'z') 
        || (path[2] >= '0' && path[2] <= '9') 
        )
      )
    {
      // deal with Windows' UNC hostnames like \\hostname (even when the os is unix)
      int i = 3;
      while (
        i < 18
        && ((path[i] >= 'A' && path[i] <= 'Z')
        || (path[i] >= 'a' && path[i] <= 'z')
        || (path[i] >= '0' && path[i] <= '9')
        || '-' == path[i] || '_' == path[i]
        ))
      {
        i++;
      }
      if (i < 18 && (MYON_String::Backslash == path[i] || MYON_String::Slash == path[i]))
      {
        if ( volume )
          *volume = path;
        path += i;
      }
    }
  }

  if ( 0 != path && 0 != *path )
  {
    e = 0;
    f = 0;
    s1 = path;
    while ( 0 != *s1 )
      s1++;
    s = (s1 > path) ? s1 - 1 : path;
  
    while ( s > path && '.' != *s && slash1 != *s && slash2 != *s )
      s--;

    if ( '.' == *s && 0 != s[1] )
    {
      // extensions must have something after the dot.
      e = s;
      s1 = e;
      s--;
    }

    while ( s > path && slash1 != *s && slash2 != *s )
      s--;

    if ( s >= path && s < s1 )
    {
      if (slash1 == *s || slash2 == *s ) 
      {
        if ( s+1 < s1 )
          f = s+1;
      }
      else if ( s == path )
      {
        f = s;
      }
    }

    if ( 0 == f )
    {
      // must have a non-empty filename in order to have and "extension"
      f = e;
      e = 0;
    }

    if ( 0 != dir && (0 == f || path < f) )
      *dir = path;

    if ( 0 != f && 0 != fname )
      *fname = f;

    if ( 0 != e && 0 != ext )
      *ext = e;
  }

}

void on_wsplitpath(
  const wchar_t* path,
  const wchar_t** volume,
  const wchar_t** dir,
  const wchar_t** fname,
  const wchar_t** ext
  )
{
  // The "const wchar_t* path" parameter is a UTF-8, UTF-16 or UTF-32
  // encoded string. Since the unicode code point values for the 
  // characters we are searching for ( '/' '\' '.' ':' A-Z a-z) are
  // all > 0 and < 128, we can simply check for an array element 
  // having the character value and not have to worry about dealing
  // with UTF-16 surrogate pair values (0xD800-0xDBFF and DC00-DFFF)
  // and UTF-8 continuation values (>= 128).

  const wchar_t slash1 = '/';
  const wchar_t slash2 = '\\'; // do this even with the os is unix because
                               // we might be parsing a file name saved
                               // in Windows.

  const wchar_t* f;
  const wchar_t* e;
  const wchar_t* s;
  const wchar_t* s1;

  if ( 0 != volume )
    *volume = 0;
  if ( 0 != dir )
    *dir = 0;
  if ( 0 != fname )
    *fname = 0;
  if ( 0 != ext )
    *ext = 0;

  if ( 0 != path && 0 != *path )
  {
    // deal with Windows' volume letter (even when the os is unix)
    if ( ':' == path[1] )
    {
      if ( (path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z') )
      {
        if ( volume )
          *volume = path;
        path += 2;
        if ( 0 == *path )
          return;
      }
    }
    else if (
      MYON_wString::Backslash == path[0] 
      && MYON_wString::Backslash == path[1]
      &&( (path[2] >= 'A' && path[2] <= 'Z') 
        || (path[2] >= 'a' && path[2] <= 'z') 
        || (path[2] >= '0' && path[2] <= '9') 
        )
      )
    {
      // deal with Windows' UNC hostnames like \\hostname (even when the os is unix)
      int i = 3;
      while (
        i < 18
        && ((path[i] >= 'A' && path[i] <= 'Z')
        || (path[i] >= 'a' && path[i] <= 'z')
        || (path[i] >= '0' && path[i] <= '9')
        || '-' == path[i] || '_' == path[i]
        ))
      {
        i++;
      }
      if (i < 18 && (MYON_wString::Backslash == path[i] || MYON_wString::Slash == path[i]))
      {
        if ( volume )
          *volume = path;
        path += i;
      }
    }
  }

  if ( 0 != path && 0 != *path )
  {
    e = 0;
    f = 0;
    s1 = path;
    while ( 0 != *s1 )
      s1++;
    s = (s1 > path) ? s1 - 1 : path;
  
    while ( s > path && '.' != *s && slash1 != *s && slash2 != *s )
      s--;

    if ( '.' == *s && 0 != s[1] )
    {
      // extensions must have something after the dot.
      e = s;
      s1 = e;
      s--;
    }

    while ( s > path && slash1 != *s && slash2 != *s )
      s--;

    if ( s >= path && s < s1 )
    {
      if (slash1 == *s || slash2 == *s ) 
      {
        if ( s+1 < s1 )
          f = s+1;
      }
      else if ( s == path )
      {
        f = s;
      }
    }

    if ( 0 == f )
    {
      // must have a non-empty filename in order to have and "extension"
      f = e;
      e = 0;
    }

    if ( 0 != dir && (0 == f || path < f) )
      *dir = path;

    if ( 0 != f && 0 != fname )
      *fname = f;

    if ( 0 != e && 0 != ext )
      *ext = e;
  }

}


FILE* MYON::OpenFile( // like fopen() - needed when OpenNURBS is used as a DLL
        const char* filename, // file name
        const char* filemode // file mode
        )
{
  return MYON_FileStream::Open(filename,filemode);
}

FILE* MYON::OpenFile( // like fopen() - needed when OpenNURBS is used as a DLL
        const wchar_t* filename, // file name
        const wchar_t* filemode // file mode
        )
{
  return MYON_FileStream::Open(filename,filemode);
}

int MYON::CloseFile( // like fclose() - needed when OpenNURBS is used as a DLL
        FILE* fp // pointer returned by OpenFile()
        )
{
  return MYON_FileStream::Close(fp);
}

int MYON::CloseAllFiles()
{
  // returns number of files closed or EOF for error
#if defined(MYON_COMPILER_MSC)
  return _fcloseall(); // ANSI C name
#elif defined(MYON_RUNTIME_APPLE) || defined(MYON_RUNTIME_ANDROID)
  //fcloseall is not supported on mac/ios or android
  return EOF;
#else
  return fcloseall();
#endif
}



MYON::active_space MYON::ActiveSpace(int i)
{
  MYON::active_space as;

  switch(i)
  {
  case no_space:          as = no_space;          break;
  case model_space:       as = model_space;       break;
  case page_space:        as = page_space;        break;
  case uveditor_space:    as = uveditor_space;    break;
  case blockeditor_space: as = blockeditor_space; break;
  default:                as = no_space;          break;
  }

  return as;
}

MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromUnsigned(
  unsigned int text_display_mode_as_unsigned
  )
{
  switch (text_display_mode_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalToScreen);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kInLine);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalInCplane);
  }
  MYON_ERROR("Invalid text_display_mode_as_unsigned value");
  return (MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine);
}

MYON::RuntimeEnvironment MYON::RuntimeEnvironmentFromUnsigned(
  unsigned int runtime_environment_as_unsigned
  )
{
  switch (runtime_environment_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::RuntimeEnvironment::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::RuntimeEnvironment::None);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::RuntimeEnvironment::Windows);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::RuntimeEnvironment::Apple);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::RuntimeEnvironment::Android);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::RuntimeEnvironment::Linux);
  }
  MYON_ERROR("Invalid runtime_environment_as_unsigned parameter value.");
  return (MYON::RuntimeEnvironment::Unset);
}

MYON::RuntimeEnvironment MYON::CurrentRuntimeEnvironment()
{
#if defined(MYON_RUNTIME_WIN)
  return MYON::RuntimeEnvironment::Windows;
#elif defined (MYON_RUNTIME_APPLE)
  return MYON::RuntimeEnvironment::Apple;
#elif defined (MYON_RUNTIME_ANDROID)
  return MYON::RuntimeEnvironment::Android;
#elif defined (MYON_RUNTIME_LINUX)
  return MYON::RuntimeEnvironment::Linux;
#else
  MYON_ERROR("MYON_RUNTIME_... not defined.");
  return MYON::RuntimeEnvironment::Unset;
#endif
}

MYON::ReadFileResult MYON::ReadFileResultFromUnsigned(
  unsigned int read_file_result_as_unsigned
)
{
  switch (read_file_result_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ReadFileResult::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ReadFileResult::Completed);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ReadFileResult::CompletedWithErrors);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ReadFileResult::Failed);
  }
  MYON_ERROR("Invalid read_file_result_as_unsigned parameter value.");
  return (MYON::ReadFileResult::Unset);
}

bool MYON::ReadFileCompleted(
  MYON::ReadFileResult read_file_result
)
{
  // true indicates partial to complete success.
  return (MYON::ReadFileResult::Unset != read_file_result && MYON::ReadFileResult::Failed != read_file_result);
}

bool MYON::ReadFileFailed(
  MYON::ReadFileResult read_file_result
)
{
  // true indicates total failure.
  return (MYON::ReadFileResult::Failed == read_file_result);
}

bool MYON::IsMetricLengthUnit(
  MYON::LengthUnitSystem length_unit_system
)
{
  bool rc;
  switch (length_unit_system)
  {
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
    rc = true;
    break;
  case MYON::LengthUnitSystem::NauticalMiles:
  case MYON::LengthUnitSystem::AstronomicalUnits:
  case MYON::LengthUnitSystem::LightYears:
  case MYON::LengthUnitSystem::Parsecs:
    rc = true;
  default:
    rc = false;
    break;
  }
  return rc;
}

bool MYON::IsUnitedStatesCustomaryLengthUnit(
  MYON::LengthUnitSystem length_unit_system
)
{
  bool rc;
  switch (length_unit_system)
  {
  case MYON::LengthUnitSystem::Microinches:
  case MYON::LengthUnitSystem::Mils:
  case MYON::LengthUnitSystem::Inches:
  case MYON::LengthUnitSystem::Feet:
  case MYON::LengthUnitSystem::Yards:
  case MYON::LengthUnitSystem::Miles:
  case MYON::LengthUnitSystem::PrinterPoints:
  case MYON::LengthUnitSystem::PrinterPicas:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

bool MYON::IsTerrestrialLengthUnit(
  MYON::LengthUnitSystem length_unit_system
)
{
  bool rc;
  switch (length_unit_system)
  {
  case MYON::LengthUnitSystem::Millimeters:
  case MYON::LengthUnitSystem::Centimeters:
  case MYON::LengthUnitSystem::Decimeters:
  case MYON::LengthUnitSystem::Meters:
  case MYON::LengthUnitSystem::Dekameters:
  case MYON::LengthUnitSystem::Hectometers:
  case MYON::LengthUnitSystem::Kilometers:
    rc = true;
    break;
  case MYON::LengthUnitSystem::Inches:
  case MYON::LengthUnitSystem::Feet:
  case MYON::LengthUnitSystem::Yards:
  case MYON::LengthUnitSystem::Miles:
    rc = true;
    break;
  case MYON::LengthUnitSystem::NauticalMiles:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

bool MYON::IsExtraTerrestrialLengthUnit(
  MYON::LengthUnitSystem length_unit_system
)
{
  bool rc;
  switch (length_unit_system)
  {
  case MYON::LengthUnitSystem::AstronomicalUnits:
  case MYON::LengthUnitSystem::LightYears:
  case MYON::LengthUnitSystem::Parsecs:
    rc = true;
  default:
    rc = false;
    break;
  }
  return rc;
}

bool MYON::IsMicroscopicLengthUnit(
  MYON::LengthUnitSystem length_unit_system
)
{
  bool rc;
  switch (length_unit_system)
  {
  case MYON::LengthUnitSystem::Angstroms:
  case MYON::LengthUnitSystem::Nanometers:
  case MYON::LengthUnitSystem::Microns:
    rc = true;
    break;
  case MYON::LengthUnitSystem::Microinches:
  case MYON::LengthUnitSystem::Mils:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

bool MYON::IsUnitedStatesPrinterLengthUnit(
  MYON::LengthUnitSystem length_unit_system
)
{
  bool rc;
  switch (length_unit_system)
  {
  case MYON::LengthUnitSystem::PrinterPoints:
  case MYON::LengthUnitSystem::PrinterPicas:
    rc = false;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

MYON::LengthUnitSystem MYON::LengthUnitSystemFromUnsigned(unsigned int length_unit_system_as_unsigned)
{
  switch (length_unit_system_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::None);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Angstroms);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Nanometers);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Microns);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Millimeters);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Centimeters);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Decimeters);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Meters);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Dekameters);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Hectometers);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Kilometers);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Megameters);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Gigameters);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Microinches);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Mils);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Inches);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Feet);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Yards);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Miles);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::PrinterPoints);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::PrinterPicas);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::NauticalMiles);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::AstronomicalUnits);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::LightYears);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Parsecs);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::CustomUnits);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LengthUnitSystem::Unset);
  }

  MYON_ERROR("Invalid length_unit_system_as_unsigned value");
  return (MYON::LengthUnitSystem::Unset);
}

static MYON::LengthUnitSystem(*Internal_func_ModelLengthUnitSystemCallback)(MYON__UINT_PTR) = nullptr;

void MYON::RegisterModelLengthUnitSystemCallback(
  MYON::LengthUnitSystem(*func_ModelLengthUnitSystemCallback)(MYON__UINT_PTR)
)
{
  Internal_func_ModelLengthUnitSystemCallback = func_ModelLengthUnitSystemCallback;
}

MYON::LengthUnitSystem MYON::ModelLengthUnitSystem(
  MYON__UINT_PTR model_serial_number
)
{
  return 
    (nullptr == Internal_func_ModelLengthUnitSystemCallback 
      || 0 == model_serial_number 
      || model_serial_number >= MYON_UNSET_UINT_INDEX
      )
    ? MYON::LengthUnitSystem::None
    : Internal_func_ModelLengthUnitSystemCallback(model_serial_number);
}

MYON::AngleUnitSystem MYON::AngleUnitSystemFromUnsigned(unsigned int angle_unit_system_as_unsigned)
{
  switch (angle_unit_system_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::None);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::Turns);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::Radians);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::Degrees);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::Minutes);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::Seconds);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::Gradians);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::AngleUnitSystem::Unset);
  }

  MYON_ERROR("Invalid angle_unit_system_as_unsigned value");
  return (MYON::AngleUnitSystem::Unset);
}


double MYON::AngleUnitScale(
    MYON::AngleUnitSystem us_from,
    MYON::AngleUnitSystem us_to
    )
{
  if (MYON::AngleUnitSystem::Unset == us_from || MYON::AngleUnitSystem::Unset == us_to)
    return MYON_DBL_QNAN;

  // the default cases are here to keep lint quiet
  double scale = 1.0;
  
  if (  us_from != us_to
        && ((int)us_to) > 0 && ((int)us_to) < 6
        // switch weeds out bogus values of us_from
      ) 
  switch( us_from ) 
  {
  case MYON::AngleUnitSystem::Turns:
    switch(us_to)
    {
    case MYON::AngleUnitSystem::Turns:
      scale = 1.0;
      break;
    case MYON::AngleUnitSystem::Radians:
      scale = 2.0*MYON_PI;
      break;
    case MYON::AngleUnitSystem::Degrees:
      scale = 360.0;
      break;
    case MYON::AngleUnitSystem::Minutes:
      scale = 60.0*360.0;
      break;
    case MYON::AngleUnitSystem::Seconds:
      scale = 60.0*60.0*360.0;
      break;
    case MYON::AngleUnitSystem::Gradians:
      scale = 400.0;
      break;
    case MYON::AngleUnitSystem::Unset:
    case MYON::AngleUnitSystem::None:
      MYON_ERROR("unit system conversion undefined");
      break;
    }
    break;

  case MYON::AngleUnitSystem::Radians:
    scale = 1.0;
    switch(us_to)
    {
    case MYON::AngleUnitSystem::Turns:
      scale = 0.5/MYON_PI;
      break;
    case MYON::AngleUnitSystem::Radians:
      scale = 1.0;
      break;
    case MYON::AngleUnitSystem::Degrees:
      scale = 180.0/MYON_PI;
      break;
    case MYON::AngleUnitSystem::Minutes:
      scale = 60.0*180.0/MYON_PI;
      break;
    case MYON::AngleUnitSystem::Seconds:
      scale = 60.0*60.0*180.0/MYON_PI;
      break;
    case MYON::AngleUnitSystem::Gradians:
      scale = 400.0/MYON_PI;
      break;
    case MYON::AngleUnitSystem::Unset:
    case MYON::AngleUnitSystem::None:
      MYON_ERROR("unit system conversion undefined");
      break;
    }
    break;

  case MYON::AngleUnitSystem::Degrees:
    scale = 1.0;
    switch(us_to)
    {
    case MYON::AngleUnitSystem::Turns:
      scale = 1.0/360.0;
      break;
    case MYON::AngleUnitSystem::Radians:
      scale = MYON_PI/180.0;
      break;
    case MYON::AngleUnitSystem::Degrees:
      scale = 1.0;
      break;
    case MYON::AngleUnitSystem::Minutes:
      scale = 60.0;
      break;
    case MYON::AngleUnitSystem::Seconds:
      scale = 60.0*60.0;
      break;
    case MYON::AngleUnitSystem::Gradians:
      scale = 10.0/9.0;
      break;
    case MYON::AngleUnitSystem::Unset:
    case MYON::AngleUnitSystem::None:
      MYON_ERROR("unit system conversion undefined");
      break;
    }
    break;

  case MYON::AngleUnitSystem::Minutes:
    scale = 1.0;
    switch(us_to)
    {
    case MYON::AngleUnitSystem::Turns:
      scale = 1.0/(60.0*360.0);
      break;
    case MYON::AngleUnitSystem::Radians:
      scale = MYON_PI/(60.0*180.0);
      break;
    case MYON::AngleUnitSystem::Degrees:
      scale = 1.0/60.0;
      break;
    case MYON::AngleUnitSystem::Minutes:
      scale = 1.0;
      break;
    case MYON::AngleUnitSystem::Seconds:
      scale = 60.0;
      break;
    case MYON::AngleUnitSystem::Gradians:
      scale = 1.0/54.0;
      break;
    case MYON::AngleUnitSystem::Unset:
    case MYON::AngleUnitSystem::None:
      MYON_ERROR("unit system conversion undefined");
      break;
    }
    break;

  case MYON::AngleUnitSystem::Seconds:
    scale = 1.0;
    switch(us_to)
    {
    case MYON::AngleUnitSystem::Turns:
      scale = 1.0/(60.0*60.0*360.0);
      break;
    case MYON::AngleUnitSystem::Radians:
      scale = MYON_PI/(60.0*60.0*180.0);
      break;
    case MYON::AngleUnitSystem::Degrees:
      scale = 1.0/(60.0*60.0);
      break;
    case MYON::AngleUnitSystem::Minutes:
      scale = 1.0/60.0;
      break;
    case MYON::AngleUnitSystem::Seconds:
      scale = 1.0;
      break;
    case MYON::AngleUnitSystem::Gradians:
      scale = 1.0/(54.0*60.0);
      break;
    case MYON::AngleUnitSystem::Unset:
    case MYON::AngleUnitSystem::None:
      MYON_ERROR("unit system conversion undefined");
      break;
    }
    break;

  case MYON::AngleUnitSystem::Gradians:
    scale = 1.0;
    switch(us_to)
    {
    case MYON::AngleUnitSystem::Turns:
      scale = 400.0;
      break;
    case MYON::AngleUnitSystem::Radians:
      scale = MYON_PI/200.0;
      break;
    case MYON::AngleUnitSystem::Degrees:
      scale = 9.0/10.0;
      break;
    case MYON::AngleUnitSystem::Minutes:
      scale = 54.0;
      break;
    case MYON::AngleUnitSystem::Seconds:
      scale = 60.0*54.0;
      break;
    case MYON::AngleUnitSystem::Gradians:
      scale = 1.0;
      break;
    case MYON::AngleUnitSystem::Unset:
    case MYON::AngleUnitSystem::None:
      MYON_ERROR("unit system conversion undefined");
      break;
    }
    break;

  case MYON::AngleUnitSystem::Unset:
  case MYON::AngleUnitSystem::None:
    MYON_ERROR("unit system conversion undefined");
    break;
  }

  return scale;
}


double MYON::UnitScale(
                     const class MYON_3dmUnitsAndTolerances& u_and_t_from, 
                     const class MYON_3dmUnitsAndTolerances& u_and_t_to
                     )
{
  return MYON::UnitScale( u_and_t_from.m_unit_system, u_and_t_to.m_unit_system );
}

double MYON::UnitScale(
    MYON::LengthUnitSystem us_from,
    const class MYON_UnitSystem& us_to
    )
{
  MYON::LengthUnitSystem us1 = us_to.UnitSystem();
  if (
    MYON::LengthUnitSystem::Unset == us_from 
    || MYON::LengthUnitSystem::Unset == us1
    || us_from != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(us_from))
    || us1 != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(us1))
    )
  {
    MYON_ERROR("Invalid parameters.");
    return MYON_DBL_QNAN;
  }

  if (MYON::LengthUnitSystem::None == us_from || MYON::LengthUnitSystem::None == us1)
    return 1.0;

  if (MYON::LengthUnitSystem::CustomUnits == us_from)
  {
    MYON_ERROR("Use MYON::UnitScale(const MYON_UnitSystem&, const MYON_UnitSystem& ) for custom units.");
    return 1.0;
  }

  if (us_from == us1)
  {
    return 1.0;
  }

  double scale = 1.0;
  if (    MYON::LengthUnitSystem::CustomUnits == us1 
       && MYON::LengthUnitSystem::None != us_from
       && MYON::LengthUnitSystem::CustomUnits != us_from
    )
  {
    const double meters_per_custom_unit = us_to.MetersPerUnit(MYON_DBL_QNAN);
    if ( meters_per_custom_unit > 0.0 && meters_per_custom_unit < MYON_UNSET_POSITIVE_VALUE )
    {
      scale *= meters_per_custom_unit;
      us1 = MYON::LengthUnitSystem::Meters;
    }
  }
  return scale*MYON::UnitScale(us_from,us1);
}

double MYON::UnitScale(
    const class MYON_UnitSystem& us_from, 
    MYON::LengthUnitSystem us_to
    )
{
  MYON::LengthUnitSystem us0 = us_from.UnitSystem();

  if (
    MYON::LengthUnitSystem::Unset == us0
    || MYON::LengthUnitSystem::Unset == us_to
    || us0 != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(us0))
    || us_to != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(us_to))
    )
  {
    MYON_ERROR("Invalid parameters.");
    return MYON_DBL_QNAN;
  }

  if (MYON::LengthUnitSystem::None == us0 || MYON::LengthUnitSystem::None == us_to)
    return 1.0;

  if (MYON::LengthUnitSystem::CustomUnits == us_to)
  {
    MYON_ERROR("Use MYON::UnitScale(const MYON_UnitSystem&, const MYON_UnitSystem& ) for custom units.");
    return 1.0;
  }

  if (us0 == us_to)
    return 1.0;

  double scale = 1.0;
  if (    MYON::LengthUnitSystem::CustomUnits == us0 
       && MYON::LengthUnitSystem::None != us_to
       && MYON::LengthUnitSystem::CustomUnits != us_to
     )
  {
    const double meters_per_custom_unit = us_from.MetersPerUnit(MYON_DBL_QNAN);
    if ( meters_per_custom_unit > 0.0 && meters_per_custom_unit < MYON_UNSET_POSITIVE_VALUE )
    {
      scale /= meters_per_custom_unit;
      us0 = MYON::LengthUnitSystem::Meters;
    }
  }
  return scale*MYON::UnitScale(us0,us_to);
}

double MYON::UnitScale(
  const class MYON_UnitSystem& u_and_t_from, 
  const class MYON_UnitSystem& u_and_t_to
  )
{
  MYON::LengthUnitSystem us_from = u_and_t_from.UnitSystem();
  MYON::LengthUnitSystem us_to   = u_and_t_to.UnitSystem();

  if (
    MYON::LengthUnitSystem::Unset == us_from
    || MYON::LengthUnitSystem::Unset == us_to
    || us_from != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(us_from))
    || us_to != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(us_to))
    )
  {
    MYON_ERROR("Invalid parameters.");
    return MYON_DBL_QNAN;
  }

  if (MYON::LengthUnitSystem::None == us_from || MYON::LengthUnitSystem::None == us_to)
    return 1.0;

  if (MYON::LengthUnitSystem::CustomUnits != us_from && MYON::LengthUnitSystem::CustomUnits != us_to)
    return MYON::UnitScale( us_from, us_to );

  // uncommon custom units case
  const double meters_per_unit_from = u_and_t_from.MetersPerUnit(MYON_DBL_QNAN);
  const double meters_per_unit_to = u_and_t_to.MetersPerUnit(MYON_DBL_QNAN);
  if (meters_per_unit_from == meters_per_unit_to)
    return 1.0;
  double scale = 1.0;
  if ( MYON::LengthUnitSystem::CustomUnits == us_from 
        && meters_per_unit_from > 0.0 
        && meters_per_unit_from < MYON_UNSET_POSITIVE_VALUE
        )
  {
    scale /= meters_per_unit_from;
    us_from = MYON::LengthUnitSystem::Meters;
  }

  if ( MYON::LengthUnitSystem::CustomUnits == us_to 
    && meters_per_unit_to > 0.0 
    && meters_per_unit_to < MYON_UNSET_POSITIVE_VALUE 
    )
  {
    scale *= meters_per_unit_to;
    us_to = MYON::LengthUnitSystem::Meters;
  }

  scale *= MYON::UnitScale( us_from, us_to );

  return scale;
}

static bool IsEnglishUnit( MYON::LengthUnitSystem us )
{
  return (
          MYON::LengthUnitSystem::Microinches == us
          || MYON::LengthUnitSystem::Mils == us
          || MYON::LengthUnitSystem::Inches == us
          || MYON::LengthUnitSystem::Feet == us
          || MYON::LengthUnitSystem::Yards == us
          || MYON::LengthUnitSystem::Miles == us
          || MYON::LengthUnitSystem::PrinterPoints == us
          || MYON::LengthUnitSystem::PrinterPicas == us
         );
}

double MYON::UnitScale(
            MYON::LengthUnitSystem u0, // from
            MYON::LengthUnitSystem u1  // to
            )
{
  // Scale factor for changing unit systems
  // Examples 
  //   100.0  = UnitScale( MYON::LengthUnitSystem::Meters, MYON::LengthUnitSystem::Centimeters ) 
  //     2.54 = UnitScale( MYON::LengthUnitSystem::Inches, MYON::LengthUnitSystem::Centimeters ) 
  //    12.0  = UnitScale( MYON::LengthUnitSystem::Feet, MYON::LengthUnitSystem::Inches ) 

  if (MYON::LengthUnitSystem::Unset == u0 || MYON::LengthUnitSystem::Unset == u1)
  {
    MYON_ERROR("Invalid parameter.");
    return MYON_DBL_QNAN;
  }

  if (
    u0 != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(u0))
    || u1 != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(u1))
    )
  {
    MYON_ERROR("Invalid parameter.");
    return MYON_DBL_QNAN;
  }

  if (MYON::LengthUnitSystem::None == u0 || MYON::LengthUnitSystem::None == u1)
  {
    return 1.0;
  }

  if (MYON::LengthUnitSystem::CustomUnits == u0 || MYON::LengthUnitSystem::CustomUnits == u1)
  {
    MYON_ERROR("Use MYON::UnitScale(const MYON_UnitSystem&, const MYON_UnitSystem& ) for custom unit scale.");
    return 1.0;
  }

  if (u0 == u1)
  {
    return 1.0;
  }

  // the default cases are here to keep lint quiet
  double scale = 1.0;

  switch( u0 ) 
  {
  case MYON::LengthUnitSystem::Angstroms:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1)*1.0e-10;
    break;

  case MYON::LengthUnitSystem::Nanometers:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1)*1.0e-9;
    break;

  case MYON::LengthUnitSystem::Microns:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1)*1.0e-6;
    break;

  case MYON::LengthUnitSystem::Millimeters:
    switch( u1 ) 
    {
    case MYON::LengthUnitSystem::Meters:      scale = 1.0e-3; break;
    case MYON::LengthUnitSystem::Microns:     scale = 1.0e+3; break;
    case MYON::LengthUnitSystem::Centimeters: scale = 1.0e-1; break;

    default:
      scale = IsEnglishUnit(u1)
            ? UnitScale( MYON::LengthUnitSystem::Inches, u1 )/25.4
            : UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1.0e-3;
      break;
    }
    break;

  case MYON::LengthUnitSystem::Centimeters:
    switch( u1 ) 
    {
    case MYON::LengthUnitSystem::Meters:      scale = 1.0e-2; break;
    case MYON::LengthUnitSystem::Millimeters: scale = 1.0e+1; break;

    default:
      scale = IsEnglishUnit(u1)
            ? UnitScale( MYON::LengthUnitSystem::Inches, u1 )/2.54
            : UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1.0e-2;
      break;
    }
    break;

  case MYON::LengthUnitSystem::Decimeters:
    scale = IsEnglishUnit(u1)
          ? UnitScale( MYON::LengthUnitSystem::Inches, u1 )/0.254
          : UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1.0e-1;
    break;

  case MYON::LengthUnitSystem::Meters:
    switch( u1 ) 
    {
    case MYON::LengthUnitSystem::Angstroms:      scale = 1.0e+10; break;
    case MYON::LengthUnitSystem::Nanometers:     scale = 1.0e+9;  break;
    case MYON::LengthUnitSystem::Microns:        scale = 1.0e+6;  break;
    case MYON::LengthUnitSystem::Millimeters:    scale = 1.0e+3;  break;
    case MYON::LengthUnitSystem::Centimeters:    scale = 1.0e+2;  break;
    case MYON::LengthUnitSystem::Decimeters:     scale = 1.0e1;   break;
    case MYON::LengthUnitSystem::Meters:         scale = 1.0;     break;
    case MYON::LengthUnitSystem::Dekameters:     scale = 1.0e-1;  break;
    case MYON::LengthUnitSystem::Hectometers:    scale = 1.0e-2;  break;
    case MYON::LengthUnitSystem::Kilometers:     scale = 1.0e-3;  break;
    case MYON::LengthUnitSystem::Megameters:     scale = 1.0e-6;  break;
    case MYON::LengthUnitSystem::Gigameters:     scale = 1.0e-9;  break;

    case MYON::LengthUnitSystem::NauticalMiles:  scale = 1.0/1852.0; break;
    case MYON::LengthUnitSystem::AstronomicalUnits:   scale = 1.0/1.4959787e+11; break;
    case MYON::LengthUnitSystem::LightYears:     scale = 1.0/9.4607304725808e+15; break;
    case MYON::LengthUnitSystem::Parsecs:        scale = 1.0/3.08567758e+16; break;

    default:
      if ( IsEnglishUnit(u1) )
        scale = UnitScale( MYON::LengthUnitSystem::Inches, u1 )/0.0254;
      break;
    }
    break;

  case MYON::LengthUnitSystem::Dekameters:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*10.0;
    break;

  case MYON::LengthUnitSystem::Hectometers:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*100.0;
    break;

  case MYON::LengthUnitSystem::Kilometers:
    scale = IsEnglishUnit(u1)
          ? UnitScale( MYON::LengthUnitSystem::Inches, u1 )/0.0000254
          : UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1000.0;
    break;

  case MYON::LengthUnitSystem::Megameters:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1.0e6;
    break;

  case MYON::LengthUnitSystem::Gigameters:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1.0e9;
    break;

  case MYON::LengthUnitSystem::Microinches:
    scale = UnitScale( MYON::LengthUnitSystem::Inches, u1 )*1.0e-6;
    break;

  case MYON::LengthUnitSystem::Mils:
    scale = UnitScale( MYON::LengthUnitSystem::Inches, u1 )*1.0e-3;
    break;

  case MYON::LengthUnitSystem::Inches:
    switch( u1 ) 
    {
    case MYON::LengthUnitSystem::Angstroms:       scale = 2.54e+8; break;
    case MYON::LengthUnitSystem::Nanometers:      scale = 2.54e+7; break;
    case MYON::LengthUnitSystem::Microns:         scale = 2.54e+4; break;
    case MYON::LengthUnitSystem::Millimeters:     scale = 25.4; break;
    case MYON::LengthUnitSystem::Centimeters:     scale = 2.54; break;
    case MYON::LengthUnitSystem::Decimeters:      scale = 2.54e-1; break;
    case MYON::LengthUnitSystem::Meters:          scale = 2.54e-2; break;
    case MYON::LengthUnitSystem::Dekameters:      scale = 2.54e-3; break;
    case MYON::LengthUnitSystem::Hectometers:     scale = 2.54e-4; break;
    case MYON::LengthUnitSystem::Kilometers:      scale = 2.54e-5; break;
    case MYON::LengthUnitSystem::Megameters:      scale = 2.54e-8; break;
    case MYON::LengthUnitSystem::Gigameters:      scale = 2.54e-11; break;

    case MYON::LengthUnitSystem::PrinterPoints: scale = 72.0;            break;
    case MYON::LengthUnitSystem::PrinterPicas:  scale = 6.0;             break;
    case MYON::LengthUnitSystem::Microinches: scale = 1.0e+6;            break;
    case MYON::LengthUnitSystem::Mils:        scale = 1.0e+3;            break;
    case MYON::LengthUnitSystem::Inches:      scale = 1.0;               break;
    case MYON::LengthUnitSystem::Feet:        scale = 1.0/12.0;          break;
    case MYON::LengthUnitSystem::Yards:       scale = 1.0/36.0;          break;
    case MYON::LengthUnitSystem::Miles:       scale = 1.0/(12.0*5280.0); break;

    default:
      scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*2.54e-2;
      break;
    }
    break;

  case MYON::LengthUnitSystem::Feet:
    switch( u1 ) 
    {      
    case MYON::LengthUnitSystem::Yards:
      scale = 1.0/3.0;
      break;
    case MYON::LengthUnitSystem::Miles:
      scale = 1.0/5280.0;
      break;
    default:
      scale = UnitScale( MYON::LengthUnitSystem::Inches, u1 )*12.0;
      break;
    }
    break;

  case MYON::LengthUnitSystem::Yards:
    switch( u1 ) 
    {      
    case MYON::LengthUnitSystem::Feet:        scale = 3.0; break;
    case MYON::LengthUnitSystem::Miles:       scale = 1.0/1760.0; break;
    default:
      scale = UnitScale( MYON::LengthUnitSystem::Inches, u1 )*36.0;
      break;
    }
    break;

  case MYON::LengthUnitSystem::Miles:
    if ( MYON::LengthUnitSystem::Feet == u1 )
    {
      scale = 5280.0;
    }
    else
    {
      scale = IsEnglishUnit(u1)
            ? UnitScale( MYON::LengthUnitSystem::Inches, u1 )*12.0*5280.0
            : UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1609.344;
    }
    break;

  case MYON::LengthUnitSystem::PrinterPoints:
    scale = UnitScale( MYON::LengthUnitSystem::Inches, u1 )/72.0;
    break;

  case MYON::LengthUnitSystem::PrinterPicas:
    scale = UnitScale( MYON::LengthUnitSystem::Inches, u1 )/6.0;
    break;

  case MYON::LengthUnitSystem::NauticalMiles:
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1852.0;
    break;

  case MYON::LengthUnitSystem::AstronomicalUnits:
    // 1.4959787e+11  http://en.wikipedia.org/wiki/Astronomical_unit
    // 1.495979e+11   http://units.nist.gov/Pubs/SP811/appenB9.htm  
    //    An astronomical unit (au) is the mean distance from the 
    //    center of the earth to the center of the sun.
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*1.4959787e+11;
    break;

  case MYON::LengthUnitSystem::LightYears:
    // 9.4607304725808e+15 http://en.wikipedia.org/wiki/Light_year
    // 9.46073e+15 meters  http://units.nist.gov/Pubs/SP811/appenB9.htm
    //    A light year is the distance light travels in one Julian year.
    //    The speed of light is exactly 299792458 meters/second.
    //    A Julian year is exactly 365.25 * 86400 seconds and is 
    //    approximately the time it takes for one earth orbit.
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*9.4607304725808e+15;
    break;

  case MYON::LengthUnitSystem::Parsecs:
    // 3.08567758e+16  // http://en.wikipedia.org/wiki/Parsec
    // 3.085678e+16    // http://units.nist.gov/Pubs/SP811/appenB9.htm  
    scale = UnitScale( MYON::LengthUnitSystem::Meters, u1 )*3.08567758e+16;
    break;

  case MYON::LengthUnitSystem::CustomUnits:
    scale = 1.0;
    break;
  case MYON::LengthUnitSystem::None:
    scale = 1.0;
    break;
  case MYON::LengthUnitSystem::Unset:
    scale = MYON_DBL_QNAN;
    break;
  }

  return scale;
}


MYON::OBSOLETE_DistanceDisplayMode MYON::DistanceDisplayModeFromUnsigned(unsigned int distance_display_mode_as_unsigned)
{
  switch (distance_display_mode_as_unsigned) 
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::OBSOLETE_DistanceDisplayMode::Decimal);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::OBSOLETE_DistanceDisplayMode::Fractional);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::OBSOLETE_DistanceDisplayMode::FeetInches);
  }

  MYON_ERROR("Invalid distance_display_mode_as_unsigned value");
  return (MYON::OBSOLETE_DistanceDisplayMode::Decimal);
}

MYON::point_style MYON::PointStyle(int i)
{
  //convertintegertopoint_styleenum
  point_style ps = unknown_point_style;
  switch (i) {
  case not_rational: ps = not_rational; break;
  case homogeneous_rational: ps = homogeneous_rational; break;
  case euclidean_rational: ps = euclidean_rational; break;
  default: ps = unknown_point_style; break;
  }
  return ps;
}


MYON::knot_style MYON::KnotStyle(int i)
{
  //convertintegertoknot_styleenum
  knot_style ks = unknown_knot_style;
  switch (i) {
  case uniform_knots: ks = uniform_knots; break;
  case quasi_uniform_knots: ks = quasi_uniform_knots; break;
  case piecewise_bezier_knots: ks = piecewise_bezier_knots; break;
  case clamped_end_knots: ks = clamped_end_knots; break;
  case non_uniform_knots: ks = non_uniform_knots; break;
  default: ks = unknown_knot_style; break;
  }
  return ks;
}

MYON::continuity MYON::Continuity(int i)
{
  MYON::continuity c = MYON::continuity::unknown_continuity;

  switch(i)
  {
  case (int)MYON::continuity::unknown_continuity: c = MYON::continuity::unknown_continuity; break;
  case (int)MYON::continuity::C0_continuous: c = MYON::continuity::C0_continuous; break;
  case (int)MYON::continuity::C1_continuous: c = MYON::continuity::C1_continuous; break;
  case (int)MYON::continuity::C2_continuous: c = MYON::continuity::C2_continuous; break;
  case (int)MYON::continuity::G1_continuous: c = MYON::continuity::G1_continuous; break;
  case (int)MYON::continuity::G2_continuous: c = MYON::continuity::G2_continuous; break;
  
  // 30 March 2003 Dale Lear added these
  case (int)MYON::continuity::C0_locus_continuous: c = MYON::continuity::C0_locus_continuous; break;
  case (int)MYON::continuity::C1_locus_continuous: c = MYON::continuity::C1_locus_continuous; break;
  case (int)MYON::continuity::C2_locus_continuous: c = MYON::continuity::C2_locus_continuous; break;
  case (int)MYON::continuity::G1_locus_continuous: c = MYON::continuity::G1_locus_continuous; break;
  case (int)MYON::continuity::G2_locus_continuous: c = MYON::continuity::G2_locus_continuous; break;

  case (int)MYON::continuity::Cinfinity_continuous: c = MYON::continuity::Cinfinity_continuous; break;

  case (int)MYON::continuity::Gsmooth_continuous: c = MYON::continuity::Gsmooth_continuous; break;
  };

  return c;
}

MYON::continuity MYON::ParametricContinuity(int i)
{
  // "erase" the locus setting.
  MYON::continuity c = MYON::continuity::unknown_continuity;

  switch(i)
  {
  case (int)MYON::continuity::unknown_continuity: c = MYON::continuity::unknown_continuity; break;
  case (int)MYON::continuity::C0_continuous: c = MYON::continuity::C0_continuous; break;
  case (int)MYON::continuity::C1_continuous: c = MYON::continuity::C1_continuous; break;
  case (int)MYON::continuity::C2_continuous: c = MYON::continuity::C2_continuous; break;
  case (int)MYON::continuity::G1_continuous: c = MYON::continuity::G1_continuous; break;
  case (int)MYON::continuity::G2_continuous: c = MYON::continuity::G2_continuous; break;
  case (int)MYON::continuity::C0_locus_continuous: c = MYON::continuity::C0_continuous; break;
  case (int)MYON::continuity::C1_locus_continuous: c = MYON::continuity::C1_continuous; break;
  case (int)MYON::continuity::C2_locus_continuous: c = MYON::continuity::C2_continuous; break;
  case (int)MYON::continuity::G1_locus_continuous: c = MYON::continuity::G1_continuous; break;
  case (int)MYON::continuity::G2_locus_continuous: c = MYON::continuity::G2_continuous; break;
  case (int)MYON::continuity::Cinfinity_continuous: c = MYON::continuity::Cinfinity_continuous; break;
  case (int)MYON::continuity::Gsmooth_continuous: c = MYON::continuity::Gsmooth_continuous; break;
  };

  return c;
}


MYON::continuity MYON::PolylineContinuity(int i)
{
  MYON::continuity c = MYON::continuity::unknown_continuity;

  switch(i)
  {
  case (int)MYON::continuity::unknown_continuity: c = MYON::continuity::unknown_continuity; break;
  case (int)MYON::continuity::C0_continuous: c = MYON::continuity::C0_continuous; break;
  case (int)MYON::continuity::C1_continuous: c = MYON::continuity::C1_continuous; break;
  case (int)MYON::continuity::C2_continuous: c = MYON::continuity::C1_continuous; break;
  case (int)MYON::continuity::G1_continuous: c = MYON::continuity::G1_continuous; break;
  case (int)MYON::continuity::G2_continuous: c = MYON::continuity::G1_continuous; break;
  case (int)MYON::continuity::C0_locus_continuous: c = MYON::continuity::C0_locus_continuous; break;
  case (int)MYON::continuity::C1_locus_continuous: c = MYON::continuity::C1_locus_continuous; break;
  case (int)MYON::continuity::C2_locus_continuous: c = MYON::continuity::C1_locus_continuous; break;
  case (int)MYON::continuity::G1_locus_continuous: c = MYON::continuity::G1_locus_continuous; break;
  case (int)MYON::continuity::G2_locus_continuous: c = MYON::continuity::G1_locus_continuous; break;
  case (int)MYON::continuity::Cinfinity_continuous: c = MYON::continuity::C1_continuous; break;
  case (int)MYON::continuity::Gsmooth_continuous: c = MYON::continuity::G1_continuous; break;
  };

  return c;
}


MYON::curve_style MYON::CurveStyle(int i)
{
  //convertintegertocurve_styleenum
  curve_style cs = unknown_curve_style;
  switch (i) {
  case line: cs = line; break;
  case circle: cs = circle; break;
  case ellipse: cs = ellipse; break;
  case parabola: cs = parabola; break;
  case hyperbola: cs = hyperbola; break;
  case planar_polyline: cs = planar_polyline; break;
  case polyline: cs = polyline; break;
  case planar_freeform_curve: cs = planar_freeform_curve; break;
  case freeform_curve: cs = freeform_curve; break;
  default: cs = unknown_curve_style; break;
  }
  return cs;
}

MYON::surface_style MYON::SurfaceStyle(int i)
{
  //convertintegertosurface_styleenum
  surface_style ss = unknown_surface_style;
  
  switch (i) {
  case plane: ss = plane; break;
  case circular_cylinder: ss = circular_cylinder; break;
  case elliptical_cylinder: ss = elliptical_cylinder; break;
  case circular_cone: ss = circular_cone; break;
  case elliptical_cone: ss = elliptical_cone; break;
  case sphere: ss = sphere; break;
  case torus: ss = torus; break;
  case surface_of_revolution: ss = surface_of_revolution; break;
  case ruled_surface: ss = ruled_surface; break;
  case freeform_surface: ss = freeform_surface; break;
  default: ss = unknown_surface_style; break;
  }
  return ss;
}

MYON::sort_algorithm MYON::SortAlgorithm(int i)
{
  sort_algorithm sa = MYON::sort_algorithm::quick_sort;
  
  switch (i) {
  case (int)MYON::sort_algorithm::heap_sort: sa = MYON::sort_algorithm::heap_sort; break;
  case (int)MYON::sort_algorithm::quick_sort: sa = MYON::sort_algorithm::quick_sort; break;
  default: sa = MYON::sort_algorithm::quick_sort; break;
  }
  return sa;
}

MYON::endian MYON::Endian(int i)
{ // convert integer to endian enum
  endian e = (i <= 0) ? MYON::endian::little_endian : MYON::endian::big_endian;
  return e;
}

MYON::endian MYON::Endian()
{
  // returns endian-ness of cpu.
  union {
    int i;
    unsigned char b[sizeof(int)];
  } u;
  u.i = 1;
  return (u.b[0] == 1) ? MYON::endian::little_endian : MYON::endian::big_endian;
}

MYON::archive_mode MYON::ArchiveMode(int i)
{
  // convert integer to endian enum
  MYON::archive_mode a = MYON::archive_mode::read;
  switch(i) {
  case (int)MYON::archive_mode::read:      a = MYON::archive_mode::read; break;
  case (int)MYON::archive_mode::write:     a = MYON::archive_mode::write; break;
  case (int)MYON::archive_mode::readwrite: a = MYON::archive_mode::readwrite; break;
  case (int)MYON::archive_mode::read3dm:   a = MYON::archive_mode::read3dm; break;
  case (int)MYON::archive_mode::write3dm:  a = MYON::archive_mode::write3dm; break;
  }
  return a;
}

MYON::view_projection MYON::ViewProjection(int i)
{
  // convert integer to view_projection enum
  view_projection v = MYON::unknown_view;
  switch(i) 
  {
  case MYON::parallel_view:          v = MYON::parallel_view;          break;
  case MYON::perspective_view:       v = MYON::perspective_view;       break;
  }
  return v;
}

bool MYON::IsParallelProjection( MYON::view_projection proj )
{
  return MYON::parallel_view == proj;
}

bool MYON::IsPerspectiveProjection( MYON::view_projection proj )
{
  return ( MYON::perspective_view == proj );
}

MYON::coordinate_system MYON::CoordinateSystem(int i)
{
  // convert integer to coordinate_system enum
  coordinate_system cs = world_cs;
  switch(i) {
  case world_cs:  cs = world_cs; break;
  case camera_cs: cs = camera_cs; break;
  case clip_cs:   cs = clip_cs; break;
  case screen_cs: cs = screen_cs; break;
  }
  return cs;
}

MYON::exception_type MYON::ExceptionType(int i)
{
  // convert integer to exception_type enum
  MYON::exception_type e = unknown_exception;
  switch(i) {
  case out_of_memory:               e = out_of_memory; break;
  case unable_to_write_archive:     e = unable_to_write_archive; break;
  case unable_to_read_archive:      e = unable_to_read_archive; break;
  case unable_to_seek_archive:      e = unable_to_seek_archive; break;
  case unexpected_end_of_archive:   e = unexpected_end_of_archive; break;
  case unexpected_value_in_archive: e = unexpected_value_in_archive; break;
  };
  return e;
}

MYON::layer_mode MYON::LayerMode(int i)
{
  MYON::layer_mode m = normal_layer;
	switch(i)
  {
    case normal_layer:        m = normal_layer;       break;
    case hidden_layer:        m = hidden_layer;       break;
    case locked_layer:        m = locked_layer;       break;
  }
  return m;
}

MYON::object_mode MYON::ObjectMode(int i)
{
  MYON::object_mode m = normal_object;
	switch(i)
  {
    case normal_object:  m = normal_object;  break;
    case hidden_object:  m = hidden_object;  break;
    case locked_object:  m = locked_object;  break;
    case idef_object:    m = idef_object;    break;
  }
  return m;
}

MYON::object_color_source MYON::ObjectColorSource(int i)
{
  // convert integer to object_mode enum
  MYON::object_color_source cs = color_from_layer;
  switch (i) 
  {
  case color_from_layer: // use color assigned to layer
    cs = color_from_layer;
    break;
  case color_from_object: // use color assigned to object
    cs = color_from_object;
    break;
  case color_from_material:  // use diffuse render material color
    cs = color_from_material;
    break;
  case color_from_parent:
    cs = color_from_parent;
    break;
  }
  return cs;
}

MYON::plot_color_source MYON::PlotColorSource(int i)
{
  // convert integer to object_mode enum
  MYON::plot_color_source cs = plot_color_from_layer;
  switch (i) 
  {
  case plot_color_from_layer:
    cs = plot_color_from_layer;
    break;
  case plot_color_from_object:
    cs = plot_color_from_object;
    break;
  case plot_color_from_display: 
    cs = plot_color_from_display;
    break;
  case plot_color_from_parent:
    cs = plot_color_from_parent;
    break;
  }
  return cs;
}

MYON::plot_weight_source MYON::PlotWeightSource(int pw)
{
  switch(pw)
  {
  case plot_weight_from_layer:  return plot_weight_from_layer;  break;
  case plot_weight_from_object: return plot_weight_from_object; break;
  case plot_weight_from_parent: return plot_weight_from_parent; break;
  }
  return plot_weight_from_layer;
}


MYON::object_linetype_source MYON::ObjectLinetypeSource(int i)
{
  // convert integer to object_mode enum
  MYON::object_linetype_source ls = linetype_from_layer;
  switch (i) {
  case linetype_from_layer: // use linetype assigned to layer
    ls = linetype_from_layer;
    break;
  case linetype_from_object: // use linetype assigned to object
    ls = linetype_from_object;
    break;
  case linetype_from_parent:
    ls = linetype_from_parent;
    break;
  }
  return ls;
}

MYON::object_material_source MYON::ObjectMaterialSource(int i)
{
  MYON::object_material_source ms = material_from_layer;
  switch(i) {
  case material_from_layer:
    ms = material_from_layer;
    break;
  case material_from_object:
    ms = material_from_object;
    break;
  case material_from_parent:
    ms = material_from_parent;
    break;
  }
  return ms;
}

MYON::light_style MYON::LightStyle(int i)
{
  // convert integer to light_style enum
  light_style ls = unknown_light_style;
  switch(i)
  {
    case unknown_light_style: ls = unknown_light_style; break;
    //case view_directional_light: ls = view_directional_light; break;
    //case view_point_light: ls = view_point_light; break;
    //case view_spot_light: ls = view_spot_light; break;
    case camera_directional_light: ls = camera_directional_light; break;
    case camera_point_light: ls = camera_point_light; break;
    case camera_spot_light: ls = camera_spot_light; break;
    case world_directional_light: ls = world_directional_light; break;
    case world_point_light: ls = world_point_light; break;
    case world_spot_light: ls = world_spot_light; break;
    case ambient_light: ls = ambient_light; break;
    case world_linear_light: ls = world_linear_light; break;
    case world_rectangular_light: ls = world_rectangular_light; break;
  }
  return ls;
}

MYON::curvature_style MYON::CurvatureStyle(int i)
{
  // convert integer to light_style enum
  MYON::curvature_style cs = unknown_curvature_style;
  switch(i) {
  case gaussian_curvature:
    cs = gaussian_curvature;
    break;
  case mean_curvature:
    cs = mean_curvature;
    break;
  case min_curvature: 
    // minimum unsigned radius of curvature
    cs = min_curvature;
    break;
  case max_curvature: 
    // maximum unsigned radius of curvature
    cs = max_curvature;
    break;
  //case section_curvature_x:
    // unsigned normal curvature with respect to sections cut perp to world x axis
    //cs = section_curvature_x;
    //break;
  //case section_curvature_y:
    // unsigned normal curvature with respect to sections cut perp to world y axis
    //cs = section_curvature_y;
    //break;
  //case section_curvature_z:
    // unsigned normal curvature with respect to sections cut perp to world z axis
    //cs = section_curvature_z;
    //break;
  }
  return cs;
}

/*enum view_type // This is already in the header. I commented it out to see if it would compile and it does. John Croudy.
{
  model_view_type     = 0,
  plot_page_view_type = 1,
  nested_view_type    = 2 
};*/

MYON::view_type MYON::ViewType(int vt)
{
  switch(vt)
  {
  case model_view_type:       return (model_view_type);       break;
  case page_view_type:        return (page_view_type);        break;
  case nested_view_type:      return (nested_view_type);      break;
  case uveditor_view_type:    return (uveditor_view_type);    break;
  case blockeditor_view_type: return (blockeditor_view_type); break;
  }

  return (model_view_type);
}


MYON::v3_display_mode MYON::V3DisplayMode(int i)
{
  // convert integer to light_style enum
  MYON::v3_display_mode dm = MYON::v3_default_display;
  switch(i) {
  case MYON::v3_default_display:
    dm = MYON::v3_default_display;
    break;
  case MYON::v3_wireframe_display:
    dm = MYON::v3_wireframe_display;
    break;
  case MYON::v3_shaded_display:
    dm = MYON::v3_shaded_display;
    break;
  case MYON::v3_renderpreview_display: 
    dm = MYON::v3_renderpreview_display;
    break;
  }
  return dm;
}


MYON::texture_mode MYON::TextureMode(int i)
{
  // convert integer to texture_mode enum
  MYON::texture_mode tm;
  switch (i) {
  case no_texture:
    tm = no_texture;
    break;
  case modulate_texture:
    tm = modulate_texture;
    break;
  case decal_texture:
    tm = decal_texture;
    break;
  default:
    tm = no_texture;
    break;
  }
  return tm;
}

MYON::object_type MYON::ObjectType(int i)
{
  // convert integer to object_type enum
  object_type ot = MYON::object_type::unknown_object_type;
  switch(i) 
  {
  case MYON::object_type::unknown_object_type:  ot = MYON::object_type::unknown_object_type; break;

  case MYON::object_type::point_object:         ot = MYON::object_type::point_object; break;
  case MYON::object_type::pointset_object:      ot = MYON::object_type::pointset_object; break;
  case MYON::object_type::curve_object:         ot = MYON::object_type::curve_object; break;
  case MYON::object_type::surface_object:       ot = MYON::object_type::surface_object; break;
  case MYON::object_type::brep_object:          ot = MYON::object_type::brep_object; break;
  case MYON::object_type::mesh_object:          ot = MYON::object_type::mesh_object; break;
  case MYON::object_type::layer_object:         ot = MYON::object_type::layer_object; break;
  case MYON::object_type::material_object:      ot = MYON::object_type::material_object; break;
  case MYON::object_type::light_object:         ot = MYON::object_type::light_object; break;
  case MYON::object_type::annotation_object:    ot = MYON::object_type::annotation_object; break;
  case MYON::object_type::userdata_object:      ot = MYON::object_type::userdata_object; break;
  case MYON::object_type::instance_definition:  ot = MYON::object_type::instance_definition; break;
  case MYON::object_type::instance_reference:   ot = MYON::object_type::instance_reference; break;
  case MYON::object_type::text_dot:             ot = MYON::object_type::text_dot; break;
  case MYON::object_type::grip_object:          ot = MYON::object_type::grip_object; break;
  case MYON::object_type::detail_object:        ot = MYON::object_type::detail_object; break;
  case MYON::object_type::hatch_object:         ot = MYON::object_type::hatch_object; break;
  case MYON::object_type::morph_control_object: ot = MYON::object_type::morph_control_object; break;
  case MYON::object_type::loop_object:          ot = MYON::object_type::loop_object; break;
  case MYON::object_type::polysrf_filter:       ot = MYON::object_type::polysrf_filter; break;
  case MYON::object_type::edge_filter:          ot = MYON::object_type::edge_filter; break;
  case MYON::object_type::polyedge_filter:      ot = MYON::object_type::polyedge_filter; break;
  case MYON::object_type::meshvertex_filter:    ot = MYON::object_type::meshvertex_filter; break;
  case MYON::object_type::meshedge_filter:      ot = MYON::object_type::meshedge_filter; break;
  case MYON::object_type::meshface_filter:      ot = MYON::object_type::meshface_filter; break;
  case MYON::object_type::cage_object:          ot = MYON::object_type::cage_object; break;
  case MYON::object_type::phantom_object:       ot = MYON::object_type::phantom_object; break;
  case MYON::object_type::extrusion_object:     ot = MYON::object_type::extrusion_object; break;
  case MYON::object_type::meshcomponent_reference: ot = MYON::object_type::meshcomponent_reference; break;

  default: ot = MYON::object_type::unknown_object_type; break;
  }

  return ot;
}

MYON::bitmap_type MYON::BitmapType(int i)
{
  // convert integer to object_type enum
  bitmap_type bt = unknown_bitmap_type;
  switch(i) {
  case unknown_bitmap_type: bt = unknown_bitmap_type; break;
  case windows_bitmap:      bt = windows_bitmap; break;
  case opengl_bitmap:       bt = opengl_bitmap; break;
  case png_bitmap:          bt = png_bitmap; break;
  default: bt = unknown_bitmap_type; break;
  }
  return bt;
}

MYON::object_decoration MYON::ObjectDecoration(int i)
{
  MYON::object_decoration d;
  switch(i)
  {
  case no_object_decoration: d = no_object_decoration; break;
  case start_arrowhead:      d = start_arrowhead; break;
  case end_arrowhead:        d = end_arrowhead;   break;
  case both_arrowhead:       d = both_arrowhead;  break;
  default:                   d = no_object_decoration; break;
  }
  return d;
}


MYON::osnap_mode MYON::OSnapMode(int i)
{
  MYON::osnap_mode osm;
  switch((unsigned int)i)
  {
  case os_none:          osm = os_none; break;
  case os_near:          osm = os_near; break;
  case os_focus:         osm = os_focus; break;
  case os_center:        osm = os_center; break;
  case os_vertex:        osm = os_vertex; break;
  case os_knot:          osm = os_knot; break;
  case os_quadrant:      osm = os_quadrant; break;
  case os_midpoint:      osm = os_midpoint; break;
  case os_intersection:  osm = os_intersection; break;
  case os_end:           osm = os_end; break;
  case os_perpendicular: osm = os_perpendicular; break;
  case os_tangent:       osm = os_tangent; break;
  case os_point:         osm = os_point; break;
  case os_all_snaps:     osm = os_all_snaps; break;
  default:
    osm = os_none;
    break;
  };
  return osm;
}


MYON::cubic_loft_end_condition MYON::CubicLoftEndCondition(int i)
{
  MYON::cubic_loft_end_condition e;
  switch(i)
  {
  case cubic_loft_ec_quadratic:
    e = MYON::cubic_loft_ec_quadratic;
    break;
  case cubic_loft_ec_linear:
    e = MYON::cubic_loft_ec_linear;
    break;
  case cubic_loft_ec_cubic:
    e = MYON::cubic_loft_ec_cubic;
    break;
  case cubic_loft_ec_natural:
    e = MYON::cubic_loft_ec_natural;
    break;
  case cubic_loft_ec_unit_tangent:
    e = MYON::cubic_loft_ec_unit_tangent;
    break;
  case cubic_loft_ec_1st_derivative:
    e = MYON::cubic_loft_ec_1st_derivative;
    break;
  case cubic_loft_ec_2nd_derivative:
    e = MYON::cubic_loft_ec_2nd_derivative;
    break;
  case cubic_loft_ec_free_cv:
    e = MYON::cubic_loft_ec_free_cv;
    break;
  default:
    MYON_ERROR("MYON::CubicLoftEndCondition(i) value of i is not valid.");
    e = MYON::cubic_loft_ec_quadratic;
    break;
  }
  return e;
}


MYON::mesh_type MYON::MeshType(int i)
{
  mesh_type mt = MYON::mesh_type::default_mesh;
  switch(i)
  {
  case (int)MYON::mesh_type::default_mesh:  mt = MYON::mesh_type::default_mesh;  break;
  case (int)MYON::mesh_type::render_mesh:   mt = MYON::mesh_type::render_mesh;   break;
  case (int)MYON::mesh_type::analysis_mesh: mt = MYON::mesh_type::analysis_mesh; break;
  case (int)MYON::mesh_type::preview_mesh:  mt = MYON::mesh_type::preview_mesh; break;
  case (int)MYON::mesh_type::any_mesh:      mt = MYON::mesh_type::any_mesh;      break;
  default:            mt = MYON::mesh_type::default_mesh;  break;
  }
  return mt;
}


MYON_INTERNAL_OBSOLETE::V5_eAnnotationType MYON_INTERNAL_OBSOLETE::V5AnnotationTypeFromUnsigned(unsigned int v5_annotation_type_as_unsigned)
{
  // convert integer to eAnnotationType enum
  switch(v5_annotation_type_as_unsigned) 
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimLinear);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAligned);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimAngular);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimDiameter);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimRadius);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtLeader);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtTextBlock);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimOrdinate);
  }
  MYON_ERROR("Invalid v5_annotation_type_as_unsigned value");
  return (MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing);
}

MYON::ComponentNameConflictResolution MYON::ComponentNameConflictResolutionFromUnsigned(
  unsigned int component_name_conflict_resolution_as_unsigned
)
{
  switch (component_name_conflict_resolution_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ComponentNameConflictResolution::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ComponentNameConflictResolution::QueryMethod);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ComponentNameConflictResolution::UseExistingComponent);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ComponentNameConflictResolution::ReplaceExistingComponent);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ComponentNameConflictResolution::KeepBothComponentsAutomaticRename);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ComponentNameConflictResolution::KeepBothComponentsQueryRename);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ComponentNameConflictResolution::NoConflict);
  }
  MYON_ERROR("Invalid component_name_conflict_resolution_as_unsigned value");
  return (MYON::ComponentNameConflictResolution::Unset);
}


MYON_INTERNAL_OBSOLETE::V5_vertical_alignment MYON_INTERNAL_OBSOLETE::V5VerticalAlignmentFromUnsigned(
  unsigned int vertical_alignment_as_unsigned
  )
{
  switch (vertical_alignment_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Centered);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Above);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Below);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Top);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::FirstLine);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Middle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::LastLine);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Bottom);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Underlined);
  }
  MYON_ERROR("invalid vertical_alignment_as_unsigned parameter.");
  return (MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Centered);
}

MYON_INTERNAL_OBSOLETE::V5_vertical_alignment MYON_INTERNAL_OBSOLETE::V5VerticalAlignmentFromV6VerticalAlignment(
  const MYON::TextVerticalAlignment text_vertical_alignment
)
{
  MYON_INTERNAL_OBSOLETE::V5_vertical_alignment valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Centered;

  switch (text_vertical_alignment)
  {
  case MYON::TextVerticalAlignment::Top:
    valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Top;
    break;
  case MYON::TextVerticalAlignment::MiddleOfTop:
    valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::FirstLine;
    break;
  case MYON::TextVerticalAlignment::BottomOfTop:
    // no exact mapping works - this works if there is one line of text
    valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Bottom;
    break;
  case MYON::TextVerticalAlignment::Middle:
    valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Middle;
    break;
  case MYON::TextVerticalAlignment::MiddleOfBottom:
    valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::LastLine;
    break;
  case MYON::TextVerticalAlignment::Bottom:
    valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Bottom;
    break;
  case MYON::TextVerticalAlignment::BottomOfBoundingBox:
    valign = MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Underlined;
    break;
  }

  return valign;
}

MYON::TextVerticalAlignment MYON_INTERNAL_OBSOLETE::V6VerticalAlignmentFromV5VerticalAlignment(
  MYON_INTERNAL_OBSOLETE::V5_vertical_alignment V5_vertical_alignment
)
{
  MYON::TextVerticalAlignment valign = MYON::TextVerticalAlignment::Middle;

  switch (V5_vertical_alignment)
  {
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Centered:
    valign = MYON::TextVerticalAlignment::Middle;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Above:
    // V5 text "above" dim line means "bottom" of text bbox is at insertion point above dim line
    valign = MYON::TextVerticalAlignment::Bottom;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Below:
    // V5 text "below" dim line means "top" of text bbox is at insertion point below dim line
    valign = MYON::TextVerticalAlignment::Top;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Top:
    valign = MYON::TextVerticalAlignment::Top;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::FirstLine:
    valign = MYON::TextVerticalAlignment::MiddleOfTop;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Middle:
    valign = MYON::TextVerticalAlignment::Middle;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::LastLine:
    valign = MYON::TextVerticalAlignment::MiddleOfBottom;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Bottom:
    valign = MYON::TextVerticalAlignment::Bottom;    
    break;
  case MYON_INTERNAL_OBSOLETE::V5_vertical_alignment::Underlined:
    valign = MYON::TextVerticalAlignment::BottomOfBoundingBox;
    break;
  }

  return valign;
}


MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment MYON_INTERNAL_OBSOLETE::V5HorizontalAlignmentFromUnsigned(
  unsigned int horizontal_alignment_as_unsigned
  )
{
  switch (horizontal_alignment_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Left);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Center);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Right);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Auto);
  }
  MYON_ERROR("invalid horizontal_alignment_as_unsigned parameter.");
  return (MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Left);
}

MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment MYON_INTERNAL_OBSOLETE::V5HorizontalAlignmentFromV6HorizontalAlignment(
  const MYON::TextHorizontalAlignment text_horizontal_alignment
)
{
  MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment halign = MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Left;

  switch (text_horizontal_alignment)
  {
  case MYON::TextHorizontalAlignment::Left:
    halign = MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Left;
    break;
  case MYON::TextHorizontalAlignment::Center:
    halign = MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Center;
    break;
  case MYON::TextHorizontalAlignment::Right:
    halign = MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Right;
    break;
  case MYON::TextHorizontalAlignment::Auto:
    halign = MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Left;
    break;
  }

  return halign;
}

MYON::TextHorizontalAlignment MYON_INTERNAL_OBSOLETE::V6HorizontalAlignmentFromV5HorizontalAlignment(
  MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5_vertical_alignment
)
{
  MYON::TextHorizontalAlignment halign = MYON::TextHorizontalAlignment::Left;

  switch (V5_vertical_alignment)
  {
  case MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Left:
    halign = MYON::TextHorizontalAlignment::Left;
    break;
  case MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Center:
    halign = MYON::TextHorizontalAlignment::Center;
    break;
  case MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Right:
    halign = MYON::TextHorizontalAlignment::Right;
    break;
  case MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment::Auto:
    halign = MYON::TextHorizontalAlignment::Left;
    break;
  }

  return halign;
}

MYON_2dex::MYON_2dex(
  int iValue,
  int jValue)
  : i(iValue)
  , j(jValue)
{}

MYON_2dex MYON_2dex::AsIncreasing() const
{
  MYON_2dex rc;
  if (j < i)
  {
    rc.i = j;
    rc.j = i;
  }
  else
  {
    rc.i = i;
    rc.j = j;
  }
  return rc;
}

MYON_2dex MYON_2dex::AsDecreasing() const
{
  MYON_2dex rc;
  if (i < j)
  {
    rc.i = j;
    rc.j = i;
  }
  else
  {
    rc.i = i;
    rc.j = j;
  }
  return rc;
}

MYON_2udex MYON_2udex::AsIncreasing() const
{
  MYON_2udex rc;
  if (j < i)
  {
    rc.i = j;
    rc.j = i;
  }
  else
  {
    rc.i = i;
    rc.j = j;
  }
  return rc;
}

MYON_2udex MYON_2udex::AsDecreasing() const
{
  MYON_2udex rc;
  if (i < j)
  {
    rc.i = j;
    rc.j = i;
  }
  else
  {
    rc.i = i;
    rc.j = j;
  }
  return rc;
}

bool MYON_2dex::operator==(const MYON_2dex& src) const
{
  return i == src.i && j == src.j;
}

bool MYON_2dex::operator!=(const MYON_2dex& src) const
{
  return i != src.i || j != src.j;
}

bool MYON_2udex::operator==(const MYON_2udex& src) const
{
  return i == src.i && j == src.j;
}

bool MYON_2udex::operator!=(const MYON_2udex& src) const
{
  return i != src.i || j != src.j;
}

bool MYON_2udex::operator<(const MYON_2udex& src) const
{
  return i < src.i || ((j == src.j) && j < src.j);
}

bool MYON_2udex::operator<=(const MYON_2udex& src) const
{
  return i < src.i || ((j == src.j) && j <= src.j);
}

bool MYON_2udex::operator>=(const MYON_2udex& src) const
{
  return i > src.i || ((j == src.j) && j >= src.j);
}

bool MYON_2udex::operator>(const MYON_2udex& src) const
{
  return i > src.i || ((j == src.j) && j > src.j);
}

bool MYON_4dex::operator==(const MYON_4dex& src) const
{
  return i == src.i && j == src.j && k == src.k && l == src.l;
}

bool MYON_4dex::operator!=(const MYON_4dex& src) const
{
  return i != src.i || j != src.j || k != src.k || l != src.l;
}

bool MYON_4udex::operator==(const MYON_4udex& src) const
{
  return i == src.i && j == src.j && k == src.k && l == src.l;
}

bool MYON_4udex::operator!=(const MYON_4udex& src) const
{
  return i != src.i || j != src.j || k != src.k || l != src.l;
}

MYON_3dex::MYON_3dex(
  int iValue,
  int jValue,
  int kValue)
  : i(iValue)
  , j(jValue)
  , k(kValue)
{}

MYON_4dex::MYON_4dex(
  int iValue,
  int jValue,
  int kValue,
  int lValue)
  : i(iValue)
  , j(jValue)
  , k(kValue)
  , l(lValue)
{}

MYON_4dex MYON_4dex::AsIncreasing() const
{
  auto rc = MYON_4dex(i, j, k, l);
  if (j < i) std::swap(rc.i, rc.j);
  if (k < i) std::swap(rc.i, rc.k);
  if (j < i) std::swap(rc.i, rc.l);
  if (k < j) std::swap(rc.j, rc.k);
  if (l < j) std::swap(rc.j, rc.l);
  if (l < k) std::swap(rc.k, rc.l);
  return rc;
}

MYON_4dex MYON_4dex::AsPairwiseIncreasing() const
{
  auto rc = MYON_4dex(i, j, k, l);
  if (k < i)
  {
    std::swap(rc.i, rc.k);
    std::swap(rc.j, rc.l);
  }
  else if (i == k && l < j)
  {
    std::swap(rc.j, rc.l);
  }
  return rc;
}

MYON_4udex MYON_4udex::AsIncreasing() const
{
  auto rc = MYON_4udex(i, j, k, l);
  if (j < i) std::swap(rc.i, rc.j);
  if (k < i) std::swap(rc.i, rc.k);
  if (j < i) std::swap(rc.i, rc.l);
  if (k < j) std::swap(rc.j, rc.k);
  if (l < j) std::swap(rc.j, rc.l);
  if (l < k) std::swap(rc.k, rc.l);
  return rc;
}

MYON_4udex MYON_4udex::AsPairwiseIncreasing() const
{
  auto rc = MYON_4udex(i, j, k, l);
  if (k < i)
  {
    std::swap(rc.i, rc.k);
    std::swap(rc.j, rc.l);
  }
  else if (i == k && l < j)
  {
    std::swap(rc.j, rc.l);
  }
  return rc;
}

int MYON_4dex::operator[](int ind) const
{ 
	switch (ind) {
	case 0:
		return i;
	case 1:
		return j;
	case 2:
		return k;
	}
	return l;
}

int& MYON_4dex::operator[](int ind)
{
	switch (ind) {
	case 0:
		return i;
	case 1:
		return j;
	case 2:
		return k;
	}
	return l;
}

MYON_2udex::MYON_2udex(
  unsigned int iValue,
  unsigned int jValue)
  : i(iValue)
  , j(jValue)
{}

int MYON_2udex::DictionaryCompare(
  const MYON_2udex* lhs,
  const MYON_2udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->i < rhs->i)
    return -1;
  if (lhs->i > rhs->i)
    return 1;
  if (lhs->j < rhs->j)
    return -1;
  if (lhs->j > rhs->j)
    return 1;
  return 0;
}

int MYON_2udex::CompareFirstIndex(
  const MYON_2udex* lhs,
  const MYON_2udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->i < rhs->i)
    return -1;
  if (lhs->i > rhs->i)
    return 1;
  return 0;
}

int MYON_2udex::CompareSecondIndex(
  const MYON_2udex* lhs,
  const MYON_2udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->j < rhs->j)
    return -1;
  if (lhs->j > rhs->j)
    return 1;
  return 0;
}



MYON_3udex::MYON_3udex(
  unsigned int iValue,
  unsigned int jValue,
  unsigned int kValue)
  : i(iValue)
  , j(jValue)
  , k(kValue)
{}

int MYON_3udex::DictionaryCompare(
  const MYON_3udex* lhs,
  const MYON_3udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->i < rhs->i)
    return -1;
  if (lhs->i > rhs->i)
    return 1;
  if (lhs->j < rhs->j)
    return -1;
  if (lhs->j > rhs->j)
    return 1;
  if (lhs->k < rhs->k)
    return -1;
  if (lhs->k > rhs->k)
    return 1;
  return 0;
}

int MYON_3udex::CompareFirstIndex(
  const MYON_3udex* lhs,
  const MYON_3udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->i < rhs->i)
    return -1;
  if (lhs->i > rhs->i)
    return 1;
  return 0;
}

int MYON_3udex::CompareSecondIndex(
  const MYON_3udex* lhs,
  const MYON_3udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->j < rhs->j)
    return -1;
  if (lhs->j > rhs->j)
    return 1;
  return 0;
}

int MYON_3udex::CompareThirdIndex(
  const MYON_3udex* lhs,
  const MYON_3udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->k < rhs->k)
    return -1;
  if (lhs->k > rhs->k)
    return 1;
  return 0;
}

int MYON_3udex::CompareFirstAndSecondIndex(
  const MYON_3udex* lhs,
  const MYON_3udex* rhs
)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  if (lhs->i < rhs->i)
    return -1;
  if (lhs->i > rhs->i)
    return 1;
  if (lhs->j < rhs->j)
    return -1;
  if (lhs->j > rhs->j)
    return 1;
  return 0;
}

MYON_4udex::MYON_4udex(
  unsigned int iValue,
  unsigned int jValue,
  unsigned int kValue,
  unsigned int lValue)
  : i(iValue)
  , j(jValue)
  , k(kValue)
  , l(lValue)
{}

void MYON_StopWatch::Start()
{
  if (MYON_StopWatch::State::Off == m_state || MYON_StopWatch::State::Stopped == m_state)
  {
    m_state = MYON_StopWatch::State::Running;
    m_start = std::chrono::high_resolution_clock::now();
  }
}

double MYON_StopWatch::Stop()
{
  const std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
  double d;
  if ( MYON_StopWatch::State::Running == m_state)
  {
    m_stop = t;
    m_state = MYON_StopWatch::State::Stopped;
    d = ElapsedTime();
  }
  else
  {
    d = 0.0;
  }
  return d;
}

void MYON_StopWatch::Reset()
{
  m_state = MYON_StopWatch::State::Off;
}

MYON_StopWatch::State MYON_StopWatch::CurrentState() const
{
  return m_state;
}

double MYON_StopWatch::ElapsedTime() const
{
  std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
  if (MYON_StopWatch::State::Stopped == m_state)
    t = m_stop;
  double d;
  if (MYON_StopWatch::State::Stopped == m_state || MYON_StopWatch::State::Running == m_state)
  {
    //d = std::chrono::duration< double, std::chrono::high_resolution_clock::period >(t - m_start).count();
    d = std::chrono::duration<double, std::ratio<1, 1> >(t - m_start).count();
  }
  else
  {
    d = 0.0;
  }
  return d;
}

MYON::LineCapStyle MYON::LineCapStyleFromUnsigned(
  unsigned int cap_as_unsigned
)
{
  switch (cap_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LineCapStyle::Round);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LineCapStyle::Flat);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LineCapStyle::Square);
  }
  MYON_ERROR("invalid cap_as_unsigned parameter.");
  return MYON::LineCapStyle::Round;
}

MYON::LineJoinStyle MYON::LineJoinStyleFromUnsigned(
  unsigned int join_as_unsigned
)
{
  switch (join_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LineJoinStyle::Round);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LineJoinStyle::Miter);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::LineJoinStyle::Bevel);
  }
  MYON_ERROR("invalid join_as_unsigned parameter.");
  return MYON::LineJoinStyle::Round;
}


MYON::ClipParticipationSource MYON::ClipParticipationSourceFromUnsigned(
  unsigned int clip_participation_source_as_unsigned
)
{
  switch (clip_participation_source_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ClipParticipationSource::FromLayer);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ClipParticipationSource::FromObject);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ClipParticipationSource::FromParent);
  }
  MYON_ERROR("invalid clip_participation_source_as_unsigned parameter.");
  return MYON::ClipParticipationSource::FromLayer;
}

MYON::SectionFillRule MYON::SectionFillRuleFromUnsigned(
  unsigned int section_fill_rule_as_unsigned
)
{
  switch (section_fill_rule_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::SectionFillRule::ClosedCurves);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::SectionFillRule::SolidObjects);
  }
  MYON_ERROR("invalid section_fill_rule_as_unsigned parameter.");
  return MYON::SectionFillRule::ClosedCurves;
}

MYON::SectionAttributesSource MYON::SectionAttributesSourceFromUnsigned(
  unsigned int section_attributes_source_as_unsigned
)
{
  switch (section_attributes_source_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::SectionAttributesSource::FromLayer);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::SectionAttributesSource::FromObject);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::SectionAttributesSource::FromParent);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::SectionAttributesSource::FromSectioner);
  }
  MYON_ERROR("invalid section_attributes_source_as_unsigned parameter.");
  return MYON::SectionAttributesSource::FromLayer;
}

MYON::ViewSectionBehavior MYON::ViewSectionBehaviorFromUnsigned(
  unsigned int view_section_behavior_as_unsigned
)
{
  switch (view_section_behavior_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ViewSectionBehavior::ClipAndSection);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::ViewSectionBehavior::SectionOnly);
  }
  MYON_ERROR("invalid view_section_behavior_as_unsigned parameter.");
  return MYON::ViewSectionBehavior::ClipAndSection;
}

/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2012 Robert McNeel & Associates. All rights reserved.
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
*/

#if !defined(OPENNURBS_INTERNAL_UNICODE_CP_INC_)
#define OPENNURBS_INTERNAL_UNICODE_CP_INC_

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#if !defined(MYON_RUNTIME_WIN)
#error Do not use for Windows builds.
#endif

#if !defined(MYON_RUNTIME_WIN)
// When we do not have access to Windows code page tools,
// we have to add in code to get convert Windows and Apple
// multibyte encodings to UNICODE encodings. 
//
// In practice, the primary use of the double byte code page support
// is in parsing rich text (RTF) in MYON_TextContent classes created
// on computers with Eastern European and Asian locales as the default
// locale. 
//
// Many Western European and Americas locales are handled by the 
// single byte code pages 1252 and 10000. Code pages for other 
// locales will be added as needed because embedding the large
// double byte tables makes the resulting libraries large. 
//
// At this time opennurbs does not ship the 
// code page N to UNICODE translation tables as separate files
// that can be loaded on demand because of the added installation
// and runtime lookup complexities.
// 
// When possible, Rhino and opennurbs replace code page 
// encodings with UNICODE in RTF. All runtimes strings
// use UNICODE UTF-8, UTF-16, or UTF-32 encodings.
// Whenever possible, the UNICODE encoding is used
// to retrieve glyph information from fonts.
#define MYON_DOUBLE_BYTE_CODE_PAGE_SUPPORT
#endif

#if defined(MYON_DOUBLE_BYTE_CODE_PAGE_SUPPORT)

/////////////////////////////////////////////////////////
//
// Code page 932
//

bool MYON_IsPotentialWindowsCodePage932SingleByteEncoding(
  MYON__UINT32 x
);

bool MYON_IsPotentialWindowsCodePage932DoubleByteEncoding(
  MYON__UINT32 lead_byte,
  MYON__UINT32 trailing_byte
);

/*
Description:
  Convert a Windows code page 932 encoded value to a UNICODE code point.
  This code page is often used for Japanese glyphs.

Parameters:
  code_page_932_character_value - [in]
    Valid values are 0 to 0xFDFE with some exceptions in that range.
  unicode_code_point - [out]
    MYON_UnicodeCodePoint::MYON_ReplacementCharacter is returned when code_page_932_character_value is not valid.

Returns:
  1: if code_page_932_character_value and the corresponding UNICODE code point is returned in *unicode_code_point. 
  0: otherwise and *unicode_code_point = MYON_UnicodeCodePoint::MYON_ReplacementCharacter.

Remarks:
  Windows code page 932: https://msdn.microsoft.com/en-us/library/cc194887.aspx
  Conversions to Unicode are based on the Unicode.org mapping of Shift JIS
  ftp://ftp.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/JIS/SHIFTJIS.TXT
*/
#if defined(MYON_COMPILER_MSC) && defined(NDEBUG)
  // Work around Release build optimization bug in Visual Studio 2017.
__declspec(noinline)
#endif
int MYON_MapWindowsCodePage932ToUnicode(
  MYON__UINT32 code_page_932_character_value,
  MYON__UINT32* unicode_code_point
);

/////////////////////////////////////////////////////////
//
// Code page 949
//

bool MYON_IsPotentialWindowsCodePage949SingleByteEncoding(
  MYON__UINT32 x
);

bool MYON_IsPotentialWindowsCodePage949DoubleByteEncoding(
  MYON__UINT32 lead_byte,
  MYON__UINT32 trailing_byte
);

/*
Description:
  Convert a Windows code page 949 encoded value to a UNICODE code point.
  This code page is often used for Korean glyphs.

Parameters:
  code_page_949_character_value - [in]
    Valid values are 0 to 0xFDFE with some exceptions in that range.
  unicode_code_point - [out]
    MYON_UnicodeCodePoint::MYON_ReplacementCharacter is returned when code_page_949_character_value is not valid.

Returns:
  1: if code_page_949_character_value and the corresponding UNICODE code point is returned in *unicode_code_point. 
  0: otherwise and *unicode_code_point = MYON_UnicodeCodePoint::MYON_ReplacementCharacter.

Remarks:
  Windows code page 949: https://msdn.microsoft.com/en-us/library/cc194941.aspx
  Conversions to Unicode are based on the Unicode.org mapping of Windows-949
  ftp://ftp.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP949.TXT
*/
#if defined(MYON_COMPILER_MSC) && defined(NDEBUG)
  // Work around Release build optimization bug in Visual Studio 2017.
__declspec(noinline)
#endif
int MYON_MapWindowsCodePage949ToUnicode(
  MYON__UINT32 code_page_949_character_value,
  MYON__UINT32* unicode_code_point
);


#endif

#endif

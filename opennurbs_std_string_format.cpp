#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

////////////////////////////////////////////////////////////////
//
// std:string - formatted printing utilities
//

std::string MYON_VARGS_FUNC_CDECL MYON_std_string_format(const char* format, ...) MYON_NOEXCEPT
{
  char stack_buffer[2048];
  MYON_StringBuffer buffer(stack_buffer, sizeof(stack_buffer) / sizeof(stack_buffer[0]));
  int n = 0;
  std::string str;

  if ( 0 != format || 0 != format[0] )
  {
    va_list args;
    va_start(args, format);
    n = MYON_String::FormatVargsIntoBuffer(buffer, format, args);
    va_end(args);
  }

  if ( n > 0 )
  {
    // exceptions are swallowed.
    try { str.append(buffer.m_buffer, n); }
    catch (const std::exception& ) {}
  }

  return str;
}


std::wstring MYON_VARGS_FUNC_CDECL MYON_std_wstring_format(const wchar_t* format, ...) MYON_NOEXCEPT
{
  wchar_t stack_buffer[2048];
  MYON_wStringBuffer buffer(stack_buffer, sizeof(stack_buffer) / sizeof(stack_buffer[0]));
  int n = 0;
  std::wstring str;

  if (0 != format || 0 != format[0])
  {
    va_list args;
    va_start(args, format);
    n = MYON_wString::FormatVargsIntoBuffer(buffer, format, args);
    va_end(args);
  }

  if ( n > 0 )
  {
    // exceptions are swallowed.
    try { str.append(buffer.m_buffer, n); }
    catch (const std::exception& ) {}
  }

  return str;
}

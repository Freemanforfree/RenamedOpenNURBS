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

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_String scanning
//

int MYON_VARGS_FUNC_CDECL MYON_String::Scan(
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_String::ScanBufferVargs(Array(), format, args);
  va_end(args);
  return rc;
}

int MYON_VARGS_FUNC_CDECL MYON_String::Scan(
  const unsigned char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_String::ScanBufferVargs(Array(), (const char*)format, args);
  va_end(args);
  return rc;
}

int MYON_VARGS_FUNC_CDECL MYON_String::ScanBuffer(
  const char* buffer,
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_String::ScanBufferVargs(buffer, format, args);
  va_end(args);
  return rc;
}

int MYON_VARGS_FUNC_CDECL MYON_String::ScanBuffer(
  const unsigned char* buffer,
  const unsigned char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_String::ScanBufferVargs(buffer, format, args);
  va_end(args);
  return rc;
}

int MYON_String::ScanBufferVargs(
  const char* buffer,
  const char* format,
  va_list args
  )
{
#if defined(MYON_COMPILER_CLANG) || defined(MYON_RUNTIME_LINUX)
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  if (nullptr == buffer || nullptr == format)
    return -1;
  return vsscanf(buffer, format, args);
#else
  if (nullptr == buffer || nullptr == format || nullptr == args)
    return -1;
  return vsscanf_l(buffer, MYON_Locale::InvariantCulture.NumericLocalePtr(), format, args);
#endif
#else
  if (nullptr == buffer || nullptr == format || nullptr == args)
    return -1;
  return _vsscanf_s_l(buffer, format, MYON_Locale::InvariantCulture.NumericLocalePtr(), args);
#endif
}

int MYON_String::ScanBufferVargs(
  const unsigned char* buffer,
  const unsigned char* format,
  va_list args
  )
{
  return MYON_String::ScanBufferVargs((const char*)buffer, (const char*)format, args);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_wString scanning
//

int MYON_VARGS_FUNC_CDECL MYON_wString::Scan(
  const wchar_t* format,
  ...
  ) const
{
  va_list args;
  va_start(args, format);
  int rc = MYON_wString::ScanBufferVargs(Array(), format, args);
  va_end(args);
  return rc;
}

int MYON_VARGS_FUNC_CDECL MYON_wString::ScanBuffer(
  const wchar_t* buffer,
  const wchar_t* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_wString::ScanBufferVargs(buffer, format, args);
  va_end(args);
  return rc;
}

int MYON_wString::ScanBufferVargs(
  const wchar_t* buffer,
  const wchar_t* format,
  va_list args
  )
{
  if (nullptr == buffer || nullptr == format)
    return -1;
  
#if defined(MYON_RUNTIME_WIN)
  if (nullptr == args)
    return -1;
  return _vswscanf_s_l(buffer, format, MYON_Locale::InvariantCulture.NumericLocalePtr(), args);
#elif defined(MYON_RUNTIME_APPLE)
  if (nullptr == args)
    return -1;
  return swscanf_l(buffer, _c_locale, format, args);
#elif defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  return swscanf(buffer, format, args);
#else
  return swscanf(buffer, format, args);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_String::ToNumber
//

#define SIGNED_TO_NUMBER( cypte, strytpe, itype, min_limit, max_limit ) \
if (nullptr == value) return nullptr; \
MYON__INT64 _i; \
const cypte* rc = strytpe::ToNumber(buffer, (MYON__INT64)value_on_failure, &_i); \
if (nullptr != rc && _i >= min_limit && _i <= max_limit) *value = (itype)_i; \
else { rc = nullptr; *value = value_on_failure; } \
return rc

#define UNSIGNED_TO_NUMBER( cypte, strytpe, utype, max_limit ) \
if (nullptr == value) return nullptr; \
MYON__UINT64 _u; \
const cypte* rc = strytpe::ToNumber(buffer, (MYON__UINT64)value_on_failure, &_u); \
if (nullptr != rc && _u <= max_limit) *value = (unsigned utype)_u; \
else { rc = nullptr; *value = value_on_failure; } \
return rc

const char* MYON_String::ToNumber(
  const char* buffer,
  char value_on_failure,
  char* value
  )
{
  SIGNED_TO_NUMBER(char, MYON_String, char, -128, 127);
}

const char* MYON_String::ToNumber(
  const char* buffer,
  unsigned char value_on_failure,
  unsigned char* value
  )
{
  UNSIGNED_TO_NUMBER(char, MYON_String, char, 0xff);
}

const char* MYON_String::ToNumber(
  const char* buffer,
  short value_on_failure,
  short* value
  )
{
  SIGNED_TO_NUMBER(char, MYON_String, short, -32768, 32767);
}

const char* MYON_String::ToNumber(
  const char* buffer,
  unsigned short value_on_failure,
  unsigned short* value
  )
{
  UNSIGNED_TO_NUMBER(char, MYON_String, short, 0xffff);
}

const char* MYON_String::ToNumber(
  const char* buffer,
  int value_on_failure,
  int* value
  )
{
  SIGNED_TO_NUMBER(char, MYON_String, int, -2147483648LL, 2147483647LL);
}

const char* MYON_String::ToNumber(
  const char* buffer,
  unsigned int value_on_failure,
  unsigned int* value
  )
{
  UNSIGNED_TO_NUMBER(char, MYON_String, int, 0xffffffff);
}

const char* MYON_String::ToNumber(
  const char* buffer,
  MYON__INT64 value_on_failure,
  MYON__INT64* value
  )
{
  if (nullptr == value)
    return nullptr;

  MYON__INT64 i;
  MYON__UINT64 u;
  const char* rc;
  if ('-' == buffer[0] && buffer[1] >= '0' && buffer[1] <= '9')
  {
    rc = MYON_String::ToNumber(buffer + 1, 0, &u);
    if (nullptr != rc && u <= 9223372036854775808LLU)
    {
      i = -((MYON__INT64)u);
    }
    else
    {
      i = value_on_failure;
      rc = nullptr;
    }
  }
  else
  {
    rc = MYON_String::ToNumber(buffer, 0, &u);
    if (nullptr != rc && u <= 9223372036854775807LLU)
    {
      i = (MYON__INT64)u;
    }
    else
    {
      i = value_on_failure;
      rc = nullptr;
    }
  }

  *value = i;
  return rc;
}

const char* MYON_String::ToNumber(
  const char* buffer,
  MYON__UINT64 value_on_failure,
  MYON__UINT64* value
  )
{
  if (nullptr == value)
    return nullptr;

  MYON__UINT64 u = value_on_failure;
  const char* rc = nullptr;

  if (nullptr != buffer)
  {
    if ('+' == buffer[0])
      buffer++;
    if (buffer[0] >= '0' && buffer[0] <= '9')
    {
      MYON__UINT64 r = (MYON__UINT64)(*buffer++ - '0');
      for (const char* s = buffer;/*empty test*/; s++)
      {
        if (*s >= '0' && *s <= '9')
        {
          MYON__UINT64 d = MYON__UINT64(*s - '0');
          MYON__UINT64 r1 = r * 10LLU + d;
          if (r1 < r)
          {
            // overflow
            break;
          }
          r = r1;
          continue;
        }
        u = r;
        rc = s;
        break;
      }
    }
  }

  *value = u;
  return rc;
}

const char* MYON_String::ToNumber(
  const char* buffer,
  double value_on_failure,
  double* value
  )
{
  if (nullptr == value)
    return nullptr;
  *value = value_on_failure;
  if (nullptr == buffer)
    return nullptr;

  char local_buffer[512];
  const size_t local_buffer_capacity = sizeof(local_buffer) / sizeof(local_buffer[0]);
  size_t local_buffer_count = 0;

  if ('-' == *buffer || '+' == *buffer)
    local_buffer[local_buffer_count++] = (char)(*buffer++);

  bool bHaveMantissa = false;
  while (buffer[0] >= '0' && buffer[0] <= '9')
  {
    if (local_buffer_count >= local_buffer_capacity)
      return nullptr;
    local_buffer[local_buffer_count++] = (char)(*buffer++);
    bHaveMantissa = true;
  }

  if ('.' == *buffer)
  {
    if (local_buffer_count >= local_buffer_capacity)
      return nullptr;
    local_buffer[local_buffer_count++] = (char)(*buffer++);

    while (buffer[0] >= '0' && buffer[0] <= '9')
    {
      if (local_buffer_count >= local_buffer_capacity)
        return nullptr;
      local_buffer[local_buffer_count++] = (char)(*buffer++);
      bHaveMantissa = true;
    }
  }

  if (false == bHaveMantissa)
    return nullptr;

  if ('e' == *buffer || 'E' == *buffer)
  {
    if (local_buffer_count >= local_buffer_capacity)
      return nullptr;
    local_buffer[local_buffer_count++] = (char)(*buffer++);

    if ('-' == *buffer || '+' == *buffer)
    {
      if (local_buffer_count >= local_buffer_capacity)
        return nullptr;
      local_buffer[local_buffer_count++] = (char)(*buffer++);
    }

    while (buffer[0] >= '0' && buffer[0] <= '9')
    {
      if (local_buffer_count >= local_buffer_capacity)
        return nullptr;
      local_buffer[local_buffer_count++] = (char)(*buffer++);
    }
  }

  local_buffer[local_buffer_count++] = 0;

  double x = value_on_failure;
#if defined(MYON_COMPILER_CLANG) || defined(MYON_RUNTIME_LINUX)
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  if (1 == sscanf(local_buffer, "%lg", &x))
  {
    *value = x;
    return buffer;
  }
#else
  if (1 == sscanf_l(local_buffer, MYON_Locale::InvariantCulture.NumericLocalePtr(), "%lg", &x))
  {
    *value = x;
    return buffer;
  }
#endif
#else
  if (1 == _sscanf_s_l(local_buffer, "%lg", MYON_Locale::InvariantCulture.NumericLocalePtr(), &x))
  {
    *value = x;
    return buffer;
  }
#endif
  *value = value_on_failure;
  return nullptr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_wString::ToNumber
//

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  char value_on_failure,
  char* value
  )
{
  SIGNED_TO_NUMBER(wchar_t, MYON_wString, char, -128, 127);
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  unsigned char value_on_failure,
  unsigned char* value
  )
{
  UNSIGNED_TO_NUMBER(wchar_t, MYON_wString, char, 0xff);
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  short value_on_failure,
  short* value
  )
{
  SIGNED_TO_NUMBER(wchar_t, MYON_wString, short, -32768, 32767);
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  unsigned short value_on_failure,
  unsigned short* value
  )
{
  UNSIGNED_TO_NUMBER(wchar_t, MYON_wString, short, 0xffff);
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  int value_on_failure,
  int* value
  )
{
  SIGNED_TO_NUMBER(wchar_t, MYON_wString, int, -2147483648LL, 2147483647LL);
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  unsigned int value_on_failure,
  unsigned int* value
  )
{
  UNSIGNED_TO_NUMBER(wchar_t, MYON_wString, int, 0xffffffff);
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  MYON__INT64 value_on_failure,
  MYON__INT64* value
  )
{
  if (nullptr == value)
    return nullptr;

  MYON__INT64 i = value_on_failure;
  MYON__UINT64 u = 0;
  const wchar_t* rc = nullptr;

  const wchar_t c0 = buffer[0];
  const int sign = MYON_wString::PlusOrMinusSignFromWideChar(c0, true, true, true);
  if (0 != sign)
    ++buffer; // c0 is some type of plus or minus sign.

  const bool b0 = MYON_wString::IsDecimalDigit(buffer[0], true, false, false);
  const bool b1 = false == b0 && MYON_wString::IsDecimalDigit(buffer[0], false, true, false);
  const bool b2 = false == b0 && false == b1 && MYON_wString::IsDecimalDigit(buffer[0], false, false, true);

  if ((b0 || b1 || b2) && sign == MYON_wString::PlusOrMinusSignFromWideChar(c0, b0, b1, b2))
  {
    if (sign < 0)
    {
      rc = MYON_wString::ToNumber(buffer, 0, &u);
      if (nullptr != rc && u <= 9223372036854775808LLU)
      {
        i = -((MYON__INT64)u);
      }
      else
      {
        i = value_on_failure;
        rc = nullptr;
      }
    }
    else
    {
      rc = MYON_wString::ToNumber(buffer, 0, &u);
      if (nullptr != rc && u <= 9223372036854775807LLU)
      {
        i = (MYON__INT64)u;
      }
      else
      {
        i = value_on_failure;
        rc = nullptr;
      }
    }
  }

  *value = i;
  return rc;
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  MYON__UINT64 value_on_failure,
  MYON__UINT64* value
  )
{
  if (nullptr == value)
    return nullptr;

  MYON__UINT64 u = value_on_failure;
  const wchar_t* rc = nullptr;

  if (nullptr != buffer)
  {
    const wchar_t c0 = buffer[0];
    const int sign = MYON_wString::PlusOrMinusSignFromWideChar(c0,true,true,true);
    if (sign > 0)
      ++buffer; // c0 is some type of plus sign.

    const bool b0 = MYON_wString::IsDecimalDigit(buffer[0], true, false, false);
    const bool b1 = false == b0 && MYON_wString::IsDecimalDigit(buffer[0], false, true, false);
    const bool b2 = false == b0 && false == b1 && MYON_wString::IsDecimalDigit(buffer[0], false, false, true);

    if ((b0 || b1 || b2) && sign == MYON_wString::PlusOrMinusSignFromWideChar(c0, b0, b1, b2))
    {
      MYON__UINT64 r = 0;
      for (const wchar_t* s = buffer;/*empty test*/; s++)
      {
        const MYON__UINT64 d = (MYON__UINT64)MYON_wString::DecimalDigitFromWideChar(*s, b0, b1, b2, 10);
        if (d < 10LLU)
        {
          const MYON__UINT64 r1 = r * 10LLU + d;
          if (r1 < r)
          {
            // overflow
            break;
          }
          r = r1;
          continue;
        }
        u = r;
        rc = s;
        break;
      }
    }
  }

  *value = u;
  return rc;
}

const wchar_t* MYON_wString::ToNumber(
  const wchar_t* buffer,
  double value_on_failure,
  double* value
  )
{
  if (nullptr == value)
    return nullptr;
  *value = value_on_failure;
  if (nullptr == buffer)
    return nullptr;

  char local_buffer[512];
  const size_t local_buffer_capacity = sizeof(local_buffer) / sizeof(local_buffer[0]);
  size_t local_buffer_count = 0;

  if ('-' == *buffer || '+' == *buffer)
    local_buffer[local_buffer_count++] = (char)(*buffer++);

  bool bHaveMantissa = false;
  while (buffer[0] >= '0' && buffer[0] <= '9')
  {
    if (local_buffer_count >= local_buffer_capacity)
      return nullptr;
    local_buffer[local_buffer_count++] = (char)(*buffer++);
    bHaveMantissa = true;
  }

  if ('.' == *buffer)
  {
    if (local_buffer_count >= local_buffer_capacity)
      return nullptr;
    local_buffer[local_buffer_count++] = (char)(*buffer++);

    while (buffer[0] >= '0' && buffer[0] <= '9')
    {
      if (local_buffer_count >= local_buffer_capacity)
        return nullptr;
      local_buffer[local_buffer_count++] = (char)(*buffer++);
      bHaveMantissa = true;
    }
  }

  if (false == bHaveMantissa)
    return nullptr;

  if ('e' == *buffer || 'E' == *buffer)
  {
    if (local_buffer_count >= local_buffer_capacity)
      return nullptr;
    local_buffer[local_buffer_count++] = (char)(*buffer++);

    if ('-' == *buffer || '+' == *buffer)
    {
      if (local_buffer_count >= local_buffer_capacity)
        return nullptr;
      local_buffer[local_buffer_count++] = (char)(*buffer++);
    }

    while (buffer[0] >= '0' && buffer[0] <= '9')
    {
      if (local_buffer_count >= local_buffer_capacity)
        return nullptr;
      local_buffer[local_buffer_count++] = (char)(*buffer++);
    }
  }

  local_buffer[local_buffer_count++] = 0;

  double x = value_on_failure;
#if defined(MYON_COMPILER_CLANG) || defined(MYON_RUNTIME_LINUX)
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  if (1 == sscanf(local_buffer, "%lg", &x))
  {
    *value = x;
    return buffer;
  }
#else
  if (1 == sscanf_l(local_buffer, MYON_Locale::InvariantCulture.NumericLocalePtr(), "%lg", &x))
  {
    *value = x;
    return buffer;
  }
#endif
#else
  if (1 == _sscanf_s_l(local_buffer, "%lg", MYON_Locale::InvariantCulture.NumericLocalePtr(), &x))
  {
    *value = x;
    return buffer;
  }
#endif
  *value = value_on_failure;
  return nullptr;
}

const MYON_wString MYON_wString::ToMemorySize(size_t size_in_bytes)
{
  if (size_in_bytes <= 0)
    return MYON_wString(L"0 bytes");

  MYON__UINT64 sz = (MYON__UINT64)size_in_bytes;

  const wchar_t* units;
  const MYON__UINT64 kb = 1024;
  const MYON__UINT64 mb = kb * kb;
  const MYON__UINT64 gb = kb * mb;
  const MYON__UINT64 tb = kb * gb;
  const MYON__UINT64 pb = kb * tb;
  if (sz >= pb)
  {
    // petabytes
    sz /= tb;
    units = L"PB";
  }
  else if (sz >= tb)
  {
    // terabytes
    sz /= gb;
    units = L"TB";
  }
  else if (sz >= gb)
  {
    // gigabytes
    sz /= mb;
    units = L"GB";
  }
  else if (sz >= mb)
  {
    // megaabytes
    sz /= kb;
    units = L"MB";
  }
  else if (sz >= kb)
  {
    // kilobytes
    units = L"KB";
  }
  else
  {
    // bytes
    return  (1==sz) ? MYON_wString(L"1 byte") : MYON_wString::FormatToString(L"%u bytes",(unsigned)sz);
  }

  const MYON__UINT64 n = sz / kb;
  const MYON__UINT64 r = sz % kb;
  if (r > 0 && n < 100)
  {
    const double x = ((double)sz)/((double)kb);
    if (0 == n)
      return MYON_wString::FormatToString(L"0.03f %ls", x, units);

    if (n >= 10)
      return MYON_wString::FormatToString(L"%0.1f %ls", x, units);

    return MYON_wString::FormatToString(L"%0.2f %ls", x, units);
  }

  return MYON_wString::FormatToString(L"%u %ls", ((unsigned)n), units);
}

#undef SIGNED_TO_NUMBER
#undef UNSIGNED_TO_NUMBER

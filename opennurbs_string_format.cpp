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
// MYON_StringBuffer formatting
//

MYON_StringBuffer::MYON_StringBuffer()
  : m_buffer(nullptr)
  , m_buffer_capacity(0)
  , m_heap_buffer(nullptr)
  , m_heap_buffer_capacity(0)
{}

MYON_StringBuffer::MYON_StringBuffer(
  char* stack_buffer,
  size_t stack_buffer_capacity
  )
  : m_buffer(stack_buffer_capacity > 0 ? stack_buffer : nullptr)
  , m_buffer_capacity(nullptr == m_buffer ? 0 : stack_buffer_capacity)
  , m_heap_buffer(nullptr)
  , m_heap_buffer_capacity(0)
{}

MYON_StringBuffer::~MYON_StringBuffer()
{
  m_buffer = nullptr;
  m_buffer_capacity = 0;
  if (nullptr != m_heap_buffer)
  {
    delete[] m_heap_buffer;
    m_heap_buffer = nullptr;
  }
  m_heap_buffer_capacity = 0;
}

bool MYON_StringBuffer::GrowBuffer(
  size_t buffer_capacity
  )
{
  if (buffer_capacity <= m_buffer_capacity && (nullptr != m_buffer || 0 == m_buffer_capacity))
    return true;

  if (buffer_capacity > m_heap_buffer_capacity || nullptr == m_heap_buffer)
  {
    if (nullptr != m_heap_buffer)
      delete[] m_heap_buffer;
    m_heap_buffer = new(std::nothrow) char[buffer_capacity];
    m_heap_buffer_capacity = (nullptr != m_heap_buffer) ? buffer_capacity : 0;
  }

  m_buffer = m_heap_buffer;
  m_buffer_capacity = m_heap_buffer_capacity;

  return (buffer_capacity <= m_buffer_capacity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_wStringBuffer formatting
//

MYON_wStringBuffer::MYON_wStringBuffer()
  : m_buffer(nullptr)
  , m_buffer_capacity(0)
  , m_heap_buffer(nullptr)
  , m_heap_buffer_capacity(0)
{}

MYON_wStringBuffer::MYON_wStringBuffer(
  wchar_t* stack_buffer,
  size_t stack_buffer_capacity
  )
  : m_buffer(stack_buffer_capacity > 0 ? stack_buffer : nullptr)
  , m_buffer_capacity(nullptr == m_buffer ? 0 : stack_buffer_capacity)
  , m_heap_buffer(nullptr)
  , m_heap_buffer_capacity(0)
{}

MYON_wStringBuffer::~MYON_wStringBuffer()
{
  m_buffer = nullptr;
  m_buffer_capacity = 0;
  if (nullptr != m_heap_buffer)
  {
    delete[] m_heap_buffer;
    m_heap_buffer = nullptr;
  }
  m_heap_buffer_capacity = 0;
}

bool MYON_wStringBuffer::GrowBuffer(
  size_t buffer_capacity
  )
{
  if (buffer_capacity <= m_buffer_capacity && (nullptr != m_buffer || 0 == m_buffer_capacity))
    return true;

  if (buffer_capacity > m_heap_buffer_capacity || nullptr == m_heap_buffer)
  {
    if (nullptr != m_heap_buffer)
      delete[] m_heap_buffer;
    m_heap_buffer = new(std::nothrow) wchar_t[buffer_capacity];
    m_heap_buffer_capacity = (nullptr != m_heap_buffer) ? buffer_capacity : 0;
  }

  m_buffer = m_heap_buffer;
  m_buffer_capacity = m_heap_buffer_capacity;

  return (buffer_capacity <= m_buffer_capacity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_String::FromNumber number formatting
//

#define BUFFER_DECL(ctype) ctype _buffer[64]; unsigned int _buffer_index = 63; const ctype _zero('0');

#define UNSIGNED_TO_BUFFER(ctype,n) \
MYON__UINT64 _u64 = (MYON__UINT64)(n); \
_buffer[_buffer_index] = 0; \
if (0==_u64) {\
_buffer[--_buffer_index] = _zero; \
} else { \
while (_u64 > 0 && _buffer_index > 0) {ctype d = (ctype)(_u64%10); _u64 /= 10; _buffer[--_buffer_index] = _zero + d;} \
}

#define SIGNED_TO_BUFFER(ctype,n) \
MYON__INT64 _i64 = (MYON__INT64)(n); UNSIGNED_TO_BUFFER(ctype,_i64<0?(-_i64):_i64) if (_i64 < 0 && _buffer_index > 0) {_buffer[--_buffer_index] = '-';}

#define UNSIGNED_TO_STRING(ctype,stype,n) BUFFER_DECL(ctype) UNSIGNED_TO_BUFFER(ctype,n) return stype(&_buffer[_buffer_index]);
#define SIGNED_TO_STRING(ctype,stype,n) BUFFER_DECL(ctype) SIGNED_TO_BUFFER(ctype,n) return stype(&_buffer[_buffer_index]);

const MYON_String MYON_String::FromNumber(
  char n
  )
{
  SIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  short n
  )
{
  SIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  int n
  )
{
  SIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  MYON__INT64 n
  )
{
  SIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  unsigned char n
  )
{
  UNSIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  unsigned short n
  )
{
  UNSIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  unsigned int n
  )
{
  UNSIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  MYON__UINT64 n
  )
{
  UNSIGNED_TO_STRING(char, MYON_String, n)
}

const MYON_String MYON_String::FromNumber(
  double d
  )
{
  char buffer[64];
  if (MYON_String::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), "%g", d) > 0)
    return MYON_String(buffer);
  return MYON_String::EmptyString;
}

const MYON_String MYON_String::ApproximateFromNumber(
  double d
  )
{
  char buffer[64];
  if (0.0 == d || (d == d && fabs(d) >= 1.0e-16 && fabs(d) <= 1.0e16))
  {
    // Do not use %f without making sure the number is in a range
    // reasonable for %f.  Otherwise we end up with number strings
    // thate are 300 digits long when a 1e300 comes by.
    if (MYON_String::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), "%f", d) > 0)
      return MYON_String(buffer);
    // It may be that 64 elements were not enought for %f format if the "reasonable range"
    // test is not good enough.  We try again with "%g".
  }
  if (MYON_String::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), "%g", d) > 0)
    return MYON_String(buffer);
  return MYON_String::EmptyString;
}

const MYON_String MYON_String::PreciseFromNumber(
  double d
  )
{
  char buffer[64];
  if (MYON_String::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), "%.17g", d) > 0)
    return MYON_String(buffer);
  return MYON_String::EmptyString;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_wString::FromNumber number formatting
//

const MYON_wString MYON_wString::FromNumber(
  char n
  )
{
  SIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  short n
  )
{
  SIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  int n
  )
{
  SIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  MYON__INT64 n
  )
{
  SIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  unsigned char n
  )
{
  UNSIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  unsigned short n
  )
{
  UNSIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  unsigned int n
  )
{
  UNSIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  MYON__UINT64 n
  )
{
  UNSIGNED_TO_STRING(wchar_t, MYON_wString, n)
}

const MYON_wString MYON_wString::FromNumber(
  double d
  )
{
  wchar_t buffer[64];
  if (MYON_wString::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%g", d) > 0)
    return MYON_wString(buffer);
  return MYON_wString::EmptyString;
}

const MYON_wString MYON_wString::ApproximateFromNumber(
  double d
  )
{
  wchar_t buffer[64];
  if (0.0 == d || (d == d && fabs(d) >= 1.0e-16 && fabs(d) <= 1.0e16))
  {
    // Do not use %f without making sure the number is in a range
    // reasonable for %f.  Otherwise we end up with number strings
    // thate are 300 digits long when a 1e300 comes by.
    if (MYON_wString::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%f", d) > 0)
      return MYON_String(buffer);
    // It may be that 64 elements were not enough for %f format if the "reasonable range"
    // test is not good enough.  We try again with "%g".
  }
  if (MYON_wString::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%g", d) > 0)
    return MYON_String(buffer);
  return MYON_String::EmptyString;
}

const MYON_wString MYON_wString::PreciseFromNumber(
  double d
  )
{
  wchar_t buffer[64];
  if (MYON_wString::FormatIntoBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%.17g", d) > 0)
    return MYON_wString(buffer);
  return MYON_wString::EmptyString;
}

const MYON_wString MYON_wString::FromCurrentCoordinatedUniversalTime(
  MYON_DateFormat date_format,
  MYON_TimeFormat time_format,
  wchar_t date_separator,
  wchar_t date_time_separator,
  wchar_t time_separator
)
{
  struct tm current_time;
  memset(&current_time,0,sizeof(current_time));
  time_t gmt = time(0);
  const struct tm* t = gmtime(&gmt);
  if ( t )
    current_time = *t;
  return MYON_wString::FromTime(
    current_time,
    date_format,
    time_format,
    date_separator,
    date_time_separator,
    time_separator
  );
}

const MYON_wString MYON_wString::FromSecondsSinceJanuaryFirst1970(
  MYON__UINT64 seconds_since_jan_first_1970,
  MYON_DateFormat date_format,
  MYON_TimeFormat time_format,
  wchar_t date_separator,
  wchar_t date_time_separator,
  wchar_t time_separator
)
{
  struct tm current_time;
  memset(&current_time, 0, sizeof(current_time));
  time_t uct = (time_t)seconds_since_jan_first_1970;
  const struct tm* t = gmtime(&uct);
  if (t)
  {
    current_time = *t;
  }  
  return FromTime(current_time, date_format, time_format, date_separator, date_time_separator, time_separator);
}

const MYON_wString MYON_wString::FromTime(
  const struct tm& t,
  MYON_DateFormat date_format,
  MYON_TimeFormat time_format,
  wchar_t date_separator,
  wchar_t date_time_separator,
  wchar_t time_separator
)
{
  int mday = t.tm_mday;
  if (mday < 1 || mday > 31)
    mday = 0;

  int yday = t.tm_yday;
  if (yday < 0 || yday > 365)
    yday = 0;
  else
    yday++;

  int mon = t.tm_mon;
  if (mon < 0 || mon > 11)
    mon = 0;
  else
    mon++;

  int year = t.tm_year;
  if (year < 0)
    year = 0;
  else
    year += 1900;

  return (mon > 0 && mday > 0)
    ? MYON_wString::FromYearMonthDayHourMinuteSecond(
      year, mon, mday,
      (int)t.tm_hour, (int)t.tm_min, (int)t.tm_sec,
      date_format,
      time_format,
      date_separator,
      date_time_separator,
      time_separator
    )
    : MYON_wString::FromYearDayHourMinuteSecond(
      year, yday,
      (int)t.tm_hour, (int)t.tm_min, (int)t.tm_sec,
      date_format,
      time_format,
      date_separator,
      date_time_separator,
      time_separator
    );
}


const MYON_wString MYON_wString::FromYearDayHourMinuteSecond(
  int year,
  int day_of_year,
  int hour,
  int minute,
  int second,
  MYON_DateFormat date_format,
  MYON_TimeFormat time_format,
  wchar_t date_separator,
  wchar_t date_time_separator,
  wchar_t time_separator
)
{
  unsigned int month = 0;
  unsigned int mday = 0;
  if ( MYON_DateFormat::Unset != date_format && MYON_DateFormat::Omit != date_format && year > 0 && day_of_year > 0 && day_of_year <= 366 )
  {
    MYON_GetGregorianMonthAndDayOfMonth(
      (unsigned int)year,
      (unsigned int)day_of_year,
      &month,
      &mday
    );
  }

  return MYON_wString::FromYearMonthDayHourMinuteSecond(
    year,
    (int)month,
    (int)mday,
    hour,
    minute,
    second,
    date_format,
    time_format,
    date_separator,
    date_time_separator,
    time_separator
  );
}

const MYON_wString MYON_wString::FromYearMonthDayHourMinuteSecond(
  int year,
  int month,
  int mday,
  int hour,
  int minute,
  int second,
  MYON_DateFormat date_format,
  MYON_TimeFormat time_format,
  wchar_t date_separator,
  wchar_t date_time_separator,
  wchar_t time_separator
)
{
  if (year < 1582)
    year = 0;

  if (mday < 1 || mday > 31)
    mday = 0;

  if (month < 1 || month > 12)
    month = 0;

  const int yday
    = (MYON_DateFormat::YearDayOfYear == date_format)
    ? MYON_DayOfGregorianYear(year, month, mday)
    : 0;

  if (0 == date_separator)
    date_separator = MYON_wString::HyphenMinus;
  if (0 == date_time_separator)
    date_time_separator = MYON_wString::Space;
  if (0 == time_separator)
    time_separator = ':';
  
  bool bValidDate 
    = MYON_DateFormat::YearDayOfYear == date_format
    ? (yday > 0)
    : (month > 0 && mday > 0);
  bool bValidHMS = (hour >= 0 && minute >= 0 && second >= 0 && hour < 24 && minute <= 59 && second <= 59);


  const wchar_t ds[2] = { date_separator, 0 };
  const wchar_t ts[2] = { time_separator, 0 };
  const wchar_t* ampm = (hour >= 12) ? L"PM" : L"AM";
  
  MYON_wString date;
  switch (date_format)
  {
  case MYON_DateFormat::Unset:
    bValidDate = true;
    break;
  case MYON_DateFormat::Omit:
    bValidDate = true;
    break;
  case MYON_DateFormat::YearMonthDay:
    date = MYON_wString::FormatToString(L"%d%ls%d%ls%d", year, ds, month, ds, mday);
    break;
  case MYON_DateFormat::YearDayMonth:
    date = MYON_wString::FormatToString(L"%d%ls%d%ls%d", year, ds, mday, ds, month);
    break;
  case MYON_DateFormat::MonthDayYear:
    date = MYON_wString::FormatToString(L"%d%ls%d%ls%d", month, ds, mday, ds, year);
    break;
  case MYON_DateFormat::DayMonthYear:
    date = MYON_wString::FormatToString(L"%d%ls%d%ls%d", mday, ds, month, ds, year);
    break;
  case MYON_DateFormat::YearDayOfYear:
    date = MYON_wString::FormatToString(L"%d%ls%d", year, ds, yday);
    break;
  default:
    break;
  }
  if (false == bValidDate)
    return MYON_wString::EmptyString;
  
  MYON_wString hms;
  switch (time_format)
  {
  case MYON_TimeFormat::Unset:
    bValidHMS = true;
    break;
  case MYON_TimeFormat::Omit:
    bValidHMS = true;
    break;
  case MYON_TimeFormat::HourMinute12:
    hms = MYON_wString::FormatToString(L"%02d%ls%02d%ls", hour%12, ts, minute, ampm);
    break;
  case MYON_TimeFormat::HourMinuteSecond12:
    hms = MYON_wString::FormatToString(L"%02d%ls%02d%ls%02d%ls", hour%12, ts, minute, ts, second, ampm);
    break;
  case MYON_TimeFormat::HourMinute24:
    hms = MYON_wString::FormatToString(L"%02d%ls%02d", hour, ts, minute);
    break;
  case MYON_TimeFormat::HourMinuteSecond24:
    hms = MYON_wString::FormatToString(L"%02d%ls%02d%ls%02d", hour, ts, minute, ts, second);
    break;
  default:
    break;
  }

  MYON_wString result = date;
  if (result.IsNotEmpty() && hms.IsNotEmpty())
  {
    const wchar_t dts[] = { date_time_separator,0 };
    result += dts;
  }
  result += hms;

  return result;
}

static bool MYON_BytesToHexadecimalString(
  const MYON__UINT8* bytes,
  size_t byte_count,
  bool bCapitalDigits,
  bool bReverse,
  char* str,
  size_t str_capacity
  )
{
  if (nullptr == str || str_capacity < 2*byte_count || byte_count <= 0 || nullptr == bytes )
  {
    if (nullptr != str && str_capacity > 0)
      str[0] = 0;
    return false;
  }
  const int A_minus_10 = (bCapitalDigits ? 'A' : 'a') - 10;
  size_t j = 0;
  int c;
  if ( bReverse )
    bytes += 19;
  const int delta_byte = bReverse ? -1 : 1;
  for (int i = 0; i < 20; i++)
  {
    MYON__UINT8 b = *bytes;
    bytes += delta_byte;
    c = (int)(b / 16);
    if (c < 10)
      c += '0';
    else
      c += A_minus_10;
    if (j < str_capacity)
      str[j++] = (char)c;
    c = (int)(b % 16);
    if (c < 10)
      c += '0';
    else
      c += A_minus_10;
    if (j < str_capacity)
      str[j++] = (char)c;
  }
  if ((size_t)j < str_capacity)
    str[j] = 0;
  return true;
}

const MYON_String MYON_String::HexadecimalFromBytes(
  const MYON__UINT8* bytes,
  size_t byte_count,
  bool bCapitalDigits,
  bool bReverse
  )
{
  if (nullptr == bytes || byte_count <= 0)
    return MYON_String::EmptyString;

  const size_t s_length = 2 * byte_count;
  MYON_String s;
  s.ReserveArray(s_length);
  s.SetLength(s_length);
  if ( false == MYON_BytesToHexadecimalString(bytes,byte_count,bCapitalDigits,bReverse,s.Array(),s_length) )
    return MYON_String::EmptyString;

  return s;
}

const MYON_wString MYON_wString::HexadecimalFromBytes(
  const MYON__UINT8* bytes,
  size_t byte_count,
  bool bCapitalDigits,
  bool bReverse
  )
{
  if (nullptr == bytes || byte_count <= 0)
    return MYON_String::EmptyString;

  size_t s_length = 2 * byte_count;
  MYON_wString s;
  s.ReserveArray(s_length);
  s.SetLength(s_length);
  wchar_t* wstr = s.Array();
  char* str = (char*)wstr;
  if ( false == MYON_BytesToHexadecimalString(bytes,byte_count,bCapitalDigits,bReverse,str,s_length) )
    return MYON_String::EmptyString;
  wchar_t* wstr0 = wstr;
  wstr += s_length;
  str += s_length;
  while (wstr > wstr0)
  {
    wstr--;
    str--;
    *wstr = (wchar_t)*str; // all str[] elements are single byte UTF-8 encodings
  }

  return s;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_String formatting
//

bool MYON_VARGS_FUNC_CDECL MYON_String::Format(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  bool rc = FormatVargs(format, args);
  va_end(args);
  return rc;
}

bool MYON_VARGS_FUNC_CDECL MYON_String::Format(const unsigned char* format, ...)
{
  va_list args;
  va_start(args, format);
  bool rc = FormatVargs(format, args);
  va_end(args);
  return rc;
}

const MYON_String MYON_VARGS_FUNC_CDECL MYON_String::FormatToString(
  const char* format,
  ...
  )
{
  MYON_StringBuffer buffer;
  va_list args;
  va_start(args, format);
  MYON_String::FormatVargsIntoBuffer(buffer, format, args);
  va_end(args);
  return MYON_String(buffer.m_buffer);
}

bool MYON_String::FormatVargs(const char* format, va_list args)
{
  const int len_count = MYON_String::FormatVargsOutputCount(format, args);
  if (len_count > 0)
  {
    // temp is used because sometimes "this" is an argument as in:
    //   MYON_String str = L"filename.ext";
    //   str.Format(L"C:\\%s",str);
    MYON_String temp;
    temp.SetLength(len_count);
    const int len_string = MYON_String::FormatVargsIntoBuffer(temp.Array(), size_t(len_count) + 1, format, args);
    if (len_string == len_count)
    {
      *this = temp;
      return true;
    }
  }
  Destroy();
  Create();
  return (0 == len_count);
}

bool MYON_String::FormatVargs(const unsigned char* format, va_list args)
{
  return FormatVargs((const char*)format, args);
}

int MYON_VARGS_FUNC_CDECL MYON_String::FormatIntoBuffer(
  char* buffer, size_t buffer_capacity,
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_String::FormatVargsIntoBuffer(buffer, buffer_capacity, format, args);
  va_end(args);
  return rc;
}

int MYON_VARGS_FUNC_CDECL MYON_String::FormatIntoBuffer(
  MYON_StringBuffer& buffer,
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_String::FormatVargsIntoBuffer(buffer, format, args);
  va_end(args);
  return rc;
}

int MYON_String::FormatVargsIntoBuffer(
  char* buffer,
  size_t buffer_capacity,
  const char* format,
  va_list args
  )
{
  if (0 == buffer || buffer_capacity <= 0)
    return -1;
  buffer[0] = 0;
#if defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
  // CLang modifies args so a copy is required
  va_list args_copy;
  va_copy (args_copy, args);
#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  int len = vsnprintf(buffer, buffer_capacity, format, args_copy);
#else
  int len = vsnprintf_l(buffer, buffer_capacity, MYON_Locale::Ordinal.NumericLocalePtr(), format, args_copy);
#endif
  va_end(args_copy);
#else
  int len = _vsprintf_p_l(buffer, buffer_capacity, format, MYON_Locale::Ordinal.NumericLocalePtr(), args);
#endif
  if (((size_t)len) >= buffer_capacity)
    len = -1;
  buffer[(len <= 0) ? 0 : len] = 0;
  buffer[buffer_capacity - 1] = 0;
  return len;
}

int MYON_String::FormatVargsIntoBuffer(
  MYON_StringBuffer& buffer,
  const char* format,
  va_list args
  )
{
  va_list args_copy;
  va_copy(args_copy, args);
  int rc = MYON_String::FormatVargsOutputCount(format, args_copy);
  va_end(args_copy);

  size_t buffer_capacity = (rc <= 0) ? 1 : (size_t(rc) + 1);
  if (false == buffer.GrowBuffer(buffer_capacity) || nullptr == buffer.m_buffer || buffer.m_buffer_capacity <= 0)
    return (rc < 0 ? rc : -1);

  buffer.m_buffer[0] = 0;
  buffer.m_buffer[buffer.m_buffer_capacity - 1] = 0;
  if (rc > 0)
  {
    rc = MYON_String::FormatVargsIntoBuffer(buffer.m_buffer, buffer.m_buffer_capacity, format, args);
  }
  return rc;
}

int MYON_String::FormatVargsOutputCount(
  const char* format,
  va_list args
  )
{
  if ( nullptr == format || 0 == format[0] )
    return 0;

#if defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)
  // CLang modifies args so a copy is required
  va_list args_copy;
  va_copy (args_copy, args);

#if defined(MYON_RUNTIME_ANDROID) || defined(MYON_RUNTIME_LINUX)
  int len = vsnprintf(nullptr, 0, format, args_copy);
#else
  int len = vsnprintf_l(nullptr, 0, MYON_Locale::Ordinal.NumericLocalePtr(), format, args_copy);
#endif
  va_end(args_copy);
  return len;
#else
  return _vscprintf_p_l(format, MYON_Locale::Ordinal.NumericLocalePtr(), args);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_wString formatting
//


bool MYON_VARGS_FUNC_CDECL MYON_wString::Format(const wchar_t* format, ...)
{
  va_list args;
  va_start(args, format);
  bool rc = FormatVargs(format, args);
  va_end(args);
  return rc;
}

bool MYON_wString::FormatVargs(const wchar_t* format, va_list args)
{
  const int len_count = MYON_wString::FormatVargsOutputCount(format, args);
  if (len_count > 0)
  {
    // temp is used because sometimes "this" is an argument as in:
    //   MYON_String str = L"filename.ext";
    //   str.Format(L"C:\\%s",str);
    MYON_wString temp;
    temp.SetLength(len_count);
    const int len_string = MYON_wString::FormatVargsIntoBuffer(temp.Array(), size_t(len_count) + 1, format, args);
    if (len_string == len_count)
    {
      *this = temp;
      return true;
    }
  }
  Destroy();
  Create();
  return (0 == len_count);
}


const MYON_wString MYON_VARGS_FUNC_CDECL MYON_wString::FormatToString(
  const wchar_t* format,
  ...
  )
{
  MYON_wStringBuffer buffer;
  va_list args;
  va_start(args, format);
  MYON_wString::FormatVargsIntoBuffer(buffer, format, args);
  va_end(args);
  return MYON_wString(buffer.m_buffer);
}

int MYON_VARGS_FUNC_CDECL MYON_wString::FormatIntoBuffer(
  wchar_t* buffer, size_t buffer_capacity,
  const wchar_t* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_wString::FormatVargsIntoBuffer(buffer, buffer_capacity, format, args);
  va_end(args);
  return rc;
}

int MYON_VARGS_FUNC_CDECL MYON_wString::FormatIntoBuffer(
  MYON_wStringBuffer& buffer,
  const wchar_t* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  int rc = MYON_wString::FormatVargsIntoBuffer(buffer, format, args);
  va_end(args);
  return rc;
}

#if defined(MYON_COMPILER_CLANG)

static const wchar_t* ConvertToCLangFormat(
  const wchar_t* format,
  MYON_wStringBuffer& clang_format_buffer
  )
{
  size_t format_capacity = 0;
  const wchar_t* s;
  wchar_t c;

  s = format;
  for(;;)
  {
    if ('%' == *s)
    {
      s++;
      c = *s;
      if ('s' == c )
      {
        // convert %s to %ls
        format_capacity++;
        s++;
      }
      else if (c >= '1' && c <= '9')
      {
        // look for %N$s  with N >= 1 ...
        s++;
        while (*s >= '0' && *s <= '9')
          s++;
        if ('$' == *s && 's' == s[1] )
        {
          // convert %N$s to %N$ls
          format_capacity++;
          s++;
          s++;
        }
      }
    }
    if ( 0 == *s++ )
      break;
  }

  if (0 == format_capacity)
    return format;

  format_capacity += (s - format) + 1; // +1 for null terminator

  if ( !clang_format_buffer.GrowBuffer(format_capacity) )
    return format;

  wchar_t* ls = clang_format_buffer.m_buffer; 
  if ( nullptr == ls )
    return format;

  s = format;
  format = ls;
  for(;;)
  {
    if ('%' == *s)
    {
      *ls++ = *s++;
      c = *s;
      if ('s' == c )
      {
        // convert %s to %ls
        *ls++ = 'l';
        *ls++ = *s++;
      }
      else if (c >= '1' && c <= '9')
      {
        // look for %N$s  with N >= 1 ...
        *ls++ = *s++;
        while (*s >= '0' && *s <= '9')
          *ls++ = *s++;
        if ('$' == *s && 's' == s[1] )
        {
          // convert %N$s to %N$ls
          *ls++ = *s++;
          *ls++ = 'l';
          *ls++ = *s++;
        }
      }

    }
    if ( 0 == (*ls++ = *s++) )
      break;
  }

  return format;
}

#endif


int MYON_wString::FormatVargsIntoBuffer(
  wchar_t* buffer,
  size_t buffer_capacity,
  const wchar_t* format,
  va_list args
  )
{
  if (0 == buffer || buffer_capacity <= 0)
    return -1;
  buffer[0] = 0;

  if ( nullptr == format || 0 == format[0] )
    return 0;
  
#if defined(MYON_COMPILER_CLANG)
  // CLang requires %ls to properly format a const wchar_t* parameter
  wchar_t clang_format_stack_buffer[128];
  MYON_wStringBuffer clang_format_buffer(clang_format_stack_buffer, sizeof(clang_format_stack_buffer) / sizeof(clang_format_stack_buffer[0]));
  format = ConvertToCLangFormat(
    format,
    clang_format_buffer
    );

  va_list args_copy;
  va_copy (args_copy, args);
  // Cannot use Apple's vswprintf_l() because it's buggy. 
  // This means we cannot be certain that a period will be used for a decimal point in formatted printing.
  // For details, see comments below in MYON_wString::FormatVargsOutputCount().
  //int len = vswprintf_l(buffer, buffer_capacity, MYON_Locale::Ordinal.NumericLocalePtr(), format, args_copy);
  int len = vswprintf(buffer, buffer_capacity, format, args_copy);
  va_end(args_copy);
  
#else

#if defined(MYON_COMPILER_GNU)
  va_list args_copy;
  va_copy (args_copy, args);
  int len = vswprintf(buffer, buffer_capacity, format, args_copy);
  va_end(args_copy);
#else
  // Using MYON_Locale::Ordinal.NumericLocalePtr() insures that a period 
  // will be use for the decimal point in formatted printing.
  int len = _vswprintf_p_l(buffer, buffer_capacity, format, MYON_Locale::Ordinal.NumericLocalePtr(), args);
#endif
#endif
  if (((size_t)len) >= buffer_capacity)
    len = -1;
  buffer[(len <= 0) ? 0 : len] = 0;
  buffer[buffer_capacity - 1] = 0;
  return len;
}

int MYON_wString::FormatVargsIntoBuffer(
  MYON_wStringBuffer& buffer,
  const wchar_t* format,
  va_list args
  )
{
  va_list args_copy;
  va_copy(args_copy, args);
  int rc = MYON_wString::FormatVargsOutputCount(format, args_copy);
  va_end(args_copy);

  size_t buffer_capacity = (rc <= 0) ? 1 : (size_t(rc) + 1);
  if (false == buffer.GrowBuffer(buffer_capacity) || nullptr == buffer.m_buffer || buffer.m_buffer_capacity <= 0)
    return (rc < 0 ? rc : -1);

  buffer.m_buffer[0] = 0;
  buffer.m_buffer[buffer.m_buffer_capacity - 1] = 0;
  if (rc > 0)
  {
    rc = MYON_wString::FormatVargsIntoBuffer(buffer.m_buffer, buffer.m_buffer_capacity, format, args);
  }
  return rc;
}

int MYON_wString::FormatVargsOutputCount(
  const wchar_t* format,
  va_list args
  )
{
  if ( nullptr == format || 0 == format[0] )
    return 0;

#if defined(MYON_COMPILER_CLANG)
  // Unlike _vscwprintf_p_l(), CLang's vswprintf() does not tell you how many characters would have 
  // been written if there was space enough in the buffer. 
  // It reports an error when there is not enough space.  
  // Assume a moderately large machine so kilobytes of wchar_t on the stack is not a problem.

  // CLang requires %ls to properly format a const wchar_t* parameter
  wchar_t clang_format_stack_buffer[128];
  MYON_wStringBuffer clang_format_buffer(clang_format_stack_buffer, sizeof(clang_format_stack_buffer) / sizeof(clang_format_stack_buffer[0]));
  format = ConvertToCLangFormat(
    format,
    clang_format_buffer
    );

  // Attempting to directly get the count fails in OS X 10.4 June 2015 (always returns fmt_size = -1)
  //
  ////va_list args_copy;
  ////va_copy(args_copy, args);
  ////const int formatted_string_count = vswprintf_l(nullptr, 0, MYON_Locale::Ordinal.NumericLocalePtr(), format, args_copy);
  ////va_end(args_copy);
  ////return (formatted_string_count >= 0) ? formatted_string_count : -1;
  
  wchar_t stack_buffer[1024];
  MYON_wStringBuffer buffer(stack_buffer, sizeof(stack_buffer) / sizeof(stack_buffer[0]));
  size_t buffer_capacity = buffer.m_buffer_capacity;
  for(;;)
  {
    va_list args_copy;
    va_copy(args_copy, args);

    // NOTE:
    //   Cannot use Apple's vswprintf_l() because it's buggy. 
    //   This means we cannot be certain that a period will be used for a decimal point in formatted printing.
    //
    //   Apple's OS X 10.4 (July 2015) formatted printing functions are buggy when locale is specified.
    //   Here's what happens:
    ////    int MYON_VARGS_FUNC_CDECL VargsFormatFuncA(const wchar_t* format, ...)
    ////    {
    ////      va_list args;
    ////      va_start(args, format);
    ////      wchar_t buffer[128];
    ////      int count = vswprintf(buffer, 128, format, args);
    ////      va_end(args);
    ////      return count;
    ////    }
    ////       
    ////    int MYON_VARGS_FUNC_CDECL VargsFormatFuncB(const wchar_t* format, ...)
    ////    {
    ////      va_list args;
    ////      va_start(args, format);
    ////      wchar_t buffer[128];
    ////    #if defined(MYON_COMPILER_CLANG)
    ////      int count = vswprintf_l(buffer, 128, MYON_Locale::Ordinal.NumericLocalePtr(), format, args_copy);
    ////    #else
    ////      int count = _vswprintf_p_l(buffer, 128, format, MYON_Locale::Ordinal.NumericLocalePtr(), args);
    ////    #endif
    ////      va_end(args);
    ////      return count;
    ////    }    ////        
    ////    
    ////    Tests()
    ////    {
    ////      const wchar_t str1[5] = {'T', 'e',   's', 't', 0};
    ////      const wchar_t str2[5] = {'T', 0xFFFD,'s', 't', 0};
    ////    
    ////      int Acount1 = VargsFormatFuncA(L"%ls",str1);
    ////      int Acount2 = VargsFormatFuncA(L"%ls",str2);
    ////    
    ////      RhinoApp().Print("Acount1 = %d Acount2 = %d\n",Acount1,Acount2);
    ////      
    ////      int Bcount1 = VargsFormatFuncB(L"%ls",str1);
    ////      int Bcount2 = VargsFormatFuncB(L"%ls",str2);
    ////    
    ////       // Windows Results: Acount1 = 4, Acount2 = 4, Bcount1 = 4, Bcount2 = 4
    ////       // Apple   Results: Acount1 = 4, Acount2 = 4, Bcount1 = 4, Bcount2 = -1
    ////    }
    ////      
    //const int formatted_string_count = vswprintf_l(buffer.m_buffer, buffer.m_buffer_capacity, MYON_Locale::Ordinal.NumericLocalePtr(), format, args_copy);
    const int formatted_string_count = vswprintf(buffer.m_buffer, buffer.m_buffer_capacity, format, args_copy);
    va_end(args_copy);
    if (formatted_string_count >= 0)
    {
      // formatted_string_count = number of wchar_t elements not including null terminator
      return formatted_string_count;
    }
    if ( buffer_capacity >= 1024*16*16*16 )
      break;
    buffer_capacity *= 16;
    if (false == buffer.GrowBuffer(buffer_capacity))
      break;
    if (nullptr == buffer.m_buffer)
      break;
    if (buffer_capacity < buffer.m_buffer_capacity)
      break;
  }
  return -1;
#else
#if defined(MYON_COMPILER_GNU)
  // 31 May 2019 S. Baer (RH-52038)
  // TODO: The following code needs to be tested. This was added by request from a user that needed
  // a GCC compile. This is obviously a cut and paste of the above clang code
  wchar_t stack_buffer[1024];
  MYON_wStringBuffer buffer(stack_buffer, sizeof(stack_buffer) / sizeof(stack_buffer[0]));
  size_t buffer_capacity = buffer.m_buffer_capacity;
  for(;;)
  {
    va_list args_copy;
    va_copy(args_copy, args);

    const int formatted_string_count = vswprintf(buffer.m_buffer, buffer.m_buffer_capacity, format, args_copy);
    va_end(args_copy);
    if (formatted_string_count >= 0)
    {
      // formatted_string_count = number of wchar_t elements not including null terminator
      return formatted_string_count;
    }
    if ( buffer_capacity >= 1024*16*16*16 )
      break;
    buffer_capacity *= 16;
    if (false == buffer.GrowBuffer(buffer_capacity))
      break;
    if (nullptr == buffer.m_buffer)
      break;
    if (buffer_capacity < buffer.m_buffer_capacity)
      break;
  }
  return -1;
#else
  // Using MYON_Locale::Ordinal.NumericLocalePtr() insures that a period 
  // will be use for the decimal point in formatted printing.
  return _vscwprintf_p_l(format, MYON_Locale::Ordinal.NumericLocalePtr(), args);
#endif
#endif
}

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
#include "opennurbs_internal_defines.h"
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#pragma warning (push)
#pragma warning (disable:4127)

MYON__INT64 Integerize(float dirty)
{
  // Use this function when CRCing floats. This eliminates the problem of
  // rounding errors causing different CRCs to be generated for essentially the same value.
  return MYON__INT64(
   (dirty      // 0.499999976 0.500000003 (both essentially 0.5 with float dirt).
    + 1e-6)    // 0.500000976 0.500001003
    * 1e+5)    // 050000.0976 050000.1003
    ;          //  50000      50000       (both will give the same CRC adjustment).
}

MYON__INT64 Integerize(double dirty)
{
  // Use this function when CRCing doubles. This eliminates the problem of
  // rounding errors causing different CRCs to be generated for essentially the same value.
  return MYON__INT64(
   (dirty      // 0.49999999999997 0.50000000000002 (both essentially 0.5 with float dirt).
    + 1e-11)   // 0.50000000000997 0.50000000001002
    * 1e+10)   //  5000000000.0997  5000000000.1002
    ;          //  5000000000       5000000000      (both will give the same CRC adjustment).
}

#ifdef _DEBUG
static void MyOutputDebugString(const wchar_t* s)
{
#ifdef MYON_COMPILER_MSC
  OutputDebugString(s);
#else
  // No Mac implementation.
#endif
}
#define OUTPUT_DEBUG_STRING(x)       MyOutputDebugString(x)
#define OUTPUT_DEBUG_STRING_EOL(x) { MyOutputDebugString(x); MyOutputDebugString(L"\n"); }
#else
#define OUTPUT_DEBUG_STRING(x)     { }
#define OUTPUT_DEBUG_STRING_EOL(x) { }
#endif

#define IMPL_CHECK { MYON_ASSERT(sizeof(m_Impl) >= (sizeof(CImpl) + sizeof(void*))); }

std::atomic<long> g_lNodeCount(0);
std::atomic<long> g_lPropertyCount(-1); // defaultProp below increments this to zero.

#define MYON_NAME           L"name"
#define MYON_PARAMETER      L"parameter"

static const wchar_t* wszBase64Prefix = L"base64:";

#ifdef MYON_COMPILER_MSC
static _locale_t __Locale(void)
{
  static _locale_t loc = nullptr;

  if (nullptr == loc)
    loc = _create_locale(LC_ALL, "C");

  return loc;
}

inline double MYON_wtof(const wchar_t* s) { return _wtof_l(s, __Locale()); }
inline int    MYON_wtoi(const wchar_t* s) { return _wtoi_l(s, __Locale()); }
#else
inline double MYON_wtof(const wchar_t* s) { return wcstod(s, nullptr); }
inline int    MYON_wtoi(const wchar_t* s) { return (int)wcstol(s, nullptr, 10); }
#endif

static const wchar_t* StringFromUnits(MYON::LengthUnitSystem units)
{
  // These strings appear in the XML -- do not change them.

  switch (units)
  {
  case MYON::LengthUnitSystem::Angstroms:         return L"angstroms";
  case MYON::LengthUnitSystem::AstronomicalUnits: return L"astronomical";
  case MYON::LengthUnitSystem::Centimeters:       return L"centimeters";
  case MYON::LengthUnitSystem::Decimeters:        return L"decimeters";
  case MYON::LengthUnitSystem::Dekameters:        return L"dekameters";
  case MYON::LengthUnitSystem::Feet:              return L"feet";
  case MYON::LengthUnitSystem::Gigameters:        return L"gigameters";
  case MYON::LengthUnitSystem::Hectometers:       return L"hectometers";
  case MYON::LengthUnitSystem::Inches:            return L"inches";
  case MYON::LengthUnitSystem::Kilometers:        return L"kilometers";
  case MYON::LengthUnitSystem::LightYears:        return L"lightyears";
  case MYON::LengthUnitSystem::Megameters:        return L"megameters";
  case MYON::LengthUnitSystem::Meters:            return L"meters";
  case MYON::LengthUnitSystem::Microinches:       return L"microinches";
  case MYON::LengthUnitSystem::Microns:           return L"microns";
  case MYON::LengthUnitSystem::Miles:             return L"miles";
  case MYON::LengthUnitSystem::Millimeters:       return L"millimeters";
  case MYON::LengthUnitSystem::Mils:              return L"mils";
  case MYON::LengthUnitSystem::Nanometers:        return L"nanometers";
  case MYON::LengthUnitSystem::NauticalMiles:     return L"nautical-miles";
  case MYON::LengthUnitSystem::Parsecs:           return L"parsecs";
  case MYON::LengthUnitSystem::Yards:             return L"yards";
    default:
        break;
  }

  return L"none";
}

static MYON::LengthUnitSystem UnitsFromString(const MYON_wString& s)
{
  if (L"angstroms" == s)       return MYON::LengthUnitSystem::Angstroms;
  if (L"astronomical" == s)    return MYON::LengthUnitSystem::AstronomicalUnits;
  if (L"centimeters" == s)     return MYON::LengthUnitSystem::Centimeters;
  if (L"decimeters" == s)      return MYON::LengthUnitSystem::Decimeters;
  if (L"dekameters" == s)      return MYON::LengthUnitSystem::Dekameters;
  if (L"feet" == s)            return MYON::LengthUnitSystem::Feet;
  if (L"gigameters" == s)      return MYON::LengthUnitSystem::Gigameters;
  if (L"hectometers" == s)     return MYON::LengthUnitSystem::Hectometers;
  if (L"inches" == s)          return MYON::LengthUnitSystem::Inches;
  if (L"kilometers" == s)      return MYON::LengthUnitSystem::Kilometers;
  if (L"lightyears" == s)      return MYON::LengthUnitSystem::LightYears;
  if (L"megameters" == s)      return MYON::LengthUnitSystem::Megameters;
  if (L"meters" == s)          return MYON::LengthUnitSystem::Meters;
  if (L"microinches" == s)     return MYON::LengthUnitSystem::Microinches;
  if (L"microns" == s)         return MYON::LengthUnitSystem::Microns;
  if (L"miles" == s)           return MYON::LengthUnitSystem::Miles;
  if (L"millimeters" == s)     return MYON::LengthUnitSystem::Millimeters;
  if (L"mils" == s)            return MYON::LengthUnitSystem::Mils;
  if (L"nanometers" == s)      return MYON::LengthUnitSystem::Nanometers;
  if (L"nautical-miles" == s)  return MYON::LengthUnitSystem::NauticalMiles;
  if (L"parsecs" == s)         return MYON::LengthUnitSystem::Parsecs;
  if (L"yards" == s)           return MYON::LengthUnitSystem::Yards;

  return MYON::LengthUnitSystem::None;
}

static void EncodeXML(MYON_wString& s)
{
  // Call this instead of directly calling MYON_wString::EncodeXMLValue().
  // The latter does not encode linefeeds and it also does not encode the string in place.
  // However, MYON_wString::DecodeXMLValue() _does_ decode linefeeds.
  s = s.EncodeXMLValue();
  s.Replace(L"\n", L"&#10;");
}

static void DecodeXML(MYON_wString& s)
{
  // Call this instead of directly calling MYON_wString::DecodeXMLValue().
  // The latter _does_ decode linefeeds but it does not decode the string in place.
  if (s.Find(L'&') >= 0)
  {
    s = s.DecodeXMLValue();
  }
}

static bool GetTimeComponents(const MYON_wString& sTime, int& y, int& m, int& d, int& h, int& n, int& s)
{
  if (19 != sTime.Length())
    return false;

  auto* wsz = static_cast<const wchar_t*>(sTime);

  if ((wsz[4] != L'.') || (wsz[7] != L'.') || (wsz[10] != L'_') || (wsz[13] != L':') || (wsz[16] != L':'))
    return false;

  y = MYON_wtoi(wsz);
  m = MYON_wtoi(wsz + 5);
  d = MYON_wtoi(wsz + 8);
  h = MYON_wtoi(wsz + 11);
  n = MYON_wtoi(wsz + 14);
  s = MYON_wtoi(wsz + 17);

  if ((y < 1900) || (y > 2500))
    return false;

  if ((m < 1) || (m > 12))
    return false;

  if ((d < 1) || (d > 31))
    return false;

  if ((h < 0) || (h > 23))
    return false;

  if ((n < 0) || (n > 59))
    return false;

  if ((s < 0) || (s > 59))
    return false;

  return true;
}

//static bool IsValidTime(const MYON_wString& sTime)
//{
//  int y = 0, m = 0, d = 0, h = 0, n = 0, s = 0;
//  if (!GetTimeComponents(sTime, y, m, d, h, n, s))
//    return false;
//
//  return true;
//}

static MYON_wString TimeToString(time_t time)
{
  tm stm = { 0 };

#ifdef MYON_COMPILER_MSC
  localtime_s(&stm, &time);
#else
  localtime_r(&time, &stm);
#endif

  MYON_wString st;
  st.Format(L"%04u.%02u.%02u_%02u:%02u:%02u", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);

  return st;
}

static time_t TimeFromString(const MYON_wString& sTime)
{
  int y = 0, m = 0, d = 0, h = 0, n = 0, s = 0;
  if (!GetTimeComponents(sTime, y, m, d, h, n, s))
    return -1;

  tm atm = { 0 };
  atm.tm_year  = y - 1900;
  atm.tm_mon   = m - 1;
  atm.tm_mday  = d;
  atm.tm_hour  = h;
  atm.tm_min   = n;
  atm.tm_sec   = s;
  atm.tm_isdst = -1;

  return mktime(&atm);
}

// MYON_XMLVariant

class MYON_XMLVariant::CImpl final
{
public:
  CImpl() { }
  ~CImpl() { ClearBuffers(); }

  MYON_Buffer& GetBuffer(void) const
  {
    if (nullptr == m_pBuffer)
      m_pBuffer = new MYON_Buffer;
    else
      m_pBuffer->SeekFromStart(0);

    return *m_pBuffer;
  }

  void ClearBuffers(void)
  {
    if (nullptr != m_pBuffer)
    {
      delete m_pBuffer;
      m_pBuffer = nullptr;
    }

    if (nullptr != m_raw_buffer)
    {
      delete[] m_raw_buffer;
      m_raw_buffer = nullptr;
    }
  }

  const MYON_wString& ConvertDoubleToString(double d) const
  {
    auto* buf = m_sVal.SetLength(30);
    if (nullptr != buf)
    {
      const auto len = MYON_wString::FormatIntoBuffer(buf, 24, L"%.15g", d);
      m_sVal.SetLength(len);
    }

    return m_sVal;
  }

  const MYON_wString& ConvertDoubleArrayToString(int count) const
  {
    constexpr int maxCount = 16;
    if ((count == 0) || (count > maxCount))
      return m_sVal;

    constexpr int maxLen = 30;

    const auto num_chars = maxLen * maxCount;
    auto* buf = m_sVal.SetLength(num_chars);
    if (nullptr == buf)
      return m_sVal;

    int totalLen = 0;
    auto* p = buf;
    for (int i = 0; i < count; i++)
    {
      const auto len = MYON_wString::FormatIntoBuffer(p, maxLen, L"%.15g", m_aVal[i]);
      p += len;
      *p++ = L',';
      totalLen += len + 1;
    }

    // Real length is totalLen - 1 because final comma is excess.
    const auto length = totalLen - 1;
    if (length >= 0)
    {
      buf[length] = 0;
    }

    m_sVal.SetLength(length);

    return m_sVal;
  }

  mutable MYON_Buffer* m_pBuffer = nullptr;
  mutable MYON_wString m_sVal;
  union
  {
    bool m_bVal;
    int m_iVal;
    float m_fVal;
    double m_dVal;
    double m_aVal[16];
    MYON_Xform m_xform;
    time_t m_timeVal;
    MYON_UUID m_uuidVal;
  };

  MYON::LengthUnitSystem m_units = MYON::LengthUnitSystem::None;
  MYON__UINT8* m_raw_buffer = nullptr;
  bool m_bTypePending = false;
  bool m_bReserved[3] = { false };
  Types m_type = Types::Null;
};

MYON_XMLVariant::MYON_XMLVariant()
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
}

MYON_XMLVariant::MYON_XMLVariant(int iValue)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(iValue);
}

MYON_XMLVariant::MYON_XMLVariant(double dValue)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(dValue);
}

MYON_XMLVariant::MYON_XMLVariant(float fValue)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(fValue);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_wString& s)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(s);
}

MYON_XMLVariant::MYON_XMLVariant(const double* point, ArrayTypes at)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(point, at);
}

MYON_XMLVariant::MYON_XMLVariant(const float* point, ArrayTypes at)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(point, at);
}

MYON_XMLVariant::MYON_XMLVariant(const wchar_t* wsz)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  MYON_wString s(wsz);
  SetValue(s);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_2dPoint& p)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(p);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_3dPoint& p)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(p);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_4fColor& c)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(c);
}

MYON_XMLVariant::MYON_XMLVariant(bool b)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(b);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_UUID& uuid)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(uuid);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_Xform& xform)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(xform);
}

MYON_XMLVariant::MYON_XMLVariant(time_t time)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(time);
}

MYON_XMLVariant::MYON_XMLVariant(const void* pBuffer, size_t size)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(pBuffer, size);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_Buffer& buffer)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  SetValue(buffer);
}

MYON_XMLVariant::MYON_XMLVariant(const MYON_XMLVariant& src)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  *this = src;
}

MYON_XMLVariant::~MYON_XMLVariant()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

MYON_Buffer& MYON_XMLVariant::GetBuffer(void) const
{
  return m_impl->GetBuffer();
}

void MYON_XMLVariant::ClearBuffers(void)
{
  m_impl->ClearBuffers();
}

const MYON_XMLVariant& MYON_XMLVariant::operator = (const MYON_XMLVariant& src)
{
  m_impl->m_type = src.Type();
  m_impl->m_bTypePending = src.TypePending();

  if (m_impl->m_type != Types::Buffer)
  {
    ClearBuffers();
  }

  switch (m_impl->m_type)
  {
  case Types::Null:                                              break;
  case Types::Bool:         m_impl->m_bVal    = src.AsBool();    break;
  case Types::Integer:      m_impl->m_iVal    = src.AsInteger(); break;
  case Types::Float:        m_impl->m_fVal    = src.AsFloat();   break;
  case Types::Double:       m_impl->m_dVal    = src.AsDouble();  break;
  case Types::String:       m_impl->m_sVal    = src.AsString();  break;
  case Types::Uuid:         m_impl->m_uuidVal = src.AsUuid();    break;
  case Types::Time:         m_impl->m_timeVal = src.AsTime();    break;
  case Types::DoubleArray2: SetValue(src.As2dPoint());           break;
  case Types::DoubleArray3: SetValue(src.As3dPoint());           break;
  case Types::DoubleArray4: SetValue(src.As4dPoint());           break;
  case Types::DoubleColor4: SetValue(src.AsColor());             break;
  case Types::Matrix:       SetValue(src.AsXform()) ;            break;
  case Types::Buffer:       GetBuffer() = src.AsBuffer();        break;

  default:
    OUTPUT_DEBUG_STRING(L"MYON_XMLVariant: Source has unknown type\n");
    MYON_ASSERT(false);
  }

  // The above calls can reset the type pending flag on the source.
  // Make sure the source is set back to the original condition.
  src.SetTypePendingFlag(m_impl->m_bTypePending);

  return *this;
}

bool MYON_XMLVariant::operator == (const MYON_XMLVariant& v) const
{
  if (m_impl->m_type != v.m_impl->m_type)
    return false;

  if (m_impl->m_units != v.m_impl->m_units)
    return false;

  switch (m_impl->m_type)
  {
  case Types::Bool:
    return m_impl->m_bVal == v.m_impl->m_bVal;

  case Types::Integer:
    return m_impl->m_iVal == v.m_impl->m_iVal;

  case Types::Float:
    return IsFloatEqual(m_impl->m_fVal, v.m_impl->m_fVal) ? true : false;

  case Types::Double:
    return IsDoubleEqual(m_impl->m_dVal, v.m_impl->m_dVal) ? true : false;

  case Types::DoubleArray2:
    return ((IsDoubleEqual(m_impl->m_aVal[0], v.m_impl->m_aVal[0])) &&
            (IsDoubleEqual(m_impl->m_aVal[1], v.m_impl->m_aVal[1]))) ? true : false;

  case Types::DoubleArray3:
    return ((IsDoubleEqual(m_impl->m_aVal[0], v.m_impl->m_aVal[0])) &&
            (IsDoubleEqual(m_impl->m_aVal[1], v.m_impl->m_aVal[1])) &&
            (IsDoubleEqual(m_impl->m_aVal[2], v.m_impl->m_aVal[2]))) ? true : false;

  case Types::DoubleColor4:
  case Types::DoubleArray4:
    return ((IsDoubleEqual(m_impl->m_aVal[0], v.m_impl->m_aVal[0])) &&
            (IsDoubleEqual(m_impl->m_aVal[1], v.m_impl->m_aVal[1])) &&
            (IsDoubleEqual(m_impl->m_aVal[2], v.m_impl->m_aVal[2])) &&
            (IsDoubleEqual(m_impl->m_aVal[3], v.m_impl->m_aVal[3]))) ? true : false;

  case Types::String:
    return m_impl->m_sVal.CompareNoCase(v.m_impl->m_sVal) == 0;

  case Types::Uuid:
    return (m_impl->m_uuidVal == v.m_impl->m_uuidVal) ? true : false;

  case Types::Time:
    return m_impl->m_timeVal == v.m_impl->m_timeVal;

  case Types::Matrix:
      for (int i = 0; i < 16; i++)
      {
        if (m_impl->m_aVal[i] != v.m_impl->m_aVal[i])
          return false;
      }
      return true;

  default:
    MYON_ASSERT(false);
  }

  return false;
}

bool MYON_XMLVariant::operator != (const MYON_XMLVariant& v) const
{
  return !(operator == (v));
}

bool MYON_XMLVariant::NeedsXMLEncode(void) const
{
  switch (m_impl->m_type)
  {
  case Types::Null:
  case Types::Bool:
  case Types::Integer:
  case Types::Float:
  case Types::Double:
  case Types::DoubleArray2:
  case Types::DoubleArray3:
  case Types::DoubleArray4:
  case Types::DoubleColor4:
  case Types::Uuid:
  case Types::Matrix:
  case Types::Buffer:
    return false;

  default:
    {
    const auto s = AsString();
    if (s.StartsWithNoCase(wszBase64Prefix))
      return false;

    return s.NeedsXMLEncode();
    }
  }
}

void MYON_XMLVariant::SetNull()
{
  m_impl->m_type = Types::Null;
}

void MYON_XMLVariant::SetValue(int v)
{
  ClearBuffers();
  m_impl->m_type = Types::Integer;
  m_impl->m_iVal = v;
  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(double v)
{
  ClearBuffers();
  m_impl->m_type = Types::Double;
  m_impl->m_dVal = v;
  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(float v)
{
  ClearBuffers();
  m_impl->m_type = Types::Float;
  m_impl->m_fVal = v;
  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const wchar_t* s)
{
  ClearBuffers();
  m_impl->m_type = Types::String;
  m_impl->m_sVal = s;
  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const MYON_wString& s)
{
  ClearBuffers();
  m_impl->m_type = Types::String;
  m_impl->m_sVal = s;
  m_impl->m_bTypePending = false;
}

inline static void Fill(double* a, const double* b, int count)
{
  for (int i = 0; i < count; i++)
  {
    a[i] = b[i];
  }
}

inline static void Fill(double* a, const float* b, int count)
{
  for (int i = 0; i < count; i++)
  {
    a[i] = double(b[i]);
  }
}

void MYON_XMLVariant::SetValue(const double* p, ArrayTypes at)
{
  ClearBuffers();

  switch (at)
  {
  case ArrayTypes::Array2:  m_impl->m_type = Types::DoubleArray2; Fill(m_impl->m_aVal, p, 2);  break;
  case ArrayTypes::Array3:  m_impl->m_type = Types::DoubleArray3; Fill(m_impl->m_aVal, p, 3);  break;
  case ArrayTypes::Array4:  m_impl->m_type = Types::DoubleArray4; Fill(m_impl->m_aVal, p, 4);  break;
  case ArrayTypes::Array16: m_impl->m_type = Types::Matrix;       Fill(m_impl->m_aVal, p, 16); break;
  }

  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const float* p, ArrayTypes at)
{
  ClearBuffers();

  switch (at)
  {
  case ArrayTypes::Array2:  m_impl->m_type = Types::DoubleArray2; Fill(m_impl->m_aVal, p, 2);  break;
  case ArrayTypes::Array3:  m_impl->m_type = Types::DoubleArray3; Fill(m_impl->m_aVal, p, 3);  break;
  case ArrayTypes::Array4:  m_impl->m_type = Types::DoubleArray4; Fill(m_impl->m_aVal, p, 4);  break;
  case ArrayTypes::Array16: m_impl->m_type = Types::Matrix;       Fill(m_impl->m_aVal, p, 16); break;
  }

  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const MYON_2dPoint& v)
{
  ClearBuffers();

  m_impl->m_type = Types::DoubleArray2;

  m_impl->m_aVal[0] = v.x;
  m_impl->m_aVal[1] = v.y;

  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const MYON_3dPoint& v)
{
  ClearBuffers();

  m_impl->m_type = Types::DoubleArray3;

  m_impl->m_aVal[0] = v.x;
  m_impl->m_aVal[1] = v.y;
  m_impl->m_aVal[2] = v.z;

  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const MYON_4dPoint& p)
{
  ClearBuffers();

  m_impl->m_type = Types::DoubleArray4;

  m_impl->m_aVal[0] = p.x;
  m_impl->m_aVal[1] = p.y;
  m_impl->m_aVal[2] = p.z;
  m_impl->m_aVal[3] = p.w;

  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const MYON_4fColor& c)
{
  ClearBuffers();

  m_impl->m_type = Types::DoubleColor4;

  m_impl->m_aVal[0] = c.Red();
  m_impl->m_aVal[1] = c.Green();
  m_impl->m_aVal[2] = c.Blue();
  m_impl->m_aVal[3] = c.Alpha();

  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(bool b)
{
  ClearBuffers();

  m_impl->m_type = Types::Bool;
  m_impl->m_bVal = b;
  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const MYON_UUID& uuid)
{
  ClearBuffers();

  m_impl->m_type = Types::Uuid;
  m_impl->m_uuidVal = uuid;
  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(time_t time)
{
  ClearBuffers();

  m_impl->m_type = Types::Time;
  m_impl->m_timeVal = time;
  m_impl->m_bTypePending = false;
}

void MYON_XMLVariant::SetValue(const void* pBuffer, size_t size)
{
  ClearBuffers();

  m_impl->m_type = Types::Buffer;
  m_impl->m_bTypePending = false;

  GetBuffer().Write(size, pBuffer);
}

void MYON_XMLVariant::SetValue(const MYON_Buffer& buffer)
{
  m_impl->m_type = Types::Buffer;
  m_impl->m_bTypePending = false;

  GetBuffer() = buffer;
}

void MYON_XMLVariant::SetValue(const MYON_Xform& xform)
{
  ClearBuffers();

  m_impl->m_type = Types::Matrix;
  m_impl->m_xform = xform;
  m_impl->m_bTypePending = false;
}

bool MYON_XMLVariant::AsBool(void) const
{
  DoAutoTyping(Types::Bool);

  switch (m_impl->m_type)
  {
  case Types::Bool:    return m_impl->m_bVal;
  case Types::Integer: return m_impl->m_iVal != 0;
  case Types::Double:  return m_impl->m_dVal != 0;
  case Types::Float:   return m_impl->m_fVal != 0;

  case Types::String:
    if (m_impl->m_sVal.CompareNoCase(L"true") == 0) return true;
    if (m_impl->m_sVal.CompareNoCase(L"t")    == 0) return true;
    return MYON_wtoi(m_impl->m_sVal) != 0;
          
  default:
    return false;
  }
}

int MYON_XMLVariant::AsInteger(void) const
{
  DoAutoTyping(Types::Integer);

  switch (m_impl->m_type)
  {
  case Types::Bool:    return     m_impl->m_bVal ? 1 : 0;
  case Types::Integer: return     m_impl->m_iVal;
  case Types::Double:  return int(m_impl->m_dVal);
  case Types::Float:   return int(m_impl->m_fVal);

  case Types::String:
    if (m_impl->m_sVal.CompareNoCase(L"true") == 0) return 1;
    if (m_impl->m_sVal.CompareNoCase(L"t") == 0) return true;
    return MYON_wtoi(m_impl->m_sVal);

  default:
    return 0;
  }
}

double MYON_XMLVariant::AsDouble(void) const
{
  DoAutoTyping(Types::Double);

  switch (m_impl->m_type)
  {
  case Types::Bool:    return         m_impl->m_bVal ? 1.0 : 0.0;
  case Types::Float:   return         m_impl->m_fVal;
  case Types::Double:  return         m_impl->m_dVal;
  case Types::Integer: return double (m_impl->m_iVal);
  case Types::String:  return MYON_wtof(m_impl->m_sVal);
  
  default:
    return 0.0;
  }
}

float MYON_XMLVariant::AsFloat(void) const
{
  DoAutoTyping(Types::Float);

  switch (m_impl->m_type)
  {
  case Types::Bool:    return       m_impl->m_bVal ? 1.0f : 0.0f;
  case Types::Float:   return       m_impl->m_fVal;
  case Types::Double:  return float(m_impl->m_dVal);
  case Types::Integer: return float(m_impl->m_iVal);
  case Types::String:  return float(MYON_wtof(m_impl->m_sVal));
          
  default:
    return 0.0f;
  }
}

MYON_2dPoint MYON_XMLVariant::As2dPoint(void) const
{
  switch (m_impl->m_type)
  {
  case Types::DoubleArray2:
  case Types::DoubleArray3:
  case Types::DoubleArray4:
  case Types::DoubleColor4:
    break;

  case Types::String:
    if (m_impl->m_sVal.IsValid2dPoint())
      StringToPoint(2);
      break;

  default:
    return MYON_2dPoint::Origin;
  }

  return MYON_2dPoint(m_impl->m_aVal[0], m_impl->m_aVal[1]);
}

MYON_3dPoint MYON_XMLVariant::As3dPoint(void) const
{
  switch (m_impl->m_type)
  {
  case Types::DoubleArray2:
    m_impl->m_aVal[2] = 0.0;
    break;

  case Types::DoubleArray3:
  case Types::DoubleArray4:
  case Types::DoubleColor4:
    break;

  case Types::String:
    if (m_impl->m_sVal.IsValid3dPoint())
      StringToPoint(3);
    break;

  default:
    return MYON_3dPoint::Origin;
  }

  return MYON_3dPoint(m_impl->m_aVal[0], m_impl->m_aVal[1], m_impl->m_aVal[2]);
}

MYON_4dPoint MYON_XMLVariant::As4dPoint(void) const
{
  switch (m_impl->m_type)
  {
  case Types::DoubleArray2:
    m_impl->m_aVal[2] = 0.0;
  case Types::DoubleArray3:
    m_impl->m_aVal[3] = 0.0;
    break;

  case Types::DoubleArray4:
  case Types::DoubleColor4:
    break;

  case Types::String:
    if (m_impl->m_sVal.IsValid4dPoint())
      StringToPoint(4);
    break;

  default:
    return MYON_4dPoint::Zero;
  }

  return MYON_4dPoint(m_impl->m_aVal[0], m_impl->m_aVal[1], m_impl->m_aVal[2], m_impl->m_aVal[3]);
}

MYON_Xform MYON_XMLVariant::AsXform(void) const
{
  switch (m_impl->m_type)
  {
  case Types::Matrix:
    break;

  case Types::String:
    if (m_impl->m_sVal.IsValidMatrix())
      StringToPoint(16); //////////////////////////////////// Risky
    break;

  default:
    return MYON_Xform::Zero4x4;
  }

  return m_impl->m_xform;
}

MYON_4fColor MYON_XMLVariant::AsColor(void) const
{
  DoAutoTyping(Types::DoubleColor4);

  MYON_4fColor col(MYON_Color(0, 0, 0, 0));

  switch (m_impl->m_type)
  {
  case Types::String:
    StringToPoint(4);
    // No break...
  case Types::DoubleArray4:
  case Types::DoubleColor4:
    col.SetRGBA(float(m_impl->m_aVal[0]), float(m_impl->m_aVal[1]), float(m_impl->m_aVal[2]), float(m_impl->m_aVal[3]));
  
  default:
    break;
  }

  return col;
}

MYON_UUID MYON_XMLVariant::AsUuid(void) const
{
  DoAutoTyping(Types::Uuid);

  switch (m_impl->m_type)
  {
  case Types::String:
    return MYON_UuidFromString(m_impl->m_sVal);

  case Types::Uuid:
    return m_impl->m_uuidVal;

  default:
    return MYON_nil_uuid;
  }
}

time_t MYON_XMLVariant::AsTime(void) const
{
  DoAutoTyping(Types::Time);

  switch (m_impl->m_type)
  {
  case Types::String:
    return TimeFromString(m_impl->m_sVal);

  case Types::Time:
    return m_impl->m_timeVal;
          
  default: return 0;
  }
}

MYON_Buffer MYON_XMLVariant::AsBuffer(void) const
{
  DoAutoTyping(Types::Buffer);

  MYON_Buffer buf;
#if defined(MYON_RUNTIME_APPLE)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif
  switch (m_impl->m_type)
  {
  case Types::Buffer:
    buf = GetBuffer();
    break;

  case Types::String:
    auto& s = m_impl->m_sVal;
    if (s.StartsWithNoCase(wszBase64Prefix) && (s != wszBase64Prefix))
    {
      // Base64 is about 33% bigger than the resulting data so the string length is always enough.
      const auto size = size_t(s.Length());
      auto* p = new MYON__UINT8[size];
      const auto prefix_len = MYON_wString(wszBase64Prefix).Length();
      const auto decoded = MYON_Base64::Decode(s.Mid(prefix_len), p);
      buf.Write(decoded, p);
      delete[] p;
    }
    break;
  }
#if defined(MYON_RUNTIME_APPLE)
#pragma clang diagnostic pop
#endif
  return buf;
}

void* MYON_XMLVariant::AsBuffer(size_t& size_out) const
{
  auto buf = AsBuffer();
  size_out = buf.Size();

  if (nullptr != m_impl->m_raw_buffer)
    delete[] m_impl->m_raw_buffer;

  m_impl->m_raw_buffer = new MYON__UINT8[size_out];
  buf.Read(size_out, m_impl->m_raw_buffer);

  return m_impl->m_raw_buffer;
}

MYON_wString MYON_XMLVariant::AsString(void) const
{
  DoAutoTyping(Types::String);

  switch (m_impl->m_type)
  {
  case Types::Integer:
    return m_impl->ConvertDoubleToString(double(m_impl->m_iVal));

  case Types::Float:
    return m_impl->ConvertDoubleToString(double(m_impl->m_fVal));

  case Types::Double:
    return m_impl->ConvertDoubleToString(m_impl->m_dVal);

  case Types::DoubleArray3:
    return m_impl->ConvertDoubleArrayToString(3);

  case Types::DoubleArray4:
  case Types::DoubleColor4:
    return m_impl->ConvertDoubleArrayToString(4);

  case Types::DoubleArray2:
    return m_impl->ConvertDoubleArrayToString(2);

  case Types::Matrix:
    return m_impl->ConvertDoubleArrayToString(16);

  case Types::Bool:
    m_impl->m_sVal = m_impl->m_bVal ? L"true" : L"false";
    // No break...
  case Types::String:
    return m_impl->m_sVal;

  case Types::Uuid:
    MYON_UuidToString(m_impl->m_uuidVal, m_impl->m_sVal);
    m_impl->m_sVal.MakeUpperOrdinal();
    return m_impl->m_sVal;

  case Types::Time:
    m_impl->m_sVal = TimeToString(m_impl->m_timeVal);
    return m_impl->m_sVal;

  case Types::Buffer:
    {
      m_impl->m_sVal = wszBase64Prefix;
      auto& buffer = GetBuffer();
      const auto buf_size = buffer.Size();
      auto* buf = new char[buf_size];
      buffer.Read(buf_size, buf);
      MYON_Base64::Encode(buf, buf_size, m_impl->m_sVal, true);
      delete[] buf;
      return m_impl->m_sVal;
    }
  
  case Types::Null:
  default:
    break;
  }

  return L"";
}

MYON_XMLVariant::operator double() const
{
  return AsDouble();
}

MYON_XMLVariant::operator float() const
{
  return AsFloat();
}

MYON_XMLVariant::operator int() const
{
  return AsInteger();
}

MYON_XMLVariant::operator MYON_wString() const
{
  return AsString();
}

MYON_XMLVariant::operator bool() const
{
  return AsBool();
}

MYON_XMLVariant::operator MYON_UUID() const
{
  return AsUuid();
}

MYON_XMLVariant::operator MYON_2dPoint() const
{
  return As2dPoint();
}

MYON_XMLVariant::operator MYON_3dPoint() const
{
  return As3dPoint();
}

MYON_XMLVariant::operator MYON_4dPoint() const
{
  return As4dPoint();
}

MYON_XMLVariant::operator MYON_4fColor() const
{
  return AsColor();
}

MYON_XMLVariant::operator time_t() const
{
  return AsTime();
}

MYON_XMLVariant::operator MYON_Buffer() const
{
  return AsBuffer();
}

void MYON_XMLVariant::StringToPoint(int numValues) const
{
  if ((numValues < 0) || (numValues > 16))
    return;

  MYON_wString s = m_impl->m_sVal + L",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,";
  auto* p = s.Array();

  for (int i = 0; i < numValues; i++)
  {
    while (iswspace(*p))
    {
      p++;
    }

    m_impl->m_aVal[i] = MYON_wtof(p);

    while (*p != L',')
    {
      p++;
    }

    p++;
  }
}

MYON_XMLVariant::Types MYON_XMLVariant::Type(void) const
{
  return m_impl->m_type;
}

bool MYON_XMLVariant::IsNull(void) const
{
  return Types::Null == m_impl->m_type;
}

bool MYON_XMLVariant::IsEmpty(void) const
{
  return AsString().IsEmpty() ? true : false;
}

MYON::LengthUnitSystem MYON_XMLVariant::Units(void) const
{
  return m_impl->m_units;
}

void MYON_XMLVariant::SetUnits(MYON::LengthUnitSystem units)
{
  m_impl->m_units = units;
}

bool MYON_XMLVariant::TypePending(void) const
{
  return m_impl->m_bTypePending;
}

void MYON_XMLVariant::SetTypePendingFlag(bool bTypePending) const
{
  m_impl->m_bTypePending = bTypePending;
}

MYON_wString MYON_XMLVariant::TypeAsString(void) const
{
  switch (m_impl->m_type)
  {
  case Types::Bool:         return L"bool";
  case Types::Integer:      return L"integer";
  case Types::Float:        return L"float";
  case Types::Double:       return L"double";
  case Types::DoubleArray2: return L"2-double-array";
  case Types::DoubleArray3: return L"3-double-array";
  case Types::DoubleArray4: return L"4-double-array";
  case Types::DoubleColor4: return L"color";
  case Types::Matrix:       return L"matrix-4x4";
  case Types::Uuid:         return L"uuid";
  case Types::String:       return L"string";
  case Types::Time:         return L"time";
  case Types::Buffer:       return L"buffer";
  case Types::Null:
  default:
    return L"null";
  }
}

template <typename T>
static MYON__UINT32 CRCReal(MYON__UINT32 crc, T v)
{
  const auto x = Integerize(v);
  return MYON_CRC32(crc, sizeof(x), &x);
}

MYON__UINT32 MYON_XMLVariant::DataCRC(MYON__UINT32 crc) const
{
  switch (Type())
  {
  default:
    return crc;

  case Types::Double:
    return CRCReal(crc, m_impl->m_dVal);

  case Types::Float:
    return CRCReal(crc, m_impl->m_fVal);

  case Types::Integer:
  {
    const auto x = m_impl->m_iVal;
    return MYON_CRC32(crc, sizeof(x), &x);
  }

  case Types::DoubleArray2:
  {
    MYON__INT64 x[2] = { 0 };
    for (int i = 0; i < 2; i++) x[i] = Integerize(m_impl->m_aVal[i]);
    return MYON_CRC32(crc, sizeof(x), &x);
  }

  case Types::DoubleArray3:
  {
    MYON__INT64 x[3] = { 0 };
    for (int i = 0; i < 3; i++) x[i] = Integerize(m_impl->m_aVal[i]);
    return MYON_CRC32(crc, sizeof(x), &x);
  }

  case Types::DoubleArray4:
  case Types::DoubleColor4:
  {
    MYON__INT64 x[4] = { 0 };
    for (int i = 0; i < 4; i++) x[i] = Integerize(m_impl->m_aVal[i]);
    return MYON_CRC32(crc, sizeof(x), &x);
  }

  case Types::Matrix:
  {
    int index = 0;
    MYON__INT64 x[16] = { 0 };
    for (int j = 0; j < 4; j++)
      for (int i = 0; i < 4; i++)
        x[index++] = Integerize(m_impl->m_xform.m_xform[i][j]);
    return MYON_CRC32(crc, sizeof(x), &x);
  }

  case Types::Bool:
  {
    const auto x = m_impl->m_bVal;
    return MYON_CRC32(crc, sizeof(x), &x);
  }

  case Types::Uuid:
  {
    const auto u = m_impl->m_uuidVal;
    return MYON_CRC32(crc, sizeof(u), &u);
  }

  case Types::Time:
  {
    const auto t = m_impl->m_timeVal;
    return MYON_CRC32(crc, sizeof(t), &t);
  }

  case Types::Buffer:
  {
    return AsBuffer().CRC32(crc);
  }

  case Types::String:
    return AsString().DataCRCLower(crc);
  }
}

void MYON_XMLVariant::Format(MYON_wString& sOut) const
{
  MYON_wString sType;

  switch (m_impl->m_type)
  {
  case Types::Null:
    sOut = "[null]";
    return;

  case Types::Bool:
    sType = L"Bool";
    break;

  case Types::Float:
    sType = L"Float";
    break;

  case Types::Double:
    sType = L"Double";
    break;

  case Types::Integer:
    sType = L"Integer";
    break;

  case Types::DoubleColor4:
    sType = L"DoubleColor4";
    break;

  case Types::DoubleArray2:
    sType = L"DoubleArray2";
    break;

  case Types::DoubleArray3:
    sType = L"DoubleArray3";
    break;

  case Types::DoubleArray4:
    sType = L"DoubleArray4";
    break;

  case Types::String:
    sType = L"String";
    break;

  case Types::Uuid:
    sType = L"UUID";
    break;

  case Types::Matrix:
    sType = L"Matrix";
    break;

  case Types::Time:
    sType = L"Time";
    break;

  case Types::Buffer:
    sType = L"Buffer";
    break;
  }

  sOut.Format(L"Type=%s, Value=%s, Units=%s, CRC=%08X",
              sType.Array(), AsString().Array(), StringFromUnits(Units()), DataCRC(0));
}

#if 1
void AutoTypeVariant(MYON_XMLVariant&) { }
void MYON_XMLVariant::DoAutoTyping(Types) const { }
#else
void MYON_XMLVariant::DoAutoTyping(Types type) const
{
  // Only do automatic typing if the flag is set.
  if (!m_impl->m_bTypePending)
    return;

  m_impl->m_bTypePending = false; // Must be first.

  // This is only meant to work if the value is a string - i.e, it has just been read out of the XML stream.
  if (m_impl->m_type != Types::String)
    return;

  // This requires a pretty big const hack - since we are changing the variant
  // pretty dramatically, but we aren't actually changing its value.

  auto* pThis = const_cast<MYON_XMLVariant*>(this);
  switch (type)
  {
  case Types::String:                                                          break;
  case Types::Bool:         pThis->SetValue(AsBool());                         break;
  case Types::Integer:      pThis->SetValue(AsInteger());                      break;
  case Types::Float:        pThis->SetValue(AsFloat());                        break;
  case Types::Double:       pThis->SetValue(AsDouble());                       break;
  case Types::Uuid:         pThis->SetValue(AsUuid());                         break;
  case Types::Time:         pThis->SetValue(AsTime());                         break;
  case Types::Buffer:       pThis->SetValue(AsBuffer());                       break;
  case Types::DoubleColor4: pThis->SetValue(AsColor());                        break;
  case Types::DoubleArray2: pThis->SetValue(As2dPoint(), ArrayTypes::Array2);  break;
  case Types::DoubleArray3: pThis->SetValue(As3dPoint(), ArrayTypes::Array3);  break;
  case Types::DoubleArray4: pThis->SetValue(As4dPoint(), ArrayTypes::Array4);  break;
  case Types::Matrix:       pThis->SetValue(AsXform(),  ArrayTypes::Array16); break;
  }

  // Otherwise we assume the conversion is not supported and just go on with life.
}

void AutoTypeVariant(MYON_XMLVariant& v)
{
  // Used by the XML reader to try to invent sensible types for variants read in from the stream.
  if (v.Type() != MYON_XMLVariant::Types::String)
    return;  // The variant already has a type.

  MYON_wString s = v.AsString();
  v.SetTypePendingFlag(true);

  if ((s == L"true") || (s == L"false"))
  {
    v.AsBool();
  }
  else
  if (s.StartsWithNoCase(wszBase64Prefix))
  {
    v.AsBuffer();
  }
  else
  if (IsValidTime(s))
  {
    v.AsTime();
  }
  else
  if (s.IsValidIntegerNumber())
  {
    v.AsInteger();
  }
  else
  if (s.IsValidMatrix())
  {
    v.AsMatrix();
  }
  else
  if (s.IsValid4dPoint())
  {
    v.As4dPoint();
  }
  else
  if (s.IsValid3dPoint())
  {
    v.As3dPoint();
  }
  else
  if (s.IsValid2dPoint())
  {
    v.As2dPoint();
  }
  else
  if (s.IsValidRealNumber())
  {
    v.AsDouble();
  }
  else
  {
    const auto uuid = MYON_UuidFromString(s);
    if (MYON_UuidIsNotNil(uuid))
    {
      v.AsUuid();
    }
  }
}
#endif

// MYON_XMLProperty

class CPropertyData final
{
public:
  CPropertyData() { g_lPropertyCount++; }
  CPropertyData(const CPropertyData& src) { g_lPropertyCount++; m_sName = src.m_sName; m_value = src.m_value; }
  CPropertyData(const MYON_XMLVariant& v)   { g_lPropertyCount++; m_value = v; }
  CPropertyData(const MYON_wString& sName, const MYON_XMLVariant& v) { g_lPropertyCount++; SetName(sName); SetValue(v); }
  ~CPropertyData() { MYON_ASSERT(m_iRefCount == 0); g_lPropertyCount--; }

public:
  void AddRef(void)
  {
    std::lock_guard<std::recursive_mutex> lg(m_mutex);
    m_iRefCount++;
  }

  void Release(void)
  {
    bool del = false;
    {
      std::lock_guard<std::recursive_mutex> lg(m_mutex);
      del = (0 == --m_iRefCount);
    }

    if (del)
      delete this;
  }

  const MYON_wString& Name(void) const { return m_sName; }
  void SetName(const wchar_t* name) { m_sName = name; m_sName.TrimLeft(); m_sName.TrimRight(); }

  MYON__UINT32 DataCRC(MYON__UINT32 crc) const { return m_value.DataCRC(m_sName.DataCRC(crc)); }

  bool operator < (const CPropertyData& pd) const { return m_sName < pd.m_sName; }

  bool IsDefaultProperty(void) const { return m_sName.IsEmpty(); }

  const MYON_XMLVariant& GetValue(void) const { return m_value; }
  void SetValue(const MYON_XMLVariant& value) { m_value = value; }

  void SetHugeStringValue(const MYON_wString& s);

public:
  MYON_wString m_sName;
  MYON_XMLVariant m_value;
  int m_iRefCount = 1;
  std::recursive_mutex m_mutex;

protected:
  const CPropertyData& operator = (const CPropertyData& d) { m_sName = d.m_sName; m_value = d.m_value; return *this; }
};

void CPropertyData::SetHugeStringValue(const MYON_wString& s)
{
  // Because MYON_wString is reference-counted, there is no longer any need to worry about huge string copies.
  m_value.SetValue(s);
}

class MYON_XMLProperty::CImpl final
{
public:
  CImpl() { m_data = new CPropertyData; }
  CImpl(const MYON_XMLVariant& sValue) { m_data = new CPropertyData(sValue); }
  CImpl(const MYON_wString& sName, const MYON_XMLVariant& value) { m_data = new CPropertyData(sName, value); }
  CImpl(const MYON_XMLProperty& prop) { prop.Impl().m_data->AddRef(); m_data = prop.Impl().m_data; }
  ~CImpl() { m_data->Release(); }

  const MYON_wString& Name(void) const { return m_data->Name(); }
  void SetName(const wchar_t* wsz) { CopyOnWrite(); m_data->SetName(wsz); }

  MYON__UINT32 DataCRC(MYON__UINT32 crc) const { return m_data->DataCRC(crc); }

  bool operator < (const MYON_XMLProperty& prop) const { return m_data->operator < (*prop.Impl().m_data); }

  void operator = (const MYON_XMLProperty::CImpl& pimpl)
  {
    if (nullptr != pimpl.m_data)
    {
      pimpl.m_data->AddRef();
    }

    if (nullptr != m_data)
    {
      m_data->Release();
    }

    m_data = pimpl.m_data;
  }

  bool IsDefaultProperty(void) const { return m_data->IsDefaultProperty(); }
  const MYON_XMLVariant& GetValue(void) const { return m_data->GetValue(); }
  MYON_XMLVariant& GetNonConstValue(void) { CopyOnWrite(); return m_data->m_value; }
  void SetValue(const MYON_XMLVariant& value) { CopyOnWrite(); m_data->SetValue(value); }
  void SetHugeStringValue(const MYON_wString& s) { CopyOnWrite(); m_data->SetHugeStringValue(s); }

  void CopyOnWrite(void)
  {
    if (m_data->m_iRefCount > 1)
    {
      auto* pData = new CPropertyData(*m_data);
      m_data->Release();
      m_data = pData;
    }
  }

  CPropertyData* m_data = nullptr;
  MYON_XMLNode* m_owner = nullptr;
  MYON_XMLProperty* m_pNext = nullptr;
};

MYON_XMLProperty::MYON_XMLProperty()
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
}

MYON_XMLProperty::MYON_XMLProperty(const MYON_XMLVariant& sValue)
{
  m_impl = new (m_Impl) CImpl(sValue); IMPL_CHECK;
}

MYON_XMLProperty::MYON_XMLProperty(const MYON_wString& sName, const MYON_XMLVariant& value)
{
  m_impl = new (m_Impl) CImpl(sName, value); IMPL_CHECK;
}

MYON_XMLProperty::MYON_XMLProperty(const MYON_XMLProperty& prop)
{
  m_impl = new (m_Impl) CImpl(prop); IMPL_CHECK;
}

MYON_XMLProperty::~MYON_XMLProperty()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

const MYON_wString& MYON_XMLProperty::Name(void) const
{
  return m_impl->Name();
}

void MYON_XMLProperty::SetName(const wchar_t* wsz)
{
  m_impl->SetName(wsz);
}

MYON__UINT32 MYON_XMLProperty::DataCRC(MYON__UINT32 crc) const
{
  return m_impl->DataCRC(crc);
}

bool MYON_XMLProperty::operator < (const MYON_XMLProperty& prop) const
{
  return m_impl->operator < (prop);
}

const MYON_XMLProperty& MYON_XMLProperty::operator = (const MYON_XMLProperty& prop)
{
  m_impl->operator = (prop.Impl());
  return *this;
}

bool MYON_XMLProperty::IsDefaultProperty(void) const
{
  return m_impl->IsDefaultProperty();
}

const MYON_XMLVariant& MYON_XMLProperty::GetValue(void) const
{
  return m_impl->GetValue();
}

void MYON_XMLProperty::SetValue(const MYON_XMLVariant& v)
{
  m_impl->SetValue(v);
}

void MYON_XMLProperty::SetHugeStringValue(const MYON_wString& s)
{
  m_impl->SetHugeStringValue(s);
}

MYON_XMLProperty::CImpl& MYON_XMLProperty::Impl(void) const
{
  return *m_impl;
}

MYON_XMLVariant& MYON_XMLProperty::GetNonConstValue(void)
{
  return m_impl->GetNonConstValue();
}

MYON_XMLProperty* MYON_XMLProperty::Next(void) const
{
  return m_impl->m_pNext;
}

// MYON_XMLSegmentedStream

class MYON_XMLSegmentedStream::CImpl
{
public:
  MYON_SimpleArray<wchar_t*> m_a;
};

MYON_XMLSegmentedStream::MYON_XMLSegmentedStream()
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
}

MYON_XMLSegmentedStream::~MYON_XMLSegmentedStream()
{
  for (int i = 0; i < m_impl->m_a.Count(); i++)
  {
    delete[] m_impl->m_a[i];
  }

  m_impl->~CImpl();
  m_impl = nullptr;
}

int MYON_XMLSegmentedStream::Count(void) const
{
  return m_impl->m_a.Count();
}

void MYON_XMLSegmentedStream::Append(wchar_t* wsz)
{
  m_impl->m_a.Append(wsz);
}

wchar_t* MYON_XMLSegmentedStream::Segment(int index) const
{
  if ((index < 0) || (index >= m_impl->m_a.Count()))
    return nullptr;

  return m_impl->m_a[index];
}

void* MYON_XMLSegmentedStream::EVF(const wchar_t*, void*)
{
  return nullptr;
}

// MYON_XMLNode

static int g_iWarningsFlagCounter = 0;

class CWarningHelper final
{
public:
  CWarningHelper(bool b) : m_bWarningsAsErrors(b) { if (m_bWarningsAsErrors) g_iWarningsFlagCounter++; }
  ~CWarningHelper()                               { if (m_bWarningsAsErrors) g_iWarningsFlagCounter--; }

private:
  bool m_bWarningsAsErrors;
};

class MYON_XMLNode::CImpl final
{
public:
  CImpl(MYON_XMLNode& n) : m_node(n) { g_lNodeCount++; }

  ~CImpl()
  {
    RemoveAllProperties();
    RemoveAllChildren();
    --g_lNodeCount;
  }

  bool GetPropertiesFromTag(const MYON_wString& sTag);
  bool IsClosingTag(const MYON_wString& sTag) const;
  MYON_XMLNode* GetNodeAtPath(const wchar_t* wszPath, bool bCreate);
  MYON_XMLProperty* AddProperty(const MYON_XMLProperty& prop);
  void AddEmptyDefaultProperty(void);
  MYON_XMLProperty* AttachProperty(MYON_XMLProperty* pProp);
  bool RemoveProperty(const wchar_t* name);
  MYON_XMLNode* DetachChild(MYON_XMLNode& child);
  void RemoveAllProperties(void);
  const MYON_XMLNode& TopmostParent(void) const;
  MYON_XMLNode* AttachChildNode(MYON_XMLNode* pNode);
  void RemoveAllChildren(void);
  const MYON_wString& TagName(void) const;
  void SetTagName(const wchar_t* name);
  MYON_XMLNode* PrevSibling(void) const;
  void MoveBefore(MYON_XMLNode& other);
  void MoveAfter(MYON_XMLNode& other);
  bool RecurseChildren(MYON_XMLRecurseChildrenCallback callback, void* pv) const;
  MYON__UINT32 DataCRC(MYON__UINT32 crc, int depth) const;

  static bool AssertValidTag(const MYON_wString& sTag);
  static void AttemptToFixTag(MYON_wString& tag);
  static bool GetNextTag(MYON_wString& tag, wchar_t*& pBuffer, bool bValidateTag);
  static bool RecoverProperty(const MYON_wString& tag, int iEqualSign, MYON_wString& sProp);

  static bool m_bAutoTypePropValue;

  MYON_XMLNode& m_node;
  MYON_wString  m_name;
  MYON_XMLNode* m_next_sibling = nullptr;
  MYON_XMLNode* m_parent = nullptr;
  MYON_XMLNode* m_first_child = nullptr;
  MYON_XMLNode* m_last_child = nullptr;
  MYON_XMLProperty* m_first_property = nullptr;
  void* m_last_read_buf_ptr = nullptr;
  mutable std::recursive_mutex m_mutex;
  bool m_debug_auto_test_read = true;
};

bool MYON_XMLNode::CImpl::m_bAutoTypePropValue = false;

bool MYON_XMLNode::AutoTypePropValue(void)
{
  return CImpl::m_bAutoTypePropValue;
}

void MYON_XMLNode::SetAutoTypePropValue(bool b)
{
  CImpl::m_bAutoTypePropValue = b;
}

static const wchar_t* StringFromPropType(MYON_XMLVariant::Types vt)
{
  switch (vt)
  {
  case MYON_XMLVariant::Types::Integer:      return L"int";
  case MYON_XMLVariant::Types::Float:        return L"float";
  case MYON_XMLVariant::Types::Double:       return L"double";
  case MYON_XMLVariant::Types::String:       return L"string";
  case MYON_XMLVariant::Types::Bool:         return L"bool";
  case MYON_XMLVariant::Types::Matrix:       return L"matrix";
  case MYON_XMLVariant::Types::Uuid:         return L"uuid";
  case MYON_XMLVariant::Types::Time:         return L"time";
  case MYON_XMLVariant::Types::Buffer:       return L"buffer";
  case MYON_XMLVariant::Types::DoubleColor4: return L"color";
  case MYON_XMLVariant::Types::DoubleArray2: return L"2da";
  case MYON_XMLVariant::Types::DoubleArray3: return L"3da";
  case MYON_XMLVariant::Types::DoubleArray4: return L"4da";
  case MYON_XMLVariant::Types::Null:
  default:
    return L"null";
  }
}

static MYON_XMLVariant::Types PropTypeFromString(const MYON_wString& s)
{
  if (L"int"    == s) return MYON_XMLVariant::Types::Integer;
  if (L"float"  == s) return MYON_XMLVariant::Types::Float;
  if (L"double" == s) return MYON_XMLVariant::Types::Double;
  if (L"string" == s) return MYON_XMLVariant::Types::String;
  if (L"bool"   == s) return MYON_XMLVariant::Types::Bool;
  if (L"matrix" == s) return MYON_XMLVariant::Types::Matrix;
  if (L"uuid"   == s) return MYON_XMLVariant::Types::Uuid;
  if (L"time"   == s) return MYON_XMLVariant::Types::Time;
  if (L"buffer" == s) return MYON_XMLVariant::Types::Buffer;
  if (L"color"  == s) return MYON_XMLVariant::Types::DoubleColor4;
  if (L"2da"    == s) return MYON_XMLVariant::Types::DoubleArray2;
  if (L"3da"    == s) return MYON_XMLVariant::Types::DoubleArray3;
  if (L"4da"    == s) return MYON_XMLVariant::Types::DoubleArray4;

  return MYON_XMLVariant::Types::Null;
}

MYON__UINT32 MYON_XMLNode::CImpl::DataCRC(MYON__UINT32 crc, int depth) const
{
  crc = TagName().DataCRCLower(crc);

  auto pit = m_node.GetPropertyIterator();
  MYON_XMLProperty* prop = nullptr;
  while (nullptr != (prop = pit.GetNextProperty()))
  {
    crc = prop->Name().DataCRCLower(crc);
    crc = prop->GetValue().DataCRC(crc);
  }

  MYON_XMLParameters p(m_node);
  auto* pIt = p.NewIterator();

  MYON_wString name;
  MYON_XMLVariant value;
  while (pIt->Next(name, value))
  {
    crc = value.DataCRC(crc);
  }

  delete pIt;

  if (depth < 2)
  {
    auto cit = m_node.GetChildIterator();
    MYON_XMLNode* child = nullptr;
    while (nullptr != (child = cit.GetNextChild()))
    {
      crc = child->m_impl->DataCRC(crc, depth + 1);
    }
  }

  return crc;
}

const MYON_wString& MYON_XMLNode::CImpl::TagName(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  return m_name;
}

void MYON_XMLNode::CImpl::SetTagName(const wchar_t* name)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  m_name = name;
  m_name.TrimLeft();
  m_name.TrimRight();
}

const MYON_XMLNode& MYON_XMLNode::CImpl::TopmostParent(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  const auto* pNode = &m_node;
  while (nullptr != pNode->Impl().m_parent)
  {
    pNode = pNode->Impl().m_parent;
  }

  return *pNode;
}

MYON_XMLNode* MYON_XMLNode::CImpl::AttachChildNode(MYON_XMLNode* pNode)
{
  if (nullptr == pNode)
    return nullptr;

  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  if (nullptr == m_last_child)
  {
    // There are no children - add one.
    m_last_child = m_first_child = pNode;
  }
  else
  {
    // There are children - add one to the end.
    m_last_child->Impl().m_next_sibling = pNode;
    m_last_child = pNode;
  }

  pNode->Impl().m_next_sibling = nullptr;
  pNode->Impl().m_parent = &m_node;

  return pNode;
}

void MYON_XMLNode::CImpl::AddEmptyDefaultProperty(void)
{
  AddProperty(MYON_XMLProperty(L""));
}

void MYON_XMLNode::CImpl::RemoveAllChildren(void)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  if (nullptr == m_first_child)
    return;

  auto* pNode = m_first_child;
  while (nullptr != pNode)
  {
    auto* pDelete = pNode;
    pNode = pNode->NextSibling();
    delete pDelete;
  }

  m_first_child = nullptr;
  m_last_child = nullptr;
}

MYON_XMLNode* MYON_XMLNode::CImpl::DetachChild(MYON_XMLNode& child)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  if (child.Impl().m_parent != &m_node)
    return nullptr;

  MYON_XMLNode* pChild = nullptr;

  auto* pChildNextSibling = child.Impl().m_next_sibling;

  if (m_first_child == &child)
  {
    if (m_last_child == m_first_child)
      m_last_child = pChildNextSibling;

    m_first_child = pChildNextSibling;

    pChild = &child;
  }
  else
  {
    auto* pNode = m_first_child;
    while (nullptr != pNode)
    {
      if (pNode->Impl().m_next_sibling == &child)
      {
        pNode->Impl().m_next_sibling = pChildNextSibling;

        if (nullptr == pChildNextSibling)
          m_last_child = pNode;

        pChild = &child;
        break;
      }

      pNode = pNode->Impl().m_next_sibling;
    }
  }

  if (nullptr != pChild)
  {
    pChild->Impl().m_parent = nullptr;
    pChild->Impl().m_next_sibling = nullptr;
  }

  return pChild;
}

MYON_XMLNode* MYON_XMLNode::CImpl::PrevSibling(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  auto* pPrev = m_parent->Impl().m_first_child;
  if (pPrev == &m_node)
    return nullptr;

  while (pPrev->Impl().m_next_sibling != &m_node)
  {
    pPrev = pPrev->Impl().m_next_sibling;
  }

  return pPrev;
}

void MYON_XMLNode::CImpl::MoveBefore(MYON_XMLNode& other)
{
  if (&other == &m_node)
    return;

  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  auto* pBeforeOther = other.PrevSibling();
  if (pBeforeOther == &m_node)
    return;

  auto* pPrev = PrevSibling();
  if (nullptr != pPrev)
  {
    pPrev->Impl().m_next_sibling = m_next_sibling;
  }
  else
  {
    // 'this' was the head; redirect the parent's first child.
    m_parent->Impl().m_first_child = m_next_sibling;
  }

  m_next_sibling = &other;

  if (nullptr == pBeforeOther)
  {
    // 'other' was the head; redirect the parent's first child.
    m_parent->Impl().m_first_child = &m_node;
  }
  else
  {
    pBeforeOther->Impl().m_next_sibling = &m_node;
  }
}

void MYON_XMLNode::CImpl::MoveAfter(MYON_XMLNode& other)
{
  if (&other == &m_node)
    return;

  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  auto* pPrev = PrevSibling();
  if (pPrev == &other)
    return;

  if (nullptr != pPrev)
  {
    pPrev->Impl().m_next_sibling = m_next_sibling;
  }
  else
  {
    // 'this' was the head; redirect the parent's first child.
    m_parent->Impl().m_first_child = m_next_sibling;
  }

  m_next_sibling = other.Impl().m_next_sibling;

  other.Impl().m_next_sibling = &m_node;
}

bool MYON_XMLNode::CImpl::RecurseChildren(MYON_XMLRecurseChildrenCallback callback, void* pv) const
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  auto it = m_node.GetChildIterator();

  MYON_XMLNode* pChild = nullptr;
  while (nullptr != (pChild = it.GetNextChild()))
  {
    if (!callback(pChild, pv))
      return false;

    if (!pChild->RecurseChildren(callback, pv))
      return false;
  }

  return true;
}

MYON_XMLProperty* MYON_XMLNode::CImpl::AddProperty(const MYON_XMLProperty& prop)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  auto* pProp = new MYON_XMLProperty(prop);
  pProp->Impl().m_owner = &m_node;
  pProp->Impl().m_pNext = m_first_property;
  m_first_property = pProp;

  return pProp;
}

MYON_XMLProperty* MYON_XMLNode::CImpl::AttachProperty(MYON_XMLProperty* pProp)
{
  if (nullptr == pProp)
    return nullptr;

  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  RemoveProperty(pProp->Name());

  pProp->Impl().m_pNext = m_first_property;
  m_first_property = pProp;
  m_first_property->Impl().m_owner = &m_node;

  return pProp;
}

bool MYON_XMLNode::CImpl::RemoveProperty(const wchar_t* name)
{
  MYON_XMLProperty* pPrev = nullptr;

  auto* pProp = m_first_property;
  while (nullptr != pProp)
  {
    if (pProp->Name().CompareNoCase(name) == 0)
    {
      if (nullptr == pPrev)
      {
        m_first_property = pProp->Next();
      }
      else
      {
        pPrev->Impl().m_pNext = pProp->Next();
      }

      delete pProp;
      return true;
    }

    pPrev = pProp;
    pProp = pProp->Impl().m_pNext;
  }

  return false;
}

void MYON_XMLNode::CImpl::RemoveAllProperties(void)
{
  if (nullptr == m_first_property)
    return;

  auto* pProp = m_first_property;
  while (nullptr != pProp)
  {
    auto* pDelete = pProp;
    pProp = pProp->Next();
    delete pDelete;
  }

  m_first_property = nullptr;
}

MYON_XMLNode* MYON_XMLNode::CImpl::GetNodeAtPath(const wchar_t* wszPath, bool bCreate)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  // Forward slash "/" is the separator.

  if (nullptr == wszPath)
    return &m_node;

  const wchar_t* p = wszPath;
  while (*p != 0)
  {
    if (!iswspace(*p) && *p != L'/')
      break;
    p++;
  }

  if (*p == 0)
    return &m_node; // The input string was empty.

  constexpr int maxBuf = 260;
  wchar_t wsz[maxBuf+1];
  wcsncpy(wsz, p, maxBuf);
  wsz[maxBuf] = 0;

  // Now right trim out the white space.
  const int iLength = (int)wcslen(wsz);

  for (int i = iLength - 1; i >= 0; i--)
  {
    if (iswspace(wsz[i]))
      wsz[i] = 0;
    else
      break;
  }

  // Check for a resultant empty string.
  if (*wsz == 0)
    return &m_node;

  const wchar_t* pNext = nullptr;
  const wchar_t* pstr = wcschr(wsz, L'/');

  const int pos = (pstr != nullptr) ? (int)(pstr - wsz) : -1;
  if (-1 != pos)
  {
    // sNext is the rest of the string that we're going to recurse through.
    pNext = wsz + pos + 1;
    wsz[pos] = 0;
  }

  auto it = m_node.GetChildIterator();
  MYON_XMLNode* pChild = nullptr;
  while (nullptr != (pChild = it.GetNextChild()))
  {
    if (on_wcsicmp(wsz, pChild->TagName()) == 0)
    {
      return pChild->Impl().GetNodeAtPath(pNext, bCreate);
    }
  }

  // The child was not found.
  if (bCreate)
  {
    return AttachChildNode(new MYON_XMLNode(wsz))->Impl().GetNodeAtPath(pNext, bCreate);
  }

  return nullptr;
}

bool MYON_XMLNode::CImpl::RecoverProperty(const MYON_wString& tag, int equalSign, MYON_wString& sProp) // Static.
{
  // Move left, looking for a space and ensuring every character is a valid name char.
  MYON_ASSERT(tag[equalSign] == L'=');

  MYON_wString sName;
  int leftScan = equalSign - 1;
  while (leftScan > 0 && tag[leftScan] != L' ')
  {
    sName = MYON_wString(tag[leftScan--]) + sName;
  }

  if (!IsValidXMLNameWithDebugging(sName) || sName.IsEmpty())
    return false;

  // Now try to recover the value.
  const int start = equalSign + 2;
  if (start >= tag.Length())
    return false;

  if (tag[equalSign + 1] != L'\"')
    return false;

  const int secondQuote = tag.Find(L'\"', start);
  if (secondQuote == -1)
    return false;

  MYON_wString sValue = tag.Mid(start, secondQuote - start);
  EncodeXML(sValue);

  sProp = sName + L"=\"" + sValue + L"\"";

  return true;
}

void MYON_XMLNode::CImpl::AttemptToFixTag(MYON_wString& tag) // Static.
{
  // We're going to rebuild the tag from the name and the number of valid properties we find.
  const MYON_wString sName = GetNameFromTag(tag);
  if (sName.IsEmpty() || tag.Length() < 2)
    return;

  const bool bSelfClosingTag = (0 == wcscmp(tag.Right(2), L"/>"));

  MYON_wString sNewTag = MYON_wString(L"<") + sName;

  // Now find all of the potential properties - looking for = signs.
  int pos = 0;
  int equalSign = -1;
  MYON_wString sProp;

  while ((equalSign = tag.Find(L'=', pos)) >= 0)
  {
    // Move the search past this one for next time around.
    pos = equalSign + 1;

    if (RecoverProperty(tag, equalSign, sProp))
    {
      sNewTag += L" ";
      sNewTag += sProp;
    }
  }

  if (bSelfClosingTag)
  {
    sNewTag += L"/>";
  }
  else
  {
    sNewTag += L">";
  }

  tag = sNewTag;
}

bool MYON_XMLNode::CImpl::GetNextTag(MYON_wString& tag, wchar_t*& pBuffer, bool bValidateTag) // Static.
{
  auto* start = pBuffer;
  while (*start != L'<')
  {
    if (0 == *start)
    {
      OUTPUT_DEBUG_STRING_EOL(L"Start of tag '<' not found");
      return false;
    }

    start++;
  }

  while (start[1] == L'?')
  {
    // This is a nasty document description tag - need to look for the end tag and skip it.
    while (!(start[0] == L'?' && start[1] == L'>'))
    {
      if (0 == *start)
      {
        OUTPUT_DEBUG_STRING_EOL(L"End of document description tag '>' not found");
        return false;
      }

      start++;
    }

    while (*start != L'<')
    {
      start++;
    }
  }

  while ((start[1] == L'!') && (start[2] == L'-') && (start[3] == L'-'))
  {
    // This is a comment tag - need to look for the end tag and skip it.
    while (!(start[0] == L'-' && start[1] == L'-' && start[2] == L'>'))
    {
      if (0 == *start)
      {
        OUTPUT_DEBUG_STRING_EOL(L"End of comment tag '>' not found");
        return false;
      }

      start++;
    }

    while (*start != L'<')
    {
      start++;
    }
  }

  wchar_t* pEnd = start;
  while (*pEnd != L'>')
  {
    if (0 == *pEnd)
    {
      OUTPUT_DEBUG_STRING_EOL(L"End of tag '>' not found");
      return false;
    }

    pEnd++;
  }

  pBuffer = pEnd + 1;

  // Copy the tag into the ready-made string.
  const auto numChars = int(pEnd - start + 1);
  if (numChars < 2)
    return false;

  tag.Set(start, numChars);

  if (bValidateTag)
  {
    if (!AssertValidTag(tag))
    {
      AttemptToFixTag(tag);
      AssertValidTag(tag);
    }
  }

  return true;
}

bool MYON_XMLNode::CImpl::AssertValidTag(const MYON_wString& tag) // Static.
{
  // Check for an even number of quotes - odd means there are quotes in the strings.
  const int quoteCount = tag.Count(L'\"');

  if ((quoteCount % 2) != 0)
  {
    // Odd number of quotes; fail.
    OUTPUT_DEBUG_STRING_EOL(L"Odd number of quotes");
    return false;
  }

  if (tag.Count(L'<') != tag.Count(L'>'))
  {
    OUTPUT_DEBUG_STRING_EOL(L"Bad tag format; even on nested tags, < and > should be equal");
    return false;
  }

  // Check for lone ampersands.
  int pos = 0;
  int find = -1;
  do
  {
    find = tag.Find(L'&', pos);
    if (find >= 0)
    {
      pos = find + 1;
      const int semicolon = tag.Find(L';', find);
      if (semicolon < 0)
      {
        OUTPUT_DEBUG_STRING_EOL(L"Unterminated entity reference");
        return false;
      }

      const auto sEsc = tag.Mid(pos - 1, semicolon - pos + 2);
      if (!sEsc.IsXMLSpecialCharacterEncoding())
      {
        OUTPUT_DEBUG_STRING_EOL(L"Malformed tag on read");
        return false;
      }
    }
  }
  while (find >= 0);

  return true;
}

MYON_XMLNode::MYON_XMLNode(const wchar_t* name)
{
  m_impl = new (m_Impl) CImpl(*this); IMPL_CHECK;

  SetTagName(name);

  m_impl->AddEmptyDefaultProperty();
}

MYON_XMLNode::MYON_XMLNode(const MYON_XMLNode& src)
{
  m_impl = new (m_Impl) CImpl(*this); IMPL_CHECK;

  *this = src;
}

MYON_XMLNode::~MYON_XMLNode()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

const MYON_XMLNode& MYON_XMLNode::operator = (const MYON_XMLNode& src)
{
  if (this == &src)
    return *this;

  std::lock_guard<std::recursive_mutex> lg1(m_impl->m_mutex);
  std::lock_guard<std::recursive_mutex> lg2(src.Impl().m_mutex);

  m_impl->RemoveAllProperties();
  m_impl->RemoveAllChildren();

  m_impl->m_name = src.Impl().m_name;

  // Copy in the properties.
  MYON_XMLProperty* pProperty = nullptr;
  auto pi = src.GetPropertyIterator();
  while (nullptr != (pProperty = pi.GetNextProperty()))
  {
    m_impl->AddProperty(*pProperty); // This does a copy anyway - no need to call the copy constructor
  }

  // Copy in the children.
  MYON_XMLNode* pChild = nullptr;
  auto ci = src.GetChildIterator();
  while (nullptr != (pChild = ci.GetNextChild()))
  {
    AttachChildNode(new MYON_XMLNode(*pChild));
  }

  return *this;
}

bool MYON_XMLNode::MergeFrom(const MYON_XMLNode& src)
{
  std::lock_guard<std::recursive_mutex> lg1(m_impl->m_mutex);
  std::lock_guard<std::recursive_mutex> lg2(src.Impl().m_mutex);

  if (m_impl->m_name != src.TagName())
    return false;

  // Copy in the parameters.
  MYON_XMLProperty* pProperty = nullptr;

  auto pi = src.GetPropertyIterator();
  while (nullptr != (pProperty = pi.GetNextProperty()))
  {
    // Replaces any that are already there.
    AddProperty(*pProperty);
  }

  // Copy in the children.
  const auto bNeedToMerge = ChildCount() != 0;

  MYON_XMLNode* pChild = nullptr;
  auto ci = src.GetChildIterator();
  while (nullptr != (pChild = ci.GetNextChild()))
  {
    auto* pLocalChildNode = bNeedToMerge ? GetNodeAtPath(pChild->TagName()) : nullptr;
    if (nullptr != pLocalChildNode)
    {
      pLocalChildNode->MergeFrom(*pChild);
    }
    else
    {
      AttachChildNode(new MYON_XMLNode(*pChild));
    }
  }

  return true;
}

void MYON_XMLNode::Clear(void)
{
  SetTagName(L"");
  RemoveAllProperties();
  RemoveAllChildren();
}

void MYON_XMLNode::RemoveAllChildren(void)
{
  m_impl->RemoveAllChildren();
}

void MYON_XMLNode::RemoveAllProperties(void)
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  m_impl->RemoveAllProperties();
  m_impl->AddEmptyDefaultProperty();
}

MYON_XMLProperty* MYON_XMLNode::AddProperty(const MYON_XMLProperty& prop)
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  if (g_iWarningsFlagCounter > 0)
  {
    if (prop.Name().Contains(L"\n") || prop.Name().Contains(L"\r"))
    {
      // The name string contain LF or CR codes - this is likely to cause problems but is still valid XML.
      MYON_ASSERT(false);
    }

    const MYON_wString sValue = prop.GetValue().AsString();
    if (sValue.Contains(L"\n") || sValue.Contains(L"\r"))
    {
      // The value string contain LF or CR codes - this is likely to cause problems but is still valid XML.
      MYON_ASSERT(false);
    }
  }

  m_impl->RemoveProperty(prop.Name());

  // Copy the property, then add it to the tree.
  return m_impl->AddProperty(prop);
}

const MYON_wString& MYON_XMLNode::TagName(void) const
{
  return m_impl->TagName();
}

void MYON_XMLNode::SetTagName(const wchar_t* name)
{
  m_impl->SetTagName(name);
}

bool MYON_XMLNode::IsValidXMLNameWithDebugging(const wchar_t* tag_name) // Static.
{
  if (IsValidXMLName(tag_name))
    return true;

  OUTPUT_DEBUG_STRING(L"Invalid XML tag syntax - ");
  OUTPUT_DEBUG_STRING_EOL(tag_name);

  MYON_ASSERT(false);

  return false;
}

bool MYON_XMLNode::IsValidXMLName(const wchar_t* wszTagName) // Static.
{
  // https://www.xml.com/pub/a/2001/07/25/namingparts.html
  //
  // A 'Name' is a token beginning with a letter or one of a few punctuation characters, and continuing with
  // letters, digits, hyphens, underscores, colons, or full stops, together known as name characters.
  // This definition is formally expressed in Extended Backus-Naur Form (EBNF) notation as follows:
  // 
  //  NameChar ::=  Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
  //  Name     ::= (Letter | '_' | ':') (NameChar)*

  // We also simplify as follows:
  //
  // - ':' is not actually allowed in tags, it's reserved for XML namespaces, so we disallow it.
  // - 'Letter' must be an ASCII letter: Letter ::= 'a' to 'z' or 'A' to 'Z'.
  // - 'Digit'  must be an ASCII digit:  Digit  ::= '0' to '9'.
  // - 'CombiningChar' and 'Extender' are not supported (disallowed).

  const MYON_wString sTagName = wszTagName;
  if (sTagName.IsEmpty())
    return false;

  bool bad = false;

  const int len = sTagName.Length();
  for (int i = 0; i < len; i++)
  {
    const auto& ch = sTagName[i];

    if ((ch >= L'a') && (ch <= L'z'))
      continue;

    if ((ch >= L'A') && (ch <= L'Z'))
      continue;

    if (ch == L'_')
      continue;

    if (i > 0)
    {
      if ((ch >= L'0') && (ch <= L'9'))
        continue;

      if ((ch == '-') || (ch == '.'))
        continue;
    }

    bad = true;
  }

  if (bad)
  {
    return false;
  }

  return true;
}

MYON_XMLNode* MYON_XMLNode::GetParent(void) const
{
  return m_impl->m_parent;
}

const MYON_XMLNode& MYON_XMLNode::TopmostParent(void) const
{
  return m_impl->TopmostParent();
}

MYON_XMLNode* MYON_XMLNode::AttachChildNode(MYON_XMLNode* pNode)
{
  return m_impl->AttachChildNode(pNode);
}

MYON_XMLNode* MYON_XMLNode::AddChildNode(MYON_XMLNode* pNode)
{
  return AttachChildNode(pNode);
}

MYON_XMLProperty* MYON_XMLNode::AttachProperty(MYON_XMLProperty* pProp)
{
  return m_impl->AttachProperty(pProp);
}

bool MYON_XMLNode::RemoveProperty(const wchar_t* wszPropertyName)
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  return m_impl->RemoveProperty(wszPropertyName);
}

void MYON_XMLNode::Remove(void)
{
  auto* pParent = GetParent();
  if (nullptr != pParent)
  {
    pParent->RemoveChild(this);
  }
  else
  {
    delete this;
  }
}

MYON_XMLNode* MYON_XMLNode::DetachChild(MYON_XMLNode& child)
{
  return m_impl->DetachChild(child);
}

bool MYON_XMLNode::RemoveChild(MYON_XMLNode* pChild)
{
  if (nullptr == pChild)
    return false;

  auto* pDetach = m_impl->DetachChild(*pChild);
  if (nullptr != pDetach)
  {
    delete pDetach;
    return true;
  }

  return false;
}

MYON_XMLNode::ChildIterator MYON_XMLNode::GetChildIterator(void) const
{
  return ChildIterator(this);
}

MYON_XMLNode::PropertyIterator MYON_XMLNode::GetPropertyIterator(bool bAlphabetized) const
{
  return PropertyIterator(this, bAlphabetized);
}

int MYON_XMLNode::PropertyCount(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  int count = 0;
  auto it = GetPropertyIterator();
  while (it.GetNextProperty())
  {
    count++;
  }

  return count;
}

int MYON_XMLNode::ChildCount(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  int count = 0;
  auto it = GetChildIterator();
  while (it.GetNextChild())
  {
    count++;
  }

  return count;
}

int MYON_XMLNode::GetNestedDepth(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  int depth = 0;

  const auto* pNode = this;
  while (nullptr != pNode->Impl().m_parent)
  {
    pNode = pNode->Impl().m_parent;
    depth++;
  }

  return depth;
}

static bool PrependNodeToStringAndRecurseParents(const MYON_XMLNode* pNode, MYON_wString& s)
{
  // Recursive function to pile up the path.

  if (nullptr == pNode)
    return false;

  auto* pParent = pNode->GetParent();
  if (nullptr == pParent)
    return false;

  s.Insert(0, L'/', 1);
  s.Insert(0, pNode->TagName());

  PrependNodeToStringAndRecurseParents(pParent, s);

  return true;
}

MYON_wString MYON_XMLNode::GetPathFromRoot(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  MYON_wString sPath = TagName();
  PrependNodeToStringAndRecurseParents(GetParent(), sPath);

  return sPath;
}

MYON_XMLProperty& MYON_XMLNode::GetDefaultProperty(void) const
{
  auto* pProp = GetNamedProperty(L"");

  MYON_ASSERT(nullptr != pProp); // Always created by constructor.

  return *pProp;
}

MYON_XMLProperty* MYON_XMLNode::GetNamedProperty(const wchar_t* name) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  auto it = GetPropertyIterator();
  MYON_XMLProperty* pProp = nullptr;

  while (nullptr != (pProp = it.GetNextProperty()))
  {
    if (on_wcsicmp(name, pProp->Name()) == 0)
      return pProp;
  }

  return nullptr;
}

MYON_XMLNode* MYON_XMLNode::GetNamedChild(const wchar_t* name) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  MYON_XMLNode* pNode = nullptr;

  auto it = GetChildIterator();
  while (nullptr != (pNode = it.GetNextChild()))
  {
    if (on_wcsicmp(name, pNode->TagName()) == 0)
      return pNode;
  }

  return nullptr;
}

void* MYON_XMLNode::LastReadBufferPointer(void) const
{
  return m_impl->m_last_read_buf_ptr;
}

bool MYON_XMLNode::CImpl::GetPropertiesFromTag(const MYON_wString& sTag)
{
  SetTagName(GetNameFromTag(sTag));

  MYON_wString tag(sTag);
  tag.TrimLeft(L"</ ");
  tag.TrimRight(L">/ ");

  int pos1 = tag.Find(L' ');
  if (pos1 < 0)
    return true; // No properties.

  tag.TruncateMid(pos1 + 1);

  MYON_wString sPropertyName, sPropertyValue;
  MYON_XMLVariant vValue;

  // We are now at the start of the properties.
  bool bClear = false;

  while (!tag.IsEmpty())
  {
    pos1 = tag.Find(L'=');
    if (pos1 >= 0)
    {
      sPropertyName = tag.Left(pos1);
      sPropertyName.TrimLeft();

      tag.TruncateMid(pos1);

      pos1 = tag.Find(L'\"');
      if (pos1 >= 0)
      {
        tag.TruncateMid(pos1 + 1);
        pos1 = tag.Find(L'\"');
        if (pos1 >= 0)
        {
          sPropertyValue = tag.Left(pos1);
          tag.TruncateMid(pos1 + 1);
          tag.TrimLeft();
          DecodeXML(sPropertyValue);

          const int pos2 = m_bAutoTypePropValue ? sPropertyValue.Find(L':') : -1;
          if (pos2 > 0)
          {
            // The type is encoded in the value.
            const auto type = PropTypeFromString(sPropertyValue.Left(pos2));
            vValue = sPropertyValue.Mid(pos2 + 1);
            vValue.SetTypePendingFlag(true);
            vValue.DoAutoTyping(type);
          }
          else
          {
            // This allows for lazy typing of the property.
            vValue.SetTypePendingFlag(true);
            vValue = sPropertyValue;
          }

          AttachProperty(new MYON_XMLProperty(sPropertyName, vValue));

          bClear = true;
        }
      }
    }

    if (!bClear)
    {
      MYON_ERROR("GetPropertiesFromTag failed");
      return false;
    }

    bClear = false;

    tag.TrimLeft();
  }

  return true;
}

bool MYON_XMLNode::CImpl::IsClosingTag(const MYON_wString& sTag) const
{
  if (sTag.Length() < 3)
    return false;

  if (sTag[0] != L'<')
    return false;

  if (sTag[1] != L'/')
    return false;

  if (GetNameFromTag(sTag).CompareNoCase(TagName()) == 0)
    return true;

  return false;
}

MYON_XMLNode::operator MYON_wString() const
{
  return String();
}

MYON_wString MYON_XMLNode::String(bool includeFormatting, bool forceLongFormat, bool sortedProperties) const
{
  // Pretend to write the node to get the buffer size that needs to be allocated.
  const auto numChars = WriteToStream(nullptr, 0, includeFormatting, forceLongFormat, sortedProperties);

  // Create a buffer of that size inside a string.
  MYON_wString s;
  auto* pBuffer = s.SetLength(numChars);
  if (nullptr != pBuffer)
  {
    // Now really write the node to the created buffer.
    const auto finalNumChars = WriteToStream(pBuffer, numChars+1, includeFormatting, forceLongFormat, sortedProperties);
    s.SetLength(finalNumChars); // Also ensures termination.
  }

  return s;
}

MYON_XMLNode::CharacterCounts MYON_XMLNode::WriteHeaderToStream(wchar_t* stream, MYON__UINT32 max_chars, bool includeFormatting, bool forceLongFormat, bool sortedProperties) const
{
  const bool write = (max_chars != 0);

  // Form the header in a string. The string is only formed if write is true. This is an optimization.
  // logical_header_length keeps track of the length of the string even if it's not being formed.

  MYON_wString header;

  if (write)
  {
    MYON_ASSERT(m_impl->m_name.IsNotEmpty());
    header = L"<";
    header += m_impl->m_name;
  }

  auto logical_header_length = MYON__UINT32(m_impl->m_name.Length()) + 1; // +1 for '<'.

  if (includeFormatting)
  {
    const int depth = GetNestedDepth();
    if (depth > 0)
    {
      if (write)
      {
        header.Insert(0, L'\t', depth);
      }

      logical_header_length += depth;
    }
  }

  /////////////////////////////////////////////////////////////////////////////////
  // This is pure evil.
  // TODO: Find a way to get rid of this psDefaultProperty horror.
  MYON_wString def_prop;
  MYON_wString encoded_def_prop;
  const MYON_wString* psDefaultProperty = nullptr;
  /////////////////////////////////////////////////////////////////////////////////

  auto it = GetPropertyIterator(sortedProperties);
  MYON_XMLProperty* pProp = nullptr;
  while (nullptr != (pProp = it.GetNextProperty()))
  {
    if (pProp->IsDefaultProperty())
    {
      const auto& vDP = pProp->GetValue();
      def_prop = vDP.AsString();

      if (vDP.NeedsXMLEncode())
      {
        encoded_def_prop = def_prop;
        EncodeXML(encoded_def_prop);

        if (!encoded_def_prop.IsEmpty())
        {
          psDefaultProperty = &encoded_def_prop;
        }
      }
      else
      {
        if (def_prop.IsNotEmpty())
          psDefaultProperty = &def_prop;
      }
    }
    else
    {
      if (write)
      {
        header += L' ';
        header += pProp->Name();
        header += L"=\"";
      }

      logical_header_length += 3; // +=3 for ' ' and '=\"'.
      logical_header_length += pProp->Name().Length();

      const auto& vValue = pProp->GetValue();

      if (CImpl::m_bAutoTypePropValue)
      {
        const MYON_wString sType = StringFromPropType(pProp->GetValue().Type());
        if (write)
          header += sType + L':';

        logical_header_length += size_t(sType.Length()) + 1; // +1 for colon.
      }

      const MYON_wString value_string = vValue.AsString();

      if (!vValue.NeedsXMLEncode())
      {
        if (write)
          header += value_string;

        logical_header_length += value_string.Length();
      }
      else
      {
        MYON_wString encoded_value_string = value_string;
        EncodeXML(encoded_value_string);

        if (write)
          header += encoded_value_string;

        logical_header_length += encoded_value_string.Length();
      }

      if (write)
        header += L'\"';

      logical_header_length++; // ++ for quote.
    }
  }

  if ((nullptr != psDefaultProperty) || (ChildCount() > 0) || forceLongFormat)
  {
    MYON_wString sType;

    const bool bType = CImpl::m_bAutoTypePropValue && (nullptr != psDefaultProperty);
    if (bType)
      sType = StringFromPropType(GetDefaultProperty().GetValue().Type());

    if (write)
    {
      header += L'>';
      if (bType)
        header += sType + L':';

      if (nullptr != psDefaultProperty)
        header += *psDefaultProperty;
    }

    logical_header_length++; // ++ for '>'.

    if (bType)
      logical_header_length += size_t(sType.Length()) + 1; // +1 for ':'

    if (nullptr != psDefaultProperty)
      logical_header_length += psDefaultProperty->Length();

    if (includeFormatting && (ChildCount() > 0))
    {
      if (write)
        header += L"\r\n";

      logical_header_length += 2; // +2 for \r\n.
    }
  }

  CharacterCounts counts;
  counts.m_logical = logical_header_length;

  if (0 != max_chars)
  {
    const auto physical_header_length = MYON__UINT32(header.Length());
    const auto chars_to_copy = std::min(max_chars, physical_header_length + 1);
    memcpy(stream, static_cast<const wchar_t*>(header), chars_to_copy * sizeof(wchar_t));
    counts.m_physical = std::min(max_chars, physical_header_length);
    MYON_ASSERT(logical_header_length == physical_header_length);
  }

  MYON_ASSERT(logical_header_length <= UINT_MAX);

  return counts;
}

MYON_XMLNode::CharacterCounts MYON_XMLNode::WriteChildrenToStream(wchar_t* stream, MYON__UINT32 max_chars, bool includeFormatting, bool forceLongFormat, bool sortedProperties) const
{
  CharacterCounts counts;

  MYON_XMLNode* pChild = nullptr;
  auto it = GetChildIterator();
  while (nullptr != (pChild = it.GetNextChild()))
  {
    const auto cc = pChild->WriteToStreamEx(stream, max_chars, includeFormatting, forceLongFormat, sortedProperties);
    counts += cc;
    if (nullptr != stream)
      stream += cc.m_physical;
    max_chars = std::max(0, int(max_chars - cc.m_logical));
  }

  return counts;
}

MYON_XMLNode::CharacterCounts MYON_XMLNode::WriteFooterToStream(wchar_t* stream, MYON__UINT32 max_chars, bool includeFormatting, bool forceLongFormat) const
{
  const auto child_count = ChildCount();
  const bool hasDefaultProp = GetDefaultProperty().GetValue().AsString().IsNotEmpty();

  MYON_wString footer;

  if (hasDefaultProp || (child_count > 0) || forceLongFormat)
  {
    footer = L"</";
    footer += Impl().m_name;
    footer += L'>';

    if (includeFormatting)
    {
      footer += L"\r\n";

      if (child_count > 0)
      {
        const int depth = GetNestedDepth();
        if (depth > 0)
        {
          footer.Insert(0, L'\t', depth);
        }
      }
    }
  }
  else
  {
    footer = L"/>";
    if (includeFormatting)
      footer += L"\r\n";
  }

  CharacterCounts counts;
  counts.m_logical = MYON__UINT32(footer.Length());

  if (0 != max_chars)
  {
    const auto chars_to_copy = std::min(max_chars, counts.m_logical + 1);
    memcpy(stream, static_cast<const wchar_t*>(footer), chars_to_copy * sizeof(wchar_t));
    counts.m_physical = std::min(max_chars, counts.m_logical);
  }

  return counts;
}

void MYON_XMLNode::SetInternalDebuggingFlags(MYON__UINT64 flags)
{
  m_impl->m_debug_auto_test_read = (flags & 1);
}

void MYON_XMLNode::CharacterCounts::operator += (const CharacterCounts& cw)
{
  m_logical  += cw.m_logical;
  m_physical += cw.m_physical;
}

MYON__UINT32 MYON_XMLNode::WriteToStream(wchar_t* stream, MYON__UINT32 max_chars, bool include_formatting,
                       bool force_long_format, bool sorted_props) const
{
  const auto cc = WriteToStreamEx(stream, max_chars, include_formatting, force_long_format, sorted_props);

#ifdef _DEBUG
  if (m_impl->m_debug_auto_test_read && (nullptr != stream))
  {
    MYON_XMLNode test(TagName());
    const auto read = test.ReadFromStream(stream, true, true);
    if (read != cc.m_logical)
    {
      MYON_ERROR("CRITICAL: Could not read back what was written");
    }
  }
#endif

  return cc.m_logical;
}

MYON_XMLNode::CharacterCounts MYON_XMLNode::WriteToStreamEx(wchar_t* stream, MYON__UINT32 max_chars, bool includeFormatting, bool forceLongFormat, bool sortedProperties) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  CharacterCounts counts;

  if ((0 != max_chars) && (nullptr == stream))
  {
    MYON_ERROR("'stream' cannot be null when 'max_chars' is non-zero");
    return counts;
  }

  auto cc = WriteHeaderToStream(stream, max_chars, includeFormatting, forceLongFormat, sortedProperties);
  counts += cc;
  if (nullptr != stream)
    stream += cc.m_physical;

  max_chars = std::max(0, int(max_chars - cc.m_logical));

  cc = WriteChildrenToStream(stream, max_chars, includeFormatting, forceLongFormat, sortedProperties);
  counts += cc;
  if (nullptr != stream)
    stream += cc.m_physical;

  max_chars = std::max(0, int(max_chars - cc.m_logical));

  cc = WriteFooterToStream(stream, max_chars, includeFormatting, forceLongFormat);
  counts += cc;

  return counts;
}

bool MYON_XMLNode::WriteToSegmentedStream(MYON_XMLSegmentedStream& segs, bool includeFormatting, bool forceLongFormat, bool sortedProperties) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  const auto header_cw = WriteHeaderToStream(nullptr, 0, includeFormatting, forceLongFormat, sortedProperties);
  auto* pHeader = new wchar_t[size_t(header_cw.m_logical) + 1];
  WriteHeaderToStream(pHeader, header_cw.m_logical + 1, includeFormatting, forceLongFormat, sortedProperties);
  segs.Append(pHeader);

  auto it = GetChildIterator();
  MYON_XMLNode* pChild = nullptr;
  while (nullptr != (pChild = it.GetNextChild()))
  {
    pChild->WriteToSegmentedStream(segs, includeFormatting, forceLongFormat, sortedProperties);
  }

  const auto footer_cw = WriteFooterToStream(nullptr, 0, includeFormatting, forceLongFormat);
  auto* pFooter = new wchar_t[size_t(footer_cw.m_logical) + 1];
  WriteFooterToStream(pFooter, footer_cw.m_logical + 1, includeFormatting, forceLongFormat);
  segs.Append(pFooter);

  return true;
}

MYON_wString MYON_XMLNode::GetNameFromTag(const wchar_t* wszTag) // Static.
{
  MYON_wString name = wszTag;
  name.TrimLeft(L"</ ");
  name.TrimRight(L">/ ");

  const int pos = name.Find(L' ');
  if (pos >= 0)
  {
    name.SetLength(pos);
  }

  return name;
}

MYON_XMLNode* MYON_XMLNode::FirstChild(void) const
{
  return m_impl->m_first_child;
}

MYON_XMLNode* MYON_XMLNode::PrevSibling(void) const
{
  return m_impl->PrevSibling();
}

void MYON_XMLNode::MoveBefore(MYON_XMLNode& other)
{
  m_impl->MoveBefore(other);
}

void MYON_XMLNode::MoveAfter(MYON_XMLNode& other)
{
  m_impl->MoveAfter(other);
}

bool MYON_XMLNode::RecurseChildren(MYON_XMLRecurseChildrenCallback callback, void* pv) const
{
  return m_impl->RecurseChildren(callback, pv);
}

MYON_XMLNode* MYON_XMLNode::NextSibling(void) const
{
  return m_impl->m_next_sibling;
}

MYON__UINT32 MYON_XMLNode::DataCRC(MYON__UINT32 crc) const
{
  return m_impl->DataCRC(crc, 0);
}

void* MYON_XMLNode::EVF(const wchar_t*, void*)
{
  return nullptr;
}

MYON_XMLNode* MYON_XMLNode::GetNodeAtPath(const wchar_t* wszPath) const
{
  return m_impl->GetNodeAtPath(wszPath, false);
}

MYON_XMLNode* MYON_XMLNode::CreateNodeAtPath(const wchar_t* wszPath)
{
  return m_impl->GetNodeAtPath(wszPath, true);
}

void MYON_XMLNode::OnNodeReadFromStream(const MYON_XMLNode*) const
{
}

MYON__UINT32 MYON_XMLNode::ReadFromStream(const wchar_t* stream, bool bWarningsAsErrors, bool bValidateTags)
{
  if (nullptr == stream)
  {
    MYON_ERROR("Stream cannot be null");
    return ReadError;
  }

  CWarningHelper wh(bWarningsAsErrors);

  Clear();

  // Search for the opening tag "<" character.
  wchar_t* pBuffer = const_cast<wchar_t*>(stream);

  MYON_wString tag;

  // 1st August 2022 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-66795
  // The original code was not checking GetNextTag() for failure and blindly continuing with an empty tag.
  // Then in some places it assumed that the tag was not empty. This is confusing because I thought
  // this was causing RH-66795, but it couldn't be because this bug was not in 7.x, and RH-66795 is 7.13.
  if (!m_impl->GetNextTag(tag, pBuffer, bValidateTags))
    return ReadError;

  if (tag.IsEmpty())
    return ReadError;

  m_impl->m_last_read_buf_ptr = (void*)stream;

  m_impl->GetPropertiesFromTag(tag);

  const auto pos1 = tag.Length() - 2; // Assumes the tag is not empty.
  if (tag[pos1] != L'/')
  {
    // This tag either has children, or a default property.
    MYON_wString sDefaultProperty;

    bool bClosingTag = false;
    do
    {
      auto* pStoreBuffer = pBuffer;

      // Get anything between the tags and store as the default parameter.
      auto* start = pBuffer;
      while ((*start != L'<') && (*start != 0))
      {
        start++;
      }

      if (0 == *start)
      {
        // We ran off the end of the buffer - no idea why, but this is bad, so fail.
        // See http://mcneel.myjetbrains.com/youtrack/issue/RH-16605 for example.
        return ReadError;
      }

      MYON_ASSERT(*start == L'<');

      if (nullptr != start && *start == L'<')
      {
        sDefaultProperty.Append(pBuffer, (int)(start - pBuffer));
      }

      pBuffer = start;

      // 1st August 2022 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-66795
      // The original code was not checking GetNextTag() for failure and blindly continuing with
      // an empty tag. This caused continuous child recursion because the buffer pointer was
      // still pointing at the same (bad) XML. This is confusing because I thought this was causing
      // RH-66795, but it couldn't be because this bug was not in 7.x, and RH-66795 is 7.13.
      if (!m_impl->GetNextTag(tag, pBuffer, bValidateTags))
        return ReadError;

      bClosingTag = m_impl->IsClosingTag(tag);

      if (!bClosingTag)
      {
        auto* pNode = new MYON_XMLNode(L"");
        AttachChildNode(pNode);

        const auto read = pNode->ReadFromStream(pStoreBuffer, bWarningsAsErrors, bValidateTags);
        if (ReadError == read)
          return ReadError;

        pBuffer = pStoreBuffer + read;
      }
    }
    while (!bClosingTag);

    sDefaultProperty.TrimLeft();
    sDefaultProperty.TrimRight();
    DecodeXML(sDefaultProperty);

    if (!sDefaultProperty.IsEmpty())
    {
      auto* pProp = new MYON_XMLProperty;
      AttachProperty(pProp);

      const int pos2 = CImpl::m_bAutoTypePropValue ? sDefaultProperty.Find(L":") : -1;
      if (pos2 > 0)
      {
        // The type is encoded in the value.
        const auto type = PropTypeFromString(sDefaultProperty.Left(pos2));
        auto& v = pProp->GetNonConstValue();
        v = sDefaultProperty.Mid(pos2 + 1);
        v.SetTypePendingFlag(true);
        v.DoAutoTyping(type);
      }
      else
      {
        pProp->Impl().SetHugeStringValue(sDefaultProperty);
        pProp->Impl().GetNonConstValue().SetTypePendingFlag(true);
      }
    }
  }

  TopmostParent().OnNodeReadFromStream(this);

  while ((*pBuffer == L'\r') || (*pBuffer == L'\n'))
    pBuffer++;

  return MYON__UINT32(pBuffer - stream);
}

MYON_XMLNode::CImpl& MYON_XMLNode::Impl(void) const
{
  return *m_impl;
}

class MYON_XMLNode::ChildIterator::CImpl final
{
public:
  MYON_XMLNode* m_pCurrent = nullptr;
};

MYON_XMLNode::ChildIterator::ChildIterator(const MYON_XMLNode* pParent)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;

  if (nullptr != pParent)
  {
    m_impl->m_pCurrent = pParent->Impl().m_first_child;
  }
}

MYON_XMLNode::ChildIterator::ChildIterator(const ChildIterator& other)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  operator = (other);
}

MYON_XMLNode::ChildIterator::~ChildIterator()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

const MYON_XMLNode::ChildIterator& MYON_XMLNode::ChildIterator::operator = (const ChildIterator& other)
{
  m_impl->m_pCurrent = other.m_impl->m_pCurrent;

  return *this;
}

MYON_XMLNode* MYON_XMLNode::ChildIterator::GetNextChild(void)
{
  auto* pNode = m_impl->m_pCurrent;
  if (nullptr != pNode)
  {
    m_impl->m_pCurrent = pNode->Impl().m_next_sibling;
  }

  return pNode;
}

void* MYON_XMLNode::ChildIterator::EVF(const wchar_t*, void*)
{
  return nullptr;
}

// MYON_XMLNode::PropertyIterator
 
class MYON_XMLNode::PropertyIterator::CImpl final
{
public:
  ~CImpl();

  MYON_XMLProperty* GetNextPropertySorted(void)
  {
    // First time through, we build the list.
    if (m_iIndex == 0)
    {
      // While sorting properties, don't allow anything else to access the parent node.
      std::lock_guard<std::recursive_mutex> lg(m_pNode->Impl().m_mutex);

      PropertyIterator pi(m_pNode, false);

      MYON_ASSERT(m_paSortedProperties == nullptr);
      m_paSortedProperties = new std::vector<MYON_XMLProperty>;

      MYON_XMLProperty* p = nullptr;
      while (nullptr != (p = pi.GetNextProperty()))
      {
        m_paSortedProperties->push_back(*p);
      }

      std::sort(m_paSortedProperties->begin(), m_paSortedProperties->end());
    }

    MYON_ASSERT(m_paSortedProperties != nullptr);

    if (m_iIndex >= m_paSortedProperties->size())
      return nullptr;

    return &(*m_paSortedProperties)[m_iIndex++];
  }

public:
  MYON_XMLProperty* m_pCurrent = nullptr;
  const MYON_XMLNode* m_pNode = nullptr;
  std::vector<MYON_XMLProperty>* m_paSortedProperties = nullptr;
  int m_iIndex = 0;
  bool m_bSorted = false;
};

MYON_XMLNode::PropertyIterator::CImpl::~CImpl()
{
  delete m_paSortedProperties;
  m_paSortedProperties = nullptr;
}

MYON_XMLNode::PropertyIterator::PropertyIterator(const MYON_XMLNode* pNode, bool bSorted)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;

  m_impl->m_bSorted = bSorted;

  if (m_impl->m_bSorted)
  {
    if (pNode->PropertyCount() > 1)
    {
      m_impl->m_pNode = pNode;
      m_impl->m_iIndex = 0;
    }
    else
    {
      m_impl->m_bSorted = false;
    }
  }

  if (!m_impl->m_bSorted)
  {
    if (nullptr != pNode)
    {
      m_impl->m_pCurrent = pNode->Impl().m_first_property;
    }
  }
}

MYON_XMLNode::PropertyIterator::PropertyIterator(const PropertyIterator& other)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
  operator = (other);
}

MYON_XMLNode::PropertyIterator::~PropertyIterator()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

const MYON_XMLNode::PropertyIterator& MYON_XMLNode::PropertyIterator::operator = (const PropertyIterator& other)
{
  m_impl->m_pCurrent           = other.m_impl->m_pCurrent;
  m_impl->m_pNode              = other.m_impl->m_pNode;
  m_impl->m_iIndex             = other.m_impl->m_iIndex;
  m_impl->m_bSorted            = other.m_impl->m_bSorted;
  m_impl->m_paSortedProperties = other.m_impl->m_paSortedProperties;

  return *this;
}

MYON_XMLProperty* MYON_XMLNode::PropertyIterator::GetNextProperty(void)
{
  if (m_impl->m_bSorted)
    return m_impl->GetNextPropertySorted();

  auto* pProp = m_impl->m_pCurrent;
  if (nullptr != pProp)
  {
    m_impl->m_pCurrent = pProp->Impl().m_pNext;
  }

  return pProp;
}

// MYON_XMLRootNode

// TODO: Somehow I managed to port the non-rc version of the root node.
//       We really need the rc version.

static const MYON_wString sXMLRootNodeName(L"xml");

class MYON_XMLRootNode::CImpl final
{
public:
};

MYON_XMLRootNode::MYON_XMLRootNode()
  :
  MYON_XMLNode(sXMLRootNodeName)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;
}

MYON_XMLRootNode::MYON_XMLRootNode(const MYON_XMLRootNode& src)
  :
  MYON_XMLNode(sXMLRootNodeName)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;

  *this = src;
}

MYON_XMLRootNode::~MYON_XMLRootNode()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

const MYON_XMLRootNode& MYON_XMLRootNode::operator = (const MYON_XMLRootNode& src)
{
  *static_cast<MYON_XMLNode*>(this) = src;

  return *this;
}

const MYON_XMLRootNode& MYON_XMLRootNode::NodeForRead(void) const
{
  return *this;
}

MYON_XMLRootNode& MYON_XMLRootNode::NodeForWrite(void)
{
  return *this;
}

bool MYON_XMLRootNode::ReadFromFile(const wchar_t* wszPath, bool bWarningsAsErrors, bool bValidateTags)
{
  CWarningHelper wh(bWarningsAsErrors);

  MYON_UnicodeTextFile file;
  if (!file.Open(wszPath, MYON_UnicodeTextFile::Modes::Read))
  {
    MYON_wString s;
    s.Format(L"Failed to open file %s", wszPath);
    OUTPUT_DEBUG_STRING_EOL(s);
    return false;
  }

  MYON_wString sXML;
  if (!file.ReadString(sXML))
  {
    MYON_wString s;
    s.Format(L"Failed to read file %s", wszPath);
    OUTPUT_DEBUG_STRING_EOL(s);
    return false;
  }

  const auto* stream = static_cast<const wchar_t*>(sXML);
  const auto read = ReadFromStream(stream, bWarningsAsErrors, bValidateTags);
  if (read == ReadError)
  {
    MYON_wString s;
    s.Format(L"XML error reading file %s", wszPath);
    OUTPUT_DEBUG_STRING_EOL(s);
    return false;
  }

  if (0 == read)
  {
    MYON_wString s;
    s.Format(L"No valid XML was read from the file %s", wszPath);
    OUTPUT_DEBUG_STRING_EOL(s);
    return false;
  }

  return true;
}

bool MYON_XMLRootNode::WriteToFile(const wchar_t* wszPath, bool includeFormatting, bool bUTF8, bool sortedProperties) const
{
  const auto type = bUTF8 ? MYON_UnicodeTextFile::Types::UTF8 : MYON_UnicodeTextFile::Types::UTF16;
  MYON_UnicodeTextFile file(type);
  if (!file.Open(wszPath, MYON_UnicodeTextFile::Modes::Write))
    return false;

  MYON_XMLSegmentedStream segs;
  WriteToSegmentedStream(segs, includeFormatting, false, sortedProperties);

  for (int i = 0; i < segs.Count(); i++)
  {
    file.WriteString(segs.Segment(i));
  }

  file.Close();

  return true;
}

void MYON_XMLRootNode::Clear(void)
{
  MYON_XMLNode::Clear();

  SetTagName(sXMLRootNodeName);
}

MYON_XMLRootNode::CImpl& MYON_XMLRootNode::Impl(void) const
{
  return *m_impl;
}

// MYON_XMLUserData -- Specializes MYON_UserData for XML use.

class MYON_XMLUserData::CImpl final
{
public:
  MYON_XMLRootNode m_XMLRoot;
};

MYON_XMLUserData::CImpl& MYON_XMLUserData::Impl(void) const
{
  return *m_impl;
}

MYON_XMLUserData::MYON_XMLUserData()
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;

  m_userdata_copycount = 1;
  m_userdata_uuid = MYON_nil_uuid;
}

MYON_XMLUserData::MYON_XMLUserData(const MYON_XMLUserData& ud)
  :
  MYON_UserData(ud)
{
  m_impl = new (m_Impl) CImpl; IMPL_CHECK;

  m_userdata_copycount = ud.m_userdata_copycount;
  m_userdata_uuid = ud.m_userdata_uuid;
}

MYON_XMLUserData::~MYON_XMLUserData()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

const MYON_XMLUserData& MYON_XMLUserData::operator = (const MYON_XMLUserData& ud)
{
  MYON_UserData::operator = (ud); // CRITICAL - Be sure to call base class.

  m_userdata_uuid = ud.m_userdata_uuid;

  m_impl->m_XMLRoot = ud.m_impl->m_XMLRoot;

  return *this;
}

const MYON_XMLRootNode& MYON_XMLUserData::XMLRootForRead(void) const
{
  return m_impl->m_XMLRoot.NodeForRead();
}

MYON_XMLRootNode& MYON_XMLUserData::XMLRootForWrite(void) const
{
  return m_impl->m_XMLRoot.NodeForWrite();
}

MYON_XMLProperty* MYON_XMLUserData::Property(const wchar_t* wszXMLPath, const wchar_t* wszPropertyName) const
{
  auto* pProp = InternalProperty(wszXMLPath, wszPropertyName);
  if (nullptr == pProp)
  {
    // Failed to get the property. This probably means that the XML strings have been changed
    // and this is old user data. Reset to defaults.
    SetToDefaults();

    // This can still return nullptr if the path and/or property were not set as one of the defaults.
    pProp = InternalProperty(wszXMLPath, wszPropertyName);
  }

  return pProp;
}

MYON_XMLVariant MYON_XMLUserData::Value(const wchar_t* wszXMLPath, const wchar_t* wszPropertyName) const
{
  const auto* pProp = Property(wszXMLPath, wszPropertyName);
  if (pProp != nullptr)
  {
    return pProp->GetValue();
  }

  return MYON_XMLVariant();
}

bool MYON_XMLUserData::SetValue(const wchar_t* wszXMLPath, const wchar_t* wszPropertyName, const MYON_XMLVariant& value)
{
  auto* pProp = Property(wszXMLPath, wszPropertyName);
  if (pProp != nullptr)
  {
    pProp->SetValue(value);
    return true;
  }

  return false;
}

void MYON_XMLUserData::SetValue(const wchar_t* wszXMLPath, const MYON_XMLVariant& value)
{
  SetValue(wszXMLPath, L"", value);
}

void MYON_XMLUserData::Clear(void) const
{
  m_impl->m_XMLRoot.Clear();
}

int MYON_XMLUserData::Version(void) const
{
  return 2;
}

void MYON_XMLUserData::_Dump(const wchar_t* wszFileName) const
{
  m_impl->m_XMLRoot.WriteToFile(wszFileName);
}

MYON_XMLProperty* MYON_XMLUserData::InternalProperty(const wchar_t* wszXMLPath, const wchar_t* wszPropertyName) const
{
  const auto* pNode = m_impl->m_XMLRoot.NodeForRead().GetNodeAtPath(wszXMLPath);
  if (nullptr == pNode)
    return nullptr;

  return pNode->GetNamedProperty(wszPropertyName);
}

void MYON_XMLUserData::SetToDefaultsImpl(int) const
{
  SetToDefaults();
}

bool MYON_XMLUserData::Archive(void) const
{
  return true;
}

bool MYON_XMLUserData::Read(MYON_BinaryArchive& archive)
{
  // Read the version number.
  int version = 0;
  if (!archive.ReadInt(&version))
    return false;

  // Cannot load user data with a version number greater than mine.
  if (version > Version())
  {
    ReportVersionError();
    return false;
  }

  // Clear any existing XML data.
  XMLRootForWrite().Clear();

  if (1 == version)
  {
    // Original version. Read the archive into a string.
    MYON_wString s;
    if (!archive.ReadString(s))
      return false;

    // Read the string into the XML root.
    XMLRootForWrite().ReadFromStream(s);

    return true;
  }

  // UTF8 version. Read the length of the UTF8 buffer from the archive.
  int length_utf8 = 0;
  if (!archive.ReadInt(&length_utf8))
    return false;

  // Read the UTF8 buffer from the archive.
  auto utf8 = std::unique_ptr<char[]>(new char[size_t(length_utf8)+1]);
  auto* pUTF8 = utf8.get();

  if (!archive.ReadChar(length_utf8, pUTF8))
    return false;

  pUTF8[length_utf8] = 0; // Terminator.

  // Convert the UTF8 data to wide chars and read it into the root node.
  const auto num_chars = MYON_ConvertUTF8ToWideChar(false, pUTF8, -1, nullptr, 0, nullptr, 0, 0, nullptr);
  auto wide = std::unique_ptr<wchar_t[]>(new wchar_t[size_t(num_chars) + 1]);
  auto* pWide = wide.get();
  MYON_ConvertUTF8ToWideChar(false, pUTF8, -1, pWide, num_chars+1, nullptr, 0, 0, nullptr);

  if (MYON_XMLNode::ReadError == XMLRootForWrite().ReadFromStream(pWide))
    return false;

  return true;
}

bool MYON_XMLUserData::Write(MYON_BinaryArchive& archive) const
{
  // Write the version number to the archive.
  if (!archive.WriteInt(Version()))
    return false;

  // Convert the XML string to UTF8.
  const auto sXML = XMLRootForRead().String();
  const auto* wsz = static_cast<const wchar_t*>(sXML);

  const auto num_chars = MYON_ConvertWideCharToUTF8(false, wsz, -1, nullptr, 0, nullptr, 0, 0, nullptr);

  auto utf8 = std::unique_ptr<char[]>(new char[num_chars]);
  auto* pUTF8 = utf8.get();

  MYON_ConvertWideCharToUTF8(false, wsz, -1, pUTF8, num_chars, nullptr, 0, 0, nullptr);

  // Write the length of the UTF8 buffer to the archive.
  if (!archive.WriteInt(num_chars))
    return false;

  // Write the UTF8 buffer to the archive.
  if (!archive.WriteChar(num_chars, pUTF8))
    return false;

  return true;
}

void* MYON_XMLUserData::EVF(const wchar_t*, void*)
{
  return nullptr;
}

// MYON_XMLParameters

class MYON_XMLParameters::CImpl
{
public:
  CImpl(MYON_XMLNode& n) : m_node(n) { }

  MYON_XMLNode& m_node;
  MYON_wString m_sDefaultType;
  bool m_bWriteTypeProperty = true;
};

MYON_XMLNode* MYON_XMLParameters::ObtainChildNodeForWrite(MYON_XMLNode& node, const wchar_t* wszParamName) const
{
  // <ambient...

  // If the parameter name is not valid XML, we can't write this parameter in the legacy format.
  if (!MYON_XMLNode::IsValidXMLName(wszParamName))
    return nullptr;

  auto* pChildNode = node.GetNamedChild(wszParamName);
  if (nullptr == pChildNode)
  {
    pChildNode = node.AttachChildNode(new MYON_XMLNode(wszParamName));
  }

  return pChildNode;
}

MYON_XMLParameters::MYON_XMLParameters(MYON_XMLNode& node)
{
  m_impl = new CImpl(node);
}

MYON_XMLParameters::MYON_XMLParameters(const MYON_XMLNode& node)
{
  m_impl = new CImpl(const_cast<MYON_XMLNode&>(node));
}

MYON_XMLParameters::~MYON_XMLParameters()
{
  delete m_impl;
  m_impl = nullptr;
}

void MYON_XMLParameters::SetWriteTypeProperty(bool b)
{
  m_impl->m_bWriteTypeProperty = b;
}

void MYON_XMLParameters::SetDefaultReadType(const wchar_t* wszType)
{
  m_impl->m_sDefaultType = wszType;
}

MYON_wString MYON_XMLParameters::AsString(void) const
{
  return m_impl->m_node.String();
}

void MYON_XMLParameters::SetAsString(const wchar_t* wsz)
{
  m_impl->m_node.ReadFromStream(wsz);
}

bool MYON_XMLParameters::GetParam(const wchar_t* wszParamName, MYON_XMLVariant& vValueOut) const
{
  // This expects the legacy format where the param name is the XML tag.
  // If you want to use the new system, you should be using MYON_XMLParametersV8.

  const auto* pNode = m_impl->m_node.GetNodeAtPath(wszParamName);
  if (nullptr == pNode)
    return false;

  if (!GetParamNode(*pNode, vValueOut))
    return false;

  return true;
}

MYON_XMLNode* MYON_XMLParameters::SetParam(const wchar_t* wszParamName, const MYON_XMLVariant& value)
{
  if (MYON_wString(wszParamName).IsEmpty())
    return nullptr;

  // Set the parameter node.
  return SetParamNode(m_impl->m_node, wszParamName, value);
}

MYON_XMLNode* MYON_XMLParameters::SetParamNode(MYON_XMLNode& node, const wchar_t* wszParamName, const MYON_XMLVariant& vValue)
{
  auto* pChildNode = ObtainChildNodeForWrite(node, wszParamName);
  if (nullptr == pChildNode)
    return nullptr;

  const wchar_t* wszType = L"";
  switch (vValue.Type())
  {
  case MYON_XMLVariant::Types::Integer:      wszType = L"int";      break;
  case MYON_XMLVariant::Types::Float:        wszType = L"float";    break;
  case MYON_XMLVariant::Types::Double:       wszType = L"double";   break;
  case MYON_XMLVariant::Types::String:       wszType = L"string";   break;
  case MYON_XMLVariant::Types::Bool:         wszType = L"bool";     break;
  case MYON_XMLVariant::Types::DoubleColor4: wszType = L"color";    break;
  case MYON_XMLVariant::Types::DoubleArray2: wszType = L"vector2d"; break;
  case MYON_XMLVariant::Types::DoubleArray3: wszType = L"vector3d"; break;
  case MYON_XMLVariant::Types::DoubleArray4: wszType = L"point4d";  break;
  case MYON_XMLVariant::Types::Uuid:         wszType = L"uuid";     break;
  case MYON_XMLVariant::Types::Matrix:       wszType = L"matrix";   break;
  case MYON_XMLVariant::Types::Time:         wszType = L"time";     break;
  case MYON_XMLVariant::Types::Buffer:       wszType = L"buffer";   break;

  default:
    ; // NULL variant is normal these days.
  }

  // Default property is the actual value.
  MYON_XMLProperty prop;
  prop.SetValue(vValue);
  pChildNode->AddProperty(prop);

  // Set units (if any).
  if (MYON::LengthUnitSystem::None != vValue.Units())
  {
    prop.SetName(L"units");
    const auto* wsz = StringFromUnits(vValue.Units());
    prop.SetValue(wsz);
    pChildNode->AddProperty(prop);
  }

  if (m_impl->m_bWriteTypeProperty)
  {
    // Set type.
    prop.SetName(L"type");
    prop.SetValue(wszType);
    pChildNode->AddProperty(prop);
  }

  return pChildNode;
}

const MYON_XMLNode& MYON_XMLParameters::Node(void) const
{
  return m_impl->m_node;
}

bool MYON_XMLParameters::GetParamNode(const MYON_XMLNode& node, MYON_XMLVariant& vValueOut) const
{
  vValueOut.SetUnits(MYON::LengthUnitSystem::None);

  const auto& v = node.GetDefaultProperty().GetValue();

  MYON_wString sType = m_impl->m_sDefaultType;

  auto* pProp = node.GetNamedProperty(L"type");
  if (nullptr != pProp)
  {
    sType = pProp->GetValue().AsString();
  }
  else
  if (sType.IsEmpty())
  {
    // The node contains no type specifier and a default type was not specified by the caller. Fail.
    vValueOut.SetNull();
    return false;
  }

  // Andy hack to solve the problem with matrices stored as strings on early versions of RDK.
  if ((vValueOut.Type() == MYON_XMLVariant::Types::Matrix) && (sType == L"string"))
  {
    sType = L"matrix";
  }

  if (sType.IsEmpty())
  {
    vValueOut.SetNull();
  }
  else
  if (sType.CompareNoCase(L"bool") == 0)
  {
    vValueOut = v.AsBool();
  }
  else
  if ((sType.CompareNoCase(L"int") == 0)   ||
      (sType.CompareNoCase(L"short") == 0) ||
      (sType.CompareNoCase(L"char") == 0)  ||
      (sType.CompareNoCase(L"long") == 0))
  {
    vValueOut = v.AsInteger();
  }
  else
  if (sType.CompareNoCase(L"float") == 0)
  {
    vValueOut = v.AsFloat();
  }
  else
  if ((sType.CompareNoCase(L"double") == 0) ||
      (sType.CompareNoCase(L"real") == 0))  // For backward compatibilty with old XML.
  {
    vValueOut = v.AsDouble();
  }
  else
  if (sType.CompareNoCase(L"point") == 0)  // For backward compatibilty with old XML.
  {
    vValueOut = v.AsColor();
  }
  else
  if (sType.CompareNoCase(L"color") == 0)
  {
    // Get both as 3d and 4d points; that way it works for old files
    // that only had 3d points and also for new files that have 4d points.

    const auto t = v.Type();
    MYON_4dPoint pt(0.0, 0.0, 0.0, 1.0);

    if (t == MYON_XMLVariant::Types::DoubleArray3)
    {
      const MYON_3dPoint p3(v.As3dPoint());
      pt.x = p3.x;
      pt.y = p3.y;
      pt.z = p3.z;
    }
    else
    if ((t == MYON_XMLVariant::Types::DoubleArray4) || (t == MYON_XMLVariant::Types::DoubleColor4))
    {
      pt = v.As4dPoint();
    }
    else
    {
      const auto sColor = v.AsString();

      if (sColor.IsValid4dPoint())
      {
        pt = v.As4dPoint();
      }

      if (sColor.IsValid3dPoint())
      {
        const MYON_3dPoint p3(v.As3dPoint());
        pt.x = p3.x;
        pt.y = p3.y;
        pt.z = p3.z;
      }
    }

    // Get x, y, z from 3d point and w from 4d point.
    MYON_4fColor c;
    c.SetRGBA(float(pt.x), float(pt.y), float(pt.z), float(pt.w));
    vValueOut = c;
  }
  else
  if (sType.CompareNoCase(L"vector2d") == 0)
  {
    vValueOut = MYON_XMLVariant(v.As2dPoint(), MYON_XMLVariant::ArrayTypes::Array2);
  }
  else
  if ((sType.CompareNoCase(L"vector3d") == 0) ||
      (sType.CompareNoCase(L"vector") == 0))  // For backward compatibilty with old XML.
  {
    vValueOut = MYON_XMLVariant(v.As3dPoint(), MYON_XMLVariant::ArrayTypes::Array3);
  }
  else
  if (sType.CompareNoCase(L"point4d") == 0)
  {
    vValueOut = MYON_XMLVariant(v.As4dPoint(), MYON_XMLVariant::ArrayTypes::Array4);
  }
  else
  if (sType.CompareNoCase(L"string") == 0)
  {
    vValueOut = v.AsString();
  }
  else
  if (sType.CompareNoCase(L"matrix") == 0)
  {
    vValueOut = v.AsXform();
  }
  else
  if (sType.CompareNoCase(L"uuid") == 0)
  {
    vValueOut = v.AsUuid();
  }
  else
  if (sType.CompareNoCase(L"time") == 0)
  {
    vValueOut = v.AsTime();
  }
  else
  if (sType.CompareNoCase(L"buffer") == 0)
  {
    vValueOut = v.AsBuffer();
  }

  pProp = node.GetNamedProperty(L"units");
  if (nullptr != pProp)
  {
    const auto sUnits = pProp->GetValue().AsString();
    vValueOut.SetUnits(UnitsFromString(sUnits));
  }

  return true;
}

MYON_XMLParameters::CIterator* MYON_XMLParameters::NewIterator(void) const
{
  return new CIterator(*this);
}

void* MYON_XMLParameters::EVF(const wchar_t*, void*)
{
  return nullptr;
}

class MYON_XMLParameters::CIterator::CImpl
{
public:
  CImpl(const MYON_XMLParameters& p) : m_Params(p), m_XMLIterator(&p.m_impl->m_node) { }

  const MYON_XMLParameters& m_Params;
  MYON_XMLNode::ChildIterator m_XMLIterator;
};

MYON_XMLParameters::CIterator::CIterator(const MYON_XMLParameters& p)
{
  m_impl = new CImpl(p);
}

MYON_XMLParameters::CIterator::~CIterator()
{
  delete m_impl;
  m_impl = nullptr;
}

bool MYON_XMLParameters::CIterator::Next(MYON_wString& sParamNameOut, MYON_XMLVariant& vParamValueOut) const
{
  const auto* pNode = m_impl->m_XMLIterator.GetNextChild();
  if (nullptr == pNode)
    return false;

  sParamNameOut = pNode->TagName();

  m_impl->m_Params.GetParamNode(*pNode, vParamValueOut);

  return true;
}

void* MYON_XMLParameters::CIterator::EVF(const wchar_t*, void*)
{
  return nullptr;
}

// MYON_XMLParametersV8

MYON_XMLParametersV8::MYON_XMLParametersV8(MYON_XMLNode& node)
  :
  MYON_XMLParameters(node)
{
}

MYON_XMLParametersV8::MYON_XMLParametersV8(const MYON_XMLNode& node)
  :
  MYON_XMLParameters(node)
{
}

MYON_XMLNode* MYON_XMLParametersV8::ObtainChildNodeForWrite(MYON_XMLNode& node, const wchar_t* wszParamName) const
{
  // <parameter name="ambient"...

  auto* pChildNode = FindNodeByNameProperty(wszParamName);
  if (nullptr == pChildNode)
  {
    pChildNode = node.AttachChildNode(new MYON_XMLNode(MYON_PARAMETER));

    // Set the parameter name.
    MYON_XMLProperty prop;
    prop.SetName(MYON_NAME);
    prop.SetValue(wszParamName);
    pChildNode->AddProperty(prop);
  }

  return pChildNode;
}

MYON_XMLNode* MYON_XMLParametersV8::FindNodeByNameProperty(const wchar_t* wszParamName) const
{
  auto* pNode = Node().FirstChild();
  while (nullptr != pNode)
  {
    auto* pProp = pNode->GetNamedProperty(MYON_NAME);
    if (nullptr != pProp)
    {
      if (pProp->GetValue().AsString() == wszParamName)
        return pNode;
    }

    pNode = pNode->NextSibling();
  }

  return nullptr;
}

bool MYON_XMLParametersV8::GetParam(const wchar_t* wszParamName, MYON_XMLVariant& vValueOut) const
{
  const auto* pNode = FindNodeByNameProperty(wszParamName);
  if (nullptr == pNode)
    return false;

  if (!GetParamNode(*pNode, vValueOut))
    return false;

  return true;
}

#define MYON_RDK_UD_MATERIAL     L"material"
#define MYON_RDK_UD_INSTANCE_ID  L"instance-id"

static const MYON_UUID uuidUniversalRenderEngine = { 0x99999999, 0x9999, 0x9999, { 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99 } };

MYON_OBJECT_IMPLEMENT(MYON_RdkUserData, MYON_UserData, "AFA82772-1525-43dd-A63C-C84AC5806911");

static MYON_RdkUserData::ReadCallback      g_RdkUserDataReadCallback = nullptr;
static MYON_RdkUserData::WriteCallback     g_RdkUserDataWriteCallback = nullptr;
static MYON_RdkUserData::TransformCallback g_RdkUserDataTransformCallback = nullptr;

void MYON_RdkUserData::SetCallbacks(ReadCallback rc, WriteCallback wc, TransformCallback tc) // Static.
{
  g_RdkUserDataReadCallback = rc;
  g_RdkUserDataWriteCallback = wc;
  g_RdkUserDataTransformCallback = tc;
}

MYON_UUID MYON_RdkUserData::Uuid(void) // Static.
{
  const MYON_UUID uuid = { 0xb63ed079, 0xcf67, 0x416c, { 0x80, 0xd, 0x22, 0x2, 0x3a, 0xe1, 0xbe, 0x21 } };
  return uuid;
}

MYON_RdkUserData::MYON_RdkUserData()
{
  m_userdata_uuid = Uuid();

  m_application_uuid = RdkPlugInId();

  m_userdata_copycount = 1;

  SetToDefaults();
}

MYON_RdkUserData::MYON_RdkUserData(const MYON_RdkUserData& ud)
  :
  MYON_XMLUserData(ud) // CRITICAL - Be sure to call base class.
{
  m_userdata_uuid = Uuid();

  m_application_uuid = RdkPlugInId();

  // DO NOT SET OTHER MYON_UserData fields
  // In particular, do not set m_userdata_copycount

  *this = ud;
}

MYON_RdkUserData::~MYON_RdkUserData()
{
}

const MYON_RdkUserData& MYON_RdkUserData::operator = (const MYON_RdkUserData& ud)
{
  if (this != &ud)
  {
    // Use the base class operator=() to correctly copy all MYON_UserData fields.
    MYON_XMLUserData::operator = (ud);
  }

  return *this;
}

void MYON_RdkUserData::SetToDefaults(void) const
{
  const_cast<MYON_RdkUserData*>(this)->Clear();
}

void MYON_RdkUserData::ReportVersionError(void) const
{
  //::RhRdkErrorBox(RhLocalizeString( L"This document was created with a more recent version of RDK", 23067));
}

bool MYON_RdkUserData::GetDescription(MYON_wString& s)
{
  s = L"RDK data";

  return true;
}

bool MYON_RdkUserData::Read(MYON_BinaryArchive& ar)
{
  if (!MYON_XMLUserData::Read(ar))
    return false;

  if (nullptr != g_RdkUserDataReadCallback)
  {
    (*g_RdkUserDataReadCallback)(*this);
  }

  return true;
}

bool MYON_RdkUserData::Write(MYON_BinaryArchive& ar) const
{
  if (!MYON_XMLUserData::Write(ar))
    return false;

  if (nullptr != g_RdkUserDataWriteCallback)
  {
    (*g_RdkUserDataWriteCallback)(*this);
  }

  return true;
}

bool MYON_RdkUserData::DeleteAfterWrite(const MYON_BinaryArchive& archive, const MYON_Object* parent_object) const
{
  // RDK user data has been used to attach information to both MYON_Material and non-MYON_Material classes.
  // Its use on MYON_Material is now obsolete and this case should never happen again.
  MYON_ASSERT(nullptr == MYON_Material::Cast(parent_object));

  return false; // false means the user data will continue to exist.
}

bool MYON_RdkUserData::DeleteAfterRead(const MYON_BinaryArchive& archive, MYON_Object* parent_object) const
{
  // Pre V6 May 16, 2014, the RDK material instance id for MYON_Materials was stored as user data.
  // As of V6, May 16, 2014, the id is stored on MYON_Material::m_rdk_material_instance_id.
  //
  // This virtual MYON_UserData override is used to delete RDK user data that is:
  //  - attached to MYON_Material
  //  - stores the obsolete material instance id that is now on MYON_Material::m_rdk_material_instance_id

  auto* pOnMaterial = MYON_Material::Cast(parent_object);
  if (nullptr == pOnMaterial)
  {
    // CRhRdkUserData has been used to attach information to both MYON_Material and non-MYON_Material classes.
    // Its use on MYON_Material is now obsolete and this case is handled below.
    // When the parent object is not an MYON_Material, MYON_RdkUserData is now used (replaces CRhRdkUserData).
    return false; // false means the user data will continue to exist.
  }

  // Because this RDK user data was on the material, set its plug-in id to 'universal'.
  // This replaces a hack in CRhinoRead3dmHelper::ReadMaterialTable() which corrected
  // the plug-in id of V4 materials. These materials were wrongly saved by V4 with the
  // current render engine id instead of 'universal'.
  pOnMaterial->SetMaterialPlugInId(uuidUniversalRenderEngine); // Fixes RH-27240 'V4 RDK materials do not work correctly in V6'

  // Transfer the instance id from the user data to the new member variable.
  const auto* wsz = MYON_RDK_UD_ROOT  MYON_RDK_SLASH  MYON_RDK_UD_MATERIAL;
  const auto uuidInstance = Value(wsz, MYON_RDK_UD_INSTANCE_ID).AsUuid();
  MYON_ASSERT(pOnMaterial->RdkMaterialInstanceIdIsNil() || (uuidInstance == pOnMaterial->RdkMaterialInstanceId()));
  pOnMaterial->SetRdkMaterialInstanceId(uuidInstance);

  return true; // true means this obsolete userdata will be deleted by the MYON_BinaryArchive reading code.
}

bool MYON_RdkUserData::Transform(const MYON_Xform& xform)
{
  if (nullptr != g_RdkUserDataTransformCallback)
  {
    (*g_RdkUserDataTransformCallback)(*this, xform);
  }

  return MYON_XMLUserData::Transform(xform);
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////             ///////////////////////////////////////
////////////////////////////////////  T E S T S  ///////////////////////////////////////
////////////////////////////////////             ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

static bool g_bXMLTestsOK = true;

#ifdef MYON_RUNTIME_WIN
static size_t CallbackDone = 0;
static char CallbackBuffer[1000];
#endif

static std::mutex g_mutex;

static void ThreadFunc(wchar_t c)
{
  std::lock_guard<std::mutex> lg(g_mutex);

  MYON_wString s = c;
  for (int i = 0; i < 100; i++)
  {
    OUTPUT_DEBUG_STRING(s);
  }

  OUTPUT_DEBUG_STRING(L"\n");
}

#ifdef MYON_RUNTIME_WIN
static bool Base64Callback(void* context, MYON__UINT32 size, const void* buffer)
{
  memcpy(CallbackBuffer + CallbackDone, buffer, size);

  CallbackDone += size;

  return true;
}
#endif

static void Validate(bool b)
{
  MYON_ASSERT(b);
  if (!b)
    g_bXMLTestsOK = false;
}

void GetRDKTestXML(MYON_wString& sXML)
{
    //const auto sFile = L"C:\\Users\\croud\\Desktop\\RdkSave.txt";
    //MYON_UnicodeTextFile file;
    //if (file.Open(sFile, MYON_UnicodeTextFile::Modes::Load))
    //{
    //  MYON_wString s;
    //  Validate(file.ReadString(s));
    //  Validate(file.Close());
    //
    //  MYON_wString sBase64;
    //  MYON_Base64::Encode(s.Array(), s.Length() * sizeof(wchar_t), sBase64, false);
    //
    //  file.Close();
    //}

    // Decode a huge Base64 string into a load of RDK XML in UTF16.
    const auto* wszBase64 =
   L"PAB4AG0AbAA+AA0ACgAJADwAcgBlAG4AZABlAHIALQBjAG8AbgB0AGUAbgB0AC0AbQBhAG4AYQBnAGUAcgAtAGQAbwBjAHUAbQBlAG4AdAA+AA0ACgAJAAkAPABtAGEAdABlAHIAaQBhAGwALQBzAGUAYwB0AGkAbwBuAD4ADQAKAAkACQAJADwAbQBhAHQAZQByAGkAYQBsACAAdAB5AHAAZQAtAG4AYQBtAGUAPQAiAHIAYwBtAC0AYgBhAHMAaQBjAC0AbQBhAHQAZQByAGkAYQBsACIAIAByAGUAbgBkAGUAcgAtAGUAbgBnAGkAbgBlAC0AaQBkAD0AIgA5ADkAOQA5ADkAOQA5ADkALQA5ADkAOQA5AC0AOQA5ADkAOQAtADkAOQA5ADkALQA5ADkAOQA5ADkAOQA5ADkAOQA5ADkAOQAiACAAcABsAHUAZwAtAGkAbgAtAGkAZAA9AC"
    "IAMQA2ADUAOQAyAEQANQA4AC0ANABBADIARgAtADQAMAAxAEQALQBCAEYANQBFAC0AMwBCADgANwA3ADQAMQBDADEAQgAxAEIAIgAgAHQAeQBwAGUALQBpAGQAPQAiAEIAQQA1ADEAQwAwADAAMAAtAEIAQQA1ADEALQBDADAAMAAwAC0AQgBBADUAMQAtAEMAMABCAEEANQAxAEMAMAAwADAAMAAwACIAIABpAG4AcwB0AGEAbgBjAGUALQBpAGQAPQAiADQAQgA5ADcAMABCADYANQAtADQARAA4AEMALQA0ADQAOQAyAC0AOQAxAEEAOQAtADkARgBEADcARgBEAEUARQA5ADMAMAAxACIAIABpAG4AcwB0AGEAbgBjAGUALQBuAGEAbQBlAD0AIgBDAHUAcwB0AG8AbQAiACAAbgBvAHQAZQBzAD0AIgAiACAAdABhAGcAcwA9ACIAIgAg"
    "AGgAaQBkAGQAZQBuAD0AIgBmAGEAbABzAGUAIgAgAHIAZQBmAGUAcgBlAG4AYwBlAD0AIgBmAGEAbABzAGUAIgAgAGEAdQB0AG8ALQBkAGUAbABlAHQAZQA9ACIAZgBhAGwAcwBlACIAPgANAAoACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByAHMALQB2ADgAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgB2AGUAcgBzAGkAbwBuACIAIAB0AHkAcABlAD0AIgBpAG4AdAAiAD4AMgA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgBkAGkAZgBmAHUAcwBlACIAIAB0AHkAcABlAD0AIgBjAG8AbABvAHIAIg"
    "A+ADAALgA5ADgAMAAzADkAMgAxADUAOAAwADMAMQA0ADYANAAsADAALgA5ADgAMAAzADkAMgAxADUAOAAwADMAMQA0ADYANAAsADAALgA5ADgAMAAzADkAMgAxADUAOAAwADMAMQA0ADYANAAsADEAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAcwBwAGUAYwB1AGwAYQByACIAIAB0AHkAcABlAD0AIgBjAG8AbABvAHIAIgA+ADEALAAxACwAMQAsADEAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAcwBoAGkAbgBlACIAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwA"
    "ZQAiAD4AMAA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQAiACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADAAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAcgBlAGYAbABlAGMAdABpAHYAaQB0AHkAIgAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAwADwALwBwAGEAcgBhAG0AZQB0AGUAcgA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAGkAbwByACIAIAB0AH"
    "kAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMQA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQAtAGMAbwBsAG8AcgAiACAAdAB5AHAAZQA9ACIAYwBvAGwAbwByACIAPgAxACwAMQAsADEALAAxADwALwBwAGEAcgBhAG0AZQB0AGUAcgA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAHIAZQBmAGwAZQBjAHQAaQB2AGkAdAB5AC0AYwBvAGwAbwByACIAIAB0AHkAcABlAD0AIgBjAG8AbABvAHIAIgA+ADEALAAxACwAMQAsADEAPAAvAHAAYQByAGEAbQBlAHQAZQBy"
    "AD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAZQBtAGkAcwBzAGkAbwBuACIAIAB0AHkAcABlAD0AIgBjAG8AbABvAHIAIgA+ADAALAAwACwAMAAsADEAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAYQBtAGIAaQBlAG4AdAAiACAAdAB5AHAAZQA9ACIAYwBvAGwAbwByACIAPgAwACwAMAAsADAALAAxADwALwBwAGEAcgBhAG0AZQB0AGUAcgA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAGYAbABhAG0AaQBuAGcAbwAtAGwAaQBiAHIAYQByAHkAIgAgAHQAeQ"
    "BwAGUAPQAiAHMAdAByAGkAbgBnACIALwA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAGQAaQBzAGEAYgBsAGUALQBsAGkAZwBoAHQAaQBuAGcAIgAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgBmAHIAZQBzAG4AZQBsAC0AZQBuAGEAYgBsAGUAZAAiACAAdAB5AHAAZQA9ACIAYgBvAG8AbAAiAD4AZgBhAGwAcwBlADwALwBwAGEAcgBhAG0AZQB0AGUAcgA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUA"
    "PQAiAHAAbwBsAGkAcwBoAC0AYQBtAG8AdQBuAHQAIgAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwBwAGEAcgBhAG0AZQB0AGUAcgA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAGMAbABhAHIAaQB0AHkALQBhAG0AbwB1AG4AdAAiACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADEAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAYQBsAHAAaABhAC0AdAByAGEAbgBzAHAAYQByAGUAbgBjAHkAIgAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AHQAcgB1AGUAPAAvAHAAYQByAG"
    "EAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAYgBpAHQAbQBhAHAALQB0AGUAeAB0AHUAcgBlACIAIAB0AHkAcABlAD0AIgAiAC8APgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgBiAGkAdABtAGEAcAAtAHQAZQB4AHQAdQByAGUALQBvAG4AIgAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgBiAGkAdABtAGEAcAAtAHQAZQB4AHQAdQByAGUALQBkAG8AdQBiAGwAZQAtAGEAbQBv"
    "AHUAbgB0ACIAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMQA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgBiAGkAdABtAGEAcAAtAHQAZQB4AHQAdQByAGUALQBhAG0AbwB1AG4AdAAiACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADEAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAYgB1AG0AcAAtAHQAZQB4AHQAdQByAGUAIgAgAHQAeQBwAGUAPQAiACIALwA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQ"
    "AiAGIAdQBtAHAALQB0AGUAeAB0AHUAcgBlAC0AbwBuACIAIAB0AHkAcABlAD0AIgBiAG8AbwBsACIAPgBmAGEAbABzAGUAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAYgB1AG0AcAAtAHQAZQB4AHQAdQByAGUALQBkAG8AdQBiAGwAZQAtAGEAbQBvAHUAbgB0ACIAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMAAuADMAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAYgB1AG0AcAAtAHQAZQB4AHQAdQByAGUALQBhAG0AbwB1AG4AdAAiACAAdAB5AHAA"
    "ZQA9ACIAZABvAHUAYgBsAGUAIgA+ADAALgAzADwALwBwAGEAcgBhAG0AZQB0AGUAcgA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAHQAcgBhAG4AcwBwAGEAcgBlAG4AYwB5AC0AdABlAHgAdAB1AHIAZQAiACAAdAB5AHAAZQA9ACIAIgAvAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAdAByAGEAbgBzAHAAYQByAGUAbgBjAHkALQB0AGUAeAB0AHUAcgBlAC0AbwBuACIAIAB0AHkAcABlAD0AIgBiAG8AbwBsACIAPgBmAGEAbABzAGUAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByAC"
    "AAbgBhAG0AZQA9ACIAdAByAGEAbgBzAHAAYQByAGUAbgBjAHkALQB0AGUAeAB0AHUAcgBlAC0AZABvAHUAYgBsAGUALQBhAG0AbwB1AG4AdAAiACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADEAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAdAByAGEAbgBzAHAAYQByAGUAbgBjAHkALQB0AGUAeAB0AHUAcgBlAC0AYQBtAG8AdQBuAHQAIgAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwBwAGEAcgBhAG0AZQB0AGUAcgA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAGUAbgB2"
    "AGkAcgBvAG4AbQBlAG4AdAAtAHQAZQB4AHQAdQByAGUAIgAgAHQAeQBwAGUAPQAiACIALwA+AA0ACgAJAAkACQAJAAkAPABwAGEAcgBhAG0AZQB0AGUAcgAgAG4AYQBtAGUAPQAiAGUAbgB2AGkAcgBvAG4AbQBlAG4AdAAtAHQAZQB4AHQAdQByAGUALQBvAG4AIgAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAIABuAGEAbQBlAD0AIgBlAG4AdgBpAHIAbwBuAG0AZQBuAHQALQB0AGUAeAB0AHUAcgBlAC0AZABvAHUAYgBsAGUALQBhAG0AbwB1AG4AdAAiACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIg"
    "A+ADEAPAAvAHAAYQByAGEAbQBlAHQAZQByAD4ADQAKAAkACQAJAAkACQA8AHAAYQByAGEAbQBlAHQAZQByACAAbgBhAG0AZQA9ACIAZQBuAHYAaQByAG8AbgBtAGUAbgB0AC0AdABlAHgAdAB1AHIAZQAtAGEAbQBvAHUAbgB0ACIAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMQA8AC8AcABhAHIAYQBtAGUAdABlAHIAPgANAAoACQAJAAkACQA8AC8AcABhAHIAYQBtAGUAdABlAHIAcwAtAHYAOAA+AA0ACgAJAAkACQAJADwAcABhAHIAYQBtAGUAdABlAHIAcwA+AA0ACgAJAAkACQAJAAkAPAB2AGUAcgBzAGkAbwBuACAAdAB5AHAAZQA9ACIAaQBuAHQAIgA+ADIAPAAvAHYAZQByAHMAaQBvAG4APgANAAoACQAJAAkA"
    "CQAJADwAZABpAGYAZgB1AHMAZQAgAHQAeQBwAGUAPQAiAGMAbwBsAG8AcgAiAD4AMAAuADkAOAAwADMAOQAyADEANQA4ADAAMwAxADQANgA0ACwAMAAuADkAOAAwADMAOQAyADEANQA4ADAAMwAxADQANgA0ACwAMAAuADkAOAAwADMAOQAyADEANQA4ADAAMwAxADQANgA0ACwAMQA8AC8AZABpAGYAZgB1AHMAZQA+AA0ACgAJAAkACQAJAAkAPABzAHAAZQBjAHUAbABhAHIAIAB0AHkAcABlAD0AIgBjAG8AbABvAHIAIgA+ADEALAAxACwAMQAsADEAPAAvAHMAcABlAGMAdQBsAGEAcgA+AA0ACgAJAAkACQAJAAkAPABzAGgAaQBuAGUAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMAA8AC8AcwBoAGkAbgBlAD4ADQAKAA"
    "kACQAJAAkACQA8AHQAcgBhAG4AcwBwAGEAcgBlAG4AYwB5ACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADAAPAAvAHQAcgBhAG4AcwBwAGEAcgBlAG4AYwB5AD4ADQAKAAkACQAJAAkACQA8AHIAZQBmAGwAZQBjAHQAaQB2AGkAdAB5ACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADAAPAAvAHIAZQBmAGwAZQBjAHQAaQB2AGkAdAB5AD4ADQAKAAkACQAJAAkACQA8AGkAbwByACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADEAPAAvAGkAbwByAD4ADQAKAAkACQAJAAkACQA8AHQAcgBhAG4AcwBwAGEAcgBlAG4AYwB5AC0AYwBvAGwAbwByACAAdAB5AHAAZQA9ACIAYwBvAGwAbwByACIAPgAxACwAMQAs"
    "ADEALAAxADwALwB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQAtAGMAbwBsAG8AcgA+AA0ACgAJAAkACQAJAAkAPAByAGUAZgBsAGUAYwB0AGkAdgBpAHQAeQAtAGMAbwBsAG8AcgAgAHQAeQBwAGUAPQAiAGMAbwBsAG8AcgAiAD4AMQAsADEALAAxACwAMQA8AC8AcgBlAGYAbABlAGMAdABpAHYAaQB0AHkALQBjAG8AbABvAHIAPgANAAoACQAJAAkACQAJADwAZQBtAGkAcwBzAGkAbwBuACAAdAB5AHAAZQA9ACIAYwBvAGwAbwByACIAPgAwACwAMAAsADAALAAxADwALwBlAG0AaQBzAHMAaQBvAG4APgANAAoACQAJAAkACQAJADwAYQBtAGIAaQBlAG4AdAAgAHQAeQBwAGUAPQAiAGMAbwBsAG8AcgAiAD4AMAAsADAALAAwACwAMQ"
    "A8AC8AYQBtAGIAaQBlAG4AdAA+AA0ACgAJAAkACQAJAAkAPABmAGwAYQBtAGkAbgBnAG8ALQBsAGkAYgByAGEAcgB5ACAAdAB5AHAAZQA9ACIAcwB0AHIAaQBuAGcAIgAvAD4ADQAKAAkACQAJAAkACQA8AGQAaQBzAGEAYgBsAGUALQBsAGkAZwBoAHQAaQBuAGcAIAB0AHkAcABlAD0AIgBiAG8AbwBsACIAPgBmAGEAbABzAGUAPAAvAGQAaQBzAGEAYgBsAGUALQBsAGkAZwBoAHQAaQBuAGcAPgANAAoACQAJAAkACQAJADwAZgByAGUAcwBuAGUAbAAtAGUAbgBhAGIAbABlAGQAIAB0AHkAcABlAD0AIgBiAG8AbwBsACIAPgBmAGEAbABzAGUAPAAvAGYAcgBlAHMAbgBlAGwALQBlAG4AYQBiAGwAZQBkAD4ADQAKAAkACQAJAAkA"
    "CQA8AHAAbwBsAGkAcwBoAC0AYQBtAG8AdQBuAHQAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMQA8AC8AcABvAGwAaQBzAGgALQBhAG0AbwB1AG4AdAA+AA0ACgAJAAkACQAJAAkAPABjAGwAYQByAGkAdAB5AC0AYQBtAG8AdQBuAHQAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMQA8AC8AYwBsAGEAcgBpAHQAeQAtAGEAbQBvAHUAbgB0AD4ADQAKAAkACQAJAAkACQA8AGEAbABwAGgAYQAtAHQAcgBhAG4AcwBwAGEAcgBlAG4AYwB5ACAAdAB5AHAAZQA9ACIAYgBvAG8AbAAiAD4AdAByAHUAZQA8AC8AYQBsAHAAaABhAC0AdAByAGEAbgBzAHAAYQByAGUAbgBjAHkAPgANAAoACQAJAAkACQAJADwAYgBpAH"
    "QAbQBhAHAALQB0AGUAeAB0AHUAcgBlACAAdAB5AHAAZQA9ACIAIgAvAD4ADQAKAAkACQAJAAkACQA8AGIAaQB0AG0AYQBwAC0AdABlAHgAdAB1AHIAZQAtAG8AbgAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AYgBpAHQAbQBhAHAALQB0AGUAeAB0AHUAcgBlAC0AbwBuAD4ADQAKAAkACQAJAAkACQA8AGIAaQB0AG0AYQBwAC0AdABlAHgAdAB1AHIAZQAtAGQAbwB1AGIAbABlAC0AYQBtAG8AdQBuAHQAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMQA8AC8AYgBpAHQAbQBhAHAALQB0AGUAeAB0AHUAcgBlAC0AZABvAHUAYgBsAGUALQBhAG0AbwB1AG4AdAA+AA0ACgAJAAkACQAJAAkAPABi"
    "AGkAdABtAGEAcAAtAHQAZQB4AHQAdQByAGUALQBhAG0AbwB1AG4AdAAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwBiAGkAdABtAGEAcAAtAHQAZQB4AHQAdQByAGUALQBhAG0AbwB1AG4AdAA+AA0ACgAJAAkACQAJAAkAPABiAHUAbQBwAC0AdABlAHgAdAB1AHIAZQAgAHQAeQBwAGUAPQAiACIALwA+AA0ACgAJAAkACQAJAAkAPABiAHUAbQBwAC0AdABlAHgAdAB1AHIAZQAtAG8AbgAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AYgB1AG0AcAAtAHQAZQB4AHQAdQByAGUALQBvAG4APgANAAoACQAJAAkACQAJADwAYgB1AG0AcAAtAHQAZQB4AHQAdQByAGUALQBkAG8AdQBiAGwAZQ"
    "AtAGEAbQBvAHUAbgB0ACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADAALgAzADwALwBiAHUAbQBwAC0AdABlAHgAdAB1AHIAZQAtAGQAbwB1AGIAbABlAC0AYQBtAG8AdQBuAHQAPgANAAoACQAJAAkACQAJADwAYgB1AG0AcAAtAHQAZQB4AHQAdQByAGUALQBhAG0AbwB1AG4AdAAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAwAC4AMwA8AC8AYgB1AG0AcAAtAHQAZQB4AHQAdQByAGUALQBhAG0AbwB1AG4AdAA+AA0ACgAJAAkACQAJAAkAPAB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQAtAHQAZQB4AHQAdQByAGUAIAB0AHkAcABlAD0AIgAiAC8APgANAAoACQAJAAkACQAJADwAdAByAGEAbgBzAHAAYQByAGUA"
    "bgBjAHkALQB0AGUAeAB0AHUAcgBlAC0AbwBuACAAdAB5AHAAZQA9ACIAYgBvAG8AbAAiAD4AZgBhAGwAcwBlADwALwB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQAtAHQAZQB4AHQAdQByAGUALQBvAG4APgANAAoACQAJAAkACQAJADwAdAByAGEAbgBzAHAAYQByAGUAbgBjAHkALQB0AGUAeAB0AHUAcgBlAC0AZABvAHUAYgBsAGUALQBhAG0AbwB1AG4AdAAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQAtAHQAZQB4AHQAdQByAGUALQBkAG8AdQBiAGwAZQAtAGEAbQBvAHUAbgB0AD4ADQAKAAkACQAJAAkACQA8AHQAcgBhAG4AcwBwAGEAcgBlAG4AYwB5AC0AdABlAH"
    "gAdAB1AHIAZQAtAGEAbQBvAHUAbgB0ACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADEAPAAvAHQAcgBhAG4AcwBwAGEAcgBlAG4AYwB5AC0AdABlAHgAdAB1AHIAZQAtAGEAbQBvAHUAbgB0AD4ADQAKAAkACQAJAAkACQA8AGUAbgB2AGkAcgBvAG4AbQBlAG4AdAAtAHQAZQB4AHQAdQByAGUAIAB0AHkAcABlAD0AIgAiAC8APgANAAoACQAJAAkACQAJADwAZQBuAHYAaQByAG8AbgBtAGUAbgB0AC0AdABlAHgAdAB1AHIAZQAtAG8AbgAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AZQBuAHYAaQByAG8AbgBtAGUAbgB0AC0AdABlAHgAdAB1AHIAZQAtAG8AbgA+AA0ACgAJAAkACQAJAAkAPABl"
    "AG4AdgBpAHIAbwBuAG0AZQBuAHQALQB0AGUAeAB0AHUAcgBlAC0AZABvAHUAYgBsAGUALQBhAG0AbwB1AG4AdAAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwBlAG4AdgBpAHIAbwBuAG0AZQBuAHQALQB0AGUAeAB0AHUAcgBlAC0AZABvAHUAYgBsAGUALQBhAG0AbwB1AG4AdAA+AA0ACgAJAAkACQAJAAkAPABlAG4AdgBpAHIAbwBuAG0AZQBuAHQALQB0AGUAeAB0AHUAcgBlAC0AYQBtAG8AdQBuAHQAIAB0AHkAcABlAD0AIgBkAG8AdQBiAGwAZQAiAD4AMQA8AC8AZQBuAHYAaQByAG8AbgBtAGUAbgB0AC0AdABlAHgAdAB1AHIAZQAtAGEAbQBvAHUAbgB0AD4ADQAKAAkACQAJAAkAPAAvAHAAYQByAGEAbQ"
    "BlAHQAZQByAHMAPgANAAoACQAJAAkACQA8AHMAaQBtAHUAbABhAHQAaQBvAG4APgANAAoACQAJAAkACQAJADwAYQBtAGIAaQBlAG4AdAAgAHQAeQBwAGUAPQAiAGMAbwBsAG8AcgAiAD4AMAAsADAALAAwACwAMQA8AC8AYQBtAGIAaQBlAG4AdAA+AA0ACgAJAAkACQAJAAkAPABkAGkAZgBmAHUAcwBlACAAdAB5AHAAZQA9ACIAYwBvAGwAbwByACIAPgAwAC4AOQA4ADAAMwA5ADIAMQA1ADgAMAAzADEANAA2ADQALAAwAC4AOQA4ADAAMwA5ADIAMQA1ADgAMAAzADEANAA2ADQALAAwAC4AOQA4ADAAMwA5ADIAMQA1ADgAMAAzADEANAA2ADQALAAxADwALwBkAGkAZgBmAHUAcwBlAD4ADQAKAAkACQAJAAkACQA8AGUAbQBpAHMA"
    "cwBpAG8AbgAgAHQAeQBwAGUAPQAiAGMAbwBsAG8AcgAiAD4AMAAsADAALAAwACwAMQA8AC8AZQBtAGkAcwBzAGkAbwBuAD4ADQAKAAkACQAJAAkACQA8AHMAcABlAGMAdQBsAGEAcgAgAHQAeQBwAGUAPQAiAGMAbwBsAG8AcgAiAD4AMQAsADEALAAxACwAMQA8AC8AcwBwAGUAYwB1AGwAYQByAD4ADQAKAAkACQAJAAkACQA8AHIAZQBmAGwAZQBjAHQAaQBvAG4AIAB0AHkAcABlAD0AIgBjAG8AbABvAHIAIgA+ADEALAAxACwAMQAsADEAPAAvAHIAZQBmAGwAZQBjAHQAaQBvAG4APgANAAoACQAJAAkACQAJADwAcwBoAGkAbgBlACAAdAB5AHAAZQA9ACIAZABvAHUAYgBsAGUAIgA+ADAAPAAvAHMAaABpAG4AZQA+AA0ACgAJAA"
    "kACQAJAAkAPAB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAwADwALwB0AHIAYQBuAHMAcABhAHIAZQBuAGMAeQA+AA0ACgAJAAkACQAJAAkAPAByAGUAZgBsAGUAYwB0AGkAdgBpAHQAeQAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAwADwALwByAGUAZgBsAGUAYwB0AGkAdgBpAHQAeQA+AA0ACgAJAAkACQAJAAkAPABpAG8AcgAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwBpAG8AcgA+AA0ACgAJAAkACQAJAAkAPABmAHIAZQBzAG4AZQBsAC0AZQBuAGEAYgBsAGUAZAAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AZgBy"
    "AGUAcwBuAGUAbAAtAGUAbgBhAGIAbABlAGQAPgANAAoACQAJAAkACQAJADwAcABvAGwAaQBzAGgALQBhAG0AbwB1AG4AdAAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwBwAG8AbABpAHMAaAAtAGEAbQBvAHUAbgB0AD4ADQAKAAkACQAJAAkACQA8AGMAbABhAHIAaQB0AHkALQBhAG0AbwB1AG4AdAAgAHQAeQBwAGUAPQAiAGQAbwB1AGIAbABlACIAPgAxADwALwBjAGwAYQByAGkAdAB5AC0AYQBtAG8AdQBuAHQAPgANAAoACQAJAAkACQAJADwAdAByAGEAbgBzAHAAYQByAGUAbgB0ACAAdAB5AHAAZQA9ACIAYwBvAGwAbwByACIAPgAxACwAMQAsADEALAAxADwALwB0AHIAYQBuAHMAcABhAHIAZQBuAHQAPg"
    "ANAAoACQAJAAkACQAJADwAaQBzAC0AcABoAHkAcwBpAGMAYQBsAGwAeQAtAGIAYQBzAGUAZAAgAHQAeQBwAGUAPQAiAGIAbwBvAGwAIgA+AGYAYQBsAHMAZQA8AC8AaQBzAC0AcABoAHkAcwBpAGMAYQBsAGwAeQAtAGIAYQBzAGUAZAA+AA0ACgAJAAkACQAJADwALwBzAGkAbQB1AGwAYQB0AGkAbwBuAD4ADQAKAAkACQAJADwALwBtAGEAdABlAHIAaQBhAGwAPgANAAoACQAJADwALwBtAGEAdABlAHIAaQBhAGwALQBzAGUAYwB0AGkAbwBuAD4ADQAKAAkAPAAvAHIAZQBuAGQAZQByAC0AYwBvAG4AdABlAG4AdAAtAG0AYQBuAGEAZwBlAHIALQBkAG8AYwB1AG0AZQBuAHQAPgANAAoAPAAvAHgAbQBsAD4ADQAKAA==";

    const auto len = wcslen(wszBase64);
    auto* p = new char[len+1];
    memset(p, 0, len);
    MYON_Base64::Decode(wszBase64, p);
#ifdef MYON_RUNTIME_WIN
    sXML = reinterpret_cast<const wchar_t*>(p);
#else
    const auto* pUTF16 = (MYON__UINT16*)p;
    unsigned int error_status = 0;
    int error_mask = 0, error_code_point = 0;
    const auto num_chars = MYON_ConvertUTF16ToUTF32(false, pUTF16, -1, nullptr, 0, &error_status, error_mask, error_code_point, nullptr);
    auto* string_buf = sXML.SetLength(num_chars);
    MYON_ASSERT(sizeof(wchar_t) == sizeof(MYON__UINT32));
    auto* pWide = reinterpret_cast<MYON__UINT32*>(string_buf);
    MYON_ConvertUTF16ToUTF32(false, pUTF16, -1, pWide, num_chars+1, &error_status, error_mask, error_code_point, nullptr);
#endif
    delete[] p;
}

static const auto*  szQuick =  "The quick brown fox jumps over the lazy dog";
static const auto* wszQuick = L"The quick brown fox jumps over the lazy dog";

bool MYON_RunXMLTests(const wchar_t* test_folder)
{
//  ::SetThreadAffinityMask(::GetCurrentThread(), 0x01);

  // Remember the current node and property counts.
  const auto nodeCount = long(g_lNodeCount);
  const auto propCount = long(g_lPropertyCount);

  // Test variant.
  {
    MYON_wString s = L"123.456e-10";
    Validate(s.IsValidRealNumber());
    Validate(!s.IsValidIntegerNumber());
  }
  {
    MYON_wString s = L"123456";
    Validate(s.IsValidIntegerNumber());
    Validate(s.IsValidRealNumber());
  }
  {
    MYON_wString s = L"Quack";
    Validate(!s.IsValidIntegerNumber());
    Validate(!s.IsValidRealNumber());
  }
  {
    MYON_XMLVariant v(true);
    Validate(v.AsBool() == true);
    Validate(v.AsInteger() == 1);
    Validate(v.AsFloat() == 1.0f);
    Validate(v.AsDouble() == 1.0);
    Validate(v.AsString() == L"true");
  }
  {
    MYON_XMLVariant v(25);
    Validate(v.AsBool() == true);
    Validate(v.AsInteger() == 25);
    Validate(v.AsFloat() == 25.0f);
    Validate(v.AsDouble() == 25.0);
    Validate(v.AsString() == L"25");
  }
  {
    MYON_XMLVariant v(63.5f);
    Validate(v.AsBool() == true);
    Validate(v.AsInteger() == 63);
    Validate(v.AsFloat() == 63.5f);
    Validate(v.AsDouble() == 63.5);
    Validate(v.AsString() == L"63.5");
  }
  {
    MYON_XMLVariant v(77.9);
    Validate(v.AsBool() == true);
    Validate(v.AsInteger() == 77);
    Validate(v.AsFloat() == 77.9f);
    Validate(v.AsDouble() == 77.9);
    Validate(v.AsString() == L"77.9");
  }
  {
    MYON_4fColor col(MYON_Color(12, 34, 56, 0));
    MYON_XMLVariant v(col);
    Validate(v.AsColor().Compare(col) == 0);

    auto s = v.AsString();
    auto pos = s.Find(L",");
    auto s1 = s.Left(pos);
    s = s.Mid(pos+1);
    Validate(s1.Left(8) == L"0.047058");
    pos = s.Find(L",");
    s1 = s.Left(pos);
    s = s.Mid(pos+1);
    Validate(s1.Left(8) == L"0.133333");
    pos = s.Find(L",");
    s1 = s.Left(pos);
    s = s.Mid(pos+1);
    Validate(s1.Left(8) == L"0.219607");
    Validate(s == L"1");

    const auto p = v.As4dPoint();
    Validate(p[0] == col.Red());
    Validate(p[1] == col.Green());
    Validate(p[2] == col.Blue());
    Validate(p[3] == col.Alpha());
  }
  {
    MYON_XMLVariant v(L"123, 456");
    const auto p = v.As2dPoint();
    Validate(p[0] == 123.0);
    Validate(p[1] == 456.0);
    Validate(v.AsString().Length() == 8);
    Validate(v.AsString() == L"123, 456");
  }
  {
    MYON_XMLVariant v(L"123, 456, 789");
    const auto p = v.As3dPoint();
    Validate(p[0] == 123.0);
    Validate(p[1] == 456.0);
    Validate(p[2] == 789.0);
    Validate(v.AsString().Length() == 13);
    Validate(v.AsString() == L"123, 456, 789");
  }
  {
    MYON_XMLVariant v(L"123, 2.4, 33.9, 456.");
    const auto p = v.As4dPoint();
    Validate(p[0] == 123.0);
    Validate(p[1] == 2.4);
    Validate(p[2] == 33.9);
    Validate(p[3] == 456.0);
    Validate(v.AsString().Length() == 20);
    Validate(v.AsString() == L"123, 2.4, 33.9, 456.");
  }
  {
    MYON_wString sTime = L"2021.11.21_02:34:00";
    MYON_XMLVariant v1(sTime);
    auto time = v1.AsTime();
    MYON_XMLVariant v2(time);
    Validate(v2.AsString().Length() == 19);
    Validate(v2.AsString() == sTime);
  }
  {
    MYON_Buffer b;
    b.Write(43, szQuick);
    MYON_XMLVariant v1(b);
    const auto& s = v1.AsString();
    Validate(s == L"base64:VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==");
  }

  const auto* wszSentence = L"The quick brown fox";

  const MYON_wString sSentence = wszSentence;

  // Test new MYON_wString functions.
  for (int i = 0; i < sSentence.Length(); i++)
  {
    MYON_wString s = sSentence;
    if (s.TruncateMid(i))
    {
      Validate(s == sSentence.Mid(i));
    }
  }
 
  {
    Validate(sSentence.StartsWith(L"The quick"));
    Validate(sSentence.StartsWithNoCase(L"the quick"));
    Validate(sSentence.StartsWithNoCase(L"THE QUICK"));
  }
  {
    MYON_wString s = sSentence;
    MYON_wString t = s; // t is a new reference to the same string.
    Validate(s.Insert(3, L'!', 4));
    Validate(s == L"The!!!! quick brown fox"); // Validate that chars were inserted.
    Validate(t == wszSentence);                // Validate that 't' is not corrupted.

    t = s; // t is a new reference to the same string.
    Validate(s.Insert(s.Length(), L'!', 1));
    Validate(s == L"The!!!! quick brown fox!"); // Validate that chars were inserted.
    Validate(t == L"The!!!! quick brown fox");  // Validate that 't' is not corrupted.

    s = sSentence + L"!";
    t = s; // t is a new reference to the same string.
    Validate(s.Insert(s.Length()-1, L" jumps over the lazy dog"));
    Validate(s == L"The quick brown fox jumps over the lazy dog!"); // Validate that chars were inserted.
    Validate(t == L"The quick brown fox!");                         // Validate that 't' is not corrupted.

    MYON_wString z;
    Validate( z.Insert(0, 'x', 1));
    Validate(!z.Insert(0, 'x', -1));
    Validate(!z.Insert(-1, 'x', 1));
    Validate(!z.Insert(-1, L"x"));
    Validate(!z.Insert(0, nullptr));
  }

  // Test XML node.
  {
    MYON_XMLNode node(L"parent");
    node.AttachChildNode(new MYON_XMLNode(L"child"));
    auto s = node.String();
    s.RemoveWhiteSpace();
    Validate(s == L"<parent><child/></parent>");
  }

  // Test XML properties.
  {
    const auto* szPropName1 = L"number";
    const auto* szPropName2 = L"time";
    MYON_XMLNode node(L"parent");
    MYON_XMLProperty prop1(szPropName1, 25);
    node.AddProperty(prop1);
    const auto time = time_t(617283945);
    MYON_XMLProperty prop2(szPropName2, time);
    node.AddProperty(prop2);
    auto s = node.String();
    s.RemoveWhiteSpace();
    Validate(s == L"<parenttime=\"1989.07.24_04:45:45\"number=\"25\"/>");

    auto pi = node.GetPropertyIterator(true); // Sorted.
    auto* pProp = pi.GetNextProperty();
    Validate((nullptr != pProp) && pProp->Name().IsEmpty());
    pProp = pi.GetNextProperty();
    Validate((nullptr != pProp) && (pProp->Name() == szPropName1));
    pProp = pi.GetNextProperty();
    Validate((nullptr != pProp) && (pProp->Name() == szPropName2));
    pProp = pi.GetNextProperty();
    Validate(nullptr == pProp);

    Validate(node.RemoveProperty(szPropName1));
    Validate(node.RemoveProperty(szPropName2));
    s = node.String();
    s.RemoveWhiteSpace();
    Validate(s == L"<parent/>");
    node.AddProperty(prop1);

    pProp = node.GetNamedProperty(szPropName1);
    Validate(pProp != nullptr);
    if (pProp != nullptr)
      pProp->SetValue(23.4);
    s = node.String();
    s.RemoveWhiteSpace();
    Validate(s == L"<parentnumber=\"23.4\"/>");

    node.RemoveAllProperties();
    s = node.String();
    s.RemoveWhiteSpace();
    Validate(s == L"<parent/>");
    node.GetDefaultProperty().SetValue(true);
    s = node.String();
    s.RemoveWhiteSpace();
    Validate(s == L"<parent>true</parent>");
  }

  // Test Unicode text file.
  const MYON_wString sFolder = test_folder;
  {
    const auto sFile = sFolder + L"UTF8.txt";
    MYON_UnicodeTextFile file(MYON_UnicodeTextFile::Types::UTF8);

    if (file.Open(sFile, MYON_UnicodeTextFile::Modes::Write))
    {
      Validate(file.WriteString(wszQuick));
      Validate(file.Close());
    }

    if (file.Open(sFile, MYON_UnicodeTextFile::Modes::Read))
    {
      MYON_wString s;
      Validate(file.ReadString(s));
      Validate(s == wszQuick);
      Validate(file.Close());
    }
  }
  {
    const auto sFile = sFolder + L"UTF16.txt";
    MYON_UnicodeTextFile file(MYON_UnicodeTextFile::Types::UTF16);

    if (file.Open(sFile, MYON_UnicodeTextFile::Modes::Write))
    {
      Validate(file.WriteString(wszQuick));
      Validate(file.Close());
    }

    if (file.Open(sFile, MYON_UnicodeTextFile::Modes::Read))
    {
      MYON_wString s;
      Validate(file.ReadString(s));
      Validate(s == wszQuick);
      Validate(file.Close());
    }
  }

  // Test XML root node.
  {
    const auto* szPropName1 = L"number";
    const auto* szPropName2 = L"time";
    MYON_XMLRootNode root;
    MYON_XMLProperty prop1(szPropName1, 25);
    root.AddProperty(prop1);
    const auto time = time_t(617283945);
    MYON_XMLProperty prop2(szPropName2, time);
    root.AddProperty(prop2);
    auto s = root.String();
    s.RemoveWhiteSpace();
    Validate(s == L"<xmltime=\"1989.07.24_04:45:45\"number=\"25\"/>");

    const auto sFile = sFolder + MYON_FileSystemPath::DirectorySeparator + L"UTF16-2.txt";
    Validate(root.WriteToFile(sFile));
    Validate(root.ReadFromFile(sFile));
  }

  {
    MYON_wString sXML;
    GetRDKTestXML(sXML);

    MYON_XMLRootNode root;
    root.ReadFromStream(sXML.Array());
    auto* pNode = root.GetNodeAtPath(L"render-content-manager-document/material-section");
    Validate(pNode != nullptr);

    const auto sFile = sFolder + MYON_FileSystemPath::DirectorySeparator + L"UTF16-3.txt";
    Validate(root.WriteToFile(sFile));
  }

#ifdef MYON_RUNTIME_WIN
  // Base 64 speed test.
  LARGE_INTEGER start, now, freq;
  ::QueryPerformanceFrequency(&freq);

  const auto* buf = szQuick;
  const auto buf_len = strlen(buf);

  MYON_wString sBase64;

#ifdef _DEBUG
  const int numIterations = 100000;
#else
  const int numIterations = 100000000;
#endif

  ::QueryPerformanceCounter(&start);
  for (int i = 0; i < numIterations; i++)
  {
    MYON_Base64::Encode(buf, buf_len, sBase64, false);
  }
  ::QueryPerformanceCounter(&now);

  MYON_wString sOutput;
  sOutput.Format(L"Time for %u Base64 iterations: %g seconds", numIterations, double(now.QuadPart - start.QuadPart) / double(freq.QuadPart));
  OUTPUT_DEBUG_STRING_EOL(sOutput);

  sBase64.Empty();
  MYON_Base64EncodeStream bes;
  bes.SetCallback(Base64Callback, nullptr);

  ::QueryPerformanceCounter(&start);
  for (int i = 0; i < numIterations; i++)
  {
    CallbackDone = 0;
    bes.Begin();
    bes.In(buf_len, buf);
    bes.End();
  }
  ::QueryPerformanceCounter(&now);

  sOutput.Format(L"Time for %u Base64 iterations: %g seconds", numIterations, double(now.QuadPart - start.QuadPart) / double(freq.QuadPart));
  OUTPUT_DEBUG_STRING_EOL(sOutput);

  sBase64 = CallbackBuffer;
#endif

  // Test critical section.
  OUTPUT_DEBUG_STRING_EOL(L"Characters should not be mixed");
  std::thread th1(ThreadFunc, L'*');
  std::thread th2(ThreadFunc, L'$');
  th1.join();
  th2.join();

  // Check for node and property leaks.
  Validate(nodeCount == long(g_lNodeCount));
  Validate(propCount == long(g_lPropertyCount));

  return g_bXMLTestsOK;
}

#pragma warning (pop)

//-------------------------------------------------------------------------------------------------------------------
MYON_UUID uuidPostEffect_ToneMapper_Clamp   = { 0xacb8d258, 0xc1d6, 0x499d, { 0xaa, 0x23, 0x02, 0xdc, 0xde, 0xa2, 0xb0, 0xa2 } };
MYON_UUID uuidPostEffect_Gamma              = { 0x84c0798d, 0xc43a, 0x4402, { 0x88, 0x91, 0xe0, 0xc8, 0x08, 0x8e, 0x67, 0xca } };
MYON_UUID chanRGBA                          = { 0x453a9a1c, 0x9307, 0x4976, { 0xb2, 0x82, 0x4e, 0xad, 0x4d, 0x53, 0x98, 0x79 } };
MYON_UUID chanDistanceFromCamera            = { 0xb752ce0b, 0xc219, 0x4bdd, { 0xb1, 0x34, 0x26, 0x42, 0x5e, 0x1c, 0x43, 0x31 } };
MYON_UUID uuidRenderSettingsPreset_Studio   = { 0x5898cc05, 0x4202, 0x4dfb, { 0x83, 0xfe, 0x8f, 0xa8, 0x8f, 0x91, 0xc7, 0xd6 } };
MYON_UUID uuidRenderSettingsPreset_Custom   = { 0xc89a74fb, 0x1451, 0x4a9b, { 0xb8, 0x7d, 0xe3, 0x0f, 0xf3, 0x51, 0x0f, 0x96 } };
MYON_UUID uuidRenderSettingsPreset_Exterior = { 0x1346FE79, 0xBF49, 0x4BB6, { 0x86, 0xF4, 0xF2, 0xC2, 0x81, 0xD1, 0xD5, 0x5A } };
MYON_UUID uuidRenderSettingsPreset_Interior = { 0x14A1D7E9, 0xC75D, 0x464D, { 0xBB, 0x81, 0x38, 0x1C, 0xA2, 0xF1, 0xC9, 0x58 } };

//-------------------------------------------------------------------------------------------------------------------

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC(4456) // Declaration hides previous local declaration

class CreateXMLException
{
};

static MYON_XMLNode& Create(MYON_XMLNode& node, const wchar_t* s)
{
  auto* child_node = node.CreateNodeAtPath(s);
  if (nullptr == child_node)
    throw CreateXMLException(); // This should never, ever happen.

  return *child_node;
}

MYON_RdkDocumentDefaults::MYON_RdkDocumentDefaults(int version, ValueSets vs, void*)
  :
  _vs(vs),
  _major_version(version),
  _reserved(nullptr)
{
  try
  {
    CreateXML();
  }
  catch (CreateXMLException)
  {
    MYON_ERROR("CRITICAL - Failed to create default XML");
  }
}

void MYON_RdkDocumentDefaults::CreateXML(void)
{
  auto& doc = Create(_root, MYON_RDK_DOCUMENT);
  {
    if (ValueSets::All == _vs)
    {
      Create(doc, MYON_RDK_CURRENT_CONTENT).CreateNodeAtPath(MYON_RDK_ENVIRONMENT);
      Create(doc, MYON_RDK_DEFAULT_CONTENT_SECTION);
    }

    auto& settings = Create(doc, MYON_RDK_SETTINGS);
    {
      if (ValueSets::All == _vs)
      {
        // Named items.
        MYON_XMLParameters(Create(settings, MYON_RDK_NAMED_VIEWS    )).SetParam(MYON_RDK_SORT_MODE, L"");
        MYON_XMLParameters(Create(settings, MYON_RDK_NAMED_CPLANES  )).SetParam(MYON_RDK_SORT_MODE, L"");
        MYON_XMLParameters(Create(settings, MYON_RDK_NAMED_POSITIONS)).SetParam(MYON_RDK_SORT_MODE, L"");
        MYON_XMLParameters(Create(settings, MYON_RDK_NAMED_SNAPSHOTS)).SetParam(MYON_RDK_SORT_MODE, L"");

        // Miscellaneous.
        auto& misc = Create(settings, MYON_RDK_MISCELLANEOUS);
        MYON_XMLParameters(misc).SetParam(MYON_RDK_CUSTOM_IMAGE_SIZE_IS_PRESET, false);
        {
          // Name collision suppression.
          MYON_XMLParameters p(Create(misc, MYON_RDK_NAME_COLLISION_SUPPRESS));
          p.SetParam(MYON_RDK_IMPORT, false);
          p.SetParam(MYON_RDK_PASTE , false);
        }

        // Excluded render engines.
        MYON_XMLParameters p(Create(settings, MYON_RDK_EXCLUDED_RENDER_ENGINES));
        p.SetParam(MYON_RDK_UUIDS, L"");

        // Filters.
        MYON_XMLParameters f(Create(settings, MYON_RDK_FILTERS));
        f.SetParam(MYON_RDK_NAME_FILTER, L"");
        f.SetParam(MYON_RDK_NAME_FILTER_INVERT, false);
        f.SetParam(MYON_RDK_SHOW_UNASSIGNED, true);
        f.SetParam(MYON_RDK_SHOW_V4, true);
        f.SetParam(MYON_RDK_SHOW_HIDDEN, false);
        f.SetParam(MYON_RDK_SHOW_REFERENCE, false);

        // Post effects.
        auto& peps = Create(settings, MYON_RDK_POST_EFFECTS);
        {
          MYON_XMLParameters p(peps);
          p.SetParam(MYON_RDK_PEP_EARLY_SELECTION, MYON_nil_uuid);
          p.SetParam(MYON_RDK_PEP_TONE_SELECTION, uuidPostEffect_ToneMapper_Clamp);
          p.SetParam(MYON_RDK_PEP_LATE_SELECTION, uuidPostEffect_Gamma);

          Create(peps, MYON_RDK_PEP_TYPE_EARLY);
          Create(peps, MYON_RDK_PEP_TYPE_TONE);
          Create(peps, MYON_RDK_PEP_TYPE_LATE);
        }
      }

      // Rendering section.
      auto& rendering = Create(settings, MYON_RDK_RENDERING);
      {
        if (ValueSets::All == _vs)
        {
          // Render channels.
          auto& render_channels = Create(rendering, MYON_RDK_RENDER_CHANNELS);
          {
            MYON_wString a, b;
            MYON_UuidToString(chanRGBA, a);
            MYON_UuidToString(chanDistanceFromCamera, b);
            a.MakeUpper(); b.MakeUpper();
            MYON_XMLParameters p(render_channels);
            p.SetParam(MYON_RDK_RCH_LIST, a + L";" + b);
            p.SetParam(MYON_RDK_RCH_MODE, MYON_RDK_RCH_MODE_AUTOMATIC);
          }

          // Misc rendering settings.
          MYON_XMLParameters p(rendering);
          p.SetParam(MYON_RDK_EMBED_SUPPORT_FILES_ON, true);
          p.SetParam(MYON_RDK_DITHERING, MYON_RDK_DITHERING_FLOYD_STEINBERG);
          p.SetParam(MYON_RDK_USE_DITHERING, false);
          p.SetParam(MYON_RDK_GAMMA, (_major_version < 6) ? 1.0f : 2.2f);
          p.SetParam(MYON_RDK_USE_POST_PROCESS_GAMMA, true);
          p.SetParam(MYON_RDK_USE_LINEAR_WORKFLOW, (_major_version < 6) ? false : true);
          p.SetParam(MYON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT, MYON_nil_uuid);
          p.SetParam(MYON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT_ON, (_major_version < 6) ? false : true);
          p.SetParam(MYON_RDK_CURRENT_PRESET, (_major_version < 8) ? uuidRenderSettingsPreset_Custom
                                                                 : uuidRenderSettingsPreset_Studio);
        }
        else
        {
          MYON_XMLParameters p(rendering);

          if (_major_version < 6)
          {
            p.SetParam(MYON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT_ON, false);
          }

          if (_major_version < 8)
          {
            p.SetParam(MYON_RDK_CURRENT_PRESET, uuidRenderSettingsPreset_Custom);
          }
        }
      }

      if (ValueSets::All == _vs)
      {
        // Sun.
        auto& sun = Create(settings, MYON_RDK_SUN);
        {
          MYON_XMLParameters p(sun);
          p.SetParam(MYON_RDK_SUN_ENABLE_ON, false);
          p.SetParam(MYON_RDK_SUN_MANUAL_CONTROL_ON, false);
          p.SetParam(MYON_RDK_SUN_MANUAL_CONTROL_ALLOWED, true);
          p.SetParam(MYON_RDK_SUN_AZIMUTH, 0.0);
          p.SetParam(MYON_RDK_SUN_ALTITUDE, 0.0);
          p.SetParam(MYON_RDK_SUN_DATE_YEAR, 2000);
          p.SetParam(MYON_RDK_SUN_DATE_MONTH, 1);
          p.SetParam(MYON_RDK_SUN_DATE_DAY, 1);
          p.SetParam(MYON_RDK_SUN_TIME_HOURS, 12.0);
          p.SetParam(MYON_RDK_SUN_DAYLIGHT_SAVING_ON, false);
          p.SetParam(MYON_RDK_SUN_DAYLIGHT_SAVING_MINUTES, 60);
          p.SetParam(MYON_RDK_SUN_SHADOW_INTENSITY, 1.0);
          p.SetParam(MYON_RDK_SUN_INTENSITY, 1.0);
          p.SetParam(MYON_RDK_SUN_OBSERVER_LATITUDE, 0.0);
          p.SetParam(MYON_RDK_SUN_OBSERVER_LONGITUDE, 0.0);
          p.SetParam(MYON_RDK_SUN_OBSERVER_TIMEZONE, 0.0);
          p.SetParam(MYON_RDK_SUN_SKYLIGHT_ON, (_major_version < 6) ? false : true);
          p.SetParam(MYON_RDK_SUN_SKYLIGHT_SHADOW_INTENSITY, 1.0);
          p.SetParam(MYON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON, (_major_version < 6) ? false : true);
          p.SetParam(MYON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT, MYON_nil_uuid);
        }

        // Safe frame.
        auto& safe_frame = Create(settings, MYON_RDK_SAFE_FRAME);
        {
          MYON_XMLParameters p(safe_frame);
          p.SetParam(MYON_RDK_SF_ON, false);
          p.SetParam(MYON_RDK_SF_PERSPECTIVE_ONLY, true);
          p.SetParam(MYON_RDK_SF_4x3_FIELD_DISPLAY_ON, false);

          auto& live_frame = Create(safe_frame, MYON_RDK_SF_LIVE_FRAME);
          {
            MYON_XMLParameters p(live_frame);
            p.SetParam(MYON_RDK_SFF_ON, true);
          }

          auto& action_frame = Create(safe_frame, MYON_RDK_SF_ACTION_FRAME);
          {
            MYON_XMLParameters p(action_frame);
            p.SetParam(MYON_RDK_SFF_ON, true);
            p.SetParam(MYON_RDK_SFF_XSCALE, 0.9);
            p.SetParam(MYON_RDK_SFF_YSCALE, 0.9);
            p.SetParam(MYON_RDK_SFF_LINK, true);
          }

          auto& title_frame = Create(safe_frame, MYON_RDK_SF_TITLE_FRAME);
          {
            MYON_XMLParameters p(title_frame);
            p.SetParam(MYON_RDK_SFF_ON, true);
            p.SetParam(MYON_RDK_SFF_XSCALE, 0.8);
            p.SetParam(MYON_RDK_SFF_YSCALE, 0.8);
            p.SetParam(MYON_RDK_SFF_LINK, true);
          }
        }
      }

      // Ground plane.
      if (ValueSets::All == _vs)
      {
        auto& ground_plane = Create(settings, MYON_RDK_GROUND_PLANE);
        {
          MYON_XMLParameters p(ground_plane);
          p.SetParam(MYON_RDK_GP_ON, (_major_version < 6) ? false : true);
          p.SetParam(MYON_RDK_GP_SHOW_UNDERSIDE, false);
          p.SetParam(MYON_RDK_GP_ALTITUDE, 0.0);
          p.SetParam(MYON_RDK_GP_AUTO_ALTITUDE, true);
          p.SetParam(MYON_RDK_GP_SHADOW_ONLY, (_major_version < 6) ? false : true);
          p.SetParam(MYON_RDK_GP_MATERIAL, L"");
          p.SetParam(MYON_RDK_GP_TEXTURE_SIZE, MYON_2dPoint(1.0, 1.0));
          p.SetParam(MYON_RDK_GP_TEXTURE_OFFSET, MYON_2dPoint(0.0, 0.0));
          p.SetParam(MYON_RDK_GP_TEXTURE_ROTATION, 0.0);
          p.SetParam(MYON_RDK_GP_OFFSET_LOCK, false);
          p.SetParam(MYON_RDK_GP_REPEAT_LOCK, true);
        }
      }
      else
      {
        if (_major_version < 6)
        {
          auto& ground_plane = Create(settings, MYON_RDK_GROUND_PLANE);
          {
            MYON_XMLParameters p(ground_plane);
            p.SetParam(MYON_RDK_GP_SHADOW_ONLY, false);
          }
        }
      }
    }
  }
}

const MYON_XMLNode& MYON_RdkDocumentDefaults::Node(void) const
{
  return _root;
}

void MYON_RdkDocumentDefaults::CopyDefaultsTo(MYON_XMLNode& dest) const
{
  if (ValueSets::All == _vs)
  {
    dest = _root;
  }
  else
  {
    dest.MergeFrom(_root);
  }
}

MYON_RdkDocumentDefaults::~MYON_RdkDocumentDefaults()
{
}

#pragma MYON_PRAGMA_WARNING_POP

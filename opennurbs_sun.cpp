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

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#define MYON_RDK_SUN_ENABLE_ALLOWED              L"enable-allowed"
#define MYON_RDK_SUN_ENABLE_ON                   L"enable-on"
#define MYON_RDK_SUN_MANUAL_CONTROL_ALLOWED      L"manual-control-allowed"
#define MYON_RDK_SUN_MANUAL_CONTROL_ON           L"manual-control-on"
#define MYON_RDK_SUN_NORTH                       L"north"
#define MYON_RDK_SUN_AZIMUTH                     L"sun-azimuth"
#define MYON_RDK_SUN_ALTITUDE                    L"sun-altitude"
#define MYON_RDK_SUN_DATE_YEAR                   L"year"
#define MYON_RDK_SUN_DATE_MONTH                  L"month"
#define MYON_RDK_SUN_DATE_DAY                    L"day"
#define MYON_RDK_SUN_TIME_HOURS                  L"time"
#define MYON_RDK_SUN_DAYLIGHT_SAVING_ON          L"daylight-saving-on"
#define MYON_RDK_SUN_DAYLIGHT_SAVING_MINUTES     L"daylight-saving-minutes"
#define MYON_RDK_SUN_OBSERVER_LATITUDE           L"observer-latitude"
#define MYON_RDK_SUN_OBSERVER_LONGITUDE          L"observer-longitude"
#define MYON_RDK_SUN_OBSERVER_TIMEZONE           L"observer-timezone"
#define MYON_RDK_SUN_SHADOW_INTENSITY            L"shadow-intensity"
#define MYON_RDK_SUN_INTENSITY                   L"intensity"

static double Sin(double deg)               { return sin(MYON_RadiansFromDegrees(deg)); }
static double Cos(double deg)               { return cos(MYON_RadiansFromDegrees(deg)); }
static double ArcSin(double sine)           { return MYON_DegreesFromRadians(asin(sine)); }
static double ArcTan2(double dy, double dx) { return MYON_DegreesFromRadians(atan2(dy, dx)); }
static double WorldToCompass(double d)      { return 90.0 - d; }
static double TwilightZone(void)            { return 3.0; }

static constexpr double c2 = 14388.0;
static constexpr double monitor_white = 5000.0;

static bool IsLeapYear(int year)
{
  // The year can only be a leap year if it is divisible by 4.
  if (0 != (year % 4))
    return false;

  // If the year is a century it is only a leap year if it is divisible by 400.
  if ((0 == (year % 100)) && (0 != (year % 400)))
    return false;

  return true;
}

static int DaysInMonth(int month, int year)
{
  month = std::max(1, std::min(12, month));

  if ((2 == month) && IsLeapYear(year))
    return 29;

  static const int tab[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  return tab[month];
}

static double Planck(double lambda, double temp)
{
  static const double E = 2.7182818284590452354;
  return (pow(double(lambda), -5.0) / (pow(E, c2 / (lambda * temp)) - 1.0));
}

static MYON_4fColor ColorTemperature(double temperature)
{
  // Use a variant of Planck's equation to get values for the three CIE wavelengths.
  double temp = monitor_white;

  double er = Planck(0.60, temp);
  double eg = Planck(0.56, temp);
  double eb = Planck(0.44, temp);

  double es = 1.0 / std::max(er, std::max(eg, eb));

  const double r_white = er * es;
  const double g_white = eg * es;
  const double b_white = eb * es;

  temp = temperature;

  er = Planck(0.60, temp);
  eg = Planck(0.56, temp);
  eb = Planck(0.44, temp);

  es = 1.0 / std::max(er,std::max(eg, eb));

  const double r = er * es / r_white;
  const double g = eg * es / g_white;
  const double b = eb * es / b_white;

  es = 1.0 / std::max(r, std::max(g, b));

  const float rr = float(pow(r * es, 0.15));
  const float gr = float(pow(g * es, 0.15));
  const float br = float(pow(b * es, 0.15));

  return MYON_4fColor(rr, gr, br, 1.0f);
}

template <typename T>
static MYON__UINT32 UpdateCRC(MYON__UINT32 crc, T x)
{
  return MYON_CRC32(crc, sizeof(x), &x);
}

static void ConvertSolarVectorToHorizonCoords(const double* vector, double& azimuth, double& altitude)
{
  altitude = ArcSin(-vector[2]);

  const double dY = vector[0] / -Cos(altitude);
  const double dX = vector[1] / -Cos(altitude);

  azimuth = ArcTan2(dY, dX);
  if (azimuth < 0.0)
    azimuth += 360.0;
}

void ConvertHorizonCoordsToSolarVector(double azimuth, double altitude, double* vector)
{
  vector[0] = -Cos(altitude) * Sin(azimuth);
  vector[1] = -Cos(altitude) * Cos(azimuth);
  vector[2] = -Sin(altitude);
}

class MYON_Sun::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath_Sun(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_SUN;
}

MYON_Sun::MYON_Sun()
{
  m_impl = new CImpl;
}

MYON_Sun::MYON_Sun(MYON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

MYON_Sun::MYON_Sun(const MYON_Sun& sun)
{
  m_impl = new CImpl;
  operator = (sun);
}

MYON_Sun::~MYON_Sun()
{
  delete m_impl;
  m_impl = nullptr;
}

int MYON_Sun::MinYear(void)
{
  return 1971;
}

int MYON_Sun::MaxYear(void)
{
  return 2199;
}

const MYON_Sun& MYON_Sun::operator = (const MYON_Sun& sun)
{
  if (this != &sun)
  {
    SetEnableOn             (sun.EnableOn());
    SetEnableAllowed        (sun.EnableAllowed());
    SetEnableOn             (sun.EnableOn());
    SetManualControlAllowed (sun.ManualControlAllowed());
    SetManualControlOn      (sun.ManualControlOn());
    SetNorth                (sun.North());
    SetDaylightSavingOn     (sun.DaylightSavingOn());
    SetDaylightSavingMinutes(sun.DaylightSavingMinutes());
    SetAzimuth              (sun.Azimuth());
    SetAltitude             (sun.Altitude());
    SetLatitude             (sun.Latitude());
    SetLongitude            (sun.Longitude());
    SetTimeZone             (sun.TimeZone());

    int y = 0, m = 0, d = 0; double h = 0.0;
    sun.LocalDateTime(y, m, d, h);
    SetLocalDateTime(y, m, d, h);
  }

  return *this;
}

bool MYON_Sun::operator == (const MYON_Sun& sun)
{
	if (EnableAllowed()         != sun.EnableAllowed())         return false;
	if (EnableOn()              != sun.EnableOn())              return false;
	if (ManualControlAllowed()  != sun.ManualControlAllowed())  return false;
	if (ManualControlOn()       != sun.ManualControlOn())       return false;
	if (North()                 != sun.North())                 return false;
	if (DaylightSavingOn()      != sun.DaylightSavingOn())      return false;
	if (DaylightSavingMinutes() != sun.DaylightSavingMinutes()) return false;

	if (!IsDoubleEqual(Azimuth()  , sun.Azimuth()))   return false;
	if (!IsDoubleEqual(Altitude() , sun.Altitude()))  return false;
	if (!IsDoubleEqual(Latitude() , sun.Latitude()))  return false;
	if (!IsDoubleEqual(Longitude(), sun.Longitude())) return false;
	if (!IsDoubleEqual(TimeZone() , sun.TimeZone()))  return false;

	int y1, m1, d1, y2, m2, d2; double h1, h2;
	LocalDateTime(y1, m1, d1, h1);
	sun.LocalDateTime(y2, m2, d2, h2);

	if ((y1 != y2) || (m1 != m2) || (d1 != d2))
		return false;

	if (!IsDoubleEqual(h1, h2))
		return false;

	return true;
}

bool MYON_Sun::operator != (const MYON_Sun& sun)
{
  return !(operator == (sun));
}

bool MYON_Sun::EnableAllowed(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_ENABLE_ALLOWED, false).AsBool();
}

bool MYON_Sun::EnableOn(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_ENABLE_ON, false).AsBool();
}

bool MYON_Sun::ManualControlAllowed(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_MANUAL_CONTROL_ALLOWED, false).AsBool();
}

bool MYON_Sun::ManualControlOn(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_MANUAL_CONTROL_ON, false).AsBool();
}

double MYON_Sun::North(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_NORTH, 90.0).AsDouble();
}

MYON_3dVector MYON_Sun::Vector(void) const
{
	const double azimuth = Azimuth() + WorldToCompass(North());

	double d[3] = { 0 };
	ConvertHorizonCoordsToSolarVector(azimuth, Altitude(), d);

	return MYON_3dVector(d);
}

double MYON_Sun::Azimuth(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_AZIMUTH, 0.0).AsDouble();
}

double MYON_Sun::Altitude(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_ALTITUDE, 0.0).AsDouble();
}

double MYON_Sun::Latitude(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_OBSERVER_LATITUDE, 0.0).AsDouble();
}

double MYON_Sun::Longitude(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_OBSERVER_LONGITUDE, 0.0).AsDouble();
}

double MYON_Sun::TimeZone(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_OBSERVER_TIMEZONE, 0.0).AsDouble();
}

bool MYON_Sun::DaylightSavingOn(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_DAYLIGHT_SAVING_ON, false).AsBool();
}

int MYON_Sun::DaylightSavingMinutes(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_DAYLIGHT_SAVING_MINUTES, 0).AsInteger();
}

void MYON_Sun::LocalDateTime(int& year, int& month, int& day, double& hours) const
{
  const wchar_t* s = XMLPath_Sun();
  year  = m_impl->GetParameter(s, MYON_RDK_SUN_DATE_YEAR,  2000);
  month = m_impl->GetParameter(s, MYON_RDK_SUN_DATE_MONTH, 1);
  day   = m_impl->GetParameter(s, MYON_RDK_SUN_DATE_DAY,   1);
  hours = m_impl->GetParameter(s, MYON_RDK_SUN_TIME_HOURS, 0.0);
}

double MYON_Sun::Intensity(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_INTENSITY, 1.0).AsDouble();
}

double MYON_Sun::ShadowIntensity(void) const
{
  return m_impl->GetParameter(XMLPath_Sun(), MYON_RDK_SUN_SHADOW_INTENSITY, 1.0).AsDouble();
}

void MYON_Sun::SetEnableAllowed(bool allowed)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_ENABLE_ALLOWED, allowed);
}

void MYON_Sun::SetEnableOn(bool on)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_ENABLE_ON, on);
}

void MYON_Sun::SetManualControlAllowed(bool allowed)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_MANUAL_CONTROL_ALLOWED, allowed);
}

void MYON_Sun::SetManualControlOn(bool manual)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_MANUAL_CONTROL_ON, manual);
}

void MYON_Sun::SetNorth(double north)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_NORTH, north);
}

void MYON_Sun::SetVector(const MYON_3dVector& v)
{
  MYON_3dVector vec = v;
  vec.Unitize();

  const double d[3] = { vec.x, vec.y, vec.z };

  double azimuth = 0.0, altitude = 0.0;
  ConvertSolarVectorToHorizonCoords(d, azimuth, altitude);

  azimuth -= WorldToCompass(North());
  SetAzimuth(azimuth);

  SetAltitude(altitude);
}

void MYON_Sun::SetAzimuth(double azimuth)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_AZIMUTH, azimuth);
}

void MYON_Sun::SetAltitude(double altitude)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_ALTITUDE, altitude);
}

void MYON_Sun::SetLatitude(double latitude)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_OBSERVER_LATITUDE, latitude);
}

void MYON_Sun::SetLongitude(double longitude)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_OBSERVER_LONGITUDE, longitude);
}

void MYON_Sun::SetTimeZone(double hours)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_OBSERVER_TIMEZONE, hours);
}

void MYON_Sun::SetDaylightSavingOn(bool on)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_DAYLIGHT_SAVING_ON, on);
}

void MYON_Sun::SetDaylightSavingMinutes(int minutes)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_DAYLIGHT_SAVING_MINUTES, minutes);
}

bool MYON_Sun::SetLocalDateTime(int year, int month, int day, double hours)
{
  if ((year < MinYear()) || (year > MaxYear()))
    return false;

  const wchar_t* s = XMLPath_Sun();
  m_impl->SetParameter(s, MYON_RDK_SUN_DATE_YEAR,  year);
  m_impl->SetParameter(s, MYON_RDK_SUN_DATE_MONTH, month);
  m_impl->SetParameter(s, MYON_RDK_SUN_DATE_DAY,   day);
  m_impl->SetParameter(s, MYON_RDK_SUN_TIME_HOURS, hours);

  return true;
}

void MYON_Sun::SetShadowIntensity(double intensity)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_SHADOW_INTENSITY, std::max(0.0, std::min(1.0, intensity)));
}

void MYON_Sun::SetIntensity(double intensity)
{
  m_impl->SetParameter(XMLPath_Sun(), MYON_RDK_SUN_INTENSITY, std::max(0.0, intensity));
}

MYON__UINT32 MYON_Sun::DataCRC(MYON__UINT32 crc) const
{
  if (ManualControlAllowed() && ManualControlOn())
  {
    crc = UpdateCRC(crc, Integerize(Azimuth()));
    crc = UpdateCRC(crc, Integerize(Altitude()));
  }
  else
  {
    crc = UpdateCRC(crc, EnableOn());
    crc = UpdateCRC(crc, Integerize(North()));
    crc = UpdateCRC(crc, Integerize(Latitude()));
    crc = UpdateCRC(crc, Integerize(Longitude()));
    crc = UpdateCRC(crc, Integerize(TimeZone()));
    crc = UpdateCRC(crc, DaylightSavingOn());
    crc = UpdateCRC(crc, DaylightSavingMinutes());

    int y = 0, m = 0, d = 0; double h = 0.0;
    LocalDateTime(y, m, d, h);
    crc = UpdateCRC(crc, y);
    crc = UpdateCRC(crc, m);
    crc = UpdateCRC(crc, d);
    crc = UpdateCRC(crc, Integerize(h));
  }

  crc = UpdateCRC(crc, Integerize(ShadowIntensity()));
  crc = UpdateCRC(crc, Integerize(Intensity()));

  return crc;
}

#define SUNASSERT(x) MYON_ASSERT(x); if (!(x)) return false;

bool MYON_Sun::IsValid(void) const
{
  // Returns false if SUNASSERT fails.

  SUNASSERT(Azimuth() >= 0.0);
  SUNASSERT(Azimuth() <= 360.0)
  SUNASSERT(Altitude() >= -90.0);
  SUNASSERT(Altitude() <= +90.0);

  double hours = 0.0;
  int year = 0, month = 0, day = 0;
  LocalDateTime(year, month, day, hours);
  SUNASSERT(year >= MinYear());
  SUNASSERT(year <= MaxYear());
  SUNASSERT(month >= 1);
  SUNASSERT(month <= 12);
  SUNASSERT(day >= 1);
  SUNASSERT(day <= DaysInMonth(month, year));
  SUNASSERT(hours >= 0.0);
  SUNASSERT(hours <= 24.0);

  SUNASSERT(North() >= 0.0);
  SUNASSERT(North() <= 360.0)
  SUNASSERT(Latitude() >= -90.0);
  SUNASSERT(Latitude() <= +90.0);
  SUNASSERT(Longitude() >= -180.0);
  SUNASSERT(Longitude() <= +180.0);
  SUNASSERT(TimeZone() >= -12.0);
  SUNASSERT(TimeZone() <= +13.0);
  SUNASSERT(DaylightSavingMinutes() >= 0);
  SUNASSERT(DaylightSavingMinutes() <= 120);

  SUNASSERT(ShadowIntensity() >= 0.0);
  SUNASSERT(ShadowIntensity() <= 1.0);

  SUNASSERT(Intensity() >= 0.0);

  return true;
}

MYON_4fColor MYON_Sun::SunColorFromAltitude(double altitude) // Static.
{
  MYON_4fColor colDark(0, 0, 64, 255);

  if (altitude < -TwilightZone())
    return colDark;

  const double value = (30.0 * pow(std::max(0.0, altitude), 1.5)) + 500.0;
  auto temperature = std::min(5300.0, value);

  MYON_4fColor col = ColorTemperature(temperature);

  if (altitude < 0.0)
  {
    const double dBlend = -altitude / TwilightZone();
    col.BlendTo(float(dBlend), colDark);
  }

  return col;
}

static MYON_UUID uuidFeatureSun = { 0x62ee2cf6, 0xb855, 0x4549, { 0xa2, 0x77, 0xe2, 0xbb, 0xf6, 0x09, 0xf3, 0x28 } };

MYON_Light MYON_Sun::Light(void) const
{
  MYON_Light light;
  light.Enable(false);
  light.SetStyle(MYON::world_directional_light);
  light.SetLocation(MYON_3dPoint(0.0, 0.0, 0.0));

  light.SetShadowIntensity(ShadowIntensity());
  light.SetIntensity(Intensity());

  if (IsValid())
  {
    light.SetDirection(Vector());

    const bool bOn = EnableOn();
    light.Enable(bOn);

    const MYON_Color col = SunColorFromAltitude(Altitude());
    light.SetDiffuse(col);
  }

  light.m_light_id = uuidFeatureSun;

  return light;
}

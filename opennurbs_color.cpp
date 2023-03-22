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


const MYON_Color MYON_Color::RandomColor()
{
  return RandomColor(MYON_RandomNumberGenerator::RandomSeed());
}


const MYON_Color MYON_Color::RandomColor(
  MYON_Interval hue_range,
  MYON_Interval saturation_range,
  MYON_Interval value_range
)
{
  return RandomColor(MYON_RandomNumberGenerator::RandomSeed(), hue_range, saturation_range, value_range);
}

const MYON_Color MYON_Color::RandomColor(
  MYON__UINT32 seed
)
{
  return MYON_Color::RandomColor(seed, MYON_Interval::ZeroToTwoPi, MYON_Interval::Singleton(1.0), MYON_Interval::Singleton(1.0));
}

const MYON_Color MYON_Color::RandomColor(
  MYON__UINT32 seed,
  MYON_Interval hue_range,
  MYON_Interval saturation_range,
  MYON_Interval value_range
)
{
  MYON_RandomNumberGenerator rg;
  rg.Seed(seed + 1U); // the +1 is so the colors for seeds (0,1,2,3,4,5 are more vistually distinct)
  const double h = hue_range.IsSingleton() ? hue_range.m_t[0] : rg.RandomDouble(hue_range);
  double s = saturation_range.IsSingleton() ? saturation_range.m_t[0] : rg.RandomDouble(saturation_range);
  if (s > MYON_UNSET_VALUE && s < MYON_UNSET_POSITIVE_VALUE)
  {
    if (s < 0.0)
      s = 0.0;
    else if (s > 1.0)
      s = 1.0;
  }
  else
    s = 1.0;
  double v = value_range.IsSingleton() ? value_range.m_t[0] : rg.RandomDouble(value_range);
  if (v > MYON_UNSET_VALUE && v < MYON_UNSET_POSITIVE_VALUE)
  {
    if (v < 0.0)
      v = 0.0;
    else if (v > 1.0)
      v = 1.0;
  }
  else
    v = 1.0;
  MYON_Color color = MYON_Color::UnsetColor;
  if (MYON_IsValid(h) && MYON_IsValid(s) && MYON_IsValid(v))
    color.SetHSV( fmod(h,MYON_2PI), s, v );
  return color;
}

MYON_Color::MYON_Color(unsigned int colorref)
  : m_color(colorref)
{
  // No adjustments are required on big endian computers because all
  // unsigned int conversion and all IO preserves the order of the
  // MYON_Color::m_RGBA[4] bytes.
}

MYON_Color::MYON_Color(int r, int g, int b)
{
  SetRGB(r,g,b);
}

MYON_Color::MYON_Color(int r, int g, int b, int a)
{
  SetRGBA(r,g,b,a);
}

unsigned int MYON_Color::WindowsRGB() const
{
  MYON_Color RGB = MYON_Color(Red(),Green(),Blue());
  return RGB;
}

MYON_Color::operator unsigned int() const
{
  // No adjustments are required on big endian computers because all
  // unsigned int conversion and all IO preserves the order of the
  // MYON_Color::m_RGBA[4] bytes.
  return m_color;
}

int MYON_Color::Compare( const MYON_Color& b ) const
{
  int ac = (int)m_color;
  int bc = (int)b.m_color;
#if defined(MYON_BIG_ENDIAN)
  unsigned char* swapper = (unsigned char*)&ac;
  unsigned char c = swapper[0]; swapper[0] = swapper[3]; swapper[3] = c;
  c = swapper[1]; swapper[1] = swapper[2]; swapper[2] = c;
  swapper = (unsigned char*)&bc;
  c = swapper[0]; swapper[0] = swapper[3]; swapper[3] = c;
  c = swapper[1]; swapper[1] = swapper[2]; swapper[2] = c;
#endif
  return ac-bc; // overflow roll over is fine - important thing is that answer is consistent.
}

int MYON_Color::Red() const
{ return m_RGBA[MYON_Color::kRedByteIndex];}

int MYON_Color::Green() const
{ return m_RGBA[MYON_Color::kGreenByteIndex];}

int MYON_Color::Blue() const
{ return m_RGBA[MYON_Color::kBlueByteIndex];}

int MYON_Color::Alpha() const
{ return m_RGBA[MYON_Color::kAlphaByteIndex];}

double MYON_Color::FractionRed() const
{
  //return Red()/255.0;
  return m_RGBA[MYON_Color::kRedByteIndex]*0.003921568627450980392156862745; // better fodder for optimizer
}

double MYON_Color::FractionGreen() const
{
  //return Green()/255.0;
  return m_RGBA[MYON_Color::kGreenByteIndex]*0.003921568627450980392156862745; // better fodder for optimizer
}

double MYON_Color::FractionBlue() const
{
  //return Blue()/255.0;
  return m_RGBA[MYON_Color::kBlueByteIndex]*0.003921568627450980392156862745; // better fodder for optimizer
}

double MYON_Color::FractionAlpha() const
{
  //return Alpha()/255.0;
  return m_RGBA[MYON_Color::kAlphaByteIndex]*0.003921568627450980392156862745; // better fodder for optimizer
}

void MYON_Color::SetRGB(int r,int g,int b) // 0 to 255
{
  SetRGBA(r,g,b,0);
}

void MYON_Color::SetFractionalRGB(double r,double g,double b)
{
  SetFractionalRGBA(r,g,b,0.0);
}

void MYON_Color::SetAlpha(int alpha)
{
	if (alpha < 0 ) alpha = 0; else if ( alpha > 255 ) alpha = 255;
	m_RGBA[MYON_Color::kAlphaByteIndex] = (unsigned char)alpha;
}

void MYON_Color::SetFractionalAlpha(double alpha)
{
	if (alpha < 0.0 ) alpha = 0.0; else if ( alpha > 1.0 ) alpha = 1.0;
  SetAlpha((int)(alpha*255.0));
}

void
MYON_Color::SetRGBA( int red, int green, int blue, int alpha )
{
	if (red   < 0 ) red   = 0; else if ( red   > 255 ) red   = 255;
	if (green < 0 ) green = 0; else if ( green > 255 ) green = 255;
	if (blue  < 0 ) blue  = 0; else if ( blue  > 255 ) blue  = 255;
	if (alpha < 0 ) alpha = 0; else if ( alpha > 255 ) alpha = 255;
  m_RGBA[MYON_Color::kRedByteIndex] = (unsigned char)red;
  m_RGBA[MYON_Color::kGreenByteIndex] = (unsigned char)green;
  m_RGBA[MYON_Color::kBlueByteIndex] = (unsigned char)blue;
  m_RGBA[MYON_Color::kAlphaByteIndex] = (unsigned char)alpha;
}

void
MYON_Color::SetFractionalRGBA( double red, double green, double blue, double alpha )
{
  int r,g,b,a;
	if (red   < 0.0 ) red   = 0.0; else if ( red   > 1.0 ) red   = 1.0;
	if (green < 0.0 ) green = 0.0; else if ( green > 1.0 ) green = 1.0;
	if (blue  < 0.0 ) blue  = 0.0; else if ( blue  > 1.0 ) blue  = 1.0;
	if (alpha < 0.0 ) alpha = 0.0; else if ( alpha > 1.0 ) alpha = 1.0;

  red   *= 255.0;
  green *= 255.0;
  blue  *= 255.0;
  alpha *= 255.0;

  r = (int)red;
  g = (int)green;
  b = (int)blue;
  a = (int)alpha;

  // round to closest int
  if( (red-r)>=0.5 ) r++;
  if( (green-g)>=0.5 ) g++;
  if( (blue-b)>=0.5 ) b++;
  if( (alpha-a)>=0.5 ) a++;

  SetRGBA( r, g, b, a );
}

double MYON_Color::Hue() const
{
  // returns 0 to 2*pi
  // 0    = red,  pi/3   = yellow, 2*pi/3 = green,
  // pi   = cyan, 4*pi/3 = blue,   5*pi/3 = magenta,
  // 2*pi = red
  double h;
  int r = Red();
  int g = Green();
  int b = Blue();
  int minrgb, maxrgb;
  if ( r <= g ) {minrgb = r; maxrgb = g;} else {minrgb = g; maxrgb = r;}
  if (minrgb > b) minrgb = b; else if (maxrgb < b ) maxrgb = b;
  if ( maxrgb != minrgb ) {
    double d = 1.0/(maxrgb - minrgb);
    if ( r == maxrgb) {
      h = (g - b)*d;
      if ( h < 0.0 )
        h += 6.0;
    }
    else if ( g == maxrgb)
      h = 2.0 + (b - r)*d;
    else
      h = 4.0 + (r - g)*d;
    h *= MYON_PI/3.0;
  }
  else
    h = 0.0;
  return h;
}

double MYON_Color::Saturation() const
{
  // 0.0 to 1.0    0.0 = gray,  1.0 = saturated
  double s;
  int r = Red();
  int g = Green();
  int b = Blue();
  int minrgb, maxrgb;
  if ( r <= g ) {minrgb = r; maxrgb = g;} else {minrgb = g; maxrgb = r;}
  if (minrgb > b) minrgb = b; else if (maxrgb < b ) maxrgb = b;
  if ( maxrgb > 0 ) {
    s = ((double)(maxrgb - minrgb))/((double)maxrgb);
  }
  else
    s = 0.0;
  return s;
}

double MYON_Color::Value() const
{
  // 0.0 to 1.0    0.0 = black, 1.0 = white
  int r = Red();
  int g = Green();
  int b = Blue();
  int maxrgb = ( r <= g ) ? g : r; if ( maxrgb < b ) maxrgb = b;
  return (maxrgb/255.0);
}

void MYON_Color::SetHSV(
       double hue,         // hue in radians
       double saturation, // satuation 0.0 = gray, 1.0 = saturated
       double value       // value
       )
{
  int i;
  double f, p, q, t, r, g, b;
  if ( saturation <= 1.0/256.0 ) {
    r = value;
    g = value;
    b = value;
  }
  else  {
    hue *= 3.0 / MYON_PI;  // (6.0 / 2.0 * MYON_PI);
    i = (int)floor(hue);
    if ( i < 0 || i > 5 ) {
      hue = fmod(hue,6.0);
      if ( hue < 0.0 )
        hue += 6.0;
      i = (int)floor(hue);
    }
    f = hue - i;
    p = value * ( 1.0 - saturation);
    q = value * ( 1.0 - ( saturation * f) );
    t = value * ( 1.0 - ( saturation * ( 1.0 - f) ) );
    switch( i)
    {
    case 0:
      r = value; g = t; b = p; break;
    case 1:
      r = q; g = value; b = p; break;
    case 2:
      r = p; g = value; b = t; break;
    case 3:
      r = p; g = q; b = value; break;
    case 4:
      r = t; g = p; b = value; break;
    case 5:
      r = value; g = p; b = q; break;
    default:
      r = 0; g = 0; b = 0; break; // to keep lint quiet
    }
  }
  SetFractionalRGB(r,g,b);
}


const MYON_wString MYON_Color::ToString(
  MYON_Color::TextFormat format,
  wchar_t separator,
  bool bFormatUnsetColor,
  class MYON_TextLog& text_log
) const
{
  MYON_wString s;
  if (MYON_Color::UnsetColor == *this)
  {
    if (bFormatUnsetColor)
      s = L"MYON_Color::UnsetColor";
  }
  else
  {
    if (0 != Alpha())
    {
      // handle conditional alpha case
      switch (format)
      {
      case MYON_Color::TextFormat::FractionalRGBa:
        format = MYON_Color::TextFormat::FractionalRGBA;
        break;
      case MYON_Color::TextFormat::DecimalRGBa:
        format = MYON_Color::TextFormat::DecimalRGBA;
        break;
      case MYON_Color::TextFormat::HexadecimalRGBa:
        format = MYON_Color::TextFormat::HexadecimalRGBA;
        break;
      case MYON_Color::TextFormat::HSVa:
        format = MYON_Color::TextFormat::HSVA;
        break;
      default:
        break;
      }
    }

    if (0 == separator)
      separator = ',';

    switch (format)
    {
    case MYON_Color::TextFormat::Unset:
      // intentionally returns empty string.
      break;
    case MYON_Color::TextFormat::FractionalRGB:
    case MYON_Color::TextFormat::FractionalRGBa: // nonzero alpha handled above
      s = MYON_wString::FormatToString(L"%g%lc%g%lc%g", FractionRed(), separator, FractionGreen(), separator, FractionBlue());
      break;
    case MYON_Color::TextFormat::FractionalRGBA:
      s = MYON_wString::FormatToString(L"%g%lc%g%lc%g%lc%g", FractionRed(), separator, FractionGreen(), separator, FractionBlue(), separator, FractionAlpha());
      break;
    case MYON_Color::TextFormat::DecimalRGB:
    case MYON_Color::TextFormat::DecimalRGBa: // nonzero alpha handled above
      s = MYON_wString::FormatToString(L"%d%lc%d%lc%d", Red(), separator, Green(), separator, Blue());
      break;
    case MYON_Color::TextFormat::DecimalRGBA:
      s = MYON_wString::FormatToString(L"%d%lc%d%lc%d%lc%d", Red(), separator, Green(), separator, Blue(), separator, Alpha());
      break;
    case MYON_Color::TextFormat::HexadecimalRGB:
    case MYON_Color::TextFormat::HexadecimalRGBa: // nonzero alpha handled above
      s = MYON_wString::FormatToString(L"%02x%lc%02x%lc%02x", Red(), separator, Green(), separator, Blue());
      break;
    case MYON_Color::TextFormat::HexadecimalRGBA:
      s = MYON_wString::FormatToString(L"%02x%lc%02x%lc%02x%lc%02x", Red(), separator, Green(), separator, Blue(), separator, Alpha());
      break;
    case MYON_Color::TextFormat::HSV:
    case MYON_Color::TextFormat::HSVa: // nonzero alpha handled above
      s = MYON_wString::FormatToString(L"%g%lc%g%lc%g", Hue(), separator, Saturation(), separator, Value());
      break;
    case MYON_Color::TextFormat::HSVA:
      s = MYON_wString::FormatToString(L"%g%lc%g%lc%g%lc%g", Hue(), separator, Saturation(), separator, Value(), separator, FractionAlpha());
      break;
    }
  }
  return s;
}

void MYON_Color::ToText(
  MYON_Color::TextFormat format,
  wchar_t separator,
  bool bFormatUnsetColor,
  class MYON_TextLog& text_log
) const
{
  const MYON_wString s = ToString(format, separator, bFormatUnsetColor, text_log);
  if (s.IsNotEmpty())
    text_log.Print(L"%ls", static_cast<const wchar_t*>(s));
}

enum { R, G, B, A };

MYON_4fColor::MYON_4fColor()
{
  for (int i = 0; i < 4; i++)
  {
    m_color[i] = MYON_UNSET_FLOAT;
  }
}

MYON_4fColor::MYON_4fColor(int r, int g, int b, int a)
{
  SetRGBA(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

MYON_4fColor::MYON_4fColor(float r, float g, float b, float a)
{
  SetRGBA(r, g, b, a);
}

// Note that this function will set the alpha correctly from MYON_Color's 'inverted' alpha.
MYON_4fColor::MYON_4fColor(const MYON_Color& in)
{
  *this = in;
}

MYON_4fColor& MYON_4fColor::operator = (const MYON_Color& in)
{
  SetRed  (       float(in.FractionRed()));
  SetGreen(       float(in.FractionGreen()));
  SetBlue (       float(in.FractionBlue()));
  SetAlpha(1.0f - float(in.FractionAlpha()));

  return *this;
}

bool MYON_4fColor::operator == (const MYON_4fColor& other)
{
  return Compare(other) == 0;
}

bool MYON_4fColor::operator != (const MYON_4fColor& other)
{
  return Compare(other) != 0;
}

// Will invert the opacity alpha to transparency.
MYON_4fColor::operator MYON_Color(void) const
{
  MYON_Color out;
  out.SetFractionalRGBA(Red(), Green(), Blue(), 1.0 - Alpha());

  return out;
}

float MYON_4fColor::Red(void) const
{
  return m_color[R];
}

void MYON_4fColor::SetRed(float f)
{
  m_color[R] = f;
}

float MYON_4fColor::Green(void) const
{
  return m_color[G];
}

void MYON_4fColor::SetGreen(float f)
{
  m_color[G] = f;
}

float MYON_4fColor::Blue(void) const
{
  return m_color[B];
}

void MYON_4fColor::SetBlue(float f)
{
  m_color[B] = f;
}

float MYON_4fColor::Alpha(void) const
{
  return m_color[A];
}

void MYON_4fColor::SetAlpha(float f)
{
  m_color[A] = f;
}

void MYON_4fColor::SetRGBA(float r, float g, float b, float a)
{
  m_color[R] = r;
  m_color[G] = g;
  m_color[B] = b;
  m_color[A] = a;
}

bool MYON_4fColor::IsValid(class MYON_TextLog*) const
{
  for (int i = 0; i < 4; i++)
  {
    if (MYON_IS_UNSET_FLOAT(m_color[i]))
      return false;
  }
  return true;
}


MYON_ColorStop::MYON_ColorStop(const MYON_Color& color, double position)
  : m_color(color)
  , m_position(position)
{
}

bool MYON_ColorStop::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 0);
  if (!rc)
    return false;

  for (;;)
  {
    rc = archive.WriteColor(m_color);
    if (!rc) break;
    rc = archive.WriteDouble(m_position);
    if (!rc) break;

    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_ColorStop::Read(MYON_BinaryArchive& file)
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version);
  if (!rc)
    return false;
  for (;;)
  {
    rc = (1 == major_version);
    if (!rc) break;
    rc = file.ReadColor(m_color);
    if (!rc) break;
    rc = file.ReadDouble(&m_position);
    if (!rc) break;

    break;
  }
  if (!file.EndRead3dmChunk())
    rc = false;

  return rc;
}

static int CompareNans(double a, double b)
{
  if (a == a)
  {
    if (b == b)
    {
      return ((a < b) ? -1 : ((a > b) ? 1 : 0));
    }
    // a is not a NAN, b is a NAN
    return -1; // a < b - NAN's are last
  }
  else if (b == b)
  {
    // a is a NAN, b is not a NAN
    return -1; // b < a - NAN's are last
  }
  return 0; // a and b are both NaNs
}

static int CompareDouble(double a, double b)
{
  return ((a < b) ? -1 : ((a > b) ? 1 : (a == b ? 0 : CompareNans(a, b))));
}

int MYON_4fColor::Compare(const MYON_4fColor& b) const
{
  int rc = CompareDouble(Red(), b.Red());
  if (0 != rc)
    return rc;

  rc = CompareDouble(Green(), b.Green());
  if (0 != rc)
    return rc;

  rc = CompareDouble(Blue(), b.Blue());
  if (0 != rc)
    return rc;

  rc = CompareDouble(Alpha(), b.Alpha());

  return rc;
}

void MYON_4fColor::BlendTo(float t, const MYON_4fColor& col, bool bClampAlpha)
{
  const auto t01 = std::max(0.0f, std::min(1.0f, t));

  for (int i = 0; i < 4; i++)
  {
    m_color[i] = Lerp(t01, m_color[i], col.m_color[i]);
  }

  if (bClampAlpha)
  {
    m_color[A] = std::max(0.0f, std::min(1.0f, m_color[A]));
  }
}

float* MYON_4fColor::FloatArray(void)
{
  return m_color;
}

const float* MYON_4fColor::FloatArray(void) const
{
  return m_color;
}

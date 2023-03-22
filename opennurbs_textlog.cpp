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

//////////////////////////////////////////////////////////////////////////////

MYON_TextLogIndent::MYON_TextLogIndent(MYON_TextLog& text_log)
: m_text_log(text_log)
, m_bEnabled(true)
{
  m_text_log.PushIndent();
}

MYON_TextLogIndent::MYON_TextLogIndent(MYON_TextLog& text_log, bool bEnable)
: m_text_log(text_log)
, m_bEnabled(bEnable)
{
  if ( m_bEnabled )
    m_text_log.PushIndent();
}

MYON_TextLogIndent::~MYON_TextLogIndent()
{
  if ( m_bEnabled )
    m_text_log.PopIndent();
}

MYON_TextLogLevelOfDetail::MYON_TextLogLevelOfDetail(
  class MYON_TextLog& text_log,
  MYON_TextLog::LevelOfDetail level_of_detail
)
  : m_text_log(text_log)
  , m_saved_level_of_detail(text_log.GetLevelOfDetail())
{
  m_text_log.SetLevelOfDetail(level_of_detail);
}

MYON_TextLogLevelOfDetail::~MYON_TextLogLevelOfDetail()
{
  m_text_log.SetLevelOfDetail(m_saved_level_of_detail);
}

MYON_TextLog::LevelOfDetail MYON_TextLogLevelOfDetail::SavedLevelOfDetail() const
{
  return m_saved_level_of_detail;
}


//////////////////////////////////////////////////////////////////////////////

MYON_TextLog::MYON_TextLog()
{
  SetFloatFormat("%g");
  SetDoubleFormat("%.17g");
}

MYON_TextLog::MYON_TextLog( FILE* pFile )
  : m_pFile(pFile)
  , m_bNullTextLog(((MYON__UINT_PTR)pFile) <= MYON_PTR_SEMAPHORE_MAX)
{
  if (m_bNullTextLog)
  {
    m_pFile = nullptr;
  }
  SetFloatFormat("%g");
  SetDoubleFormat("%.17g");
}

MYON_TextLog::MYON_TextLog( MYON_wString& wstr ) 
  : m_pString(&wstr)
  , m_bNullTextLog(((MYON__UINT_PTR)&wstr) <= MYON_PTR_SEMAPHORE_MAX)
{
  if (m_bNullTextLog)
  {
    m_pString = nullptr;
  }
  SetFloatFormat("%g");
  SetDoubleFormat("%.17g");
}

MYON_TextLog::~MYON_TextLog()
{
}


MYON_TextLog::LevelOfDetail MYON_TextLog::LevelOfDetailFromUnsigned(
  unsigned int level_of_detail
)
{
  switch (level_of_detail)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextLog::LevelOfDetail::Minimum);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextLog::LevelOfDetail::Medium);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextLog::LevelOfDetail::Maximum);
  }
  MYON_ERROR("Invalid level_of_detail parameter value");
  return (MYON_TextLog::LevelOfDetail::Medium);
}

void MYON_TextLog::SetLevelOfDetail(MYON_TextLog::LevelOfDetail level_of_detail)
{
  if (false == IsNull() && false == IsTextHash())
  {
    if (level_of_detail == MYON_TextLog::LevelOfDetailFromUnsigned(static_cast<unsigned int>(level_of_detail)))
      m_level_of_detail = level_of_detail;
  }
}

MYON_TextLog::LevelOfDetail MYON_TextLog::GetLevelOfDetail() const
{
  return m_level_of_detail;
}

bool MYON_TextLog::LevelOfDetailIsAtLeast(MYON_TextLog::LevelOfDetail level_of_detail)
{
  return static_cast<unsigned int>(m_level_of_detail) >= static_cast<unsigned int>(level_of_detail);
}

void MYON_TextLog::SetColorFormat(MYON_Color::TextFormat color_format)
{
  if (MYON_Color::TextFormat::Unset == color_format)
    ClearColorFormat();
  else
    m_color_format = (color_format);
}

MYON_Color::TextFormat MYON_TextLog::GetColorFormat()
{
  return m_color_format;
}

void MYON_TextLog::ClearColorFormat()
{
  m_color_format = MYON_Color::TextFormat::DecimalRGBa;
}

void MYON_TextLog::SetDoubleFormat(const char* sFormat)
{
  m_double_format = sFormat;
  m_double2_format = m_double_format + ", " + m_double_format;
  m_double3_format = m_double2_format + ", " + m_double_format;
  m_double4_format = m_double3_format + ", " + m_double_format;
}

void MYON_TextLog::GetDoubleFormat( MYON_String& s ) const
{
  s = m_double_format;
}

void MYON_TextLog::SetFloatFormat(const char* sFormat)
{
  m_float_format = sFormat;
  m_float2_format = m_float_format + ", " + m_float_format;
  m_float3_format = m_float2_format + ", " + m_float_format;
  m_float4_format = m_float3_format + ", " + m_float_format;
}

void MYON_TextLog::GetFloatFormat( MYON_String& s ) const
{
  s = m_float_format;
}

void MYON_TextLog::PushIndent()
{
  if ( m_indent_size > 0 )
  {
    for ( int i = 0; i < m_indent_size; i++ )
    {
      m_indent += ' ';
    }
  }
  else 
  {
    m_indent += "\t";
  }
  m_indent_count++;
}

void MYON_TextLog::PopIndent()
{
  const int length = m_indent.Length();
  const int indent_lenth = m_indent_size>0 ? m_indent_size : 1;
  if ( length >= indent_lenth && m_indent_count > 0)
  {
    m_indent_count--;
    m_indent.SetLength(length-indent_lenth);
  }
  else
  {
    m_indent.Destroy();
    m_indent_count = 0;
  }
}

int MYON_TextLog::IndentSize() const
{
  //  0: one tab per indent
  // >0: number of spaces per indent
  return m_indent_size;
}

void MYON_TextLog::SetIndentSize(int indent_size)
{
  m_indent_size = (indent_size>0) ? indent_size : 0;
}

int MYON_TextLog::IndentCount()
{
  return m_indent_count;
}
void MYON_TextLog::SetIndentCount(
  int indent_count
)
{
  if (indent_count < 0)
    indent_count = 0;
  while (m_indent_count > indent_count)
  {
    PopIndent();
  }
  while (m_indent_count < indent_count)
  {
    PushIndent();
  }
}


void MYON_VARGS_FUNC_CDECL MYON_TextLog::Print(const char* format, ...)
{
  if (m_bNullTextLog)
    return;

  // NOTE:
  //  This code does not work on Traditional Chinese Windows 10.
  //
  if (nullptr == format || 0 == format[0])
    return;

  // format message and append it to the log
  char stack_buffer[2048];
  MYON_StringBuffer buffer(stack_buffer, sizeof(stack_buffer) / sizeof(stack_buffer[0]));
  
  va_list args;
  va_start(args, format);
  MYON_String::FormatVargsIntoBuffer(buffer, format, args);
  va_end(args);

  char* s0 = buffer.m_buffer;
  if (nullptr == s0 || 0 == s0[0])
    return;

  for (  char* s1 = s0; *s1; s1++) 
  {
    if ( *s1 == '\n' ) 
    {
      *s1 = 0;
      if ( m_beginning_of_line && m_indent.IsNotEmpty() )
        AppendText( static_cast< const char* >(m_indent) );
      if (*s0) 
        AppendText(s0);
      AppendText("\n");
      m_beginning_of_line = 1;
      s0 = s1+1;
    }
  }
  if (*s0)
  {
    if ( m_beginning_of_line && m_indent.IsNotEmpty() )
      AppendText( static_cast< const char* >(m_indent) );
    AppendText(s0);
    m_beginning_of_line = 0;
  }

}

void MYON_VARGS_FUNC_CDECL MYON_TextLog::Print(const wchar_t* format, ...)
{
  if (m_bNullTextLog)
    return;

  if (nullptr == format || 0 == format[0])
    return;

  // format message and append it to the log
  wchar_t stack_buffer[2048];
  MYON_wStringBuffer buffer(stack_buffer, sizeof(stack_buffer) / sizeof(stack_buffer[0]));

  va_list args;
  va_start(args, format);
  MYON_wString::FormatVargsIntoBuffer(buffer, format, args);
  va_end(args);

  wchar_t* s0 = buffer.m_buffer;
  if (nullptr == s0 || 0 == s0[0])
    return;

  for (  wchar_t* s1 = s0; *s1; s1++)
  {
    if ( *s1 == '\n' )
    {
      *s1 = 0;
      if ( m_beginning_of_line && m_indent.IsNotEmpty() )
        AppendText( static_cast< const char* >(m_indent) );
      if (*s0) 
        AppendText(s0);
      AppendText(L"\n");
      m_beginning_of_line = 1;
      s0 = s1+1;
    }
  }
  if (*s0)
  {
    if ( m_beginning_of_line && m_indent.IsNotEmpty() )
      AppendText( static_cast< const char* >(m_indent) );
    AppendText(s0);
    m_beginning_of_line = 0;
  }

}

void MYON_TextLog::AppendText( const char* s )
{
  // This is a virtual function 
  if (m_bNullTextLog)
    return;

  if ( s && *s ) 
  {
    if ( nullptr != m_pString)
    {
      (*m_pString) += s;
    }
    else if ( nullptr != m_pFile ) 
    {
      fputs( s, m_pFile );
    }
    else
    {
      printf("%s",s);
    }
  }
}

void MYON_TextLog::AppendText( const wchar_t* s )
{
  // This is a virtual function 
  if (m_bNullTextLog)
    return;

  if ( nullptr != m_pString )
  {
    (*m_pString) += s;
  }
  else
  {
    // If sizeof(wchar_t) = 2, str = s performs
    // performs UTF-16 to UTF-8 conversion.
    // If sizeof(wchar_t) = 4, str = s performs
    // performs UTF-32 to UTF-8 conversion.
    MYON_String str = s;
    AppendText(str.Array());
  }
}

void MYON_TextLog::Print( float x )
{
  if ( MYON_UNSET_FLOAT == x )
    Print("MYON_UNSET_FLOAT");
  else
    Print(static_cast< const char* >(m_float_format),x);
}

void MYON_TextLog::Print( double x )
{
  if ( MYON_UNSET_VALUE == x )
    Print("MYON_UNSET_VALUE");
  else
    Print(static_cast< const char* >(m_double_format),x);
}

void MYON_TextLog::Print( const MYON_2dPoint& p )
{
  Print("(");
  Print(static_cast< const char* >(m_double2_format), p.x, p.y);
  Print(")");
}

void MYON_TextLog::Print( const MYON_3dPoint& p )
{
  Print("(");
  if ( MYON_3dPoint::UnsetPoint == p )
    Print("UnsetPoint");
  else
    Print(static_cast< const char* >(m_double3_format), p.x, p.y, p.z );
  Print(")");
}

void MYON_TextLog::Print( const MYON_4dPoint& p )
{
  Print("[");
  Print(static_cast< const char* >(m_double4_format), p.x, p.y, p.z, p.w );
  Print("]");
}

void MYON_TextLog::Print( const MYON_2dVector& p )
{
  Print("<");
  Print(static_cast< const char* >(m_double2_format), p.x, p.y);
  Print(">");
}

void MYON_TextLog::Print( const MYON_3dVector& p )
{
  Print("<");
  if ( MYON_3dVector::UnsetVector == p )
    Print("UnsetVector");
  else
    Print(static_cast< const char* >(m_double3_format), p.x, p.y, p.z);
  Print(">");
}

void MYON_TextLog::Print( const MYON_Xform& xform )
{
  if ( xform.IsIdentity() )
  {
    Print("MYON_Xform::IdentityTransformation\n");
  }
  else if ( xform.IsZero() )
  {
    Print("MYON_Xform::ZeroTransformation\n");
  }
  else
  {
    Print(static_cast< const char* >(m_double4_format),xform[0][0],xform[0][1],xform[0][2],xform[0][3]);
    Print("\n");
    Print(static_cast< const char* >(m_double4_format),xform[1][0],xform[1][1],xform[1][2],xform[1][3]);
    Print("\n");
    Print(static_cast< const char* >(m_double4_format),xform[2][0],xform[2][1],xform[2][2],xform[2][3]);
    Print("\n");
    Print(static_cast< const char* >(m_double4_format),xform[3][0],xform[3][1],xform[3][2],xform[3][3]);
    Print("\n");
  }
}

void MYON_TextLog::Print( const MYON_UUID& uuid )
{
  if ( 0 == uuid.Data2 && 0x11dc == uuid.Data3
       && 0x98 == uuid.Data4[0]
       && 0x85 == uuid.Data4[1]
       && 0x00 == uuid.Data4[2]
       && 0x13 == uuid.Data4[3]
       && 0x72 == uuid.Data4[4]
       && 0xc3 == uuid.Data4[5]
       && 0x38 == uuid.Data4[6]
       && 0x78 == uuid.Data4[7]
     )
  {
    // xxxxxxxx-0000-11dc-9885-001372C33878
    // This uuid value is a "repeatably unique" id used
    // for testing purposes.
    // See MYON_CreateUuid(MYON_UUID&) for more details.
    Print("%08X-...(runtime value varies)", uuid.Data1 );
  }
  else
  {
    Print("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X", 
          uuid.Data1, uuid.Data2, uuid.Data3,
          uuid.Data4[0], uuid.Data4[1], uuid.Data4[2], uuid.Data4[3],
          uuid.Data4[4], uuid.Data4[5], uuid.Data4[6], uuid.Data4[7]
          );
  }
}

void MYON_TextLog::Print( const MYON_COMPONENT_INDEX& ci )
{
  switch( ci.m_type )
  {
    case MYON_COMPONENT_INDEX::invalid_type:
      Print("invalid_type(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::brep_vertex:
      Print("brep_vertex(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::brep_edge:
      Print("brep_edge(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::brep_face:
      Print("brep_face(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::brep_trim:
      Print("brep_trim(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::brep_loop:
      Print("brep_loop(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::mesh_vertex:
      Print("mesh_vertex(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::meshtop_vertex:
      Print("meshtop_vertex(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::meshtop_edge:
      Print("meshtop_edge(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::mesh_face:
      Print("mesh_face(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::mesh_ngon:
      Print("mesh_ngon(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::idef_part:
      Print("idef_part(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::polycurve_segment:
      Print("polycurve_segment(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::pointcloud_point:
      Print("pointcloud_point(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::group_member:
      Print("group_member(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::extrusion_bottom_profile:
      Print("extrusion_bottom_profile(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::extrusion_top_profile:
      Print("extrusion_top_profile(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::extrusion_wall_edge:
      Print("extrusion_wall_edge(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::extrusion_wall_surface:
      Print("extrusion_wall_surface(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::extrusion_cap_surface:
      Print("extrusion_cap_surface(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::extrusion_path:
      Print("extrusion_path(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::subd_vertex:
      Print("subd_vertex(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::subd_edge:
      Print("subd_edge(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::subd_face:
      Print("subd_face(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::hatch_loop:
      Print("hatch_loop(%d)", ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::dim_linear_point:
      Print("dim_linear_point(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::dim_radial_point:
      Print("dim_radial_point(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::dim_angular_point:
      Print("dim_angular_point(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::dim_ordinate_point:
      Print("dim_ordinate_point(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::dim_text_point:
      Print("dim_text_point(%d)",ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::dim_centermark_point:
      Print("dim_centermark_point(%d)", ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::dim_leader_point:
      Print("dim_leader_point(%d)", ci.m_index);
      break;
    case MYON_COMPONENT_INDEX::no_type:
      Print("no_type(%d)",ci.m_index);
      break;
    default:
      Print("MYON_COMPONENT_INDEX(%d,%d)",ci.m_type,ci.m_index);
      break;
  }
}

void MYON_TextLog::Print( const MYON_wString& string )
{
  const MYON_wString local(string); // increment reference count to insure s is valid for the duration of this call.
  const wchar_t* s = static_cast< const wchar_t* >(local);
  if ( s && *s )
  {
    if (m_beginning_of_line && m_indent.IsNotEmpty())
      AppendText(static_cast<const char*>(m_indent));
    AppendText(s);
  }
}

void MYON_TextLog::Print( const MYON_String& string )
{
  const MYON_String local(string); // increment reference count to insure s is valid for the duration of this call.
  const char* s = static_cast< const char* >(local);
  if (s && *s)
  {
    if (m_beginning_of_line && m_indent.IsNotEmpty())
      AppendText(static_cast<const char*>(m_indent));
    AppendText(s);
  }
}


void MYON_TextLog::PrintString( const char* s )
{
  if (s && *s)
  {
    if (m_beginning_of_line && m_indent.IsNotEmpty())
      AppendText(static_cast<const char*>(m_indent));
    AppendText(s);
  }
}

void MYON_TextLog::PrintString(MYON_String s)
{
  PrintString(static_cast<const char*>(s));
}

void MYON_TextLog::PrintNewLine()
{
  Print("\n");
}

void MYON_TextLog::PrintSpace()
{
  Print("\n");
}

void MYON_TextLog::PrintTab()
{
  Print("\t");
}


void MYON_TextLog::PrintString( const wchar_t* s )
{
  if (s && *s)
  {
    if (m_beginning_of_line && m_indent.IsNotEmpty())
      AppendText(static_cast<const char*>(m_indent));
    AppendText(s);
  }
}

void MYON_TextLog::PrintString(MYON_wString s)
{
  PrintString(static_cast<const wchar_t*>(s));
}

void MYON_TextLog::PrintRGB(const MYON_Color& color)
{
  color.ToText(MYON_Color::TextFormat::DecimalRGB, 0, true, *this);
}

void MYON_TextLog::PrintColor(const MYON_Color& color)
{
  color.ToText(this->GetColorFormat(), 0, true, *this);
}

void MYON_TextLog::PrintCurrentTime()
{
  struct tm current_time;
  memset(&current_time, 0, sizeof(current_time));
  time_t uct = time(nullptr);
  const struct tm* t = gmtime(&uct);
  if (t)
  {
    current_time = *t;
  }
  PrintTime(current_time);
}

void MYON_TextLog::PrintTime( const struct tm& t )
{
  if (   0 != t.tm_sec
      || 0 != t.tm_min
      || 0 != t.tm_hour
      || 0 != t.tm_mday
      || 0 != t.tm_mon
      || 0 != t.tm_year
      || 0 != t.tm_wday
    )
  {
    const char* sDayName[8] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","<invalid day>"};
    const char* sMonName[13] = {"January","February","March","April","May","June",
                               "July","August","September","October","November","December","<invalid month>"};
    int wday = t.tm_wday;
    if ( wday < 0 || wday > 6 )
      wday = 7;
    int mon = t.tm_mon;
    if ( mon < 0 || mon > 11 )
      mon = 12;

    Print("%s %s %02d %02d:%02d:%02d %4d",
                sDayName[wday],
                sMonName[mon],
                t.tm_mday,
                t.tm_hour,
                t.tm_min,
                t.tm_sec,
                t.tm_year+1900);
  }
}


void MYON_TextLog::PrintPointList( int dim, bool is_rat, int count, int stride, const double* P,
                                const char* sPreamble )
{
  double w, x;
  int i, j, cvdim;

  MYON_String preamble = "";
  if ( sPreamble && *sPreamble )
    preamble += sPreamble;
  cvdim = (is_rat) ? dim+1 : dim;

  if ( count == 0 ) {
    Print( "%sEMPTY point list\n", preamble.Array() );
  }
  else if ( !P ) {
    Print( "%sNULL point list\n", preamble.Array() );
  }

  for ( i = 0; i < count; i++ ) {
    Print( "%s[%2d] %c", preamble.Array(), i, (is_rat) ? '[' : '(' );
    Print( static_cast< const char* >(m_double_format), P[0] );
    for ( j = 1; j < cvdim; j++ ) {
      Print( ", ");
      Print(static_cast< const char* >(m_double_format), P[j] );
    }
    Print("%c", (is_rat) ? ']' : ')' );
    if ( is_rat ) 
    {
      w = P[dim];
      if ( w != 0.0 ) 
      {
        // print euclidean coordinates
        w = 1.0/w;
        x = w*P[0];
        Print( " = (");
        Print( static_cast< const char* >(m_double_format), x );
        for ( j = 1; j < dim; j++ ) 
        {
          x = w*P[j];
          Print( ", ");
          Print( static_cast< const char* >(m_double_format), x );
        }
        Print(")");
      }
    }
    Print("\n");
    P += stride;
  }
}

void MYON_TextLog::PrintPointGrid( int dim, bool is_rat, 
                                int point_count0, int point_count1, 
                                int point_stride0, int point_stride1,
                                const double* P,
                                const char* sPreamble )
{
  char s[1024];
  const size_t s_capacity = sizeof(s) / sizeof(s[0]);
  int i;
  if (!sPreamble || !sPreamble[0])
    sPreamble = "point";
  for ( i = 0; i < point_count0; i++ ) 
  {
    MYON_String::FormatIntoBuffer(s, s_capacity, "%s[%2d]", sPreamble, i);
    PrintPointList( dim, is_rat, point_count1, point_stride1, P + i*point_stride0, s );
  }
}

void MYON_TextLog::PrintKnotVector( int order, int cv_count, const double* knot )
{
  int i, i0, mult, knot_count;
  if ( !knot )
    Print("nullptr knot vector\n");
  if ( order < 2 )
    Print("knot vector order < 2\n");
  if ( cv_count < order )
    Print("knot vector cv_count < order\n");
  if ( order >= 2 && cv_count >= order && knot ) {
    knot_count = MYON_KnotCount( order, cv_count );
    i = i0 = 0;
    Print("index                     value  mult       delta\n");
    while ( i < knot_count ) {
      mult = 1;
      while ( i+mult < knot_count && knot[i] == knot[i+mult] )
        mult++;
      if ( i == 0 ) {
        Print( "%5d  %23.17g  %4d\n", i, knot[i], mult );
      }
      else {
        Print( "%5d  %23.17g  %4d  %10.4g\n", i, knot[i], mult, knot[i]-knot[i0] );
      }
      i0 = i;
      i += mult;
    }
  }
}

void MYON_TextLog::Print( const MYON_3dPointArray& a, const char* sPreamble )
{
  const double* p = (a.Array() ? &a.Array()[0].x : nullptr );
  PrintPointList( 3, false, a.Count(), 3, p, sPreamble );
}

void MYON_TextLog::Print( const MYON_Matrix& M, const char* sPreamble, int precision )
{
  double x;
  char digit[10] = {'0','1','2','3','4','5','6','7','8','9'};
  char* sRow;
  char* sIJ;
  int xi, row_count, column_count, row_index, column_index;
  
  row_count = M.RowCount();
  column_count = M.ColCount();

  sRow = (char*)alloca( (5*column_count + 2 + 64)*sizeof(*sRow) );

  if ( !sPreamble )
    sPreamble = "Matrix";

  Print("%s (%d rows %d columns)\n",sPreamble,row_count,column_count);
  for ( row_index = 0; row_index < row_count; row_index++ ) {
    sIJ = sRow;
    Print("%5d:",row_index);
    if ( precision > 3 ) {
      for ( column_index = 0; column_index < column_count; column_index++ ) {
        x = M.m[row_index][column_index];
        Print( " %8f",x);
      }
      Print("\n");
    }
    else {
      for ( column_index = 0; column_index < column_count; column_index++ ) {
        x = M.m[row_index][column_index];
        if ( x == 0.0 ) {
          strcpy( sIJ, "  0   " );
          sIJ += 4;
        }
        else {
          *sIJ++ = ' ';
          *sIJ++ = ( x >0.0 ) ? '+' : '-';
          x = fabs( x );
          if      ( x >= 10.0 ) {
            *sIJ++ = '*';
            *sIJ++ = ' ';
            *sIJ++ = ' ';
          }
          else if ( x <= MYON_SQRT_EPSILON) {
            *sIJ++ = '0';
            *sIJ++ = ' ';
            *sIJ++ = ' ';
          }
          else if ( x < 0.1) {
            *sIJ++ = '~';
            *sIJ++ = ' ';
            *sIJ++ = ' ';
          }
          else if ( x < .95 ) {
            *sIJ++ = '.';
            xi = (int)floor(x*10.0);
            if ( xi > 9 )
              xi = 9;
            else if (xi < 1)
              xi = 1;
            *sIJ++ = digit[xi];
            *sIJ++ = '~';
          }
          else {
            xi = (int)floor(x);
            if ( xi < 1 )
              xi = 1;
            else if (xi > 9)
              xi = 9;
            *sIJ++ = digit[xi];
            if ( x == floor(x) ) {
              *sIJ++ = ' ';
              *sIJ++ = ' ';
            }
            else {
              *sIJ++ = '.';
              *sIJ++ = '~';
            }
          }
        }
      }
      *sIJ = 0;
      Print("%s\n",sRow);
    }
  }
}

MYON_TextLog& MYON_TextLog::operator<<(const char* s)
{
  Print( "%s", s );
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<(char c)
{
  Print( "%c", c );
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<(short i)
{
  int ii = (int)i;
  Print("%d", ii );
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<(int i)
{
  Print("%d",i);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<(float x)
{
  Print(static_cast< const char* >(m_float_format),x);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<(double x)  
{
  Print(static_cast< const char* >(m_double_format),x);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<( const MYON_2dPoint& p )
{
  Print(p);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<( const MYON_3dPoint& p )
{
  Print(p);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<( const MYON_4dPoint& p )
{
  Print(p);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<( const MYON_2dVector& p )
{
  Print(p);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<( const MYON_3dVector& p )
{
  Print(p);
  return *this;
}

MYON_TextLog& MYON_TextLog::operator<<( const MYON_Xform& xform )
{
  Print(xform);
  return *this;
}

void MYON_TextLog::PrintWrappedText( const char* s, int line_length )
{
  MYON_wString ws = s;
  PrintWrappedText(static_cast< const wchar_t* >(ws),line_length);
}

static void wsncpy(wchar_t* dst, const wchar_t* src, int n)
{
  // can't use _wcsncpy() because this has to compile on UNIX boxes
  if ( dst && n > 0 ) {
    if ( src ) {
      while ( 0 != (*dst++ = *src++) && n-- > 0 );
    }
    else
      *dst = 0;
  }
}


void MYON_TextLog::PrintWrappedText( const wchar_t* s, int line_length )
{
  MYON_Workspace ws;
  if ( s && *s && line_length > 0 ) {
    const int max_line_length = line_length+255;
    wchar_t* sLine = (wchar_t*)ws.GetMemory((max_line_length+1)*sizeof(*sLine));
    const int wrap_length = line_length;
    int i  = 0;
    int i1 = 0;
    int isp = 0;
    bool bPrintLine = false;
    while ( s[i] ) {
      i1 = i;
      if ( s[i] == 10 || s[i] == 13 ) {
        // hard break at CR or LF
        i++;
        if ( s[i] == 10 && s[i-1] == 13 ) {
          // it's a CR+LF hard end of line - skip LF too
          i++;
        }      
        bPrintLine = true;
      }
      else if ( i && s[i] == 32 ) {
        if ( !isp ) {
          isp = i++;
        }
        if ( i < wrap_length ) {
          isp = i++;
        }
        else {
          bPrintLine = true;
          if ( isp ) {
            i1 = i = isp;
            while ( s[i] == 32 )
              i++;
          }
          else {
            i++;
          }
        }
      }
      else {
        i++;
      }
      if ( bPrintLine ) {
        if ( i1 >= max_line_length )
          i1 = max_line_length-1;
        if ( i1 > 0 ) {
          wsncpy( sLine, s, i1 );
          sLine[i1] = 0;
          Print( "%ls\n", sLine );
        }
        else {
          Print("\n");
        }

        s += i;
        i = i1 = isp = 0;
        bPrintLine = false;
      }
    }
    if ( s[0] ) {
      Print( "%ls", s );
    }
  }
}



bool MYON_TextHash::Internal_IsHexDigit(char c)
{
  if (c >= '0' && c <= '9')
    return true;
  if (c >= 'a' && c <= 'f')
    return true;
  if (c >= 'A' && c <= 'F')
    return true;
  return false;
}

const char* MYON_TextHash::Internal_ParseId(
  const char* s,
  MYON_UUID* id
)
{
/*
XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
*/
  const int digit_count[] = { 8,4,4,4,12,0 };
  const int* count = digit_count;
  int c;
  const char* s1 = s;
  while (0 != (c = *count++))
  {
    for (int i = 0; i < c; i++)
    {
      if ( Internal_IsHexDigit(*s1++) )
        continue;
      if (nullptr != id)
        *id = MYON_nil_uuid;
      return nullptr;
    }
    if (0 == *count)
    {
      const char* s2 = MYON_ParseUuidString(s, id);
      if (s2 == s1 
        && '-' != *s1 
        && false == Internal_IsHexDigit(*s1)
        )
      {
        return s1;
      }
      break;
    }
    if ( '-' == *s1++)
      continue;  
    break;
  }
  if (nullptr != id)
    *id = MYON_nil_uuid;
  return nullptr;
}

bool MYON_TextLog::IsTextHash() const
{
  return (nullptr != dynamic_cast<const MYON_TextHash*>(this));
}

bool MYON_TextLog::IsNull() const
{
  return this == &MYON_TextLog::Null;
}

MYON_StringMapType MYON_TextHash::StringMapType() const
{
  return m_string_map_type;
}

const class MYON_Locale& MYON_TextHash::StringMapLocale() const
{
  return m_string_map_local;
}

void MYON_TextHash::SetStringMap(
  MYON_StringMapOrdinalType map_type
)
{
  SetStringMap(MYON_Locale::Ordinal, MYON_StringMapType::Identity);
  m_string_map_ordinal_type = map_type;
  m_bApplyStringMap 
    = MYON_StringMapType::Identity != m_string_map_type
    || MYON_StringMapOrdinalType::Identity != m_string_map_ordinal_type;
}


void MYON_TextHash::SetStringMap(
  const class MYON_Locale& locale,
  MYON_StringMapType map_type
)
{
  m_string_map_local = locale;
  m_string_map_type = map_type;
  m_string_map_ordinal_type
    = (m_string_map_local.IsOrdinalOrInvariantCulture())
    ? MYON_StringMapOrdinalTypeFromStringMapType(m_string_map_type)
    : MYON_StringMapOrdinalType::Identity;
  m_bApplyStringMap 
    = MYON_StringMapType::Identity != m_string_map_type
    || MYON_StringMapOrdinalType::Identity != m_string_map_ordinal_type;
}

void MYON_TextHash::SetIdRemap(
  bool bEnableIdRemap
)
{
  m_bApplyIdRemap = bEnableIdRemap ? true : false;
}

bool MYON_TextHash::IdRemap() const
{
  return m_bApplyIdRemap;
}

void MYON_TextHash::SetOutputTextLog(
  MYON_TextLog* output_text_log
)
{
  m_output_text_log = output_text_log;
}

MYON_TextLog* MYON_TextHash::OutputTextLog() const
{
  return m_output_text_log;
}


MYON__UINT64 MYON_TextHash::ByteCount() const
{
  return m_sha1.ByteCount();
}


MYON_SHA1_Hash MYON_TextHash::Hash() const
{
  return m_sha1.Hash();
}


void MYON_TextHash::AppendText(const char* s)
{
  if (false == m_bApplyIdRemap)
  {
    // no id remapping - just accumulate
    m_sha1.AccumulateString(s, -1, m_string_map_ordinal_type);
    return;
  }

  size_t element_count;
  const char* s0;
  for ( s0 = s; 0 != *s; s++ )
  {
    MYON_UUID original_id;
    const char* s1 = Internal_ParseId(s, &original_id);
    if (nullptr == s1)
      continue;

    // A UUID beings at s and ends at s1.

    // Accumulate everything up to s
    element_count = s - s0;
    if (element_count > 0)
    {
      m_sha1.AccumulateString(s0, (int)element_count, m_string_map_ordinal_type);
      if (nullptr != m_output_text_log)
      {
        MYON_String tmp(s0, (int)element_count);
        m_output_text_log->AppendText(static_cast<const char*>(tmp));
      }
    }

    // remap original_id to sequential_id
    MYON_UUID sequential_id;
    if (false == m_remap_id_list.FindId1(original_id, &sequential_id))
    {
      // First time original_id has appeared in the stream.
      m_remap_id = MYON_NextNotUniqueId(m_remap_id);
      sequential_id = m_remap_id;
      m_remap_id_list.AddPair(original_id, sequential_id);
    }

    // accumlate string version of sequential_id
    char sequential_id_str[40];
    MYON_UuidToString(sequential_id, sequential_id_str);
    sequential_id_str[36] = 0;
    m_sha1.AccumulateString(sequential_id_str, 36, m_string_map_ordinal_type);
    if (nullptr != m_output_text_log)
    {
      m_output_text_log->AppendText(sequential_id_str);
    }
    s0 = s1; // s0 = first element after the original uuid.
    s = s1 - 1; // because the for() loop incrementer is s++
  }

  element_count = s - s0;
  if (element_count > 0)
  {
    m_sha1.AccumulateString(s0, (int)element_count, m_string_map_ordinal_type);
    if (nullptr != m_output_text_log)
      m_output_text_log->AppendText(s0);
  }
}

void MYON_TextHash::AppendText(const wchar_t* s)
{
  MYON_String sUTF8_buffer(s);
  const char* sUTF8 = sUTF8_buffer;
  AppendText(sUTF8);
}


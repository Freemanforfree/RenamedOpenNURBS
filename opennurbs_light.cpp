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

MYON_OBJECT_IMPLEMENT(MYON_Light,MYON_Geometry,"85A08513-F383-11d3-BFE7-0010830122F0");

void MYON_Light::Default()
{
  m_light_name.Destroy();
  m_bOn = true;
  m_intensity = 1.0;
  m_watts = 0.0;
  m_style =  MYON::camera_directional_light;
  m_ambient = MYON_Color(0,0,0);
  m_diffuse = MYON_Color(255,255,255);
  m_specular = MYON_Color(255,255,255);
  m_direction = MYON_3dVector(0.0,0.0,-1.0);
  m_location = MYON_3dPoint(0.0,0.0,0.0);
  m_length = MYON_3dVector(0.0,0.0,0.0);
  m_width = MYON_3dVector(0.0,0.0,0.0);
  m_spot_angle = 180.0;
  m_spot_exponent = 0.0;
  m_hotspot = 1.0;
  m_attenuation = MYON_3dVector(1.0,0.0,0.0);
  m_shadow_intensity = 1.0;
  m_light_index = 0;
  memset(&m_light_id,0,sizeof(m_light_id));
}

MYON_Light::MYON_Light() 
{
  Default();
}

MYON_Light::~MYON_Light()
{
}

bool MYON_Light::IsValid( MYON_TextLog* text_log ) const
{
  int s = Style();
  if ( s <= MYON::unknown_light_style || s >= MYON::light_style_count ) {
    MYON_ERROR("MYON_Light::IsValid(): illegal light style.");
    return false;
  }
  return true;
}

void MYON_Light::Dump( MYON_TextLog& dump ) const
{
  bool bDumpDir = false;
  bool bDumpLength = false;
  bool bDumpWidth = false;

  const char* sStyle = "unknown";
  switch(Style())
  {
  //case MYON::view_directional_light:
  //  sStyle = "view_directional_light";
  //  bDumpDir = true;
  //  break;
  //case MYON::view_point_light:
  //  sStyle = "view_point_light";
  //  break;
  //case MYON::view_spot_light:
  //  sStyle = "view_spot_light";
  //  bDumpDir = true;
  //  break;
  case MYON::camera_directional_light:
    sStyle = "camera_directional_light";
    bDumpDir = true;
    break;
  case MYON::camera_point_light:
    sStyle = "camera_point_light";
    break;
  case MYON::camera_spot_light:
    sStyle = "camera_spot_light";
    bDumpDir = true;
    break;
  case MYON::world_directional_light:
    sStyle = "world_directional_light";
    bDumpDir = true;
    break;
  case MYON::world_point_light:
    sStyle = "world_point_light";
    break;
  case MYON::world_spot_light:
    sStyle = "world_spot_light";
    bDumpDir = true;
    break;
  case MYON::world_linear_light:
    sStyle = "linear_light";
    bDumpDir = true;
    bDumpLength = true;
    break;
  case MYON::world_rectangular_light:
    sStyle = "rectangular_light";
    bDumpDir = true;
    bDumpLength = true;
    bDumpWidth = true;
    break;
  case MYON::ambient_light:
    sStyle = "ambient_light";
    break;
  case MYON::unknown_light_style:
    sStyle = "unknown";
    break;
  default:
    sStyle = "unknown";
    break;
  }
  dump.Print("index = %d  style = %s\n",LightIndex(),sStyle);

  dump.Print("location = "); dump.Print(Location()); dump.Print("\n");
  if ( bDumpDir )
    dump.Print("direction = "); dump.Print(Direction()); dump.Print("\n");
  if ( bDumpLength )
    dump.Print("length = "); dump.Print(Length()); dump.Print("\n");
  if ( bDumpWidth )
    dump.Print("width = "); dump.Print(Width()); dump.Print("\n");

  dump.Print("intensity = %g%%\n",Intensity()*100.0);

  dump.Print("ambient rgb = "); dump.PrintRGB(Ambient()); dump.Print("\n");
  dump.Print("diffuse rgb = "); dump.PrintRGB(Diffuse()); dump.Print("\n");
  dump.Print("specular rgb = "); dump.PrintRGB(Specular()); dump.Print("\n");

  dump.Print("spot angle = %g degrees\n",SpotAngleDegrees());
}

bool MYON_Light::Write(
       MYON_BinaryArchive& file
     ) const
{
  int i;
  bool rc = file.Write3dmChunkVersion(1,2);
  // version 1.0 fields
  if ( rc ) rc = file.WriteInt( m_bOn?1:0 );
  i = m_style;
  if ( rc ) rc = file.WriteInt( i );
  if ( rc ) rc = file.WriteDouble( m_intensity );
  if ( rc ) rc = file.WriteDouble( m_watts );
  if ( rc ) rc = file.WriteColor( m_ambient );
  if ( rc ) rc = file.WriteColor( m_diffuse );
  if ( rc ) rc = file.WriteColor( m_specular );
  if ( rc ) rc = file.WriteVector( m_direction );
  if ( rc ) rc = file.WritePoint( m_location );
  if ( rc ) rc = file.WriteDouble( m_spot_angle );
  if ( rc ) rc = file.WriteDouble( m_spot_exponent );
  if ( rc ) rc = file.WriteVector( m_attenuation );
  if ( rc ) rc = file.WriteDouble( m_shadow_intensity );
  if ( rc ) rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::RenderLight, m_light_index );
  if ( rc ) rc = file.WriteUuid( m_light_id );
  if ( rc ) rc = file.WriteString( m_light_name );
  // version 1.1 added support for linear and rectangular
  if ( rc ) rc = file.WriteVector( m_length );
  if ( rc ) rc = file.WriteVector( m_width );
  // version 1.2 added m_hotspot support
  if ( rc ) rc = file.WriteDouble( m_hotspot );
  return rc;
}

bool MYON_Light::Read(
       MYON_BinaryArchive&  file
     )
{
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( rc && major_version == 1 ) {
    int i;
    // version 1.0 fields
    i = 0;
    if ( rc ) rc = file.ReadInt( &i );
    if ( rc ) Enable(0 != i);
    if ( rc ) rc = file.ReadInt( &i );
    if ( rc ) SetStyle(MYON::LightStyle(i));
    if ( rc ) rc = file.ReadDouble( &m_intensity );
    if ( rc ) rc = file.ReadDouble( &m_watts );
    if ( rc ) rc = file.ReadColor( m_ambient );
    if ( rc ) rc = file.ReadColor( m_diffuse );
    if ( rc ) rc = file.ReadColor( m_specular );
    if ( rc ) rc = file.ReadVector( m_direction );
    if ( rc ) rc = file.ReadPoint( m_location );
    if ( rc ) rc = file.ReadDouble( &m_spot_angle );
    if ( rc ) rc = file.ReadDouble( &m_spot_exponent );
    if ( rc ) rc = file.ReadVector( m_attenuation );
    if ( rc ) rc = file.ReadDouble( &m_shadow_intensity );
    if ( rc ) rc = file.ReadInt( &m_light_index );
    if ( rc ) rc = file.ReadUuid( m_light_id );
    if ( rc ) rc = file.ReadString( m_light_name );

    if ( minor_version < 2 ) {
      // set hotspot from 1.0 or 1.1 m_spot_exponent
      double h = 1.0 - m_spot_exponent/128.0;
      if ( h < 0.0 ) 
        h = 0.0;
      else if ( h > 1.0 )
        h = 1.0;
      m_hotspot = h;
      m_spot_exponent = 0.0;
    }

    if ( minor_version >= 1 ) {
      // version 1.1 fields
      if ( rc ) rc = file.ReadVector( m_length );
      if ( rc ) rc = file.ReadVector( m_width );
      if ( minor_version >= 2 ) {
        // version 1.2 fields
        if ( rc ) rc = file.ReadDouble( &m_hotspot );
      }
    }
  }
  return rc;
}

MYON::object_type MYON_Light::ObjectType() const
{
  return MYON::light_object;
}

int MYON_Light::Dimension() const
{
  return 3;
}

bool MYON_Light::GetBBox( // returns true if successful
       double* boxmin,    // boxmin[dim]
       double* boxmax,    // boxmax[dim]
       bool bGrowBox
       ) const
{
  bool rc = true;
  MYON_3dPointArray points(16);

  switch(m_style)
  {
  case MYON::camera_directional_light:
  case MYON::world_directional_light:
    points.Append(m_location);
    points.Append(m_location+m_direction);
    break;

  case MYON::camera_point_light:
  case MYON::world_point_light:
    points.Append(m_location);
    break;

  case MYON::camera_spot_light:
  case MYON::world_spot_light:
    if ( m_spot_angle > 0.0 && m_spot_angle < 90.0 )
    {
      double r = m_direction.Length()*tan(MYON_PI*m_spot_angle/180.0);
      MYON_Circle c(MYON_Plane(m_location+m_direction,m_direction),r);
      MYON_BoundingBox cbox = c.BoundingBox();
      cbox.GetCorners( points );
    }
    else
    {
      points.Append(m_location+m_direction);
    }
    points.Append(m_location);
    break;

  case MYON::ambient_light:
    points.Append(m_location);
    rc = false;
    break;
  
  case MYON::world_linear_light:
    points.Append(m_location);
    points.Append(m_location+m_length);
    break;

  case MYON::world_rectangular_light:
    points.Append(m_location);
    points.Append(m_location+m_length);
    points.Append(m_location+m_width);
    points.Append(m_location+m_width+m_length);
    {
      // include target and direction marker to avoid display clipping
      MYON_3dPoint center(m_location+(m_width+m_length)*0.5);
      points.Append(center+m_direction);
      MYON_3dVector marker(m_direction); 
      marker.Unitize();
      marker *= (m_width+m_length).Length()/12.0; // from GetRectangularLightSegments
      points.Append(center+marker);
    }
    break;

  default:
    rc = false;
    break;
  }

  if ( rc && points.Count() > 0 )
  {
     rc = MYON_GetPointListBoundingBox( 3, 0, points.Count(), 3, 
                                      (double*)points.Array(), 
                                      boxmin, boxmax, 
                                      bGrowBox?true:false )
        ? true 
        : false;
  }

  return rc;
}

bool MYON_Light::GetTightBoundingBox(
  MYON_BoundingBox& tight_bbox,
  bool bGrowBox,
  const MYON_Xform* xform
) const
{
  // only spotlights have a tight bbox that differs from regular bbox
  if (MYON::world_spot_light != m_style && MYON::camera_spot_light != m_style)
    return MYON_Geometry::GetTightBoundingBox(tight_bbox, bGrowBox, xform);

  if (bGrowBox && !tight_bbox.IsValid())
  {
    bGrowBox = false;
  }
  if (!bGrowBox)
  {
    tight_bbox.Destroy();
  }

  bool rc = true;
  MYON_3dPointArray points(16);
  if (m_spot_angle > 0.0 && m_spot_angle < 90.0)
  {
    double r = m_direction.Length() * tan(MYON_PI * m_spot_angle / 180.0);
    MYON_Circle c(MYON_Plane(m_location + m_direction, m_direction), r);
    MYON_BoundingBox cbox;
    c.GetTightBoundingBox(cbox);
    cbox.GetCorners(points);
  }
  else
  {
    points.Append(m_location + m_direction);
  }
  points.Append(m_location);

  if (points.Count() > 0)
  {
    rc = MYON_GetPointListBoundingBox(3, 0, points.Count(), 3,
      (double*)points.Array(),
      tight_bbox.m_min, tight_bbox.m_max,
      bGrowBox ? true : false)
      ? true
      : false;
  }

  return rc;
}


bool MYON_Light::Transform( 
       const MYON_Xform& xform
       )
{
  MYON_3dVector v;
  double vlen;
  TransformUserData(xform);
  m_location = xform*m_location;
  
  v = xform*m_direction;
  vlen = v.Length();
  if ( vlen > 0.0 ) {
    m_direction = v;
  }
  
  v = xform*m_length;
  vlen = v.Length();
  if ( vlen > 0.0 ) {
    m_length = v;
  }
  
  v = xform*m_width;
  vlen = v.Length();
  if ( vlen > 0.0 ) {
    m_width = v;
  }
  return true;
}

bool MYON_Light::Enable(bool b)
{
  bool oldb = m_bOn;
  m_bOn = (b)?true:false;
  return oldb;
}

bool MYON_Light::IsEnabled() const
{
  return m_bOn;
}

MYON::light_style MYON_Light::Style() const
{
  return m_style;
}

const bool MYON_Light::IsPointLight() const
{
  bool rc;
  switch(m_style)
  {
  //case MYON::view_point_light:
  case MYON::camera_point_light:
  case MYON::world_point_light:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

const bool MYON_Light::IsDirectionalLight() const
{
  bool rc;
  switch(m_style)
  {
  //case MYON::view_directional_light:
  case MYON::camera_directional_light:
  case MYON::world_directional_light:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

const bool MYON_Light::IsSpotLight() const
{
  bool rc;
  switch(m_style)
  {
  //case MYON::view_spot_light:
  case MYON::camera_spot_light:
  case MYON::world_spot_light:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

const bool MYON_Light::IsLinearLight() const
{
  bool rc;
  switch(m_style)
  {
  //case MYON::view_linear_light:
  //case MYON::camera_linear_light:
  case MYON::world_linear_light:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}

const bool MYON_Light::IsRectangularLight() const
{
  bool rc;
  switch(m_style)
  {
  //case MYON::view_rectangular_light:
  //case MYON::camera_rectangular_light:
  case MYON::world_rectangular_light:
    rc = true;
    break;
  default:
    rc = false;
    break;
  }
  return rc;
}


void MYON_Light::SetStyle(MYON::light_style s )
{
  m_style = s;
}

void MYON_Light::SetLightName( const char* s )
{
  m_light_name = s;
  m_light_name.TrimLeftAndRight();
}

void MYON_Light::SetLightName( const wchar_t* s )
{
  m_light_name = s;
  m_light_name.TrimLeftAndRight();
}

const MYON_wString& MYON_Light::LightName() const
{
  return m_light_name;
}

//void MYON_Light::SetLightUuid( const MYON_UUID& uuid )
//{
//  // m_light_id is set and maintained by Rhino - if your
//  // plug-in is messing with this field, fix the plugin
//  m_light_id = uuid;
//}

//const MYON_UUID& MYON_Light::LightUuid() const
//{
//  // m_light_id is set and maintained by Rhino - if your
//  // plug-in is messing with this field, fix the plugin
//  return m_light_id;
//}


void MYON_Light::SetAttenuation(double a,double b,double c)
{
  m_attenuation = MYON_3dVector(a,b,c);
}

void MYON_Light::SetAttenuation(const MYON_3dVector& att )
{
  m_attenuation = att;
}

MYON_3dVector MYON_Light::Attenuation() const
{
  return m_attenuation;
}

double MYON_Light::Attenuation(double d) const
{
  // computes 1/(a[0] + d*a[1] + d^2*a[2]) where d = argument
  // returns 0 if a[0] + d*a[1] + d^2*a[2] <= 0
  double a = m_attenuation.x + d*(m_attenuation.y + d*m_attenuation.z);
  if ( a > 0.0 )
    a = 1.0/a;
  else
    a = 0.0;
  return a;
}

/////////////////////////////////////////////////////////
//
// spot light parameters (ignored for non-spot lights)
//
// angle = 0 to 90 degrees
// exponent = 0 to 128 (0=uniform, 128=high focus)
//
void MYON_Light::SetSpotAngleRadians( double a )
{
  a *= 180.0/MYON_PI;
  if ( a > 90.0 )
    m_spot_angle = 90.0;
  else if ( a > 0.0 )
    m_spot_angle = a;
}

double MYON_Light::SpotAngleRadians() const
{
  return m_spot_angle*MYON_PI/180.0;
}

void MYON_Light::SetSpotAngleDegrees( double a )
{
  if ( a >= 90.0 )
    m_spot_angle = 90.0;
  else if ( a > 0.0 )
    m_spot_angle = a;
}

double MYON_Light::SpotAngleDegrees() const
{
  return m_spot_angle;
}

// The spot exponent "e", hot spot "h", and spotlight cone angle "a"
// are mutually constrained by the formula
//   cos(h*angle)^e = hotspot_min
// where hotspot_min = value of spotlight exponential attenuation factor 
// at the hot spot radius.  hotspot_min must be >0, < 1, and should be >= 1/2;
//static double log_hotspot_min = log(0.5);
static double log_hotspot_min = log(0.70710678118654752440084436210485);

void MYON_Light::SetSpotExponent( double e )
{
  // cos(h)^e = 0.5
  if ( e < 0.0 || !MYON_IsValid(e) )
    m_spot_exponent = 0.0;
  else
    m_spot_exponent = e;
  m_hotspot = MYON_UNSET_VALUE; // indicates hotspot should be computed from m_spot_exponent
}

void MYON_Light::SetHotSpot( double h )
{
  if ( h == MYON_UNSET_VALUE || !MYON_IsValid(h) )
    m_hotspot = MYON_UNSET_VALUE;
  else if ( h <= 0.0 ) 
    m_hotspot = 0.0;
  else if ( h >= 1.0 )
    m_hotspot = 1.0;
  else
    m_hotspot = h;
}

double MYON_Light::SpotExponent() const
{
  double e = m_spot_exponent;
  if ( m_hotspot >= 0.0 && m_hotspot <= 1.0 ) {
    // spotlight is using hot spot interface
    double h = m_hotspot;
    if ( h < 0.015 )
      h = 0.015;
    if ( h >= 1.0 || m_spot_angle <= 0.0 || m_spot_angle > 90.0)
      e = 0.0;
    else {
      // compute SpotExponent() from  cos(h*angle)^e = hotspot_min
      double a, c;
      a = h*SpotAngleRadians();
      c = cos(a);
      if ( c <= 0.0 )
        e = 1.0;
      else {
        e = log_hotspot_min/log(c);
        if ( e < 0.0 )
          e = 0.0;
      }
    }
  }
  return e;
}

double MYON_Light::HotSpot() const
{
  double h = m_hotspot;
  if ( h < 0.0 || h > 1.0 ) {
    // spotlight is using spot exponent interface
    if ( m_spot_exponent >= 65536.0 )
      h = 0.0;
    else if ( m_spot_exponent <= 0.0 || m_spot_angle <= 0.0 || m_spot_angle > 90.0 )
      h = 1.0;
    else {
      // compute HotSpot() from cos(h*angle)^e = hotspot_min
      double x, a, cos_ha;
      x = log_hotspot_min/m_spot_exponent; // note that x < 0.0
      if ( x < -690.0 ) {
        // prevent underflow.  cos_ha is close to zero so
        h = 1.0;
      }
      else 
      {
        cos_ha = exp(x);
        if (!MYON_IsValid(cos_ha))  cos_ha =  0.0;
        else if ( cos_ha >  1.0 ) cos_ha =  1.0;
        else if ( cos_ha < -1.0 ) cos_ha = -1.0;
        a = SpotAngleRadians();
        h = acos(cos_ha)/a;
        if ( h < 0.0 )
          h = 0.0;
        else if ( h > 1.0 ) {
          // happens for smaller e
          h = 1.0;
        }
      }
    }
  }
  return h;
}

void MYON_Light::SetLength( const MYON_3dVector& v )
{
  m_length = v;
}

MYON_3dVector MYON_Light::Length() const
{
  return m_length;
}

void MYON_Light::SetWidth( const MYON_3dVector& v )
{
  m_width = v;
}

MYON_3dVector MYON_Light::Width() const
{
  return m_width;
}


void MYON_Light::SetShadowIntensity(double s )
{
  if ( s < 0.0 )
    s = 0.0;
  else if (s > 1.0 )
    s = 1.0;
  m_shadow_intensity = s;
}

double MYON_Light::ShadowIntensity() const
{
  return m_shadow_intensity;
}

void MYON_Light::SetLightIndex( int i )
{
  m_light_index = i;
}

int MYON_Light::LightIndex() const
{
  return m_light_index;
}

void MYON_Light::SetAmbient(  MYON_Color c )
{
  m_ambient = c;
}

void MYON_Light::SetDiffuse(  MYON_Color c )
{
  m_diffuse = c;
}

void MYON_Light::SetSpecular( MYON_Color c )
{
  m_specular = c;
}

MYON_Color MYON_Light::Ambient() const
{
  return m_ambient;
}

MYON_Color MYON_Light::Diffuse() const
{
  return m_diffuse;
}

MYON_Color MYON_Light::Specular() const
{
  return m_specular;
}

MYON::coordinate_system MYON_Light::CoordinateSystem() const // determined by style
{
  MYON::coordinate_system cs = MYON::world_cs;
  switch( m_style ) {
  case MYON::unknown_light_style:
    cs = MYON::world_cs;
    break;
  //case MYON::view_directional_light:
  //case MYON::view_point_light:
  //case MYON::view_spot_light:
  //  cs = MYON::clip_cs;
  //  break;
  case MYON::camera_directional_light:
  case MYON::camera_point_light:
  case MYON::camera_spot_light:
    cs = MYON::camera_cs;
    break;
  case MYON::world_directional_light:
  case MYON::world_point_light:
  case MYON::world_spot_light:
  case MYON::world_linear_light:
  case MYON::world_rectangular_light:
  case MYON::ambient_light:
    cs = MYON::world_cs;
    break;
  default:
    cs = MYON::world_cs;
    break;
  }
  return cs;
}

bool MYON_Light::GetLightXform( 
         const MYON_Viewport& vp,
         MYON::coordinate_system dest_cs, 
         MYON_Xform& xform 
         ) const
{
  MYON::coordinate_system src_cs = CoordinateSystem();
  return vp.GetXform( src_cs, dest_cs, xform );
}



void MYON_Light::SetLocation( const MYON_3dPoint& loc )
{
  m_location = loc;
}

void MYON_Light::SetDirection( const MYON_3dVector& dir )
{
  m_direction = dir;
}

MYON_3dPoint MYON_Light::Location() const
{
  return m_location;
}

MYON_3dVector MYON_Light::Direction() const
{
  return m_direction;
}

MYON_3dVector MYON_Light::PerpindicularDirection() const
{
  // returns a consistent vector perpendicular to the
  // light's direction.  This vector is useful for
  // user interface display.
  MYON_3dVector dir = m_direction;
  if ( !dir.IsValid() || !dir.Unitize() )
    return MYON_3dVector::UnsetVector;

  MYON_3dVector xdir;
  if ( IsLinearLight() || IsRectangularLight() )
  {
    xdir = m_length;
    if ( xdir.IsValid() && xdir.Unitize() && fabs(xdir*dir) <= MYON_SQRT_EPSILON )
      return xdir;
  }

  if( dir.IsParallelTo( MYON_3dVector::ZAxis, MYON_DEGREES_TO_RADIANS * 3.0))
    xdir = MYON_CrossProduct( dir, MYON_3dVector::XAxis);
  else
    xdir = MYON_CrossProduct( dir, MYON_3dVector::ZAxis);
  xdir.Unitize();
  MYON_3dVector ydir = MYON_CrossProduct(dir,xdir);
  ydir.Unitize();
  MYON_3dVector right;

  switch(dir.MaximumCoordinateIndex())
  {
  case 0:
    right = (fabs(xdir.y) > fabs(ydir.y)) ? xdir : ydir;
    if (right.y < 0.0)
      right = -right;
    break;
  case 1:
  case 2:
    right = (fabs(xdir.x) > fabs(ydir.x)) ? xdir : ydir;
    if (right.x < 0.0)
      right = -right;
    break;
  default:
    right = xdir;
    break;
  }

  if (right[right.MaximumCoordinateIndex()] < 0.0)
    right = -right;

  return right;  
}

bool MYON_Light::GetSpotLightRadii( double* inner_radius, double* outer_radius ) const
{
  bool rc = IsSpotLight()?true:false;
  if (rc)
  {
    double angle = SpotAngleRadians();
    if ( !MYON_IsValid(angle) || angle <= 0.0 || angle >= 0.5*MYON_PI )
      angle = 0.25*MYON_PI;
    double spot = HotSpot();
    if ( !MYON_IsValid(spot) || spot < 0.0 || spot > 1.0 )
      spot = 0.5;
    double cone_height = Direction().Length();
    if ( !MYON_IsValid(cone_height) || cone_height <= 0.0 )
      cone_height = 1.0;

    if ( outer_radius )
      *outer_radius = tan( angle) * cone_height;
    if ( inner_radius )
      *inner_radius = tan( angle * spot) * cone_height;
  }
  return rc;
}



double MYON_Light::Intensity() const
{
  // 0.0 = 0%  
  // 1.0 = 100%
  // > 1.0 is used by renderers that support high dynamic range inputs
  return m_intensity;
}

void MYON_Light::SetIntensity(double v)
{
  //ALB 2014.04.28
  //Fixes http://mcneel.myjetbrains.com/youtrack/issue/RH-26585
  //Lights should not be clamped to 1.0.  There is absolutely no reason to do so.
  if ( MYON_IsValid(v) )
  {
    if (v <= 0.0)
      m_intensity = 0.0;
    else
      m_intensity = v;
  }
}

double MYON_Light::PowerWatts() const
{
  return m_watts;
}

double MYON_Light::PowerLumens() const
{
  return m_watts/683.0;
}

double MYON_Light::PowerCandela() const
{
  return m_watts/683.0;
}

void MYON_Light::SetPowerWatts( double watts )
{
  m_watts = (watts > 0.0) ? watts : 0.0;
}

void MYON_Light::SetPowerLumens( double lumens )
{
  m_watts = (lumens > 0.0) ? lumens*683.0 : 0.0;
}

void MYON_Light::SetPowerCandela( double candela )
{
  m_watts = (candela > 0.0) ? candela*683.0 : 0.0;
}




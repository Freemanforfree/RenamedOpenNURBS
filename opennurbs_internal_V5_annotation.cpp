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

// obsolete V2 and V5 annotation objects
#include "opennurbs_internal_V2_annotation.h"
#include "opennurbs_internal_V5_annotation.h"

MYON_VIRTUAL_OBJECT_IMPLEMENT( MYON_OBSOLETE_V2_Annotation,       MYON_Geometry,   "ABAF5873-4145-11d4-800F-0010830122F0" );
MYON_OBJECT_IMPLEMENT( MYON_OBSOLETE_V2_DimLinear,  MYON_OBSOLETE_V2_Annotation, "5DE6B20D-486B-11d4-8014-0010830122F0" );
MYON_OBJECT_IMPLEMENT( MYON_OBSOLETE_V2_DimRadial,  MYON_OBSOLETE_V2_Annotation, "5DE6B20E-486B-11d4-8014-0010830122F0" );
MYON_OBJECT_IMPLEMENT( MYON_OBSOLETE_V2_DimAngular, MYON_OBSOLETE_V2_Annotation, "5DE6B20F-486B-11d4-8014-0010830122F0" );
MYON_OBJECT_IMPLEMENT( MYON_OBSOLETE_V2_TextObject,       MYON_OBSOLETE_V2_Annotation, "5DE6B210-486B-11d4-8014-0010830122F0" );
MYON_OBJECT_IMPLEMENT( MYON_OBSOLETE_V2_Leader,           MYON_OBSOLETE_V2_Annotation, "5DE6B211-486B-11d4-8014-0010830122F0" );

#define REALLY_BIG_NUMBER 1.0e150

static const MYON_3dmAnnotationSettings* sglb_asets = 0;

void MYON_OBSOLETE_V2_Annotation::SetAnnotationSettings( const MYON_3dmAnnotationSettings* p )
{
  sglb_asets = p;
}

const MYON_3dmAnnotationSettings& MYON_OBSOLETE_V2_Annotation::AnnotationSettings()
{
  static MYON_3dmAnnotationSettings defaults;
  return sglb_asets ? *sglb_asets : defaults;
}

void MYON_OBSOLETE_V2_Annotation::Internal_Initialize()
{
  // TODO: initialize class members assuming any member that is not a class
  // is not initialized.
  m_type = MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing;
  m_plane = MYON_xy_plane;
  m_points.EmergencyDestroy();
  m_usertext.EmergencyDestroy();
  m_defaulttext.EmergencyDestroy();
  m_userpositionedtext = false;
}

void MYON_OBSOLETE_V2_Annotation::Destroy()
{
  m_points.Destroy();
  m_usertext.Destroy();
  m_defaulttext.Destroy();
  m_type = MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing;
  m_plane = MYON_xy_plane;
  m_userpositionedtext = false;
}

void MYON_OBSOLETE_V2_Annotation::EmergencyDestroy()
{
  m_points.EmergencyDestroy();
  m_usertext.EmergencyDestroy();
  m_defaulttext.EmergencyDestroy();
}


MYON_OBSOLETE_V2_Annotation* MYON_OBSOLETE_V2_Annotation::CreateFromV6Annotation(
  const class MYON_Annotation& V6_annotation,
  const class MYON_3dmAnnotationContext* annotation_context
)
{
  MYON_OBSOLETE_V5_Annotation* V5_annotation = MYON_OBSOLETE_V5_Annotation::CreateFromV6Annotation(V6_annotation, annotation_context);
  if (nullptr == V5_annotation)
    return nullptr;
  MYON_OBSOLETE_V2_Annotation* V2_annotation = MYON_OBSOLETE_V2_Annotation::CreateFromV5Annotation(*V5_annotation, annotation_context);
  delete V5_annotation;
  return V2_annotation;
}

MYON_OBSOLETE_V2_Annotation* MYON_OBSOLETE_V2_Annotation::CreateFromV5Annotation(
  const class MYON_OBSOLETE_V5_Annotation& V5_annotation,
  const class MYON_3dmAnnotationContext* annotation_context
)
{
  {
    const MYON_OBSOLETE_V5_DimLinear* V5_linear_dimension = MYON_OBSOLETE_V5_DimLinear::Cast(&V5_annotation);
    if (nullptr != V5_linear_dimension)
      return MYON_OBSOLETE_V2_DimLinear::CreateFromV5LinearDimension(*V5_linear_dimension, annotation_context, nullptr);
  }

  {
    const MYON_OBSOLETE_V5_DimAngular* V5_angular_dimension = MYON_OBSOLETE_V5_DimAngular::Cast(&V5_annotation);
    if (nullptr != V5_angular_dimension)
      return MYON_OBSOLETE_V2_DimAngular::CreateFromV5AngularDimension(*V5_angular_dimension, annotation_context, nullptr);
  }

  {
    const MYON_OBSOLETE_V5_DimRadial* V5_radial_dimension = MYON_OBSOLETE_V5_DimRadial::Cast(&V5_annotation);
    if (nullptr != V5_radial_dimension)
      return MYON_OBSOLETE_V2_DimRadial::CreateFromV5RadialDimension(*V5_radial_dimension, annotation_context, nullptr);
  }

  {
    const MYON_OBSOLETE_V5_Leader* V5_leader = MYON_OBSOLETE_V5_Leader::Cast(&V5_annotation);
    if (nullptr != V5_leader)
      return MYON_OBSOLETE_V2_Leader::CreateFromV5Leader(*V5_leader, annotation_context, nullptr);
  }

  {
    const MYON_OBSOLETE_V5_TextObject* V5_text_object = MYON_OBSOLETE_V5_TextObject::Cast(&V5_annotation);
    if (nullptr != V5_text_object)
      return MYON_OBSOLETE_V2_TextObject::CreateFromV5TextObject(*V5_text_object, annotation_context, nullptr);
  }

  return nullptr;
}

bool MYON_OBSOLETE_V2_Annotation::IsText() const { return Type() == MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtTextBlock; }
bool MYON_OBSOLETE_V2_Annotation::IsLeader() const { return Type() == MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtLeader; }
bool MYON_OBSOLETE_V2_Annotation::IsDimension() const { if( IsText() || IsLeader()) return false; return true; }

//virtual 
double MYON_OBSOLETE_V2_Annotation::NumericValue() const { return 0.0; }
//virtual 
void MYON_OBSOLETE_V2_Annotation::SetTextToDefault() { SetDefaultText( L""); }

void MYON_OBSOLETE_V2_Annotation::SetType( MYON_INTERNAL_OBSOLETE::V5_eAnnotationType type ) { m_type = type; }
MYON_INTERNAL_OBSOLETE::V5_eAnnotationType MYON_OBSOLETE_V2_Annotation::Type() const { return m_type; }
void MYON_OBSOLETE_V2_Annotation::SetTextDisplayMode( MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode mode) { m_textdisplaymode = mode; }
MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode MYON_OBSOLETE_V2_Annotation::TextDisplayMode() const { return m_textdisplaymode; }

void MYON_OBSOLETE_V2_Annotation::SetPlane( const MYON_Plane& plane ) { m_plane = plane; }
MYON_Plane MYON_OBSOLETE_V2_Annotation::Plane() const { return m_plane; }
int MYON_OBSOLETE_V2_Annotation::PointCount() const { return m_points.Count(); }
void MYON_OBSOLETE_V2_Annotation::SetPoints( const MYON_SimpleArray<MYON_2dPoint>& points ) { m_points = points; }
const MYON_SimpleArray<MYON_2dPoint>& MYON_OBSOLETE_V2_Annotation::Points() const { return m_points; }
void MYON_OBSOLETE_V2_Annotation::SetUserText( const wchar_t* string ) {m_usertext = string; }
const MYON_wString& MYON_OBSOLETE_V2_Annotation::UserText() const { return m_usertext; }
void MYON_OBSOLETE_V2_Annotation::SetDefaultText( const wchar_t* string ) { m_defaulttext = string; }
const MYON_wString& MYON_OBSOLETE_V2_Annotation::DefaultText() const { return m_defaulttext; }
void MYON_OBSOLETE_V2_Annotation::SetUserPositionedText( int bUserPositionedText ) { m_userpositionedtext = (bUserPositionedText?true:false); }
bool MYON_OBSOLETE_V2_Annotation::UserPositionedText() const { return m_userpositionedtext; }

bool MYON_OBSOLETE_V2_Annotation::IsValid( MYON_TextLog* text_log ) const
{
  // TODO: quickly inspect object and return true/false
  bool rc = true;
  if ( MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing == m_type  )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_OBSOLETE_V2_Annotation has m_type = MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing.\n");
    rc = false;
  }
  return rc;
}

void MYON_OBSOLETE_V2_Annotation::Dump( MYON_TextLog& dump ) const
{
  // for debugging
  dump.Print("MYON_OBSOLETE_V2_Annotation: ....\n");
}

bool MYON_OBSOLETE_V2_Annotation::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.Write3dmChunkVersion( 1, 0 );
    // TODO: use 
    //    if (rc) rc = file.WritePoint(....);
    //    if (rc) rc = file.WriteString(....);
    //    if (rc) rc = file.WriteDouble(....);
    // to write object.
  unsigned int ui = static_cast<unsigned char>(m_type);
  if (rc) 
    rc = file.WriteInt( ui );
  if (rc) 
    rc = file.WritePlane( m_plane );
  if (rc) 
    rc = file.WriteArray( m_points );
  if (rc) 
    rc = file.WriteString( m_usertext );
  if (rc) 
    rc = file.WriteString( m_defaulttext );
  int i = m_userpositionedtext ? 1 : 0;
  if( rc )
    rc = file.WriteInt( i );
  return rc;
}

bool MYON_OBSOLETE_V2_Annotation::Read( MYON_BinaryArchive& file )
{
  Destroy();
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( rc && major_version == 1 ) 
  {
    if (rc)
    {
      unsigned int ui = 0;
      rc = file.ReadInt( &ui );
      if (rc)
        m_type = MYON_INTERNAL_OBSOLETE::V5AnnotationTypeFromUnsigned(ui);
    }
    if (rc)
      rc = file.ReadPlane( m_plane );
    if (rc)
      rc = file.ReadArray( m_points );
    if (rc)
      rc = file.ReadString( m_usertext );
    if (rc)
      rc = file.ReadString( m_defaulttext );
    if( rc )
    {
      int i = 0;
      rc = file.ReadInt( &i );
      if (rc)
        m_userpositionedtext = (i ? true : false);
    }
  }

  if( fabs( m_plane.origin.x) > REALLY_BIG_NUMBER || fabs( m_plane.origin.y) > REALLY_BIG_NUMBER || fabs( m_plane.origin.z) > REALLY_BIG_NUMBER)
    return false;

  for( int i = 0; i < m_points.Count(); i++)
  {
    if( fabs( m_points[i].x) > REALLY_BIG_NUMBER || fabs( m_points[i].y) > REALLY_BIG_NUMBER)
      return false;
  }


  return rc;
}

MYON::object_type MYON_OBSOLETE_V2_Annotation::ObjectType() const
{
  return MYON::annotation_object;
}



int MYON_OBSOLETE_V2_Annotation::Dimension() const
{
  return 3; 
}

bool MYON_OBSOLETE_V2_Annotation::GetBBox( // returns true if successful
       double* boxmin,
       double* boxmax,
       bool bGrowBox // default = false
       ) const
{
  // TODO:
  //   If the class is not valid, return false.
  //
  //   If the class is valid and bGrowBox is false, 
  //   return the 3d bounding box of the annotation.
  //
  //   If the class is valid and bGrowBox is true, 
  //   return the union of the input box and the 3d bounding 
  //   box of the annotation.
  if( !bGrowBox )
  {
    boxmin[0] = boxmin[1] = boxmin[2] =  1e300;
    boxmax[0] = boxmax[1] = boxmax[2] = -1e300;
  }

  MYON_3dPoint wpt;
  MYON_Xform xform;
  GetECStoWCSXform( xform );
  for( int i = 0; i < m_points.Count(); i++ )
  {
    wpt = m_points[i];
    
    if( wpt.y < boxmin[1] )
      boxmin[1] = wpt.y;
    if( wpt.z < boxmin[2] )
      boxmin[2] = wpt.z;
    if( wpt.x > boxmax[0] )
      boxmax[0] = wpt.x;
    if( wpt.y > boxmax[1] )
      boxmax[1] = wpt.y;
    if( wpt.z > boxmax[2] )
      boxmax[2] = wpt.z;
  }
  return true;
}

bool MYON_OBSOLETE_V2_Annotation::Transform( const MYON_Xform& xform )
{
  // TODO: Return false if class is invalid or xform cannot be applied.
  //       Otherwise, apply xform to geometry and return true.
  TransformUserData(xform);
  return m_plane.Transform( xform );
}

// Converts 2d points in annotation to 3d WCS points 
bool MYON_OBSOLETE_V2_Annotation::GetECStoWCSXform( MYON_Xform& xform ) const
{
  MYON_3dVector z = MYON_CrossProduct( m_plane.xaxis, m_plane.yaxis );
  return xform.ChangeBasis( m_plane.origin, m_plane.xaxis, m_plane.yaxis, z, 
                            MYON_3dPoint::Origin, MYON_3dVector::XAxis, MYON_3dVector::YAxis, MYON_3dVector::ZAxis );
}

// Converts from WCS 3d points to 2d points in annotation
bool MYON_OBSOLETE_V2_Annotation::GeWCStoECSXform( MYON_Xform& xform ) const
{
  MYON_3dVector z = MYON_CrossProduct( m_plane.xaxis, m_plane.yaxis );
  return xform.ChangeBasis( MYON_3dPoint::Origin, MYON_3dVector::XAxis, MYON_3dVector::YAxis, MYON_3dVector::ZAxis,
                            m_plane.origin, m_plane.xaxis, m_plane.yaxis, z );
}

void MYON_OBSOLETE_V2_Annotation::SetPoint( int idx, MYON_3dPoint point )
{
  if( idx >= 0 && idx < m_points.Count() )
    m_points[idx] = point;
}
  
MYON_2dPoint MYON_OBSOLETE_V2_Annotation::Point( int idx ) const
{
  if( idx >= 0 && idx < m_points.Count() )
    return m_points[idx];

  return MYON_2dPoint( 0.0, 0.0 );
}



//----- MYON_OBSOLETE_V2_DimLinear ------------------------------------------
MYON_OBSOLETE_V2_DimLinear::MYON_OBSOLETE_V2_DimLinear()
{
}

MYON_OBSOLETE_V2_DimLinear::MYON_OBSOLETE_V2_DimLinear(const MYON_OBSOLETE_V2_DimLinear& src) : MYON_OBSOLETE_V2_Annotation(src)
{
}

MYON_OBSOLETE_V2_DimLinear::~MYON_OBSOLETE_V2_DimLinear()
{
}

MYON_OBSOLETE_V2_DimLinear& MYON_OBSOLETE_V2_DimLinear::operator=(const MYON_OBSOLETE_V2_DimLinear& src)
{
  if ( this != &src ) {
    MYON_OBSOLETE_V2_Annotation::operator=(src);
  }
  return *this;
}

void MYON_OBSOLETE_V2_DimLinear::EmergencyDestroy()
{
  MYON_OBSOLETE_V2_Annotation::EmergencyDestroy();
}

double MYON_OBSOLETE_V2_DimLinear::NumericValue() const
{
  return (Point( 1) - Point( 3)).Length();
}
void MYON_OBSOLETE_V2_DimLinear::SetTextToDefault() 
{ 
  SetUserText( L"<>"); 
}


//----- MYON_OBSOLETE_V2_DimRadial ------------------------------------------
MYON_OBSOLETE_V2_DimRadial::MYON_OBSOLETE_V2_DimRadial()
{
}

MYON_OBSOLETE_V2_DimRadial::MYON_OBSOLETE_V2_DimRadial(const MYON_OBSOLETE_V2_DimRadial& src) : MYON_OBSOLETE_V2_Annotation(src)
{
}

MYON_OBSOLETE_V2_DimRadial::~MYON_OBSOLETE_V2_DimRadial()
{
}

MYON_OBSOLETE_V2_DimRadial& MYON_OBSOLETE_V2_DimRadial::operator=(const MYON_OBSOLETE_V2_DimRadial& src)
{
  if ( this != &src ) {
    MYON_OBSOLETE_V2_Annotation::operator=(src);
  }
  return *this;
}

void MYON_OBSOLETE_V2_DimRadial::EmergencyDestroy()
{
  MYON_OBSOLETE_V2_Annotation::EmergencyDestroy();
}

double MYON_OBSOLETE_V2_DimRadial::NumericValue() const
{
  double d = (Point( 0) - Point( 1)).Length();
  if( Type() == MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimDiameter)
    d *= 2.0;
  return d;
}

void MYON_OBSOLETE_V2_DimRadial::SetTextToDefault() 
{ 
  MYON_wString s; 
  if( Type() == MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtDimDiameter) 
    s.Format( L"%c<>", MYON_wString::DiameterSymbol ); 
  else 
    s.Format( L"%c<>", MYON_wString::RadiusSymbol ); 
  SetUserText( static_cast< const wchar_t* >(s)); 
}

//----- MYON_OBSOLETE_V2_DimAngular -----------------------------------------
MYON_OBSOLETE_V2_DimAngular::MYON_OBSOLETE_V2_DimAngular() : m_angle(0.0), m_radius(0.0)
{
}

MYON_OBSOLETE_V2_DimAngular::MYON_OBSOLETE_V2_DimAngular(const MYON_OBSOLETE_V2_DimAngular& src) : MYON_OBSOLETE_V2_Annotation(src)
{
  m_angle = src.m_angle;
  m_radius = src.m_radius;
}

MYON_OBSOLETE_V2_DimAngular::~MYON_OBSOLETE_V2_DimAngular()
{
}

MYON_OBSOLETE_V2_DimAngular& MYON_OBSOLETE_V2_DimAngular::operator=(const MYON_OBSOLETE_V2_DimAngular& src)
{
  if ( this != &src ) {
    MYON_OBSOLETE_V2_Annotation::operator=(src);
    m_angle = src.m_angle;
    m_radius = src.m_radius;
  }
  return *this;
}

void MYON_OBSOLETE_V2_DimAngular::EmergencyDestroy()
{
  MYON_OBSOLETE_V2_Annotation::EmergencyDestroy();
}

bool MYON_OBSOLETE_V2_DimAngular::Write( MYON_BinaryArchive& file ) const
{
  bool rc = MYON_OBSOLETE_V2_Annotation::Write( file );
  if( rc )
    rc = file.WriteDouble( m_angle );
  if( rc )
    rc = file.WriteDouble( m_radius );
  return rc;
}

bool MYON_OBSOLETE_V2_DimAngular::Read( MYON_BinaryArchive& file )
{
  bool rc = MYON_OBSOLETE_V2_Annotation::Read( file );
  if( rc )
    rc = file.ReadDouble( &m_angle );
  if( rc )
    rc = file.ReadDouble( &m_radius );

  if( m_angle <= 0.0 || m_angle > REALLY_BIG_NUMBER)
    return false;

  if( m_radius <= 0.0 || m_radius > REALLY_BIG_NUMBER)
    return false;

  return rc;
}

double MYON_OBSOLETE_V2_DimAngular::NumericValue() const
{
  return Angle() * 180.0 / MYON_PI;
}

void MYON_OBSOLETE_V2_DimAngular::SetTextToDefault() 
{ 
  MYON_wString s; 
  s.Format( L"<>%c", MYON_wString::DegreeSymbol ); 
  SetUserText( static_cast< const wchar_t* >(s)); 
}

//----- MYON_OBSOLETE_V2_TextObject -----------------------------------------------
MYON_OBSOLETE_V2_TextObject::MYON_OBSOLETE_V2_TextObject() : m_fontweight(400), m_height(20.0)
{
}

MYON_OBSOLETE_V2_TextObject::MYON_OBSOLETE_V2_TextObject(const MYON_OBSOLETE_V2_TextObject& src) : MYON_OBSOLETE_V2_Annotation(src)
{
  m_facename = src.m_facename;
  m_fontweight = src.m_fontweight;
  m_height = src.m_height;
}

MYON_OBSOLETE_V2_TextObject::~MYON_OBSOLETE_V2_TextObject()
{
  m_facename.Destroy();
}

MYON_OBSOLETE_V2_TextObject& MYON_OBSOLETE_V2_TextObject::operator=(const MYON_OBSOLETE_V2_TextObject& src)
{
  if ( this != &src ) {
    m_facename = src.m_facename;
    m_fontweight = src.m_fontweight;
    m_height = src.m_height;
    MYON_OBSOLETE_V2_Annotation::operator=(src);
  }
  return *this;
}

void MYON_OBSOLETE_V2_TextObject::EmergencyDestroy()
{
  MYON_OBSOLETE_V2_Annotation::EmergencyDestroy();
  m_facename.EmergencyDestroy();
}

bool MYON_OBSOLETE_V2_TextObject::Write( MYON_BinaryArchive& file ) const
{
  bool rc = MYON_OBSOLETE_V2_Annotation::Write( file );
  if( rc )
    rc = file.WriteString( m_facename );
  if( rc )
    rc = file.WriteInt( m_fontweight );
  if( rc )
    rc = file.WriteDouble( m_height );
  return rc;
}

bool MYON_OBSOLETE_V2_TextObject::Read( MYON_BinaryArchive& file )
{
  bool rc = MYON_OBSOLETE_V2_Annotation::Read( file );
  if( rc )
    rc = file.ReadString( m_facename );
  if( rc )
    rc = file.ReadInt( &m_fontweight );
  if( rc )
    rc = file.ReadDouble( &m_height );

  if( fabs( m_height) > REALLY_BIG_NUMBER)
    return false;


  return rc;
}

//----- MYON_OBSOLETE_V2_Leader ------------------------------------------
MYON_OBSOLETE_V2_Leader::MYON_OBSOLETE_V2_Leader()
{
}

MYON_OBSOLETE_V2_Leader::MYON_OBSOLETE_V2_Leader(const MYON_OBSOLETE_V2_Leader& src) : MYON_OBSOLETE_V2_Annotation(src)
{
}

MYON_OBSOLETE_V2_Leader::~MYON_OBSOLETE_V2_Leader()
{
}

MYON_OBSOLETE_V2_Leader& MYON_OBSOLETE_V2_Leader::operator=(const MYON_OBSOLETE_V2_Leader& src)
{
  if ( this != &src ) {
    MYON_OBSOLETE_V2_Annotation::operator=(src);
  }
  return *this;
}

void MYON_OBSOLETE_V2_Leader::EmergencyDestroy()
{
  MYON_OBSOLETE_V2_Annotation::EmergencyDestroy();
}


MYON_OBJECT_IMPLEMENT(MYON_OBSOLETE_V2_TextDot,MYON_Point,"8BD94E19-59E1-11d4-8018-0010830122F0");

MYON_OBSOLETE_V2_TextDot::MYON_OBSOLETE_V2_TextDot()
{}

MYON_OBSOLETE_V2_TextDot::~MYON_OBSOLETE_V2_TextDot()
{
  m_text.Destroy();
}

MYON_OBSOLETE_V2_TextDot::MYON_OBSOLETE_V2_TextDot(const MYON_OBSOLETE_V2_TextDot& src) : MYON_Point(src), m_text(src.m_text)
{}

MYON_OBSOLETE_V2_TextDot& MYON_OBSOLETE_V2_TextDot::operator=(const MYON_OBSOLETE_V2_TextDot& src)
{
  if ( this != &src ) {
    MYON_Point::operator=(src);
    m_text = src.m_text;
  }
  return *this;
}

bool MYON_OBSOLETE_V2_TextDot::IsValid( MYON_TextLog* text_log ) const
{
  bool rc = true;
  if ( m_text.IsEmpty() )
  {
    if ( 0 != text_log )
      text_log->Print("MYON_OBSOLETE_V2_TextDot.m_text is empty\n");
    rc = false;
  }
  return rc;
}

void MYON_OBSOLETE_V2_TextDot::Dump( MYON_TextLog& log ) const
{
  log.Print("MYON_OBSOLETE_V2_TextDot \"%ls\" at ",m_text.Array());
  log.Print(point);
  log.Print("\n");
}

bool MYON_OBSOLETE_V2_TextDot::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.Write3dmChunkVersion(1,0);
  if (rc) rc = file.WritePoint( point );
  if (rc) rc = file.WriteString( m_text );
  return rc;
}

bool MYON_OBSOLETE_V2_TextDot::Read( MYON_BinaryArchive& file )
{
  m_text.Destroy();
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( major_version == 1 ) {
    if (rc) rc = file.ReadPoint( point );
    if (rc) rc = file.ReadString( m_text );
  }
  else {
    rc = false;
  }
  return rc;
}

MYON_OBJECT_IMPLEMENT(MYON_OBSOLETE_V2_AnnotationArrow,MYON_Geometry,"8BD94E1A-59E1-11d4-8018-0010830122F0");

MYON_OBSOLETE_V2_AnnotationArrow::MYON_OBSOLETE_V2_AnnotationArrow() : m_tail(0.0,0.0,0.0), m_head(0.0,0.0,0.0)
{}

MYON_OBSOLETE_V2_AnnotationArrow::~MYON_OBSOLETE_V2_AnnotationArrow()
{}

MYON_OBSOLETE_V2_AnnotationArrow::MYON_OBSOLETE_V2_AnnotationArrow(const MYON_OBSOLETE_V2_AnnotationArrow& src) : MYON_Geometry(src), m_tail(src.m_tail), m_head(src.m_head)
{}

MYON_OBSOLETE_V2_AnnotationArrow& MYON_OBSOLETE_V2_AnnotationArrow::operator=(const MYON_OBSOLETE_V2_AnnotationArrow& src)
{
  if ( this != &src ) {
    MYON_Geometry::operator=(src);
    m_tail = src.m_tail;
    m_head = src.m_head;
  }
  return *this;
}

bool MYON_OBSOLETE_V2_AnnotationArrow::IsValid( MYON_TextLog* text_log ) const
{
  bool rc = true;
  if (m_tail == m_head)
  {
    if ( 0 != text_log )
      text_log->Print("MYON_OBSOLETE_V2_AnnotationArrow has m_head=m_tail.\n");
    rc = false;
  }
  return rc;
}

void MYON_OBSOLETE_V2_AnnotationArrow::Dump( MYON_TextLog& log ) const
{
  log.Print("MYON_OBSOLETE_V2_AnnotationArrow: ");
  log.Print(m_tail);
  log.Print(" to ");
  log.Print(m_head);
  log.Print("\n");
}

bool MYON_OBSOLETE_V2_AnnotationArrow::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.Write3dmChunkVersion(1,0);
  if (rc) rc = file.WritePoint( m_tail );
  if (rc) rc = file.WritePoint( m_head );
  return rc;
}

bool MYON_OBSOLETE_V2_AnnotationArrow::Read(MYON_BinaryArchive& file)
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( major_version == 1 ) {
    if (rc) rc = file.ReadPoint( m_tail );
    if (rc) rc = file.ReadPoint( m_head );

  }
  else {
    rc = false;
  }
  return rc;
}

MYON::object_type MYON_OBSOLETE_V2_AnnotationArrow::ObjectType() const
{
  return MYON::annotation_object;
}

int MYON_OBSOLETE_V2_AnnotationArrow::Dimension() const
{
  return 3;
}

bool MYON_OBSOLETE_V2_AnnotationArrow::GetBBox( double* boxmin, double* boxmax, bool bGrowBox ) const
{
  bool rc = MYON_GetPointListBoundingBox( 3, false, 1, 3, m_tail, boxmin, boxmax, bGrowBox?true:false );
  if (rc)
    rc = MYON_GetPointListBoundingBox( 3, false, 1, 3, m_head, boxmin, boxmax, true );
  return rc;
}

bool MYON_OBSOLETE_V2_AnnotationArrow::Transform( const MYON_Xform& xform )
{
  TransformUserData(xform);
  m_tail = xform*m_tail;
  m_head = xform*m_head;
  return true;
}

MYON_3dVector MYON_OBSOLETE_V2_AnnotationArrow::Vector() const
{
  return (m_head-m_tail);
}

MYON_3dPoint MYON_OBSOLETE_V2_AnnotationArrow::Head() const
{
  return m_head;
}

MYON_3dPoint MYON_OBSOLETE_V2_AnnotationArrow::Tail() const
{
  return m_tail;
}


static bool MYON_Internal_UseSubDMeshProxy(
  const MYON_BinaryArchive& archive
)
{
  if (archive.Archive3dmVersion() != 60)
  {
    return false; // subd mesh proxy only applies when reading or writing a V6 files.
  }

  // In a WIP, used subd objects.
  const unsigned int min_subd_version =
#if defined(RHINO_COMMERCIAL_BUILD)
    // Sep 5 2017 Dale Lear RH-41113
    // Rhino 6 commercial will have SubD objects.
    // Nope. -> V6 commercial builds do not have subd objects - use proxy mesh when version < 7.
    // 7 
    6 
#else
    // V6 WIP builds and all V7 and later builds have subd objects.
    // Use proxy mesh when version < 6.
    6
#endif
    ;
  return (MYON::VersionMajor() < min_subd_version);
}


/*
Description:
  In rare cases one object must be converted into another.
  Examples include reading obsolete objects and converting them into their 
  current counterpart, converting WIP objects into a proxy for a commercial build, 
  and converting a proxy object into a WIP object for a WIP build.
*/
MYON_Object* MYON_BinaryArchive::Internal_ConvertObject(
    const MYON_Object* archive_object,
    const MYON_3dmObjectAttributes* attributes
  ) const
{
  if (nullptr == archive_object)
    return nullptr;

  if (MYON::object_type::annotation_object == archive_object->ObjectType())
  {
    m_annotation_context.SetViewContext((nullptr != attributes) ? attributes->m_space : MYON::active_space::no_space);
    
    const MYON_OBSOLETE_V2_AnnotationArrow* annotation_arrow = MYON_OBSOLETE_V2_AnnotationArrow::Cast(archive_object);
    if (nullptr != annotation_arrow)
    {
      MYON_LineCurve* line_curve = new MYON_LineCurve(annotation_arrow->m_tail, annotation_arrow->m_head);
      //if (nullptr != pAttributes)
      //  pAttributes->m_object_decoration = MYON::object_decoration::end_arrowhead;
      return line_curve;
    }

    const MYON_OBSOLETE_V2_Annotation* V2_annotation = MYON_OBSOLETE_V2_Annotation::Cast(archive_object);
    if (nullptr != V2_annotation)
    {
      // convert V2 annotation object to V5 annotation object
      const MYON_DimStyle& dim_style = ArchiveCurrentDimStyle();
      m_annotation_context.SetReferencedDimStyle(&dim_style,nullptr,ArchiveCurrentDimStyleIndex());
      MYON_Annotation* V6_annotation = MYON_Annotation::CreateFromV2Annotation(*V2_annotation, &m_annotation_context);
      if (nullptr != V6_annotation)
        V6_annotation->SetDimensionStyleId(dim_style.Id());
      return V6_annotation;
    }

    const MYON_OBSOLETE_V5_Annotation* V5_annotation = MYON_OBSOLETE_V5_Annotation::Cast(archive_object);
    if (nullptr != V5_annotation)
    {
      // convert V5 annotation object to V6 annotation object
      int V5_dim_style_index = V5_annotation->V5_3dmArchiveDimStyleIndex();
      const MYON_DimStyle* dim_style = nullptr;
      const MYON_DimStyle* override_dim_style = nullptr;
      if (V5_dim_style_index >= 0 && V5_dim_style_index < m_archive_dim_style_table.Count())
        dim_style = m_archive_dim_style_table[V5_dim_style_index];
      if (nullptr != dim_style && dim_style->ParentIdIsNotNil())
      {
        if (dim_style->Id() != dim_style->ParentId())
        {
          for (int i = 0; i < this->m_archive_dim_style_table.Count(); i++)
          {
            const MYON_DimStyle* archive_dim_style = m_archive_dim_style_table[i];
            if (nullptr == archive_dim_style)
              continue;
            if (archive_dim_style->ParentIdIsNotNil())
              continue;
            if (archive_dim_style->Id() == dim_style->ParentId())
            {
              override_dim_style = dim_style;
              dim_style = archive_dim_style;
              break;
            }
          }
        }
        if (nullptr == override_dim_style)
          dim_style = nullptr;
      }

      if (nullptr == dim_style)
      {
        dim_style = &ArchiveCurrentDimStyle();
        V5_dim_style_index = ArchiveCurrentDimStyleIndex();
      }

      m_annotation_context.SetReferencedDimStyle( dim_style, override_dim_style, V5_dim_style_index );
      MYON_Annotation* V6_annotation = MYON_Annotation::CreateFromV5Annotation( *V5_annotation, &m_annotation_context );
      if (nullptr != V6_annotation)
      {
        const MYON_UUID archive_id = dim_style->Id();
        MYON_UUID model_id = archive_id;
        const MYON_ManifestMapItem mapitem = this->ManifestMap().MapItemFromSourceId(archive_id);
        if (MYON_ModelComponent::Type::DimStyle == mapitem.ComponentType())
          model_id = mapitem.DestinationId();
        V6_annotation->SetDimensionStyleIdForExperts(model_id, true);
      }

      return V6_annotation;
    }

    return nullptr;
  }

  if (MYON::object_type::point_object == archive_object->ObjectType())
  {
    const MYON_OBSOLETE_V2_TextDot* v1_text_dot = MYON_OBSOLETE_V2_TextDot::Cast(archive_object);
    if (nullptr != v1_text_dot)
    {
      MYON_TextDot* text_dot = new MYON_TextDot();
      text_dot->SetPrimaryText(v1_text_dot->m_text);
      text_dot->SetCenterPoint(v1_text_dot->point);
      text_dot->SetFontFace(MYON_DimStyle::Default.Font().WindowsLogfontName());
      return text_dot;
    }
    return nullptr;
  }

  if (MYON::object_type::mesh_object == archive_object->ObjectType())
  {
    const MYON_Mesh* mesh = MYON_Mesh::Cast(archive_object);
    if (nullptr != mesh )
    {
      if ( false == MYON_Internal_UseSubDMeshProxy(*this) )
      {
        // If mesh is a subd mesh proxy, return the original subd.
        MYON_SubD* subd = MYON_SubDMeshProxyUserData::SubDFromMeshProxy(mesh);
        if ( nullptr != subd )
          return subd;
      }
    }
  }
  else if (MYON::object_type::subd_object == archive_object->ObjectType())
  {
    const MYON_SubD* subd = MYON_SubD::Cast(archive_object);
    if (nullptr != subd)
    {
      MYON_Mesh* mesh = nullptr;
      if ( Archive3dmVersion() < 60 )
      {
        mesh = subd->GetControlNetMesh(nullptr, MYON_SubDGetControlNetMeshPriority::Geometry);
      }
      else if ( MYON_Internal_UseSubDMeshProxy(*this) )
      {
        // Use a subd mesh proxy for V6 commercial builds.
        mesh = MYON_SubDMeshProxyUserData::MeshProxyFromSubD(subd);
      }
      if (nullptr != mesh)
        return mesh;
    }
  }
  
  // no conversion required.
  return nullptr;
}

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

#if defined(OPENNURBS_EXPORTS)


//
//
////////////////////////////////////////////////////////////////////////////////

#endif

unsigned int MYON_IsRhinoApplicationId(
  MYON_UUID id
  )
{
  if (MYON_rhino2_id == id)
    return 2;
  if (MYON_rhino3_id == id)
    return 3;
  if (MYON_rhino4_id == id)
    return 4;
  if (MYON_rhino5_id == id)
    return 5;
  if (MYON_rhino6_id == id)
    return 6;
  if (MYON_rhino7_id == id)
    return 7;
  if (MYON_rhino8_id == id)
    return 8;
  return 0;
}

unsigned int MYON_IsOpennurbsApplicationId(
  MYON_UUID id
  )
{
  if (MYON_opennurbs4_id == id)
    return 4;
  if (MYON_opennurbs5_id == id)
    return 5;
  if (MYON_opennurbs6_id == id)
    return 6;
  if (MYON_opennurbs7_id == id)
    return 7;
  if (MYON_opennurbs8_id == id)
    return 8;
  return 0;
}

static int MYON__isnand(const double* x)
{
  const unsigned char* b = (const unsigned char*)x;
  static unsigned int b7 = 0;
  static unsigned int b6 = 0;
  if (0 == b6)
  {
    // different bytes on
    union
    {
      double x;
      unsigned char b[8];
    } u;  u.x = 2.0; // sign = 0; fraction = 0; exponent = 100 0000 0000 binary

    if (0x40 == u.b[7] && 0 == u.b[0]
      && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
      && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6]
      )
    {
      // little endian doubles
      b7 = 7;
      b6 = 6;
    }
    else if (0x40 == u.b[0] && 0 == u.b[7]
      && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
      && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6]
      )
    {
      // big endian doubles
      b7 = 0;
      b6 = 1;
    }
    else
    {
      // This sitation is not handled by this algorithm
      // and that is a bug in the algorithm.
      MYON_ERROR("Unexpected bit pattern in double 2.0.");
      // assum little endian doubles
      b7 = 7;
      b6 = 6;
    }
  }

  if (0x7F == (0x7F & b[b7]) && (0xF0 == (0xF0 & b[b6])))
  {
    // All exponent bits are set
    if (0x08 & b[b6])
    {
      // The most significant fraction bit is set.
      // This must be a QNaN
      return 2;
    }
    else
    {
      // The most significant fraction bit is is clear.  
      // If any other fraction bits are set, it's an SNaN
      if (0 != (0x0F & b[b6]))
        return 1;
      if (6 == b6)
      {
        // little endian
        return
          (0 != b[0] || 0 != b[1] || 0 != b[2] || 0 != b[3] || 0 != b[4] || 0 != b[5])
          ? 1  // some fraction bit is set
          : 0; // all fraction bits are clear.
      }
      else
      {
        // big endian
        return
          (0 != b[2] || 0 != b[3] || 0 != b[4] || 0 != b[5] || 0 != b[6] || 0 != b[7])
          ? 1  // some fraction bit is set
          : 0; // all fraction bits are clear.
      }
    }
  }

  return 0; // not a NaN
}

static int MYON__isnanf(const float* x)
{
  const unsigned char* b = (const unsigned char*)x;

  static unsigned int b3 = 0;
  static unsigned int b2 = 0;

  if (0 == b2)
  {
    union
    {
      float x;
      unsigned char b[4];
    } u;

    // different bytes on
    u.x = 2.0f; // sign = 0; mantissa = 0; exponent = 1000 0000
    if (0x40 == u.b[3] && 0 == u.b[0] && 0 == u.b[1] && 0 == u.b[2])
    {
      // little endian floats
      b3 = 3; b2 = 2;
    }
    else if (0x40 == u.b[0] && 0 == u.b[3] && 0 == u.b[1] && 0 == u.b[2])
    {
      // big endian floats
      b3 = 0; b2 = 1;
    }
    else
    {
      // This sitation is not handled by this algorithm
      // and that is a bug in the algorithm.
      MYON_ERROR("Unexpected bit pattern in float 2.0f.");
      // assume little endian floats
      b3 = 3; b2 = 2;
    }
  }

  if (0x7F == (0x7F & b[b3]) && (0x80 == (0x80 & b[b2])))
  {
    // All exponent bits are set

    if (0x7F & b[b2])
    {
      // The most significant fraction bit is set.
      // This must be a QNaN
      return 2;
    }
    else
    {
      // The most significant fraction bit is is clear.  
      // If any other fraction bits are set, it's an SNaN
      if (0 != (0x0F & b[b2]))
        return 1;
      if (2 == b2)
      {
        // little endian
        return
          (0 != b[0] || 0 != b[1])
          ? 1  // some fraction bit is set
          : 0; // all fraction bits are clear.
      }
      else
      {
        // big endian
        return
          (0 != b[2] || 0 != b[3])
          ? 1  // some fraction bit is set
          : 0; // all fraction bits are clear.
      }
    }
  }

  return 0; // not a NaN
}

void MYON_DBL_SNAN( double* x)
{
  union 
  {
    double x;
    unsigned char b[8];
  } u;

#if   defined(MYON_LITTLE_ENDIAN)
#define i7 7
#define i6 6
#elif defined(MYON_BIG_ENDIAN)
#define i7 0
#define i6 1
#else
  unsigned int i7, i6;

  u.x = 2.0; // sign = 0; fraction = 0; exponent = 100 0000 0000 binary

  if ( 0x40 == u.b[7] && 0 == u.b[0]
       && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
       && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6] 
     )
  {
    // little endian doubles
    i7 = 7; i6 = 6;
  }
  else if ( 0x40 == u.b[0]  && 0 == u.b[7]
            && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
            && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6] 
          )
  {
    // big endian doubles
    i7 = 0; i6 = 1;
  }
  else
  {
    // this sitation is not handled by this algorithm
    // and that is a bug in the algorithm.
    MYON_ERROR("CPU has unexpected bit pattern in double 2.0.");
    i7 = 0;
    i6 = 0;
    memset(&x,0xFF,sizeof(*x));
    return;
  }
#endif

  // all exponent bits = 1
  // fraction bits = 010...0
  u.b[i7]   = 0x7F; // 0111 1111
  u.b[i6]   = 0xF4; // 1111 0100
  u.b[5]    = 0;    // 0...
  u.b[4]    = 0;
  u.b[3]    = 0;
  u.b[2]    = 0;
  u.b[7-i6] = 0;
  u.b[7-i7] = 0;

#if defined(i7)
#undef i7
#undef i6
#endif


  // must use memcpy().  On Intel FPU, assignment using x = u.x 
  // will set x to qnan and invalid op exception occures.
  memcpy(x,&u.x,sizeof(*x));
}

void MYON_FLT_SNAN( float* x)
{
  union 
  {
    float x;
    unsigned char b[4];
  } u;

#if   defined(MYON_LITTLE_ENDIAN)
#define i3 3
#define i2 2
#elif defined(MYON_BIG_ENDIAN)
#define i3 0
#define i2 1
#else
  unsigned int i3, i2;

  u.x = 2.0f; // sign = 0; mantissa = 0; exponent = 1000 0000

  if ( 0x40 == u.b[3] && 0 == u.b[0] && 0 == u.b[1] && 0 == u.b[2] )
  {
    // little endian doubles
    i3 = 3; i2 = 2;
  }
  else if ( 0x40 == u.b[0] && 0 == u.b[3] && 0 == u.b[1] && 0 == u.b[2] )
  {
    // big endian doubles
    i3 = 0; i2 = 1;
  }
  else
  {
    // this sitation is not handled by this algorithm
    // and that is a bug in the algorithm.
    MYON_ERROR("CPU has unexpected bit pattern in float 2.0f.");
    memset(&x,0xFF,sizeof(*x));
    return;
  }
#endif

  // all exponent bits = 1
  // fraction bits = 011...1
  u.b[i3]   = 0x7F; // 0111 1111
  u.b[i2]   = 0xA0; // 1010 0000
  u.b[3-i2] = 0;    // 0...
  u.b[3-i3] = 0;

#if defined(i3)
#undef i3
#undef i2
#endif

  // must use memcpy().  On Intel FPU, assignment using x = u.x 
  // will set x to qnan and invalid op exception occures.
  memcpy(x,&u.x,sizeof(*x));
}

bool MYON_IsNaNd(double x)
{
  return 0 != MYON__isnand(&x);
}

bool MYON_IsQNaNd(double x)
{
  return 2 == MYON__isnand(&x);
}

bool MYON_IsSNaNd(double x)
{
  return 1 == MYON__isnand(&x);
}

bool MYON_IsNaNf(float x)
{
  return 0 != MYON__isnanf(&x);
}

bool MYON_IsQNaNf(float x)
{
  return 2 == MYON__isnanf(&x);
}

bool MYON_IsSNaNf(float x)
{
  return 1 == MYON__isnanf(&x);
}

float MYON_FloatFromDouble(
  double x
)
{
  if (MYON_UNSET_VALUE == x)
    return MYON_UNSET_FLOAT;
  if (MYON_UNSET_POSITIVE_VALUE == x)
    return MYON_UNSET_POSITIVE_FLOAT;
  return static_cast<float>(x);
}

double MYON_DoubleFromFloat(
  float x
)
{
  if (MYON_UNSET_FLOAT == x)
    return MYON_UNSET_VALUE;
  if (MYON_UNSET_POSITIVE_FLOAT == x)
    return MYON_UNSET_POSITIVE_VALUE;
  return static_cast<double>(x);
}


bool MYON_IsNullPtr(const void* ptr)
{
  return (nullptr == ptr);
}

bool MYON_IsNullPtr(const MYON__UINT_PTR ptr)
{
  return (0 == ptr);
}

bool MYON_IsNullPtr(const MYON__INT_PTR ptr)
{
  return (0 == ptr);
}


static void ValidateSizesHelper()
{
  static bool bSizedValidated = false;
  if ( !bSizedValidated )
  {
    // Validate int and pointer sizes
    bSizedValidated = true;

    // These conditional expressions are all constant and should
    // all be false.  If any are true, then portions of OpenNURBS
    // code will fail and probably crash.

#if defined(MYON_COMPILER_MSC)
#pragma MYON_PRAGMA_WARNING_PUSH
// Disable warning C4127: conditional expression is constant
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4127 )
#endif

    if ( MYON_SIZEOF_POINTER != sizeof(void*) )
    {
      MYON_ERROR("MYON_SIZEOF_POINTER is not correctly defined.");
    }
    if ( MYON_SIZEOF_POINTER != sizeof(MYON__INT_PTR) )
    {
      MYON_ERROR("MYON_INT_PTR is not correctly defined.");
    }
    if ( 1 !=  sizeof(char) )
    {
      MYON_ERROR("OpenNURBS assumes sizeof(char) = 1.");
    }
    if ( 2 !=  sizeof(MYON__INT16) )
    {
      MYON_ERROR("MYON__INT16 is not correctly defined.");
    }
    if ( 4 !=  sizeof(MYON__INT32) )
    {
      MYON_ERROR("MYON__INT32 is not correctly defined.");
    }
    if ( 8 !=  sizeof(MYON__INT64) )
    {
      MYON_ERROR("MYON__INT32 is not correctly defined.");
    }
    if ( sizeof(int) > sizeof(void*) )
    {
      MYON_ERROR("OpenNURBS assumes sizeof(int) <= sizeof(void*).");
    }
    if ( 4 !=  sizeof(float) )
    {
      MYON_ERROR("OpenNURBS assumes sizeof(float) = 4.");
    }
    if ( 8 !=  sizeof(double) )
    {
      MYON_ERROR("OpenNURBS assumes sizeof(double) = 8.");
    }

#if defined(MYON_COMPILER_MSC)
#pragma MYON_PRAGMA_WARNING_POP
#endif
  }
}


unsigned int MYON::LibraryStatus()
{
  return MYON::m_opennurbs_library_status;
}

void MYON::SetLibraryStatus(unsigned int status)
{
  MYON::m_opennurbs_library_status = status;
}


void MYON::Begin()
{
  if ( 0 != MYON::m_opennurbs_library_status )
    return;

  MYON::m_opennurbs_library_status = 1;

  ValidateSizesHelper();


#if !defined(OPENNURBS_EXPORTS)
  // Some statically linked library optimizations discard
  // object code that is not explicitly referenced.
  // By explicitly calling all the MYON_Object::Cast overrides,
  // we can insure that the class definitions get linked in
  // by making a single call to MYON::Begin().  These definitions
  // are needed for the file reading code to work right.
  const MYON_Object* p=0;

  MYON_Object::Cast(p);
  MYON_3dmObjectAttributes::Cast(p);
  MYON_Bitmap::Cast(p);
  MYON_EmbeddedBitmap::Cast(p);
  MYON_WindowsBitmap::Cast(p);
  MYON_WindowsBitmapEx::Cast(p);
  MYON_DimStyle::Cast(p);
  MYON_DocumentUserStringList::Cast(p);
  MYON_TextStyle::Cast(p);
  MYON_Geometry::Cast(p);
  MYON_Annotation::Cast(p);
  MYON_Text::Cast(p);
  MYON_Leader::Cast(p);
  MYON_Dimension::Cast(p);
  MYON_DimLinear::Cast(p);
  MYON_DimAngular::Cast(p);
  MYON_DimRadial::Cast(p);
  MYON_DimOrdinate::Cast(p);
  MYON_Centermark::Cast(p);
  MYON_Brep::Cast(p);
  MYON_BrepLoop::Cast(p);
  MYON_Curve::Cast(p);
  MYON_ArcCurve::Cast(p);
  MYON_CurveOnSurface::Cast(p);
  MYON_CurveProxy::Cast(p);
  MYON_BrepEdge::Cast(p);
  MYON_BrepTrim::Cast(p);
  MYON_LineCurve::Cast(p);
  MYON_NurbsCurve::Cast(p);
  MYON_PolyCurve::Cast(p);
  MYON_PolylineCurve::Cast(p);
  MYON_DetailView::Cast(p);
  MYON_Hatch::Cast(p);
  MYON_InstanceDefinition::Cast(p);
  MYON_InstanceRef::Cast(p);
  MYON_Light::Cast(p);
  MYON_Mesh::Cast(p);
  MYON_MeshComponentRef::Cast(p);
  MYON_MorphControl::Cast(p);
  MYON_NurbsCage::Cast(p);
  MYON_Point::Cast(p);
  MYON_BrepVertex::Cast(p);
  MYON_PointCloud::Cast(p);
  MYON_PointGrid::Cast(p);
  MYON_Surface::Cast(p);
  MYON_Extrusion::Cast(p);
  MYON_NurbsSurface::Cast(p);
  MYON_PlaneSurface::Cast(p);
  MYON_ClippingPlaneSurface::Cast(p);
  MYON_RevSurface::Cast(p);
  MYON_SumSurface::Cast(p);
  MYON_SurfaceProxy::Cast(p);
  MYON_BrepFace::Cast(p);
  MYON_OffsetSurface::Cast(p);
  MYON_TextDot::Cast(p);
  MYON_Viewport::Cast(p);
  MYON_Group::Cast(p);
  MYON_HatchPattern::Cast(p);
  MYON_HistoryRecord::Cast(p);
  MYON_Layer::Cast(p);
  MYON_Linetype::Cast(p);
  MYON_Material::Cast(p);
  MYON_Texture::Cast(p);
  MYON_TextureMapping::Cast(p);
  MYON_UserData::Cast(p);
  MYON_UnknownUserData::Cast(p);
  MYON_UserStringList::Cast(p);

#endif

  // Lock and mark all constant system components
  MYON_ModelComponent::Internal_SystemComponentHelper();

  MYON::m_opennurbs_library_status = 2;
}

void MYON::End()
{
}

int MYON_ClassId::CurrentMark()
{
  return m_mark0;
}

int MYON_ClassId::IncrementMark()
{
  m_mark0++;
  return m_mark0;
}

int MYON_ClassId::Purge( int mark_value )
{
  // Fundamental openNURBS class ids have a mark value of 0 and cannot be purged.
  int purge_count = 0;
  if ( mark_value > 0 ) {
    MYON_ClassId* prev = 0;
    MYON_ClassId* next = 0;
    MYON_ClassId* p;
    for ( p = m_p0; p; p = next )
    {
      next = p->m_pNext;
      if ( (0x7FFFFFFF & p->m_mark) == mark_value ) {
        purge_count++;
        if ( prev )
          prev->m_pNext = next;
        else
          m_p0 = next;
        p->m_pNext = 0;
      }
      else
        prev = p;
    }
  }
  return purge_count;
}

const MYON_ClassId* MYON_ClassId::LastClassId()
{
  return m_p1;
}

bool MYON_ClassId::PurgeAfter(const MYON_ClassId* pClassId)
{
  // If you crash in on the p=p->m_pNext iterator in
  // the for() loop, it is because somebody incorrectly
  // unloaded a dll that contains an MYON_OBJECT_IMPLEMENT 
  // macro.
  for (MYON_ClassId* p = m_p0; p; p = p->m_pNext)
  {
    if (pClassId == p)
    {
      // All class ids after pClassId are assumed to
      // be bad.
      p->m_pNext = 0;
      m_p1 = p;
      return true;
    }
  }

  MYON_ERROR("MYON_ClassId::PurgeAfter pClassId is not active");
  return false;
}

//////////////////////////////////////////////////////////////////////////////

static bool g_bDisableDemotion = false;

static void IntToString( int i, char s[7] )
{
  // avoid format printing during early start up
  int j;
  int digit;
  char sdig[10];
  sdig[0] = '0';
  sdig[1] = '1';
  sdig[2] = '2';
  sdig[3] = '3';
  sdig[4] = '4';
  sdig[5] = '5';
  sdig[6] = '6';
  sdig[7] = '7';
  sdig[8] = '8';
  sdig[9] = '9';
  for ( digit = 5; digit > 0; digit-- )
  {
    j = i%10;
    if ( j > 9 || j < 0 )
    {
      s[digit] = '-';
    }
    else
    {
      s[digit] = sdig[j];
    }
    i /= 10;
  }
  s[0] = '-';
  s[6] = 0;
}

MYON_ClassId::MYON_ClassId( const char* sClassName, 
                        const char* sBaseClassName, 
                        MYON_Object* (*create)(),
                        const char* sUUID // UUID in registry format from guidgen
                        ) 
                        : m_pNext(0),
                          m_pBaseClassId(0),
                          m_create(create),
                          m_mark(m_mark0),
                          m_class_id_version(0),
                          m_f1(0),
                          m_f2(0),
                          m_f3(0),
                          m_f4(0),
                          m_f5(0),
                          m_f6(0),
                          m_f7(0),
                          m_f8(0)
{
  // code compiled on or after opennurbs 200703060 calls this constructor
  ConstructorHelper(sClassName,sBaseClassName,sUUID);
  m_mark |= 0x80000000; // This bit of m_mark is a flag that indicates 
                        // the new constructor was called.
}

void MYON_ClassId::ConstructorHelper( const char* sClassName, 
                        const char* sBaseClassName, 
                        const char* sUUID // UUID in registry format from guidgen
                        ) 
{
  // Do not initialize "m_class_id_version" or any fields
  // after it in this helper.  See comments in the constructors
  // for more information.
  memset( m_sClassName, 0, sizeof(m_sClassName) );
  memset( m_sBaseClassName, 0, sizeof(m_sBaseClassName) );
  m_uuid = MYON_UuidFromString(sUUID);
  if ( sClassName ) {
    strncpy( m_sClassName, sClassName, sizeof(m_sClassName)-1 );
  }
  if ( sBaseClassName ) {
    strncpy( m_sBaseClassName, sBaseClassName, sizeof(m_sBaseClassName)-1 );
  }
  m_pBaseClassId = ClassId( m_sBaseClassName );

  if ( !m_sClassName[0] ) {
    MYON_ERROR("MYON_ClassId::MYON_ClassId() - missing class name");
    return;
  }

  const MYON_ClassId* duplicate_class = ClassId( m_sClassName );
  // The m_mark0 > 2 test prevents opennurbs and Rhino from
  // having two MYON_Object derived classes that have the same
  // name.  Plug-ins are free to use any name.
  if ( 0 != duplicate_class && m_mark0 > 2 )
  {
    char s[7];
    int ver;
    MYON_WARNING("MYON_ClassId::MYON_ClassId() - class name already in use.  Will append number to make it unique.");
    for ( ver = 1; ver < 10000 && 0 != duplicate_class; ver++ )
    {
      IntToString(ver,s);
      s[6] = 0;
      strncpy( m_sClassName, sClassName, sizeof(m_sClassName)-1 );
      strncat( m_sClassName, s, sizeof(m_sClassName)-1 );
      duplicate_class = ClassId( m_sClassName );
    }
  }

  if ( 0 != duplicate_class )
  {
    // Do NOT permit core classes to have duplicate names.
    MYON_ERROR("MYON_ClassId::MYON_ClassId() - class name already in use.");
    return;
  }

  if (    m_sClassName[0] != 'O'
       || m_sClassName[1] != 'N'
       || m_sClassName[2] != '_'
       || m_sClassName[3] != 'O'
       || m_sClassName[4] != 'b'
       || m_sClassName[5] != 'j'
       || m_sClassName[6] != 'e'
       || m_sClassName[7] != 'c'
       || m_sClassName[8] != 't'
       || m_sClassName[9] != 0 ) {
    if ( !m_sBaseClassName[0] ) 
    {
      MYON_ERROR("MYON_ClassId::MYON_ClassId() - missing baseclass name.");
      return;
    }
  }

  g_bDisableDemotion = true;
  if ( ClassId( m_uuid ) ) 
  {
    g_bDisableDemotion = false;
    MYON_ERROR("MYON_ClassId::MYON_ClassId() - class uuid already in use.");
    return;
  }
  g_bDisableDemotion = false;

  if ( MYON_UuidIsNil( m_uuid ) ) {
    MYON_ERROR("MYON_ClassId::MYON_ClassId() - class uuid is nill.");
    return;
  }

  // see if any derived classes need to be updated because their static
  // members got initialized first
  if ( m_sClassName[0] ) 
  {
    for ( MYON_ClassId* p = m_p0; p; p = p->m_pNext )
    {
      if ( 
        0 == p->m_pBaseClassId 
        && 0 != p->m_sBaseClassName[0] 
        && 0 == p->m_sBaseClassName[sizeof(p->m_sBaseClassName)/sizeof(p->m_sBaseClassName[0]) - 1] 
        )
      {
        if ( 0 == strcmp( m_sClassName, p->m_sBaseClassName ) )
          p->m_pBaseClassId = this;
      }
    }
  }

  // Append to the list of class ids
  if ( m_p0 && m_p1 )
  {
    m_p1->m_pNext = this;
    m_p1 = this;
  }
  else
  {
    // first class id
    m_p0 = this;
  }
  m_p1 = this;
  m_p1->m_pNext = 0;
}

MYON_ClassId::~MYON_ClassId()
{}

static MYON_UUID s_most_recent_class_id_create_uuid;

MYON_UUID MYON_GetMostRecentClassIdCreateUuid()
{
  return s_most_recent_class_id_create_uuid;
}

MYON_Object* MYON_ClassId::Create() const
{
  // Save the uuid so that Rhino's .NET SDK
  // can create approprate class.  The C++
  // opennurbs toolkit never uses this value.
  s_most_recent_class_id_create_uuid = m_uuid;
  return m_create ? m_create() : 0;
}

const MYON_ClassId* MYON_ClassId::ClassId( const char* sClassName )
{
  // static member function
  // search list of class ids for one with a matching class name
  MYON_ClassId* p;
  const char* s0;
  const char* s1;
  if ( !sClassName || !sClassName[0] || sClassName[0] == '0' )
    return nullptr;
  for(p = m_p0; p; p = p->m_pNext) {
    // avoid strcmp() because it crashes on nullptr strings
    s0 = sClassName;
    s1 = p->m_sClassName;
    if ( s0 && s1 && *s0 ) {
      while ( *s0 && *s0 == *s1 )
        {s0++; s1++;}
      if ( !(*s0) && !(*s1) )
        break;
    }
    else {
      break;
    }
  }
  return p;
}


const MYON_ClassId* MYON_ClassId::ClassId( MYON_UUID uuid )
{
  // static member function
  // search list of class ids for one with a matching typecode
  const MYON_ClassId* p;
  for(p = m_p0; p; p = p->m_pNext) 
  {
    if ( !MYON_UuidCompare(&p->m_uuid,&uuid) )
      break;
  }

  if ( nullptr == p && false == g_bDisableDemotion) 
  {
    // enable OpenNURBS toolkit to read files that contain old uuids even when
    // old class definitions are not loaded.

    // 5EAF1119-0B51-11d4-BFFE-0010830122F0 = TL_NurbsCurve
    MYON_UUID nc0 = {0x5EAF1119,0x0B51,0x11d4,{0xBF,0xFE,  0x00,0x10,0x83,0x01,0x22,0xF0}};

    // 76A709D5-1550-11d4-8000-0010830122F0 = old nurbs curve
    MYON_UUID nc1 = {0x76A709D5,0x1550,0x11d4,{0x80,0x00,  0x00,0x10,0x83,0x01,0x22,0xF0}};

    // 4760C817-0BE3-11d4-BFFE-0010830122F0 = TL_NurbsSurface
    MYON_UUID ns0 = {0x4760C817,0x0BE3,0x11d4,{0xBF,0xFE,  0x00,0x10,0x83,0x01,0x22,0xF0}};

    // FA4FD4B5-1613-11d4-8000-0010830122F0 = old nurbs surface
    MYON_UUID ns1 = {0xFA4FD4B5,0x1613,0x11d4,{0x80,0x00,  0x00,0x10,0x83,0x01,0x22,0xF0}};

    // EF638317-154B-11d4-8000-0010830122F0 = old poly curve
    MYON_UUID pc0 = {0xEF638317,0x154B,0x11d4,{0x80,0x00,  0x00,0x10,0x83,0x01,0x22,0xF0}};

    // 0705FDEF-3E2A-11d4-800E-0010830122F0 = old trimmed surface
    MYON_UUID br0 = {0x0705FDEF,0x3E2A,0x11d4,{0x80,0x0E,  0x00,0x10,0x83,0x01,0x22,0xF0}};

    // 2D4CFEDB-3E2A-11d4-800E-0010830122F0 = old b-rep
    MYON_UUID br1 = {0x2D4CFEDB,0x3E2A,0x11d4,{0x80,0x0E,  0x00,0x10,0x83,0x01,0x22,0xF0}};

    // F06FC243-A32A-4608-9DD8-A7D2C4CE2A36 = TL_Brep
    MYON_UUID br2 = {0xF06FC243,0xA32A,0x4608,{0x9D,0xD8, 0xA7,0xD2,0xC4,0xCE,0x2A,0x36}};

    // 0A8401B6-4D34-4b99-8615-1B4E723DC4E5 = TL_RevSurface
    MYON_UUID revsrf = { 0xa8401b6, 0x4d34, 0x4b99, { 0x86, 0x15, 0x1b, 0x4e, 0x72, 0x3d, 0xc4, 0xe5 } };

    // 665F6331-2A66-4cce-81D0-B5EEBD9B5417 = TL_SumSurface
    MYON_UUID sumsrf = { 0x665f6331, 0x2a66, 0x4cce, { 0x81, 0xd0, 0xb5, 0xee, 0xbd, 0x9b, 0x54, 0x17 } };

    if      ( !MYON_UuidCompare( &uuid, &nc0 ) || !MYON_UuidCompare( &uuid, &nc1 ) )
      p = &MYON_CLASS_RTTI(MYON_NurbsCurve);
    else if ( !MYON_UuidCompare( &uuid, &ns0 ) || !MYON_UuidCompare( &uuid, &ns1 ) )
      p = &MYON_CLASS_RTTI(MYON_NurbsSurface);
    else if ( !MYON_UuidCompare( &uuid, &pc0 ) )
      p = &MYON_CLASS_RTTI(MYON_PolyCurve);
    else if ( !MYON_UuidCompare( &uuid, &br0 ) || !MYON_UuidCompare( &uuid, &br1 ) || !MYON_UuidCompare( &uuid, &br2 ) )
      p = &MYON_CLASS_RTTI(MYON_Brep);
    else if ( !MYON_UuidCompare( &uuid, &revsrf ) )
      p = &MYON_CLASS_RTTI(MYON_RevSurface);
    else if ( !MYON_UuidCompare( &uuid, &sumsrf ) )
      p = &MYON_CLASS_RTTI(MYON_SumSurface);
    else
    {
      // The p = nullptr line does nothing (p is already nullptr) but, if you're working on
      // file reading bugs or other cases that involving rtti bugs, then it's a good 
      // location for a debugger breakpoint.
      p = nullptr;
    }
  }
  return p;
}

class MYON__ClassIdDumpNode
{
public:
  MYON__ClassIdDumpNode();
  ~MYON__ClassIdDumpNode();
  const MYON_ClassId* m_class_id;
  class MYON__ClassIdDumpNode* m_parent_node;
  int m_depth;
  MYON_SimpleArray<class MYON__ClassIdDumpNode*> m_child_nodes;
  int CompareClassUuid( const class MYON__ClassIdDumpNode& ) const;
  int CompareClassName( const class MYON__ClassIdDumpNode& ) const;
  bool Dump( int depth, MYON_TextLog& text_log );
};

MYON__ClassIdDumpNode::MYON__ClassIdDumpNode() 
{
  m_class_id=0;
  m_parent_node=0;
  m_depth=0;
};

MYON__ClassIdDumpNode::~MYON__ClassIdDumpNode() 
{
}

int MYON__ClassIdDumpNode::CompareClassUuid( const class MYON__ClassIdDumpNode& other ) const
{
  int rc = 0;
  const MYON_ClassId* a = m_class_id;
  const MYON_ClassId* b = other.m_class_id;
  if ( a != b )
  {
    if ( 0 == a )
    {
      rc = -1;
    }
    else if ( 0 == b )
      rc = 1;
    else
    {
      rc = MYON_UuidCompare(a->Uuid(),b->Uuid());
      if ( 0 == rc )
      {
        rc = CompareClassName(other);
      }
    }
  }
  return rc;
}

int MYON__ClassIdDumpNode::CompareClassName( const class MYON__ClassIdDumpNode& other ) const
{
  int rc = 0;
  const MYON_ClassId* a = m_class_id;
  const MYON_ClassId* b = other.m_class_id;
  if ( a != b )
  {
    if ( 0 == a )
    {
      rc = -1;
    }
    else if ( 0 == b )
      rc = 1;
    else
    {
      const char* a_name = a->ClassName();
      const char* b_name = b->ClassName();
      if ( 0 == a_name )
      {
        if ( 0 == b_name )
        {
          rc = b->Mark() - a->Mark();
          if ( 0 == rc )
            rc = MYON_UuidCompare(a->Uuid(),b->Uuid());
        }
        else
          rc = -1;
      }
      else if ( 0 == b_name )
      {
        rc = 1;
      }
      else
      {
        rc = on_stricmp(a_name,b_name);
        if ( 0 == rc )
        {
          rc = strcmp(a_name,b_name);
          if ( 0 == rc )
          {
            rc = b->Mark() - a->Mark();
            if ( 0 == rc )
              rc = MYON_UuidCompare(a->Uuid(),b->Uuid());
          }
        }
      }
    }
  }
  return rc;
}


static int MYON__ClassIdDumpNode_CompareUuid( const MYON__ClassIdDumpNode* a, const MYON__ClassIdDumpNode* b )
{
  int rc = 0;
  if ( 0 == a )
  {
    rc = (0 == b) ? 0 : -1;
  }
  else if ( 0 == b )
  {
    rc = 1;
  }
  else
  {
    rc = a->CompareClassUuid(*b);
  }
  return rc;
}

static int MYON__ClassIdDumpNode_CompareName( MYON__ClassIdDumpNode *const* a, MYON__ClassIdDumpNode *const* b )
{
  int rc = 0;
  if ( 0 == a )
  {
    rc = (0 == b) ? 0 : -1;
  }
  else if ( 0 == b )
  {
    rc = 1;
  }
  else
  {
    rc = (*a)->CompareClassName(*(*b));
  }
  return rc;
}

bool MYON__ClassIdDumpNode::Dump( int depth, MYON_TextLog& text_log )
{
  bool rc = true;
  if ( 0 == m_class_id || m_depth != 0 || depth < 1)
    rc = false;
  else
  {
    m_depth = depth;
    const char* class_name = m_class_id->ClassName();
    if ( 0 == class_name )
    {
      class_name = "!!ERROR!!";
      rc = false;
    }
    text_log.Print("%s::ClassId: ",m_class_id->ClassName());
    text_log.Print( "mark=%d ",m_class_id->Mark() );
    text_log.Print( m_class_id->Uuid() );
    text_log.Print("  (%08x)\n",m_class_id);
    int i, count = m_child_nodes.Count();
    if ( count > 0 )
    {
      // dump children names alphabetically
      m_child_nodes.QuickSort( MYON__ClassIdDumpNode_CompareName );

      text_log.PushIndent();
      for ( i = 0; i < count; i++ )
      {
        MYON__ClassIdDumpNode* child_node = m_child_nodes[i];
        if ( 0 == child_node )
          rc = false;
        else
        {
          if ( !child_node->Dump(depth+1,text_log) )
            rc = false;
        }
      }
      text_log.PopIndent();
    }
  }
  return rc;
}

void MYON_ClassId::Dump( MYON_TextLog& dump )
{
  int i, j, count = 0;
  const MYON_ClassId* p;
  for(p = m_p0; p && count < 1000000; p = p->m_pNext) 
  {
    count++;
  }
  if ( 0 != p )
  {
    dump.Print("MYON_ClassId::m_p0 list is damaged.\n");
  }
  else
  {
    MYON__ClassIdDumpNode tmp_node;
    MYON_ClassArray<MYON__ClassIdDumpNode> nodes(count);
    for(p = m_p0; p; p = p->m_pNext) 
    {
      MYON__ClassIdDumpNode& node = nodes.AppendNew();
      node.m_class_id = p;
    }

    // sort nodes by class id's uuid
    nodes.QuickSort(MYON__ClassIdDumpNode_CompareUuid);

    // fill in m_parent_node and m_child_nodes[]
    for ( i = 0; i < count; i++ )
    {
      MYON__ClassIdDumpNode& node = nodes[i];
      p = node.m_class_id;
      if ( 0 != p )
      {
        tmp_node.m_class_id = p->BaseClass();
        j = nodes.BinarySearch(&tmp_node,MYON__ClassIdDumpNode_CompareUuid);
        if ( j >= 0 && i != j)
        {
          MYON__ClassIdDumpNode& base_node = nodes[j];
          node.m_parent_node = &base_node;
          base_node.m_child_nodes.Append(&node);
        }
      }      
    }

    // print class tree
	  tmp_node.m_class_id = &MYON_CLASS_RTTI(MYON_Object);
    i = nodes.BinarySearch(&tmp_node,MYON__ClassIdDumpNode_CompareUuid);
    bool rc = false;
    if ( i >= 0 )
    {
      // recursively dump class tree
      rc = nodes[i].Dump(1,dump);
      for ( i = 0; i < count && rc; i++ )
      {
        if ( nodes[i].m_depth <= 0 )
          rc = false;
      }
    }

    if (!rc)
    {
      // should never happen
      for(p = m_p0; p; p = p->m_pNext) 
      {
        dump.Print("%s::ClassId: ",p->m_sClassName);
        dump.Print( "mark=%d ",p->m_mark );
        dump.Print( p->m_uuid );
        dump.Print("  (%08x)\n",p);
      }
    }
  }
}

const char* MYON_ClassId::ClassName() const
{
  return m_sClassName;
}

const char* MYON_ClassId::BaseClassName() const
{
  return m_sBaseClassName;
}

MYON_UUID MYON_ClassId::Uuid() const
{
  return m_uuid;
}

int MYON_ClassId::Mark() const
{
  return (m_mark & 0x7FFFFFFF);
}

unsigned int MYON_ClassId::ClassIdVersion() const
{
  return (0 != (m_mark & 0x80000000)) ? m_class_id_version : 0;
}


const MYON_ClassId* MYON_ClassId::BaseClass() const
{
  return m_pBaseClassId;
}

bool MYON_ClassId::IsDerivedFrom( const MYON_ClassId* pBaseClassId ) const
{
  // determine if this is derived from pBaseClassId
  bool b = false;
  if ( pBaseClassId ) {
    const MYON_ClassId* p = this;
    for(;p;) {
      if ( p == pBaseClassId ) {
        b = true;
        break;
      }
      p = p->m_pBaseClassId;
    }
  }
  return b;
}

//////////////////////////////////////////////////////////////////////////////

MYON_VIRTUAL_OBJECT_IMPLEMENT(MYON_Object,0,"60B5DBBD-E660-11d3-BFE4-0010830122F0");

MYON_Object::MYON_Object() MYON_NOEXCEPT
: m_userdata_list(0)
{}

MYON_Object::MYON_Object(const MYON_Object& src)
  : m_userdata_list(0)
{
  this->CopyUserData(src);
}

MYON_Object& MYON_Object::operator=(const MYON_Object& src)
{
  // DO NOT MODIFY this->m_mempool here
  if ( this != &src ) 
  {
    this->PurgeUserData();
    this->CopyUserData(src);
  }
  return *this;
}

#if defined(MYON_HAS_RVALUEREF)
MYON_Object::MYON_Object( MYON_Object&& src ) MYON_NOEXCEPT
  : m_userdata_list(0)
{
  this->MoveUserData(src);
}

MYON_Object& MYON_Object::operator=( MYON_Object&& src )
{
  if ( this != &src )
  {
    this->PurgeUserData();
    this->MoveUserData(src);
  }
  return *this;
}
#endif

MYON_Object::~MYON_Object()
{
  this->PurgeUserData();
}

// DO NOT PUT THIS DECL IN A HEADER FILE.
// THIS FUNCTION IS USED IN SPECIAL CIRCUMSTANCES
// AND IS NOT INTENDED TO BE CALLED.
MYON_DECL bool MYON__EnableLeakUserData(bool bEnable);

static bool g__bLeakUserData = false;

bool MYON__EnableLeakUserData(bool bEnable)
{
  bool b = bEnable;
  g__bLeakUserData = bEnable ? true : false;
  return b;
}

void MYON_Object::EmergencyDestroy()
{
  m_userdata_list = 0;
}


void MYON_Object::PurgeUserData()
{
  MYON_UserData* p;
  MYON_UserData* next;
  bool bDeleteUserData;
  if ( 0 != (next=m_userdata_list) ) 
  {
    m_userdata_list = 0;
    bDeleteUserData = !g__bLeakUserData;
    while( 0 != (p=next) ) 
    {
      next = p->m_userdata_next;
      p->m_userdata_owner = 0;
      p->m_userdata_next = 0;
      if ( bDeleteUserData )
        delete p;
    }
  }
}

bool MYON_Object::AttachUserData( MYON_UserData* p )
{
  bool rc = false;
  if ( p 
       && nullptr == p->m_userdata_owner
       && MYON_UuidCompare( &MYON_nil_uuid, &p->m_userdata_uuid) 
       && nullptr == GetUserData( p->m_userdata_uuid )
       ) {
    if ( p->IsUnknownUserData() ) {
      // make sure we have valid user data - the first beta release of Rhino 2.0 
      // created empty user data.
      MYON_UnknownUserData* uud = MYON_UnknownUserData::Cast(p);
      if (uud)
        rc = uud->IsValid();
      if ( !rc ) {
        MYON_ERROR("MYON_Object::AttachUserData() - attempt to attach invalid UnknownUserData.");
      }
    }
    else
      rc = true;
    if (rc) 
    {
      p->m_userdata_owner = this;
      p->m_userdata_next = m_userdata_list;
      m_userdata_list = p;
    }
  }
  return rc;
}

bool MYON_Object::DetachUserData( MYON_UserData* p )
{
  bool rc = false;
  if ( p && p->m_userdata_owner == this ) 
  {
    MYON_UserData* prev = 0;
    MYON_UserData* ud = m_userdata_list;
    while ( ud ) 
    {
      if ( ud == p ) 
      {
        if ( prev )
          prev->m_userdata_next = ud->m_userdata_next;
        else
          m_userdata_list = ud->m_userdata_next;
        ud->m_userdata_owner = 0;
        ud->m_userdata_next = 0;
        rc = true;
        break;
      }
      prev = ud;
      ud = ud->m_userdata_next;
    }
  }
  return rc;
}


MYON_UserData* MYON_Object::GetUserData( const MYON_UUID& userdata_uuid ) const
 {
  MYON_UserData* prev = nullptr;
  MYON_UserData* p;
  for ( p = m_userdata_list; p; prev = p, p = p->m_userdata_next ) 
  {
    if ( !MYON_UuidCompare( &p->m_userdata_uuid, &userdata_uuid ) ) 
    {
      if ( p->IsUnknownUserData() ) 
      {
        // See if we can convert this unknown user data into something useful.
        // Unknown user data is created when a 3dm archive is read and
        // the definition of the specific user data class is not loaded.
        // If something is getting around to asking for a specific kind
        // of user data, the class definition has probably be dynamically
        // loaded.
        MYON_UnknownUserData* uud = MYON_UnknownUserData::Cast(p);
        if ( uud ) {
          MYON_UserData* realp = uud->Convert();
          if ( realp ) 
          {
            // replace unknown user data with the real thing
            if ( prev )
              prev->m_userdata_next = realp;
            else if ( p == m_userdata_list ) 
            {
              // little white lie to attach the "real" user
              // data to the object in place of the unknown
              // user data.
              MYON_Object* pNotConst = const_cast<MYON_Object*>(this);
              pNotConst->m_userdata_list = realp;
              realp->m_userdata_owner = pNotConst; // Dale Lear added 22 Jan 2004 to fix I/O bug 
            }
            realp->m_userdata_next = p->m_userdata_next;
            p->m_userdata_next = 0;
            p->m_userdata_owner = 0;
            delete p;
            p = realp;
          }
        }
      }
      break;
    }
  }
  return p; 
}

MYON_UserData* MYON_Object::FirstUserData() const
{
  return m_userdata_list;
}


void MYON_Object::TransformUserData( const MYON_Xform& x )
{
  MYON_UserData *p, *next;
  for ( p = m_userdata_list; p; p = next ) {
    next = p->m_userdata_next;
    if ( !p->Transform(x) )
      delete p;
  }
}

MYON_UserData* MYON_Object::TransferUserDataItem(
  const MYON_UserData* source_ud_copy_this,
  MYON_UserData* source_ud_move_this,
  bool bPerformConflictCheck,
  MYON_Object::UserDataConflictResolution userdata_conflict_resolution
  )
{
  const MYON_UserData* source_ud; // do not initialize so compiler will detect future bugs

  if (nullptr != source_ud_move_this)
  {
    if (nullptr != source_ud_copy_this)
    {
      MYON_ERROR("At most one source_ud pointer can be not null.");
      return nullptr;
    }
    if (nullptr != source_ud_move_this->m_userdata_owner || nullptr != source_ud_move_this->m_userdata_next)
    {
      MYON_ERROR("Cannot move userdata that is attached to another object.");
      return nullptr;
    }
    source_ud = source_ud_move_this;
  }
  else if ( nullptr != source_ud_copy_this )
  {
    if (this == source_ud_copy_this->m_userdata_owner)
    {
      MYON_ERROR("source_ud_copy_this is already attached to this object.");
      return nullptr;
    }
    source_ud = source_ud_copy_this;
  }
  else
  {
    // nothing to do
    return nullptr;
  }

  if (source_ud->IsUnknownUserData())
  {
    // make sure we have valid user data - the first beta release of Rhino 2.0 
    // created empty user data.
    const MYON_UnknownUserData* uud = MYON_UnknownUserData::Cast(source_ud);
    if (nullptr == uud && false == uud->IsValid())
    {
      return nullptr;
    }
  }
  
  MYON_UserData* dest_ud = bPerformConflictCheck ? GetUserData(source_ud->m_userdata_uuid) : nullptr;

  bool bDeleteDestinationItem = false;
  bool bTransferSourceItem; // no initialization
  if (nullptr == dest_ud)
  {
    bTransferSourceItem = true;
  }
  else
  {
    switch (userdata_conflict_resolution)
    {
    case MYON_Object::UserDataConflictResolution::destination_object:
      bTransferSourceItem = false;
      break;

    case MYON_Object::UserDataConflictResolution::source_object:
      bTransferSourceItem = true;
      break;

    case MYON_Object::UserDataConflictResolution::source_copycount_gt:
      bTransferSourceItem = (source_ud->m_userdata_copycount > dest_ud->m_userdata_copycount);
      break;

    case MYON_Object::UserDataConflictResolution::source_copycount_ge:
      bTransferSourceItem =  (source_ud->m_userdata_copycount >= dest_ud->m_userdata_copycount);
      break;

    case MYON_Object::UserDataConflictResolution::destination_copycount_gt:
      bTransferSourceItem = (dest_ud->m_userdata_copycount > source_ud->m_userdata_copycount);
      break;

    case MYON_Object::UserDataConflictResolution::destination_copycount_ge:
      bTransferSourceItem =  (dest_ud->m_userdata_copycount >= source_ud->m_userdata_copycount);
      break;

    case MYON_Object::UserDataConflictResolution::delete_item:
      bTransferSourceItem = false;
      bDeleteDestinationItem = true;
      break;

    default:
      bTransferSourceItem = false;
      break;
    }
  }

  if (false == bTransferSourceItem)
  {
    if (bDeleteDestinationItem && nullptr != dest_ud)
    {
      delete dest_ud;
    }
    return nullptr;
  }


  if (nullptr != source_ud_copy_this)
  {
    MYON_Object* p = source_ud_copy_this->Duplicate();
    if ( nullptr == p )
      return nullptr;
    source_ud_move_this = MYON_UserData::Cast(p);

    if (nullptr == source_ud_move_this)
    {
      delete p;
      return nullptr;
    }
    source_ud_move_this->m_userdata_owner = nullptr;
  }

  if (nullptr == source_ud_move_this)
  {
    MYON_ERROR("Bug in the code above.");
    return nullptr;
  }

  if (nullptr != dest_ud)
  {
    delete dest_ud;
  }

  source_ud_move_this->m_userdata_owner = this;
  source_ud_move_this->m_userdata_next = m_userdata_list;
  m_userdata_list = source_ud_move_this;
  
  return m_userdata_list;
};

unsigned int MYON_Object::CopyUserData(
  const MYON_Object& source_object,
  MYON_UUID source_userdata_item_id,
  MYON_Object::UserDataConflictResolution userdata_conflict_resolution
  )
{
  unsigned int copied_item_count = 0;

  if (this == &source_object)
    return copied_item_count;

  const bool bPerformConflictCheck = (nullptr != m_userdata_list);

  const bool bIgnoreUserDataItemId = (MYON_nil_uuid == source_userdata_item_id);

  for (const MYON_UserData* source_ud = source_object.m_userdata_list; nullptr != source_ud; source_ud = source_ud->m_userdata_next)
  {
    if (source_ud->m_userdata_copycount <= 0)
      continue;
    if (bIgnoreUserDataItemId || source_ud->m_userdata_uuid == source_userdata_item_id)
    {
      if (nullptr != TransferUserDataItem(source_ud, nullptr, bPerformConflictCheck, userdata_conflict_resolution))
        copied_item_count++;
    }
  }
  return copied_item_count;
}


unsigned int MYON_Object::MoveUserData(
  MYON_Object& source_object,
  MYON_UUID source_userdata_item_id,
  MYON_Object::UserDataConflictResolution userdata_conflict_resolution,
  bool bDeleteAllSourceItems)
{
  unsigned int moved_item_count = 0;

  const bool bIgnoreUserDataItemId = (MYON_nil_uuid == source_userdata_item_id);

  if ( nullptr == m_userdata_list && bIgnoreUserDataItemId )
  {
    // quick and simple when the "this" doesn't
    // have any user data.
    if ( nullptr != source_object.m_userdata_list )
    {
      m_userdata_list = source_object.m_userdata_list;
      source_object.m_userdata_list = nullptr;
      for (MYON_UserData* dest_ud = m_userdata_list; nullptr != dest_ud; dest_ud = dest_ud->m_userdata_next)
      {
        dest_ud->m_userdata_owner = this;
        moved_item_count++;
      }
    }
  }
  else
  {    
    // Carefully move userdata an item at a time from source_object to "this"
    const bool bPerformConflictCheck = true;
    MYON_UserData* source_ud_next = source_object.m_userdata_list;
    source_object.m_userdata_list = nullptr;
    MYON_UserData* source_object_userdata_last = nullptr;
    for ( MYON_UserData* source_ud = source_ud_next; nullptr != source_ud; source_ud = source_ud_next)
    {
      source_ud_next = source_ud->m_userdata_next;
      source_ud->m_userdata_next = nullptr;
      source_ud->m_userdata_owner = nullptr;

      if (bIgnoreUserDataItemId || source_ud->m_userdata_uuid == source_userdata_item_id)
      {
        if (TransferUserDataItem(nullptr, source_ud, bPerformConflictCheck, userdata_conflict_resolution))
        {
          moved_item_count++;
          continue;
        }
      }

      // The transfer did not occur.  Resolve state of orphaned source_ud.
      if (nullptr != source_ud->m_userdata_owner || nullptr != source_ud->m_userdata_next)
      {
        MYON_ERROR("There is a serious bug in this code.");
        continue;
      }
      if (bDeleteAllSourceItems)
      {
        // delete the orphan
        delete source_ud;
      }
      else
      {
        // reattach the orphan to source_object
        source_ud->m_userdata_owner = &source_object;
        if (nullptr == source_object.m_userdata_list)
        {
          source_object.m_userdata_list = source_ud;
        }
        else if (nullptr != source_object_userdata_last)
        {
          source_object_userdata_last->m_userdata_next = source_ud;
        }
        source_object_userdata_last = source_ud;
      }
    }
  }

  return moved_item_count;
}


void MYON_Object::CopyUserData( const MYON_Object& src )
{
  CopyUserData(src,MYON_nil_uuid,MYON_Object::UserDataConflictResolution::destination_object);
}

void MYON_Object::MoveUserData( MYON_Object& src )
{
  MoveUserData(src,MYON_nil_uuid,MYON_Object::UserDataConflictResolution::destination_object,true);
}



bool MYON_Object::UpdateReferencedComponents(
  const class MYON_ComponentManifest& source_manifest,
  const class MYON_ComponentManifest& destination_manifest,
  const class MYON_ManifestMap& manifest_map
  )
{
  return true;
}

void MYON_Object::MemoryRelocate()
{
  // When the memory location of an MYON_Object changes,
  // the back-pointers on its user data need to be updated.
  MYON_UserData* ud;
  for ( ud = m_userdata_list; ud; ud = ud->m_userdata_next )
  {
    ud->m_userdata_owner = this;
  }
}

bool MYON_Object::IsKindOf( const MYON_ClassId* pBaseClassId ) const
{
  bool b = false;
  const MYON_ClassId* p = ClassId();
  if ( p )
    b = p->IsDerivedFrom( pBaseClassId );
  return b;
}


MYON::object_type MYON_Object::ObjectType() const
{
  // virtual function that is generally overridden
  return MYON::unknown_object_type;
}

MYON_UUID MYON_Object::ModelObjectId() const
{
  return MYON_nil_uuid;
}

MYON_UUID MYON_Light::ModelObjectId() const
{
  return m_light_id;
}

unsigned int MYON_Object::SizeOf() const
{
  unsigned int sz = sizeof(*this);
  const MYON_UserData* ud = m_userdata_list;
  while ( ud )
  {
    sz += ud->SizeOf();
    ud = ud->m_userdata_next;
  }
  return sz;
}

MYON__UINT32 MYON_Object::DataCRC(MYON__UINT32 current_remainder) const
{
  // do not include user data.
  return current_remainder;
}

bool MYON_Object::IsValid(MYON_TextLog* text_log) const
{
  return true;
}

bool MYON_Object::ThisIsNullptr(
  bool bSilentError
) const
{
  // CLang warns that these tests may be ommitted because in "well-defined C++ code"
  // they are always false. 
  //
  // Earth to CLang: 
  //   This tool to find code that is not well formed, alert us to that fact, 
  //   but not potentiall not crash so our loyal customers don't loose their work.
  //
  // return (this == nullptr);
  // return ( nullptr == this );
  if (0 != ((MYON__UINT_PTR)this))
    return false;

  if (false == bSilentError)
    MYON_ERROR("this is nullptr.");

  return true;
}

bool MYON_Object::IsCorrupt(
  bool bRepair,
  bool bSilentError,
  class MYON_TextLog* text_log
) const
{
  bool rc = true;
  if (false == ThisIsNullptr(bSilentError) )
  {
    switch (ObjectType())
    {
    case MYON::object_type::brep_object:
      {
        const MYON_Brep* brep = MYON_Brep::Cast(this);
        if (brep)
          rc = brep->MYON_Brep::IsCorrupt(bRepair, bSilentError, text_log);
        else if ( false == bSilentError )
          MYON_ERROR("MYON_Brep::Cast(this) failed.");
      }
      break;

    case MYON::object_type::mesh_object:
      {
        const MYON_Mesh* mesh = MYON_Mesh::Cast(this);
        if (mesh)
          rc = mesh->MYON_Mesh::IsCorrupt(bRepair, bSilentError, text_log);
        else if ( false == bSilentError )
          MYON_ERROR("MYON_Mesh::Cast(this) failed.");
      }
      break;

    default:
      rc = false;
      break;
    }
  }
  return rc;
}

void MYON_Object::Dump( MYON_TextLog& dump ) const
{
  const MYON_ClassId* p = ClassId();
  if ( p ) 
  {
    const char* class_name = p->ClassName();
    if ( 0 == class_name ) 
      class_name = "unknown";
    dump.Print("class name: %s\n",class_name);
    dump.Print("class uuid: ");
    dump.Print(p->Uuid());
    dump.Print("\n");
  }
  else 
  {
    dump.Print("MYON_Object::ClassId() FAILED\n");
  }
}

bool MYON_Object::Write(
       MYON_BinaryArchive&
     ) const
{
  // default Write() does nothing.
  return false;

  // object derived from MYON_Object should have a Write() that looks
  // something like

  /*
  bool rc = file.Write3dmChunkVersion(1,0);
  if (rc) {
    // TODO
  }
  return rc;
  */

}

bool MYON_Object::Read(
       MYON_BinaryArchive&
     )
{
  // default Read() does nothing.
  return false;

  // object derived from MYON_Object should have a Read() that looks
  // something like

  /*
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc && major_version==1) {
    // common to all 1.x versions
    // TODO
  }
  return rc;
  */

}

void MYON_Object::DestroyRuntimeCache( bool bDelete )
{
}

void MYON_Curve::DestroyRuntimeCache( bool bDelete )
{
}


void MYON_CurveProxy::DestroyRuntimeCache( bool bDelete )
{
  MYON_Curve::DestroyRuntimeCache(bDelete);
  if ( 0 != m_real_curve && m_real_curve != this )
  {
    MYON_Curve* curve = const_cast<MYON_Curve*>(m_real_curve);
    if ( 0 != curve )
      curve->DestroyRuntimeCache( bDelete );
  }
}

void MYON_Surface::DestroyRuntimeCache( bool bDelete )
{
}

void MYON_SurfaceProxy::DestroyRuntimeCache( bool bDelete )
{
  MYON_Surface::DestroyRuntimeCache( bDelete );
  if ( 0 != m_surface && m_surface != this )
  {
    MYON_Surface* surface = const_cast<MYON_Surface*>(m_surface);
    if ( 0 != surface )
      surface->DestroyRuntimeCache( bDelete );
  }
}

void MYON_Brep::DestroyRuntimeCache( bool bDelete )
{
  int i, count;

  count = m_C2.Count();
  for ( i = 0; i < count; i++ )
  {
    if ( m_C2[i] )
      m_C2[i]->DestroyRuntimeCache(bDelete);
  }

  count = m_C3.Count();
  for ( i = 0; i < count; i++ )
  {
    if ( m_C3[i] )
      m_C3[i]->DestroyRuntimeCache(bDelete);
  }

  count = m_S.Count();
  for ( i = 0; i < count; i++ )
  {
    if ( m_S[i] )
      m_S[i]->DestroyRuntimeCache(bDelete);
  }

  count = m_T.Count();
  for ( i = 0; i < count; i++ )
  {
    m_T[i].DestroyRuntimeCache(bDelete);
  }

  count = m_E.Count();
  for ( i = 0; i < count; i++ )
  {
    m_E[i].DestroyRuntimeCache(bDelete);
  }

  count = m_F.Count();
  for ( i = 0; i < count; i++ )
  {
    m_F[i].DestroyRuntimeCache(bDelete);
  }

  // 15 August 2003 Dale Lear:
  //    I added the line to destroy the face's m_bbox.
  //    Since m_bbox is private, it will be recalculated
  //    when it is needed.  (We hope.)  The fact the face
  //    m_bbox is private and recalculated as needed makes
  //    it different than the m_pbox info on trims and loops.
  m_bbox.Destroy();
}


template <class T> class MYON_ArrayIterator
{
public:
  MYON_ArrayIterator() = default;
  ~MYON_ArrayIterator() = default;
  MYON_ArrayIterator(const MYON_ArrayIterator<T>&) = default;
  MYON_ArrayIterator& operator=(const MYON_ArrayIterator<T>&) = default;

  MYON_ArrayIterator(
    T* first,
    size_t count
    )
  {
    m_first = (count > 0) ? first : nullptr;
    m_last = (nullptr != m_first) ? (m_first+(count-1)) : nullptr;
    m_current = m_first;
  }

  MYON_ArrayIterator(
    T* first,
    T* last
    )
  {
    m_first = (nullptr != first && first <= last) ? first : nullptr;
    m_last = (nullptr != m_first) ? last : nullptr;
    m_current = m_first;
  }

  MYON_ArrayIterator(
    MYON_SimpleArray < T >& a
    )
  {
    unsigned int count = a.UnsignedCount();
    T* first = a.Array();
    m_first = (count > 0) ? first : nullptr;
    m_last = (nullptr != m_first) ? (m_first+(count-1)) : nullptr;
    m_current = m_first;
  }

  MYON_ArrayIterator(
    MYON_ClassArray < T >& a
    )
  {
    unsigned int count = a.UnsignedCount();
    T* first = a.Array();
    m_first = (count > 0) ? first : nullptr;
    m_last = (nullptr != m_first) ? (m_first+(count-1)) : nullptr;
    m_current = m_first;
  }

  MYON_ArrayIterator(
    MYON_ObjectArray < T >& a
    )
  {
    unsigned int count = a.UnsignedCount();
    T* first = a.Array();
    m_first = (count > 0) ? first : nullptr;
    m_last = (nullptr != m_first) ? (m_first+(count-1)) : nullptr;
    m_current = m_first;
  }

  T* First()
  {
    m_current = m_first;
    return m_first;
  }

  T* Next()
  {
    if (m_current < m_last)
      m_current++;
    else
      m_current = nullptr;
    return m_current;
  }

  T* Current() const
  {
    return m_current;
  }

  size_t Count() const
  {
    return (m_last - m_first);
  }

private:
  T* m_first = nullptr;
  T* m_last = nullptr;
  T* m_current = nullptr;
};

//virtual
unsigned int MYON_Brep::ClearComponentStates(
  MYON_ComponentStatus states_to_clear
  ) const
{
  if (states_to_clear.IsClear())
    return 0U;

  m_aggregate_status = MYON_AggregateComponentStatus::NotCurrent;

  unsigned int rc = 0;

  MYON_ArrayIterator< const MYON_BrepVertex > vit( m_V.Array(), m_V.UnsignedCount() );
  for ( const MYON_BrepVertex* p = vit.First(); nullptr != p; p = vit.Next())
  {
    rc += p->m_status.ClearStates(states_to_clear);
  }

  MYON_ArrayIterator< const MYON_BrepEdge > eit( m_E.Array(), m_E.UnsignedCount() );
  for ( const MYON_BrepEdge* p = eit.First(); nullptr != p; p = eit.Next())
  {
    rc += p->m_status.ClearStates(states_to_clear);
  }

  MYON_ArrayIterator< const MYON_BrepTrim > tit( m_T.Array(), m_T.UnsignedCount() );
  for ( const MYON_BrepTrim* p = tit.First(); nullptr != p; p = tit.Next())
  {
    rc += p->m_status.ClearStates(states_to_clear);
  }

  MYON_ArrayIterator< const MYON_BrepLoop > lit( m_L.Array(), m_L.UnsignedCount() );
  for ( const MYON_BrepLoop* p = lit.First(); nullptr != p; p = lit.Next())
  {
    rc += p->m_status.ClearStates(states_to_clear);
  }

  MYON_ArrayIterator< const MYON_BrepFace > fit( m_F.Array(), m_F.UnsignedCount() );
  for ( const MYON_BrepFace* p = fit.First(); nullptr != p; p = fit.Next())
  {
    rc += p->m_status.ClearStates(states_to_clear);
  }

  return rc;
}

//virtual
unsigned int MYON_Brep::GetComponentsWithSetStates(
  MYON_ComponentStatus states_filter,
  bool bAllEqualStates,
  MYON_SimpleArray< MYON_COMPONENT_INDEX >& components
  ) const
{
  components.SetCount(0);

  if (states_filter.IsClear())
    return 0;

  MYON_AggregateComponentStatus acs = AggregateComponentStatus();

  MYON_ComponentStatus as = acs.AggregateStatus();
  if (bAllEqualStates)
  {
    if ( false == as.AllEqualStates(states_filter, states_filter) )
      return 0;
  }
  else
  {
    if ( false == as.SomeEqualStates(states_filter, states_filter) )
      return 0;
  }

  unsigned int c = 0;
  if ( states_filter.IsSelected() && c < m_aggregate_status.SelectedCount() )
    c = m_aggregate_status.SelectedCount();
  if ( states_filter.IsHighlighted() && c < m_aggregate_status.HighlightedCount() )
    c = m_aggregate_status.HighlightedCount();
  if ( states_filter.IsHidden() && c < m_aggregate_status.HiddenCount() )
    c = m_aggregate_status.HiddenCount();
  if ( states_filter.IsLocked() && c < m_aggregate_status.LockedCount() )
    c = m_aggregate_status.LockedCount();
  if ( states_filter.IsDamaged() && c < m_aggregate_status.DamagedCount() )
    c = m_aggregate_status.DamagedCount();
  if ( states_filter.IsSelected() && c < m_aggregate_status.SelectedCount() )
    c = m_aggregate_status.SelectedCount();
  components.Reserve(c);

  MYON_ArrayIterator< const MYON_BrepVertex > vit( m_V.Array(), m_V.UnsignedCount() );
  for ( const MYON_BrepVertex* p = vit.First(); nullptr != p; p = vit.Next())
  {
    if (bAllEqualStates ? p->m_status.AllEqualStates(states_filter, states_filter) : p->m_status.SomeEqualStates(states_filter, states_filter))
      components.Append(p->ComponentIndex());
  }

  MYON_ArrayIterator< const MYON_BrepEdge > eit( m_E.Array(), m_E.UnsignedCount() );
  for ( const MYON_BrepEdge* p = eit.First(); nullptr != p; p = eit.Next())
  {
    if (bAllEqualStates ? p->m_status.AllEqualStates(states_filter, states_filter) : p->m_status.SomeEqualStates(states_filter, states_filter))
      components.Append(p->ComponentIndex());
  }

  MYON_ArrayIterator< const MYON_BrepTrim > tit( m_T.Array(), m_T.UnsignedCount() );
  for ( const MYON_BrepTrim* p = tit.First(); nullptr != p; p = tit.Next())
  {
    if (bAllEqualStates ? p->m_status.AllEqualStates(states_filter, states_filter) : p->m_status.SomeEqualStates(states_filter, states_filter))
      components.Append(p->ComponentIndex());
  }

  MYON_ArrayIterator< const MYON_BrepLoop > lit( m_L.Array(), m_L.UnsignedCount() );
  for ( const MYON_BrepLoop* p = lit.First(); nullptr != p; p = lit.Next())
  {
    if (bAllEqualStates ? p->m_status.AllEqualStates(states_filter, states_filter) : p->m_status.SomeEqualStates(states_filter, states_filter))
      components.Append(p->ComponentIndex());
  }

  MYON_ArrayIterator< const MYON_BrepFace > fit( m_F.Array(), m_F.UnsignedCount() );
  for ( const MYON_BrepFace* p = fit.First(); nullptr != p; p = fit.Next())
  {
    if (bAllEqualStates ? p->m_status.AllEqualStates(states_filter, states_filter) : p->m_status.SomeEqualStates(states_filter, states_filter))
      components.Append(p->ComponentIndex());
  }

  return components.UnsignedCount();
}

static MYON_ComponentStatus* BrepComponentStatus(
  const MYON_Brep& brep,
  MYON_COMPONENT_INDEX component_index
  )
{
  if (component_index.m_index >= 0)
  {
    switch (component_index.m_type)
    {
    case MYON_COMPONENT_INDEX::TYPE::brep_vertex:
      if ( component_index.m_index < brep.m_V.Count() )
        return &brep.m_V[component_index.m_index].m_status;
      break;
    case MYON_COMPONENT_INDEX::TYPE::brep_edge:
      if ( component_index.m_index < brep.m_E.Count() )
        return &brep.m_E[component_index.m_index].m_status;
      break;
    case MYON_COMPONENT_INDEX::TYPE::brep_trim:
      if ( component_index.m_index < brep.m_T.Count() )
        return &brep.m_T[component_index.m_index].m_status;
      break;
    case MYON_COMPONENT_INDEX::TYPE::brep_loop:
      if ( component_index.m_index < brep.m_L.Count() )
        return &brep.m_L[component_index.m_index].m_status;
      break;
    case MYON_COMPONENT_INDEX::TYPE::brep_face:
      if ( component_index.m_index < brep.m_F.Count() )
        return &brep.m_F[component_index.m_index].m_status;
      break;
    default:
      break;
    }
  }
  return nullptr;
}
  
//virtual
unsigned int MYON_Brep::SetComponentStates(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus states_to_set
  ) const
{
  MYON_ComponentStatus* s = BrepComponentStatus(*this,component_index);
  return 
    (nullptr == s)
    ? 0U
    : s->SetStates(states_to_set);
}

//virtual
unsigned int MYON_Brep::ClearComponentStates(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus states_to_clear
  ) const
{
  MYON_ComponentStatus* s = BrepComponentStatus(*this,component_index);
  return 
    (nullptr == s)
    ? 0U
    : s->ClearStates(states_to_clear);
}
  
//virtual
unsigned int MYON_Brep::SetComponentStatus(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus status_to_copy
  ) const
{
  MYON_ComponentStatus* s = BrepComponentStatus(*this,component_index);
  return 
    (nullptr == s)
    ? 0U
    : s->SetStatus(status_to_copy);
}

//virtual
MYON_AggregateComponentStatus MYON_Brep::AggregateComponentStatus() const
{
  if (false == m_aggregate_status.IsCurrent())
  {
    if (m_V.UnsignedCount() == 0 )
      return MYON_AggregateComponentStatus::Empty;

    MYON_AggregateComponentStatus a = MYON_AggregateComponentStatus::Empty;

    MYON_ArrayIterator< const MYON_BrepVertex > vit( m_V.Array(), m_V.UnsignedCount() );
    for ( const MYON_BrepVertex* p = vit.First(); nullptr != p; p = vit.Next())
    {
      a.Add(p->m_status);
    }

    MYON_ArrayIterator< const MYON_BrepEdge > eit( m_E.Array(), m_E.UnsignedCount() );
    for ( const MYON_BrepEdge* p = eit.First(); nullptr != p; p = eit.Next())
    {
      a.Add(p->m_status);
    }

    MYON_ArrayIterator< const MYON_BrepTrim > tit( m_T.Array(), m_T.UnsignedCount() );
    for ( const MYON_BrepTrim* p = tit.First(); nullptr != p; p = tit.Next())
    {
      a.Add(p->m_status);
    }

    MYON_ArrayIterator< const MYON_BrepLoop > lit( m_L.Array(), m_L.UnsignedCount() );
    for ( const MYON_BrepLoop* p = lit.First(); nullptr != p; p = lit.Next())
    {
      a.Add(p->m_status);
    }

    MYON_ArrayIterator< const MYON_BrepFace > fit( m_F.Array(), m_F.UnsignedCount() );
    for ( const MYON_BrepFace* p = fit.First(); nullptr != p; p = fit.Next())
    {
      a.Add(p->m_status);
    }

    m_aggregate_status = a;
  }

  return m_aggregate_status;
}

//virtual
void MYON_Brep::MarkAggregateComponentStatusAsNotCurrent() const
{
  this->m_aggregate_status.MarkAsNotCurrent();
}




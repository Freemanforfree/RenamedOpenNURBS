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

class MYON_OBSOLETE_V5_HatchExtra : public MYON_UserData
{
  //March 23, 2008 - LW
  //Adding MYON_OBSOLETE_V5_HatchExtra class to support movable base point for hatches
  //This should be combined with the MYON_Hatch class next time that is possible
  // Don't put this extension class in a header file or export it.

  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V5_HatchExtra);

public:
  MYON_OBSOLETE_V5_HatchExtra();
  ~MYON_OBSOLETE_V5_HatchExtra() = default;
  MYON_OBSOLETE_V5_HatchExtra(const MYON_OBSOLETE_V5_HatchExtra&) = default;
  MYON_OBSOLETE_V5_HatchExtra& operator=(const MYON_OBSOLETE_V5_HatchExtra&) = default;

public:
  static MYON_OBSOLETE_V5_HatchExtra* HatchExtension(const MYON_Hatch* pHatch);

  // override virtual MYON_Object::Dump function
  void Dump( MYON_TextLog& text_log ) const override;

  // override virtual MYON_Object::SizeOf function
  unsigned int SizeOf() const override;

  // override virtual MYON_Object::Write function
  bool Write(MYON_BinaryArchive& binary_archive) const override;

  // override virtual MYON_Object::Read function
  bool Read(MYON_BinaryArchive& binary_archive) override;

  // override virtual MYON_UserData::GetDescription function
  bool GetDescription( MYON_wString& description ) override;

  // override virtual MYON_UserData::Archive function
  bool Archive() const override; 

  bool WriteToArchive(
    const class MYON_BinaryArchive& archive,
    const class MYON_Object* parent_object
    ) const override;

  bool DeleteAfterWrite(
    const class MYON_BinaryArchive& archive,
    const class MYON_Object* parent_object
  ) const override;

  bool DeleteAfterRead(
    const class MYON_BinaryArchive& archive,
    class MYON_Object* parent_object
  ) const override;

  MYON_2dPoint m_basepoint = MYON_2dPoint::Origin;
};

MYON_OBJECT_IMPLEMENT(MYON_OBSOLETE_V5_HatchExtra,MYON_UserData,"3FF7007C-3D04-463f-84E3-132ACEB91062");

MYON_OBSOLETE_V5_HatchExtra* MYON_OBSOLETE_V5_HatchExtra::HatchExtension(const MYON_Hatch* pHatch)
{
  MYON_OBSOLETE_V5_HatchExtra* pExtra = nullptr;
  if(pHatch)
  {
    pExtra = MYON_OBSOLETE_V5_HatchExtra::Cast(pHatch->GetUserData(MYON_CLASS_ID(MYON_OBSOLETE_V5_HatchExtra)));
    if(pExtra == nullptr)
    {
      pExtra = new MYON_OBSOLETE_V5_HatchExtra;
      if(pExtra)
      {
        if(!const_cast<MYON_Hatch*>(pHatch)->AttachUserData(pExtra))
        {
          delete pExtra;
          pExtra = nullptr;
        }
      }  
    }
  }
  return pExtra;
}

MYON_OBSOLETE_V5_HatchExtra::MYON_OBSOLETE_V5_HatchExtra()
{
  m_userdata_uuid = MYON_CLASS_ID(MYON_OBSOLETE_V5_HatchExtra);
  m_application_uuid = MYON_opennurbs5_id; // opennurbs.dll reads/writes this userdata
                                         // The id must be the version 5 id because
                                         // V6 SaveAs V5 needs to work, but SaveAs
                                         // V4 should not write this userdata.   
  m_userdata_copycount = 0;
}

void MYON_OBSOLETE_V5_HatchExtra::Dump(MYON_TextLog& text_log) const
{
}

unsigned int MYON_OBSOLETE_V5_HatchExtra::SizeOf() const
{
  unsigned int sz = MYON_UserData::SizeOf();
  sz += sizeof(*this)-sizeof(MYON_UserData);
  return sz;
}

bool MYON_OBSOLETE_V5_HatchExtra::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);

  if(rc) rc = archive.WriteUuid( MYON_nil_uuid);
  if(rc) rc = archive.WritePoint(m_basepoint);

  if(!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_OBSOLETE_V5_HatchExtra::Read(MYON_BinaryArchive& archive)
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);

  if(major_version != 1)
      rc = false;

  m_basepoint.Set(0.0,0.0);
  MYON_UUID ignored_id = MYON_nil_uuid;
  if(rc) rc = archive.ReadUuid(ignored_id);
  if(rc) rc = archive.ReadPoint(m_basepoint);

  if(!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_OBSOLETE_V5_HatchExtra::GetDescription( MYON_wString& description)
{
  description = L"OBSOLETE V5 MYON_Hatch basepoint userdata.";
  return true;
}

bool MYON_OBSOLETE_V5_HatchExtra::Archive() const
{
  return true;
}

bool MYON_OBSOLETE_V5_HatchExtra::WriteToArchive(
  const class MYON_BinaryArchive& archive,
  const class MYON_Object* parent_object
  ) const
{
  return (50 == archive.Archive3dmVersion() && nullptr != MYON_Hatch::Cast(parent_object));
}


bool MYON_OBSOLETE_V5_HatchExtra::DeleteAfterWrite(
  const class MYON_BinaryArchive& archive,
  const class MYON_Object* parent_object
) const
{
  return true;
}

bool MYON_OBSOLETE_V5_HatchExtra::DeleteAfterRead(
  const class MYON_BinaryArchive& archive,
  class MYON_Object* parent_object
) const
{
  for (;;)
  {
    if (false == m_basepoint.IsValid())
      break;
    MYON_Hatch* hatch = MYON_Hatch::Cast(parent_object);
    if (nullptr == hatch)
      break;
    MYON_2dPoint p = hatch->BasePoint2d();
    if (p.x != m_basepoint.x || p.y != m_basepoint.y)
    {
      hatch->SetBasePoint(m_basepoint);
    }
    break;
  }
  return true;
}

/////////////////////////////////////////////////////////////////
//  class MYON_HatchLine
/////////////////////////////////////////////////////////////////

MYON_HatchLine::MYON_HatchLine(
  double angle_in_radians
  )
  : m_angle_radians( angle_in_radians)
{}

MYON_HatchLine::MYON_HatchLine(
  double angle_in_radians, 
  MYON_2dPoint base, 
  MYON_2dVector offset,
  const MYON_SimpleArray<double>& dashes
  )
  : m_angle_radians( angle_in_radians)
  , m_base( base)
  , m_offset( offset)
  , m_dashes( dashes)
{}

bool MYON_HatchLine::operator==(const MYON_HatchLine& src) const
{
  return(
    m_angle_radians == src.m_angle_radians
    && m_base == src.m_base
    && m_offset == src.m_offset
    && m_dashes == src.m_dashes
    );
}

bool MYON_HatchLine::operator!=(const MYON_HatchLine& src) const
{
  return(
    m_angle_radians != src.m_angle_radians
    || m_base != src.m_base
    || m_offset != src.m_offset
    || m_dashes != src.m_dashes
    );
}

bool MYON_HatchLine::IsValid( MYON_TextLog* text_log) const
{
  bool rc = m_angle_radians >= 0.0;
  if( !rc)
  {
    if( text_log)
      text_log->Print( "Angle ( %lf) must be >= 0.0\n", m_angle_radians);
    return false;
  }
  rc = m_angle_radians < MYON_PI * 2.0;
  if( !rc)
  {
    if( text_log)
      text_log->Print( "Angle ( %lf) must be < 2*Pi.\n", m_angle_radians);
    return false;
  }
  rc = m_base.IsValid();
  if( !rc)
  {
    if( text_log)
      text_log->Print( "Base is not a valid point.\n");
    return false;
  }
  rc = m_offset.IsValid();
  if( !rc)
  {
    if( text_log)
      text_log->Print( "Offset is not a valid vector.\n");
    return false;
  }
  return true;
}

void MYON_HatchLine::Dump( MYON_TextLog& dump) const
{
  dump.Print( "MYON_HatchLine: angle = %lf radians ( %lf degrees) ", 
    AngleRadians(), AngleDegrees());
  dump.Print( " base = ");
  dump.Print( m_base);
  dump.Print( " offset = ");
  dump.Print( m_offset);
  int count = m_dashes.Count();
  dump.Print( "\nDash count = %d: ", count);
  for( int i = 0; i < count; i++)
  {
    dump.Print( "%lf", Dash( i));
    if( i < count-1)
      dump.Print( ", ");
  }
  dump.Print( "\n");
}


bool MYON_HatchLine::Write(MYON_BinaryArchive& archive) const
{
  if (archive.Archive3dmVersion() < 60)
    return WriteV5(archive);

  const int major_version = 1;
  const int minor_version = 0;
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, major_version, minor_version))
    return false;
  bool rc = false;
  for (;;)
  {
    if (!archive.WriteDouble(m_angle_radians))
      break;
    if (!archive.WritePoint(m_base))
      break;
    if (!archive.WriteVector(m_offset))
      break;
    if (!archive.WriteArray(m_dashes))
      break;
    rc = true;
    break;
  }
  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

static bool Internal_UseHatchReadV5(
  MYON_BinaryArchive& archive
  )
{
  if (archive.Archive3dmVersion() < 60)
    return true;
  if (archive.Archive3dmVersion() > 60)
    return false;
  
  // There are 4 possible Feb 26 version numbers depending on the build system
  // archive.Archive3dmVersion() % 4 = (0=developer,1=windows build system,2 = mac build system, 3=unused)
  const unsigned int Feb_26_2016_opennurbs_version_number0 = 2348833956;
  const unsigned int Feb_26_2016_opennurbs_version_number3 = 2348833956+3; 

  const unsigned int archive_opennurbs_version_number = archive.ArchiveOpenNURBSVersion();

  if (archive_opennurbs_version_number < Feb_26_2016_opennurbs_version_number0)
    return true; // "old" opennurbs hatch code

  if (  archive_opennurbs_version_number >= Feb_26_2016_opennurbs_version_number0
    && archive_opennurbs_version_number <= Feb_26_2016_opennurbs_version_number3
    )
  {
    // new hatch IO code pushed to master branch on Feb 26, 2016.
    // THis clunky test has to be used on files writting with the Feb 26, 2016 version of opennurbs source code.
    MYON__UINT32 tcode = 0;
    MYON__INT64 big_value = 0;
    archive.PeekAt3dmBigChunkType(&tcode, &big_value);
    if (TCODE_ANONYMOUS_CHUNK == tcode)
      return false; // use "new" IO
    return true; // use "old" IO
  }

  // 6.0 or later file created with executable using opennurbs code on Feb 27, 2016 or later
  return false;
}

bool MYON_HatchLine::Read(MYON_BinaryArchive& archive)
{
  if ( Internal_UseHatchReadV5(archive) )
    return ReadV5(archive);

  *this = MYON_HatchLine::SolidHorizontal;
  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;
  bool rc = false;
  for (;;)
  {
    if (1 != major_version)
      break;
    if (!archive.ReadDouble(&m_angle_radians))
      break;
    if (!archive.ReadPoint(m_base))
      break;
    if (!archive.ReadVector(m_offset))
      break;
    if (!archive.ReadArray(m_dashes))
      break;
    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}


bool MYON_HatchLine::WriteV5( MYON_BinaryArchive& ar) const
{
  // Incorrectly designed chunk IO - never change version numbers.
  bool rc = ar.Write3dmChunkVersion(1,1);

  if (rc) rc = ar.WriteDouble( m_angle_radians);
  if (rc) rc = ar.WritePoint( m_base);
  if (rc) rc = ar.WriteVector( m_offset);
  if (rc) rc = ar.WriteArray( m_dashes);

  return rc;
}

bool MYON_HatchLine::ReadV5( MYON_BinaryArchive& ar)
{
  *this = MYON_HatchLine::SolidHorizontal;
  // Incorrectly designed chunk IO - never change version numbers.
  int major_version = 0;
  int minor_version = 0;
  bool rc = ar.Read3dmChunkVersion( &major_version, &minor_version);
  if (rc)
    rc = (1 == major_version);
  if ( rc )
  {
    if ( rc) rc = ar.ReadDouble( &m_angle_radians);
    if ( rc) rc = ar.ReadPoint( m_base);
    if ( rc) rc = ar.ReadVector( m_offset);
    if ( rc) rc = ar.ReadArray( m_dashes);
  }
  return rc;
}



int MYON_HatchLine::Compare(
  const MYON_HatchLine& a,
  const MYON_HatchLine& b
  )
{
  int rc = MYON_CompareDouble(a.m_angle_radians, b.m_angle_radians);
  if (rc) return rc;
  rc = MYON_2dPoint::Compare(a.m_base, b.m_base);
  if (rc) return rc;
  rc = MYON_2dVector::Compare(a.m_offset, b.m_offset);
  if (rc) return rc;
  size_t acount = (size_t)a.m_dashes.UnsignedCount();
  size_t bcount = (size_t)b.m_dashes.UnsignedCount();
  if (acount < bcount)
    return -1;
  if (acount > bcount)
    return 1;
  return MYON_CompareDoubleArray(acount, a.m_dashes.Array(), b.m_dashes.Array());
}

// MYON_HatchLine Interface
double MYON_HatchLine::AngleRadians() const
{
  return m_angle_radians;
}

double MYON_HatchLine::AngleDegrees() const
{
  if (0.0 <= m_angle_radians && m_angle_radians < 2.0*MYON_PI)
  {
    double angle_degrees = m_angle_radians*(180.0/MYON_PI);
    if (angle_degrees >= 360.0)
      angle_degrees = 0.0;
    return angle_degrees;
  }
  return m_angle_radians; // UNSET VALUE OR NaN
}

static double DealWithUnsetAngles(
  double a
  )
{
  if (((double)MYON_UNSET_FLOAT) == a)
    return MYON_UNSET_VALUE;
  
  if (((double)MYON_UNSET_POSITIVE_FLOAT) == a)
    return MYON_UNSET_POSITIVE_VALUE;
  
  if (a < MYON_UNSET_VALUE)
    return MYON_UNSET_VALUE;
  
  if (a > MYON_UNSET_POSITIVE_VALUE)
    return MYON_UNSET_POSITIVE_VALUE;

  return a;
}

void MYON_HatchLine::SetAngleRadians( double angle_radians)
{
  angle_radians = DealWithUnsetAngles(angle_radians);
  
  if (angle_radians > MYON_UNSET_VALUE && angle_radians < MYON_UNSET_POSITIVE_VALUE)
  {
    const double twopi = MYON_PI * 2.0;
    // clamp between [0  2pi)
    while (angle_radians < 0.0)
      angle_radians += twopi;
    while (angle_radians > twopi)
      angle_radians -= twopi;

    const double zero_angle_tol = MYON_ZERO_TOLERANCE*twopi;
    if (
      fabs(angle_radians) <= zero_angle_tol 
      || fabs(angle_radians - twopi) <= zero_angle_tol 
      )
      angle_radians = 0.0;
  }
  m_angle_radians = angle_radians;
}


void MYON_HatchLine::SetAngleDegrees(double angle_degrees)
{
  angle_degrees = DealWithUnsetAngles(angle_degrees);

  if (angle_degrees > MYON_UNSET_VALUE && angle_degrees < MYON_UNSET_POSITIVE_VALUE)
    SetAngleRadians(angle_degrees*(MYON_PI/180.0));
  else
    m_angle_radians = angle_degrees;
}

MYON_2dPoint MYON_HatchLine::Base() const
{
  return m_base;
}

void MYON_HatchLine::SetBase( const MYON_2dPoint& base)
{
  m_base = base;
}

MYON_2dVector MYON_HatchLine::Offset() const
{
  return m_offset;
}

void MYON_HatchLine::SetOffset( const MYON_2dVector& offset)
{
  m_offset = offset;
}

int MYON_HatchLine::DashCount() const
{
  return m_dashes.Count();
}

double MYON_HatchLine::Dash( int index) const
{
  if( index >= 0 && index < m_dashes.Count())
    return m_dashes[index];
  return 0.0;
}

void MYON_HatchLine::AppendDash( double dash)
{
//  if( fabs( dash) > MYON_SQRT_EPSILON)
    m_dashes.Append( dash);
}

void MYON_HatchLine::SetDashes( const MYON_SimpleArray<double>& dashes)
{
  m_dashes = dashes;
}

const MYON_SimpleArray<double>& MYON_HatchLine::Dashes() const
{
  return m_dashes;
}

void MYON_HatchLine::GetLineData( double& angle, 
                                MYON_2dPoint& base, 
                                MYON_2dVector& offset, 
                                MYON_SimpleArray<double>& dashes) const
{
  angle = m_angle_radians;
  base = m_base;
  offset = m_offset;  dashes = m_dashes;
}

double MYON_HatchLine::GetPatternLength() const
{
  int i;
  double length = 0.0;
  for( i = 0; i < m_dashes.Count(); i++)
    length += fabs( m_dashes[i]);

  return length;
}


//  class MYON_HatchPattern
/////////////////////////////////////////////////////////////////
MYON_OBJECT_IMPLEMENT( MYON_HatchPattern, MYON_ModelComponent, "064E7C91-35F6-4734-A446-79FF7CD659E1" );

MYON_HatchPattern::HatchFillType MYON_HatchPattern::HatchFillTypeFromUnsigned(
  unsigned hatch_fill_type_as_unsigned
  )
{
  switch (hatch_fill_type_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_HatchPattern::HatchFillType::Solid);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_HatchPattern::HatchFillType::Lines);
    //MYON_ENUM_FROM_UNSIGNED_CASE(MYON_HatchPattern::HatchFillType::Gradient);
  }
  MYON_ERROR("Invalid hatch_fill_type_as_unsigned value.");
  return MYON_HatchPattern::HatchFillType::Solid;
}

int MYON_HatchPattern::Compare(
  const MYON_HatchPattern& a,
  const MYON_HatchPattern& b
  )
{
  int rc = MYON_ModelComponent::CompareNameAndId(a, b);
  if (0 == rc)
    rc = CompareAppearance(a, b);
  if (0 == rc)
    rc = MYON_wString::CompareOrdinal(a.m_description, b.m_description, false);
  return rc;
}

int MYON_HatchPattern::CompareAppearance( 
  const MYON_HatchPattern& a,
  const MYON_HatchPattern& b
  )
{
  const unsigned int atype = static_cast<unsigned int>(a.FillType());
  const unsigned int btype = static_cast<unsigned int>(b.FillType());
  if (atype < btype)
    return -1;
  if (atype > btype)
    return 1;
  
  if (MYON_HatchPattern::HatchFillType::Lines != a.FillType())
    return 0;

  const unsigned acount = a.m_lines.UnsignedCount();
  const unsigned bcount = b.m_lines.UnsignedCount();
  if (acount < bcount)
    return -1;
  if (acount > bcount)
    return 1;

  for (unsigned int i = 0; i < acount; i++)
  {
    int rc = MYON_HatchLine::Compare(a.m_lines[i], b.m_lines[i]);
    if (0 != rc)
      return rc;
  }

  return 0;
}

const MYON_HatchPattern* MYON_HatchPattern::FromModelComponentRef(
  const class MYON_ModelComponentReference& model_component_reference,
  const MYON_HatchPattern* none_return_value
  )
{
  const MYON_HatchPattern* hatch_pattern = MYON_HatchPattern::Cast(model_component_reference.ModelComponent());
  return (nullptr != hatch_pattern) ? hatch_pattern : none_return_value;
}

MYON_HatchPattern::MYON_HatchPattern() MYON_NOEXCEPT
  : MYON_ModelComponent(MYON_ModelComponent::Type::HatchPattern)
{}

MYON_HatchPattern::MYON_HatchPattern(const MYON_HatchPattern& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::HatchPattern, src)
  , m_type(src.m_type)
  , m_description(src.m_description)
  , m_lines(src.m_lines)
{}

bool MYON_HatchPattern::IsValid( MYON_TextLog* text_log) const
{
  MYON_HatchPattern::HatchFillType type = FillType();
  bool rc = true;
  if( type != MYON_HatchPattern::HatchFillTypeFromUnsigned(static_cast<unsigned int>(type)))
  {
    if( text_log)
      text_log->Print( "Type field not set correctly.\n");
    rc = false;
  }
  if( type == MYON_HatchPattern::HatchFillType::Lines)
  {
    int count = m_lines.Count();
    if( count < 1)
    {
      if( text_log)
        text_log->Print( "Line type patetern with no lines.\n");
      return false;
    }
    for( int i = 0; i < count; i++)
    {
      if( !m_lines[i].IsValid())
      {
        if( text_log)
          text_log->Print( "Line[%d] is not valid.\n", i);
        return false;
      }
    }
    return true;
  }
  return rc;
}

void MYON_HatchPattern::Dump( MYON_TextLog& dump) const
{
  MYON_ModelComponent::Dump(dump);

  switch( m_type)
  {
  case MYON_HatchPattern::HatchFillType::Solid:
    dump.Print( "fill type: Solid");
    break;
  case MYON_HatchPattern::HatchFillType::Lines:
    dump.Print( "fill type: Lines");
    break;
  //case MYON_HatchPattern::HatchFillType::Gradient:
  //  dump.Print( "fill type: Gradient");
  //  break;
  }
  dump.Print( "\n");

  const wchar_t* wsDescription =  static_cast< const wchar_t* >(m_description);
  if ( 0 == wsDescription )
    wsDescription = L"";
  dump.Print( "Description: %ls\n", wsDescription);

  if( m_type == MYON_HatchPattern::HatchFillType::Lines)
  {
    int count = m_lines.Count();
    dump.Print( "Line count = %d\n", count);
    for( int i = 0; i < count; i++)
    {
      m_lines[i].Dump( dump);
    }
    dump.Print( "\n");
  }
}

bool MYON_HatchPattern::Write(MYON_BinaryArchive& archive ) const
{
  if (archive.Archive3dmVersion() < 60)
    return WriteV5(archive);

  const int major_version = 1;
  const int minor_version = 0;
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, major_version, minor_version))
    return false;
  bool rc = false;
  for (;;)
  {
    unsigned int attributes_filter
      = MYON_ModelComponent::Attributes::IdAttribute
      | MYON_ModelComponent::Attributes::IndexAttribute
      | MYON_ModelComponent::Attributes::NameAttribute;
    if (!WriteModelComponentAttributes(archive, attributes_filter))
      break;

    if (!archive.WriteInt(static_cast<unsigned int>(m_type)))
      break;

    if (!archive.WriteString(m_description))
      break;

    if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 0))
      break;
    {
      const unsigned int count = (m_type == MYON_HatchPattern::HatchFillType::Lines ? m_lines.UnsignedCount() : 0);
      bool lines_rc = archive.WriteInt(count);
      for (unsigned int i = 0; i < count && lines_rc; i++)
        lines_rc = m_lines[i].Write(archive);
      if (!archive.EndWrite3dmChunk())
        lines_rc = false;
      if (false == lines_rc)
        break;
    }

    rc = true;
    break;
  }
  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_HatchPattern::Read(MYON_BinaryArchive& archive)
{
  if ( Internal_UseHatchReadV5(archive) )
    return ReadV5(archive);

  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;
  bool rc = false;
  for (;;)
  {
    if (!ReadModelComponentAttributes(archive))
      break;

    unsigned int type_as_unsigned = 0;
    if (!archive.ReadInt(&type_as_unsigned))
      break;
    m_type = MYON_HatchPattern::HatchFillTypeFromUnsigned(type_as_unsigned);

    if (!archive.ReadString(m_description))
      break;

    unsigned int tcode = 0;
    MYON__INT64 value = 0;
    if (!archive.BeginRead3dmBigChunk(&tcode, &value))
      break;
    {
      bool lines_rc = (TCODE_ANONYMOUS_CHUNK == tcode && value > 0);
      unsigned int count = 0;
      lines_rc = archive.ReadInt(&count);
      m_lines.Reserve(count);
      for (unsigned int i = 0; i < count && lines_rc; i++)
        lines_rc = m_lines.AppendNew().Read(archive);
      if (!archive.EndRead3dmChunk())
        lines_rc = false;
      if (false == lines_rc)
        break;
    }

    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}

bool MYON_HatchPattern::WriteV5( MYON_BinaryArchive& ar) const
{
  bool rc = ar.Write3dmChunkVersion(1,2);

  if (rc) rc = ar.Write3dmReferencedComponentIndex( *this );
  if (rc) rc = ar.WriteInt( static_cast<unsigned int>(m_type));
  MYON_wString name = Name();
  if (name.IsEmpty() && ModelComponentStatus().IsDeleted())
    name = DeletedName();
  if (rc) rc = ar.WriteString(name );
  if (rc) rc = ar.WriteString( m_description);
  if( rc)
  {
    if( m_type == MYON_HatchPattern::HatchFillType::Lines)
    {
      int i, count = m_lines.Count();
      if ( count < 0 )
        count = 0;
      rc = ar.WriteInt( count );
      for( i = 0; i < count && rc; i++)
        rc = m_lines[i].Write( ar);
    }
  }
  // version 1.2 field
  if (rc) rc = ar.WriteUuid(Id());

  return rc;
}

bool MYON_HatchPattern::ReadV5( MYON_BinaryArchive& ar)
{
  *this = MYON_HatchPattern::Unset;

  int major_version = 0;
  int minor_version = 0;
  bool rc = ar.Read3dmChunkVersion( &major_version, &minor_version);
  if (rc && 1 != major_version)
    rc = false;
  if ( rc ) 
  {
    int hatchpattern_index = Index();
    if (rc) rc = ar.ReadInt(&hatchpattern_index);
    if (rc) SetIndex(hatchpattern_index);

    int fill_type_as_unsigned = 0;
    if( rc) rc = ar.ReadInt( &fill_type_as_unsigned);
    if( rc)  m_type = MYON_HatchPattern::HatchFillTypeFromUnsigned(fill_type_as_unsigned);
    MYON_wString hatchpattern_name;
    if( rc) rc = ar.ReadString( hatchpattern_name);
    if (rc) SetName(hatchpattern_name);
    if( rc) rc = ar.ReadString( m_description);
    if( rc)
    {
      if( m_type == MYON_HatchPattern::HatchFillType::Lines)
      {
        m_lines.Empty();
        int count = 0;
        rc = ar.ReadInt( &count);
        if( rc && count > 0 ) 
        {
          m_lines.SetCapacity( count);
          for(int li = 0; rc && li < count; li++)
          {
            MYON_HatchLine& line = m_lines.AppendNew();
            rc = line.Read( ar);
          }
        }
      }
    }
    if ( minor_version >= 2 )
    {
      MYON_UUID hatchpattern_id = MYON_nil_uuid;
      rc = ar.ReadUuid(hatchpattern_id);
      if (rc) SetId(hatchpattern_id);
    }
  }
  if (rc && IdIsNil())
    SetId(); // modern hatch patterns require a uniquie id.
  return rc;
}

MYON_HatchPattern::MYON_HatchPattern::HatchFillType MYON_HatchPattern::FillType() const
{
  return m_type;
}

void MYON_HatchPattern::SetFillType( MYON_HatchPattern::HatchFillType type)
{
  if ( m_type != type && type == MYON_HatchPattern::HatchFillTypeFromUnsigned(static_cast<unsigned int>(type)) )
  {
    if (MYON_HatchPattern::HatchFillType::Lines != type)
      m_lines.Destroy();
    m_type = type;
    IncrementContentVersionNumber();
  }
}

void MYON_HatchPattern::SetDescription( const wchar_t* pDescription)
{
  MYON_wString s(pDescription);
  s.TrimLeftAndRight();
  if (s != m_description)
  {
    m_description = s;
    IncrementContentVersionNumber();
  }
}


const MYON_wString& MYON_HatchPattern::Description() const
{
  return m_description;
}

//  Line HatchPattern functions

int MYON_HatchPattern::HatchLineCount() const
{
  return m_lines.Count();
}

int MYON_HatchPattern::AddHatchLine( const MYON_HatchLine& line)
{
  m_lines.Append( line);
  IncrementContentVersionNumber();
  return m_lines.Count()-1;
}

const MYON_HatchLine* MYON_HatchPattern::HatchLine( int index) const
{
  if( index >= 0 && index < m_lines.Count())
    return &m_lines[index];
  else
    return nullptr;
}

bool MYON_HatchPattern::RemoveHatchLine( int index)
{
  if( index >= 0 && index < m_lines.Count())
  {
    m_lines.Remove( index);
    IncrementContentVersionNumber();
    return true;
  }
  return false;
}

void MYON_HatchPattern::RemoveAllHatchLines()
{
  if ( m_lines.UnsignedCount() > 0 )
    IncrementContentVersionNumber();
  m_lines.Empty();
}

int MYON_HatchPattern::SetHatchLines( const MYON_ClassArray<MYON_HatchLine>& lines)
{
  return SetHatchLines(
    lines.UnsignedCount(),
    lines.Array()
    );
}

int MYON_HatchPattern::SetHatchLines(
  size_t count,
  const MYON_HatchLine* lines
  )
{

  if (count > 0 && nullptr != lines )
  {
    m_lines.SetCount(0);
    m_lines.Append((int)count, lines);
    IncrementContentVersionNumber();
    SetFillType(MYON_HatchPattern::HatchFillType::Lines);
  }
  else
  {
    if (0 != m_lines.UnsignedCount())
      IncrementContentVersionNumber();
    m_lines.Destroy();
    if ( MYON_HatchPattern::HatchFillType::Lines == FillType())
      SetFillType(MYON_HatchPattern::HatchFillType::Solid);
  }
  return m_lines.Count();
}

const MYON_ClassArray<MYON_HatchLine>& MYON_HatchPattern::HatchLines() const
{
  return m_lines;
}


//  class MYON_HatchLoop
/////////////////////////////////////////////////////////////////

#if defined(OPENNURBS_EXPORTS)

// When the Microsoft CRT(s) is/are used, this is the best
// way to prevent crashes that happen when a hatch loop is
// allocated with new in one DLL and deallocated with
// delete in another DLL.

void* MYON_HatchLoop::operator new(size_t sz)
{
  // MYON_HatchLoop new
  return onmalloc(sz);
}

void MYON_HatchLoop::operator delete(void* p)
{
  // MYON_HatchLoop delete
  onfree(p);
}

void* MYON_HatchLoop::operator new[] (size_t sz)
{
  // MYON_HatchLoop array new
  return onmalloc(sz);
}

void MYON_HatchLoop::operator delete[] (void* p)
{
  // MYON_HatchLoop array delete
  onfree(p);
}

void* MYON_HatchLoop::operator new(size_t, void* p)
{
  // MYON_HatchLoop placement new
  return p;
}

void MYON_HatchLoop::operator delete(void*, void*)
{
  // MYON_HatchLoop placement delete
  return;
}

#endif


MYON_HatchLoop::MYON_HatchLoop()
: m_type( MYON_HatchLoop::ltOuter), m_p2dCurve( nullptr)
{
}

MYON_HatchLoop::MYON_HatchLoop( MYON_Curve* pCurve2d, eLoopType type)
: m_type( type), m_p2dCurve( pCurve2d)
{
}

MYON_HatchLoop::MYON_HatchLoop( const MYON_HatchLoop& src)
: m_type( src.m_type), m_p2dCurve( nullptr)
{ 
  if( src.m_p2dCurve)
    m_p2dCurve = src.m_p2dCurve->DuplicateCurve();
}

MYON_HatchLoop::~MYON_HatchLoop()
{
  delete m_p2dCurve;
}

MYON_HatchLoop& MYON_HatchLoop::operator=( const MYON_HatchLoop& src)
{
  if( this != &src)
  {
    if( m_p2dCurve)
      delete m_p2dCurve;
    m_p2dCurve = src.m_p2dCurve->DuplicateCurve();

    m_type = src.m_type;
  }
  return *this;
}

bool MYON_HatchLoop::IsValid( MYON_TextLog* text_log) const
{
  bool rc = m_p2dCurve != nullptr;
  if( !rc)
  {
    if( text_log)
      text_log->Print( "2d loop curve is nullptr\n");
  }
  if( rc)
  {
    rc = m_p2dCurve->IsValid( text_log);
    if( !rc)
    {
      if( text_log)
        text_log->Print( "Loop curve is not valid\n");
    }
  }

  if(rc)
  {
  }

  if( rc)
  {
    MYON_BoundingBox box;
    m_p2dCurve->GetBoundingBox( box);
    rc = ( box.Max().z == box.Min().z && box.Max().z == 0.0);
    if( !rc)
    {
      if( text_log)
        text_log->Print( "2d loop curve has non-zero z coordinates\n");
    }
  }

  if( rc && m_type != ltOuter && m_type != ltInner)
  {
    if( text_log)
      text_log->Print( "Loop type is invalid.\n");
    rc = false;
  }

  return rc;
}

void MYON_HatchLoop::Dump( MYON_TextLog& dump) const
{
  if( m_type == ltOuter)
    dump.Print( "Outer hatch loop\n");
  if( m_type == ltInner)
    dump.Print( "Inner hatch loop\n");

  if ( 0 == m_p2dCurve )
  {
    dump.Print( "2d curve: null pointer\n");
  }
  else
  {
    dump.Print( "2d curve:\n");
    m_p2dCurve->Dump(dump);
  }

}

bool MYON_HatchLoop::Write( MYON_BinaryArchive& ar) const
{
  bool rc = ar.Write3dmChunkVersion(1,1);
  if( rc) rc = ar.WriteInt( m_type);
  if( rc) rc = ar.WriteObject( m_p2dCurve);
  return rc;
}

bool MYON_HatchLoop::Read( MYON_BinaryArchive& ar)
{
  m_type = ltOuter;
  delete m_p2dCurve;
  m_p2dCurve = nullptr;
  int major_version = 0;
  int minor_version = 0;
  bool rc = ar.Read3dmChunkVersion( &major_version, &minor_version);
  if ( major_version == 1 ) 
  {
    int type = 0;
    if( rc) rc = ar.ReadInt( &type);
    if( rc) 
    {
      switch( type)
      {
      case ltOuter:  m_type = ltOuter; break;
      case ltInner:  m_type = ltInner; break;
      default: rc = false; break;
      }
    }
    if( rc)
    {
      MYON_Object* pObj = nullptr;
      rc = ar.ReadObject( &pObj);
      if( pObj)
      {
        m_p2dCurve = MYON_Curve::Cast( pObj);
        if( !m_p2dCurve) // read something, but it wasn't right
        {
          rc = false;
          delete pObj;
        }
      }
    }
  }
  return rc;
}

const MYON_Curve* MYON_HatchLoop::Curve() const
{
  return m_p2dCurve;
}

bool MYON_HatchLoop::SetCurve( const MYON_Curve& curve)
{
  MYON_Curve* pC = curve.DuplicateCurve();
  if( pC)
  {
    if(pC->Dimension() == 3 && !pC->ChangeDimension(2))
      return false;

    if( m_p2dCurve)
      delete m_p2dCurve;
    m_p2dCurve = pC;
  }
  return true;
}
MYON_HatchLoop::eLoopType MYON_HatchLoop::Type() const
{
  return m_type;
}

void MYON_HatchLoop::SetType( eLoopType type)
{
  m_type = type;
}

//  class MYON_Hatch
/////////////////////////////////////////////////////////////////
MYON_OBJECT_IMPLEMENT( MYON_Hatch, MYON_Geometry, "0559733B-5332-49d1-A936-0532AC76ADE5");

MYON_Hatch::MYON_Hatch( const MYON_Hatch& src)
  : MYON_Geometry(src)
{
  Internal_CopyFrom(src);
}

MYON_Hatch& MYON_Hatch::operator=( const MYON_Hatch& src)
{
  if( this != &src)
  {
    Internal_Destroy();
    MYON_Geometry::operator =(src);
    Internal_CopyFrom(src);
  }
  return *this;
}

MYON_Hatch::~MYON_Hatch()
{
  Internal_Destroy();
}

void MYON_Hatch::Internal_Destroy()
{
  const int count = m_loops.Count();
  for ( int i = 0; i < count; i++ )
  {
    MYON_HatchLoop* pL = m_loops[i];
    if ( pL )
    {
      m_loops[i] = nullptr;
      delete pL;
    }
  }
  m_loops.Destroy();
}

void MYON_Hatch::Internal_CopyFrom(const MYON_Hatch& src)
{
  m_plane = src.m_plane;
  m_pattern_scale = src.m_pattern_scale;
  m_pattern_rotation = src.m_pattern_rotation;
  m_basepoint = src.m_basepoint;
  const int count = src.m_loops.Count();
  m_loops.SetCount(0);
  m_loops.Reserve(count);
  for( int i = 0; i < count; i++)
  {
    MYON_HatchLoop* pL = new MYON_HatchLoop( *src.m_loops[i]);
    m_loops.Append( pL);
  }
  m_pattern_index = src.m_pattern_index;
}

MYON_Hatch* MYON_Hatch::DuplicateHatch() const
{
  return Duplicate();
}


MYON_Hatch* MYON_Hatch::HatchFromBrep(
  MYON_Hatch* use_this_hatch,
  const MYON_Brep* brep,
  int face_index,
  int pattern_index,
  double pattern_rotation_radians,
  double pattern_scale,
  MYON_3dPoint basepoint)
{
  MYON_Hatch* newhatch = nullptr;
  if (nullptr != use_this_hatch)
    use_this_hatch->Internal_Destroy();

  if (nullptr == brep)
    return nullptr;

  if (face_index < 0 || face_index >= brep->m_F.Count())
    face_index = 0;

  const MYON_BrepFace* face = brep->Face(face_index);
  if (nullptr == face)
    return nullptr;

  MYON_Plane plane;
  if (!face->IsPlanar(&plane))
    return nullptr;

  if (nullptr == use_this_hatch)
    newhatch = new MYON_Hatch();
  else
  {
    newhatch = use_this_hatch;
    newhatch->Internal_Destroy();
  }

  if (0 > pattern_index)
    pattern_index = 0;

  if (pattern_scale < MYON_SQRT_EPSILON)
    pattern_scale = 1.0;

  if (MYON_3dPoint::UnsetPoint == basepoint)
    basepoint = MYON_3dPoint::Origin;

  newhatch->SetPlane(plane);
  newhatch->SetPatternIndex(pattern_index);
  newhatch->SetPatternRotation(pattern_rotation_radians);
  newhatch->SetPatternScale(pattern_scale);
  newhatch->SetBasePoint(basepoint);

  bool rc = false;
  int loopcount = face->LoopCount();
  for (int li = 0; li < loopcount; li++)
  {
    MYON_Curve* loopcurve = nullptr;
    MYON_SimpleArray< MYON_Curve* > edgecurves;
    MYON_BrepLoop* loop = face->Loop(li);
    if (nullptr != loop)
    {
      int trimcount = loop->TrimCount();
      for (int ti = 0; ti < trimcount; ti++)
      {
        MYON_BrepTrim* trim = loop->Trim(ti);
        if (nullptr != trim)
        {
          const MYON_Curve* edgecurve = trim->EdgeCurveOf();
          if (nullptr != edgecurve)
          {
            MYON_Curve* ec = edgecurve->Duplicate();
            if (trim->m_bRev3d)
              ec->Reverse();
            edgecurves.Append(ec);
          }
        }
      }

      int edgecount = edgecurves.Count();
      if (edgecount == 1)
      {
        loopcurve = edgecurves[0];
      }
      else if (edgecount > 1)
      {
        MYON_PolyCurve* pc = new MYON_PolyCurve(edgecount);
        if (nullptr != pc)
        {
          for (int ei = 0; ei < edgecount; ei++)
          {
            MYON_Curve* ec = edgecurves[ei];
            if (nullptr != ec)
              pc->AppendAndMatch(ec);
          }
          loopcurve = pc;
        }
      }
      if (nullptr != loopcurve)
      {
        MYON_Xform xf;
        xf.ChangeBasis(MYON_xy_plane, plane);
        loopcurve->Transform(xf);
        MYON_HatchLoop* hloop = new MYON_HatchLoop();
        hloop->SetCurve(*loopcurve);
        if(loop->m_type == MYON_BrepLoop::TYPE::outer)
        {
          hloop->SetType(MYON_HatchLoop::eLoopType::ltOuter);
          rc = true;
        }
        else
          hloop->SetType(MYON_HatchLoop::eLoopType::ltInner);
        newhatch->AddLoop(hloop);
      }
    }
  }
  if (!rc)
  {
    if (nullptr != newhatch)
    {
      delete newhatch;
      newhatch = nullptr;
    }
  }

  return newhatch;
}


bool MYON_Hatch::IsValid( MYON_TextLog* text_log) const
{
  bool rc = m_plane.IsValid();
  if( !rc)
  {
    if( text_log)
      text_log->Print( "Plane is not valid\n");
    return false;
  }
  // 18 June 2012 - Lowell - Added loop self-intersection and 
  // intersecting other loops tests
  int count = m_loops.Count();
  for(int i = 0; i < count; i++)
  {
    if(m_loops[i] == 0)
    {
      if( text_log)
        text_log->Print( "Loop[%d] is nullptr\n", i);
      return false;
    }
    if(rc)
      rc = m_loops[i]->IsValid( text_log);
    if( !rc)
    {
      if( text_log)
        text_log->Print( "Loop[%d] is not valid\n", i);
      return false;
    }
  }
  

  return true;
}

void MYON_Hatch::Dump( MYON_TextLog& dump) const
{
  dump.Print("Hatch: Pattern index: %d\n", PatternIndex());
  dump.Print("Pattern rotation: %g\n", PatternRotation());
  dump.Print("Pattern scale: %g\n", PatternScale());
  MYON_3dPoint p = this->BasePoint();
  dump.Print("Base point: %g, %g, %g\n", p.x, p.y, p.z);
  dump.Print("2d base point: %g, %g\n", m_basepoint.x, m_basepoint.y);
  dump.Print("Plane origin: %g, %g, %g\n", m_plane.origin.x, m_plane.origin.y, m_plane.origin.z);
  dump.Print("Plane x axis: %g, %g, %g\n", m_plane.xaxis.x, m_plane.xaxis.y, m_plane.xaxis.z);
  dump.Print("Plane y axis: %g, %g, %g\n", m_plane.yaxis.x, m_plane.yaxis.y, m_plane.yaxis.z);
  dump.Print("Plane z axis: %g, %g, %g\n", m_plane.zaxis.x, m_plane.zaxis.y, m_plane.zaxis.z);
  int count = m_loops.Count();
  dump.Print("Loop count = %d\n", count);
  for( int i = 0; i < count; i++)
    m_loops[i]->Dump( dump);
}

bool MYON_Hatch::Write( MYON_BinaryArchive& ar) const
{
  // Added basepoint to 1.2;
  const int minor_version = (ar.Archive3dmVersion() >= 60) ? 2 : 1;
  bool rc = ar.Write3dmChunkVersion(1,minor_version);
  if (rc) rc = ar.WritePlane( m_plane);
  if (rc) rc = ar.WriteDouble( m_pattern_scale);
  if (rc) rc = ar.WriteDouble( m_pattern_rotation);
  if (rc) rc = ar.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::HatchPattern, m_pattern_index);
  if (rc)
  {
    int i, count = m_loops.Count();
    if( count < 0 )
      count = 0;
    rc = ar.WriteInt( count);
    for( i = 0; i < count && rc; i++)
      rc = m_loops[i]->Write( ar);
  }
  const MYON_2dPoint basepoint = BasePoint2d();
  if (minor_version >= 2)
  {
    // m_basepoint = a 2d (x,y) point.
    if (rc) rc = ar.WritePoint(basepoint);
  }
  else if (
    basepoint.IsValid() 
    && false == basepoint.IsZero()
    && 50 == ar.Archive3dmVersion()
    )
  {
    // add temporary V5 user data cache
    MYON_OBSOLETE_V5_HatchExtra* v5_ud = MYON_OBSOLETE_V5_HatchExtra::HatchExtension(this);
    if( nullptr!= v5_ud)
      v5_ud->m_basepoint = basepoint;
  }
  return rc;
}

bool MYON_Hatch::Read( MYON_BinaryArchive& ar)
{
  m_plane.CreateFromNormal( MYON_3dPoint::Origin, MYON_3dVector::ZAxis);
  m_pattern_scale = 1.0;
  m_pattern_rotation = 0.0;
  m_pattern_index = -1;
  m_loops.Empty();
  int major_version = 0;
  int minor_version = 0;
  bool rc = ar.Read3dmChunkVersion( &major_version, &minor_version);
  if ( rc && major_version == 1 ) 
  {
    if( rc) rc = ar.ReadPlane( m_plane);
    if( rc) rc = ar.ReadDouble( &m_pattern_scale);
    if( rc) rc = ar.ReadDouble( &m_pattern_rotation);
    if( rc) rc = ar.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::HatchPattern,&m_pattern_index);
    if( rc)
    {
      m_loops.Empty();
      int i, count = 0;
      rc = ar.ReadInt( &count);
      if( rc && count > 0)
      {
        m_loops.SetCapacity( count );
        for( i = 0; rc && i < count; i++)
        {
          MYON_HatchLoop*& pLoop = m_loops.AppendNew();
          pLoop = new MYON_HatchLoop;
          if( pLoop)
            rc = pLoop->Read( ar);
          else
            rc = false;
        }
      }
    }
    if (minor_version >= 2)
    {
      MYON_2dPoint basepoint = BasePoint2d();
      if (rc) rc = ar.ReadPoint(basepoint);
      if (rc)
        SetBasePoint(basepoint);
    }
  }
  return rc;
}

MYON::object_type MYON_Hatch::ObjectType() const
{
  return MYON::hatch_object;
}

int MYON_Hatch::Dimension() const
{
  return 3;
}

// Copy the 2d curve, make it 3d, and transform it 
// to the 3d plane position
MYON_Curve* MYON_Hatch::LoopCurve3d( int index) const
{
  int count = m_loops.Count();
  MYON_Curve* pC = nullptr;

  if( index >= 0 && index < count)
  {
    if( m_loops[index]->Curve())
    {
      pC = m_loops[index]->Curve()->DuplicateCurve();
      if( pC)
      {
        pC->ChangeDimension( 3);

        MYON_Xform xf;
        xf.Rotation( MYON_xy_plane, m_plane);

        pC->Transform( xf);
      }
    }
  }
  return pC;
}


int MYON_Hatch::PatternIndex() const
{
  return m_pattern_index;
}

void MYON_Hatch::SetPatternIndex( int index)
{
  m_pattern_index = index;
}


bool MYON_Hatch::GetBBox( double* bmin, double* bmax, bool bGrowBox) const
{
  int i;
  int count = m_loops.Count();
  bool rc = true;
  MYON_Curve* pC;
  for( i = 0; rc && i < count; i++)
  {
    pC = LoopCurve3d( i);
    if( pC)
    {
      rc = pC->GetBBox( bmin, bmax, i?true:bGrowBox);
      delete pC;
    }
  }
  return rc;
}

bool MYON_Hatch::GetTightBoundingBox( MYON_BoundingBox& tight_bbox, bool bGrowBox, const MYON_Xform* xform) const
{
  int i;
  int count = m_loops.Count();
  MYON_CurveArray curves(count);
  for( i = 0; i < count; i++)
  {
    curves.Append( LoopCurve3d(i) );
  }
  return curves.GetTightBoundingBox(tight_bbox,bGrowBox,xform);
}

static double Angle3d(const MYON_3dVector& axis, MYON_3dVector& from, const MYON_3dVector& to)
{
  MYON_3dVector x = from, a = to;
  x.Unitize();
  a.Unitize();

  MYON_3dVector y = MYON_CrossProduct(axis, from);
  y.Unitize();

  double cosa = x * a;

  if(cosa > 1.0 - MYON_SQRT_EPSILON)
    return 0.0;
  if(cosa < MYON_SQRT_EPSILON - 1.0)
    return MYON_PI;

  double sina = a * y;

  return atan2(sina, cosa);
}


#define ARBBOUND  0.015625
void arbaxis(const MYON_3dVector& givenaxis, MYON_3dVector& newaxis)
{
  if(fabs(givenaxis[0]) < ARBBOUND && fabs(givenaxis[1]) < ARBBOUND) // near world z
    newaxis = MYON_CrossProduct(MYON_3dVector::YAxis, givenaxis);
  else
    newaxis = MYON_CrossProduct(MYON_3dVector::ZAxis, givenaxis);

  newaxis.Unitize();
}

double arbaxisRotation(const MYON_Plane& plane)
{
  // get arbaxis frame and angle of rotation from it
  MYON_3dVector arbXaxis;
  arbaxis(plane.zaxis, arbXaxis);
  return Angle3d(plane.zaxis, arbXaxis, plane.xaxis);
}

// 20 June 2012 - Lowell - rr44706, 68320
// This will find A, the arbaxis direction for the hatch plane
// and rotate the hatch plane by -A and rotate the hatch boundaries
// by A and add A to the hatch rotation.
// The picture will be the same after that, but the way the
// angle is represented will match the way AutoCAD does it
// so hatches can be round-tripped with acad files.
// In addition, after several hatches are rotated by different amounts
// the hatch angles can be set to look the same by setting them all
// to the same pattern rotation
 
static void UnrotateHatch(MYON_Hatch* hatch)
{
  double a = arbaxisRotation(hatch->Plane());

  MYON_Plane& plane = *(MYON_Plane*)(&hatch->Plane());
  if(fabs(a) > MYON_ZERO_TOLERANCE)
  {
    MYON_2dPoint base2 = hatch->BasePoint2d();
    plane.Rotate(-a, plane.zaxis);
    for(int i = 0; i < hatch->LoopCount(); i++)
    {
      MYON_Curve* pC = (MYON_Curve*)hatch->Loop(i)->Curve();
      pC->Rotate(a, MYON_3dVector::ZAxis, MYON_3dPoint::Origin);
    }
    base2.Rotate(a, MYON_2dPoint::Origin);
    hatch->SetBasePoint(base2);

    hatch->SetPatternRotation(hatch->PatternRotation()+a);
  }
  // Project world origin to hatch plane and set hatch plane origin to the result
  // Translate hatch 2d curves to get back to the right position
  MYON_3dPoint P;
  plane.ClosestPointTo(MYON_3dPoint::Origin, &P.x, &P.y);

  if(fabs(P.x) > MYON_ZERO_TOLERANCE ||fabs(P.y) > MYON_ZERO_TOLERANCE ||fabs(P.z) > MYON_ZERO_TOLERANCE)
  {
    const MYON_3dVector V(-P.x, -P.y, 0.0);
    for(int i = 0; i < hatch->LoopCount(); i++)
    {
      MYON_Curve* pC = (MYON_Curve*)hatch->Loop(i)->Curve();
      pC->Translate(V);
    }

    MYON_2dPoint base2 = hatch->BasePoint2d();
    base2 = base2 + MYON_2dVector(-P.x, -P.y);
    hatch->SetBasePoint(base2);

    P = plane.PointAt(P.x, P.y);
    plane.origin = P;
  }
}
  
bool MYON_Hatch::Transform( const MYON_Xform& xform)
{
  if( fabs( fabs( xform.Determinant()) - 1.0) > 1.0e-4)
  {
    // xform has a scale component
    MYON_Plane tmp( m_plane);
    tmp.Transform( xform);
    MYON_Xform A, B, T;
    A.Rotation( MYON_xy_plane, m_plane);
    B.Rotation( tmp, MYON_xy_plane);
    T = B * xform * A;

    // kill translation and z-scaling
    T[0][2] = T[0][3] = 0.0;
    T[1][2] = T[1][3] = 0.0;
    T[2][0] = T[2][1] = 0.0; T[2][2] = 1.0; T[2][3] = 0.0; 
    T[3][0] = T[3][1] = T[3][2] = 0.0; T[3][3] = 1.0;

    for( int i = 0; i < LoopCount(); i++)
      m_loops[i]->m_p2dCurve->Transform( T);
  }
  
  MYON_3dPoint base = m_plane.PointAt(m_basepoint.x, m_basepoint.y);
  base.Transform(xform);
  int rc = m_plane.Transform(xform);
  SetBasePoint(base);

  //MYON_3dVector x = m_plane.xaxis;
  //x.Transform(xform);
  //double scale = x.Length() * PatternScale();
  //SetPatternScale(scale);

  UnrotateHatch(this);

  TransformUserData(xform);

  return rc;
}

bool MYON_Hatch::ScalePattern(MYON_Xform xform)
{
  MYON_3dVector v = m_plane.xaxis;
  v.Transform(xform);

  double l = v.Length();
  if (v.Unitize())
  {
    m_pattern_scale *= l;
    return true;
  }
  return false;
}

MYON_Brep* MYON_Hatch::BrepForm(MYON_Brep* brep) const
{
  if (brep)
    brep->Destroy();

  MYON_Brep* newbrep = brep ? brep : MYON_Brep::New();

  if (0 == newbrep)
  {
    MYON_ERROR("Unable to get allocate brep.");
    return 0;
  }
  brep = newbrep;

  MYON_Plane plane = Plane();
  MYON_PlaneSurface* srf = new MYON_PlaneSurface(plane);
  int srf_i = -1, face_i = -1;

  int loopcount = LoopCount();
  for (int i = 0; i < loopcount; i++)
  {
    const MYON_HatchLoop* loop = Loop(i);
    const MYON_Curve* loopcurve = loop->Curve();
    MYON_Curve* edgecurve = this->LoopCurve3d(i);
    MYON_SimpleArray< MYON_Curve* > bounds;
    bounds.Append(edgecurve);

    if (i == 0)
    {
      MYON_BoundingBox bbox;
      loopcurve->GetBBox(&bbox.m_min.x, &bbox.m_max.x, false);
      srf->SetExtents(0, MYON_Interval(bbox.m_min.x - 1.0, bbox.m_max.x + 1.0), true);
      srf->SetExtents(1, MYON_Interval(bbox.m_min.y - 1.0, bbox.m_max.y + 1.0), true);
      srf_i = brep->AddSurface(srf);
      MYON_BrepFace& face = brep->NewFace(srf_i);
      face_i = face.m_face_index;
      brep->NewPlanarFaceLoop(face_i, MYON_BrepLoop::TYPE::outer, bounds, false);
    }
    else
      brep->NewPlanarFaceLoop(face_i, MYON_BrepLoop::TYPE::inner, bounds, false);
  }
  brep->SetTolerancesBoxesAndFlags();

  return brep;
}


bool MYON_Hatch::Create( const MYON_Plane& plane,
                       const MYON_SimpleArray<const MYON_Curve*> loops, 
                       int pattern_index, 
                       double pattern_rotation, 
                       double pattern_scale)
{
  if( loops.Count() < 1)
    return false;
  if( pattern_index < 0)
    return false;
  SetPlane( plane);
  for( int i = 0; i < loops.Count(); i++)
  {
    MYON_HatchLoop* pLoop = new MYON_HatchLoop;
    pLoop->SetCurve( *loops[i]);
    pLoop->SetType( i?MYON_HatchLoop::ltInner:MYON_HatchLoop::ltOuter);
    AddLoop( pLoop);
  }
  SetPatternIndex( pattern_index);
  SetPatternRotation( pattern_rotation);
  SetPatternScale( pattern_scale);
  return true;
}

const MYON_Plane& MYON_Hatch::Plane() const
{
  return m_plane;
}

void MYON_Hatch::SetPlane( const MYON_Plane& plane)
{
  m_plane = plane;
}

double MYON_Hatch::PatternRotation() const
{
  return m_pattern_rotation;
}

void MYON_Hatch::SetPatternRotation( double rotation)
{
  m_pattern_rotation = rotation;
}

double MYON_Hatch::PatternScale() const
{
  return m_pattern_scale;
}

void MYON_Hatch::SetPatternScale( double scale)
{
  if( scale > 0.001) // Changed May 13, 2009 - Lowell - rr39185
    m_pattern_scale = scale;
}

int MYON_Hatch::LoopCount() const
{
  return m_loops.Count();
}

void MYON_Hatch::AddLoop( MYON_HatchLoop* pLoop)
{
  m_loops.Append( pLoop);
}

bool MYON_Hatch::InsertLoop( int index, MYON_HatchLoop* loop)
{
  if( index >= 0 && index <= m_loops.Count()) // 26 June 2012 - Lowell - Changed ndex < to ndex <= 
  {
    m_loops.Insert(index, loop);
	return true;
  }

  return false;
}

bool MYON_Hatch::RemoveLoop( int index)
{
  if( index >= 0 && index < m_loops.Count())
  {
    delete m_loops[index];
    m_loops.Remove(index);
    return true;
  }
  
  return false;
}


bool MYON_Hatch::ReplaceLoops(MYON_SimpleArray<const MYON_Curve*>& loop_curves)
{
  if(loop_curves.Count() < 1)
    return false;

  bool rc = true;
  MYON_Xform xf;
  bool flat = false;
  MYON_SimpleArray<MYON_HatchLoop*> loops;

  for(int i = 0; i < loop_curves.Count(); i++)
  {
    if(loop_curves[i] == 0)
    {
      rc = false;
      break;
    }
    MYON_Curve* p2d = loop_curves[i]->Duplicate();
    if(p2d == 0)
    {
      rc = false;
      break;
    }
    if(p2d->Dimension() == 3)
    {
      if(!flat)
      {
        MYON_Xform planexf, flatxf;
        MYON_Plane hplane(MYON_xy_plane);
        hplane.origin = m_plane.origin;
        planexf.Rotation(m_plane, hplane);
        flatxf.PlanarProjection(hplane);
        xf = flatxf * planexf;
        flat = true;
      }
      if(!p2d->Transform(xf) ||
         !p2d->ChangeDimension(2))
      {
        delete p2d;
        rc = false;
        break;
      }
    }
    MYON_HatchLoop* loop = new MYON_HatchLoop(p2d,loops.Count()?MYON_HatchLoop::ltInner:MYON_HatchLoop::ltOuter);
    if(loop)
      loops.Append(loop);
    else
      delete p2d;
  }
  if(!rc)
  {
    for(int i = 0; i < loops.Count(); i++)
      delete loops[i];

    loops.Empty();
  }

  if(loops.Count() < 1)
    return false;

  for(int i = 0; i < m_loops.Count(); i++)
    delete m_loops[i];
  m_loops.Empty();
  for(int i = 0; i < loops.Count(); i++)
    m_loops.Append(loops[i]);
  return true;
}

const MYON_HatchLoop* MYON_Hatch::Loop( int index) const
{
  if( index >= 0 && index < m_loops.Count())
    return m_loops[index];  
  return nullptr;
}

// Basepoint functions added March 23, 2008 -LW
void MYON_Hatch::SetBasePoint(MYON_2dPoint basepoint)
{
  m_basepoint = basepoint;
}

void MYON_Hatch::SetBasePoint(MYON_3dPoint point)
{
  m_plane.ClosestPointTo(point, &m_basepoint.x, &m_basepoint.y);
}

MYON_3dPoint MYON_Hatch::BasePoint() const
{
  return m_plane.PointAt(m_basepoint.x, m_basepoint.y);
}

MYON_2dPoint MYON_Hatch::BasePoint2d() const
{
  return m_basepoint;
}

MYON_CurveRegionBoundaryElement::MYON_CurveRegionBoundaryElement()
  : m_curve_id(-1), m_bReversed(false)
{}
MYON_CurveRegionBoundaryElement::MYON_CurveRegionBoundaryElement(const MYON_CurveRegionBoundaryElement& src)
{
  *this = src;
}

MYON_CurveRegionBoundaryElement::~MYON_CurveRegionBoundaryElement()
{}

MYON_CurveRegionBoundaryElement& MYON_CurveRegionBoundaryElement::operator=(const MYON_CurveRegionBoundaryElement& src)
{
  if (this != &src){
    m_curve_id = src.m_curve_id;
    m_subdomain = src.m_subdomain;
    m_bReversed = src.m_bReversed;
  }
  return *this;
}

class MYON_CLASS MYON_GradientColorData : public MYON_UserData
{
  MYON_OBJECT_DECLARE(MYON_GradientColorData);
public:
  MYON_GradientColorData();
  ~MYON_GradientColorData() = default;

  MYON_GradientColorData(const MYON_GradientColorData&) = default;
  MYON_GradientColorData& operator=(const MYON_GradientColorData&) = default;

  //MYON_GradientColorData(const MYON_GradientColorData&);
  //MYON_GradientColorData& operator=(const MYON_GradientColorData&);

  static MYON_GradientColorData* FromObject( const MYON_Object* );
  static MYON_GradientColorData* FromObject(MYON_Object* obj, bool createAndAttachIfMissing);

  // override virtual MYON_Object::Dump function
  void Dump(MYON_TextLog& text_log) const override;

  // override virtual MYON_Object::SizeOf function
  unsigned int SizeOf() const override;

  // override virtual MYON_Object::DataCRC function
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;

  // override virtual MYON_Object::Write function
  bool Write(MYON_BinaryArchive& binary_archive) const override;

  // override virtual MYON_Object::Read function
  bool Read(MYON_BinaryArchive& binary_archive) override;

  // override virtual MYON_UserData::GetDescription function
  bool GetDescription(MYON_wString& description) override;

  // override virtual MYON_UserData::Archive function
  bool WriteToArchive(
    const class MYON_BinaryArchive& archive,
    const class MYON_Object* parent_object
  ) const override;

  // override virtual MYON_UserData::Transform function
  bool Transform(const MYON_Xform&) override;

  MYON_GradientType m_gradient_type = MYON_GradientType::Linear;
  MYON_3dPoint m_start = MYON_3dPoint::UnsetPoint;
  MYON_3dPoint m_end = MYON_3dPoint::UnsetPoint;
  double m_repeat = 0;
  MYON_SimpleArray<MYON_ColorStop> m_colors;

//private:
  //void Internal_CopyFrom(const MYON_GradientColorData&);
};

MYON_OBJECT_IMPLEMENT(MYON_GradientColorData, MYON_UserData, "0C1AD613-4EFA-4F47-A147-4D79D77FCB0C");

MYON_GradientColorData::MYON_GradientColorData()
{
  m_userdata_uuid = MYON_CLASS_ID(MYON_GradientColorData);
  m_application_uuid = MYON_opennurbs6_id;
  m_userdata_copycount = 1;
}

//void MYON_GradientColorData::Internal_CopyFrom(const MYON_GradientColorData & src)
//{
//  m_gradient_type = src.m_gradient_type;
//  m_start = src.m_start;
//  m_end = src.m_end;
//  m_repeat = src.m_repeat;
//  m_colors = src.m_colors;
//}
//
//MYON_GradientColorData::MYON_GradientColorData(const MYON_GradientColorData &src)
//  : MYON_UserData(src)
//{
//  Internal_CopyFrom(src);
//}

MYON_GradientColorData* MYON_GradientColorData::FromObject(const MYON_Object* p)
{
  return p
    ? MYON_GradientColorData::Cast(p->GetUserData(MYON_CLASS_ID(MYON_GradientColorData)))
    : nullptr;
}

//MYON_GradientColorData & MYON_GradientColorData::operator=(const MYON_GradientColorData & src)
//{
//  if (this != &src)
//  {
//    MYON_UserData::operator=(src);
//    Internal_CopyFrom(src);
//  }
//  return *this;
//}

MYON_GradientColorData* MYON_GradientColorData::FromObject(MYON_Object* obj, bool createAndAttachIfMissing)
{
  if (nullptr == obj)
    return nullptr;
  MYON_GradientColorData* rc = MYON_GradientColorData::Cast(obj->GetUserData(MYON_CLASS_ID(MYON_GradientColorData)));
  if (nullptr == rc && createAndAttachIfMissing)
  {
    rc = new MYON_GradientColorData();
    if (!obj->AttachUserData(rc))
    {
      delete rc;
      return nullptr;
    }
  }
  return rc;
}

bool MYON_GradientColorData::GetDescription(MYON_wString& description)
{
  description = L"Color Gradient UserData";
  return true;
}

bool MYON_GradientColorData::WriteToArchive(const MYON_BinaryArchive & archive, const MYON_Object * parent_object) const
{
  return (archive.Archive3dmVersion() >= 60);
}

unsigned int MYON_GradientColorData::SizeOf() const
{
  unsigned int sz = MYON_UserData::SizeOf();
  sz += sizeof(*this) - sizeof(MYON_UserData);
  sz += m_colors.SizeOfArray();
  return sz;
}

MYON__UINT32 MYON_GradientColorData::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = MYON_CRC32(current_remainder, sizeof(m_gradient_type), &m_gradient_type);
  current_remainder = m_start.DataCRC(current_remainder);
  current_remainder = m_end.DataCRC(current_remainder);
  current_remainder = MYON_CRC32(current_remainder, sizeof(m_repeat), &m_repeat);
  current_remainder = m_colors.DataCRC(current_remainder);
  return current_remainder;
}

void MYON_GradientColorData::Dump(MYON_TextLog& text_log) const
{
  switch (m_gradient_type)
  {
  case MYON_GradientType::None:
    text_log.Print("None gradient\n");
    break;
  case MYON_GradientType::Linear:
    text_log.Print("Linear gradient\n");
    break;
  case MYON_GradientType::Radial:
    text_log.Print("Radial gradient\n");
    break;
  case MYON_GradientType::LinearDisabled:
    text_log.Print("Linear(disabled) gradient\n");
    break;
  case MYON_GradientType::RadialDisabled:
    text_log.Print(L"Radial(disabled) gradient\n");
    break;
  default:
    break;
  }

  text_log.PushIndent();
  text_log.Print("points ");
  text_log.Print(m_start);
  text_log.Print("-");
  text_log.Print(m_end);
  text_log.Print("\nrepeat %d\n", m_repeat);
  text_log.PopIndent();
}

bool MYON_GradientColorData::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 0);
  if (!rc)
    return false;

  for (;;)
  {
    rc = archive.WriteInt((int)m_gradient_type);
    if (!rc) break;
    rc = archive.WritePoint(m_start);
    if (!rc) break;
    rc = archive.WritePoint(m_end);
    if (!rc) break;
    rc = archive.WriteDouble(m_repeat);
    if (!rc) break;

    int count = m_colors.Count();
    rc = archive.WriteInt(count);
    if (!rc) break;

    for (int i = 0; i < count && rc; i++)
    {
      rc = m_colors[i].Write(archive);
      if (!rc) break;
    }
    if (!rc) break;

    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_GradientColorData::Read(MYON_BinaryArchive& archive)
{
  m_colors.SetCount(0);

  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version);
  if (!rc)
    return false;

  for (;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    int gt = (int)MYON_GradientType::None;
    rc = archive.ReadInt(&gt);
    if (gt < (int)MYON_GradientType::None || gt >(int)MYON_GradientType::RadialDisabled)
      rc = false;
    if (!rc) break;
    m_gradient_type = (MYON_GradientType)gt;

    rc = archive.ReadPoint(m_start);
    if (!rc) break;
    rc = archive.ReadPoint(m_end);
    if (!rc) break;
    rc = archive.ReadDouble(&m_repeat);
    if (!rc) break;
    int count = 0;
    rc = archive.ReadInt(&count);
    if (!rc) break;

    m_colors.Reserve(count);
    for (int i = 0; i < count && rc; i++)
    {
      MYON_ColorStop cs;
      rc = cs.Read(archive);
      if (!rc) break;

      m_colors.Append(cs);
    }
    if (!rc) break;

    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool MYON_GradientColorData::Transform(const MYON_Xform& xform)
{
  m_start.Transform(xform);
  m_end.Transform(xform);
  return MYON_UserData::Transform(xform);
}

/////////////////////////////////////////////////////////////////////////

MYON_GradientType MYON_Hatch::GetGradientType() const
{
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this);
  if (data)
    return data->m_gradient_type;
  return MYON_GradientType::None;
}

void MYON_Hatch::SetGradientType(MYON_GradientType gt)
{
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this, true);
  if (data)
    data->m_gradient_type = gt;
}

void MYON_Hatch::GetGradientColors(MYON_SimpleArray<MYON_ColorStop>& colors) const
{
  colors.Empty();
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this);
  if (data)
    colors = data->m_colors;
}
bool MYON_Hatch::SetGradientColors(const MYON_SimpleArray<MYON_ColorStop>& colors)
{
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this, true);
  if (nullptr == data)
    return false;
  data->m_colors = colors;
  return true;
}

double MYON_Hatch::GetGradientRepeat() const
{
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this);
  if (data)
    return data->m_repeat;
  return 0;
}
bool MYON_Hatch::SetGradientRepeat(double repeat)
{
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this);
  if (nullptr == data && 0 == repeat)
    return true;

  data = MYON_GradientColorData::FromObject(this, true);
  if (nullptr == data)
    return false;
  
  data->m_repeat = repeat;
  return true;
}

void MYON_Hatch::GetGradientEndPoints(MYON_3dPoint& startPoint, MYON_3dPoint& endPoint) const
{
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this);
  if (nullptr == data)
  {
    startPoint = MYON_3dPoint::UnsetPoint;
    endPoint = MYON_3dPoint::UnsetPoint;
    return;
  }

  startPoint = data->m_start;
  endPoint = data->m_end;
}
bool MYON_Hatch::SetGradientEndPoints(MYON_3dPoint startpoint, MYON_3dPoint endpoint)
{
  MYON_GradientColorData* data = MYON_GradientColorData::FromObject(this, true);
  if (nullptr == data)
    return false;

  data->m_start = startpoint;
  data->m_end = endpoint;
  return true;
}

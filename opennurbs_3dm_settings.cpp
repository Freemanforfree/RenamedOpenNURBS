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

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmUnitsAndTolerances
//

static double MYON_Internal_UnitSystemCtorMetersPerUnit(
  MYON::LengthUnitSystem length_unit_system
)
{
  double meters_per_unit;
  switch (length_unit_system)
  {
  case MYON::LengthUnitSystem::None:
    meters_per_unit = 1.0;
    break;
  case MYON::LengthUnitSystem::Angstroms:
  case MYON::LengthUnitSystem::Nanometers:
  case MYON::LengthUnitSystem::Microns:
  case MYON::LengthUnitSystem::Millimeters:
  case MYON::LengthUnitSystem::Centimeters:
  case MYON::LengthUnitSystem::Decimeters:
  case MYON::LengthUnitSystem::Meters:
  case MYON::LengthUnitSystem::Dekameters:
  case MYON::LengthUnitSystem::Hectometers:
  case MYON::LengthUnitSystem::Kilometers:
  case MYON::LengthUnitSystem::Megameters:
  case MYON::LengthUnitSystem::Gigameters:
  case MYON::LengthUnitSystem::Microinches:
  case MYON::LengthUnitSystem::Mils:
  case MYON::LengthUnitSystem::Inches:
  case MYON::LengthUnitSystem::Feet:
  case MYON::LengthUnitSystem::Yards:
  case MYON::LengthUnitSystem::Miles:
  case MYON::LengthUnitSystem::PrinterPoints:
  case MYON::LengthUnitSystem::PrinterPicas:
  case MYON::LengthUnitSystem::NauticalMiles:
  case MYON::LengthUnitSystem::AstronomicalUnits:
  case MYON::LengthUnitSystem::LightYears:
  case MYON::LengthUnitSystem::Parsecs:
    meters_per_unit = MYON::UnitScale(length_unit_system, MYON::LengthUnitSystem::Meters);
    break;
  case MYON::LengthUnitSystem::CustomUnits:
    meters_per_unit = 1.0;
    break;
  case MYON::LengthUnitSystem::Unset:
    meters_per_unit = MYON_DBL_QNAN;
    break;
  default:
    meters_per_unit = MYON_DBL_QNAN;
    break;
  }
  return meters_per_unit;
}


MYON_UnitSystem::MYON_UnitSystem(MYON::LengthUnitSystem length_unit_system)
: m_unit_system(MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(length_unit_system)))
, m_meters_per_custom_unit(MYON_Internal_UnitSystemCtorMetersPerUnit(m_unit_system))
{}

MYON_UnitSystem& MYON_UnitSystem::operator=(
  MYON::LengthUnitSystem length_unit_system
  )
{
  *this = MYON_UnitSystem(length_unit_system);
  return *this;
}


bool MYON_UnitSystem::operator==(const MYON_UnitSystem& other) const
{
  if ( m_unit_system != other.m_unit_system )
    return false;

  if ( MYON::LengthUnitSystem::CustomUnits == m_unit_system )
  {
    if ( !(m_meters_per_custom_unit == other.m_meters_per_custom_unit) )
      return false;
    if ( false == m_custom_unit_name.EqualOrdinal(other.m_custom_unit_name,false) )
      return false;
  }

  return true;
}

bool MYON_UnitSystem::operator!=(const MYON_UnitSystem& other) const
{
  if ( m_unit_system != other.m_unit_system )
    return true;

  if ( MYON::LengthUnitSystem::CustomUnits == m_unit_system )
  {
    if (m_meters_per_custom_unit != other.m_meters_per_custom_unit)
      return true;
    if ( false == m_custom_unit_name.EqualOrdinal(other.m_custom_unit_name,false) )
      return true;
  }

  return false;
}

bool MYON_UnitSystem::IsValid() const
{
  if ( m_unit_system != MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(m_unit_system)) )
  {
    // invalid enum value
    return false;
  }

  if (MYON::LengthUnitSystem::Unset == m_unit_system)
    return false;

  if (MYON::LengthUnitSystem::CustomUnits == m_unit_system)
  {
    if (false == MYON_IsValidPositiveNumber(m_meters_per_custom_unit))
      return false;
  }

  return true;
}

bool MYON_UnitSystem::IsSet() const
{
  return (MYON::LengthUnitSystem::Unset != m_unit_system && MYON::LengthUnitSystem::None != m_unit_system && IsValid() );
}

bool MYON_UnitSystem::IsCustomUnitSystem() const
{
  return (MYON::LengthUnitSystem::CustomUnits == m_unit_system && IsSet());
}

void MYON_UnitSystem::SetUnitSystem(
  MYON::LengthUnitSystem us
  )
{
  *this = MYON_UnitSystem(us);
}

MYON_UnitSystem MYON_UnitSystem::CreateCustomUnitSystem(
  const wchar_t* custom_unit_name,
  double meters_per_custom_unit
)
{
  MYON_UnitSystem custom_unit_system = MYON_UnitSystem::Unset;
  custom_unit_system.SetCustomUnitSystem(custom_unit_name, meters_per_custom_unit);
  return custom_unit_system;
}

void MYON_UnitSystem::SetCustomUnitSystem(
  const wchar_t* custom_unit_name,
  double meters_per_custom_unit
  )
{
  MYON_wString local_str(custom_unit_name);
  local_str.TrimLeftAndRight();
  m_unit_system = MYON::LengthUnitSystem::CustomUnits;
  m_custom_unit_name = local_str;
  if ( MYON_IsValidPositiveNumber(meters_per_custom_unit) )
  {
    m_meters_per_custom_unit = meters_per_custom_unit;
  }
  else
  {
    MYON_ERROR("Invalid meters_per_custom_unit parameter");
    m_meters_per_custom_unit = 1.0; // must be > 0.0 and < MYON_UNSET_POSITIVE_VALUE
  }
}

void MYON_UnitSystem::SetCustomUnitSystemName(
  const wchar_t* custom_unit_name
  )
{
  const bool bIsCustomUnitSystem = (MYON::LengthUnitSystem::CustomUnits == m_unit_system);
  MYON_wString local_name(custom_unit_name);
  local_name.TrimLeftAndRight();
  if (local_name.IsNotEmpty() || bIsCustomUnitSystem)
  {
    const double meters_per_custom_unit
      = bIsCustomUnitSystem
      ? m_meters_per_custom_unit
      : 1.0;
    SetCustomUnitSystem(local_name, meters_per_custom_unit);
  }
}

void MYON_UnitSystem::SetCustomUnitSystemScale(
  double meters_per_custom_unit
  )
{
  if (MYON_IsValidPositiveNumber(meters_per_custom_unit))
  {
    const bool bIsCustomUnitSystem = (MYON::LengthUnitSystem::CustomUnits == m_unit_system);
    if (false == (meters_per_custom_unit == m_meters_per_custom_unit) || bIsCustomUnitSystem)
    {
      const MYON_wString unit_system_name
        = (MYON::LengthUnitSystem::CustomUnits == m_unit_system)
        ? m_custom_unit_name
        : MYON_wString::EmptyString;
      SetCustomUnitSystem(unit_system_name, meters_per_custom_unit);
    }
  }
}

double MYON_UnitSystem::MetersPerUnit(
  double unset_return_value
) const
{
  switch (m_unit_system)
  {
  case MYON::LengthUnitSystem::None:
    return 1.0;
    break;
  case MYON::LengthUnitSystem::CustomUnits:
    return m_meters_per_custom_unit;
    break;
  case MYON::LengthUnitSystem::Unset:
    return unset_return_value;
    break;
  default:
    break;
  }
  return MYON::UnitScale(m_unit_system, MYON::LengthUnitSystem::Meters);
}

double MYON_UnitSystem::MillimetersPerUnit(
  double unset_return_value
) const
{
  switch (m_unit_system)
  {
  case MYON::LengthUnitSystem::None:
    return 1.0;
    break;
  case MYON::LengthUnitSystem::CustomUnits:
    return 1000.0*m_meters_per_custom_unit;
    break;
  case MYON::LengthUnitSystem::Unset:
    return unset_return_value;
    break;
  default:
    break;
  }
  return MYON::UnitScale(m_unit_system, MYON::LengthUnitSystem::Millimeters);
}

double MYON_UnitSystem::MetersPerUnit() const
{
  // NOTE WELL:
  //   https://mcneel.myjetbrains.com/youtrack/issue/RH-60700
  //   For standard units, this function returns the WRONG value (inverse of the correct value).
  //   The reason is the Rhino 6 VRay plug-in assumes the incorrect value is returned
  //   and V6 VRay does not work correctly in Rhino 7 if the correct value is returned.
  //   After some discussion (see the bug above), we will leave the invers bug in
  //   MYON_UnitSystem::MetersPerUnit(), deprecate MYON_UnitSystem::MetersPerUnit(),
  //   and add a new function that returns the correct answer.
  if (MYON::LengthUnitSystem::CustomUnits == m_unit_system)
  {
    // correct answer for custome units - V6 behavior.
    return m_meters_per_custom_unit; //
  }


  // For standard units, the inverse of the correct answer is returned
  // to preserve V6 bug so VRay works in Rhino 7.
  return 1.0/MYON_UnitSystem::MetersPerUnit(MYON_DBL_QNAN);
}

MYON::LengthUnitSystem MYON_UnitSystem::UnitSystem() const
{
  return m_unit_system;
}

static void MYON_Internal_InitUnitSystemName(
  const wchar_t* name,
  MYON_wString& local_storage
  )
{
  if (local_storage.IsEmpty())
    local_storage = name;
}

const MYON_wString& MYON_UnitSystem::UnitSystemName() const
{
  switch (m_unit_system)
  {
  case MYON::LengthUnitSystem::None:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"no units",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Angstroms:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"angstroms",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Nanometers:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"nanometers",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Microns:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"microns",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Millimeters:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"millimeters",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Decimeters:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"decimeters",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Centimeters:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"centimeters",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Meters:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"meters",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Dekameters:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"dekameters",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Hectometers:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"hectometers",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Kilometers:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"kilometers",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Megameters:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"megameters",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Gigameters:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"gigameters",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Microinches:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"microinches",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Mils:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"mils",s_name);// (= 0.001 inches)";
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Inches:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"inches",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Feet:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"feet",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Yards:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"yards",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Miles:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"miles",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::PrinterPoints:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"points",s_name); // (1/72 inch)";
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::PrinterPicas:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"picas",s_name); // (1/6 inch)";
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::NauticalMiles:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"nautical miles",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::AstronomicalUnits:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"astronomical units",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::LightYears:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"light years",s_name);
      return s_name;
    }
    break;
  case MYON::LengthUnitSystem::Parsecs:
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"parsecs",s_name);
      return s_name;
    }
    break;

  case MYON::LengthUnitSystem::CustomUnits:
    if (m_custom_unit_name.IsEmpty())
    {
      static MYON_wString s_name;
      MYON_Internal_InitUnitSystemName(L"custom units", s_name);
      return s_name;
    }
    return m_custom_unit_name;
    break;

  case MYON::LengthUnitSystem::Unset:
    {
    static MYON_wString s_name;
    MYON_Internal_InitUnitSystemName(L"unset", s_name);
    return s_name;
    }
    break;
  }

  return MYON_wString::EmptyString;
}


bool MYON_UnitSystem::Read( MYON_BinaryArchive& file )
{
  *this = MYON_UnitSystem::None;

  int major_version = 0;
  int minor_version = 0;
  
  if ( !file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version) )
    return false;

  MYON::LengthUnitSystem us = MYON::LengthUnitSystem::None;
  double meters_per_unit  = 0.0;
  MYON_wString custom_unit_name;
  bool rc = false;
  for (;;)
  {

    if (1 != major_version)
      break;
    unsigned int i = MYON_UNSET_UINT_INDEX;
    if (false == file.ReadInt(&i))
      break;
    us = MYON::LengthUnitSystemFromUnsigned(i);
    if (false == file.ReadDouble(&meters_per_unit))
      break;
    if (false == file.ReadString(custom_unit_name))
      break;
    rc = true;
    break;
  }

  if (!file.EndRead3dmChunk())
  {
    rc = false;
  }
  else
  {
    if (MYON::LengthUnitSystem::CustomUnits == us)
    {
      m_unit_system = us;
      m_custom_unit_name = custom_unit_name;
      m_meters_per_custom_unit = meters_per_unit;
    }
    else
    {
      *this = MYON_UnitSystem(us);
    }
  }

  return rc;
}

bool MYON_UnitSystem::Write( MYON_BinaryArchive& file ) const
{
  if ( !file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0) )
    return false;


  // values saved in the file
  //no_unit_system =  0, 
  //microns        =  1,  // 1.0e-6 meters
  //millimeters    =  2,  // 1.0e-3 meters
  //centimeters    =  3,  // 1.0e-2 meters
  //meters         =  4,
  //kilometers     =  5,  // 1.0e+3 meters
  //microinches    =  6,  // 1.0e-6 inches
  //mils           =  7,  // 1.0e-3 inches
  //inches         =  8,  // 0.0254 meters
  //feet           =  9,  // 12 inches
  //miles          = 10,  // 63360 inches
  //custom_unit_system = 11, // x meters with x defined in MYON_3dmUnitsAndTolerances.m_custom_unit_scale

  bool rc = false;
  for (;;)
  {
    if (!file.WriteInt(static_cast<unsigned int>(m_unit_system)))
      break;
    if (!file.WriteDouble(MYON::LengthUnitSystem::CustomUnits == m_unit_system ? m_meters_per_custom_unit : MYON::UnitScale(m_unit_system, MYON::LengthUnitSystem::Meters)))
      break;
    if (!file.WriteString(MYON::LengthUnitSystem::CustomUnits == m_unit_system ? m_custom_unit_name : MYON_wString::EmptyString))
      break;
    rc = true;
    break;
  }

  if ( !file.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

const MYON_wString MYON_UnitSystem::ToString() const
{
  MYON_wString str(UnitSystemName());
  if (MYON::LengthUnitSystem::CustomUnits == m_unit_system)
  {
    MYON_wString meters_per_unit;
    meters_per_unit.Format(L" (= %g meters )", m_meters_per_custom_unit);
    str += meters_per_unit;
  }
  return str;

}

void MYON_UnitSystem::Dump( MYON_TextLog& dump ) const
{
  const MYON_wString sUnitSystem(ToString());
  dump.Print("Unit system: %ls\n",static_cast<const wchar_t*>(sUnitSystem));
}

bool MYON_3dmUnitsAndTolerances::Write( MYON_BinaryArchive& file ) const
{
  const int version = 102;
  unsigned int i;

  // version 100 MYON_3dmUnitsAndTolerances settings
  bool rc = file.WriteInt( version );
  i = static_cast<unsigned int>(m_unit_system.UnitSystem());
  if ( rc ) rc = file.WriteInt( i );
  if ( rc ) rc = file.WriteDouble( m_absolute_tolerance );
  if ( rc ) rc = file.WriteDouble( m_angle_tolerance );
  if ( rc ) rc = file.WriteDouble( m_relative_tolerance );

  // added in version 101
  i = static_cast<unsigned int>(m_distance_display_mode);
  if ( rc ) rc = file.WriteInt( i );
  i = m_distance_display_precision;
  if ( i > 20 ) 
  {
    MYON_ERROR("MYON_3dmUnitsAndTolerances::Write() - m_distance_display_precision out of range.");
    i = 3;
  }
  if ( rc ) rc = file.WriteInt( i );

  // added in version 102
  if ( rc ) rc = file.WriteDouble( m_unit_system.MetersPerUnit(MYON_DBL_QNAN));
  if ( rc ) rc = file.WriteString( (MYON::LengthUnitSystem::CustomUnits == m_unit_system.UnitSystem() ? m_unit_system.UnitSystemName() : MYON_wString::EmptyString) );
  return rc;
}

bool MYON_3dmUnitsAndTolerances::Read( MYON_BinaryArchive& file )
{
  *this = MYON_3dmUnitsAndTolerances::Millimeters;
  int version = 0;
  bool rc = file.ReadInt( &version );
  if ( rc && version >= 100 && version < 200 )
  {
    MYON::LengthUnitSystem us = MYON::LengthUnitSystem::None;
    double meters_per_unit = 1.0;
    MYON_wString custom_unit_name;

    int i = MYON_UNSET_UINT_INDEX;
    rc = file.ReadInt( &i );
    if ( rc )
      us = MYON::LengthUnitSystemFromUnsigned(i);
    if ( rc ) rc = file.ReadDouble( &m_absolute_tolerance );
    if ( rc ) rc = file.ReadDouble( &m_angle_tolerance );
    if ( rc ) rc = file.ReadDouble( &m_relative_tolerance );
    if ( version >= 101 ) 
    {
      unsigned int dm = static_cast<unsigned int>(MYON::OBSOLETE_DistanceDisplayMode::Decimal);
      if ( rc ) rc = file.ReadInt( &dm );
      if ( rc ) m_distance_display_mode = MYON::DistanceDisplayModeFromUnsigned(dm);
      if ( rc ) rc = file.ReadInt( &m_distance_display_precision );
      if ( m_distance_display_precision < 0 || m_distance_display_precision > 20 )
        m_distance_display_precision = 3; // some beta files had bogus values stored in file
      if ( version >= 102 ) 
      {
        if ( rc ) rc = file.ReadDouble( &meters_per_unit );
        if ( rc ) rc = file.ReadString( custom_unit_name );
      }
    }
    if ( MYON::LengthUnitSystem::CustomUnits == us )
      m_unit_system.SetCustomUnitSystem(custom_unit_name,meters_per_unit);
    else
      m_unit_system.SetUnitSystem(us);
  }
  return rc;
}

void MYON_3dmUnitsAndTolerances::Dump( MYON_TextLog& dump) const
{
  m_unit_system.Dump(dump);
  dump.Print("Absolute tolerance: %g\n",m_absolute_tolerance);
  dump.Print("Angle tolerance: %g\n",m_angle_tolerance);
}

double MYON_3dmUnitsAndTolerances::Scale( MYON::LengthUnitSystem us ) const
{
  // Example: If us = meters and m_unit_system = centimeters,
  // then Scale() returns 100.
  return MYON::UnitScale( us, m_unit_system );
}

bool MYON_3dmUnitsAndTolerances::IsValid() const
{
  for (;;)
  {
    if (!(m_distance_display_precision >= 0 && m_distance_display_precision <= 7))
      break;

    if (!((int)m_distance_display_mode >= 0 && (int)m_distance_display_mode <= 3))
      break;

    if (!TolerancesAreValid())
      break;

    return true;
  }
  return false;
}

double MYON_3dmUnitsAndTolerances::AbsoluteTolerance() const
{
  return m_absolute_tolerance;
}

void MYON_3dmUnitsAndTolerances::SetAbsoluteTolerance(double absolute_tolerance)
{
  if (absolute_tolerance > 0.0)
    m_absolute_tolerance = absolute_tolerance;
}

double MYON_3dmUnitsAndTolerances::AngleTolerance() const
{
  return m_angle_tolerance;
}

void MYON_3dmUnitsAndTolerances::SetAngleTolerance(double angle_tolerance)
{
  if (angle_tolerance > 0.0 && angle_tolerance <= MYON_PI)
    m_angle_tolerance = angle_tolerance;
}

double MYON_3dmUnitsAndTolerances::RelativeTolerance() const
{
  return m_relative_tolerance;
}

void MYON_3dmUnitsAndTolerances::SetRelativeTolerance(double relative_tolerance)
{
  if (relative_tolerance > 0.0 && relative_tolerance < 1.0)
    m_relative_tolerance = relative_tolerance;
}

MYON::OBSOLETE_DistanceDisplayMode MYON_3dmUnitsAndTolerances::DistanceDisplayMode() const
{
  return m_distance_display_mode;
}

void MYON_3dmUnitsAndTolerances::SetDistanceDisplayMode(MYON::OBSOLETE_DistanceDisplayMode distance_display_mode)
{
  m_distance_display_mode = distance_display_mode;
}

int MYON_3dmUnitsAndTolerances::DistanceDisplayPrecision() const
{
  return m_distance_display_precision;
}

void MYON_3dmUnitsAndTolerances::SetDistanceDisplayPrecision(int distance_display_precision)
{
  if (distance_display_precision >= 0 && distance_display_precision <= 7)
    m_distance_display_precision = distance_display_precision;
}

bool MYON_3dmUnitsAndTolerances::TolerancesAreValid() const
{
  for (;;)
  {
    if (!(m_absolute_tolerance > 0.0))
      break;

    if (!(m_angle_tolerance > 0.0 && m_angle_tolerance <= MYON_PI))
      break;

    if (!(m_relative_tolerance > 0.0 && m_relative_tolerance < 1.0))
      break;

    return true;
  }
  return false;
}

unsigned int MYON_3dmUnitsAndTolerances::SetInvalidTolerancesToDefaultValues()
{
  unsigned int rc = 0;

  if ( !(m_absolute_tolerance > 0.0) )
  {
    rc |= 1;
    // Do NOT apply scaling from mm to current units.
    m_absolute_tolerance = MYON_3dmUnitsAndTolerances::Millimeters.m_absolute_tolerance;
  }

  if ( !(m_angle_tolerance > 0.0 && m_angle_tolerance <= MYON_PI) )
  {
    rc |= 2;
    m_angle_tolerance = MYON_3dmUnitsAndTolerances::Millimeters.m_angle_tolerance;
  }

  if ( !( m_relative_tolerance > 0.0 && m_relative_tolerance < 1.0) )
  {
    rc |= 4;
    m_relative_tolerance = MYON_3dmUnitsAndTolerances::Millimeters.m_relative_tolerance;
  }

  return rc;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmRenderSettings
//

// This is inside the 'current content' section.
#define MYON_RENDER_BACKGROUND_ENVIRONMENT              L"environment"

// These are inside the 'rendering' section.
#define MYON_RENDER_CUSTOM_REFLECTIVE_ENVIRONMENT_ON    L"custom-env-for-refl-and-refr-on"
#define MYON_RENDER_CUSTOM_REFLECTIVE_ENVIRONMENT       L"custom-env-for-refl-and-refr"

// These are inside the 'sun' section.
#define MYON_RENDER_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON  L"skylight-custom-environment-on"
#define MYON_RENDER_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT     L"skylight-custom-environment"

static const wchar_t* XMLPathBack360(void) // Not used for 'override'.
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_CURRENT_CONTENT;
}

static const wchar_t* XMLPathReflRefr(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_RENDERING;
}

static const wchar_t* XMLPathSkylight(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_SUN;
}

MYON_EnvironmentsPrivate::MYON_EnvironmentsPrivate(const MYON_EnvironmentsPrivate& ei)
{
  operator = (ei);
}

MYON_EnvironmentsPrivate& MYON_EnvironmentsPrivate::operator = (const MYON_EnvironmentsPrivate& ep)
{
  if (this != &ep)
  {
    SetBackgroundRenderEnvironment         (ep.BackgroundRenderEnvironment());
    SetSkylightingRenderEnvironmentOverride(ep.SkylightingRenderEnvironmentOverride());
    SetSkylightingRenderEnvironment        (ep.SkylightingRenderEnvironment());
    SetReflectionRenderEnvironmentOverride (ep.ReflectionRenderEnvironmentOverride());
    SetReflectionRenderEnvironment         (ep.ReflectionRenderEnvironment());
  }

  return *this;
}

bool MYON_EnvironmentsPrivate::operator == (const MYON_EnvironmentsPrivate& ep)
{
  if (BackgroundRenderEnvironment()          != ep.BackgroundRenderEnvironment())          return false;
  if (SkylightingRenderEnvironmentOverride() != ep.SkylightingRenderEnvironmentOverride()) return false;
  if (SkylightingRenderEnvironment()         != ep.SkylightingRenderEnvironment())         return false;
  if (ReflectionRenderEnvironmentOverride()  != ep.ReflectionRenderEnvironmentOverride())  return false;
  if (ReflectionRenderEnvironment()          != ep.ReflectionRenderEnvironment())          return false;

  return true;
}

MYON_UUID MYON_EnvironmentsPrivate::BackgroundRenderEnvironment(void) const
{
  const wchar_t* s = MYON_RENDER_BACKGROUND_ENVIRONMENT;
  return GetParameter_NoType(XMLPathBack360(), s, L"uuid", MYON_nil_uuid).AsUuid();
}

void MYON_EnvironmentsPrivate::SetBackgroundRenderEnvironment(const MYON_UUID& id)
{
  const wchar_t* s = MYON_RENDER_BACKGROUND_ENVIRONMENT;
  SetParameter_NoType(XMLPathBack360(), s, id);
}

bool MYON_EnvironmentsPrivate::SkylightingRenderEnvironmentOverride(void) const
{
  const wchar_t* s = MYON_RENDER_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON;
  return GetParameter(XMLPathSkylight(), s, false);
}

void MYON_EnvironmentsPrivate::SetSkylightingRenderEnvironmentOverride(bool on)
{
  const wchar_t* s = MYON_RENDER_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON;
  SetParameter(XMLPathSkylight(), s, on);
}

MYON_UUID MYON_EnvironmentsPrivate::SkylightingRenderEnvironment(void) const
{
  const wchar_t* s = MYON_RENDER_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT;
  return GetParameter_NoType(XMLPathSkylight(), s, L"uuid", MYON_nil_uuid).AsUuid();
}

void MYON_EnvironmentsPrivate::SetSkylightingRenderEnvironment(const MYON_UUID& id)
{
  const wchar_t* s = MYON_RENDER_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT;
  SetParameter_NoType(XMLPathSkylight(), s, id);
}

bool MYON_EnvironmentsPrivate::ReflectionRenderEnvironmentOverride(void) const
{
  const wchar_t* s = MYON_RENDER_CUSTOM_REFLECTIVE_ENVIRONMENT_ON;
  return GetParameter(XMLPathReflRefr(), s, false);
}

void MYON_EnvironmentsPrivate::SetReflectionRenderEnvironmentOverride(bool on)
{
  const wchar_t* s = MYON_RENDER_CUSTOM_REFLECTIVE_ENVIRONMENT_ON;
  SetParameter(XMLPathReflRefr(), s, on);
}

MYON_UUID MYON_EnvironmentsPrivate::ReflectionRenderEnvironment(void) const
{
  const wchar_t* s = MYON_RENDER_CUSTOM_REFLECTIVE_ENVIRONMENT;
  return GetParameter_NoType(XMLPathReflRefr(), s, L"uuid", MYON_nil_uuid).AsUuid();
}

void MYON_EnvironmentsPrivate::SetReflectionRenderEnvironment(const MYON_UUID& id)
{
  const wchar_t* s = MYON_RENDER_CUSTOM_REFLECTIVE_ENVIRONMENT;
  SetParameter_NoType(XMLPathReflRefr(), s, id);
}

MYON_OBJECT_IMPLEMENT(MYON_3dmRenderSettings, MYON_Object, "58A5953A-57C5-4FD3-84F5-7D4240478D15");

MYON_3dmRenderSettingsPrivate::MYON_3dmRenderSettingsPrivate()
  :
  _dithering      (_rdk_document_data),
  _ground_plane   (_rdk_document_data),
  _linear_workflow(_rdk_document_data),
  _render_channels(_rdk_document_data),
  _safe_frame     (_rdk_document_data),
  _skylight       (_rdk_document_data),
  _sun            (_rdk_document_data),
  _environments   (_rdk_document_data)
{
  // 26th January 2023 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-71396
  // Populate the RDK document data with defaults. The previous fix for this did it in RdkDocNode()
  // which was the wrong place. We have to do it even if the RDK isn't being used.
  MYON_RdkDocumentDefaults dd(MYON::VersionMajor(), MYON_RdkDocumentDefaults::ValueSets::All);
  dd.CopyDefaultsTo(_rdk_document_data);
}

MYON_3dmRenderSettingsPrivate::MYON_3dmRenderSettingsPrivate(const MYON_3dmRenderSettingsPrivate& p)
  :
  _dithering      (_rdk_document_data),
  _ground_plane   (_rdk_document_data),
  _linear_workflow(_rdk_document_data),
  _render_channels(_rdk_document_data),
  _safe_frame     (_rdk_document_data),
  _skylight       (_rdk_document_data),
  _sun            (_rdk_document_data),
  _environments   (_rdk_document_data)
{
  operator = (p);
}

const MYON_3dmRenderSettingsPrivate& MYON_3dmRenderSettingsPrivate::operator = (const MYON_3dmRenderSettingsPrivate& p)
{
  if (this != &p)
  {
    // Copy the XML.
    _rdk_document_data = p._rdk_document_data;

    // Check that all of the document objects now have matching properties.
    MYON_ASSERT(_dithering       == p._dithering);
    MYON_ASSERT(_ground_plane    == p._ground_plane);
    MYON_ASSERT(_linear_workflow == p._linear_workflow);
    MYON_ASSERT(_render_channels == p._render_channels);
    MYON_ASSERT(_safe_frame      == p._safe_frame);
    MYON_ASSERT(_skylight        == p._skylight);
    MYON_ASSERT(_sun             == p._sun);
    MYON_ASSERT(_environments    == p._environments);
  }

  return *this;
}

MYON_3dmRenderSettings::~MYON_3dmRenderSettings()
{
  if (nullptr != m_private)
  {
    delete m_private;
    m_private = nullptr;
  }
}

MYON_3dmRenderSettings::MYON_3dmRenderSettings(const MYON_3dmRenderSettings& rs)
{
  operator = (rs);
}

MYON_3dmRenderSettings& MYON_3dmRenderSettings::operator = (const MYON_3dmRenderSettings& rs)
{
  if (this != &rs)
  {
    MYON_Object::operator = (rs);

    if (nullptr != rs.m_private)
    {
      if (nullptr == m_private)
      {
        m_private = new MYON_3dmRenderSettingsPrivate(*rs.m_private);
      }
      else
      {
        *m_private = *rs.m_private;
      }
    }
    else
    {
      if (nullptr != m_private)
      {
        // We can't delete the private because it might have specialized document objects (gp etc).
        // So don't delete it, but set it to defaults because the incoming one doesn't exist which
        // is essentially the same as the incoming one being an implied default one.
        m_private->SetToDefaults();
      }
    }

    m_ambient_light              = rs.m_ambient_light;
    m_antialias_style            = rs.m_antialias_style;
    m_background_bitmap_filename = rs.m_background_bitmap_filename;
    m_background_bottom_color    = rs.m_background_bottom_color;
    m_background_color           = rs.m_background_color;
    m_background_style           = rs.m_background_style;
    m_bCustomImageSize           = rs.m_bCustomImageSize;
    m_bDepthCue                  = rs.m_bDepthCue;
    m_bFlatShade                 = rs.m_bFlatShade;
    m_bForceViewportAspectRatio  = rs.m_bForceViewportAspectRatio;
    m_bRenderAnnotation          = rs.m_bRenderAnnotation;
    m_bRenderBackfaces           = rs.m_bRenderBackfaces;
    m_bRenderCurves              = rs.m_bRenderCurves;
    m_bRenderIsoparams           = rs.m_bRenderIsoparams;
    m_bRenderMeshEdges           = rs.m_bRenderMeshEdges;
    m_bRenderPoints              = rs.m_bRenderPoints;
    m_bScaleBackgroundToFit      = rs.m_bScaleBackgroundToFit;
    m_bTransparentBackground     = rs.m_bTransparentBackground;
    m_bUseHiddenLights           = rs.m_bUseHiddenLights;
    m_bUsesAmbientAttr           = rs.m_bUsesAmbientAttr;
    m_bUsesAnnotationAttr        = rs.m_bUsesAnnotationAttr;
    m_bUsesBackfaceAttr          = rs.m_bUsesBackfaceAttr;
    m_bUsesBackgroundAttr        = rs.m_bUsesBackgroundAttr;
    m_bUsesCurvesAttr            = rs.m_bUsesCurvesAttr;
    m_bUsesHiddenLightsAttr      = rs.m_bUsesHiddenLightsAttr;
    m_bUsesIsoparmsAttr          = rs.m_bUsesIsoparmsAttr;
    m_bUsesMeshEdgesAttr         = rs.m_bUsesMeshEdgesAttr;
    m_bUsesPointsAttr            = rs.m_bUsesPointsAttr;
    m_embedded_image_file_id     = rs.m_embedded_image_file_id;
    m_image_dpi                  = rs.m_image_dpi;
    m_image_height               = rs.m_image_height;
    m_image_us                   = rs.m_image_us;
    m_image_width                = rs.m_image_width;
    m_named_view                 = rs.m_named_view;
    m_rendering_source           = rs.m_rendering_source;
    m_reserved1                  = rs.m_reserved1;
    m_reserved3                  = rs.m_reserved3;
    m_reserved4                  = rs.m_reserved4;
    m_reserved5                  = rs.m_reserved5;
    m_reserved6                  = rs.m_reserved6;
    m_reserved7                  = rs.m_reserved7;
    m_reserved8                  = rs.m_reserved8;
    m_shadowmap_height           = rs.m_shadowmap_height;
    m_shadowmap_offset           = rs.m_shadowmap_offset;
    m_shadowmap_style            = rs.m_shadowmap_style;
    m_shadowmap_width            = rs.m_shadowmap_width;
    m_snapshot                   = rs.m_snapshot;
    m_specific_viewport          = rs.m_specific_viewport;
  }

  return *this;
}

void MYON_3dmRenderSettingsPrivate::SetToDefaults(void)
{
  MYON_RdkDocumentDefaults dd(MYON::VersionMajor(), MYON_RdkDocumentDefaults::ValueSets::All);
  dd.CopyDefaultsTo(_rdk_document_data);
}

void MYON_3dmRenderSettings::Dump( MYON_TextLog& text_log ) const
{
  text_log.Print("m_bCustomImageSize = %s\n",m_bCustomImageSize?"true":"false");
  text_log.Print("m_image_width = %d\n",m_image_width);
  text_log.Print("m_image_height = %d\n",m_image_height);
  text_log.Print("m_image_dpi = %g\n",m_image_dpi);
  text_log.Print("m_image_us = %d\n",m_image_us);
  text_log.Print("m_ambient_light rgb = ");text_log.PrintRGB(m_ambient_light);text_log.Print("\n");
  text_log.Print("m_background_style = %d\n",m_background_style);
  text_log.Print("m_background_color rgb = ");text_log.PrintRGB(m_background_color);text_log.Print("\n");
  text_log.Print("m_background_bitmap_filename = ");text_log.Print(m_background_bitmap_filename);text_log.Print("\n");
  text_log.Print("m_bUseHiddenLights = %s\n",m_bUseHiddenLights?"true":"false");
  text_log.Print("m_bDepthCue = %s\n",m_bDepthCue?"true":"false");
  text_log.Print("m_bFlatShade = %s\n",m_bFlatShade?"true":"false");
  text_log.Print("m_bRenderBackfaces = %s\n",m_bRenderBackfaces?"true":"false");
  text_log.Print("m_bRenderPoints = %s\n",m_bRenderPoints?"true":"false");
  text_log.Print("m_bRenderCurves = %s\n",m_bRenderCurves?"true":"false");
  text_log.Print("m_bRenderIsoparams = %s\n",m_bRenderIsoparams?"true":"false");
  text_log.Print("m_bRenderMeshEdges = %s\n",m_bRenderMeshEdges?"true":"false");
  text_log.Print("m_bRenderAnnotation = %s\n",m_bRenderAnnotation?"true":"false");

  text_log.Print("m_antialias_style = %d\n",m_antialias_style);
  text_log.Print("m_shadowmap_style = %d\n",m_shadowmap_style);
  text_log.Print("m_shadowmap_width = %d\n",m_shadowmap_width);
  text_log.Print("m_shadowmap_height = %d\n",m_shadowmap_height);
  text_log.Print("m_shadowmap_offset = %g\n",m_shadowmap_offset);

  text_log.Print("m_bScaleBackgroundToFit = %s\n",m_bScaleBackgroundToFit?"true":"false");

  text_log.Print(L"m_rendering_source = %u\n", int(m_rendering_source));
  text_log.Print(L"m_specific_viewport = %s\n", (const wchar_t*)m_specific_viewport);
  text_log.Print(L"m_named_view = %s\n", (const wchar_t*)m_named_view);
  text_log.Print(L"m_snapshot = %s\n", (const wchar_t*)m_snapshot);

  text_log.Print("m_bForceViewportAspectRatio = %s\n", m_bForceViewportAspectRatio ? "true" : "false");
}

bool MYON_3dmRenderSettings::UseV5ReadWrite(const MYON_BinaryArchive& file)
{
  if ( file.Archive3dmVersion() <= 50 )
    return true;
  
  if ( file.Archive3dmVersion() > 60 )
    return false;

  // Prior to  November 5, 2013, version 6 files used the old V5 format.
  unsigned int v6_2013_11_05_version = MYON_VersionNumberConstruct(6,0,2013,11,5,0);
  unsigned int archive_opennurbs_version = file.ArchiveOpenNURBSVersion();
  return (archive_opennurbs_version < v6_2013_11_05_version);
}

bool MYON_3dmRenderSettings::Write( MYON_BinaryArchive& file ) const
{
  if ( MYON_3dmRenderSettings::UseV5ReadWrite(file) )
    return WriteV5(file);

  // November 5, 2013 V6 files and later
  // March 11th 2016 "1.1"- adds focal blur data
  // June  20th 2017 "1.2"- adds rendering source data
  if ( !file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,3) )
    return false;

  bool rc = false;
  for(;;)
  {
    if ( !file.WriteBool( m_bCustomImageSize ) ) break;
    if ( !file.WriteInt( m_image_width ) ) break;
    if ( !file.WriteInt( m_image_height ) ) break;

    if ( !file.WriteDouble( m_image_dpi ) ) break;
    if ( !file.WriteInt( (int)m_image_us ) ) break;

    if ( !file.WriteColor( m_ambient_light ) ) break;

    if ( !file.WriteInt( m_background_style ) ) break;
    if ( !file.WriteColor( m_background_color ) ) break;
    if ( !file.WriteColor( m_background_bottom_color ) ) break;
    if ( !file.WriteString( m_background_bitmap_filename ) ) break;

    if ( !file.WriteBool( m_bUseHiddenLights ) ) break;
    if ( !file.WriteBool( m_bDepthCue ) ) break;
    if ( !file.WriteBool( m_bFlatShade ) ) break;
    if ( !file.WriteBool( m_bRenderBackfaces ) ) break;
    if ( !file.WriteBool( m_bRenderPoints ) ) break;
    if ( !file.WriteBool( m_bRenderCurves ) ) break;
    if ( !file.WriteBool( m_bRenderIsoparams ) ) break;
    if ( !file.WriteBool( m_bRenderMeshEdges ) ) break;
    if ( !file.WriteBool( m_bRenderAnnotation ) ) break;
    if ( !file.WriteBool( m_bScaleBackgroundToFit ) ) break;
    if ( !file.WriteBool( m_bTransparentBackground ) ) break;

    if ( !file.WriteInt( m_antialias_style ) ) break;
    if ( !file.WriteInt( m_shadowmap_style ) ) break;
    if ( !file.WriteInt( m_shadowmap_width ) ) break;
    if ( !file.WriteInt( m_shadowmap_height ) ) break;
    if ( !file.WriteDouble( m_shadowmap_offset ) ) break;

	// March 11th 2016 "1.1"- adds focal blur data
	// ALB. June 24th 2017 - this information is now on MYON_3dmView.  Write default values.  Note, these values were never in Rhino 5.0 files.
	//http://mcneel.myjetbrains.com/youtrack/issue/RH-32342
	if (!file.WriteInt((int)0)) break;
	if (!file.WriteDouble(100.0)) break;
	if (!file.WriteDouble(64.0)) break;
	if (!file.WriteDouble(0.1)) break;
	if (!file.WriteInt(10)) break;

	// June 20th 2017 "1.2"- adds rendering source data
	//https://mcneel.myjetbrains.com/youtrack/issue/RH-39593
	if (!file.WriteInt((int)m_rendering_source)) break;
	if (!file.WriteString(m_specific_viewport)) break;
	if (!file.WriteString(m_named_view)) break;
	if (!file.WriteString(m_snapshot)) break;

  // September 28th 2017 "1.3"- adds aspect ratio lock
  if (!file.WriteBool(m_bForceViewportAspectRatio)) break;

    rc = true;
    break;
  }  

  if ( !file.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_3dmRenderSettings::WriteV5( MYON_BinaryArchive& file ) const
{
  unsigned int i;
  // version 103: 11 November 2010
  const int version = 103;
  bool rc = file.WriteInt( version );
  // version >= 100
  if (rc) rc = file.WriteInt( m_bCustomImageSize );
  if (rc) rc = file.WriteInt( m_image_width );
  if (rc) rc = file.WriteInt( m_image_height );
  if (rc) rc = file.WriteColor( m_ambient_light );
  if (rc) rc = file.WriteInt( m_background_style );
  if (rc) rc = file.WriteColor( m_background_color );
  if (rc) rc = file.WriteString( m_background_bitmap_filename );
  if (rc) rc = file.WriteInt( m_bUseHiddenLights );
  if (rc) rc = file.WriteInt( m_bDepthCue );
  if (rc) rc = file.WriteInt( m_bFlatShade );

  // 26 August 2003 Dale Lear:
  //     When saving V2 files, turn on backfaces. RR 11656
  //
  i = (file.Archive3dmVersion() >= 3) ? m_bRenderBackfaces : 1;
  if (rc) rc = file.WriteInt( i );

  if (rc) rc = file.WriteInt( m_bRenderPoints );
  if (rc) rc = file.WriteInt( m_bRenderCurves );
  if (rc) rc = file.WriteInt( m_bRenderIsoparams );
  if (rc) rc = file.WriteInt( m_bRenderMeshEdges );
  if (rc) rc = file.WriteInt( m_bRenderAnnotation );
  if (rc) rc = file.WriteInt( m_antialias_style );
  if (rc) rc = file.WriteInt( m_shadowmap_style );
  if (rc) rc = file.WriteInt( m_shadowmap_width );
  if (rc) rc = file.WriteInt( m_shadowmap_height );
  if (rc) rc = file.WriteDouble( m_shadowmap_offset );
  // version >= 101 begins here
  if (rc) rc = file.WriteDouble( m_image_dpi );
  i = static_cast<unsigned int>(m_image_us);
  if (rc) rc = file.WriteInt( i );
  // version >= 102 begins here
  if (rc) rc = file.WriteColor( m_background_bottom_color );

  // version >= 103 begins here - added 11 November 2010
  if (rc) rc = file.WriteBool( m_bScaleBackgroundToFit );

  return rc;
}

bool MYON_3dmRenderSettings::Read( MYON_BinaryArchive& file )
{
  *this = MYON_3dmRenderSettings::Default;
  if ( MYON_3dmRenderSettings::UseV5ReadWrite(file) )
    return ReadV5(file);

  // November 5, 2013 V6 files and later
  int major_version = 0;
  int minor_version = 0;
  if ( !file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version) )
    return false;

  bool rc = false;
  int i;
  for(;;)
  {
    if ( 1 != major_version ) break;

    if ( !file.ReadBool( &m_bCustomImageSize ) ) break;
    if ( !file.ReadInt( &m_image_width ) ) break;
    if ( !file.ReadInt( &m_image_height ) ) break;

    if ( !file.ReadDouble( &m_image_dpi ) ) break;
    i = MYON_UNSET_UINT_INDEX;
    if ( !file.ReadInt( &i ) ) break;
    m_image_us = MYON::LengthUnitSystemFromUnsigned(i);

    if ( !file.ReadColor( m_ambient_light ) ) break;

    if ( !file.ReadInt( &m_background_style ) ) break;
    if ( !file.ReadColor( m_background_color ) ) break;
    if ( !file.ReadColor( m_background_bottom_color ) ) break;
    if ( !file.ReadString( m_background_bitmap_filename ) ) break;

    if ( !file.ReadBool( &m_bUseHiddenLights ) ) break;
    if ( !file.ReadBool( &m_bDepthCue ) ) break;
    if ( !file.ReadBool( &m_bFlatShade ) ) break;
    if ( !file.ReadBool( &m_bRenderBackfaces ) ) break;
    if ( !file.ReadBool( &m_bRenderPoints ) ) break;
    if ( !file.ReadBool( &m_bRenderCurves ) ) break;
    if ( !file.ReadBool( &m_bRenderIsoparams ) ) break;
    if ( !file.ReadBool( &m_bRenderMeshEdges ) ) break;
    if ( !file.ReadBool( &m_bRenderAnnotation ) ) break;
    if ( !file.ReadBool( &m_bScaleBackgroundToFit ) ) break;
    if ( !file.ReadBool( &m_bTransparentBackground ) ) break;

    if ( !file.ReadInt( &m_antialias_style ) ) break;
    if ( !file.ReadInt( &m_shadowmap_style ) ) break;
    if ( !file.ReadInt( &m_shadowmap_width ) ) break;
    if ( !file.ReadInt( &m_shadowmap_height ) ) break;
    if ( !file.ReadDouble( &m_shadowmap_offset ) ) break;

	// March 11th 2016 "1.1"- adds focal blur data
	//http://mcneel.myjetbrains.com/youtrack/issue/RH-32342
	if (minor_version >= 1)
	{
		// ALB. June 24th 2017 - this information is now on MYON_3dmView.  Reads the values out, but doesn't store them.
		i = MYON_UNSET_UINT_INDEX;
		double d;
		if (!file.ReadInt( &i)) break;

		if (!file.ReadDouble(&d)) break;
		if (!file.ReadDouble(&d)) break;
		if (!file.ReadDouble(&d)) break;
		if (!file.ReadInt(&i)) break;
	}

	// June 20th 2017 "1.2"- adds rendering source data
	// https://mcneel.myjetbrains.com/youtrack/issue/RH-39593
	if (minor_version >= 2)
	{
		i = MYON_UNSET_UINT_INDEX;
		if (!file.ReadInt( &i)) break;
		m_rendering_source = (RenderingSources)i;

		if (!file.ReadString(m_specific_viewport)) break;
		if (!file.ReadString(m_named_view)) break;
		if (!file.ReadString(m_snapshot)) break;
	}

  // September 28th 2017 "1.3"- adds aspect ratio lock
  // https://mcneel.myjetbrains.com/youtrack/issue/RH-41608
  if (minor_version >= 3)
  {
    if (!file.ReadBool(&m_bForceViewportAspectRatio)) break;
  }

    rc = true;
    break;
  }  

  if ( !file.EndRead3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_3dmRenderSettings::ReadV5( MYON_BinaryArchive& file )
{
  bool rc;
  int b, i;
  int version;
  rc = false;
  for(;;)
  {
    version = 0;
    if ( !file.ReadInt( &version ) )
      break;
    if ( version < 100 )
      break;
    if ( version >= 200 )
      break;
    if ( !file.ReadInt( &b ) )
      break;
    m_bCustomImageSize = (0 != b);
    if ( !file.ReadInt( &m_image_width ) )
      break;
    if ( !file.ReadInt( &m_image_height ) )
      break;
    if ( !file.ReadColor( m_ambient_light ) )
      break;
    if ( !file.ReadInt( &m_background_style ) )
      break;
    if ( !file.ReadColor( m_background_color ) )
      break;
    if ( !file.ReadString( m_background_bitmap_filename ) )
      break;
    if ( !file.ReadInt( &b ) )
      break;
    m_bUseHiddenLights = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bDepthCue = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bFlatShade = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bRenderBackfaces = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bRenderPoints = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bRenderCurves = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bRenderIsoparams = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bRenderMeshEdges = (0 != b);
    if ( !file.ReadInt( &b ) )
      break;
    m_bRenderAnnotation = (0 != b);
    if ( !file.ReadInt( &m_antialias_style ) )
      break;
    if ( !file.ReadInt( &m_shadowmap_style ) )
      break;
    if ( !file.ReadInt( &m_shadowmap_width ) )
      break;
    if ( !file.ReadInt( &m_shadowmap_height ) )
      break;
    if ( !file.ReadDouble( &m_shadowmap_offset ) )
      break;
    
    if ( version < 101 )
    {
      rc = true;
      break;
    }
    if ( !file.ReadDouble( &m_image_dpi ) )
      break;
    if ( !file.ReadInt(&i) )
      break;
    m_image_us = MYON::LengthUnitSystemFromUnsigned(i);
     
    
    if ( version < 102 )
    {
      rc = true;
      break;
    }
    if ( !file.ReadColor( m_background_bottom_color ) )
      break;

    if ( version < 103 )
    {
      rc = true;
      break;
    }
    if ( !file.ReadBool( &m_bScaleBackgroundToFit ) )
      break;

    rc = true;
    break;
  }

  return rc;
}

MYON_3dmRenderSettings::RenderingSources MYON_3dmRenderSettings::RenderingSource(void) const
{
	return m_rendering_source;
}

void MYON_3dmRenderSettings::SetRenderingSource(MYON_3dmRenderSettings::RenderingSources rs)
{
	m_rendering_source = rs;
}

MYON_wString MYON_3dmRenderSettings::SpecificViewport(void) const
{
	return m_specific_viewport;
}

void MYON_3dmRenderSettings::SetSpecificViewport(const MYON_wString& s)
{
	m_specific_viewport = s;
}

MYON_wString MYON_3dmRenderSettings::NamedView(void) const
{
	return m_named_view;
}

void MYON_3dmRenderSettings::SetNamedView(const MYON_wString& s)
{
	m_named_view = s;
}

MYON_wString MYON_3dmRenderSettings::Snapshot(void) const
{
	return m_snapshot;
}

void MYON_3dmRenderSettings::SetSnapshot(const MYON_wString& s)
{
	m_snapshot = s;
}

bool MYON_3dmRenderSettings::ScaleBackgroundToFit() const
{
  return m_bScaleBackgroundToFit;
}

void MYON_3dmRenderSettings::SetScaleBackgroundToFit( bool bScaleBackgroundToFit )
{
  // The "? true : false" is here to prevent hacks from using a bool
  // to store settings besides 1 and 0.
  m_bScaleBackgroundToFit = bScaleBackgroundToFit?true:false;
}

MYON_SafeFrame& MYON_3dmRenderSettings::SafeFrame(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_safe_frame;
}

MYON_GroundPlane& MYON_3dmRenderSettings::GroundPlane(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_ground_plane;
}

MYON_LinearWorkflow& MYON_3dmRenderSettings::LinearWorkflow(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_linear_workflow;
}

MYON_Skylight& MYON_3dmRenderSettings::Skylight(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_skylight;
}

MYON_Sun& MYON_3dmRenderSettings::Sun(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_sun;
}

MYON_Dithering& MYON_3dmRenderSettings::Dithering(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_dithering;
}

MYON_RenderChannels& MYON_3dmRenderSettings::RenderChannels(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_render_channels;
}

MYON_XMLNode& MYON_3dmRenderSettings::RdkDocNode(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_rdk_document_data;
}

MYON_UUID MYON_3dmRenderSettings::BackgroundRenderEnvironment(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_environments.BackgroundRenderEnvironment();
}

void MYON_3dmRenderSettings::SetBackgroundRenderEnvironment(const MYON_UUID& id)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  m_private->_environments.SetBackgroundRenderEnvironment(id);
}

bool MYON_3dmRenderSettings::SkylightingRenderEnvironmentOverride(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_environments.SkylightingRenderEnvironmentOverride();
}

void MYON_3dmRenderSettings::SetSkylightingRenderEnvironmentOverride(bool on)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  m_private->_environments.SetSkylightingRenderEnvironmentOverride(on);
}

MYON_UUID MYON_3dmRenderSettings::SkylightingRenderEnvironment(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_environments.SkylightingRenderEnvironment();
}

void MYON_3dmRenderSettings::SetSkylightingRenderEnvironment(const MYON_UUID& id)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  m_private->_environments.SetSkylightingRenderEnvironment(id);
}

bool MYON_3dmRenderSettings::ReflectionRenderEnvironmentOverride(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_environments.ReflectionRenderEnvironmentOverride();
}

void MYON_3dmRenderSettings::SetReflectionRenderEnvironmentOverride(bool on)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  m_private->_environments.SetReflectionRenderEnvironmentOverride(on);
}

MYON_UUID MYON_3dmRenderSettings::ReflectionRenderEnvironment(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  return m_private->_environments.ReflectionRenderEnvironment();
}

void MYON_3dmRenderSettings::SetReflectionRenderEnvironment(const MYON_UUID& id)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmRenderSettingsPrivate;

  m_private->_environments.SetReflectionRenderEnvironment(id);
}

extern MYON_UUID uuidRenderSettingsPreset_Studio;
extern MYON_UUID uuidRenderSettingsPreset_Custom;
extern MYON_UUID uuidRenderSettingsPreset_Exterior;
extern MYON_UUID uuidRenderSettingsPreset_Interior;

static const wchar_t* rendering = MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_RENDERING;

MYON_UUID MYON_3dmRenderSettings::CurrentRenderingPreset(void) const
{
  const auto* node = RdkDocNode().GetNodeAtPath(rendering);
  if (nullptr != node)
  {
    MYON_XMLParameters p(*node);
    MYON_XMLVariant value;
    if (p.GetParam(MYON_RDK_CURRENT_PRESET, value))
      return value.AsUuid();
  }

  return MYON_nil_uuid;
}

void MYON_3dmRenderSettings::SetCurrentRenderingPreset(const MYON_UUID& uuid)
{
  MYON_ASSERT((uuidRenderSettingsPreset_Studio == uuid) || (uuidRenderSettingsPreset_Custom == uuid) || (uuidRenderSettingsPreset_Exterior == uuid) || (uuidRenderSettingsPreset_Interior == uuid));

  auto* node = RdkDocNode().GetNodeAtPath(rendering);
  if (nullptr != node)
  {
    MYON_XMLParameters p(*node);
    p.SetParam(MYON_RDK_CURRENT_PRESET, uuid);
  }
}

void MYON_3dmRenderSettings::GetRenderingPresets(MYON_SimpleArray<MYON_UUID>& presets) const
{
  presets.Append(uuidRenderSettingsPreset_Studio);
  presets.Append(uuidRenderSettingsPreset_Exterior);
  presets.Append(uuidRenderSettingsPreset_Interior);
  presets.Append(uuidRenderSettingsPreset_Custom);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmAnnotationSettings
//

void MYON_3dmAnnotationSettings::Dump( MYON_TextLog& text_log ) const
{
  // TODO
}

double MYON_3dmAnnotationSettings::WorldViewTextScale() const
{
  return m_world_view_text_scale;
}

double MYON_3dmAnnotationSettings::WorldViewHatchScale() const
{
  return m_world_view_hatch_scale;
}

void MYON_3dmAnnotationSettings::SetWorldViewTextScale(double world_view_text_scale )
{
  if ( MYON_IsValid(world_view_text_scale) && world_view_text_scale > 0.0 )
    m_world_view_text_scale = (float)world_view_text_scale;
}

void MYON_3dmAnnotationSettings::SetWorldViewHatchScale(double world_view_hatch_scale )
{
  if ( MYON_IsValid(world_view_hatch_scale) && world_view_hatch_scale > 0.0 )
    m_world_view_hatch_scale = (float)world_view_hatch_scale;
}

bool MYON_3dmAnnotationSettings::Is_V5_AnnotationScalingEnabled() const
{
  return m_b_V5_EnableAnnotationScaling?true:false;
}

void MYON_3dmAnnotationSettings::Enable_V5_AnnotationScaling( bool bEnable )
{
  m_b_V5_EnableAnnotationScaling = bEnable?1:0;
}

bool MYON_3dmAnnotationSettings::IsModelSpaceAnnotationScalingEnabled() const
{
  return m_bEnableModelSpaceAnnotationScaling ? true : false;
}

void MYON_3dmAnnotationSettings::EnableModelSpaceAnnotationScaling(bool bEnable)
{
  m_bEnableModelSpaceAnnotationScaling = bEnable ? 1 : 0;
}


bool MYON_3dmAnnotationSettings::IsLayoutSpaceAnnotationScalingEnabled() const
{
  return m_bEnableLayoutSpaceAnnotationScaling ? true : false;
}

void MYON_3dmAnnotationSettings::EnableLayoutSpaceAnnotationScaling(bool bEnable)
{
  m_bEnableLayoutSpaceAnnotationScaling = bEnable ? 1 : 0;
}

bool MYON_3dmAnnotationSettings::IsHatchScalingEnabled() const
{
  return m_bEnableHatchScaling?true:false;
}

void MYON_3dmAnnotationSettings::EnableHatchScaling( bool bEnable )
{
  m_bEnableHatchScaling = bEnable?1:0;
}


bool MYON_3dmAnnotationSettings::Read( MYON_BinaryArchive& file )
{
  *this = MYON_3dmAnnotationSettings::Default;

  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( major_version == 1 ) {
    if (minor_version >= 0) {
      if (rc) rc = file.ReadDouble(&m_dimscale);
      if (rc) rc = file.ReadDouble(&m_textheight);
      if (rc) rc = file.ReadDouble(&m_dimexe);
      if (rc) rc = file.ReadDouble(&m_dimexo);
      if (rc) rc = file.ReadDouble(&m_arrowlength);
      if (rc) rc = file.ReadDouble(&m_arrowwidth);
      if (rc) rc = file.ReadDouble(&m_centermark);

      {
        unsigned int i;
        if (rc)
        {
          rc = file.ReadInt(&i);
          if (rc)
            m_dimunits = MYON::LengthUnitSystemFromUnsigned(i);
        }
      }

      if (rc) rc = file.ReadInt(&m_arrowtype);
      if (rc) rc = file.ReadInt(&m_angularunits);
      if (rc) rc = file.ReadInt(&m_lengthformat);
      if (rc) rc = file.ReadInt(&m_angleformat);

      unsigned int obsolete_value = 0;
      if (rc) rc = file.ReadInt(&obsolete_value);

      if (rc) rc = file.ReadInt(&m_resolution);

      if (rc) rc = file.ReadString(m_facename);

      bool bV6orLater = file.Archive3dmVersion() >= 60;
      bool bV5 = !bV6orLater && file.Archive3dmVersion() >= 5;

      if (bV6orLater)
      {
        // files that do not contain m_bEnableAnnotationScaling,
        // set m_bEnableAnnotationScaling = false so the display 
        // image does not change.
        m_b_V5_EnableAnnotationScaling = 1;

        // files that do not contain m_bEnableModelSpaceAnnotationScaling,
        // set m_bEnableModelSpaceAnnotationScaling = true so the display 
        // image does not change.
        //*********** This is probably right for v5 files, but not for pre-V5 *************
        m_bEnableModelSpaceAnnotationScaling = 1;

        // files that do not contain m_bEnableLayoutSpaceAnnotationScaling,
        // set m_bEnableLayoutAnnotationScaling = false so the display 
        // image does not change.
        // ********** This should be set from m_b_V5_EnableAnnotationScaling for V5 files   *************
        m_bEnableLayoutSpaceAnnotationScaling = 1;
      }
      else if (bV5)
      {
        m_bEnableModelSpaceAnnotationScaling = 1;
      }
      else
      {
        // v4 or earlier - no layout or model space scaling
        m_b_V5_EnableAnnotationScaling = 0;
        m_bEnableModelSpaceAnnotationScaling = 0;
        m_bEnableLayoutSpaceAnnotationScaling = 0;
      }

      // files that do not contain m_bEnableHatchScaling,
      // set m_bEnableHatchScaling = false so the display
      // image does not change.
      m_bEnableHatchScaling = 0;

      if ( minor_version >= 1 )
      {
        // Added 25 August 2010 chunk version 1.1
        double d = m_world_view_text_scale;
        if (rc) rc = file.ReadDouble(&d);
        if (rc && MYON_IsValid(d) && d >= 0.0 ) m_world_view_text_scale = (float)d;
        if (rc) rc = file.ReadChar(&m_b_V5_EnableAnnotationScaling);
        if (rc)
        {
          if (m_b_V5_EnableAnnotationScaling)
          {
            m_bEnableLayoutSpaceAnnotationScaling = 1;
          }
          else
          {
            m_bEnableLayoutSpaceAnnotationScaling = 0;
          }
        }

        if ( minor_version >= 2 )
        {
          d = m_world_view_hatch_scale;
          if (rc) rc = file.ReadDouble(&d);
          if (rc && MYON_IsValid(d) && d >= 0.0) m_world_view_hatch_scale = (float)d;
          if (rc) rc = file.ReadChar(&m_bEnableHatchScaling);
          if (minor_version >= 3)
          {
            // [Lowell 3-28-2013] New fields for V6
            if (rc) rc = file.ReadChar(&m_bEnableModelSpaceAnnotationScaling);
            if (rc) rc = file.ReadChar(&m_bEnableLayoutSpaceAnnotationScaling);
          }
        }
      }
    }
  }
  else {
    rc = false;
  }
  return rc;
}

bool MYON_3dmAnnotationSettings::Write( MYON_BinaryArchive& file ) const
{
  int minor_version
    = file.Archive3dmVersion() >= 60
    ? 3
    : 2;

  unsigned int i;
  bool rc = file.Write3dmChunkVersion(1, minor_version);
  // March 22, 2010 - Global DimScale abandoned and moved into DimStyles, so now
  // in older files, the dimscale values are multiplied into the DimStyle lengths and
  // DimScale is written as 1.0

  // March 2017 Add
  /*
    unsigned char m_b_V5_EnableAnnotationScaling = 1;

  // [Lowell 3-28-2013] New fields for V6
  unsigned char m_bEnableModelSpaceAnnotationScaling = 1;
  unsigned char m_bEnableLayoutSpaceAnnotationScaling = 1;
  */
  if (rc) rc = file.WriteDouble(1.0);

  if (rc) rc = file.WriteDouble(m_textheight);
  if (rc) rc = file.WriteDouble(m_dimexe);
  if (rc) rc = file.WriteDouble(m_dimexo);
  if (rc) rc = file.WriteDouble(m_arrowlength);
  if (rc) rc = file.WriteDouble(m_arrowwidth);
  if (rc) rc = file.WriteDouble(m_centermark);

  i = static_cast<unsigned int>(m_dimunits);
  if (rc) rc = file.WriteInt( i );
  if (rc) rc = file.WriteInt( m_arrowtype );
  if (rc) rc = file.WriteInt( m_angularunits );
  if (rc) rc = file.WriteInt( m_lengthformat );
  if (rc) rc = file.WriteInt( m_angleformat );

  const unsigned int textalign 
    = (file.Archive3dmVersion() <= 2)
    ? 0
    : 2;
  if (rc) rc = file.WriteInt( textalign );

  if (rc) rc = file.WriteInt( m_resolution );

  if (rc) rc = file.WriteString( m_facename );

  // Added 25 August 2010 chunk version 1.1
  double d = m_world_view_text_scale;
  if (rc) rc = file.WriteDouble(d);
  if (rc) rc = file.WriteChar(m_b_V5_EnableAnnotationScaling);

  // Added 14 January 2011 chunk version 1.2
  d = m_world_view_hatch_scale;
  if (rc) rc = file.WriteDouble(d);
  if (rc) rc = file.WriteChar(m_bEnableHatchScaling);

  if (minor_version >= 3)
  {
    // [Lowell 3-28-2013] New fields for V6
    if (rc) rc = file.WriteChar(m_bEnableModelSpaceAnnotationScaling);
    if (rc) rc = file.WriteChar(m_bEnableLayoutSpaceAnnotationScaling);
  }

  return rc;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmConstructionPlane
//
MYON_3dmConstructionPlane::MYON_3dmConstructionPlane()
{
  Default();
}

MYON_3dmConstructionPlane::MYON_3dmConstructionPlane(const MYON_Plane& plane)
{
  Default();
  m_plane = plane;
}

MYON_3dmConstructionPlane::~MYON_3dmConstructionPlane()
{
}

// default copy constructor and operator= work fine
/*
MYON_3dmConstructionPlane::MYON_3dmConstructionPlane(const MYON_3dmConstructionPlane& src)
{
  Default();
  *this = src;
}
MYON_3dmConstructionPlane& MYON_3dmConstructionPlane::operator=(const MYON_3dmConstructionPlane& src)
{
  if ( this != &src ) 
  {
    m_plane = src.m_plane;
    m_grid_spacing = src.m_grid_spacing;
    m_snap_spacing = src.m_snap_spacing;
    m_grid_line_count = src.m_grid_line_count;
    m_grid_thick_frequency = src.m_grid_thick_frequency;
    m_name = src.m_name;
    m_bDepthBuffer = src.m_bDepthBuffer;
  }
  return *this;
}
*/

void MYON_3dmConstructionPlane::Dump( MYON_TextLog& text_log ) const
{
  // TODO
}

void MYON_3dmConstructionPlane::Default()
{
  m_name.Destroy();
  m_plane = MYON_xy_plane;

  // construction grid appearance
	m_grid_spacing = 1.0;   // distance between grid lines
	m_snap_spacing = 1.0;   // distance between grid snap points
	m_grid_line_count = 70;     // number of grid lines in each direction
  m_grid_thick_frequency = 5; // thick line frequency
  m_bDepthBuffer = true;
}

bool MYON_3dmConstructionPlane::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.Write3dmChunkVersion(1,1);

  if (rc) rc = file.WritePlane(m_plane);
  if (rc) rc = file.WriteDouble(m_grid_spacing);
  if (rc) rc = file.WriteDouble(m_snap_spacing);
  if (rc) rc = file.WriteInt(m_grid_line_count);
  if (rc) rc = file.WriteInt(m_grid_thick_frequency);
  if (rc) rc = file.WriteString(m_name);

  // added for version 1.1 chunks
  if (rc) rc = file.WriteBool(m_bDepthBuffer);

  return rc;
}

bool MYON_3dmConstructionPlane::Read( MYON_BinaryArchive& file )
{
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc && major_version==1) 
  {
    if (rc) rc = file.ReadPlane(m_plane);
    if (rc) rc = file.ReadDouble(&m_grid_spacing);
    if (rc) rc = file.ReadDouble(&m_snap_spacing);
    if (rc) rc = file.ReadInt(&m_grid_line_count);
    if (rc) rc = file.ReadInt(&m_grid_thick_frequency);
    if (rc) rc = file.ReadString(m_name);

    if ( minor_version >= 1 )
    {
      if (rc) rc = file.ReadBool(&m_bDepthBuffer);
    }
  }
  return rc;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmConstructionPlaneGridDefaults
//


void MYON_3dmConstructionPlaneGridDefaults::Dump(MYON_TextLog& text_log) const
{
  // TODO
}

bool MYON_3dmConstructionPlaneGridDefaults::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.Write3dmChunkVersion(1,0);
  if (rc) rc = file.WriteDouble(m_grid_spacing);
  if (rc) rc = file.WriteDouble(m_snap_spacing);
  if (rc) rc = file.WriteInt(m_grid_line_count);
  if (rc) rc = file.WriteInt(m_grid_thick_frequency);
  if (rc) rc = file.WriteInt(m_bShowGrid?1:0);
  if (rc) rc = file.WriteInt(m_bShowGridAxes?1:0);
  if (rc) rc = file.WriteInt(m_bShowWorldAxes?1:0);
  return rc;
}

bool MYON_3dmConstructionPlaneGridDefaults::Read( MYON_BinaryArchive& file )
{
  int major_version = 0;
  int minor_version = 0;
  
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  
  if (rc && major_version==1) 
  {
    if (rc) rc = file.ReadDouble(&m_grid_spacing);
    if (rc) rc = file.ReadDouble(&m_snap_spacing);
    if (rc) rc = file.ReadInt(&m_grid_line_count);
    if (rc) rc = file.ReadInt(&m_grid_thick_frequency);
    
    int b = m_bShowGrid ? 1 : 0;
    if (rc) rc = file.ReadInt(&b);
    m_bShowGrid = (b != 0);
    
    b = m_bShowGridAxes ? 1 : 0;
    if (rc) rc = file.ReadInt(&b);
    m_bShowGridAxes = (b != 0);
    
    b = m_bShowWorldAxes ? 1 : 0;
    if (rc) rc = file.ReadInt(&b);
    m_bShowWorldAxes = (b != 0);
  }

  return rc;
}



//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmViewPosition
//
MYON_3dmViewPosition::MYON_3dmViewPosition()
{
  Default();
}

MYON_3dmViewPosition::~MYON_3dmViewPosition()
{
}

MYON_3dmViewPosition::MYON_3dmViewPosition(const MYON_3dmViewPosition& src)
{
  Default();
  *this = src;
}

MYON_3dmViewPosition& MYON_3dmViewPosition::operator=(const MYON_3dmViewPosition& src)
{
  if ( this != &src ) {
    m_wnd_left   = src.m_wnd_left;
    m_wnd_right  = src.m_wnd_right;
    m_wnd_top    = src.m_wnd_top;
    m_wnd_bottom = src.m_wnd_bottom;
    m_bMaximized = src.m_bMaximized;
    m_floating_viewport = src.m_floating_viewport;

    // reserved fields are not used
    // m_reserved_1 = src.m_reserved_1;
    // m_reserved_2 = src.m_reserved_2;
    // m_reserved_3 = src.m_reserved_3;
  }
  return *this;
}

void MYON_3dmViewPosition::Default()
{
  m_wnd_left   = 0.0;
  m_wnd_right  = 1.0;
  m_wnd_top    = 0.0;
  m_wnd_bottom = 1.0;
  m_bMaximized = false;

  m_floating_viewport = 0;
  m_reserved_1 = 0;
  m_reserved_2 = 0;
  m_reserved_3 = 0;
}

bool MYON_3dmViewPosition::Write( MYON_BinaryArchive& file ) const
{
  int minor_version =  ( file.Archive3dmVersion() >= 5 ) ? 1 : 0;

  bool rc = file.Write3dmChunkVersion(1,minor_version);
  if (rc) 
  {
    int i = m_bMaximized ? 1 : 0;
    if (rc) rc = file.WriteInt( i );
    if (rc) rc = file.WriteDouble( m_wnd_left );
    if (rc) rc = file.WriteDouble( m_wnd_right );
    if (rc) rc = file.WriteDouble( m_wnd_top );
    if (rc) rc = file.WriteDouble( m_wnd_bottom );

    if ( minor_version >= 1 )
    {
      // 13 March 2009 S. Baer
      // version 1.1 added support for m_floating_viewport
      // in V5 files.  This info is not in V4 files because
      // they might use this to write userdata.
      if (rc) rc = file.WriteChar( m_floating_viewport );
    }
  }
  return rc;
}

bool MYON_3dmViewPosition::Read( MYON_BinaryArchive& file )
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  double x;
  Default();
  if (rc && major_version==1) 
  {
    int i = m_bMaximized ? 1 : 0;
    if (rc) rc = file.ReadInt( &i );
    if (rc) m_bMaximized = (i != 0) ? true : false;
    if (rc) rc = file.ReadDouble( &m_wnd_left );
    if (rc) rc = file.ReadDouble( &m_wnd_right );
    if (rc) rc = file.ReadDouble( &m_wnd_top );
    if (rc) rc = file.ReadDouble( &m_wnd_bottom );

    // 13 March 2009 S. Baer
    // version 1.1 added support for m_floating_viewport
    if( rc && minor_version >= 1 )
    {
      rc = file.ReadChar( &m_floating_viewport );
    }
  }

  // if people put bogus values in a file, tune them up to something that will work
  if ( m_wnd_left > m_wnd_right ) {
    x = m_wnd_left; m_wnd_left = m_wnd_right; m_wnd_right = x;
  }
  if ( m_wnd_left  < 0.0 ) 
    m_wnd_left  = 0.0; 
  if ( m_wnd_right >= 1.0 ) 
    m_wnd_right = 1.0;
  if ( m_wnd_left >= m_wnd_right ) {
    m_wnd_left = 0.0;
    m_wnd_right = 1.0;
  }
  
  if ( m_wnd_top > m_wnd_bottom ) {
    x = m_wnd_top; m_wnd_top = m_wnd_bottom; m_wnd_bottom = x;
  }
  if ( m_wnd_top  < 0.0 ) 
    m_wnd_top  = 0.0; 
  if ( m_wnd_bottom >= 1.0 )
    m_wnd_bottom = 1.0;
  if ( m_wnd_top >= m_wnd_bottom ) {
    m_wnd_top = 0.0;
    m_wnd_bottom = 1.0;
  }

  return rc;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmViewTraceImage
//
MYON_3dmViewTraceImage::MYON_3dmViewTraceImage()
{
  Default();
}

MYON_3dmViewTraceImage::~MYON_3dmViewTraceImage()
{
}

void MYON_3dmViewTraceImage::Default()
{
  m_plane = MYON_xy_plane;
  m_width = 0.0;
  m_height = 0.0;
  m_image_file_reference = MYON_FileReference::Unset;
  m_bGrayScale = true;
  m_bHidden = false;
  m_bFiltered = false;
}

bool MYON_3dmViewTraceImage::Write( MYON_BinaryArchive& file ) const
{
  // opennurbs version  < 200307300 - version 1.0 or 1.1 chunk
  // opennurbs version >= 200307300 - version 1.2 chunk
  const int minor_version = (file.Archive3dmVersion() >= 60) ? 4 : 3;
  bool rc = file.Write3dmChunkVersion(1,minor_version);
  if (rc) 
  {
    if (rc) rc = file.WriteString( m_image_file_reference.FullPath() );
    if (rc) rc = file.WriteDouble( m_width );
    if (rc) rc = file.WriteDouble( m_height );
    if (rc) rc = file.WritePlane( m_plane );

    // version 1.1
    if (rc) rc = file.WriteBool( m_bGrayScale );

    // version 1.2
    if (rc) rc = file.WriteBool( m_bHidden );
    
    // version 1.3
    if (rc) rc = file.WriteBool( m_bFiltered );

    if (rc && minor_version >= 4)
    {
      // version 1.4
      if (rc) rc = m_image_file_reference.Write(true, file);
    }
  }
  return rc;
}


bool MYON_3dmViewTraceImage::Read( MYON_BinaryArchive& file )
{
  // opennurbs version  < 200307300 - version 1.0 or 1.1 chunk
  // opennurbs version >= 200307300 - version 1.2 chunk
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc && major_version==1)
  {
    MYON_wString bitmap_filename;
    if (rc) rc = file.ReadString( bitmap_filename );
    if (rc)
      m_image_file_reference.SetFullPath(bitmap_filename, false);
    if (rc) rc = file.ReadDouble( &m_width );
    if (rc) rc = file.ReadDouble( &m_height );
    if (rc) rc = file.ReadPlane( m_plane );
    if ( minor_version >= 1 )
    {
      if (rc) rc = file.ReadBool(&m_bGrayScale);
      
      if ( minor_version >= 2 )
      {
        if (rc) rc = file.ReadBool(&m_bHidden);
        
        if ( minor_version >= 3 )
        {
          if (rc) rc = file.ReadBool( &m_bFiltered );

          if (rc && minor_version >= 4)
          {
            rc = m_image_file_reference.Read(file);
          }
        }
      }
    }
  }
  else
    rc = false;
  return rc;
}


bool MYON_3dmViewTraceImage::operator==( const MYON_3dmViewTraceImage& other ) const
{
  if ( m_plane != other.m_plane )
    return false;
  if ( m_width != other.m_width )
    return false;
  if ( m_height != other.m_height )
    return false;
  if( m_image_file_reference.FullPathHash() != other.m_image_file_reference.FullPathHash() )
    return false;
  if ( m_bHidden != other.m_bHidden )
    return false;
  if ( m_bGrayScale != other.m_bGrayScale )
    return false;
  if ( m_bFiltered != other.m_bFiltered )
    return false;
    
  return true;
}

bool MYON_3dmViewTraceImage::operator!=( const MYON_3dmViewTraceImage& other ) const
{
  return operator==(other) ? false : true;
}


MYON_3dmWallpaperImage::MYON_3dmWallpaperImage()
{
  Default();
}

MYON_3dmWallpaperImage::~MYON_3dmWallpaperImage()
{
}

bool MYON_3dmWallpaperImage::operator==( const MYON_3dmWallpaperImage& other ) const
{
  if ( m_image_file_reference.FullPathHash() != other.m_image_file_reference.FullPathHash() )
    return false;
  if ( m_bHidden != other.m_bHidden )
    return false;
  return ( m_bGrayScale == other.m_bGrayScale );
}

bool MYON_3dmWallpaperImage::operator!=( const MYON_3dmWallpaperImage& other ) const
{
  return operator==(other) ? false : true;
}

void MYON_3dmWallpaperImage::Default()
{
  m_image_file_reference = MYON_FileReference::Unset;
  m_bGrayScale = true;
  m_bHidden = false;
}

bool MYON_3dmWallpaperImage::Write( MYON_BinaryArchive& file ) const
{
  // version  < 200307300 - version 1.0 chunk
  // version >= 200307300 - version 1.1 chunk
  const int minor_version = (file.Archive3dmVersion() >= 60) ? 2 : 1;
  bool rc = file.Write3dmChunkVersion(1,minor_version);
  if (rc) 
  {
    if (rc) rc = file.WriteString( m_image_file_reference.FullPath() );
    if (rc) rc = file.WriteBool( m_bGrayScale );

    if (rc) rc = file.WriteBool( m_bHidden ); // added in 1.1 chunk

    if (rc && minor_version >= 2)
    {
      rc = m_image_file_reference.Write(true, file);
    }
  }
  return rc;
}

bool MYON_3dmWallpaperImage::Read( MYON_BinaryArchive& file )
{
  // version  < 200307300 - version 1.0 chunk
  // version >= 200307300 - version 1.1 chunk
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc && major_version==1)
  {
    MYON_wString bitmap_filename;
    if (rc) rc = file.ReadString( bitmap_filename );
    if (rc)
      m_image_file_reference.SetFullPath(bitmap_filename,false);
    if (rc) rc = file.ReadBool( &m_bGrayScale );

    if ( minor_version >= 1 )
    {
      if (rc) rc = file.ReadBool( &m_bHidden );
      if (rc && minor_version >= 2)
      {
        rc = m_image_file_reference.Read(file);
      }
    }
  }
  else
    rc = false;
  return rc;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmView
//

MYON_3dmPageSettings::MYON_3dmPageSettings()
{
  Default();
}

MYON_3dmPageSettings::~MYON_3dmPageSettings()
{
}

void MYON_3dmPageSettings::Default()
{
  m_page_number = 0;

  m_width_mm  = 0.0;
  m_height_mm = 0.0;

  m_left_margin_mm   = 0.0;
  m_right_margin_mm  = 0.0;
  m_top_margin_mm    = 0.0;
  m_bottom_margin_mm = 0.0;

  m_printer_name.Destroy();
}


bool MYON_3dmPageSettings::IsValid( MYON_TextLog* text_log ) const
{
  bool rc = true;

  if ( m_width_mm != 0.0 || m_height_mm != 0.0 )
  {
    if ( !MYON_IsValid(m_width_mm) || m_width_mm <= 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_width_mm = %g (should be > 0.0).\n",m_width_mm);
      }
      rc = false;
    }
    if ( !MYON_IsValid(m_height_mm) || m_height_mm <= 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_height_mm = %g (should be > 0.0).\n",m_height_mm);
      }
      rc = false;
    }
    if ( !MYON_IsValid(m_top_margin_mm) || m_top_margin_mm < 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_top_margin_mm = %g (should be >= 0.0).\n",m_top_margin_mm);
      }
      rc = false;
    }
    if ( !MYON_IsValid(m_bottom_margin_mm) || m_bottom_margin_mm < 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_bottom_margin_mm = %g (should be >= 0.0).\n",m_bottom_margin_mm);
      }
      rc = false;
    }
    if ( !MYON_IsValid(m_left_margin_mm) || m_left_margin_mm < 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_left_margin_mm = %g (should be >= 0.0).\n",m_left_margin_mm);
      }
      rc = false;
    }
    if ( !MYON_IsValid(m_right_margin_mm) || m_right_margin_mm < 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_right_margin_mm = %g (should be >= 0.0).\n",m_right_margin_mm);
      }
      rc = false;
    }
    if ( m_left_margin_mm + m_right_margin_mm >= m_width_mm )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_left_margin_mm+m_right_margin_mm = %g > %g = m_width_mm.\n",m_left_margin_mm + m_right_margin_mm, m_width_mm);
      }
      rc = false;
    }
    if ( m_top_margin_mm + m_bottom_margin_mm >= m_height_mm )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_top_margin_mm+m_bottom_margin_mm = %g > %g = m_height_mm.\n",m_top_margin_mm + m_bottom_margin_mm, m_height_mm);
      }
      rc = false;
    }
  }
  else
  {
    if ( m_top_margin_mm != 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_top_margin_mm = %g (should be 0.0).\n",m_top_margin_mm);
      }
      rc = false;
    }
    if ( m_bottom_margin_mm != 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_bottom_margin_mm = %g (should be 0.0).\n",m_bottom_margin_mm);
      }
      rc = false;
    }
    if ( m_left_margin_mm != 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_left_margin_mm = %g (should be 0.0).\n",m_left_margin_mm);
      }
      rc = false;
    }
    if ( m_right_margin_mm != 0.0 )
    {
      if ( text_log )
      {
        text_log->Print("MYON_3dmPageSettings has m_right_margin_mm = %g (should be 0.0).\n",m_right_margin_mm);
      }
      rc = false;
    }
  }

  return rc;
}

bool MYON_3dmPageSettings::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK, 1, 0 );
  if ( !rc )
    return false;

  for(;;)
  {
    rc = archive.WriteInt( m_page_number );
    if (!rc) break;

    rc = archive.WriteDouble(m_width_mm);
    if ( !rc) break;

    rc = archive.WriteDouble(m_height_mm);
    if ( !rc) break;

    rc = archive.WriteDouble(m_left_margin_mm);
    if ( !rc) break;

    rc = archive.WriteDouble(m_right_margin_mm);
    if ( !rc) break;

    rc = archive.WriteDouble(m_top_margin_mm);
    if ( !rc) break;

    rc = archive.WriteDouble(m_bottom_margin_mm);
    if ( !rc) break;

    rc = archive.WriteString(m_printer_name);
    if (!rc) break;

    break;
  }

  if ( !archive.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_3dmPageSettings::Read(MYON_BinaryArchive& archive)
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version );
  if ( !rc )
    return false;

  for(;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    rc = archive.ReadInt(&m_page_number );
    if (!rc) break;

    rc = archive.ReadDouble(&m_width_mm);
    if ( !rc) break;

    rc = archive.ReadDouble(&m_height_mm);
    if ( !rc) break;

    rc = archive.ReadDouble(&m_left_margin_mm);
    if ( !rc) break;

    rc = archive.ReadDouble(&m_right_margin_mm);
    if ( !rc) break;

    rc = archive.ReadDouble(&m_top_margin_mm);
    if ( !rc) break;

    rc = archive.ReadDouble(&m_bottom_margin_mm);
    if ( !rc) break;

    rc = archive.ReadString(m_printer_name);
    if (!rc) break;

    break;
  }

  if ( !archive.EndRead3dmChunk() )
    rc = false;

  return rc;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_StandardDisplayModeId
//



bool MYON_StandardDisplayModeId::IsStandardDisplayModeId(
    MYON_UUID id
    )
{
  if ( MYON_StandardDisplayModeId::Wireframe == id )
    return true;
  if ( MYON_StandardDisplayModeId::Shaded == id )
    return true;
  if ( MYON_StandardDisplayModeId::Rendered == id )
    return true;
  if ( MYON_StandardDisplayModeId::Ghosted == id )
    return true;
  if ( MYON_StandardDisplayModeId::XrayShade == id )
    return true;
  if ( MYON_StandardDisplayModeId::RenderedShadows == id )
    return true;
  if ( MYON_StandardDisplayModeId::Technical == id )
    return true;
  if ( MYON_StandardDisplayModeId::Artistic == id )
    return true;
  if ( MYON_StandardDisplayModeId::Pen == id )
    return true;
  if ( MYON_StandardDisplayModeId::AmbientOcclusion == id)
    return true;
  if ( MYON_StandardDisplayModeId::Raytraced == id)
    return true;

  return false;
}


MYON::v3_display_mode MYON_StandardDisplayModeId::ToV3DisplayMode(
  MYON_UUID id
  )
{
  if ( MYON_nil_uuid == id )
    return MYON::v3_default_display;

  if ( MYON_StandardDisplayModeId::Wireframe == id )
    return MYON::v3_wireframe_display;

  if ( MYON_StandardDisplayModeId::Shaded == id )
    return MYON::v3_shaded_display;

  if ( MYON_StandardDisplayModeId::Rendered == id )
    return MYON::v3_renderpreview_display;

  if ( MYON_StandardDisplayModeId::IsStandardDisplayModeId(id) )
    return MYON::v3_shaded_display;

  return MYON::v3_default_display;
}

MYON_UUID MYON_StandardDisplayModeId::FromV3DisplayMode(
  MYON::v3_display_mode dm
  )
{
  MYON_UUID id;
  switch ( dm )
  {
  case MYON::v3_default_display:
    id = MYON_nil_uuid;
    break;

  case MYON::v3_wireframe_display:
    id = MYON_StandardDisplayModeId::Wireframe;
    break;

  case MYON::v3_shaded_display:
    id = MYON_StandardDisplayModeId::Shaded;
    break;

  case MYON::v3_renderpreview_display:
    id = MYON_StandardDisplayModeId::Rendered;
    break;

  default:
    id = MYON_nil_uuid;
    break;
  }

  return id;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmView
//
MYON_3dmView::MYON_3dmView()
{
  Default();
} 

MYON_3dmView::~MYON_3dmView()
{
}

void MYON_3dmView::Dump( MYON_TextLog& dump ) const
{
  const wchar_t* wsViewName = static_cast< const wchar_t* >(m_name);
  if ( !wsViewName )
    wsViewName = L"";
  MYON::view_projection proj = m_vp.Projection();
  

  MYON_3dPoint camLoc;
  MYON_3dVector camX, camY, camZ;
  bool bValidCamera = m_vp.GetCameraFrame( camLoc, camX, camY, camZ );
  double frus_left,frus_right,frus_bottom,frus_top,frus_near,frus_far;
  bool bValidFrustum = m_vp.GetFrustum(&frus_left,&frus_right,&frus_bottom,&frus_top,&frus_near,&frus_far);
  int port_left, port_right, port_bottom, port_top, port_near, port_far;
  bool bValidPort = m_vp.GetScreenPort(&port_left,&port_right,&port_bottom,&port_top,&port_near,&port_far);

  const char* sProjectionName;
  switch(proj)
  {
  case MYON::parallel_view: sProjectionName = "parallel"; break;
  case MYON::perspective_view: sProjectionName = "perspective"; break;
  case MYON::unknown_view:
  default: 
    sProjectionName = "unknown";
    break;
  }
  dump.Print("Viewport: name = \"%ls\" projection = %s\n",wsViewName,sProjectionName);

  dump.PushIndent();

  if ( bValidCamera ) 
  {
    dump.Print("viewport camera frame\n"
           "  location = %g, %g, %g\n"
           "  X = %g, %g, %g\n"
           "  Y = %g, %g, %g\n"
           "  Z = %g, %g, %g\n",
           camLoc.x,camLoc.y,camLoc.z,
           camX.x,camX.y,camX.z,
           camY.x,camY.y,camY.z,
           camZ.x,camZ.y,camZ.z
           );
    MYON_3dPoint target_point = TargetPoint();
    double target_distance = target_point.DistanceTo( camLoc );
    dump.Print("camera target\n"
               "  distance = %g\n"
               "  point = %g,%g,%g\n",
               target_distance,
               target_point.x,target_point.y,target_point.z
               );
  }


  if ( bValidFrustum ) {
    dump.Print("view frustum\n"
           "  left   = %g, right = %g\n"
           "  bottom = %g, top   = %g\n"
           "  near   = %g, far   = %g\n",
           frus_left,frus_right,
           frus_bottom,frus_top,
           frus_near,frus_far
           );
  }

  if ( bValidPort ) {
    // location of viewport window on screen
    dump.Print("viewport window screen location\n"
           "  left   = %4d, right = %4d\n"
           "  bottom = %4d, top   = %4d\n"
           "  near   = %4d, far   = %4d\n",
           port_left,port_right,
           port_bottom,port_top,
           port_near,port_far
           );
  }


  // relative position of viewport window in application main frame
  double rel_left,rel_right,rel_bottom,rel_top;
  rel_left = m_position.m_wnd_left;
  rel_right = m_position.m_wnd_right;
  rel_bottom = m_position.m_wnd_bottom;
  rel_top = m_position.m_wnd_top;
  dump.Print("relative viewport window position in application frame window\n"
             "  left   = %6.2f%%, right = %6.2f%%\n"
             "  bottom = %6.2f%%, top   = %6.2f%%\n",
             100.0*rel_left, 100.0*rel_right,
             100.0*rel_bottom, 100.0*rel_top
             );

  dump.PopIndent();

}

void MYON_3dmView::Default()
{
  m_name.Destroy();

  m_vp = MYON_Viewport::DefaultTopViewYUp;

  m_cplane.Default();
  m_display_mode_id = MYON_nil_uuid;
  m_view_type = MYON::model_view_type;
  m_position.Default();
  if ( m_vp.Projection() == MYON::parallel_view ) {
    m_cplane.m_plane.CreateFromFrame( m_cplane.m_plane.origin, m_vp.CameraX(), m_vp.CameraY() );
  }
  m_bShowConstructionGrid = true;
  m_bShowConstructionAxes = true;
  m_bShowWorldAxes = true;
  m_bShowConstructionZAxis = false;

  m_trace_image.Default();
  m_wallpaper_image.Default();

  m_page_settings.Default();

  m_named_view_id = MYON_nil_uuid;

  m_bLockedProjection = false;

  m_dFocalBlurDistance = 100.0;
  m_dFocalBlurAperture = 64.0;
  m_dFocalBlurJitter = 0.1;
  m_uFocalBlurSampleCount = 10;
  m_FocalBlurMode = MYON_FocalBlurModes::None;
  m_sizeRendering.cx = 640;
  m_sizeRendering.cy = 480;
}

MYON::ViewSectionBehavior MYON_3dmView::SectionBehavior() const
{
  return m_section_behavior;
}
void MYON_3dmView::SetSectionBehavior(MYON::ViewSectionBehavior behavior)
{
  m_section_behavior = behavior;
}


MYON_3dPoint MYON_3dmView::TargetPoint() const
{
  // This function must return the valud saved on m_vp.m_target_point.
  // Do not modify that value here.
  return m_vp.TargetPoint();
}

bool MYON_3dmView::SetTargetPoint(MYON_3dPoint target_point)
{
  return m_vp.SetTargetPoint(target_point);
}

bool MYON_3dmView::IsValid(MYON_TextLog* text_log) const
{
  bool rc = m_vp.IsValid(text_log)?true:false;
  while(rc)
  {
    switch(m_view_type)
    {
    case MYON::model_view_type:
      if ( m_page_settings.m_width_mm != 0.0 || m_page_settings.m_height_mm != 0.0 )
      {
        if ( text_log )
        {
          text_log->Print("MYON_3dmView has m_view_type = MYON::model_view_type but m_page_settings width,height = (%g,%g) (both should be zero).\n",
                          m_page_settings.m_width_mm,
                          m_page_settings.m_height_mm);
        }
        rc = false;
      }
      //if (    m_nested_view_position.m_min.x != 0.0 || m_nested_view_position.m_max.x != 0.0
      //     || m_nested_view_position.m_min.y != 0.0 || m_nested_view_position.m_max.y != 0.0
      //     || m_nested_view_position.m_min.z != 0.0 || m_nested_view_position.m_max.z != 0.0 )
      //{
      //  if ( text_log )
      //  {
      //    text_log->Print("MYON_3dmView has m_view_type = MYON::model_view_type and m_nested_view_position is not identically zero.\n");
      //  }
      //  rc = false;
      //}
      //if ( !MYON_UuidIsNil(m_parent_viewport_id) )
      //{
      //  if ( text_log )
      //  {
      //    text_log->Print("MYON_3dmView has m_view_type = MYON::model_view_type and m_parent_viewport_id is not nil\n");
      //  }
      //  rc = false;
      //}
      break;
    case MYON::page_view_type:
      //if (    m_nested_view_position.m_min.x != 0.0 || m_nested_view_position.m_max.x != 0.0
      //     || m_nested_view_position.m_min.y != 0.0 || m_nested_view_position.m_max.y != 0.0
      //     || m_nested_view_position.m_min.z != 0.0 || m_nested_view_position.m_max.z != 0.0 )
      //{
      //  if ( text_log )
      //  {
      //    text_log->Print("MYON_3dmView has m_view_type = MYON::page_view_type and m_nested_view_position is not identically zero.\n");
      //  }
      //  rc = false;
      //}
      //if ( !MYON_UuidIsNil(m_parent_viewport_id) )
      //{
      //  if ( text_log )
      //  {
      //    text_log->Print("MYON_3dmView has m_view_type = MYON::page_view_type and m_parent_viewport_id is not nil\n");
      //  }
      //  rc = false;
      //}
      if ( m_page_settings.m_width_mm <= 0.0 || m_page_settings.m_height_mm <= 0.0 )
      {
        if ( text_log )
        {
          text_log->Print("MYON_3dmView has m_view_type = MYON::page_view_type but page width,height = (%g,%g)\n",
                          m_page_settings.m_width_mm,
                          m_page_settings.m_height_mm);
        }
        rc = false;
      }
      break;

    case MYON::nested_view_type:
      if ( m_page_settings.m_width_mm != 0.0 || m_page_settings.m_height_mm != 0.0 )
      {
        if ( text_log )
        {
          text_log->Print("MYON_3dmView has m_view_type = MYON::nested_view_type but m_page_settings width,height = (%g,%g) (both should be zero).\n",
                          m_page_settings.m_width_mm,
                          m_page_settings.m_height_mm);
        }
        rc = false;
      }
      //if ( MYON_UuidIsNil(m_parent_viewport_id) )
      //{
      //  if ( text_log )
      //  {
      //    text_log->Print("MYON_3dmView has m_view_type = MYON::nested_view_type and m_parent_viewport_id is nil.\n");
      //  }
      //  rc = false;
      //}

      //if ( !m_nested_view_position.IsValid() 
      //     || m_nested_view_position.m_min.x >= m_nested_view_position.m_max.x
      //     || m_nested_view_position.m_min.y >= m_nested_view_position.m_max.y
      //     || m_nested_view_position.m_min.z != m_nested_view_position.m_max.z )
      //{
      //  if ( text_log )
      //  {
      //    text_log->Print("MYON_3dmView has m_view_type = MYON::nested_view_type and m_nested_view_position is bogus.\n");
      //  }
      //  rc = false;
      //}
      break;

    case MYON::uveditor_view_type:
      break;

    case MYON::blockeditor_view_type:
      break;

    default:
      if ( text_log )
      {
        text_log->Print("MYON_3dmView m_view_type = %d (illegal enum value)\n",m_view_type);
      }
      rc = false;
      break;
    }
    if (rc)
      break;




    break;
  }
  return rc;
}

bool MYON_3dmView::Write( MYON_BinaryArchive& file ) const
{
  // Everything in a view is in a subchunk so new records can 
  // be added to a view and old I/O code will still
  // work.
  bool rc = true;

  // 27 June 2008 Dale Lear
  //   I added support for saving userdata attached to
  //   the m_vp MYON_Viewport.  Ideally, I would just call
  //   file.WriteObject(m_vp), but userdata support is being
  //   added years after millions of files have been written
  //   by calling m_vp.Write().
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_VIEWPORT, 0 );
    if(rc) {
      rc = m_vp.Write(file)?true:false;
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc && file.Archive3dmVersion() >= 4 && file.ObjectHasUserDataToWrite(&m_vp) ) 
  {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_VIEWPORT_USERDATA, 0 );
    if(rc) 
    {
      rc = file.WriteObjectUserData(m_vp);
      // write a "fake" TCODE_OPENNURBS_CLASS_END end of class 
      // mark so I can use
      // MYON_BinaryArchive::ReadObjectUserData() 
      // to read this user data.
      if ( file.BeginWrite3dmChunk( TCODE_OPENNURBS_CLASS_END, 0 ) ) 
      {
        if ( !file.EndWrite3dmChunk() )
          rc = false;
      }
      else
      {
        rc = false;
      }
      if ( !file.EndWrite3dmChunk() ) // end of TCODE_VIEW_VIEWPORT_USERDATA
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_CPLANE, 0 );
    if(rc) {
      rc = m_cplane.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_TARGET, 0 );
    if(rc) {
      MYON_3dPoint target_point = TargetPoint();
      if(rc) rc = file.WritePoint(target_point);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    MYON::v3_display_mode dm = MYON_StandardDisplayModeId::ToV3DisplayMode( m_display_mode_id );
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_V3_DISPLAYMODE, dm );
    if(rc) {
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_POSITION, 0 );
    if(rc) {
      if(rc) rc = m_position.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_SHOWCONGRID, m_bShowConstructionGrid );
    if(rc) {
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_SHOWCONAXES, m_bShowConstructionAxes );
    if(rc) {
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_SHOWWORLDAXES, m_bShowWorldAxes );
    if(rc) {
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_NAME, 0 );
    if(rc) {
      if(rc) rc = file.WriteString(m_name);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_TRACEIMAGE, 0 );
    if(rc) 
    {
      if(rc) 
        rc = m_trace_image.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc) 
  {
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_WALLPAPER, 0 );
    if(rc) 
    {
      if(rc) rc = file.WriteString(m_wallpaper_image.m_image_file_reference.FullPath());
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  if(rc && file.Archive3dmVersion() >= 3 ) 
  {
    // Added 5 June 2003 to support additional wallpaper attributes.
    // Older versions of Rhino/opennurbs
    // will just skip this chunk and get filename from the
    // TCODE_VIEW_WALLPAPER chunk written above.
    rc = file.BeginWrite3dmChunk( TCODE_VIEW_WALLPAPER_V3, 0 );
    if(rc) 
    {
      if(rc) 
        rc = m_wallpaper_image.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  if (rc && file.Archive3dmVersion() >= 4)
  {
    // 23 March 2005 Dale Lear:
    //   The "chunks" above trace their history back to Rhino 1.0;
    //   The TCODE_VIEW_ATTRIBUTES chunk uses a chunk version so that
    //   new view information can be added without inventing a new 
    //   TCODE for each new piece of information.

    rc = file.BeginWrite3dmChunk( TCODE_VIEW_ATTRIBUTES, 0 );
    if (rc)
    {
      rc = file.Write3dmChunkVersion( 1, 9 ); // (there are no 1.0 fields)

      while(rc)
      {
        // 1.1 fields (there are no 1.0 fields)
        rc = file.WriteInt( m_view_type );
        if (!rc) break;
        
        // obsolete values - superceded by m_page_settings
        rc = file.WriteDouble( m_page_settings.m_width_mm );
        if (!rc) break;

        rc = file.WriteDouble( m_page_settings.m_height_mm );
        if (!rc) break;

        MYON_UUID obsolete_parent_viewport_id;
        memset(&obsolete_parent_viewport_id,0,sizeof(obsolete_parent_viewport_id));
        rc = file.WriteUuid( obsolete_parent_viewport_id );
        if (!rc) break;

        MYON_BoundingBox obsolete_nested_view_position;
        rc = file.WriteBoundingBox( obsolete_nested_view_position );
        if (!rc) break;

        // 28 feb 2006 version 1.2 fields
        rc = file.WriteUuid(m_display_mode_id);
        if (!rc) break;

        rc = m_page_settings.Write(file);
        if (!rc) break;

        // 7 March 2006 version 1.3 fields
        rc = file.WriteBool(m_bLockedProjection);
        if (!rc) break;

        // 12 December 2010 version 1.4
        rc = file.WriteArray(m_clipping_planes);
        if (!rc) break;

        // 13 June 2013 version 1.5
        rc = file.WriteUuid(m_named_view_id);
        if (!rc) break;

        // 8 Dec 2016 version 1.6 (ALB)
        rc = file.WriteBool(m_bShowConstructionZAxis);
        if (!rc) break;

        // 6 June 2017 - Andy.  version 1.7 fields
        rc = file.WriteDouble(m_dFocalBlurDistance);
        if (!rc) break;

        rc = file.WriteDouble(m_dFocalBlurAperture);
        if (!rc) break;

        rc = file.WriteDouble(m_dFocalBlurJitter);
        if (!rc) break;

        rc = file.WriteInt(m_uFocalBlurSampleCount);
        if (!rc) break;

        rc = file.WriteInt((int)m_FocalBlurMode);
        if (!rc) break;

        // 4 August 2017 - Andy.  version 1.8 fields
        rc = file.WriteInt(m_sizeRendering.cx);
        if (!rc) break;

        rc = file.WriteInt(m_sizeRendering.cy);
        if (!rc) break;

        // 10 July 2022 - S. Baer version 1.9 fields
        rc = file.WriteChar((unsigned char)SectionBehavior());
        if (!rc) break;

        break;
      }

      // Since BeginWrite3dmChunk() returned true, EndWrite3dmChunk()
      // must be called even when rc is false
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // required TCODE_ENDOFTABLE chunk - marks end of view table
  if ( rc ) {
    rc = file.BeginWrite3dmChunk( TCODE_ENDOFTABLE, 0 );
    if ( rc ) {
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }
  return rc;
}

bool MYON_3dmView::Read( MYON_BinaryArchive& file )
{
  // Everything in a view is in a subchunk so new records can 
  // be added to a view and old I/O code will still
  // work.
  MYON_3dPoint obsolete_target_point = MYON_3dPoint::UnsetPoint;
  unsigned int tcode = 0;
  MYON__INT64 big_value = 0;
  int i32;
  bool rc = true;

  Default();

  while(rc) {
    rc = file.BeginRead3dmBigChunk(&tcode,&big_value);
    if (!rc)
      break;
    switch(tcode) 
    {
    case TCODE_VIEW_CPLANE:
      rc = m_cplane.Read(file);
      break;
    case TCODE_VIEW_VIEWPORT:
      rc = m_vp.Read(file)?true:false;
      break;
    case TCODE_VIEW_VIEWPORT_USERDATA:
      // 27 June 2008 Dale Lear
      //   I added support for saving userdata attached to
      //   the m_vp MYON_Viewport.  Ideally, the MYON_Viewport
      //   would be read by calling file.ReadObject(), but
      //   userdata support is being added years after 
      //   millions of files have been written by calling
      //   m_vp.Write()/Read().
      rc = file.ReadObjectUserData(m_vp);
      break;
    case TCODE_VIEW_SHOWCONGRID:
      m_bShowConstructionGrid = big_value?true:false;
      break;
    case TCODE_VIEW_SHOWCONAXES:
      m_bShowConstructionAxes = big_value?true:false;
      break;
    case TCODE_VIEW_SHOWWORLDAXES:
      m_bShowWorldAxes = big_value?true:false;
      break;
    case TCODE_VIEW_TRACEIMAGE:
      rc = m_trace_image.Read(file);
      break;
    case TCODE_VIEW_WALLPAPER:
      // used prior to 5 June 2003 and still written
      // after 5 June 2003 so older Rhinos/opennurbs
      // will not loose the filename information.
      {
        MYON_wString bitmap_filename;
        rc = file.ReadString(bitmap_filename);
        m_wallpaper_image.m_image_file_reference.SetFullPath(bitmap_filename,false);
      }
      m_wallpaper_image.m_bGrayScale = true;
      break;
    case TCODE_VIEW_WALLPAPER_V3:
      // Added 5 June 2003 to support additional wallpaper attributes.
      rc = m_wallpaper_image.Read(file);
      break;
    case TCODE_VIEW_TARGET:
      // 13 June 2013
      // This point has been obsolete for more than 5 years 
      // and is now ignored.  The target point is on MYON_Viewport.
      rc = file.ReadPoint(obsolete_target_point);
      break;
    case TCODE_VIEW_V3_DISPLAYMODE:
      if ( MYON_nil_uuid == m_display_mode_id )
      {
        i32 = (int)big_value;
        MYON::v3_display_mode dm = MYON::V3DisplayMode(i32);
        m_display_mode_id = MYON_StandardDisplayModeId::FromV3DisplayMode(dm);
      }
      break;
    case TCODE_VIEW_NAME:
      rc = file.ReadString(m_name);
      break;
    case TCODE_VIEW_POSITION:
      rc = m_position.Read(file);
      break;

    case TCODE_VIEW_ATTRIBUTES:
      {
        int major_version = 0;
        int minor_version = 0;
        rc = file.Read3dmChunkVersion(&major_version,&minor_version);
        // there are no 1.0 fields in this chunk
        while ( rc 
                && major_version == 1 && minor_version >= 1 
                && file.Archive3dmVersion() >= 4 
                && file.ArchiveOpenNURBSVersion() >= 200503170 )
        {
          // Added 23 March 2005 Dale Lear
          // 1.1 fields (there are no 1.0 fields)
          i32 = 0;
          rc = file.ReadInt( &i32 );
          if (!rc) break;
          m_view_type = MYON::ViewType(i32);
          
          rc = file.ReadDouble( &m_page_settings.m_width_mm );
          if (!rc) break;

          rc = file.ReadDouble( &m_page_settings.m_height_mm );
          if (!rc) break;

          MYON_UUID obsolete_parent_viewport_id;
          rc = file.ReadUuid( obsolete_parent_viewport_id );
          if (!rc) break;

          MYON_BoundingBox obsolete_nested_view_position;
          rc = file.ReadBoundingBox( obsolete_nested_view_position );
          if (!rc) break;

          if ( minor_version >= 2 )
          {
            // 28 feb 2006 version 1.2 field
            rc = file.ReadUuid(m_display_mode_id);
            if (!rc) break;

            rc = m_page_settings.Read(file);
            if (!rc) break;

            if ( minor_version >= 3 )
            {
              rc = file.ReadBool(&m_bLockedProjection);
              if (!rc) break;

              if ( minor_version >= 4 )
              {
                rc = file.ReadArray(m_clipping_planes);
                if (!rc) break;
                if ( minor_version >= 5 )
                {
                  // 13 June 2013 version 1.5 field
                  rc = file.ReadUuid(m_named_view_id);
                  if (!rc) break;
                  if (minor_version >= 6)
                  {
                    // 8 Dec 2016 version 1.6 field (ALB)
                    rc = file.ReadBool(&m_bShowConstructionZAxis);
                    if (!rc) break;
                    if (minor_version >= 7)
                    {
                      // 11 May 2016 - Andy.  version 1.7 fields
                      rc = file.ReadDouble(&m_dFocalBlurDistance);
                      if (!rc) break;

                      rc = file.ReadDouble(&m_dFocalBlurAperture);
                      if (!rc) break;

                      rc = file.ReadDouble(&m_dFocalBlurJitter);
                      if (!rc) break;

                      rc = file.ReadInt((int*)&m_uFocalBlurSampleCount);
                      if (!rc) break;

                      rc = file.ReadInt((int*)&m_FocalBlurMode);
                      if (!rc) break;
                      if (minor_version >= 8)
                      {
                        // 4 August 2017 - Andy.  version 1.8 fields
                        rc = file.ReadInt(&m_sizeRendering.cx);
                        if (!rc) break;
                        rc = file.ReadInt(&m_sizeRendering.cy);
                        if (!rc) break;
                      }
                      if (minor_version >= 9)
                      {
                        // 9 July 2022 S. Baer add section behaviour
                        unsigned char c = 0;
                        rc = file.ReadChar(&c);
                        if (!rc) break;
                        SetSectionBehavior(MYON::ViewSectionBehaviorFromUnsigned(c));
                      }
                    }
                  }
                }
              }
            }
          }

          // Add new inforamation here - ask Dale Lear for help.

          break;
        }
      }
      break;
    }

    if (!file.EndRead3dmChunk())
      rc = false;
    if ( tcode == TCODE_ENDOFTABLE )
      break;
  }

  if ( obsolete_target_point.IsValid() && m_vp.TargetPoint().IsUnset() )
    m_vp.SetTargetPoint(obsolete_target_point);

  return rc;
}

double MYON_3dmView::FocalBlurDistance(void) const
{
  return m_dFocalBlurDistance;
}

void MYON_3dmView::SetFocalBlurDistance(double d)
{
  m_dFocalBlurDistance = d;
}

double MYON_3dmView::FocalBlurAperture(void) const
{
  return m_dFocalBlurAperture;
}

void MYON_3dmView::SetFocalBlurAperture(double d)
{
  m_dFocalBlurAperture = d;
}

double MYON_3dmView::FocalBlurJitter(void) const
{
  return m_dFocalBlurJitter;
}

void MYON_3dmView::SetFocalBlurJitter(double d)
{
  m_dFocalBlurJitter = d;
}

unsigned int MYON_3dmView::FocalBlurSampleCount(void) const
{
  return m_uFocalBlurSampleCount;
}

void MYON_3dmView::SetFocalBlurSampleCount(unsigned int sc)
{
  m_uFocalBlurSampleCount = sc;
}

MYON_FocalBlurModes MYON_3dmView::FocalBlurMode(void) const
{
  return m_FocalBlurMode;
}

void MYON_3dmView::SetFocalBlurMode(MYON_FocalBlurModes m)
{
  m_FocalBlurMode = m;
}


MYON_2iSize MYON_3dmView::RenderingSize() const
{
  return m_sizeRendering;
}

void MYON_3dmView::SetRenderingSize(const MYON_2iSize& size)
{
  m_sizeRendering = size;
}

int MYON_EarthAnchorPoint::CompareEarthLocation(const MYON_EarthAnchorPoint* a, const MYON_EarthAnchorPoint* b)
{
  if ( !a )
  {
    return b ? -1 : 0;
  }
  if (!b)
  {
    return 1;
  }

  double xa = a->m_earth_longitude;
  double xb = b->m_earth_longitude;
  if ( !MYON_IsValid(xa) )
  {
    if ( MYON_IsValid(xb) ) return -1;
  }
  else if ( !MYON_IsValid(xb) )
  {
    return 1;
  }
  else
  {
    while(xa <= 0.0)
      xa += 360.0;
    while(xa > 360.0)
      xa -= 360.0;
    while(xb <= 0.0)
      xb += 360.0;
    while(xb > 360.0)
      xb -= 360.0;
    if ( xa < xb ) return -1;
    if ( xa > xb ) return 1;
  }

  xa = a->m_earth_latitude;
  xb = b->m_earth_latitude;
  if ( !MYON_IsValid(xa) )
  {
    if ( MYON_IsValid(xb) ) return -1;
  }
  else if ( !MYON_IsValid(xb) )
  {
    return 1;
  }
  else
  {
    while(xa <= 0.0)
      xa += 360.0;
    while(xa > 360.0)
      xa -= 360.0;
    while(xb <= 0.0)
      xb += 360.0;
    while(xb > 360.0)
      xb -= 360.0;
    if ( xa < xb ) return -1;
    if ( xa > xb ) return 1;
  }

  const unsigned int aecs = static_cast<unsigned char>(a->m_earth_coordinate_system);
  const unsigned int becs = static_cast<unsigned char>(a->m_earth_coordinate_system);
  if (aecs < becs)
    return -1;
  if (aecs > becs)
    return 1;

  xa = a->m_earth_elevation_meters;
  xb = b->m_earth_elevation_meters;
  if ( !MYON_IsValid(xa) )
  {
    if ( MYON_IsValid(xb) ) return -1;
  }
  else if ( !MYON_IsValid(xb) )
  {
    return 1;
  }
  else
  {
    if ( xa < xb ) return -1;
    if ( xa > xb ) return 1;
  }

  return 0;   
}

int MYON_EarthAnchorPoint::CompareModelDirection(const MYON_EarthAnchorPoint* a, const MYON_EarthAnchorPoint* b)
{
  if ( !a )
  {
    return b ? -1 : 0;
  }
  if (!b)
  {
    return 1;
  }

  int i = MYON_ComparePoint(3,false,&a->m_model_point.x,&b->m_model_point.x);
  if ( !i )
  {
    i = MYON_ComparePoint(3,false,&a->m_model_north.x,&b->m_model_north.x);
    if ( !i )
    {
      i = MYON_ComparePoint(3,false,&a->m_model_east.x,&b->m_model_east.x);
    }
  }
  return i;  
}

int MYON_EarthAnchorPoint::CompareIdentification(const MYON_EarthAnchorPoint* a, const MYON_EarthAnchorPoint* b)
{
  if ( !a )
  {
    return b ? -1 : 0;
  }
  if (!b)
  {
    return 1;
  }

  int i = MYON_UuidCompare(a->m_id,b->m_id);
  if ( !i)
  {
    i = a->m_name.CompareOrdinal(b->m_name,false);
    if (!i)
    {
      i = a->m_description.CompareOrdinal(b->m_description,false);
      if (!i)
      {
        i = a->m_url.CompareOrdinal(b->m_url,true);
        if ( !i)
        {
          i = a->m_url_tag.CompareOrdinal(b->m_url_tag,false);
        }
      }
    }
  }
  return i;  
}

int MYON_EarthAnchorPoint::Compare(const MYON_EarthAnchorPoint* a, const MYON_EarthAnchorPoint* b)
{
  int i = MYON_EarthAnchorPoint::CompareEarthLocation(a,b);
  if ( !i)
  {
    i = MYON_EarthAnchorPoint::CompareModelDirection(a,b);
    if (!i)
    {
      i = MYON_EarthAnchorPoint::CompareIdentification(a,b);
    }
  }
  return i;
}

bool MYON_EarthAnchorPoint::Read( MYON_BinaryArchive& file )
{
  *this = MYON_EarthAnchorPoint::Unset;
  
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if ( !rc )
    return false;

  for(;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    rc = file.ReadDouble(&m_earth_latitude);
    if (!rc) break;
    rc = file.ReadDouble(&m_earth_longitude);
    if (!rc) break;
    rc = file.ReadDouble(&m_earth_elevation_meters);
    if (!rc) break;

    if (
      minor_version < 2 
      && 0.0 == m_earth_latitude
      && 0.0 == m_earth_longitude 
      && 0.0 == m_earth_elevation_meters
      && file.ArchiveOpenNURBSVersion() <= 2348834428
      )
    {
      m_earth_latitude = MYON_EarthAnchorPoint::Unset.m_earth_latitude;
      m_earth_longitude = MYON_EarthAnchorPoint::Unset.m_earth_longitude;
      m_earth_elevation_meters = MYON_EarthAnchorPoint::Unset.m_earth_elevation_meters;
    }


    rc = file.ReadPoint(m_model_point);
    if (!rc) break;
    rc = file.ReadVector(m_model_north);
    if (!rc) break;
    rc = file.ReadVector(m_model_east);
    if (!rc) break;
   
    if ( minor_version >= 1 )
    {
      // 1.1 fields
      int earth_basepoint_elevation_zero = MYON_UNSET_INT_INDEX;
      rc = file.ReadInt(&earth_basepoint_elevation_zero);
      if (!rc) break;
      switch (earth_basepoint_elevation_zero)
      {
      case 0:
        m_earth_coordinate_system = MYON::EarthCoordinateSystem::GroundLevel;
        break;
      case 1:
        m_earth_coordinate_system = MYON::EarthCoordinateSystem::MeanSeaLevel;
        break;
      case 2:
        m_earth_coordinate_system = MYON::EarthCoordinateSystem::CenterOfEarth;
        break;
      default:
        break;
      }
      rc = file.ReadUuid(m_id);
      if (!rc) break;
      rc = file.ReadString(m_name);
      if (!rc) break;
      rc = file.ReadString(m_description);
      if (!rc) break;
      rc = file.ReadString(m_url);
      if (!rc) break;
      rc = file.ReadString(m_url_tag);
      if (!rc) break;

      if (minor_version >= 2)
      {
        unsigned int u;
        
        u = static_cast<unsigned char>(MYON_EarthAnchorPoint::Unset.EarthCoordinateSystem());
        rc = file.ReadInt(&u);
        if (!rc) break;
        m_earth_coordinate_system = MYON::EarthCoordinateSystemFromUnsigned(u);
      }
    }

    break;
  }

  if ( !file.EndRead3dmChunk() )
    rc = false;

  return rc;
}

static double Internal_UnsetToZeroInV5Files(
  const MYON_BinaryArchive& file,
  double value
)
{
  // http://mcneel.myjetbrains.com/youtrack/issue/RH-34700
  // V5 RDK sun dialog expects 0.0 for unset latitude and longitude values
  // and crashes if it encounters MYON_UNSET_VALUE.
  return (file.Archive3dmVersion() < 60 && MYON_UNSET_VALUE == value)
      ? 0.0
      : value;
}

bool MYON_EarthAnchorPoint::Write( MYON_BinaryArchive& file ) const
{
  bool rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,2);
  if ( !rc )
    return false;

  for(;;)
  {
    const double earth_latitude = Internal_UnsetToZeroInV5Files(file, m_earth_latitude);
    const double earth_longitude = Internal_UnsetToZeroInV5Files(file, m_earth_longitude);
    rc = file.WriteDouble(earth_latitude);
    if (!rc) break;
    rc = file.WriteDouble(earth_longitude);
    if (!rc) break;
    rc = file.WriteDouble(m_earth_elevation_meters);
    if (!rc) break;
    rc = file.WritePoint(m_model_point);
    if (!rc) break;
    rc = file.WriteVector(m_model_north);
    if (!rc) break;
    rc = file.WriteVector(m_model_east);
    if (!rc) break;

    // 1.1 fields
    int earth_basepoint_elevation_zero;
    switch (m_earth_coordinate_system)
    {
    case MYON::EarthCoordinateSystem::GroundLevel:
      earth_basepoint_elevation_zero = 0;
      break;
    case MYON::EarthCoordinateSystem::MeanSeaLevel:
      earth_basepoint_elevation_zero = 1;
      break;
    case MYON::EarthCoordinateSystem::CenterOfEarth:
      earth_basepoint_elevation_zero = 2;
      break;
    default:
      earth_basepoint_elevation_zero = 0;
      break;
    }
    rc = file.WriteInt(earth_basepoint_elevation_zero); // legacy value for old files
    if (!rc) break;
    rc = file.WriteUuid(m_id);
    if (!rc) break;
    rc = file.WriteString(m_name);
    if (!rc) break;
    rc = file.WriteString(m_description);
    if (!rc) break;
    rc = file.WriteString(m_url);
    if (!rc) break;
    rc = file.WriteString(m_url_tag);
    if (!rc) break;

    // 1.2 fields
    unsigned int u;
    u = static_cast<unsigned char>(MYON_EarthAnchorPoint::Unset.EarthCoordinateSystem());
    rc = file.WriteInt(u);
    if (!rc) break;

    break;
  }

  if ( !file.EndWrite3dmChunk() )
    rc = false;

  return rc;
}


bool MYON_EarthAnchorPoint::GetModelCompass(MYON_Plane& model_compass) const
{
  MYON_Plane mc;
  mc.xaxis = m_model_east;
  mc.yaxis = m_model_north;
  if ( fabs(mc.xaxis.Length() - 1.0) > MYON_SQRT_EPSILON )
  {
    if ( !mc.xaxis.Unitize() )
      return false;
  }
  if ( fabs(mc.yaxis.Length() - 1.0) > MYON_SQRT_EPSILON )
  {
    if ( !mc.yaxis.Unitize() )
      return false;
  }
  double d = mc.xaxis*mc.yaxis;
  if ( fabs(d) > MYON_SQRT_EPSILON )
  {
    // assume north is correct
    mc.xaxis.x -= d*mc.yaxis.x;
    mc.xaxis.y -= d*mc.yaxis.y;
    mc.xaxis.z -= d*mc.yaxis.z;
    if( !mc.xaxis.Unitize() )
      return false;
  }
  mc.zaxis = MYON_CrossProduct(mc.xaxis,mc.yaxis);
  if ( fabs(mc.zaxis.Length() - 1.0) > MYON_SQRT_EPSILON )
  {
    if ( !mc.zaxis.Unitize() )
      return false;
  }
  mc.origin = m_model_point;
  mc.UpdateEquation();
  model_compass = mc;
  return model_compass.IsValid();
}

bool MYON_EarthAnchorPoint::EarthLocationIsSet() const
{
  return (
    MYON_IsValid(m_earth_latitude)
    && MYON_IsValid(m_earth_longitude)
    && MYON_IsValid(m_earth_elevation_meters)
  );
}


bool MYON_EarthAnchorPoint::ModelLocationIsSet() const
{
  return (
    m_model_point.IsValid()
    && m_model_north.IsNotZero()
    && m_model_east.IsNotZero()
    && m_model_north.Length() > MYON_ZERO_TOLERANCE
    && m_model_east.Length() > MYON_ZERO_TOLERANCE
    && fabs(m_model_north.UnitVector()* m_model_east.UnitVector()) <= 1e-8
  );
}

bool MYON_EarthAnchorPoint::GetModelToEarthXform(
          const MYON_UnitSystem& model_unit_system,
          MYON_Xform& model_to_earth
          ) const
{
  if (false == EarthLocationIsSet() || false == ModelLocationIsSet())
  {
    model_to_earth = MYON_Xform::IdentityTransformation;
    return false;
  }

  // The orient_model rotates the model so that
  //   xaxis runs from west to east
  //   yaxis runs from south to north
  //   zaxis points "up"
  MYON_Plane model_compass;
  bool rc = GetModelCompass( model_compass );
  model_compass.origin = m_model_point;
  model_compass.UpdateEquation();
  MYON_Xform orient_model;
  orient_model.Rotation( model_compass, MYON_xy_plane  );

  MYON_Xform coord_change(MYON_Xform::IdentityTransformation);

  const double lat_radians = m_earth_latitude/180.0*MYON_PI;
  const double cos_lat = cos(lat_radians);
  const double sin_lat = sin(lat_radians);
  
  // get radius of earth at this latitude
  const double earth_polar_radius      = 6356750.0; // Earth's radius at poles (meters)
  const double earth_equatorial_radius = 6378135.0; // Earth's radius at equator (meters)
  MYON_2dVector r;
  r.x = cos_lat;
  r.y = sin_lat*(earth_equatorial_radius/earth_polar_radius);
  double earth_radius = earth_equatorial_radius/r.Length();
  if ( earth_radius > earth_equatorial_radius )
    earth_radius = earth_equatorial_radius;
  else if ( earth_radius < earth_polar_radius )
    earth_radius = earth_polar_radius;

  const double meters_per_degree_latitude = earth_radius*MYON_PI/180.0; // meters per degree of latitude

  const double model_to_meters_scale = MYON::UnitScale(model_unit_system, MYON::LengthUnitSystem::Meters);
  const double north_south_scale  = model_to_meters_scale/meters_per_degree_latitude;
  const double east_west_scale = ( 1.0e100*cos_lat < north_south_scale )
                               ? north_south_scale
                               : north_south_scale/cos_lat;

  coord_change.m_xform[0][0] = east_west_scale;
  coord_change.m_xform[0][3] = m_earth_longitude;
  coord_change.m_xform[1][1] = north_south_scale;
  coord_change.m_xform[1][3] = m_earth_latitude;
  coord_change.m_xform[2][2] = model_to_meters_scale;
  coord_change.m_xform[3][2] = m_earth_elevation_meters;

  model_to_earth = coord_change*orient_model;

  return rc;
}


MYON::EarthCoordinateSystem MYON::EarthCoordinateSystemFromUnsigned(
  unsigned int earth_location_standard_as_unsigned
)
{
  switch (earth_location_standard_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::EarthCoordinateSystem::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::EarthCoordinateSystem::GroundLevel);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::EarthCoordinateSystem::MeanSeaLevel);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::EarthCoordinateSystem::CenterOfEarth);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::EarthCoordinateSystem::WGS1984);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON::EarthCoordinateSystem::EGM2008);
  };
  MYON_ERROR("Invalid earth_location_standard_as_unsigned parameter value");
  return MYON::EarthCoordinateSystem::Unset;

}

void MYON_EarthAnchorPoint::SetEarthLocation(
  MYON::EarthCoordinateSystem earth_coordinate_system,
  const class MYON_UnitSystem& elevation_unit_system,
  double latitude_degrees,
  double longitude_degrees,
  double elevation
)
{
  m_earth_coordinate_system = earth_coordinate_system;
  SetLatitudeAndLongitude(latitude_degrees,longitude_degrees);
  SetElevation(elevation_unit_system, elevation);
}

void MYON_EarthAnchorPoint::SetEarthLocation(
  MYON::EarthCoordinateSystem earth_coordinate_system,
  MYON::LengthUnitSystem elevation_unit_system,
  double latitude_degrees,
  double longitude_degrees,
  double elevation
)
{
  MYON_UnitSystem us(elevation_unit_system);
  SetEarthLocation(earth_coordinate_system, us, latitude_degrees, longitude_degrees, elevation);
}


MYON_3dPoint MYON_EarthAnchorPoint::EarthLocation() const
{
  return MYON_3dPoint(m_earth_latitude, m_earth_longitude, m_earth_elevation_meters);
}

MYON_3dPoint MYON_EarthAnchorPoint::EarthLocation(
  MYON_3dPoint unset_location
) const
{
  return (EarthLocationIsSet() ? EarthLocation() : unset_location);
}


double MYON_EarthAnchorPoint::Latitude() const
{
  return m_earth_latitude;
}

double MYON_EarthAnchorPoint::Latitude(
  double unset_latitude
) const
{
  return (m_earth_latitude > -360.0 && m_earth_latitude < 360.0) ? m_earth_latitude : unset_latitude;
}

void MYON_EarthAnchorPoint::SetLatitudeAndLongitude(
  double latitude_degrees,
  double longitude_degrees
)
{
  const double tol = MYON_ZERO_TOLERANCE;
  const bool bSetLatitude = MYON_IsValid(latitude_degrees) && fabs(latitude_degrees) <= 10000.0;
  const bool bSetLongitude = MYON_IsValid(longitude_degrees) && fabs(longitude_degrees) <= 10000.0;
  double x, y;
  if (bSetLatitude)
  {
    y = latitude_degrees;

    x = 0.0;
    while (y > 180.0)
    {
      x -= 360.0;
      y = latitude_degrees + x;
    }

    x = 0.0;
    while (y < -180.0)
    {
      x += 360.0;
      y = latitude_degrees + x;
    }
    if (y != latitude_degrees)
    {
      if (fabs(y - 180.0) <= tol)
        y = 180.0;
      if (fabs(y + 180.0) <= tol)
        y = -180.0;
    }

    if ( bSetLongitude && fabs(y) > 90.0 )
    {
      if (y > 90.0)
        y = 180.0 - y;
      else
        y = -180.0 - y;
      longitude_degrees += 180.0;
    }

    if (y != latitude_degrees)
    {
      if (fabs(y) <= tol)
        y = 0.0;
      if (fabs(y - 90.0) <= tol)
        y = 90.0;
      else if (fabs(y + 90.0) <= tol)
        y = -90.0;
    }

    m_earth_latitude = y;
  }
  else
  {
    m_earth_latitude = MYON_EarthAnchorPoint::Unset.m_earth_latitude;
  }

  if (bSetLongitude)
  {
    y = longitude_degrees;

    x = 0.0;
    while (y >= 360.0)
    {
      x -= 360.0;
      y = longitude_degrees + x;
    }

    x = 0.0;
    while (y <= -360.0)
    {
      x += 360.0;
      y = longitude_degrees + x;
    }

    if (y != longitude_degrees)
    {
      if (fabs(y) <= tol)
        y = 0.0;
      if (fabs(y-360.0) <= tol)
        y = 0.0;
      else if (fabs(y+360.0) <= tol)
        y = 0.0;
    }

    m_earth_longitude = y;
  }
  else
  {
    m_earth_longitude = MYON_EarthAnchorPoint::Unset.m_earth_longitude;
  }
}

void MYON_EarthAnchorPoint::SetLatitude(
  double latitude_degrees
)
{
  const double x = m_earth_longitude;
  SetLatitudeAndLongitude(latitude_degrees, MYON_UNSET_VALUE);
  m_earth_longitude = x;
}


double MYON_EarthAnchorPoint::Longitude() const
{
  return m_earth_longitude;
}

double MYON_EarthAnchorPoint::Longitude(
  double unset_longitude
) const
{
  return (m_earth_longitude > -360.0 && m_earth_longitude < 360.0) ? m_earth_longitude : unset_longitude;
}

  
void MYON_EarthAnchorPoint::SetLongitude(
  double longitude_degrees
)
{
  const double x = m_earth_latitude;
  SetLatitudeAndLongitude(MYON_UNSET_VALUE, longitude_degrees);
  m_earth_latitude = x;
}


MYON::EarthCoordinateSystem MYON_EarthAnchorPoint::EarthCoordinateSystem() const
{
  return m_earth_coordinate_system;
}

void MYON_EarthAnchorPoint::SetEarthCoordinateSystem(
  MYON::EarthCoordinateSystem earth_coordinate_system
)
{
  m_earth_coordinate_system = earth_coordinate_system;
}

double MYON_EarthAnchorPoint::ElevationInMeters() const
{
  return m_earth_elevation_meters;
}

double MYON_EarthAnchorPoint::Elevation(
  const class MYON_UnitSystem& elevation_unit_system
  ) const
{
  if (MYON_IsValid(m_earth_elevation_meters && 0.0 != m_earth_elevation_meters && MYON::LengthUnitSystem::Meters != elevation_unit_system.UnitSystem()))
  {
    return m_earth_elevation_meters*MYON::UnitScale(elevation_unit_system, MYON_UnitSystem::Meters);
  }
  return m_earth_elevation_meters;
}

double MYON_EarthAnchorPoint::Elevation(
  MYON::LengthUnitSystem elevation_unit_system
  ) const
{
  MYON_UnitSystem us(elevation_unit_system);
  return Elevation(us);
}

double MYON_EarthAnchorPoint::Elevation(
  const class MYON_UnitSystem& elevation_unit_system,
  double unset_elevation
) const
{
  return MYON_IsValid(m_earth_elevation_meters) ? Elevation(elevation_unit_system) : unset_elevation;
}

double MYON_EarthAnchorPoint::Elevation(
  MYON::LengthUnitSystem elevation_unit_system,
  double unset_elevation
) const
{
  return MYON_IsValid(m_earth_elevation_meters) ? Elevation(elevation_unit_system) : unset_elevation;
}

void MYON_EarthAnchorPoint::SetElevation(
  const class MYON_UnitSystem& elevation_unit_system,
  double elevation
)
{
  if (MYON_IsValid(elevation))
  {
    if (MYON::LengthUnitSystem::Meters == elevation_unit_system.UnitSystem())
      m_earth_elevation_meters = elevation;
    else
      m_earth_elevation_meters = elevation*MYON::UnitScale(elevation_unit_system, MYON_UnitSystem::Meters);
  }
  else
  {
    m_earth_elevation_meters = MYON_EarthAnchorPoint::Unset.m_earth_elevation_meters;
  }
}

void MYON_EarthAnchorPoint::SetElevation(
  MYON::LengthUnitSystem elevation_unit_system,
  double elevation
)
{
  MYON_UnitSystem us(elevation_unit_system);
  SetElevation(us, elevation);
}

const MYON_3dPoint& MYON_EarthAnchorPoint::ModelPoint() const
{
  return m_model_point;
}

const MYON_3dVector& MYON_EarthAnchorPoint::ModelNorth() const
{
  return m_model_north;
}

const MYON_3dVector& MYON_EarthAnchorPoint::ModelEast() const
{
  return m_model_east;
}


void MYON_EarthAnchorPoint::SetModelPoint(
  MYON_3dPoint model_point
)
{
  m_model_point = model_point;
}

void MYON_EarthAnchorPoint::SetModelNorth(
  MYON_3dVector model_north
)
{
  m_model_north = model_north;
}

void MYON_EarthAnchorPoint::SetModelEast(
  MYON_3dVector model_east
)
{
  m_model_east = model_east;
}

void MYON_EarthAnchorPoint::SetModelLocation(
  MYON_3dPoint model_point,
  MYON_3dVector model_north,
  MYON_3dVector model_east
  )
{
  SetModelPoint(model_point);
  SetModelNorth(model_north);
  SetModelEast(model_east);
}

const MYON_Xform MYON_EarthAnchorPoint::Internal_KMLOrientationXform() const
{
  if (false == this->ModelLocationIsSet())
  {
    MYON_ERROR("Corrupt model location.");
    return MYON_Xform::Nan;
  }

  MYON_Plane model_directions;
  if (false == model_directions.CreateFromFrame(MYON_3dPoint::Origin, ModelEast(), ModelNorth()))
    return MYON_Xform::Nan;
  if (false == model_directions.IsValid())
    return MYON_Xform::Nan;

  // The KML orientation moves the model so that
  // east = x-axis
  // north = Y-axis
  // up = Z-axis
  // https://developers.google.com/kml/documentation/kmlreference#orientation
  // "Describes rotation of a 3D model's coordinate system to position the object in Google Earth."

  MYON_Xform KMLorientation;
  KMLorientation.Rotation(model_directions, MYON_Plane::World_xy);

  const MYON_3dPoint M[4] = {
    MYON_3dPoint::Origin,
    MYON_3dPoint::Origin + ModelEast().UnitVector(),
    MYON_3dPoint::Origin + ModelNorth().UnitVector(),
    MYON_3dPoint::Origin + MYON_CrossProduct(ModelEast().UnitVector(), ModelNorth().UnitVector())
  };
  const MYON_3dPoint E[sizeof(M)/sizeof(M[0])] = {
    MYON_3dPoint::Origin,
    MYON_3dPoint::Origin + MYON_3dVector::XAxis,
    MYON_3dPoint::Origin + MYON_3dVector::YAxis,
    MYON_3dPoint::Origin + MYON_3dVector::ZAxis
  };
  MYON_3dPoint rM[sizeof(M) / sizeof(M[0])] = {};
  double err[sizeof(M) / sizeof(M[0])] = {};
  double maxerr = 0.0;
  for (size_t i = 0; i < sizeof(M) / sizeof(M[0]); ++i)
  {
    rM[i] = KMLorientation * M[i];
    double d = E[i].DistanceTo(rM[i]);
    if (d != d || d > err[i])
      err[i] = d;
    if (err[i] != err[i] || err[i] > maxerr)
      maxerr = err[i];
  }

  if (false == maxerr <= MYON_ZERO_TOLERANCE)
  {
    MYON_ERROR("Sloppy rotation matrix.");
  }

  return KMLorientation;
}

bool MYON_EarthAnchorPoint::GetKMLOrientationAnglesRadians(double& heading_radians, double& tilt_radians, double& roll_radians) const
{
  heading_radians = MYON_DBL_QNAN;
  tilt_radians = MYON_DBL_QNAN;
  roll_radians = MYON_DBL_QNAN;
  const MYON_Xform rot = Internal_KMLOrientationXform();
  return rot.GetKMLOrientationAnglesRadians(heading_radians, tilt_radians, roll_radians);
}

bool MYON_EarthAnchorPoint::GetKMLOrientationAnglesDegrees(double& heading_degrees, double& tilt_degrees, double& roll_degrees) const
{
  heading_degrees = MYON_DBL_QNAN;
  tilt_degrees = MYON_DBL_QNAN;
  roll_degrees = MYON_DBL_QNAN;
  const MYON_Xform rot = Internal_KMLOrientationXform();
  return rot.GetKMLOrientationAnglesDegrees(heading_degrees, tilt_degrees, roll_degrees);
}

const double MYON_EarthAnchorPoint::KMLOrientationHeadingAngleRadians() const
{
  double heading_radians = MYON_DBL_QNAN;
  double tilt_radians = MYON_DBL_QNAN;
  double roll_radians = MYON_DBL_QNAN;
  const bool rc = GetKMLOrientationAnglesRadians(heading_radians, tilt_radians, roll_radians);
  return rc ? heading_radians : MYON_DBL_QNAN;
}

const double MYON_EarthAnchorPoint::KMLOrientationTiltAngleRadians() const
{
  double heading_radians = MYON_DBL_QNAN;
  double tilt_radians = MYON_DBL_QNAN;
  double roll_radians = MYON_DBL_QNAN;
  const bool rc = GetKMLOrientationAnglesRadians(heading_radians, tilt_radians, roll_radians);
  return rc ? tilt_radians : MYON_DBL_QNAN;
}

const double MYON_EarthAnchorPoint::KMLOrientationRollAngleRadians() const
{
  double heading_radians = MYON_DBL_QNAN;
  double tilt_radians = MYON_DBL_QNAN;
  double roll_radians = MYON_DBL_QNAN;
  const bool rc = GetKMLOrientationAnglesRadians(heading_radians, tilt_radians, roll_radians);
  return rc ? roll_radians : MYON_DBL_QNAN;
}

const double MYON_EarthAnchorPoint::KMLOrientationHeadingAngleDegrees() const
{
  double heading_degrees = MYON_DBL_QNAN;
  double tilt_degrees = MYON_DBL_QNAN;
  double roll_degrees = MYON_DBL_QNAN;
  const bool rc = GetKMLOrientationAnglesDegrees(heading_degrees, tilt_degrees, roll_degrees);
  return rc ? heading_degrees : MYON_DBL_QNAN;
}

const double MYON_EarthAnchorPoint::KMLOrientationTiltAngleDegrees() const
{
  double heading_degrees = MYON_DBL_QNAN;
  double tilt_degrees = MYON_DBL_QNAN;
  double roll_degrees = MYON_DBL_QNAN;
  const bool rc = GetKMLOrientationAnglesDegrees(heading_degrees, tilt_degrees, roll_degrees);
  return rc ? tilt_degrees : MYON_DBL_QNAN;
}

const double MYON_EarthAnchorPoint::KMLOrientationRollAngleDegrees() const
{
  double heading_degrees = MYON_DBL_QNAN;
  double tilt_degrees = MYON_DBL_QNAN;
  double roll_degrees = MYON_DBL_QNAN;
  const bool rc = GetKMLOrientationAnglesDegrees(heading_degrees, tilt_degrees, roll_degrees);
  return rc ? roll_degrees : MYON_DBL_QNAN;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmSettings
//

void MYON_3dmSettings::SetCurrentLayerId(
  MYON_UUID layer_id
  )
{
  m_current_layer_id = layer_id;
  m_V5_current_layer_index = MYON_UNSET_INT_INDEX;
}

void MYON_3dmSettings::SetV5CurrentLayerIndex(
  int V5_current_layer_index
)
{
  m_V5_current_layer_index = V5_current_layer_index;
}

int MYON_3dmSettings::CurrentLayerIndex() const
{
  return m_V5_current_layer_index;
}

MYON_UUID MYON_3dmSettings::CurrentLayerId() const
{
  return m_current_layer_id;
}

void MYON_3dmSettings::SetCurrentMaterialId(
  MYON_UUID render_material_id
  )
{
  m_current_render_material_id = render_material_id;
  m_V5_current_render_material_index = MYON_UNSET_INT_INDEX;
}

int MYON_3dmSettings::CurrentMaterialIndex() const
{
  return m_V5_current_render_material_index;
}

MYON_UUID MYON_3dmSettings::CurrentMaterialId() const
{
  return m_current_render_material_id;
}


void MYON_3dmSettings::SetCurrentDimensionStyleId(
  MYON_UUID dimension_style_id
  )
{
  m_current_dimension_style_id = dimension_style_id;
  m_V5_current_dimension_style_index = MYON_UNSET_INT_INDEX;
}

int MYON_3dmSettings::CurrentDimensionStyleIndex() const
{
  return m_V5_current_dimension_style_index;
}

MYON_UUID MYON_3dmSettings::CurrentDimensionStyleId() const
{
  return m_current_dimension_style_id;
}

void MYON_3dmSettings::SetCurrentTextStyleId(
  MYON_UUID text_style_id
  )
{
  m_current_text_style_id = text_style_id;
  m_V5_current_text_style_index = MYON_UNSET_INT_INDEX;
}

int MYON_3dmSettings::CurrentTextStyleIndex() const
{
  return m_V5_current_text_style_index;
}

MYON_UUID MYON_3dmSettings::CurrentTextStyleId() const
{
  return m_current_text_style_id;
}

void MYON_3dmSettings::SetCurrentLinePatternId(
  MYON_UUID line_pattern_id
  )
{
  m_current_line_pattern_id = line_pattern_id;
  m_V5_current_line_pattern_index = MYON_UNSET_INT_INDEX;
}

int MYON_3dmSettings::CurrentLinePatternIndex() const
{
  return m_V5_current_line_pattern_index;
}

MYON_UUID MYON_3dmSettings::CurrentLinePatternId() const
{
  return m_current_line_pattern_id;
}

void MYON_3dmSettings::SetCurrentHatchPatternId(
  MYON_UUID hatch_pattern_id
  )
{
  m_current_hatch_pattern_id = hatch_pattern_id;
}

MYON_UUID MYON_3dmSettings::CurrentHatchPatternId() const
{
  return m_current_hatch_pattern_id;
}


//void MYON_3dmSettings::Default()
//{
//  // default properties
//  m_model_URL.Destroy();
//  m_model_basepoint.Set(0.0,0.0,0.0);
//  m_earth_anchor_point.Default();
//  m_ModelUnitsAndTolerances = MYON_3dmUnitsAndTolerances::Millimeters;
//  m_PageUnitsAndTolerances = MYON_3dmUnitsAndTolerances::Millimeters;
//
//  m_RenderMeshSettings = MYON_MeshParameters::DefaultMesh;
//  m_CustomRenderMeshSettings = MYON_MeshParameters::DefaultMesh;
//  m_AnalysisMeshSettings = MYON_MeshParameters::DefaultAnalysisMesh;
//
//  m_IO_settings.Default();
//  
//  m_AnnotationSettings.Default();
//  m_named_cplanes.Empty();
//  m_named_views.Empty();
//  m_views.Empty();
//  m_active_view_id = MYON_nil_uuid;
//
//  m_V5_current_layer_index = MYON_UNSET_INT_INDEX;
//  m_current_layer_id = MYON_nil_uuid;
//
//  m_V5_current_render_material_index = MYON_UNSET_INT_INDEX;
//  m_current_render_material_id = MYON_nil_uuid;
//
//
//  m_V5_current_text_style_index = MYON_UNSET_INT_INDEX;
//  m_current_text_style_id = MYON_nil_uuid;
//
//  m_V5_current_dimension_style_index = 0;
//  m_current_dimension_style_id = MYON_nil_uuid;
//
//  m_current_color.SetRGB(0,0,0);
//  m_current_color_source = MYON::color_from_layer;
//
//  m_V5_current_line_pattern_index = MYON_UNSET_INT_INDEX;
//  m_current_line_pattern_id = MYON_nil_uuid;
//
//  m_current_plot_color = MYON_UNSET_COLOR;
//  m_current_plot_color_source = MYON::plot_color_from_layer;
//
//  m_current_wire_density = 1;
//
//  m_RenderSettings = MYON_3dmRenderSettings::Default;
//
//  m_GridDefaults.Default();
//
//  m_linetype_display_scale = 1.0;
//
//  m_plugin_list.Destroy();
//}


MYON_MeshParameters::MESH_STYLE MYON_3dmSettings::RenderMeshStyle(
  MYON_MeshParameters::MESH_STYLE no_match_found_result
  ) const
{
  return m_RenderMeshSettings.GeometrySettingsRenderMeshStyle(
     &m_CustomRenderMeshSettings,
     no_match_found_result
    );
}


bool MYON_3dmIOSettings::Read(MYON_BinaryArchive& file)
{
  *this = MYON_3dmIOSettings::Default;

  int major_version = 0;
  int minor_version = 0;
  bool rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    rc = ( 1 == major_version );
    if (!rc) break;

    rc = file.ReadBool(&m_bSaveTextureBitmapsInFile);
    if(!rc) break;

    rc = file.ReadInt(&m_idef_link_update);
    if(!rc) break;

    if ( 0 == m_idef_link_update && file.Archive3dmVersion() >= 5 )
    {
      // 7 February 2011 - old 0 value is no longer an option.
      m_idef_link_update = 1;
    }

    break;
  }

  if ( !file.EndRead3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_3dmIOSettings::Write(MYON_BinaryArchive& file) const
{
  bool rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if (!rc)
    return false;
  for(;;)
  {
    rc = file.WriteBool(m_bSaveTextureBitmapsInFile);
    if(!rc) break;

    int i = m_idef_link_update;
    if ( 0 == i && file.Archive3dmVersion() >= 5 )
    {
      // 7 February 2011 - old 0 value is no longer an option.
      i = 1;
    }
    rc = file.WriteInt(i);
    if(!rc) break;

    break;
  }
  if ( !file.EndWrite3dmChunk() )
    rc = false;

  return rc;
}


static bool MYON_3dmSettings_Read_v1_TCODE_NAME(MYON_BinaryArchive& file, MYON_wString& str )
{
  // reads legacy 1.0 named view TCODE_NAME chunk
  str.Empty();
  int len = 0;
  bool rc = file.ReadInt( &len );
  if (rc && len > 0) {
    char* name = (char*)oncalloc( 1, len + 1);
    rc = file.ReadString( len, name );
    if (rc)
      str = name; // ASCII -> UNICODE
    if (name)
      onfree(name);
  }
  return rc;
}

static bool MYON_3dmSettings_Read_v1_TCODE_CPLANE(MYON_BinaryArchive& file, MYON_3dmConstructionPlane& cplane)
{
  // reads legacy 1.0 named view TCODE_CPLANE chunk

  // do NOT call cplane.Default() here
  bool rc = true;
	MYON_3dPoint origin;
	MYON_3dVector xaxis, yaxis;
	double gridsize;
	int gridsections, gridthicksections;
  if (rc) rc = file.ReadPoint( origin );
  if (rc) rc = file.ReadVector( xaxis );
  if (rc) rc = file.ReadVector( yaxis );
  if (rc) 
  {
    rc = file.ReadDouble(&gridsize);
    if (rc) 
    {
      rc = file.ReadInt(&gridsections);
      if (rc) 
      {
        rc = file.ReadInt(&gridthicksections);
        if (rc) 
        {
          cplane.m_plane.CreateFromFrame(origin,xaxis,yaxis);
          cplane.m_grid_line_count = gridsections;
          cplane.m_grid_thick_frequency = gridthicksections;
          cplane.m_grid_spacing = gridsize;
          cplane.m_snap_spacing = gridsize;
        }
        }
    }
  }
  return rc;
}

static bool MYON_3dmSettings_Read_v1_TCODE_VIEW(MYON_BinaryArchive& file, MYON_3dmView& view)
{
  // reads legacy 1.0 named view TCODE_VIEW chunk
  // do NOT call view.Default() here
  bool rc = true;

	int projection, valid;
	double angle1, angle2, angle3, viewsize, cameradist;
  MYON_3dPoint target_point;
  while(rc)
  {
    rc = file.ReadInt(&projection);
    if (!rc) break;
    rc = file.ReadInt(&valid);
    if (!rc) break;
    rc = file.ReadPoint( target_point );
    if (!rc) break;
    rc = file.ReadDouble( &angle1 );
    if (!rc) break;
    rc = file.ReadDouble( &angle2 );
    if (!rc) break;
    rc = file.ReadDouble( &angle3 );
    if (!rc) break;
    rc = file.ReadDouble( &viewsize );
    if (!rc) break;
    rc = file.ReadDouble( &cameradist );
    if (!rc) break;

    if( cameradist <= 0.0 || cameradist >= ( DBL_MAX / 2.0 ))
      cameradist = 100.0;
    if( viewsize <= 0.0 || viewsize >= ( DBL_MAX / 2.0 ))
      viewsize = 0.125;
    MYON_ViewportFromRhinoView(
          projection == 2 ? MYON::perspective_view : MYON::parallel_view,
          target_point,
          angle1,
          angle2,
          angle3,
          viewsize,
          cameradist,
          100, // screen_width, 
          100, // screen_height,
          view.m_vp
          );
    break;
  }

  return rc;
}

static bool MYON_3dmSettings_Read_v1_TCODE_NAMED_VIEW(MYON_BinaryArchive& file, MYON_3dmView& view)
{
  // reads legacy 1.0 named view TCODE_NAMED_VIEW chunk
  view.Default();
  bool rc = true;
  unsigned int tcode;
  MYON__INT64 big_value;

  while(rc) 
  {
    rc = file.BeginRead3dmBigChunk( &tcode, &big_value );
    if (!rc )
      break;
    switch(tcode) {

    case TCODE_NAME:
      rc = MYON_3dmSettings_Read_v1_TCODE_NAME(file,view.m_name);
      break;

    case TCODE_CPLANE:
      rc = MYON_3dmSettings_Read_v1_TCODE_CPLANE(file,view.m_cplane);
      break;

    case TCODE_VIEW:
      rc = MYON_3dmSettings_Read_v1_TCODE_VIEW( file, view );
      break;

    case TCODE_SHOWGRID:
      view.m_bShowConstructionGrid = big_value?true:false;
      break;
						
    case TCODE_SHOWGRIDAXES:
      view.m_bShowConstructionAxes = big_value?true:false;
      break;
						
    case TCODE_SHOWWORLDAXES:
      view.m_bShowWorldAxes = big_value?true:false;
      break; 			
      
    }
    if ( !file.EndRead3dmChunk() )
      rc = false;
    if ( tcode == TCODE_ENDOFTABLE )
      break;
  }
  return rc;
}

static bool MYON_3dmSettings_Read_v1_TCODE_NAMED_CPLANE(MYON_BinaryArchive& file, MYON_3dmConstructionPlane& cplane)
{
  // reads legacy 1.0 named construction plane TCODE_NAMED_CPLANE chunk
  cplane.Default();

  bool rc = true;
  unsigned int tcode;
  MYON__INT64 big_value;

  while(rc) 
  {
    rc = file.BeginRead3dmBigChunk( &tcode, &big_value );
    if (!rc )
      break;
    switch(tcode) {

    case TCODE_NAME:
      rc = MYON_3dmSettings_Read_v1_TCODE_NAME(file, cplane.m_name );
      break;

    case TCODE_CPLANE:
      rc = MYON_3dmSettings_Read_v1_TCODE_CPLANE(file, cplane );
      break;
    }
    if ( !file.EndRead3dmChunk() )
      rc = false;
    if ( tcode == TCODE_ENDOFTABLE )
      break;
  }
  return rc;
}

static bool MYON_3dmSettings_Read_v1_TCODE_UNIT_AND_TOLERANCES(
  MYON_BinaryArchive& file, 
  MYON_3dmUnitsAndTolerances& UnitsAndTolerances 
  )
{
  bool rc = true;
  int v = 0;
  int us = 0;
  UnitsAndTolerances = MYON_3dmUnitsAndTolerances::Millimeters;
  if (rc) 
    rc = file.ReadInt( &v ); // should get v = 1
  if (rc) 
    rc = file.ReadInt( &us );
  switch (us) 
  {
  case 0: // NO_UNIT_SYSTEM:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::None);
    break;
  case 1: // MICRONS:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Microns);
    break;
  case 2: // MILLIMETERS:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Millimeters);
    break;
  case 3: // CENTIMETERS:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Centimeters);
    break;
  case 4: // METERS:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Meters);
    break;
  case 5: // KILOMETERS:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Kilometers);
    break;
  case 6: // MICROINCHES:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Microinches);
    break;
  case 7: // MILS:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Mils);
    break;
  case 8: // INCHES:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Inches);
    break;
  case 9: // FEET:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Feet);
    break;
  case 10: // MILES:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::Miles);
    break;
  default: // NO_UNIT_SYSTEM:
    UnitsAndTolerances.m_unit_system.SetUnitSystem(MYON::LengthUnitSystem::None);
    break;
  }
  if (rc) rc = file.ReadDouble( &UnitsAndTolerances.m_absolute_tolerance );
  if (rc) rc = file.ReadDouble( &UnitsAndTolerances.m_relative_tolerance );
  if (rc) rc = file.ReadDouble( &UnitsAndTolerances.m_angle_tolerance );
  return rc;
}

static bool MYON_3dmSettings_Read_v1_TCODE_VIEWPORT(MYON_BinaryArchive& file, MYON_3dmView& view)
{
  // reads legacy 1.0 named construction plane TCODE_VIEWPORT chunk
  view.Default();
  bool rc = true;
  MYON__UINT32 tcode;
  MYON__INT64 big_value;

  double clipdist = 0.0;
  double snapsize = 0.0;

  int chunk_count = 0;// debugging counter
  for ( chunk_count = 0; rc; chunk_count++ )
  {
    rc = file.BeginRead3dmBigChunk( &tcode, &big_value );
    if (!rc )
      break;
    switch(tcode) {

    case TCODE_NEAR_CLIP_PLANE:
      rc = file.ReadDouble(&clipdist);
      break;

    case TCODE_SNAPSIZE:
      rc = file.ReadDouble(&snapsize);
      break;
      
    case TCODE_NAME:
      rc = MYON_3dmSettings_Read_v1_TCODE_NAME(file,view.m_name);
      break;

    case TCODE_CPLANE:
      rc = MYON_3dmSettings_Read_v1_TCODE_CPLANE(file,view.m_cplane);
      break;

    case TCODE_VIEW:
      rc = MYON_3dmSettings_Read_v1_TCODE_VIEW( file, view );
      break;

    case TCODE_SHOWGRID:
      view.m_bShowConstructionGrid = big_value?true:false;
      break;
						
    case TCODE_SHOWGRIDAXES:
      view.m_bShowConstructionAxes = big_value?true:false;
      break;
						
    case TCODE_SHOWWORLDAXES:
      view.m_bShowWorldAxes = big_value?true:false;
      break; 			
      
    case TCODE_VIEWPORT_POSITION:
      rc = file.ReadDouble(&view.m_position.m_wnd_left);
      rc = file.ReadDouble(&view.m_position.m_wnd_top);
      rc = file.ReadDouble(&view.m_position.m_wnd_right);
      rc = file.ReadDouble(&view.m_position.m_wnd_bottom);
      break;
						
    case TCODE_VIEWPORT_TRACEINFO:
      {
        MYON_3dPoint origin;
        MYON_3dVector xaxis, yaxis;
        if (rc) rc = file.ReadPoint( origin );
        if (rc) rc = file.ReadVector( xaxis );
        if (rc) rc = file.ReadVector( yaxis );
        view.m_trace_image.m_plane.CreateFromFrame(origin,xaxis,yaxis);
        if (rc) rc = file.ReadDouble(&view.m_trace_image.m_width);
        if (rc) rc = file.ReadDouble(&view.m_trace_image.m_height);
        if (rc)
        {
          MYON_wString bitmap_filename;
          rc = MYON_3dmSettings_Read_v1_TCODE_NAME(file, bitmap_filename);
          if (rc) view.m_trace_image.m_image_file_reference.SetFullPath(bitmap_filename,false);
        }
      }
      break;
      
    case TCODE_VIEWPORT_WALLPAPER:
      {
        MYON_wString bitmap_filename;
        rc = MYON_3dmSettings_Read_v1_TCODE_NAME(file, bitmap_filename);
        if (rc)
          view.m_wallpaper_image.m_image_file_reference.SetFullPath(bitmap_filename,false);
      }
      break;
						
    case TCODE_HIDE_TRACE:
      // TCODE_HIDE_TRACE was used in early 1.0 betas.  
      // It should have add the short bit set and it is no longer used.
      // This case is here so that these old files will read correctly.
      tcode |= TCODE_SHORT; // so goo skip will work
      break;
      
    case TCODE_MAXIMIZED_VIEWPORT:
      if ( big_value )
        view.m_position.m_bMaximized = true;
      break; 

    case TCODE_VIEWPORT_V1_DISPLAYMODE: // short TCODE with display mode value
      if ( MYON_nil_uuid == view.m_display_mode_id )
      {
        switch ( big_value ) 
        {
        case 0: // wireframe working mode
          view.m_display_mode_id = MYON_StandardDisplayModeId::Wireframe;
          break;
        case 1: // shaded working mode
          view.m_display_mode_id = MYON_StandardDisplayModeId::Shaded;
          break;
        }
      }
      break;
						
    }
    if ( !file.EndRead3dmChunk() )
      rc = false;
    if ( tcode == TCODE_ENDOFTABLE )
      break;
  }
  return rc;
}

bool MYON_3dmSettings::Read_v1( MYON_BinaryArchive& file )
{
  //bool bGotSomething = false;
  bool rc = false;
  // read settings from old version 1 file
  MYON__UINT64 pos0 = file.CurrentPosition();

  // need to start at the beginning of the file
  MYON__UINT32 tcode;
  MYON__INT64 big_value;
  rc = file.SeekFromStart(32)?true:false; // skip 32 byte header
  
  int chunk_count = 0; // debugging counter
  for ( chunk_count = 0; rc; chunk_count++ )
  {
    rc = file.BeginRead3dmBigChunk( &tcode, &big_value );
    if (!rc)
    {
      rc = true;
      break; // assume we are at the end of the file
    }

    switch(tcode) {
    case TCODE_VIEWPORT:
      //bGotSomething = true;
      {
        MYON_3dmView view;
        rc = MYON_3dmSettings_Read_v1_TCODE_VIEWPORT(file, view);
        if (rc)
          m_views.Append(view);
      }
      break;

    case TCODE_NAMED_CPLANE:
      //bGotSomething = true;
      {
        MYON_3dmConstructionPlane cplane;
        rc = MYON_3dmSettings_Read_v1_TCODE_NAMED_CPLANE(file,cplane);
        if (rc)
          m_named_cplanes.Append(cplane);
      }
      break;

    case TCODE_NAMED_VIEW:
      //bGotSomething = true;
      {
        MYON_3dmView view;
        rc = MYON_3dmSettings_Read_v1_TCODE_NAMED_VIEW(file, view);
        if (rc)
          m_named_views.Append(view);
      }
      break;
    
    case TCODE_UNIT_AND_TOLERANCES:
      //bGotSomething = true;
      rc = MYON_3dmSettings_Read_v1_TCODE_UNIT_AND_TOLERANCES(file,m_ModelUnitsAndTolerances);
      break;
    }

    rc = file.EndRead3dmChunk();
  }

  if (false == file.SeekFromStart(pos0))
    rc = false;

  return rc;
}

bool MYON_3dmSettings::Read_v2(MYON_BinaryArchive& file )
{
  bool rc = true;
  MYON__UINT32 tcode;
  MYON__INT64 big_value;

  bool bHave3dmRenderSettings = false;

  while(rc) 
  {
    tcode = 0;
    big_value = 0;
    rc = file.BeginRead3dmBigChunk( &tcode, &big_value );
    if ( !rc )
      break;

    switch(tcode) 
    {
    case TCODE_SETTINGS_PLUGINLIST: 
      {
        int major_version = 0, minor_version = 0, count = 0, i;
        rc = file.Read3dmChunkVersion(&major_version,&minor_version);
        if (rc && 1 == major_version && minor_version >= 0 )
        {
          rc = file.ReadInt( &count );
          if ( count > 0 )
          {
            for ( i = 0; rc && i < count; i++ )
            {
              rc = m_plugin_list.AppendNew().Read(file);
            }
          }
        }
      }
      break;
      
    case TCODE_SETTINGS_UNITSANDTOLS: // units and tolerances
      rc = m_ModelUnitsAndTolerances.Read(file);
      // Copy model settings to page settings so reading old files
      // will work right.  If the file is new enough to have page
      // units and tolerances in it, they get read later.
      m_PageUnitsAndTolerances = m_ModelUnitsAndTolerances;
      break;
      
    case TCODE_SETTINGS_RENDERMESH:
      rc = m_RenderMeshSettings.Read(file);
      break;
      
    case TCODE_SETTINGS_ANALYSISMESH:
      rc = m_AnalysisMeshSettings.Read(file);
      break;
      
    case TCODE_SETTINGS_ANNOTATION:
      rc = m_AnnotationSettings.Read(file);
      break;
      
    case TCODE_SETTINGS_NAMED_CPLANE_LIST: // named cplanes
      {
        m_named_cplanes.Empty();
        MYON__UINT32 subtcode = 0;
        MYON__INT64 subvalue = 0;
        int count, i;
        rc = file.ReadInt(&count);
        for ( i = 0; i < count && rc ; i++ ) {
          rc = file.BeginRead3dmBigChunk( &subtcode, &subvalue );
          if (rc ) {
            if ( subtcode != TCODE_VIEW_CPLANE )
              rc = false;
            else {
              MYON_3dmConstructionPlane& cplane = m_named_cplanes.AppendNew();
              rc = cplane.Read(file);
            }
            if ( !file.EndRead3dmChunk() ) {
              rc = false;
            }
          }
        }
      }
      break;
      
    case TCODE_SETTINGS_NAMED_VIEW_LIST: // named views
      {
        m_named_views.Empty();
        MYON__UINT32 subtcode = 0;
        MYON__INT64 subvalue = 0;
        int count, i;
        rc = file.ReadInt(&count);
        for ( i = 0; i < count && rc ; i++ ) 
        {
          rc = file.BeginRead3dmBigChunk( &subtcode, &subvalue );
          if (rc ) 
          {
            if ( subtcode != TCODE_VIEW_RECORD )
              rc = false;
            else 
            {
              MYON_3dmView& namedView = m_named_views.AppendNew();
              rc = namedView.Read(file);

			  //Named views were attached to the named view table without an id.
			  //the documentation says it will always have one, so this code ensures that the ids are non-nil.
			  //http://mcneel.myjetbrains.com/youtrack/issue/RH-19520
			  if (MYON_nil_uuid == namedView.m_named_view_id)
			  {
				  MYON_CreateUuid(namedView.m_named_view_id);
			  }
            }
            if ( !file.EndRead3dmChunk() )
            {
              rc = false;
            }
          }
        }
      }
      break;
      
    case TCODE_SETTINGS_VIEW_LIST: // active view is first in list
      {
        m_views.Empty();
        MYON__UINT32 subtcode = 0;
        MYON__INT64 subvalue = 0;
        int count, i;
        rc = file.ReadInt(&count);
        m_views.Reserve(count);
        for ( i = 0; i < count && rc ; i++ ) 
        {
          rc = file.BeginRead3dmBigChunk( &subtcode, &subvalue );
          if (rc ) 
          {
            if ( subtcode != TCODE_VIEW_RECORD )
              rc = false;
            else 
            {
              MYON_3dmView& view = m_views.AppendNew();
              rc = view.Read(file);
            }
            if ( !file.EndRead3dmChunk() )
            {
              rc = false;
            }
          }
        }
      }
      break;
      
    case TCODE_SETTINGS__NEVER__USE__THIS:
      {
        if ( 28 == big_value )
        {
          // 23 March 2005 Dale Lear - this was the MYON_LineStyle
          //                           and a linesytlesource int
          //                           that never got used.
          unsigned char b[24];
          if (rc) rc = file.ReadByte(24,b);
          // The other 4 bytes are a 32 bit chunk crc
          // that gets read by EndRead3dmChunk()
        }
      }
      break;

    case TCODE_SETTINGS_CURRENT_LAYER_INDEX:
      if ( big_value < -1 || big_value > 0x7FFFFFFF )
      {
        MYON_ERROR("MYON_3dmSettings::Read_v2() - TCODE_SETTINGS_CURRENT_LAYER_INDEX - invalid layer index value");
      }
      else
      {
        m_V5_current_layer_index = (int)big_value;
        m_current_layer_id = MYON_nil_uuid;
      }
      break;
      
    case TCODE_SETTINGS_CURRENT_FONT_INDEX:
      if ( big_value < -1 || big_value > 0x7FFFFFFF )
      {
        MYON_ERROR("MYON_3dmSettings::Read_v2() - TCODE_SETTINGS_CURRENT_FONT_INDEX - invalid font index value");
      }
      else
      {
        // in archives with opennurbs version >= 200106100
        m_V5_current_text_style_index = (int)big_value;
        m_current_text_style_id = MYON_nil_uuid;
      }
      break;
      
    case TCODE_SETTINGS_CURRENT_DIMSTYLE_INDEX:
      if ( big_value < -1 || big_value > 0x7FFFFFFF )
      {
        MYON_ERROR("MYON_3dmSettings::Read_v2() - TCODE_SETTINGS_CURRENT_DIMSTYLE_INDEX - invalid dimstyle index value");
      }
      else
      {
        // in archives with opennurbs version >= 200106100
        m_V5_current_dimension_style_index = (int)big_value;
        m_current_dimension_style_id = MYON_nil_uuid;
      }
      break;
      
    case TCODE_SETTINGS_CURRENT_MATERIAL_INDEX:
      {
        int i32 = 0;
        if (rc) rc = file.ReadInt( &m_V5_current_render_material_index );
        if (rc) rc = file.ReadInt( &i32 );
        if (rc) m_current_material_source = MYON::ObjectMaterialSource(i32);
        m_current_render_material_id = MYON_nil_uuid;
      }
      break;
      
    case TCODE_SETTINGS_CURRENT_COLOR:
      {
        int i32 = 0;
        if (rc) rc = file.ReadColor( m_current_color );
        if (rc) rc = file.ReadInt( &i32 );
        if (rc) m_current_color_source = MYON::ObjectColorSource(i32);
      }
      break;
      
    case TCODE_SETTINGS_CURRENT_WIRE_DENSITY:
      if ( big_value < -2 || big_value > 0x7FFFFFFF )
      {
        MYON_ERROR("MYON_3dmSettings::Read_v2() - TCODE_SETTINGS_CURRENT_WIRE_DENSITY - invalid current_wire_density value");
      }
      else
      {
        m_current_wire_density = (int)big_value;
      }
      break;
      
    case TCODE_SETTINGS_RENDER:
      rc = m_RenderSettings.Read(file)?true:false;
      if (rc)
        bHave3dmRenderSettings = true;
      break;

      
    case TCODE_SETTINGS_RENDER_USERDATA:
      if (bHave3dmRenderSettings)
      {
        // 2016-Nov-28 RH-33298 Dale Lear
        // MYON_3dmRenderSettings user data in MYON_3dmSettings
        //   I added support for saving userdata attached to the m_RenderSettings MYON_3dmRenderSettings.
        //   Ideally, the MYON_3dmRenderSettings would be read by calling file.ReadObject(), but
        //   userdata support is being added years after millions of files have been written by calling
        //   MYON_3dmRenderSettings.Write()/Read().
        rc = file.ReadObjectUserData(m_RenderSettings);
      }
      break;

    case TCODE_SETTINGS_GRID_DEFAULTS:
      rc = m_GridDefaults.Read(file);
      break;

    case TCODE_SETTINGS_MODEL_URL: // added 21 June 2001
      rc = file.ReadString(m_model_URL);
      break;

    case TCODE_SETTINGS_ATTRIBUTES:
      {
        int major_version = 0;
        int minor_version = 0;
        for(;;)
        {
          rc = file.Read3dmChunkVersion(&major_version,&minor_version);
          if (!rc) break;
          if ( 1 == major_version )
          {
            // version 1.0 fields 23 March 2005
            rc = file.ReadDouble( &m_linetype_display_scale );
            if (!rc) break;

            rc = file.ReadColor(m_current_plot_color);
            if (!rc) break;
  
            int i;
            rc = file.ReadInt(&i);
            if (!rc) break;
            m_current_plot_color_source = MYON::PlotColorSource(i);

            m_current_line_pattern_id = MYON_nil_uuid;
            rc = file.ReadInt(&m_V5_current_line_pattern_index);
            if (!rc) break;

            rc = file.ReadInt(&i);
            if (!rc) break;
            m_current_linetype_source = MYON::ObjectLinetypeSource(i);

            if ( minor_version >= 1 )
            {
              // Added 6 Feb 2006
              int mjv = 1, mnv = 1;
              rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&mjv,&mnv);
              if (rc)
              {
                rc = m_PageUnitsAndTolerances.Read(file);
                if ( !file.EndRead3dmChunk() )
                  rc = false;
              }


              if ( minor_version >= 2 )
              {
                // 1 Mar 2006 1.2 fields
                rc = file.ReadUuid(m_active_view_id);
                if (!rc) break;

                if ( minor_version >= 3 )
                {
                  rc = file.ReadPoint( m_model_basepoint);
                  if (!rc) break;
                  rc = m_earth_anchor_point.Read(file);
                  if (!rc) break;

                  if ( minor_version >= 4 )
                  {
                    rc = file.ReadBool(&m_IO_settings.m_bSaveTextureBitmapsInFile);
                    if (rc && minor_version >= 5)
                    {
                      rc = m_IO_settings.Read(file);
                      if (rc && minor_version >= 6 )
                      {
                        // 7 June 2006
                        m_CustomRenderMeshSettings.Read(file);

                        if (rc && minor_version >= 7 )
                        {
                          // V6 and later files use ids for the "current" table elements.
                          m_V5_current_layer_index = MYON_UNSET_INT_INDEX;
                          m_V5_current_render_material_index = MYON_UNSET_INT_INDEX;
                          m_V5_current_line_pattern_index = MYON_UNSET_INT_INDEX;
                          m_V5_current_text_style_index = MYON_UNSET_INT_INDEX;
                          m_V5_current_dimension_style_index = MYON_UNSET_INT_INDEX;
                          // 30 September 2015
                          rc = file.ReadUuid(m_current_layer_id);
                          if (!rc) break;
                          rc = file.ReadUuid(m_current_render_material_id);
                          if (!rc) break;
                          rc = file.ReadUuid(m_current_line_pattern_id);
                          if (!rc) break;
                          rc = file.ReadUuid(m_current_text_style_id);
                          if (!rc) break;
                          rc = file.ReadUuid(m_current_dimension_style_id);
                          if (!rc) break;
                          rc = file.ReadUuid(m_current_hatch_pattern_id);
                          if (!rc) break;
                        }
                      }
                    }
                  }
                }
              }
            }

          }

          break;
        }
      }
      break;

    default:
      // information added in future will be skipped by file.EndRead3dmChunk()
      break;
    }

    if ( !file.EndRead3dmChunk() )
      rc = false;
    if ( TCODE_ENDOFTABLE == tcode )
      break;
  }

  return rc;
}

bool MYON_3dmSettings::Read(MYON_BinaryArchive& file )
{
  bool rc = false;

  *this = MYON_3dmSettings::Default;

  if (60 > file.Archive3dmVersion())
  {
    m_AnnotationSettings.EnableLayoutSpaceAnnotationScaling(false);
    m_AnnotationSettings.EnableModelSpaceAnnotationScaling(false);
    m_AnnotationSettings.EnableHatchScaling(false);
    m_AnnotationSettings.Enable_V5_AnnotationScaling(false);
  }



  if ( 1 == file.Archive3dmVersion() ) 
  {
    rc = Read_v1(file); 
  }
  else
  {
    rc = Read_v2(file); 
  }

  return rc;
}


static bool MYON_3dmSettings_Write_v1_TCODE_UNIT_AND_TOLERANCES(MYON_BinaryArchive& file, const MYON_3dmUnitsAndTolerances& UnitsAndTolerances )
{
  bool rc = true;
  int v = 1, us = 0;
  if (rc) rc = file.WriteInt( v ); // v = 1
  switch (UnitsAndTolerances.m_unit_system.UnitSystem()) 
  {
  case MYON::LengthUnitSystem::None: 
    us=0; // NO_UNIT_SYSTEM
    break;
  case MYON::LengthUnitSystem::Microns: 
    us=1; // MICRONS
    break;
  case MYON::LengthUnitSystem::Millimeters:
    us=2; // MILLIMETERS
    break;
  case MYON::LengthUnitSystem::Centimeters:
    us=3; // CENTIMETERS
    break;
  case MYON::LengthUnitSystem::Meters:
    us=4; // METERS
    break;
  case MYON::LengthUnitSystem::Kilometers:
    us=5; // KILOMETERS
    break;
  case MYON::LengthUnitSystem::Microinches:
    us=6; // MICROINCHES
    break;
  case MYON::LengthUnitSystem::Mils:
    us=7; // MILS
    break;
  case MYON::LengthUnitSystem::Inches:
    us=8; // INCHES
    break;
  case MYON::LengthUnitSystem::Feet:
    us=9; // FEET
    break;
  case MYON::LengthUnitSystem::Miles:
    us=10; // MILES
    break;
  default:
    us=0; // NO_UNIT_SYSTEM
    break;
  }
  if (rc) rc = file.WriteInt( us );
  if (rc) rc = file.WriteDouble( UnitsAndTolerances.m_absolute_tolerance );
  if (rc) rc = file.WriteDouble( UnitsAndTolerances.m_relative_tolerance );
  if (rc) rc = file.WriteDouble( UnitsAndTolerances.m_angle_tolerance );

  return rc;
}

bool MYON_3dmSettings::Write_v1(MYON_BinaryArchive& file) const
{
  bool rc = true;

  // version 1 units and tolerances chunk
  rc = file.BeginWrite3dmChunk(TCODE_UNIT_AND_TOLERANCES,0);
  if (rc) {
    rc = MYON_3dmSettings_Write_v1_TCODE_UNIT_AND_TOLERANCES( file, m_ModelUnitsAndTolerances );
    if (!file.EndWrite3dmChunk())
      rc = false;
  }

  return rc;
}

bool MYON_3dmSettings::Write_v2(MYON_BinaryArchive& file) const
{
  int i;
  bool rc = true;

  // TCODE_SETTINGS_PLUGINLIST - plugins that may have saved userdata in the file
  if (rc && file.Archive3dmVersion() >= 4 && m_plugin_list.Count() > 0 )
  {
    // The plug-in list chunk needs to be first, so the plug-ins that save
    // userdata on views can be loaded as needed.
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_PLUGINLIST,0);
    if ( rc ) 
    {
      if (rc) rc = file.Write3dmChunkVersion(1,0);
      if (rc) rc = file.WriteInt( m_plugin_list.Count() );
      for ( i = 0; rc && i < m_plugin_list.Count(); i++ )
      {
        rc = m_plugin_list[i].Write(file);
      }

      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_PROPERTIES_UNITSANDTOLS - units and tolerances
  if ( rc  ) {
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_UNITSANDTOLS,0);
    if ( rc ) {
      rc = m_ModelUnitsAndTolerances.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_RENDERMESH - rendering defaults
  if ( rc  ) {
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_RENDERMESH,0);
    if ( rc ) {
      rc = m_RenderMeshSettings.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_ANALYSISMESH - analysis mesh defaults
  if ( rc  ) {
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_ANALYSISMESH,0);
    if ( rc ) {
      rc = m_AnalysisMeshSettings.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_ANNOTATION - annotation settings
  if ( rc  ) {
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_ANNOTATION,0);
    if ( rc ) {
      rc = m_AnnotationSettings.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_NAMED_CPLANE_LIST
  if ( rc  ) {
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_NAMED_CPLANE_LIST,0);
    if ( rc ) {
      const int count = m_named_cplanes.Count();
      rc = file.WriteInt(count);
      for ( i = 0; i < count && rc; i++ ) {
        rc = file.BeginWrite3dmChunk( TCODE_VIEW_CPLANE, 0 );
        if (rc ) {
          rc = m_named_cplanes[i].Write(file);
          if ( !file.EndWrite3dmChunk() )
            rc = false;
        }
      }
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_NAMED_VIEW_LIST
  if ( rc  ) {
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_NAMED_VIEW_LIST,0);
    if ( rc ) {
      const int count = m_named_views.Count();
      rc = file.WriteInt(count);
      for ( i = 0; i < count && rc; i++ ) {
        rc = file.BeginWrite3dmChunk( TCODE_VIEW_RECORD, 0 );
        if (rc ) {
          rc = m_named_views[i].Write(file);
          if ( !file.EndWrite3dmChunk() )
            rc = false;
        }
      }
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_VIEW_LIST
  if ( rc  ) {
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_VIEW_LIST,0);
    if ( rc ) {
      const int count = m_views.Count();
      rc = file.WriteInt(count);
      for ( i = 0; i < count && rc; i++ ) {
        rc = file.BeginWrite3dmChunk( TCODE_VIEW_RECORD, 0 );
        if (rc ) {
          rc = m_views[i].Write(file);
          if ( !file.EndWrite3dmChunk() )
            rc = false;
        }
      }
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_CURRENT_LAYER_INDEX
  if (rc) {
    int V5_index = m_V5_current_layer_index >= 0 ? m_V5_current_layer_index : 0;
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_CURRENT_LAYER_INDEX, V5_index );
    if ( !file.EndWrite3dmChunk() )
      rc = false;
  }

  // TCODE_SETTINGS_CURRENT_MATERIAL_INDEX
  if (rc) {
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_CURRENT_MATERIAL_INDEX, 0 );
    if (rc) {
      int V5_index = m_V5_current_render_material_index >= 0 ? m_V5_current_render_material_index : -1;
      rc = file.WriteInt( V5_index );
      i = m_current_material_source;
      if (rc) rc = file.WriteInt( i );
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_CURRENT_COLOR
  if (rc) {
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_CURRENT_COLOR, 0 );
    if (rc) {
      rc = file.WriteColor( m_current_color );
      i = m_current_color_source;
      if (rc) rc = file.WriteInt( i );
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }


  // TCODE_SETTINGS_CURRENT_WIRE_DENSITY
  if (rc) {
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_CURRENT_WIRE_DENSITY, m_current_wire_density );
    if (rc) {
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_RENDER
  if (rc) 
  {
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_RENDER, 0 );
    if (rc) 
    {
      rc = m_RenderSettings.Write(file)?true:false;
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
    if (rc && file.Archive3dmVersion() >= 60 && file.ObjectHasUserDataToWrite(&m_RenderSettings) )
    {
      // 2016-Nov-28 RH-33298 Dale Lear
      // MYON_3dmRenderSettings user data in MYON_3dmSettings
      //   I added support for saving userdata attached to the m_RenderSettings MYON_3dmRenderSettings.
      //   Ideally, the MYON_3dmRenderSettings would be read by calling file.WriteObject(), but
      //   userdata support is being added years after millions of files have been written by calling
      //   MYON_3dmRenderSettings.Write()/Read().
      rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_RENDER_USERDATA, 0 );
      if(rc) 
      {
        rc = file.WriteObjectUserData(m_RenderSettings);
        // write a "fake" TCODE_OPENNURBS_CLASS_END end of class mark so I can use
        // MYON_BinaryArchive::ReadObjectUserData() to read the m_RenderSettings user data.
        if ( file.BeginWrite3dmChunk( TCODE_OPENNURBS_CLASS_END, 0 ) ) 
        {
          if ( !file.EndWrite3dmChunk() )
            rc = false;
        }
        else
        {
          rc = false;
        }
        if ( !file.EndWrite3dmChunk() ) // end of TCODE_SETTINGS_RENDER_USERDATA
          rc = false;
      }
    }
  }

  // TCODE_SETTINGS_GRID_DEFAULTS
  if (rc) {
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_GRID_DEFAULTS, 0 );
    if (rc) {
      rc = m_GridDefaults.Write(file);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_MODEL_URL - added 21 June 2001
  if (rc && m_model_URL.Length() > 0 ) {
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_MODEL_URL, 0 );
    if (rc) {
      rc = file.WriteString(m_model_URL);
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // TCODE_SETTINGS_CURRENT_FONT_INDEX - added 10 June 2002
  if (rc) {
    int V5_index = m_V5_current_text_style_index >= 0 ? m_V5_current_text_style_index : 0;
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_CURRENT_FONT_INDEX, V5_index );
    if ( !file.EndWrite3dmChunk() )
      rc = false;
  }
    
  // TCODE_SETTINGS_CURRENT_DIMSTYLE_INDEX - added 10 June 2002
  if (rc) {
    int V5_index = m_V5_current_dimension_style_index >= 0 ? m_V5_current_dimension_style_index : 0;
    rc = file.BeginWrite3dmChunk( TCODE_SETTINGS_CURRENT_DIMSTYLE_INDEX, V5_index );
    if ( !file.EndWrite3dmChunk() )
      rc = false;
  }

  // TCODE_SETTINGS_ATTRIBUTES
  if (rc && file.Archive3dmVersion() >= 4 )
  {
    // 23 March 2005 Dale Lear
    rc = file.BeginWrite3dmChunk(TCODE_SETTINGS_ATTRIBUTES, 0 );
    if (rc)
    {
      for(;;)
      {
        // 1.0 - 23 March 2005
        // 1.1 -  6 Feb   2006
        // 1.2 -  1 March 2006
        // 1.3 - 29 March 2006
        // 1.4 - 18 April 2006
        // 1.5 - 21 April 2006
        // 1.6 -  7 June  2006
        // 1.7 - 30 September 2015
        rc = file.Write3dmChunkVersion(1,7);

        // version 1.0 fields 
        rc = file.WriteDouble( m_linetype_display_scale );
        if (!rc) break;

        rc = file.WriteColor(m_current_plot_color);
        if (!rc) break;

        rc = file.WriteInt(m_current_plot_color_source);
        if (!rc) break;

        int V5_index = m_V5_current_line_pattern_index >= 0 ? m_V5_current_line_pattern_index : -1;
        rc = file.WriteInt(V5_index);
        if (!rc) break;

        rc = file.WriteInt(m_current_linetype_source);
        if (!rc) break;

        // 6 Feb 2006, 1.1 fields

        // the units and tols has to be inside a chunk
        rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
        if (rc)
        {
          rc = m_PageUnitsAndTolerances.Write(file);
          if ( !file.EndWrite3dmChunk() )
            rc = false;
        }
        if (!rc) break;

        // 1 Mar 2006 1.2 fields
        rc = file.WriteUuid(m_active_view_id);
        if (!rc) break;

        // 29 March 2006 1.3 fields
        rc = file.WritePoint( m_model_basepoint);
        if (!rc) break;
        rc = m_earth_anchor_point.Write(file);
        if (!rc) break;

        // 18 April 2006 1.4 fields
        rc = file.WriteBool(m_IO_settings.m_bSaveTextureBitmapsInFile);
        if (!rc) break;

        // 21 April 2006 1.5 fields
        rc = m_IO_settings.Write(file);
        if (!rc) break;

        // 7 June 2006 1.6 fields
        rc = m_CustomRenderMeshSettings.Write(file);
        if (!rc) break;

        // 1.7 - 30 September 2015
        //  switching from indices to ids
        rc = file.WriteUuid(m_current_layer_id);
        if (!rc) break;
        rc = file.WriteUuid(m_current_render_material_id);
        if (!rc) break;
        rc = file.WriteUuid(m_current_line_pattern_id);
        if (!rc) break;
        rc = file.WriteUuid(m_current_text_style_id);
        if (!rc) break;
        rc = file.WriteUuid(m_current_dimension_style_id);
        if (!rc) break;
        rc = file.WriteUuid(m_current_hatch_pattern_id);
        if (!rc) break;

        break;
      }

      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  // required TCODE_ENDOFTABLE chunk - marks end of settings table
  if ( rc ) {
    rc = file.BeginWrite3dmChunk( TCODE_ENDOFTABLE, 0 );
    if ( rc ) {
      if ( !file.EndWrite3dmChunk() )
        rc = false;
    }
  }


  return rc;
}

bool MYON_3dmSettings::Write(MYON_BinaryArchive& file) const
{
  bool rc = false;
  if ( 1 == file.Archive3dmVersion() ) 
  {
    rc = Write_v1(file); 
  }
  else
  {
    rc = Write_v2(file); 
  }
  return rc;
}

void MYON_3dmSettings::Dump( MYON_TextLog& dump ) const
{
  int i;

  const wchar_t* model_URL = static_cast< const wchar_t* >(m_model_URL);
  if ( model_URL && *model_URL ) 
  {
    dump.Print("Model URL: %ls\n",model_URL);
  }
  dump.Print("Model space units and tolerances:\n");
  dump.PushIndent();
  m_ModelUnitsAndTolerances.Dump(dump);
  dump.PopIndent();

  dump.Print("Page space units and tolerances:\n");
  dump.PushIndent();
  m_PageUnitsAndTolerances.Dump(dump);
  dump.PopIndent();

  dump.Print("Render mesh settings:\n");
  dump.PushIndent();
  m_RenderMeshSettings.Dump(dump);
  dump.PopIndent();

  dump.Print("Analysis mesh settings:\n");
  dump.PushIndent();
  m_AnalysisMeshSettings.Dump(dump);
  dump.PopIndent();

  dump.Print("Render settings:\n");
  dump.PushIndent();
  m_RenderSettings.Dump(dump);
  dump.PopIndent();

  dump.Print("Annotation settings:\n");
  dump.PushIndent();
  m_AnnotationSettings.Dump(dump);
  dump.PopIndent();

  dump.Print("Construction plane grid defaults:\n");
  dump.PushIndent();
  m_GridDefaults.Dump(dump);
  dump.PopIndent();

  dump.Print("Named construction planes:\n");
  dump.PushIndent();
  for ( i = 0; i < m_named_cplanes.Count(); i++ )
  {
    dump.Print("named construction plane %d:\n");
    dump.PushIndent();
    m_named_cplanes[i].Dump(dump);
    dump.PopIndent();
  }
  dump.PopIndent();

  dump.Print("Named views:\n");
  dump.PushIndent();
  for ( i = 0; i < m_named_views.Count(); i++ )
  {
    dump.Print("named view %d:\n",i);
    dump.PushIndent();
    m_named_views[i].Dump(dump);
    dump.PopIndent();
  }
  dump.PopIndent();

  dump.Print("Model views:\n");
  dump.PushIndent();
  for ( i = 0; i < m_views.Count(); i++ )
  {
    dump.Print("model view %d:\n",i);
    dump.PushIndent();
    m_views[i].Dump(dump);
    dump.PopIndent();
  }
  dump.PopIndent();

  dump.Print("New object attributes:\n");
  dump.PushIndent();
  {
    dump.Print("Current display color rgb");dump.PrintRGB(m_current_color); dump.Print(":\n");
    dump.Print("Current display color source = %d\n",m_current_color_source);
    dump.Print("Current plot color rgb");dump.PrintRGB(m_current_plot_color); dump.Print(":\n");
    dump.Print("Current plot color source = %d\n",m_current_plot_color_source);
    if ( MYON_UNSET_INT_INDEX != m_V5_current_render_material_index)
      dump.Print("Current V5 material index = %d\n",m_V5_current_render_material_index);
    dump.Print("Current material source = %d\n",m_current_material_source);
    if ( MYON_UNSET_INT_INDEX != m_V5_current_line_pattern_index)
      dump.Print("Current V5 linetype index = %d\n",m_V5_current_line_pattern_index);
    dump.Print("Current linetype source = %d\n",m_current_linetype_source);
    if ( MYON_UNSET_INT_INDEX != m_V5_current_layer_index)
      dump.Print("Current V5 layer index = %d\n",m_V5_current_layer_index);
    if (false == dump.IsTextHash())
    {
      // The legacy m_V5_current_text_style_index varies
      // depending on what application writes the file.
      // It must be ignored when comparing content.
      if (MYON_UNSET_INT_INDEX != m_V5_current_text_style_index)
        dump.Print("Current font index = %d\n", m_V5_current_text_style_index);
    }
    if ( MYON_UNSET_INT_INDEX != m_V5_current_dimension_style_index)
      dump.Print("Current V5 dimstyle index = %d\n",m_V5_current_dimension_style_index);
    dump.Print("Current wire density = %d\n",m_current_wire_density);
    dump.Print("Linetype diaplay scale = %g\n",m_linetype_display_scale);
  }
  dump.PopIndent();

  dump.Print("Plug-in list\n");
  dump.PushIndent();
  for ( i = 0; i < m_plugin_list.Count(); i++ )
  {
    dump.Print("plug-in %d:\n",i);
    dump.PushIndent();
    m_plugin_list[i].Dump(dump);
    dump.PopIndent();
  }
  dump.PopIndent();

}


//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmAnimationProperties
//

const MYON_3dmAnimationProperties MYON_3dmAnimationProperties::Default;

bool MYON_3dmAnimationProperties::Read(MYON_BinaryArchive& archive)
{
  //*this = MYON_3dmAnimationProperties::Default;

// If the file archive version is not 4,
  // just return.
  if( archive.Archive3dmVersion() < 4 )
    return true;

  if (archive.Archive3dmVersion() == 4)
  {
    // This fixes bugs like RH-35784
    // It appears that the current code in CAnimationToolsPlugIn::ReadDocument()
    // is not capable of reading information saved by the V4 version of this plug-in.

    return true;
  }

  // Read the major and minor version of the
  // document data
  int major = 0, minor = 0;
  if( !archive.Read3dmChunkVersion(&major, &minor) )
    return false;

  // At this point, if we've changed the format of
  // our document data, we'll want to compare the
  // major and minor revision numbers and read our
  // data accordingly.

	//READ camera and target points count
	int cp_count = 0;
  if( !archive.ReadInt(&cp_count) )
    return false;
	int tp_count = 0;
  if( !archive.ReadInt(&tp_count) )
    return false;
	if( cp_count < 1 || tp_count < 1 )
		return true;

	m_aCameraPoints.SetCount(0);
	m_aTargetPoints.SetCount(0);

	for( int i = 0; i < cp_count; i++ )
	{
		//read camera points
		MYON_3dPoint pt = MYON_3dPoint::UnsetPoint;
		if( !archive.ReadPoint( pt )  )
			return false;
		m_aCameraPoints.Append( pt );
	}
	for( int i = 0; i < tp_count; i++ )
	{
		//read camera points
		MYON_3dPoint pt = MYON_3dPoint::UnsetPoint;
		if( !archive.ReadPoint( pt )  )
			return false;
		m_aTargetPoints.Append( pt );
	}

	//READ OTHER PARAMETERS
	int act = 5;//none
	if( !archive.ReadInt( &act ) )
		return false;
	if( act < 0 || act > 5 )
		return true;
	m_CaptureTypes = static_cast<CaptureTypes>(act);

	if( !archive.ReadString(m_sFileExtension))
		return false;
	if( !archive.ReadString(m_sCaptureMethod))
		return false;
	if( !archive.ReadUuid(m_idDisplayMode))
		return false;
	if( !archive.ReadString(m_sHtmlFilename))
		return false;
	if( !archive.ReadString(m_sViewport))
		return false;
	if( !archive.ReadInt(&m_iFrameCount))
		return false;
	if( !archive.ReadInt(&m_iCurrentFrame))
		return false;
	if( !archive.ReadUuid(m_idCameraPath))
		return false;
	if( !archive.ReadUuid(m_idTargetPath))
		return false;
	if( !archive.ReadDouble(&m_dLatitude))
		return false;
	if( !archive.ReadDouble(&m_dLongitude))
		return false;
	if( !archive.ReadDouble(&m_dNorthAngle))
		return false;
	if( !archive.ReadInt(&m_iStartDay))
		return false;
	if( !archive.ReadInt(&m_iStartMonth))
		return false;
	if( !archive.ReadInt(&m_iStartYear))
		return false;
	if( !archive.ReadInt(&m_iEndDay)  )
		return false;
	if( !archive.ReadInt(&m_iEndMonth))
		return false;
	if( !archive.ReadInt(&m_iEndYear))
		return false;
	if( !archive.ReadInt(&m_iStartHour))
		return false;
	if( !archive.ReadInt(&m_iStartMinutes))
		return false;
	if( !archive.ReadInt(&m_iStartSeconds))
		return false;
	if( !archive.ReadInt(&m_iEndHour))
		return false;
	if( !archive.ReadInt(&m_iEndMinutes))
		return false;
	if( !archive.ReadInt(&m_iEndSeconds))
		return false;
	if( !archive.ReadInt(&m_iDaysBetweenFrames))
		return false;
	if( !archive.ReadInt(&m_iMinutesBetweenFrames))
		return false;
	if( !archive.ReadString(m_sFolderName))
		return false;
	if( !archive.ReadInt(&m_iLightIndex))
		return false;
	if( !archive.ReadBool(&m_bRenderFull))
		return false;
	if( !archive.ReadBool(&m_bRenderPreview))
		return false;
	
  return true;
}

bool MYON_3dmAnimationProperties::Write(MYON_BinaryArchive& archive) const
{
  int cp_count = CameraPoints().Count();
  int tp_count = TargetPoints().Count();

	//Added by Rajaa - May 2, 2009 - next 2 lines commented out (test added to CallWriteDocument)
	// Bug # 48383 - all validation happens on CallWriteDocument and not WriteDocument

	if( cp_count < 1 || tp_count < 1)
		return true;

  if( !archive.WriteInt( cp_count ) )//Count of camera points
    return true;
  if( !archive.WriteInt( tp_count ) )// count of target points
    return false;

	for( int i = 0; i < cp_count; i++ )
	{
		//write camera points
		if( !archive.WritePoint( m_aCameraPoints[i] )  )
			return false;
	}
	for( int i = 0; i < tp_count; i++ )
	{
		//write camera points
		if( !archive.WritePoint( m_aTargetPoints[i] )  )
			return false;
	}

	//SAVE OTHER PARAMETERS
//	ARecord::AType AnimationCaptureType;
	const int iCaptureType = static_cast<int>(m_CaptureTypes);
	if( !archive.WriteInt(iCaptureType))
		return false;
	if( !archive.WriteString(m_sFileExtension))
		return false;
	if( !archive.WriteString(m_sCaptureMethod))
		return false;
	if( !archive.WriteUuid(m_idDisplayMode))
		return false;
	if( !archive.WriteString(m_sHtmlFilename))
		return false;
	if( !archive.WriteString(m_sViewport))
		return false;
	if( !archive.WriteInt(m_iFrameCount))
		return false;
	if( !archive.WriteInt(m_iCurrentFrame))
		return false;
	if( !archive.WriteUuid(m_idCameraPath))
		return false;
	if( !archive.WriteUuid(m_idTargetPath))
		return false;
	if( !archive.WriteDouble(m_dLatitude))
		return false;
	if( !archive.WriteDouble(m_dLongitude))
		return false;
	if( !archive.WriteDouble(m_dNorthAngle))
		return false;
	if( !archive.WriteInt(m_iStartDay))
		return false;
	if( !archive.WriteInt(m_iStartMonth))
		return false;
	if( !archive.WriteInt(m_iStartYear))
		return false;
	if( !archive.WriteInt(m_iEndDay))
		return false;
	if( !archive.WriteInt(m_iEndMonth))
		return false;
	if( !archive.WriteInt(m_iEndYear))
		return false;
	if( !archive.WriteInt(m_iStartHour))
		return false;
	if( !archive.WriteInt(m_iStartMinutes))
		return false;
	if( !archive.WriteInt(m_iStartSeconds))
		return false;
	if( !archive.WriteInt(m_iEndHour))
		return false;
	if( !archive.WriteInt(m_iEndMinutes))
		return false;
	if( !archive.WriteInt(m_iEndSeconds))
		return false;
	if( !archive.WriteInt(m_iDaysBetweenFrames))
		return false;
	if( !archive.WriteInt(m_iMinutesBetweenFrames))
		return false;
	if( !archive.WriteString(m_sFolderName))
		return false;
	if( !archive.WriteInt(m_iLightIndex))
		return false;
	if( !archive.WriteBool(m_bRenderFull))
		return false;
	if( !archive.WriteBool(m_bRenderPreview))
		return false;

  return true;
}

MYON_3dmAnimationProperties::CaptureTypes MYON_3dmAnimationProperties::CaptureType(void) const
{
  return m_CaptureTypes;
}

void MYON_3dmAnimationProperties::SetCaptureType(CaptureTypes type)
{
  m_CaptureTypes = type;
}

MYON_wString MYON_3dmAnimationProperties::FileExtension(void) const
{
  return m_sFileExtension;
}

void MYON_3dmAnimationProperties::SetFileExtension(const MYON_wString& s)
{
  m_sFileExtension = s;
}

MYON_UUID MYON_3dmAnimationProperties::DisplayMode(void) const
{
  return m_idDisplayMode;
}

void MYON_3dmAnimationProperties::SetDisplayMode(const MYON_UUID& id)
{
  m_idDisplayMode = id;
}

MYON_wString MYON_3dmAnimationProperties::ViewportName(void) const
{
  return m_sViewport;
}

void MYON_3dmAnimationProperties::SetViewportName(const MYON_wString& s)
{
  m_sViewport = s;
}

MYON_wString MYON_3dmAnimationProperties::HtmlFilename(void) const
{
  return m_sHtmlFilename;
}

void MYON_3dmAnimationProperties::SetHtmlFilename(const MYON_wString& s)
{
  m_sHtmlFilename = s;
}

MYON_wString MYON_3dmAnimationProperties::CaptureMethod(void) const
{
  return m_sCaptureMethod;
}

void MYON_3dmAnimationProperties::SetCaptureMethod(const MYON_wString& s)
{
  m_sCaptureMethod = s;
}

MYON_3dPointArray& MYON_3dmAnimationProperties::CameraPoints(void)
{
  return m_aCameraPoints;
}

const MYON_3dPointArray& MYON_3dmAnimationProperties::CameraPoints(void) const
{
  return m_aCameraPoints;
}

MYON_3dPointArray& MYON_3dmAnimationProperties::TargetPoints(void)
{
  return m_aTargetPoints;
}

const MYON_3dPointArray& MYON_3dmAnimationProperties::TargetPoints(void) const
{
  return m_aTargetPoints;
}

int MYON_3dmAnimationProperties::FrameCount(void) const
{
  return m_iFrameCount;
}

void MYON_3dmAnimationProperties::SetFrameCount(int i)
{
  m_iFrameCount = i;
}

int MYON_3dmAnimationProperties::CurrentFrame(void) const
{
  return m_iCurrentFrame;
}

void MYON_3dmAnimationProperties::SetCurrentFrame(int i)
{
  m_iCurrentFrame = i;
}

MYON_UUID MYON_3dmAnimationProperties::CameraPathId(void) const
{
  return m_idCameraPath;
}

void MYON_3dmAnimationProperties::SetCameraPathId(const MYON_UUID& id)
{
  m_idCameraPath = id;
}

MYON_UUID MYON_3dmAnimationProperties::TargetPathId(void) const
{
  return m_idTargetPath;
}

void MYON_3dmAnimationProperties::SetTargetPathId(const MYON_UUID& id)
{
  m_idTargetPath = id;
}

double MYON_3dmAnimationProperties::Latitude(void) const
{
  return m_dLatitude;
}

void MYON_3dmAnimationProperties::SetLatitude(double d)
{
  m_dLatitude = d;
}

double MYON_3dmAnimationProperties::Longitude(void) const
{
  return m_dLongitude;
}

void MYON_3dmAnimationProperties::SetLongitude(double d)
{
  m_dLongitude = d;
}

double MYON_3dmAnimationProperties::NorthAngle(void) const
{
  return m_dNorthAngle;
}

void MYON_3dmAnimationProperties::SetNorthAngle(double d)
{
  m_dNorthAngle = d;
}

int MYON_3dmAnimationProperties::StartDay(void) const
{
  return m_iStartDay;
}

void MYON_3dmAnimationProperties::SetStartDay(int i)
{
  m_iStartDay = i;
}

int MYON_3dmAnimationProperties::StartMonth(void) const
{
  return m_iStartMonth;
}

void MYON_3dmAnimationProperties::SetStartMonth(int i)
{
  m_iStartMonth = i;
}

int MYON_3dmAnimationProperties::StartYear(void) const
{
  return m_iStartYear;
}

void MYON_3dmAnimationProperties::SetStartYear(int i)
{
  m_iStartYear = i;
}

int MYON_3dmAnimationProperties::EndDay(void) const
{
  return m_iEndDay;
}

void MYON_3dmAnimationProperties::SetEndDay(int i)
{
  m_iEndDay = i;
}

int MYON_3dmAnimationProperties::EndMonth(void) const
{
  return m_iEndMonth;
}

void MYON_3dmAnimationProperties::SetEndMonth(int i)
{
  m_iEndMonth= i;
}

int MYON_3dmAnimationProperties::EndYear(void) const
{
  return m_iEndYear;
}

void MYON_3dmAnimationProperties::SetEndYear(int i)
{
  m_iEndYear = i;
}

int MYON_3dmAnimationProperties::StartHour(void) const
{
  return m_iStartHour;
}

void MYON_3dmAnimationProperties::SetStartHour(int i)
{
  m_iStartHour = i;
}

int MYON_3dmAnimationProperties::StartMinutes(void) const
{
  return m_iStartMinutes;
}

void MYON_3dmAnimationProperties::SetStartMinutes(int i)
{
  m_iStartMinutes = i;
}

int MYON_3dmAnimationProperties::StartSeconds(void) const
{
  return m_iStartSeconds;
}

void MYON_3dmAnimationProperties::SetStartSeconds(int i)
{
  m_iStartSeconds = i;
}

int MYON_3dmAnimationProperties::EndHour(void) const
{
  return m_iEndHour;
}

void MYON_3dmAnimationProperties::SetEndHour(int i)
{
  m_iEndHour = i;
}

int MYON_3dmAnimationProperties::EndMinutes(void) const
{
  return m_iEndMinutes;
}

void MYON_3dmAnimationProperties::SetEndMinutes(int i)
{
  m_iEndMinutes = i;
}

int MYON_3dmAnimationProperties::EndSeconds(void) const
{
  return m_iEndSeconds;
}

void MYON_3dmAnimationProperties::SetEndSeconds(int i)
{
  m_iEndSeconds = i;
}

int MYON_3dmAnimationProperties::DaysBetweenFrames(void) const
{
  return m_iDaysBetweenFrames;
}

void MYON_3dmAnimationProperties::SetDaysBetweenFrames(int i)
{
  m_iDaysBetweenFrames = i;
}

int MYON_3dmAnimationProperties::MinutesBetweenFrames(void) const
{
  return m_iMinutesBetweenFrames;
}

void MYON_3dmAnimationProperties::SetMinutesBetweenFrames(int i)
{
  m_iMinutesBetweenFrames = i;
}

int MYON_3dmAnimationProperties::LightIndex(void) const
{
  return m_iLightIndex;
}

void MYON_3dmAnimationProperties::SetLightIndex(int i)
{
  m_iLightIndex = i;
}

MYON_wString MYON_3dmAnimationProperties::FolderName(void) const
{return m_sFolderName;
}

void MYON_3dmAnimationProperties::SetFolderName(const MYON_wString& s)
{
  m_sFolderName = s;
}

const MYON_ClassArray<MYON_wString>& MYON_3dmAnimationProperties::Images(void) const
{
  return m_aImages;
}

MYON_ClassArray<MYON_wString>& MYON_3dmAnimationProperties::Images(void)
{
  return m_aImages;
}

MYON_ClassArray<MYON_wString>& MYON_3dmAnimationProperties::Dates(void)
{
  return m_aDates;
}

const MYON_ClassArray<MYON_wString>& MYON_3dmAnimationProperties::Dates(void) const
{
  return m_aDates;
}

bool MYON_3dmAnimationProperties::RenderFull(void) const
{
  return m_bRenderFull;
}

void MYON_3dmAnimationProperties::SetRenderFull(bool b)
{
  m_bRenderFull = b;
}

bool MYON_3dmAnimationProperties::RenderPreview(void) const
{
  return m_bRenderPreview;
}

void MYON_3dmAnimationProperties::SetRenderPreview(bool b)
{
  m_bRenderPreview = b;
}
